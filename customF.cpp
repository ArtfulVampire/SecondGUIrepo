#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

void MainWindow::customFunc()
{
    ui->matiCheckBox->setChecked(false);
    ui->realButton->setChecked(true);

    /// for standalone eyesClean
//    ui->windowLengthSpinBox->setValue(7);
//    ui->justSliceButton->setChecked(true);


//    edfFile fil;
//    fil.readEdfFile("/media/michael/Files/Data/Galya/Ivanova/Ivanova_Anna_Exam_1.edf");
//    matrix dat;
//    dat = fil.getData();
//    dat.resizeCols(2000);
//    drawEeg(dat, dat.rows(), dat.cols(), def::freq, "/media/michael/Files/Data/Galya/Ivanova/Ivanova_Anna_Exam_1.jpg");


//    exit(0);
//    return;


#if 1
//    areEqualFiles("/media/michael/Files/Data/Galya/Krasnobaev2/Krasnobaev_Pavel_Exam_16.edf",
//                  "/media/michael/Files/Data/Galya/Krasnobaev/Krasnobaev_Pavel_Exam_16.edf"); exit(0);
    def::ntFlag = true; /// for Dasha's
//    def::ntFlag = false; /// encephalan
//    deleteSpaces(def::GalyaFolder + "/BerlinHenis");
    GalyaCut(def::GalyaFolder + "/BerlinHenis/smalls");
//    GalyaProcessing(def::GalyaFolder + "/BerlinHenis/smalls");
    exit(0);
#endif

#if 0
    const QString pth = "/media/Files/Data/test";

    matrix pe = makeTestData(pth + ".txt");
    drawRealisation(pth + ".txt_");

    int num;

    matrix dat;
    readPlainData(pth + ".txt", dat, num);

    matrix eigenVects;
    lineType eigenVals;

    matrix matA;

    dat.transpose();
    vector<int> ex;
    for(int i = 0; i < dat.rows(); ++i)
    {
        if(rand() % 1000 > 40)
        {
            ex.push_back(i);
        }
    }
    dat.eraseRows(ex);
    dat.transpose();

    ica(dat, matA, pow(10., -9), pow(10., -12));
    matA.invert();
    readPlainData(pth + ".txt", dat, num);
    matrix comps = matA * dat;

    writePlainData(pth + "_new.txt", comps, 2000);
    drawRealisation(pth + "_new.txt");

    (matA * pe).print();

    exit(0);
#endif



#if 0
    /// test on AAX
    const QString workPath = "/media/Files/Data/AAX";
    const QString fileName = "AAX_rr_f_new.edf";
    const int fftLen = 4096;
    const QString mode = "N";
    const int NumOfPairs = 1;

    QString helpString = workPath;
    if(!workPath.endsWith(slash()))
    {
        helpString += slash();
    }
    helpString += fileName;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    countSpectraSimple(fftLen);

    Net * ANN = new Net();
    ANN->setMode(mode);
    ANN->setSource("real");

    ANN->setAutoProcessingFlag(true);
    ANN->setNumOfPairs(NumOfPairs);

    ofstream ou;
    ou.open((workPath + "/softmaxErrCrit.txt").toStdString());

//    for(QString act : {"soft", "log"})
//    {
        ANN->setActFunc("soft");
        for(double err : {0.005, 0.004, 0.003, 0.002, 0.001})
        {
            ANN->setErrCrit(err);
            ANN->autoClassificationSimple();
            ou << err << "\t" << ANN->getAverageAccuracy() << "\t" << ANN->getKappa() << endl;
        }
//    }
    delete ANN;
    ou.close();
    exit(1);
#endif


    const QString path = "/media/Files/Data/Feedback/SuccessClass/";
    setEdfFile(path + "AAU_train.edf");
    readData();
    ui->reduceChannelsComboBox->setCurrentText("20");
    const QStringList names {"AAU", "BEA", "CAA", "SUA", "GAS"};
    for(QString name : names)
    {
#if 0
        /// test classification parameters
        for(QString suffix : {"_train", "_test"})
        {

            setEdfFile(path + name + suffix + ".edf");
            ui->windButton->setChecked(true);
            ui->windowLengthSpinBox->setValue(4.);
            ui->timeShiftSpinBox->setValue(4.);
            readData();
            cleanDirs();
            sliceAll();

            countSpectraSimple(1024, 8);

            Net * amn = new Net();
            amn->setSource("w");
            amn->setMode("k");
            amn->setNumOfPairs(15);
            amn->setFold(5);
            amn->setActFunc("s");


            ofstream of;
            of.open((path + "softMaxTest.txt").toStdString(),
                    ios_base::app);
            of << (name + suffix).toStdString() << '\n';
            for(double errcrit : {0.1, 0.075, 0.05, 0.025})//, 0.01, 0.005})
            {
                for(double lrate : {0.1, 0.075, 0.05, 0.025})//, 0.01, 0.005})
                {
                    amn->setErrCrit(errcrit);
                    amn->setLrate(lrate);
                    amn->autoClassificationSimple();
                    of << errcrit << '\t'
                       << lrate << '\t'
                       << amn->getAverageAccuracy() << '\t'
                       << amn->getKappa() << endl;
                }
            }
            of.close();
            delete amn;
        }
        continue;

#endif

#if 0
        /// inner
//        for(QString suffix : {"_train", "_test", "_train_ica", "_test_ica"})
        for(QString suffix : {"_train"})
        {
            setEdfFile(path + name + suffix + ".edf");
            cleanDirs();
//            ui->windButton->setChecked(true);
            ui->realButton->setChecked(true);
            sliceAll();
//            countSpectraSimple(1024, 8);
            countSpectraSimple(4096, 15);

            Net * annA = new Net();
            annA->setMode("N");
            annA->setSource("real");
            annA->autoClassificationSimple();
            delete annA;
        }
        continue;
#endif

#if 0
        /// ICA
        /// make test by train maps
        /// deal with matrixA
        matrix matA;
        readICAMatrix(path + "Help"
                      + slash() + "ica"
                      + slash() + name + "_train_maps.txt",
                      matA);
        matA.invert();

        /// convert test file with that matrix
        edfFile fil2;
        fil2.readEdfFile(path + name + "_test.edf");
        matrix newData;

        matrixProduct(matA,
                      fil2.getData(),
                      newData,
                      def::nsWOM()); // w/o markers from globalEdf data

        newData.push_back(fil2.getData()[fil2.getMarkChan()]); //copy markers

        fil2.writeOtherData(newData,
                            path + name + "_test_ica.edf");
#endif

#if 0
        /// reduce ICA
        QString chanString;
        if(name == "AAU")
        {
            chanString = "1 2 3 5 6 9 10 12 14 15 16 18 19 20";
        }
        else if(name == "BEA")
        {
            chanString = "6 15 16 17 18 20";
        }
        else if(name == "CAA")
        {
            chanString = "2 8 11 18 20";
        }
        else if(name == "GAS")
        {
            chanString = "2 5 7 10 11 12 20";
        }
        else if(name == "SUA")
        {
            chanString = "2 3 14 15 16 20";
        }

        ui->reduceChannelsLineEdit->setText(chanString);
        vector<int> chanList;
        makeChanList(chanList);

        edfFile fil;
        fil.readEdfFile(path + name + "_train_ica.edf");
        fil.reduceChannels(chanList);
        fil.writeEdfFile(path + name + "_train_ica_rdc.edf");

        /// deal with matrixA
        matrix matA;
        readICAMatrix(path + "Help"
                      + slash() + "ica"
                      + slash() + name + "_train_maps.txt",
                      matA);
        matA.invert();
//        matA.print();
        matrix matNew;
        for(int i = 0; i < chanList.size() - 1; ++i)
        {
            matNew.push_back(matA[ chanList[i] ]);
        }

        /// convert test file with that matrix
        edfFile fil2;
        fil2.readEdfFile(path + name + "_test.edf");
        matrix newData;

        matrixProduct(matNew,
                      fil2.getData(),
                      newData,
                      def::nsWOM()); // w/o markers from globalEdf data

        newData.push_back(fil2.getData()[fil2.getMarkChan()]); //copy markers

        fil2.setChannels(fil.getChannels());

        fil2.writeOtherData(newData,
                            path + name + "_test_ica_rdc.edf");


//        exit(0);
#endif

#if 0
        /// draw spectra
        def::drawNorm = -1.;
        def::drawNormTyp = all;
        for(QString suffix : {"_train", "_test"})
        {
            setEdfFile(path + name + suffix + "_ica_rdc.edf");
//            drawMapsICA();
            cleanDirs();
            sliceAll();
            countSpectraSimple(4096, 15);
//            drawMapsOnSpectra();
        }
//        exit(0);
#endif



#if 0
        /// successive
        const QString suffix = ""; /// empty, _ica, _ica_ord

#if 1
        setEdfFile(path + name + "_train" + suffix + ".edf");
        ui->timeShiftSpinBox->setValue(2.);
        ui->windowLengthSpinBox->setValue(4.);
        ui->windButton->setChecked(true); // sliceWindFromReal
        cleanDirs(); sliceAll();

        // initially reduce number of windows
        QStringList windowsList;
        // delete first three windows from each realisation
        windowsList = QDir(path + "windows/fromreal").entryList({"*_train*.00",
                                                                 "*_train*.01",
                                                                 "*_train*.02"},
                                                                QDir::Files);

        for(const QString & name : windowsList)
        {
            QFile::remove(path + "windows/fromreal/" + name);
        }

        // leave last 600 (some will fall out due to zeros)
        makeFullFileList(path + "windows/fromreal", windowsList, {"_train"});
        for(int i = 0; i < windowsList.length() - 600; ++i) /// constant
        {
            QFile::remove(path + "windows/fromreal/" + windowsList[i]);
        }
#endif

#if 0
        // delete badFiles from saved file
//        ifstream badFiles((path + "badFiles-12_400_3.txt").toStdString());
        ifstream badFiles((path + "badFiles_3sec.txt").toStdString());
        string nam;
        while(!badFiles.eof())
        {
            badFiles >> nam;
            const QString toRem = path + "windows/fromreal/" + QString(nam.c_str());
//            cout  << toRem << endl;
            QFile::remove(toRem);
        }
        badFiles.close();
        exit(0);
#endif
        countSpectraSimple(1024, 8);

        Net * ann = new Net();
        ann->setSource("w");
        ann->setActFunc("s");
        ann->setMode("N");

#if 1
        // N-fold cleaning
        cout << "cleaning" << endl;
        ann->loadData(path + "SpectraSmooth/windows");

        ann->setTallCleanFlag(true); /// with deleting the bad-classified

//        // k-fold
//        ann->setMode("k");
//        ann->setNumOfPairs(1);
//        ann->setFold(20);
//        ann->autoClassificationSimple(); /// assume only train files in the spectraDir
        // N-fold
        for(int i = 0; i < 3; ++i)
        {
            ann->autoClassificationSimple(); /// assume only train files in the spectraDir
        }

        ann->setTallCleanFlag(false);
#endif

        setEdfFile(path + name + "_test" + suffix + ".edf");
        ui->windowLengthSpinBox->setValue(4.);
        ui->timeShiftSpinBox->setValue(2.); /// really should be 0.5
        ui->windButton->setChecked(true);
        /// DON'T CLEAR, TRAIN WINDOWS TAKEN BY SUCCESSIVE
        sliceAll(); countSpectraSimple(1024, 8);

        ann->setMode("t"); // train-test

        /// current best set (logistic)
        suc::numGoodNewLimit = 6;
        suc::learnSetStay = 100;
        suc::decayRate = 0.01;

        ann->setTallCleanFlag(false);

        cout << "successive" << endl;

//        ann->setErrCrit(0.05);
//        ann->setLrate(0.05);

        ann->successiveProcessing();

        ofstream outFil;
        outFil.open((path + "successiveResults_4sec_fin.txt").toStdString(), ios_base::app);
        outFil << def::ExpName << endl;
        outFil << "learnSet = " << suc::learnSetStay << '\t';
        outFil << "numGood = " << suc::numGoodNewLimit << '\t';
        outFil << "decay = " << suc::decayRate << '\t';
        outFil << ann->getAverageAccuracy() << '\t';
        outFil << ann->getKappa() << endl;
        outFil.close();

        delete ann;

#if 0
        ofstream outFil;
        outFil.open((path + "successiveResults_4sec.txt").toStdString(), ios_base::app);
        outFil << def::ExpName << endl;
        for(int lss : {100})
        {
            for(int numG : {4, 6, 8, 10, 12})
            {
                for(double dR : {0.03, 0.02, 0.01, 0.005, 0.00}) // 1.00, 0.5
                {
                    suc::numGoodNewLimit = numG;
                    suc::decayRate = dR;
                    suc::learnSetStay = lss;

                    ann->successiveProcessing();
                    outFil << "learnSet = " << lss << '\t';
                    outFil << "numGood = " << numG << '\t';
                    outFil << "decay = " << dR << '\t';
                    outFil << ann->getAverageAccuracy() << '\t';
                    outFil << ann->getKappa() << endl;
                }
            }
        }
        outFil.close();

        delete ann;
#endif

#endif
    }
    exit(0);









#if 0
    /// Ossadtchi
//    setEdfFile("/media/Files/Data/Ossadtchi/lisa2/lisa2.edf");
    setEdfFile("/media/Files/Data/Ossadtchi/alex1/alex1.edf");
    def::freq = 100;
    def::ns = 32;
#if 0
    // reduce channels in realisations
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

    vector<pair<int, double>> pew;
    for(int i : {17, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20})
    {
        countSpectraSimple(1024, i);

        Net * ann = new Net();

        ann->autoClassification(def::dir->absolutePath()
                                + slash() + "SpectraSmooth"
                                + slash() + "windows");

        pew.push_back(make_pair(i, ann->getAverageAccuracy()));
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
    /// naive artmap

    //norm
    double maxV;
    matrix pa[2];
    int num[2];
    vector<QString> names[2];
    vector<double> count[2];

    const int NetLength = def::spLength() * def::nsWOM();
    int errors = 0;

    int matchNum = -1;
    double matchScore = 0.;
    double score = 0.;

    for(int t = 0; t < 50; ++t)
    {
        makePaStatic("/media/michael/Files/Data/AAX/SpectraSmooth",
                     4, 7.);
        for(int i = 0; i < 2; ++i)
        {
            readPaFile("/media/Files/Data/AAX/PA/" + QString::number(i + 1) + ".pa",
                       pa[i],
                       num[i],
                       names[i],
                       count[i]);
        }
        maxV = fmax(pa[0].maxVal(), pa[1].maxVal());

        pa[0] /= maxV;
        pa[1] /= maxV;


        for(int i = 0; i < num[1]; ++i)
        {
            matchNum = -1;
            matchScore = 0.;
            for(int j = 0; j < num[0]; ++j)
            {
                score = 0.;
                for(int a = 0; a < NetLength; ++a)
                {
                    score += min(pa[0][j][a], pa[1][i][a]);
                    score += min(1. - pa[0][j][a], 1. - pa[1][i][a]);
                }
                if(score > matchScore)
                {
                    matchNum = j;
                    matchScore = score;
                }
            }
            //        cout << names[1][i] << "\t" << pa[1][i][NetLength + 1] * maxV << endl;
            //        cout << names[0][matchNum] << "\t" << pa[0][matchNum][NetLength + 1] * maxV << endl << endl;

            if(pa[1][i][NetLength + 1] != pa[0][matchNum][NetLength + 1]) ++errors;
        }
        cout << "epoch = " << t << "\t";
        cout << "num of errors = " << errors << endl;

    }
    cout << "errors rate = "
         << doubleRound((errors / double(num[1] * 50.)) * 100., 2) << " %" << endl;
    exit(0);
#endif


#if 0
    const QStringList names{"SIV", "BSA", "FEV", "KMX", "NVV", "PYV", "SDA", "ADA"};
    for(const QString & guy : names)
    {
        const QString fileName = guy + "_cl_f5-20_ica_ord.edf";
        setEdfFile(def::dataFolder + slash() + "GoodData" + slash() + fileName);

        cleanDirs();
        sliceAll();
        countSpectraSimple(4096);

        continue;

        // mapsPath
        QString helpString;
        helpString = fileName;
        helpString.remove("_ica");
        helpString.replace(".edf", "_maps.txt");
        helpString = def::dataFolder
                     + slash() + "GoodData"
                     + slash() + "Help"
                     + slash() + helpString;
        drawMapsICA(helpString);


        // inPath & outPath
        // inSpectre
        helpString = fileName;
        helpString.replace(".edf", "_all.jpg");
        helpString = def::dataFolder
                     + slash() + "GoodData"
                     + slash() + "Help"
                     + slash() + helpString;
        // outSpectre
        QString helpString2;
        helpString2 = fileName;
        helpString2.replace(".edf", "_all_wm.jpg");
        helpString2 = def::dataFolder
                     + slash() + "GoodData"
                     + slash() + "Help"
                      + slash() + "WM"
                     + slash() + helpString2;
        drawMapsOnSpectra(helpString,
                          helpString2,
                          def::dataFolder
                          + slash() + "GoodData"
                          + slash() + "Help"
                          + slash() + "Maps");
        for(int i = 0; i < 19; ++i)
        {
            drawCutOneChannel(helpString2, i);
        }

//        exit(9);


        // wts
        helpString = fileName;
        helpString.replace(".edf", "_wts.jpg");
        helpString = def::dataFolder
                     + slash() + "GoodData"
                     + slash() + helpString;
        // outwts
        helpString2 = fileName;
        helpString2.replace(".edf", "_wts_wm.jpg");
        helpString2 = def::dataFolder
                     + slash() + "GoodData"
                     + slash() + "Help"
                      + slash() + "WM"
                     + slash() + helpString2;
        drawMapsOnSpectra(helpString,
                          helpString2,
                          def::dataFolder
                          + slash() + "GoodData"
                          + slash() + "Help"
                          + slash() + "Maps");
        for(int i = 0; i < 19; ++i)
        {
            drawCutOneChannel(helpString2, i);
        }


//        cleanDirs();
//        sliceAll();
//        countSpectraSimple(4096);
//        fileInnerClassification(def::dataFolder + slash() + "GoodData",
//                                fileName,
//                                4096, 1);
    }
    exit(0);
#endif


#if 0
    /// cut central 9 channels

    QPixmap pic;
    QDir tmp("/media/michael/Files/IHNA/Pew");
    QStringList lii = tmp.entryList(QDir::Files);
    for(QString picPath : lii)
    {
        pic = QPixmap(tmp.absolutePath() + slash() + picPath);
        QPixmap cut = pic.copy(QRect(330, 350, 900, 900));
        QString out = tmp.absolutePath() + slash() + picPath;
        out.replace(".jpg", "_cut.jpg");
        cut.save(out, 0, 100);
    }


    exit(0);

#endif


#if 0

    ui->reduceChannelsCheckBox->setChecked(true);
    ui->reduceChannelsComboBox->setCurrentText("Mati");
    ui->matiPieceLengthSpinBox->setValue(16);
    ui->lowFreqFilterDoubleSpinBox->setValue(5.);
    ui->highFreqFilterDoubleSpinBox->setValue(20.);
    const QStringList names{"SIV", "BSA", "FEV", "KMX", "NVV", "PYV", "SDA", "ADA"};

    const QStringList fileEndingList{"_cl_f2-35_ica_ord.edf",
                                     "_cl_f3-30_ica_ord.edf",
                                     "_cl_f4-25_ica_ord.edf",
                                     "_cl_f5-20_ica_ord.edf"};



    double tempDouble = 0;
    const QString logPath = def::dataFolder + slash() + "GoodData/class_ord.txt";
    ofstream ofStr;

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(false);
    ui->cleanWindSpectraCheckBox->setChecked(false);

    const QString workDir = def::dataFolder + slash() + "GoodData";

    QString helpString;
    QString helpString2;

    ui->reduceChannelsComboBox->setCurrentText("20");

    for(const QString & ending : fileEndingList)
    {
        for(const QString & guy : names)
        {
            helpString = workDir + slash() + guy + ending;

            setEdfFile(helpString);

            cleanDirs();
            sliceAll();
            countSpectraSimple(4096);

            trivector<int> MW;
            countMannWhitney(MW);

#if 0
            // spectra
            helpString = workDir + slash() + "Help"
                         + slash() + "Ordered"
                         + slash() + guy + ending;
            helpString.replace(".edf", "_all_wm.jpg");
            helpString2 = workDir + slash() + "Help"
                          + slash() + "Ordered"
                          + slash() + guy + ending;
            helpString2.replace(".edf", "_all_wm_new.jpg");

            QFile::remove(helpString2);
            QFile::copy(helpString, helpString2);

            drawMannWitney(helpString2,
                           MW);
#endif

#if 1
            // weights
            helpString = workDir + slash() + "Help"
                         + slash() + "weights"
                         + slash() + guy + ending;
            helpString.replace(".edf", "_wm.jpg");
            helpString2 = workDir + slash() + "Help"
                          + slash() + "weights"
                          + slash() + guy + ending;
            helpString2.replace(".edf", "_wm_new.jpg");

            QFile::remove(helpString2);
            QFile::copy(helpString, helpString2);

            drawMannWitney(helpString2,
                           MW);
#endif
        }
    }

    exit(0);
#endif


#if 0
    ui->reduceChannelsCheckBox->setChecked(true);
    ui->reduceChannelsComboBox->setCurrentText("Mati");
    ui->matiPieceLengthSpinBox->setValue(16);
    ui->lowFreqFilterDoubleSpinBox->setValue(5.);
    ui->highFreqFilterDoubleSpinBox->setValue(20.);
    const QStringList names{"SIV", "BSA", "FEV", "KMX", "NVV", "PYV", "SDA", "ADA"};

//    const QStringList fileEndingList{"_cl_f2-35_ica_ord.edf",
//                                     "_cl_f3-30_ica_ord.edf",
//                                     "_cl_f4-25_ica_ord.edf",
//                                     "_cl_f5-20_ica_ord.edf"};
    const QStringList fileEndingList{"_cl_f5-20_ica_ord.edf"};


    ui->reduceChannelsComboBox->setCurrentText("20");

    double tempDouble = 0;
    const QString logPath = def::dataFolder + slash() + "GoodData/class_ord.txt";
    ofstream ofStr;

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(false);
    ui->cleanWindSpectraCheckBox->setChecked(false);

    const QString workDir = def::dataFolder + slash() + "GoodData";

    QString helpString;

    ui->reduceChannelsComboBox->setCurrentText("20");



    for(const int wndLen : {16}) // what with 4-len windows?
    {
        ui->matiPieceLengthSpinBox->setValue(wndLen);
        for(const QString & ending : fileEndingList)
        {
            for(const QString & guy : names)
//            for(const QString & guy : {"FEV"})
            {
                helpString = workDir + slash() + guy + ending;

                setEdfFile(helpString);


//                cleanDirs();
//                sliceAll();
//                countSpectraSimple(fftL(wndLen * 250)); //continue;

//                helpString = workDir + slash() + "Help"
//                             + slash() + guy + ending;
//                helpString.replace(".edf", "_all.jpg");

//                QString wmPath = workDir + slash() + "Help"
//                                 + slash() + "Ordered"
//                                 + slash() + guy + ending;
//                wmPath.replace(".edf", "_all_wm.jpg");

//                drawMapsOnSpectra(helpString,
//                                  wmPath,
//                                  workDir + slash() + "Help" + slash() + "Maps");

//                continue;


//                countSpectraSimple(4096); continue;


//                cleanDirs();
//                sliceAll();

                ofStr.open(logPath.toStdString(), ios_base::app);

                tempDouble = fileInnerClassification(workDir,
                                                     guy + ending,
                                                     fftL(wndLen * 250), 1);

#if 0
                // maps on spectra
                helpString = workDir + slash() +  "Help"
                             + slash() + guy + ending;
                helpString.replace(".edf", "_all.jpg", Qt::CaseInsensitive);

                QString newWtsPath = workDir
                                     + slash() + "Help"
                                     + slash() + "WM"
                                     + slash() + guy + ending;
                newWtsPath.replace(".edf", "_wm_1.jpg", Qt::CaseInsensitive);

////                cout << newWtsPath << endl;

                drawMapsOnSpectra(helpString,
                                  newWtsPath,
                                  workDir + slash() + "Help" + slash() + "Maps");
#endif


#if 0
                // maps on weights
                helpString = workDir + slash() + guy + ending;
                helpString.replace(".edf", ".jpg", Qt::CaseInsensitive);

                QString newWtsPath = workDir
                                     + slash() + "Help"
                                     + slash() + "weights"
                                     + slash() + guy + ending;
                newWtsPath.replace(".edf", "_wm_1.jpg", Qt::CaseInsensitive);

////                cout << newWtsPath << endl;

                drawMapsOnSpectra(helpString,
                                  newWtsPath,
                                  workDir + slash() + "Help" + slash() + "Maps");
#endif



                ofStr << doubleRound(tempDouble, 1) << '\t'
                      << guy + ending + " " + QString::number(wndLen) << endl;

                ofStr.close();
                break;
            }
        }
    }
    exit(0);

#endif

#if 0
    ui->reduceChannelsCheckBox->setChecked(true);
    ui->reduceChannelsComboBox->setCurrentText("Mati");
    ui->matiPieceLengthSpinBox->setValue(16);
    ui->lowFreqFilterDoubleSpinBox->setValue(5.);
    ui->highFreqFilterDoubleSpinBox->setValue(20.);
    const QStringList names{"ADA", "BSA", "FEV", "KMX", "NVV", "PYV", "SDA", "SIV"};
    const QStringList fileEndingList{"_cl_f2-35_ica_ord.edf",
                                     "_cl_f3-30_ica_ord.edf",
                                     "_cl_f4-25_ica_ord.edf",
                                     "_cl_f5-20_ica_ord.edf"};

    ui->reduceChannelsComboBox->setCurrentText("20");

    double tempDouble = 0;
    const QString logPath = def::dataFolder + slash() + "GoodData/class.txt";
    ofstream ofStr;

#if 0
    // classify 16 sec
    ui->matiPieceLengthSpinBox->setValue(16);
    ofStr.open(logPath.toStdString(), ios_base::app);
    for(const QString & guy : names)
    {

        tempDouble = fileInnerClassification(def::dataFolder + slash() + "GoodData",
                                             guy + fileEnding,
                                             "16sec19ch");
        ofStr << tempDouble << '\t' << guy + fileEnding +  " 16 sec" << endl;
    }
    ofStr.close();
#endif

#if 0
    // classify 8 sec
    ui->matiPieceLengthSpinBox->setValue(8);
    ofStr.open(logPath.toStdString(), ios_base::app);
    for(const QString & guy : names)
    {

        tempDouble = fileInnerClassification(def::dataFolder + slash() + "GoodData",
                                             guy + fileEnding,
                                             "8sec19ch");
        ofStr << tempDouble << '\t' << guy + fileEnding +  " 8 sec" << endl;
    }
    ofStr.close();
#endif


    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(false);
    ui->cleanWindSpectraCheckBox->setChecked(false);

    const QString workDir = def::dataFolder + slash() + "GoodData";

    QString helpString;
    QString tmp;
    ui->reduceChannelsComboBox->setCurrentText("20");

    for(const int low : {2, 3, 4, 5})
    {
        const int high = 35 - 5 * (low - 2);
        for(const int wndLen : {16, 8})
        {
            ui->matiPieceLengthSpinBox->setValue(wndLen);
            tmp = QString::number(wndLen) + "sec19ch";


            for(const QString & guy : names)
            {
#if 0
                helpString = workDir + slash() + guy
                             + "_cl_f" + QString::number(low)
                             + "-" + QString::number(high)
                             + "_ica.edf";

                const QString initEdf = workDir + slash() + guy + "_cl_ica.edf";
                ICsSequence(initEdf, helpString);
//                exit(0);
//                continue;
#endif

                helpString = workDir + slash() + guy
                             + "_cl_f" + QString::number(low)
                             + "-" + QString::number(high)
                             + "_ica_ord.edf";

                setEdfFile(helpString);

                ui->reduceChannelsComboBox->setCurrentText("20");

                const QString inSpectraPath = workDir + slash() + "Help"
                                              + slash() + guy
                                              + "_cl_f" + QString::number(low)
                                              + "-" + QString::number(high)
                                              + "_ica_ord_all.jpg";
                const QString outSpectraPath = workDir + slash() + "Help"
                                               + slash() + "Ordered"
                                               + slash() + guy
                                               + "_cl_f" + QString::number(low)
                                               + "-" + QString::number(high)
                                               + "_ica_ord_all_wm.jpg";

                ui->matiPieceLengthSpinBox->setValue(16);
                cleanDirs();
                sliceAll();
                countSpectraSimple(pow(2, ceil(log2(ui->matiPieceLengthSpinBox->value()
                                                    * def::freq))));

                tmp = def::ExpName;
                tmp.remove("_ica");
                drawMapsICA(workDir
                            + slash() + "Help"
                            + slash() + tmp + "_maps.txt",
                            workDir
                            + slash() + "Help"
                            + slash() + "Maps");

                drawMapsOnSpectra(inSpectraPath,
                                  outSpectraPath,
                                  workDir
                                  + slash() + "Help"
                                  + slash() + "Maps");





//                ofStr.open(logPath.toStdString(),
//                           ios_base::app);

//                tempDouble = fileInnerClassification(def::dataFolder + slash() + "GoodData",
//                                                     helpString,
//                                                     tmp);
//                ofStr << tempDouble << '\t'
//                      << getFileName(helpString, false) << " "
//                      << wndLen << " sec" << endl;
//                ofStr.close();


                //            setEdfFile(helpString);
                //            ICA();
            }
        }
    }
#endif





#if 0
    /// Galya processing
    QDir tempDir;
    const QString initDir = "/media/Files/Data/Galya/";
    const QStringList addDir = { "MoreFiles",
                                 "ChaosKids",
                                 "TBI/healthy adults",
                                 "TBI/moderate TBI",
                                 "TBI/severe TBI",
                                 "FromXenon",
                                 "Ischemia",
                                 "part2",
                                 "New",
                                 "Norm_children_equalize",
                                 "autists_all"};
    for(const QString &guy : addDir)
    {
        GalyaProcessing(initDir + guy);
        tempDir.cd(initDir + guy + "/out");
        tempDir.mkdir("newResults");
        const QStringList newFiles = tempDir.entryList(QStringList{"*spectre.txt", "*alpha.txt"});
        for(const QString & fileName : newFiles)
        {
            QFile::copy(tempDir.absolutePath() + slash() + fileName,
                        tempDir.absolutePath() + slash() + "newResults" + slash() + fileName);
        }
        break;
    }
    exit(0);
#endif

#if 0
    /// draw maps on spectra
    const QStringList names{"ADA", "BSA", "FEV", "KMX", "NVV", "PYV", "SDA", "SIV"};
    const QString path = "/media/Files/Data/Mati/GoodData";
    const QString hlp = "/media/Files/Data/Mati/GoodData/Help";
    const QString out = "/media/Files/Data/Mati/GoodData/Help/Maps";
    QString helpString;
    QString nam;
    def::dir->cd(path);
    QStringList dataFiles = def::dir->entryList(QStringList("*_ica.edf"));

    ui->matiCheckBox->setChecked(true);

    for(const QString & guy : names)
    {
        helpString = path + slash() + guy + "_cl_ica.edf";
        setEdfFile(helpString);

//        helpString = hlp + slash() + guy + "_cl_maps.txt";
//        drawMapsICA(helpString,
//                    out,
//                    guy);

        ui->cleanRealisationsCheckBox->setChecked(true);
        ui->cleanRealsSpectraCheckBox->setChecked(true);
        ui->cleanWindowsCheckBox->setChecked(false);
        ui->cleanWindSpectraCheckBox->setChecked(false);
        cleanDirs();

        sliceAll();
        Spectre * sp = new Spectre();
        sp->countSpectra();
        sp->compare();
        sp->compare();
        sp->compare();
        sp->compare();
        sp->psaSlot();
        sp->close();
        delete sp;

        helpString = hlp + slash() + guy + "_cl_ica_all.jpg";
        drawMapsOnSpectra(helpString,
                          QString(hlp + slash() + guy + "_cl_ica_all_wm.jpg"),
                          hlp,
                          guy + "_cl");
//        break;


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
    drawRealisations();
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
    // test new Morle

    QPixmap pic(600, 160); // 4 seconds
    QPainter pnt;
    pic.fill();
    pnt.begin(&pic);

    double pew = pic.height() / 2;
    double inFreq = 5; // in Hz

    double sum = 0.;
    for(int i = 0; i < pic.width() - 1; ++i)
    {
        pnt.drawLine(i,
                     pic.height()/2 - morletCosNew(inFreq, pic.width() / 2, i) * pew,
                     i+1,
                     pic.height()/2 - morletCosNew(inFreq, pic.width() / 2, i+1) * pew);
        sum += pow(morletCosNew(inFreq, pic.width() / 2, i), 2);
    }

    pic.save("/media/Files/Data/MorleCos.jpg", 0, 100);
    pnt.end();
    exit(0);
#endif

#if 0
    // different filtrations low frequency
    ui->reduceChannelsCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);

    setEdfFile("/media/Files/Data/CAA/CAA_rr.edf");
    ui->reduceChannelsComboBox->setCurrentText("MyCurrent");
    sliceAll();
    drawRealisations();

    QString pew1, pew2;

    for(double j = 5; j >= 0.1; j -= 0.03)
    {
        pew2 = "/media/Files/Data/CAA/CAA_rr_f_" + QString::number(j) + ".edf";

        setEdfFile(pew2);
        ui->reduceChannelsComboBox->setCurrentText("MyCurrent");
        cleanDirs();
        sliceAll();
        drawRealisations();
    }
    exit(0);

    for(double j = 20; j <= 50; j += 0.5)
    {
        setEdfFile("/media/Files/Data/CAA/CAA_rr.edf");

        ui->highFreqFilterDoubleSpinBox->setValue(j);
        ui->lowFreqFilterDoubleSpinBox->setValue(0.);
        readData();
        refilterDataSlot();

        pew1 = "/media/Files/Data/CAA/CAA_rr_f.edf";
        pew2 = "/media/Files/Data/CAA/CAA_rr_f_" + QString::number(j) + ".edf";
        QFile::copy(pew1, pew2);

        setEdfFile(pew2);
        ui->reduceChannelsComboBox->setCurrentText("MyCurrent");
        cleanDirs();
        sliceAll();
        drawRealisations();
    }
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
    /// components ordering, based on EDFs and maps
    const QString init = "/media/Files/Data/Mati/ICAstudy/";
    const QString pth = "/media/Files/Data/Mati/ICAstudy/Help/";
    QString helpString;

    def::dir->cd(init);
    QStringList dataFiles = def::dir->entryList(QStringList("*_ica.edf"));

    matrix dataMat;
    matrix matrixA(19);

    for(QString &oneFile : dataFiles)
    {
        helpString = init + oneFile;
        setEdfFile(helpString);
        readData();

        dataMat = globalEdf.getData();

        helpString = oneFile;
        helpString.replace("_ica.edf", "_maps.txt");
        helpString = pth + helpString;

        readICAMatrix(helpString, matrixA, ns - 1);

        // ordering itself
        std::vector <std::pair <double, int>> colsNorms;

        double sumSquares = 0.;
        double sum1 = 0.;

        for(int i = 0; i < ns - 1; ++i) // w/o markers
        {
            sum1 = 0.;
            std::for_each(dataMat[i].begin(),
                          dataMat[i].end(),
                          [&sum1](double in){sum1 += in * in;});
            sumSquares += sum1;
            colsNorms.push_back(std::make_pair(sum1, i));
        }
        std::sort(colsNorms.begin(),
                  colsNorms.end(),
                  [](std::pair <double, int> i, std::pair <double, int> j)
        {return i.first > j.first;});

        for(int i = 0; i < ns - 1; ++i)
        {
//            cout << colsNorms[i].first << "\t" << colsNorms[i].second << endl;
        }

        int tempIndex;
//        std::vector <double> tempComp;
        for(int i = 0; i < ns - 2; ++i) // dont move the last
        {

            // swap matrixA cols
            matrixA.swapCols(i, colsNorms[i].second);

            // swap dataMat rows
            dataMat.swapRows(i, colsNorms[i].second);

            // swap i and colsNorms[i].second values in colsNorms
            auto it1 = std::find_if(colsNorms.begin(),
                                    colsNorms.end(),
                                    [i](std::pair <double, int> in)
            {return in.second == i;});

            // auto it2 = colsNorms.begin() + i;
            auto it2 = std::find_if(colsNorms.begin(),
                                    colsNorms.end(),
                                    [colsNorms, i](std::pair <double, int> in)
            {return in.second == colsNorms[i].second;});

            tempIndex = (*it1).second;
            (*it1).second = (*it2).second;
            (*it2).second = tempIndex;
        }





        helpString = oneFile;
        helpString.replace("_ica.edf", "_maps_after.txt");
        helpString = pth + helpString;
        writeICAMatrix(helpString, matrixA, ns - 1);

        helpString = oneFile;
        helpString.replace(".edf", "_after.edf");
        helpString = init + helpString;
        globalEdf.writeOtherData(dataMat, helpString);
    }

    exit(0);

#endif

#if 0
    // test components ordering
    QStringList names;
    names << "ADA" << "BSA" << "FEV" << "KMX" << "NVV" << "PYV" << "SDV" << "SIV";
    QStringList exts;
    exts << "_0" << "_1" << "_2" << "_full";
    QStringList leest;
    leest << "_maps_after_len" << "_maps_before_len" << "_eigenValues" << "_eigenMatrix";

    const int numIter = 5;

    def::dir->cd("/media/Files/Data/Mati/ICAstudy/");

    QString helpString;



    for(QString & guy : names)
    {
        break;
        for(QString &ext : exts)
        {
            helpString = def::dir->absolutePath()
                    + slash() + guy + ext + ".edf";
            setEdfFile(helpString);
            if(!helpString.endsWith("BSA_0.edf")) continue;

            for(int i = 0; i < numIter; ++i)
            {
                ICA();
                for(QString &typ : leest)
                {
                    const QString hlp = def::dir->absolutePath()
                            + slash() + "Help"
                            + slash() + guy + ext
                            + typ;

                    QFile::remove(hlp + "_" + QString::number(i) + ".txt");
                    QFile::copy(hlp + ".txt",
                            hlp + "_" + QString::number(i) + ".txt");
                }
            }
        }
    }



    sleep(10);
    for(QString & guy : names)
    {
        for(QString &ext : exts)
        {
            cout << endl << guy << ext << endl;
            helpString = def::dir->absolutePath()
                    + slash() + guy + ext + ".edf";

            if(!helpString.endsWith("BSA_0.edf")) continue;
            setEdfFile(helpString);

            globalEdf.readEdfFile(helpString);

            for(int i = 0; i < numIter; ++i)
            {
                const QString hlp = def::dir->absolutePath()
                        + slash() + "Help"
                        + slash() + guy + ext + "_maps_after_len_"
                        + QString::number(i) + ".txt";
                if(!QFile::exists(hlp))
                {
                    exit(0);
                }


                matrix matrixA(dim);
                matrix comps(dim, globalEdf.getDataLen());
                matrix EEG;
                globalEdf.getDataCopy(EEG.data);
                EEG.data.resize(dim);

                readICAMatrix(hlp,
                              matrixA.data,
                              dim);

                matrixA.invert();
                matrixProduct(matrixA, EEG, comps);
                vector <double> sum;
                sum.resize(dim);
                std::transform(comps.data.begin(),
                               comps.data.end(),
                               sum.begin(),
                               [](vector <double> in)
                {
                    double s = 0.;
                    std::for_each(in.begin(),
                                  in.end(),
                                  [&s](double inn)
                    {
                        s += inn * inn;
                    }
                    );
                    return s;
                }
                );
                double summ = 0.;
                std::for_each(sum.begin(),
                              sum.end(),
                              [&summ](double in)
                {
                    summ += in;
                }
                );
                for(int j = 0; j < dim; ++j)
                {
                    cout << doubleRound(sum[j] / summ, 3) << "\t";
                }
                cout << endl;
            }
        }
    }
    exit(0);
    // best set of components

    filesAddComponentsInner("/media/Files/Data/Mati/ICAstudy/",
                            "ADA_full.edf",
                            40);
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

        QString helpString = def::dir->absolutePath() + slash() + guy + "_0.edf";
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
        QString helpString2 = def::dir->absolutePath() + slash() + guy + "_full.edf";
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
//        drawRealisations();
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
            drawRealisations();
            tdef::dir->cdUp();

        }
        exit(0);
    }
//#endif
//#if 0
    if(0)
    {
        /*
        // test matrix class
        matrix<double> mat;
        mat.resize(3, 3);
        mat.fill(1.);
        cout << mat[0][1] << endl;
        mat[0][1] = 1.;
        cout << mat[0][1] << endl;
        exit(0);
        */

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
                + pow(2, 10) * ((matiCountBit(firstMarker, 10))?-1:1); //adjust the last marker
        }
        matiPrintMarker(lastMarker, "last");
        matiPrintMarker(doub2, "newData");
        exit(0);
    }
    //copy EDFs into folders
    if(0)
    {
        QDir * tDir = new QDir();
        tdef::dir->cd("/media/Files/Data/Mati");
        QStringList nameFilters;
        nameFilters << "*.edf" << "*.EDF";
        QString fileName;

        QStringList lst = tdef::dir->entryList(nameFilters);
        for(int i = 0; i < lst.length(); ++i)
        {
            fileName = lst[i];
            fileName.resize(fileName.indexOf('.'));
            cout << fileName.toStdString() << endl;

            tdef::dir->mkdir(fileName);
            QFile::copy(tdef::dir->absolutePath() + "/" + lst[i], tdef::dir->absolutePath() + "/" + fileName + "/" + lst[i]);
//            QFile::remove(tdef::dir->absolutePath() + "/" + lst[i]);
        }
        exit(0);
    }



    //AAX
    if(0)
    {
        setEdfFile("/media/Files/Data/AAX/AAX_sum.edf");
        cleanDirs();
        ui->reduceChannelsComboBox->setCurrentText("MyCurrentNoEyes");
        ui->matiCheckBox->setChecked(false);
        ui->sliceCheckBox->setChecked(true);
        ui->sliceWithMarkersCheckBox->setChecked(false);
        ui->reduceChannelsCheckBox->setChecked(false);
        sliceAll();
        Spectre * sp = new Spectre();
        sp->countSpectra();
        sp->compare();
        sp->compare();
        sp->compare();
        sp->psaSlot();
//        system("eog /media/Files/Data/AAX/Help/AAX_sum.jpg");
        exit(0);
    }


    //test a saw and sine into ica
    if(0)
    {
        QString inPath = "/media/Files/Data/AAX/AAX_rr_f_new.edf";
        QString outPath = "/media/Files/Data/AAX/TEST.edf";
        setEdfFile(inPath);
        readData();
        int lent = 200000;

        for(int i = 0; i < ndr*def::freq; ++i)
        {
            helpDouble = 2. * pi * double(i)/def::freq * 5;
            data[0][i] = 10 * sin(helpDouble - pi/2);//+ 0.17); //10.5 Hz

            if(i%50 < 25)
            {
                data[1][i] =  (i%25) - 12.5;
            }
            else
            {
                data[1][i] = -(i%25) + 12.5;
            }

//            helpDouble = 2. * pi * double(i)/def::freq * 5;
//            data[1][i] = 10 * sin(helpDouble);//+ 0.17); //10.5 Hz
        }
        cout << correlation(data[0], data[1], lent) << endl;

        ui->reduceChannelsLineEdit->setText("1 2 20");
        QList<int> lss;
        lss << 1 << 2 << 20;

        writeEdf(inPath, data, outPath, lent, lss);
        setEdfFile(outPath);
        cleanDirs();
        sliceAll();
        drawRealisations();
        ui->sliceWithMarkersCheckBox->setChecked(true);
        ui->reduceChannelsLineEdit->setText("1 2 3");
        ui->numOfIcSpinBox->setValue(2);
        ICA();
        ui->sliceWithMarkersCheckBox->setChecked(false);

        outPath.replace(".edf", "_ica.edf");
        setEdfFile(outPath);
        cleanDirs();
        sliceAll();
        ui->drawCoeffSpinBox->setValue(0.1);
        drawRealisations();
        ui->drawCoeffSpinBox->setValue(1);
        cout << correlation(data[0], data[1], lent) << endl;

        double ** mat;
        matrixCreate(&mat, 2, 2);
        double ** mat1;
        matrixCreate(&mat1, 2, lent);
        readICAMatrix("/media/Files/Data/AAX/Help/TEST_maps.txt", &mat, 2);
        cout << "    sdsd s sf sf sdfsdf " << endl;
        for(int i = 0; i < lent; ++i)
        {
            mat1[0][i] = mat[0][0]*data[0][i] + mat[0][1]*data[1][i];
            mat1[1][i] = mat[1][0]*data[0][i] + mat[1][1]*data[1][i];
            data[0][i] = mat1[0][i];
            data[1][i] = mat1[1][i];
        }
        sliceOneByOneNew(2);
        drawRealisations();

    }

    //distribution
    if(0)
    {
        setEdfFile("/media/Files/Data/AB/SMM_sum_ica.edf");
        readData();
        int num = 80; //full size
        double step = 1./double(num);
        double distr[num];
        double maxA = 0.;
        int chn = 16;
        cout << ndr * nr[chn] << endl;
        for(int i = 0; i < ndr*nr[chn]; ++i)
        {
            data[chn][i] = sin(double(i)/250. * 2. * pi * 0.01);
//            if(i < 50) cout << data[chn][i] << endl;
        }

        for(int i = 0; i < ndr*nr[chn]; ++i)
        {
            maxA = fmax(maxA, fabs(data[chn][i]));
        }
        cout << "maxA = " << maxA << endl;


        for(int i = 0; i < num; ++i)
        {
            distr[i] = 0.;
        }
        for(int i = 0; i < ndr*nr[chn]; ++i)
        {
            distr[int(ceil((data[chn][i]/maxA + 1.)/2./step - 0.5))] += 1.;
        }
        double maxD = 0.;
        for(int i = 0; i < num; ++i)
        {
            maxD = fmax(maxD, fabs(distr[i]));
            cout << distr[i] << endl;
        }
        cout << "maxD = " << maxD << endl;

        QPixmap pic(800, 600);
        QPainter * pnt = new QPainter();
        pic.fill();
        pnt->begin(&pic);

        pnt->setBrush(QBrush("black"));
        for(int i = 0; i < num; ++i)
        {
            pnt->drawRect(i*pic.width()/double(num+1), pic.height() * (1. - distr[i]/maxD), pic.width()/double(num+1) - (i==num-1), pic.height() * distr[i]/maxD);
        }
        pic.save("/media/Files/Data/distr.png", 0, 100);
        pnt->end();
        delete pnt;
        system("eog /media/Files/Data/distr.png");
        exit(0);
    }

    //function test
    if(0)
    {
        int leng = 65536;
        double * array = new double [leng];

        for(int i = 0; i < leng; ++i)
        {
            array[i] = 0.;
        }
        double freq, ampl, phas;

        for(int j = 0; j < 10; ++j)
        {
            srand(time(NULL));
            freq = 1. + (rand()%5000)/100.;
            ampl = 5. + (rand()%200)/10.;
            phas = -1. + (rand()%2000)/1000.;

            for(int i = 0; i < leng; ++i)
            {
                array[i] += ampl*sin(2 * pi * i/def::freq * freq - phas);
            }
        }

        for(int i = 0; i < leng; ++i)
        {

            ampl = (1. + rand()%10000)/10001.;
            phas = (1. + rand()%10000)/10001.;
            array[i] = 20.*sqrt(-2. * log(ampl)) * sin(2. * M_PI * phas);


            freq = 5. + (rand()%1000)/100.;
            ampl = 5. + (rand()%200)/10.;
            phas = -1. + (rand()%2000)/1000.;
            array[i] =  ampl*sin(2 * pi * i/def::freq * freq - phas);// * sin (2 * pi * i/def::freq * freq/4.);
            array[i] = 50. * (rand()%1000)/1000.;
            array[i] = i%150;
        }

        for(int i = 10; i < 100; i += 5)
        {
            cout << i << "   " << enthropy(array, leng, "", i) << endl;
        }
        //    cout << "   " << enthropy(array, leng, "", 50) << endl;

    }

    //RCP
    if(0)
    {
        def::dir->cd(def::dataFolder + "/AA");


        FILE * results = fopen(QString(def::dataFolder + "/AA/res.txt").toStdString().c_str(), "r");



        outStream.open(QString(def::dataFolder + "AA/discr.txt").toStdString().c_str());
        outStream << "name" << "\t";
        outStream << "mean" << "\t";
        outStream << "sigma" << endl;

        QString helpString;
        QStringList lst = def::dir->entryList(QStringList("*randIca.txt"), QDir::NoFilter, QDir::Name);
        int num1 = lst.length()*2;
        double * sigm = new double [num1];
        double * men = new double [num1];
        double * classf = new double [num1];
        double * drawVars = new double [num1];
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = def::dir->absolutePath() + slash() + lst[i];
            helpString.replace(".txt", ".png");
            countRCP(QString(def::dir->absolutePath() + slash() + lst[i]), helpString, &men[i], &sigm[i]);

            outStream << lst[i].toStdString() << "\t" << doubleRound(men[i], 2) << "\t" << doubleRound(sigm[i], 2) << endl;

        }
        for(int i = 0; i < lst.length(); ++i)
        {
            cout << lst[i].toStdString() << "\t";

            fscanf(results, "%*s %lf", &classf[i]);
    //        cout << classf[i] << "\t";
            drawVars[2*i + 0] = (classf[i] - men[i]) / sigm[i];
            cout << (classf[i] - men[i]) / sigm[i] << "\t";

            fscanf(results, "%*s %lf", &classf[i]);
    //        cout << classf[i] << "\t";
            drawVars[2*i + 1] = (classf[i] - men[i]) / sigm[i];
            cout << (classf[i] - men[i]) / sigm[i] << endl;
        }
        drawRCP(drawVars, num1);
        fclose(results);
    }

    if(0)
    {
        //many edf to my format
        def::dir->cd(def::dataFolder + "/GPP/edf/1");
        QString pth = "/media/michael/Files/Data/GPP/Realisations";
        QStringList lst = def::dir->entryList(QStringList("*.edf"), QDir::Files);
        QString helpString;
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = def::dir->absolutePath() + slash() + lst[i];
            setEdfFile(helpString);
            readData();
            helpString = lst[i];
            helpString.resize(helpString.indexOf('.'));
            helpString += "_1";
            outStream.open((pth + '/' + helpString).toStdString().c_str());
            outStream << "NumOfSlices " << ndr*def::freq << '\n';
            for(int j = 0; j < ndr*def::freq; ++j)
            {
                for(int k = 0; k < 19; ++k)
                {
                    outStream << data[k][j] << '\n';
                }
                outStream << "0\n";
            }
            outStream.close();

        }


        def::dir->cd(def::dataFolder + "/GPP/edf/2");
        lst = def::dir->entryList(QStringList("*.edf"), QDir::Files);
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = def::dir->absolutePath() + slash() + lst[i];
            setEdfFile(helpString);
            readData();
            helpString = lst[i];
            helpString.resize(helpString.indexOf('.'));
            helpString += "_2";
            outStream.open((pth + '/' + helpString).toStdString().c_str());
            outStream << "NumOfSlices " << ndr*def::freq << '\n';
            for(int j = 0; j < ndr*def::freq; ++j)
            {
                for(int k = 0; k < 19; ++k)
                {
                    outStream << data[k][j] << '\n';
                }

                outStream << "0\n";
            }
            outStream.close();
        }
    }


    //    ui->sliceCheckBox->setChecked(true);
    //    ui->sliceWithMarkersCheckBox->setChecked(true);
    //    ui->reduceChannelsCheckBox->setChecked(true);
    //    ui->reduceChannelsComboBox->setCurrentText("MyCurrent");

    //    ui->cleanRealisationsCheckBox->setChecked(true);
    //    ui->cleanWindowsCheckBox->setChecked(true);
    //    ui->cleanWindSpectraCheckBox->setChecked(true);



    //    QString map1 = "/media/Files/Data/AB/Help/AAX_sum_maps.txt";
    //    QString map2 = "/media/Files/Data/AB/AAX_2_maps.txt";
    //    helpString = "/media/Files/Data/AB/AAX_sum_ica.edf";
    //    helpString2 = "/media/Files/Data/AB/AAX_2_ica.edf";/
    //    ICsSequence(helpString, helpString, map1, map1);

    //    def::dir->cd("/media/Files/Data/AB/");
    //    QStringList lst11 = def::dir->entryList(QStringList("*_sum_ica.edf"));
    //    for(int i = 0; i < lst11.length(); ++i)
    //    {
    //        helpString = "/media/Files/Data/AB/" + lst11[i].left(3) + "_sum_ica.edf";
    //        helpString2 = "/media/Files/Data/AB/Help/" + lst11[i].left(3) + "_sum_maps.txt";
    //        ICsSequence(helpString, helpString, helpString2, helpString2);

    //    }

    //check variance
    if(0)
    {
        setEdfFile("/media/Files/Data/AB/VDA_1_ica.edf");
        readData();
        for(int i = 0; i < 19; ++i)
        {
            cout << variance(data[i], ndr * nr[i]) << endl;
        }
    }

    //check linear transform on perceprton input (Net.cpp: autoClassification & PaIntoMatrix)
    if(0)
    {
        setEdfFile("/media/Files/Data/AAX/AAX_sum.edf");
        Net * ann = new Net();
        ann->setAutoProcessingFlag(1);
        for(int i = 0; i < 50; ++i)
        {
            ann->autoClassificationSimple();
            outStream.open("/media/Files/Data/AAX/percRand.txt", ios_base::app);
            outStream << ann->getAverageAccuracy() << endl;
            outStream.close();
        }
    }

    //mahalanobis LDA test
    if(0)
    {
        def::dir->cd("/media/Files/Data/AB/SpectraSmooth/PCA");
        QString pcaPath = "/media/Files/Data/AB/SpectraSmooth/PCA";
        QStringList lst;
        QString helpString;
        QStringList vars;
        vars.clear();

        vars << "*_241" << "*_247" << "*_254";
        //    vars << "*_254" << "*_241";

        for(def::spLength() = 45; def::spLength() < 50; def::spLength() += 5)
        {
            ns = 1;
            int dim = def::spLength() * ns;

            double ** matrix;
            matrixCreate(&matrix, 200, dim+2); //matrix data, bias, type

            double *** covMatrix = new double ** [vars.length()];
            for(int i = 0; i < vars.length(); ++i)
            {
                matrixCreate(&(covMatrix[i]), dim, dim); //3 covariance matrices
            }

            double ** meanVect;
            matrixCreate(&meanVect, vars.length(), dim); // mean values for 3 types and dim coordinates

            double * dists = new double [vars.length()];
            double * currentVect = new double [dim];

            QString tempItem;

            int errors = 0;
            for(int h = 0; h < vars.length(); ++h)
            {
                //count covMatrices and meanVectors
                lst = def::dir->entryList(QStringList(vars[h]), QDir::Files, QDir::Name);
                makeMatrixFromFiles(pcaPath, lst, ns, def::spLength(), 1., &matrix);
                matrixMahCount(matrix, lst.length(), dim, &(covMatrix[h]), &(meanVect[h]));
            }


            double ** covMatrixWhole;
            matrixCreate(&covMatrixWhole, dim, dim); //3 covariance matrices
            for(int i = 0; i < dim; ++i)
            {
                for(int j = 0; j < dim; ++j)
                {
                    covMatrixWhole[i][j] = 0.;
                    for(int k = 0; k < vars.length(); ++k)
                    {
                        covMatrixWhole[i][j] += covMatrix[k][i][j];
                    }
                }
            }
            matrixInvert(&covMatrix[0], dim);
            matrixInvert(&covMatrix[1], dim);
            matrixInvert(&covMatrix[2], dim);
            matrixInvert(&covMatrixWhole, dim);


            for(int k = 0; k < vars.length(); ++k)
            {
                lst = def::dir->entryList(QStringList(vars[k]), QDir::Files, QDir::Name);
                for(int i = 0; i < lst.length(); ++i)
                {
                    helpString = pcaPath + slash() + lst[i];
                    readFileInLine(helpString, &currentVect, dim);

                    //count distances
                    for(int l = 0; l < vars.length(); ++l)
                    {
                        dists[l] = distanceMah(currentVect, covMatrix[l], meanVect[l], dim);
                    }

                    for(int l = 1; l < vars.length(); ++l)
                    {
                        if(dists[(k+l) % vars.length()] < dists [k])
                        {
                            ++errors;
                            break;
                        }
                    }
                }

            }


            matrixDelete(&matrix, 200);
            for(int i = 0; i < vars.length(); ++i)
            {
                matrixDelete(&(covMatrix[i]), dim);
            }
            delete covMatrix;

            matrixDelete(&meanVect, vars.length());

            delete []dists;
            delete []currentVect;


            lst = def::dir->entryList(vars, QDir::Files);
            cout << "spL = " << def::spLength() << "\tpercentage = " << errors * 100. / lst.length() << " %" << endl;
        }
        exit(0);

    }

    //test matrixInvert and Gauss - OK
    if(0)
    {
        int dim = 60;
        double ** mat1;
        matrixCreate(&mat1, dim, dim);
        double ** mat2;
        matrixCreate(&mat2, dim, dim);
        double ** mat3;
        matrixCreate(&mat3, dim, dim);
        double a;
        srand(time(NULL));
        for(int i = 0; i < dim; ++i)
        {
            for(int j = 0; j < dim; ++j)
            {
                a = rand()%50 / 50.;
                mat1[i][j] = a;
                mat1[j][i] = a;
            }
        }
        matrixInvert(mat1, dim, &mat2);
        matrixProduct(mat1, mat2, &mat3, dim, dim);

        for(int i = 0; i < dim; ++i)
        {
            for(int j = 0; j < dim; ++j)
            {
                if(abs(mat3[i][j] - (i==j)) > 1e-8)
                {
                    cout <<i << "  " << j << "   " << mat3[i][j] << endl;
                }
            }
        }
        cout << endl;

        matrixInvertGauss(mat1, dim, &mat3);
        matrixProduct(mat1, mat3, &mat2, dim, dim);

        for(int i = 0; i < dim; ++i)
        {
            for(int j = 0; j < dim; ++j)
            {
                if(abs(mat2[i][j] - (i==j)) > 1e-8)
                {
                    cout << i << "  " << j << "   " << mat2[i][j] << endl;
                }
            }
        }
        cout << endl;

        //    for(int i = 0; i < dim; ++i)
        //    {
        //        for(int j = 0; j < dim; ++j)
        //        {
        //            if(abs(mat1[i][j]) > 1e-6)
        //            {
        //                cout <<i << "  " << j << "   " << mat1[i][j] << endl;
        //            }
        //        }
        //    }
        exit(0);
    }

    //spline test - OK
    if(0)
    {
        int dim = 5;
        double * x = new double [dim];
        double * y = new double [dim];
        double * A = new double [dim-1];
        double * B = new double [dim-1];
        for(int i = 0; i < dim; ++i)
        {
            x[i] = i;
        }
        y[0] = 2.;
        y[1] = 4.;
        y[2] = 3.;
        y[3] = 0.5;
        y[4] = 1.5;
        y[5] = 7.;
        y[6] = 3.;

        //        x[0] = -1; y[0] = 0.5;
        //        x[1] = 0; y[1] = 0;
        //        x[2] = 3; y[2] = 3;


        splineCoeffCount(x, y, dim, &A, &B);
        //        exit(0);


        double norm = 100;
        QPixmap pic(dim*1000, 1500);
        pic.fill();
        QPainter * pnt = new QPainter();
        pnt->begin(&pic);
        double y1, y2;
        y2 = splineOutput(x, y, dim, A, B, x[0]);
        for(int i = 0; i < pic.width() - 1; ++i)
        {
            y1 = y2;
            y2 = splineOutput(x, y, dim, A, B, x[0] + double(i)*(x[dim-1] - x[0])/pic.width());

            //            cout << y2 << endl;
            if(i%10 == 9) cout << endl;
            pnt->drawLine(i, pic.height() - norm * y1, i+1, pic.height() - norm * y2);

        }
        for(int i = 0; i < dim; ++i)
        {
            pnt->drawEllipse(x[i]*pic.width() / (x[dim-1] - x[0]) - (i==dim-1)*5, pic.height() - y[i] * norm, 5, 5);
        }

        pic.save("/media/Files/Data/splineTest.png", 0, 100);
        pic.save("/media/Files/Data/splineTest.jpg", 0, 100);
        exit(0);
    }

    //draw maps and on spectra
    if(0)
    {
        QString helpString;
        QString helpString2;
        def::dir->cd("/media/Files/Data/AB/Help");

        //        def::dir->cd("/media/Files/Data/AB");
        QStringList list0 = def::dir->entryList(QStringList("???_1_maps.txt"), QDir::NoFilter, QDir::Name);
        //        QStringList list0 = def::dir->entryList(QStringList("???_1_ica.edf"), QDir::NoFilter, QDir::Name);
        ui->cleanRealsSpectraCheckBox->setChecked(true);
        ui->cleanRealisationsCheckBox->setChecked(true);
        for(int i = 0; i < list0.length(); ++i)
        {
            //draw separate maps only
            if(1)
            {
                helpString = def::dir->absolutePath() + slash() + list0[i];
                drawMapsICA(helpString, 19, def::dir->absolutePath(), QString(list0[i].left(3) + "-m"));
            }

            //draw average spectra
            if(0)
            {
                helpString = def::dir->absolutePath() + slash() + list0[i];
                setEdfFile(helpString);
                cleanDirs();
                sliceAll();
                countSpectraSimple(4096);
            }
            //draw maps on average spectra
            if(0)
            {
                helpString = def::dir->absolutePath() + slash() + list0[i];
                helpString = def::dir->absolutePath() + slash() + list0[i].left(3) + "_1_ica_all.png";
                helpString2 = def::dir->absolutePath() + slash() + list0[i].left(3) + "_1_ica_all_withmaps.png";
                drawMapsOnSpectra(helpString, helpString2, def::dir->absolutePath(), QString(list0[i].left(3) + "-m"));
            }
        }
        exit(0);
    }

    //splitZeros and overwrite all files !!!!
    if(0)
    {
        QStringList nameFilters;
        QString helpString;
        QString helpString2;

        def::dir->cd("/media/Files/Data/AB");
        ui->sliceWithMarkersCheckBox->setChecked(true);
        ui->splitZerosCheckBox->setChecked(true);
        nameFilters.clear();
        nameFilters << "???_1.edf" << "???_2.edf" << "???_sum.edf";


        QStringList lst11 = def::dir->entryList(nameFilters);
        for(int i = 0; i < lst11.length(); ++i)
        {
            cout << endl << lst11[i].toStdString() << "\tstart" << endl;
            helpString = def::dir->absolutePath() + slash() + lst11[i];
            setEdfFile(helpString);
            cleanDirs();
            sliceAll();
            constructEDFSlot();
            helpString = def::dir->absolutePath() + slash() + lst11[i];
            helpString2 = helpString;
            helpString2.replace(".edf", "_new.edf");
            QFile::remove(helpString);
            QFile::copy(helpString2, helpString);
            QFile::remove(helpString2);
        }
        exit(0);
    }

    //count Ica for all files
    if(0)
    {
        def::dir->cd("/media/Files/Data/AB");
        QStringList list11 = def::dir->entryList(QStringList("???_1.edf"));
        QStringList list22;
        for(int i = 0; i < list11.length(); ++i)
        {
            countICAs(def::dir->absolutePath(), list11[i], 1, 1, 0, 1);
        }
        list11 = def::dir->entryList(QStringList("???_1_ica.edf"));
        list22 = def::dir->entryList(QStringList("???_2_ica.edf"));
        if(list11.length() != list22.length())
        {
            cout << "not equal lists" << endl;
            exit(0);
        }

        exit(0);
    }

    //check memory leakage
    if(0)
    {
        def::dir->cd("/media/Files/Data/AB");
        QString helpString = def::dir->absolutePath() + "/AAX_sum.edf";
        setEdfFile(helpString);
        sliceAll();
        readData();
        Net * ANN;
        ANN = new Net();
        for(int i = 0; i < 100000; ++i)
        {
            countSpectraSimple(4096);
        }
    }

    //check 3 components stability classification - OK
    if(0)
    {
        QString fileName1 = "AAX_1_ica_by1.edf";
        QString fileName2 = "AAX_2_ica_by1.edf";
        def::dir->cd("/media/Files/Data/AB");
        QString helpString;
        makeCfgStatic(def::dir->absolutePath(), 3*247, "Reduced");
        //drop some channels
        helpString = "3 7 16 20";
        ui->reduceChannelsLineEdit->setText(helpString);

        //exclude channels from 1st file
        helpString = def::dir->absolutePath() + slash() + fileName1;
        setEdfFile(helpString);
        helpString = def::dir->absolutePath() + slash() + fileName1;
        helpString.replace(".edf", "_rdc.edf");
        reduceChannelsEDF(helpString);

        //exclude channels from 2nd file
        helpString = def::dir->absolutePath() + slash() + fileName2;
        setEdfFile(helpString);
        helpString = def::dir->absolutePath() + slash() + fileName2;
        helpString.replace(".edf", "_rdc.edf");
        reduceChannelsEDF(helpString);


        helpString = fileName1;
        helpString.replace(".edf", "_rdc.edf");
        QString helpString2 = fileName2;
        helpString2.replace(".edf", "_rdc.edf");

        ofstream outStream;
        outStream.open("/media/Files/Data/AB/stab3comp.txt", ios_base::app);
        double tempAccuracy;
        for(int i = 0; i < 100; ++i)
        {
            tempAccuracy = filesCrossClassification(def::dir->absolutePath(), helpString, helpString2, "Reduced", 50, 0.15); //0.5 generality
            outStream << doubleRound(tempAccuracy, 2) << endl;
        }
        outStream.close();
    }

    //3 components - DONE
    if(0)
    {
        QTime myTime;
        myTime.start();
        def::dir->cd("/media/Files/Data/AB");
        cleanDir(def::dir->absolutePath(), "markers", 0);
        QStringList list1 = def::dir->entryList(QStringList("???_1.edf"));
        QString helpString;
        double res;
        ofstream outStream;

        //cross classifications
        if(0)
        {
            helpString = def::dir->absolutePath() + "/classLog.txt";
            outStream.open(helpString.toStdString().c_str(), ios_base::app);
            for(int i = 0; i < list1.length(); ++i)
            {
                myTime.restart();
                cout << list1[i].left(3).toStdString() << " start" << endl;
                helpString = list1[i];
                helpString.replace("_1", "_2");
                res = filesCrossClassification(def::dir->absolutePath(), list1[i], helpString, "16sec19ch", 50, 5, false, 01000, 125);
                outStream << list1[i].left(3).toStdString() << " cross\t" << res << endl;
                res = filesCrossClassification(def::dir->absolutePath(), list1[i], helpString, "4sec19ch", 50, 3, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " wnd cross\t" << res << endl;

                helpString = list1[i];
                helpString.replace("_1", "_sum");
                res = fileInnerClassification(def::dir->absolutePath(), helpString, "16sec19ch", 50, false, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " inner\t" << res << endl << endl;
                res = fileInnerClassification(def::dir->absolutePath(), helpString, "4sec19ch", 50, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " wnd inner\t" << res << endl << endl;
                cout << list1[i].left(3).toStdString() << " finished\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
            }
            outStream.close();
            list1 = def::dir->entryList(QStringList("???_1_ica_by1.edf"));
            helpString = def::dir->absolutePath() + "/classLog.txt";
            outStream.open(helpString.toStdString().c_str(), ios_base::app);
            for(int i = 0; i < list1.length(); ++i)
            {
                myTime.restart();
                cout << list1[i].left(3).toStdString() << " start" << endl;
                helpString = list1[i];
                helpString.replace("_1", "_2");
                res = filesCrossClassification(def::dir->absolutePath(), list1[i], helpString, "16sec19ch", 50, 5, false, 01000, 125);
                outStream << list1[i].left(3).toStdString() << " ica cross\t" << res << endl;
                res = filesCrossClassification(def::dir->absolutePath(), list1[i], helpString, "4sec19ch", 50, 3, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " ica wnd cross\t" << res << endl;

                helpString = list1[i];
                helpString.replace("_1_ica_by1", "_sum_ica");
                res = fileInnerClassification(def::dir->absolutePath(), helpString, "16sec19ch", 50, false, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " ica inner\t" << res << endl;
                res = fileInnerClassification(def::dir->absolutePath(), helpString, "4sec19ch", 50, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " ica wnd inner\t" << res << endl;
                cout << list1[i].left(3).toStdString() << " ica finished\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
            }
            outStream.close();
        }
        list1 = def::dir->entryList(QStringList("???_1_ica_by1.edf"));
        //best 3 components
        if(1)
        {
            for(int i = 0; i < list1.length(); ++i)
            {
                myTime.restart();
                cout << list1[i].left(3).toStdString() << " addComps start from 3" << endl;
                helpString = list1[i];
                helpString.replace("_1", "_2");
                res = filesAddComponents(def::dir->absolutePath(), list1[i], helpString, 30, false);
                cout << list1[i].left(3).toStdString() << " addComps from 3 finished\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
            }
        }

    }
#endif
}