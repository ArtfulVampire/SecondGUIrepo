#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/mati.h>
#include <myLib/dataHandlers.h>

using namespace myOut;


void MainWindow::sliceAll() /////// aaaaaaaaaaaaaaaaaaaaaaaaaa//////////////////
{
	QTime myTime;
	myTime.start();

	if(ui->sliceCheckBox->isChecked())
	{
		if(ui->matiCheckBox->isChecked())
		{
			// almost equal time, should use sessionEdges
#if 1
			sliceMati();
			sliceMatiPieces(true);
#else
			sliceMatiSimple();
#endif
		}
		else
		{
			if(ui->windsButton->isChecked())
			{
				sliceWinds();
			}
			else if(ui->realsButton->isChecked())
			{
				if(ui->reduceChannelsComboBox->currentText().contains("MichaelBak")) // generality
				{
					sliceBak(1, 60, "241");
					sliceBak(61, 120, "247");
					sliceBak(121, 180, "241");
					sliceBak(181, 240, "247");
				}
				else
				{
					//						sliceOneByOneNew(); /// by number after 241/247
					if(ui->elenaSliceCheckBox->isChecked())
					{
						sliceElena();
					}
					else
					{
						sliceOneByOne();
//						sliceOneByOneNew();
					}
				}
			}
		}
	}
	ui->progressBar->setValue(0);

	QString helpString = "data sliced\nns equals to " + nm(globalEdf.getNs());
	ui->textEdit->append(helpString);

	std::cout << "sliceAll: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
}

void MainWindow::sliceWinds()
{
	QTime myTime;
	myTime.start();

	QString helpString;

	const edfFile & fil = globalEdf;

	const int timeShift = ui->timeShiftSpinBox->value() * fil.getFreq();
	const int wndLength = ui->windowLengthSpinBox->value() * fil.getFreq();

	const std::valarray<double> & marks = fil.getMarkArr();

	uint sta = std::min(
					myLib::indexOfVal(marks, 241.),
					myLib::indexOfVal(marks, 247.)) + 1;

	/// start, typ, filepath
	std::vector<std::tuple<int, int, QString>> forSave;


	int typ = -1;

	QString marker;
	if(marks[sta - 1] == 241.) { typ = 0; marker = "241"; }
	else if(marks[sta - 1] == 247.) { typ = 1; marker = "247"; }

	int numSkipStartWinds = 2;
	int windowCounter = 0;
	int numReal = 1;



	forSave.reserve(fil.getDataLen() / timeShift);
	for(uint i = sta; i < fil.getDataLen() - wndLength; i += timeShift)
	{
		auto mark = smLib::valarSubsec(marks, i, i + wndLength);

		/// hope only one of them occurs
		std::pair<bool, double> a = myLib::contains(mark, {241., 247., 254.});
		if(a.first)
		{
			if(a.second == 241.) typ = 0;
			else if(a.second == 247.) typ = 1;
			else if(a.second == 254.) typ = 2;
			marker = nm(a.second);

			++numReal;

			/// jump to the beginning of a real/rest
			i = i + myLib::indexOfVal(mark, a.second) + 1
				+ timeShift * (numSkipStartWinds - 1)
				;
			windowCounter = numSkipStartWinds;	/// = 1 if not taking the first window
		}
		else
		{
			helpString = def::windsFromRealsDir()
						 + "/" + fil.getExpName()
						 + "." + rn(numReal, 4)
						 + "_" + marker
//						 + "_typ_" + typ
						 + "." + rn(windowCounter++, 2);
			forSave.push_back(std::tuple<int, int, QString>(i, typ, helpString));
//			fil.saveSubsection(i, i + wndLength, helpString, true);
		}
		qApp->processEvents();
		if(stopFlag)
		{
			stopFlag = false;
			break;
		}
	}

	auto it = std::end(forSave); --it;
	while(std::get<1>(*it) == 2) { --it; } /// don't see last rest



	/// save all or some last
	if(ui->succPrecleanCheckBox->isChecked())
	{
		const int succMax = suc::learnSetStay * 2;
		std::valarray<int> succCounter(3); succCounter = 1; /// 3 - numOfClasses

		/// save succMax each type
		for(; (succCounter != succMax).max() && it != std::begin(forSave); --it)
		{
			int locTyp = std::get<1>(*it);
			if(succCounter[locTyp] < succMax)
			{
				fil.saveSubsection(std::get<0>(*it),
								   std::get<0>(*it) + wndLength,
								   std::get<2>(*it),
								   true);
				++succCounter[locTyp];

				ui->progressBar->setValue( succCounter.sum() * 100. / (3. * succMax));
			}
		}
	}
	else
	{
		int siz = std::distance(std::begin(forSave), it);
		int c = 0;
		while(1)
		{
			fil.saveSubsection(std::get<0>(*it),
							   std::get<0>(*it) + wndLength,
							   std::get<2>(*it),
							   true);

			ui->progressBar->setValue(c++ * 100. / siz);

			if(it == std::begin(forSave)) { break; }
			--it;
		}
	}
}

void MainWindow::sliceElena()
{
	QString helpString;
	int marker = -1;
	int number = -1;
	int start = -1;
	bool startFlag = false;

	const edfFile & fil = globalEdf;
	const std::valarray<double> & markChanArr = fil.getMarkArr();

	std::set<int> allNumbers;
	for(int i = 1; i < 240; ++i)
	{
		allNumbers.emplace(i);
	}

	// (241-244) - instruction, (1-240) - number(start), (255) - optional click,
	// (245-254) - operational, 255 - ready for next task
	for(int i = 0; i < fil.getDataLen(); ++i)
	{
		if(!startFlag)
		{
			if(241 <= markChanArr[i] && markChanArr[i] <= 244)
			{
				marker = markChanArr[i];
				startFlag = true;
			}
			else if(markChanArr[i] != 0 && markChanArr[i] != 255)
			{
				std::cout << "sliceElena: startFlag == false, "
						  << "bad marker " << markChanArr[i] << ", "
						  << "time = " << i / 250. << " sec" << std::endl;
			}
			else
			{
				// 0 or 255 - do nothing
			}
		}
		else // if(startFlag)
		{
			if(1 <= markChanArr[i] && markChanArr[i] <= 240) // task number
			{
				number = markChanArr[i];
				start = i;
			}
			else if(245 <= markChanArr[i] && markChanArr[i] <= 254) // task end
			{
				helpString = def::dir->absolutePath()
							 + "/Reals"
							 + "/" + def::ExpName
							 + "_n_" + nm(number)
							 + "_m_" + nm(marker)
							 + "_t_" + nm(markChanArr[i]);

				if(start != -1)
				{
					if(i - start < 60 * fil.getFreq())
					{
						fil.saveSubsection(start,
										   i,
										   helpString,
										   true);
						allNumbers.erase(number);
					}
					else
					{
						std::cout << "sliceElena: too long piece, "
								  << "start time = " << start / 250. << " sec, "
								  << "end time = " << i / 250. << " sec" << std::endl;
					}
				}
				else
				{
					std::cout << "sliceElena: startFlag == true, "
							  << "end mark = " << markChanArr[i] << ", "
							  << "start == -1, "
							  << "end time = " << i / 250. << " sec" << std::endl;
				}

				startFlag = false;
				start = -1;
				marker = 0;


				ui->progressBar->setValue(i * 100. / fil.getDataLen());
				qApp->processEvents();
				if(stopFlag)
				{
					stopFlag = 0;
					return;
				}

			}
		}
	}

	if(!allNumbers.empty())
	{
		std::cout << "sliceElena: not detected reals:" << std::endl;
		for(auto each : allNumbers)
		{
			std::cout << each << " "; std::cout.flush();
		}
		std::cout << std::endl;
	}
}


void MainWindow::sliceBak(int marker1, int marker2, QString marker) // beginning - from mark1 to mark 2, end 250. Marker - included in filename
{
    // for Baklushev
    QString helpString;

    int number = 0;
    int k;
	int j = 0;                                     // flag of marker1 read
	int h = 0;                                     // flag of marker2 read
    const edfFile & fil = globalEdf;
	const auto & markerChan = fil.getData()[fil.getMarkChan()];

    for(int i = 0; i < fil.getDataLen(); ++i)
    {
		if(markerChan[i] >= marker1
		   && markerChan[i] <= marker2
		   && h== 0)
        {
            j = i;
            h = 1;
            continue;
        }
		if(markerChan[i] == 250.)
        {
            k = i;
            if(h == 1) ++h;
        }
        if(h == 2)
        {
            ++number;
            helpString = def::dir->absolutePath()
					+ "/Reals"
					+ "/" + def::ExpName
                    + "_" + marker
					+ "." + rn(number, 4);

            // to test?
            fil.saveSubsection(j, k, helpString, true);

            i += 17;
            h = 0;
        }
    }
}


void MainWindow::sliceOneByOne()
{
    QString helpString;
    int number = 0;
    QString marker = "000";
	int start = 0;

    const edfFile & fil = globalEdf;
//	const std::valarray<double> & markChanArr = fil.getMarkArr();
	const std::vector<std::pair<int, int>> & markers = fil.getMarkers();

    // 200, (241||247, (1), 254, 255)
//	for(int i = 0; i < fil.getDataLen(); ++i)
	for(const std::pair<int, int> & in : markers)
    {
//		if(markChanArr[i] == 0 ||
//		   !(markChanArr[i] == 241 ||
//			 markChanArr[i] == 247 ||
//			 markChanArr[i] == 254))
//		{
//			continue;
//		}
		if(in.second != 241
		   && in.second != 247
		   && in.second != 254)
		{
			continue;
		}
        else
        {
//			const int finish = i;
			const int finish = in.first;

            helpString = def::dir->absolutePath()
						 + "/Reals"
						 + "/" + def::ExpName
						 + "." + rn(number++, 4);


            if(finish > start)
            {
				if(finish - start <= def::freq * 62) /// const generality limit
                {
                    helpString += "_" + marker;
                    fil.saveSubsection(start,
                                       finish,
                                       helpString, true);
                }
                else /// pause rest
                {
					if(def::writeLongStartEnd)
                    {
                        helpString += "_000";
                        fil.saveSubsection(start,
                                           finish,
                                           helpString, true);
                    }
                    else
                    {
						/// why do I need this?
//						helpString += "_" + marker;
//						matrix tempData(fil.getNs(), 100, 0.);
//						tempData[fil.getMarkChan()][0] = markChanArr[start];
//						myLib::writePlainData(helpString, tempData);
                    }
				}
			}
//			ui->progressBar->setValue(i * 100. / fil.getDataLen());
			ui->progressBar->setValue(in.first * 100. / fil.getDataLen());

            qApp->processEvents();
            if(stopFlag)
            {
                stopFlag = 0;
                return;
            }

//			marker = nm(markChanArr[finish]);
			marker = nm(in.second);
			start = finish;
        }
    }
    /// write final
    {
        helpString = def::dir->absolutePath()
					 + "/Reals"
					 + "/" + def::ExpName
					 + "." + rn(number++, 4);
        if(fil.getDataLen() - start < 40 * def::freq) /// if last realisation or interstimulus
        {
            helpString += "_" + marker;
            fil.saveSubsection(start,
                               fil.getDataLen(),
                               helpString, true);
        }
        else /// just last big rest with eyes closed/open
        {
			if(def::writeLongStartEnd)
            {
                helpString += "_000";
                fil.saveSubsection(start,
                                   fil.getDataLen(),
                                   helpString, true);
            }
            else /// not to loose the last marker
            {
//				helpString += "_" + marker;
//				matrix tempData(fil.getNs(), 100, 0.);
//				tempData[fil.getMarkChan()][0] = markChanArr[start];
//				myLib::writePlainData(helpString, tempData);
            }
        }
    }
}

// deprecated numChanWrite - always with markers
void MainWindow::sliceOneByOneNew()
{
    QString helpString;
    int number = 0;
    int j = 0;
	int h = 0; // h == 0 - 241, h == 1 - 247
    QString marker = "000";

    const edfFile & fil = globalEdf;

    const std::valarray<double> & markChanArr = fil.getData()[fil.getMarkChan()];

    // 200, 255, (241||247, num, 254, 255)
    // with feedback 200 (241||247, num, 231||237, (234), 254, 255)
    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        if(markChanArr[i] == 0.)
        {
            continue;
        }
        else if((markChanArr[i] > 200 && markChanArr[i] < 241) ||
                markChanArr[i] == 255 ||
                markChanArr[i] == 250 ||
				markChanArr[i] == 251) // all not interesting markers
        {
            continue;
        }
        else if(markChanArr[i] == 241 || markChanArr[i] == 247)
        {
            marker = "254";
            if(markChanArr[i] == 241) h = 0;
            else if (markChanArr[i] == 247) h = 1;
            continue; // wait for num marker
        }
		else if(true) // marker can be num <= 200, ==254, smth else
        {
            if(marker.isEmpty())
            {
                marker = "sht";
            }

            helpString = def::dir->absolutePath()
						 + "/Reals"
						 + "/" + def::ExpName
						 + "." + rn(number++, 4);
//            std::cout << helpString << std::endl;
            if(i > j)
            {
                if(i - j <= def::freq * 60) /// const generality limit
                {
                    helpString += "_" + marker;
                    fil.saveSubsection(j, i, helpString, true);
                }
                else /// pause rest
                {
					if(def::writeLongStartEnd)
                    {
                        helpString += "_000";
                        fil.saveSubsection(j, i, helpString, true);
                    }
                    else
                    {
                        helpString += "_" + marker;
                        matrix tempData(fil.getNs(), 100, 0.);
                        tempData[fil.getMarkChan()][0] = markChanArr[j];
						myLib::writePlainData(helpString, tempData);
                    }
                }
            }
            ui->progressBar->setValue(i * 100. / fil.getDataLen());

            qApp->processEvents();
            if(stopFlag)
            {
                stopFlag = 0;
                break;
            }

            marker.clear();
            if(h == 0) marker = "241";
            else if(h == 1) marker = "247";
            h = -1;
            j = i;
            continue;
        }
    }
    /// write final
    {
        helpString = def::dir->absolutePath()
					 + "/Reals"
					 + "/" + def::ExpName
					 + "." + rn(number++, 4);
        if(fil.getDataLen() - j < 40 * def::freq) /// if last realisation or interstimulus
        {
            helpString += "_" + marker;
            fil.saveSubsection(j, fil.getDataLen(), helpString, true);
        }
        else /// just last big rest with eyes closed/open
        {
			if(def::writeLongStartEnd)
            {
                helpString += "_000";
                fil.saveSubsection(j, fil.getDataLen(), helpString, true);
            }
            else /// not to loose the last marker
            {
                helpString += "_" + marker;
                matrix tempData(fil.getNs(), 100, 0.);
                tempData[fil.getMarkChan()][0] = markChanArr[j];
				myLib::writePlainData(helpString, tempData);
            }
        }
    }

}

void MainWindow::sliceMatiSimple()
{
#if 0
    QTime myTime;
    myTime.start();

    QString helpString;
    int start = 0;
    int end = -1;
    std::vector<bool> markers;
    bool state[3];
    QString fileMark;
	int session[4]; // generality
    int type = 3;

    for(int i = 0; i < 4; ++i)
    {
        session[i] = 0;
    }

    const edfFile & fil = globalEdf;
    double currMarker;
    int number;
    double piece = ui->matiPieceLengthSpinBox->value() * def::freq;

    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        currMarker = fil.getData()[fil.getMarkChan()][i];
        if(currMarker == 0)
        {
            continue;
        }
        else
        {
			markers = myLib::matiCountByte(currMarker);
			// decide whether the marker is interesting: 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0
            for(int i = 0; i < 3; ++i)
            {
				state[i] = markers[i + 8]; // always elder byte is for count adn type of the session
            }

            if(!(state[0] || state[1] || state[2])) continue; // if all are zeros

			if(state[2] == 1) // the end of a session
            {
				if(state[1] == 0 && state[0] == 1) // end of a counting session
                {
                    type = 0;
					fileMark = "241"; // count
                }
				if(state[1] == 1 && state[0] == 0) // end of a tracking session
                {
                    type = 1;
					fileMark = "247"; // follow
                }
				if(state[1] == 1 && state[0] == 1) // end of a composed session
                {
                    type = 2;
					fileMark = "244"; // composed
                }
            }
			else // if the start of a session
            {
                type = 3;
				fileMark = "254"; // rest. start of the session is sliced too
            }
            end = i + 1; // end marker should be included
        }

		// save session edf
        if(end > start)
        {
            if(state[2]) // if not rest
            {
                number = int(ceil((end-start)/double(piece)));

                for(int j = 0; j < number; ++j) // num of pieces
                {
                    helpString = (def::dir->absolutePath()
														  + "/Reals"
														  + "/" + def::ExpName
														  + "_" + nm(type)
														  + "_" + nm(session[type])
														  + "_" + rn(j, 2)
                                                          + '.' + fileMark);

					int NumOfSlices = std::min(end - start - j * piece, piece);
					/// PEWPEWPEWPEWPEWPEWPEW
//					myLib::writePlainData(helpString, fil.getData(), NumOfSlices, start + j * piece);
                }
                fileMark.clear();
                ++session[type];
            }

            start = end - 1; // = i // start marker should be included
            end = -1;
        }
        ui->progressBar->setValue(end * 100. / fil.getDataLen());

        qApp->processEvents();
        if (stopFlag == 1)
        {
            break;
        }
    }
    ui->progressBar->setValue(0);

	std::cout << "sliceMatiSimple: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
    stopFlag = 0;
#endif
}

void MainWindow::sliceMati()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int start = 0;
    int end = -1;
    std::vector<bool> markers;
    bool state[3];
    QString fileMark;
	int session[4]; // generality
    int type = 3;

    for(int i = 0; i < 4; ++i)
    {
        session[i] = 0;
    }

    const edfFile & fil = globalEdf;
    double currMarker;


    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        currMarker = fil.getData()[fil.getMarkChan()][i];
        if(currMarker == 0)
        {
            continue;
        }
        else
        {
			markers = myLib::matiCountByte(currMarker);
			// decide whether the marker is interesting: 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0
            for(int i = 0; i < 3; ++i)
            {
				state[i] = markers[i + 8]; // always elder byte is for count adn type of the session
            }

            if(!(state[0] || state[1] || state[2])) continue; // if all are zeros

			if(state[2] == 1) // the end of a session
            {
				if(state[1] == 0 && state[0] == 1) // end of a counting session
                {
                    type = 0;
					fileMark = "241"; // count
                }
				if(state[1] == 1 && state[0] == 0) // end of a tracking session
                {
                    type = 1;
					fileMark = "247"; // follow
                }
				if(state[1] == 1 && state[0] == 1) // end of a composed session
                {
                    type = 2;
					fileMark = "244"; // composed
                }
            }
			else // if the start of a session
            {
                type = 3;
				fileMark = "254"; // rest. start of the session is sliced too
            }
            end = i + 1; // end marker should be included
        }

		// save session edf
        if(end > start)
        {
            if(type != 3) // dont write rests
            {
                helpString = (def::dir->absolutePath()

													  + "/auxEdfs"

													  + "/" + def::ExpName
													  + "_" + nm(type)
													  + "_" + nm(session[type])
                                                      + ".edf");

                fil.saveSubsection(start,
                                   end,
                                   helpString);
            }

			start = end - 1; // start marker should be included
            end = -1;
            ++session[type];
        }
        ui->progressBar->setValue(end * 100. / fil.getDataLen());

        qApp->processEvents();
        if (stopFlag == 1)
        {
            break;
        }
    }
    ui->progressBar->setValue(0);
	std::cout << "sliceMati: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
    stopFlag = 0;
}

/// add markChan alias
void MainWindow::sliceMatiPieces(bool plainFlag)
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int dataLen;
    QString fileMark;
    QString folder;
    int pieceNum;
    int currStart;
    int currEnd;
    const double pieceLength = ui->matiPieceLengthSpinBox->value();
    const bool adjustPieces = ui->adjustPiecesCheckBox->isChecked();

    def::dir->cd(globalEdf.getDirPath());
    edfFile fil;

    if(pieceLength <= 4.)
    {
		folder = "winds";
    }
    else
    {
		folder = "Reals";
    }

    for(int type = 0; type < 3; ++type)
    {
        for(int session = 0; session < 15; ++session)
        {
            // edf session path
            helpString = (def::dir->absolutePath()

												  + "/auxEdfs"

												  + "/" + globalEdf.getExpName()
												  + "_" + nm(type)
												  + "_" + nm(session)
                                                  + ".edf");

            if(QFile::exists(helpString))
            {
                fil.readEdfFile(helpString);
                dataLen = fil.getDataLen();
                pieceNum = 0;
                currStart = 0;
                currEnd = -1; // [currStart, currEnd)

                if(type == 0)       fileMark = "241";
                else if(type == 1)  fileMark = "247";
                else if(type == 2)  fileMark = "244";
                else                fileMark = "254";

                if(adjustPieces)
                {
                    do
                    {
						currEnd = std::min(int(currStart + pieceLength * def::freq), dataLen);

                        if(type == 0 || type == 2)
                        {
                            // std::search
							while ( ! (myLib::matiCountBit(fil.getData()[fil.getMarkChan()][currEnd-1], 14) ||
									   myLib::matiCountBit(fil.getData()[fil.getMarkChan()][currEnd-1], 10)) ) // while not (given answer OR session End)
                            {
                                --currEnd;
                            }
                        }
                        else if(currEnd == dataLen) // should do nothing due to edfFile::cutZerosAtEnd
                        {
							while ( ! (myLib::matiCountBit(fil.getData()[fil.getMarkChan()][currEnd - 1], 10)) ) // while not session end
                            {
                                --currEnd;
                            }
                        }

                        if(currEnd <= currStart) // no count answers during pieceLength seconds
                        {
							currEnd = std::min(int(currStart + pieceLength * def::freq), dataLen);
                        }

                        // type and session already in the fil.ExpName

						helpString = def::dir->absolutePath()
									 + "/" + folder
									 + "/" + fil.getExpName()
									 + "_" + rn(pieceNum, 2)
									 + '_' + fileMark;


                        fil.saveSubsection(currStart, currEnd, helpString, plainFlag);
                        ++pieceNum;
                        currStart = currEnd;

					} while (!myLib::matiCountBit(fil.getData()[fil.getMarkChan()][currEnd - 1], 10) );
                }
                else
                {
                    while(currStart < dataLen)
                    {
						currEnd = std::min(int(currStart + pieceLength * def::freq), dataLen);
                        helpString = (def::dir->absolutePath()
															  + "/" + folder
															  + "/" + fil.getExpName()
															  + "_" + rn(pieceNum, 2)
                                                              + '_' + fileMark);
                        fil.saveSubsection(currStart, currEnd, helpString, plainFlag);
                        ++pieceNum;
                        currStart = currEnd;
                    }
                }
            }
        }
    }
	std::cout << "sliceMatiPieces: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}
