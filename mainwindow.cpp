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

    setlocale(LC_NUMERIC, "C");

    ns=-1;
    spLength=-1;

    ns = 19;
    right = defaults::right;
    left = defaults::left;
    spStep = 250./defaults::fftLength;
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

    //wavelets
    ui->classBox->setChecked(true);
    ui->weightsBox->setChecked(true);
    ui->waveletsBox->setChecked(true);
    ui->fullBox->setChecked(true);
    ui->r2RadioButton->setChecked(true);

    ui->tempBox->setMaximum(1e7);
    ui->tempBox->setValue(2e3);

    ui->enRadio->setChecked(true);

    ui->doubleSpinBox->setValue(1.0); //draw coeff
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
    ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(ui->reduceChannelsComboBox->currentIndex()).toString());


    ui->timeShiftBox->setMinimum(25);
    ui->timeShiftBox->setMaximum(1000);
    ui->timeShiftBox->setValue(250);
    ui->timeShiftBox->setSingleStep(25);
    ui->windowLengthBox->setMaximum(1000);
    ui->windowLengthBox->setMinimum(250);
    ui->windowLengthBox->setSingleStep(125);
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
//    ui->vectwDoubleSpinBox->setValue(1.5);
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

    ui->cleanFromRealsCheckBox->setChecked(false);
    ui->cleanHelpCheckBox->setChecked(false);
    ui->cleanRealisationsCheckBox->setChecked(false);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(false);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->highFreqFilterDoubleSpinBox->setValue(20.);
    ui->highFreqFilterDoubleSpinBox->setSingleStep(1.0);
    ui->lowFreqFilterDoubleSpinBox->setValue(5.0);
    ui->lowFreqFilterDoubleSpinBox->setSingleStep(0.1);




    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(setEdfFile()));

    QObject::connect(ui->countSpectra, SIGNAL(clicked()), this, SLOT(countSpectra()));

    QObject::connect(ui->Name, SIGNAL(returnPressed()), this, SLOT(setExpName()));

    QObject::connect(ui->cutEDF, SIGNAL(clicked()), this, SLOT(sliceAll()));

    QObject::connect(ui->windFromRealButton, SIGNAL(clicked()), this, SLOT(sliceWindFromReal()));

    QObject::connect(ui->cut_e, SIGNAL(clicked()), this, SLOT(cutShow()));

    QObject::connect(ui->makeCFG, SIGNAL(clicked()), this, SLOT(makeShow()));

    QObject::connect(ui->makePA, SIGNAL(clicked()), this, SLOT(makePaShow()));

    QObject::connect(ui->netButton, SIGNAL(clicked()), this, SLOT(netShow()));

    QObject::connect(ui->reduceChannesPushButton, SIGNAL(clicked()), this, SLOT(reduceChannels()));

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

    QObject::connect(ui->constructEdfButton, SIGNAL(clicked()), this, SLOT(constructEDF()));

    QObject::connect(ui->drawMapsPushButton, SIGNAL(clicked()), this, SLOT(drawMaps()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    QObject::connect(ui->BayesPushButton, SIGNAL(clicked()), this, SLOT(Bayes()));

    QObject::connect(ui->makeTestDataPushButton, SIGNAL(clicked()), this, SLOT(makeTestData()));

    QObject::connect(ui->icaTestClassPushButton, SIGNAL(clicked()), this, SLOT(icaClassTest()));

    QObject::connect(ui->spocPushButton, SIGNAL(clicked()), this, SLOT(spoc()));

    QObject::connect(ui->hilbertPushButton, SIGNAL(clicked()), this, SLOT(hilbertCount()));

    QObject::connect(ui->throwICPushButton, SIGNAL(clicked()), this, SLOT(throwIC()));

    QObject::connect(ui->randomDecomposePushButton, SIGNAL(clicked()), this, SLOT(randomDecomposition()));

    QObject::connect(ui->refilterDataPushButton, SIGNAL(clicked()), this, SLOT(refilterData()));

    QObject::connect(ui->transformRealsPushButton, SIGNAL(clicked()), this, SLOT(transformReals()));

    QObject::connect(ui->sliceWithMarkersCheckBox, SIGNAL(stateChanged(int)), this, SLOT(sliceWithMarkersSlot(int)));

    QObject::connect(ui->reduceChannelsEDFComboBox, SIGNAL(clicked()), this, SLOT(reduceChannelsEDFSlot()));
/*
    //function test
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
            array[i] += ampl*sin(2 * pi * i/250. * freq - phas);
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
        array[i] =  ampl*sin(2 * pi * i/250. * freq - phas);// * sin (2 * pi * i/250. * freq/4.);
        array[i] = 50. * (rand()%1000)/1000.;
        array[i] = i%150;
    }

    for(int i = 10; i < 100; i += 5)
    {
        cout << i << "   " << enthropy(array, leng, "", i) << endl;
    }
//    cout << "   " << enthropy(array, leng, "", 50) << endl;

    */



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

        outStream << lst[i].left(3).toStdString() << "\t" << doubleRound(men[i], 2) << "\t" << doubleRound(sigm[i], 2) << endl;

    }
    for(int i = 0; i < lst.length(); ++i)
    {
        cout << lst[i].left(3).toStdString() << "\t";

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


/*
    //many edf to my format
    dir->cd(defaults::dataFolder + "/GPP/edf/1");
    QString pth = "/media/michael/Files/Data/GPP/Realisations";
    lst = dir->entryList(QStringList("*.edf"), QDir::Files);
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
*/


//    double mean_, sigma_;
//    countRCP(defaults::dataFolder + "/AAX/rcp-40.txt", defaults::dataFolder + "/AAX/rcp-40.png", &mean_, &sigma_);
//    cout << mean_ << " " << sigma_ << " " << gaussApproval(defaults::dataFolder + "/AAX/rcp-40.txt") << endl;
//    countRCP(defaults::dataFolder + "/AAX/rcp-30.txt", defaults::dataFolder + "/AAX/rcp-30.png", &mean_, &sigma_);
//    cout << mean_ << " " << sigma_ << " " << gaussApproval(defaults::dataFolder + "/AAX/rcp-30.txt") << endl;
//    countRCP(defaults::dataFolder + "/AAX/rcp-20.txt", defaults::dataFolder + "/AAX/rcp-20.png", &mean_, &sigma_);
//    cout << mean_ << " " << sigma_ << " " << gaussApproval(defaults::dataFolder + "/AAX/rcp-20.txt") << endl;
//    autoIcaAnalysis();
//    system("sudo shutdown -P now");


    //conf youung scientists automatization
//    autoIcaAnalysis2();



/*
    //sequence ICs
    double ** mat1;
    double ** mat2;
    matrixCreate(&mat1, 19, 19);
    matrixCreate(&mat2, 19, 19);
    dir->cd("/media/Files/Data/AAX");

    //read matrices
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + "AAX_maps.txt";
    readICAMatrix(helpString, &mat1, 19);
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + "AAX_maps_05.txt";
    readICAMatrix(helpString, &mat2, 19);
    cout << matrixInnerMaxCorrelation(mat1, 19, 19) << endl;
    cout << matrixInnerMaxCorrelation(mat2, 19, 19) << endl;



    //invert ICA maps
    matrixTranspose(&mat1, 19);
    matrixTranspose(&mat2, 19);

    QList<int> listIC;
    listIC.clear();

    double tempDouble;
    int tempNumber;

    helpString.clear();
    for(int k = 0; k < 19; ++k)
    {
        tempDouble = 0.;
        for(int j = 0; j < 19; ++j)
        {
            if(listIC.contains(j)) continue;
            helpDouble = fabs(correlation(mat1[k], mat2[j], 19, 0));
            if(helpDouble > tempDouble)
            {
                tempDouble = helpDouble;
                tempNumber = j;
            }
        }
        listIC << tempNumber;
        helpString += QString::number(tempNumber+1) + " ";
        cout << k << "\t" << tempNumber << "\t" << tempDouble << endl;
    }
*/

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(true);
    ui->reduceChannelsCheckBox->setChecked(true);
    ui->reduceChannelsComboBox->setCurrentText("MyCurrent");

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);

//    autoIcaAnalysis2();
//    system("sudo shutdown -P now");

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

//enable Escape key for all widgets
void QWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape) close();
}

void MainWindow::stop()
{
    stopFlag = 1;
}

//void MainWindow::stopControl(int &a, int b)
//{
//    if(a%b == (b-1))
//    {
//        qApp->processEvents();
//        if(stopFlag == 1)
//        {
//            stopFlag = 0;
//            return;

//        }
//    }
//}

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
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append(ExpName).append("_weights_").append(QString::number(i)).append(".jpg"));
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

void MainWindow::changeNsLine(int a)
{
    ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(a).toString());
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
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
}

void MainWindow::drawEeg(int NumOfSlices_, double **dataD_, QString helpString_, int freq)
{
    int start = 0; // staSlice; //generality
    QPixmap pic = QPixmap(NumOfSlices_, 600);  //cut.cpp Cut::paint() generality
    pic.fill();
    QPainter * paint_ = new QPainter();
    paint_->begin(&pic);

    QString colour;

    double norm=ui->doubleSpinBox->value();              //////////////////////

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
    QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
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
        ANN->readCfg();
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
    ns=ui->setNsLine->text().toInt();
    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}

void MainWindow::setNsSlot(int a)
{
    ns=a;
    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
    ui->setNsLine->clear();
}


void MainWindow::countSpectra()
{
    Spectre *sp = new Spectre(dir, ns, ExpName);
    QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
    sp->show();
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
    if(spStep == 250./4096. || spStep == 250./2048.) helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
    else if(spStep == 250./1024.)     helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows"));
    else helpString = QDir::toNativeSeparators(dir->absolutePath());


    MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep, QVector<int>());
    mkPa->show();
}

void MainWindow::makeShow()//250 - frequency generality
{
    if(spStep == 250./4096.) helpString = "16sec19ch";
    else if(spStep == 250./1024.) helpString = "4sec19ch";
    else if(spStep == 250./2048.) helpString = "8sec19ch";
    else helpString = "netFile";
    cfg *config = new cfg(dir, ns, spLength, 0.10, 0.10, helpString);
    config->show();
}

void MainWindow::setEdfFile()
{
    NumOfEdf = 0;
    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)this, tr("EDF to open"), defaults::dataFolder, tr("EDF files (*.EDF *.edf)")));
    if(helpString=="")
    {
        QMessageBox::warning((QWidget*)this, tr("Warning"), tr("no EDF file was chosen"), QMessageBox::Ok);
        return;
    }


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
    ui->Name->setText(ExpName);

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
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

}

void MainWindow::setEdfFile(QString filePath)
{
    NumOfEdf = 0;
    helpString = filePath;
    if(!helpString.endsWith(".edf", Qt::CaseInsensitive))
    {
        helpString += ".edf";
    }

    if(helpString=="")
    {
        QMessageBox::warning((QWidget*)this, tr("Warning"), tr("no EDF file was chosen"), QMessageBox::Ok);
        return;
    }


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
    ui->Name->setText(ExpName);

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
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

}
void MainWindow::setExpName()
{
    ExpName=ui->Name->text();
    helpString="Name approved\n";
    ui->textEdit->append(helpString);
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
    ns=atoi(helpCharArr);                        //Number of channels
//    cout << "ns = " << ns << endl;

    //labels
    for(int i = 0; i < ns*16; ++i)
    {
        fscanf(edf, "%c", &label[i/16][i%16]);
        fprintf(header, "%c", label[i/16][i%16]);
        if(flag==1) fprintf(edfNew, "%c", label[i/16][i%16]);
        if(i%16==15) label[i/16][16]='\0';

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

    QString * annotations = new QString [1000];
    int numOfAnn = 0;
    int currStart;


    fpos_t *position = new fpos_t;
    fgetpos(edf, position);
    fclose(edf);
    edf = fopen(QDir::toNativeSeparators(ui->filePath->text()).toStdString().c_str(), "rb"); //generality
    fsetpos(edf, position);
    delete position;
    bool byteMarker[8];
    bool boolBuf;

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
                    else
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

                            if(data[j][i*nr[j]+k] < 32768. && data[j][i*nr[j]+k] != 0 )
                            {

                                for(int h = 0; h < 16; ++h)
                                {
                                    byteMarker[h] = (int(data[j][i*nr[j]+k])%(int(pow(2,h+1))))/(int(pow(2,h)));
                                }

//                                cout << i*nr[j]+k << "\t" << (i*nr[j]+k)/250. << endl << data[j][i*nr[j]+k] << "\t";
//                                for(int h = 15; h >= 0; --h)
//                                {
//                                    cout << byteMarker[h];
//                                    if(h%4==0) cout << " ";
//                                }
//                                cout<<endl;

                                if(byteMarker[15] || byteMarker[7])
                                {
                                    for(int h = 0; h < 8; ++h) //swap bytes if wrong order
                                    {
                                        boolBuf = byteMarker[h];
                                        byteMarker[h] = byteMarker[h+8];
                                        byteMarker[h+8] = boolBuf;
                                    }

                                    data[j][i*nr[j]+k] = 0.;
                                    for(int h = 0; h < 16; ++h)
                                    {
                                        data[j][i*nr[j]+k] += byteMarker[h]*pow(2,h);
                                    }
//                                    cout << data[j][i*nr[j]+k] << "\t";
//                                    for(int h = 15; h >= 0; --h)
//                                    {
//                                        cout << byteMarker[h];
//                                        if(h%4==0) cout << " ";
//                                    }
//                                    cout<<endl<<endl;
                                }
                            }
                        }
                    }

                    if(j==(ns-1))
                    {
                        if(data[j][i*nr[j]+k]!=0)
                        {
                            bytes=i*nr[j]+k;
                            fprintf(markers, "%d %d\n", bytes, int(data[j][i*nr[j]+k]));
                        }

                        if(data[j][i*nr[j]+k]==200)
                        {
                            staSlice=i*nr[j]+k;
                        }
                    }
                }

            }
        }
//        cout << "staSlice=" << staSlice << " staTime=" << staSlice/250. << endl;
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

}


void MainWindow::makeDatFile()
{
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

    for(int i = int(startTime * 250.); i < int(finishTime * 250.); ++i) //generality 250
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


    timeShift=ui->timeShiftBox->value();
    wndLength = ui->windowLengthBox->value();

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
            helpDouble = 2.*3.1415926*double(i)/250. * (10.1 + x) + y;
            testSignals[j][i] = sin(helpDouble);
        }
    }
//        helpDouble = 2.*3.1415926*double(i)/250. * 10.3;
//        testSignals[1][i] = sin(helpDouble);//+ 0.17); //10.5 Hz
//        helpDouble = 2.*3.1415926*double(i)/250. * 10.25;
//        testSignals[2][i] = sin(helpDouble);//- 0.17); //10.5 Hz
//        helpDouble = 2.*3.1415926*double(i)/250. * 10.0;
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
//            testSignals[j][i] *= sin(2*3.1415926*i/250. * helpDouble + x) + y;
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
//    writeEdf(edf, testSignals2, helpString, ndr*nr[0]);



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

void MainWindow::refilterData()
{
    QTime myTime;
    myTime.start();

    readData();
    int fr = nr[0];
    int fftLength = pow(2., ceil(log2(ndr*fr)));

    spStep = fr/double(fftLength);

    double lowFreq = ui->lowFreqFilterDoubleSpinBox->value();
    double highFreq = ui->highFreqFilterDoubleSpinBox->value();

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
    helpString = dir->absolutePath() + QDir::separator() + ExpName + "_f.edf";
    writeEdf(edf, data, helpString, ndr*fr);

    cout << "RefilterData: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::reduceChannelsEDFSlot()
{
    helpString = dir->absolutePath() + QDir::separator() + ExpName + "_rdcChan.edf";
    reduceChannelsEDF(helpString);
}



void MainWindow::reduceChannelsEDF(QString newFilePath)
{
    readData();
    reduceChannelsFast();
    writeEdf(edf, data, newFilePath, ndr*nr[0]);
}

void MainWindow::writeCorrelationMatrix(QString edfPath, QString outPath)
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
    matrixDelete(&corrs, ns, ns);


}


void MainWindow::ICsSequence(QString EDFica1, QString EDFica2, QString maps1, QString maps2)
{
    /*
    double * spectr;

    setEdfFile(EDFica1);
    readData();


    //sequence ICs
    double ** mat1;
    double ** mat2;
    matrixCreate(&mat1, 19, 19);
    matrixCreate(&mat2, 19, 19);

    //read matrices
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_1_maps.txt";
    readICAMatrix(helpString, &mat1, 19);
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_2_maps.txt";
    readICAMatrix(helpString, &mat2, 19);

    //invert ICA maps
    matrixTranspose(&mat1, 19);
    matrixTranspose(&mat2, 19);

//            matrixPrint(mat1, 19, 19);
//            matrixPrint(mat2, 19, 19);

    QList<int> listIC;
    listIC.clear();

    double tempDouble;
    int tempNumber;

    helpString.clear();
    for(int k = 0; k < 19; ++k)
    {
        tempDouble = 0.;
        for(int j = 0; j < 19; ++j)
        {
            if(listIC.contains(j)) continue;
            helpDouble = fabs(correlation(mat1[k], mat2[j], 19, 0));
            if(helpDouble > tempDouble)
            {
                tempDouble = helpDouble;
                tempNumber = j;
            }
        }
        listIC << tempNumber;
        helpString += QString::number(tempNumber+1) + " ";
        cout << k << "\t" << tempNumber << "\t" << tempDouble << endl;
        ICAcorrArr[k].num1 = k;
        ICAcorrArr[k].num2 = tempNumber;
        ICAcorrArr[k].coeff = tempDouble;
    }
    helpString += "20";

    //set appropriate channel sequence for 2nd ica file
    ui->reduceChannelsLineEdit->setText(helpString);

    //write new 2nd ica file with needed channels' sequence
    helpString = dir->absolutePath() + QDir::separator() + list0[i];
    helpString.replace("_1.edf", "_2_ica.edf");
    setEdfFile(helpString); //open ExpName_2_ica.edf
    cleanDirs();
    reduceChannelsEDFSlot(); //_rdcChan.edf

    matrixDelete(&mat1, 19, 19);
    matrixDelete(&mat2, 19, 19);

*/

}

void MainWindow::sliceAll() ////////////////////////aaaaaaaaaaaaaaaaaaaaaaaaaa//////////////////
{
    QTime myTime;
    myTime.start();
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
                    timeShift = ui->timeShiftBox->value();
                    wndLength = ui->windowLengthBox->value();
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
    if(ui->eyesCleanCheckBox->isChecked()) fprintf(res, "solve time %s \t %lf \n", marker.toStdString().c_str(), solveTime);
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
    file2->write(QString::number(number).toStdString().c_str());
    file2->write("\r\n");
    file2->write(contents);
    file2->close();
    delete file2;

//    kernelest(helpString);

    solveTime/=(250.*number);
//    cout << "average time before feedback " << marker.toStdString() << " =" << solveTime << endl;

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
        if((data[ns-1][i] > 200 && data[ns-1][i] < 241) || data[ns-1][i] == 255 || data[ns-1][i] == 250 || data[ns-1][i] == 251) //not interesting markers
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

            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);


            if(!( (marker == "000") ) && !defaults::wirteStartEndLong) // dont write big rests and beginning
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

    if(defaults::wirteStartEndLong)
    {
        //write last rest state
        marker = "end";
        k = ndr*nr[ns-1];
        ++number;
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
//        helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
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
    int start = 0;
    int end = -1;
    bool markers[16];
    QString fileMark;
    int number = 0;
    FILE * file;
    int piece = 4096; //16*250; //length of a piece

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
            if(!(markers[0] || markers[1] || markers[2])) continue; //if not an interesting marker;

            //output marker number
//            cout << i << "\t" << data[ns-1][i] << "\t";
//            for(int j = 15; j >= 0; --j)
//            {
//                cout << markers[j];
//                if(j%4==0) cout << " ";
//            }
//            cout<<endl;

            if(markers[2] == 1) //the end of a session
            {
                if(markers[1] == 0 && markers[0] == 1) //end of a counting session
                {
                    end = i;
                    fileMark = "241"; //count
                }
                if(markers[1] == 1 && markers[0] == 0) //end of a following session
                {
                    end = i;
                    fileMark = "247"; //follow
                }
                if(markers[1] == 1 && markers[0] == 1) //end of a composed session
                {
                    end = i;
                    fileMark = "244"; //composed
                }
            }
            else //if the start of a session
            {
                end = i;
                fileMark = "254"; //rest. start of the session is sliced too
            }
        }

        if(end > start)
        {
            for(int j = 0; j < int(ceil((end-start)/double(piece))); ++j) // num of pieces
            {
                ++number;
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(fileMark);
                file = fopen(helpString.toStdString().c_str(), "w");
//                cout << helpString.toStdString() << endl;
//                cout << end - start - j*piece << "\t" << piece << endl;
                NumOfSlices = min(end - start - j*piece, piece);
                fprintf(file, "NumOfSlices %d \n", NumOfSlices);
                {
                    for(int l = start+j*piece; l < min(start+(j+1)*piece, end); ++l)
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
        }

    }

    //slice end piece
    end = ndr*nr[ns-1];
    fileMark = "254";
    for(int j = 0; j < int(ceil((end-start)/double(piece))); ++j)
    {
        ++number;
        helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(fileMark);
        file = fopen(helpString.toStdString().c_str(), "w");
//        cout << helpString.toStdString() << endl;
//        cout << end - start - j*piece << "\t" << piece << endl;
        NumOfSlices = min(end - start - j*piece, piece);
        fprintf(file, "NumOfSlices %d \n", NumOfSlices);
        {
            for(int l = start+j*piece; l < min(start+(j+1)*piece, end); ++l)
            {
                for(int m = 0; m < numChanWrite; ++m)
                {
                    fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                }
            }
        }
        fclose(file);
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

void MainWindow::kernelest(const QString &inputString)
{
    /*
    FILE * file = fopen(QDir::toNativeSeparators(inputString).toStdString().c_str(), "r");
    int num;
    fscanf(file, "NumOfFiles %d\n", &num);
    int *arr = new int [num];
    double average = 0.;
    double sigma = 0.;
    for(int i = 0; i < num; ++i)
    {
        fscanf(file, "%d", &arr[i]);
    }

    sigma = variance(arr, num);

    sigma = sqrt(sigma);   //// (sigma)/num ??


    average = -0.2*log(num);
    double h = 1.06*sigma*exp(average);

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
        pnt->drawText(i*250+10, pic.height()*0.95, QString::number(i));
    }
    helpString = inputString;
    helpString.append(".jpg");
    pic.save(helpString, 0, 100);

    delete pnt;

    delete []arr;
    */

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
            if(QString(label[i]).contains("EOG 1", Qt::CaseInsensitive)) //generality eog channels names
            {
                a[0] = i;
            }
            else if(QString(label[i]).contains("EOG 2", Qt::CaseInsensitive)) //generality eog channels names
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
    spLength = c-b+1;
    left = b;
    right = c;
    spStep = d;
    helpString="SpVal taken";
    ui->textEdit->append(helpString);
}


void MainWindow::reduceChannels()
{
    helpString=ui->reduceChannelsLineEdit->text();

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
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);
}

void MainWindow::reduceChannelsFast()
{
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
    int sign;
    int lengthCounter;

    for(int k = 0; k < list.length(); ++k)
    {
        if(QString::number(list[k].toInt()) == list[k])
        {
            for(int j = 0; j < ndr*nr[list[k].toInt()-1]; ++j)
            {
                temp[k][j] = data[list[k].toInt()-1][j];
            }
        }
        else if(list[k].contains('-') || list[k].contains('+'))
        {
            lengthCounter = 0;
            lst = list[k].split(QRegExp("[-+]"), QString::SkipEmptyParts);
            for(int h = 0; h < lst.length(); ++ h)
            {
                if(QString::number(lst[h].toInt()) != lst[h]) // if nan
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
                temp[k][j] = data[lst[0].toInt() - 1][j];
            }
            lengthCounter += lst[0].length();
            for(int h = 1; h < lst.length(); ++h)
            {
                if(list[k][lengthCounter] == '+') sign = 1;
                else if(list[k][lengthCounter] == '-') sign = -1;
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
                lengthCounter += 1; //sign length
                cout << sign*(lst[h].toInt() - 1)  <<endl;
                for(int j = 0; j < ndr*nr[k]; ++j) //generality k
                {
                    temp[k][j] += sign*data[lst[h].toInt() - 1][j];
                }
                lengthCounter += lst[h].length();
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
    }


    for(int i = 0; i < ns; ++i)
    {
        delete []temp[i];
    }
    delete []temp;


    ns=list.length();
    cout << "channels reduced, ns=" << ns << endl;

    helpString="channels reduced fast ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
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

void MainWindow::constructEDF()
{
    QTime myTime;
    myTime.start();
    readData(); // needed?
//    cout << "constructEDF: data read" << endl;

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    ns = lst.length();
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
    {
        QMessageBox::critical(this, tr("Error"), tr("bad channels list - no markers"), QMessageBox::Ok);
        return;
    }

    if(!ui->sliceWithMarkersCheckBox->isChecked())
    {
        QMessageBox::critical(this, tr("Error"), tr("withMarkersCheckBox is not checked"), QMessageBox::Ok);
        return;
    }

    double ** newData = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        newData[i] = new double [ndr * nr[i]];  //generality, maybe bad nr from other channel?
    }
//    cout << "constructEDF: memory allocated" << endl;

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files, QDir::Name); //generality
    dir->cdUp();

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

    int nsB = ns;

    cout << "construct EDF: Initial NumOfSlices = " << ndr*ddr*nr[0] << endl;
    cout << "construct EDF: NumOfSlices to write = " << currSlice << endl;
    helpString = dir->absolutePath() + QDir::separator() + ExpName + "_new.edf";
    writeEdf(edf, newData, helpString, currSlice);

    for(int i = 0; i < nsB; ++i)
    {
        delete []newData[i];
    }
    delete []newData;
    cout << "construct EDF: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
}


void MainWindow::writeEdf(FILE * edfIn, double ** inData, QString outFilePath, int numSlices)
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
    QTime myTime;
    myTime.start();


    //all bounded to nsLine
    char *helpCharArr = new char[50];
    char helpChar;
    int bytes;
    short int a;
    unsigned short markA;

    FILE *edfNew;


    lst.clear();
    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    int newNs = lst.length();

//    cout << "writeEDF: newNs = " << newNs << endl;
//    for(int i = 0; i < newNs; ++i)
//    {
//        cout << lst[i].toStdString() << " ";
//    }
//    cout << endl;


    cout << "writeEDF: output path = " << outFilePath.toStdString() << endl;
    edfNew = fopen(outFilePath.toStdString().c_str(), "w");
    if(edfNew == NULL)
    {
        cout << "writeEDF: cannot open edf file to write" << endl;
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


    ndr = int(numSlices/250.)/ddr; //250 generality
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


//    //labels
//    char **label_ = new char* [ns];     //memory for channels' labels
//    for(int i = 0; i < ns; ++i)
//    {
//        label_[i] = new char [16];
//    }

    for(int i = 0; i < ns*16; ++i)                      //label read
    {
        fscanf(edfIn, "%*c");
//        fprintf(edfNew, "%c", label_[i/16][i%16]);
//        if(i%16==15) label_[i/16][i%16]='\0';
    }

    //better labels
    for(int i = 0; i < newNs; ++i)
    {
//        cout << label[lst[i].toInt() - 1] << endl;
        fprintf(edfNew, "%s", label[lst[i].toInt() - 1]);
    }
    helpString=dir->absolutePath().append(QDir::separator()).append("labels.txt");
    FILE * labels=fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    for(int i = 0; i < newNs; ++i)                         //label write in file
    {
        fprintf(labels, "%s\n", label[lst[i].toInt() - 1]);
    }
//    cout << "labels written" << endl;

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
        fprintf(edfNew, "%s", transducer[lst[i].toInt() - 1]);
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
        fprintf(edfNew, "%s", physDim[lst[i].toInt() - 1]);
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
        helpString = QString::number(physMin[lst[i].toInt() - 1]);
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
        helpString = QString::number(physMax[lst[i].toInt() - 1]);
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
        helpString = QString::number(digMin[lst[i].toInt() - 1]);
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
        helpString = QString::number(digMax[lst[i].toInt() - 1]);
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
//        cout << prefilter[lst[i].toInt() - 1] << endl;
        fprintf(edfNew, "%s", prefilter[lst[i].toInt() - 1]);
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
        helpString = QString::number(nr[lst[i].toInt() - 1]);
        for(int i = helpString.length(); i < 8; ++i)
        {
            helpString.append(' ');
        }

//        cout << nr[lst[i].toInt() - 1] << endl;
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
        fprintf(edfNew, "%s", reserved[lst[i].toInt() - 1]);
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []reserved[i];
    }
    delete []reserved;


//    cout << "writeEDF: bytes left = " << (bytes-(newNs+1)*256) << endl;
    for(int i = 0; i < (bytes-(newNs+1)*256); ++i)                      //Neurotravel//generality//strange
    {
        fscanf(edfIn, "%c", &helpChar);
        fprintf(edfNew, "%c", helpChar);
    }

    //header write ended
    fclose(edfNew);


//    return;



    edfNew = fopen(outFilePath.toStdString().c_str(), "ab");

    ui->finishTimeBox->setMaximum(ddr*ndr);

//    cout << "writeEDF: data write start, newNs = " << newNs << endl;
    int oldIndex;

    if(ui->enRadio->isChecked())
    {
        for(int i = 0; i < ndr; ++i)
        {
            for(int j = 0; j < newNs; ++j) // j - number of channel in "new" file
            {
                oldIndex = lst[j].toInt() - 1; //number of channel in "old" file edfIn
                for(int k = 0; k < nr[oldIndex]; ++k)
                {

//                    a = (short)((inData[ j ][ i * nr[oldIndex] + k ] - physMin[oldIndex]) * (digMax[oldIndex] - digMin[oldIndex]) / (physMax[oldIndex] - physMin[oldIndex]) + digMin[oldIndex]);

                    //generality
                    if(oldIndex != ns - 1)
                    {
                        a = (short)(inData[ j ][ i * nr[oldIndex] + k ] * 8.);
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
    fclose(labels);
    fclose(edfNew);
    rewind(edf);


    delete [] physMin;
    delete [] physMax;
    delete [] digMin;
    delete [] digMax;

    cout << "EDF constructed: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;

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
        QMessageBox::critical(this, tr("Error"), tr("bad channels list"), QMessageBox::Ok);
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

    //subtract averages
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            if(data[i][j] != 0.) data[i][j] -= averages[i] * (double(ndr*fr)/(ndr*fr - Eyes));
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
            //should norm!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
            covMatrix[i][j] /= (ndr*fr); //should be -1 ?
        }
    }
//    cout << "covMatrix counted" << endl;

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




    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_eigenMatrix.txt";
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



    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_eigenValues.txt";
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
            if(components[i][j] != 0.) components[i][j] -= sum1 * (double(ndr*fr)/(ndr*fr - Eyes));
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
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_maps.txt");
    writeICAMatrix(helpString, matrixA, ns); //generality 19-ns




    FILE * edf0 = fopen(ui->filePath->text().toStdString().c_str(), "r");
    //    ui->reduceChannelsComboBox->setCurrentText("MyCurrentNoEyes"); //generality
    helpString = dir->absolutePath() + QDir::separator() + ExpName + "_ica.edf";
    writeEdf(edf, components, helpString, ndr*nr[0]);
    fclose(edf0);

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

void MainWindow::icaClassTest() //non-optimized
{

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

    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_maps.txt");
    readICAMatrix(helpString, &matrixA, numOfIC);

    Spectre * spectr;// = Spectre(dir, numOfIC, ExpName);
    helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth";
    Net * ANN = new Net(dir, numOfIC, left, right, spStep, ExpName);
    helpString = dir->absolutePath() + QDir::separator() + "16sec19ch.net";
    ANN->readCfgByName(helpString);
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
    cout << "sliced" << endl;
    spectr = new Spectre(dir, numOfIC, ExpName);
    cout << "spectra widget created" << endl;
    spectr->countSpectra();
    cout << "spectra counted" << endl;
    ANN->autoClassificationSimple();
    tempAccuracy = ANN->getAverageAccuracy();
    cout << "initAccuracy = " << tempAccuracy << endl;

//    delete ANN;
//    delete spectr;

    tempIndex = -1;
    for(int j = numOfIC; j > 0; --j) //num of components left
    {
        if(thrownComp.length() == 3)
        {
            ANN->setReduceCoeff(5.);
        }
        if(thrownComp.length() == 5)
        {
            ANN->setReduceCoeff(4.);
        }
        if(thrownComp.length() == 8)
        {
            ANN->setReduceCoeff(3.);
        }
        tempIndex = -1;
        initAccuracy = tempAccuracy;
        for(int i = 0; i < numOfIC; ++i) //generality
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
            sliceOneByOneNew(ns-1);
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
    //makes a signals-file from opened ICA file and appropriate matrixA file
    if(!ui->filePath->text().contains("ica", Qt::CaseInsensitive))
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

    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_maps.txt");
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

    constructEDF();

}

void MainWindow::transformEDF(QString edfPath, QString mapsPath, QString newEdfPath)
{
    setEdfFile(edfPath);

    readData();
    double ** mat1;
    matrixCreate(&mat1, 19, 19);
    readICAMatrix(mapsPath, &mat1, 19);
    matrixInvert(&mat1, 19);

    double ** newData;
    matrixCreate(&newData, 20, ndr*nr[0]);
    matrixProduct(mat1, data, &newData, 19, ndr*nr[0]);
    //copy markers
    memcpy(newData[19], data[19], ndr*nr[0]*sizeof(double));

    writeEdf(edf, newData, newEdfPath, ndr*nr[0]);

    matrixDelete(&mat1, 19, 19);
    matrixDelete(&newData, 19, ndr*nr[0]);
}

void MainWindow::transformReals()
{
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

    matrixDelete(&mat1, 19,19);
    matrixDelete(&mat2, 19,250*50);
    matrixDelete(&mat3, 19,19);

}

void MainWindow::hilbertCount()
{
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
    writeEdf(edf, hilbertData, helpString, ndr*fr);

    matrixDelete(&hilbertData, ns, ndr*fr);
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
    readData();
    nsBackup = ns;
    ns = ui->numComponentsSpinBox->value(); //test

    double * W = new double  [ns];
    double * WOld = new double  [ns];
    int epochLength = ui->windowLengthBox->value();
    timeShift = ui->timeShiftBox->value();
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
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_maps.txt");
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

void MainWindow::randomDecomposition()
{
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

    Spectre * spectr = new Spectre(dir, compNum, ExpName.left(3));
    Net * ANN = new Net(dir, compNum, left, right, spStep, ExpName); //generality parameters
    helpString = dir->absolutePath() + QDir::separator() + "16sec19ch.net";
    ANN->readCfgByName(helpString);
    ANN->setReduceCoeff(15.); //generality
    ANN->setNumOfPairs(50);
    FILE * outFile;


    double ** eigenMatrix = matrixCreate(compNum, compNum);

    /*
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_eigenMatrix.txt";
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
    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName.left(3) + "_eigenValues.txt";
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
    fclose(edf);


    for(int i = 0; i < 100; ++i)
    {
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
        helpString = dir->absolutePath() + QDir::separator() + ExpName.left(3) + "_randIca.edf";
        writeEdf(edf, newData, helpString, ndr*nr[0]);
        fclose(edf); //opened in setEdfFile();

        helpString = dir->absolutePath() + QDir::separator() + ExpName.left(3) + "_randIca.edf";
        setEdfFile(helpString);
        readData(); //read newData

        ui->cleanRealisationsCheckBox->setChecked(true);
        ui->cleanRealsSpectraCheckBox->setChecked(true);
        cleanDirs();

        sliceOneByOneNew(19);
        spectr->defaultState();
        spectr->countSpectra();
        ANN->autoClassificationSimple();
        helpString = dir->absolutePath() + QDir::separator() + initName.left(3) + "_randIca.txt";
        outFile = fopen(helpString.toStdString().c_str(), "a");
        fprintf(outFile, "%.2lf\n", ANN->getAverageAccuracy());
        fclose(outFile);
        fclose(edf); //opened in setEdfFile();

    }




    matrixDelete(&randMatrix, compNum, compNum);
    matrixDelete(&matrixW, compNum, compNum);
    matrixDelete(&newData, ns, ndr*nr[0]);
    matrixDelete(&eigenMatrix, compNum, compNum);
    matrixDelete(&eigenMatrixInv, compNum, compNum);

    delete []eigenValues;
    delete ANN;
    delete spectr;


}

void MainWindow::autoIcaAnalysis()
{

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");

    dir->cd(defaults::dataFolder + "/AA");
    QStringList list0 = dir->entryList(QStringList("*.edf"), QDir::NoFilter, QDir::Name);

    Spectre * spectr;
    Net * ANN;
    MakePa * mkPa;

    FILE * outFile;

    for(int i = 0; i < list0.length(); ++i)
    {
        //for new icas
        if(list0[i].contains("randIca")) continue;
        if(!(list0[i].contains("GAN") || list0[i].contains("RMS") ||  list0[i].contains("VDA") ||  list0[i].contains("SEV") ||  list0[i].contains("PMI") || list0[i].contains("PAS"))) continue;



        helpString = dir->absolutePath() + QDir::separator() + list0[i];
        setEdfFile(helpString);
        cleanDirs();
        sliceAll();

        spectr = new Spectre(dir, ns, ExpName);
        spectr->countSpectra();
        spectr->close();
        spectr->compare();
        spectr->compare();
        spectr->compare();
        spectr->psaSlot();
        delete spectr;

        helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth";
        mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
        mkPa->setRdcCoeff(10);

        ANN = new Net(dir, ns, left, right, spStep, ExpName);


        //set appropriate coeff
        while(1)
        {
            mkPa->makePaSlot();
            ANN->PaIntoMatrixByName("1");
            ANN->LearnNet();
            if(ANN->getEpoch() > 150 || ANN->getEpoch() < 80)
            {
                mkPa->setRdcCoeff(mkPa->getRdcCoeff() / sqrt(ANN->getEpoch() / 120.));
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

        ANN->setNumOfPairs(50);
        ANN->autoClassificationSimple();

        outFile = fopen(QString(defaults::dataFolder + "/AA/res.txt").toStdString().c_str(), "a");
        fprintf(outFile, "%s\t%.2lf\r\n", ExpName.toStdString().c_str(), ANN->getAverageAccuracy());
        fclose(outFile);

        ANN->close();
        delete ANN;



        if(!ExpName.contains("ica", Qt::CaseInsensitive))
        {
            helpString = defaults::dataFolder + "/AA/" + ExpName.left(3) + "_randIca.txt";
            outFile = fopen(helpString.toStdString().c_str(), "w");
            fclose(outFile);

            randomDecomposition();
        }
    }
}


void MainWindow::autoIcaAnalysis2()
{
    //for young scientists conference
    QTime myTime;
    myTime.start();

    ui->svdDoubleSpinBox->setValue(7.5);
    ui->vectwDoubleSpinBox->setValue(7.5);

    ui->svdDoubleSpinBox->setValue(9.0);
    ui->vectwDoubleSpinBox->setValue(9.0);

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsCheckBox->setChecked(false);
    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    dir->cd(defaults::dataFolder + "/AB");
    QStringList list0 = dir->entryList(QStringList("*_1.edf"), QDir::NoFilter, QDir::Name);

    Spectre * spectr;
    Net * ANN;
    MakePa * mkPa;

    //make needed cfgs
    cfg * config;
    config = new cfg(dir, 19, 247, 0.1, 0.1, "16sec19ch");
    config->makeCfg();
    delete config;
    config = new cfg(dir, 19, 63, 0.1, 0.1, "4sec19ch");
    config->makeCfg();
    delete config;

    FILE * outFile;


    int NumOfRepeats = 50;

    QString ExpName1;

    struct ICAcorr
    {
        int num1;
        int num2;
        double coeff;
    };

    ICAcorr ICAcorrArr[19];
    double ICAcorrThreshold = 0.666;


    for(int i = 0; i < list0.length(); ++i) /////////////////////////////////////////////////////////////start with GAS+
    {
//        if(!list0[i].contains("SUA") && !list0[i].contains("BED") && !list0[i].contains("VDA")) continue;
        cout << endl << list0[i].toStdString()  << endl;

        ui->realButton->setChecked(true);
        if(1 && i != 2)
        {
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            setEdfFile(helpString); // open ExpName_1.edf
            ExpName1 = ExpName;
            ICA();
            helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_maps.txt";
            helpString2 = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_1_maps.txt";
            if(QFile::exists(helpString2)) QFile::remove(helpString2);
            QFile::copy(helpString, helpString2);


            /*
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            helpString.replace("_1.edf", "_2.edf");
            setEdfFile(helpString); // open ExpName_1.edf
            cleanDirs();
            ICA();
            helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_maps.txt";
            helpString2 = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_2_maps.txt";
            if(QFile::exists(helpString2)) QFile::remove(helpString2);
            QFile::copy(helpString, helpString2);

            */


            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            helpString.replace("_1.edf", "_sum.edf");
            setEdfFile(helpString); // open ExpName_sum.edf
            cleanDirs();
            ICA();
            helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_maps.txt";
            helpString2 = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_sum_maps.txt";
            if(QFile::exists(helpString2)) QFile::remove(helpString2);
            QFile::copy(helpString, helpString2);



            //transform 2nd file with 1st maps
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
            ExpName1 = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_1_maps.txt";
            helpString2 = dir->absolutePath() + QDir::separator() + list0[i];
            helpString2.replace("_1.edf", "_2_ica_by1.edf"); //write to ExpName_2_ica_by1.edf
            transformEDF(helpString, ExpName1, helpString2);


            //transform both files with general maps
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            ExpName1 = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_sum_maps.txt";
            helpString2 = dir->absolutePath() + QDir::separator() + list0[i];
            helpString2.replace("_1.edf", "_1_ica_sum.edf"); //write to ExpName_2_ica_by1.edf
            transformEDF(helpString, ExpName1, helpString2);

            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
            ExpName1 = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + list0[i].left(3) + "_sum_maps.txt";
            helpString2 = dir->absolutePath() + QDir::separator() + list0[i];
            helpString2.replace("_1.edf", "_2_ica_sum.edf"); //write to ExpName_2_ica_by1.edf
            transformEDF(helpString, ExpName1, helpString2);




/*

            //drop low correlated components - 1st file
            helpString.clear();
            cout << "high corr 1st file" << endl;
            for(int k = 0; k < 19; ++k)
            {
                if(ICAcorrArr[k].coeff < ICAcorrThreshold) continue;
                else helpString += QString::number(k+1) + " ";
                cout << k+1 << " ";
            }
            helpString += "20";
            cout << "20" << endl;
            ui->reduceChannelsLineEdit->setText(helpString);

            //write new 1st ica file with high correlated components
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            setEdfFile(helpString);
            cleanDirs();
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            helpString.replace("_1.edf", "_1_highCorr.edf");
            reduceChannelsEDF(helpString);

            //2nd file
            helpString.clear();
            cout << "high corr 2nd file" << endl;
            for(int k = 0; k < 19; ++k)
            {
                if(ICAcorrArr[k].coeff < ICAcorrThreshold) continue;
                else helpString += QString::number(ICAcorrArr[k].num2+1) + " ";
                cout << ICAcorrArr[k].num2+1 << " ";
            }
            helpString += "20";
            cout << "20" << endl;
            ui->reduceChannelsLineEdit->setText(helpString);

            //write new 1st ica file with high correlated components
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
            setEdfFile(helpString);
            cleanDirs();
            helpString = dir->absolutePath() + QDir::separator() + list0[i];
            helpString.replace("_1.edf", "_2_highCorr.edf");
            reduceChannelsEDF(helpString);

            helpInt = 0;
            for(int k = 0; k < 19; ++k)
            {
                if(!(ICAcorrArr[k].coeff < ICAcorrThreshold)) ++helpInt;
            }


            config = new cfg(dir, helpInt, 247, 0.1, 0.1, "highCorr");
            config->makeCfg();
            delete config;

            config = new cfg(dir, helpInt, 63, 0.1, 0.1, "highCorr_wnd");
            config->makeCfg();
            delete config;
            */


        }
        ui->timeShiftBox->setValue(125); //0.5 seconds shift


        for(int k = 0; k <= 1; ++k)
        {
            //k == 0 for realisations
            //k == 1 for windows
            if (k == 1) continue;

            if(k == 0)
            {
                ui->realButton->setChecked(true);
                ui->cleanRealisationsCheckBox->setChecked(true);
                ui->cleanRealsSpectraCheckBox->setChecked(true);
            }
            else if (k == 1)
            {
                ui->windButton->setChecked(true);
                ui->cleanWindowsCheckBox->setChecked(true);
                ui->cleanWindSpectraCheckBox->setChecked(true);
            }
            for(int j = 0; j <= 4; ++j)
            {
                if(j == 1 || j == 3) continue;
                //j == 0 for initial
                //j == 1 for ica
                //j == 2 for ica by maps_1 only
                //j == 3 for high correlations
                //j == 4 by whole ica
                for(int wndL = 1000; wndL >= 500; wndL -= 125) //too short limit in spectre.cpp::readFile();
                {
                    if(k == 1) cout << wndL << " windows start" << endl;
                    else cout << "reals start" << endl;

                    //clean wts
                    lst = dir->entryList(QStringList("*.wts"), QDir::Files|QDir::NoDotAndDotDot);
                    for(int h = 0; h < lst.length(); ++h)
                    {
                        remove(QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + lst[h]).toStdString().c_str());
                    }

                    //clean markers.txt
                    lst = dir->entryList(QStringList("*markers*"), QDir::Files|QDir::NoDotAndDotDot);
                    for(int h = 0; h < lst.length(); ++h)
                    {
                        remove(QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + lst[h]).toStdString().c_str());
                    }

                    //process
                    ui->windowLengthBox->setValue(wndL);

                    helpString = dir->absolutePath() + QDir::separator() + list0[i];
                    if(j == 1) helpString.replace("_1.edf", "_1_ica.edf");
                    else if(j == 2) helpString.replace("_1.edf", "_1_ica.edf");
                    else if(j == 3) helpString.replace("_1.edf", "_1_highCorr.edf");
                    else if(j == 4) helpString.replace("_1.edf", "_1_ica_sum.edf");
                    setEdfFile(helpString); // open ExpName_1.edf
                    ExpName1 = ExpName;
                    cleanDirs();
                    sliceAll();
                    fclose(edf);

                    spectr = new Spectre(dir, ns, ExpName);
                    QObject::connect(spectr, SIGNAL(spValues(int,int,double)), this, SLOT(takeSpValues(int,int,double)));
                    if(k == 0) spectr->setFftLength(4096);
                    else if(k == 1) spectr->setFftLength(1024);
                    spectr->countSpectra();
                    spectr->defaultState();
                    if(k == 0) spectr->setFftLength(4096);
                    else if(k == 1) spectr->setFftLength(1024);

                    ANN = new Net(dir, ns, left, right, spStep, ExpName);
                    ANN->setAutoProcessingFlag(true);
                    if(j == 3)
                    {
                        if(k == 0) ANN->readCfgByName("highCorr");
                        else if(k == 1) ANN->readCfgByName("highCorr_wnd");
                    }
                    else
                    {
                        if(k == 0) ANN->readCfgByName("16sec19ch");
                        else if(k == 1) ANN->readCfgByName("4sec19ch");
                    }

                    helpString = dir->absolutePath() + QDir::separator() + "SpectraSmooth";
                    if(k == 1) helpString += QString(QDir::separator()) + "windows";

                    mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
                    mkPa->setRdcCoeff(10); //is it right value?
                    while(1)
                    {
                        mkPa->makePaSlot();
                        if(k == 0) ANN->PaIntoMatrixByName("all");
                        else if(k == 1) ANN->PaIntoMatrixByName("all_wnd");
                        ANN->LearnNet();
                        if(ANN->getEpoch() > 160 || ANN->getEpoch() < 100)
                        {
                            mkPa->setRdcCoeff(mkPa->getRdcCoeff() / sqrt(ANN->getEpoch() / 130.));
                        }
                        else
                        {
                            reduceCoefficient = mkPa->getRdcCoeff();
                            cout << "file = " << ExpName.toStdString() << "\t" << "reduceCoeff = " << reduceCoefficient << endl;
                            ANN->setReduceCoeff(reduceCoefficient);
                            break;
                        }
                    }

                    for(int l = 0; l < NumOfRepeats; ++l)
                    {
                        mkPa->makePaSlot();

                        if(k == 0) ANN->PaIntoMatrixByName("all");
                        else if(k == 1) ANN->PaIntoMatrixByName("all_wnd");

                        ANN->LearnNet();
                        ANN->saveWts();
                    }

                    //open 2nd file
                    helpString = dir->absolutePath() + QDir::separator() + list0[i];
                    if(j == 0) helpString.replace("_1.edf", "_2.edf");
                    else if(j == 1) helpString.replace("_1.edf", "_2_ica_rdcChan.edf");
                    else if(j == 2) helpString.replace("_1.edf", "_2_ica_by1.edf");
                    else if(j == 3) helpString.replace("_1.edf", "_2_highCorr.edf");
                    else if(j == 4) helpString.replace("_1.edf", "_2_ica_sum.edf");
                    setEdfFile(helpString); // open ExpName_2.edf
                    cleanDirs();
                    sliceAll();
                    fclose(edf);

                    spectr->countSpectra();
                    spectr->close();
                    delete spectr;


                    mkPa->makePaSlot();
                    mkPa->close();
                    delete mkPa;

                    if(k == 0) ANN->PaIntoMatrixByName("whole");
                    else if(k == 1) ANN->PaIntoMatrixByName("whole_wnd");

                    for(int l = 0; l < NumOfRepeats; ++l)
                    {
                        helpString = dir->absolutePath() + QDir::separator() + ExpName1 + "_weights_" + QString::number(l) + ".wts";
//                        cout << "wts-file to load: " << helpString.toStdString() << endl;
                        ANN->loadWtsByName(helpString);
                        ANN->tall();
                    }
                    ANN->averageClassification();
                    outFile = fopen(QString(defaults::dataFolder + "/AB/res.txt").toStdString().c_str(), "a");

                    //print to res.txt
                    fprintf(outFile, "%s", ExpName.left(3).toStdString().c_str());
                    if(k == 1)
                    {
                        fprintf(outFile, "_winds");
                    }
                    else if(k == 0)
                    {
                        fprintf(outFile, "_reals");
                    }
                    if(j == 0) fprintf(outFile, "");
                    else if(j == 1) fprintf(outFile, "_ica");
                    else if(j == 2) fprintf(outFile, "_by1");
                    else if(j == 3) fprintf(outFile, "_highCorr");
                    else if(j == 4) fprintf(outFile, "_ica_sum");

                    if(k == 1) fprintf(outFile, "\t%d", wndL);
                    fprintf(outFile, "\t%.2lf\r\n", ANN->getAverageAccuracy());

                    fclose(outFile);
                    ANN->close();
                    delete ANN;

                    if (k == 0) break; //stop wndL cycle if realistions
                }
            }
        }

    }
    lst = dir->entryList(QStringList("*.wts"), QDir::Files|QDir::NoDotAndDotDot);
    for(int h = 0; h < lst.length(); ++h)
    {
        remove(QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + lst[h]).toStdString().c_str());
    }

    lst = dir->entryList(QStringList("*markers*"), QDir::Files|QDir::NoDotAndDotDot);
    for(int h = 0; h < lst.length(); ++h)
    {
        remove(QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + lst[h]).toStdString().c_str());
    }
    helpString = dir->absolutePath() + QDir::separator() + "results.txt";
    outStream.open(helpString.toStdString().c_str());
    outStream << "auto Ica analysis time elapsed " << myTime.elapsed()/1000. << " sec" << endl;
    outStream.close();
}
