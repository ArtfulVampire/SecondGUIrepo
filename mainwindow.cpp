#define maxNs 45
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

    ns = defaults::genNs;
    right = numOfLim(defaults::rightFreq, defaults::frequency, defaults::fftLength);
    left = numOfLim(defaults::leftFreq, defaults::frequency, defaults::fftLength);

    spStep = defaults::frequency/defaults::fftLength;
    spLength = right - left + 1;



    withMarkersFlag = 0;
    ui->sliceWithMarkersCheckBox->setChecked(false);
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
    data = new double * [maxNs];
    for(int i = 0; i < maxNs; ++i)
    {
        data[i] = new double [200*60*250];           //////////////for 200 minutes//////////////        
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

    helpInt = 0;
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
    ui->reduceChannelsComboBox->addItem("20");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
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
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->highFreqFilterDoubleSpinBox->setValue(20.);
    ui->highFreqFilterDoubleSpinBox->setSingleStep(1.0);
    ui->lowFreqFilterDoubleSpinBox->setValue(5.0);
    ui->lowFreqFilterDoubleSpinBox->setSingleStep(0.1);

    ui->rereferenceDataComboBox->addItem("A1");
    ui->rereferenceDataComboBox->addItem("A2");
    ui->rereferenceDataComboBox->addItem("Ar");
    ui->rereferenceDataComboBox->addItem("N");




    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(setEdfFileSlot()));

    QObject::connect(ui->countSpectra, SIGNAL(clicked()), this, SLOT(countSpectraShow()));

    QObject::connect(ui->Name, SIGNAL(returnPressed()), this, SLOT(setExpName()));

    QObject::connect(ui->cutEDF, SIGNAL(clicked()), this, SLOT(sliceAll()));

    QObject::connect(ui->windFromRealButton, SIGNAL(clicked()), this, SLOT(sliceWindFromReal()));

    QObject::connect(ui->cut_e, SIGNAL(clicked()), this, SLOT(cutShow()));

    QObject::connect(ui->makeCFG, SIGNAL(clicked()), this, SLOT(makeCfgShow()));

    QObject::connect(ui->makePA, SIGNAL(clicked()), this, SLOT(makePaShow()));

    QObject::connect(ui->netButton, SIGNAL(clicked()), this, SLOT(netShow()));

    QObject::connect(ui->reduceChannesPushButton, SIGNAL(clicked()), this, SLOT(reduceChannelsSlot()));

    QObject::connect(ui->drawButton, SIGNAL(clicked()), this, SLOT(drawRealisations()));

    QObject::connect(ui->eyesButton, SIGNAL(clicked()), this, SLOT(eyesShow()));

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

    QObject::connect(ui->markerBin0LineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetDecValueSlot()));
    QObject::connect(ui->markerBin0LineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetDecValueSlot()));

    QObject::connect(ui->markerBin1LineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetDecValueSlot()));
    QObject::connect(ui->markerBin1LineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetDecValueSlot()));

    customFunc();
}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i = 0; i < maxNs; ++i)
    {
        delete []label[i];
        delete []data[i];
    }
    delete []data;
    delete []label;
    delete dir;
    delete paint;
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
//enable Escape key for all widgets
void QWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape) close();
}



void MainWindow::changeNsLine(int a)
{
    ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(a).toString());
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
        helpString = dir->absolutePath() + QDir::separator() + "windows";
        cleanDir(helpString);
    }

    //SpectraSmooth/windows
    if(ui->cleanWindSpectraCheckBox->isChecked())
    {
        helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth" + QDir::separator() + "windows";
        cleanDir(helpString);
    }


    //SpectraSmooth
    if(ui->cleanRealsSpectraCheckBox->isChecked())
    {
        helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth";
        cleanDir(helpString);
    }

    //windows/fromreal
    if(ui->cleanFromRealsCheckBox->isChecked())
    {
        helpString = dir->absolutePath() + QDir::separator() + "windows" + QDir::separator() + "fromreal";
        cleanDir(helpString);
    }

    //Realisations
    if(ui->cleanRealisationsCheckBox->isChecked())
    {
        helpString = dir->absolutePath() + QDir::separator() + "Realisations";
        cleanDir(helpString);
    }

    //Help
    if(ui->cleanHelpCheckBox->isChecked())
    {
        helpString = dir->absolutePath() + QDir::separator() + "Help";
        cleanDir(helpString);
    }

//    cout << "dirs cleaned" << endl;

    helpString="dirs cleaned ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
}

void MainWindow::drawEeg(int NumOfSlices_, double **dataD_, QString helpString_, int freq)
{
    QString helpString;
    int start = 0; // staSlice; //generality
    QPixmap pic = QPixmap(NumOfSlices_, 600);  //cut.cpp Cut::paint() generality
    pic.fill();
    QPainter * paint_ = new QPainter();
    paint_->begin(&pic);

    QString colour;

    double norm=ui->drawCoeffSpinBox->value();              //////////////////////

    for(int c1 = 0; c1 < pic.width(); ++c1)
    {
        for(int c2 = 0; c2 < ns; ++c2)
        {
            if(ns==21 && c2==19) colour = "red"; //paint_->setPen("red");
            else if(ns==21 && c2==20) colour = "blue"; //paint_->setPen("blue");
            else colour = "black";//paint_->setPen("black");
            paint_->setPen(QPen(QBrush(QColor(colour)), 2));

            paint_->drawLine(c1, (c2+1)*pic.height()/(ns+2) +dataD_[c2][c1+start]/norm, c1+1, (c2+1)*pic.height()/(ns+2) +dataD_[c2][c1+1+start]/norm);
        }
    }
    norm=1.;
//    paint_->setPen("black");
    paint_->setPen(QPen(QBrush("black"), 2));
    for(int c3 = 0; c3 < NumOfSlices_*10/250; ++c3)
    {
        if(c3%5  == 0) norm=15.;
        if(c3%10 == 0) norm=20.;

        paint_->drawLine(c3*freq/5, pic.height()-2, c3*freq/5, pic.height()-2*norm);
        int helpInt=c3;
        helpString=QString::number(helpInt);
        paint_->drawText(c3*freq, pic.height()-35, helpString);
        norm=10.;
    }

    norm=1;
    pic.save(helpString_, 0, 100);
    paint_->end();
    delete paint_;

}

void MainWindow::drawEeg(double **dataD_, double startTime, double endTime, QString helpString_, int freq)
{
    QString helpString;
    double stretch = ceil(freq/512);
    QPixmap pic = QPixmap((endTime-startTime)*freq/stretch, 600);  //cut.cpp 851
    pic.fill();
    QPainter * paint_ = new QPainter();
    paint_->begin(&pic);

    double norm=ui->drawCoeffSpinBox->value();              //////////////////////


    for(int c1 = 0; c1 < pic.width(); ++c1)
    {
        for(int c2 = 0; c2 < ns; ++c2)
        {
            if(ns==21 && c2==19) paint_->setPen("red");
            else
            {
                if(ns==21 && c2==20) paint_->setPen("blue");
                else paint_->setPen("black");
            }
            paint_->drawLine(c1, (c2+1)*pic.height()/(ns+2) +dataD_[c2][int(startTime*freq+c1*stretch)]/norm, c1+1, (c2+1)*pic.height()/(ns+2) +dataD_[c2][int(startTime*freq+(c1+1)*stretch)]/norm);
        }
    }
    paint_->setPen("black");
    norm=1.;
    for(int c3 = 0; c3 < 25*5+10; ++c3)
    {
        if(c3%5== 0) norm=15.;
        if(c3%10== 0) norm=20.;

        paint_->drawLine(c3*freq/5, pic.height()-2, c3*freq/5, pic.height()-2*norm);
        int helpInt=c3;
        helpString=QString::number(helpInt);
        paint_->drawText(c3*freq, pic.height()-35, helpString);
        norm=10.;
    }

    norm=1;

    pic.save(helpString_, 0, 100);
    paint_->end();
    delete paint_;

}

void MainWindow::drawRealisations()
{
    QStringList lst;
    QStringList nameFilters;
    QString helpString;

    QTime myTime;
    myTime.start();
    ui->progressBar->setValue(0);
    double **dataD = new double * [ns];
    int Eyes;

    dir->cd(ui->drawDirBox->currentText());    //->windows or Realisations or cut
    lst.clear();
    nameFilters.clear();
    nameFilters.append("*_241*");
    nameFilters.append("*_247*");
    nameFilters.append("*_254*");
    nameFilters.append("*_244*");
    //for Roma's files
    nameFilters.append("*_2");
    nameFilters.append("*_1");
//    lst = dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot);

    lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
    cout << "lst.len = " << lst.length() << endl;
    FILE * file;
    for(int i = 0; i < lst.length(); ++i)
    {
        if(stopFlag) break;
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i));
        file=fopen(helpString.toStdString().c_str(), "r");
        fscanf(file, "%*s %d \n", &NumOfSlices);
        if(NumOfSlices > 15000)
        {
            fclose(file);
            continue;
        }
        fscanf(file, "Eyes %d \n", &Eyes);
        for(int j = 0; j < ns; ++j)
        {
            dataD[j] = new double [NumOfSlices]; /////////generality for 32 seconds
        }

        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                fscanf(file, "%lf", &dataD[k][j]);
            }
        }
        helpString=lst.at(i);
        helpString.replace('.', '_');

        dir->cdUp();  //-> into EDF-file dir
        if(ui->drawDirBox->currentText() == "windows/fromreal") dir->cdUp();

        if(ui->drawDirBox->currentText().contains("Realisations"))
        {
            switch(ns)
            {
            case 19:
                {
                    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("after").append(QDir::separator()).append(helpString).append(".jpg");
                    break;
                }
            case 21:
                {
                    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("before").append(QDir::separator()).append(helpString).append(".jpg");
                    break;
                }
            default:
                {
                    helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("other").append(QDir::separator()).append(helpString).append(".jpg");
                    break;
                }
            }

        }

        else if(ui->drawDirBox->currentText().contains("cut"))
        {
            if(ns==19)  //add eyesCleanFlag?
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("after").append(QDir::separator()).append(helpString).append(".jpg");
            }
            else if(ns==21)
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("before").append(QDir::separator()).append(helpString).append(".jpg");
            }
            else
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("other").append(QDir::separator()).append(helpString).append(".jpg");
            }
        }

        else //windows
        {
            if(ns==19)  //add eyesCleanFlag?
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("windows").append(QDir::separator()).append("after").append(QDir::separator()).append(helpString).append(".jpg");
            }
            else if(ns==21)
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("windows").append(QDir::separator()).append("before").append(QDir::separator()).append(helpString).append(".jpg");
            }
            else
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("windows").append(QDir::separator()).append("other").append(QDir::separator()).append(helpString).append(".jpg");
            }
        }
        dir->cd(ui->drawDirBox->currentText());  //back to the windows or Realisations dir

        drawEeg(NumOfSlices, dataD, helpString, 250);
        fclose(file);
        for(int j = 0; j < ns; ++j)
        {
            delete []dataD[j];
        }
        if(int(100*(i+1)/lst.length()) > ui->progressBar->value()) ui->progressBar->setValue(int(100*(i+1)/lst.length()));
        qApp->processEvents();
    }

    delete []dataD;
    dir->cdUp();   //-> into EDF-file dir
    if(ui->drawDirBox->currentText() == "windows/fromreal") dir->cdUp();
    cout << dir->absolutePath().toStdString() << endl;

    helpString="signals are drawn ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    ui->progressBar->setValue(0);

    cout << "DrawRealisations: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;

    helpString.setNum(myTime.elapsed()/1000.);
    helpString.prepend("Signals drawn \nTime = ");
    helpString.append(" sec");
    stopFlag = 0;
//    QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);

    helpString = "DrawReals: time elapsed " + QString::number(myTime.elapsed()/1000.) + " sec";
    cout << helpString.toStdString() << endl;
}

void MainWindow::diffSmooth()
{
    Spectre *sp;
    Net * ANN;
    QTime myTime;
    myTime.start();
    for(int i = 10; i <= 100; i += 10)
    {
        sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
        sp->setSmooth(i);
        sp->countSpectra();
        sp->close();
        delete sp;

        ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(true);
        ANN->loadCfg();
        ANN->autoClassification("SpectraSmooth");
        ANN->close();
        cout << "smooth = " << i << " done" << endl;
        delete ANN;
        sleep(60);

    }
    cout << "diffSmooth: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;

}


void MainWindow::diffPow()
{
    QString helpString;
    Spectre *sp;
    Net * ANN;
    MakePa * mkPa;
    QTime myTime;
    FILE * outFile;
    myTime.start();
    helpString = dir->absolutePath() + QDir::separator() + "diffPow.txt";
//    for(double i = 0.45; i >= 0.45; i -= 0.0)
        while(1)
    {
        double i = 0.45;
        sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
        sp->setPow(i);
        sp->countSpectra();
        sp->close();
        delete sp;

        ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(1);


        //set appropriate coeff

        helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth";
        mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
        mkPa->setRdcCoeff(4);
        while(1)
        {
            cout << "reduce coeff = " << mkPa->getRdcCoeff() << endl;
            mkPa->makePaSlot();
            ANN->PaIntoMatrixByName("1");
            ANN->LearnNet();
            if(ANN->getEpoch() > 140 || ANN->getEpoch() < 60)
            {
                mkPa->setRdcCoeff(mkPa->getRdcCoeff() / sqrt(ANN->getEpoch() / 100.) );
            }
            else
            {
                reduceCoefficient = mkPa->getRdcCoeff();
                cout << "file = " << ExpName.toStdString() << "\t" << "reduceCoeff = " << reduceCoefficient << endl;
                ANN->setReduceCoeff(reduceCoefficient);
                break;
            }
        }
        mkPa->close();
        delete mkPa;

        helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth";
        ANN->autoClassification(helpString);
        ANN->close();

        helpString = dir->absolutePath() + QDir::separator() + "diffPow.txt";
        outFile = fopen(helpString.toStdString().c_str(), "a");
        fprintf(outFile, "pow = %.2lf\t%.2lf\n", i, ANN->getAverageAccuracy());
        fclose(outFile);
        delete ANN;

    }
    cout << "diffPow: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;

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




void MainWindow::countSpectraShow()
{
    Spectre *sp = new Spectre(dir, ns, ExpName);
    QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
    sp->show();
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


void MainWindow::Bayes()
{
    QStringList lst;
    QString helpString;

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files);
    dir->cdUp();

    FILE * file;
    double ** dataBayes = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataBayes[i] = new double [250*60*5];
    }
    double maxAmpl = 10.; //generality from readData

    maxAmpl += 0.001; //bicycle

    int numOfIntervals = ui->BayesSpinBox->value();
    numOfIntervals *= 2; //odd or even?

    spLength = numOfIntervals;
    left = 1;
    right = spLength;

    int * count = new int [numOfIntervals];

    double helpDouble = 0.;

    for(int i = 0; i < lst.length(); ++i)
    {
        if(lst[i].contains("num")) continue;
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Realisations" + QDir::separator() + lst[i]);
        file = fopen(helpString.toStdString().c_str(), "r");
        fscanf(file, "NumOfSlices %d\n", &NumOfSlices);
        if(NumOfSlices < 250)
        {
            fclose(file);
            continue;
        }

        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                fscanf(file, "%lf\n", &dataBayes[k][j]);
            }
        }
        fclose(file);

        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth" + QDir::separator() + "Bayes" + QDir::separator() + lst[i]);
        file = fopen(helpString.toStdString().c_str(), "w");
        for(int l = 0; l < ns; ++l)
        {
            if(ui->BayesRadioButton->isChecked())
            {
                //bayes itself;
                for(int k = 0; k < numOfIntervals; ++k)
                {
                    count[k] = 0;
                }
                for(int j = 0; j < NumOfSlices; ++j)
                {
                    helpInt = int(floor((dataBayes[l][j] + maxAmpl) / (2.*maxAmpl/double(numOfIntervals))));

                    if(helpInt != min(max(0, helpInt), numOfIntervals-1)) continue; // out of range

                    count[helpInt] += 1;
                }
                for(int k = 0; k < numOfIntervals; ++k)
                {
                    helpDouble = count[k]/double(NumOfSlices); // norm coeff
                    fprintf(file, "%lf\n", helpDouble);
                }
            }

            if(ui->HiguchiRadioButton->isChecked())
            {
                //fractal dimension
                helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("Fractals").append(QDir::separator()).append(lst[i]).append("_").append(QString::number(l)).append(".png"));
                helpDouble = fractalDimension(dataBayes[l], NumOfSlices, helpString);
                fprintf(file, "%.3lf\n", helpDouble);
            }
        }
        fclose(file);
    }
    delete [] count;
    for(int i = 0; i < ns; ++i)
    {
        delete [] dataBayes[i];
    }
    delete [] dataBayes;
}


void MainWindow::eyesShow()
{
    Eyes *trololo = new Eyes(dir, ns);
    trololo->setAutoProcessingFlag(false);
    trololo->show();
    QObject::connect(trololo, SIGNAL(setNsMain(int)), this, SLOT(setNsSlot(int)));
}

void MainWindow::netShow()
{
    Net * ANN = new Net(dir, ns, left, right, spStep, ExpName);
    ANN->show();
}

void MainWindow::cutShow()
{
    Cut *cut_e = new Cut(dir, ns, withMarkersFlag);
    cut_e->show();
}

void MainWindow::makePaShow() //250 - frequency generality
{
    QString helpString;
    if(spStep == defaults::frequency/4096. || spStep == defaults::frequency/2048.)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth");
    }
    else if(spStep == defaults::frequency/1024.)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth" + QDir::separator() + "windows");
    }
    else
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath());
    }


    MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep, QVector<int>());
    mkPa->show();
}

void MainWindow::makeCfgShow()//250 - frequency generality
{
    QString helpString;
    if(spStep == defaults::frequency/4096.) helpString = "16sec19ch";
    else if(spStep == defaults::frequency/1024.) helpString = "4sec19ch";
    else if(spStep == defaults::frequency/2048.) helpString = "8sec19ch";
    else helpString = "netFile";
    cfg *config = new cfg(dir, ns, spLength, 0.10, 0.10, helpString);
    config->show();
}

void MainWindow::setEdfFileSlot()
{
    QString helpString;
    NumOfEdf = 0;
    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)this, tr("EDF to open"), defaults::dataFolder, tr("EDF files (*.EDF *.edf)")));
    if(helpString.isEmpty())
    {
        QMessageBox::warning((QWidget*)this, tr("Warning"), tr("no EDF file was chosen"), QMessageBox::Ok);
        return;
    }
    setEdfFile(helpString);
}

void MainWindow::setEdfFile(QString const filePath)
{
    QString helpString2;
    QString helpString;
    NumOfEdf = 0;
    helpString = filePath;




    if(!helpString.endsWith(".edf", Qt::CaseInsensitive))
    {
        helpString += ".edf";
    }
    if(!QFile::exists(helpString))
    {
        helpString2 = "Cannot open EDF file:\n" + helpString;
//        QMessageBox::critical((QWidget*)this, tr("Critical"), helpString2, QMessageBox::Ok);
        cout << helpString2.toStdString() << endl;
        return;
    }

    ui->filePathLineEdit->setText(QDir::toNativeSeparators(helpString));


    //set ExpName

    ExpName = expName(filePath);
    ui->Name->setText(ExpName);

    helpString.resize(helpString.lastIndexOf(QDir::separator()));
    dir->cd(helpString);

    if(redirectCoutFlag)
    {
        //redirect cout to logfile
        if(generalLogStream.is_open())
        {
            generalLogStream.close();
        }
        helpString += QString(QDir::separator()) + "generalLog.txt";
        generalLogStream.open(helpString.toStdString().c_str(), ios_base::app);
        generalLogStream << endl << endl << endl;
        cout.rdbuf(generalLogStream.rdbuf());
    }
    else
    {
        cout.rdbuf(coutBuf);
    }


    dir->mkdir("Help");
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


void MainWindow::makeDatFile()
{
    QString helpString;
    readData();
    if(ui->eyesCleanCheckBox->isChecked()) eyesFast();
    if(ui->reduceChannelsCheckBox->isChecked()) reduceChannelsFast();

    double startTime=ui->startTimeBox->value();
    cout << "startTime = " << startTime << endl;
    double finishTime=ui->finishTimeBox->value();
    cout << "finishTime = " << finishTime << endl;

    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(ExpName).append(".dat"));

    cout << "ns in dat-file=" << ns << endl;
    FILE * datFile = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(),"w");
    if(datFile==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("critical"), tr("cannot open datFile"), QMessageBox::Ok);
        return;
    }

    fprintf(datFile, "NumOfSlices %d\n", int((finishTime - startTime)*nr[0]));

    for(int i = int(startTime * defaults::frequency); i < int(finishTime * defaults::frequency); ++i) //generality 250
    {
        for(int j = 0; j < ns; ++j)
        {
            fprintf(datFile, "%lf\n", data[j][i]);
        }
    }
    fclose(datFile);


}

void MainWindow::avTime()
{
//    return;/
    /*
    QStringList names = QStringList("AMA");
    names.append("CAA");
    names.append("SMM");
    names.append("VMV");
    names.append("PMI");
    names.append("BEA");
    names.append("RMS");
    names.append("GAS");
    names.append("AAU");
    names.append("BED");
    names.append("SMS");
    helpString = ExpName;
    helpString.resize(3);
    if(names.contains(helpString))
    {
        if(ExpName.contains("_FB") || ExpName.contains("_2"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0007");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else if(ExpName.contains("_3"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0026");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0003");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
    }

    if(helpString == "PTS")
    {
        if(ExpName.contains("_FB") || ExpName.contains("_2"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0048");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else if(ExpName.contains("_3"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0029");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0047");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
    }

    if(helpString == "REV")
    {
        if(ExpName.contains("_FB") || ExpName.contains("_2"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0024");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else if(ExpName.contains("_3"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0020");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else
        {
            //            dir->cd("Realisations");
            //            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0000");
            //            if(remove(helpString.toStdString().c_str()) != 0)
            //            {
            //                perror("cannot delete file");
            //            }
            //            dir->cdUp();
        }
    }

    if(helpString == "PKM")
    {
        if(ExpName.contains("_FB") || ExpName.contains("_2")) //no file
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0049");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else if(ExpName.contains("_3"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0029");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0003");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
    }

    if(helpString == "SUA")
    {
        if(ExpName.contains("_FB") || ExpName.contains("_2"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0049");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else if(ExpName.contains("_3"))
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0035");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
        else
        {
            dir->cd("Realisations");
            helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_241.0003");
            if(remove(helpString.toStdString().c_str()) != 0)
            {
                perror("cannot delete file");
            }
            dir->cdUp();
        }
    }

*/


    int maxLen = 10000;
    int numNotSolved = 0;
    int shortReals = 0;
    double av = 0.;
    double solveTime;
    int num;
    FILE * fil;

    QStringList lst;
    QString helpString;


    dir->cd("Realisations");
    lst = dir->entryList(QStringList("*_241*"), QDir::Files);
    for(int i = 0; i <  lst.length(); ++i)
    {
        helpString = dir->absolutePath().append(QDir::separator()).append(lst[i]);
        fil = fopen(helpString.toStdString().c_str(), "r");
        fscanf(fil, "NumOfSlices %d", &num);
        av += num;

        if(num < 750) ++shortReals;
        if(fabs(maxLen/double(num)-1.) < 0.01) ++numNotSolved;
        else cout << num/defaults::frequency << endl;
        fclose(fil);

    }
    dir->cdUp();

    solveTime = av / (defaults::frequency*lst.length());
    cout << "solveTime 241 = " << solveTime << endl << endl;
    cout << "num not solved 241 = " << numNotSolved << endl << endl;

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "solve time 241 \t %.1lf \n", solveTime);
    fprintf(res, "num not solved 241 \t %d \n", numNotSolved);
    fprintf(res, "short realisations 241 \t %d \n", shortReals);
    fclose(res);


    av = 0.;
    shortReals = 0;
    numNotSolved = 0;


    dir->cd("Realisations");
    lst = dir->entryList(QStringList("*_247*"), QDir::Files);
    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = dir->absolutePath().append(QDir::separator()).append(lst[i]);
        fil = fopen(helpString.toStdString().c_str(), "r");
        fscanf(fil, "NumOfSlices %d", &num);

        av += num;

        if(num < 750) ++shortReals;
        if(fabs(maxLen/double(num)-1.) < 0.01) ++numNotSolved;
        fclose(fil);
    }
    dir->cdUp();

    solveTime = av / (defaults::frequency*lst.length());
    cout << "solveTime 247 = " << solveTime << endl << endl;
    cout << "num not solved 247 = " << numNotSolved << endl << endl;

    res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "solve time 247 \t %.1lf \n", solveTime);
    fprintf(res, "num not solved 247 \t %d \n", numNotSolved);
    fprintf(res, "short realisations 247 \t %d \n", shortReals);
    fclose(res);

}

void MainWindow::sliceWindFromReal()
{

    QStringList lst;
    QStringList nameFilters;
    QString helpString;
    int eyesCounter;
    QTime myTime;
    myTime.start();
    FILE *real;
    FILE *out;
    dir->cd("Realisations");
    lst.clear();
    nameFilters.clear();
    nameFilters.append("*_241*");
    nameFilters.append("*_247*");
    nameFilters.append("*_254*");
    lst = dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot);
//    cout << "lst.len = " << lst.length() << endl;
    dir->cdUp();


    timeShift=ui->timeShiftSpinBox->value();
    wndLength = ui->windowLengthSpinBox->value();

//    cout << "timeShift = " << timeShift << endl;
//    cout << "wndLength = " << wndLength << endl;

    double ** dataReal = new double *[ns];
    for(int i = 0; i < ns; ++i)
    {
        dataReal[i] = new double [250*60*1]; //generality 5 min
    }

    for(int i = 0; i < lst.length(); ++i)
    {
        offset = 0;
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator()).append(lst.at(i)));
//        cout << "current file " << helpString.toStdString() << endl;
        real = fopen(helpString.toStdString().c_str(), "r");
        if(real == NULL)
        {
            QMessageBox::warning((QWidget*)this, tr("Warning"), tr("Cannot open file"), QMessageBox::Ok);
            return;
        }
        fscanf(real, "NumOfSlices %d\n", &NumOfSlices);
//        fscanf(real, "NumOfSlicesEyesCut %d\n", &eyes);
//        cout << "NumOfSlices = " << NumOfSlices << " ns = " << ns << endl;
        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                fscanf(real, "%lf", &dataReal[k][j]);
            }
        }
//        cout << "data read" << endl;


        for(int h = 0; h < ceil((NumOfSlices - wndLength)/double(timeShift)); ++h)
        {
            if(helpString.contains("241"))
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("windows").append(QDir::separator()).append("fromreal").append(QDir::separator()).append(ExpName).append("_241_").append(QString::number(i)).append(".").append(QString::number(h)));
            }
            if(helpString.contains("247"))
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("windows").append(QDir::separator()).append("fromreal").append(QDir::separator()).append(ExpName).append("_247_").append(QString::number(i)).append(".").append(QString::number(h)));
            }
            if(helpString.contains("254"))
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("windows").append(QDir::separator()).append("fromreal").append(QDir::separator()).append(ExpName).append("_254_").append(QString::number(i)).append(".").append(QString::number(h)));
            }

//            cout << helpString.toStdString() << endl;

            out = fopen(helpString.toStdString().c_str(), "w");
            if(out == NULL)
            {
                QMessageBox::warning((QWidget*)this, tr("Warning"), tr("Cannot open file"), QMessageBox::Ok);
                return;
            }



            //correct eyes number
            eyes = 0;
            for(int l = offset; l < (offset + wndLength); ++l)
            {
                eyesCounter = 0.;
                for(int m = 0; m < ns; ++m)
                {
                    if(dataReal[m][l]== 0.)  //generality different nr
                    eyesCounter += 1;
                }
                if(eyesCounter>=(ns-1)) eyes += 1;
            }

            if(eyes/double(wndLength)>0.3)  //generality
            {
                fclose(out);

                if(remove(helpString.toStdString().c_str()) != 0)
                {
                    perror("cannot delete file");
                }

                continue;
            }

            fprintf(out, "NumOfSlices %d \n", wndLength);
//            fprintf(out, "NumOfSlicesEyesCut %d \n", eyes);

            for(int l = offset; l < (offset + wndLength); ++l)
            {
                for(int m = 0; m < ns; ++m)
                {
                    fprintf(out, "%lf\n", dataReal[m][l]);  //generality different nr
                }
            }
            fclose(out);
            offset += timeShift;
        }
        fclose(real);
        ui->progressBar->setValue(i*100/lst.length());
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []dataReal[i];
    }
    delete []dataReal;
    ui->progressBar->setValue(0);



    helpString="windows from realisations sliced ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    cout << "WindFromReals: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;

    helpString.setNum(myTime.elapsed()/1000.);
    helpString.prepend("Data sliced \nTime = ");
    helpString.append(" sec");
    //automatization
    if(autoProcessingFlag == false)
    {
        QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
    }

}

void MainWindow::makeTestData()
{
    QString helpString;
    readData();

    nsBackup = ns;
    int indepNum = ui->numComponentsSpinBox->value();
    double ** testSignals = new double * [indepNum];
    for(int i = 0; i < indepNum; ++i)
    {
        testSignals[i] = new double [ndr*nr[i]];
    }

    double ** testSignals2 = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        testSignals2[i] = new double [ndr*nr[i]];
    }


    double x,y;
    srand(time(NULL));
    //signals

    for(int j = 0; j < ui->numComponentsSpinBox->value(); ++j)
    {
        x = (rand()%30)/40.;
        y = (-0.3 + (rand()%600)/100.);
        for(int i = 0; i < ndr*nr[0]; ++i)
        {
            helpDouble = 2.*3.1415926*double(i)/defaults::frequency * (10.1 + x) + y;
            testSignals[j][i] = sin(helpDouble);
        }
    }
//        helpDouble = 2.*3.1415926*double(i)/defaults::frequency * 10.3;
//        testSignals[1][i] = sin(helpDouble);//+ 0.17); //10.5 Hz
//        helpDouble = 2.*3.1415926*double(i)/defaults::frequency * 10.25;
//        testSignals[2][i] = sin(helpDouble);//- 0.17); //10.5 Hz
//        helpDouble = 2.*3.1415926*double(i)/defaults::frequency * 10.0;
//        testSignals[3][i] = sin(helpDouble);//- 0.06); //10.5 Hz
//        testSignals[1][i] = i%41 - 20.;      //a saw 40 period
//        testSignals[2][i] = sin(2*3.1415926*(double(i)/23.) + 0.175);//

//        x = (1 + rand()%10000)/10001.;
//        y = (1 + rand()%10000)/10001.;
//        testSignals[2][i] = sqrt(-2. * log(x)) * sin(2. * M_PI * y);

//        x = (1 + rand()%10000)/10001.;
//        y = (1 + rand()%10000)/10001.;
//        testSignals[3][i] = sqrt(-2. * log(x)) * cos(2. * M_PI * y);
//        testSignals[3][i] = ((i%34 >13) - 0.5); //rectangle


//        testSignals[2][i] = fabs(i%55 - 27) - 27./2.; //triangle

    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "spocVar.txt");
    FILE * in = fopen(helpString.toStdString().c_str(), "w");
    //modulation

    for(int j = 0; j < ui->numComponentsSpinBox->value()-1; ++j)
    {
        helpDouble = 0.05 + (rand()%100)/500.;
        x = (rand()%100)/100.;
        y = 1.5 + (rand()%20)/10.;
        for(int i = 0; i < ndr*nr[0]; ++i)
        {
//            testSignals[j][i] *= sin(2*3.1415926*i/defaults::frequency * helpDouble + x) + y;
        }
    }
    //object signal
    for(int i = 0; i < ndr*nr[0]; ++i)
    {
        helpDouble = sin(2.*3.1415926*int(i/250) * 0.02 - 0.138) + 1.8;
        testSignals[ui->numComponentsSpinBox->value() - 1][i] *= helpDouble;
        if(i%250 == 0)
        {
            fprintf(in, "%lf\n", helpDouble);
        }
    }
    fclose(in);

    double sum1, sum2;
    //normalize by dispersion = 10
    double coeff = 10.;
    for(int i = 0; i < indepNum; ++i)
    {
        sum1 = mean(testSignals[i], ndr*nr[0]);
        sum2 = variance(testSignals[i], ndr*nr[0]);

        for(int j = 0; j < ndr*nr[0]; ++j)
        {
            testSignals[i][j] -= sum1;
            testSignals[i][j] /= sqrt(sum2);
            testSignals[i][j] *= coeff;
        }
    }




    spocMixMatrix = new double * [ui->numComponentsSpinBox->value()];
    for(int k = 0; k < ui->numComponentsSpinBox->value(); ++k)
    {
        spocMixMatrix[k] = new double [ui->numComponentsSpinBox->value()];
    }
    for(int j = 0; j < 19; ++j)
    {
        for(int i = 0; i < ndr*nr[0]; ++i)
        {
            testSignals2[j][i] = 0.;
        }
        for(int k = 0; k < ui->numComponentsSpinBox->value(); ++k)
        {
            helpDouble = (-0.5 + (rand()%21)/20.);

            for(int i = 0; i < ndr*nr[0]; ++i)
            {
                testSignals2[j][i] += helpDouble * testSignals[k][i];
//                testSignals2[j][i] += (j==k) * testSignals[k][i];
            }
            if(j < ui->numComponentsSpinBox->value())
            {
                cout << helpDouble << "\t";
                spocMixMatrix[j][k] = helpDouble;
            }
        }
        if(j < ui->numComponentsSpinBox->value()) cout << endl;
    }
    cout << endl;


    cout << "1" << endl;
//    helpString = ExpName; helpString.append("_test.edf");
    helpString = "SDA_test.edf";
//    writeEdf(ui->filePathLineEdit->text(), testSignals2, helpString, ndr*nr[0]);



    for(int i = 0; i < indepNum; ++i)
    {
        delete []testSignals[i];
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []testSignals2[i];
    }
    delete []testSignals2;
    delete []testSignals;


}


void MainWindow::rereferenceDataSlot()
{
    QString helpString = dir->absolutePath() + QDir::separator() + ExpName + ".edf"; //ui->filePathLineEdit->text()
    helpString.replace(".edf", "_rr.edf");
    rereferenceData(ui->rereferenceDataComboBox->currentText(), helpString);

}

void MainWindow::rereferenceData(QString newRef, QString newPath)
{
    //A1, A2, Ar, Cz, N
    //A1-A2, A1-N
    // Ar means -0.5(A1+A2)

    QTime myTime;
    myTime.start();

    QStringList lst;
    QString helpString;
    QString helpString2;

    readData();
    helpString.clear();
    for(int i = 0; i < ns; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
//    cout << helpString.toStdString() << endl;
    ui->reduceChannelsLineEdit->setText(helpString);



    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
    {
        cout << "refilterData: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }
    int fr = nr[0];

    int groundChan = -1; //A1-N
    int earsChan = -1; //A1-A2
    for(int i = 0; i < ns; ++i)
    {
        if(QString(label[i]).contains("A1-N"))
        {
            groundChan = i;
        }
        else if(QString(label[i]).contains("A1-A2"))
        {
            earsChan = i;
        }
    }
    if((groundChan+1) * (earsChan+1) == 0)
    {
        cout << "some of reref channels is absent" << endl;
        return;
    }


    helpString.clear();
    if(newRef == "A1")
    {
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-A1")) //generality
            {
                if(QString(label[i]).contains("-A2"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1);
                }
                else if(QString(label[i]).contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1);
                }
                else if(QString(label[i]).contains("-Ar"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "/2";
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else //EOG and a shet
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    else if(newRef == "A2")
    {
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-A2")) //generality
            {
                if(QString(label[i]).contains("-A1"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(earsChan+1);
                }
                else if(QString(label[i]).contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1) + "+" + QString::number(earsChan+1);
                }
                else if(QString(label[i]).contains("-Ar"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(earsChan+1) + "/2";
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    else if(newRef == "N")
    {
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-N")) //generality
            {
                if(QString(label[i]).contains("-A1"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(groundChan+1);
                }
                else if(QString(label[i]).contains("-A2"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "+" + QString::number(groundChan+1);
                }
                else if(QString(label[i]).contains("-Ar"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "/2" + "+" + QString::number(groundChan+1);
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    else if(newRef == "Ar")
    {
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-Ar")) //generality
            {
                if(QString(label[i]).contains("-A1"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(earsChan+1) + "/2";
                }
                else if(QString(label[i]).contains("-A2"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "/2";
                }
                else if(QString(label[i]).contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1) + "+" + QString::number(earsChan+1) + "/2";
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    cout << helpString.toStdString() << endl;
    ui->reduceChannelsLineEdit->setText(helpString);

    //change labels
    for(int i = 0; i < ns; ++i)
    {
        helpString = QString(label[lst[i].toInt()-1]);
        if(helpString.contains('-') && (i != groundChan && i != earsChan))
        {
            helpString2 = helpString;
            helpString2.remove(0, helpString.indexOf('-')+1);
            helpString2.remove(helpString2.indexOf(' '), helpString2.length());
            helpString.replace(helpString2, newRef);
        }
        strcpy(label[i], helpString.toStdString().c_str());
        cout << "reref Data: label[" << i << "]= " << label[i] << endl;
    }
    reduceChannelsFast();

    //set all of channels to the lineedit
    helpString.clear();
    for(int i = 0; i < ns; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);

    writeEdf(ui->filePathLineEdit->text(), data, newPath, ndr*fr);

    cout << "Reref Data: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::refilterDataSlot()
{
    double lowFreq = ui->lowFreqFilterDoubleSpinBox->value();
    double highFreq = ui->highFreqFilterDoubleSpinBox->value();
    QString helpString = dir->absolutePath() + QDir::separator() + ExpName + ".edf"; //ui->filePathLineEdit->text()
    helpString.replace(".edf", "_f.edf");
    refilterData(lowFreq, highFreq, helpString);

}

void MainWindow::refilterData(double lowFreq, double highFreq, QString newPath)
{
    QTime myTime;
    myTime.start();

    QStringList lst;
    QString helpString;

    readData();
    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
    {
        cout << "refilterData: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }

    int fr = nr[0];
    int fftLength = fftL(ndr*fr);

    double spStep = fr/double(fftLength);

    /////////////////////////////////////////////////////////how many channels to filter????
    int numOfChan = ns - 1; //NOT MARKERS

    for(int i = 0; i < numOfChan; ++i)
    {
        for(int j = ndr*fr; j < fftLength; ++j)
        {
            data[i][j] = 0.;
        }
    }

    double norm1 = fftLength / double(ndr*fr);
    double * spectre = new double [fftLength*2];

    for(int j = 0; j < numOfChan; ++j)
    {
        for(int i = 0; i < fftLength; ++i)            //make appropriate array
        {
            spectre[ i * 2 + 0 ] = (double)(data[j][ i ] * sqrt(norm1));
            spectre[ i * 2 + 1 ] = 0.;
        }
        four1(spectre-1, fftLength, 1);       //Fourier transform

        //filtering
        for(int i = 0; i < fftLength; ++i)
        {
            if(i < 2.*lowFreq/spStep || i > 2.*highFreq/spStep)
                spectre[i] = 0.;
        }
        for(int i = fftLength; i < 2*fftLength; ++i)
        {
            if(((2*fftLength - i) < 2.*lowFreq/spStep) || (2*fftLength - i > 2.*highFreq/spStep))
                spectre[i] = 0.;
        }
        //end filtering

        four1(spectre-1, fftLength, -1);
        for(int i = 0; i < ndr*fr; ++i)
        {
            data[j][i] = spectre[2*i]/fftLength/sqrt(norm1);
        }
    }
//    memcpy(data[numOfChan], data[ns-1], ndr*fr*sizeof(double)); //stupid bicycle generality
    helpString.clear();
    for(int i = 0; i < ns; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }

    ui->reduceChannelsLineEdit->setText(helpString);
    writeEdf(ui->filePathLineEdit->text(), data, newPath, ndr*fr);

    cout << "RefilterData: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::reduceChannelsEDFSlot()
{
    QString helpString;
    helpString = dir->absolutePath() + QDir::separator() + ExpName + "_rdcChan.edf";
    reduceChannelsEDF(helpString);
}


void MainWindow::reduceChannelsEDF(QString newFilePath)
{
    QStringList lst;
    readData();

    //bad
//    reduceChannelsFast();
//    QString helpString;
//    helpString.clear();
//    for(int i = 0; i < ns; ++ i)
//    {
//        helpString += QString(i+1) + " ";
//    }
//    ui->reduceChannelsLineEdit->setText(helpString);

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst[lst.length() - 1].toInt() - 1]).contains("Markers"))
    {
        cout << "reduceChannelsEDF: no markers channel" << endl;
        return;
    }

    writeEdf(ui->filePathLineEdit->text(), data, newFilePath, ndr*nr[0]);
}

void MainWindow::reduceChannelsSlot()
{
    QStringList lst;
    QString helpString;
    helpString = ui->reduceChannelsLineEdit->text();

    int * num = new int [maxNs];
    FILE * file;

    double ** dataR = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataR[i] = new double [10000];   ///////////////generality spLength
    }


    QStringList list = helpString.split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(int i = 0; i < list.length(); ++i)
    {
        num[i]=list[i].toInt();
    }

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files, QDir::NoSort);

    for(int i = 0; i < lst.length(); ++i)
    {
        file = fopen((QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i))).toStdString().c_str(), "r");
        if(file==NULL)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open file to read"), QMessageBox::Ok);
            return;
        }

        fscanf(file, "NumOfSlices %d\n", &NumOfSlices);
        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                fscanf(file, "%lf", &dataR[k][j]);
            }
        }
        fclose(file);

        file=fopen((QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i))).toStdString().c_str(), "w");
        if(file==NULL)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open file to write"), QMessageBox::Ok);
            return;
        }

        fprintf(file, "NumOfSlices %d\n", NumOfSlices);
        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < list.length(); ++k)
            {
                fprintf(file, "%lf\n", dataR[num[k]-1][j]);
            }
        }
        fclose(file);

    }

    for(int i = 0; i < ns; ++i)
    {
        delete []dataR[i];
    }
    delete[]dataR;
    ns = list.length();
    delete []num;
    dir->cdUp();

    helpString="channels reduced ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
}

void MainWindow::reduceChannelsFast()
{
    QStringList lst;
    QString helpString;
    QStringList list = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[list[list.length() - 1].toInt() - 1]).contains("Markers"))
    {
        QMessageBox::critical(this, tr("Error"), tr("bad channels list"), QMessageBox::Ok);
        return;
    }

    double ** temp = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        temp[i] = new double [ndr*nr[i]];
    }
    double sign;
    int lengthCounter; //length of the expression in chars

    for(int k = 0; k < list.length(); ++k)
    {
        if(QString::number(list[k].toInt()) == list[k])
        {
            for(int j = 0; j < ndr*nr[list[k].toInt() - 1]; ++j)
            {
                temp[k][j] = data[list[k].toInt() - 1][j];
            }
            //or
//            memccpy(temp[k], data[list[k].toInt() - 1], ndr*nr[list[k].toInt() - 1] * sizeof(double));
        }
        else if(list[k].contains('-') || list[k].contains('+') || list[k].contains('/') )
        {
            lengthCounter = 0;
            lst = list[k].split(QRegExp("[-+/*]"), QString::SkipEmptyParts);
            for(int h = 0; h < lst.length(); ++ h)
            {
                if(QString::number(lst[h].toInt()) != lst[h]) // if not a number between operations
                {
                    cout << "bad rdc chan string" << endl;
                    for(int i = 0; i < ns; ++i)
                    {
                        delete []temp[i];
                    }
                    delete []temp;
                    return;
                }
            }
            for(int j = 0; j < ndr*nr[k]; ++j) //generality k
            {
                temp[k][j] = data[lst[0].toInt() - 1][j]; //copy the data from first channel in the expression into temp
            }
            //or
//            memccpy(temp[k], data[lst[0].toInt() - 1], ndr*nr[k] * sizeof(double));

            lengthCounter += lst[0].length();
            for(int h = 1; h < lst.length(); ++h)
            {
                if(list[k][lengthCounter] == '+') sign = 1.;
                else if(list[k][lengthCounter] == '-') sign = -1.;
                else //this should never happen!
                {
                    cout << "bad rdc chan string" << endl;
                    for(int i = 0; i < ns; ++i)
                    {
                        delete []temp[i];
                    }
                    delete []temp;
                    return;
                }
                lengthCounter += 1; //sign length
                lengthCounter += lst[h].length();

                //check '/' and '*'
                if(list[k][lengthCounter] == '/')
                {
                    sign /= lst[h+1].toDouble();
                }
                else if(list[k][lengthCounter] == '*')
                {
                    sign *= lst[h+1].toDouble();
                }

                cout << sign << " * " << lst[h].toInt() << endl;
                for(int j = 0; j < ndr*nr[k]; ++j) //generality k
                {
                    temp[k][j] += sign * data[lst[h].toInt() - 1][j];
                }

                if(list[k][lengthCounter] == '/' || list[k][lengthCounter] == '*')
                {
                    lengthCounter += 1; // / or *
                    lengthCounter += lst[h+1].length(); //what was divided onto
                    ++h;
                }
            }



        }
        else
        {
            cout << "bad rdc chan string" << endl;
            for(int i = 0; i < ns; ++i)
            {
                delete []temp[i];
            }
            delete []temp;
            return;
        }
    }
    for(int k = 0; k < list.length(); ++k)
    {
        for(int j = 0; j < ddr*ndr*nr[k]; ++j)
        {
            data[k][j] = temp[k][j];
        }
        //or
//        memcpy(data[k], temp[k], ddr*ndr*nr[k] * sizeof(double));
    }


    for(int i = 0; i < ns; ++i)
    {
        delete []temp[i];
    }
    delete []temp;


    ns = list.length();
    cout << "channels reduced, ns = " << ns << endl;

    helpString="channels reduced fast ";
    ui->textEdit->append(helpString);

    helpString="ns equals to " + QString::number(ns);
    ui->textEdit->append(helpString);

}

void MainWindow::concatenateEDFs(QStringList inPath, QString outPath)
{
    if(inPath.isEmpty())
    {
        cout << "concatenateEDFs: input list is empty" << endl;
        return;
    }
    QTime myTime;
    myTime.start();

    //assume the files are concatenable
    int newNdr = 0;
    int tempPos = 0;
    for(int k = 0; k < inPath.length(); ++k)
    {
        setEdfFile(inPath[k]);
        readData();
        newNdr += ndr;
    }

    double ** newData;
    matrixCreate(&newData, ns, newNdr * nr[0]); ////////generality

    for(int k = 0; k < inPath.length(); ++k)
    {
        setEdfFile(inPath[k]);
        readData();
        for(int i = 0; i < ns; ++i)
        {
//            memcpy(newData[i] + tempPos, data[i], sizeof(double) * ndr*nr[0]);
            for(int j = 0; j < ndr*nr[0]; ++j)////////generality
            {
                newData[i][j + tempPos] = data[i][j];
            }
        }
        tempPos += ndr*nr[0];
    }
    QList<int> ls;
    for(int i = 0; i < ns; ++i)
    {
        ls << i;
    }

    setEdfFile(inPath[0]);
    readData();
    writeEdf(inPath[0], newData, outPath, tempPos, ls);
    matrixDelete(&newData, ns);
    cout << "concatenate EDF: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::concatenateEDFs(QString inPath1, QString inPath2, QString outPath)
{
    QTime myTime;
    myTime.start();
    //assume the files are concatenable
    int newNdr = 0;
    int tempPos = 0;

    setEdfFile(inPath2);
    readData();
    newNdr += ndr;

    setEdfFile(inPath1);
    readData();
    tempPos = ndr;
    newNdr += ndr;

    double ** newData;
    matrixCreate(&newData, ns, newNdr * defaults::frequency); ////////generality
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < tempPos*nr[0]; ++j)////////generality
        {
            newData[i][j] = data[i][j];
        }
    }


    setEdfFile(inPath2);
    readData();
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*nr[0]; ++j)////////generality
        {
            newData[i][j + tempPos*nr[0]] = data[i][j];
        }
    }

    QList<int> ls;
    for(int i = 0; i < ns; ++i)
    {
        ls << i;
    }

    writeEdf(inPath1, newData, outPath, newNdr*nr[0], ls);
    matrixDelete(&newData, ns);
    cout << "concatenate EDF: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;

}


void MainWindow::constructEDFSlot()
{
    QString helpString;
    QStringList filters;
    if(!ui->matiCheckBox->isChecked())
    {
        helpString = dir->absolutePath() + QDir::separator() + ExpName + "_new.edf";
        constructEDF(helpString);
    }
    else //if(ui->matiCheckBox->isChecked())
    {
        QString const initEDF = ui->filePathLineEdit->text();
        helpString = dir->absolutePath() + QDir::separator() + "sliceLog.txt";
        ofstream outStream;
        outStream.open(helpString.toStdString().c_str());
        outStream << ExpName.left(3).toStdString() << "\t";
        outStream << "type" << "\t";
        outStream << "sessn" << "\t";
        outStream << "offset" << "\t";
        outStream << "offsetS" << endl;
        outStream.close();

        for(int i = 0; i < 4; ++i) //every type 0-count, 1-track, 2-composed, 3-rest
        {
            setEdfFile(initEDF);
            for(int j = 0; j < 15; ++j) //every session
            {
                if(i != 3 && j >= 5) continue;
                //filter for realisations
                filters.clear();
                helpString = ExpName + "_" + QString::number(i) + "_" + QString::number(j) + "*";
                filters << helpString;

                helpString = dir->absolutePath() + QDir::separator() + ExpName + "_" + QString::number(i) + "_" + QString::number(j) + ".edf";
                constructEDF(helpString, filters);
            }

            cout << "start concatenating" << endl;
            helpString = ExpName + "_" + QString::number(i) + "_*.edf";
            QStringList lst = dir->entryList(QStringList(helpString));  //filter for edfs
            for(int k = 0; k < lst.length(); ++k)
            {
                lst[k].prepend(QString(dir->absolutePath() + QDir::separator()));
            }
            helpString = dir->absolutePath() + QDir::separator() + ExpName.left(3) + "_" + QString::number(i) + ".edf";
            concatenateEDFs(lst, helpString);
        }

    }
}

void MainWindow::constructEDF(QString newPath, QStringList nameFilters) // all the realisations, to newPath based on ui->filePathLineEdit
{
    QStringList lst;
    QString helpString;

    QTime myTime;
    myTime.start();
    readData(); // needed for nr

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    ns = lst.length();
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
    {
        cout << "constructEDF: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }

    if(!ui->sliceWithMarkersCheckBox->isChecked())
    {
        cout << "constructEDF: withMarkersCheckBox is not checked" << endl;
        return;
    }

    dir->cd("Realisations");
    if(!nameFilters.isEmpty())
    {
        lst = dir->entryList(nameFilters, QDir::Files, QDir::Name); //generality
    }
    else
    {
        lst = dir->entryList(QDir::Files, QDir::Name); //generality
    }
    dir->cdUp();

    if(lst.isEmpty())
    {
        cout << "constructEDF: list of realisations is empty. filter[0] = " << nameFilters[0].toStdString() << endl;
        return;
    }

    double ** newData = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        newData[i] = new double [ndr * nr[i]];  //generality, maybe bad nr from other channel?
    }



    FILE * file;
    int currSlice = 0;
    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Realisations" + QDir::separator() + lst[i]);
        file = fopen(helpString.toStdString().c_str(), "r");
        fscanf(file, "NumOfSlices %d", &NumOfSlices);

        for(int i = 0; i < NumOfSlices; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
                fscanf(file, "%lf\n", &newData[j][currSlice]);
            }
            ++currSlice;
        }
        fclose(file);
    }
//    cout << "constructEDF: currSlice = " << currSlice << endl;
    int helpInt = currSlice;
    if(ui->splitZerosCheckBox->isChecked()) splitZeros(&newData, ns, helpInt, &currSlice);

    int offset;
    if(ui->matiCheckBox->isChecked())
    {
        offset = currSlice%(16*250) + 16*250; ////////////////////////////////////////////////////Mati offset ~20 seconds
        helpString = dir->absolutePath() + QDir::separator() + "sliceLog.txt";
        ofstream outStream;
        outStream.open(helpString.toStdString().c_str(), ios_base::app);
        vector<double> tempSlice;
//        cout << "1" << endl;
        for(int i = 0; i < ns; ++i)
        {
            tempSlice.push_back(newData[i][0]); //save first slice with markers
        }
//        cout << "2" << endl;
        for (int i = 0; i < ns; ++i)
        {
            newData[i] = newData[i] + offset;
            newData[i][0] = tempSlice[i];
        }
//        cout << "3" << endl;
        helpString = newPath;
        helpString.remove(0, helpString.lastIndexOf(QDir::separator())); //filename
//        cout << helpString.toStdString() << endl;
        QStringList ls = helpString.split(QRegExp("[_.]"), QString::SkipEmptyParts);
//        cout << ls[3].toStdString() << endl;
//        cout << ls[4].toStdString() << endl;
//        cout << "4" << endl;
        outStream << ExpName.left(3).toStdString() << "\t";
        outStream << ls[3].toStdString() << "\t";
        outStream << ls[4].toStdString() << "\t";
        outStream << offset << "\t";
        outStream << offset/double(nr[0]) << endl;
        outStream.close();
//        cout << "4" << endl;
        currSlice -= offset;

    }


    int nsB = ns;

    cout << "construct EDF: Initial NumOfSlices = " << ndr*ddr*nr[0] << endl;
    cout << "construct EDF: NumOfSlices to write = " << currSlice << endl;

//    helpString.clear();
//    for(int i = 0; i < ns; ++i)
//    {
//        helpString += QString::number(i+1) + " ";
//    }
//    ui->reduceChannelsLineEdit->setText(helpString);


    writeEdf(ui->filePathLineEdit->text(), newData, newPath, currSlice);

    for(int i = 0; i < nsB; ++i)
    {
        newData[i] = newData[i] - offset;
        delete []newData[i];
    }
    delete []newData;
    cout << "construct EDF: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::writeCorrelationMatrix(QString edfPath, QString outPath) //unused
{
    setEdfFile(edfPath);
    readData();
    --ns; // markers out
    int fr = nr[0];

    //write components cross-correlation matrix
    double ** corrs;
    matrixCreate(&corrs, ns, ns);
    for(int i = 0; i < ns; ++i)
    {
        for(int j = i; j < ns; ++j)
        {
            corrs[i][j] = correlation(data[i], data[j], ndr*fr);
            corrs[j][i] = corrs[i][j];
        }
        corrs[i][i] = 0.;
    }
    writeICAMatrix(outPath, corrs, ns); //generality 19-ns
    matrixDelete(&corrs, ns);
    ++ns; //back ns
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


    QString helpString;
    helpString = QDir::toNativeSeparators(ui->filePathLineEdit->text());
    if(!QFile::exists(helpString))
    {
        cout << "readData: edf file doent exist\n" << helpString.toStdString() << endl;
        return;
    }
    FILE * edf = fopen(helpString.toStdString().c_str(), "r"); //generality
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

    helpString = dir->absolutePath() + QDir::separator() + "header.txt";
    FILE * header = fopen(helpString.toStdString().c_str(), "w");



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
    bytes=atoi(helpCharArr);




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
    ndr=atoi(helpCharArr);                      //NumberOfDataRecords

    //duration of a data record, in seconds
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(header, "%c", helpCharArr[i]);
    }
    ddr = atoi(helpCharArr);                       //DurationOfDataRecord
//    cout << "ddr=" << ddr << endl;

    if(flag==1)
    {
        helpInt=floor((ui->finishTimeBox->value()-ui->startTimeBox->value())/double(ddr));
        helpString.setNum(helpInt);

        int s = 0;
        s=fprintf(edfNew, "%d", helpInt);
        for(int i=s; i < 8; ++i)
        {
            fprintf(edfNew, "%c", char(32));
        }

        s=fprintf(edfNew, "%d", ddr);
        for(int i=s; i < 8; ++i)
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
    ns = atoi(helpCharArr);                        //Number of channels
//    cout << "ns = " << ns << endl;

    //labels
    for(int i = 0; i < ns*16; ++i)
    {
        fscanf(edf, "%c", &label[i/16][i%16]);
        fprintf(header, "%c", label[i/16][i%16]);
        if(flag==1) fprintf(edfNew, "%c", label[i/16][i%16]);
        if(i%16==15) label[i/16][16]='\0';

    }

    //edit EOG channels generality for encephalan

    //or A1 A2 vice versa???
    for(int i = 0; i < ns; ++i)
    {
        if(QString(label[i]).contains("EOG 1"))
        {
            strcpy(label[i], "EOG EOG1-A1     ");
        }
        else if(QString(label[i]).contains("EOG 2"))
        {
            strcpy(label[i], "EOG EOG2-A2     ");
        }
    }

    helpString=dir->absolutePath().append(QDir::separator()).append("labels.txt");
    FILE * labels=fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    for(int i = 0; i < ns; ++i)                         //label write in file
    {
        fprintf(labels, "%s \n", label[i]);
    }
    for(int i = ns; i < maxNs; ++i)
    {
        label[i][0] = '\0';
    }

    //transducer type
    for(int i = 0; i < ns*80; ++i)                      //rest of header
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

    helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_markers.txt");
    FILE * markers = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");

    QString * annotations = new QString [5000];
    int numOfAnn = 0;
    int currStart;


    fpos_t *position = new fpos_t;
    fgetpos(edf, position);
    fclose(edf);
    edf = fopen(QDir::toNativeSeparators(ui->filePathLineEdit->text()).toStdString().c_str(), "rb"); //generality
    fsetpos(edf, position);
    delete position;
    bool byteMarker[16];
    bool boolBuf;

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

                //edf+
                if(j==ns-1)  ////////generality?/////////
                {
                    currStart = 0;
                    for(int l = 0; l < len(helpString); ++l)
                    {
                        if(int(helpString.toStdString()[l])== 0 || (int(helpString.toStdString()[l])==20 && (int(helpString.toStdString()[l+1])== 0 || int(helpString.toStdString()[l+1])==20)))
                        {
                            for(int p=currStart; p < l; ++p)
                            {
                                annotations[numOfAnn].append(helpString[p]);
                            }
                            ++numOfAnn;
                            while((int(helpString.toStdString()[l])== 0 || int(helpString.toStdString()[l])==20) && l < len(helpString)) ++l;
                            currStart=l;
                        }
                    }
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
            if(flag==1 && (i+1)>ui->finishTimeBox->value()) break;  //save as EDF
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
//                        data[j][i*nr[j]+k] = physMin[j] + (physMax[j]-physMin[j]) * (double(a)-digMin[j]) / (digMax[j] - digMin[j]);   //enc
                        if(j != ns-1)
                        {
                            data[j][i*nr[j]+k] = a *1./8.; //generality
                        }
                        else
                        {
                            data[j][i*nr[j]+k] = a;
                        }
                    }
                    else if(ui->matiCheckBox->isChecked())
                    {
                        if(j!=ns-1)
                        {
                            fread(&a, sizeof(short), 1, edf);
                            if(flag==1 && ((i*ddr)>=ui->startTimeBox->value()) && ((i*ddr+1) <=ui->finishTimeBox->value()))    //save as EDF
                            {
                                fwrite(&a, sizeof(short), 1, edfNew);
                            }
                            data[j][i*nr[j]+k] = physMin[j] + (physMax[j]-physMin[j]) * (double(a)-digMin[j]) / (digMax[j] - digMin[j]);   //enc
                            data[j][i*nr[j]+k] = a *1./8.;
                        }
                        else
                        {
                            fread(&markA, sizeof(unsigned short), 1, edf);
                            data[j][i*nr[j]+k] = markA;

//                            a += (a<0)*65536;
//                            data[j][i*nr[j]+k] = a + (a<0)*65536;

                            if(data[j][i*nr[j]+k] != 0 ) //////////////////////////////////////////////////   wtf?
                            {

                                for(int h = 0; h < 16; ++h)
                                {
                                    byteMarker[h] = (int(data[j][i*nr[j]+k]) % (int(pow(2,h+1)))) / (int(pow(2,h)));
                                }

                                if(byteMarker[15] && !byteMarker[7]) //elder byte should start with 0 and younger - with 1
                                {
                                    //swap bytes if wrong order
                                    for(int h = 0; h < 8; ++h)
                                    {
                                        boolBuf = byteMarker[h];
                                        byteMarker[h] = byteMarker[h+8];
                                        byteMarker[h+8] = boolBuf;
                                    }


                                    //recalculate the value
                                    data[j][i*nr[j]+k] = 0.;
                                    for(int h = 0; h < 16; ++h)
                                    {
                                        data[j][i*nr[j]+k] += byteMarker[h] * pow(2,h);
                                    }
                                }
                            }
                        }
                    }

                    if(j==(ns-1))
                    {
                        if(data[j][i*nr[j]+k] != 0)
                        {
                            bytes = i*nr[j]+k; //time
                            fprintf(markers, "%d %d", bytes, int(data[j][i*nr[j]+k]));
                            if(ui->matiCheckBox->isChecked())
                            {
                                fprintf(markers, "\t");
                                for(int s = 15; s >= 0; --s)
                                {
                                    fprintf(markers, "%d", byteMarker[s]);
                                    if(s == 8) fprintf(markers, " ");
                                }
                            }
                            fprintf(markers, "\n");
                        }

                        if(data[j][i*nr[j]+k]==200)
                        {
                            staSlice = i*nr[j]+k;
                        }
                    }
                }

            }
        }
//        cout << "staSlice=" << staSlice << " staTime=" << staSlice/defaults::frequency << endl;
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
    fclose(labels);
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

//    cout<<"ndr*ddr = " << ndr*ddr << endl;


//    cout << "data have been read" << endl;
    helpString="data have been read ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    staSlice += 3; //generality LAWL
    fclose(edf);

    ui->markerSecTimeDoubleSpinBox->setMaximum(ndr * ddr);
    ui->markerBinTimeSpinBox->setMaximum(nr[ns-1] * ndr * ddr);
}


void MainWindow::markerSetSecTime(int a)
{
    ui->markerSecTimeDoubleSpinBox->setValue(double(a)/nr[ns-1]);
}

void MainWindow::markerGetSlot()
{
    bool byteMarker[16];
    int timeIndex = ui->markerBinTimeSpinBox->value();
    int marker = data[ns-1][timeIndex];
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
    /*
    int timeIndex = ui->markerBinTimeSpinBox->value();
    int marker = 0;
    QString helpString;

    helpString = ui->markerBin0LineEdit->text();
    for(int h = 0; h < 8; ++h)
    {
        marker += pow(2, 8) * (helpString[h] == '1') * pow(2, 7-h);
    }

    helpString = ui->markerBin1LineEdit->text();
    for(int h = 0; h < 8; ++h)
    {
        marker += (helpString[h] == '1') * pow(2, 7-h);
    }
    ui->markerDecimalLineEdit->setText(QString::number(marker));
    */

    int timeIndex = ui->markerBinTimeSpinBox->value();
    int marker = ui->markerDecimalLineEdit->text().toInt();
    data[ns-1][timeIndex] = marker;
}

void MainWindow::markerSetDecValueSlot()
{
    int timeIndex = ui->markerBinTimeSpinBox->value();
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

    if(ui->eyesCleanCheckBox->isChecked())
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
        if(!ui->matiCheckBox->isChecked())
        {

            QStringList list = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
            if(!QString(label[list.last().toInt() - 1]).contains("Markers") && ui->reduceChannelsCheckBox->isChecked())
            {
                QMessageBox::critical(this, tr("Doge"), tr("Bad Markers channel in rdc channel lineEdit"), QMessageBox::Ok);
                return;
            }

            if(ui->ntRadio->isChecked()) // for Boris
            {
                slice(10, 49, "m"); //math.operation
                slice(50, 89, "e"); //emotional verb
                slice(90, 129, "v"); //verb
                slice(130, 169, "n"); //noun
                slice(170, 209, "a"); //number
            }
            else if(ui->enRadio->isChecked())
            {
                if(ui->windButton->isChecked()) //bad work
                {

                    timeShift = ui->timeShiftSpinBox->value();
                    wndLength = ui->windowLengthSpinBox->value();
//                    return;

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

                        if(marker!=300) sliceWindow(staSlice+i*timeShift, staSlice+i*timeShift+wndLength, int(i+1), marker);

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

                        //                    cout << dir->absolutePath().toStdString() << endl;

                        //delete pre-files from Realisations
                        dir->cd("Realisations");
                        lst.clear();
                        lst = dir->entryList(QStringList("*_pre*"));
                        for(int i = 0; i < lst.length(); ++i)
                        {
                            helpString = dir->absolutePath().append(QDir::separator()).append(lst.at(i));
                            if(remove(helpString.toStdString().c_str()) != 0)
                            {
                                perror("cannot delete file");
                                break;
                            }
                        }
                        dir->cdUp();


                    }
                }
            }
        }
        else //if matiCheckBox->isChecked()
        {
            sliceMati(numChanToWrite);
        }

        --ns; //-markers channel generality
        ns = numChanToWrite; //generality

        ui->progressBar->setValue(0);

        helpString="data has been sliced \n";
        ui->textEdit->append(helpString);


    }


    helpString="data sliced ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    cout << "SliceAll: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;
}


void MainWindow::sliceIlya(const QString &fileName, QString marker) //beginning - from mark1 to mark 2, end - from 251 to 255. Marker - included in filename
{
    QString helpString;
    FILE * file;
    FILE * in;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    int tmpMarker;

    helpString=dir->absolutePath().append(QDir::separator()).append(fileName);
    in = fopen (QDir::toNativeSeparators(helpString).toStdString().c_str(), "r");
    if(in==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open file"), QMessageBox::Ok);
        return;
    }
    fscanf(in, "NumOfSlices %d\r\n", &NumOfSlices);
    double ** dataI = new double *[ns];
    for(int i = 0; i < ns; ++i)
    {
        dataI[i] = new double [NumOfSlices];
    }

    for(int i = 0; i < NumOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fscanf(in, "%lf", &dataI[j][i]);
        }
    }


    for(int i = 0; i < NumOfSlices; ++i)
    {
        if(dataI[ns-1][i] != 0 && dataI[ns-1][i]!=1 && (dataI[ns-1][i] < 50 || dataI[ns-1][i]>53) && h== 0)
        {
            j=i;
            h=1;
            tmpMarker=dataI[ns-1][i];
            continue;
        }
        if(dataI[ns-1][i] != 0 && dataI[ns-1][i]!=1 && (dataI[ns-1][i] < 50 || dataI[ns-1][i]>53) && h==1)
        {

            if(dataI[ns-1][i]==27)
            {
                k=i+250;
                if(tmpMarker==21) marker="DODO";
                if(tmpMarker==22) marker="DONT";
            }
            else
            {
                k=i;
                if(dataI[ns-1][i]==2) marker="Begin";
                if(dataI[ns-1][i]>=11 && dataI[ns-1][i] <= 15) marker="Beep";
                if(dataI[ns-1][i]==21 || dataI[ns-1][i]==22) marker="St";
            }
            ++h;

        }
        if(h==2)
        {
            ++number;
            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(marker).append(QDir::separator()).append(fileName).append("_").append(marker);
            file=fopen(helpString.toStdString().c_str(), "w");

            fprintf(file, "NumOfSlices %d \r\n", k-j);
            for(int l=j; l < k; ++l)
            {
                for(int m = 0; m < ns-1; ++m)   //w/o markers
                {
                    fprintf(file, "%lf\r\n", dataI[m][l]);
                }
            }
            fclose(file);
            i -= 2;
            h = 0;
        }
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []dataI[i];

    }
    delete []dataI;
}

void MainWindow::sliceIlya(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - from 251 to 255. Marker - included in filename
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            marker.setNum(int(data[ns-1][i]));
            continue;
        }
//        if(data[ns-1][i]>=251 && data[ns-1][i] <= 254)
        if(data[ns-1][i]==21 || data[ns-1][i]==22)
        {
            k=i+500;
            if(h==1) ++h;

        }
        if(h==2)
        {
            ++number;
            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker);
            file=fopen(helpString.toStdString().c_str(), "w");

            fprintf(file, "NumOfSlices %d \r\n", k-j);
            for(int l=j; l < k; ++l)
            {
                for(int m = 0; m < ns; ++m)  //with markers
                {
                    fprintf(file, "%lf\r\n", data[m][l*nr[m]/nr[ns-1]]);
                }
            }
            fclose(file);
            i -= 2;
            h = 0;
        }
    }
}

void MainWindow::slice(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - 254. Marker - included in filename
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    double solveTime = 0.;
//    double solve[4];
//    for(int i = 0; i < 4; ++i)
//    {
//        solve[i] = 0.;
//    }
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            continue;
        }
        if(data[ns-1][i]==254)
        {
            k=i;
            if(h==1)
            {
                h = 0;
                if(k-j < 500) continue;
                ++number;
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices %d \n", k-j);
                solveTime += (k-j);


//                switch(number/12)
//                {
//                case 0: {solve[0] += (k-j); break;}
//                case 1: {solve[1] += (k-j); break;}
//                case 2: {solve[2] += (k-j); break;}
//                case 3: {solve[3] += (k-j); break;}
//                default: break;
//                }

                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
                fclose(file);
                i += 17;
            }
        }
    }
//    cout << number << endl;
//    for(int i = 0; i < 4; ++i)
//    {
//        cout << "solve[" << i << "]=" << solve[i]/(12 *defaults::frequency) << endl;
//    }

    solveTime/=(defaults::frequency*number);
    cout << "solveTime " << marker.toStdString() << " =" << solveTime << endl << endl;

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    if(ui->eyesCleanCheckBox->isChecked()) fprintf(res, "solve time %s \t %lf \n", marker.toStdString().c_str(), solveTime);
    fclose(res);
}

void MainWindow::sliceFromTo(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - 254. Marker - included in filename
{
    QString helpString;
    FILE * file;
    FILE * out;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    double solveTime = 0.;

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(marker));
    out = fopen(helpString.toStdString().c_str(), "w");
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(h== 0)
        {
            if(data[ns-1][i]==marker1)
            {
                j=i;
                h=1;
                continue;
            }
        }
        else
        {
            if(data[ns-1][i]==254)
            {
                h = 0;
                continue;
            }
            if(data[ns-1][i]==marker2)
            {
                k=i;
                if(h==1)
                {
                    h = 0;
                    if(k-j < 1000) continue;
                    ++number;
                    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4)));
                    file=fopen(helpString.toStdString().c_str(), "w");
                    fprintf(file, "NumOfSlices %d \n", k-j);

                    fprintf(out, "%d \n", k-j);
                    solveTime += (k-j);

                    for(int l=j; l < k; ++l)
                    {
                        for(int m = 0; m < ns-1; ++m)
                        {
                            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                        }
                    }
                    fclose(file);
                    i += 17;
                }
            }
        }
    }
    fclose(out);
    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(marker));
    QFile *file2 = new QFile(helpString);
    file2->open(QIODevice::ReadOnly);
    QByteArray contents = file2->readAll();
    file2->close();
    file2->open(QIODevice::WriteOnly);
    file2->write("NumOfFiles ");
    file2->write(QString::number(number).toStdString().c_str());
    file2->write("\r\n");
    file2->write(contents);
    file2->close();
    delete file2;

//    kernelest(helpString);

    solveTime/=(defaults::frequency*number);
//    cout << "average time before feedback " << marker.toStdString() << " =" << solveTime << endl;

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    if(ExpName.contains("FB")) fprintf(res, "time before feedback %s\t%lf\n", marker.toStdString().c_str(), solveTime);
    fclose(res);
}

void MainWindow::sliceByNumberAfter(int marker1, int marker2, QString marker)
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            h=1;
            continue;
        }
        if(h==1 && data[ns-1][i] != 0 && data[ns-1][i]!=254)
        {
            h=2;
            j=i;
            continue;
        }
        if(h==1 && data[ns-1][i]==254)
        {
            h = 0;
            continue;
        }
        if(data[ns-1][i]==254)
        {
            k=i;
            if(h==2)
            {
                ++number;
                h = 0;
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices %d \n", k-j);

                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
                fclose(file);
                i += 17;
            }
        }
    }




}

void MainWindow::sliceBak(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end 250. Marker - included in filename
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            continue;
        }
        if(data[ns-1][i]==250)
        {
            k=i;
            if(h==1) ++h;

        }
        if(h==2)
        {
            ++number;
            helpString=dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
            file=fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");

            fprintf(file, "NumOfSlices %d \n", k-j);
//            fprintf(file, "NumOfSlicesEyesCut 0 \n");
            for(int l=j; l < k; ++l)
            {
                for(int m = 0; m < ns-1; ++m)
                {
                    fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                }
            }
            fclose(file);
            i += 17;
            h = 0;
        }
    }
}

void MainWindow::sliceWindow(int startSlice, int endSlice, int number, int marker)
{
    QString helpString;
    if(endSlice - startSlice > 2500) return;


    helpDouble = 0.;
    //check real signal contained
    for(int l = startSlice; l < endSlice; ++l)
    {
        helpInt = 0;
        for(int m = 0; m < ns-1; ++m) // no marker channel
        {
            if(data[m][l*nr[m]/nr[ns-1]] == 0) ++helpInt;
        }
        if(helpInt == ns-1) helpDouble +=1.;
    }
    if(helpDouble > 0.1 * wndLength) return;

    FILE * file;
    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("windows").append(QDir::separator()).append(ExpName).append("-").append(rightNumber(number, 4)).append("_").append(QString::number(marker)); //number.marker
    file=fopen(helpString.toStdString().c_str(), "w");
    if(file==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open file"), QMessageBox::Ok);
        return;
    }

    fprintf(file, "NumOfSlices %d \n", endSlice-startSlice);
    for(int l = startSlice; l < endSlice; ++l)
    {
        for(int m = 0; m < ns-1; ++m) // no marker channel
        {
            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
        }
    }
    fclose(file);
}

void MainWindow::sliceGaps()
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(data[ns-1][i]==254 && h== 0)
        {
            j=i;
            h=1;
            continue;
        }

        if(data[ns-1][i]==201 && h==1) //don't consider with pauses
        {
            h = 0;
            continue;
        }

        if(data[ns-1][i]!=254 && data[ns-1][i]!=255 && data[ns-1][i] != 0) //generality
        {
            k=i;
            if(h==1)
            {
                h = 0;
                if(k-j < 500 || (k-j)>10000) continue;
                ++number;
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_254.").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");
                fprintf(file, "NumOfSlices %d \n", k-j);
                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }

                }
                fclose(file);
            }
        }
    }
}

void MainWindow::sliceOneByOne() //generality, just for my current
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;
    int h = 1;
    QString marker = "000";
    //200, 255, (241||247, num, 254, 255)
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(data[ns-1][i] == 0)
        {
            continue;
        }
        if((data[ns-1][i] > 200 && data[ns-1][i] < 241) || data[ns-1][i] == 255 || data[ns-1][i] == 250 || data[ns-1][i] == 251) //order inportant! - not interesting markers
        {
            continue;
        }
        if((data[ns-1][i] == 254 || data[ns-1][i] == 200) && h == 0) //start gap
        {
            j = i;
            h = 1;
            marker.setNum(data[ns-1][i]);
            continue;
        }
//        if((data[ns-1][i] == 241 || data[ns-1][i] == 247) && h != 1) //start task
        if((data[ns-1][i] == 241 || data[ns-1][i] == 247) && h == 0) //start task
        {
            j = i;
            h = 2;
            marker.setNum(data[ns-1][i]);
            continue;
        }
        if(data[ns-1][i] != 0 && h == 2) //write between unknown marker and 241 || 247
        {
            ++number;
            helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append("num");
            file = fopen(helpString.toStdString().c_str(), "w");
            fprintf(file, "NumOfSlices %d \n", i-j);
            if(i-j > 15000 && (marker == "255" || marker == "254"))
            {
                for(int l = j; l < i; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "0.000\n");
                    }
                }
            }
            else
            {
                for(int l = j; l < i; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
            }
            fclose(file);


            h = 1;
            j = i;
            continue;
        }

        if(data[ns-1][i] != 0 && h == 1) //generality first nonzero marker after start - write
        {
            k = i;
            h = 0;
            ++number;
            helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
//            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
            file = fopen(helpString.toStdString().c_str(), "w");
            fprintf(file, "NumOfSlices %d \n", k-j);
            for(int l = j; l < k; ++l)
            {
                for(int m = 0; m < ns-1; ++m)
                {
                    fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                }

            }
            fclose(file);
            --i; //for checking the last marker once more
        }
//        if(number == 125) cout<<i<<endl;
    }

    //write last rest state
    marker = "end";
    k = ndr*nr[ns-1];
    ++number;
    helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
//    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
    file=fopen(helpString.toStdString().c_str(), "w");
    fprintf(file, "NumOfSlices %d \n", k-j);
    for(int l = j; l < k; ++l)
    {
        for(int m = 0; m < ns-1; ++m)
        {
            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
        }

    }
    fclose(file);
}

void MainWindow::sliceOneByOneNew(int numChanWrite)
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;
    int h = 0; //h == 0 - 241, h == 1 - 247
    QString marker = "000";
    //200, 255, (241||247, num, 254, 255)
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(data[ns-1][i] == 0)
        {
            continue;
        }
        else if((data[ns-1][i] > 200 && data[ns-1][i] < 241) || data[ns-1][i] == 255 || data[ns-1][i] == 250 || data[ns-1][i] == 251) //all not interesting markers
        {
            continue;
        }
        else if(data[ns-1][i] == 241 || data[ns-1][i] == 247)
        {
            marker = "254";
            if(data[ns-1][i] == 241) h = 0;
            else if (data[ns-1][i] == 247) h = 1;
            continue; // wait for num marker
        }
        else if(1) //marker can be <=200, ==254, smth else
        {
            if(marker.isEmpty())
            {
                marker = "sht";
            }

            ++number;

            helpString=QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Realisations" + QDir::separator() + ExpName + "." + rightNumber(number, 4) + "_" + marker);

            if(((marker == "000") && defaults::wirteStartEndLong) || (marker != "000")) // write the beginning?
            {
                file = fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices %d \n", i-j);
                for(int l = j; l < i; ++l)
                {
                    for(int m = 0; m < numChanWrite; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
                fclose(file);
            }
            else if (i-j > 2500 && (marker == "254"))
            {
                file = fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices 1000 \n");
                for(int l = 0; l < 1000; ++l)
                {
                    for(int m = 0; m < numChanWrite; ++m)
                    {
                        fprintf(file, "0.000\n");
                    }
                }
                fclose(file);
            }

            ui->progressBar->setValue(double(i)*100./ndr*nr[ns-1]);
            qApp->processEvents();

            marker.clear();
            if(h == 0) marker = "241";
            else if(h == 1) marker = "247";
            h = -1;
            j = i;
            continue;
        }

    }

    //write last file
    if(0)
    {
        marker = "254";
        ++number;
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
        file = fopen(helpString.toStdString().c_str(), "w");
        fprintf(file, "NumOfSlices 10 \n");
        for(int l = 0; l < 10; ++l)
        {
            for(int m = 0; m < numChanWrite; ++m)
            {
                if(m != numChanWrite-1 && l != 0)
                {
                    fprintf(file, "0.000\n");
                }
                else
                {
                    fprintf(file, "254.000\n");
                }
            }
        }
        fclose(file);
    }
    else if(0)
    {
        //write last rest state
        k = ndr*nr[ns-1];
        ++number;
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
        file=fopen(helpString.toStdString().c_str(), "w");
        fprintf(file, "NumOfSlices %d \n", k-j);
        for(int l = j; l < k; ++l)
        {
            for(int m = 0; m < numChanWrite; ++m)
            {
                fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
            }
        }
        fclose(file);
    }

}

void MainWindow::sliceMati(int numChanWrite)
{
    QStringList lst;
    QString helpString;
    int start = 0;
    int end = -1;
    bool markers[16];
    bool state[3];
    QString fileMark;
    int number = 0;
    int session[4];
    int type = 3;
    FILE * file;
    int piece = 16*250; //length of a piece just for visual processing
    for(int i = 0; i < 4; ++i)
    {
        session[i] = 0;
    }

    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(data[ns-1][i] == 0)
        {
            continue;
        }
        else //nonzero marker
        {
            for(int j = 0; j < 16; ++j)
            {
                markers[j] = (int(data[ns-1][i])%(int(pow(2,j+1))))/(int(pow(2,j))); //provide bitwise form
            }

            //decide whether the marker is interesting: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
            if(!markers[7]) //0xxxxxxx 0xxxx(000) and 1xxxxxxx 0xxxx(000)
            {
                for(int i = 0; i < 3; ++i)
                {
                    state[i] = markers[i];
                }
            }
            else if(!markers[15]) //xxxxx(000) 1xxxxxxx
            {
                for(int i = 0; i < 3; ++i)
                {
                    state[i] = markers[8 + i];
                }
            }

            if(!(state[0] || state[1] || state[2])) continue;

            //output marker number
//            cout << i << "\t" << data[ns-1][i] << "\t";
//            for(int j = 15; j >= 0; --j)
//            {
//                cout << markers[j];
//                if(j%4==0) cout << " ";
//            }
//            cout<<endl;

            if(state[2] == 1) //the end of a session
            {
                if(state[1] == 0 && state[0] == 1) //end of a counting session
                {
                    type = 0;
                    fileMark = "241"; //count
                }
                if(state[1] == 1 && state[0] == 0) //end of a tracking session
                {
                    type = 1;
                    fileMark = "247"; //follow
                }
                if(state[1] == 1 && state[0] == 1) //end of a composed session
                {
                    type = 2;
                    fileMark = "244"; //composed
                }
            }
            else //if the start of a session
            {
                type = 3;
                fileMark = "254"; //rest. start of the session is sliced too
            }
            end = i;
        }

        if(end > start)
        {
            number = int(ceil((end-start)/double(piece)));

            //adjust for whole wndLen windows - cut start ~1,5 pieces
//            start = end - piece * (number-2);  //////////////////////////////////////////////////////1 or 2



            for(int j = 0; j < number; ++j) // num of pieces
            {
                helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Realisations" + QDir::separator() + ExpName + "_" + QString::number(type) + "_" + QString::number(session[type]) + "_" + rightNumber(j, 2)) + '.' + fileMark;
                file = fopen(helpString.toStdString().c_str(), "w");
                NumOfSlices = min(end - start - j*piece, piece);
                fprintf(file, "NumOfSlices %d \n", NumOfSlices);
                {
                    for(int l = start  + j*piece; l < min(start + (j+1)*piece, end); ++l) //end slice not included
                    {
                        for(int m = 0; m < numChanWrite; ++m)
                        {
                            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                        }
                    }
                }
                fclose(file);
            }

            ui->progressBar->setValue(double(i)*100./ndr*nr[ns-1]);

            fileMark.clear();
            start = i;
            end = -1;
            ++session[type];
        }

    }

    //slice end piece
    if(defaults::wirteStartEndLong)
    {
        type = 3;
        end = ndr*nr[ns-1];
        ++session[type];
        fileMark = "254";
        number = int(ceil((end-start)/double(piece)));
        //adjust for whole wndLen windows - cut start ~1,5 pieces
//        start = end - piece * (number-2);  //////////////////////////////////////////////////////1 or 2


        for(int j = 0; j < number; ++j) // num of pieces
        {
            helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Realisations" + QDir::separator() + ExpName + "_" + QString(type) + "_" + QString::number(session[type]) + "_" + rightNumber(j, 2) + '.' + fileMark);
            file = fopen(helpString.toStdString().c_str(), "w");
            NumOfSlices = min(end - start - j*piece, piece);
            fprintf(file, "NumOfSlices %d \n", NumOfSlices);
            {
                for(int l = start  + j*piece; l < min(start + (j+1)*piece, end); ++l) //end slice not included
                {
                    for(int m = 0; m < numChanWrite; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
            }
            fclose(file);
        }
    }

    //count NumOfSlices
    cout << "init slices = " << ndr*nr[ns-1] << endl;

    dir->cd("Realisations");
    lst.clear();
    lst = dir->entryList(QDir::Files);
    cout << lst.length() << endl;
    dir->cdUp();
    number = 0;
    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(lst[i]);

        file = fopen(helpString.toStdString().c_str(), "r");
        fscanf(file, "NumOfSlices %d", &NumOfSlices);
        number += NumOfSlices;
        fclose(file);
    }
    cout << "sliced = " << number << endl;
}

void MainWindow::eyesFast()  //generality
{
    QString helpString;
    FILE * coef=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("eyes.txt").toStdString().c_str(), "r");
    if(coef==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No eyes coefficients found"), QMessageBox::Ok);
        return;
    }

    int NumEog, NumEeg;

    fscanf(coef, "NumOfEyesChannels %d\n", &NumEog);
    fscanf(coef, "NumOfEegChannels %d\n", &NumEeg);

    double **coefficients = new double * [NumEeg];
    for(int i = 0; i < NumEeg; ++i)
    {
        coefficients[i] = new double [NumEog];
    }

    for(int k = 0; k < NumEeg; ++k)
    {
        for(int i = 0; i < NumEog; ++i)
        {
            fscanf(coef, "%lf ", &coefficients[k][i]);
        }
        fscanf(coef, "\n");
    }
    fclose(coef);

    int a[2]; //generality 2
    a[0] = -1;
    a[1] = -1;

    if(ui->enRadio->isChecked())
    {
//        if(ui->reduceChannelsComboBox->currentText().contains("MichaelBak"))  //generality
//        {
//            a[0]=22; //NumOfEEg channel for En (19 EEG, A1-A2, A1-N, ECG, Eog1, Eog2) //generality
//            a[1]=23;
//        }
//        else if(ui->reduceChannelsComboBox->currentText().contains("MyCurrent") || ui->reduceChannelsComboBox->currentText().contains("Mati", Qt::CaseInsensitive))
//        {
//            //my current
//            a[0]=21; //NumOfEEg channel for En (19 EEG, A1-A2, A1-N, Eog1, Eog2) //generality
//            a[1]=22;
//        }
        for(int i = 0; i < ns; ++i)
        {
            if(QString(label[i]).contains("EOG1", Qt::CaseInsensitive)) //generality eog channels names
            {
                a[0] = i;
            }
            else if(QString(label[i]).contains("EOG2", Qt::CaseInsensitive)) //generality eog channels names
            {
                a[1] = i;
            }
        }
    }
    if(a[0] == -1 && a[1] == -1)
    {
        cout << "eyesFast: EOG channels not found" << endl;
        for(int i = 0; i < NumEeg; ++i)
        {
            delete []coefficients[i];
        }
        delete []coefficients;
        return;
    }

    for(int k = 0; k < NumEeg; ++k)
    {
        for(int j = 0; j < ndr*nr[k]; ++j)
        {
            for(int z = 0; z < NumEog; ++z)
            {
                data[k][j] -= coefficients[k][z]*data[a[z]][j]; //a[z]
            }
        }
    }

    cout << "eyes cleaned, ns=" << ns << endl;

    for(int i = 0; i < NumEeg; ++i)
    {
        delete []coefficients[i];
    }
    delete []coefficients;

    helpString="eyes cleaned fast ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
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





//products for ICA
void product1(double ** const arr, int length, int ns, double * vec, double ** outVector)
{
    //<X*g(Wt*X)>
    //vec = Wt
    //X[j] = arr[][j] dimension = ns
    //average over j

    for(int i = 0; i < ns; ++i)
    {
        (*outVector)[i] = 0.;
    }

    double sum = 0.;


    for(int j = 0; j < length; ++j)
    {
        sum = 0.;

        for(int i = 0; i < ns; ++i)
        {
            sum += vec[i] * arr[i][j];
        }
        for(int i = 0; i < ns; ++i)
        {
            (*outVector)[i] += tanh(sum) * arr[i][j];
        }
    }
    for(int i = 0; i < ns; ++i)
    {
        (*outVector)[i] /= length;
    }
}



void product2(double ** const arr, int length, int ns, double * vec, double ** outVector)
{
    //g'(Wt*X)*1*W
    //vec = Wt
    //X = arr[][j]
    //average over j

    double sum = 0.;
    double sum1 = 0.;


    for(int j = 0; j < length; ++j)
    {
        sum = 0.;
        for(int i = 0; i < ns; ++i)
        {
            sum += vec[i] * arr[i][j];
        }
        sum1 += 1 - tanh(sum)*tanh(sum);
    }
    sum1 /= length;

    for(int i = 0; i < ns; ++i)
    {
        (*outVector)[i] = sum1 * vec[i];
    }

}



void product3(double ** vec, int ns, int currNum, double ** outVector)
{
    //sum(Wt*Wi*Wi)

    for(int k = 0; k < ns; ++k)
    {
        (*outVector)[k] = 0.;
    }
    double sum = 0.;

    for(int j = 0; j < currNum; ++j)
    {
        sum = 0.;
        for(int k = 0; k < ns; ++k)
        {
            sum += vec[currNum][k]*vec[j][k];
        }
        for(int k = 0; k < ns; ++k)
        {
            (*outVector)[k] += vec[j][k] * sum;
        }

    }


}

double * randomVector(int ns)
{

    double * tempVector2 = new double [ns];
    srand(time(NULL));
    double sum = 0.;
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] = rand()%30 - 15;
        sum += tempVector2[i] * tempVector2[i];
    }
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] /= sqrt(sum);
    }
    return tempVector2;

}


void MainWindow::writeEdf(QString inFilePath, double ** inData, QString outFilePath, int numSlices, QList<int> chanList)
{
    //    8 ascii : version of this data format (0)
    //    80 ascii : local patient identification (mind item 3 of the additional EDF+ specs)
    //    80 ascii : local recording identification (mind item 4 of the additional EDF+ specs)
    //    8 ascii : startdate of recording (dd.mm.yy) (mind item 2 of the additional EDF+ specs)
    //    8 ascii : starttime of recording (hh.mm.ss)
    //    8*3 + 80*2 = 184
    //    8 ascii : number of bytes in header record
    //    44 ascii : reserved
    //    184 + 8 + 44 = 236
    //    8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs)
    //    8 ascii : duration of a data record, in seconds
    //    4 ascii : number of signals (ns) in data record
    //    236 + 8 + 8 + 4 = 256

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
    //    16 + 80 + 8 + 8 + 8 + 8 + 8 + 80 + 8 + 32 = 256


    if(!QFile::exists(inFilePath))
    {
        cout << "writeEDF: bad inFilePath\n" << inFilePath.toStdString() << endl;
        return;
    }

    QTime myTime;
    myTime.start();

    QStringList lst;
    QString helpString;

    //all bounded to nsLine
    char *helpCharArr = new char[50];
    char helpChar;
    int bytes;
    short int a;

    FILE * edfNew;
    FILE * edfIn;


//old
    /*
    lst.clear();
    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    int newNs = lst.length();
    */
//new
    if(chanList.isEmpty()) //use ui->reduceChannelsLineEdit
    {
        chanList.clear();
        lst.clear();
        lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
        for(int i = 0; i < lst.length(); ++i)
        {
            chanList << lst[i].toInt()-1;
        }
    }

    int newNs = chanList.length();






    edfIn = fopen(inFilePath.toStdString().c_str(), "r");
    if(edfIn == NULL)
    {
        cout << "writeEDF: cannot open edf file to read\n" << inFilePath.toStdString() << endl;
        return;
    }
    edfNew = fopen(outFilePath.toStdString().c_str(), "w");
    if(edfNew == NULL)
    {
        cout << "writeEDF: cannot open edf file to write\n" << outFilePath.toStdString()<< endl;
        fclose(edfIn);
        return;
    }


    //header read
    for(int i = 0; i < 184; ++i)
    {
        fscanf(edfIn, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }

    //number of bytes in header record
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edfIn, "%c", &helpCharArr[i]);
    }
    bytes = atoi(helpCharArr);

    bytes = 256 * (newNs + 1);

    helpString = QString::number(bytes);
    for(int i = helpString.length(); i < 8; ++i)
    {
        helpString.append(' ');
    }
    fprintf(edfNew, "%s", helpString.toStdString().c_str());

    //"reserved"
    for(int i = 0; i < 44; ++i)
    {
        fscanf(edfIn, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }






    //number of data records
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edfIn, "%c", &helpCharArr[i]);
    }
    ndr=atoi(helpCharArr);//NumberOfDataRecords

    //duration of a data record, in seconds
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edfIn, "%c", &helpCharArr[i]);
    }
    ddr=atoi(helpCharArr); //generality double ddr


    ndr = int(numSlices/defaults::frequency)/ddr;
    helpString = QString::number(ndr);
    for(int i = helpString.length(); i < 8; ++i)
    {
        helpString.append(' ');
    }
    fprintf(edfNew, "%s", helpString.toStdString().c_str());

    for(int i = 0; i < 8; ++i)
    {
        fprintf(edfNew, "%c", helpCharArr[i]); //ddr
    }





    for(int i = 0; i < 4; ++i)
    {
        fscanf(edfIn, "%c", &helpCharArr[i]);
    }
    ns = atoi(helpCharArr);                        //Number of channels
//    cout << "writeEDF: oldNs = " << ns << endl;

    helpString = QString::number(newNs);
    for(int i = helpString.length(); i < 4; ++i)
    {
        helpString.append(' ');
    }
    fprintf(edfNew, "%s", helpString.toStdString().c_str());


    for(int i = 0; i < ns*16; ++i)                      //label read and DONT write yet
    {
        fscanf(edfIn, "%*c");
    }

    //better labels
    for(int i = 0; i < newNs; ++i)
    {
        fprintf(edfNew, "%s", label[chanList[i]]);

    }
    helpString=dir->absolutePath().append(QDir::separator()).append("labels.txt");
    FILE * labels = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    for(int i = 0; i < newNs; ++i)                         //label write in file
    {
        fprintf(labels, "%s\n", label[chanList[i]]);
    }
    fclose(labels);

    //transducer type
    char ** transducer = new char * [ns];
    for(int i = 0; i < ns; ++i)
    {
        transducer[i] = new char [80 + 1];
        for(int j = 0; j < 80; ++j)
        {
            fscanf(edfIn, "%c", &transducer[i][j]);
        }
        transducer[i][80] = '\0';
    }
    for(int i = 0; i < newNs; ++i)
    {
        fprintf(edfNew, "%s", transducer[chanList[i]]);
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []transducer[i];
    }
    delete []transducer;




    //physical dimension
//    cout << "writeEDF: physDim = " << endl;
    char ** physDim = new char * [ns];
    for(int i = 0; i < ns; ++i)
    {
        physDim[i] = new char [8 + 1];
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edfIn, "%c", &physDim[i][j]);
        }
        physDim[i][8] = '\0';
//        cout << physDim[i] << endl;
    }

    for(int i = 0; i < newNs; ++i)
    {
        fprintf(edfNew, "%s", physDim[chanList[i]]);
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []physDim[i];
    }
    delete []physDim;


    //physical minimum
    double *physMin;
    physMin = new double [ns];
    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edfIn, "%c", &helpCharArr[j]);
        }
        physMin[i]=double(atoi(helpCharArr));
    }

    for(int i = 0; i < newNs; ++i)
    {
        helpString = QString::number(physMin[chanList[i]]);
        for(int i = helpString.length(); i < 8; ++i)
        {
            helpString.append(' ');
        }
        fprintf(edfNew, "%s", helpString.toStdString().c_str());
    }

    //physical maximum
    double *physMax;
    physMax = new double [ns];

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edfIn, "%c", &helpCharArr[j]);
        }
        physMax[i]=double(atoi(helpCharArr));
    }

    for(int i = 0; i < newNs; ++i)
    {
        helpString = QString::number(physMax[chanList[i]]);
        for(int i = helpString.length(); i < 8; ++i)
        {
            helpString.append(' ');
        }
        fprintf(edfNew, "%s", helpString.toStdString().c_str());
    }

    //digital minimum
    double *digMin;
    digMin = new double [ns];

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edfIn, "%c", &helpCharArr[j]);
        }
        digMin[i]=double(atoi(helpCharArr));
    }

    for(int i = 0; i < newNs; ++i)
    {
        helpString = QString::number(digMin[chanList[i]]);
        for(int i = helpString.length(); i < 8; ++i)
        {
            helpString.append(' ');
        }
        fprintf(edfNew, "%s", helpString.toStdString().c_str());
    }

    //digital maximum
    double *digMax;
    digMax = new double [ns];     //memory for channels' labels

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edfIn, "%c", &helpCharArr[j]);
        }
        digMax[i]=double(atoi(helpCharArr));
    }

    for(int i = 0; i < newNs; ++i)
    {
        helpString = QString::number(digMax[chanList[i]]);
        for(int i = helpString.length(); i < 8; ++i)
        {
            helpString.append(' ');
        }
        fprintf(edfNew, "%s", helpString.toStdString().c_str());
    }


    //prefiltering
//    cout << "writeEDF: prefiltering = " << endl;
    char ** prefilter = new char * [ns];
    for(int i = 0; i < ns; ++i)
    {
        prefilter[i] = new char [80 + 1];
        for(int j = 0; j < 80; ++j)
        {
            fscanf(edfIn, "%c", &prefilter[i][j]);
        }
        prefilter[i][80] = '\0';
    }
    for(int i = 0; i < newNs; ++i)
    {
        fprintf(edfNew, "%s", prefilter[chanList[i]]);
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []prefilter[i];
    }
    delete []prefilter;


    //number of records (nr samples in ddr seconds)

//    cout << "writeEDF: nr = " << endl;
    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edfIn, "%c", &helpCharArr[j]);
        }
        helpCharArr[8] = '\0';
        nr[i]=atoi(helpCharArr);
    }
    for(int i = 0; i < newNs; ++i)
    {
        helpString = QString::number(nr[chanList[i]]);
        for(int i = helpString.length(); i < 8; ++i)
        {
            helpString += ' ';
        }
        fprintf(edfNew, "%s", helpString.toStdString().c_str());
    }


    //reserved
    char ** reserved = new char * [ns];
    for(int i = 0; i < ns; ++i)
    {
        reserved[i] = new char [32 + 1];
        for(int j = 0; j < 32; ++j)
        {
            fscanf(edfIn, "%c", &reserved[i][j]);
        }
        reserved[i][32] = '\0';
    }
    for(int i = 0; i < newNs; ++i)
    {
        fprintf(edfNew, "%s", reserved[chanList[i]]);
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []reserved[i];
    }
    delete []reserved;


    for(int i = 0; i < (bytes-(newNs+1)*256); ++i)                      //Neurotravel//generality//strange
    {
        fscanf(edfIn, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }

    //header write ended
    fclose(edfNew);
    fclose(edfIn);
    edfNew = fopen(outFilePath.toStdString().c_str(), "ab");

    ui->finishTimeBox->setMaximum(ddr*ndr);

//    cout << "writeEDF: data write start" << endl;
    int oldIndex;
    if(ui->enRadio->isChecked())
    {
        for(int i = 0; i < ndr; ++i)
        {
//            cout << i << " "; cout.flush();
            for(int j = 0; j < newNs; ++j) // j - number of channel in "new" file
            {
                oldIndex = chanList[j];
              //newIndex == j


                for(int k = 0; k < nr[oldIndex]; ++k)
                {
//                    cout << i << " " << j << " " << k << endl;

//                    a = (short)((inData[ j ][ i * nr[oldIndex] + k ] - physMin[oldIndex]) * (digMax[oldIndex] - digMin[oldIndex]) / (physMax[oldIndex] - physMin[oldIndex]) + digMin[oldIndex]);

                    //generality
                    if(oldIndex != ns - 1)
                    {
                        a = (short)(inData[ j ][ i * nr[oldIndex] + k ] * 8.); //*8 generality
                    }
                    else if(!ui->matiCheckBox->isChecked())
                    {
                        a = (short)(inData[ j ][ i * nr[oldIndex] + k ]);
                    }
                    else
                    {
                        a = (unsigned short)(inData[ j ][ i * nr[oldIndex] + k ]);
                    }

                    if(ui->matiCheckBox->isChecked() && oldIndex == ns - 1)
                    {

                        fwrite(&a, sizeof(unsigned short), 1, edfNew);
                    }
                    else
                    {
                        fwrite(&a, sizeof(short), 1, edfNew);
                    }
                }
            }
        }
    }
    delete []helpCharArr;
    fclose(edfNew);


    delete [] physMin;
    delete [] physMax;
    delete [] digMin;
    delete [] digMax;
//    cout << "writeEDF: output path = " << outFilePath.toStdString() << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl;

}

void MainWindow::ICA() //fastICA
{
    //we have data[ns][ndr*nr], ns, ndr, nr
    //at first - whiten signals using eigen linear superposition to get E as covMatrix
    //then count matrixW

    //data = A * comps, comps = W * data

    //count components = matrixW*data and write to ExpName_ICA.edf
    //count inverse matrixA = matrixW^-1 and draw maps of components
    //write automatization for classification different sets of components, find best set, explain
    QTime wholeTime;
    wholeTime.start();
    QTime myTime;
    myTime.start();

    QStringList lst;
    QString helpString;

    helpString = dir->absolutePath() + QDir::separator() + ExpName + ".edf";
    cout << "Ica started: " << helpString.toStdString() << endl;

    readData();
    //check reduceChannelsLineEdit for write edf

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
//    for(int i = 0; i < lst.length(); ++i)
//    {
//        cout << label[lst[i].toInt()-1] << endl;
//    }

    if(!QString(label[lst[lst.length() - 1].toInt() - 1]).contains("Markers"))
    {
//        QMessageBox::critical(this, tr("Error"), tr("bad channels list"), QMessageBox::Ok);
        cout << "ICA: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }
    ns = ui->numOfIcSpinBox->value(); //generality. Bind to reduceChannelsLineEdit?

    double eigenValuesTreshold = pow(10., -ui->svdDoubleSpinBox->value());
    double vectorWTreshold = pow(10., -ui->vectwDoubleSpinBox->value());

    int fr = nr[0];


//    dataICA - matrix of data
//    centeredMatrix - matrix of centered data: data[i][j] -= average[j]
//    random quantity is a amplitude-vector of ns dimension
//    there are ndr*nr samples of this random quantity
//        covMatrix - matrix of covariations of different components of the random quantity. Its size is ns*ns
//        sampleCovMatrix - matrix of sample covariations - UNUSED
//        similarMatrix - matrix of similarity
//        differenceMatrix - matrix of difference, according to some metric

//    cout << "ns = " << ns << endl;
//    cout << "ndr*fr = " << ndr*fr << endl;

    double ** covMatrix = new double * [ns];
    double ** centeredMatrix = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        covMatrix[i] = new double [ns];
    }
    for(int i = 0; i < ns; ++i)
    {
        centeredMatrix[i] = new double [ndr*fr];
    }

    //vectors for the las stage
    double ** vectorW = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        vectorW[i] = new double [ns];
    }
    double * vector1 = new double [ns];
    double * vector2 = new double [ns];
    double * vector3 = new double [ns];
    double * vectorOld = new double [ns];

    //for full A-matrix count
    double ** matrixA = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        matrixA[i] = new double [ns];
    }
    double * tempVector = new double [ns];

    //components time-flow
    double ** components = new double * [ns + 1]; //+1 for markers channel
    for(int i = 0; i < ns + 1; ++i)
    {
        components[i] = new double [ndr*fr];
    }
    double ** dataICA;
    double * averages;
    double * eigenValues;
    double ** eigenVectors;
    double * tempA;
    double * tempB;


    //wtf is this?
    int numOfMark = 0;
    while(1)
    {
        if(QString(label[numOfMark]).contains("Markers")) break;
        ++numOfMark;
    }
    for(int i = 0; i < ndr*fr; ++i)
    {
        components[ns][i] = data[numOfMark][i];
    }


    //count covariations
    //count averages
    averages = new double [ns];
    for(int i = 0; i < ns; ++i)
    {
        averages[i] = mean(data[i], ndr*fr);
    }

    //count zeros
    int h = 0;
    int Eyes = 0;
    for(int i = 0; i < ndr*fr; ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(fabs(data[j][i]) == 0.) ++h;
        }
        if(h == ns) Eyes += 1;
    }
    double realSignalFrac = (double(ndr*fr - Eyes)/(ndr*fr));

    //subtract averages
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            if(data[i][j] != 0.) data[i][j] -= averages[i] / realSignalFrac;
        }
    }



    //covariation between different spectra-bins
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            covMatrix[i][j] = 0.;
            for(int k = 0; k < ndr*fr; ++k)
            {
                covMatrix[i][j] += data[i][k] * data[j][k];
            }
            covMatrix[i][j] /= (ndr*fr); //should be -1 ?
        }
    }

    //test covMatrix symmetric
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            if(covMatrix[i][j] != covMatrix[j][i]) cout << i << " " << j << " warning" << endl;
        }
    }


    //count eigenvalues & eigenvectors of covMatrix
    eigenValues = new double [ns];
    eigenVectors = new double * [ns]; //vector is a coloumn
    for(int j = 0; j < ns; ++j)
    {
        eigenVectors[j] = new double [ns];
    }

    tempA = new double [ns]; //i
    tempB = new double [ndr*fr]; //j
    double sum1, sum2; //temporary help values
    double dF, F;
    int counter;
    double trace = 0.;

    for(int j = 0; j < ns; ++j)
    {
        trace += covMatrix[j][j];
    }
//    cout << "trace covMatrix = " << trace << endl;


//    cout << "start eigenValues processing" << endl;
    //count eigenValues & eigenVectors


    //array for components
    dataICA = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        for(int j = 0; j < ndr*fr; ++j)
        {
            dataICA[i][j] = data[i][j];
        }
    }
    int numOfPc = 0;



    //counter j - for B, i - for A
    for(int k = 0; k < ns; ++k)
    {
        myTime.restart();
        dF = 1.0;
        F = 1.0;

        //set 1-normalized vector tempA
        for(int i = 0; i < ns; ++i)
        {
            tempA[i] = 1./sqrt(ns);
        }
        for(int j = 0; j < ndr*fr; ++j)
        {
            tempB[j] = 1./sqrt(ndr*fr);
        }



        //approximate P[i] = tempA x tempB;
        counter = 0;
//        cout<<"curr val = "<<k<<endl;
        while(1) //when stop approximate?
        {
            //countF - error
            F = 0.;
            for(int i = 0; i < ns; ++i)
            {
                for(int j = 0; j < ndr*fr; ++j)
                {
                    F += 0.5*(dataICA[i][j]-tempB[j]*tempA[i])*(dataICA[i][j]-tempB[j]*tempA[i]);
                }
            }
            //count vector tempB
            for(int j = 0; j < ndr*fr; ++j)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int i = 0; i < ns; ++i)
                {
                    sum1 += dataICA[i][j]*tempA[i];
                    sum2 += tempA[i]*tempA[i];
                }
                tempB[j] = sum1/sum2;
            }

            //count vector tempA
            for(int i = 0; i < ns; ++i)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int j = 0; j < ndr*fr; ++j)
                {
                    sum1 += tempB[j]*dataICA[i][j];
                    sum2 += tempB[j]*tempB[j];
                }
                tempA[i] = sum1/sum2;
            }

            dF = 0.;
            for(int i = 0; i < ns; ++i)
            {
                for(int j = 0; j < ndr*fr; ++j)
                {
                    dF += 0.5*(dataICA[i][j]-tempB[j]*tempA[i])*(dataICA[i][j]-tempB[j]*tempA[i]);

                }
//                cout << dF << " ";
            }
            dF = (F-dF)/F;
            ++counter;
            if(counter==150)
            {
//                cout<<"dF = "<<abs(dF)<<endl;
                break;
            }
            if(fabs(dF) < eigenValuesTreshold) break; //crucial cap

            qApp->processEvents();
            if(stopFlag == 1)
            {
                cout << "ICA stopped by user" << endl;
                stopFlag = 0;
                if(1)
                {
                    helpInt = 0;
                    //clear memory
                    for(int i = 0; i < ns; ++i)
                    {
                        delete [] covMatrix[i];
                        delete [] centeredMatrix[i];
                        delete [] eigenVectors[i];
                        delete [] vectorW[i];
                        delete [] matrixA[i];
                        delete [] components[i];
                        delete [] dataICA[i];
                    }
                    delete [] centeredMatrix;
                    delete [] covMatrix;
                    delete [] eigenVectors;
                    delete [] averages;
                    delete [] eigenValues;
                    delete [] tempA;
                    delete [] tempB;
                    delete [] vectorOld;
                    delete [] tempVector;
                    delete [] vectorW;
                    delete [] matrixA;
                    delete [] components[ns];
                    delete [] components;
                    delete [] dataICA;

                }
                return;
            }
        }
//        cout<<"val № " << k <<" dF = "<<abs(dF)<< " counter = " << counter << endl;

        //edit covMatrix
        for(int i = 0; i < ns; ++i)
        {
            for(int j = 0; j < ndr*fr; ++j)
            {
                dataICA[i][j] -= tempB[j]*tempA[i];
            }
        }

        //count eigenVectors && eigenValues
        sum1 = 0.;
        sum2 = 0.;
        for(int i = 0; i < ns; ++i)
        {
            sum1 += tempA[i]*tempA[i];
        }
        for(int j = 0; j < ndr*fr; ++j)
        {
            sum2 += tempB[j]*tempB[j];
        }
        for(int i = 0; i < ns; ++i)
        {
            tempA[i]/=sqrt(sum1);
            //test equality of left and right singular vectors
//            if((abs((tempB[i]-tempA[i])/tempB[i]))>threshold) cout << k << " " << i << " warning" << endl;  //till k==19 - OK
        }
        for(int j = 0; j < ndr*fr; ++j)
        {
            tempB[j] /= sqrt(sum2);
        }

        eigenValues[k] = sum1*sum2/double(ndr*fr-1.);



        sum1 = 0.;

        for(int i = 0; i <= k; ++i)
        {
            sum1 += eigenValues[i];
        }

        cout << "numOfPC = " << k << "\t";
        cout << "value = " << eigenValues[k] << "\t";
        cout << "disp = " << 100. * eigenValues[k]/trace << "\t";
        cout << "total = " << 100. * sum1/trace << "\t";
        cout << "iterations = " << counter << "\t";
        cout << myTime.elapsed()/1000. << " sec" << endl;


        for(int i = 0; i < ns; ++i)
        {
            eigenVectors[i][k] = tempA[i]; //1-normalized
        }

    }
    numOfPc = ns;




    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName + "_eigenMatrix.txt";
    outStream.open(helpString.toStdString().c_str());
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            outStream << eigenVectors[i][j] << "  ";
        }
        outStream << endl;
    }
    outStream << endl;
    outStream.close();



    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName + "_eigenValues.txt";
    outStream.open(helpString.toStdString().c_str());
    for(int i = 0; i < ns; ++i)
    {
        outStream << eigenValues[i] << '\n';
    }
    outStream.close();



//    cout << "eigenVectors dot product= " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            sum1 = 0.;
//            for(int k = 0; k < ns; ++k)
//            {
//                sum1 += eigenVectors[i][k] * eigenVectors[j][k];

//            }
//            sum1 = int(sum1*100)/100.;
//            cout << sum1 << "\t";
//        }
//        cout << endl;
//    }
//    cout<<endl;




//    //matrix E * D^-0.5 * Et
//    double ** tempMatrix = new double * [ns];
//    for(int i = 0; i < ns; ++i)
//    {
//        tempMatrix[i] = new double [ns];
//    }

//    double * tempVector = new double [ns];

//    for(int i = 0; i < ns; ++i) //columns Et
//    {
//        for(int k = 0; k < ns; ++k) //rows E
//        {

//            tempVector[k] = 0.;
//            for(int s = 0; s < ns; ++s) //counter
//            {
//                tempVector[k] += /*eigenVectors[k][s] * */ eigenVectors[s][i] / sqrt(eigenValues[k]);
//            }
//        }
//        for(int s = 0; s < ns; ++s) //counter
//        {
//            tempMatrix[s][i] = tempVector[s];
//        }

//    }

    //count linear decomposition on PCAs
    for(int j = 0; j < ndr*fr; ++j) //columns initData
    {
        for(int i = 0; i < ns; ++i) //rows tempMatrix
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k) //rows initData = coloumns tempMatrix
            {
                sum1 += eigenVectors[k][i] * data[k][j] / sqrt(eigenValues[i]);
            }
            components[i][j] = sum1;
        }
    }



    for(int j = 0; j < ndr*fr; ++j) //columns X
    {
        for(int i = 0; i < ns; ++i) //rows tempMatrix
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += eigenVectors[i][k] * components[k][j];
            }
            dataICA[i][j] = sum1;
        }
    }
//    cout << "linear decomposition counted" << endl;



    //now dataICA are uncovariated signals with variance 1
    //test of covMatrix dataICA
//    cout << "covMatrixICA = " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            covMatrix[i][j] = 0.;
//            for(int k = 0; k < ndr*fr; ++k)
//            {
//                covMatrix[i][j] += dataICA[i][k] * dataICA[j][k];
//            }
//            covMatrix[i][j] /= ( ndr*fr - 1 );
//            covMatrix[i][j] = int(covMatrix[i][j]*100)/100.;
//            cout << covMatrix[i][j] << " ";
//        }
//        cout<<endl;
//    }
//    cout<<"covMatrixICA counted"<<endl<<endl;

    //ICA itself!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //fastIca wiki - first function
    for(int i = 0; i < ns; ++i) //number of current vectorW
    {
        myTime.restart();
        counter = 0;

        memcpy(vectorW[i], randomVector(ns), ns*sizeof(double));

        while(1)
        {
            for(int j = 0; j < ns; ++j)
            {
                vectorOld[j] = vectorW[i][j];
            }
            product1(dataICA, ndr*fr, ns, vectorW[i], &vector1);
            product2(dataICA, ndr*fr, ns, vectorW[i], &vector2);
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] = vector1[j] - vector2[j];
            }

            //orthogonalization
            product3(vectorW, ns, i, &vector3);
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] -= vector3[j];
            }

            //check norma
            for(int k = 0; k < i; ++k)
            {
                sum1 = 0.;
                for(int h = 0; h < ns; ++h)
                {
                    sum1 += vectorW[k][h]  *vectorW[k][h];
                }
                if(fabs(sum1 - 1.) > 0.01)
                {
                    cout << i << "'th vector not 1-l" << endl;
                }
            }
            //check ortho
            for(int k = 0; k < i; ++k)
            {
                sum1 = 0.;
                for(int h = 0; h < ns; ++h)
                {
                    sum1 += vectorW[k][h] * vectorW[i][h];
                }
                if(sum1 > 0.01)
                {
                    cout << i << "'th and " << k << "'th  vectors not ortho" << endl;
                }

            }

            sum2 = 0.;
            sum1 = 0.;

            //normalization
            for(int j = 0; j < ns; ++j)
            {
                sum1 += vectorW[i][j] * vectorW[i][j];
            }
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] /= sqrt(sum1);
            }


            //check norma difference
            for(int j = 0; j < ns; ++j)
            {
                sum2 += (vectorOld[j] - vectorW[i][j]) * (vectorOld[j] - vectorW[i][j]);
            }
            sum2 = sqrt(sum2);
            ++counter;
            if(sum2 < vectorWTreshold || 2 - sum2 < vectorWTreshold) break;
            if(counter == 300) break;

            /*
            qApp->processEvents();
            if(stopFlag == 1)
            {
                cout << "ICA stopped by user" << endl;
                stopFlag = 0;
                if(1)
                {
                    //clear memory
                    for(int i = 0; i < ns; ++i)
                    {
                        delete [] covMatrix[i];
                        delete [] centeredMatrix[i];
                        delete [] eigenVectors[i];
                        delete [] vectorW[i];
                        delete [] matrixA[i];
                        delete [] components[i];
                        delete [] dataICA[i];
                    }
                    delete [] centeredMatrix;
                    delete [] covMatrix;
                    delete [] eigenVectors;
                    delete [] averages;
                    delete [] eigenValues;
                    delete [] tempA;
                    delete [] tempB;
                    delete [] vector1;
                    delete [] vector2;
                    delete [] vector3;
                    delete [] vectorOld;
                    delete [] tempVector;
                    delete [] vectorW;
                    delete [] matrixA;
                    delete [] components[ns];
                    delete [] components;
                    delete [] dataICA;

                }
                return;
            }
            */
        }
        cout << "NumOf vectorW component = " << i << "\t";
        cout << "iterations = " << counter << "\t";
        cout << "error = " << fabs(sum2 - int(sum2+0.5)) << "\t";
        cout << "time = " << myTime.elapsed()/1000. << " sec" << endl;

    }
//    cout << "VectorsW counted" << endl;



//    //test vectorsW - ok
//    cout << "Mixing matrix = " << endl; //A, W^-1
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            cout << vectorW[j][i] << "\t";
//        }
//        cout << endl;
//    }

    //test orthoNorm VectorsW  -OK
//    cout << "test W*W^t = " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            sum1 = 0.;
//            for(int k = 0; k < ns; ++k)
//            {
//                sum1 += vectorW[i][k] * vectorW[j][k];
//            }
//            sum1 = int(sum1*100)/100.;
//            cout << sum1 << " ";
//        }
//        cout << endl;
//    }
//    cout<<endl;

    //count components
    matrixProduct(vectorW, dataICA, &components, ns, ndr*fr);
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ndr*fr; ++j)
//        {
//            sum1 = 0.;
//            for(int k = 0; k < ns; ++k)
//            {
//                sum1 += vectorW[i][k] * dataICA[k][j]; //initial
//            }
//            components[i][j] = sum1;
//        }
//    }


    //count full mixing matrix A = E * D^0.5 * Et * Wt
    //X = AS (sensor data = A*components)
    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] = vectorW[k][i]; //A = Wt
        }
    }

    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            tempVector[k] = 0.; //new i-th coloumn for matrix A
            for(int s = 0; s < ns; ++s)
            {
                tempVector[k] += eigenVectors[s][k] * matrixA[s][i]; //A = Et * Wt
            }
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] = tempVector[k];
        }
    }
    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(eigenValues[i]);//A = D^0.5 * Et * Wt
        }
    }

    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            tempVector[k] = 0.; //new i-th coloumn for matrix A
            for(int s = 0; s < ns; ++s)
            {
                tempVector[k] += eigenVectors[k][s] * matrixA[s][i]; //A = Et * Wt
            }
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] = tempVector[k];
        }
    }


    //test matrix A

    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += matrixA[i][k] * components[k][j];
            }
            if(fabs((data[i][j] - sum1)/data[i][j]) > 0.05 && fabs(data[i][j]) > 0.5)
            {
                cout << i << "\t" << j << "\t" << fabs((data[i][j] - sum1)/data[i][j]) << "\t"<< data[i][j] << endl;
            }
        }
    }

//    cout << "correlations of A*comps[i] and data[i]:" << endl;
//    matrixProduct(matrixA, components, &dataICA, ns, ndr*fr);
//    for(int i = 0; i < ns; ++i)
//    {
//        cout << correlation(dataICA[i], data[i], ndr*fr) << endl;
//    }







//    //norm components - by 1-length vector of mixing matrix
//    for(int i = 0; i < ns; ++i)
//    {
//        sum1 = 0.;

//        for(int k = 0; k < ns; ++k)
//        {
//            sum1 += matrixA[k][i] * matrixA[k][i];
//        }
//        sum1 = sqrt(sum1);
//        for(int k = 0; k < ns; ++k)
//        {
//            matrixA[k][i] /= sum1;
//        }

//        for(int j = 0; j < ndr*fr; ++j)
//        {
//            components[i][j] *= sum1;
//        }
//    }


    //norm components - by equal dispersion ????????????????????
    double coeff = 1.5;
    for(int i = 0; i < ns; ++i)
    {
        sum1 = 0.;
        sum2 = 0.;

        sum1 = mean(components[i], ndr*fr);
        sum2 = variance(components[i], ndr*fr);

        for(int j = 0; j < ndr*fr; ++j)
        {
            if(components[i][j] != 0.) components[i][j] -= sum1 / realSignalFrac;
            components[i][j] /= sqrt(sum2);
            components[i][j] *= coeff;
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(sum2);
            matrixA[i][k] /= coeff;
        }
    }

    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += matrixA[i][k] * components[k][j];
            }
            if(fabs((data[i][j] - sum1)/data[i][j]) > 0.05 && fabs(data[i][j]) > 0.5)
            {
                cout << "after norm\t" << i << "\t" << j << "\t" << fabs((data[i][j] - sum1)/data[i][j]) << "\t"<< data[i][j] << endl;
            }
        }
    }



    //now matrixA is true Mixing matrix A*components = initialSignals

//    //test - ok
//    for(int i = 0; i < 30; ++i)
//    {
//        sum1 = 0.;
//        for(int k = 0; k < ns; ++k)
//        {
//            sum1 += matrixA[0][k] * components[k][i];
//        }
//        cout<<sum1<<" ";
//    }
//    cout<<endl<<endl;

//    for(int i = 0; i < 30; ++i)
//    {
//        cout<<data[0][i]<<" ";
//    }
//    cout<<endl;

    //now should draw amplitude maps OR write to file
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName + "_maps.txt");
    writeICAMatrix(helpString, matrixA, ns); //generality 19-ns


    helpString = dir->absolutePath() + QDir::separator() + ExpName + "_ica.edf";
    writeEdf(ui->filePathLineEdit->text(), components, helpString, ndr*nr[0]);
    cout << "ICA ended. Time elapsed = " << wholeTime.elapsed()/1000. << " sec" << endl;

    ns = ui->numOfIcSpinBox->value();
    for(int i = 0; i < ns; ++i)
    {
        delete [] covMatrix[i];
        delete [] centeredMatrix[i];
        delete [] eigenVectors[i];
        delete [] vectorW[i];
        delete [] matrixA[i];
        delete [] components[i];
        delete [] dataICA[i];
    }
    delete [] centeredMatrix;
    delete [] covMatrix;
    delete [] eigenVectors;
    delete [] averages;
    delete [] eigenValues;
    delete [] tempA;
    delete [] tempB;
    delete [] vectorOld;
    delete [] tempVector;
    delete [] vectorW;
    delete [] matrixA;
    delete [] components[ns];
    delete [] components;
    delete [] dataICA;
}


void MainWindow::ICsSequence(QString EDFica1, QString EDFica2, QString maps1Path, QString maps2Path, int mode)
{

    ////////////////////////////////////////////////////////////////////////////// TO REWORK ////////////////////////////////////////////////////
    //mode == 0 -> sequency by most stability
    //mode == 1 -> sequency by first File & no overwrite
    if(mode != 0 && mode != 1)
    {
        cout << "bad mode" << endl;
        return;
    }

    QTime myTime;
    myTime.start();
    //count cross-correlation by maps and spectra
    int ns_ = 19;

    double corrMap;
    double corrSpectr[3];
    int offset5hz;
    int offset20hz;
    QString helpString2;
    QString helpString;

    double ** dataFFT1;
    matrixCreate(&dataFFT1, 3, 247*19);
    double ** dataFFT2;
    matrixCreate(&dataFFT2, 3, 247*19);


    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");
    ui->reduceChannelsCheckBox->setChecked(false);
    ui->sliceWithMarkersCheckBox->setChecked(false);

    Spectre * sp;

    setEdfFile(EDFica1);
    cleanDirs();
    readData();
    sliceAll();
    sp = new Spectre(dir, ns_, ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
    for(int i = 0; i < 3; ++i)
    {
        helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName;
        switch(i)
        {
        case 0: {helpString += "_241.psa"; break;}
        case 1: {helpString += "_247.psa"; break;}
        case 2: {helpString += "_254.psa"; break;}
        }
        readSpectraFileLine(helpString, &dataFFT1[i], 19, 247);
    }



    setEdfFile(EDFica2);
    cleanDirs();
    readData();
    sliceAll();
    sp = new Spectre(dir, ns_, ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
    for(int i = 0; i < 3; ++i)
    {
        helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName;
        switch(i)
        {
        case 0: {helpString += "_241.psa"; break;}
        case 1: {helpString += "_247.psa"; break;}
        case 2: {helpString += "_254.psa"; break;}
        }
        readSpectraFileLine(helpString, &dataFFT2[i], 19, 247);
    }


    //sequence ICs
    double ** mat1;
    double ** mat2;
    matrixCreate(&mat1, ns_, ns_);
    matrixCreate(&mat2, ns_, ns_);

    //read matrices
    readICAMatrix(maps1Path, &mat1, ns_);
    readICAMatrix(maps2Path, &mat2, ns_);

    //transpose ICA maps
    matrixTranspose(&mat1, ns_);
    matrixTranspose(&mat2, ns_);

    QList<int> list1;
    QList<int> list2;
    list1.clear();
    list2.clear();

    int fftLength = 4096;
    offset5hz = 5./ (defaults::frequency/fftLength) - 1;
    offset20hz = 20./ (defaults::frequency/fftLength) + 1;

    struct ICAcoeff
    {
        double cMap;
        double cSpectr[3];
        double sumCoef;
    };
    ICAcoeff coeffs[ns_][ns_];

    struct ICAcorr
    {
        int num1;
        int num2;
        ICAcoeff coeff;
    };
    ICAcorr ICAcorrArr[ns_];

    double ** corrs;
    matrixCreate(&corrs, ns_, ns_);

    double tempDouble;
    int maxShift = 2; ////////////////////////////////////////////////////////////

    cout << "1" << endl;

    helpString.clear();
    for(int k = 0; k < ns_; ++k)
    {
        for(int j = 0; j < ns_; ++j)
        {
            corrMap = (correlationFromZero(mat1[k], mat2[j], ns_));
            corrMap = corrMap * corrMap;

            helpDouble = corrMap; /////////////////////////////////////////////////////////////////////////////////////
            coeffs[k][j].cMap = corrMap;

            for(int h = 0; h < 3; ++h)
            {
                corrSpectr[h] = 0.;
                for(int shift = -maxShift; shift <= maxShift; ++shift)
                {
                    corrSpectr[h] = fmax( fabs(correlation(dataFFT1[h] + k*247, dataFFT2[h] + j*247, 247, shift)), corrSpectr[h]);
                }
                corrSpectr[h] = corrSpectr[h] * corrSpectr[h];
                helpDouble += corrSpectr[h]; /////////////////////////////////////////////////////////////////////////////////////

                coeffs[k][j].cSpectr[h] = corrSpectr[h];
            }
            helpDouble = sqrt(helpDouble);
            corrs[k][j] = helpDouble;
            coeffs[k][j].sumCoef = helpDouble;
        }
    }

    //find best correlations
    int temp1;
    int temp2;
    for(int j = 0; j < ns_; ++j) //j pairs
    {
        tempDouble = 0.;
        for(int i = 0; i < ns_; ++i) //rows
        {
            if(list1.contains(i)) continue;
            for(int k = 0; k < ns_; ++k) //cols
            {
                if(list2.contains(k)) continue;
//                if(i == k) continue;  ////////////////////////////////////////////////////////////////////////////////////////////////////////
                if(corrs[i][k] > tempDouble)
                {
                    tempDouble = corrs[i][k];
                    temp1 = i;
                    temp2 = k;
                }
            }
        }



        if(j == ns_ - 1 && EDFica1 == EDFica2) ////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            for(int i = 0; i < ns_; ++i) //rows
            {
                if(list1.contains(i)) continue;
                list1 << i;
                list2 << i;

                ICAcorrArr[j].num1 = i;
                ICAcorrArr[j].num2 = i;


            }
            ICAcorrArr[j].coeff.cMap = 1;
            for(int h = 0; h < 3; ++h)
            {
                ICAcorrArr[j].coeff.cSpectr[h] = 1;
            }
            ICAcorrArr[j].coeff.sumCoef = 1;
            break;

        }




        list1 << temp1;
        list2 << temp2;
        ICAcorrArr[j].num1 = temp1;
        ICAcorrArr[j].num2 = temp2;
        ICAcorrArr[j].coeff.cMap = coeffs[temp1][temp2].cMap;
        for(int h = 0; h < 3; ++h)
        {
            ICAcorrArr[j].coeff.cSpectr[h] = coeffs[temp1][temp2].cSpectr[h];
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        tempDouble = ICAcorrArr[j].coeff.cMap;
        for(int h = 0; h < 3; ++h)
        {
            tempDouble += ICAcorrArr[j].coeff.cSpectr[h];
        }
        tempDouble = sqrt(tempDouble);


        ICAcorrArr[j].coeff.sumCoef = tempDouble;
    }







    double ** newMaps;
    matrixCreate(&newMaps, ns_, ns_);

    ui->sliceWithMarkersCheckBox->setChecked(true);
    ui->reduceChannelsCheckBox->setChecked(true);

    //sequence
    //1st file
    if(mode == 0)
    {
        helpString.clear();
        for(int k = 0; k < ns_; ++k)
        {
            helpString += QString::number( ICAcorrArr[k].num1 + 1) + " "; ///////////////////////////////by most stability
//            helpString += QString::number( k + 1 ) + " "; /////////////////////////////by first file
            for(int j = 0; j < ns_; ++j)
            {
                newMaps[k][j] = mat1[ICAcorrArr[k].num1][j]; /////////////////////////////////////////by most stability
//                newMaps[k][j] = mat1[k][j]; ////////////////////////////////////////////////////by first file
            }
            //        memcpy(newMaps[k], mat1[ICAcorrArr[k].num1], ns_*sizeof(double));
        }
        matrixTranspose(&newMaps, ns_);
        helpString2 = maps1Path;
        helpString2.replace("_maps.txt", "_newSeq_maps.txt");
        writeICAMatrix(helpString2, newMaps, ns_);

        helpString += "20";
        ui->reduceChannelsLineEdit->setText(helpString);
        cout << helpString.toStdString() << endl;

        setEdfFile(EDFica1);
        cleanDirs();
        sliceAll();
        helpString2 = EDFica1;
        helpString2.replace(".edf", "_newSeq.edf");
        constructEDF(helpString2);
        //    sp = new Spectre(dir, ns_, ExpName);
        //    sp->countSpectra();
        //    sp->compare();
        //    sp->compare();
        //    sp->compare();
        //    sp->psaSlot();
        //    sp->close();
        //    delete sp;
    }

    //second file
    helpString.clear();
    for(int k = 0; k < ns_; ++k)
    {
        if(mode == 0)
        {
            helpString += QString::number( ICAcorrArr[k].num2 + 1) + " "; ///////////////////////////////////////////////////////////by most stability
        }
        else if(mode == 1)
        {
            helpString += QString::number( list2[ list1.indexOf(k) ] + 1) + " "; /////////////////////////////by first file
        }

        for(int j = 0; j < ns_; ++j)
        {        if(mode == 0)
            {
                newMaps[k][j] = mat2[ICAcorrArr[k].num2][j]; ///////////////////////////////////////////////////////////by most stability
            }
            else if(mode == 1)
            {
                newMaps[k][j] = mat2[ list2[list1.indexOf(k)] ][j]; /////////////////////////////by first file
            }
        }
//        memcpy(newMaps[k], mat1[ICAcorrArr[k].num2], ns_*sizeof(double));
    }
    matrixTranspose(&newMaps, ns_);
    helpString2 = maps2Path;
    helpString2.replace("_maps.txt", "_newSeq_maps.txt");
    writeICAMatrix(helpString2, newMaps, ns_);

    helpString += "20";
    ui->reduceChannelsLineEdit->setText(helpString);
    cout << helpString.toStdString() << endl;
    setEdfFile(EDFica2);
    cleanDirs();
    sliceAll();
    helpString2 = EDFica2;
    helpString2.replace(".edf", "_newSeq.edf");
    constructEDF(helpString2);





    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);
    ui->reduceChannelsComboBox->setCurrentText("20");
    helpString2 = EDFica1;
    if(mode == 0)
    {
        helpString2.replace(".edf", "_newSeq.edf");
    }
    setEdfFile(helpString2);
    cleanDirs();
    sliceAll();
    sp = new Spectre(dir, ns_, ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;

    helpString2 = EDFica2;
    helpString2.replace(".edf", "_newSeq.edf");
    setEdfFile(helpString2);
    cleanDirs();
    sliceAll();
    sp = new Spectre(dir, ns_, ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;



    cout << endl;
    outStream.open("/media/Files/Data/AB/12", ios_base::out|ios_base::app);
    outStream << ExpName.left(3).toStdString() << endl;
    for(int k = 0; k < ns_; ++k)
    {
        cout << k+1 << "\t";
        cout << ICAcorrArr[k].num1 + 1 << "\t";
        cout << ICAcorrArr[k].num2 + 1 <<  "\t";
        cout << doubleRound(sqrt(ICAcorrArr[k].coeff.cMap), 3) <<  "\t";

        for(int h = 0; h < 3; ++h)
        {
            cout << doubleRound(sqrt(ICAcorrArr[k].coeff.cSpectr[h]), 3) <<  "\t";
        }
        cout << doubleRound(ICAcorrArr[k].coeff.sumCoef, 3) << endl;


        outStream << k+1 << "\t";
        outStream << ICAcorrArr[k].num1 + 1 << "\t";
        outStream << ICAcorrArr[k].num2 + 1 <<  "\t";
        outStream << doubleRound(sqrt(ICAcorrArr[k].coeff.cMap), 3) <<  "\t";

        for(int h = 0; h < 3; ++h)
        {
            outStream << doubleRound(sqrt(ICAcorrArr[k].coeff.cSpectr[h]), 3) <<  "\t";
        }
        outStream << doubleRound(ICAcorrArr[k].coeff.sumCoef, 3) << endl;
    }


    outStream.close();

    //leave only 7 high-score components


    matrixDelete(&mat1, ns_);
    matrixDelete(&mat2, ns_);
    matrixDelete(&newMaps, ns_);
    matrixDelete(&corrs, ns_);

    matrixDelete(&dataFFT1, 3);
    matrixDelete(&dataFFT2, 3);

    cout << "ICsSequence ended. Time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
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

void MainWindow::icaClassTest() //non-optimized
{
    QString helpString2;
    QString helpString;

    int tempIndex = -1;
    //load ica file
    readData();

    int fr = nr[0]; // generality


    double ** dataICA = new double * [ns]; //with markers
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        for(int j = 0; j < ndr*fr; ++j)
        {
            dataICA[i][j] = data[i][j];
        }
    }

    int numOfIC = ui->numOfIcSpinBox->value();

    double ** matrixA = new double * [numOfIC];
    for(int i = 0; i < numOfIC; ++i)
    {
        matrixA[i] = new double [numOfIC];
    }

    helpString2 = ExpName;
    helpString2.replace("_ica", "");
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + helpString2 + "_maps.txt");
    if(!readICAMatrix(helpString, &matrixA, numOfIC))
    {
        return;
    }

    Spectre * spectr;// = Spectre(dir, numOfIC, ExpName);
    helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth";
    Net * ANN = new Net(dir, numOfIC, left, right, spStep, ExpName);
    helpString = dir->absolutePath() + QDir::separator() + "16sec19ch.net";
    ANN->loadCfgByName(helpString);
    ANN->setReduceCoeff(10.);
    double tempAccuracy;
    double currentAccuracy;
    double initAccuracy;
    this->ui->cleanRealisationsCheckBox->setChecked(true);
    QList<int> thrownComp;
    thrownComp.clear();
    QList<int> stayComp;
    stayComp.clear();


    for(int i = 0; i < thrownComp.length(); ++i)
    {
        cout << thrownComp[i] << endl;
    }

    for(int j = 0; j < fr*ndr; ++j)
    {
        for(int i = 0; i < numOfIC; ++i)
        {
            helpDouble = 0.;
            for(int k = 0; k < numOfIC; ++k)
            {
                if(!thrownComp.contains(k)) helpDouble += matrixA[i][k] * dataICA[k][j];
            }
            data[i][j] = helpDouble;
        }
    }

    cleanDirs();
    sliceOneByOneNew(numOfIC);

    spectr = new Spectre(dir, numOfIC, ExpName);

    spectr->countSpectra();

    ANN->setNumOfPairs(30);
    ANN->autoClassificationSimple();
    tempAccuracy = ANN->getAverageAccuracy();
    cout << "initAccuracy = " << tempAccuracy << endl;

    tempIndex = -1;
    for(int j = numOfIC; j > 0; --j) //num of components left
    {
        tempIndex = -1;
        initAccuracy = tempAccuracy;
        for(int i = 0; i < numOfIC; ++i)
        {
            if(thrownComp.contains(i)) continue;
            if(stayComp.contains(i)) continue;

            thrownComp.push_back(i);

            for(int j = 0; j < fr*ndr; ++j)
            {
                for(int i = 0; i < numOfIC; ++i)
                {
                    helpDouble = 0.;
                    for(int k = 0; k < numOfIC; ++k)
                    {
                        if(!thrownComp.contains(k)) helpDouble += matrixA[i][k] * dataICA[k][j];
                    }
                    data[i][j] = helpDouble;
                }
            }

            cleanDirs();
            sliceOneByOneNew(numOfIC);

            spectr->defaultState();
            spectr->countSpectra();

            ANN->autoClassificationSimple();

            currentAccuracy = ANN->getAverageAccuracy();
            cout << "AverageAccuracy " << i << " = " << currentAccuracy << endl;
            thrownComp.removeLast();

            if(currentAccuracy > initAccuracy + 1.5)
            {
                tempIndex = i;
                tempAccuracy = currentAccuracy;
                break;
            }
            else if(currentAccuracy > initAccuracy + 0.3 && currentAccuracy > tempAccuracy)
            {
                tempIndex = i;
                cout << tempIndex << "'th component saved as temp" <<endl;
                tempAccuracy = currentAccuracy;
            }
            else if(currentAccuracy < initAccuracy - 0.5)
            {
                stayComp.push_back(i);
                cout << tempIndex << "'th component is necessary" <<endl;
            }

        }
        if(tempIndex != -1)
        {
            cout << tempIndex << "'th component thrown" <<endl;
            thrownComp.push_back(tempIndex);
        }
        else
        {
            cout << "optimal components set:" << "\n";
            for(int i = 0; i < numOfIC; ++i)
            {
                if(!thrownComp.contains(i)) cout << i << "  ";
            }
            cout << "\n";
            cout << endl;
            break;
        }
    }
    delete ANN;
    delete spectr;
}

void MainWindow::throwIC()
{
    QStringList lst;
    QString helpString;

    //makes a signals-file from opened ICA file and appropriate matrixA file
    if(!ui->filePathLineEdit->text().contains("ica", Qt::CaseInsensitive))
    {
        cout << "bad ica file" << endl;
        return;
    }

    //load ica file
    readData();

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
    {
        QMessageBox::critical(this, tr("Error"), tr("bad channels list"), QMessageBox::Ok);
        return;
    }


    lst = ui->throwICLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    cout << "throwIC: going to throw " << lst.length() << " components - ";
    for(int i = 0; i < lst.length(); ++i)
    {
        cout << lst[i].toInt() << " ";
    }
    cout << "count from 1" << endl;




    int fr = nr[0]; // generality


    double ** dataICA = new double * [ns]; //with markers
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        for(int j = 0; j < ndr*fr; ++j)
        {
            dataICA[i][j] = data[i][j];
        }
    }

    int numOfIC = ui->numOfIcSpinBox->value(); // = 19

    double ** matrixA = new double * [numOfIC];
    for(int i = 0; i < numOfIC; ++i)
    {
        matrixA[i] = new double [numOfIC];
    }

    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName + "_maps.txt");
    readICAMatrix(helpString, &matrixA, numOfIC);

    QList<int> thrownComp;
    thrownComp.clear();


    for(int i = 0; i < lst.length(); ++i)
    {
        thrownComp << (lst[i].toInt() - 1);
    }

    for(int j = 0; j < fr*ndr; ++j)
    {
        for(int i = 0; i < numOfIC; ++i)
        {
            helpDouble = 0.;
            for(int k = 0; k < numOfIC; ++k)
            {
                if(!thrownComp.contains(k)) helpDouble += matrixA[i][k] * dataICA[k][j]; //generality -1
            }
            data[i][j] = helpDouble;
        }
    }

    ui->cleanRealisationsCheckBox->setChecked(true);
    cleanDirs();
    sliceOneByOneNew(ns);
    cout << "sliced" << endl;

    constructEDFSlot();

}

void MainWindow::transformEDF(QString inEdfPath, QString mapsPath, QString newEdfPath) // for 19 channels generality
{
    setEdfFile(inEdfPath);
    readData();

    double ** mat1;
    matrixCreate(&mat1, 19, 19);
    readICAMatrix(mapsPath, &mat1, 19);
    matrixInvert(&mat1, 19);

    double ** newData;
    matrixCreate(&newData, 20, ndr*nr[0]);
    matrixProduct(mat1, data, &newData, 19, ndr*nr[0]);
    memcpy(newData[19], data[19], ndr*nr[0]*sizeof(double));    //copy markers

    QList<int> chanList;
    chanList.clear();
    for(int i = 1; i <= 20; ++i)
    {
        chanList << i;
    }
    writeEdf(ui->filePathLineEdit->text(), newData, newEdfPath, ndr*nr[0], chanList);

    matrixDelete(&mat1, 19);
    matrixDelete(&newData, 20);
}

void MainWindow::transformReals() //move to library
{
    QStringList lst;
    QString helpString;

    helpString = QFileDialog::getOpenFileName(this, tr("choose maps file"), dir->absolutePath(), tr("*.txt"));
    if(helpString.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No file chosen"), QMessageBox::Ok);
        return;
    }


    double ** mat1;
    matrixCreate(&mat1, 19,19);
    readICAMatrix(helpString, &mat1, 19);


    double ** mat3;
    matrixCreate(&mat3, 19, 19);

    matrixInvert(mat1, 19, &mat3);

    double ** mat2;
    matrixCreate(&mat2, 19, 250*50);

    QString procDir = ui->drawDirBox->currentText();
    dir->cd(procDir);
    lst = dir->entryList(QDir::Files);
    dir->cdUp();

    qApp->processEvents();

    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = dir->absolutePath() + QDir::separator() + procDir + QDir::separator() + lst[i];


        inStream.open(helpString.toStdString().c_str());
        inStream.ignore(12); //"NumOfSlices "
        inStream >> NumOfSlices;
        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < 19; ++k)
            {
                inStream >> data[k][j];
            }
        }
        inStream.close();
//        cout << NumOfSlices << endl;

        matrixProduct(mat3, data, &mat2, 19, NumOfSlices);
//        cout << "product" << endl;

        helpString = dir->absolutePath() + QDir::separator() + procDir + QDir::separator() + lst[i];
        outStream.open((helpString).toStdString().c_str());
        outStream << "NumOfSlices " << NumOfSlices << '\n';
        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < 19; ++k)
            {
                outStream << mat2[k][j] << '\n';
            }
        }
        outStream.close();
        ui->progressBar->setValue(i*100./lst.length());
        qApp->processEvents();
    }
    ui->progressBar->setValue(0);

    matrixDelete(&mat1, 19);
    matrixDelete(&mat2, 19);
    matrixDelete(&mat3, 19);

}

void MainWindow::hilbertCount()
{
    QString helpString;
    readData();
    if(ui->reduceChannelsCheckBox->isChecked()) reduceChannelsFast();

    double fr = nr[0]; //generality

    double ** hilbertData = new double * [ns];
    hilbertPieces(data[0], 45536, fr, 9., 11., &hilbertData[0], "");

    cout << variance(hilbertData[0], 45536) << endl;
    hilbert(data[0], 65536, fr, 9., 11., &hilbertData[0], "");
    cout << variance(hilbertData[0], 65536) << endl;
    return;




    for(int i = 0; i < ns-1; ++i) //no markers
    {
//        hilbert(data[i], ndr*fr, fr, 5., 20., &hilbertData[i]);
    }
    memcpy(hilbertData[ns-1], data[ns-1], ndr*fr*sizeof(double)); //markers channel
    helpString = dir->absolutePath() + QDir::separator() + ExpName + "_hilbert.edf";
    writeEdf(ui->filePathLineEdit->text(), hilbertData, helpString, ndr*fr); ////////////////////////////need fix

    matrixDelete(&hilbertData, ns);
}

double objFunc(double *W_, double ***Ce_, double **Cz_, double **Cav_, double ns_, double numOfEpoches_)
{
    //count new r2
    double sum1 = 0.;
    double sum2 = 0.;
    double sum3 = 0.;

    for(int h = 0; h < numOfEpoches_; ++h)
    {
        sum1 = 0.;
        for(int j = 0; j < ns_; ++j)
        {
            for(int k = 0; k < ns_; ++k)
            {
                sum1 += ((Ce_[h][j][k] - Cav_[j][k]) * W_[j] * W_[k]);
            }
        }
        sum3 += pow(sum1, 2.);

    }
    sum3 /= numOfEpoches_;


    sum1 = 0.;
    sum2 = 0.;
    for(int j = 0; j < ns_; ++j)
    {
        for(int k = 0; k < ns_; ++k)
        {
            sum2 += (Cz_[j][k] * W_[j] * W_[k]);
        }
    }
    sum1 = pow(sum2, 2.);

    return sum1/sum3;
}

void MainWindow::spoc()
{
    QString helpString;

    readData();
    nsBackup = ns;
    ns = ui->numComponentsSpinBox->value(); //test

    double * W = new double  [ns];
    double * WOld = new double  [ns];
    int epochLength = ui->windowLengthSpinBox->value();
    timeShift = ui->timeShiftSpinBox->value();
    int numOfEpoches = (ndr*nr[0] - epochLength)/timeShift;

    double * Z = new double [numOfEpoches];

    double *** Ce = new double ** [numOfEpoches]; //covariance matrix
    for(int i = 0; i < numOfEpoches; ++i)
    {
        Ce[i] = new double * [ns];
        for(int j = 0; j < ns; ++j)
        {
            Ce[i][j] = new double [ns];
        }
    }

    double * averages = new double [ns];

    double ** Cav = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        Cav[i] = new double [ns];
    }

    double ** Cz = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        Cz[i] = new double [ns];
    }

    double * Znew = new double [numOfEpoches];

    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "spocVar.txt");
    FILE * in = fopen(helpString.toStdString().c_str(), "r");
    helpInt = 0;
    while(!feof(in) && helpInt < numOfEpoches)
    {
        fscanf(in, "%lf", &Z[helpInt++]);
    }
    fclose(in);

    //zero mean
    helpDouble = 0.;
    for(int i = 0; i < numOfEpoches; ++i)
    {
        helpDouble += Z[i];
    }
    helpDouble /= numOfEpoches;
    for(int i = 0; i < numOfEpoches; ++i)
    {
        Z[i] -= helpDouble;
    }

    //unit variance
    helpDouble = variance(Z, numOfEpoches);
    helpDouble = sqrt(helpDouble);
    for(int i = 0; i < numOfEpoches; ++i)
    {
        Z[i] /= helpDouble;
    }


    //count Ce
    for(int i = 0; i < numOfEpoches; ++i)
    {
        //count averages
        for(int j = 0; j < ns; ++j)
        {
//            averages[j] = 0.;
//            for(int h = 0; h < epochLength; ++h)
//            {
//                averages[j] += data[j][i * timeShift + h];
//            }
//            averages[j] /= epochLength;

            averages[j] = mean((data[j]+i*timeShift), epochLength);
        }

        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
//                helpDouble = 0.;
//                for(int h = 0; h < epochLength; ++h)
//                {
//                    helpDouble += (data[j][i * timeShift + h] - averages[j]) * (data[k][i * timeShift + h] - averages[k]);
//                }
//                helpDouble /= epochLength;
//                Ce[i][j][k] = helpDouble;

                Ce[i][j][k] = variance(data[j] + i*timeShift, epochLength);
            }
        }
    }

    //count Cav
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cav[j][k] = 0.;
        }
    }

    for(int i = 0; i < numOfEpoches; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                Cav[j][k] += Ce[i][j][k];
            }
        }
    }
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cav[j][k] /= numOfEpoches;
        }
    }

    //count Cz
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cz[j][k] = 0.;
        }
    }

    for(int i = 0; i < numOfEpoches; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                Cz[j][k] += Ce[i][j][k] * Z[i];
            }
        }
    }
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cz[j][k] /= numOfEpoches;
        }
    }

    double * gradientW = new double [ns];
    double sum1, sum2;
    double value1, value2;
    int counter1, counter2;
    double coeff = pow(10., -ui->spocCoeffDoubleSpinBox->value());
//    srand(time(NULL));


    if(ui->r2RadioButton->isChecked())
    {

        //W = randomVector
        for(int i = 0; i < ns; ++i)
        {
            W[i] = rand()%30 - 15.;
            sum1 += W[i] * W[i];
        }
        for(int i = 0; i < ns; ++i)
        {
            W[i] /= sqrt(sum1);
        }

        counter1 = 0;
        do
        {

            srand(time(NULL));
            sum1 = 1.;
            sum2 = 1.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 *= WOld[i];
                sum2 *= W[i];
            }

            for(int i = 0; i < ns; ++i)
            {
                WOld[i] = W[i];
            }

            if(sum1 == sum2)
            {
                srand(time(NULL));
                for(int i = 0; i < ns; ++i)
                {
                    W[i] = rand()%30 - 15.;
                    sum1 += W[i] * W[i];
                }
                for(int i = 0; i < ns; ++i)
                {
                    W[i] /= sqrt(sum1);
                }
            }

            //count gradient
            for(int i = 0; i < ns; ++i)
            {
                gradientW[i] = 0.;
            }


            for(int i = 0; i < ns; ++i)
            {
                //count <(wt*(Ce-Cav)w)^2>
                sum1 = 0.;

                for(int h = 0; h < numOfEpoches; ++h)
                {
                    sum2 = 0.;
                    for(int j = 0; j < ns; ++j)
                    {
                        for(int k = 0; k < ns; ++k)
                        {
                            sum2 += ((Ce[h][j][k] - Cav[j][k]) * W[j] * W[k]);
                        }
                    }
                    sum1 += pow(sum2, 2.);
                }
                sum1 /= numOfEpoches; //sum1 = <(wt*(Ce-Cav)w)^2>

                //count (wt*Cz*w)'
                sum2 = 0.;
                for(int j = 0; j < ns; ++j)
                {
                    sum2 += Cz[i][j] * W[j];
                }
                gradientW[i] = sum1 * sum2; //gradientW[i] = <(wt*(Ce-Cav)w)^2> * (wt*Cz*w)'

                //count <(wt*(Ce-Cav)*w) * (wt*(Ce-Cav)*w)'> * (wt * Cz * w)

                //1)
                //count <(wt*(Ce-Cav)*w) * (wt*(Ce-Cav)*w)'>
                helpDouble = 0.;
                for(int h = 0; h < numOfEpoches; ++h)
                {

                    sum1 = 0.;
                    sum2 = 0.;
                    for(int j = 0; j < ns; ++j)
                    {
                        for(int k = 0; k < ns; ++k)
                        {
                            sum1 += ((Ce[h][j][k] - Cav[j][k]) * W[j] * W[k]); //(wt*(Ce-Cav)*w)
                        }
                        sum2 += ((Ce[h][i][j] - Cav[i][j]) * W[j] ); //(wt*(Ce-Cav)*w)'
                    }
                    helpDouble += sum1 * sum2;

                }
                helpDouble /= numOfEpoches; // = <(wt*(Ce-Cav)*w) * (wt*(Ce-Cav)*w)'>

                //count (wt*Cz*w)
                sum1 = 0.;
                for(int j = 0; j < ns; ++j)
                {
                    for(int k = 0; k < ns; ++k)
                    {
                        sum1 += (Cz[j][k] * W[j] * W[k]);
                    }
                }
                gradientW[i] -= sum1 * helpDouble;

                sum2 = ((sum1 > 0.) - 0.5) * 2.; //sum2 = sign(sum1)
                gradientW[i] *= sum2;
            }



            //another count gradient
//            helpDouble = 1e-4;
//            for(int i = 0; i < ns; ++i)
//            {
//                W[i] += helpDouble;
//                gradientW[i] = objFunc(W, Ce,Cz,Cav,ns,numOfEpoches);
//                W[i] -= helpDouble;
//                gradientW[i] -= objFunc(W, Ce,Cz,Cav,ns,numOfEpoches);
//                gradientW[i] /= helpDouble;
//            }


            //1-norm gradient
            sum1 = 0.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 += gradientW[i] * gradientW[i];
            }
            sum1 = sqrt(sum1);
            for(int i = 0; i < ns; ++i)
            {
                gradientW[i] /= sum1;
            }

            //count objFunc before move
            value1 = objFunc(W, Ce, Cz, Cav, ns, numOfEpoches);

            value2 = value1;

            counter2 = 0;
            do
            {
                value1 = value2;
//                cout << value1 << " -> ";
                coeff = pow(10., -fmax(ui->spocCoeffDoubleSpinBox->value(), (value1 - 6.5)));

                for(int i = 0; i < ns; ++i)
                {
                    W[i] += gradientW[i] * coeff;
                }

                //count objFunc after move
                value2 = objFunc(W, Ce, Cz, Cav, ns, numOfEpoches);
//                cout << value2 << endl;

                ++counter2;


                qApp->processEvents();
                if(stopFlag)
                {

                    for(int i = 0; i < numOfEpoches; ++i)
                    {
                        for(int j = 0; j < ns; ++j)
                        {
                            delete []Ce[i][j];
                        }
                        delete []Ce[i];
                    }
                    delete []Ce;

                    for(int i = 0; i < ns; ++i)
                    {
                        delete []Cav[i];
                        delete []Cz[i];
                    }
                    delete []W;
                    delete []WOld;
                    delete []Cav;
                    delete []Cz;
                    delete []Z;
                    delete []Znew;
                    delete []averages;
                    delete []gradientW;
                    stopFlag = 0;
                    cout << "STOPPED" << endl;
                    return;
                }

            } while (value2 > value1 * 1.00);
            //step back
            for(int i = 0; i < ns; ++i)
            {
                W[i] -= gradientW[i] * coeff;
            }

            if(counter2 > 1)
            {
                cout << "value = " << value2 << "\t" << counter2 << " iterations" << endl;
            }

            sum1 = 0.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 += W[i] * W[i];
            }
            sum1 = sqrt(sum1);
            for(int i = 0; i < ns; ++i)
            {
                W[i] /= sum1 * ((W[2] > 0.)?1.:-1.); //generality
            }


            qApp->processEvents();
            if(stopFlag)
            {


                for(int i = 0; i < numOfEpoches; ++i)
                {
                    for(int j = 0; j < ns; ++j)
                    {
                        delete []Ce[i][j];
                    }
                    delete []Ce[i];
                }
                delete []Ce;

                for(int i = 0; i < ns; ++i)
                {
                    delete []Cav[i];
                    delete []Cz[i];
                }
                delete []W;
                delete []WOld;
                delete []Cav;
                delete []Cz;
                delete []Z;
                delete []Znew;
                delete []averages;
                delete []gradientW;
                stopFlag = 0;
                cout << "STOPPED" << endl;
                return;
            }


            sum1 = 0.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 += (W[i] - WOld[i]) * (W[i] - WOld[i]);
            }
            sum1 = sqrt(sum1);
            ++counter1;
        } while(sum1 > pow(10., -1.5) || value2 < ui->spocTresholdDoubleSpinBox->value());
        cout << "final correlation = " << value2 << "\t" << counter1 << " iterations" << endl;


        //1-norm W
        sum1 = 0.;
        for(int i = 0; i < ns; ++i)
        {
            sum1 += W[i] * W[i];
        }
        sum1 = sqrt(sum1);
        for(int i = 0; i < ns; ++i)
        {
            W[i] /= sum1 * ((W[2] > 0.)?1.:-1.); //generality
        }


        for(int i = 0; i < ns; ++i)
        {
            cout << W[i] << "   ";
        }
        cout << endl;

        spocWVector = new double [ns];
        for(int i = 0; i < ns; ++i)
        {
            spocWVector[i] = W[i];
        }

        for(int i = 0; i < ns; ++i)
        {
            W[i] = 0.;
            for(int j = 0; j < ns; ++j)
            {
                W[i] += spocWVector[j] * spocMixMatrix[j][i];
            }
        }

        sum1 = 0.;
        sum2 = 0.;
        for(int i = 0; i < ns; ++i)
        {
            if(fabs(W[i]) > sum1)
            {
                sum1 = fabs(W[i]);
                sum2 = (W[i] > 0.)?1:-1;
            }
        }
        for(int i = 0; i < ns; ++i)
        {
            W[i] /= sum1*sum2;
        }


        cout << "wt*MixMatrix = " << endl;
        for(int i = 0; i < ns; ++i)
        {
            cout << W[i] << "   ";
        }
        cout << endl;



    }
    /*
    else if (ui->lambdaRadioButton->isChecked())
    {
        double ** detMatrix = new double * [ns];
        for(int i = 0; i < ns; ++i)
        {
            detMatrix[i] = new double [ns];
        }
        double * dets = new double [ndr*nr[0]];

        //(Cz-l*Cav)*w = 0
        //matrixDet(Cz-l*Cav) = 0
        for(double L = 0.; L < ndr*nr[0]; L+=1.)
        {
            for(int j = 0; j < ns; ++j)
            {
                for(int k = 0; k < ns; ++k)
                {
                    detMatrix[j][k] = Cz[j][k] - L*Cav[j][k];
                }
            }
            dets[int(L)] = matrixDet(detMatrix, ns);
            if(dets[int(L)] * dets[int(L-1)] < 0 || ( L > 2. && (dets[int(L)] > dets[int(L-1)]) && (dets[int(L-2)] > dets[int(L-1)]) && dets[int(L-1)] < 10.))
            {
                cout << "L = " << L << endl;
            }
        }

        for(int i = 0; i < ns; ++i)
        {
            delete []detMatrix[i];
        }
        delete []detMatrix;
        delete []dets;
    }
*/






    for(int i = 0; i < numOfEpoches; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            delete []Ce[i][j];
        }
        delete []Ce[i];
    }
    delete []Ce;

    for(int i = 0; i < ns; ++i)
    {
        delete []Cav[i];
        delete []Cz[i];
    }
    delete []W;
    delete []WOld;
    delete []Cav;
    delete []Cz;
    delete []Z;
    delete []Znew;
    delete []averages;
    delete []gradientW;

}

void MainWindow::visualisation()   //just video
{


        QString helpString;
        ns=20;

        spLength = 63;


        char *helpCharArr = new char[50];
        int staSlice = 0;
        int NumOfSlices;
        setlocale(LC_NUMERIC, "C");

        helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append(".dat");
        FILE * datFile = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(),"r");
        if(!datFile)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open dat-file"), QMessageBox::Ok);
            return;
        }




        fscanf(datFile, "NumOfSlices %d", &NumOfSlices);

        double ** data = new double * [ns];
        for(int i = 0; i < ns; ++i)
        {
            data[i] = new double [NumOfSlices];
        }

        for(int i = 0; i < NumOfSlices; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
                fscanf(datFile, "%lf\n", &data[j][i]);
            }
        }
        fclose(datFile);


        delete []helpCharArr;


        int NumOfClasses=3;
        int NetLength=19*63;  //generality

        double ** weight = new double * [NumOfClasses];
        for(int i = 0; i < NumOfClasses; ++i)
        {
            weight[i] = new double [NetLength+1];
        }



        helpString = dir->absolutePath().append(QDir::separator()).append("weights.wts"); //wts Name generality
        FILE * wts=fopen(helpString.toStdString().c_str(),"r"); // path generality
        if(wts==NULL)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open wts-file"), QMessageBox::Ok);
            return;
        }

        for(int i = 0; i < NumOfClasses*(NetLength); ++i)
        {
            fscanf(wts, "%lf", &weight[i/(NetLength)][i%(NetLength)]);
        }
        for(int i = 0; i < NumOfClasses; ++i)
        {
            fscanf(wts, "%lf", &weight[i][NetLength]);
        }
        fclose(wts);


        dir->cd("visualisation");
        //generality
        dir->rmdir("video");
        dir->mkdir("video");

        dir->cd("video");



//        QStringList lst = dir->entryList(QDir::Files);
//        for(int i = 0; i < lst.length(); ++i)
//        {
//            remove(lst.at(i).toStdString().c_str());
//        }
        helpString = dir->absolutePath().append(QDir::separator()).append("visual.mpg");
        if(QFile::exists(helpString))
        {
            remove(helpString.toStdString().c_str());
        }


        QPixmap pic;
        QPainter * painter = new QPainter();


        double * spectre;
        spectre = new double [2*1024];  //generality


        double **dataFFT = new double * [ns];
        for(int k = 0; k < ns; ++k)
        {
            dataFFT[k] = new double [85];
        }

        double * matrix = new double [NetLength+1];

        double temperature = 1000.;

        int offsetTime = 0;  //in seconds
        int timeShift = 25;    //in slices
        int wholeLength = NumOfSlices/250-4;  //in seconds
        int length = 7;        //displayed window time in seconds

        int outNorm=40;       //graph. parameter

        int helpInt=ceil(wholeLength*250/double(timeShift));   //num of windows
    //    helpInt=300;

        double help1, help2;   //for smooth
        double ** output = new double * [NumOfClasses];

        for(int i = 0; i < NumOfClasses; ++i)
        {
            output[i] = new double [helpInt];
        }

//        cout << "start" << endl;

        int l;                  //
        int percentage = 0;

        for(int i = 0; i < 250*wholeLength; i += timeShift)

        {
//            cout << "1 " << i << endl;
            if((i-(staSlice+250*offsetTime))*100/(wholeLength*250)>percentage)
            {
                percentage=(i-(staSlice+250*offsetTime))*100/(wholeLength*250);
                cout << percentage << "% completed" << endl;
            }
            l=i-(staSlice+250*offsetTime);

//            cout << "2 " << i << endl;

            for(int k = 0; k < ns; ++k)
            {
                //count spectra
                for(int j = 0; j < 1024; ++j)
                {
                    spectre[2 * j ] = 0.;
                    spectre[2 * j + 1] = data[k][i+250*length-1024 + j];   //last ~4sec of length visual length of signal
                }
                four1(spectre-1, 1024, 1);

                for(int i = 0; i < 85; ++i )      //get the absolute value of FFT
                {
                    dataFFT[k][ i ] = ( spectre[ i * 2 +1] * spectre[ i * 2 +1] + spectre[ i * 2 + 1 +1] * spectre[ i * 2 + 1 +1] )*2*0.004/1024.; //generality
//                    dataFFT[k][ i ] = sqrt(dataFFT[k][ i ]);
                }

                //smooth spectra
                for(int a = 0; a < 3; ++a)
                {
                    help1=dataFFT[k][0];
                    for(int t=1; t < 85-1; ++t)
                    {
                        help2=dataFFT[k][t];
                        dataFFT[k][t]=(help1+help2+dataFFT[k][t+1])/3.;
                        help1=help2;
                    }
                }
            }

//            cout << "3 " << i << endl;

            for(int k = 0; k < ns; ++k)
            {
                for(int j = 0; j < 63; ++j) //generality spLength
                {
                    matrix[k*63+j] = dataFFT [k] [j+20];   //make a vector of appropriate spectra values
                }
            }
            matrix[NetLength]=1.;

//            cout << "4 " << i << endl;


            //count output
            for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
            {
                output[j][l/timeShift] = 0.;
                for(int i = 0; i < NetLength; ++i)
                {
                    output[j][l/timeShift] += weight[j][i]*matrix[i];
                }
                output[j][l/timeShift] += weight[j][NetLength]*matrix[NetLength];
                output[j][l/timeShift]=logistic(output[j][l/timeShift], temperature); // unlinear conformation
            }

//            cout << "5 " << i << endl;


            pic = QPixmap(length*100, 300);
            pic.fill();
            painter->begin(&pic);

            for(int j=i; j < i+250*length; ++j)
            {
                if(j < i+250*length-1024) painter->setPen("lightgray");
                else painter->setPen("black");


                //draw signals
                for(int k = 0; k < ns; ++k)
                {
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height()/(ns+3)*(k+1) - data[k][j], (j-i+1)/(250.*length)*pic.width(), pic.height()/(ns+3)*(k+1) - data[k][j+1]);
                }

                //draw outputs
                if(j>staSlice+250*offsetTime+250*length)
                {
                    int k=j-(staSlice+250*offsetTime+250*length);
                    painter->setPen(QPen(QBrush("blue"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height() - output[0][k/timeShift]*outNorm, (j-i+1)/(250.*length)*pic.width(),  pic.height() - output[0][(k+1)/timeShift]*outNorm);
                    painter->setPen(QPen(QBrush("red"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height() - output[1][k/timeShift]*outNorm, (j-i+1)/(250.*length)*pic.width(),  pic.height() - output[1][(k+1)/timeShift]*outNorm);
                    painter->setPen(QPen(QBrush("green"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height() - output[2][k/timeShift]*outNorm, (j-i+1)/(250.*length)*pic.width(),  pic.height() - output[2][(k+1)/timeShift]*outNorm);
                }

                //draw markers
                if(data[19][j]==241.)
                {
                    painter->setPen(QPen(QBrush("blue"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), 0, (j-i)/(250.*length)*pic.width(), pic.height());
                }
                if(data[19][j]==247.)
                {
                    painter->setPen(QPen(QBrush("red"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), 0, (j-i)/(250.*length)*pic.width(), pic.height());
                }
                if(data[19][j]==254.)
                {
                    painter->setPen(QPen(QBrush("green"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), 0, (j-i)/(250.*length)*pic.width(), pic.height());
                }

            }
//            cout << "6 " << i << endl;
            pic.save(dir->absolutePath().append(QDir::separator()).append(rightNumber(i, 7)).append(".jpg"), 0, 100);
            painter->end();
        }

        helpString = "cd ";
        helpString.append(dir->absolutePath()).append(" && mencoder \"mf://*.jpg\" -mf type=jpg -o ").append(dir->absolutePath().append(QDir::separator()).append("visual.mpg")).append(" -ovc lavc -lavcopts vcodec=msmpeg4v2"); //path generality
        system(helpString.toStdString().c_str());

        dir->cdUp();
        dir->cdUp();
        cout << dir->absolutePath().toStdString() << endl;


        delete []spectre;


        for(int k = 0; k < ns; ++k)
        {
            delete []dataFFT[k];
        }
        delete []dataFFT;


        for(int i = 0; i < NumOfClasses; ++i)
        {
            delete []output[i];
        }
        delete [] output ;


        delete []matrix;
}


void MainWindow::waveletCount()
{
    QString helpString;

    NumOfSlices=-1;

    FILE * file1;
    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(ExpName).append(".dat"));

    file1 = fopen(helpString.toStdString().c_str(),"r");
    if(file1 == NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Error"), tr("ExpName.dat file not found"), QMessageBox::Ok);
        return;
    }
    fscanf(file1, "NumOfSlices %d\n", &NumOfSlices);
    fclose(file1);

    if(ui->classBox->isChecked()) drawClassification();  //needs *.dat & weights.wts
    if(ui->weightsBox->isChecked()) drawWeights();       //needs         weights.wts

    if(ui->waveletsBox->isChecked())
    {
        for(int channel = 0; channel < 19; ++channel)
        {
            helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(ExpName).append(".dat"));
            file1 = fopen(helpString.toStdString().c_str(),"r");
            if(file1==NULL)
            {
                QMessageBox::critical((QWidget*)this, tr("Error"), tr("ExpName.dat file not found"), QMessageBox::Ok);
                break;
            }

            helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_wavelet_").append(QString::number(channel)).append(".jpg"));
            //                cout << helpString.toStdString() << endl;

            wavelet(helpString, file1, ns, channel, 20., 5., 0.98, 32);
            cout << channel << " wavelet drawn" << endl;
        }
    }

    if(ui->fullBox->isChecked())
    {
        QPixmap full(150*NumOfSlices/250 + 600, 800*20 + 50*19);
        QPainter *painter = new QPainter;
        full.fill();
        painter->begin(&full);

        QRectF target;
        QRectF source;
        QPixmap pic;

        //class
        target = QRectF(600, 0, 150*NumOfSlices/250, 800);
        helpString =  QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append("class.jpg"));
        pic = QPixmap(helpString);
        source = QRectF(pic.rect());
        painter->drawPixmap(target, pic, source);

        for(int i = 0; i < 19; ++i)
        {
            //void QPainter::drawPixmap ( const QRectF & target, const QPixmap & pixmap, const QRectF & source )

            //weights
            target = QRectF(0, (i+1)*(800 + 50), 600, 800);
            helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_weights_").append(QString::number(i)).append(".jpg"));
            pic = QPixmap(helpString);
            source = QRectF(pic.rect());
            painter->drawPixmap(target, pic, source);

            //wavelets
            target = QRectF(600, (i+1)*(800 + 50), 150*NumOfSlices/250, 800);
            helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_wavelet_").append(QString::number(i)).append(".jpg"));
            pic = QPixmap(helpString);
            source = QRectF(pic.rect());
            painter->drawPixmap(target, pic, source);
        }
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("full.jpg"));
        full.save(helpString, 0, 100);

        cout << "full picture drawn" << endl;

        painter->end();
        delete painter;
    }
    if(ui->visualisationBox->isChecked()) visualisation();
}


void MainWindow::drawWeights()
{
    QString helpString;

    //read wts
    spLength = 63;
    int NumOfClasses=3;
    int NetLength=19*spLength;

    double ** weight = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        weight[i] = new double [NetLength+1];
    }

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("weights.wts"));
    FILE * w = fopen(helpString.toStdString().c_str(),"r");
    if(w==NULL)
    {
        cout << "cannot open wts file" << endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open wts-file"), QMessageBox::Ok);
        return;
    }
    double maxWeight = 0.;
    for(int i = 0; i < NumOfClasses*(NetLength); ++i)
    {
        if(feof(w))
        {
            cout << "wts-file too small" << endl;

            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too small. Nothing happened"), QMessageBox::Ok);
            return;
        }
        fscanf(w, "%lf\n", &weight[i/(NetLength)][i%(NetLength)]);
        maxWeight = max(weight[i/(NetLength)][i%(NetLength)], maxWeight);
    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        if(feof(w))
        {
            cout << "wts-file too small" << endl;

            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too small. Nothing happened"), QMessageBox::Ok);
            return;
        }
        fscanf(w, "%lf\n", &weight[i][NetLength]);
    }
    if(!feof(w))
    {
        cout << "wts-file too big" << endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too long. Nothing happened"), QMessageBox::Ok);
        return;
    }
    fclose(w);



    QPixmap pic(600,800);   //generality 600
    QPainter * painter = new QPainter;
    pic.fill();
    painter->begin(&pic);

    int lineWidth=2;
    double norm = maxWeight/300.;

    for(int i = 0; i < 19; ++i)
    {
        pic.fill();
        for(int k = 0; k < NumOfClasses; ++k)
        {
            if(k == 0) painter->setPen(QPen(QBrush("blue"), lineWidth ));
            if(k == 1) painter->setPen(QPen(QBrush("red"), lineWidth ));
            if(k == 2) painter->setPen(QPen(QBrush("green"), lineWidth ));
            for(int j = 0; j < 63-1; ++j)
            {
                painter->drawLine(pic.width()/2-weight[k][i*63+j]/norm, pic.height()*(1.-j/63.), pic.width()/2-weight[k][i*63+(j+1)]/norm, pic.height()*(1.-(j+1)/63.));  //generality 63
            }
        }
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_weights_").append(QString::number(i)).append(".jpg"));
        pic.save(helpString, 0, 100);

    }
    painter->end();
    delete painter;

    cout << "weights drawn" << endl;


}

void MainWindow::drawClassification()  //needs *.dat & weights.wts
{
    QString helpString;


    FILE * file1;

    spLength = 63;

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(ExpName).append(".dat"));
    file1 = fopen(helpString.toStdString().c_str(),"r");
    fscanf(file1, "NumOfSlices %d\n", &NumOfSlices);
//    cout << "NumOfSlices=" << NumOfSlices << endl;
//    cout << "ns=" << ns << endl;

    data = new double *[ns];
    for(int i = 0; i < ns; ++i)
    {
        data[i] = new double [NumOfSlices];
    }

    for(int i = 0; i < NumOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fscanf(file1, "%lf", &data[j][i]);
//            cout << data[j][i] << endl;
        }
    }
//    cout << "read" << endl;

    //generality
    int NumOfClasses=3;
    int NetLength=19*spLength;
    int timeShift = 125;
    FILE * wts;

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("weights.wts")); //generality
    wts = fopen(helpString.toStdString().c_str(),"r");
    if(wts==NULL)
    {
        cout << "wts==NULL" << endl;
    }

    double ** weight = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        weight[i] = new double [NetLength+1];
    }
    for(int i = 0; i < NumOfClasses*(NetLength); ++i)
    {
        fscanf(wts, "%lf", &weight[i/(NetLength)][i%(NetLength)]);
//        cout << weight[i/(NetLength)][i%(NetLength)] << endl;

    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        fscanf(wts, "%lf", &weight[i][NetLength]);
    }
    fclose(wts);

//    cout << "wts read" << endl;

    double * spectre;
    spectre = new double [2*1024]; //generality


    double **dataFFT = new double * [19];
    for(int k = 0; k < 19; ++k)
    {
        dataFFT[k] = new double [85];
    }
    double * matrix = new double [NetLength+1];
    double ** output = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        output[i] = new double [int((NumOfSlices-1024)/timeShift + 10)]; //generality
    }
    double temperature = ui->tempBox->value();
    int l = 0;
    double outNorm = 0.8; //graphical parameter
    double help1, help2; // for smooth
    int lineWidth = 2;


    //for every window shifted with timeShift
    for(int i = 0; i < int(NumOfSlices-1024); i += timeShift) //generality
    {
        //count spectra
        for(int k = 0; k < 19; ++k)
        {
            for(int j = 0; j < 1024; ++j)
            {
                spectre[2 * j ] = 0.;
                spectre[2 * j + 1] = data[k][i+j];
            }
            four1(spectre-1, 1024, 1);
            for(int i = 0; i < 85; ++i )      //get the absolute value of FFT
            {
                dataFFT[k][ i ] = ( spectre[ i * 2 +1] * spectre[ i * 2 +1] + spectre[ i * 2 + 1 +1] * spectre[ i * 2 + 1 +1] )*2*0.004/1024.;
            }
            //smooth spectra
            for(int a = 0; a < 3; ++a) //generality numberOfSmooth
            {
                help1=dataFFT[k][0];
                for(int t=1; t < 85-1; ++t)
                {
                    help2=dataFFT[k][t];
                    dataFFT[k][t]=(help1+help2+dataFFT[k][t+1])/3.;
                    help1=help2;
                }

            }
        }
        for(int k = 0; k < 19; ++k)
        {
            for(int j = 0; j < spLength; ++j)
            {
                matrix[k*spLength+j] = dataFFT [k] [j+20];   //add reduce channels
            }
        }
        matrix[NetLength]=1.;

        //count output
        for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
        {
            output[j][l] = 0.;
            for(int i = 0; i < NetLength; ++i)
            {
                output[j][l] += weight[j][i]*matrix[i];
            }
            output[j][l] += weight[j][NetLength]*matrix[NetLength];
            output[j][l]=logistic(output[j][l], temperature); // unlinear conformation
        }

        ++l; //l=number of windows processed
    }

    --l;             //???


        QPixmap pic(150*NumOfSlices/250, 800);  //150 pix/sec
        QPainter * painter = new QPainter;
        pic.fill();
        painter->begin(&pic);

//        int lineWidth = 2;  //graphical parameter

        //for every window processed
        for(int j = 0; j < l-1; ++j)
        {
            painter->setPen("black");

            //draw output lines
            painter->setPen(QPen(QBrush("blue"), lineWidth ));
//            painter->drawLine((j+0.5)*(pic.width()/(l+1)), pic.height()*(1 - output[0][j]*outNorm), (j+1.5)*(pic.width()/(l+1)),  pic.height()*(1 - output[0][j+1]*outNorm));
//            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(1 - output[0][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(1 - output[0][j+1]*outNorm));
            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(outNorm - output[0][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(outNorm - output[0][j+1]*outNorm));


            painter->setPen(QPen(QBrush("red"), lineWidth ));
//            painter->drawLine((j+0.5)*(pic.width()/(l+1)), pic.height()*(1 - output[1][j]*outNorm), (j+1.5)*(pic.width()/(l+1)),  pic.height()*(1 - output[1][j+1]*outNorm));
            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(outNorm - output[1][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(outNorm - output[1][j+1]*outNorm));


            painter->setPen(QPen(QBrush("green"), lineWidth ));
//            painter->drawLine((j+0.5)*(pic.width()/(l+1)), pic.height()*(1 - output[2][j]*outNorm), (j+1.5)*(pic.width()/(l+1)),  pic.height()*(1 - output[2][j+1]*outNorm));
            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(outNorm - output[2][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(outNorm - output[2][j+1]*outNorm));

        }

        painter->setPen("black");
        painter->drawLine(0, pic.height()*(outNorm), pic.width(),  pic.height()*(outNorm));
//        painter->drawLine(0, pic.height()*(outNorm), pic.width(),  pic.height()*(outNorm));

//
        //check markers
        for(int i = 0; i < NumOfSlices; ++i)
        {
            //draw markers
            if(data[ns-1][i]==241.)
            {
                painter->setPen(QPen(QBrush("blue"), lineWidth ));
                painter->drawLine(i*pic.width()/NumOfSlices, 0, i*pic.width()/NumOfSlices, pic.height()); //j+0.5 ???
            }
            if(data[ns-1][i]==247.)
            {
                painter->setPen(QPen(QBrush("red"), lineWidth ));
                painter->drawLine(i*pic.width()/NumOfSlices, 0, i*pic.width()/NumOfSlices, pic.height()); //j+0.5 ???
            }
            if(data[ns-1][i]==254.)
            {
                painter->setPen(QPen(QBrush("green"), lineWidth ));
                painter->drawLine(i*pic.width()/NumOfSlices, 0, i*pic.width()/NumOfSlices, pic.height()); //j+0.5 ???
            }
        }

        painter->setFont(QFont("Helvetica", 32, -1, -1));
        painter->setPen(QPen(QBrush("black"), lineWidth ));
        for(int j = 0; j < int(NumOfSlices/250); ++j)
        {
            painter->drawLine(150*j, pic.height(), 150*j, pic.height()*(1.0-0.10));
            helpString.setNum(j);
            painter->drawText(150*j-5, pic.height()*(1.0-0.12), helpString);
        }

        pic.save(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append("class.jpg"), 0, 100);
        painter->end();
        delete painter;
        for(int i = 0; i < NumOfClasses; ++i)
        {
            delete []output[i];
        }
        delete []output;
        for(int k = 0; k < 19; ++k)
        {
            delete []dataFFT[k];
        }
        delete []dataFFT;
        delete []matrix;
        delete []spectre;
        cout << "classification drawn" << endl;

}


void MainWindow::randomDecomposition()
{
    QStringList lst;
    QString helpString;

    QString initName = ExpName;
    readData(); //should have opened initial data-file

    cout << "data read" << endl;

    int h = 0;
    int Eyes = 0;
    for(int i = 0; i < ndr*nr[0]; ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(fabs(data[j][i]) == 0.) ++h;
        }
        if(h == ns) Eyes += 1;
    }

    lst.clear();
    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst.last().toInt() - 1]).contains("Markers"))
    {
        QMessageBox::critical(this, tr("Doge"), tr("Bad Markers channel in rdc channel lineEdit"), QMessageBox::Ok);
        return;
    }


    int compNum = ui->numOfIcSpinBox->value();
    double ** randMatrix = matrixCreate(compNum, compNum);
    double ** matrixW = matrixCreate(compNum, compNum);
    double ** newData = matrixCreate(ns, ndr*nr[0]);

    double sum1;
    double sum2;

    Spectre * spectr = new Spectre(dir, compNum, ExpName);
    Net * ANN = new Net(dir, compNum, left, right, spStep, ExpName); //generality parameters
    helpString = dir->absolutePath() + QDir::separator() + "16sec19ch.net";
    ANN->loadCfgByName(helpString);

    ANN->setReduceCoeff(15.); //generality
    ANN->setNumOfPairs(50);   ////////////////////////////////////////////////////////////////////////////////////////////
    FILE * outFile;


    double ** eigenMatrix = matrixCreate(compNum, compNum);

    /*
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName + "_eigenMatrix.txt";
    inStream.open(helpString.toStdString().c_str());
    if(!inStream.good())
    {
        cout << "bad inStream eigenMatrix" << endl;
        return;
    }
    for(int i = 0; i < compNum; ++i)
    {
        for(int j = 0; j < compNum; ++j)
        {
            inStream >> eigenMatrix[i][j];
        }
    }
    inStream.close();
*/
    double ** eigenMatrixInv = matrixCreate(compNum, compNum);
    matrixTranspose(eigenMatrix, compNum, compNum, &eigenMatrixInv);

    double * eigenValues = new double [compNum];

    /*
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName + "_eigenValues.txt";
    inStream.open(helpString.toStdString().c_str());
    if(!inStream.good())
    {
        cout << "bad inStream eigenValues" << endl;
        return;
    }
    for(int i = 0; i < compNum; ++i)
    {
        inStream >> eigenValues[i];
    }
    inStream.close();
*/

    QTime myTime;
    myTime.start();

    for(int i = 0; i < 55; ++i) /////////////////////////////////////////////////////////////////////////////////
    {
        myTime.restart();
        helpString = dir->absolutePath() + QDir::separator() + initName;
        setEdfFile(helpString);
        readData(); //should have opened initial data-file


        //set random matrix
        srand(time(NULL));

        for(int i = 0; i < compNum; ++i) //i'th coloumn
        {
            sum1 = 0.;
            for(int j = 0; j < compNum; ++j)
            {
                randMatrix[j][i] = (-20 + rand()%41)/20.;
                sum1 += randMatrix[j][i] * randMatrix[j][i];
            }
            sum1 = sqrt(sum1);

            //norm raandMatrix by 1-length coloumns
            for(int j = 0; j < compNum; ++j)
            {
                randMatrix[j][i] /= sum1;
            }
        }



  /*
   * //make randon matrix in terms of matrixW after whitening the initial signals
        matrixProduct(randMatrix, eigenMatrix, &matrixW, compNum, compNum); //W = rand * Eig

        for(int i = 0; i < compNum; ++i)
        {
            sum1 = 0.;
            for(int j = 0; j < compNum; ++j)
            {
                matrixW[j][i] /= sqrt(eigenValues[i]);
            }
        }
        //W = rand * Eig * D^-0.5

        matrixProduct(matrixW, eigenMatrixInv, &randMatrix, compNum, compNum); //randMatrix = randW * Eig * d^-0.5 *Eig^t
*/
        //decompose
        matrixProduct(randMatrix, data, &newData, compNum, ndr*nr[0]);
        memcpy(newData[ns-1], data[ns-1], ndr*nr[0]*sizeof(double)); //markers

        //norm components and rand matrix
        for(int i = 0; i < compNum; ++i)
        {
            sum1 = mean(newData[i], ndr*nr[0]);
            for(int j = 0; j < ndr*nr[0]; ++j)
            {
                if(newData[i][j] != 0.) newData[i][j] -= sum1 * (double(ndr*nr[0])/(ndr*nr[0] - Eyes));
            }

            sum2 = sqrt(variance(newData[i], ndr*nr[0]));
            for(int j = 0; j < ndr*nr[0]; ++j)
            {
                newData[i][j] /= sum2;
                newData[i][j] *= 10.;  //10 generality
            }

            for(int k = 0; k < compNum; ++k)
            {
                randMatrix[k][i] *= sum2;
                randMatrix[k][i] /= 10.;
            }

        }

        //write newData
        helpString = dir->absolutePath() + QDir::separator() + ExpName + "_randIca.edf";
        writeEdf(ui->filePathLineEdit->text(), newData, helpString, ndr*nr[0]);

        helpString = dir->absolutePath() + QDir::separator() + ExpName + "_randIca.edf";
        setEdfFile(helpString);
        readData(); //read newData

        ui->cleanRealisationsCheckBox->setChecked(true);
        ui->cleanRealsSpectraCheckBox->setChecked(true);
        cleanDirs();

        sliceOneByOneNew(19); //sliceAll()
        spectr->defaultState();
        spectr->countSpectra();
        ANN->autoClassificationSimple();
        helpString = dir->absolutePath() + QDir::separator() + initName + "_randIca.txt";
        outFile = fopen(helpString.toStdString().c_str(), "a");
        fprintf(outFile, "%.2lf\n", ANN->getAverageAccuracy());
        fclose(outFile);
    }




    matrixDelete(&randMatrix, compNum);
    matrixDelete(&matrixW, compNum);
    matrixDelete(&newData, ns);
    matrixDelete(&eigenMatrix, compNum);
    matrixDelete(&eigenMatrixInv, compNum);

    delete []eigenValues;
    delete ANN;
    delete spectr;


}
//
double MainWindow::fileInnerClassification(QString workPath, QString fileName, QString cfgFileName, int NumOfPairs, bool windows, int wndLen, int tShift)
{
    QString helpString;

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);
    Net * ANN;

    if(!windows)
    {
        ui->realButton->setChecked(true);
    }
    else
    {
        ui->windButton->setChecked(true);
        ui->windowLengthSpinBox->setValue(wndLen);
        ui->timeShiftSpinBox->setValue(tShift);
    }

    helpString = tmpDir->absolutePath() + QDir::separator() + fileName;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    ANN = new Net(tmpDir, ns, left, right, spStep, ExpName);
    ANN->loadCfgByName(cfgFileName);
    ANN->setAutoProcessingFlag(true);
    ANN->setNumOfPairs(NumOfPairs);
    ANN->autoClassificationSimple();
    double res = ANN->getAverageAccuracy();
    ANN->close();
    delete ANN;
    delete tmpDir;
    return res;

}


double MainWindow::filesCrossClassification(QString workPath, QString fileName1, QString fileName2, QString cfgFileName, int NumOfRepeats, double startCoeff, bool windows, int wndLen, int tShift)
{
    QString helpString;

    //learn by 1 file
    //recognize 2 file
    //yet for 19 channels only

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    Net * ANN;
    MakePa * mkPa;

    if(windows)
    {
        ui->windButton->setChecked(true);
        ui->windowLengthSpinBox->setValue(wndLen);
        ui->timeShiftSpinBox->setValue(tShift);
    }
    else
    {
        ui->realButton->setChecked(true);
    }

    //open 1 file
    helpString = tmpDir->absolutePath() + QDir::separator() + fileName1;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    ANN = new Net(tmpDir, ns, left, right, spStep, ExpName);
    ANN->setReduceCoeff(startCoeff);
    ANN->setAutoProcessingFlag(true);
    ANN->loadCfgByName(cfgFileName);
    helpString = tmpDir->absolutePath() + QDir::separator() + "SpectraSmooth";
    if(windows)
    {
        helpString += QString(QDir::separator()) + "windows";
    }
    mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
    ANN->adjustReduceCoeff(helpString, 90, 150, mkPa, "all");
    cleanDir(tmpDir->absolutePath(), "wts");

    cout << "crossClass: learn (max " << NumOfRepeats << ") ";
    for(int k = 0; k < NumOfRepeats; ++k)
    {
        cout << k+1 << " ";
        flush(cout);
        mkPa->makePaSlot();
        ANN->PaIntoMatrixByName("all");
        ANN->LearnNet();
        helpString = tmpDir->absolutePath() + QDir::separator() + "crossClass_" + QString::number(k) + ".wts";
        ANN->saveWts(helpString);
    }
    cout << endl;


    //open 2nd file
    helpString = tmpDir->absolutePath() + QDir::separator() + fileName2;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    mkPa->makePaSlot();
    ANN->PaIntoMatrixByName("all");

    QStringList wtsFiles = tmpDir->entryList(QStringList("*.wts"));
//    cout << "crossClass: tall (max " << NumOfRepeats << ") ";
    for(int k = 0; k < NumOfRepeats; ++k)
    {
//        cout << k+1 << " "; cout.flush();
        helpString = tmpDir->absolutePath() + QDir::separator() + wtsFiles[k];
        ANN->loadWtsByName(helpString);
        ANN->tall();
    }
//    cout << endl;
    cleanDir(tmpDir->absolutePath(), "wts");
    ANN->averageClassification();
    double res = ANN->getAverageAccuracy();
    mkPa->close();
    delete mkPa;
    ANN->close();
    delete ANN;
    return res;
}


void MainWindow::countICAs(QString workPath, QString fileName, bool icaFlag, bool transformFlag, bool sequenceFlag, bool sumFlag) //to delete (listFileCrossClassification)
{
    QString helpString2;
    QString helpString;

    //for young scientists conference
    QTime myTime;
    myTime.start();

    ui->svdDoubleSpinBox->setValue(9.5);
    ui->vectwDoubleSpinBox->setValue(9.5);

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsCheckBox->setChecked(false);
    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);



    QString addName = fileName.left(3);
    QString ExpName2;
    QString mapsPath;
    QString mapsPath2;


    ui->realButton->setChecked(true);
    if(icaFlag) //count ICA on corresponding 3 files
    {

        helpString = tmpDir->absolutePath() + QDir::separator() + fileName;
        setEdfFile(helpString); // open ExpName_1.edf
        ICA();

        helpString = tmpDir->absolutePath() + QDir::separator() + fileName;
        helpString.replace("_1.edf", "_2.edf");
        setEdfFile(helpString); // open ExpName_2.edf
        cleanDirs();
        ICA();

        if(sumFlag)
        {
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName;
            helpString.replace("_1.edf", "_sum.edf");
            setEdfFile(helpString); // open ExpName_sum.edf
            cleanDirs();
            ICA();
        }
    }

    if(transformFlag) //transform 2nd with 1st maps, and both with comman maps
    {

        //transform 2nd file with 1st maps
        helpString = tmpDir->absolutePath() + QDir::separator() + fileName;
        helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
        mapsPath = tmpDir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + addName + "_1_maps.txt";

        helpString2 = tmpDir->absolutePath() + QDir::separator() + fileName;
        helpString2.replace("_1.edf", "_2_ica_by1.edf"); //write to ExpName_2_ica_by1.edf
        transformEDF(helpString, mapsPath, helpString2);

        if(sumFlag)
        {
            //transform both files with general maps
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName; //
            mapsPath = tmpDir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + addName + "_sum_maps.txt";
            helpString2 = tmpDir->absolutePath() + QDir::separator() + fileName;
            helpString2.replace("_1.edf", "_1_ica_sum.edf"); //write to ExpName_1_ica_sum.edf
            transformEDF(helpString, mapsPath, helpString2);

            helpString = dir->absolutePath() + QDir::separator() + fileName;
            helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
            mapsPath = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + addName + "_sum_maps.txt";
            helpString2 = dir->absolutePath() + QDir::separator() + fileName;
            helpString2.replace("_1.edf", "_2_ica_sum.edf"); //write to ExpName_2_ica_sum.edf
            transformEDF(helpString, mapsPath, helpString2);
        }

        //_1_ica.edf -> _1_ica_by1.edf
        helpString = dir->absolutePath() + QDir::separator() + fileName;
        helpString.replace("_1.edf", "_1_ica.edf");
        helpString2 = dir->absolutePath() + QDir::separator() + fileName;
        helpString2.replace("_1.edf", "_1_ica_by1.edf");
        QFile::copy(helpString, helpString2);
    }

    if(sequenceFlag) //sequencing 1 and 2
    {
        //transform 1st and 2nd correspondingly
        helpString = tmpDir->absolutePath() + QDir::separator() + fileName;
        helpString.replace("_1.edf", "_sum_ica.edf");
        helpString2 = tmpDir->absolutePath() + QDir::separator() + fileName;
        helpString2.replace("_1.edf", "_1_ica.edf");
        mapsPath = tmpDir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + addName + "_sum_maps.txt";
        mapsPath2 = tmpDir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + addName + "_1_maps.txt";
        ICsSequence(helpString, helpString2, mapsPath, mapsPath2);

        helpString = tmpDir->absolutePath() + QDir::separator() + fileName;
        helpString.replace("_1.edf", "_1_ica.edf");
        helpString2 = tmpDir->absolutePath() + QDir::separator() + fileName;
        helpString2.replace("_1.edf", "_2_ica.edf");
        mapsPath = tmpDir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + addName + "_1_maps.txt";
        mapsPath2 = tmpDir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + addName + "_2_maps.txt";
        ICsSequence(helpString, helpString2, mapsPath, mapsPath2);
    }
    delete tmpDir;
}


double MainWindow::filesDropComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats, bool windows, int wndLen, int tShift)
{
    QString helpString2;
    QString helpString;

    //for young scientists conference
    //learn on 1 -> classify 2
    //Icas or rawData

    QTime myTime;
    myTime.start();

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    QList <int> neededChannels;
    QList <int> channelsSet;
    QList <int> channelsSetExclude;

    int tempItem;
    int tempIndex;
    double tempAccuracy;
    double lastAccuracy;
    bool foundFlag;
    ofstream logF;
    QString logPath = workPath + QDir::separator() + "dropCompsLog .txt";


    ns = 19;

    channelsSet.clear();
    for(int j = 0; j < 19; ++j)
    {
        channelsSet << j;
    }
    channelsSetExclude.clear();
    neededChannels.clear();

    //////////////////////////////////////////////////////////initial accuracy count
    //clean wts
    cleanDir(workPath, "wts");
    makeCfgStatic(workPath, 19*247, "Reduced");

    helpString = fileName1;
    helpString2 = fileName2;
    double initAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 2., windows, wndLen, tShift);

    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << ExpName.left(3).toStdString() << " initialAccuracy = " << tempAccuracy << endl << endl;
        logF.close();
    }
    ////////////////////////////////////////////////////////////////////////////////////////initial accuracy count end

    while(1)
    {
        ns = channelsSet.length() - 1;
        makeCfgStatic(tmpDir->absolutePath(), ns*247, "Reduced");
        foundFlag = false;

        for(int l = 0; l < channelsSet.length(); ++l)
        {
            //clean wts
            cleanDir(dir->absolutePath(), "wts");
            cleanDir(dir->absolutePath(), "markers", 0);

            tempItem = channelsSet[l];
            if(neededChannels.contains(tempItem)) continue;

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << endl << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " start" << endl;
                logF.close();
            }

            channelsSet.removeAt(l);
            channelsSetExclude.push_back(tempItem);

            //drop some channels
            helpString.clear();
            for(int k = 0; k < 19; ++k)
            {
                if(channelsSet.contains(k))
                {
                    helpString += QString::number(k+1) + " ";
                }
            }
            helpString += "20";
            ui->reduceChannelsLineEdit->setText(helpString);

            //exclude channels from 1st file
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName1;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName1;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //exclude channels from 2nd file
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName2;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName2;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);



            //cross class
            helpString = fileName1;
            helpString.replace(".edf", "_rdc.edf");
            helpString2 = fileName2;
            helpString2.replace(".edf", "_rdc.edf");
            lastAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift);


            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " done" << endl;
                logF << "channelsSet:" << endl;
                for(int q = 0; q < channelsSet.length(); ++q)
                {
                    logF << channelsSet[q] + 1 << "  ";
                }
                logF << endl << "Accuracy = " << lastAccuracy << endl;
                logF.close();
            }


            //set back
            channelsSet.insert(l, tempItem);
            channelsSetExclude.removeLast();



            if(lastAccuracy > tempAccuracy + 1.5)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
                break;
            }
            else if(lastAccuracy > tempAccuracy + 0.7)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
            }
            else if(lastAccuracy < tempAccuracy - 2.0)
            {
                logF.open(logPath.toStdString().c_str(), ios_base::app);
                if(!logF.is_open())
                {
                    cout << "cannot open logFile: " << logPath.toStdString() << endl;
                }
                else
                {
                    logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to needed channels" << endl;
                    logF.close();
                }
                neededChannels << tempItem;
            }

        } //end of cycle l

        if(foundFlag)
        {
            channelsSetExclude.push_back(channelsSet[tempIndex]);
            channelsSet.removeAt(tempIndex);

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << channelsSet[tempIndex] + 1 << " to drop\t";
                logF << "currentAccuracy" << tempAccuracy << endl;
                logF.close();
            }

        }
        else
        {

            break;
        }

    } //end of while(1)
    cout << ExpName.left(3).toStdString() << " ended in " << myTime.elapsed()/1000. << " sec" << endl;



    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << "FINAL SET" << endl;
        logF << ExpName.left(3).toStdString() << endl;
        logF << "channelsSet:" << endl;
        for(int q = 0; q < channelsSet.length(); ++q)
        {
            logF << channelsSet[q] + 1 << "  ";
        }
        logF << endl << "neededChannels:" <<endl;
        for(int q = 0; q < neededChannels.length(); ++q)
        {
            logF << neededChannels[q] + 1 << "  ";
        }
        logF << endl << "Accuracy = " << tempAccuracy << endl;
        logF.close();
    }
    cleanDir(tmpDir->absolutePath(), "wts");
    cleanDir(tmpDir->absolutePath(), "markers", 0);
    delete tmpDir;
    cout << initAccuracy << "->" << tempAccuracy << endl;
    return tempAccuracy;
}

double MainWindow::filesAddComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats, bool windows, int wndLen, int tShift)
{

    QString helpString2;
    QString helpString;

    //check the percentage on all of 3 components
    //add the best one giving the maximum of percentage

    QTime myTime;
    myTime.start();

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    QList <int> unneededChannels;
    QList <int> channelsSet;
    QList <int> channelsSetExclude;

    int tempItem;
    int tempIndex;
    double tempAccuracy = 0.;
    double lastAccuracy;
    bool foundFlag;
    QString logPath = tmpDir->absolutePath() + QDir::separator() + "addCompsLog.txt";



    channelsSet.clear();
    unneededChannels.clear();

    //////////////////////////////////////////////////////////initial accuracy count
    //checkpercentage on the set of components (i, j, k)
    double initAccuracy = 0;
    int iS, jS, kS;
    makeCfgStatic(workPath, 1*247, "Reduced");

    ofstream logF;
    logF.open(logPath.toStdString().c_str(), ios_base::app);
    logF << fileName1.toStdString() << endl;

    for(int i = 0; i < 19; ++i) //19 is not ns
    {
        break; //already done
        for(int j = i+1; j < 19; ++j) //19 is not ns
        {
            for(int k = j+1; k < 19; ++k) //19 is not ns
            {
                myTime.restart();

                //clean wts
                cleanDir(workPath, "wts");
                channelsSet.clear();
                channelsSet << i;
                channelsSet << j;
                channelsSet << k;

                makeCfgStatic(workPath, channelsSet.length()*247, "Reduced");
                //drop some channels
                helpString.clear();
                for(int l = 0; l < 19; ++l)
                {
                    if(channelsSet.contains(l))
                    {
                        helpString += QString::number(l+1) + " ";
                    }
                }
                helpString += "20";
                ui->reduceChannelsLineEdit->setText(helpString);

                //exclude channels from 1st file
                helpString = tmpDir->absolutePath() + QDir::separator() + fileName1;
                setEdfFile(helpString);
                helpString = tmpDir->absolutePath() + QDir::separator() + fileName1;
                helpString.replace(".edf", "_rdc.edf");
                reduceChannelsEDF(helpString);

                //exclude channels from 2nd file
                helpString = tmpDir->absolutePath() + QDir::separator() + fileName2;
                setEdfFile(helpString);
                helpString = tmpDir->absolutePath() + QDir::separator() + fileName2;
                helpString.replace(".edf", "_rdc.edf");
                reduceChannelsEDF(helpString);


                helpString = fileName1;
                helpString.replace(".edf", "_rdc.edf");
                helpString2 = fileName2;
                helpString2.replace(".edf", "_rdc.edf");

                tempAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift); //0.5 generality
                cout << "check percentage: ";
                for(int p = 0; p < channelsSet.length(); ++p)
                {
                    cout << channelsSet[p]+1 << " ";
                    logF << channelsSet[p]+1 << " ";
                }
                cout << " = " << tempAccuracy << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl << endl;
                logF << " = " << tempAccuracy << endl;

                if(tempAccuracy > initAccuracy + 0.5)
                {
                    initAccuracy = tempAccuracy;
                    iS = i;
                    jS = j;
                    kS = k;
                }
            }
        }
    }
//    logF << "final set: " << iS+1 <<" " << jS+1 << " " << kS+1 << " " << initAccuracy << endl << endl;
//    logF.close();
//    return initAccuracy;


    //results generality
    if(fileName1.contains("AAX"))
    {
        iS = 3;
        jS = 7;
        kS = 16;
        initAccuracy = 97.01;
    }
    else if(fileName1.contains("BED"))
    {
        iS = 2;
        jS = 7;
        kS = 13;
        initAccuracy = 89.80;
    }
    else if(fileName1.contains("FIX"))
    {
        iS = 3;
        jS = 11;
        kS = 18;
        initAccuracy = 93.75;
    }
    else if(fileName1.contains("GAS"))
    {
        iS = 7;
        jS = 13;
        kS = 17;
        initAccuracy = 89.73;
    }
    else if(fileName1.contains("KUS"))
    {
        iS = 4;
        jS = 5;
        kS = 7;
        initAccuracy = 97.59;
    }
    else if(fileName1.contains("PAS"))
    {
        iS = 8;
        jS = 15;
        kS = 16;
        initAccuracy = 95.53;
    }
    else if(fileName1.contains("RMS"))
    {
        iS = 3;
        jS = 11;
        kS = 13;
        initAccuracy = 93.05;
    }
    else if(fileName1.contains("SMM"))
    {
        iS = 1;
        jS = 2;
        kS = 4;
        initAccuracy = 96.51;
    }
    else if(fileName1.contains("SUA"))
    {
        iS = 2;
        jS = 7;
        kS = 9;
        initAccuracy = 97.79;
    }
    else if(fileName1.contains("VDA"))
    {
        iS = 3;
        jS = 9;
        kS = 16;
        initAccuracy = 95.93;
    }
    else return 0;
    --iS; --jS; --kS; //because of start from 0 or 1
    channelsSet << iS << jS << kS;


    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << ExpName.left(3).toStdString() << " initialAccuracy = " << initAccuracy << endl;
        logF << ExpName.left(3).toStdString() << " initialSet: " << iS+1 << " " << jS+1 << " " << kS+1 << " " << endl << endl;
        logF.close();
    }
    ////////////////////////////////////////////////////////////////////////////////////////initial accuracy count end

    channelsSetExclude.clear();
    for(int j = 0; j < 19; ++j)
    {
        channelsSetExclude << j;
    }
    channelsSetExclude.removeOne(iS);
    channelsSetExclude.removeOne(jS);
    channelsSetExclude.removeOne(kS);


    tempAccuracy = 0.;
    while(1)
    {
        ns = channelsSet.length() + 1;
        makeCfgStatic(tmpDir->absolutePath(), ns*247, "Reduced");
        foundFlag = false;

        for(int l = 0; l < channelsSetExclude.length(); ++l)
        {
            //clean wts
            cleanDir(dir->absolutePath(), "wts");
            cleanDir(dir->absolutePath(), "markers", 0);
            tempItem = channelsSetExclude[l];

            if(unneededChannels.contains(tempItem)) continue; //if the channel is veru bad

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << endl << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " start" << endl;
                logF.close();
            }

            channelsSet.push_back(tempItem); //add to considered channels
            channelsSetExclude.removeOne(tempItem); //temporarily remove from non-considered list

            //drop some channels
            helpString.clear();
            for(int k = 0; k < 19; ++k)
            {
                if(channelsSet.contains(k))
                {
                    helpString += QString::number(k+1) + " ";
                }
            }
            helpString += "20";
            ui->reduceChannelsLineEdit->setText(helpString);

            //exclude channels from 1st file
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName1;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName1;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //exclude channels from 2nd file
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName2;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + QDir::separator() + fileName2;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //cross class
            helpString = fileName1;
            helpString.replace(".edf", "_rdc.edf");
            helpString2 = fileName2;
            helpString2.replace(".edf", "_rdc.edf");
            lastAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift);


            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " done" << endl;
                logF << "channelsSet:" << endl;
                for(int q = 0; q < channelsSet.length(); ++q)
                {
                    logF << channelsSet[q] + 1 << "  ";
                }
                logF << endl << "Accuracy = " << lastAccuracy << endl;
                logF.close();
            }


            //set back
            channelsSet.removeOne(tempItem); //pop tempItem back
            channelsSetExclude.insert(l, tempItem); //return tempItem onto its place in channelsSetExclude



            if(lastAccuracy > tempAccuracy + 0.5)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
            }
            else if(lastAccuracy < tempAccuracy - 2.0)
            {
                logF.open(logPath.toStdString().c_str(), ios_base::app);
                if(!logF.is_open())
                {
                    cout << "cannot open logFile: " << logPath.toStdString() << endl;
                }
                else
                {
                    logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to unneeded channels" << endl;
                    logF.close();
                }
                unneededChannels << tempItem;
            }

        } //end of cycle l

        if(foundFlag)
        {

            tempItem = channelsSetExclude[tempIndex];
            cout << endl << "channel " << tempItem << " at index " << tempIndex << " to add" << endl << endl;
            channelsSetExclude.removeOne(tempItem);
            channelsSet.push_back(tempItem);

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to add" << endl;
                logF << "currentAccuracy = " << tempAccuracy << endl;
                logF.close();
            }

        }
        else
        {

            break;
        }

    } //end of while(1)
    cout << ExpName.left(3).toStdString() << " ended in " << myTime.elapsed()/1000. << " sec" << endl;


    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << "FINAL SET" << endl;
        logF << ExpName.left(3).toStdString() << endl;
        logF << "channelsSet:" << endl;
        for(int q = 0; q < channelsSet.length(); ++q)
        {
            logF << channelsSet[q]+1 << "  ";
        }
        logF << endl << "unneededChannels:" <<endl;
        for(int q = 0; q < unneededChannels.length(); ++q)
        {
            logF << unneededChannels[q]+1 << "  ";
        }
        logF << endl << "Accuracy = " << tempAccuracy << endl;
        logF.close();
    }
    cleanDir(tmpDir->absolutePath(), "wts");
    cleanDir(tmpDir->absolutePath(), "markers", 0);
    delete tmpDir;
    cout << initAccuracy << "->" << tempAccuracy << endl;
    return tempAccuracy;
}

void MainWindow::customFunc()
{
    //MATI
    if(1)
    {
        setEdfFile("/media/Files/Data/Mati/SDA/SDA_rr_f.edf");
        ui->matiCheckBox->setChecked(true);
        ui->sliceCheckBox->setChecked(true);
        ui->sliceWithMarkersCheckBox->setChecked(true);
        ui->reduceChannelsCheckBox->setChecked(true);
        ui->reduceChannelsComboBox->setCurrentText("MatiNoEyes");
//        cleanDirs();
//        sliceAll();
    }

    //pca for AAX
    if(0)
    {
        left = 1;
        right = 48;
        spLength = 48;
        setEdfFile("/media/Files/Data/AAX/AAX_rr_f_new.edf");
        Net * ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->show();
    }


    //test a saw and sine into ica
    if(0)
    {
        QString inPath = "/media/Files/Data/AAX/AAX_rr_f_new.edf";
        QString outPath = "/media/Files/Data/AAX/TEST.edf";
        setEdfFile(inPath);
        readData();
        int lent = 200000;

        for(int i = 0; i < ndr*nr[0]; ++i)
        {
            helpDouble = 2. * pi * double(i)/defaults::frequency * 5;
            data[0][i] = 10 * sin(helpDouble - pi/2);//+ 0.17); //10.5 Hz

            if(i%50 < 25)
            {
                data[1][i] =  (i%25) - 12.5;
            }
            else
            {
                data[1][i] = -(i%25) + 12.5;
            }

//            helpDouble = 2. * pi * double(i)/defaults::frequency * 5;
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
                array[i] += ampl*sin(2 * pi * i/defaults::frequency * freq - phas);
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
            array[i] =  ampl*sin(2 * pi * i/defaults::frequency * freq - phas);// * sin (2 * pi * i/defaults::frequency * freq/4.);
            array[i] = 50. * (rand()%1000)/1000.;
            array[i] = i%150;
        }

        for(int i = 10; i < 100; i += 5)
        {
            cout << i << "   " << enthropy(array, leng, "", i) << endl;
        }
        //    cout << "   " << enthropy(array, leng, "", 50) << endl;

    }



    /*
        dir->cd(defaults::dataFolder + "/AA");


        FILE * results = fopen(QString(defaults::dataFolder + "/AA/res.txt").toStdString().c_str(), "r");



        outStream.open(defaults::dataFolder + "AA/discr.txt");
        outStream << "name" << "\t";
        outStream << "mean" << "\t";
        outStream << "sigma" << endl;

        lst = dir->entryList(QStringList("*randIca.txt"), QDir::NoFilter, QDir::Name);
        int num1 = lst.length()*2;
        double * sigm = new double [num1];
        double * men = new double [num1];
        double * classf = new double [num1];
        double * drawVars = new double [num1];
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = dir->absolutePath() + QDir::separator() + lst[i];
            helpString.replace(".txt", ".png");
            countRCP(QString(dir->absolutePath() + QDir::separator() + lst[i]), helpString, &men[i], &sigm[i]);

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
    */


    if(0)
    {
        //many edf to my format
        dir->cd(defaults::dataFolder + "/GPP/edf/1");
        QString pth = "/media/michael/Files/Data/GPP/Realisations";
        QStringList lst = dir->entryList(QStringList("*.edf"), QDir::Files);
        QString helpString;
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = dir->absolutePath() + QDir::separator() + lst[i];
            setEdfFile(helpString);
            readData();
            helpString = lst[i];
            helpString.resize(helpString.indexOf('.'));
            helpString += "_1";
            outStream.open((pth + '/' + helpString).toStdString().c_str());
            outStream << "NumOfSlices " << ndr*nr[0] << '\n';
            for(int j = 0; j < ndr*nr[0]; ++j)
            {
                for(int k = 0; k < 19; ++k)
                {
                    outStream << data[k][j] << '\n';
                }
                outStream << "0\n";
            }
            outStream.close();

        }


        dir->cd(defaults::dataFolder + "/GPP/edf/2");
        lst = dir->entryList(QStringList("*.edf"), QDir::Files);
        for(int i = 0; i < lst.length(); ++i)
        {
            helpString = dir->absolutePath() + QDir::separator() + lst[i];
            setEdfFile(helpString);
            readData();
            helpString = lst[i];
            helpString.resize(helpString.indexOf('.'));
            helpString += "_2";
            outStream.open((pth + '/' + helpString).toStdString().c_str());
            outStream << "NumOfSlices " << ndr*nr[0] << '\n';
            for(int j = 0; j < ndr*nr[0]; ++j)
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
                    helpString = pcaPath + QDir::separator() + lst[i];
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
        for(int i = 0; i < dim; ++ i)
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
            if(0)
            {
                helpString = dir->absolutePath() + QDir::separator() + list0[i];
                drawMapsICA(helpString, 19, dir->absolutePath(), QString(list0[i].left(3) + "-m"));
            }

            //draw average spectra
            if(0)
            {
                helpString = dir->absolutePath() + QDir::separator() + list0[i];
                setEdfFile(helpString);
                cleanDirs();
                sliceAll();
                countSpectraSimple(4096);
            }
            //draw maps on average spectra
            if(1)
            {
                helpString = dir->absolutePath() + QDir::separator() + list0[i];
                helpString = dir->absolutePath() + QDir::separator() + list0[i].left(3) + "_1_ica_all.png";
                helpString2 = dir->absolutePath() + QDir::separator() + list0[i].left(3) + "_1_ica_all_withmaps.png";
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
            helpString = dir->absolutePath() + QDir::separator() + lst11[i];
            setEdfFile(helpString);
            cleanDirs();
            sliceAll();
            constructEDFSlot();
            helpString = dir->absolutePath() + QDir::separator() + lst11[i];
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
        helpString = dir->absolutePath() + QDir::separator() + fileName1;
        setEdfFile(helpString);
        helpString = dir->absolutePath() + QDir::separator() + fileName1;
        helpString.replace(".edf", "_rdc.edf");
        reduceChannelsEDF(helpString);

        //exclude channels from 2nd file
        helpString = dir->absolutePath() + QDir::separator() + fileName2;
        setEdfFile(helpString);
        helpString = dir->absolutePath() + QDir::separator() + fileName2;
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
                cout << list1[i].left(3).toStdString() << " finished\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl << endl;
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
                cout << list1[i].left(3).toStdString() << " ica finished\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl << endl;
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
                cout << list1[i].left(3).toStdString() << " addComps from 3 finished\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl << endl;
            }
        }

    }
}
