#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() :
    ui(new Ui::MainWindow)
{
    QButtonGroup * group1, *group2, *group3, *group4;
    autoProcessingFlag = false;
    ui->setupUi(this);
    setWindowTitle("Main");

    redirectCoutFlag = false;
    coutBuf = cout.rdbuf();

    setlocale(LC_NUMERIC, "C");


    ns = -1;
    spLength = -1;
    ns = def::ns;

    right = fftLimit(def::rightFreq, def::freq, def::fftLength);
    left  = fftLimit(def::leftFreq, def::freq, def::fftLength);

    spStep = def::freq/def::fftLength;
    spLength = right - left + 1;

    withMarkersFlag = 1;
    ui->sliceWithMarkersCheckBox->setChecked(true);

    staSlice = 0;
    stopFlag = 0;

    dir = new QDir();
    dir->cd(QDir::root().absolutePath());

    NumOfEdf = 0; //for EDF cut


    paint = new QPainter();


    label = new char* [maxNs];     //memory for channels' labels
    for(int i = 0; i < maxNs; ++i)
    {
        label[i] = new char [17];
    }
    nr = new int [maxNs];

    group1 = new QButtonGroup();
    group1->addButton(ui->enRadio);
    group1->addButton(ui->ntRadio);
    group2 = new QButtonGroup();
    group2->addButton(ui->windButton);
    group2->addButton(ui->realButton);
    group3 = new QButtonGroup();
    group3->addButton(ui->BayesRadioButton);
    group3->addButton(ui->HiguchiRadioButton);
    group4 = new QButtonGroup();
    group4->addButton(ui->lambdaRadioButton);
    group4->addButton(ui->r2RadioButton);

    ui->BayesRadioButton->setChecked(true);

    //wavelets
    ui->classBox->setChecked(true);
    ui->weightsBox->setChecked(true);
    ui->waveletsBox->setChecked(true);
    ui->fullBox->setChecked(true);
    ui->r2RadioButton->setChecked(true);

    ui->tempBox->setMaximum(1e7);
    ui->tempBox->setValue(2e3);

    ui->enRadio->setChecked(true);

    ui->drawCoeffSpinBox->setValue(1.0); //draw coeff
    ui->drawCoeffSpinBox->setSingleStep(0.1); //draw coeff
    ui->sliceCheckBox->setChecked(true);
    ui->eyesCleanCheckBox->setChecked(false);
//    ui->eyesCleanCheckBox->setChecked(true);   ///for windows
    ui->reduceChannelsCheckBox->setChecked(true);
    ui->reduceChannelsCheckBox->setChecked(false);
    ui->progressBar->setValue(0);
    ui->setNsLine->property("S&et");

    ui->drawDirBox->addItem("Realisations");
    ui->drawDirBox->addItem("cut");
    ui->drawDirBox->addItem("windows");
    ui->drawDirBox->addItem("windows/fromreal"); //generality

    int helpInt = 0;
    QVariant var;
    /*
    ui->reduceChannelsComboBox->addItem("old En->real-time");   //encephalan for real time
    var = QVariant("19 18 16 14 11 9 6 4 2 1 17 13 12 8 7 3 15 10 5 24");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    ui->reduceChannelsComboBox->addItem("En-19");   //encephalan w/o veget channels
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 22");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    ui->reduceChannelsComboBox->addItem("LN->En");
    var = QVariant("1 2 11 3 17 4 12 13 5 18 6 14 15 7 19 8 16 9 10");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    ui->reduceChannelsComboBox->addItem("Boris Nt->En");
    var = QVariant("1 3 7 4 5 6 8 25 14 15 16 26 27 20 21 22 28 29 31");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);
*/

    QString helpString;
    //0
    ui->reduceChannelsComboBox->addItem("MichaelBak");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 23 24 31");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //1
    ui->reduceChannelsComboBox->addItem("MichaelBakNew");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 23 24 27");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //2
    ui->reduceChannelsComboBox->addItem("MichaelBakNewNoEyes");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 27");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //3
    ui->reduceChannelsComboBox->addItem("MyCurrent");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 22 23 24");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //4
    ui->reduceChannelsComboBox->addItem("MyCurrentNoEyes");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 24");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //5
    ui->reduceChannelsComboBox->addItem("NewEnceph");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 22-23-20 24 25");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //6
    ui->reduceChannelsComboBox->addItem("NewEncephNoEyes");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 25");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //7
    ui->reduceChannelsComboBox->addItem("Mati");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 22 23 28");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //8
    ui->reduceChannelsComboBox->addItem("MatiNoEyes");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 28");
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //9
    ui->reduceChannelsComboBox->addItem("MatiAmod");
    helpString.clear();
    for(int i = 0; i < 41; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    var = QVariant(helpString);
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //10
    ui->reduceChannelsComboBox->addItem("MatiAmodNoEyes");
    helpString.clear();
    for(int i = 0; i < 41; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    helpString.remove(" 22 23");
    var = QVariant(helpString);
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //11
    ui->reduceChannelsComboBox->addItem("20");
    helpString.clear();
    for(int i = 0; i < 20; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    var = QVariant(helpString);
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //12
    ui->reduceChannelsComboBox->addItem("22");
    helpString.clear();
    for(int i = 0; i < 22; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    var = QVariant(helpString);
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    //13
    ui->reduceChannelsComboBox->addItem("22NoEyes");
    helpString.clear();
    for(int i = 0; i < 19; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    helpString += "22";
    var = QVariant(helpString);
    ui->reduceChannelsComboBox->setItemData(helpInt++, var);

    ui->reduceChannelsComboBox->setCurrentText("MyCurrentNoEyes");
    ui->reduceChannelsComboBox->setCurrentText("20");
    ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(ui->reduceChannelsComboBox->currentIndex()).toString());


    ui->timeShiftSpinBox->setMinimum(25);
    ui->timeShiftSpinBox->setMaximum(1000);
    ui->timeShiftSpinBox->setValue(250);
    ui->timeShiftSpinBox->setSingleStep(25);
    ui->windowLengthSpinBox->setMaximum(1000);
    ui->windowLengthSpinBox->setMinimum(250);
    ui->windowLengthSpinBox->setSingleStep(125);
    ui->realButton->setChecked(true);

    ui->numOfIcSpinBox->setMaximum(19); //generality
    ui->numOfIcSpinBox->setMinimum(2);
    ui->numOfIcSpinBox->setValue(19);

    ui->numComponentsSpinBox->setMaximum(19);
    ui->numComponentsSpinBox->setValue(10);

    ui->svdDoubleSpinBox->setDecimals(1);
    ui->svdDoubleSpinBox->setMaximum(15);
    ui->svdDoubleSpinBox->setMinimum(2);
    ui->svdDoubleSpinBox->setValue(4.0);
    ui->svdDoubleSpinBox->setValue(9.0);
    ui->svdDoubleSpinBox->setSingleStep(0.5);

    ui->vectwDoubleSpinBox->setDecimals(1);
    ui->vectwDoubleSpinBox->setMaximum(10.0);
    ui->vectwDoubleSpinBox->setMinimum(1.5);
    ui->vectwDoubleSpinBox->setValue(9.0);
    ui->vectwDoubleSpinBox->setSingleStep(0.5);

    ui->spocCoeffDoubleSpinBox->setMaximum(5);
    ui->spocCoeffDoubleSpinBox->setMinimum(0);
    ui->spocCoeffDoubleSpinBox->setSingleStep(0.1);
    ui->spocCoeffDoubleSpinBox->setDecimals(1);
    ui->spocCoeffDoubleSpinBox->setValue(2.2);

    ui->spocTresholdDoubleSpinBox->setMaximum(1.0);
    ui->spocTresholdDoubleSpinBox->setMinimum(0.6);
    ui->spocTresholdDoubleSpinBox->setSingleStep(0.005);
    ui->spocTresholdDoubleSpinBox->setDecimals(3);
    ui->spocTresholdDoubleSpinBox->setValue(0.95);

    ui->finishTimeBox->setMaximum(60*30.);
    ui->startTimeBox->setMaximum(ui->finishTimeBox->maximum());

    ui->BayesSpinBox->setMaximum(50);
    ui->BayesSpinBox->setMinimum(1);
    ui->BayesSpinBox->setValue(20);

    ui->cleanFromRealsCheckBox->setChecked(true);
    ui->cleanHelpCheckBox->setChecked(false);
    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(false);
    ui->cleanWindSpectraCheckBox->setChecked(true);
    ui->cleanMarkersCheckBox->setChecked(true);

    ui->highFreqFilterDoubleSpinBox->setValue(def::rightFreq);
    ui->highFreqFilterDoubleSpinBox->setSingleStep(1.0);
    ui->lowFreqFilterDoubleSpinBox->setValue(def::leftFreq);
    ui->lowFreqFilterDoubleSpinBox->setSingleStep(0.1);

    ui->rereferenceDataComboBox->addItem("A1");
    ui->rereferenceDataComboBox->addItem("A2");
    ui->rereferenceDataComboBox->addItem("Ar");
    ui->rereferenceDataComboBox->addItem("N");
    ui->rereferenceDataComboBox->setCurrentText("Ar");

    ui->matiPieceLengthSpinBox->setMaximum(32);
    ui->matiPieceLengthSpinBox->setMinimum(4);
    ui->matiPieceLengthSpinBox->setValue(7);
    ui->matiCheckBox->setChecked(true);
    ui->markerBinTimeSpinBox->setMaximum(250*60*60*2);   //2 hours
    ui->markerSecTimeDoubleSpinBox->setMaximum(60*60*2); //2 hours

    ui->roundOffsetCheckBox->setChecked(true);


    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(setEdfFileSlot()));

    QObject::connect(ui->Name, SIGNAL(returnPressed()), this, SLOT(setExpName()));

    QObject::connect(ui->cutEDF, SIGNAL(clicked()), this, SLOT(sliceAll()));

    QObject::connect(ui->matiPreprocessingPushButton, SIGNAL(clicked()), this, SLOT(matiPreprocessingSlot()));

    QObject::connect(ui->windFromRealButton, SIGNAL(clicked()), this, SLOT(sliceWindFromReal()));

    QObject::connect(ui->cut_e, SIGNAL(clicked()), this, SLOT(showCut()));

    QObject::connect(ui->makeCFG, SIGNAL(clicked()), this, SLOT(showMakeCfg()));

    QObject::connect(ui->makePA, SIGNAL(clicked()), this, SLOT(showMakePa()));

    QObject::connect(ui->netButton, SIGNAL(clicked()), this, SLOT(showNet()));

    QObject::connect(ui->countSpectra, SIGNAL(clicked()), this, SLOT(showCountSpectra()));

    QObject::connect(ui->eyesButton, SIGNAL(clicked()), this, SLOT(showEyes()));

    QObject::connect(ui->reduceChannesPushButton, SIGNAL(clicked()), this, SLOT(reduceChannelsSlot()));

    QObject::connect(ui->drawButton, SIGNAL(clicked()), this, SLOT(drawRealisations()));

    QObject::connect(ui->cleanDirsButton, SIGNAL(clicked()), this, SLOT(cleanDirs()));

    QObject::connect(ui->reduceChannelsComboBox, SIGNAL(highlighted(int)), this, SLOT(changeNsLine(int)));

    QObject::connect(ui->reduceChannelsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeNsLine(int)));

    QObject::connect(ui->setNsLine, SIGNAL(returnPressed()), this, SLOT(setNs()));

    QObject::connect(ui->waveletButton, SIGNAL(clicked()), this, SLOT(waveletCount()));

    QObject::connect(ui->finishTimeBox, SIGNAL(valueChanged(double)), this, SLOT(setBoxMax(double)));

    QObject::connect(ui->datFileButton, SIGNAL(clicked()), this, SLOT(makeDatFile()));

    QObject::connect(ui->diffSmoothPushButton, SIGNAL(clicked()), this, SLOT(diffSmooth()));

    QObject::connect(ui->diffPowPushButton, SIGNAL(clicked()), this, SLOT(diffPow()));

    QObject::connect(ui->avTimeButton, SIGNAL(clicked()), this, SLOT(avTime()));

    QObject::connect(ui->icaPushButton, SIGNAL(clicked()), this, SLOT(ICA()));

    QObject::connect(ui->constructEdfButton, SIGNAL(clicked()), this, SLOT(constructEDFSlot()));

    QObject::connect(ui->drawMapsPushButton, SIGNAL(clicked()), this, SLOT(drawMapsSlot()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    QObject::connect(ui->BayesPushButton, SIGNAL(clicked()), this, SLOT(Bayes()));

    QObject::connect(ui->makeTestDataPushButton, SIGNAL(clicked()), this, SLOT(makeTestData()));

    QObject::connect(ui->icaTestClassPushButton, SIGNAL(clicked()), this, SLOT(icaClassTest()));

    QObject::connect(ui->spocPushButton, SIGNAL(clicked()), this, SLOT(spoc()));

    QObject::connect(ui->hilbertPushButton, SIGNAL(clicked()), this, SLOT(hilbertCount()));

    QObject::connect(ui->throwICPushButton, SIGNAL(clicked()), this, SLOT(throwIC()));

    QObject::connect(ui->randomDecomposePushButton, SIGNAL(clicked()), this, SLOT(randomDecomposition()));

    QObject::connect(ui->refilterDataPushButton, SIGNAL(clicked()), this, SLOT(refilterDataSlot()));

    QObject::connect(ui->transformRealsPushButton, SIGNAL(clicked()), this, SLOT(transformReals()));

    QObject::connect(ui->sliceWithMarkersCheckBox, SIGNAL(stateChanged(int)), this, SLOT(sliceWithMarkersSlot(int)));

    QObject::connect(ui->reduceChannelsNewEDFPushButton, SIGNAL(clicked()), this, SLOT(reduceChannelsEDFSlot()));

    QObject::connect(ui->rereferenceDataPushButton, SIGNAL(clicked()), this, SLOT(rereferenceDataSlot()));

    QObject::connect(ui->markerGetPushButton, SIGNAL(clicked()), this, SLOT(markerGetSlot()));
    QObject::connect(ui->markerSetPushButton, SIGNAL(clicked()), this, SLOT(markerSetSlot()));
    QObject::connect(ui->markerBinTimeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(markerSetSecTime(int)));
//    QObject::connect(ui->markerDecimalLineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetBinValueSlot()));
    QObject::connect(ui->markerDecimalLineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetBinValueSlot()));
//    QObject::connect(ui->markerBin0LineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetDecValueSlot()));
    QObject::connect(ui->markerBin0LineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetDecValueSlot()));
//    QObject::connect(ui->markerBin1LineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetDecValueSlot()));
    QObject::connect(ui->markerBin1LineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetDecValueSlot()));
    QObject::connect(ui->markerSaveEdfPushButton, SIGNAL(clicked()), this, SLOT(markerSaveEdf()));
    QObject::connect(ui->matiCheckBox, SIGNAL(stateChanged(int)), this, SLOT(matiCheckBoxSlot(int)));

    customFunc();
}

void MainWindow::matiCheckBoxSlot(int a)
{
    globalEdf.setMatiFlag(a);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void QWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape) close();
}


void MainWindow::countSpectraSimple(int fftLen)
{
    Spectre *sp = new Spectre(dir, ns, ExpName);
    QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
    sp->setFftLength(fftLen);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    QObject::disconnect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
    sp->close();
    delete sp;
}

void MainWindow::showCountSpectra()
{
    Spectre *sp = new Spectre(dir, ns, ExpName);
    QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
    sp->show();
}

void MainWindow::showEyes()
{
    Eyes *trololo = new Eyes(dir, ns);
    trololo->setAutoProcessingFlag(false);
    trololo->show();
    QObject::connect(trololo, SIGNAL(setNsMain(int)), this, SLOT(setNsSlot(int)));
}

void MainWindow::showNet()
{
    Net * ANN = new Net(dir, ns, left, right, spStep, ExpName);
    ANN->show();
}

void MainWindow::showCut()
{
    Cut *cut_e = new Cut(dir, ns, withMarkersFlag);
    cut_e->show();
}

void MainWindow::showMakePa() //250 - frequency generality
{
    QString helpString;
    if(spStep == def::freq/4096. || spStep == def::freq/2048.)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "SpectraSmooth");
    }
    else if(spStep == def::freq/1024.)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "SpectraSmooth" + slash() + "windows");
    }
    else
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath());
    }


    MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep, QVector<int>());
    mkPa->show();
}

void MainWindow::showMakeCfg()//250 - frequency generality
{
    QString helpString;
    if(spStep == def::freq/4096.) helpString = "16sec19ch";
    else if(spStep == def::freq/1024.) helpString = "4sec19ch";
    else if(spStep == def::freq/2048.) helpString = "8sec19ch";
    else helpString = "netFile";
    cfg *config = new cfg(dir, ns, spLength, 0.10, 0.10, helpString);
    config->show();
}



void MainWindow::setEdfFileSlot()
{
    QString helpString;
    NumOfEdf = 0;
    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)this, tr("EDF to open"), def::dataFolder, tr("EDF files (*.EDF *.edf)")));
    if(helpString.isEmpty())
    {
        QMessageBox::warning((QWidget*)this, tr("Warning"), tr("no EDF file was chosen"), QMessageBox::Ok);
        return;
    }
    setEdfFile(helpString);
}

void MainWindow::setEdfFile(QString const &filePath)
{
    QString helpString;
    NumOfEdf = 0;
    helpString = filePath;


    if(!helpString.endsWith(".edf", Qt::CaseInsensitive))
    {
        helpString += ".edf";
    }
    if(!QFile::exists(helpString))
    {
        helpString = "Cannot open EDF file:\n" + helpString;
        cout << helpString << endl;
        return;
    }

    ui->filePathLineEdit->setText(QDir::toNativeSeparators(helpString));

    //set ExpName

    ExpName = getExpNameLib(filePath);
    ui->Name->setText(ExpName);

    helpString.resize(helpString.lastIndexOf(slash()));
    dir->cd(helpString);

    if(redirectCoutFlag)
    {
        //redirect cout to logfile
        if(generalLogStream.is_open())
        {
            generalLogStream.close();
        }
        helpString += QString(slash()) + "generalLog.txt";
        generalLogStream.open(helpString.toStdString().c_str(), ios_base::app);
        generalLogStream << endl << endl << endl;
        cout.rdbuf(generalLogStream.rdbuf());
    }
    else
    {
        cout.rdbuf(coutBuf);
    }


    dir->mkdir("Help");
    dir->mkdir("amod");
    dir->mkdir("auxEdfs");
    dir->mkdir("PA");
    dir->mkdir("visualisation");
    dir->mkdir("visualisation/video");
    dir->mkdir("visualisation/wavelets");

    for(int i = 0; i < 19; ++i)
    {
        helpString = "visualisation/wavelets/";
        dir->mkdir(helpString.append(QString::number(i)));
    }
    dir->mkdir("Signals");
    dir->mkdir("Signals/before");
    dir->mkdir("Signals/after");
    dir->mkdir("Signals/other");
    dir->mkdir("Signals/windows");
    dir->mkdir("Signals/windows/after");
    dir->mkdir("Signals/windows/before");
    dir->mkdir("Signals/windows/other");
    dir->mkdir("SignalsCut");
    dir->mkdir("SignalsCut/before");
    dir->mkdir("SignalsCut/after");
    dir->mkdir("SpectraImg");
    dir->mkdir("SpectraSmooth");
    dir->mkdir("SpectraSmooth/windows");
    dir->mkdir("SpectraSmooth/Bayes");
    dir->mkdir("SpectraSmooth/PCA");
    dir->mkdir("windows");
    dir->mkdir("windows/fromreal");
    dir->mkdir("Realisations");
    dir->mkdir("Realisations/BC");
    dir->mkdir("cut");


    helpString = "EDF file read successfull";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

}

void MainWindow::setExpName()
{
    QString helpString;
    ExpName = ui->Name->text();
    helpString = "Name approved: " + ExpName + "\n";
    ui->textEdit->append(helpString);
}

void MainWindow::sliceAll() ////////////////////////aaaaaaaaaaaaaaaaaaaaaaaaaa//////////////////
{
    QTime myTime;
    myTime.start();
    QStringList lst;
    QString helpString;

    int marker=254;
    int markerFlag = 0;
    int numChanToWrite = -1;

    readData();

    if(ui->matiCheckBox->isChecked())
    {
        edfFile & fil = globalEdf;
        if(ui->eyesCleanCheckBox->isChecked())
        {
            fil.cleanFromEyes();
            ns = fil.getNs();
        }
        if(ui->reduceChannelsCheckBox->isChecked())
        {
            QList <int> chanList;
            QStringList lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[ ,;]"),
                                                                       QString::SkipEmptyParts);
            for(int i = 0; i < lst.length(); ++i)
            {
                chanList << lst[i].toInt() - 1;
                if(chanList[i] > fil.getNs())
                {
                    cout << "sliceAll: mati bad channels list, return" << endl;
                    return;
                }
            }
            fil.reduceChannels(chanList);
            ns = fil.getNs();
        }
        // almost equal time, should use sessionEdges
        if(ui->sliceCheckBox->isChecked())
        {
#if 1
            sliceMati();
            sliceMatiPieces(true);
#else
            sliceMatiSimple();
#endif
        }
    }
    else //if !matiCheckBox->isChecked()
    {
        if(ui->eyesCleanCheckBox->isChecked()) /////////////////////////////////
        {
            eyesFast();
            if(!ui->reduceChannelsComboBox->currentText().contains("NoEyes", Qt::CaseInsensitive))
            {
                ui->reduceChannelsComboBox->setCurrentIndex(ui->reduceChannelsComboBox->currentIndex()+1); //generality
            }
        }

        if(ui->reduceChannelsCheckBox->isChecked()) reduceChannelsFast();

        if(ui->sliceWithMarkersCheckBox->isChecked())
        {
            numChanToWrite = ns;
        }
        else
        {
            numChanToWrite = ns - 1;
        }

        if(ui->sliceCheckBox->isChecked())
        {
            QStringList list = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
            if(!QString(label[list.last().toInt() - 1]).contains("Markers") && ui->reduceChannelsCheckBox->isChecked())
            {
                QMessageBox::critical(this, tr("Doge"), tr("Bad Markers channel in rdc channel lineEdit"), QMessageBox::Ok);
                return;
            }

            if(ui->ntRadio->isChecked()) // for Boris
            {
#if 0
                slice(10, 49, "m"); //math.operation
                slice(50, 89, "e"); //emotional verb
                slice(90, 129, "v"); //verb
                slice(130, 169, "n"); //noun
                slice(170, 209, "a"); //number
#endif
            }
            else if(ui->enRadio->isChecked())
            {
                if(ui->windButton->isChecked()) //bad work
                {
                    int timeShift = ui->timeShiftSpinBox->value();
                    int wndLength = ui->windowLengthSpinBox->value();
#if 0
                    for(int i = 0; i < (ndr*nr[ns-1]-staSlice-10*nr[ns-1])/timeShift; ++i)
                    {
                        for(int j = 0; j < wndLength; ++j)
                        {
                            if((data[ns-1][staSlice+i*timeShift + j] - 241) * (data[ns-1][staSlice+i*timeShift + j] - 247) * (data[ns-1][staSlice+i*timeShift + j] - 254) == 0 )
                            {
                                markerFlag = 0;
                                marker=300;
                            }
                        }
                        if(markerFlag == 1) marker=241;
                        if(markerFlag == 2) marker=247;
                        if(markerFlag == 3) marker=254;

                        if(marker != 300) sliceWindow(staSlice+i*timeShift, staSlice+i*timeShift+wndLength, int(i+1), marker);

                        for(int j = 0; j < wndLength; ++j)
                        {
                            //                            switch(int(data[ns-1][staSlice + i*timeShift + j]))
                            //                            {
                            //                            case 241:{markerFlag = 0; break;}
                            //                            case 247:{markerFlag = 1; break;}
                            //                            case 254:{markerFlag = 2; break;}
                            //                            }
                            if(data[ns-1][staSlice+i*timeShift + j]==241) {markerFlag=1;}
                            if(data[ns-1][staSlice+i*timeShift + j]==247) {markerFlag=2;}
                            if(data[ns-1][staSlice+i*timeShift + j]==254) {markerFlag=3;}
                        }
                        if(int(100*(i+1)/((ndr*nr[ns-1]-staSlice-10*nr[ns-1])/timeShift)) > ui->progressBar->value()) ui->progressBar->setValue(int(100*(i+1)/((ndr*nr[ns-1]-staSlice-10*nr[ns-1])/timeShift)));
                    }
#endif
                }
                if(ui->realButton->isChecked())
                {
                    if(ui->reduceChannelsComboBox->currentText().contains("MichaelBak")) //generality
                    {
                        sliceBak(1, 60, "241");
                        sliceBak(61, 120, "247");
                        sliceBak(121, 180, "241");
                        sliceBak(181, 240, "247");
                    }
                    else
                    {
                        sliceOneByOneNew(numChanToWrite);
                        //                        sliceFromTo(241, 231, "241_pre");
                        //                        sliceFromTo(247, 231, "247_pre");  //accord with presentation markers
                        //                        sliceFromTo(247, 237, "247_pre");
                        //                        helpString = dir->absolutePath() + slash() + "Realisations";
                        //                        cleanDir(helpString, "_pre", false);
                    }
                }
            }
        }


        --ns; //-markers channel generality
        ns = numChanToWrite; //generality

        ui->progressBar->setValue(0);

        helpString="data has been sliced \n";
        ui->textEdit->append(helpString);


    }


    helpString = "data sliced ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    cout << "sliceAll: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::readData()
{
    //    a = a0 + (a1-a0) * (d-d0) / (d1-d0).
    //    8 ascii : version of this data format (0)
    //    80 ascii : local patient identification (mind item 3 of the additional EDF+ specs)
    //    80 ascii : local recording identification (mind item 4 of the additional EDF+ specs)
    //    8 ascii : startdate of recording (dd.mm.yy) (mind item 2 of the additional EDF+ specs)
    //    8 ascii : starttime of recording (hh.mm.ss)
    //    8 ascii : number of bytes in header record
    //    44 ascii : reserved
    //    236

    //    8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs)
    //    8 ascii : duration of a data record, in seconds
    //    4 ascii : number of signals (ns) in data record
    //    ns * 16 ascii : ns * label (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)
    //    ns * 80 ascii : ns * transducer type (e.g. AgAgCl electrode)
    //    ns * 8 ascii : ns * physical dimension (e.g. uV or degreeC)
    //    ns * 8 ascii : ns * physical minimum (e.g. -500 or 34)
    //    ns * 8 ascii : ns * physical maximum (e.g. 500 or 40)
    //    ns * 8 ascii : ns * digital minimum (e.g. -2048)
    //    ns * 8 ascii : ns * digital maximum (e.g. 2047)
    //    ns * 80 ascii : ns * prefiltering (e.g. HP:0.1Hz LP:75Hz)
    //    ns * 8 ascii : ns * nr of samples in each data record
    //    ns * 32 ascii : ns * reserved

        //    Physical minimum: -4096   a0
        //    Physical maximum: 4096    a1
        //    Digital minimum: -32768  d0
        //    Digital maximum: 32767   d1

    QTime myTime;
    myTime.start();

    QString helpString;
    helpString = QDir::toNativeSeparators( ui->filePathLineEdit->text() );
    if(!QFile::exists(helpString))
    {
        cout << "readData: edf file doent exist\n" << helpString.toStdString() << endl;
        return;
    }


#if 1

    globalEdf.readEdfFile(helpString);
    ns = globalEdf.getNs();
    for(int i = 0; i < ns; ++i)
    {
        nr[i] = int(globalEdf.getNr()[i]);
    }
    ndr = globalEdf.getNdr();
    ddr = globalEdf.getDdr();
    globalEdf.getLabelsCopy(label);
#else
    FILE * edf = fopen(helpString, "r"); //generality
    if(edf == NULL)
    {
        cout << "readData: cannot open edf file, but it should exist" << endl;
        return;
    }

    char *helpCharArr = new char[50];
    char helpChar;
    int bytes;
    short int a;
    unsigned short markA;

    helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "header.txt");
    FILE * header = fopen(helpString, "w");



    FILE *edfNew;
    int flag = 0;

    QFile *tempFile = new QFile();
    tempFile->open(edf, QIODevice::ReadOnly);
    if(ui->edfBox->isChecked())
    {
        flag=1;
        helpString = tempFile->fileName();
        helpString.resize(helpString.length()-4);
        helpString.append(QString::number(NumOfEdf)).append(".edf");
        edfNew = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
        ++NumOfEdf;
    }
    tempFile->close();
    delete tempFile;
//    cout << "start header read" << endl;

    //header read
    for(int i = 0; i < 184; ++i)
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(header, "%c", helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }

    //number of bytes in header record
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);

        fprintf(header, "%c", helpCharArr[i]);
        if(flag==1) fprintf(edfNew, "%c", helpCharArr[i]);
    }
    helpCharArr[8] = '\0';
    bytes = atoi(helpCharArr);

    //"reserved"
    for(int i = 0; i < 44; ++i)
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(header, "%c", helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }

    //number of data records
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(header, "%c", helpCharArr[i]);
    }
    helpCharArr[8] = '\0';
    ndr = atoi(helpCharArr);                      //NumberOfDataRecords

    //duration of a data record, in seconds
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(header, "%c", helpCharArr[i]);
    }
    helpCharArr[8] = '\0';
    ddr = atoi(helpCharArr);                       //DurationOfDataRecord
//    cout << "ddr=" << ddr << endl;


    int helpInt = 0;
    if(flag==1)
    {
        helpInt=floor((ui->finishTimeBox->value() - ui->startTimeBox->value())/double(ddr));
        helpString.setNum(helpInt);

        int s = 0;
        s = fprintf(edfNew, "%d", helpInt);
        for(int i = s; i < 8; ++i)
        {
            fprintf(edfNew, "%c", char(32)); // char(32) is space
        }

        s = fprintf(edfNew, "%d", ddr);
        for(int i = s; i < 8; ++i)
        {
            fprintf(edfNew, "%c", char(32));
        }

    }

    //number of signals
    for(int i = 0; i < 4; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(header, "%c", helpCharArr[i]);
        if(flag==1) fprintf(edfNew, "%c", helpCharArr[i]);
    }
    helpCharArr[4] = '\0';
    ns = atoi(helpCharArr);                        //Number of channels

    //labels
    for(int i = 0; i < ns*16; ++i)
    {
        fscanf(edf, "%c", &label[i/16][i%16]);
        fprintf(header, "%c", label[i/16][i%16]);
        if(flag==1) fprintf(edfNew, "%c", label[i/16][i%16]);
        if(i%16==15) label[i/16][16]='\0';
    }

    //edit EOG channels generality for encephalan
    for(int i = 0; i < ns; ++i)
    {
        if(QString(label[i]).contains("EOG 1"))
        {
            strcpy(label[i], "EOG EOG1-A2     ");
        }
        else if(QString(label[i]).contains("EOG 2"))
        {
            strcpy(label[i], "EOG EOG2-A1     ");
        }
    }

    helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "labels.txt");
    FILE * labels=fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    for(int i = 0; i < ns; ++i)                         //label write in file
    {
        fprintf(labels, "%s \n", label[i]);
    }
    fclose(labels);
    for(int i = ns; i < maxNs; ++i)
    {
        label[i][0] = '\0';
    }

    //transducer type
    for(int i = 0; i < ns * 80; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(header, "%c", helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }


    //physical dimension
    for(int i = 0; i < ns*8; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(header, "%c", helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }

    //physical minimum
    double *physMin;
    physMin = new double [ns];

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(header, "%c", helpCharArr[j]);
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        physMin[i]=double(atoi(helpCharArr));
    }

    //physical maximum
    double *physMax;
    physMax = new double [ns];

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(header, "%c", helpCharArr[j]);
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        physMax[i]=double(atoi(helpCharArr));
    }

    //digital minimum
    double *digMin;
    digMin = new double [ns];

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(header, "%c", helpCharArr[j]);
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        digMin[i]=double(atoi(helpCharArr));
    }

    //digital maximum
    double *digMax;
    digMax = new double [ns];

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(header, "%c", helpCharArr[j]);
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        digMax[i]=double(atoi(helpCharArr));
    }

    //prefiltering
    for(int i = 0; i < ns*80; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(header, "%c", helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }


    //number of records (nr samples in ddr seconds)
    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(header, "%c", helpCharArr[j]);
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        helpCharArr[8] = '\0';
        nr[i]=atoi(helpCharArr);
    }


    //reserved
    for(int i = 0; i < ns*32; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(header, "%c", helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }


    for(int i = 0; i < (bytes-(ns+1)*256); ++i)                      //Neurotravel//generality//strange
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(header, "%c", helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }
    fclose(header);

    ui->finishTimeBox->setMaximum(ddr*ndr);

    helpString = dir->absolutePath() + slash() + ExpName + "_markers.txt";
    FILE * markers = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");

    QString * annotations = new QString [5000];
    int numOfAnn = 0;

    fpos_t *position = new fpos_t;
    fgetpos(edf, position);
    fclose(edf);
    edf = fopen(QDir::toNativeSeparators(ui->filePathLineEdit->text()).toStdString().c_str(), "rb"); //generality
    fsetpos(edf, position);
    delete position;
    vector<bool> byteMarker;

    double markerValue = 0.;
//    cout << "start data read ndr=" << ndr << " ns=" << ns << endl;
    if(ui->ntRadio->isChecked())
    {
//        ofstream outStr;
//        outStr.open("/media/Files/Data/aanot.txt");
        for(int i = 0; i < ndr; ++i)
        {
//            cout << i << " "; cout.flush();
//            if(i==1000) break;
            for(int j = 0; j < ns; ++j)
            {
                helpString.clear();
                for(int k = 0; k < nr[j]; ++k)
                {
                    if(j!=(ns-1))  ////////////generality////////////
                    {
                        fread(&a, sizeof(short), 1, edf);
                        data[j][i*nr[j]+k] = physMin[j] + (physMax[j]-physMin[j]) * (double(a)-digMin[j]) / (digMax[j] - digMin[j]);   //neurotravel
                    }
                    else
                    {
                        //edf+
                        fscanf(edf, "%c", &helpChar);
                        helpString.append(QChar(helpChar));
                        fscanf(edf, "%c", &helpChar);
                        helpString.append(QChar(helpChar));
                    }
                }

                //handle annotations
                //edf+
                if(j == ns-1)  ////////generality?/////////
                {

                }

            }
            if(!annotations[i].isEmpty())
            {
//                outStr << annotations[i].toStdString() << endl;
            }
        }
//        outStr.close();
    }

    if(ui->enRadio->isChecked())
    {
        for(int i = 0; i < ndr; ++i)
        {
            if(flag == 1 && (i+1) > ui->finishTimeBox->value()) break;  //save as EDF
            for(int j = 0; j < ns; ++j)
            {
                for(int k = 0; k < nr[j]; ++k)
                {
                    if(!ui->matiCheckBox->isChecked())
                    {
                        fread(&a, sizeof(short), 1, edf);
                        if(flag==1 && ((i*ddr)>=ui->startTimeBox->value()) && ((i*ddr+1) <=ui->finishTimeBox->value()))    //save as EDF
                        {
                            fwrite(&a, sizeof(short), 1, edfNew);
                        }
                        if(j != ns-1)
                        {
                            data[j][i*nr[j]+k] = physMin[j]
                                    + (physMax[j]-physMin[j])
                                    * (double(a)-digMin[j])
                                    / (digMax[j] - digMin[j]  + 1);
//                            data[j][i*nr[j]+k] = a *1./8.; //generality
                        }
                        else
                        {
                            data[j][i*nr[j]+k] = physMin[j]
                                    + (physMax[j]-physMin[j])
                                    * (double(a)-digMin[j])
                                    / (digMax[j] - digMin[j]);
//                            data[j][i*nr[j]+k] = a;
                        }
                    }
                    else if(ui->matiCheckBox->isChecked())
                    {
                        if(j != ns-1)
                        {
                            fread(&a, sizeof(short), 1, edf);
                            if(flag==1 && ((i*ddr)>=ui->startTimeBox->value()) && ((i*ddr+1) <=ui->finishTimeBox->value()))    //save as EDF
                            {
                                fwrite(&a, sizeof(short), 1, edfNew);
                            }
                            data[j][i*nr[j]+k] = physMin[j]
                                    + (physMax[j]-physMin[j])
                                    * (double(a)-digMin[j])
                                    / (digMax[j] - digMin[j]  + 1);
//                            data[j][i*nr[j]+k] = a *1./8.;
                        }
                        else //if(j == ns - 1)
                        {
                            fread(&markA, sizeof(unsigned short), 1, edf);
                            data[j][i*nr[j]+k] = physMin[j]
                                    + (double(markA) - digMin[j])
                                    * (physMax[j] - physMin[j])
                                    / (digMax[j] - digMin[j]);
//                            data[j][i*nr[j]+k] = markA;

                            if(data[j][i*nr[j]+k] != 0 )
                            {
                                matiFixMarker(data[j][i*nr[j]+k]);
//                                matiPrintMarker("", data[j][i*nr[j]+k]);
                            }
                        }
                    }

                    //handle markers
                    if(j == ns-1)
                    {
                        markerValue = data[j][i*nr[j]+k];
                        if(markerValue != 0.)
                        {
                            bytes = i*nr[j]+k; //time
                            fprintf(markers, "%d %d", bytes, int(markerValue));

                            if(ui->matiCheckBox->isChecked())
                            {
                                byteMarker = matiCountByte(markerValue);
                                fprintf(markers, "\t");
                                for(int s = 15; s >= 0; --s)
                                {
                                    fprintf(markers, "%d", int(byteMarker[s]));
                                    if(s == 8)
                                    {
                                        fprintf(markers, " ");  //byte delimiter
                                    }
                                }
                                if(byteMarker[10])
                                {
                                    fprintf(markers, " - session end");
                                }
                                else if(byteMarker[9] || byteMarker[8])
                                {
                                    fprintf(markers, " - session start");
                                }
                                if(byteMarker[12])
                                {
                                    fprintf(markers, " - 12 bit error");
                                }
                                if(byteMarker[11])
                                {
                                    fprintf(markers, " - 11 bit error");
                                }
                            }

                            fprintf(markers, "\n");
                        }

                        if(data[j][i*nr[j]+k] == 200)
                        {
                            staSlice = i*nr[j]+k;
                        }
                    }
                }

            }
        }
//        cout << "staSlice=" << staSlice << " staTime=" << staSlice/def::freq << endl;
    }
    fclose(markers);

    if(ui->ntRadio->isChecked())
    {
        double markTime;
        char * markNum = new char[60];
        QString markValue;
        for(int j = 0; j < numOfAnn; ++j)
        {
            markNum[0]='\0';
            markValue="";
            sscanf(annotations[j].toStdString().c_str(), "+%lf\24", &markTime);
            //set time into helpString with 3 float numbers
            helpString.setNum(markTime);
            if(helpString[helpString.length()-3]=='.') helpString.append("0"); //float part - 2 or 3 signs
            else
            {
                if(helpString[helpString.length()-2]=='.') helpString.append("00");
                else helpString.append(".000");
            }
            for(int i = helpString.length()+2; i < annotations[j].length(); ++i) //+2 because of '+' and '\24'
            {
                markValue.append(annotations[j][i]);
            }
            sscanf(annotations[j].toStdString().c_str(), "+%lf\24%s", &markTime, markNum);
            data[ns-1][int(markTime*nr[ns-1]/ddr)] = atoi(markNum);
        }
        delete []markNum;
    }
    delete []helpCharArr;
    if(flag==1)     //save as EDF
    {
        fclose(edfNew);
    }

    rewind(edf);

    delete []physMin;
    delete []physMax;
    delete []digMin;
    delete []digMax;
    delete[] annotations;
    fclose(edf);
#endif

    helpString="data have been read ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to " + QString::number(ns);
    ui->textEdit->append(helpString);

//    staSlice += 3; //generality LAWL

    ui->markerSecTimeDoubleSpinBox->setMaximum(ndr * ddr);
    ui->markerBinTimeSpinBox->setMaximum(nr[ns-1] * ndr * ddr);

//    cout << "readData: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

#if 0
void MainWindow::writeEdf(QString inFilePath, double ** inData, QString outFilePath, int numSlices, QList<int> chanList)
{
    if(!QFile::exists(inFilePath))
    {
        cout << "writeEDF: bad inFilePath\n" << inFilePath.toStdString() << endl;
        return;
    }

    QTime myTime;
    myTime.start();


    if(chanList.isEmpty()) //use ui->reduceChannelsLineEdit
    {
        QStringList lst;
        lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
        chanList.clear();
        for(int i = 0; i < lst.length(); ++i)
        {
            chanList << lst[i].toInt() - 1;
        }
    }

    edfFile fil;
    fil.readEdfFile(inFilePath);
    fil.writeOtherData(inData, numSlices, outFilePath, chanList);

//    globalEdf.writeOtherData(inData, numSlices, outFilePath, chanList); // should test

//    cout << "writeEDF: output path = " << outFilePath.toStdString() << "\ttime = = " << myTime.elapsed()/1000. << " sec" << endl;
}

#endif
void MainWindow::drawRealisations()
{
    QStringList lst;
    QStringList nameFilters;
    QString helpString;
    QString prePath;

    QTime myTime;
    myTime.start();
    ui->progressBar->setValue(0);
    matrix dataD(ns, 50 * 60 * 250); // for 50 seconds

    lst.clear();
    nameFilters.clear();
    nameFilters.append("*_241*");
    nameFilters.append("*_247*");
    nameFilters.append("*_254*");
    nameFilters.append("*_244*");

    //for Roma's files
    nameFilters.append("*_2");
    nameFilters.append("*_1");

    dir->cd(ui->drawDirBox->currentText());    //->windows or Realisations or cut
    prePath = dir->absolutePath();

    lst = dir->entryList(QDir::Files);
//    lst = dir->entryList(nameFilters, QDir::Files);

    dir->cdUp();   //-> into EDF-file dir
    if(ui->drawDirBox->currentText() == "windows/fromreal") dir->cdUp();

    int redCh = -1;
    int blueCh = -1;
    if(ns == 41)
    {
        redCh = 21;
        blueCh = 22;
    }
    else if(ns == 22 || ns == 21)
    {
        redCh = 19;
        blueCh = 20;
    }

    for(int i = 0; i < lst.length(); ++i)
    {
        if(stopFlag) break;
        helpString = prePath + slash() + lst[i];
        readPlainData(helpString,
                      dataD,
                      ns,
                      NumOfSlices);

        if(NumOfSlices > 15000)
        {
            continue;
        }

        helpString = getPicPath(helpString, dir, ns);
        drawEeg(dataD,
                ns,
                NumOfSlices,
                def::freq,
                helpString,
                ui->drawCoeffSpinBox->value(),
                blueCh,
                redCh); // generality freq

        if(int(100*(i+1)/lst.length()) > ui->progressBar->value()) ui->progressBar->setValue(int(100*(i+1)/lst.length()));
        qApp->processEvents();
    }

    ui->progressBar->setValue(0);


    helpString = "signals are drawn ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    stopFlag = 0;
    cout << "drawRealisations: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::cleanDirs()
{
    QString helpString;

    if(dir->isRoot())
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No dir"), QMessageBox::Ok);
        return;
    }

    //windows
    if(ui->cleanWindowsCheckBox->isChecked())
    {
        helpString = dir->absolutePath()
                + slash() + "windows";
        cleanDir(helpString);
    }

    //SpectraSmooth/windows
    if(ui->cleanWindSpectraCheckBox->isChecked())
    {
        helpString = dir->absolutePath()
                + slash() + "SpectraSmooth"
                + slash() + "windows";
        cleanDir(helpString);
    }


    //SpectraSmooth
    if(ui->cleanRealsSpectraCheckBox->isChecked())
    {
        helpString = dir->absolutePath()
                + slash() + "SpectraSmooth";
        cleanDir(helpString);
    }

    //windows/fromreal
    if(ui->cleanFromRealsCheckBox->isChecked())
    {
        helpString = dir->absolutePath()
                + slash() + "windows"
                + slash() + "fromreal";
        cleanDir(helpString);
    }

    //Realisations
    if(ui->cleanRealisationsCheckBox->isChecked())
    {
        helpString = dir->absolutePath()
                + slash() + "Realisations";
        cleanDir(helpString);
    }

    //Help
    if(ui->cleanHelpCheckBox->isChecked())
    {
        helpString = dir->absolutePath()
                + slash() + "Help";
        cleanDir(helpString);
    }

    //markers
    if(ui->cleanMarkersCheckBox->isChecked())
    {
        helpString = dir->absolutePath();
        cleanDir(helpString, "markers", 0);
    }

    helpString="dirs cleaned ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
}



void MainWindow::markerSetSecTime(int a)
{
    ui->markerSecTimeDoubleSpinBox->setValue(double(a)/nr[ns-1]);
}

void MainWindow::markerGetSlot()
{
    bool byteMarker[16];
    int timeIndex = ui->markerBinTimeSpinBox->value();
    int marker = 0;
    marker = globalEdf.getData()[globalEdf.getMarkChan()][timeIndex];
    QString helpString;

    for(int h = 0; h < 16; ++h)
    {
        byteMarker[16-1 - h] = (marker%(int(pow(2,h+1)))) / (int(pow(2,h)));
    }

    helpString.clear();
    for(int h = 0; h < 8; ++h)
    {
        helpString += QString::number(byteMarker[h]);
    }
    ui->markerBin0LineEdit->setText(helpString);

    helpString.clear();
    for(int h = 0; h < 8; ++h)
    {
        helpString += QString::number(byteMarker[h + 8]);
    }
    ui->markerBin1LineEdit->setText(helpString);

    ui->markerDecimalLineEdit->setText(QString::number(marker));
}

void MainWindow::markerSetSlot()
{
    int timeIndex = ui->markerBinTimeSpinBox->value();
    int marker = ui->markerDecimalLineEdit->text().toInt();
    globalEdf.setData(globalEdf.getMarkChan(), timeIndex, marker);
}

void MainWindow::markerSetDecValueSlot()
{
    int marker = 0;
    QString helpString;

    helpString = ui->markerBin0LineEdit->text();
    for(int h = 0; h < 8; ++h)
    {
        if(helpString[h] != '0' && helpString[h] != '1') return;
        marker += pow(2, 8) * (helpString[h] == '1') * pow(2, 7-h);
    }

    helpString = ui->markerBin1LineEdit->text();
    for(int h = 0; h < 8; ++h)
    {
        if(helpString[h] != '0' && helpString[h] != '1') return;
        marker += (helpString[h] == '1') * pow(2, 7-h);
    }
    ui->markerDecimalLineEdit->setText(QString::number(marker));
}

void MainWindow::markerSetBinValueSlot()
{
    int marker = ui->markerDecimalLineEdit->text().toInt();
    QString helpString;

    vector<bool> byteMarker = matiCountByte(double(marker));

    helpString.clear();
    for(int h = 15; h >= 8; --h)
    {
        helpString += (byteMarker[h])?"1":"0";
    }
    ui->markerBin0LineEdit->setText(helpString);

    helpString.clear();
    for(int h = 7; h >= 0; --h)
    {
        helpString += (byteMarker[h])?"1":"0";
    }
    ui->markerBin1LineEdit->setText(helpString);
}

void MainWindow::markerSaveEdf()
{
    QString helpString;
    helpString = dir->absolutePath() + slash() + ExpName + ui->newEdfNameLineEdit->text();
    if(!helpString.endsWith(".edf", Qt::CaseInsensitive))
    {
        helpString += ".edf";
    }
    helpString = setFileName(helpString);
    globalEdf.writeEdfFile(helpString);
}



void MainWindow::takeSpValues(int b, int c, double d)
{
    QString helpString;
    spLength = c-b+1;
    left = b;
    right = c;
    spStep = d;
    helpString="SpVal taken";
    ui->textEdit->append(helpString);
}

void MainWindow::drawMapsSlot()
{
    QString helpString;
    helpString = QFileDialog::getOpenFileName(this, tr("Choose maps file"), dir->absolutePath(), tr("*.txt"));
    if(!QFile::exists(helpString))
    {
        return;
    }
    drawMapsICA(dir->absolutePath(), ns, helpString, ExpName);
}

void MainWindow::avTime()
{
    int maxLen = 10000;
    int numNotSolved = 0;
    int shortReals = 0;
    double av = 0.;
    double solveTime = 0.;
    int num = 0;
    FILE * fil;

    QStringList lst;
    QString helpString;


    QString tmp;
    for(int j = 0; j < 2; ++j)
    {
        av = 0.;
        shortReals = 0;
        numNotSolved = 0;
        if(j == 0) tmp = "241";
        else if(j == 1) tmp = "247";

        dir->cd("Realisations");
        lst = dir->entryList(QStringList(QString("*_" + tmp + "*")), QDir::Files);
        for(int i = 0; i <  lst.length(); ++i)
        {
            helpString = dir->absolutePath() + slash() + lst[i];
            fil = fopen(helpString, "r");
            fscanf(fil, "NumOfSlices %d", &num);
            fclose(fil);
            av += num;

            if(num < 750) ++shortReals;
            if(fabs(maxLen/double(num) - 1.) < 0.01) ++numNotSolved;
            else cout << num/def::freq << endl;
        }
        dir->cdUp();

        solveTime = av / (def::freq*lst.length());
        cout << "solveTime " << tmp << " = " << solveTime << endl << endl;
        cout << "num not solved " << tmp << " = " << numNotSolved << endl << endl;

        helpString = dir->absolutePath() + slash() + "results.txt";
        FILE * res = fopen(helpString, "a+");
        fprintf(res, "solve time %d\t%.1lf\n", tmp.toInt(), solveTime);
        fprintf(res, "num not solved %d\t%d\n", tmp.toInt(), numNotSolved);
        fprintf(res, "short realisations %d\t%d\n", tmp.toInt(), shortReals);
        fclose(res);
    }

}

void MainWindow::writeCorrelationMatrix(QString edfPath, QString outPath) //unused
{
    setEdfFile(edfPath);
    readData();
    int fr = def::freq;

    //write components cross-correlation matrix
    int size = ns - 1;
    matrix corrs(size, size);
    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            corrs[i][j] = correlation(globalEdf.getData()[i].data(),
                                      globalEdf.getData()[j].data(),
                                      globalEdf.getDataLen());
            corrs[j][i] = corrs[i][j];
        }
        corrs[i][i] = 0.;
    }
    writeICAMatrix(outPath, corrs, size); //generality 19-ns

}

void MainWindow::setBoxMax(double a)
{
    ui->startTimeBox->setValue(min(a-1., ui->startTimeBox->value()));
    ui->startTimeBox->setMaximum(a-1.);
}

void MainWindow::sliceWithMarkersSlot(int a)
{
    withMarkersFlag = a;
}

void MainWindow::stop()
{
    stopFlag = 1;
}

void MainWindow::changeNsLine(int a)
{
    ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(a).toString());
}

void MainWindow::setNs()
{
    QString helpString;
    ns=ui->setNsLine->text().toInt();
    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}

void MainWindow::setNsSlot(int a)
{
    QString helpString;
    ns=a;
    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}

void MainWindow::customFunc()
{

    return;
    // concat all mati sessions

//    dir->cd("/media/Files/Data/Mati");
//    QStringList dirLst = dir->entryList(QStringList("???"), QDir::Dirs|QDir::NoDotAndDotDot);
//    for(QString & guy : dirLst)
//    {
//        dir->cd(guy);
//        dir->cd("auxEdfs");

//        QString helpString = dir->absolutePath() + slash() + guy + "_0.edf";
//        if(!QFile::exists(helpString))
//        {
//            dir->cdUp();
//            dir->cdUp();
//            continue;
//        }
//        edfFile initFile;
//        initFile.readEdfFile(helpString);
//        helpString.replace("_0.edf", "_1.edf");
//        initFile.concatFile(helpString);
//        helpString.replace("_1.edf", "_2.edf");
//        initFile.concatFile(helpString);

//        dir->cdUp();
//        QString helpString2 = dir->absolutePath() + slash() + guy + "_full.edf";
//        initFile.writeEdfFile(helpString2);
//        dir->cdUp();
//    }
//    exit(0);

    // do the ICA
    dir->cd("/media/Files/Data/Mati");
    QStringList dirLst = dir->entryList(QStringList("???"), QDir::Dirs|QDir::NoDotAndDotDot);
    for(QString & guy : dirLst)
    {
        if(guy == "ADA") continue;
        dir->cd(guy);

        QString helpString = dir->absolutePath() + slash() + guy + "_full.edf";
        if(!QFile::exists(helpString))
        {
            dir->cdUp();
            continue;
        }
        setEdfFile(helpString);
        ICA();
        dir->cdUp();

    }
    exit(0);



    GalyaProcessing();
    exit(0);


#if 0
    double ** dataFFT = new double * [maxNs];
    for(int i = 0; i < maxNs; ++i)
    {
        dataFFT[i] = new double [65536];
    }
    setEdfFile("/media/Files/Data/Mati/PYV/PYV_rr.edf");
    readData();
    globalEdf.saveSubsection(100000, 106000,
                             "/media/Files/Data/Mati/PYV/test.edf");

    setEdfFile("/media/Files/Data/Mati/PYV/test.edf");
    readData();
    calcSpectre(data, dataFFT, ns-1, 6000, 50);
    drawSpectra(dataFFT, ns-1,
                fftLimit(5., def::freq, fftL(6000)),
                fftLimit(20., def::freq, fftL(6000)),
                "/media/Files/Data/Mati/PYV/test0.jpg");


    for(int i = 0; i < ns-1; ++i)
    {
        for(int j = 0; j < globalEdf.getDataLen(); ++j)
        {
            data[i][j] *= 0.5 * (1 - cos(2 * pi * j / (globalEdf.getDataLen() - 1)));
        }
    }
    calcSpectre(data, dataFFT, ns-1, 6000, 50);

    drawSpectra(dataFFT, ns-1,
                fftLimit(5., def::freq, fftL(6000)),
                fftLimit(20., def::freq, fftL(6000)),
                "/media/Files/Data/Mati/PYV/test1.jpg");


    exit(0);
#endif

//    cout << sizeof(globalEdf.getData()) << endl;

//    exit(0);
//    ui->eyesCleanCheckBox->setChecked(true);
//    sliceAll();


//    edfFile fil;
//    fil.readEdfFile("/media/Files/IHNA/Data/MATI/archive/NOS_1.EDF");
//    fil.concatFile("/media/Files/IHNA/Data/MATI/archive/NOS_2.EDF",
//                    "/media/Files/IHNA/Data/MATI/archive/NOS.EDF");
//    edfFile fil;
//    QList<int> leest;
//    for(int i = 0; i < 18; ++i)
//    {
//        leest << i;
//    }
//    leest << 19;
//    fil.readEdfFile("/media/Files/Data/Mati/FEV/auxEdfs/FEV_2.edf");
//    fil.reduceChannels(leest);
//    fil.writeEdfFile("/media/Files/Data/Mati/FEV/auxEdfs/FEV_18_2.edf");
//    exit(0);

//    setEdfFile("/media/Files/Data/Mati/ADA/ADA_w.edf");
//    ui->reduceChannelsComboBox->setCurrentText("22NoEyes");
//    constructEDFSlot();



#if 0
    {
        dir->cd("/media/Files/Data/Mati/SDA");
        QStringList lst = dir->entryList(QStringList("*_a_c_*markers.txt"));
        QString str1, str2;
        for(int i = 0; i < lst.length(); ++i)
        {
            str1 = dir->absolutePath() + slash() + lst[i];
            str2 = lst[i];
            str2.replace("_a_c_", "_a_");
            str2 = dir->absolutePath() + slash() + str2;
            cout << str1 << '\n' << str2 << '\n' << areEqualFiles(str1, str2) << endl << endl;
        }
    }
#endif
#if 0
    {
        edfFile fil;
        fil.readEdfFile("/media/Files/Data/Mati/SDA/SDA_rr_f_a_0_2.edf");
        fil.writeEdfFile("/media/Files/Data/Mati/SDA/SDA_rr_f_a_0_2.txt", true);
//        edfFile fil2;
//        fil2.readEdfFile("/media/Files/Data/Mati/SDA/SDA_rr_f_a_c_0_0.edf");
//        for(int i = 0; i < 41; ++i)
//        {
//            cout << correlation(fil.getData()[i].data(), fil2.getData()[i].data(), fil.getDataLen()) << endl;
//        }
        exit(1);
    }
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
        tDir->cd("/media/Files/Data/Mati");
        QStringList nameFilters;
        nameFilters << "*.edf" << "*.EDF";
        QString fileName;
        QString helpString;

        QStringList lst = tDir->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            tDir->cd(lst[i]);

            helpString = tDir->absolutePath() + "/" + lst[i] + ".EDF";
            setEdfFile(helpString);
            rereferenceDataSlot();

            helpString = tDir->absolutePath() + "/" + lst[i] + "_rr.edf";
            setEdfFile(helpString);
            refilterDataSlot();

            helpString = tDir->absolutePath() + "/" + lst[i] + "_rr_f.edf";
            setEdfFile(helpString);
            sliceAll();
            drawRealisations();
            tDir->cdUp();

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
        tDir->cd("/media/Files/Data/Mati");
        QStringList nameFilters;
        nameFilters << "*.edf" << "*.EDF";
        QString fileName;

        QStringList lst = tDir->entryList(nameFilters);
        for(int i = 0; i < lst.length(); ++i)
        {
            fileName = lst[i];
            fileName.resize(fileName.indexOf('.'));
            cout << fileName.toStdString() << endl;

            tDir->mkdir(fileName);
            QFile::copy(tDir->absolutePath() + "/" + lst[i], tDir->absolutePath() + "/" + fileName + "/" + lst[i]);
//            QFile::remove(tDir->absolutePath() + "/" + lst[i]);
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
        Spectre * sp = new Spectre(dir, ns, ExpName);
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
            distr[int(floor((data[chn][i]/maxA + 1.)/2./step + 0.5))] += 1.;
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
        dir->cd(def::dataFolder + "/AA");


        FILE * results = fopen(QString(def::dataFolder + "/AA/res.txt").toStdString().c_str(), "r");



        outStream.open(QString(def::dataFolder + "AA/discr.txt").toStdString().c_str());
        outStream << "name" << "\t";
        outStream << "mean" << "\t";
        outStream << "sigma" << endl;

        QString helpString;
        QStringList lst = dir->entryList(QStringList("*randIca.txt"), QDir::NoFilter, QDir::Name);
        int num1 = lst.length()*2;
        double * sigm = new double [num1];
        double * men = new double [num1];
        double * classf = new double [num1];
        double * drawVars = new double [num1];
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = dir->absolutePath() + slash() + lst[i];
            helpString.replace(".txt", ".png");
            countRCP(QString(dir->absolutePath() + slash() + lst[i]), helpString, &men[i], &sigm[i]);

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
        dir->cd(def::dataFolder + "/GPP/edf/1");
        QString pth = "/media/michael/Files/Data/GPP/Realisations";
        QStringList lst = dir->entryList(QStringList("*.edf"), QDir::Files);
        QString helpString;
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = dir->absolutePath() + slash() + lst[i];
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


        dir->cd(def::dataFolder + "/GPP/edf/2");
        lst = dir->entryList(QStringList("*.edf"), QDir::Files);
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = dir->absolutePath() + slash() + lst[i];
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

    //    dir->cd("/media/Files/Data/AB/");
    //    QStringList lst11 = dir->entryList(QStringList("*_sum_ica.edf"));
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
        Net * ann = new Net(dir, ns, left, right, spStep, ExpName);
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
        dir->cd("/media/Files/Data/AB/SpectraSmooth/PCA");
        QString pcaPath = "/media/Files/Data/AB/SpectraSmooth/PCA";
        QStringList lst;
        QString helpString;
        QStringList vars;
        vars.clear();

        vars << "*_241" << "*_247" << "*_254";
        //    vars << "*_254" << "*_241";

        for(spLength = 45; spLength < 50; spLength += 5)
        {
            ns = 1;
            int dim = spLength * ns;

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
                lst = dir->entryList(QStringList(vars[h]), QDir::Files, QDir::Name);
                makeMatrixFromFiles(pcaPath, lst, ns, spLength, 1., &matrix);
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
                lst = dir->entryList(QStringList(vars[k]), QDir::Files, QDir::Name);
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


            lst = dir->entryList(vars, QDir::Files);
            cout << "spL = " << spLength << "\tpercentage = " << errors * 100. / lst.length() << " %" << endl;
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
        dir->cd("/media/Files/Data/AB/Help");

        //        dir->cd("/media/Files/Data/AB");
        QStringList list0 = dir->entryList(QStringList("???_1_maps.txt"), QDir::NoFilter, QDir::Name);
        //        QStringList list0 = dir->entryList(QStringList("???_1_ica.edf"), QDir::NoFilter, QDir::Name);
        ui->cleanRealsSpectraCheckBox->setChecked(true);
        ui->cleanRealisationsCheckBox->setChecked(true);
        for(int i = 0; i < list0.length(); ++i)
        {
            //draw separate maps only
            if(1)
            {
                helpString = dir->absolutePath() + slash() + list0[i];
                drawMapsICA(helpString, 19, dir->absolutePath(), QString(list0[i].left(3) + "-m"));
            }

            //draw average spectra
            if(0)
            {
                helpString = dir->absolutePath() + slash() + list0[i];
                setEdfFile(helpString);
                cleanDirs();
                sliceAll();
                countSpectraSimple(4096);
            }
            //draw maps on average spectra
            if(0)
            {
                helpString = dir->absolutePath() + slash() + list0[i];
                helpString = dir->absolutePath() + slash() + list0[i].left(3) + "_1_ica_all.png";
                helpString2 = dir->absolutePath() + slash() + list0[i].left(3) + "_1_ica_all_withmaps.png";
                drawMapsOnSpectra(helpString, helpString2, dir->absolutePath(), QString(list0[i].left(3) + "-m"));
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

        dir->cd("/media/Files/Data/AB");
        ui->sliceWithMarkersCheckBox->setChecked(true);
        ui->splitZerosCheckBox->setChecked(true);
        nameFilters.clear();
        nameFilters << "???_1.edf" << "???_2.edf" << "???_sum.edf";


        QStringList lst11 = dir->entryList(nameFilters);
        for(int i = 0; i < lst11.length(); ++i)
        {
            cout << endl << lst11[i].toStdString() << "\tstart" << endl;
            helpString = dir->absolutePath() + slash() + lst11[i];
            setEdfFile(helpString);
            cleanDirs();
            sliceAll();
            constructEDFSlot();
            helpString = dir->absolutePath() + slash() + lst11[i];
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
        dir->cd("/media/Files/Data/AB");
        QStringList list11 = dir->entryList(QStringList("???_1.edf"));
        QStringList list22;
        for(int i = 0; i < list11.length(); ++i)
        {
            countICAs(dir->absolutePath(), list11[i], 1, 1, 0, 1);
        }
        list11 = dir->entryList(QStringList("???_1_ica.edf"));
        list22 = dir->entryList(QStringList("???_2_ica.edf"));
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
        dir->cd("/media/Files/Data/AB");
        QString helpString = dir->absolutePath() + "/AAX_sum.edf";
        setEdfFile(helpString);
        sliceAll();
        readData();
        Net * ANN;
        ANN = new Net(dir, ns, left, right, spStep, ExpName);
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
        dir->cd("/media/Files/Data/AB");
        QString helpString;
        makeCfgStatic(dir->absolutePath(), 3*247, "Reduced");
        //drop some channels
        helpString = "3 7 16 20";
        ui->reduceChannelsLineEdit->setText(helpString);

        //exclude channels from 1st file
        helpString = dir->absolutePath() + slash() + fileName1;
        setEdfFile(helpString);
        helpString = dir->absolutePath() + slash() + fileName1;
        helpString.replace(".edf", "_rdc.edf");
        reduceChannelsEDF(helpString);

        //exclude channels from 2nd file
        helpString = dir->absolutePath() + slash() + fileName2;
        setEdfFile(helpString);
        helpString = dir->absolutePath() + slash() + fileName2;
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
            tempAccuracy = filesCrossClassification(dir->absolutePath(), helpString, helpString2, "Reduced", 50, 0.15); //0.5 generality
            outStream << doubleRound(tempAccuracy, 2) << endl;
        }
        outStream.close();
    }

    //3 components - DONE
    if(0)
    {
        QTime myTime;
        myTime.start();
        dir->cd("/media/Files/Data/AB");
        cleanDir(dir->absolutePath(), "markers", 0);
        QStringList list1 = dir->entryList(QStringList("???_1.edf"));
        QString helpString;
        double res;
        ofstream outStream;

        //cross classifications
        if(0)
        {
            helpString = dir->absolutePath() + "/classLog.txt";
            outStream.open(helpString.toStdString().c_str(), ios_base::app);
            for(int i = 0; i < list1.length(); ++i)
            {
                myTime.restart();
                cout << list1[i].left(3).toStdString() << " start" << endl;
                helpString = list1[i];
                helpString.replace("_1", "_2");
                res = filesCrossClassification(dir->absolutePath(), list1[i], helpString, "16sec19ch", 50, 5, false, 01000, 125);
                outStream << list1[i].left(3).toStdString() << " cross\t" << res << endl;
                res = filesCrossClassification(dir->absolutePath(), list1[i], helpString, "4sec19ch", 50, 3, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " wnd cross\t" << res << endl;

                helpString = list1[i];
                helpString.replace("_1", "_sum");
                res = fileInnerClassification(dir->absolutePath(), helpString, "16sec19ch", 50, false, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " inner\t" << res << endl << endl;
                res = fileInnerClassification(dir->absolutePath(), helpString, "4sec19ch", 50, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " wnd inner\t" << res << endl << endl;
                cout << list1[i].left(3).toStdString() << " finished\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
            }
            outStream.close();
            list1 = dir->entryList(QStringList("???_1_ica_by1.edf"));
            helpString = dir->absolutePath() + "/classLog.txt";
            outStream.open(helpString.toStdString().c_str(), ios_base::app);
            for(int i = 0; i < list1.length(); ++i)
            {
                myTime.restart();
                cout << list1[i].left(3).toStdString() << " start" << endl;
                helpString = list1[i];
                helpString.replace("_1", "_2");
                res = filesCrossClassification(dir->absolutePath(), list1[i], helpString, "16sec19ch", 50, 5, false, 01000, 125);
                outStream << list1[i].left(3).toStdString() << " ica cross\t" << res << endl;
                res = filesCrossClassification(dir->absolutePath(), list1[i], helpString, "4sec19ch", 50, 3, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " ica wnd cross\t" << res << endl;

                helpString = list1[i];
                helpString.replace("_1_ica_by1", "_sum_ica");
                res = fileInnerClassification(dir->absolutePath(), helpString, "16sec19ch", 50, false, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " ica inner\t" << res << endl;
                res = fileInnerClassification(dir->absolutePath(), helpString, "4sec19ch", 50, true, 1000, 125);
                outStream << list1[i].left(3).toStdString() << " ica wnd inner\t" << res << endl;
                cout << list1[i].left(3).toStdString() << " ica finished\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
            }
            outStream.close();
        }
        list1 = dir->entryList(QStringList("???_1_ica_by1.edf"));
        //best 3 components
        if(1)
        {
            for(int i = 0; i < list1.length(); ++i)
            {
                myTime.restart();
                cout << list1[i].left(3).toStdString() << " addComps start from 3" << endl;
                helpString = list1[i];
                helpString.replace("_1", "_2");
                res = filesAddComponents(dir->absolutePath(), list1[i], helpString, 30, false);
                cout << list1[i].left(3).toStdString() << " addComps from 3 finished\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
            }
        }

    }
#endif
}
