#define maxNs 45
#include "mainwindow.h"
#include "ui_mainwindow.h"


QString str(int &input)
{
    QString a;
    a.setNum(input);
    return a;
}

QString rightNumber(int &input, int N)
{
    QString h;
    h.setNum(input);
    if(input== 0)
    {
        h="";
        for(int j = 0; j < N; ++j)
        {
            h.prepend("0");
        }
        return h;
    }
    for(int i=1; i < N; ++i)
    {
        if(log10(input)>=i-1 && log10(input) < i)
            for(int j=i; j < N; ++j)
            {
                h.prepend("0");
            }
    }
    return h;
}

int len(QString s) //lentgh till double \0-byte for EDF+annotations
{
    int l = 0;
    for(int i = 0; i < 100500; ++i)
    {
        if(s[i]!='\0') ++l;
        else
        {
            if(s[i+1]!='\0') ++l;
            if(s[i+1]=='\0')
            {
                ++l;
                return l;
            }
        }
    }
    return -1;
}

MainWindow::MainWindow() :
    ui(new Ui::MainWindow)
{

    QButtonGroup * group1, *group2, *group3;
    autoProcessingFlag = false;
    ui->setupUi(this);
    this->setWindowTitle("Main");

    setlocale(LC_NUMERIC, "C");

    ns=-1;
    spLength=-1;

    ns=19;

    spLength=247;
    left = 82;
    right = 328;
    spStep = 250./4096.;

//    spLength=63;
//    left = 20;
//    right = 82;
//    spStep = 250./1024.;

    staSlice = 0;

    stopFlag = 0;

    dir = new QDir();
    dir->cd(QDir::root().absolutePath());

    NumOfEdf = 0; //for EDF cut

    paint = new QPainter();



    ui->autoProcessingBox->setChecked(false);

    group1 = new QButtonGroup();
    group1->addButton(ui->enRadio);
    group1->addButton(ui->ntRadio);
    group2 = new QButtonGroup();
    group2->addButton(ui->windButton);
    group2->addButton(ui->realButton);
    group3 = new QButtonGroup();
    group3->addButton(ui->BayesRadioButton);
    group3->addButton(ui->HiguchiRadioButton);

    //wavelets
    ui->classBox->setChecked(true);
    ui->weightsBox->setChecked(true);
    ui->waveletsBox->setChecked(true);
    ui->fullBox->setChecked(true);

    ui->tempBox->setMaximum(1e7);
    ui->tempBox->setValue(2e3);

    ui->enRadio->setChecked(true);

    ui->doubleSpinBox->setValue(1.0); //draw coeff
    ui->sliceBox->setChecked(true);
    ui->eyesBox->setChecked(false);
    ui->eyesBox->setChecked(true);   ///for windows
    ui->chRdcBox->setChecked(true);
    ui->progressBar->setValue(0);
    ui->setNsLine->property("S&et");

    ui->drawDirBox->addItem("Realisations");
    ui->drawDirBox->addItem("cut");
    ui->drawDirBox->addItem("windows");
    ui->drawDirBox->addItem("windows/fromreal"); //generality
    ui->rdcChannelBox->addItem("Realisations");
    ui->rdcChannelBox->addItem("windows");

    ui->reduceNsBox->addItem("En->real-time");   //encephalan for real time
    var = QVariant("19 18 16 14 11 9 6 4 2 1 17 13 12 8 7 3 15 10 5 24");
    ui->reduceNsBox->setItemData(0, var);

    ui->reduceNsBox->addItem("En-19");   //encephalan w/o veget channels
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 22");
    ui->reduceNsBox->setItemData(1, var);

    ui->reduceNsBox->addItem("LN->En");
    var = QVariant("1 2 11 3 17 4 12 13 5 18 6 14 15 7 19 8 16 9 10");
    ui->reduceNsBox->setItemData(2, var);

    ui->reduceNsBox->addItem("Boris Nt->En");
    var = QVariant("1 3 7 4 5 6 8 25 14 15 16 26 27 20 21 22 28 29 31");
    ui->reduceNsBox->setItemData(3, var);

    ui->reduceNsBox->addItem("MyCurrent");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 22 23 24");
    ui->reduceNsBox->setItemData(4, var);

    ui->reduceNsBox->addItem("MichaelBak");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 23 24 31");
    ui->reduceNsBox->setItemData(5, var);

    ui->reduceNsBox->addItem("MichaelBakNew");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 23 24 27");
    ui->reduceNsBox->setItemData(6, var);

    ui->reduceNsBox->addItem("MyCurrentNoEyes");
    var = QVariant("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 24");
    ui->reduceNsBox->setItemData(7, var);


    ui->reduceNsBox->setCurrentIndex(4);
    ui->nsLine->setText(ui->reduceNsBox->itemData(ui->reduceNsBox->currentIndex()).toString());


    ui->timeShiftBox->setMinimum(25);
    ui->timeShiftBox->setMaximum(1000);
    ui->timeShiftBox->setValue(125);
    ui->timeShiftBox->setSingleStep(25);
    ui->wndLengthBox->addItem("400");
    ui->wndLengthBox->addItem("450");
    ui->wndLengthBox->addItem("500");
    ui->wndLengthBox->addItem("625");
    ui->wndLengthBox->addItem("750");
    ui->wndLengthBox->addItem("875");
    ui->wndLengthBox->addItem("1000");
    ui->wndLengthBox->setCurrentIndex(6);
    ui->realButton->setChecked(true);
//    ui->windButton->setChecked(true); //for windows

    ui->numOfIcSpinBox->setMaximum(19); //generality
    ui->numOfIcSpinBox->setMinimum(2);
    ui->numOfIcSpinBox->setValue(19);

    ui->finishTimeBox->setMaximum(60*30.);
    ui->startTimeBox->setMaximum(ui->finishTimeBox->maximum());

    ui->BayesSpinBox->setMaximum(50);
    ui->BayesSpinBox->setMinimum(1);
    ui->BayesSpinBox->setValue(20);

    ui->cleanFromRealsCheckBox->setChecked(false);
    ui->cleanHelpCheckBox->setChecked(false);
    ui->cleanRealisationsCheckBox->setChecked(false);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(false);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(setEdfFile()));

    QObject::connect(ui->countSpectra, SIGNAL(clicked()), this, SLOT(countSpectra()));

    QObject::connect(ui->Name, SIGNAL(returnPressed()), this, SLOT(setExpName()));

    QObject::connect(ui->cutEDF, SIGNAL(clicked()), this, SLOT(sliceAll()));

    QObject::connect(ui->windFromRealButton, SIGNAL(clicked()), this, SLOT(sliceWindFromReal()));

    QObject::connect(ui->cut_e, SIGNAL(clicked()), this, SLOT(cutShow()));

    QObject::connect(ui->makeCFG, SIGNAL(clicked()), this, SLOT(makeShow()));

    QObject::connect(ui->makePA, SIGNAL(clicked()), this, SLOT(makePaSlot()));

    QObject::connect(ui->netButton, SIGNAL(clicked()), this, SLOT(netShow()));

    QObject::connect(ui->rdcNsButton, SIGNAL(clicked()), this, SLOT(reduceChannels()));

    QObject::connect(ui->drawButton, SIGNAL(clicked()), this, SLOT(drawRealisations()));

    QObject::connect(ui->eyesButton, SIGNAL(clicked()), this, SLOT(eyesShow()));

    QObject::connect(ui->cleanDirsButton, SIGNAL(clicked()), this, SLOT(cleanDirs()));

    QObject::connect(ui->reduceNsBox, SIGNAL(highlighted(int)), this, SLOT(changeNsLine(int)));
    QObject::connect(ui->reduceNsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeNsLine(int)));

    QObject::connect(ui->setNsLine, SIGNAL(returnPressed()), this, SLOT(setNs()));

    QObject::connect(ui->waveletButton, SIGNAL(clicked()), this, SLOT(waveletCount()));

    QObject::connect(ui->finishTimeBox, SIGNAL(valueChanged(double)), this, SLOT(setBoxMax(double)));

    QObject::connect(ui->datFileButton, SIGNAL(clicked()), this, SLOT(makeDatFile()));

    QObject::connect(ui->autoFBPushButton, SIGNAL(clicked()), this, SLOT(autoProcessingFB()));

    QObject::connect(ui->smoothProcButton, SIGNAL(clicked()), this, SLOT(diffSmooth()));

    QObject::connect(ui->avTimeButton, SIGNAL(clicked()), this, SLOT(avTime()));

    QObject::connect(ui->icaButton, SIGNAL(clicked()), this, SLOT(ICA()));

    QObject::connect(ui->constructEdfButton, SIGNAL(clicked()), this, SLOT(constructEDF()));

    QObject::connect(ui->drawMapsPushButton, SIGNAL(clicked()), this, SLOT(drawMaps()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    QObject::connect(ui->BayesPushButton, SIGNAL(clicked()), this, SLOT(Bayes()));

    ui->reduceNsBox->setCurrentIndex(4);
    ui->reduceNsBox->setCurrentIndex(7); //for windows

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setBoxMax(double a)
{
    ui->startTimeBox->setValue(min(a-1., ui->startTimeBox->value()));
    ui->startTimeBox->setMaximum(a-1.);
}

//enable Escape key for all widgets
void QWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape) this->close();
}

void MainWindow::stop()
{
    stopFlag = 1;
}

void MainWindow::waveletCount()
{
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

            helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_wavelet_").append(str(channel)).append(".jpg"));
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
            helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_weights_").append(str(i)).append(".jpg"));
            pic = QPixmap(helpString);
            source = QRectF(pic.rect());
            painter->drawPixmap(target, pic, source);

            //wavelets
            target = QRectF(600, (i+1)*(800 + 50), 150*NumOfSlices/250, 800);
            helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_wavelet_").append(str(i)).append(".jpg"));
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

double logistic2(double &x, double t)
{
    if( x >   37.*t )  return 1.;
    if( x < -115.*t )  return 0.;
    return 1. / ( 1. + exp(-x/t) );
}

void MainWindow::autoProcessingFB()
{
    autoProcessingFlag = true;
    time_t wholeTime = time(NULL);


    setEdfFile();



    QDir * dirBC = new QDir(dir->absolutePath());
    dir->cdUp();
    helpString = ExpName;
    helpString.append("_FB");
    dir->cd(helpString);
    QDir * dirFB = new QDir(dir->absolutePath());
    dir->cd(dirBC->absolutePath());
    this->ui->timeShiftBox->setValue(125); //timeShift=wndLength

    fclose(edf);


    FILE * res;
    for(int num = 0; num < this->ui->wndLengthBox->count(); ++num)
    {
        helpString = dirBC->absolutePath().append(QDir::separator()).append(ExpName).append(".EDF");
        edf = fopen(helpString.toStdString().c_str(), "r");
        dir->cd(dirBC->absolutePath());

        emit cleanDirs();

        this->ui->eyesBox->setChecked(true);
        this->ui->windButton->setChecked(true);
        this->ui->wndLengthBox->setCurrentIndex(num);
//        this->ui->timeShiftBox->setValue(125); //timeShift=wndLength
        this->ui->reduceNsBox->setCurrentIndex(7);
//        this->ui->rdcChannelBox->setCurrentIndex( 7 /*this->ui->rdcChannelBox->count()*/); //current no eyes

        cout << endl << "wndLength = " << ui->wndLengthBox->currentText().toStdString() << endl << endl;

        res = fopen(QDir::toNativeSeparators(dirFB->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
        fprintf(res, "\nwndLength\t%d\n", this->ui->wndLengthBox->currentText().toInt());
        fclose(res);

        //slice
        sliceAll();
        fclose(edf);

        //count spectra
        Spectre *sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));

        sp->countSpectra();
        sp->close();
        if(sp!=NULL) delete sp;

        cout << "spectra counted" << endl;

        //cycle Net + write to file
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows"));
        MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
        mkPa->setRdcCoeff(20);

        Net * ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(true);
        ANN->readCfg();
        switch(this->ui->wndLengthBox->currentIndex())
        {
        case 0: {ANN->setErrcrit(0.55); break;}
        case 1: {ANN->setErrcrit(0.5); break;}
        case 2: {ANN->setErrcrit(0.45); break;}
        case 3: {ANN->setErrcrit(0.4); break;}
        case 4: {ANN->setErrcrit(0.35); break;}
        case 5: {ANN->setErrcrit(0.3); break;}
        case 6: {ANN->setErrcrit(0.25); break;}
        default: {ANN->setErrcrit(0.45); break;}
        }

        ANN->PaIntoMatrixByName("all_wnd");
        ANN->LearnNet();
        ANN->saveWts();
//        ANN->drawWts();
        ANN->close();
        if(ANN!=NULL) delete ANN;

        mkPa->close();
        if(mkPa!=NULL) delete mkPa;


        helpString = ExpName; helpString.append("_FB.EDF");
        helpString = dirFB->absolutePath().append(QDir::separator()).append(helpString);

        edf = fopen(helpString.toStdString().c_str(), "r");
        dir->cd(dirFB->absolutePath());

        emit cleanDirs();

        this->ui->eyesBox->setChecked(true);
        this->ui->windButton->setChecked(true);
        this->ui->wndLengthBox->setCurrentIndex(num);
//        this->ui->timeShiftBox->setValue(125); //timeShift=wndLength
        this->ui->reduceNsBox->setCurrentIndex(7);
//        this->ui->rdcChannelBox->setCurrentIndex( 7 /*this->ui->rdcChannelBox->count()*/); //current no eyes

        res = fopen(QDir::toNativeSeparators(dirFB->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
        fprintf(res, "\nwndLength\t%d\n", this->ui->wndLengthBox->currentText().toInt());
        fclose(res);

        //slice
        sliceAll();
        fclose(edf);


        Spectre *sp2 = new Spectre(dir, ns, ExpName);
        QObject::connect(sp2, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));

        sp2->countSpectra();
        sp2->close();
        if(sp2!=NULL) delete sp2;

        cout << "spectra counted" << endl;
        //cycle Net + write to file
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows"));
        MakePa *mkPa2 = new MakePa(helpString, ExpName, ns, left, right, spStep);
        mkPa2->setRdcCoeff(20);

        Net * ANN2 = new Net(dir, ns, left, right, spStep, ExpName);
        ANN2->setAutoProcessingFlag(true);
        ANN2->readCfg();

        helpString = dirBC->absolutePath().append(QDir::separator()).append(ExpName).append("_weights.wts");
        ANN2->loadWtsByName(helpString);
        switch(this->ui->wndLengthBox->currentIndex())
        {
        case 0: {ANN2->setErrcrit(0.55); break;}
        case 1: {ANN2->setErrcrit(0.5); break;}
        case 2: {ANN2->setErrcrit(0.45); break;}
        case 3: {ANN2->setErrcrit(0.4); break;}
        case 4: {ANN2->setErrcrit(0.35); break;}
        case 5: {ANN2->setErrcrit(0.3); break;}
        case 6: {ANN2->setErrcrit(0.25); break;}
        default: {ANN2->setErrcrit(0.45); break;}
        }

        int numOfRepeats = 10;
        for(int i = 0; i < numOfRepeats; ++i)
        {
            cout << i << " ";
            mkPa2->makePaSlot();
            ANN2->PaIntoMatrixByName("all_wnd");
            ANN2->tall();
        }
        cout << endl;
        ANN2->reset();
        ANN2->closeLogFile();

        ANN2->close();
        if(ANN2!=NULL) delete ANN2;

        mkPa2->close();
        if(mkPa2!=NULL) delete mkPa2;



        FILE * logFile;
        helpString=QDir::toNativeSeparators(dirFB->absolutePath().append(QDir::separator()).append("log.txt"));
        //    cout << helpString.toStdString() << endl;
        logFile = fopen(helpString.toStdString().c_str(),"r");
        if(logFile==NULL)
        {
            cout << "logFile==NULL" << endl; return;
        }
        double averagePercentage[4];
        double tempDouble[4];
        for(int j = 0; j < 4; ++j)
        {
            averagePercentage[j] = 0.;
            tempDouble[j] = 0.;
        }

        for(int i = 0; i < numOfRepeats; ++i)
        {
            for(int j = 0; j < 4; ++j)
            {
                fscanf(logFile, "%lf", &tempDouble[j]);
                averagePercentage[j] += tempDouble[j];
            }
        }
        for(int j = 0; j < 4; ++j)
        {
            averagePercentage[j]/=(numOfRepeats);
        }
        fclose(logFile);


        res = fopen(QDir::toNativeSeparators(dirFB->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
        fprintf(res, "PRR \t(");
        for(int j = 0; j < 4-2; ++j)
        {
            fprintf(res, "%.2lf - ", averagePercentage[j]);
        }
        fprintf(res, "%.2lf", averagePercentage[4-2]);
        fprintf(res, ")  -  %.2lf \n", averagePercentage[3]);
        fclose(res);
    }
    wholeTime = time(NULL) - wholeTime;

    cout << "Duration of autoWindowsProcessing = " << wholeTime << " sec" << endl;

    helpString.setNum(int(wholeTime));
    helpString.prepend("All finished \nTime = ");
    helpString.append(" sec");
    QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);

}

void MainWindow::autoWindowsProcessing()
{
    autoProcessingFlag = true;

    time_t wholeTime = time(NULL);
    FILE * res;
    for(int num = 0; num < this->ui->wndLengthBox->count(); ++num)
    {
        emit cleanDirs();



        this->ui->eyesBox->setChecked(true);
        this->ui->windButton->setChecked(true);
        this->ui->wndLengthBox->setCurrentIndex(num);
        this->ui->timeShiftBox->setValue(this->ui->wndLengthBox->currentText().toInt()); //timeShift=wndLength
        this->ui->reduceNsBox->setCurrentIndex(7);
//        this->ui->rdcChannelBox->setCurrentIndex( 7 /*this->ui->rdcChannelBox->count()*/); //current no eyes

        res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
        fprintf(res, "\nwndLength\t%d\n", this->ui->wndLengthBox->currentText().toInt());
        fclose(res);

        //slice
        sliceAll();

        //count spectra
        Spectre *sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));

        sp->countSpectra();
//        sp->compare();
//        sp->compare();
//        sp->compare();
//        sp->psaSlot();
        sp->close();
        if(sp!=NULL) delete sp;

        //cycle Net + write to file
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows"));
        MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
        mkPa->setRdcCoeff(20);

        Net * ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(true);
        ANN->readCfg();
        switch(this->ui->wndLengthBox->currentIndex())
        {
        case 0: {ANN->setErrcrit(0.55); break;}
        case 1: {ANN->setErrcrit(0.5); break;}
        case 2: {ANN->setErrcrit(0.45); break;}
        case 3: {ANN->setErrcrit(0.4); break;}
        case 4: {ANN->setErrcrit(0.35); break;}
        case 5: {ANN->setErrcrit(0.3); break;}
        case 6: {ANN->setErrcrit(0.25); break;}
        default: {ANN->setErrcrit(0.45); break;}
        }

//        while(1)
//        {
//            mkPa->makePaSlot();
//            ANN->PaIntoMatrixByName("all");
//            ANN->LearnNet();
//            ANN->reset();
//            if(ANN->getEpoch() < 70)
//            {
//                mkPa->setRdcCoeff(mkPa->getRdcCoeff()+5);
//            }
//            else if(ANN->getEpoch()>100)
//            {
//                mkPa->setRdcCoeff(mkPa->getRdcCoeff()-1);

//            }
//            else
//            {
//                cout << "NumOfEpoches=" << ANN->getEpoch() << endl << endl;
//                break;
//            }

//            cout << "NumOfEpoches=" << ANN->getEpoch() << endl;
//            cout << "rdcCoeff=" << mkPa->getRdcCoeff() << endl;
//        }
//        res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a");
//        fprintf(res, "rdcCoeff\t%d\n", mkPa->getRdcCoeff());
//        fclose(res);



        int numOfPairs=15;

        for(int i = 0; i < numOfPairs; ++i)
        {
            //make PA
            mkPa->makePaSlot();
            ANN->PaIntoMatrixByName("1_wnd");
            ANN->reset();
            ANN->LearnNet();
            ANN->PaIntoMatrixByName("2_wnd");
            ANN->tall();
            ANN->reset();
            ANN->LearnNet();
            ANN->PaIntoMatrixByName("1_wnd");
            ANN->tall();
        }
        ANN->reset();
//        ANN->PaIntoMatrixByName("all");
//        ANN->LearnNet();
//        ANN->saveWts();
//        ANN->drawWts();
        ANN->closeLogFile();
        //    ANN->leaveOneOut();
        mkPa->close();
        ANN->close();

        if(mkPa!=NULL) delete mkPa;
        if(ANN!=NULL) delete ANN;

        FILE * logFile;
        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("log.txt"));
        //    cout << helpString.toStdString() << endl;
        logFile = fopen(helpString.toStdString().c_str(),"r");
        if(logFile==NULL)
        {
            cout << "logFile==NULL" << endl; return;
        }
        double averagePercentage[4];
        double tempDouble[4];
        for(int j = 0; j < 4; ++j)
        {
            averagePercentage[j] = 0.;
            tempDouble[j] = 0.;
        }

        for(int i = 0; i < numOfPairs*2; ++i)
        {
            for(int j = 0; j < 4; ++j)
            {
                fscanf(logFile, "%lf", &tempDouble[j]);
                averagePercentage[j] += tempDouble[j];
            }
        }
        for(int j = 0; j < 4; ++j)
        {
            averagePercentage[j]/=(numOfPairs*2);
        }
        fclose(logFile);


        res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
        fprintf(res, "PRR \t(");
        for(int j = 0; j < 4-2; ++j)
        {
            fprintf(res, "%.2lf - ", averagePercentage[j]);
        }
        fprintf(res, "%.2lf", averagePercentage[4-2]);
        fprintf(res, ")  -  %.2lf \n", averagePercentage[3]);
        fclose(res);


    }
    wholeTime = time(NULL) - wholeTime;

    cout << "Duration of autoWindowsProcessing = " << wholeTime << " sec" << endl;

    helpString.setNum(int(wholeTime));
    helpString.prepend("All finished \nTime = ");
    helpString.append(" sec");
    QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
}

void MainWindow::autoProcessing()
{
    autoProcessingFlag = true;
    time_t wholeTime = time(NULL);



    //slice
    sliceAll();

    //find eyes
    Cut *cut_e = new Cut(dir, ns);
    cut_e->setAutoProcessingFlag(true);
    cut_e->cutEyesAll();
    cut_e->close();
    if(cut_e!=NULL) delete cut_e;

//    //eyes
    Eyes *trololo = new Eyes(dir, ns);
    trololo->setAutoProcessingFlag(true);
    trololo->eyesProcessing();
    delete trololo;

    //slice w|o eyes
    ui->reduceNsBox->setCurrentIndex(7);
//        ui->eyesBox->setChecked(false);
    ui->eyesBox->setChecked(true);
    sliceAll();
    ui->eyesBox->setChecked(false);
    ui->reduceNsBox->setCurrentIndex(4);

    //count spectra
    Spectre *sp = new Spectre(dir, ns, ExpName);
    QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
    sp->countSpectra();
//    sp->smooth();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    if(sp!=NULL) delete sp;

    //make cfg
    cfg *config = new cfg(dir, spLength);
    config->makeCfg();
    if(config!=NULL) delete config;

    //cycle Net + write to file
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
    MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
    mkPa->setRdcCoeff(5);
    mkPa->mwTest();

    Net * ANN = new Net(dir, ns, left, right, spStep, ExpName);
    ANN->setAutoProcessingFlag(true);
    ANN->readCfg();

    while(1)
    {
        mkPa->makePaSlot();
        ANN->PaIntoMatrixByName("all");
        ANN->LearnNet();
        ANN->reset();
        if(ANN->getEpoch() < 70)
        {
            mkPa->setRdcCoeff(mkPa->getRdcCoeff()+5);
        }
        else if(ANN->getEpoch()>100)
        {
            mkPa->setRdcCoeff(mkPa->getRdcCoeff()-1);

        }
        else
        {
            cout << "NumOfEpoches=" << ANN->getEpoch() << endl << endl;
            break;
        }

        cout << "NumOfEpoches=" << ANN->getEpoch() << endl;
        cout << "rdcCoeff=" << mkPa->getRdcCoeff() << endl;
    }
    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "rdcCoeff\t%d\n", mkPa->getRdcCoeff());
    fclose(res);



    int numOfPairs=10;

    for(int i = 0; i < numOfPairs; ++i)
    {
        //make PA
        mkPa->makePaSlot();
        ANN->PaIntoMatrixByName("1");
        ANN->reset();
        ANN->LearnNet();
        ANN->PaIntoMatrixByName("2");
        ANN->tall();
        ANN->reset();
        ANN->LearnNet();
        ANN->PaIntoMatrixByName("1");
        ANN->tall();
    }    
    ANN->reset();
    ANN->PaIntoMatrixByName("all");
    ANN->LearnNet();
    ANN->saveWts();
    ANN->drawWts();
    ANN->averageClassification();
//    ANN->closeLogFile();   //generality net::averageClassification
//    ANN->leaveOneOut();
    mkPa->close();
    ANN->close();

    if(mkPa!=NULL) delete mkPa;
    if(ANN!=NULL) delete ANN;

//    FILE * logFile;
//    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("log.txt"));
////    cout << helpString.toStdString() << endl;
//    logFile = fopen(helpString.toStdString().c_str(),"r");
//    if(logFile==NULL)
//    {
//        cout << "logFile==NULL" << endl; return;
//    }
//    double averagePercentage[4];
//    double tempDouble[4];
//    for(int j = 0; j < 4; ++j)
//    {
//        averagePercentage[j] = 0.;
//        tempDouble[j] = 0.;
//    }

//    for(int i = 0; i < numOfPairs*2; ++i)
//    {
//        for(int j = 0; j < 4; ++j)
//        {
//            fscanf(logFile, "%lf", &tempDouble[j]);
//            averagePercentage[j] += tempDouble[j];
//        }
//    }
//    for(int j = 0; j < 4; ++j)
//    {
//        averagePercentage[j]/=(numOfPairs*2);
//    }
//    fclose(logFile);


//    res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a");
//    fprintf(res, "PRR \t(");
//    for(int j = 0; j < 4-2; ++j)
//    {
//        fprintf(res, "%.2lf - ", averagePercentage[j]);
//    }
//    fprintf(res, "%.2lf", averagePercentage[4-2]);
//    fprintf(res, ")  -  %.2lf ", averagePercentage[3]);
//    fclose(res);

    wholeTime = time(NULL) - wholeTime;
    cout << "Duration of All = " << wholeTime << " sec" << endl;

    helpString.setNum(int(wholeTime));
    helpString.prepend("All finished \nTime = ");
    helpString.append(" sec");
    QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
}

double MainWindow::fractalDimension(double *arr, int N, QString picPath)
{
    int k; //timeShift
    double L; //average length
//    double sum; //tempSumm

    //NumOfSlices = 8000;
    //max k = 1000; log2(kmax) = 10
    //log(Lmax) = ?

    int minLimit = 2;
//    int maxLimit = int(N/10);
    int maxLimit = floor(log(sqrt(N)) * 4. - 5.);

//    cout<<"minLimit = "<<minLimit<<endl;
//    cout<<"maxLimit = "<<maxLimit<<endl<<endl;

    double * drawK = new double [maxLimit - minLimit];
    double * drawL = new double [maxLimit - minLimit];
//    for(int i = 0; i < N; ++i)
//    {
//        cout<<arr[i]<<" ";
//    }
//    cout<<endl;

//    cout<<"1"<<endl;
//    pic = QPixmap(800, 600);
//    QPainter *pnt     = new QPainter;
//    pic.fill();
//    pnt->begin(&pic);
//    pnt->setPen("black");
//    pnt->setBrush(QBrush("black"));

    for(int h = minLimit; h < maxLimit; ++h)
    {
        k=h;
//        if(h < 5) k = h;
//        else k = exp(log(2) * (h + 5.)/4.);

        L = 0.;
        //count L(k) by averaging Lm(k) over m
//        cout<<"2"<<endl;
        for(int m = 0; m < k; ++m)
        {
            for(int i = 1; i < floor((N - m) / k); ++i)
            {
//                cout<<"3.5 k="<<k<<" m="<<m<<" i="<<i<<" L="<<L<<endl;
//                cout<<arr[m + i * k]
                L += fabs(arr[m + i * k] - arr[m + (i - 1) * k]);
            }
            L = L * (N - 1) / (floor((N - m) / k) * k);
        }
        L /= k;
        drawK[h - minLimit] = log(k);
        drawL[h - minLimit] = log(L);
    }
    for(int h = 0; h < maxLimit - minLimit; ++h)
    {
//        pnt->drawRect(abs((drawK[h] - drawK[0])/(drawK[0] - drawK[maxLimit - minLimit - 1])) * pic.width() - 1, (1. - abs(drawL[h] - drawL[maxLimit - minLimit - 1])/(drawL[0] - drawL[maxLimit - minLimit - 1])) * pic.height() - 1, 2, 2);
    }



//    for(int h = minLimit; h < maxLimit; ++h)
//    {
//        cout<<drawK[h - minLimit]<<"\t";
//    }
//    cout<<endl;
//    for(int h = minLimit; h < maxLimit; ++h)
//    {
//        cout<<drawL[h - minLimit]<<"\t";
//    }
//    cout<<endl;



    //least square approximation
    double slope;
    double add;
    double *temp = new double [5];
    for(int i = 0; i < 5; ++i)
    {
        temp[i] = 0.;
    }
    for(int h = 0; h < maxLimit - minLimit; ++h)
    {
        temp[0] += drawK[h]*drawL[h];
        temp[1] += drawK[h];
        temp[2] += drawL[h];
        temp[3] += drawK[h]*drawK[h];
        temp[4] += drawK[h];
    }
    for(int i = 0; i < 5; ++i)
    {
        temp[i] /= (maxLimit - minLimit);
    }

    slope = (temp[0] - temp[1]*temp[2]) / (temp[3] - temp[1]*temp[1]); // ???????????????????
    add = temp[2] - slope*temp[1]; ///???

//    pnt->setPen("red");
//    pnt->setBrush(QBrush("red"));
//    pnt->drawLine(0, ( (slope*drawK[0]+add - drawL[0])/(drawL[maxLimit - minLimit - 1] - drawL[0])) * pic.height(), pic.width(), ( (slope*drawK[maxLimit - minLimit - 1]+add - drawL[0])/(drawL[maxLimit - minLimit - 1] - drawL[0])) * pic.height());


    slope = -slope;
//    if(picPath.contains("_12.png")) pic.save(picPath);
//    pnt->end();
//    delete pnt;
    delete []temp;
    delete []drawK;
    delete []drawL;
    return slope;
}

void MainWindow::Bayes()
{
    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files);
    dir->cdUp();

    FILE * file;
    double ** dataBayes = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataBayes[i] = new double [250*60*5];
    }
    double maxAmpl = 80.; //generality from readData

    maxAmpl += 0.001; //bicycle

//    int numOfIntervals = maxAmpl/2;
    int numOfIntervals = ui->BayesSpinBox->value();
    numOfIntervals *=2; //odd or even?

    int * count = new int [numOfIntervals];

    double helpDouble = 0.;

    for(int i = 0; i < lst.length(); ++i)
    {
        if(lst[i].contains("num")) continue;
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator()).append(lst[i]));
        file = fopen(helpString.toStdString().c_str(), "r");
        fscanf(file, "NumOfSlices %d\n", &NumOfSlices);
        if(NumOfSlices < 250)
        {
            fclose(file);
            continue;
        }


//        cout<<helpString.toStdString()<<"\t"<<NumOfSlices<<endl;


        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                fscanf(file, "%lf\n", &dataBayes[k][j]);
            }
        }
        fclose(file);

        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("Bayes").append(QDir::separator()).append(lst[i]));
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
                    if(helpInt != min(max(0, helpInt), numOfIntervals-1)) continue;

                    count[helpInt] += 1;
                }
                for(int k = 0; k < numOfIntervals; ++k)
                {
                    helpDouble = count[k]/double(NumOfSlices)*10.;
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
//        cout<<endl;
        fclose(file);


    }
//    cout<<"1"<<endl;
    delete [] count;
//    cout<<"1"<<endl;
    for(int i = 0; i < ns; ++i)
    {
        delete [] dataBayes[i];
    }
    delete [] dataBayes;
    cout<<"Bayes counted"<<endl;
}


void MainWindow::drawWeights()
{
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
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_weights_").append(str(i)).append(".jpg"));
        pic.save(helpString, 0, 100);

    }
    painter->end();
    delete painter;

    cout << "weights drawn" << endl;


}

void MainWindow::drawClassification()  //needs *.dat & weights.wts
{
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
            output[j][l]=logistic2(output[j][l], temperature); // unlinear conformation
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

void MainWindow::changeNsLine(int a)
{
    ui->nsLine->setText(ui->reduceNsBox->itemData(a).toString());
}

void MainWindow::cleanDirs()
{
    if(dir->isRoot())
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No dir"), QMessageBox::Ok);
        return;
    }

    //windows
    if(ui->cleanWindowsCheckBox->isChecked())
    {
        dir->cd("windows");
        lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i)).toStdString().c_str());
        }
        dir->cdUp();
    }

    //SpectraSmooth/windows
    if(ui->cleanWindSpectraCheckBox->isChecked())
    {
        dir->cd("SpectraSmooth");
        dir->cd("windows");
        lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i)).toStdString().c_str());
        }
        dir->cdUp();
        dir->cdUp();
    }


    //SpectraSmooth
    if(ui->cleanRealsSpectraCheckBox->isChecked())
    {
        dir->cd("SpectraSmooth");
        lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i)).toStdString().c_str());
        }
        dir->cdUp();
    }

    //windows/fromreal
    if(ui->cleanFromRealsCheckBox->isChecked())
    {
        dir->cd("windows");
        dir->cd("fromreal");
        lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i)).toStdString().c_str());
        }
        dir->cdUp();
        dir->cdUp();
    }

    //Realisations
    if(ui->cleanRealisationsCheckBox->isChecked())
    {
        dir->cd("Realisations");
        lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i)).toStdString().c_str());
        }
        dir->cdUp();
    }

    //Help
    if(ui->cleanHelpCheckBox->isChecked())
    {
        dir->cd("Help");
        lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i)).toStdString().c_str());
        }
        dir->cdUp();
    }

    cout << "dirs cleaned" << endl;

    helpString="dirs cleaned ";
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);
}

void MainWindow::drawEeg(int NumOfSlices_, double **dataD_, QString helpString_, int freq)
{
    int start = 0; // staSlice; //generality
    QPixmap pic = QPixmap(NumOfSlices_, 600);  //cut.cpp Cut::paint() generality
    pic.fill();
    QPainter * paint_ = new QPainter();
    paint_->begin(&pic);

    double norm=ui->doubleSpinBox->value();              //////////////////////

    for(int c1 = 0; c1 < pic.width(); ++c1)
    {
        for(int c2 = 0; c2 < ns; ++c2)
        {
            if(ns==21 && c2==19) paint_->setPen("red");
            else if(ns==21 && c2==20) paint_->setPen("blue");
            else paint_->setPen("black");

            paint_->drawLine(c1, (c2+1)*pic.height()/(ns+2) +dataD_[c2][c1+start]/norm, c1+1, (c2+1)*pic.height()/(ns+2) +dataD_[c2][c1+1+start]/norm);
        }
    }
    norm=1.;
    paint_->setPen("black");
    for(int c3 = 0; c3 < NumOfSlices_*10/250; ++c3)
    {
        if(c3%5  == 0) norm=15.;
        if(c3%10 == 0) norm=20.;

        paint_->drawLine(c3*freq/5, pic.height()-2, c3*freq/5, pic.height()-2*norm);
        int helpInt=c3;
        helpString=str(helpInt);
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
    double stretch = ceil(freq/512);
    QPixmap pic = QPixmap((endTime-startTime)*freq/stretch, 600);  //cut.cpp 851
    pic.fill();
    QPainter * paint_ = new QPainter();
    paint_->begin(&pic);

    double norm=ui->doubleSpinBox->value();              //////////////////////


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
        helpString=str(helpInt);
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

    duration = time(NULL);
    ui->progressBar->setValue(0);
    double **dataD = new double * [ns];
    int Eyes;

    dir->cd(ui->drawDirBox->currentText());    //->windows or Realisations or cut
    lst.clear();
    nameFilters.clear();
    nameFilters.append("*_241*");
    nameFilters.append("*_247*");
    nameFilters.append("*_254*");
    lst = dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot);
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
            if(ns==21)
            {
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("before").append(QDir::separator()).append(helpString).append(".jpg");
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
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);

    ui->progressBar->setValue(0);

    duration = time(NULL) - duration;
    cout << "Duration of DrawRealisations = " << duration << " sec" << endl;

    helpString.setNum(int(duration));
    helpString.prepend("Signals drawn \nTime = ");
    helpString.append(" sec");
    stopFlag = 0;
    QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
}

void MainWindow::diffSmooth()
{
    Spectre *sp;
    Net * ANN;
    duration = time(NULL);
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
        ANN->readCfg();
        ANN->autoClassification("SpectraSmooth");
        ANN->close();
        cout << "smooth = " << i << " done" << endl;
        delete ANN;
        sleep(60);

    }
    duration = time(NULL) - duration;
    cout << "Duration of diffSmooth = " << duration << " sec" << endl;

}

void MainWindow::setNs()
{
    ns=ui->setNsLine->text().toInt();
    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}

void MainWindow::setNs2(int a)
{
    ns=a;
    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}

void MainWindow::netShow()
{
    Net * ANN = new Net(dir, ns, left, right, spStep, ExpName);
    ANN->show();
}

void MainWindow::cutShow()
{
    Cut *cut_e = new Cut(dir, ns);
    cut_e->show();
//    cut_e->cutEyesAll();
//    cut_e->close();
}

void MainWindow::makePaSlot() //250 - frequency generality
{
    if(spStep == 250./4096.) helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
    else if(spStep == 250./1024.)     helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows"));
    else helpString = QDir::toNativeSeparators(dir->absolutePath());


    MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
    mkPa->show();
}

void MainWindow::makeShow()//250 - frequency generality
{
    if(spStep == 250./4096.) helpString = "16sec19ch";
    else if(spStep == 250./1024.) helpString = "4sec19ch";
    else helpString = "netFile";
    cfg *config = new cfg(dir, ns, spLength, 0.10, 0.10, helpString);
    config->show();
}

void MainWindow::setEdfFile()
{
    NumOfEdf = 0;
    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)this, tr("EDF to open"), "/media/Files/Data", tr("EDF files (*.edf)")));
    if(helpString=="")
    {
        QMessageBox::warning((QWidget*)this, tr("Warning"), tr("no EDF file was chosen"), QMessageBox::Ok);
        return;
    }
//    ui->reduceNsBox->setCurrentIndex(4);


    ui->filePath->setText(helpString);
    edf = fopen(helpString.toStdString().c_str(), "r");
    if(edf==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Critical"), tr("Cannot open EDF file"), QMessageBox::Ok);
        return;
    }



    //set ExpName
    int pointNumber;
    int slashNumber;

    for(int i=helpString.length(); i>0; --i)
    {
        if(helpString[i]=='.') pointNumber=i;
        if(helpString[i]==QDir::separator())
        {
            slashNumber=i;
            break;
        }
    }

    ExpName="";
    for(int i=slashNumber+1; i < pointNumber; ++i)
    {
        ExpName.append(helpString[i]);
    }
    this->ui->Name->setText(ExpName);

    helpString.resize(slashNumber);
    dir->cd(helpString);                                            //current dir

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
    dir->mkdir("SpectraSmooth/Bayes");
    dir->mkdir("SpectraSmooth/windows");
    dir->mkdir("windows");
    dir->mkdir("windows/fromreal");
    dir->mkdir("Realisations");
    dir->mkdir("Realisations/BC");
    dir->mkdir("SpectraPCA");
    dir->mkdir("cut");


    helpString="EDF file read successfull";
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);


    //automatization
    if(ui->autoProcessingBox->isChecked())
    {
        FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
        fprintf(res, "\n%s\n", ExpName.toStdString().c_str());
        fclose(res);
        autoProcessing();
    }

    if(ui->autoWindowsCheckBox->isChecked())
    {
        FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
        fprintf(res, "\n%s\n", ExpName.toStdString().c_str());
        fclose(res);
        autoWindowsProcessing();
    }

/*
    FILE * file = fopen("/media/Files/Data/AAX/test.txt", "w");
    fprintf(file, "NumOfSlices 3000\n");
    double **arr = new double * [2];
    arr[0] = new double [3000];
    arr[1] = new double [3000];
    for(int i = 0; i < 3000; ++i)
    {
        arr[0][i] = sin(10. * 2.*M_PI * i/250.);
        arr[1][i] = sin(10. * 2.*M_PI * (i + 25)/250.);
        fprintf(file, "%lf\n%lf\n", arr[0][i], arr[1][i]);
    }
    fclose(file);



    //test wavelet
    helpString = dir->absolutePath() + "/test.txt";
    file = fopen(helpString.toStdString().c_str(), "r");

//    helpString = dir->absolutePath() + "/ww1.jpg";
//    wavelet(helpString, file, 2, 0, 20., 5., 0.98, 32);

//    helpString = dir->absolutePath() + "/ww2.jpg";
//    wavelet(helpString, file, 2, 1, 20., 5., 0.98, 32);

    helpString = dir->absolutePath() + "/www.jpg";
    waveletPhase(helpString, file, 2, 0, 1, 20., 5., 0.98, 32);
    fclose(file);
    this->close();*/
}

void MainWindow::setExpName()
{
    ExpName=this->ui->Name->text();
    helpString="Name approved\n";
    this->ui->textEdit->append(helpString);
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

    char *helpCharArr = new char[50];
    char helpChar;
    int bytes;
    short int a;


    FILE *edfNew;
    int flag = 0;

    QFile *tempFile = new QFile();
    tempFile->open(edf, QIODevice::ReadOnly);
    if(ui->edfBox->isChecked())
    {
        flag=1;
        helpString = tempFile->fileName();
        helpString.resize(helpString.length()-4);
        helpString.append(str(NumOfEdf)).append(".edf");
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
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }

    //number of bytes in header record
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        if(flag==1) fprintf(edfNew, "%c", helpCharArr[i]);
    }
    bytes=atoi(helpCharArr);




    //"reserved"
    for(int i = 0; i < 44; ++i)
    {
        fscanf(edf, "%c", &helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }

    //number of data records
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
    }
    ndr=atoi(helpCharArr);                      //NumberOfDataRecords

    //duration of a data record, in seconds
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
    }
    ddr=atoi(helpCharArr);                       //DurationOfDataRecord
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
        if(flag==1) fprintf(edfNew, "%c", helpCharArr[i]);
    }
    ns=atoi(helpCharArr);                        //Number of channels
    cout << "ns=" << ns << endl;

    //labels
    char **label;
    label = new char* [ns];     //memory for channels' labels

    for(int i = 0; i < ns; ++i)
    {
        label[i] = new char [16];
    }
    for(int i = 0; i < ns*16; ++i)                      //label read
    {
        fscanf(edf, "%c", &label[i/16][i%16]);
        if(flag==1) fprintf(edfNew, "%c", label[i/16][i%16]);
        if(i%16==15) label[i/16][i%16]='\0';

    }
    helpString=dir->absolutePath().append(QDir::separator()).append("labels.txt");
    FILE * labels=fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    for(int i = 0; i < ns; ++i)                         //label write in file
    {
        fprintf(labels, "%s \n", label[i]);
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []label[i];
    }
    delete []label;


    //transducer type
    for(int i = 0; i < ns*80; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }


    //physical dimension
    for(int i = 0; i < ns*8; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
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
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        digMin[i]=double(atoi(helpCharArr));
    }

    //digital maximum
    double *digMax;
    digMax = new double [ns];     //memory for channels' labels

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        digMax[i]=double(atoi(helpCharArr));
    }

    //prefiltering
    for(int i = 0; i < ns*80; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }



    //number of records (nr samples in ddr seconds)
    nr = new int [ns];
    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            if(flag==1) fprintf(edfNew, "%c", helpCharArr[j]);
        }
        helpCharArr[8] = '\0';
        nr[i]=atoi(helpCharArr);
    }


    //reserved
    for(int i = 0; i < ns*32; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }


    for(int i = 0; i < (bytes-(ns+1)*256); ++i)                      //Neurotravel//generality//strange
    {
        fscanf(edf, "%c", &helpChar);
        if(flag==1) fprintf(edfNew, "%c", helpChar);
    }

//    int maxNr = 0;
//    for(int i = 0; i < ns; ++i)
//    {
//        maxNr = max(maxNr, nr[i]);
//    }


    data = new double* [ns];         //array for all data[numOfChan][numOfTimePin]
    nsBackup=ns;
    for(int i = 0; i < ns; ++i)
    {
        data[i] = new double [nr[i]*ndr];           //////////////for 200 minutes//////////////
    }


    ui->finishTimeBox->setMaximum(ddr*ndr);

    helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_markers.txt");
    FILE * markers = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");

    QString *annotations = new QString [10000];
    int numOfAnn = 0;
    int currStart;


    fpos_t *position = new fpos_t;
    fgetpos(edf, position);
    fclose(edf);
    edf = fopen(QDir::toNativeSeparators(ui->filePath->text()).toStdString().c_str(), "rb"); //generality
    fsetpos(edf, position);
    delete position;

//    cout << "start data read ndr=" << ndr << " ns=" << ns << endl;
    if(ui->ntRadio->isChecked())
    {
        for(int i = 0; i < ndr; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
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
        }
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
                    fread(&a, sizeof(short), 1, edf);
                    if(flag==1 && ((i*ddr)>=ui->startTimeBox->value()) && ((i*ddr+1) <=ui->finishTimeBox->value()))    //save as EDF
                    {
                        fwrite(&a, sizeof(short), 1, edfNew);
                    }
                    data[j][i*nr[j]+k] = physMin[j] + (physMax[j]-physMin[j]) * (double(a)-digMin[j]) / (digMax[j] - digMin[j]);   //enc

                    if(j==(ns-1))
                    {
                        if(a>=1)
                        {
                            bytes=i*nr[j]+k;
                            fprintf(markers, "%d %d\n", bytes, int(a));
                        }

                        if(a==200)
                        {
                            staSlice=i*nr[j]+k;
                        }
                    }
                }

            }
        }
        cout << "staSlice=" << staSlice << " staTime=" << staSlice/250. << endl;
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
    delete []annotations;

    cout<<"ndr*ddr = " << ndr*ddr << endl;


//    cout << "data have been read" << endl;
    helpString="data have been read ";
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);

    staSlice += 3; //generality LAWL

}

void MainWindow::makeDatFile()
{
    this->readData();
    if(ui->eyesBox->isChecked()) this->eyesFast();
    ui->reduceNsBox->setCurrentIndex(7); //20 channels 19+markers
    if(ui->chRdcBox->isChecked()) this->reduceChannelsFast();


//    helpString=dir->absolutePath().append(QDir::separator()).append(ExpName).append(".dat");

    int startTime=ui->startTimeBox->value();
    cout << "startTime=" << startTime << endl;
    int finishTime=ui->finishTimeBox->value();
    cout << "finishTime=" << finishTime << endl;

    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(ExpName).append(".dat"));

    cout << "ns in dat-file=" << ns << endl;
    FILE * datFile = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(),"w");
    if(datFile==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("critical"), tr("cannot open datFile"), QMessageBox::Ok);
        return;
    }

    fprintf(datFile, "NumOfSlices %d\n", int(finishTime-startTime)*nr[0]);

    for(int i=startTime*250; i < finishTime*250; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fprintf(datFile, "%lf\n", data[j][i]);
        }
    }
    fclose(datFile);

    for(int i = 0; i < nsBackup; ++i)  ///////////////////////////
    {
        if(data[i]==NULL) break;
        delete []data[i];
    }
    delete []data;
    delete []nr;
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
        else cout << num/250. << endl;
        fclose(fil);

    }
    dir->cdUp();

    solveTime = av / (250.*lst.length());
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

    solveTime = av / (250.*lst.length());
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

    int eyesCounter;
    duration = time(NULL);
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


    timeShift=ui->timeShiftBox->value();
    wndLength=ui->wndLengthBox->currentText().toInt();

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
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);

    duration = time(NULL) - duration;
    cout << "Duration of WindRealSlice = " << duration << " sec" << endl;

    helpString.setNum(int(duration));
    helpString.prepend("Data sliced \nTime = ");
    helpString.append(" sec");
    //automatization
    if(autoProcessingFlag == false)
    {
        QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
    }

}

void MainWindow::sliceAll() ////////////////////////aaaaaaaaaaaaaaaaaaaaaaaaaa//////////////////
{
    duration=time(NULL);
    int marker=254;
    int markerFlag = 0;

    this->readData();


    // ICA test - OK
//    int indepNum = 4;

//    double ** testSignals = new double * [indepNum];
//    for(int i = 0; i < indepNum; ++i)
//    {
//        testSignals[i] = new double [ndr*nr[i]];
//    }

//    double ** testSignals2 = new double * [ns];
//    for(int i = 0; i < ns; ++i)
//    {
//        testSignals2[i] = new double [ndr*nr[i]];
//    }

//    srand(QTime::currentTime().msec());
//    for(int i = 0; i < ndr*nr[0]; ++i)
//    {
//        testSignals[0][i] = 20.*sin(2*3.1415926*(double(i)/23.)) + 1.; //23 bins period
//        testSignals[1][i] = i%41 - 20.;      //a saw 40 period
//        testSignals[2][i] = ((i/31)%2 - 0.5)*40.;//square signal 32 period
//        testSignals[3][i] = (rand()%30 - 15.)/5.; //white noise
//    }

//    for(int i = 0; i < ndr*nr[0]; ++i)
//    {
//        testSignals2[0][i] = 0.33 * testSignals[0][i] + 0.45 * testSignals[1][i] - 0.71 * testSignals[2][i] + 0.3 * testSignals[3][i];
//        testSignals2[1][i] = 0.15 * testSignals[0][i] + 0.50 * testSignals[1][i] + 0.20 * testSignals[2][i] + 0.2 * testSignals[3][i];
//        testSignals2[2][i] = 0.30 * testSignals[0][i] + 0.50 * testSignals[1][i] + 0.50 * testSignals[2][i] + 0.4 * testSignals[3][i];
//        testSignals2[3][i] = 0.2 * testSignals[0][i] + 0.23 * testSignals[1][i] + 0.25 * testSignals[2][i] + 0.5 * testSignals[3][i];
//        for(int j = indepNum; j < ns; ++j)
//        {
//            testSignals2[j][i] = data[j][i];
//        }
//    }
//    cout << "1" << endl;
//    helpString = ExpName; helpString.append("_test.edf");
//    writeEdf(edf, testSignals2, helpString, indepNum);


    if(ui->eyesBox->isChecked())
    {
        this->eyesFast();
        this->ui->reduceNsBox->setCurrentIndex(7);
        helpString = ExpName.append("_ec.edf");
    }


    if(ui->chRdcBox->isChecked()) this->reduceChannelsFast();

    if(this->ui->sliceBox->isChecked())
    {
        if(ui->ntRadio->isChecked())
        {
            slice(10, 49, "m"); //math.operation
            slice(50, 89, "e"); //emotional verb
            slice(90, 129, "v"); //verb
            slice(130, 169, "n"); //noun
            slice(170, 209, "a"); //number
        }

        if(ui->enRadio->isChecked())
        {
            if(ui->windButton->isChecked())
            {
                timeShift=ui->timeShiftBox->value();
                wndLength=ui->wndLengthBox->currentText().toInt();

                for(int i = 0; i < (ndr*nr[ns-1]-staSlice-10*nr[ns-1])/timeShift; ++i)
                {

                    for(int j = 0; j < wndLength; ++j)
                    {
                        if(data[ns-1][staSlice+i*timeShift + j]==241)
                        {
                            markerFlag = 0;
                            marker=300;
                        }
                        if(data[ns-1][staSlice+i*timeShift + j]==247)
                        {
                            markerFlag = 0;
                            marker=300;
                        }
                        if(data[ns-1][staSlice+i*timeShift + j]==254)
                        {
                            markerFlag = 0;
                            marker=300;
                        }
                    }
                    if(markerFlag==1) marker=241;
                    if(markerFlag==2) marker=247;
                    if(markerFlag==3) marker=254;

//                    cout << i << endl;
                    if(marker!=300) sliceWindow(staSlice+i*timeShift, staSlice+i*timeShift+wndLength, int(i+1), marker);

                    for(int j = 0; j < wndLength; ++j)
                    {
//                        if(data[ns-1][staSlice+i*timeShift + j]==241) {markerFlag=1; break;}
//                        if(data[ns-1][staSlice+i*timeShift + j]==247) {markerFlag=2; break;}
//                        if(data[ns-1][staSlice+i*timeShift + j]==254) {markerFlag=3; break;}
                        if(data[ns-1][staSlice+i*timeShift + j]==241) {markerFlag=1;}
                        if(data[ns-1][staSlice+i*timeShift + j]==247) {markerFlag=2;}
                        if(data[ns-1][staSlice+i*timeShift + j]==254) {markerFlag=3;}
                    }
                    if(int(100*(i+1)/((ndr*nr[ns-1]-staSlice-10*nr[ns-1])/timeShift)) > ui->progressBar->value()) ui->progressBar->setValue(int(100*(i+1)/((ndr*nr[ns-1]-staSlice-10*nr[ns-1])/timeShift)));

//                                        if(i==50) break; //20 windows

                }
            }
            if(ui->realButton->isChecked())
            {
                if(ui->reduceNsBox->currentText().contains("MichaelBak"))
                {
                    sliceBak(1, 60, "241");
                    sliceBak(61, 120, "247");
                    sliceBak(121, 180, "241");
                    sliceBak(181, 240, "247");
                }
                else
                {
//                    sliceOneByOne();
                    sliceOneByOneNew();
//                    sliceGaps();
//                    sliceByNumberAfter(241, 241, "241"); //Spatial
//                    sliceByNumberAfter(247, 247, "247"); //Verbal

                    sliceFromTo(241, 231, "241_pre");
                    sliceFromTo(247, 231, "247_pre");  //accord with presentation markers
                    sliceFromTo(247, 237, "247_pre");

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


                    //                    //count average time of solving
                    //                    dir->cd("Realisations");
                    //                    lst = dir->entryList(QStringList("*_241*"));

                    //                    FILE * real;
                    //                    int tmp;

                    //                    helpInt = 0;
                    //                    for(int i = 0; i < lst.length(); ++i)
                    //                    {
                    //                        helpString = dir->absolutePath().append(QDir::separator()).append(lst.at(i));
                    //                        real = fopen(helpString.toStdString().c_str(), "r");
                    //                        fscanf(real, "NumOfSlices %d", &tmp);
                    //                        helpInt += tmp;
                    //                        fclose(real);
                    //                    }
                    //                    helpInt/=lst.length()*250.;
                    //                    dir->cdUp();
                    //                    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a");
                    //                    fprintf(res, "solve time 241 \t %lf \n", helpInt);
                    //                    fclose(res);


                    //                    dir->cd("Realisations");
                    //                     lst = dir->entryList(QStringList("*_247*"));

                    //                    helpInt = 0;
                    //                    for(int i = 0; i < lst.length(); ++i)
                    //                    {
                    //                        helpString = dir->absolutePath().append(QDir::separator()).append(lst.at(i));
                    //                        real = fopen(helpString.toStdString().c_str(), "r");
                    //                        fscanf(real, "NumOfSlices %d", &tmp);
                    //                        helpInt += tmp;
                    //                        fclose(real);
                    //                    }
                    //                    helpInt/=lst.length()*250.;
                    //                    dir->cdUp();
                    //                    res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a");
                    //                    fprintf(res, "solve time 247 \t %lf \n", helpInt);
                    //                    fclose(res);


                }
            }

            /*
                //Ilya

                ns=20;
                sliceIlya(101, 160, helpString);
                dir->cd("Realisations");

                dir->mkdir("DODO");
                dir->mkdir("DONT");
                dir->mkdir("Begin");
                dir->mkdir("St");
                dir->mkdir("Beep");

                QStringList lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
                for(int i = 0; i < lst.length(); ++i)
                {
                    sliceIlya(lst.at(i), helpString);
                }
                dir->cdUp();
*/

        }
        --ns; //-markers channel generality
        ui->progressBar->setValue(0);

        //            ui->sliceBox->setChecked(false);
        //            ui->sliceBox->setCheckable(false);
        //            ui->eyesBox->setChecked(false);
        //            ui->eyesBox->setCheckable(false);
        //            ui->chRdcBox->setChecked(false);
        //            ui->chRdcBox->setCheckable(false);

        for(int i = 0; i < nsBackup; ++i)  ///////////////////////////
        {
            if(data[i]==NULL) break;
            delete []data[i];
        }
        delete []data;
//        delete []nr;
        helpString="data has been sliced \n";
        this->ui->textEdit->append(helpString);


    }
    else
    {
        for(int i = 0; i < nsBackup; ++i) //////////////////////////////////////////////
        {
            if(data[i]==NULL) break;
            delete []data[i];
        }
        delete []data;
//        delete []nr;
    }

    helpString="data sliced ";
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);

    duration = time(NULL) - duration;
    cout << "Duration of SliceAll = " << duration << " sec" << endl;

    helpString.setNum(int(duration));
    helpString.prepend("Data sliced \nTime = ");
    helpString.append(" sec");
    //automatization
    if(autoProcessingFlag == false)
    {
        QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
    }
}


void MainWindow::sliceIlya(const QString &fileName, QString marker) //beginning - from mark1 to mark 2, end - from 251 to 255. Marker - included in filename
{
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
//        cout << "solve[" << i << "]=" << solve[i]/(12 *250.) << endl;
//    }

    solveTime/=(250.*number);
    cout << "solveTime " << marker.toStdString() << " =" << solveTime << endl << endl;

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    if(ui->eyesBox->isChecked()) fprintf(res, "solve time %s \t %lf \n", marker.toStdString().c_str(), solveTime);
    fclose(res);
}

void MainWindow::sliceFromTo(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - 254. Marker - included in filename
{
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
    file2->write(str(number).toStdString().c_str());
    file2->write("\r\n");
    file2->write(contents);
    file2->close();
    delete file2;

    kernelest(helpString);

    solveTime/=(250.*number);
    cout << "average time before feedback " << marker.toStdString() << " =" << solveTime << endl;

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    if(ExpName.contains("FB")) fprintf(res, "time before feedback %s\t%lf\n", marker.toStdString().c_str(), solveTime);
    fclose(res);
}

void MainWindow::sliceByNumberAfter(int marker1, int marker2, QString marker)
{
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
    if(endSlice - startSlice > 2500) return;
    FILE * file;
    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("windows").append(QDir::separator()).append(ExpName).append("-").append(rightNumber(number, 4)).append("_").append(str(marker)); //number.marker
    file=fopen(helpString.toStdString().c_str(), "w");
    if(file==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open file"), QMessageBox::Ok);
        return;
    }

    fprintf(file, "NumOfSlices %d \n", endSlice-startSlice);
    for(int l=startSlice; l < endSlice; ++l)
    {
        for(int m = 0; m < ns-1; ++m)
        {
            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
        }
    }
    fclose(file);
}

void MainWindow::sliceGaps()
{
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
                for(int l=j; l < k; ++l)         //save BY SLICES!!
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
//            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
            file = fopen(helpString.toStdString().c_str(), "w");
            fprintf(file, "NumOfSlices %d \n", i-j);
            if(i-j > 15000 && (marker == "255" || marker == "254"))
            {
                for(int l = j; l < i; ++l)         //save BY SLICES!!
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "0.000\n");
                    }
                }
            }
            else
            {
                for(int l = j; l < i; ++l)         //save BY SLICES!!
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
            for(int l = j; l < k; ++l)         //save BY SLICES!!
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
    for(int l = j; l < k; ++l)         //save BY SLICES!!
    {
        for(int m = 0; m < ns-1; ++m)
        {
            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
        }

    }
    fclose(file);
}

void MainWindow::sliceOneByOneNew()
{
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
        if((data[ns-1][i] > 200 && data[ns-1][i] < 241) || data[ns-1][i] == 255 || data[ns-1][i] == 250 || data[ns-1][i] == 251) //order inportant! - not interesting markers
        {
            continue;
        }
        if(data[ns-1][i] == 241 || data[ns-1][i] == 247)
        {
            marker = "254";
            if(data[ns-1][i] == 241) h=0;
            else if (data[ns-1][i] == 247) h=1;
            continue;
        }
        if(1)
        {
            if(marker.isEmpty())
            {
                marker = "sht";
            }
            ++number;
            helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);

//            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
            file = fopen(helpString.toStdString().c_str(), "w");
            fprintf(file, "NumOfSlices %d \n", i-j);
            if(i-j > 15000 && (marker == "254"))
            {
                for(int l = j; l < i; ++l)         //save BY SLICES!!
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "0.000\n");
                    }
                }
            }
            else
            {
                for(int l = j; l < i; ++l)         //save BY SLICES!!
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
            }
            fclose(file);

            marker.clear();
            if(h == 0) marker = "241";
            else if(h == 1) marker = "247";
            h = -1;
            j = i;
            continue;
            ui->progressBar->setValue(double(i)*100./ndr*nr[ns-1]);
        }

    }

    //write last rest state
    marker = "end";
    k = ndr*nr[ns-1];
    ++number;
    helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
//    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
    file=fopen(helpString.toStdString().c_str(), "w");
    fprintf(file, "NumOfSlices %d \n", k-j);
    for(int l = j; l < k; ++l)         //save BY SLICES!!
    {
        for(int m = 0; m < ns-1; ++m)
        {
            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
        }

    }
    fclose(file);
}

double gaussian(double x)
{
    return 1/(sqrt(2*M_PI))*exp(-x*x/2.);
}

void MainWindow::kernelest(const QString &inputString)
{
    FILE * file = fopen(QDir::toNativeSeparators(inputString).toStdString().c_str(), "r");
    int num;
    fscanf(file, "NumOfFiles %d\n", &num);
    int *arr = new int [num];
    double average = 0.;
    double sigma = 0.;
    for(int i = 0; i < num; ++i)
    {
        fscanf(file, "%d", &arr[i]);
        average += arr[i];
    }
    average/=num;
    for(int i = 0; i < num; ++i)
    {
        sigma += (average-arr[i])*(average-arr[i]);
    }
    sigma=sqrt(sigma)/num;   //// (sigma)/num ??
    average=-0.2*log(num);
    double h=1.06*sigma*exp(average);

    QPixmap pic(1000, 400);
    QPainter * pnt = new QPainter();
    pic.fill();
    pnt->begin(&pic);
    pnt->setPen("black");

    for(int i = 0; i < pic.width(); ++i)
    {
        average = 0.;
        for(int j = 0; j < num; ++j)
        {
            average += 1/(num*h) * gaussian((i+1000 - arr[j])/h)*1e4; //1000 - 4 sec input lag
        }
//        cout << i << " " << average << endl;
        if(i>0) pnt->drawLine(i-1, pic.height()*0.9-sigma, i, pic.height()*0.9-average);
        sigma = average;
    }
    pnt->drawLine(0, pic.height()*0.9+1, pic.width(), pic.height()*0.9+1);
    for(int i = 0; i < int(pic.width()/250); ++i)
    {
        pnt->drawLine(i*250, pic.height()*0.9+1, i*250, pic.height());
        pnt->drawText(i*250+10, pic.height()*0.95, str(i));
    }
    helpString = inputString;
    helpString.append(".jpg");
    pic.save(helpString, 0, 100);

    delete pnt;

    delete []arr;
}

void MainWindow::reduceChannelsFast()
{
    int *num = new int[maxNs];

    QStringList list = this->ui->nsLine->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);

    for(int i = 0; i < list.length(); ++i)
    {
        num[i]=list.at(i).toInt();
    }
    double ** temp = new double *[ns];
    for(int i = 0; i < ns; ++i)
    {
        temp[i] = new double [250*60*200]; //generality for 200 minutes
    }
    for(int k = 0; k < list.length(); ++k)
    {
        for(int j = 0; j < ndr*nr[num[k]-1]; ++j)
        {
            temp[k][j]=data[num[k]-1][j];
        }
    }
    for(int k = 0; k < list.length(); ++k)
    {
//        nr[k]=nr[num[k]-1];             //generality
        for(int j = 0; j < ddr*ndr*nr[k]; ++j)
        {            
            data[k][j]=temp[k][j];
        }
    }


    for(int i = 0; i < ns; ++i)
    {
        delete []temp[i];
    }
    delete []temp;


    ns=list.length();
    cout << "channels reduced, ns=" << ns << endl;
    delete []num;

    helpString="channels reduced fast ";
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);

}

void MainWindow::eyesFast()  //generality
{
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

    if(ui->enRadio->isChecked())
    {
        if(ui->reduceNsBox->currentText().contains("MichaelBak"))  //generality
        {
            a[0]=22; //NumOfEEg channel for En (19 EEG, A1-A2, A1-N, ECG, Eog1, Eog2) //generality
            a[1]=23;
        }
        else
        {
            //my current
            a[0]=21; //NumOfEEg channel for En (19 EEG, A1-A2, A1-N, Eog1, Eog2) //generality
            a[1]=22;
        }
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
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);
}

void MainWindow::takeSpValues(int b, int c, double d)
{
    spLength = c-b+1;
    left = b;
    right = c;
    spStep = d;

//    cout << "spVal taken " << spLength << " " << left << " " << right << " " << spStep << endl;
    helpString="SpVal taken";
    this->ui->textEdit->append(helpString);
}

void MainWindow::countSpectra()               ////////////////to remake////////////////////
{    
    Spectre *sp = new Spectre(dir, ns, ExpName);
    QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
    sp->show();


    if(ui->autoProcessingBox->isChecked())
    {
        sp->countSpectra();
//        sp->smooth();
        sp->compare();
        sp->compare();
        sp->compare();
        sp->psaSlot();
        sp->close();
    }

}

void MainWindow::reduceChannels()
{
    helpString=this->ui->nsLine->text();

    int *num = new int[maxNs];
    FILE * file;

    double **dataR = new double*[ns];
    for(int i = 0; i < ns; ++i)
    {
        dataR[i] = new double [10000];   ///////////////generality spLength
    }


    QStringList list = helpString.split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(int i = 0; i < list.length(); ++i)
    {
        num[i]=list.at(i).toInt();
    }

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files, QDir::NoSort);

    for(int i = 0; i < lst.length(); ++i)
    {
        file=fopen((QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i))).toStdString().c_str(), "r");
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
    ns=list.length();
    delete []num;
    dir->cdUp();

    helpString="channels reduced ";
    this->ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(str(ns));
    this->ui->textEdit->append(helpString);
}


//products for ICA


double * product1(double ** arr, int length, int ns, double * vec)
{
    //X*g(Wt*X)

    double * tempVector2 = new double [ns];
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] = 0.;
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
            tempVector2[i] += tanh(sum)*arr[i][j];
        }
    }
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] /= length;
    }

    return tempVector2;
}



double * product2(double ** arr, int length, int ns, double * vec)
{
    //g'(Wt*X)*1*W
    double * tempVector2 = new double [ns];
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] = 0.;
    }

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
        tempVector2[i] += sum1 * vec[i];
    }

    return tempVector2;

}



double * product3(double ** vec, int ns, int i)
{
    //sum(Wt*Wi*Wi)

    double * tempVector2 = new double [ns];
    for(int k = 0; k < ns; ++k)
    {
        tempVector2[k] = 0.;
    }
    double sum = 0.;

    for(int j = 0; j < i; ++j)
    {
        sum = 0.;
        for(int k = 0; k < ns; ++k)
        {
            sum += vec[i][k]*vec[j][k];
        }
        for(int k = 0; k < ns; ++k)
        {
            tempVector2[k] += vec[j][k] * sum;
        }

    }

    return tempVector2;

}

double * randomVector(int ns)
{

    double * tempVector2 = new double [ns];
    srand(QTime::currentTime().msec());
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

void MainWindow::constructEDF()
{
//    cout<<"ndr*nr[0] = "<<ndr*nr[0]<<endl;

    double ** newData = new double * [ns];//19 generality?
    for(int i = 0; i < ns; ++i)//19 generality?
    {
        newData[i] = new double [ndr*nr[i]];
    }

    dir->cd("Realisations");


    lst = dir->entryList(QDir::Files, QDir::Name);
    cout<<"NumOfFiles = "<<lst.length()<<endl;
    dir->cdUp();

    FILE * file;
    int currSlice = 0;
    for(int i = 0; i < lst.length(); ++i)
    {
//        cout<<"currSlice = "<<currSlice<<endl;
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator()).append(lst[i]));
        file = fopen(helpString.toStdString().c_str(), "r");
        fscanf(file, "NumOfSlices %d\n", &NumOfSlices);
//        cout<<"NumOfSlices = "<<NumOfSlices<<endl;
        for(int i = 0; i < NumOfSlices; ++i)
        {
            for(int j = 0; j < ns; ++j) //19 most generality?
            {
                fscanf(file, "%lf\n", &newData[j][currSlice]);
//                cout<<"chan "<<j<<" slice "<<currSlice<<endl;
            }
            ++currSlice;
        }
        fclose(file);
    }


    int nsB = ns;
    cout<<ns<<endl;


    helpString = ExpName.append("_clean.edf");
    writeEdf(edf, newData, helpString, ns); //19 generality?

    for(int i = 0; i < nsB; ++i)
    {
        delete []newData[i];
    }
    delete []newData;
}


void MainWindow::writeEdf(FILE * edf, double ** inData, QString fileName, int indepNum)
{

    char *helpCharArr = new char[50];
    char helpChar;
    int bytes;
    short int a;


    FILE *edfNew;


    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(fileName));
    edfNew = fopen(helpString.toStdString().c_str(), "w");

    //header read
    for(int i = 0; i < 184; ++i)
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }

    //number of bytes in header record
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(edfNew, "%c", helpCharArr[i]);
    }
    bytes=atoi(helpCharArr);




    //"reserved"
    for(int i = 0; i < 44; ++i)
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }

    //number of data records
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(edfNew, "%c", helpCharArr[i]);
    }
    ndr=atoi(helpCharArr);//NumberOfDataRecords

    //duration of a data record, in seconds
    for(int i = 0; i < 8; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(edfNew, "%c", helpCharArr[i]);
    }
    ddr=atoi(helpCharArr);                       //DurationOfDataRecord
//    cout << "ddr=" << ddr << endl;

//    if(flag==1)
//    {
//        helpInt=floor((ui->finishTimeBox->value()-ui->startTimeBox->value())/double(ddr));
//        helpString.setNum(helpInt);

//        int s = 0;
//        s=fprintf(edfNew, "%d", helpInt);
//        for(int i=s; i < 8; ++i)
//        {
//            fprintf(edfNew, "%c", char(32));
//        }

//        s=fprintf(edfNew, "%d", ddr);
//        for(int i=s; i < 8; ++i)
//        {
//            fprintf(edfNew, "%c", char(32));
//        }

//    }

    //number of signals

//    QStringList list = this->ui->nsLine->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
//    helpString.setNum(list.length());
//    for(int i = helpString.length(); i < 4; ++i)
//    {
//        helpString.prepend(' ');
//    }

    for(int i = 0; i < 4; ++i)
    {
        fscanf(edf, "%c", &helpCharArr[i]);
        fprintf(edfNew, "%c", helpCharArr[i]);

//        fprintf(edfNew, "%c", helpString.toStdString().c_str()[i]);
    }

    ns=atoi(helpCharArr);                        //Number of channels

    //test
//    ns=4;

    cout << "ns = " << ns << endl;
//    cout << "nsNew = " << list.length() << endl;

    //labels
    char **label;
    label = new char* [ns];     //memory for channels' labels

    for(int i = 0; i < ns; ++i)
    {
        label[i] = new char [16];
    }
    for(int i = 0; i < ns*16; ++i)                      //label read
    {
        fscanf(edf, "%c", &label[i/16][i%16]);
        fprintf(edfNew, "%c", label[i/16][i%16]);
        if(i%16==15) label[i/16][i%16]='\0';

    }
    helpString=dir->absolutePath().append(QDir::separator()).append("labels.txt");
    FILE * labels=fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    for(int i = 0; i < ns; ++i)                         //label write in file
    {
        fprintf(labels, "%s \n", label[i]);
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []label[i];
    }
    delete []label;


    //transducer type
    for(int i = 0; i < ns*80; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }


    //physical dimension
    for(int i = 0; i < ns*8; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }

    //physical minimum
    double *physMin;
    physMin = new double [ns];

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(edfNew, "%c", helpCharArr[j]);
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
            fprintf(edfNew, "%c", helpCharArr[j]);
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
            fprintf(edfNew, "%c", helpCharArr[j]);
        }
        digMin[i]=double(atoi(helpCharArr));
    }

    //digital maximum
    double *digMax;
    digMax = new double [ns];     //memory for channels' labels

    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(edfNew, "%c", helpCharArr[j]);
        }
        digMax[i]=double(atoi(helpCharArr));
    }

    //prefiltering
    for(int i = 0; i < ns*80; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }



    //number of records (nr samples in ddr seconds)
    nr = new int [ns];
    for(int i = 0; i < ns; ++i)                      //rest of header
    {
        for(int j = 0; j < 8; ++j)
        {
            fscanf(edf, "%c", &helpCharArr[j]);
            fprintf(edfNew, "%c", helpCharArr[j]);
        }
        helpCharArr[8] = '\0';
        nr[i]=atoi(helpCharArr);
    }


    //reserved
    for(int i = 0; i < ns*32; ++i)                      //rest of header
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }


    for(int i = 0; i < (bytes-(ns+1)*256); ++i)                      //Neurotravel//generality//strange
    {
        fscanf(edf, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }

//    int maxNr = 0;
//    for(int i = 0; i < ns; ++i)
//    {
//        maxNr = max(maxNr, nr[i]);
//    }

    ui->finishTimeBox->setMaximum(ddr*ndr);

    helpString = dir->absolutePath().append(QDir::separator()).append(ExpName).append("_markers.txt");
//    FILE * markers = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");



    fpos_t *position = new fpos_t;
    fgetpos(edf, position);
    fclose(edf);
    edf = fopen(QDir::toNativeSeparators(ui->filePath->text()).toStdString().c_str(), "rb"); //generality
    fsetpos(edf, position);
    delete position;

//    cout << "start data read ndr=" << ndr << " ns=" << ns << endl;
//    if(ui->ntRadio->isChecked())
//    {
//        for(int i = 0; i < ndr; ++i)
//        {
//            for(int j = 0; j < ns; ++j)
//            {
//                for(int k = 0; k < nr[j]; ++k)
//                {
//                    if(j!=(ns-1))  ////////////generality////////////
//                    {
//                        fread(&a, sizeof(short), 1, edf);
//                        data[j][i*nr[j]+k] = physMin[j] + (physMax[j]-physMin[j]) * (double(a)-digMin[j]) / (digMax[j] - digMin[j]);   //neurotravel
//                    }
//                    else
//                    {
//                        //edf+
//                        fscanf(edf, "%c", &helpChar);
//                        helpString.append(QChar(helpChar));
//                        fscanf(edf, "%c", &helpChar);
//                        helpString.append(QChar(helpChar));
//                    }
//                }

//                //edf+
//                if(j==ns-1)  ////////generality?/////////
//                {
//                    currStart = 0;
//                    for(int l = 0; l < len(helpString); ++l)
//                    {
//                        if(int(helpString.toStdString()[l])== 0 || (int(helpString.toStdString()[l])==20 && (int(helpString.toStdString()[l+1])== 0 || int(helpString.toStdString()[l+1])==20)))
//                        {
//                            for(int p=currStart; p < l; ++p)
//                            {
//                                annotations[numOfAnn].append(helpString[p]);
//                            }
//                            ++numOfAnn;
//                            while((int(helpString.toStdString()[l])== 0 || int(helpString.toStdString()[l])==20) && l < len(helpString)) ++l;
//                            currStart=l;
//                        }
//                    }
//                }
//            }
//        }
//    }
    cout << "data write start, indepNum = " << indepNum << endl;

    if(ui->enRadio->isChecked())
    {
        for(int i = 0; i < ndr; ++i)
        {
            for(int j = 0; j < ns; ++j) //j -> list[j].toInt()-1
            {
                for(int k = 0; k < nr[j]; ++k)
                {
                    fread(&a, sizeof(short), 1, edf);
                    if(j < 19) //generality EEG Channels
                    {
                        a = (short)((inData[j][i*nr[j]+k] - physMin[j]) * (digMax[j] - digMin[j]) / (physMax[j]-physMin[j]) + digMin[j]);
                    }
                    if(indepNum > 19 && (j == 21 || j == 22))
                    {
                        a = (short)((inData[j-2][i*nr[j]+k] - physMin[j]) * (digMax[j] - digMin[j]) / (physMax[j]-physMin[j]) + digMin[j]); //-3 generality for EOG
                    }
                    fwrite(&a, sizeof(short), 1, edfNew);
//                    cout << "current nr = " << k << " current channel = " << j << " current ndr = " << i << endl;
                }

            }
        }
        cout << "staSlice=" << staSlice << " staTime=" << staSlice/250. << endl;
    }
//    fclose(markers);


    //    QString *annotations = new QString [10000];
    //    int numOfAnn = 0;
    //    int currStart;

//    if(ui->ntRadio->isChecked())
//    {
//        double markTime;
//        char * markNum = new char[60];
//        QString markValue;
//        for(int j = 0; j < numOfAnn; ++j)
//        {
//            markNum[0]='\0';
//            markValue="";
//            sscanf(annotations[j].toStdString().c_str(), "+%lf\24", &markTime);
//            //set time into helpString with 3 float numbers
//            helpString.setNum(markTime);
//            if(helpString[helpString.length()-3]=='.') helpString.append("0"); //float part - 2 or 3 signs
//            else
//            {
//                if(helpString[helpString.length()-2]=='.') helpString.append("00");
//                else helpString.append(".000");
//            }
//            for(int i = helpString.length()+2; i < annotations[j].length(); ++i) //+2 because of '+' and '\24'
//            {
//                markValue.append(annotations[j][i]);
//            }
//            sscanf(annotations[j].toStdString().c_str(), "+%lf\24%s", &markTime, markNum);
//            data[ns-1][int(markTime*nr[ns-1]/ddr)] = atoi(markNum);
//        }
//        delete []markNum;
//    }

    delete [] helpCharArr;
    fclose(labels);
    fclose(edfNew);
    rewind(edf);


    delete [] physMin;
    delete [] physMax;
    delete [] digMin;
    delete [] digMax;

    cout << "EDF constructed" <<endl;

}

void MainWindow::ICA()
{
    //we have data[ns][ndr*nr], ns, ndr, nr
    //at first - whiten signals using eigen linear superposition to get E as covMatrix
    //then count matrixW
    //count components matrixW*data and write to ExpName_ICA.edf
    //count inverse matrixW^-1 and draw maps of components
    //write automatization for classification different sets of components, find best set, explain


    duration=time(NULL);
    this->readData();
    ns = ui->numOfIcSpinBox->value();

    int fr = nr[0];


//    dataICA - matrix of data
//    centeredMatrix - matrix of centered data: data[i][j] -= average[j]
//    random quantity is a amplitude-vector of ns dimension
//    there are ndr*nr samples of this random quantity
//        covMatrix - matrix of covariations of different components of the random quantity. Its size is ns*ns
//        sampleCovMatrix - matrix of sample covariations - UNUSED
//        similarMatrix - matrix of similarity
//        differenceMatrix - matrix of difference, according to some metric

    cout << "ns = " << ns << endl;
    cout << "ndr*fr = " << ndr*fr << endl;

    double ** covMatrix = new double * [ns];
//    double * projectedVector = new double [NetLength];
//    double ** differenceMatrix = new double * [NumberOfVectors];
    double ** centeredMatrix = new double * [ns];


    for(int i = 0; i < ns; ++i)
    {
        covMatrix[i] = new double [ns];
    }

    for(int i = 0; i < ns; ++i)
//    {
//        differenceMatrix[i] = new double [NumberOfVectors];
        centeredMatrix[i] = new double [ndr*fr];
//    }


//    for(int i = 0; i < NumberOfVectors; ++i)
//    {
//        for(int j = 0; j < NumberOfVectors; ++j)
//        {
//            differenceMatrix[i][j] = 0.;
//        }
//    }


    //count covariations
    //count averages
    double * averages = new double [ns];
    for(int i = 0; i < ns; ++i)
    {
        averages[i] = 0.;
        for(int j = 0; j < ndr*fr; ++j)
        {
            averages[i] += data[i][j];
        }
        averages[i] /= ndr*fr;
//        cout << i << " average = " << averages[i] << endl; //test
    }


    //subtract averages
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            data[i][j] -= averages[i];
        }
    }


    //covariation between different spectra-bins
//    cout << "covMatrix = " << endl;
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            covMatrix[i][j] = 0.;
            for(int k = 0; k < ndr*fr; ++k)
            {
                covMatrix[i][j] += data[i][k] * data[j][k];
            }
            //should norm!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
            covMatrix[i][j] /= (ndr*fr - 1);
//            cout << covMatrix[i][j] << " ";
        }
//        cout << endl;
    }
    cout << "covMatrix counted" << endl;

    //test covMatrix symmetric
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            if(covMatrix[i][j]!=covMatrix[j][i]) cout << i << " " << j << " warning" << endl;
        }
    }


    //count eigenvalues & eigenvectors of covMatrix
    double * eigenValues = new double [ns];
    double ** eigenVectors = new double * [ns]; //vector is a coloumn
    for(int j = 0; j < ns; ++j)
    {
        eigenVectors[j] = new double [ns];
    }
    double * tempA = new double [ns]; //i
    double * tempB = new double [ndr*fr]; //j
    double sum1, sum2; //temporary help values
    double dF, F;
    int counter;
    double trace = 0.;
    for(int j = 0; j < ns; ++j)
    {
        trace += covMatrix[j][j];
    }
    cout << "trace covMatrix = " << trace << endl;


    cout << "start eigenValues processing" << endl;
    //count eigenValues & eigenVectors


    //array for components
    double ** dataICA = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        for(int j = 0; j < ndr*fr; ++j)
        {
            dataICA[i][j] = data[i][j];
        }
    }
    int numOfPc = 0;

    //data "is transposed"
    //counter j - for B, i - for A
    for(int k = 0; k < ns; ++k)
    {
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
        counter = 0.;
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
            if(counter==200)
            {
//                cout<<"dF = "<<abs(dF)<<endl;
                break;
            }
            if(fabs(dF) < 1e-3) break;
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
        cout << k << "  " << eigenValues[k] << endl;
        for(int i = 0; i < ns; ++i)
        {
            eigenVectors[i][k] = tempA[i]; //1-normalized
        }

        sum1 = 0.;

        for(int i = 0; i < k; ++i)
        {
            sum1 += eigenValues[i];
        }
        if(sum1 > 0.95*trace)
        {
            cout<<"numOf 95% PC = "<<k+1<<endl;
            numOfPc = k+1;
//            break;
        }



        //heat control
    }
    numOfPc = ns;



//    //test eigenVectors - OK
//    cout << "eigenVectors = " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            cout << eigenVectors[i][j] << "\t";
//        }
//        cout << endl;
//    }
//    cout << endl;



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

    //count linear decomposition

    for(int j = 0; j < ndr*fr; ++j) //columns X
    {
        for(int i = 0; i < ns; ++i) //rows tempMatrix
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += eigenVectors[k][i] * data[k][j] / sqrt(eigenValues[i]);
            }
            dataICA[i][j] = sum1;
        }
    }
    cout << "linear decomposition counted" << endl;

    //now dataICA are uncovariated signals with variance 1

//    //test of covMatrix dataICA
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
//            cout << covMatrix[i][j] << " ";
//        }
//        cout<<endl;
//    }
//    cout<<"covMatrixICA counted"<<endl;


    //PCA itself!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    double ** vectorW = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        vectorW[i] = new double [ns];
    }
    double * vector1;// = new double [ns];
    double * vector2;// = new double [ns];
    double * vector3;// = new double [ns];
    double * vectorOld = new double [ns];

    for(int i = 0; i < ns; ++i)
    {
        cout << "Num of vectorW = " << i << endl;
        for(int j = 0; j < ns; ++j)
        {
            vectorW[i][j] = randomVector(ns)[j];
        }
        while(1)
        {
            for(int j = 0; j < ns; ++j)
            {
                vectorOld[j] = vectorW[i][j];
            }
            vector1 = product1(dataICA, ndr*fr, ns, vectorW[i]);
            vector2 = product2(dataICA, ndr*fr, ns, vectorW[i]);
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] = 1./(ndr*fr) * vector1[j] - 1/(ndr*fr) * vector2[j];
            }

            //orthogonalization
            vector3 = product3(vectorW, ns, i);
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] -= vector3[j];
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
            if(sum2 < 3e-2) break;
        }
        //heat control

    }
    cout << "VectorsW counted" << endl;

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
//    cout << "W*W^t = " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            sum1 = 0.;
//            for(int k = 0; k < ns; ++k)
//            {
//                sum1 += vectorW[i][k] * vectorW[j][k];
//            }
//            cout << sum1 << " ";
//        }
//        cout << endl;
//    }


    double ** components = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        components[i] = new double [ndr*fr];
    }

    //count components
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += vectorW[i][k] * dataICA[k][j]; //initial
            }
            components[i][j] = sum1;
        }
    }



    //count full mixing matrix A = E * D^0.5 * Wt
    double ** matrixA = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        matrixA[i] = new double [ns];
    }

    double * tempVector = new double [ns];

    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] = sqrt(eigenValues[i]) * vectorW[k][i];
        }
    }

    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            tempVector[k] = 0.;
            for(int s = 0; s < ns; ++s)
            {
                tempVector[k] += eigenVectors[k][s] * matrixA[s][i];

            }
            for(int s = 0; s < ns; ++s)
            {
                matrixA[s][i] = tempVector[s];
            }

        }
    }



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


    //norm components - by equal dispersion
    double coeff = 10.;
    for(int i = 0; i < ns; ++i)
    {
        sum1 = 0.;
        sum2 = 0.;

        for(int j = 0; j < ndr*fr; ++j)
        {
            sum1 += components[i][j]; //average
        }
        sum1 /= ndr*fr;
        //        cout<<"average of component = "<<sum1<<endl; // e-15

        for(int j = 0; j < ndr*fr; ++j)
        {
            sum2 += (components[i][j] - sum1) * (components[i][j] - sum1); //variance
        }
        sum2 /= ndr*fr;

        for(int j = 0; j < ndr*fr; ++j)
        {
            components[i][j] -= sum1;
            components[i][j] /= sqrt(sum2);
            components[i][j] *= coeff;
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(sum2);
            matrixA[i][k] /= coeff;
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
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("maps.txt"));
    FILE * map = fopen(helpString.toStdString().c_str(), "w");
    double maxMagn = 0.;
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fprintf(map, "%.3lf\t", matrixA[i][j]);
            maxMagn = max(maxMagn, double(fabs(matrixA[i][j])));
        }
        fprintf(map, "\n");
    }
    fprintf(map, "max = %.3lf\n", maxMagn);
    fclose(map);

//    drawMap(matrixA, 0, maxMagn);







    FILE * edf0 = fopen(ui->filePath->text().toStdString().c_str(), "r");
    helpString = ExpName; helpString.append("_ica.edf");
    writeEdf(edf, components, helpString, ns);
    fclose(edf0);

cout << "ICA ended" << endl;



    for(int i = 0; i < ui->numOfIcSpinBox->value(); ++i)
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
    delete [] components;
    delete [] dataICA;

    duration = time(NULL) - duration;
    cout << "Duration of SliceAll = " << duration << " sec" << endl;
}

QColor mapColor(double maxMagn, double ** helpMatrix, int numX, int numY, double partX, double partY)
{
//    cout<<partX<<"\t"<<partY<<endl;
    double a[4];
    a[0] = helpMatrix[numY][numX];
    a[1] = helpMatrix[numY][numX+1];
    a[2] = helpMatrix[numY+1][numX];
    a[3] = helpMatrix[numY+1][numX+1];

    //mean harmonic
    double val = 0.;
    val += a[0] / (partX * partX + partY * partY) ;
    val += a[1] / ( (1. - partX) * (1. - partX) + partY * partY ) ;
    val += a[2] / ( partX * partX + (1. - partY) * (1. - partY) );
    val += a[3] / ( (1. - partX) * (1. - partX) + (1. - partY) * (1. - partY) );
    val /= 1. / (partX * partX + partY * partY) + 1. / ( (1. - partX) * (1. - partX) + partY * partY ) + 1. / ( partX * partX + (1. - partY) * (1. - partY) ) + 1. / ( (1. - partX) * (1. - partX) + (1. - partY) * (1. - partY) );


//    cout<<"val = "<<val<<"\tval/maxMagn = "<<val/maxMagn<<endl;

    return hue(256, (val/maxMagn)*256., 1., 1.);


}


void MainWindow::drawMap(double ** matrixA, int num)
{
    double maxMagn = 0.;
    int size = 240;
    QPixmap pic = QPixmap(size, size);
    QPainter * painter = new QPainter;
    pic.fill();
    painter->begin(&pic);

    double ** helpMatrix = new double * [5];
    for(int i = 0; i < 5 ; ++i)
    {
        helpMatrix[i] = new double [5];
    }

    int currIndex = 0.;
    for(int i = 0; i < 25; ++i)
    {
        if(i == 0 || i == 2 || i == 4 || i == 20 || i == 22 || i == 24)
        {
            helpMatrix[i/5][i%5] = 0.;
            continue;
        }

        helpMatrix[i/5][i%5] = fabs(matrixA[currIndex++][num]);
    }

    //approximation
    helpMatrix[0][0] = (helpMatrix[0][1] + helpMatrix[1][0] + helpMatrix[1][1])/3.;
    helpMatrix[0][2] = (helpMatrix[0][1] + helpMatrix[1][1] + helpMatrix[1][2] + helpMatrix[1][2] + helpMatrix[1][3] + helpMatrix[0][3])/6.;
    helpMatrix[0][4] = (helpMatrix[0][3] + helpMatrix[1][3] + helpMatrix[1][4])/3.;;
    helpMatrix[4][0] = (helpMatrix[3][0] + helpMatrix[3][1] + helpMatrix[4][1])/3.;
    helpMatrix[4][2] = (helpMatrix[4][1] + helpMatrix[3][1] + helpMatrix[3][2] + helpMatrix[3][2] + helpMatrix[3][3] + helpMatrix[4][3])/6.;
    helpMatrix[4][4] = (helpMatrix[4][3] + helpMatrix[3][3] + helpMatrix[3][4])/3.;

    double scale = size/6.;
    int numX, numY;
    double leftCoeff = 1.0;
    double rightCoeff = 5.0;

//    test - OK
//    if(num==0)
//    {
//        for(int i = 0; i < 5 ; ++i)
//        {
//            for(int j = 0; j < 5 ; ++j)
//            {
//                cout<<helpMatrix[i][j]<<"\t";
//            }
//            cout<<endl;
//        }

//    }

    //generality 5 -> ns=19
    for(int i = 0; i < 5 ; ++i)
    {
        for(int j = 0; j < 5 ; ++j)
        {
            maxMagn = max(helpMatrix[i][j], maxMagn);
        }
    }


    for(int x = floor(size/6.)*leftCoeff; x < floor(size/6.)*rightCoeff; ++x)
    {
        for(int y = floor(size/6.)*leftCoeff; y < floor(size/6.)*rightCoeff; ++y)
        {
//            cout<<x<<"\t"<<y;
            numX = floor(x/int(scale)) - 1; //1 2
            numY = floor(y/int(scale)) - 1; //3 4
//            cout<<"\t"<<numX<<"\t"<<numY;
            painter->setPen(mapColor(maxMagn, helpMatrix, numX, numY, double(double(x%int(scale))/scale + 0.003/scale), double(double(y%int(scale))/scale) + 0.003/scale));
            painter->drawPoint(x,y);
//            cout<<endl;

        }
    }

    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("map_").append(QString::number(num)).append(".jpg"));
    pic.save(helpString, 0, 100);



    for(int i = 0; i < 5 ; ++i)
    {
        delete []helpMatrix[i];
    }
    delete []helpMatrix;
    delete painter;
}

void MainWindow::drawMaps()
{
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("maps.txt"));
    FILE * map = fopen(helpString.toStdString().c_str(), "r");
//    double maxMagn = 0.;

    double ** matrixA = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        matrixA[i] = new double [ns];
    }


    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fscanf(map, "%lf\t", &matrixA[i][j]);
        }
    }
//    fscanf(map, "max = %lf\n", &maxMagn);
    fclose(map);

    for(int i = 0; i < ns; ++i)
    {
        drawMap(matrixA, i);
    }



    for(int i = 0; i < ns; ++i)
    {
        delete [] matrixA[i];
    }
    delete [] matrixA;
}


void MainWindow::eyesShow()
{
    Eyes *trololo = new Eyes(dir, ns);
    trololo->setAutoProcessingFlag(false);
    trololo->show();
    QObject::connect(trololo, SIGNAL(setNsMain(int)), this, SLOT(setNs2(int)));
//    trololo->eyesProcessing();
//    trololo->close();

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

        cout << "start" << endl;

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
                output[j][l/timeShift]=logistic2(output[j][l/timeShift], temperature); // unlinear conformation
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
    //        delete []data[k];
            delete []dataFFT[k];
        }
        delete []dataFFT;
        delete []data;


        for(int i = 0; i < NumOfClasses; ++i)
        {
            delete []output[i];
        }
        delete [] output ;


        delete []matrix;
}
