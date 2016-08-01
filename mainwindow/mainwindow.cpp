#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace myLib;

MainWindow::MainWindow() :
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Main");
    setlocale(LC_NUMERIC, "C");

    autoProcessingFlag = false;

    coutBuf = cout.rdbuf();

    staSlice = 0;
    stopFlag = 0;

    def::dir->cd(QDir::root().absolutePath());

    QString helpString;

    // what with deletion?
    QButtonGroup * group1, *group2, *group3, *group4;
    group1 = new QButtonGroup();
    group1->addButton(ui->enRadio);
    group1->addButton(ui->ntRadio);
    group2 = new QButtonGroup();
    group2->addButton(ui->windButton);
    group2->addButton(ui->realButton);
    group2->addButton(ui->justSliceButton);
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

    ui->drawDirBox->addItem("SpectraSmooth");
    ui->drawDirBox->addItem("SpectraSmooth/windows");
    ui->drawDirBox->addItem("windows/SpectraSmooth"); /// tcpClient test
    ui->drawDirBox->addItem("windows/fromreal"); //generality

    /// set fileMarks lineEdit
    helpString.clear();
    for(const QString & fileMark : def::fileMarkers)
    {
        helpString += fileMark + "; ";
    }
    helpString.resize(helpString.size() - 2); // remove the last "; "
    ui->fileMarkersLineEdit->setText(helpString);


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

    ui->reduceChannelsComboBox->setCurrentText("MyCurrent");
//    ui->reduceChannelsComboBox->setCurrentText("20");
    ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(ui->reduceChannelsComboBox->currentIndex()).toString());


    /// seconds !!!!!
    ui->timeShiftSpinBox->setMinimum(0.1);
    ui->timeShiftSpinBox->setMaximum(32);
    ui->timeShiftSpinBox->setValue(0.5);
    ui->timeShiftSpinBox->setSingleStep(0.1);

    ui->windowLengthSpinBox->setMaximum(32);
    ui->windowLengthSpinBox->setMinimum(1);
    ui->windowLengthSpinBox->setSingleStep(0.1);
    ui->windowLengthSpinBox->setValue(4);
    ui->realButton->setChecked(true);

    if(def::OssadtchiFlag)
    {
        ui->timeShiftSpinBox->setValue(10);
        ui->windowLengthSpinBox->setValue(10);
    }

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
    ui->vectwDoubleSpinBox->setMaximum(15.0);
    ui->vectwDoubleSpinBox->setMinimum(1.5);
    ui->vectwDoubleSpinBox->setValue(12.0);
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

    ui->finishTimeBox->setMaximum(60 * 30.);
    ui->startTimeBox->setMaximum(ui->finishTimeBox->maximum());

    ui->BayesSpinBox->setMaximum(50);
    ui->BayesSpinBox->setMinimum(1);
    ui->BayesSpinBox->setValue(20);

    ui->cleanFromRealsCheckBox->setChecked(true);
    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(false);
    ui->cleanWindSpectraCheckBox->setChecked(true);
    ui->cleanMarkersCheckBox->setChecked(true);
    ui->cleanSignalsCheckBox->setChecked(true);
    ui->cleanSpectraImgCheckBox->setChecked(true);

    ui->highFreqFilterDoubleSpinBox->setValue(40.);
    ui->highFreqFilterDoubleSpinBox->setSingleStep(1.0);
    ui->lowFreqFilterDoubleSpinBox->setValue(3.5);
    ui->lowFreqFilterDoubleSpinBox->setSingleStep(0.1);

    ui->rereferenceDataComboBox->addItem("A1");
    ui->rereferenceDataComboBox->addItem("A2");
    ui->rereferenceDataComboBox->addItem("Ar");
    ui->rereferenceDataComboBox->addItem("N");
    ui->rereferenceDataComboBox->addItem("Base");
    ui->rereferenceDataComboBox->setCurrentText("Ar");

    ui->matiPieceLengthSpinBox->setMaximum(64);
    ui->matiPieceLengthSpinBox->setMinimum(4);
    ui->matiPieceLengthSpinBox->setValue(16);
    ui->matiCheckBox->setChecked(def::matiFlag);
    ui->markerBinTimeSpinBox->setMaximum(250*60*60*2);   //2 hours
    ui->markerSecTimeDoubleSpinBox->setMaximum(60*60*2); //2 hours

    ui->roundOffsetCheckBox->setChecked(true); // for mati constructEDF


    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(setEdfFileSlot()));

    QObject::connect(ui->Name, SIGNAL(returnPressed()), this, SLOT(setExpName()));

    QObject::connect(ui->cutEDF, SIGNAL(clicked()), this, SLOT(sliceAll()));

    QObject::connect(ui->matiPreprocessingPushButton, SIGNAL(clicked()), this, SLOT(matiPreprocessingSlot()));

    QObject::connect(ui->windFromRealButton, SIGNAL(clicked()), this, SLOT(sliceWindFromReal()));

    QObject::connect(ui->cut_e, SIGNAL(clicked()), this, SLOT(showCut()));

    QObject::connect(ui->netButton, SIGNAL(clicked()), this, SLOT(showNet()));

    QObject::connect(ui->countSpectra, SIGNAL(clicked()), this, SLOT(showCountSpectra()));

    QObject::connect(ui->eyesButton, SIGNAL(clicked()), this, SLOT(processEyes()));

    QObject::connect(ui->cleanEdfFromEyesButton, SIGNAL(clicked()),
                     this, SLOT(cleanEdfFromEyesSlot()));

    QObject::connect(ui->reduceChannesPushButton, SIGNAL(clicked()), this, SLOT(reduceChannelsSlot()));

    QObject::connect(ui->drawButton, SIGNAL(clicked()), this, SLOT(drawDirSlot()));

    QObject::connect(ui->cleanDirsButton, SIGNAL(clicked()), this, SLOT(cleanDirs()));

    QObject::connect(ui->reduceChannelsComboBox, SIGNAL(highlighted(int)), this, SLOT(changeNsLine(int)));

    QObject::connect(ui->reduceChannelsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeNsLine(int)));

    QObject::connect(ui->setNsLine, SIGNAL(returnPressed()), this, SLOT(setNs()));

    QObject::connect(ui->fileMarkersLineEdit, SIGNAL(returnPressed()), this, SLOT(setFileMarkers()));

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

    QObject::connect(ui->icaTestClassPushButton, SIGNAL(clicked()), this, SLOT(icaClassTest()));

    QObject::connect(ui->spocPushButton, SIGNAL(clicked()), this, SLOT(spoc()));

    QObject::connect(ui->hilbertPushButton, SIGNAL(clicked()), this, SLOT(hilbertCount()));

    QObject::connect(ui->throwICPushButton, SIGNAL(clicked()), this, SLOT(throwIC()));

    QObject::connect(ui->randomDecomposePushButton, SIGNAL(clicked()), this, SLOT(randomDecomposition()));

    QObject::connect(ui->refilterDataPushButton, SIGNAL(clicked()), this, SLOT(refilterDataSlot()));

    QObject::connect(ui->transformRealsPushButton, SIGNAL(clicked()), this, SLOT(transformReals()));

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

    globalEdf.setMatiFlag(ui->matiCheckBox->isChecked());
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
    if(event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

void MainWindow::showCountSpectra()
{
    Spectre *sp = new Spectre();
    sp->show();
}

void MainWindow::processEyes()
{
    const edfFile & fil = globalEdf;
    std::vector<int> eegs;
    std::vector<int> eogs;
    for(int i = 0; i < fil.getNs(); ++i)
    {
        const auto & labl = fil.getLabels()[i];
        if(labl.contains("EOG"))
        {
            eogs.push_back(i);
        }
        /// what with A1-A2 and A1-N ???
        else if(labl.contains("EEG"))
        {
            if(!labl.contains("A1-A2") &&
               !labl.contains("A2-A1") &&
               !labl.contains("A1-N" ) &&
               !labl.contains("A2-N"))
            {
                eegs.push_back(i);
            }
        }
    }
    /// or (eogs, eegs)
    eyesProcessingStatic(eogs); // for first 19 eeg channels
}

void MainWindow::showNet()
{
    Net * ANN = new Net();
    ANN->show();
}

void MainWindow::showCut()
{
    Cut *cut_e = new Cut();
    cut_e->show();
}

void MainWindow::setEdfFileSlot()
{
    QString helpString;

    if(def::dir->absolutePath() == QDir::root().absolutePath())
    {
        helpString = QFileDialog::getOpenFileName((QWidget*)this,
                                                  tr("EDF to open"),
                                                  def::dataFolder,
                                                  tr("EDF files (*.EDF *.edf)"));
    }
    else
    {
        helpString = QFileDialog::getOpenFileName((QWidget*)this,
                                                  tr("EDF to open"),
                                                  def::dir->absolutePath(),
                                                  tr("EDF files (*.EDF *.edf)"));
    }

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

    def::ExpName = getExpNameLib(filePath);
    ui->Name->setText(def::ExpName);

    helpString.resize(helpString.lastIndexOf(slash));
    def::dir->cd(helpString);

    if(def::redirectCoutFlag)
    {
        //redirect cout to logfile
        if(generalLogStream.is_open())
        {
            generalLogStream.close();
        }
        helpString += QString(slash) + "generalLog.txt";

        generalLogStream.open(helpString.toStdString().c_str(), ios_base::app);
        generalLogStream << endl << endl << endl;

        cout.rdbuf(generalLogStream.rdbuf());
    }
    else
    {
        cout.rdbuf(coutBuf);
    }


    def::dir->mkdir("Help");
    def::dir->mkdir("Help/maps");
    def::dir->mkdir("Help/psa");
    def::dir->mkdir("Help/ica");
    def::dir->mkdir("Help/wm");
    def::dir->mkdir("amod");
    def::dir->mkdir("auxEdfs");
    def::dir->mkdir("PA");

    def::dir->mkdir("Signals");
    def::dir->mkdir("Signals/before");
    def::dir->mkdir("Signals/after");
    def::dir->mkdir("Signals/other");
    def::dir->mkdir("Signals/windows");
    def::dir->mkdir("Signals/windows/after");
    def::dir->mkdir("Signals/windows/before");
    def::dir->mkdir("Signals/windows/other");
    def::dir->mkdir("SignalsCut");
    def::dir->mkdir("SignalsCut/before");
    def::dir->mkdir("SignalsCut/after");
    def::dir->mkdir("SpectraImg");
    def::dir->mkdir("SpectraSmooth");
    def::dir->mkdir("SpectraSmooth/windows");
    def::dir->mkdir("SpectraSmooth/Bayes");
    def::dir->mkdir("SpectraSmooth/PCA");
    def::dir->mkdir("SpectraSmooth/Clean");
    def::dir->mkdir("windows");
    def::dir->mkdir("windows/fromreal");
    def::dir->mkdir("Realisations");
    def::dir->mkdir("Realisations/BC");
    def::dir->mkdir("cut");
    def::dir->mkdir("visualisation");
    def::dir->mkdir("visualisation/video");
    def::dir->mkdir("visualisation/wavelets");

    for(int i = 0; i < 19; ++i)
    {
        helpString = "visualisation/wavelets/";
        def::dir->mkdir(helpString.append(QString::number(i)));
    }


    helpString = "EDF file read successfull";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(def::ns));
    ui->textEdit->append(helpString);

}

void MainWindow::setExpName()
{
    QString helpString;
    def::ExpName = ui->Name->text();
    helpString = "Name approved: " + def::ExpName + "\n";
    ui->textEdit->append(helpString);
}

void MainWindow::sliceAll() /////// aaaaaaaaaaaaaaaaaaaaaaaaaa//////////////////
{
    QTime myTime;
    myTime.start();

    readData();
    edfFile & fil = globalEdf;

    if(ui->matiCheckBox->isChecked())
    {
        if(ui->eyesCleanCheckBox->isChecked())
        {
            fil.cleanFromEyes();
            def::ns = fil.getNs();
        }
        if(ui->reduceChannelsCheckBox->isChecked())
        {
            vector<int> chanList;
            makeChanList(chanList);
            fil.reduceChannels(chanList);
            def::ns = fil.getNs();
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
            fil.cleanFromEyes();
            def::ns = fil.getNs();
        }
        if(ui->reduceChannelsCheckBox->isChecked())
        {
            // more general reduce channels
            vector<int> chanList;
            makeChanList(chanList);
            fil.reduceChannels(chanList);
            def::ns = fil.getNs();
        }
        if(ui->sliceCheckBox->isChecked())
        {
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
                    sliceOneByOneNew();
                    sliceWindFromReal();
                }
                else if(ui->justSliceButton->isChecked())
                {
                    QString helpString;
                    const double wndLen = ui->windowLengthSpinBox->value() * def::freq;

                    for(int i = 0;
                        i < min(ceil(fil.getData().cols() / wndLen), 60.); /// const generality
                        ++i)
                    {
                        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                              + slash + "windows"
                                                              + slash + "fromreal"
                                                              + slash + def::ExpName
                                                              + "-" + rightNumber(i, 4)
                                                              + "_" + QString::number(254)
//                                                              + ".edf"
                                                              );

                        fil.saveSubsection(i * wndLen,
                                           min((i + 1) * wndLen,
                                               double(fil.getData().cols())),
                                           helpString,
                                           true
//                                           false
                                           );

                    }
                }
                else if(ui->realButton->isChecked())
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
//                        sliceOneByOneNew(); /// by number after 241/247
                        sliceOneByOne();
//                        sliceFromTo(241, 231, "241_pre");
//                        sliceFromTo(247, 231, "247_pre");  //accord with presentation markers
//                        sliceFromTo(247, 237, "247_pre");
//                        helpString = def::dir->absolutePath() + slash + "Realisations";
//                        cleanDir(helpString, "_pre", false);
                    }
                }
            }
        }
        ui->progressBar->setValue(0);
    }

    QString helpString = "data sliced ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);

    cout << "sliceAll: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::readData()
{

    QTime myTime;
    myTime.start();

    QString helpString;
    helpString = QDir::toNativeSeparators( ui->filePathLineEdit->text() );
    if(!QFile::exists(helpString))
    {
        cout << "readData: edf file doent exist\n" << helpString.toStdString() << endl;
        return;
    }

    globalEdf.readEdfFile(helpString);
    def::ns = globalEdf.getNs();

    helpString = "data have been read ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to " + QString::number(def::ns);
    ui->textEdit->append(helpString);
    ui->markerSecTimeDoubleSpinBox->setMaximum(globalEdf.getDataLen() / def::freq);
    ui->markerBinTimeSpinBox->setMaximum(globalEdf.getDataLen());
}

void MainWindow::drawDirSlot()
{
    const QString deer = ui->drawDirBox->currentText();
    if(deer.contains("spectr", Qt::CaseInsensitive))
    {
        drawSpectraSlot();
    }
    else
    {
        drawRealisations();
    }
}


void MainWindow::drawSpectraSlot()
{
    const QString prePath = def::dir->absolutePath() + slash + ui->drawDirBox->currentText();
    const QString outPath = def::dir->absolutePath() + slash + "SpectraImg";
    drawSpectra(prePath, outPath);
}


void MainWindow::drawSpectra(const QString & prePath,
                             const QString & outPath)
{
    QTime myTime;
    myTime.start();

    QStringList lst;
    makeFullFileList(prePath, lst);
    lineType dataS;
    int i = 0;
    QString helpString;
    for(const QString & str : lst)
    {
        helpString = prePath + slash + str;
        readFileInLine(helpString,
                       dataS);

        helpString = outPath + slash + str + ".jpg";
        drawTemplate(helpString);
        drawArray(helpString, dataS);

        ui->progressBar->setValue(100 * (++i) / lst.length());
        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = false;
            break;
        }
    }
    ui->progressBar->setValue(0);
    cout << "drawSpectra: time = " << myTime.elapsed() / 1000. << " sec" << endl;
}

void MainWindow::drawRealisations()
{
    QStringList lst;
    QString helpString;

    QTime myTime;
    myTime.start();
    ui->progressBar->setValue(0);
    matrix dataD;

    QString prePath = def::dir->absolutePath() + slash + ui->drawDirBox->currentText();
    makeFullFileList(prePath, lst);

    int redCh = -1;
    int blueCh = -1;
    const edfFile & fil = globalEdf;

    if(def::ns == 41)
    {
        redCh = 21;
        blueCh = 22;
    }
    else if(def::ns == 22 || def::ns == 21)
    {
        redCh = 19;
        blueCh = 20;
    }
    else if(def::ns == 24)
    {
        redCh = 21;
        blueCh = 22;
    }
    else // not the best solution
    {
        for(int i = 0; i < fil.getNs(); ++i)
        {
            if(fil.getLabels()[i].contains("EOG1")) redCh = i;
            else if(fil.getLabels()[i].contains("EOG2")) blueCh = i;
        }
    }

    int NumOfSlices;
    for(int i = 0; i < lst.length(); ++i)
    {
        if(stopFlag)
        {
            break;
        }
        helpString = prePath + slash + lst[i];
        readPlainData(helpString,
                      dataD,
                      NumOfSlices);

        if(NumOfSlices > 15000)
        {
            continue;
        }

        helpString = getPicPath(helpString);

        drawEeg(dataD,
                def::ns,
                NumOfSlices,
                def::freq,
                helpString,
                ui->drawCoeffSpinBox->value(),
                blueCh,
                redCh); // generality freq

        ui->progressBar->setValue(100 * (i + 1) / lst.length());
        qApp->processEvents();
    }
    ui->progressBar->setValue(0);

    helpString = "signals are drawn ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to ";
    helpString.append(QString::number(def::ns));
    ui->textEdit->append(helpString);

    stopFlag = 0;
    cout << "drawRealisations: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::cleanDirs()
{
    QString helpString;

    if(def::dir->isRoot())
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No dir"), QMessageBox::Ok);
        return;
    }

    // windows
    if(ui->cleanWindowsCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                + slash + "windows";
        cleanDir(helpString);
    }

    // SpectraSmooth/windows
    if(ui->cleanWindSpectraCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                + slash + "SpectraSmooth"
                + slash + "windows";
        cleanDir(helpString);
    }

    // SpectraSmooth
    if(ui->cleanRealsSpectraCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                + slash + "SpectraSmooth";
        cleanDir(helpString);
    }

    // windows/fromreal
    if(ui->cleanFromRealsCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                + slash + "windows"
                + slash + "fromreal";
        cleanDir(helpString);
    }

    // Realisations
    if(ui->cleanRealisationsCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                + slash + "Realisations";
        cleanDir(helpString);
    }

    // SpectraImg
    if(ui->cleanSpectraImgCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                     + slash + "SpectraImg";
        cleanDir(helpString);
    }

    // signals
    if(ui->cleanSignalsCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                     + slash + "Signals" + slash;

        for(auto str2 : {"before", "after", "other"})
        {
            cleanDir(helpString + str2);
        }
    }



    // signals windows
    if(ui->cleanWindowsSignalsCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath()
                     + slash + "Signals" + slash + "windows" + slash;

        for(auto str2 : {"before", "after", "other"})
        {
            cleanDir(helpString + str2);
        }
    }

    // markers
    if(ui->cleanMarkersCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath();
        cleanDir(helpString, "markers", 0);
    }

    helpString = "dirs cleaned ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);
}



void MainWindow::markerSetSecTime(int a)
{
    ui->markerSecTimeDoubleSpinBox->setValue(double(a) / def::freq);
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
        byteMarker[16-1 - h] = (marker%(int(pow(2, h+1)))) / (int(pow(2,h)));
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
    helpString = def::dir->absolutePath() + slash + def::ExpName + ui->newEdfNameLineEdit->text();
    if(!helpString.endsWith(".edf", Qt::CaseInsensitive))
    {
        helpString += ".edf";
    }
    helpString = setFileName(helpString);
    globalEdf.writeEdfFile(helpString);
}

void MainWindow::drawMapsSlot()
{
    QString helpString = QFileDialog::getOpenFileName(this,
                                                      tr("Choose maps file"),
                                                      def::dir->absolutePath(),
                                                      tr("*.txt"));
    if(helpString.isEmpty())
    {
        helpString = def::dir->absolutePath()
                     + slash + "Help"
                     + slash + def::ExpName
                     + "_maps.txt";
        helpString.remove("_ica");
    }
    if(!QFile::exists(helpString))
    {
        return;
    }
    drawMapsICA(helpString,
                def::dir->absolutePath()
                + slash + "Help"
                + slash + "Maps",
                def::ExpName);
}

void MainWindow::avTime()
{
    const int maxLen = 32 * def::freq;
    int numNotSolved = 0;
    int shortReals = 0;

    std::valarray<double> means{};
    means.resize(4); /// 4 types of mean - with or w/o shorts and longs

    int num = 0;
    ifstream inStr;

    QStringList lst;
    QString helpString;

    QDir localDir;
    localDir.cd(def::dir->absolutePath() + slash + "Realisations");

    for(const QString & tmp : {"241", "247"})
    {
        means = 0.;
        shortReals = 0;
        numNotSolved = 0;

        lst = localDir.entryList({"*_" + tmp + "*"}, QDir::Files);
        for(const QString & fileName : lst)
        {
            helpString = localDir.absolutePath() + slash + fileName;

            inStr.open(helpString.toStdString());
            inStr.ignore(64, ' ');
            inStr >> num;
            inStr.close();

            means += num;

            if(num < 500)
            {
                means[1] -= num;
                means[3] -= num;
                ++shortReals;
            }
            else if(num > maxLen - 100)
            {
                means[2] -= num;
                means[3] -= num;
                ++numNotSolved;
            }
        }
        means[0] /= lst.length();
        means[1] /= (lst.length() - shortReals);
        means[2] /= (lst.length() - numNotSolved);
        means[3] /= (lst.length() - shortReals - numNotSolved);
        means /= def::freq;

        helpString = def::dir->absolutePath() + slash + "results.txt";
        ofstream res;
        res.open(helpString.toStdString(), ios_base::app);
        res << "Reals type\t" << tmp << ":\r\n";
        res << "shortReals\t" << shortReals << "\r\n";
        res << "longReals\t" << numNotSolved << "\r\n";
        res << "All together\t" << means[0] << "\r\n";
        res << "w/o shorts\t" << means[1] << "\r\n";
        res << "w/o longs\t" << means[2] << "\r\n";
        res << "w/o both\t" << means[3] << "\r\n\r\n";
        res.close();
    }
    cout << "avTime finished" << endl;
}


void MainWindow::setBoxMax(double a)
{
    ui->startTimeBox->setValue(min(a - 1., ui->startTimeBox->value()));
    ui->startTimeBox->setMaximum(a - 1.);
}


void MainWindow::stop()
{
    stopFlag = 1;
}

void MainWindow::changeNsLine(int a)
{
    ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(a).toString());
}

void MainWindow::setFileMarkers()
{
    def::fileMarkers = ui->fileMarkersLineEdit->text().split(QRegExp(R"([,;])"),
                                                             QString::SkipEmptyParts);
    ui->textEdit->append(R"(fileMarkers renewed)");
}

void MainWindow::setNs()
{
    QString helpString;
    def::ns = ui->setNsLine->text().toInt();

    helpString = "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}

void MainWindow::setNsSlot(int a)
{
    QString helpString;
    def::ns = a;

    helpString = "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}
