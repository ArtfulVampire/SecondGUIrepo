#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/mati.h>
#include <myLib/dataHandlers.h>
#include <functional>

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
//				sliceJustWinds(); /// IITP
			}
//			else if(ui->pauseRadioButton->isChecked())
//			{
//				bool a = ui->typeCheckButton->isChecked();
//				pausePieces(a);
//			}
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
					if(ui->elenaSliceCheckBox->isChecked())
					{
						sliceElena();
					}
					else
					{
						/// remake using myLib::sliceData
						sliceOneByOne();
//						sliceOneByOneNew(); /// by number after 241/247
					}
				}
			}
		}
	}
	ui->progressBar->setValue(0);

	QString helpString = "data sliced\nns equals to " + nm(globalEdf.getNs());
	ui->textEdit->append(helpString);

	std::cout << "sliceAll: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void MainWindow::sliceJustWinds()
{
	const edfFile & fil = globalEdf;

	const int timeShift = ui->timeShiftSpinBox->value() * fil.getFreq();
	const int wndLength = ui->windowLengthSpinBox->value() * fil.getFreq();

	int windowCounter = 0;
	for(int i = 0; i < fil.getDataLen() - wndLength; i += timeShift)
	{
		QString helpString = DEFS.windsFromRealsDir()
							 + "/" + fil.getExpName()
							 + "." + rn(windowCounter++, 4);
		fil.saveSubsection(i, i + wndLength, helpString, true);

		ui->progressBar->setValue( i * 100. / fil.getDataLen() );

		qApp->processEvents();
		if(stopFlag)
		{
			stopFlag = false;
			break;
		}
	}
	ui->progressBar->setValue(0);
}

void MainWindow::sliceWinds()
{
	QTime myTime;
	myTime.start();

	QString helpString;

	const std::vector<double> staMarks{241., 247.};
	const std::vector<double> endMarks{254.};
	const std::vector<double> allMarks = smLib::unite<std::vector<double>>({staMarks, endMarks});

	const edfFile & fil = globalEdf;

	const int wndLength = std::round(fil.getFreq() * ui->windowLengthSpinBox->value());
	const int timeShift = std::round(fil.getFreq() * ui->timeShiftSpinBox->value());

	const std::valarray<double> & marks = fil.getMarkArr();


	int sta = myLib::indexOfVal(marks, staMarks[0]);
	for(int i = 1; i < staMarks.size(); ++i)
	{
		sta = std::min( myLib::indexOfVal(marks, staMarks[i]), sta );
	}
	sta += 1;

	/// start, typ, filepath
	std::vector<std::tuple<int, int, QString>> forSave;

	int typ = -1;

	QString marker;
	for(int i = 0; i < allMarks.size(); ++i)
	{
		if(marks[sta - 1] == allMarks[i]) { typ = i; marker = nm(allMarks[i]); break; }
	}

	int numSkipStartWinds = 2;
	int windowCounter = 0;
	int numReal = 1;



	forSave.reserve(fil.getDataLen() / timeShift);
	for(uint i = sta; i < fil.getDataLen() - wndLength; i += timeShift)
	{
		std::valarray<double> mark = smLib::contSubsec(marks, i, i + wndLength);

		/// hope only one of them occurs
		/// if it's not true - skip
		std::pair<bool, double> a = myLib::contains(mark, allMarks);
		if(a.first)
		{
			for(int i = 0; i < allMarks.size(); ++i)
			{
				if(a.second == allMarks[i]) { typ = i; }
			}
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
			helpString = DEFS.windsFromRealsDir()
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
	while(std::get<1>(*it) == 2) { --it; } /// don't save last rest

//	int n = 0;
//	for(auto bit = it; bit != std::begin(forSave); --bit)
//	{
//		if(std::get<1>(*bit) == 2) { ++n; }
//	}
//	std::cout << n << std::endl;

	/// save all or some last
	if(ui->succPrecleanCheckBox->isChecked())
	{
		const int succMax = suc::learnSetStay * 2; /// ~=120
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
	const edfFile & fil = globalEdf;
	const int numOfTasks = 180;

	const std::vector<std::vector<int>> eyesMarks{{210, 211}, {212, 213}};
	const std::vector<int> eyesCodes{214, 215};

	const double restWindow = 10.;	/// window length in seconds
	const int windFft = smLib::fftL(restWindow * fil.getFreq());
	const double restShift = 7.;	/// time shift between windows in seconds
	const int numSmoothWind = 10;

	const std::valarray<double> & markChanArr = fil.getMarkArr();
	const auto & marks = fil.getMarkers();

	const auto eegChannels = fil.findChannels("EEG ");

	const int leftFreqLim = fftLimit(DEFS.getLeftFreq(),
							  fil.getFreq(),
							  DEFS.getFftLen());
	const int rightFreqLim = fftLimit(DEFS.getRightFreq(),
							   fil.getFreq(),
							   DEFS.getFftLen()) + 1;

	/// for table 13-Mar-18
	const std::vector<std::pair<double, double>> integrLimits
	{{4, 8}, {8, 12}, {12, 16}, {16, 20}, {20, 24}};
	const std::vector<std::vector<QString>> integrChans
	{{"F3", "F7"}, {"F4", "F8"}, {"T3", "T5"}, {"C3"}, {"C4"}, {"T4", "T6"},
		{"P3"}, {"P4"}, {"O1"}, {"O2"}};

	matrix table(numOfTasks, 1);
	std::vector<QString> tableCols
	{
		"taskNum",
		"taskMark",
		"operMark",
		"SGRval",
		"SGRlat",
		"PPGampl"
//		"PPGfreq",
	};
	/// add average spectra
	for(const auto & chs : integrChans)
	{
		/// F3_F7_12-16
		QString chanStr{};
		for(const auto & ch : chs)
		{
			chanStr += ch + "_";
		}
//		chanStr.chop(1); /// chop last '_'

		for(const auto & lim : integrLimits)
		{
			chanStr += nm(lim.first) + "-" + nm(lim.second);
		}
		tableCols.push_back(chanStr);
	}

	/// new 6-Mar-18
	const QString RDstring{"RD"};
	const QString PPGstring{"FPG"};
	const QString EDAstring{"KGR"};

	const int RDnum = fil.findChannel(RDstring);
	const int PPGnum = fil.findChannel(PPGstring);
	const int EDAnum = fil.findChannel(EDAstring);

	if(RDnum == -1 || PPGnum == -1 || EDAnum == -1)
	{
		std::cout << "sliceElena: some of vegetative channels is absent" << std::endl;
	}

	auto saveSpecPoly = [&](int startBin,
						const QString & pieceNumber,
						const QString & taskMark,
						const QString & operMark)
	{
		/// save spectre+polygraph
		matrix subData = fil.getData()
						 .subCols(startBin, startBin + restWindow * fil.getFreq())
						 .subRows(eegChannels);

		std::valarray<double> edaBase{};
		if(EDAnum != -1)
		{
			edaBase = smLib::contSubsec(fil.getData(EDAnum),
										std::max(0., startBin - 2 * fil.getFreq()), /// -2 sec
										startBin);
		}

		matrix spec = myLib::countSpectre(subData,
										  windFft,
										  numSmoothWind);

		/// check bad file
		if(spec.isEmpty() || edaBase.size() < fil.getFreq()) { return; }




		std::vector<double> outVector = spec.subCols(leftFreqLim, rightFreqLim).toVectorByRows();
		double RDfr{};
		if(RDnum != -1) { RDfr = myLib::RDfreq(subData[RDnum], windFft); }
		double PPGampl{};
		if(PPGnum != -1) { PPGampl = myLib::PPGrange(subData[fil.findChannel(PPGstring)]); }
		std::pair<double, double> EDAval{};
		if(EDAnum != -1) { EDAval = myLib::EDAmax(subData[fil.findChannel(EDAstring)], edaBase); }

		if(0)
		{
			/// temporarily turn off poly
			outVector.push_back(RDfr);
			outVector.push_back(PPGampl);
			outVector.push_back(EDAval.first);						/// value of max EDA
//			outVector.push_back(EDAval.second);						/// latency in bins
//			outVector.push_back(EDAval.second / fil.getFreq());		/// latency in seconds
		}
		/// write to file
		QString savePath = fil.getDirPath()
						   + "/SpectraSmooth"
						   + "/" + fil.getExpName()
						   + "_n_" + pieceNumber
						   + "_m_" + taskMark
						   + "_t_" + operMark
				+ "." + def::spectraDataExtension;

		myLib::writeFileInLine(savePath, outVector);



		/// for tables
		/// check rest or task
		bool ok = false;
		pieceNumber.toInt(&ok);
		if(!ok) { return; }

		/// calculate integrated spectra

		matrix integratedSpectra = myLib::integrateSpectra(spec, fil.getFreq(), integrLimits);

		/// integrate over channels
		matrix integratedSpectraOut(integrChans.size(), integratedSpectra.cols());
		int counter = 0;
		for(const auto & chs : integrChans) /// each subset
		{
			std::valarray<double> res(0., integratedSpectra.cols());
			for(const auto & ch : chs) /// each chan from subset
			{
				res += integratedSpectra[fil.findChannel(ch)];
			}
			res /= chs.size();
			integratedSpectraOut[counter++] = res;
		}

		std::vector<double> forTable{}; forTable.reserve(20 + integrChans.size() * integrLimits.size());
		forTable.push_back(pieceNumber.toInt());
		forTable.push_back(taskMark.toInt());
		forTable.push_back(operMark.toInt());
		if(1) /// for table
		{
			forTable.push_back(EDAval.first);						/// value of max EDA
//			outVector.push_back(EDAval.second);						/// latency in bins
			forTable.push_back(EDAval.second / fil.getFreq());		/// latency in seconds
			forTable.push_back(RDfr);
			forTable.push_back(PPGampl);
		}

		/// push_back spectra
		const int prevSize = forTable.size();
		forTable.resize(prevSize + integrChans.size() * integrLimits.size());
		const std::vector<double> specRow = integratedSpectraOut.toVectorByRows();
		std::copy(std::begin(specRow), std::end(specRow),
				  std::begin(forTable) + prevSize);
		table[pieceNumber.toInt()] = smLib::vecToValar(forTable);
	};


	/// slice rest backgrounds
	for(int typ = 0; typ < 2; ++typ)	/// 0 - closed, 1 - open
	{
		auto openSta = std::find_if(std::begin(marks),
									std::end(marks),
									[eyesMarks, typ](const auto & in)
		{ return in.second == eyesMarks[typ][0]; }); /// [0] - start

		auto openFin = std::find_if(std::begin(marks),
									std::end(marks),
									[eyesMarks, typ](const auto & in)
		{ return in.second == eyesMarks[typ][1]; }); /// [1] - finish

		int windCounter = 0;
		for(int i = (*openSta).first;
			i < (*openFin).first - restWindow * fil.getFreq();
			i += restShift * fil.getFreq(), ++windCounter)
		{
			QString helpString = DEFS.dirPath()
								 + "/Reals"
								 + "/" + fil.getExpName()
								 + "_n_0_" + nm(windCounter)
//								 + "_n_" + nm(windCounter)
								 + "_m_" + nm(eyesMarks[typ][0])
								 + "_t_" + nm(eyesCodes[typ]);
			/// save window
			fil.saveSubsection(i,
							   i + restWindow * fil.getFreq(),
							   helpString,
							   true);

			saveSpecPoly(i,
						 QString("0_" + nm(windCounter)),	/// restNumber
//						 nm(windCounter),				/// restNumber
						 nm(eyesMarks[typ][0]),			/// taskMark
						 nm(eyesCodes[typ])				/// operMark
						 );
		}
	}

	std::set<int> allNumbers;
	for(int i = 1; i < numOfTasks; ++i)
	{
		allNumbers.emplace(i);
	}

	int marker = -1;
	int number = -1;
	int start = -1;
	bool startFlag = false;

	/// slice all tasks
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
						  << "time = " << i / fil.getFreq() << " sec" << std::endl;
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
				QString helpString = DEFS.dirPath()
									 + "/Reals"
									 + "/" + fil.getExpName()
									 + "_n_" + nm(number)				/// task number
									 + "_m_" + nm(marker)				/// task marker
									 + "_t_" + nm(markChanArr[i]);		/// operational marker

				if(start != -1)
				{
					if(i - start < 60 * fil.getFreq())
					{
						fil.saveSubsection(start,
										   i,
										   helpString,
										   true);

						/// new 6-Mar-18
						saveSpecPoly(start,
									 nm(number),
									 nm(marker),
									 nm(markChanArr[i]));

						allNumbers.erase(number);
					}
					else
					{
						std::cout << "sliceElena: too long piece, "
								  << "start time = " << start / fil.getFreq() << " sec, "
								  << "end time = " << i / fil.getFreq() << " sec" << std::endl;
					}
				}
				else
				{
					std::cout << "sliceElena: startFlag == true, "
							  << "end mark = " << markChanArr[i] << ", "
							  << "start == -1, "
							  << "end time = " << i / fil.getFreq() << " sec" << std::endl;
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

	/// table into file
	std::ofstream tableStream((fil.getDirPath() + "_table.txt").toStdString());
	tableStream << tableCols << std::endl;
	tableStream << table;
//	myOut::operator <<(tableStream, table);
	tableStream.close();

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

// beginning - from mark1 to mark 2, end 250 Marker - included in filename
void MainWindow::sliceBak(int marker1, int marker2, QString marker)
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
			helpString = DEFS.dirPath()
					+ "/Reals"
					+ "/" + fil.getExpName()
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

#define USE_MARKERS 01

    QString helpString;
    int number = 0;
    QString marker = "000";
	int start = 0;

    const edfFile & fil = globalEdf;
#if USE_MARKERS
	const std::vector<std::pair<int, int>> & markers = fil.getMarkers();
#else
	const std::valarray<double> & markChanArr = fil.getMarkArr();
#endif

    // 200, (241||247, (1), 254, 255)
#if USE_MARKERS
	for(const std::pair<int, int> & in : markers)
#else
	for(int i = 0; i < fil.getDataLen(); ++i)
#endif
    {


#if USE_MARKERS
		if(in.second != 241
		   && in.second != 247
		   && in.second != 254)
		{
			continue;
		}
#else
		if(markChanArr[i] == 0 ||
		   !(markChanArr[i] == 241 ||
			 markChanArr[i] == 247 ||
			 markChanArr[i] == 254))
		{
			continue;
		}
#endif
        else
        {
#if USE_MARKERS
			const int finish = in.first;
#else
			const int finish = i;
#endif

			helpString = DEFS.dirPath()
						 + "/Reals"
						 + "/" + fil.getExpName()
						 + "." + rn(number++, 4);


            if(finish > start)
            {
				if(finish - start <= DEFS.getFreq() * 62) /// magic const generality limit
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
#if USE_MARKERS
			ui->progressBar->setValue(in.first * 100. / fil.getDataLen());
#else
			ui->progressBar->setValue(i * 100. / fil.getDataLen());
#endif

            qApp->processEvents();
            if(stopFlag)
            {
                stopFlag = 0;
                return;
            }
#if USE_MARKERS
			marker = nm(in.second);
#else
			marker = nm(markChanArr[finish]);
#endif
			start = finish;
        }
    }


    /// write final
    {
		helpString = DEFS.dirPath()
					 + "/Reals"
					 + "/" + fil.getExpName()
					 + "." + rn(number++, 4);
		if(fil.getDataLen() - start < 40 * DEFS.getFreq()) /// if last realisation or interstimulus
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

			helpString = DEFS.dirPath()
						 + "/Reals"
						 + "/" + fil.getExpName()
						 + "." + rn(number++, 4);
//            std::cout << helpString << std::endl;
            if(i > j)
            {
				if(i - j <= DEFS.getFreq() * 60) /// const generality limit
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
			if(h == 0)			{ marker = "241"; }
			else if(h == 1)		{ marker = "247"; }
            h = -1;
            j = i;
            continue;
        }
    }
    /// write final
    {
		helpString = DEFS.dirPath()
					 + "/Reals"
					 + "/" + fil.getExpName()
					 + "." + rn(number++, 4);
		if(fil.getDataLen() - j < 40 * DEFS.getFreq()) /// if last realisation or interstimulus
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
	double piece = ui->matiPieceLengthSpinBox->value() * DEFS.getFreq();

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
					helpString = (DEFS.dirPath()
														  + "/Reals"
														  + "/" + globalEdf.getExpName()
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

	std::cout << "sliceMatiSimple: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
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
				helpString = DEFS.dirPath()
							 + "/auxEdfs"
							 + "/" + fil.getExpName()
							 + "_" + nm(type)
							 + "_" + nm(session[type])
							 + ".edf";

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
	std::cout << "sliceMati: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
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
	const bool adjustPieces = ui->matiAdjustPiecesCheckBox->isChecked();

    DEFS.setDir(globalEdf.getDirPath());
    edfFile fil;

    if(pieceLength <= 4.)
    {
		folder = "winds";
    }
    else
    {
		folder = "Reals";
    }

	for(int type = 0; type < 3; ++type) /// magic const
    {
		for(int session = 0; session < 15; ++session) /// magic const
        {
            // edf session path
			helpString = DEFS.dirPath()
						 + "/auxEdfs"
						 + "/" + globalEdf.getExpName()
						 + "_" + nm(type)
						 + "_" + nm(session)
						 + ".edf";

            if(QFile::exists(helpString))
            {
                fil.readEdfFile(helpString);
                dataLen = fil.getDataLen();
                pieceNum = 0;
                currStart = 0;
                currEnd = -1; // [currStart, currEnd)

				switch(type)
				{
				case 0:		{ fileMark = "241"; break; }
				case 1:		{ fileMark = "247"; break; }
				case 2:		{ fileMark = "244"; break; }
				default:	{ fileMark = "254"; break; }
				}

                if(adjustPieces)
                {
                    do
                    {
						currEnd = std::min(int(currStart + pieceLength * DEFS.getFreq()), dataLen);

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
							currEnd = std::min(int(currStart + pieceLength * DEFS.getFreq()), dataLen);
                        }

                        // type and session already in the fil.ExpName

						helpString = DEFS.dirPath()
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
						currEnd = std::min(int(currStart + pieceLength * DEFS.getFreq()), dataLen);
						helpString = (DEFS.dirPath()
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

//void MainWindow::pausePieces(bool in)
//{

//	std::function<bool(char)> cond1;
//	std::function<bool(char)> cond2;
//	std::function<bool(char)> cond3;
//	QString mark1;
//	QString mark2;
//	QString mark3;

//	if(in)	/// in == true - ans/noans
//	{
//		cond1 = [](char a) -> bool { return a == '0'; };
//		mark1 = "260";
//		cond2 = [](char a) -> bool { return a == '1' || a == '2'; };
//		mark2 = "261";
//		cond3 = [](char a) -> bool { return true; };
//		mark3 = "000";
//	}
//	else /// in == false - right/wrong
//	{
//		cond1 = [](char a) -> bool { return a == '1'; };
//		mark1 = "262";
//		cond2 = [](char a) -> bool { return a == '2'; };
//		mark2 = "263";
//		cond3 = [](char a) -> bool { return a == '0'; };
//		mark3 = "264";
//	}

//	for(...)
//	{
//		char h;
//		fin >> h;
//		if(cond1(h)) { marker = mark1; }
//		else if(cond2(h)) { marker = mark2; }
//		else if(cond3(h)) { marker = mark3; }
//	}

//}
