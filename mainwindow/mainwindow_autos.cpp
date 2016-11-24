#ifndef AUTOS_CPP
#define AUTOS_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace myLib;
using namespace smallLib;

void MainWindow::testSuccessive()
{
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";
	setEdfFile(path + "GAS_train.edf");
	readData();

	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};
//    const QStringList names {"GAS"};

//	bool sliceAndCount = true;
	bool sliceAndCount = false;

	ui->timeShiftSpinBox->setValue(2.);
	ui->windowLengthSpinBox->setValue(4.);
	ui->windsButton->setChecked(true); // sliceWindFromReal

	for(QString name : names)
	{
		/// successive
		setEdfFile(path + name + "_train.edf");

		cleanDir(path + "Reals");

		ui->timeShiftSpinBox->setValue(2.);
		if(sliceAndCount)
		{
			sliceAll();
			cleanDir(path + "Reals");


			QStringList windsList;
			// delete first three winds from each realisation
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
			makeFullFileList(path + "winds/fromreal",
							 windsList, {def::ExpName.left(3) + "_train"});
			for(int i = 0; i < windsList.length() - 800; ++i) /// constant
			{
				QFile::remove(path + "winds/fromreal/" + windsList[i]);
			}
		}

		setEdfFile(path + name + "_test" + ".edf");
		ui->timeShiftSpinBox->setValue(1.); /// really should be 0.5
		/// DON'T CLEAR, TRAIN winds TAKEN BY SUCCESSIVE

		if(sliceAndCount)
		{
			sliceAll();
			cleanDir(path + "Reals");
			countSpectraSimple(1024, 8);
		}
		else
		{
			readData();
		}

		/// current best set
		suc::numGoodNewLimit = 3;
		suc::learnSetStay = 40;
		suc::decayRate = 0.00;

		/// should not change averageDatum and sigmaVector
		Net * net = new Net();
		net->loadData(def::windsSpectraDir(), {name + "_train"});

		net->setClassifier(ClassifierType::ANN);
		net->setSource("w");
		net->setMode("t"); // train-test

		cout << name << endl;
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

			net->setClassifier(ClassifierType::ANN);
			net->setMode("k");
			net->setSource("w");
			net->setNumOfPairs(30);
			net->setFold(4);

//			net->customF(); /// clean to 3*N train windows


//			cout << guy + suff << endl;
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
						cout << guy << endl;
						cout << suc::numGoodNewLimit << '\t';
						cout << suc::learnSetStay << '\t';
						cout << suc::decayRate << endl;
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

void MainWindow::BaklushevDraw()
{

	QString dr = "/media/Files/Data/Baklushev";
	for(QString guy : {"ANO"})
//	for(QString guy : QDir(dr).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
		QString filePath = dr + slash + guy + slash + guy + "_draw.edf";
		if(!QFile::exists(filePath)) continue;

		setEdfFile(filePath);

//		readData();
//		sliceBak(1, 60, "241");
//		sliceBak(61, 120, "247");
//		sliceBak(121, 180, "241");
//		sliceBak(181, 240, "247");
//		exit(0);

//		sliceAll();
//		countSpectraSimple(2048, 3);
//		exit(0);

		Spectre * sp = new Spectre();
		sp->setFftLength(2048);
		delete sp;

		QString spectraPath = dr + slash + guy + slash + "SpectraSmooth";




		QPixmap pics[2];
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
				readFileInLine(spectraPath + slash + lst[i][j], drawMat[i][j]);
			}
		}


		double norm = max(drawMat[0].maxVal(), drawMat[1].maxVal());
		norm = 16;
		QColor currColor;

		const QString picture[2] = {dr + slash + guy + slash + guy + "_picSpat.jpg",
									dr + slash + guy + slash + guy + "_picVerb.jpg"};
		const QString pictures[2] = {dr + slash + guy + slash + guy + "_picSpat_2.jpg",
									dr + slash + guy + slash + guy + "_picVerb_2.jpg"};
		for(int i = 0; i < 2; ++i)
		{
			currColor = ((i == 0) ? "blue" : "red");
			pics[i] = drawTemplate(QString(), true, 1600, 1600);

			pics[i].save(pictures[i], 0, 100);
			auto avArr = drawMat[i].averageRow();
			auto sigmArr = drawMat[i].sigmaOfCols();
//			norm = max(avArr.max() + sigmArr.max(), drawMat[i].maxVal());
			drawArrayWithSigma(pictures[i], avArr, sigmArr,
							   norm,
							   currColor.name(), 2);

			pics[i] = drawArrays(pics[i], drawMat[i], false, spectraGraphsNormalization::all,

//								 drawMat[i].maxVal(),
								 norm,

								 std::vector<QColor>(drawMat[i].rows(), currColor),
								 1,
								 1);
			pics[i].save(picture[i], 0, 100);

		}
		exit(7);
	}
	exit(2);
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
            cout << "comma->dot replace start" << endl;

            char ch;
            QString helpString;

            helpString = def::dataFolder + slash + dirList[dirNum] + slash + "amod";
            def::dir->cd(helpString);
            QStringList lst = def::dir->entryList(QStringList("*.txt"));
            FILE * fil;
            FILE * fil1;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
                        + slash + dirList[dirNum]
                        + slash + "amod"
                        + slash + lst[i];
                fil = fopen(helpString, "r");
                helpString = def::dataFolder
                        + slash + dirList[dirNum]
                        + slash + "amod"
                        + slash + lst[i] + "_";
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

            cout << "comma->dot replace finish: time = " << myTime.elapsed()/1000. << " sec" << endl;
            ui->progressBar->setValue(0.);
        }

        if(flagAmodLogToEdf) // corrected logFiles to amod edfs
        {
            ui->progressBar->setValue(0.);
            QTime myTime;
            myTime.start();
            cout << "amod.txt -> amod.edf start" << endl;
            QString helpString = def::dataFolder + slash + dirList[dirNum] + slash + "amod";
            def::dir->cd(helpString);
            QStringList lst = def::dir->entryList(QStringList("*.txt_")); // _ for corrected logs

            edfFile tempEdf;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
                        + slash + dirList[dirNum]
                        + slash + "amod"
                        + slash + lst[i];
                tempEdf = edfFile(helpString); // edfFile(QString) for MATI only

                helpString = def::dataFolder
                        + slash + "auxEdfs"
                        + slash + dirList[dirNum]
                        + slash + getExpNameLib(lst[i])
                        + "_amod.edf";
                tempEdf.writeEdfFile(helpString);

                ui->progressBar->setValue(i * 100. / lst.length());
            }
            cout << "amod.txt -> amod.edf: time = " << myTime.elapsed()/1000. << " sec" << endl;
            ui->progressBar->setValue(0);
        }

        if(flagSliceEdfBySessions) // slice initial edf to edf sessions
        {
            QTime myTime;
            myTime.start();
            cout << "slice edf by sessions start" << endl;

            QString helpString;
            helpString = def::dataFolder
                    + slash + dirList[dirNum]
                    + slash + dirList[dirNum] + fileSuffix
                    + ".edf";
            if(!QFile(helpString).exists())
            {
                cout << "cant open ExpName_fileSuffix.edf (flagSliceEdfBySessions)" << endl;
                break;
            }
            setEdfFile(helpString);
            sliceMati();

            cout << "slice edf by sessions: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }


        if(flagAppendAmodToEeg) // append amod data to EEG data
        {
            QTime myTime;
            myTime.start();
            cout << "append amod.edf to eeg.edf start" << endl;


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
                            + slash + dirList[dirNum]
                            + slash + "auxEdfs"
                            + slash + dirList[dirNum] + fileSuffix
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf";

                    if(!QFile(helpString).exists())
                    {
                        cout << "cant open session.edf (flagAppendAmodToEeg)" << endl;
                        continue;
                    }

                    tempEdf.readEdfFile(helpString);

                    // amod file
                    addPath = def::dataFolder
                            + slash + dirList[dirNum]
                            + slash + "auxEdfs"
                            + slash + dirList[dirNum]
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + "_amod.edf"; // generality

                    // eeg+amod path
                    outPath = def::dataFolder
                            + slash + dirList[dirNum]
                            + slash + "auxEdfs"
                            + slash + dirList[dirNum] + fileSuffix
                            + "_a"
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf"; // generality

                    tempEdf.vertcatFile(addPath, outPath);

#if 0
                    // copy the same files into amod dir

                    helpString = def::dataFolder
                                 + slash + dirList[dirNum]
                                 + slash + "amod"
                                 + slash + dirList[dirNum] + fileSuffix
                                 + "_a"
                                 + "_" + QString::number(type)
                                 + "_" + QString::number(session)
                                 + ".edf"; // generality
                    if(QFile::exists(helpString)) QFile::remove(helpString);
                    QFile::copy(outPath, helpString);
#endif


                }
            }
            cout << "append amod.edf to eeg.edf: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }

        if(flagMakeDiffMark) // make files of markers differences
        {
            QTime myTime;
            myTime.start();
            cout << "make diffMark files start" << endl;

            edfFile fil;
            QString diffMark;
            QString helpString;
            for(int type = 0; type < 3; ++type)
            {
                for(int session = 0; session < 6; ++session)
                {
                    // eeg+amod path
                    helpString = def::dataFolder
                            + slash + dirList[dirNum]

                            + slash + "auxEdfs"

                            + slash + dirList[dirNum] + fileSuffix
                            + "_a"
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf";
                    if(!QFile::exists(helpString)) continue;

                    diffMark = helpString;
                    diffMark.replace(".edf", "_diffMark.txt");

                    fil.readEdfFile(helpString);
                    ofstream outStr(diffMark.toStdString().c_str());
                    outStr << "board" << '\t' << "amod" << '\t' << endl;

                    for(int i = 0; i < fil.getDataLen(); ++i)
                    {
                        if(fil.getData()[fil.getMarkChan()][i] == 0) continue;
                        if(matiCountBit(fil.getData()[fil.getMarkChan()][i], 14))
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

            cout << "make diffMark files: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }

        if(flagSliceSessionsToPieces) // slice constructed eeg+amod files for small pieces
        {
            // not finished
            QTime myTime;
            myTime.start();
            cout << "slice sessions to pieces start" << endl;

            QString outPath;
            QString helpString;
            helpString = def::dataFolder
                    + slash + dirList[dirNum]

                    + slash + "auxEdfs"

                    + slash + dirList[dirNum] + fileSuffix
                    + "_a"
                    + "_0"
                    + "_1"
                    + ".edf"; // generality
            outPath = def::dataFolder
                    + slash + dirList[dirNum]
                    + slash + dirList[dirNum] + fileSuffix
                    + "_a"
                    + ".edf"; // generality
            if(QFile::exists(outPath)) QFile::remove(outPath);
            QFile::copy(helpString, outPath);

            setEdfFile(outPath);

            sliceMatiPieces(true);

            cout << "slice sessions to pieces: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }
    }
}


#endif // AUTOS_CPP
