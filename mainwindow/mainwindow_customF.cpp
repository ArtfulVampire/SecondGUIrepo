#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;
using namespace myLib;
using namespace smallLib;



void MainWindow::customFunc()
{
    ui->matiCheckBox->setChecked(false);
	ui->realsButton->setChecked(true);

//	return;

#if 0
	/// Baklushev draw

	QString dr = "/media/Files/Data/Baklushev";
	for(QString guy : QDir(dr).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
		QString filePath = dr + slash + guy + slash + guy + ".EDF";

		edfFile fil;
		fil.readEdfFile(filePath);
		std::vector<int> rdc(22);
		std::iota(std::begin(rdc), std::end(rdc), 0);
		rdc[19] = 22;
		rdc[20] = 23;
		rdc.back() = fil.getMarkChan();


		QString newPath = dr + slash + guy + slash + guy + ".edf";
		(fil.reduceChannels(rdc)).writeEdfFile(newPath);
	}
	exit(2);
#endif


#if 0
	/// IITP
	def::ntFlag = true;

	/// make edfs from dats
	QString folder = "XIrinaX";
	QString guy = "Ira";
//	repair::deleteSpacesDir(def::iitpFolder + slash + folder);
//	repair::toLatinDir(def::iitpFolder + slash + folder);
//	exit(0);
	QString ExpNamePre;
	QString ExpName;
	edfFile fil;
	for(int fileNum = 0; fileNum < 30; ++fileNum)
	{
//		fileNum = 14;
		QString num = rightNumber(fileNum, 2);
		ExpNamePre = def::iitpFolder + slash +
					 folder + slash + guy + "_";


#if 0
		/// dat to edf
		ExpName = ExpNamePre + num + ".dat";
		if(!QFile::exists(ExpName)) continue;
		edfFile fil1(ExpName, inst::iitp);
		ExpName.replace(".dat", ".edf");
		fil1.writeEdfFile(ExpName);
		continue;
#endif

#if 01
		/// upsample EEGs
		ExpName = ExpNamePre + "eeg_" + num + ".edf";
		if(!QFile::exists(ExpName)) continue;
		fil.readEdfFile(ExpName);

		ExpName = ExpNamePre + "eeg_" + num + "_up.edf";
		fil.upsample(1000., ExpName);

//		break;

		continue;
#endif

#if 0
		/// filter EEG edfs
		ExpName = ExpNamePre + "eeg_" + num + ".edf";
		if(!QFile::exists(ExpName)) continue;
		fil.readEdfFile(ExpName);

		ExpName.replace(".edf", "_f.edf");
		fil.refilter(0.5, 70);
		fil.refilter(45, 55, ExpName, true);
		continue;
#endif

#if 0
		/// filter EEGs double notch
		ExpName = ExpNamePre + "eeg_" + num + ".edf";
		if(!QFile::exists(ExpName)) continue;
		fil.readEdfFile(ExpName);

		ExpName.replace(".edf", "_2notch.edf");
		fil.refilter(45, 55, {}, true);
		fil.refilter(95, 105, ExpName, true);
		continue;
#endif

#if 0
		/// downsample EMGs
		ExpName = ExpNamePre + num + ".edf";
		if(!QFile::exists(ExpName)) continue;
		fil.readEdfFile(ExpName);

		ExpName = ExpNamePre + num + "_dwn.edf";
		fil.downsample(250., ExpName);

//		break;

//		continue;
#endif

#if 0
		/// filter downsampled EMGs double notch
		ExpName = ExpNamePre + num + ".edf";
		if(!QFile::exists(ExpName)) continue;
		fil.readEdfFile(ExpName);

		fil.refilter(45, 55, {}, true);
		fil.refilter(95, 105, {}, true);
		ExpName = ExpNamePre + "emg_" + num + ".edf";
		fil.writeEdfFile(ExpName);

//		break;

//		continue;
#endif

	}
	return;
	exit(0);
#endif


#if 0
	/// test new classifiers
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
    exit(0);
#endif

#if 0
	/// successive auxiliary
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};


	ui->realsButton->setChecked(true); // sliceWindFromReal

	for(QString name : names)
	{
		def::drawNorm = -1;
		setEdfFile(path + name + "_train.edf");
		sliceAll();
		countSpectraSimple(4096, 15);

		setEdfFile(path + name + "_test.edf");
		sliceAll();
		countSpectraSimple(4096, 15);
//		exit(0);
	}
	exit(0);

#endif


#if 0
/// successive
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
    exit(0);
#endif

#if 0
	/// Xenia - check all files markers
	QString p = def::XeniaFolder + "/15Nov";
	QDir dr(p);
	for(QString str : {"healthy", "moderate_TBI", "severe_TBI"})
	{
		dr.cd(str);
		for(QString str2 : dr.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
		{
			dr.cd(str2);
			auto lst = dr.entryList(QDir::Files);
			for(QString marker : {"_bd", "_bw", "_cr", "_fon", "_kh", "_no", "_sm"})
			{
				bool peeew = false;
				for(auto str3 : lst)
				{
					if(str3.contains(marker + ".edf")) peeew = true;
				}
				if(!peeew)
				{
					cout << dr.absolutePath() << ": no " << marker << endl;
				}
			}


			dr.cdUp();
		}
		dr.cdUp();
	}
	exit(0);
#endif

#if 0
	/// Xenia cut
	QString initPath = "/media/Files/Data/Xenia/15Nov";
	for(QString str : {"healthy", "moderate_TBI", "severe_TBI"})
	{
		QString workPath = initPath + slash + str;

		/// repair dirs and files
//		repair::deleteSpacesFolders(workPath);
//		repair::toLatinDir(workPath, {});
//		repair::toLowerDir(workPath, {});
//		continue;

		/// list of guys
		QStringList dirs = QDir(workPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		if(1)
		{
			for(QString guy : dirs)
				//		QString guy = "Larina_Irina_Igorevna_30";
			{
//				QStringList files = QDir(workPath + slash + guy).entryList(def::edfFilters);
//				for(QString fileName : files)
//				{

					edfFile fil;

//					if(fil.getPhysMax()[0] > 4100)
//					{
//						cout << workPath + slash + guy + slash + fileName << endl;
//						break;
//					}

//					if(fileName.contains("Portnova") || fileName.contains("Natasha_Ber"))
//					{
//						fil.readEdfFile(workPath + slash + guy + slash + fileName);
//						fil.refilter(1.6, 30,
//									 workPath + slash + guy + slash + fileName,
//									 false);
//						cout << fileName << endl;
//					}

//				}

				autos::GalyaCut(workPath + slash + guy, 8,
								workPath + "_cut" + slash + guy);
			}
		}

		if(0)
		{
			wvlt::initMtlb();
			for(QString deer : dirs) /// each guy
			{
				QString pew = workPath + slash + deer;
				autos::GalyaProcessing(pew, 19, workPath + slash + getFileName(workPath) + "_results");
				autos::GalyaWavelets(pew, 19, 250, workPath + slash + getFileName(workPath) + "_wavelet");
			}
			wvlt::termMtlb();
		}

		if(0)
		{
			ui->rereferenceDataComboBox->setCurrentText("Base");
			for(QString deer : dirs) /// each guy
			{
				QString pew = workPath + slash + deer;
				for(QString fil : QDir().entryList(def::edfFilters)) /// each file
				{
					setEdfFile(pew + slash + fil);
					rereferenceDataSlot();
				}
			}
		}
	}
    exit(0);
#endif

#if 01
	/// tables
	def::ntFlag = true;

//	QStringList markers{"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"};
	QStringList markers{"_isopropanol", "_vanilla", "_needles", "_brush",
						"_cry", "_fire", "_flower", "_wc"};

//	QString tbi_path = def::XeniaFolder + "/3Nov";
	QString tbi_path = "/media/Files/Data/Dasha";

//	QStringList subdirs{"severe_TBI", "moderate_TBI", "severe_TBI"};
	QStringList subdirs{"Totable"};



#if 01
	/// make tables by stimulus
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + slash + subdir + "_tmp2";
		for(QString marker : markers)
		{
			autos::makeTableFromRows(workPath,
									 tbi_path + slash + subdir + "_table" + marker + ".txt",
									 marker);
		}
	}
#endif

#if 01
	/// make tables whole
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + slash + subdir + "_OUT";
		autos::makeTableFromRows(workPath,
								 tbi_path + slash + subdir + "_all" + ".txt");
	}
#endif
	exit(0);


	/// count
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + slash + subdir;

		/// list of guys
		QStringList guys = QDir(workPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		for(QString guy : guys)
		{

			QStringList t = QDir(workPath + slash + guy).entryList(def::edfFilters);
			if(t.isEmpty()) continue;

			QString ExpName = t[0];
			ExpName = ExpName.left(ExpName.lastIndexOf('_'));

			autos::GalyaProcessing(workPath + slash + guy,
								   19,
								   workPath + "_tmp");

			autos::GalyaWavelets(workPath + slash + guy,
								 19,
								 250,
								 workPath + "_tmp");
			QStringList fileNames;
			for(QString marker : markers)
			{
				fileNames.clear();
				for(QString typ : {"_alpha", "_d2_dim", "_med_freq", "_spectre", "_wavelet"})
				{
					fileNames << ExpName + marker + typ + ".txt";
				}
				autos::XeniaArrangeToLine(workPath + "_tmp",
										  fileNames,
										  workPath + "_tmp2" + slash
										  + ExpName + marker + ".txt"); /// guy <-> ExpName
			}

			fileNames.clear();
			for(QString marker : markers)
			{
				fileNames <<  ExpName + marker + ".txt"; /// guy <-> ExpName
			}
			autos::XeniaArrangeToLine(workPath + "_tmp2",
									  fileNames,
									  workPath + "_OUT" + slash
									  + ExpName + ".txt"); /// guy <-> ExpName
		}
	}
	exit(0);
#endif


#if 0
    /// EEG fMRI
	def::ntFlag = false;

	QString guy = "Senotov";
//	for(QString guy : subjects::leest_mri)
//	for(QString guy : leest)
	{
//		autos::GalyaCut(def::mriFolder + slash + guy, 2);
		autos::GalyaFull(def::mriFolder + slash + guy + slash + guy + "_winds_cleaned");
	}

//	QString type = "_med.getFreq()";
//	areSimilarFiles("/media/Files/Data/MRI/OUT/" + guy + "_old/" + guy + type + ".txt",
//					"/media/Files/Data/MRI/OUT/" + guy + "/" + guy + type + ".txt");

    exit(0);
#endif



#if 0
	/// backuping
	///
	/// find -type f -regextype sed -iregex ".*/[^/]\{3,5\}\.edf" | sed 's/\.\//\/media\/Files\/Data\/Feedback\//g'
	/// find -type f -regextype sed -iregex ".*/[^/]\{3\}\(_train\|_test\|_FB\|E\)\.edf"
	/// find -type f -regextype sed -iregex ".*/[^/]\{3\}\(_train\|_test\|_FB\|E\|_1\|_3\)\.edf"


	QDir tmp("/media/Files/Data/Twins");
	QDir dest("/media/michael/Seagate Expansion Drive/Michael/Data/BACKUPS/Twins");
	for(auto deer : tmp.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
//		if(deer.startsWith("Succ")) continue;
		tmp.cd(deer);
		dest.mkdir(deer);
		dest.cd(deer);

		QString edfF = tmp.entryList(def::edfFilters, QDir::Files, QDir::Time|QDir::Reversed)[0];
		cout << edfF << endl;

		QFile::copy(tmp.absolutePath() + slash + edfF,
					dest.absolutePath() + slash + edfF);

		tmp.cdUp();
		dest.cdUp();
	}
	exit(0);
#endif

#if 0
	/// files copying
	QDir tmp("/media/Files/Data/MRI/OUT");
	for(const QString & deer : tmp.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
		QString pth = tmp.absolutePath() + slash + deer;
		for(const QString & feel : QDir(pth).entryList({"*_med*", "*_d2*"}))
		{
			QFile::copy(pth + slash + feel,
						tmp.absolutePath() + slash + feel);
		}
	}
	exit(0);

#endif


#if 0
	/// Dasha rename files totable
	QDir dr("/media/Files/Data/Dasha/Totable");
	QStringList lst = dr.entryList(def::edfFilters);
	for(QString str : lst)
	{
		if(!str.contains(".edf") && !str.contains(".EDF"))
		{
			QFile::rename(dr.absolutePath() + slash + str,
						  dr.absolutePath() + slash + str + ".edf");
		}

		if(str.contains("_48.edf"))
		{
			QString newName = str;
			newName = newName.left(newName.indexOf('_'));
			newName += "_wc.edf";
			QFile::rename(dr.absolutePath() + slash + str,
						  dr.absolutePath() + slash + newName);
		}

#if 01
		/// copy to folders
		QString newName = str;
		newName = newName.left(newName.indexOf('_'));
		if(!dr.exists(newName))
		{
			dr.mkdir(newName);
		}
		QFile::copy(dr.absolutePath() + slash + str,
					dr.absolutePath() + slash + newName + slash + str);
#endif

	}
exit(0);
#endif

#if 0
    /// Dasha processing

    def::ntFlag = true; /// for Dasha's and EEGMRI
//    def::ntFlag = false; /// encephalan (+1)



    /// 32 channels for EEGMRI
    for(auto str : leest_more)
    {
        testChannelsOrderConsistency(def::DashaFolder + slash + str);
        deleteSpaces(def::DashaFolder + slash + str);
//        channelsOrderDir(def::DashaFolder + slash + str,
//                       def::DashaFolder + slash + "Nonaudio_to_less",
//                       coords::lbl31_less);

//        GalyaProcessing(def::DashaFolder + "/AUDIO/" + str,
//                        31,
//                        def::DashaFolder + "/AUDIO/CHANS/Audio_to_less_out_ALL");
//            GalyaCut(def::DashaFolder + "/NONAUDIO/" + str,
//                     16,
//                     def::DashaFolder + "/AUDIO/CHANS/Audio_to_less_13");

    }

    exit(0);
#endif


#if 0
	/// make current numbers to rightNumbers in edf files

    const QString listFile = "/media/michael/Files/Data/list.txt";
    int count = 0;
    QString oldPath;
    QString newPath;
    for(int i = 1; i < 100; ++i)
    {
        QString hlp = "rm " + listFile;
        system(hlp.toStdString().c_str());

        QString filter = "_" + QString::number(i) + ".edf";
        QString newFilter = "_" + rightNumber(i, 3) + ".edf";

        hlp = "find " + def::mriFolder + " | grep " + filter + " >> " + listFile;
        system(hlp.toStdString().c_str());

        QFile inStr(listFile);
        inStr.open(QIODevice::ReadOnly);
        while(!inStr.atEnd())
        {
            newPath = oldPath = QString(inStr.readLine()).remove('\n');
            newPath.replace(filter, newFilter);
            cout << oldPath << endl;
            cout << newPath << endl;
            cout << QFile::rename(oldPath, newPath) << endl;

        }
        inStr.close();
        ++count;
    }
    exit(0);
#endif


#if 0
	/// averaging data in results files
	const QString pth = "/media/Files/Data/Feedback/SuccessClass/Help/Succ";
//	for(QString suf :{"_z", "_noz"})
	QString suf = "_z_more";

	{
		QDir dr(pth + suf);
		QString outFilePath = "/media/Files/Data/Feedback/SuccessClass/avres.txt";

		auto fls = dr.entryList(QDir::Files);
		for(QString nam : fls)
		{
			std::vector<std::vector<double>> values(5);
			double tmp;
			std::string tmpStr;
			QString filePath = pth + suf + slash + nam;
			ifstream inStr;
			inStr.open(filePath.toStdString());
			while(inStr.good())
			{
				for(int i = 0; i < 5; ++i)
				{
					inStr >> tmp;
					values[i].push_back(tmp);
				}
				inStr >> tmpStr;
			}
			inStr.close();

			ofstream outStr;
			outStr.open(outFilePath.toStdString(), std::ios_base::app);
			auto splitList = nam.split("_", QString::SkipEmptyParts);
			outStr << splitList[1] << '\t'
								   << splitList[2] << '\t'
								   << splitList[3].remove(".txt") << '\t';
			for(int i = 0; i < 5; ++i)
			{
				outStr << doubleRound( std::accumulate(std::begin(values[i]),
										  std::end(values[i]) - 1, 0.) / (values[i].size() - 1), 2) << '\t';
			}
			outStr << "\r\n";
			outStr.close();
		}

	}
	exit(0);
#endif



#if 0
	/// thesholding pictures
	QImage pic;
	pic.load("/media/Files/Rep/home2.png");
	const int thr = 170;
	for(int i = 0; i < pic.width(); ++i)
	{
		for(int j = 0; j < pic.height(); ++j)
		{
			QColor color = QColor(pic.pixel(i, j));

			if(color.red() > thr &&
			   color.green() > thr &&
			   color.blue() > thr)
			{
				pic.setPixelColor(i, j, QColor("white"));
			}
			else
			{
				pic.setPixelColor(i, j, QColor("black"));
			}
		}
	}
	pic.save("/media/Files/Rep/home2_.jpg", nullptr, 100);
	exit(0);
#endif

#if 0
	/// conference drawing
	std::vector<std::valarray<double>> myArr(4);
	std::vector<QString> colors {"red", "green", "blue", "orange"};
	for(int i = 0; i < myArr.size(); ++i)
	{
		std::valarray<double> & my = myArr[i];
		my.resize(250);
		/// fil random
	}
	/// 16 ~ 1 Hz
	myArr[0][18] = 50.; // 6 Hz
	myArr[1][83] = 90.; // 10 Hz
	myArr[2][105] = 70.; // 11 Hz
	const int r = 250;
	const int l = 100;
	for(int i = l; i < r; ++i)
	{
		std::valarray<double> & my = myArr[3];
		const double arg = (i - (l+r) / 2) / double(r - l)*2;
		my[i] += 1.6 / (1. +  exp (-arg * 4));
	}
//	std::for_each(std::begin(myArr[3]) + 190, std::end(myArr[3]) - 20, 10.);// 15-20 Hz
//	my[3][80] = 50.;


	for(int i = 0; i < myArr.size(); ++i)
	{
		std::valarray<double> & my = myArr[i];
		my = myLib::smoothSpectre(my, 15);
		auto myDraw = my;
		std::for_each(std::begin(myDraw), std::end(myDraw),
					  [](double & in){in += 0.5 + (rand() % 100) / 500.;});
		myDraw = myLib::smoothSpectre(myDraw, 2);
		myLib::drawOneArray(myDraw, "/media/Files/Data/gr_" + QString::number(i) + ".jpg",
							"black");
	}

	std::valarray<double> al = myArr[0] + myArr[1] + myArr[2] + myArr[3];
	std::for_each(std::begin(al), std::end(al),
				  [](double & in){in += 0.5 + (rand() % 100) / 500.;});
	al = myLib::smoothSpectre(al, 2);
	myLib::drawOneArray(al, "/media/Files/Data/gr_al.jpg",
						"black");

	std::valarray<double> al2 = 1.1 * myArr[0] + 0.9 * myArr[1] + 1.4 * myArr[2] + 0.3 * myArr[3];
	std::for_each(std::begin(al2), std::end(al2),
				  [](double & in){in += 0.5 + (rand() % 100) / 500.;});
	al2 = myLib::smoothSpectre(al2, 2);
	myLib::drawOneArray(al2, "/media/Files/Data/gr_al2.jpg",
						"black");


	exit(0);
#endif

#if 0
    /// batch recursively rename files in a dir
	QString renameDir = "/media/michael/Files/Data/iitp/XIX";
    std::vector<std::pair<QString, QString>> renam{
		{"Ира", "Ira_"}
//    {"Ali-zade", "Ali-Zade"},
//    {"ZavylovaV", "Zavyalova"},
//    {"GAVRILOV", "Gavrilov"},
//    {"atanov", "Atanov"},
//    {"KhoKhlov", "Khokhlov"},
//    {"LevandoA", "Levando"},
//    {"novoselova", "Novoselova"},
//    {"sushinsky", "Sushinsky"},
//    {"ZavylovaV", "Zavyalova"},
};
	const QString listFile = "/media/michael/Files/Data/iitp/list.txt";
    int count = 0;
    QString oldPath;
    QString newPath;
    for(auto paar : renam)
    {
        QString hlp = "rm " + listFile;
        system(hlp.toStdString().c_str());

        hlp = "find " + renameDir + " | grep " + paar.first + " >> " + listFile;
        system(hlp.toStdString().c_str());

        QFile inStr(listFile);
        inStr.open(QIODevice::ReadOnly);
        while(!inStr.atEnd())
        {
            newPath = oldPath = QString(inStr.readLine()).remove('\n');
            newPath.replace(paar.first, paar.second);
            cout << oldPath << endl;
            cout << newPath << endl;
            cout << QFile::rename(oldPath, newPath) << endl;

        }
        inStr.close();
        break;
        ++count;
    }
    exit(0);
#endif

#if 0
    /// compare data
    edfFile fil;
    fil.readEdfFile(def::DashaFolder + "/Tinyanova/Tinyanova_Varya_Exam_3.edf");
    matrix dat1 = fil.getData(); dat1.resize(31);
    fil.readEdfFile(def::DashaFolder + "/Tinyanova/Tinyanova_Varya_Exam_3_new.edf");
    matrix dat2 = fil.getData(); dat2.resize(31);
    cout << (dat1 == dat2) << endl;
#endif

#if 0
    /// compare files in folders
	QString hgf = "/media/michael/Files/Data/Dasha/AUDIO/CHANS/Audio_to_less/Audio_to_less_windows/";
	QString q = "/media/michael/KINGSTON/GOODCHANS_to_less_windows/";
    QStringList leeest = QDir(q).entryList(QDir::Files);
    for(QString name : leeest)
    {
        areEqualFiles(q + name,
                      hgf + name);
    }
#endif

#if 0
    /// clean "new" Dasha files
	QStringList leest_audio = subjects::leest_more + subjects::leest_less;
	leest_audio.sort(Qt::CaseInsensitive); /// alphabet order
    const QString work = def::DashaFolder + "/CHANS/Audio_to_less_out_ALL";
    QDir deer(work);

    QStringList subdirs = deer.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    for(auto subdir : subdirs)
    {
        deer.cd(subdir);
        QString tablePath = deer.absolutePath() + slash + "Audio_" + subdir + ".txt";

        QFile outStr(tablePath);
        outStr.open(QIODevice::WriteOnly);

//        QStringList news = deer.entryList({"*_new_*"});
//        for(const QString & oneNew : news)
//        {
//            QString oldName = oneNew; oldName.replace("_new_", "_");

//            QFile::remove(deer.absolutePath() + slash + oldName);
//            QFile::copy(deer.absolutePath() + slash + oneNew,
//                        deer.absolutePath() + slash + oldName);
//            QFile::remove(deer.absolutePath() + slash + oneNew);
//        }
        for(const QString & guy : leest_audio)
        {
			/// should be "alpha", "d2_dim", "med_freq", "spectre"
            QStringList tryam = deer.entryList({guy + "*"});
			if(tryam[1].contains("med_freq"))
            {
                std::swap(tryam[1], tryam[2]);
            }
            for(const QString & fileName : tryam)
            {
                QFile fil(deer.absolutePath() + slash + fileName);
                fil.open(QIODevice::ReadOnly);
                auto contents = fil.readAll();

                outStr.write(contents);
//                ifstream inStr;
//                inStr.open(deer.absolutePath() + slash + fileName);


            }
            outStr.write("\r\n");


        }
        outStr.close();
        deer.cdUp();
//        exit(0);
    }
    exit(3);
#endif


#if 0
    /// remake markers in edf file - Balaev my experiment
    setEdfFile("/media/michael/Files/Data/BVX/BVX_rr_f3.5-40.edf");
    readData();
    edfFile & fil = globalEdf;
    bool flag = 0;
    int
    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        if(fil[fil.getMarkChan()][i] == 254 && !flag)
        {
            flag = 1;
            continue;
        }
        if(fil[fil.getMarkChan()][i] == 254 && flag)
        {
            fil.setData(fil.getMarkChan(), i, 255);
            flag = 0;
        }
    }
    fil.writeEdfFile("/media/michael/Files/Data/BVX/BVX_new.edf");
    exit(0);
#endif


#if 0
    /// Ossadtchi
//    setEdfFile("/media/Files/Data/Ossadtchi/lisa2/lisa2.edf");
    setEdfFile("/media/Files/Data/Ossadtchi/alex1/alex1.edf");
	def::freq = 100;
    def::ns = 32;
#if 0
	// reduce channels in Reals
    def::ns = 32;
    const set<int, std::less<int>> exclude{3, 4, 14};
    QString helpString;
    for(int i = 0; i < def::ns; ++i)
    {
        if(std::find(exclude.begin(), exclude.end(), i) != exclude.end()) continue;
        helpString += QString::number(i + 1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);
    reduceChannelsSlot();
#endif
//    return;



    def::ns = 29;

    std::vector<pair<int, double>> pew;
    for(int i : {17, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20})
    {
        countSpectraSimple(1024, i);

        Net * net = new Net();

        net->autoClassification(def::dir->absolutePath()
                                + slash + "SpectraSmooth"
								+ slash + "winds");

        pew.push_back(make_pair(i, net->getAverageAccuracy()));
        delete ann;

        break;
    }
    for(auto out : pew)
    {
        cout << out.first << "\t" << out.second << endl;
    }
    exit(0);
    return;
#endif



#if 0
    /// cut central 9 channels

    QPixmap pic;
    QDir tmp("/media/michael/Files/IHNA/Pew");
    QStringList lii = tmp.entryList(QDir::Files);
    for(QString picPath : lii)
    {
        pic = QPixmap(tmp.absolutePath() + slash + picPath);
        QPixmap cut = pic.copy(QRect(330, 350, 900, 900));
        QString out = tmp.absolutePath() + slash + picPath;
        out.replace(".jpg", "_cut.jpg");
        cut.save(out, 0, 100);
    }


    exit(0);

#endif



#if 0
    /// different filtering check
	ui->highFreqFilterDoubleSpinBox->setValue(40.);
	double highFreq = ui->highFreqFilterDoubleSpinBox->value();
	for(double lowFreq = 2.0; lowFreq < 3.5; lowFreq += 0.05)
    {
        setEdfFile(def::dataFolder + "/AAU_train_rr.edf");
		ui->lowFreqFilterDoubleSpinBox->setValue(lowFreq);
        refilterDataSlot();
        setEdfFile(def::dataFolder
                   + "/AAU_train_rr_f"
				   + QString::number(lowFreq) + '-' + QString::number(highFreq)
                   + ".edf");
        sliceAll();
    }
	drawReals();
    exit(0);
#endif




#if 0
    /// uncode matlab color scale
    QImage img;
    img.load("/media/Files/Data/matlab.png");
    QRgb color1 = img.pixel(img.width()/2, img.height() - 1);
    QRgb color2;

    QPixmap pic(img.height(), img.height() * 0.7);
    pic.fill();
    QPainter pnt;
    pnt.begin(&pic);


    for(int i = 2; i < img.height(); ++i)
    {
        color2 = img.pixel(img.width()/2, img.height() - i);

        pnt.setPen("red");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qRed(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qRed(color2) / 255.));

        pnt.setPen("green");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qGreen(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qGreen(color2) / 255.));

        pnt.setPen("blue");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qBlue(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qBlue(color2) / 255.));

        pnt.setPen(QColor(color1));
        pnt.setBrush(QBrush(QColor(color1)));
        pnt.drawRect(i-1, 0,
                     1, 0.07 * pic.height());
        color1 = color2;
    }
    pnt.end();
    pic.save("/media/Files/Data/matlabOut.jpg", 0, 100);

    exit(0);
#endif


#if 0
    /// read ica data from matlab
    const QString init = "/media/Files/Data/Mati/ICAstudy/";
    const QString pth = "/media/Files/Data/Mati/ICAstudy/Help/";
    QString helpString;
    def::dir->cd(pth);
    QStringList dataFiles = def::dir->entryList(QStringList("*_ica.txt"));

    mat dataMat;
    dataMat.resize(20);


    int newDataLen = 0;

    for(QString &oneFile : dataFiles)
    {

        for(int i = 0; i < 20; ++i)
        {
            dataMat[i].resize(60 * 60 * 250); // 1 hour
        }

        helpString = pth + oneFile;
        readPlainData(helpString,
                      dataMat,
                      newDataLen);
        cout << newDataLen << endl;

        for(int i = 0; i < 20; ++i)
        {
            dataMat[i].resize(newDataLen); // 1 hour
        }

        // add markers
        helpString = oneFile;
        helpString.replace("_ica.txt", ".edf");
        helpString = init + helpString;
        cout << "initFile = " << helpString << endl;

        globalEdf.readEdfFile(helpString);
        dataMat[19] = globalEdf.getData()[globalEdf.getMarkChan()];

        helpString = oneFile;
        helpString.replace(".txt", ".edf");
        helpString = init + helpString;
        cout << "outFile = " << helpString << endl;
        globalEdf.writeOtherData(dataMat, helpString);
    }

    exit(0);
#endif



#if 0
    // concat all mati sessions
    def::dir->cd("/media/Files/Data/Mati");
    QStringList dirLst = def::dir->entryList(QStringList("???"), QDir::Dirs|QDir::NoDotAndDotDot);
    for(QString & guy : dirLst)
    {
        def::dir->cd(guy);
        def::dir->cd("auxEdfs");

        QString helpString = def::dir->absolutePath() + slash + guy + "_0.edf";
        if(!QFile::exists(helpString))
        {
            def::dir->cdUp();
            def::dir->cdUp();
            continue;
        }
        edfFile initFile;
        initFile.readEdfFile(helpString);
        helpString.replace("_0.edf", "_1.edf");
        initFile.concatFile(helpString);
        helpString.replace("_1.edf", "_2.edf");
        initFile.concatFile(helpString);

        def::dir->cdUp();
        QString helpString2 = def::dir->absolutePath() + slash + guy + "_full.edf";
        initFile.writeEdfFile(helpString2);
        def::dir->cdUp();
    }
    exit(0);
#endif

#if 0
    //MATI
    if(1)
    {
        concatenateEDFs("/media/Files/IHNA/Data/MATI/archive/NOS_1.EDF",
                        "/media/Files/IHNA/Data/MATI/archive/NOS_2.EDF",
                        "/media/Files/IHNA/Data/MATI/archive/NOS.EDF");
        exit(0);

//        QString helpString1 = "/media/Files/Data/Mati/PYV/PY_3.edf";
//        cout << fileNameOf(helpString1) << endl;


//        ui->matiCheckBox->setChecked(true);
//        ui->sliceCheckBox->setChecked(true);
//        ui->sliceWithMarkersCheckBox->setChecked(true);
//        ui->reduceChannelsCheckBox->setChecked(true);
//        ui->reduceChannelsComboBox->setCurrentText("Mati");
//        setEdfFile("/media/Files/Data/Mati/SDA/SDA_rr_f.edf");
        readData();
        exit(0);
//        ns = 22;
//        cutShow();
//        drawReals();
//        sliceAll();
//        ns = 19;
//        ui->reduceChannelsLineEdit->setText("1 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 28");
//        constructEDFSlot();
        return;

        QDir * tDir = new QDir();
        tdef::dir->cd("/media/Files/Data/Mati");
        QStringList nameFilters;
        nameFilters << "*.edf" << "*.EDF";
        QString fileName;
        QString helpString;

        QStringList lst = tdef::dir->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            tdef::dir->cd(lst[i]);

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + ".EDF";
            setEdfFile(helpString);
            rereferenceDataSlot();

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + "_rr.edf";
            setEdfFile(helpString);
            refilterDataSlot();

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + "_rr_f.edf";
            setEdfFile(helpString);
            sliceAll();
			drawReals();
            tdef::dir->cdUp();

        }
        exit(0);
    }

    //test matiMarkers functions
    if(0)
    {
        double doub1 = 0b0000010110000000;
        double doub2 = 0b0000000010000000;

        double & firstMarker = doub1;
        double & lastMarker  = doub2;

        matiFixMarker(firstMarker); //fix the start marker for this small edf file
        matiFixMarker(lastMarker);  //fix the last  marker for this small edf file
        if(matiCountBit(firstMarker, 10) != matiCountBit(lastMarker, 10)) //if not one of them is the end of some session
        {
            lastMarker = firstMarker
				+ pow(2, 10) * (matiCountBit(firstMarker, 10) ? -1 : 1); //adjust the last marker
        }
        matiPrintMarker(lastMarker, "last");
        matiPrintMarker(doub2, "newData");
        exit(0);
    }

#endif
}
