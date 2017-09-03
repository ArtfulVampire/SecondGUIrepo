#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/drw.h>
#include <myLib/mati.h>
#include <myLib/dataHandlers.h>
#include <myLib/general.h>

using namespace myOut;

matrix countEvoked(const std::vector<matrix> & inData, int length)
{
	matrix res(inData[0].rows(), length);

	for(const matrix & in : inData)
	{
		res += in.subCols(0, length);
	}
	res /= inData.size();
	return res;
}

void MainWindow::makeEvoked(const QString & edfPath,
							const std::valarray<double> & startMarkers,
							const QString outPath)
{
	const int length = 600;
	edfFile fil;
	fil.readEdfFile(edfPath);
	std::vector<matrix> dat;
	for(auto mark : fil.getMarkers())
	{
		if(std::find(std::begin(startMarkers),
					 std::end(startMarkers),
					 mark.second) != std::end(startMarkers))
		{
			dat.push_back(fil.getData().subCols(mark.first, mark.first + length));
		}
	}
	auto res = countEvoked(dat, length);
	myLib::writeMatrixFile(outPath, res);
}

void MainWindow::makeEvoked(const QString & realsPath,
							const QStringList & fileFilters,
							const QString outPath)
{
	const int length = 600;
	QStringList reals = QDir(realsPath).entryList(fileFilters);

	std::vector<matrix> dat(reals.size());
	int i = 0;
	for(QString fileName : reals)
	{
		myLib::readMatrixFile(realsPath + "/" + fileName,
							  dat[i++]);
	}
	auto res = countEvoked(dat, length);
	myLib::writeMatrixFile(outPath, res);
}

void MainWindow::testSuccessive2()
{
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};
//	const QStringList names {"GAS"};


	std::vector<double> res;
	for(QString name : names)
	{
		/// current best set
		suc::numGoodNewLimit = 3;
		suc::learnSetStay = 40;
		suc::decayRate = 0.00;


		/// should not change averageDatum and sigmaVector ?
		Net * net = new Net();

		net->setSource("w");
		net->setMode("t"); // train-test

		std::cout << name << std::endl;
		res.push_back(
					net->successiveByEDF(path + name + "_train" + ".edf",
										 path + name + "_test" + ".edf")
					.first);
		delete net;
	}
	std::cout << "average by people = "
			  << std::accumulate(std::begin(res), std::end(res), 0.) / res.size() << std::endl;
}

void MainWindow::testSuccessive(const std::vector<double> & vals)
{
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

//	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};
	const QStringList names {"GAS"};

	bool sliceAndCount = true;
//	bool sliceAndCount = false;



	const double windLength	= suc::windLength;
	const double shiftTest	= suc::shiftTest;
	const double shiftLearn	= suc::shiftLearn;
	const int numSmooth		= suc::numSmooth;


	ui->windowLengthSpinBox->setValue(windLength);
	ui->windsButton->setChecked(true); // sliceWindFromReal

	for(QString name : names)
	{
		/// successive
		setEdfFile(path + name + "_train.edf");

		myLib::cleanDir(path + "Reals");
		myLib::cleanDir(path + "winds/fromreal");
		myLib::cleanDir(path + "SpectraSmooth/winds");

		ui->timeShiftSpinBox->setValue(shiftLearn);
		if(sliceAndCount)
		{
			sliceAll();
			myLib::cleanDir(path + "Reals");
//			countSpectraSimple(1024, numSmooth);


			QStringList windsList;
			/// delete first three winds from each realisation
			windsList = QDir(path + "winds/fromreal").entryList({"*_train*.00",
																 "*_train*.01",
																 "*_train*.02"},
																QDir::Files);
			/// delete first some winds from reals
			for(const QString & name : windsList)
			{
				QFile::remove(path + "winds/fromreal/" + name);
			}

			/// magic constant
			/// leave last 600 winds (some will fall out further due to zeros)
			/// REMAKE - leave 120 each type
			myLib::makeFullFileList(path + "winds/fromreal",
									windsList, {def::ExpName.left(3) + "_train"});
			for(int i = 0; i < windsList.length() - 800; ++i) /// constant
			{
				QFile::remove(path + "winds/fromreal/" + windsList[i]);
			}
		}
		setEdfFile(path + name + "_test" + ".edf");
		ui->timeShiftSpinBox->setValue(shiftTest);

		/// DON'T CLEAR, TRAIN winds TAKEN BY SUCCESSIVE
		if(sliceAndCount)
		{
			sliceAll();
			myLib::cleanDir(path + "Reals");
//			countSpectraSimple(1024, numSmooth);
		}
		else
		{
			readData();
		}

		/// current best set
		suc::numGoodNewLimit = 3;
		suc::learnSetStay = 40;
		suc::decayRate = 0.00;


		countSpectraSimple(1024, numSmooth);
		/// should not change averageDatum and sigmaVector ?
		Net * net = new Net();
		net->loadData(def::windsSpectraDir(), {name + "_train"});

		net->setClassifier(ModelType::ANN);
		net->setSource("w");
		net->setMode("t"); // train-test

		std::cout << name << std::endl;
		net->successiveProcessing();
		delete net;


	}
}

void MainWindow::testNewClassifiers()
{
	QString paath = "/media/Files/Data/Feedback/SuccessClass/";

//	for(QString guy : {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"})
	for(QString guy : {"PMI", "SMM", "SMS", "SUA"})
//	for(QString guy : {"GAS"})
	{
		for(QString suff :{"_train", "_test"})
		{
			setEdfFile(paath + guy + suff + ".edf");
//			readData();

			Net * net = new Net();
			net->loadData(paath + "/SpectraSmooth/winds", {def::ExpName});

			net->setClassifier(ModelType::ANN);
			net->setMode("k");
			net->setSource("w");
			net->setNumOfPairs(30);
			net->setFold(4);

//			net->customF(); /// clean to 3*N train windows


//			std::cout << guy + suff << std::endl;
//			net->autoClassification();



			std::vector<std::vector<double>> allPew =  {{3., 30.},
														{3., 40.},
														{4., 40.},
														{4., 50.},
														{5., 50.},
														{6., 60.},
														{8., 80.},
														{8., 100.},
														{10., 100.}
													   };
//			for(auto pewww : allPew)
//			{
			for(int i1 = 3; i1 <= 8; ++i1)
			{
				for(int i2 = 70; i2 <= 110; i2 += 10)
				{
					for(double i3 : {-0.005, 0.00, 0.005})
					{
//						suc::numGoodNewLimit = pewww[0];
//						suc::learnSetStay = pewww[1];
						suc::numGoodNewLimit = i1;
						suc::learnSetStay = i2;
						suc::decayRate = i3;
						std::cout << guy << std::endl;
						std::cout << suc::numGoodNewLimit << '\t';
						std::cout << suc::learnSetStay << '\t';
						std::cout << suc::decayRate << std::endl;
						net->successiveProcessing();
					}
				}

			}

			delete net;
			break; /// only suff = "_train"
		}

//		continue;

		/// loading UCI dataset - add enum
//		net->loadDataUCI("cmi");
//		net->setClassifier(ClassifierType::RDA);
//		net->setRdaLambdaSlot(0.8);
//		net->setRdaShrinkSlot(0.8);
//		net->autoClassification();

	}
}

void MainWindow::BaklushevDraw(const QString & workPath, const QString & edfName)
{
	QString guy = myLib::getFileName(edfName, false);

	QString filePath = workPath + "/" + edfName;
	if(!QFile::exists(filePath))
	{
		std::cout << "file doesn't exist: " << filePath << std::endl;
		std::cout << "nothing will be drawn" << std::endl;
		return;
	}

	setEdfFile(filePath);
	readData();
	Spectre * sp = new Spectre();
	sp->setFftLength(4096);
	delete sp;

	QString spectraPath = workPath + "/SpectraSmooth";

	int numOfReals[2];
	QStringList lst[2];
	QString marker[2] = {"*_241*" , "*_247*"};
	matrix drawMat[2];

	for(int i = 0; i < 2; ++i)
	{
		lst[i] = QDir(spectraPath).entryList({marker[i]});
		numOfReals[i] = lst[i].size();

		drawMat[i] = matrix(numOfReals[i], 1);

		for(int j = 0; j < numOfReals[i]; ++j)
		{
			myLib::readFileInLine(spectraPath + "/" + lst[i][j], drawMat[i][j]);
		}
	}
	double norm = 0;

	std::vector<std::valarray<double>> avArr = {drawMat[0].averageRow(),
												drawMat[1].averageRow()};
	std::vector<std::valarray<double>> sigmArr = {drawMat[0].sigmaOfCols(),
												  drawMat[1].sigmaOfCols()};


	QColor currColor[2] = {QColor("blue"), QColor("red")};

	const QString picture[2] = {workPath + "/" + guy + "_picSpat.jpg",
								workPath + "/" + guy + "_picVerb.jpg"};
	const QString pictures[2] = {workPath + "/"  + guy + "_picSpat_2.jpg",
								 workPath + "/"  + guy + "_picVerb_2.jpg"};


	/// max val of avArr + sigmArr
	norm = std::max((avArr[0] + sigmArr[0]).max(), (avArr[1] + sigmArr[1]).max());
	for(int i = 0; i < 2; ++i)
	{
		myLib::drw::drawArrayWithSigma(myLib::drw::drawTemplate(true),
									   avArr[i],
									   sigmArr[i],
									   norm,
									   currColor[i]).save(pictures[i], nullptr, 100);
	}


	/// max val in any of realisations
	norm = std::max(drawMat[0].maxVal(), drawMat[1].maxVal());
	/// draw each realisation spectre
	for(int i = 0; i < 2; ++i)
	{
		myLib::drw::drawArrays(myLib::drw::drawTemplate(),
							   drawMat[i],
							   norm,
							   false,
							   std::vector<QColor>(drawMat[i].rows(), currColor[i]),
							   std::vector<int>(drawMat[i].rows(), 1)).save(
					picture[i], 0, 100);

	}
}

void MainWindow::countSpectraSimple(int fftLen, int inSmooth)
{
    Spectre * sp = new Spectre();
    sp->setFftLength(fftLen);
    if(inSmooth >= 0)
    {
        sp->setSmooth(inSmooth);
    }
    sp->countSpectraSlot();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
}


void MainWindow::matiPreprocessingSlot()
{
    // for eeg+amod composition


    def::dir->cd(def::dataFolder); // "/media/Files/Data/Mati/"
    QStringList dirList = def::dir->entryList(QDir::Dirs|QDir::NoDotAndDotDot); // list of folders
//    dirList = {"ADA", "BSA", "FEV", "KMX", "NOS", "NVV", "PYV", "SDA", "SDV", "SIV"};
    bool flagCommaDot = false;
    bool flagAmodLogToEdf = false;
    bool flagSliceEdfBySessions = false;
    bool flagAppendAmodToEeg = false;
    bool flagMakeDiffMark = false;
    bool flagSliceSessionsToPieces = false;

//    flagCommaDot = true;
//    flagAmodLogToEdf = true;
//    flagSliceEdfBySessions = true;
//    flagAppendAmodToEeg = true;
    flagMakeDiffMark = true;
//    flagSliceSessionsToPieces = true;


    QString fileSuffix = "_w";
//    fileSuffix = "_rr_f";


    for(int dirNum = 0; dirNum < dirList.length(); ++dirNum)
    {
        if(!dirList[dirNum].contains("SDA")) continue;

        if(flagCommaDot) // change , to . in amod logFiles
        {
            ui->progressBar->setValue(0.);
            QTime myTime;
            myTime.start();
			std::cout << "comma->dot replace start" << std::endl;

            char ch;
            QString helpString;

			helpString = def::dataFolder + "/" + dirList[dirNum] + "/amod";
            def::dir->cd(helpString);
            QStringList lst = def::dir->entryList(QStringList("*.txt"));
            FILE * fil;
            FILE * fil1;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
						+ "/" + dirList[dirNum]
						+ "/amod"
						+ "/" + lst[i];
				fil = fopen(helpString, "r");
                helpString = def::dataFolder
						+ "/" + dirList[dirNum]
						+ "/amod"
						+ "/" + lst[i] + "_";
                fil1 = fopen(helpString, "w");
                while(!feof(fil))
                {
                    fread(&ch, sizeof(char), 1, fil);
                    if(ch == ',') ch = '.';
                    fwrite(&ch, sizeof(char), 1, fil1);
                }
                fclose(fil);
                fclose(fil1);
                ui->progressBar->setValue(i * 100. / lst.length());
            }

			std::cout << "comma->dot replace finish: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
            ui->progressBar->setValue(0.);
        }

        if(flagAmodLogToEdf) // corrected logFiles to amod edfs
        {
            ui->progressBar->setValue(0.);
            QTime myTime;
            myTime.start();
			std::cout << "amod.txt -> amod.edf start" << std::endl;
			QString helpString = def::dataFolder + "/" + dirList[dirNum] + "/amod";
            def::dir->cd(helpString);
            QStringList lst = def::dir->entryList(QStringList("*.txt_")); // _ for corrected logs

            edfFile tempEdf;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
						+ "/" + dirList[dirNum]
						+ "/amod"
						+ "/" + lst[i];
                tempEdf = edfFile(helpString); // edfFile(QString) for MATI only

                helpString = def::dataFolder
						+ "/auxEdfs"
						+ "/" + dirList[dirNum]
						+ "/" + myLib::getExpNameLib(lst[i])
                        + "_amod.edf";
                tempEdf.writeEdfFile(helpString);

                ui->progressBar->setValue(i * 100. / lst.length());
            }
			std::cout << "amod.txt -> amod.edf: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
            ui->progressBar->setValue(0);
        }

        if(flagSliceEdfBySessions) // slice initial edf to edf sessions
        {
            QTime myTime;
            myTime.start();
			std::cout << "slice edf by sessions start" << std::endl;

            QString helpString;
            helpString = def::dataFolder
					+ "/" + dirList[dirNum]
					+ "/" + dirList[dirNum] + fileSuffix
                    + ".edf";
            if(!QFile(helpString).exists())
            {
				std::cout << "cant open ExpName_fileSuffix.edf (flagSliceEdfBySessions)" << std::endl;
                break;
            }
            setEdfFile(helpString);
            sliceMati();

			std::cout << "slice edf by sessions: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
        }


        if(flagAppendAmodToEeg) // append amod data to EEG data
        {
            QTime myTime;
            myTime.start();
			std::cout << "append amod.edf to eeg.edf start" << std::endl;


            QString outPath;
            QString addPath;
            QString helpString;
            edfFile tempEdf;
            for(int type = 0; type < 3; ++type)
            {
                for(int session = 0; session < 6; ++session)
                {
                    // session path
                    helpString = def::dataFolder
							+ "/" + dirList[dirNum]
							+ "/auxEdfs"
							+ "/" + dirList[dirNum] + fileSuffix
							+ "_" + nm(type)
							+ "_" + nm(session)
                            + ".edf";

                    if(!QFile(helpString).exists())
                    {
						std::cout << "cant open session.edf (flagAppendAmodToEeg)" << std::endl;
                        continue;
                    }

                    tempEdf.readEdfFile(helpString);

                    // amod file
                    addPath = def::dataFolder
							+ "/" + dirList[dirNum]
							+ "/auxEdfs"
							+ "/" + dirList[dirNum]
							+ "_" + nm(type)
							+ "_" + nm(session)
                            + "_amod.edf"; // generality

                    // eeg+amod path
                    outPath = def::dataFolder
							+ "/" + dirList[dirNum]
							+ "/auxEdfs"
							+ "/" + dirList[dirNum] + fileSuffix
                            + "_a"
							+ "_" + nm(type)
							+ "_" + nm(session)
                            + ".edf"; // generality

                    tempEdf.vertcatFile(addPath, outPath);

#if 0
                    // copy the same files into amod dir

                    helpString = def::dataFolder
								 + "/" + dirList[dirNum]
								 + "/amod"
								 + "/" + dirList[dirNum] + fileSuffix
                                 + "_a"
								 + "_" + nm(type)
								 + "_" + nm(session)
                                 + ".edf"; // generality
                    if(QFile::exists(helpString)) QFile::remove(helpString);
                    QFile::copy(outPath, helpString);
#endif


                }
            }
			std::cout << "append amod.edf to eeg.edf: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
        }

        if(flagMakeDiffMark) // make files of markers differences
        {
            QTime myTime;
            myTime.start();
			std::cout << "make diffMark files start" << std::endl;

            edfFile fil;
            QString diffMark;
            QString helpString;
            for(int type = 0; type < 3; ++type)
            {
                for(int session = 0; session < 6; ++session)
                {
                    // eeg+amod path
                    helpString = def::dataFolder
							+ "/" + dirList[dirNum]

							+ "/auxEdfs"

							+ "/" + dirList[dirNum] + fileSuffix
                            + "_a"
							+ "_" + nm(type)
							+ "_" + nm(session)
                            + ".edf";
                    if(!QFile::exists(helpString)) continue;

                    diffMark = helpString;
                    diffMark.replace(".edf", "_diffMark.txt");

                    fil.readEdfFile(helpString);
					std::ofstream outStr(diffMark.toStdString().c_str());
					outStr << "board" << '\t' << "amod" << '\t' << std::endl;

                    for(int i = 0; i < fil.getDataLen(); ++i)
                    {
                        if(fil.getData()[fil.getMarkChan()][i] == 0) continue;
						if(myLib::matiCountBit(fil.getData()[fil.getMarkChan()][i], 14))
                        {
                            for(int j = i - 40; j < i + 40; ++j)
                            {
                                if(fil.getData()[37][j+1] != fil.getData()[37][j]) // 37 for EEG + AMOD
                                {
                                    outStr << i << "\t" << j+1 << "\t" << (i-j-1)*4 << '\n';
                                    break;
                                }
                            }
                        }
                    }
                    outStr.flush();
                    outStr.close();
                }
            }

			std::cout << "make diffMark files: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
        }

        if(flagSliceSessionsToPieces) // slice constructed eeg+amod files for small pieces
        {
            // not finished
            QTime myTime;
            myTime.start();
			std::cout << "slice sessions to pieces start" << std::endl;

            QString outPath;
            QString helpString;
            helpString = def::dataFolder
					+ "/" + dirList[dirNum]

					+ "/auxEdfs"

					+ "/" + dirList[dirNum] + fileSuffix
                    + "_a"
                    + "_0"
                    + "_1"
                    + ".edf"; // generality
            outPath = def::dataFolder
					+ "/" + dirList[dirNum]
					+ "/" + dirList[dirNum] + fileSuffix
                    + "_a"
                    + ".edf"; // generality
            if(QFile::exists(outPath)) QFile::remove(outPath);
            QFile::copy(helpString, outPath);

            setEdfFile(outPath);

            sliceMatiPieces(true);

			std::cout << "slice sessions to pieces: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
        }
    }
}
