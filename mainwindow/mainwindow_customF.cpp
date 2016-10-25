#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace myLib;
using namespace smallLib;




void MainWindow::customFunc()
{
    ui->matiCheckBox->setChecked(false);
	ui->realsButton->setChecked(true);

//	setEdfFile("/media/Files/Data/AAX/AAX_final.edf");
//	readData();
//	Net * ann = new Net();
//	ann->loadData("/media/Files/Data/AAX/SpectraSmooth");
//	ann->setClassifier(ClassifierType::ANN);
//	ann->autoClassification();


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
//	return;

#if 0
	/// test new classifiers
	QString paath = "/media/Files/Data/Feedback/SuccessClass/";

	for(QString guy : {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"})
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


			cout << guy + suff << endl;
			net->autoClassification();



//			std::vector<std::vector<double>> allPew =  {{3., 40.},
//														{5., 60.},
//														{6., 70.},
//														{8., 90.}
//													   };
//			for(auto pewww : allPew)
//			{
//				for(double i3 : {0.00, 0.01})
//				{
//					suc::numGoodNewLimit = pewww[0];
//					suc::learnSetStay = pewww[1];
//					suc::decayRate = i3;
//					cout << suc::numGoodNewLimit << '\t';
//					cout << suc::learnSetStay << '\t';
//					cout << suc::decayRate << endl;
//					net->successiveProcessing();
//				}

//			}
//			ofstream res;
//			res.open((def::dir->absolutePath() + slash + "results.txt").toStdString(),
//					 ios_base::app);
//			res << std::endl;
//			res.close();

			delete net;
//			break; /// only suff = "_train"
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


//	return;
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
    /// Xenia cut
	QString workPath = "/media/Files/Data/Xenia/26Sep/moderate_TBI";
	/// repair dirs and files
	repair::deleteSpacesFolders(workPath);
	repair::toLatinDir(workPath, {});
	repair::toLowerDir(workPath, {});
	QStringList dirs = QDir(workPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString deer : dirs)
    {
//		cout << deer << endl;
		repair::fullRepairDir(workPath + myLib::slash + deer);
    }

	wvlt::initMtlb();
	for(QString deer : dirs) /// each guy
	{
		QString pew = workPath + slash + deer;
		autos::GalyaProcessing(pew, 19, workPath + slash + getFileName(workPath) + "_results");
		autos::GalyaWavelets(pew, 19, 250, workPath + slash + getFileName(workPath) + "_wavelet");
	}
	wvlt::termMtlb();

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
    exit(0);
#endif


#if 0
    /// EEG fMRI
    def::ntFlag = true;

//	QString guy = "Atanov";
	for(QString guy : subjects::leest_mri)
    {

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
    /// batch recursively rename files in a dir
    QString renameDir = "/media/michael/Files/Data/Galya/Norm_tactile_July";
    std::vector<std::pair<QString, QString>> renam{
    {"Ali-zade", "Ali-Zade"},
    {"ZavylovaV", "Zavyalova"},
    {"GAVRILOV", "Gavrilov"},
    {"atanov", "Atanov"},
    {"KhoKhlov", "Khokhlov"},
    {"LevandoA", "Levando"},
    {"novoselova", "Novoselova"},
    {"sushinsky", "Sushinsky"},
    {"ZavylovaV", "Zavyalova"},
};
    const QString listFile = "/media/michael/Files/Data/list.txt";
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
			/// should be "alpha", "d2_dim", "med.getFreq()", "spectre"
            QStringList tryam = deer.entryList({guy + "*"});
			if(tryam[1].contains("med.getFreq()"))
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