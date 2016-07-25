#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace myLib;
using namespace smallLib;


QStringList leest_less = {
    "Berlin-Henis",
    "Bushenkov",
    "CHemerisova",
    "Didkovskaya",
    "Grishina",
    "Ivanova",
    "Krasnobaev",
    "Melnik",
    "Paramonova",
    "Ryibalko",
    "Sarvardinova",
    "SHkarina",
    "Vasina",
//    "Zelenkov"
};
QStringList leest_more = {
    "Burmistrova",
    "Garmotko",
    "Hanenya",
    "Kalinichenko",
    "Tinyanova"
};

QStringList leest_non = {
    "tactile",
    "Dasha_GO",
    "Dasha_GZ",
    "Dasha_smell",
    "Dasha_smell_2"
};

QStringList leest_mri = {
    "Ali-Zade",
    "Atanov",
    "Balaev",
    "Gudovich",
    "Khasanov",
    "Khokhlov",
    "Levando",
    "Mezhenova",
    "Moskovtsev",
    "Muchanova",
    "Nikolaenko",
    "Novoselova",
    "Rest",
    "Sushinsky",
    "Umanskaya",
    "Zavyalova"
};
void MainWindow::customFunc()
{
    ui->matiCheckBox->setChecked(false);
    ui->realButton->setChecked(true);

    /// for standalone eyesClean
//    ui->windowLengthSpinBox->setValue(5);
//    ui->justSliceButton->setChecked(true);

    QStringList leest_audio = leest_more + leest_less;
    leest_audio.sort(Qt::CaseInsensitive); /// alphabet order

//    return;

#if 0

        QString paath = "/media/michael/Files/Data/Feedback/SuccessClass";
        setEdfFile(paath + "/AAU_train.edf");
        readData();

        /// loading UCI dataset
//        matrix uciData{};
//        std::vector<int> uciTypes{};
//        readUCIdataSet("wine", uciData, uciTypes);

        Net * net = new Net();
        net->loadData(paath + "/SpectraSmooth/windows", {"*_test*"});
//        net->setClassifier(ClassifierType::KNN);
        net->setCentering(true);
        net->setVariancing(true);
        net->setSource("w");
        net->show();
        return;

        net->autoClassification();
        exit(0);
    }
#endif




#if 0
    const QString path = "/media/Files/Data/Feedback/SuccessClass/";
    setEdfFile(path + "AAU_train.edf");
    readData();
    ui->reduceChannelsComboBox->setCurrentText("20");

//    const QStringList names {"AAU", "BEA", "CAA", "SUA", "GAS"};
    const QStringList names {"AAU"};

    bool sliceAndCount = true;
//    bool sliceAndCount = false;

    for(QString name : names)
    {
        /// successive
        setEdfFile(path + name + "_train" + ".edf");
        ui->timeShiftSpinBox->setValue(2.);
        ui->windowLengthSpinBox->setValue(4.);
        ui->windButton->setChecked(true); // sliceWindFromReal

        if(sliceAndCount)
        {
            cleanDirs();
            sliceAll();
        }


        {
            /// initially reduce number of windows
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
        }

        if(sliceAndCount)
        {
            countSpectraSimple(1024, 8);
        }

        Net * ann = new Net();
        ann->show();
        ann->loadData(def::dir->absolutePath()
                      + slash + "SpectraSmooth"
                      + slash + "windows", {"_train"}); /// only for ANN set - dataMatrix->cols()

        ann->setClassifier(ClassifierType::NBC);

        ann->setSource("w");
        ann->setMode("t"); // train-test


        setEdfFile(path + name + "_test" + ".edf");
        ui->windowLengthSpinBox->setValue(4.);
        ui->timeShiftSpinBox->setValue(2.); /// really should be 0.5
        ui->windButton->setChecked(true);
        /// DON'T CLEAR, TRAIN WINDOWS TAKEN BY SUCCESSIVE

        if(sliceAndCount)
        {
            sliceAll();
            countSpectraSimple(1024, 8);
        }

        /// current best set
        suc::numGoodNewLimit = 6;
        suc::learnSetStay = 100;
        suc::decayRate = 0.01;

        exit(0);

        ann->successiveProcessing();

        delete ann;
    }
    exit(0);
#endif


#if 0
    /// Xenia rereference + cut

    const QString pew = "/media/michael/Files/Data/Xenia/Mihalkova_rr";

    GalyaCut(pew, 18); exit(0); /// comment to reref

    ui->rereferenceDataComboBox->setCurrentText("Base");
    for(QString fil : QDir(pew).entryList({"*.edf"}))
    {
        setEdfFile(pew + slash + fil);
        rereferenceDataSlot();
    }
    exit(0);
#endif

#if 0
    /// Galya
    def::ntFlag = false;
    const QString procDir = def::GalyaFolder + slash + "Aut_tactile_July";
//    repair::dirToLatin(procDir, def::edfFilters);
//    repair::deleteSpaces(procDir, def::edfFilters);
//    repair::checkPhysBadChan(procDir);
    repair::repairPhysBadChan(procDir + slash + "bad");

    exit(0);

    GalyaProcessing(procDir, 19);
    exit(0);
#endif

#if 0
    /// test MK channel
    QString pewpewpew = "/media/michael/Files/Data/MRI/Khokhlov/Khokhlov_windows_cleaned";
    QStringList leest = QDir(pewpewpew).entryList({"*.edf"});
    for(QString str : leest)
    {
        edfFile feel;
        feel.readEdfFile(pewpewpew + slash + str);
        if(feel.getPhysMin()[feel.getNs() - 2] == feel.getPhysMax()[feel.getNs() - 2])
        {
            cout << str << endl;
        }
    }
    exit(0);
#endif


#if 0
    /// EEG fMRI
    def::ntFlag = true;


//    GalyaCut(def::mriFolder + slash + "Kabanov",
//             2);
//    exit(0);


    QString guy = "Gudovich";
//    for(QString guy : leest_mri)
    {
        QDir tmp(def::mriFolder + slash + "OUT");
        tmp.mkdir(guy);

        GalyaProcessing(def::mriFolder
                        + slash + guy
                        + slash + guy + "_windows_cleaned",
                        32,
                        def::mriFolder
                        + slash + guy
                        + slash + guy + "_windows_cleaned_out");


        /// check rightNumbers in all subjects
        makeRightNumbers(def::mriFolder
                         + slash + guy
                         + slash + guy + "_windows_cleaned_out");

        for(QString type : {"_spectre", "_alpha", "_d2_dim", "_med_freq"})
        {
            makeTableFromRows(def::mriFolder
                              + slash + guy
                              + slash + guy + "_windows_cleaned_out",
                              def::mriFolder
                              + slash + "OUT"
                              + slash + guy
                              + slash + guy + type + ".txt",
                              type);
        }


        // execute matlab & wavelet_new
        /// rename the folder in OUT to guy
        makeRightNumbers(def::mriFolder
                         + slash + "wavelet"
                         + slash + guy);
        makeTableFromRows(def::mriFolder
                          + slash + "wavelet"
                          + slash + guy,
                          def::mriFolder
                          + slash + "OUT"
                          + slash + guy
                          + slash + guy + "_wavelet.txt");

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
//        repairChannels(def::DashaFolder + slash + str,
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
    /// make current numbers to rightNumbers in files

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
                                + slash + "SpectraSmooth"
                                + slash + "windows");

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

#endif
}
