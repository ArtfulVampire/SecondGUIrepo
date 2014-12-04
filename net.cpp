#include "net.h"
#include "ui_net.h"
//#include <CL/cl.h>




Net::Net(QDir  * dir_, int ns_, int left_, int right_, double spStep_, QString ExpName_) :
    ui(new Ui::Net)
{
    paFileBC = "";
    ui->setupUi(this);
    dir = new QDir;
    dir->cd(dir_->absolutePath());
    dirBC = new QDir;
    dirBC->cd(dir_->absolutePath());

    this->setWindowTitle("Net");
    ExpName = ExpName_;

    weight = 0;
    dimensionality = 0;


    left = left_;
    right = right_;
    spLength = right_ - left_ + 1;

    spStep = spStep_;
    ns = ns_;

    //clean log file
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "log.txt");
    log = fopen(helpString.toStdString().c_str(),"w");
    if(log == NULL)
    {
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("Cannot open log file to write"), QMessageBox::Ok);
        return;
    }
    fclose(log);


/*
    cout << "left = " << left << endl;
    cout << "right = " << right << endl;
    cout << "spStep = " << spStep << endl;
    cout << "ns = " << ns << endl;
    cout << "spLength = " << spLength << endl;
*/

    stopFlag = 0;
    numTest = 0;
    numOfTall = 0;

    NumberOfVectors = -1;
    loadPAflag = 0;

    helpCharArr = new char [200];

    classCount = new double [3]; //generality in cfg


    matrixCreate(&tempRandomMatrix, 19, 19);

    group1 = new QButtonGroup();
    group1->addButton(ui->leaveOneOutRadioButton);
    group1->addButton(ui->crossRadioButton);
    group2 = new QButtonGroup();
    group2->addButton(ui->realsRadioButton);
    group2->addButton(ui->windowsRadioButton);
    group2->addButton(ui->pcaRadioButton);
    group2->addButton(ui->bayesRadioButton);
    group3 = new QButtonGroup();
    group3->addButton(ui->deltaRadioButton);
    group3->addButton(ui->backpropRadioButton);
    ui->crossRadioButton->setChecked(true);
    ui->realsRadioButton->setChecked(true);
    ui->deltaRadioButton->setChecked(false);
    ui->backpropRadioButton->setChecked(false);

    if(spStep == 250./1024) ui->windowsRadioButton->setChecked(true);
    else if(spStep == 250./4096) ui->realsRadioButton->setChecked(true);

    ui->tempBox->setValue(10);
    ui->tempBox->setSingleStep(1);
    ui->critErrorDoubleSpinBox->setValue(0.10);
    ui->critErrorDoubleSpinBox->setSingleStep(0.01);
    ui->critErrorDoubleSpinBox->setDecimals(4);
    ui->learnRateBox->setValue(0.1);
    ui->learnRateBox->setSingleStep(0.01);
    ui->learnRateBox->setDecimals(3);
    ui->epochSpinBox->setMaximum(1000);
    ui->epochSpinBox->setSingleStep(50);
    ui->epochSpinBox->setValue(300);
    ui->numOfPairsBox->setMaximum(100);

    ui->numOfPairsBox->setValue(50); /////////////////////////////

    ui->rdcCoeffSpinBox->setMaximum(100);
    ui->rdcCoeffSpinBox->setDecimals(3);
    ui->rdcCoeffSpinBox->setMinimum(0.001);
    ui->rdcCoeffSpinBox->setValue(5.);

    ui->highLimitSpinBox->setMaximum(500);
    ui->highLimitSpinBox->setMinimum(100);
    ui->lowLimitSpinBox->setMaximum(500);
    ui->lowLimitSpinBox->setMinimum(50);

    ui->foldSpinBox->setMaximum(10);
    ui->foldSpinBox->setMinimum(1);
    ui->foldSpinBox->setValue(2);


    ui->pcaNumberSpinBox->setMinimum(2);
    ui->pcaNumberSpinBox->setMaximum(500);
    ui->pcaNumberSpinBox->setValue(60);
    ui->traceDoubleSpinBox->setMaximum(1.0);
    ui->traceDoubleSpinBox->setMinimum(0.05);
    ui->traceDoubleSpinBox->setSingleStep(0.01);
    ui->traceDoubleSpinBox->setValue(1.0);
    ui->sammonComboBox->addItem("wetData");
    ui->sammonComboBox->addItem("some PCs");
    ui->sammonComboBox->setCurrentIndex(1);

    ui->pcaCheckBox->setChecked(false);
    ui->kohonenCheckBox->setChecked(false);

    ui->lambdaDoubleSpinBox->setSingleStep(1.);
    ui->lambdaDoubleSpinBox->setMinimum(1e-5);
    ui->lambdaDoubleSpinBox->setMaximum(100.);
    ui->lambdaDoubleSpinBox->setValue(10.);

    ui->optimizationMethodComboBox->addItem("coords");
    ui->optimizationMethodComboBox->addItem("gradient");

    ui->autpPCAMaxSpinBox->setMinimum(1);
    ui->autoPCAMinSpinBox->setMinimum(1);
    ui->autoPCAStepSpinBox->setMinimum(0);
    ui->autoPCAStepSpinBox->setMaximum(5);

    ui->momentumDoubleSpinBox->setMaximum(1.0);
    ui->momentumDoubleSpinBox->setMinimum(0.0);
    ui->momentumDoubleSpinBox->setDecimals(2);
    ui->momentumDoubleSpinBox->setSingleStep(0.05);
    ui->momentumDoubleSpinBox->setValue(0.5);

    ui->numOfLayersSpinBox->setMinimum(2);
    ui->numOfLayersSpinBox->setValue(3);
    ui->numOfLayersSpinBox->setMaximum(10);
    ui->dimensionalityLineEdit->setText("0 20 0");

    ui->sizeSpinBox->setValue(6);
    paint = new QPainter;
    tempEvent = new QTempEvent;
    QObject::connect(this, SIGNAL(destroyed()), &myThread, SLOT(quit()));

    QObject::connect(ui->loadNetButton, SIGNAL(clicked()), this, SLOT(loadCfg()));

    QObject::connect(ui->loadWtsButton, SIGNAL(clicked()), this, SLOT(loadWts()));

    QObject::connect(ui->loadPaButton, SIGNAL(clicked()), this, SLOT(PaIntoMatrix()));

    QObject::connect(ui->learnButton, SIGNAL(clicked()), this, SLOT(LearnNet()));

    QObject::connect(ui->testAllButton, SIGNAL(clicked()), this, SLOT(tall()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopActivity()));

    QObject::connect(ui->saveWtsButton, SIGNAL(clicked()), this, SLOT(saveWtsSlot()));

    QObject::connect(ui->drawWtsButton, SIGNAL(clicked()), this, SLOT(drawWts()));

    QObject::connect(ui->neuronGasPushButton, SIGNAL(clicked()), this, SLOT(neuronGas()));

    QObject::connect(ui->pcaPushButton, SIGNAL(clicked()), this, SLOT(pca()));

    QObject::connect(ui->drawSammonPushButton, SIGNAL(clicked()), this, SLOT(drawSammon()));

    QObject::connect(ui->autoClassButton, SIGNAL(clicked()), this, SLOT(autoClassificationSimple()));

    QObject::connect(ui->svmPushButton, SIGNAL(clicked()), this, SLOT(SVM()));

//    QObject::connect(ui->hopfieldPushButton, SIGNAL(clicked()), this, SLOT(Hopfield()));

    QObject::connect(group3, SIGNAL(buttonToggled(int,bool)), this, SLOT(methodSetParam(int,bool)));

    QObject::connect(ui->dimensionalityLineEdit, SIGNAL(returnPressed()), this, SLOT(memoryAndParamsAllocation()));

    QObject::connect(ui->distancesPushButton, SIGNAL(clicked()), this, SLOT(testDistances()));

    QObject::connect(ui->optimizeChannelsPushButton, SIGNAL(clicked()), this, SLOT(optimizeChannelsSet()));

    QObject::connect(group2, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(adjustParamsGroup2(QAbstractButton*)));
    this->setAttribute(Qt::WA_DeleteOnClose);


    helpString = dir->absolutePath() + QDir::separator() + defaults::cfgFileName;
    loadCfgByName(helpString);

    if(spStep == 250./4096)
    {
        helpString = dir->absolutePath() + QDir::separator() + "16sec19ch.net";
    }
    else if(spStep == 250./1024)
    {
        helpString = dir->absolutePath() + QDir::separator() + "4sec19ch.net";
    }
    loadCfgByName(helpString);

    this->ui->deltaRadioButton->setChecked(true);

    NumberOfVectors = 200;
    matrix = new double * [NumberOfVectors];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        matrix[i] = new double [1];
    }
    FileName = new char * [NumberOfVectors];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        FileName[i] = new char [1];
    }
}

Net::~Net()
{

    delete dir;
    delete dirBC;
    delete []helpCharArr;
    delete []classCount;
    matrixDelete(&tempRandomMatrix, 19);
    delete group1;
    delete group2;
    delete group3;
    delete paint;
    delete tempEvent;
    delete ui;

    for(int i = 0; i < numOfLayers - 1; ++i)
    {
        for(int j = 0; j < dimensionality[i] + 1; ++j) //
        {
            delete []weight[i][j];
        }
        delete []weight[i];
    }
    delete []weight;
    delete []dimensionality;




    for(int i = 0; i < NumberOfVectors; ++i)
    {
        delete []matrix[i];
        delete []FileName[i];
    }
    delete []matrix;
    delete []FileName;

    myThread.quit();
    myThread.wait();

}

bool Net::event(QEvent  * ev)
{
    if(ev->type() != 1200)
    {
        return QWidget::event(ev);
    }
    else
    {
        QTempEvent * tempEvent = static_cast<QTempEvent * >(ev);
        if(tempEvent->value() >= 90.)
        {
            cout << "CRIT temp = " << tempEvent->value() << endl;
            sleep(2);///////////////////////////////////////////////////////////////wrong!!!!!!!1111111111
//            QApplication::sendEvent(this, tempEvent);
            return true;
        }
        else
        {
            cout << "curr temp = " << tempEvent->value() << endl;
            return false;
        }
    }
}

void Net::mousePressEvent(QMouseEvent  * event)
{
//    cout << "global(X,Y) = (" << event->globalX() << "," << event->globalY() << ")" << endl;
//    cout << "(X,Y) = (" << event->x() << "," << event->y() << ")" << endl;
//    cout << "pos(x,y) = (" << ui->clearSetsLabel->pos().x() << "," << ui->clearSetsLabel->pos().y() << ")" << endl;
//    cout << "size(w,h) = (" << ui->clearSetsLabel->width() << "," << ui->clearSetsLabel->height() << ")" << endl;

    if(event->x() > ui->clearSetsLabel->pos().x() && event->x() < ui->clearSetsLabel->pos().x() + ui->clearSetsLabel->width() && event->y() > ui->clearSetsLabel->pos().y() && event->y() < ui->clearSetsLabel->pos().y() + ui->clearSetsLabel->height() )
    {
//        cout << "aimed!" << endl;
        this->mouseShit = double(event->x() - ui->clearSetsLabel->pos().x())/double(ui->clearSetsLabel->width());
    }
    else
    {
//        cout << "fail" << endl;
    }
}

double Net::setPercentageForClean()
{
    //what a bicycle is it???!!!! signal/slot & eventFilter needed

    cout << "set for clean" << endl;
    mouseShit = 0.;
    int i = 0;
    while(mouseShit == 0.)
    {
        ++i;
        if(i%100 == 0) qApp->processEvents();

        //donothing
    }

    return mouseShit;
}

void Net::autoClassificationSimple()
{

//    ui->deltaRadioButton->setChecked(true); //generality
    helpString.clear();
    if(ui->realsRadioButton->isChecked())
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth");
    }
    else if(ui->windowsRadioButton->isChecked()) //generality
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth" + QDir::separator() + "windows");
    }
    else if(ui->bayesRadioButton->isChecked())
    {
        //generality
        spLength = NetLength/19;
        left = 1;
        right = spLength;
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth" + QDir::separator() + "Bayes");
    }
    else if(ui->pcaRadioButton->isChecked())
    {
        autoPCAClassification();
        return;
        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth" + QDir::separator() + "PCA");

    }
    if(!helpString.isEmpty()) autoClassification(helpString);
}

void Net::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

bool Net::adjustReduceCoeff(QString spectraDir, int lowLimit, int highLimit, MakePa * outMkPa, QString paFileName)
{
    QTime myTime;
    myTime.start();
    bool tmpAutoFlag = autoFlag;
    autoFlag = 1;
    double res;
    MakePa  * mkPa = new MakePa(spectraDir, ExpName, ns, left, right, spStep, channelsSetExclude);
    mkPa->setNumOfClasses(NumOfClasses);
    mkPa->setRdcCoeff(this->ui->rdcCoeffSpinBox->value());
    mkPa->setFold(ui->foldSpinBox->value());
    while(1)
    {
        mkPa->makePaSlot();
        this->PaIntoMatrixByName(paFileName);
        LearnNet();
        if(this->getEpoch() > highLimit || this->getEpoch() < lowLimit)
        {
            mkPa->setRdcCoeff(mkPa->getRdcCoeff() / sqrt(2. * this->getEpoch() /  double(lowLimit + highLimit) ));
            if(mkPa->getRdcCoeff() == 1e-3) //possible minimum
            {
                cout << "cant adjust rdc coefficient" << endl;
                res = 1e-3;
                break;
            }
        }
        else
        {
            res = mkPa->getRdcCoeff();
            break;
        }
    }
    mkPa->close();
    delete mkPa;
    outMkPa->setRdcCoeff(res);
    outMkPa->setFold(ui->foldSpinBox->value()); //optional
    autoFlag = tmpAutoFlag;
    cout << "adjustReduceCoeff: reduceCoeff = " << res << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
    if(res == 1e-3)
    {
        return false;
    }
    else
    {
        return true;
    }

}

void Net::autoClassification(QString spectraDir)
{
    QTime myTime;
    myTime.start();
    numOfTall = 0;

    if(loadPAflag  != 1)
    {
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("No CFG-file loaded yet"), QMessageBox::Ok);
        return;
    }
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "log.txt");
    log = fopen(helpString.toStdString().c_str(),"w");
    if(log == NULL)
    {
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("Cannot open log file to write"), QMessageBox::Ok);
        return;
    }
    fclose(log);

    //set random matrix - add in PaIntoMatrixByName
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            tempRandomMatrix[i][j] = (i == j); //identity matrix
//            tempRandomMatrix[i][j] = (i == j) * (5 + rand()%36) / 10.; //random diagonal
//            tempRandomMatrix[i][j] = (5. + rand()%36) / 50.; //full random
//            tempRandomMatrix[i][j] = (i == j) * (1. + (rand()%8 == 0) * (5. + rand()%35) / 5.); //random diagonal
        }
    }

    bool tempBool = autoFlag;
    autoFlag = 1;
    int numOfPairs = ui->numOfPairsBox->value();

    //adjust reduce coefficient
    MakePa  * mkPa = new MakePa(spectraDir, ExpName, ns, left, right, spStep, channelsSetExclude);
    if(!adjustReduceCoeff(spectraDir, ui->lowLimitSpinBox->value(), ui->highLimitSpinBox->value(), mkPa))
    {
        averageAccuracy = 0.;
        delete mkPa;
        autoFlag = tempBool;
        cout <<  "AutoClass: unsuccessful, time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
    }
    mkPa->setFold(ui->foldSpinBox->value()); //double set

    if(ui->crossRadioButton->isChecked())
    {
        cout << "Net: autoclass (max " << numOfPairs << ") ";
        for(int i = 0; i < numOfPairs; ++i)
        {
            cout << i+1;
            cout << " "; cout.flush();
//            cout << " iteration in process" << endl;
            //make PA
            mkPa->makePaSlot();


            PaIntoMatrixByName("1");

            LearnNet();
            PaIntoMatrixByName("2");

            tall();
            //comment because of k-fold cross-validation
            //            LearnNet();
            //            helpString = "1";
            //            PaIntoMatrixByName(helpString);
            //            tall();

            qApp->processEvents();
            if(stopFlag)
            {
                delete mkPa;
                stopFlag = 0;
                autoFlag = tempBool;
                return;
            }
        }
        cout << endl;
    }
    else if(ui->leaveOneOutRadioButton->isChecked())
    {
        cout << "N-fold cross-validation done" << endl;
        PaIntoMatrixByName("all");
        leaveOneOutSlot();
        numOfTall = 1;
    }


    averageClassification();
    delete mkPa;
    autoFlag = tempBool;
    cout <<  "AutoClass: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;

}

void Net::autoPCAClassification()
{
    autoFlag = 1;
    cfg * config;

    int nsBC = ns;
    int leftBC = left;
    int rightBC = right;
    double spStepBC = spStep;

    for(int i = ui->autpPCAMaxSpinBox->value(); i >= ui->autoPCAMinSpinBox->value(); i-=ui->autoPCAStepSpinBox->value())
    {
        cout << "numOfPc = " << i  << " started" << endl;
        config = new cfg(dirBC, 1, i, ui->critErrorDoubleSpinBox->value(), ui->learnRateBox->value(), "pca");
        config->makeCfg();
        config->close();
        if(config != NULL) delete config;

        helpString = QDir::toNativeSeparators(dirBC->absolutePath() + QDir::separator() + "pca.net");
        loadCfgByName(helpString);

        ns = 1;
        left = 1;
        right = i;
        spStep = 0.1;

        helpString = QDir::toNativeSeparators(dirBC->absolutePath() + QDir::separator() + "SpectraSmooth" + QDir::separator() + "PCA");
        cout << "numOfPc = " << i  << " ended" << endl;
        autoClassification(helpString);


        ns = nsBC;
        left = leftBC;
        right = rightBC;
        spStep = spStepBC;


        qApp->processEvents();
        if(stopFlag == 1)
        {
            stopFlag = 0;
            break;
        }

    }


}

double Net::getAverageAccuracy()
{
    return this->averageAccuracy;
}

void Net::setReduceCoeff(double coeff)
{
    this->ui->rdcCoeffSpinBox->setValue(coeff);
}

double Net::getReduceCoeff()
{
    return this->ui->rdcCoeffSpinBox->value();
}

void Net::setNumOfPairs(int num)
{
    this->ui->numOfPairsBox->setValue(num);
}

void Net::averageClassification()
{
    FILE * logFile;
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "log.txt");
    logFile = fopen(helpString.toStdString().c_str(),"r");
    if(logFile == NULL)
    {
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("cannot open logFile"), QMessageBox::Ok);
        cout << "logFile == NULL" << endl;
        return;
    }
    double  * averagePercentage = new double [NumOfClasses+1];
    double  * tempDouble = new double [NumOfClasses+1];

    for(int j = 0; j < NumOfClasses+1; ++j)
    {
        averagePercentage[j] = 0.;
        tempDouble[j] = 0.;
    }

    int num = 0;
    for(int i = 0; i < numOfTall; ++i)
    {
        for(int j = 0; j < NumOfClasses+1; ++j)
        {
            fscanf(logFile, "%lf", &tempDouble[j]);
            averagePercentage[j] += tempDouble[j];
        }
        ++num;
    }

    for(int j = 0; j < NumOfClasses+1; ++j)
    {
        averagePercentage[j] /= num;
    }
    fclose(logFile);

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "results.txt").toStdString().c_str(), "a+");
    //indents
    if(spStep != 250./1024.) fprintf(res, "\nPRR \t(");
    else fprintf(res, "\nPRR wnd \t(");

    for(int j = 0; j < NumOfClasses-1; ++j)
    {
        fprintf(res, "%.2lf - ", averagePercentage[j]);
    }
    fprintf(res, "%.2lf", averagePercentage[NumOfClasses-1]);
    fprintf(res, ")  -  %.2lf ", averagePercentage[NumOfClasses]);
    fclose(res);
    averageAccuracy = averagePercentage[NumOfClasses];
    cout << "average accuracy = " << averageAccuracy << endl;
    delete []averagePercentage;
    delete []tempDouble;
}

void Net::drawWts()  //generality
{
    if( numOfLayers != 2 ) return;
    QString out;

    QPixmap pic;
    QPainter * paint = new QPainter();

    double *** weight = new double ** [numOfLayers - 1]; // 0 - the lowest layer
    for(int i = 0; i < numOfLayers - 1; ++i)
    {
        weight[i] = new double * [dimensionality[i] + 1]; //+1 for bias
        for(int j = 0; j < dimensionality[i] + 1; ++j) //
        {
            weight[i][j] = new double [dimensionality[i+1]];
        }
    }

    if(!autoFlag)
    {
        helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget * )this, tr("wts to draw"), dirBC->absolutePath(), tr("wts files (*.wts)")));
    }
    else
    {
        helpString = dir->absolutePath();
        helpString.append(QDir::separator() + ExpName + "_weights.wts");
    }
    if(helpString == "")
    {
        cout << "no wts-file to draw was opened" << endl;
        QMessageBox::information((QWidget * )this, tr("Information"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }

    out = helpString;
    FILE * w = fopen(helpString.toStdString().c_str(),"r");
    if(w == NULL)
    {
        cout << "cannot open file" << endl;
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("Cannot open wts-file"), QMessageBox::Ok);
        return;
    }

    double maxWeight = 0.;



    for(int i = 0; i < numOfLayers-1; ++i)
    {
        for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
        {
            for(int k = 0; k < dimensionality[i+1]; ++k)
            {
                if(feof(w))
                {
                    cout << "wts-file too small" << endl;

                    QMessageBox::critical((QWidget * )this, tr("Warning"), tr("Wts-file too small. Nothing happened"), QMessageBox::Ok);
                    return;
                }

                fscanf(w, "%lf\r\n", &weight[i][j][k]);
                maxWeight = fmax(weight[i][j][k], maxWeight);
            }
            fscanf(w, "\r\n");
        }
        fscanf(w, "\r\n");
    }
    if(!feof(w))
    {
        cout << "wts-file too big" << endl;
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("Wts-file too long. Nothing happened"), QMessageBox::Ok);
        return;
    }
    fclose(w);




    pic = QPixmap(1600, 1600);
    pic.fill();
    paint->begin(&pic);

    double ext = spLength/250.;   //extension - graphical parameter
    for(int c2 = 0; c2 < ns; ++c2)  //exept markers channel & Fp1,2
    {
        for(int k = 0; k < 250-1; ++k)
        {
            paint->setPen(QPen(QBrush("blue"), 2));
            paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - weight[0][int((c2) * spLength+k * ext)][0] * 250/maxWeight, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - weight[0][int((c2) * spLength+(k+1) * ext)][0] * 250/maxWeight);
            paint->setPen(QPen(QBrush("red"), 2));
            paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - weight[0][int((c2) * spLength+k * ext)][1] * 250/maxWeight, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - weight[0][int((c2) * spLength+(k+1) * ext)][1] * 250/maxWeight);
            if(NumOfClasses == 3)
            {
                paint->setPen(QPen(QBrush("green"), 2));
                paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - weight[0][int((c2) * spLength+k * ext)][2] * 250/maxWeight, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - weight[0][int((c2) * spLength+(k+1) * ext)][2] * 250/maxWeight);
            }
        }
        paint->setPen("black");
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);

        paint->setFont(QFont("Helvitica", 8));
        for(int k = 0; k<250-1; ++k) //for every Hz generality
        {
//            paint->drawLine(paint->device()->width() * coords::x[c2]+250 * k/15, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250 * k/15, paint->device()->height() * coords::y[c2]+10);
            if( (left + k * (spLength)/250.) * spStep - floor((left + k * (spLength)/250.) * spStep) < spLength/250. * spStep/2. || ceil((left + k * (spLength)/250.) * spStep) - (left + k * (spLength)/250.) * spStep < spLength/250. * spStep/2.)
            {
                paint->drawLine(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]+5);

                helpInt = int((left + k * (spLength)/250.) * spStep + 0.5);
                helpString.setNum(helpInt);
                if(helpInt<10)
                {
                    paint->drawText(paint->device()->width() * coords::x[c2] + k-3, paint->device()->height() * coords::y[c2]+15, helpString);
                }
                else
                {
                    paint->drawText(paint->device()->width() * coords::x[c2] + k-5, paint->device()->height() * coords::y[c2]+15, helpString);
                }
            }
        }

    }
    paint->setFont(QFont("Helvetica", 24, -1, false));
    for(int c2 = 0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText((paint->device()->width() * coords::x[c2]-20), (paint->device()->height() * coords::y[c2]-252), QString(coords::lbl[c2]));
    }

    paint->drawLine(QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1]), QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())));  //250 - graph height generality

    //returning norm = max magnitude
    maxWeight = int(maxWeight * 100.)/100.;

    helpString.setNum(maxWeight);
//    helpString.append(tr(" mcV^2/Hz"));
    paint->drawText(QPointF(paint->device()->width() * coords::x[6]+5., paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())/2.), helpString);


    paint->end();
    out.replace(".wts", ".jpg");
    cout << out.toStdString() << endl;
    pic.save(out, 0, 100);

    for(int i = 0; i < numOfLayers - 1; ++i)
    {
        for(int j = 0; j < dimensionality[i] + 1; ++j) //
        {
            delete []weight[i][j];
        }
        delete []weight[i];
    }
    delete []weight;

    //automatization
    if(!autoFlag)
    {
        QMessageBox::information((QWidget * )this, tr("Info"), tr("wts is drawn"), QMessageBox::Ok);
    }
}

void Net::stopActivity()
{
    stopFlag = 1;
}


void Net::saveWts(QString wtsPath)
{
    ofstream weights;
    weights.open(wtsPath.toStdString().c_str());

    if(!weights.good())
    {
        cout << "saveWts: cannot open file = " << wtsPath.toStdString() << endl;
        return;
    }

    for(int i = 0; i < numOfLayers-1; ++i)
    {
        for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
        {
            for(int k = 0; k < dimensionality[i+1]; ++k)
            {
                weights << weight[i][j][k] << endl;
            }
            weights << endl;
        }
        weights << endl;
    }
    weights.close();
}

void Net::saveWtsSlot()
{
    //automatization
    int wtsCounter = 0;
    if(!autoFlag)
    {
        helpString = QDir::toNativeSeparators(QFileDialog::getSaveFileName((QWidget * )this, tr("wts to save"), dirBC->absolutePath(), tr("wts files (*.wts)")));
        if(!helpString.endsWith(".wts", Qt::CaseInsensitive))
        {
            helpString.append(".wts");
        }
    }
    else /////////wtf?
    {
        do
        {
            helpString = dir->absolutePath() + QDir::separator() + ExpName + "_weights_" + QString::number(wtsCounter) + ".wts";
            ++wtsCounter;
        } while(QFile::exists(helpString));
    }

    if(helpString.isEmpty())
    {
        cout << "saveWtsSlot: no file is chosen to save" << endl;
        return;
    }
    saveWts(helpString);

}

void Net::reset()
{
    srand(time(NULL));
    for(int i = 0; i < numOfLayers - 1; ++i)
    {
        for(int j = 0; j < dimensionality[i] + 1; ++j) //
        {
            for(int k = 0; k < dimensionality[i+1]; ++k)
            {
                if(ui->deltaRadioButton->isChecked()) {weight[i][j][k] = 0.;}
                else if(ui->backpropRadioButton->isChecked()) {weight[i][j][k] = (-500 + rand()%1000)/50000.;}
            }
        }
    }

}

void Net::testDistances()
{
    PaIntoMatrixByName("1");
    NumberOfErrors = new int[NumOfClasses];
    double ** averageSpectra = new double *[NumOfClasses];
//    FILE * toread;
    for(int i = 0; i < NumOfClasses; ++i)
    {
        NumberOfErrors[i] = 0;
        averageSpectra[i] = new double [NetLength];
        for(int j = 0; j < NetLength; ++j)
        {
            averageSpectra[i][j] = 0.;
        }

    }

    int * count = new int [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        count[i] = 0;
    }

    int inType;
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        inType = int(matrix[i][NetLength+1]);
        ++count[inType];
        for(int j = 0; j < NetLength; ++j)
        {
            averageSpectra[inType][j] += matrix[i][j];
        }
    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        for(int j = 0; j < NetLength; ++j)
        {
            averageSpectra[i][j] /= count[i];
        }

    }
    PaIntoMatrixByName("2");


    double *distances = new double [NumOfClasses];
    int outType;
    double helpDist = 0;

    for(int i = 0; i < NumberOfVectors; ++i)
    {

        inType = int(matrix[i][NetLength+1]);
        for(int j = 0; j < NumOfClasses; ++j)
        {
            distances[j] = distance(averageSpectra[j], matrix[i], NetLength);
        }
        outType = 0;
        helpDist = distances[0];
        for(int j = 1; j < NumOfClasses; ++j)
        {
            if(distances[j] < helpDist)
            {
                outType = j;
                helpDist = distances[j];
            }
        }
        if(outType != inType) ++NumberOfErrors[inType];
    }
    cout << "NumberOfVectors = " << NumberOfVectors << endl;
    for(int j = 0; j < NumOfClasses; ++j)
    {
        cout << "NumberOfErrors = " << NumberOfErrors[j] << endl;
    }
    int sum = 0;
    for(int j = 0; j < NumOfClasses; ++j)
    {
        sum += NumberOfErrors[j];
    }

    cout << "Percentage = " <<  100. * (1. - double(sum)/NumberOfVectors) << endl;


    for(int i = 0; i < NumOfClasses; ++i)
    {
        delete averageSpectra[i];
    }
    delete []averageSpectra;
    delete []distances;
    delete []NumberOfErrors;
    delete []count;

}

void Net::tall()
{
    Error = 0.;
    NumberOfErrors = new int[NumOfClasses];
    double * NumOfVectorsOfClass = new double [3]; // countClass
    for(int i = 0; i < NumOfClasses; ++i)
    {
        NumberOfErrors[i] = 0;
        NumOfVectorsOfClass[i] = 0;
    }
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        ClassificateVector(i);
        //generality
        NumOfVectorsOfClass[int(matrix[i][NetLength+1])] += 1;
    }
    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "log.txt");
    log = fopen(helpString.toStdString().c_str(),"a+");
    helpString.clear();
    for(int i = 0; i < NumOfClasses; ++i)
    {
        fprintf(log, "%.2lf\t", double((1. - double(NumberOfErrors[i]/double(NumOfVectorsOfClass[i]))) * 100.));
        helpString.append("Percentage[" + tmp.setNum(i) + "] = ");
        helpString.append(tmp.setNum((1. - double(NumberOfErrors[i]/double(NumOfVectorsOfClass[i]))) * 100.) + " % \n");
    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        Error += NumberOfErrors[i];
    }
    helpString.append("Percentage[all] = " + tmp.setNum((1. - double(Error/double(NumberOfVectors))) * 100.) + " % \n");

    averageAccuracy = (1. - double(Error/double(NumberOfVectors))) * 100.;

    fprintf(log, "%.2lf\n", double((1. - double(Error/double(NumberOfVectors))) * 100.));
    fclose(log);
    //automatization
    if(!autoFlag) QMessageBox::information((QWidget * )this, tr("Classification results"), helpString, QMessageBox::Ok);
    delete [] NumberOfErrors;
    delete [] NumOfVectorsOfClass;
    ++numOfTall;
}

void Net::closeLogFile()
{
    if(log != NULL)
    {
        fclose(log);
    }
}

void Net::loadCfg()
{
    //automatization
    if(!autoFlag)
    {
        helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget * )NULL,tr("open cfg"), dirBC->absolutePath(), tr("cfg files (*.net)")));
        if(helpString == "")
        {
            QMessageBox::information((QWidget * )this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
            return;
        }
    }
    else
    {
        if(spStep == 250./1024.)
        {
            helpString = "4sec19ch.net";     //for windows
        }
        else if(spStep == 250./4096.)
        {
            helpString = "16sec19ch.net";     //for realisations
        }
        else if(spStep == 250./2048.)
        {
            helpString = "8sec19ch.net";     //for realisations
        }
        else
        {
            helpString = "pca.net";     //for PCAs
        }
        helpString.prepend(dirBC->absolutePath() + QDir::separator());
        cout << "cfg auto path = " << helpString.toStdString() << endl;
    }

    loadCfgByName(helpString);

}

void Net::memoryAndParamsAllocation()
{
    //NetLength should be set
    //NumOfClasses should be set

//    cout << "memoryAndParamsAllocation: before memdel" << endl;
    if((weight != NULL) && (dimensionality != NULL) && loadPAflag)
    {
        for(int i = 0; i < numOfLayers - 1; ++i)
        {
            for(int j = 0; j < dimensionality[i] + 1; ++j) //
            {
                delete []weight[i][j];
            }
            delete []weight[i];
        }
        delete []weight;
        delete []dimensionality;
    }
//    cout << "memoryAndParamsAllocation: after memdel" << endl;

    numOfLayers = ui->numOfLayersSpinBox->value();
    helpString = ui->dimensionalityLineEdit->text();
    QStringList lst = helpString.split(QRegExp("[., ;]"), QString::SkipEmptyParts);
    if(lst.length() != numOfLayers)
    {
        QMessageBox::critical(this, tr("Error"), tr("Dimensionality and numOfLayers dont correspond"), QMessageBox::Ok);
        return;
    }
//    cout << "memoryAndParamsAllocation: before dimensionality" << endl;
    dimensionality = new int [numOfLayers];
    dimensionality[0] = NetLength;
    for(int i = 1; i < numOfLayers-1; ++i)
    {
        if(QString::number(lst[i].toInt()) != lst[i])
        {
            helpString = QString::number(i) + "'th dimensionality has bad format";
            QMessageBox::critical(this, tr("Error"), tr(helpString.toStdString().c_str()), QMessageBox::Ok);
            return;
        }
        dimensionality[i] = lst[i].toInt();
    }
    dimensionality[numOfLayers-1] = NumOfClasses;

//    cout << "memoryAndParamsAllocation: after dimensionality, before weight" << endl;

    weight = new double ** [numOfLayers - 1]; // 0 - the lowest layer

//    cout << "memoryAndParamsAllocation: weight **" << endl;
    for(int i = 0; i < numOfLayers - 1; ++i)
    {
        weight[i] = new double * [dimensionality[i] + 1]; //+1 for bias
//        cout << "memoryAndParamsAllocation: weight *" << endl;
        for(int j = 0; j < dimensionality[i] + 1; ++j) //
        {
            weight[i][j] = new double [dimensionality[i+1]];

        }
//        cout << "memoryAndParamsAllocation: after weight " << endl;
    }

    reset();

}

void Net::loadCfgByName(QString FileName)
{
    helpString = FileName;
    if(!helpString.endsWith(".net", Qt::CaseInsensitive))
    {
        helpString += ".net";
    }

    if(!FileName.contains(QDir::separator())) helpString = dir->absolutePath() + QDir::separator() + helpString;

    FILE * cfg = fopen(helpString.toStdString().c_str(),"r");
    if(cfg == NULL)
    {
        cout << "loadCfgByName: wrong cfg path = " << helpString.toStdString() << endl;
        return;
    }
    fscanf(cfg, "%*s%d\n", &NetLength);
    fscanf(cfg, "%*s%d\n", &NumOfClasses);
    fscanf(cfg, "%*s%lf\n", &learnRate);
    fscanf(cfg, "%*s%lf\n", &critError);
    fscanf(cfg, "%*s%lf\n", &temperature);
    fclose(cfg);

    memoryAndParamsAllocation();

    ui->critErrorDoubleSpinBox->setValue(critError);
    ui->tempBox->setValue(temperature);
    ui->learnRateBox->setValue(learnRate);

    loadPAflag = 1;

    channelsSet.clear();
    channelsSetExclude.clear();
    for(int i = 0; i < ns; ++i)
    {
        channelsSet << i;
    }

}

void Net::loadWtsByName(QString filename) //
{
    FILE * wts = fopen(filename.toStdString().c_str(),"r");
    if(wts == NULL)
    {
        cout << "wts-file == NULL" << endl;
        return;
    }

        if(weight == NULL) //if hasn't been allocated
        {
            numOfLayers = ui->numOfLayersSpinBox->value();
            helpString = ui->dimensionalityLineEdit->text();
            QStringList lst = helpString.split(QRegExp("[., ;]"), QString::SkipEmptyParts);
            if(lst.length() != numOfLayers)
            {
                QMessageBox::critical(this, tr("Error"), tr("Dimensionality and numOfLayers dont correspond\nweightBP hasn't been read"), QMessageBox::Ok);
                return;
            }

            int * dimensionality = new int [numOfLayers];

            dimensionality[0] = NetLength;
            for(int i = 1; i < numOfLayers-1; ++i)
            {
                if(QString::number(lst[i].toInt()) != lst[i])
                {
                    helpString = QString::number(i) + "'th dimensionality has bad format";
                    QMessageBox::critical(this, tr("Error"), tr(helpString.toStdString().c_str()), QMessageBox::Ok);
                    return;
                }
                dimensionality[i] = lst[i].toInt();
            }
            dimensionality[numOfLayers-1] = NumOfClasses;

            weight = new double ** [numOfLayers - 1]; // 0 - the lowest layer
            for(int i = 0; i < numOfLayers - 1; ++i)
            {
                weight[i] = new double * [dimensionality[i] + 1]; //+1 for bias
                for(int j = 0; j < dimensionality[i] + 1; ++j) //
                {
                    weight[i][j] = new double [dimensionality[i+1]];
                }
            }
            delete []dimensionality;
        }


        for(int i = 0; i < numOfLayers-1; ++i)
        {
            for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
            {
                for(int k = 0; k < dimensionality[i+1]; ++k)
                {
                    fscanf(wts, "%lf\r\n", &weight[i][j][k]);
                }
                fscanf(wts, "\r\n");
            }
            fscanf(wts, "\r\n");
        }

    fclose(wts);
}
//?????


void Net::loadWts()
{
    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget * )NULL,tr("load wts"), dir->absolutePath(), tr("wts files (*.wts)")));
    if(helpString == "")
    {
        QMessageBox::information((QWidget * )this, tr("Warning"), tr("No wts-file was chosen"), QMessageBox::Ok);
        return;
    }
    FILE * wts = fopen(helpString.toStdString().c_str(),"r");

        if(weight == NULL) //if hasn't been allocated
        {
            numOfLayers = ui->numOfLayersSpinBox->value();
            helpString = ui->dimensionalityLineEdit->text();
            QStringList lst = helpString.split(QRegExp("[., ;]"), QString::SkipEmptyParts);
            if(lst.length() != numOfLayers)
            {
                QMessageBox::critical(this, tr("Error"), tr("Dimensionality and numOfLayers dont correspond\nweightBP hasn't been read"), QMessageBox::Ok);
                return;
            }

            int * dimensionality = new int [numOfLayers];

            dimensionality[0] = NetLength;
            for(int i = 1; i < numOfLayers-1; ++i)
            {
                if(QString::number(lst[i].toInt()) != lst[i])
                {
                    helpString = QString::number(i) + "'th dimensionality has bad format";
                    QMessageBox::critical(this, tr("Error"), tr(helpString.toStdString().c_str()), QMessageBox::Ok);
                    return;
                }
                dimensionality[i] = lst[i].toInt();
            }
            dimensionality[numOfLayers-1] = NumOfClasses;

            weight = new double ** [numOfLayers - 1]; // 0 - the lowest layer
            for(int i = 0; i < numOfLayers - 1; ++i)
            {
                weight[i] = new double * [dimensionality[i] + 1]; //+1 for bias
                for(int j = 0; j < dimensionality[i] + 1; ++j) //
                {
                    weight[i][j] = new double [dimensionality[i+1]];
                }
            }
            delete []dimensionality;
        }


        for(int i = 0; i < numOfLayers-1; ++i)
        {
            for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
            {
                for(int k = 0; k < dimensionality[i+1]; ++k)
                {
                    fscanf(wts, "%lf\r\n", &weight[i][j][k]);
                }
                fscanf(wts, "\r\n");
            }
            fscanf(wts, "\r\n");
        }

    fclose(wts);
}


void Net::leaveOneOutSlot()
{
    if(!ui->openclCheckBox->isChecked())
    {
//        cout << "simple leaveOneOut"  << endl;
        leaveOneOut();
    }
    else
    {
//        cout << "OpenCL leaveOneOut"  << endl;
//        leaveOneOutCL();
    }
}


const char *  kernelFromFile(char *  path)
{
    char *  tempString = new char [300];
    char *  shaderString = new char [30000];
    int currentIndex = 0;
    FILE * shad = fopen(path, "r");
    if(shad == NULL)
    {
        cout << "Cannot open file\n" << endl;
        return (const char * )NULL;
    }
    while(1)
    {

        fgets(tempString, 50, shad);
        if(feof(shad)) break;
        for(int i = 0; i < strlen(tempString); ++i)
        {
            shaderString[currentIndex++] = tempString[i];
        }
    }
    shaderString[currentIndex] = '\0';
    fclose(shad);

    delete []tempString;
    return shaderString;
}


const char *  errorMessage(int err_)
{
    return QString::number(err_).toStdString().c_str();
}


/*
void Net::leaveOneOutCL()
{

    QTime myTime;
    myTime.start();
    cout << "leaveOneOutCL started" << endl;
    NumberOfErrors = new int[NumOfClasses];
    helpString = "";
    for(int i = 0; i < NumOfClasses; ++i)
    {
        NumberOfErrors[i] = 0;
    }

    cl_int clError;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel leaveOneOutKernel;
    cl_device_type devType;
    cl_platform_id platform;


    devType = CL_DEVICE_TYPE_CPU;

    clError = clGetPlatformIDs(1,
                               &platform,
                               NULL);
    if(clError != CL_SUCCESS)
    {
        cout << "Cannot get platform Id: " << errorMessage(clError) << endl;
        exit(clError);
    }


    // Find the device.
    clError = clGetDeviceIDs( platform,
                    devType,
                    1,
                    &device,
                    NULL);
    if(clError != CL_SUCCESS)
    {
        cout << "Error getting device ids: " << errorMessage(clError) << endl;
        exit(clError);
    }

//    cl_device_fp_config doubleSupport;
//    cl_int doubleWork = clGetDeviceInfo(device, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(cl_device_fp_config), &doubleSupport, NULL);
//    cout << doubleWork << "\tDoubleSupport = " << doubleSupport << endl;
//    return -2;

//    CL_INVALID_DEVICE;// if device is not valid.
//    CL_INVALID_VALUE;// if param_name is not one of the supported values or if size in bytes specified by param_value_size is less than size of return type as shown in the table above and param_value is not a NULL value or if param_name is a value that is available as an extension and the corresponding extension is not supported by the device.
//    CL_OUT_OF_RESOURCES;// if there is a failure to allocate resources required by the OpenCL implementation on the device.
//    CL_OUT_OF_HOST_MEMORY;// if there is a failure to allocate resources required by the OpenCL implementation on the host.

    // 4. Compute work sizes.
    cl_uint compute_units;
    size_t global_work_size;
    clError = clGetDeviceInfo(device,
                     CL_DEVICE_MAX_COMPUTE_UNITS,
                     sizeof(cl_uint),
                     &compute_units,
                     NULL);
    if(clError != CL_SUCCESS)
    {
        cout << "Error getting device info: " << errorMessage(clError) << endl;
        exit(clError);
    }
    cout << "Max compute units = " << compute_units << endl;

    cl_ulong maxConstBufSize;
    clError = clGetDeviceInfo(device,
                     CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
                     sizeof(cl_ulong),
                     &maxConstBufSize,
                     NULL);
    if(clError != CL_SUCCESS)
    {
        cout << "Error getting device info: " << errorMessage(clError) << endl;
        exit(clError);
    }
    cout << "Max const buffer size = " << maxConstBufSize << " bytes" <<  endl;
    cout << "it is = " << maxConstBufSize/sizeof(double) << " doubles" <<  endl;



//    if(compute_units > NumberOfVectors)
//    {
//        global_work_size = NumberOfVectors;
//        local_work_size = 1;
//    }
//    else
//    {
//        local_work_size = ceil(double(NumberOfVectors)/compute_units);
//        global_work_size = NumberOfVectors;
//    }


    global_work_size = NumberOfVectors;



    // Create a context and command queue on that device.
    context = clCreateContext( NULL,
                               1,
                               &device,
                               NULL, NULL, NULL);

    queue = clCreateCommandQueue(context,
                                 device,
                                 0, NULL);
    // Minimal error check.
    if( queue == NULL )
    {
        cout << "Compute device setup failed" << endl; ;
        return;
    }

    // Perform runtime source compilation, and obtain kernel entry point.
    const char  * kernel_source = (const char * )kernelFromFile("/home/michael/Qt/Projects/SecondGUI/kernels.cl"); //generality
    cl_int ret = 0;
    program = clCreateProgramWithSource( context,
                                         1,
                                         &kernel_source,
                                         NULL, &ret );


    cout << "start build program" << endl;
    clError = clBuildProgram( program, 1, &device, NULL, NULL, NULL);
    cout << "end build program" << endl;
    //Tell compiler to dump intermediate .il and .isa GPU files.
    // 5. Print compiler error messages
    if(clError != CL_SUCCESS)
    {
        cout << "clBuildProgram failed: " << errorMessage(clError) << endl;
        char buf[0x10000];
        clGetProgramBuildInfo( program,
                               device,
                               CL_PROGRAM_BUILD_LOG,
                               0x10000,
                               buf,
                               NULL);
        cout << buf << endl;
        exit(clError);
    }

    leaveOneOutKernel = clCreateKernel( program, "leaveOneOut", &clError );
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create kernel leaveOneOut: " << errorMessage(clError) << endl;
        exit(clError);
    }
    cout << "all the preparations done, memory allocation start, elapsed " << myTime.elapsed()/1000. << " sec" << endl;
    myTime.restart();
    // Create input, output and debug buffers.
//    __global double ecrit,
//    __global double lrate,
//    __global double error,
//    __global double temp,
//    __global double matrix,
//    __global int NumberOfVectors,
//    __global int NumOfClasses,
//    __global int NetLength,
//    __private double ** weight,
//    __private int * mixNum,
//    __private double * output,
//    __private bool answer,
//    __private double outError,
//    __private double * outputClass,
//    __global double * NumberOfErrors,
//    __private int NumOfThread
    cl_mem ecritBuf;
    cl_mem lrateBuf;
    cl_mem errorBuf;
    cl_mem tempBuf;
    cl_mem matrixBuf;
    cl_mem numOfVectsBuf;
    cl_mem numOfClassesBuf;
    cl_mem netLengthBuf;
    cl_mem weightBuf;
    cl_mem mixNumBuf;
    cl_mem outputBuf;
    cl_mem answerBuf;
    cl_mem outErrorBuf;
    cl_mem outputClassBuf;
    cl_mem numOfErrorsBuf;
    cl_mem NumOfThreadBuf;
    cl_mem NumOfVectorToSkipBuf;


//    CL_INVALID_CONTEXT;
//    CL_INVALID_VALUE;
//    CL_INVALID_BUFFER_SIZE;
//    CL_INVALID_HOST_PTR;
//    CL_MEM_OBJECT_ALLOCATION_FAILURE;
//    CL_OUT_OF_RESOURCES;
//    CL_OUT_OF_HOST_MEMORY;
    ecritBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_double),
                              &critError,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 0: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    lrateBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_double),
                              &learnRate,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 1: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    errorBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_double),
                              &Error,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 2: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    tempBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_double),
                              &temperature,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 2.5: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    double  * matrixArray = new double [NumberOfVectors * (NetLength + 2)];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        for(int j = 0; j < (NetLength + 2); ++j)
        {
            matrixArray[i * (NetLength + 2) + j] = matrix[i][j];
        }
    }

    matrixBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_double) * NumberOfVectors * (NetLength + 2),
                              matrixArray,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 3: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    numOfVectsBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_int),
                              &NumberOfVectors,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 4: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    numOfClassesBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_int),
                              &NumOfClasses,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 5: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    netLengthBuf = clCreateBuffer(context,
                              CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_int),
                              &NetLength,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 6: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    //privates:

    weightBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE,
                              sizeof(cl_double) * NumOfClasses * (NetLength + 1),
                              NULL,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 7: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }


    mixNumBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE,
                              sizeof(cl_int) * global_work_size,
                              NULL,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 8: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    outputBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE,
                              sizeof(cl_double) * NumOfClasses,
                              NULL,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 9: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    answerBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE,
                              sizeof(cl_bool) * global_work_size,
                              NULL,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 10: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    outErrorBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE,
                              sizeof(cl_double) * global_work_size,
                              NULL,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 11: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    outputClassBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE,
                              sizeof(cl_double) * NumOfClasses,
                              NULL,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 12: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    //global:
    numOfErrorsBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                              sizeof(cl_int) * NumOfClasses,
                              &NumberOfErrors,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 13: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    //private:
    NumOfThreadBuf = clCreateBuffer(context,
                              CL_MEM_READ_WRITE,
                              sizeof(cl_int) * global_work_size,
                              NULL,
                              &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 14: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    NumOfVectorToSkipBuf = clCreateBuffer(context,
                                          CL_MEM_READ_WRITE,
                                          sizeof(cl_int) * global_work_size,
                                          NULL,
                                          &clError);
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer 15: " << errorMessage(clError) << endl;
        exit(clError);
    }
    else
    {
        cout << "Memory buffer created" << endl;
    }

    cout << "buffers ready, elapsed " << myTime.elapsed()/1000. << " sec" << endl;
    myTime.restart();




    clSetKernelArg(leaveOneOutKernel, 0, sizeof(ecritBuf), (void * ) &ecritBuf);
    clSetKernelArg(leaveOneOutKernel, 1, sizeof(lrateBuf), (void * ) &lrateBuf);
    clSetKernelArg(leaveOneOutKernel, 2, sizeof(errorBuf), (void * ) &errorBuf);
    clSetKernelArg(leaveOneOutKernel, 3, sizeof(tempBuf), (void * ) &tempBuf);
    clSetKernelArg(leaveOneOutKernel, 4, sizeof(matrixBuf), (void * ) &matrixBuf);
    clSetKernelArg(leaveOneOutKernel, 5, sizeof(numOfVectsBuf), (void * ) &numOfVectsBuf);
    clSetKernelArg(leaveOneOutKernel, 6, sizeof(numOfClassesBuf), (void * ) &numOfClassesBuf);
    clSetKernelArg(leaveOneOutKernel, 7, sizeof(netLengthBuf), (void * ) &netLengthBuf);
    clSetKernelArg(leaveOneOutKernel, 8, sizeof(weightBuf), (void * ) &weightBuf);
    clSetKernelArg(leaveOneOutKernel, 9, sizeof(mixNumBuf), (void * ) &mixNumBuf);
    clSetKernelArg(leaveOneOutKernel, 10, sizeof(outputBuf), (void * ) &outputBuf);
    clSetKernelArg(leaveOneOutKernel, 11, sizeof(answerBuf), (void * ) &answerBuf);
    clSetKernelArg(leaveOneOutKernel, 12, sizeof(outErrorBuf), (void * ) &outErrorBuf);
    clSetKernelArg(leaveOneOutKernel, 13, sizeof(outputClassBuf), (void * ) &outputClassBuf);
    clSetKernelArg(leaveOneOutKernel, 14, sizeof(numOfErrorsBuf), (void * ) &numOfErrorsBuf);
    clSetKernelArg(leaveOneOutKernel, 15, sizeof(NumOfThreadBuf), (void * ) &NumOfThreadBuf);
    clSetKernelArg(leaveOneOutKernel, 16, sizeof(NumOfVectorToSkipBuf), (void * ) &NumOfVectorToSkipBuf);


    cout << "kernelArgs are set, elapsed " << myTime.elapsed()/1000. << " sec" << endl;
    myTime.restart();

    clEnqueueNDRangeKernel( queue,
                            leaveOneOutKernel,
                            1,
                            NULL,
                            &global_work_size,
                            NULL,
                            0, NULL, NULL);

    clFinish( queue );



    //    values to look at the results
//    cl_bool  * returnedAnswer;
//    cl_double  * returnedError;
    cl_int  * returnedNumofthread;
//    cl_int  * returnedNumOfSkipped;


//    returnedAnswer = (cl_bool  * ) clEnqueueMapBuffer( queue,
//                                                  answerBuf,
//                                                  CL_TRUE,
//                                                  CL_MAP_READ,
//                                                  0,
//                                                  sizeof(cl_bool) * global_work_size,
//                                                  0, NULL, NULL, &clError );
//    if (clError != CL_SUCCESS)
//    {
//        cout << "Cannot create memory buffer: " << errorMessage(clError) << endl;
//        exit(clError);
//    }

//    returnedError = (cl_double  * ) clEnqueueMapBuffer( queue,
//                                                  outErrorBuf,
//                                                  CL_TRUE,
//                                                  CL_MAP_READ,
//                                                  0,
//                                                  sizeof(cl_double) * global_work_size,
//                                                  0, NULL, NULL, &clError );
//    if (clError != CL_SUCCESS)
//    {
//        cout << "Cannot create memory buffer: " << errorMessage(clError) << endl;
//        exit(clError);
//    }

    returnedNumofthread = (cl_int  * ) clEnqueueMapBuffer( queue,
                                                  NumOfThreadBuf,
                                                  CL_TRUE,
                                                  CL_MAP_READ,
                                                  0,
                                                  sizeof(cl_int) * global_work_size,
                                                  0, NULL, NULL, &clError );
    if (clError != CL_SUCCESS)
    {
        cout << "Cannot create memory buffer: " << errorMessage(clError) << endl;
        exit(clError);
    }

//    returnedNumOfSkipped = (cl_int  * ) clEnqueueMapBuffer( queue,
//                                                         NumOfVectorToSkipBuf,
//                                                         CL_TRUE,
//                                                         CL_MAP_READ,
//                                                         0,
//                                                         sizeof(cl_int) * global_work_size,
//                                                         0, NULL, NULL, &clError );
//    if (clError != CL_SUCCESS)
//    {
//        cout << "Cannot create memory buffer: " << errorMessage(clError) << endl;
//        exit(clError);
//    }

    for(int i = 0; i < global_work_size; ++i)
    {
//        cout << "NumOfThread = " << returnedNumofthread[i] << " NumOfSkipped = " << returnedNumOfSkipped[i] << "\tError = " << returnedError[i] << "\tAnswer = " << returnedAnswer[i]  << endl;
        cout << "NumOfThread = " << returnedNumofthread[i]  << endl;
    }



    cout << "leaveOneOut ended " << endl;

    cout << "N-fold cross-validation: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;

    delete []NumberOfErrors;
    delete []matrixArray;
}
*/


void Net::leaveOneOut()
{
    QTime myTime;
    myTime.start();

    srand(time(NULL));

    critError = ui->critErrorDoubleSpinBox->value();
    temperature = ui->tempBox->value();
    learnRate = ui->learnRateBox->value();
    currentError = critError + 0.1;

    double ** output = new double * [numOfLayers]; //data and global output together
    for(int i = 0; i < numOfLayers; ++i)
    {
        output[i] = new double [dimensionality[i] + 1];
    }
    double ** deltaWeights = new double * [numOfLayers]; // 0 - unused for lowest layer
    for(int i = 0; i < numOfLayers; ++i)
    {
        deltaWeights[i] = new double [dimensionality[i]];
    }


    int * mixNum = new int [NumberOfVectors];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        mixNum[i] = i;
    }

    //part from tall();
    Error = 0.;
    NumberOfErrors = new int[NumOfClasses];
    helpString = "";
    for(int i = 0; i < NumOfClasses; ++i)
    {
        NumberOfErrors[i] = 0;
    }

    double * normCoeff = new double [NumOfClasses];
    double helpMin = classCount[0];
    for(int i = 1; i < NumOfClasses; ++i)
    {
        helpMin = min(helpMin, classCount[i]);
    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        normCoeff[i] = helpMin/classCount[i];
    }


    int a1, a2, buffer;
    int index;
    int type;
    for(int h = 0; h < NumberOfVectors; ++h) //learn & classify every vector
    {
        myTime.restart();

        reset();
        epoch = 0;
        //start learning
        while(currentError > critError && epoch < ui->epochSpinBox->value())
        {
            currentError = 0.0;
            //mix vectors
            for(int i = 0; i < 5 * NumberOfVectors; ++i)
            {
                a1 = rand()%(NumberOfVectors);
                a2 = rand()%(NumberOfVectors);
                buffer = mixNum[a2];
                mixNum[a2] = mixNum[a1];
                mixNum[a1] = buffer;
            }
            //        cout << "epoch = " << epoch << endl;

            for(int vecNum = 0; vecNum <NumberOfVectors; ++vecNum)
            {
                index = mixNum[vecNum];
                type = matrix[index][NetLength + 1];
                if(index == h) continue; //not to learn with h'th vector

                for(int j = 0; j < dimensionality[0]; ++j)
                {
                    output[0][j] = matrix[index][j];
                }
                output[0][dimensionality[0]] = 1.;

                //obtain outputs
                for(int i = 1; i < numOfLayers; ++i)
                {
                    for(int j = 0; j < dimensionality[i]; ++j)
                    {
                        output[i][j] = 0.;
                        for(int k = 0; k < dimensionality[i-1] + 1; ++k) //-1 for prev.layer, +1 for bias
                        {
                            output[i][j] += weight[i-1][k][j] * output[i-1][k];
                        }
                        output[i][j] = logistic(output[i][j], temperature);
                    }
                    output[i][dimensionality[i]] = 1.; //unused for the highest layer
                }

                //error in the last layer
                for(int j = 0; j < dimensionality[numOfLayers-1]; ++j)
                {
                    currentError += (output[numOfLayers-1][j] - (type == j)) * (output[numOfLayers-1][j] - (type == j));
                }

                //count deltaweights
                //for the last layer
                for(int j = 0; j < dimensionality[numOfLayers-1]; ++j)
                {
                    deltaWeights[numOfLayers-1][j] = -1./temperature * output[numOfLayers-1][j] * (1. - output[numOfLayers-1][j]) * ((type == j) - output[numOfLayers-1][j]); //~0.1
                }


                //for the other layers, upside->down
                for(int i = numOfLayers-2; i >= 1; --i)
                {
                    for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
                    {
                        deltaWeights[i][j] = 0.;
                        for(int k = 0; k < dimensionality[i+1]; ++k) //connected to the higher-layer
                        {
                            deltaWeights[i][j] += deltaWeights[i+1][k] * weight[i][j][k];
                        }
                        deltaWeights[i][j]  *= 1./temperature * output[i][j] * (1. - output[i][j]);
                    }
                }

                for(int i = 0; i < numOfLayers-1; ++i)
                {
                    for(int j = 0; j < dimensionality[i]; ++j)
                    {
                        for(int k = 0; k < dimensionality[i+1]; ++k)
                        {
                            if(ui->backpropRadioButton->isChecked())    weight[i][j][k] -= learnRate * normCoeff[type] * deltaWeights[i+1][k] * output[i][j];
                            else if(ui->deltaRadioButton->isChecked())  weight[i][j][k] += learnRate * normCoeff[type] * output[i][j] * ((type == k) - output[i+1][k]);
                        }
                    }
                }
            }

            currentError/=NumberOfVectors;
            currentError = sqrt(currentError);
            ++epoch;
            this->ui->currentErrorDoubleSpinBox->setValue(currentError);

        }//endof all epoches, end of learning

        ClassificateVector(h);
        cout << h << " vector ended\t" << epoch << " epoches\t" << myTime.elapsed()/1000. << " sec" << endl;
        currentError = critError + 0.1;
    }

    cout << "leaveOneOut ended " << endl;

    cout << "N-fold cross-validation: time elapsed = " << myTime.elapsed()/1000. << " sec"  << endl;


    helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "log.txt");
    log = fopen(helpString.toStdString().c_str(),"a+");
    if(log == NULL)
    {
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("Cannot open log file to write"), QMessageBox::Ok);
        return;
    }




    for(int i = 0; i < NumOfClasses; ++i)
    {
        fprintf(log, "%.2lf\t", double((1. - double(NumberOfErrors[i] * NumOfClasses/double(NumberOfVectors))) * 100.));
    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        Error += NumberOfErrors[i];
    }
    fprintf(log, "%.2lf\n", double((1. - double(Error/double(NumberOfVectors))) * 100.));
    fclose(log);

    //time

    delete []NumberOfErrors;
    delete []mixNum;



    for(int i = 0; i < numOfLayers; ++i)
    {
        delete []deltaWeights[i];
        delete []output[i];
    }
    delete []deltaWeights;
    delete []output;



}


void Net::PaIntoMatrix()
{
    if(loadPAflag != 1)
    {
        QMessageBox::critical((QWidget * )this, tr("Warning"), tr("No CFG-file loaded yet"), QMessageBox::Ok);
        return;
    }

    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget * )NULL, tr("load PA"), dir->absolutePath(), tr("PA files (*.pa)")));
    if(helpString == "")
    {
        QMessageBox::information((QWidget * )this, tr("Information"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }
//    QTime myTime;
//    myTime.start();
    cout << "PaIntoMatrix: NetLength = " << NetLength << endl;
    cout << "PaIntoMatrix: ns = " << ns << endl;
    readPaFile(helpString, &matrix, NetLength, NumOfClasses, &NumberOfVectors, &FileName, &classCount);
//    cout << "PaRead: time elapsed = " << myTime.elapsed()/1000. << " sec"  << endl;

}


void Net::PaIntoMatrixByName(QString fileName)
{
    if(loadPAflag != 1)
    {
        QMessageBox::critical((QWidget * )this, tr("net.cpp: PaIntoMatrixByName"), tr("No CFG-file loaded yet"), QMessageBox::Ok);
        return;
    }
    helpString = dirBC->absolutePath() + QDir::separator() + "PA" + QDir::separator() + fileName;
    if(!fileName.contains(".pa"))
    {
        helpString += ".pa";
    }
    paFileBC = helpString;
    readPaFile(helpString, &matrix, NetLength, NumOfClasses, &NumberOfVectors, &FileName, &classCount);
    /*
    double * tempVector = new double [ns*spLength];

    for(int k = 0; k < NumberOfVectors; ++k)
    {
        for(int j = 0; j < spLength*ns; ++j)
        {
            tempVector[j] = 0.;
        }

        for(int i = 0; i < ns; ++i) //new channel number
        {
            for(int j = 0; j < spLength; ++j) // each spectra-bin
            {
                for(int h = 0; h < ns; ++h) //new channel number
                {
                    tempVector[i*spLength + j] += tempRandomMatrix[i][h] * matrix[k][h*spLength + j]; //matrixNew^T = tempRandom* matrix^T
                }
            }
        }
        memcpy(matrix[k], tempVector, sizeof(double) * ns*spLength);
    }

    delete []tempVector;
*/

}

double HopfieldActivation(double x, double temp)
{
    return ((1./(1. + exp(-x/temp/300.)) * 2.) - 1.) * temp;
}

void Net::Hopfield()
{
    double maxH = 0.;
    double * output1 = new double [NetLength];
    double * output2 = new double [NetLength];
    MakePa  * mkPa = new MakePa(dir->absolutePath() + QDir::separator() + "SpectraSmooth", ExpName, ns, left, right, spStep);
    mkPa->setRdcCoeff(10);
    mkPa->makePaSlot();

    PaIntoMatrixByName("all");
    for(int i = 0; i < NetLength; ++i)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            maxH = fmax(fabs(matrix[j][i]), maxH);
        }
    }
    cout << "maxH = " << maxH << endl;
//    LearnNet();

    double ** weightH = new double * [NetLength];
    for(int i = 0; i < NetLength; ++i)
    {
        weightH[i] = new double [NetLength];
    }


    PaIntoMatrixByName("1");
    for(int i = 0; i < NetLength; ++i)
    {
        for(int j = 0; j < NetLength; ++j)
        {
            weightH[i][j] = 0.;
            for(int k = 0; k < NumberOfVectors; ++k)
            {
                weightH[i][j] += matrix[k][i] * matrix[k][j];
            }
            weightH[i][j] /= NumberOfVectors;
        }
    }


    int * mixNumH = new int [NetLength];
    for(int i = 0; i < NetLength; ++i)
    {
        mixNumH[i] = i;
    }
    int a1,a2,buff;
    srand(time(NULL));



    double sumH;

    double * outputClass = new double [NumOfClasses];
//    int type;
    bool answer;
    int NumberOfErrorsH = 0;
    double ** attractorsH = new double * [NumberOfVectors];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        attractorsH[i] = new double [NetLength+1];
    }

    for(int i = 0; i < NumberOfVectors; ++i) //number of vector to classify
    {
        for(int j = 0; j < NetLength; ++j)
        {
            output1[j] = matrix[i][j];
            output2[j] = matrix[i][j] + 1;
        }

        for(int j = 0; j < NetLength * 10; ++j)
        {
            a1 = rand()%NetLength;
            a2 = rand()%NetLength;
            buff = mixNumH[a1];
            mixNumH[a1] = mixNumH[a2];
            mixNumH[a2] = buff;
        }
        cout << i << " vector Hopfield start" << endl;

        //Hopfield convergence
        while(distance(output1, output2, NetLength) > 1e-4)
        {
            for(int j = 0; j < NetLength; ++j)
            {
                output2[j] = output1[j];
            }

            for(int j = 0; j < NetLength * 5; ++j)
            {
                a1 = rand()%NetLength;
                a2 = rand()%NetLength;
                buff = mixNumH[a1];
                mixNumH[a1] = mixNumH[a2];
                mixNumH[a2] = buff;
            }

            for(int j = 0; j < NetLength; ++j) //mixNum[j] - number of neuron to recalculate
            {
                //asynchronous
                sumH = 0.;
                for(int k = 0; k < NetLength; ++k)
                {
                    sumH += weightH[mixNumH[j]][k] * output1[k];
                }
                output1[mixNumH[j]] = HopfieldActivation(sumH, maxH);

            }
//            cout << "diff = " << distance(output1, output2, NetLength)  << endl;
        }
        for(int j = 0; j < NetLength; ++j)
        {
            attractorsH[i][j] = output1[j];
        }
        attractorsH[i][NetLength] = matrix[i][NetLength+1];

    }

    for(int i = 0; i < NumberOfVectors; ++i)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            if( distance(attractorsH[i], attractorsH[j], NetLength) > 1.) cout << " x  ";
            else cout << "000 ";
        }
        cout << matrix[i][NetLength+1] << endl;
    }
//    return;
//    LearnNet();


    PaIntoMatrixByName("2");

    for(int i = 0; i < NumberOfVectors; ++i) //number of vector to classify
    {
        for(int j = 0; j < NetLength; ++j)
        {
            output1[j] = matrix[i][j];
            output2[j] = matrix[i][j] + 1;
        }

        for(int j = 0; j < NetLength * 10; ++j)
        {
            a1 = rand()%NetLength;
            a2 = rand()%NetLength;
            buff = mixNumH[a1];
            mixNumH[a1] = mixNumH[a2];
            mixNumH[a2] = buff;
        }
        cout << i << " vector Hopfield start" << endl;

        //Hopfield convergence
        while(distance(output1, output2, NetLength) > 1e-4)
        {
            for(int j = 0; j < NetLength; ++j)
            {
                output2[j] = output1[j];
            }

            for(int j = 0; j < NetLength * 5; ++j)
            {
                a1 = rand()%NetLength;
                a2 = rand()%NetLength;
                buff = mixNumH[a1];
                mixNumH[a1] = mixNumH[a2];
                mixNumH[a2] = buff;
            }

            for(int j = 0; j < NetLength; ++j) //number of neuron
            {
                //asynchronous
                sumH = 0.;
                for(int k = 0; k < NetLength; ++k)
                {
                    sumH += weightH[mixNumH[j]][k] * output1[k];
                }
                output1[mixNumH[j]] = HopfieldActivation(sumH, maxH);
            }
//            cout << "diff = " << distance(output1, output2, NetLength)  << endl;
        }

        buff = 0;
        sumH = distance(output1, attractorsH[0], NetLength);

        for(int j = 0; j < NumberOfVectors; ++j)
        {
            if(sumH > distance(output1, attractorsH[j], NetLength))
            {
                buff = j;
                sumH = distance(output1, attractorsH[j], NetLength);
            }
//            cout << distance(output1, attractorsH[j], NetLength) << " ";
        }
        cout << endl;
        if(matrix[i][NetLength+1] == attractorsH[buff][NetLength]) answer = true;
        else answer = false;
        cout << "classification: dist = " << sumH << " attrNum = " << buff << " class coincidence = " << answer  << endl;

//        //classifying perceptron
//        type = int(matrix[i][NetLength+1]);
//        for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
//        {
//            outputClass[j] = 0.;
//            for(int h = 0; h < NetLength; ++h)
//            {
//                outputClass[j] += weight[j][h] * output1[h];
//            }
//            outputClass[j] += weight[j][NetLength] * matrix[i][NetLength];
//            outputClass[j] = logistic(outputClass[j], temp); // unlinear conformation
//        }
//        answer = true;
//        for(int k = 0; k<NumOfClasses; ++k)
//        {
//            if(k  != type && outputClass[k] >= outputClass[type])
//            {
//                answer = false;
//            }
//        }
//        cout << int(answer) << endl;
        if(!answer) ++NumberOfErrorsH;
    }
    cout << "Percentage right = " << (1. - double(NumberOfErrorsH)/NumberOfVectors) * 100. << " %" << endl;





    for(int i = 0; i < NetLength; ++i)
    {
        delete [] weightH[i];
    }
    delete [] weightH;
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        delete [] attractorsH[i];
    }
    delete [] attractorsH;

    delete [] mixNumH;
    delete [] output1;
    delete [] output2;
    delete [] outputClass;
    delete mkPa;


}

void Net::methodSetParam(int a, bool ch)
{
    if(!loadPAflag) return;
    if(!ch) return;
    if(a == -2 )
    {
//        cout << "delta button pressed" << endl;
        ui->epochSpinBox->setValue(250);
        ui->tempBox->setValue(10);
        ui->learnRateBox->setValue(0.1);
        ui->critErrorDoubleSpinBox->setValue(0.1);

        ui->dimensionalityLineEdit->setText("0 0");
        ui->dimensionalityLineEdit->setReadOnly(true);

        ui->numOfLayersSpinBox->setValue(2);
        ui->numOfLayersSpinBox->setReadOnly(true);

    }
    if(a == -3)
    {
//        cout << "backprop button pressed" << endl;
        ui->epochSpinBox->setValue(300);
        ui->tempBox->setValue(2);
        ui->learnRateBox->setValue(1.0);
        ui->critErrorDoubleSpinBox->setValue(0.05);

        ui->dimensionalityLineEdit->setText("0 20 0");
        ui->dimensionalityLineEdit->setReadOnly(false);

        ui->numOfLayersSpinBox->setValue(3);
        ui->numOfLayersSpinBox->setReadOnly(false);
    }
    memoryAndParamsAllocation();
}

void Net::LearnNet() //(double ** data, int * numOfClass, int NumOfVectors, int numOfLayers, int * dimensionality) //data[NumOfVectors][dimensionality[0]]. dimensionality doesn't include bias
{
    QTime myTime;
    myTime.start();
    srand(time(NULL));



    double ** deltaWeights = new double * [numOfLayers]; // 0 - unused for lowest layer
    for(int i = 0; i < numOfLayers; ++i)
    {
        deltaWeights[i] = new double [dimensionality[i]];
    }

    double ** output = new double * [numOfLayers]; //data and global output together
    for(int i = 0; i < numOfLayers; ++i)
    {
        output[i] = new double [dimensionality[i] + 1];
    }

    critError = ui->critErrorDoubleSpinBox->value();
    currentError = critError + 0.1;
    temperature = ui->tempBox->value();
    learnRate = ui->learnRateBox->value();
//    double momentum = ui->momentumDoubleSpinBox->value(); //unused yet

    int type = 0;

    epoch = 0;

    int  * mixNum = new int [NumberOfVectors];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        mixNum[i] = i;
    }
    int a1, a2, buffer; //for mixNum mixing
    int index;

    reset();

//    cout << "LearnNet: dimensionality[0] = " << dimensionality[0] << endl;
//    cout << "LearnNet: NetLength = " << NetLength << endl;

    double * normCoeff = new double [NumOfClasses];
    double helpMin = classCount[0];
    for(int i = 1; i < NumOfClasses; ++i)
    {
        helpMin = min(helpMin, classCount[i]);
    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        normCoeff[i] = helpMin/classCount[i];
    }

    while(currentError > critError && epoch < ui->epochSpinBox->value())
    {
        srand(time(NULL));
        currentError = 0.0;
        for(int i = 0; i < 5 * NumberOfVectors; ++i)
        {
            a1 = rand()%(NumberOfVectors);
            a2 = rand()%(NumberOfVectors);
            buffer = mixNum[a2];
            mixNum[a2] = mixNum[a1];
            mixNum[a1] = buffer;
        }
        for(int num = 0; num < NumberOfVectors; ++num)
        {

            index = mixNum[num];
            type = matrix[index][dimensionality[0] + 1]; //generality
            if( type*(type-1.)*(type-2.) != 0. )
            {
                cout << "bad type" << endl;
                this->close();
                this->~Net();
            }


            for(int j = 0; j < dimensionality[0]; ++j)
            {
                output[0][j] = matrix[index][j];
            }
            output[0][dimensionality[0]] = 1.; //bias???

            //obtain outputs
            for(int i = 1; i < numOfLayers; ++i)
            {

//#pragma omp parallel for schedule(dynamic) num_threads(dimensionality[i]) shared(output)
                for(int j = 0; j < dimensionality[i]; ++j)
                {
                    output[i][j] = 0.;

                        for(int k = 0; k < dimensionality[i-1] + 1; ++k) //-1 for prev.layer, +1 for bias
                        {
                            output[i][j] += weight[i-1][k][j] * output[i-1][k];
                        }

                    output[i][j] = logistic(output[i][j], temperature);
                }
                output[i][dimensionality[i]] = 1.; //unused for the highest layer
            }

            //error in the last layer
            for(int j = 0; j < dimensionality[numOfLayers-1]; ++j)
            {
                currentError += (output[numOfLayers-1][j] - (type == j)) * (output[numOfLayers-1][j] - (type == j));
            }

            //count deltaweights
            //for the last layer
            for(int j = 0; j < dimensionality[numOfLayers-1]; ++j)
            {
                deltaWeights[numOfLayers-1][j] = -1./temperature * output[numOfLayers-1][j] * (1. - output[numOfLayers-1][j]) * ((type == j) - output[numOfLayers-1][j]); //~0.1
            }


            //for the other layers, upside->down
            for(int i = numOfLayers-2; i >= 1; --i)
            {
                for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
                {
                    deltaWeights[i][j] = 0.;
                    for(int k = 0; k < dimensionality[i+1]; ++k) //connected to the higher-layer
                    {
                        deltaWeights[i][j] += deltaWeights[i+1][k] * weight[i][j][k];
                    }
                    deltaWeights[i][j]  *= 1./temperature * output[i][j] * (1. - output[i][j]);
                }
            }
            for(int i = 0; i < numOfLayers-1; ++i)
            {
                for(int j = 0; j < dimensionality[i] + 1; ++j)//+1 for bias? 01.12.2014
                {
                    for(int k = 0; k < dimensionality[i+1]; ++k)
                    {
                        if(ui->backpropRadioButton->isChecked())    weight[i][j][k] -= learnRate * normCoeff[type] * deltaWeights[i+1][k] * output[i][j];
                        else if(ui->deltaRadioButton->isChecked())  weight[i][j][k] += learnRate * normCoeff[type] * output[i][j] * ((type == k) - output[i+1][k]); // numOfLayers = 2 in this case
                    }
                }
            }
        }

        ++epoch;
        //count error
        currentError /= NumberOfVectors;
        currentError = sqrt(currentError);
        if(!autoFlag) cout << "epoch = " << epoch << "\terror = " << currentError << endl;
        this->ui->currentErrorDoubleSpinBox->setValue(currentError);        
//        if(epoch == 50) break;
    }


//    cout << "learning ended " << epoch << " epoches" << "\terror = " << doubleRound(currentError, 3) << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec"  << endl;



    delete []mixNum;
    for(int i = 0; i < numOfLayers; ++i)
    {
        delete []deltaWeights[i];
        delete []output[i];
    }
    delete []deltaWeights;
    delete []output;
    delete []normCoeff;
}



bool Net::ClassificateVector(int &vecNum)
{
    double * outputClass = new double [NumOfClasses];

    int type = int(matrix[vecNum][NetLength+1]);


    double ** output = new double * [numOfLayers]; //data and global output together
    for(int i = 0; i < numOfLayers; ++i)
    {
        output[i] = new double [dimensionality[i] + 1];
    }

    for(int j = 0; j < dimensionality[0]; ++j)
    {
        output[0][j] = matrix[vecNum][j];
    }
    output[0][dimensionality[0]] = 1.;

    for(int i = 1; i < numOfLayers; ++i)
    {
        for(int j = 0; j < dimensionality[i]; ++j)
        {
            output[i][j] = 0.;
            for(int k = 0; k < dimensionality[i-1] + 1; ++k) //-1 for prev.layer, +1 for bias
            {
                output[i][j] += weight[i-1][k][j] * output[i-1][k];
            }
            output[i][j] = logistic(output[i][j], temperature);
        }
        output[i][dimensionality[i]] = 1.; //unused for the highest layer
    }
    for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
    {
        outputClass[j] = output[numOfLayers - 1][j];
    }


    for(int i = 0; i < numOfLayers; ++i)
    {
        delete []output[i];
    }
    delete []output;




    bool right = true;
    for(int k = 0; k < NumOfClasses; ++k)
    {
        if(k != type && outputClass[k] >= outputClass[type])
        {
            right = false;
            ++NumberOfErrors[type];
            break;
        }
    }
    delete [] outputClass;
    return right;
}

int Net::getEpoch()
{
    return epoch;
}

double Net::getErrcrit()
{
    return this->critError;
}

void Net::setErrcrit(double a)
{
    this->critError = a;
}

double errorSammon(double ** distOld, double ** distNew, int size)
{
    double sum1_ = 0., sum2_ = 0., ret;
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < i; ++j)
        {
            if(distOld[i][j]  != 0.)
            {
                sum1_ += distOld[i][j];
                sum2_ += (distOld[i][j] - distNew[i][j]) * (distOld[i][j] - distNew[i][j])/distOld[i][j];
            }
        }
    }
    ret = sum2_/double(sum1_);

    return ret;
}

void refreshDistAll( double ** dist, double ** coordsNew, int size)
{
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < i; ++j)
        {
            dist[i][j] = distance(coordsNew[i], coordsNew[j], 2);
            dist[j][i] = dist[i][j]; //distance(coordsNew[i], coordsNew[j], 2);
        }
    }


}

void refreshDist( double ** dist, double ** coordsNew, int size, int i)
{
    for(int j = 0; j < size; ++j)
    {
        dist[i][j] = distance(coordsNew[i], coordsNew[j], 2);
        dist[j][i] = dist[i][j];
    }
}

void countGradient(double ** coords, double ** distOld, double ** distNew, int size, double * gradient)
{
    double delta = 0.05;
    double tmp1;

    for(int i = 0; i < size; ++i)
    {
        tmp1 = coords[i][0];
        coords[i][0] = tmp1 + delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2 * i] = errorSammon(distOld, distNew, size);

        coords[i][0] = tmp1 - delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2 * i] -= errorSammon(distOld, distNew, size);
        gradient[2 * i] /= delta;
        coords[i][0] = tmp1;


        tmp1 = coords[i][1];
        coords[i][1] = tmp1 + delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2 * i+1] = errorSammon(distOld, distNew, size);

        coords[i][1] = tmp1 - delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2 * i+1] -= errorSammon(distOld, distNew, size);
        gradient[2 * i+1] /= delta;
        coords[i][1] = tmp1;
        refreshDist(distNew, coords, size, i);
    }
}

void Net::moveCoordsGradient(double ** coords, double ** distOld, double ** distNew, int size)
{

    //gradient method
    double * gradient = new double [size * 2];
    double lambda = ui->lambdaDoubleSpinBox->value(); //how to estimate - add into UI
    double num = 0;

    double errorBackup;

    countGradient(coords, distOld, distNew, size, gradient);
    int j = 0;
    while(1)
    {
        errorBackup = errorSammon(distOld, distNew, size);

        for(int i = 0; i < size; ++i)
        {
            coords[i][0] -= gradient[2 * i] * lambda;
            coords[i][1] -= gradient[2 * i+1] * lambda;
        }
        refreshDistAll(distNew, coords, size);


        if(errorBackup > errorSammon(distOld, distNew, size))
        {
            num = j;
        }
        else
        {
            for(int i = 0; i < size; ++i)
            {
                coords[i][0] += gradient[2 * i] * lambda;
                coords[i][1] += gradient[2 * i+1] * lambda;
            }
            refreshDistAll(distNew, coords, size);
            break;
        }
        ++j;
        if(j%5 == 4) lambda *= 2;
    }
    cout << j  << endl;
}

void moveCoords(double ** coords, double ** distOld, double ** distNew, int size, int i, int iterationsCount)
{
    int num = 10. * (iterationsCount/5. + 1.);
    double coordTemp1, coordTemp2;

    coordTemp1 = coords[i][0];
    coordTemp2 = coords[i][1];

    double tmp1,tmp2;


    tmp1 = errorSammon(distOld, distNew, size);

    for(int j = 0; j < num; ++j)
    {
        tmp2 = coords[i][0];
        coords[i][0] = 0. + j * coordTemp1 * 2./(num-1) ; //from 0 to 1.5 * coords

        refreshDist(distNew, coords, size, i);
        if(errorSammon(distOld, distNew, size) > tmp1)
        {
            coords[i][0] = tmp2;
        }
        else
        {
            tmp1 = errorSammon(distOld, distNew, size);
        }
        refreshDist(distNew, coords, size, i);
    }

    tmp1 = errorSammon(distOld, distNew, size);
    for(int j = 0; j < num; ++j)
    {
        tmp2 = coords[i][1];
        coords[i][1] = 0. + j * coordTemp2 * 2./(num-1);

        refreshDist(distNew, coords, size, i);
        if(errorSammon(distOld, distNew, size) > tmp1)
        {
            coords[i][1] = tmp2;
        }
        else
        {
            tmp1 = errorSammon(distOld, distNew, size);
        }
        refreshDist(distNew, coords, size, i);
    }

}

//Sammon's projection
void Net::Sammon(double ** distArr, int size, int * types)
{
    srand(time(NULL));
    double ** distNew = new double * [size];
    for(int i = 0; i < size; ++i)
    {
        distNew[i] = new double [size];
    }

    coords = new double * [size];
    for(int i = 0; i < size; ++i)
    {
        coords[i] = new double [3]; //x and y - coords, type-color
    }

    //set random coordinates
    for(int i = 0; i < size; ++i)
    {
        coords[i][0] = (rand()%20 + 10.);
        coords[i][1] = (rand()%20 + 10.);
        coords[i][2] = types[i];
    }

//    double maxDist = 0.;
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            distNew[i][j] = distance(coords[i], coords[j], 2); //plane euclid distance
//            maxDist = fmax(distNew[i][j], maxDist);
        }
    }
//    cout << "maxDistanceRandomSet = " << maxDist << endl;




    //coordinate method
    double tmpError1 = 0.;
    double tmpError2 = errorSammon(distArr, distNew, size);
    int iterationsCount = 0;

    int * mixNum = new int [size];
    for(int i = 0; i < size; ++i)
    {
        mixNum[i] = i;
    }
    int a1, a2, temp;
    for(int i = 0; i < size * 5; ++i)
    {
        a1 = rand()%size;
        a2 = rand()%size;
        temp = mixNum[a1];
        mixNum[a1] = mixNum[a2];
        mixNum[a2] = temp;
    }



    if(ui->optimizationMethodComboBox->currentIndex() == 0)
    {
        cout << "start coordinate method, error = " << tmpError2 << endl;
        while(1)
        {
            tmpError1 = tmpError2; //error before
            for(int i = 0; i < size; ++i)
            {
                //            cout << iterationsCount << "   " << i  << endl;
                moveCoords(coords, distArr, distNew, size, mixNum[i], iterationsCount);
                refreshDist(distNew, coords, size, mixNum[i]);
            }
            tmpError2 = errorSammon(distArr, distNew, size);

            cout << iterationsCount << " error = " << tmpError2 << endl;
            ++iterationsCount;
            if(tmpError2 < 0.05 || (fabs(tmpError1-tmpError2)/tmpError1)<0.002) break;
        }
        cout << "NumOfIterations = " << iterationsCount << " error = " << tmpError2 << endl;
    }

    if(ui->optimizationMethodComboBox->currentIndex() == 1)
    {

        cout << "start gradient method, error = " << tmpError2 << endl;
        while(1)
        {
            tmpError1 = tmpError2; //error before
            moveCoordsGradient(coords, distArr, distNew, size);
            tmpError2 = errorSammon(distArr, distNew, size);

            cout << iterationsCount << " error = " << tmpError2 << endl;
            ++iterationsCount;
            if(tmpError2 < 0.05 || (fabs(tmpError1-tmpError2)/tmpError1)<0.002) break;
        }
        cout << "NumOfIterations = " << iterationsCount << " error = " << tmpError2 << endl;
    }




    delete [] mixNum;




    for(int i = 0; i < size; ++i)
    {
        delete [] distNew[i];
    }
    delete [] distNew;


    QMessageBox::information((QWidget * )this, tr("info"), tr("Sammon projection counted"), QMessageBox::Ok);
}

void Net::drawSammon() //uses coords array
{
    if(coords == NULL)
    {
        cout << "coords array == NULL" << endl;
        QMessageBox::information((QWidget * )this, tr("warning"), tr("Coords array == NULL"), QMessageBox::Ok);
        return;
    }


    //draw the points
    QPixmap pic(1800,1200);
    pic.fill();
    QPainter * painter = new QPainter();
    painter->begin(&pic);

    //axes
    painter->drawLine(QPointF(0, pic.height()/2.), QPointF(pic.width(), pic.height()/2.));
    painter->drawLine(QPointF(pic.width()/2., 0), QPointF(pic.width()/2, pic.height()));

    double minX = 0., minY = 0., maxX = 0., maxY = 0., avX,avY, rangeX, rangeY;
    int rectSize = ui->sizeSpinBox->value();

    double sum1 = 0., sum2 = 0.;
    minX = coords[0][0];
    minY = coords[0][1];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        maxX = fmax(maxX, coords[i][0]);
        maxY = fmax(maxY, coords[i][1]);
        minX = fmin(minX, coords[i][0]);
        minY = fmin(minY, coords[i][1]);
    }
    avX = (minX + maxX)/2.;
    avY = (minY + maxY)/2.;

    rangeX = (maxX - minX)/2.;
    rangeY = (maxY - minY)/2.;


    for(int i = 0; i < NumberOfVectors; ++i)
    {
        sum1 = coords[i][0];
        sum2 = coords[i][1];

//        cout << sum1 << "  " << sum2 << endl;

        painter->setBrush(QBrush("black"));
        painter->setPen("black");

        if(coords[i][2] == 0)
        {
            painter->setBrush(QBrush("blue"));
            painter->setPen("blue");
        }
        if(coords[i][2] == 1)
        {
            painter->setBrush(QBrush("red"));
            painter->setPen("red");
        }
        if(coords[i][2] == 2)
        {
            painter->setBrush(QBrush("green"));
            painter->setPen("green");
        }
        painter->drawRect(QRectF(QPointF(pic.width()/2. + (sum1-avX)/rangeX * pic.width()/2. - rectSize - avX, pic.height()/2. + (sum2-avY)/rangeY * pic.height()/2. - rectSize - avY), QPointF(pic.width()/2. + (sum1-avX)/rangeX * pic.width()/2. + rectSize  - avX, pic.height()/2. + (sum2-avY)/rangeY * pic.height()/2. + rectSize - avY)));

    }

    helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + "Sammon-" + ui->sammonLineEdit->text() + ".jpg";

    cout << helpString.toStdString() << endl;
    pic.save(helpString, 0, 100);

    painter->end();
//    delete [] painter;
    for(int i = 0; i < 3; ++i)
    {
        delete [] coords[i];
    }
    delete [] coords;
    cout << "Sammon projection done" << endl;
    QMessageBox::information((QWidget * )this, tr("info"), tr("Sammon projection drawn"), QMessageBox::Ok);
}

//principal component analisys
void Net::pca()
{
    QTime wholeTime;
    wholeTime.start();
//    spLength - 1 channel
//    spLength * ns = NetLength
//    matrix - matrix of data
//    centeredMatrix - matrix of centered data: matrix[i][j]-=average[j]
//    random quantity is a spectre-vector of spLength dimension
//    there are NumberOfVectors samples of this random quantity
//        similarMatrix - matrix of similarity
//        differenceMatrix - matrix of difference, according to some metric
    //projectedMatrix - vectors with just some PCs left
    cout << "NetLength = " << NetLength << endl;
    cout << "NumberOfVectors = " << NumberOfVectors << endl;
    double ** differenceMatrix = new double * [NumberOfVectors];
    double ** centeredMatrix = new double * [NumberOfVectors];
    double ** pcaMatrix = new double * [NumberOfVectors];


    //i - spectral point
    //j - number of current vector


    for(int j = 0; j < NumberOfVectors; ++j)
    {
        differenceMatrix[j] = new double [NumberOfVectors];
        centeredMatrix[j] = new double [NetLength];
    }
    for(int i = 0; i < NetLength; ++i)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            centeredMatrix[j][i] = matrix[j][i];
        }
    }

    //count covariations
    //count averages
    double * averages = new double [NetLength];
    for(int i = 0; i < NetLength; ++i)
    {
        averages[i] = 0.;
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            averages[i] += centeredMatrix[j][i];
        }
        averages[i] /= NumberOfVectors;
//        cout << i  << "   " << averages[i] << endl; //test - OK
    }

    //centered matrix
    for(int i = 0; i < NetLength; ++i)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            centeredMatrix[j][i] -= averages[i];
        }
    }

    cout << "centeredMatrix counted" << endl;

    //NetLength ~ = 45000
    //NumberOfVectors ~ = 100
    //covariation between different spectra-bins
    double tempDouble;

    QTime initTime;
    initTime.start();
//    dir->mkdir("PCA");
//    FILE * covMatrixFile;
//    for(int i = 0; i < NetLength; ++i)
//    {
//        if((i+1)%50 == 0)
//        {
//            cout << "50 passed, time = " << initTime.elapsed() << " i = " << i  << endl;
//            initTime.restart();
//        }
//        helpString = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "PCA" + QDir::separator() + "covMatrix_" + QString::number(i));
//        covMatrixFile = fopen(helpString.toStdString().c_str(), "w");
//        for(int k = 0; k < NetLength; ++k)
//        {
//            tempDouble = 0.;
//            for(int j = 0; j < NumberOfVectors; ++j)
//            {
//                tempDouble += centeredMatrix[j][i] * centeredMatrix[j][k];
//            }
//            tempDouble /= (NumberOfVectors - 1);
//            fprintf(covMatrixFile, "%lf\n", tempDouble); //covariation between i'th and k'th components of vectors
////            covMatrix[i][k] = tempDouble;
//        }
//        fclose(covMatrixFile);
//    }


//    //test covMatrix symmetric - OK
//    for(int i = 0; i < NetLength; ++i)
//    {
//        for(int k = 0; k < NetLength; ++k)
//        {
//            if(covMatrix[i][k] != covMatrix[k][i]) cout << i  << " " << k << " warning" << endl;
//        }
//    }


    double trace = 0.;
    for(int i = 0; i < NetLength; ++i)
    {
        tempDouble = 0.;
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            tempDouble += centeredMatrix[j][i] * centeredMatrix[j][i];
        }
        tempDouble /= (NumberOfVectors - 1);
        trace += tempDouble;
    }
    cout << "trace covMatrix = " << trace << endl;

//    return;

    //count eigenvalues & eigenvectors of covMatrix
    double * eigenValues = new double [NetLength];
    double ** eigenVectors = new double * [NetLength]; //vector is a coloumn
    for(int i = 0; i < NetLength; ++i)
    {
        eigenVectors[i] = new double [ui->pcaNumberSpinBox->value()];
    }
    double * tempA = new double [NetLength]; //i
    double * tempB = new double [NumberOfVectors];//j
    double sum1, sum2; //temporary help values
    double dF, F;
    int counter;




    int numOfPc;

    cout << "start eigenValues processing" << endl;
    //count eigenValues & eigenVectors
    //matrix
    for(int k = 0; k < NetLength; ++k)
    {
        initTime.restart();
        dF = 1.0;
        F = 1.0;

        //set 1-normalized vector tempA        
        for(int i = 0; i < NetLength; ++i)
        {
            tempA[i] = 1./sqrt(NetLength);
        }
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            tempB[j] = 1./sqrt(NumberOfVectors);
        }


        //approximate P[i] = tempA x tempB;
        counter = 0.;
        while(1) //when stop approximate?
        {
            //countF
            F = 0.;
            for(int i = 0; i < NetLength; ++i)
            {
                for(int j = 0; j < NumberOfVectors; ++j)
                {
                    F += 0.5 * (centeredMatrix[j][i] - tempB[j] * tempA[i]) * (centeredMatrix[j][i] - tempB[j] * tempA[i]);
                }
//                cout << F << " ";
            }
            //count vector tempB
            for(int j = 0; j < NumberOfVectors; ++j)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int i = 0; i < NetLength; ++i)
                {
                    sum1 += centeredMatrix[j][i] * tempA[i];
                    sum2 += tempA[i] * tempA[i];
                }
                tempB[j] = sum1 / sum2;
            }
//            if(k == 0) cout << endl;

            //count vector tempA
            for(int i = 0; i < NetLength; ++i)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int j = 0; j < NumberOfVectors; ++j)
                {
                    sum1 += tempB[j] * centeredMatrix[j][i];
                    sum2 += tempB[j] * tempB[j];
                }
                tempA[i] = sum1 / sum2;
            }

            dF = 0.;
            for(int i = 0; i < NetLength; ++i)
            {
                for(int j = 0; j < NumberOfVectors; ++j)
                {
                    dF += 0.5 * (centeredMatrix[j][i] - tempB[j] * tempA[i]) * (centeredMatrix[j][i] - tempB[j] * tempA[i]);
                }
            }
            dF = (F-dF)/F;
            ++counter;
            if(fabs(dF) < 1e-8 || counter == 300)
            {
                break;
            }
        }
//        cout << k << "   " << counter << endl;

        //edit covMatrix
        for(int i = 0; i < NetLength; ++i)
        {
            for(int j = 0; j < NumberOfVectors; ++j)
            {
                centeredMatrix[j][i] -= tempB[j] * tempA[i];
            }
        }

        //count eigenVectors && eigenValues
        sum1 = 0.;
        sum2 = 0.;
        for(int i = 0; i < NetLength; ++i)
        {
            sum1 += tempA[i] * tempA[i];
        }
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            sum2 += tempB[j] * tempB[j];
        }
        for(int i = 0; i < NetLength; ++i)
        {
            tempA[i] /= sqrt(sum1);
            //test equality of left and right singular vectors
//            if(((tempB[i]-tempA[i])/tempB[i])<-0.05 || ((tempB[i]-tempA[i])/tempB[i])>0.05) cout << k << " " << i  << " warning" << endl;  //till k == 19 - OK
        }

        eigenValues[k] = sum1 * sum2 / double(NumberOfVectors-1.);


        sum1 = 0.;
        for(int j = 0; j <= k; ++j)
        {
            sum1 += eigenValues[j];
        }
//        cout << "Part of dispersion explained = " << sum1 * 100./double(trace) << " %" << endl;

        cout << k+1;
        cout << "\t" << eigenValues[k];
        cout << "\tDisp = " << eigenValues[k]*100./trace;
        cout << "\tTotal = " << sum1 * 100./trace;
        cout << "\ttimeElapsed = " << initTime.elapsed()/1000. << " seconds";
        cout << "\tSVD-iterations = " << counter;
        cout << endl;
        for(int i = 0; i < NetLength; ++i)
        {
            eigenVectors[i][k] = tempA[i]; //1-normalized
        }

        //need a rule
        if(k+1 == ui->pcaNumberSpinBox->value() || sum1/trace >= ui->traceDoubleSpinBox->value())
        {
            cout << "numOfEigenValues = " << k+1 << endl;
            numOfPc = k+1;
            break;
        }

    }
    ui->autpPCAMaxSpinBox->setValue(numOfPc);

    double helpDouble;
    if(0) //eigenVectors length - OK
    {
        for(int i = 0; i < numOfPc; ++i)
        {
            helpDouble = 0.;
            for(int j = 0; j < NetLength; ++j)
            {
                helpDouble += eigenVectors[j][i] * eigenVectors[j][i];
            }
            helpDouble = sqrt(helpDouble);
            cout << i << "'th eigenVector norm\t" << helpDouble << endl;
        }
    }


    if(1) //eigenVectors output
    {
        ofstream eigenVectorsFile;
        helpString = dir->absolutePath() + QDir::separator() + "Help" + QDir::separator() + ExpName + "_pcaEigenVectors.txt";
        eigenVectorsFile.open(helpString.toStdString().c_str());
        for(int k = 0; k < numOfPc; ++k)
        {
            for(int i = 0; i < NetLength; ++i)
            {
                eigenVectorsFile << eigenVectors[i][k] << "\t";
            }
            eigenVectorsFile << "\n";
        }
        eigenVectorsFile.close();
    }


    sum1 = 0.;
    for(int k = 0; k < numOfPc; ++k)
    {
        sum1 += eigenValues[k];
    }
    cout << "Part of dispersion explained = " << sum1 * 100./double(trace) << " %" << endl;
    cout << "Number of Components = " << numOfPc << endl;

    //memory for pcaProjections
    for(int j = 0; j < NumberOfVectors; ++j)
    {
        pcaMatrix[j] = new double [numOfPc];
    }

    //return centeredMatrix
    for(int j = 0; j < NumberOfVectors; ++j)
    {
        for(int i = 0; i < NetLength; ++i)
        {
            centeredMatrix[j][i] = matrix[j][i] - averages[i];
        }
    }

    for(int j = 0; j < NumberOfVectors; ++j)
    {
        for(int k = 0; k < numOfPc; ++k)
        {
            pcaMatrix[j][k] = 0.;
            for(int i = 0; i < NetLength; ++i)
            {
                pcaMatrix[j][k] += centeredMatrix[j][i] * eigenVectors[i][k];
            }
        }
    }

    if(0)
    {
        double ** pcaMatrixTrans;
        matrixCreate(&pcaMatrixTrans, numOfPc, NumberOfVectors);
        matrixTranspose(pcaMatrix, NumberOfVectors, numOfPc, &pcaMatrixTrans);
        for(int k = 0; k < numOfPc; ++k)
        {
            for(int j = 0; j < numOfPc; ++j)
            {
//                if(k!=j && covariance(pcaMatrixTrans[k], pcaMatrixTrans[j], NumberOfVectors) > 100.) cout << k << j << endl;
            }
            cout << "cov " << k << " = " << covariance(pcaMatrixTrans[k], pcaMatrixTrans[k], NumberOfVectors) << endl;
        }
    }




    FILE * pcaFile;
    //count reduced Data - first some PC
    for(int j = 0; j < NumberOfVectors; ++j) //i->j
    {
        helpString = dirBC->absolutePath();
        helpString += QString(QDir::separator()) + "SpectraSmooth";
        helpString += QString(QDir::separator()) + "PCA";
        helpString += QString(QDir::separator()) + FileName[j];
        pcaFile = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
        for(int k = 0; k < numOfPc; ++k) //j->k
        {
            fprintf(pcaFile, "%lf\n", double(10. * pcaMatrix[j][k])); //PC coefficients
        }
        fclose(pcaFile);
    }


    //count distances between different spectre-vectors (projections on first numOfPc PCs)
    for(int h = 0; h < NumberOfVectors; ++h)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            if(ui->sammonComboBox->currentIndex() == 0) differenceMatrix[h][j] = distance(matrix[h], matrix[j], NetLength);  //wet data
            else if(ui->sammonComboBox->currentIndex() == 1) differenceMatrix[h][j] = distance(pcaMatrix[h], pcaMatrix[j], numOfPc); //by some PC
        }
    }


    if(ui->pcaCheckBox->isChecked())
    {
        int * vecTypes = new int [NumberOfVectors];
        for(int i = 0; i < NumberOfVectors; ++i)
        {
            vecTypes[i] = matrix[i][NetLength+1];
        }

        ui->sammonLineEdit->setText("pca");

        Sammon(differenceMatrix, NumberOfVectors, vecTypes);

        delete [] vecTypes;
        ui->sammonLineEdit->clear();;
    }

    if(ui->kohonenCheckBox->isChecked())
    {

        //count average projection on 2 main principal components
//        numOfPc = 5; //maybe was test
        double * avProj = new double [numOfPc];
        for(int j = 0; j < numOfPc; ++j)
        {
            avProj[j] = 0.;
            for(int i = 0; i < NumberOfVectors; ++i)
            {
                avProj[j] += centeredMatrix[j][i];

            }
            avProj[j]/=NumberOfVectors;
            cout << "averageProjection[" << j  << "] = " << avProj[j] << endl;
        }
        ui->sammonLineEdit->setText("kohonen");
        Kohonen(matrix, eigenVectors, avProj, NumberOfVectors, NetLength);
        delete [] avProj;
        ui->sammonLineEdit->clear();
    }



    cout << "end" << endl;
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        delete [] differenceMatrix[i];
        delete [] pcaMatrix[i];
        delete [] centeredMatrix[i];
    }


    delete [] centeredMatrix;
    delete [] pcaMatrix;
    delete [] differenceMatrix;

    delete [] averages;
    delete [] eigenValues;
    for(int i = 0; i < ui->pcaNumberSpinBox->value(); ++i)
    {
        delete eigenVectors[i];
    }
    delete [] eigenVectors;
    delete [] tempA;
    delete [] tempB;

    cout << "whole time elapsed " << wholeTime.elapsed()/1000. << " sec" << endl;

//    delete painter;

//    if(!this->autoFlag) QMessageBox::information((QWidget * )this, tr("info"), tr("PCA drawn"), QMessageBox::Ok);
}

double Net::thetalpha(int bmu_, int j_, int step_, double ** arr, int length_)
{
    //approximately 20-30 steps
    double neighbour = 0., alpha = 0., temp = 0., sigma = 0.;
    alpha = 5./(step_+5.);
    sigma = 10./(step_+10.);
    neighbour = exp(- distance(arr[bmu_], arr[j_], length_) * distance(arr[bmu_], arr[j_], length_) / (2. * sigma * sigma));
    temp = neighbour * alpha;
//    if(bmu_ == j_) temp = 1.;
    return temp;
}

//length - dimension of input vectors
void Net::Kohonen(double ** input, double ** eigenVects, double * averageProjection, int size, int length)
{
    srand(time(NULL));
//    int numOfComponents = SizeOfArray(averageProjection); //=2
//    cout << "size = " << size << " SizeOfArray(input) = " << SizeOfArray(input) << endl;

//    int sqrLen = floor(sqrt(size)) + 1;
    int sqrLen = 10;
    int numOfNodes = sqrLen * sqrLen;

    //set the coords
    double ** coordsKohonen = new double * [numOfNodes];
    for(int i = 0; i < numOfNodes; ++i)
    {
        coordsKohonen[i] = new double [2];

        coordsKohonen[i][0] = (i%sqrLen);
        coordsKohonen[i][1] = floor(i/sqrLen);
    }

    //set the weights
    double a1,a2;
    double ** weightsKohonen = new double * [numOfNodes];
    for(int i = 0; i < numOfNodes; ++i)
    {
        weightsKohonen[i] = new double [length+1]; //+ type

        //+- 10%
        a1 = (0.9 + 0.05 * (rand()%41)/10.);
        a2 = (0.9 + 0.05 * (rand()%41)/10.);
        for(int j = 0; j < length; ++j)
        {
            weightsKohonen[i][j] = averageProjection[0] * a1 * eigenVects[j][0]  + averageProjection[1] * a2 * eigenVects[j][1];
//            cout << weightsKohonen[i][j] << endl; //OK
        }
    }

    int * mixNum = new int [size];
    int tmp1, tmp2, buf;
    double minDist;
    int * bmu = new int [size]; //num of bet match neuron

    double * distances = new double [numOfNodes];
    for(int i = 0; i < numOfNodes; ++i)
    {
        distances[i] = 0.;
    }

    double error = 0., tempError = 0.;

    int step = 0;
    while(1)
    {

        //mix the order
        for(int i = 0; i < size; ++i)
        {
            mixNum[i] = i;
        }


        for(int i = 0; i < 5 * size; ++i)
        {
            tmp1 = rand()%(size);
            tmp2 = rand()%(size);
            buf = mixNum[tmp2];
            mixNum[tmp2] = mixNum[tmp1];
            mixNum[tmp1] = buf;
        }

        //for every inputVector
        for(int i = 0; i < size; ++i)
        {

            //find BMU
            for(int j = 0; j < numOfNodes; ++j)
            {
                distances[j] = distance(input[mixNum[i]], weightsKohonen[j], length);
                if(j == 0)
                {
                    minDist = distances[j]; bmu[mixNum[i]] = j;
                }
                else
                {
                    if(distances[j] < minDist)
                    {
                        minDist = distances[j];
                        bmu[mixNum[i]] = j;
                        weightsKohonen[j][length] = input[mixNum[i]][NetLength+1];
                    }
                }
            }


            for(int j = 0; j < numOfNodes; ++j)
            {
                //adjust the weights
                for(int k = 0; k<length; ++k)
                {
                     weightsKohonen[j][k] += thetalpha(bmu[mixNum[i]], j, step, coordsKohonen, 2) * (input[mixNum[i]][k] - weightsKohonen[j][k]);
                }

                //adjust the coords???
                coordsKohonen[j][0]  += 0;// theta(bmu[mixNum[i]], j, step) * alpha(step) * (input[mixNum[i]][k] - weightsKohonen[j][k]);
                coordsKohonen[j][1]  += 0;// theta(bmu[mixNum[i]], j, step) * alpha(step) * (input[mixNum[i]][k] - weightsKohonen[j][k]);
            }
        }
        tempError = error;
        error = 0.;
        //count error
        for(int i = 0; i < size; ++i)
        {
            error += distance(input[i], weightsKohonen[bmu[i]], length);
        }
        error/=size;
        cout << step << " " << "error = " << error << endl;



        ++step;
        if(step == 100 || error<0.01 || fabs(tempError-error)/error <0.002) break;
    }



    double ** distKohonen = new double * [numOfNodes];
    for(int i = 0; i < numOfNodes; ++i)
    {
        distKohonen[i] = new double [numOfNodes];
        for(int j = 0; j < numOfNodes; ++j)
        {
            distKohonen[i][j] = distance(weightsKohonen[i], weightsKohonen[j], length);
//            cout << distKohonen[i][j] << endl; //OK
        }
    }

    int * typesKohonen = new int [numOfNodes];
    for(int i = 0; i < numOfNodes; ++i)
    {
        typesKohonen[i] = weightsKohonen[i][length];
        if(typesKohonen[i]  != 0 && typesKohonen[i]  != 1 && typesKohonen[i]  != 2)
        {
            cout << "typeKohonen = " << typesKohonen[i] << endl;
        }
    }
    Sammon(distKohonen, numOfNodes, typesKohonen);


    delete [] mixNum;
    delete [] distances;
    delete [] bmu;
    for(int i = 0; i < numOfNodes; ++i)
    {
        delete [] distKohonen[i];
        delete [] weightsKohonen[i];
        delete [] coordsKohonen[i];
    }
    delete [] distKohonen;
    delete [] coordsKohonen;
    delete [] weightsKohonen;
    delete [] typesKohonen;



}

void Net::neuronGas()
{
}

void Net::SVM()
{
    helpString = dir->absolutePath() + QDir::separator() + "PA" + QDir::separator() + "output1";
    FILE * out = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    fclose(out);
//    QString spectraDir = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth"));
    QString spectraDir = QFileDialog::getExistingDirectory(this, tr("Choose spectra dir"), dir->absolutePath());
    if(spectraDir.isEmpty()) spectraDir = QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "SpectraSmooth");
    if(spectraDir.isEmpty())
    {
        cout << "spectraDir for SVM is empty" << endl;
        return;
    }
    MakePa * mkPa = new MakePa(spectraDir, ExpName, ns, left, right, spStep);

    for(int i = 0; i < ui->numOfPairsBox->value(); ++i)
    {
        mkPa->makePaSlot();

        helpString = dir->absolutePath() + QDir::separator() + "PA";
        helpString.prepend("cd ");
        helpString.append(" && svm-train -t " + QString::number(ui->svmKernelSpinBox->value()) + " svm1 && svm-predict svm2 svm1.model output >> output1");
        system(helpString.toStdString().c_str());

        helpString = dir->absolutePath() + QDir::separator() + "PA";
        helpString.prepend("cd ");
        helpString += " && svm-train -t " + QString::number(ui->svmKernelSpinBox->value()) + " svm2 && svm-predict svm1 svm2.model output >> output1";
        system(helpString.toStdString().c_str());
    }


    helpString = dir->absolutePath() + QDir::separator() + "PA" + QDir::separator() + "output1";

    double helpDouble, average = 0.;

    QFile file(helpString);
    if(!file.open(QIODevice::ReadOnly)) return;
    int lines = 0;
    while(!file.atEnd())
    {
        helpString = file.readLine();
        if(!helpString.contains(QRegExp("[% = ]"))) break;
        helpString = helpString.split(QRegExp("[% = ]"), QString::SkipEmptyParts)[1]; //generality [1]
        helpDouble = helpString.toDouble();
        average += helpDouble;
        ++lines;
    }
    average /= lines;
    cout << average << endl;
    file.close();

//    helpString = dir->absolutePath();
//    helpString.append(QDir::separator() + "PA" + QDir::separator() + "output1");
//    fstream in;
//    in.open(QDir::toNativeSeparators(helpString).toStdString().c_str(), fstream::in);

//    for(int i = 0; i < ui->numOfPairsBox->value() * 2; ++i)
//    {
//        in.getline(helpCharArr, 200);
//        sscanf(helpCharArr, "Accuracy = %lf", &helpDouble);
//        average += helpDouble;
//    }
//    average /= ui->numOfPairsBox->value() * 2;
//    cout << average << endl;
//    in.close();

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath() + QDir::separator() + "results.txt").toStdString().c_str(), "a+");
    fprintf(res, "\nSVM\t");
    fprintf(res, "%.2lf %%\n", average);
    fclose(res);

    delete mkPa;
}

void Net::optimizeChannelsSet()
{
    int tempItem;
    int tempIndex;

    //test
//    tempItem = channelsSet[0];
//    channelsSet.remove(0);
//    channelsSetExclude.push_back(tempItem);
//    cout << "current set:" << "\n";
//    for(int i = 0; i < channelsSet.length(); ++i)
//    {
//        cout << channelsSet[i] << "  ";
//    }
//    cout << endl;
//    cout << "exclude set:" << "\n";
//    for(int i = 0; i < channelsSetExclude.length(); ++i)
//    {
//        cout << channelsSetExclude[i] << "  ";
//    }
//    cout << endl;




//    NetLength = spLength * (NetLength/spLength - 1);
//    dimensionality[0] = NetLength;
//    autoClassificationSimple();
//    cout << "classified\t" << averageAccuracy << endl << endl;



    spLength = NetLength/ns;
    channelsSetExclude.clear();
    autoClassificationSimple();
    double tempAccuracy = averageAccuracy;


    cout << "Init Accuracy = " << averageAccuracy << endl;
    bool foundFlag = false;

    channelsSet.clear();
    for(int i = 0; i < 19; ++i)
    {
        channelsSet << i;
    }
    QList <int> neededChannels;
    neededChannels.clear();


    while(1)
    {

        cout << endl << "optimization iteration" << endl;
        foundFlag = false;
        NetLength = spLength * (NetLength/spLength - 1);
        dimensionality[0] = NetLength;
        cout << "Optimizing: NetLength = " << NetLength << endl;
        cout << "channelsSet.length() = " << channelsSet.length() << endl;



        tempIndex = -1;
        for(int i = tempIndex+1; i < channelsSet.length(); ++i)
        {

            tempItem = channelsSet[i];
            if(neededChannels.contains(tempItem))
            {
                cout << "needed channel[" << i << "] = " << tempItem << endl;
                continue;
            }

            channelsSet.removeAt(i);
            channelsSetExclude.push_back(tempItem);

            //try classify w/o tempitem
            autoClassificationSimple();
            channelsSet.insert(i, tempItem);
            channelsSetExclude.removeLast();

            cout << "classified ch[" << i << "] = " << tempItem << "  accuracy = " << averageAccuracy << endl << endl;


            if(averageAccuracy > tempAccuracy + 0.6)
            {
                tempAccuracy = averageAccuracy;
                tempIndex = i;
                foundFlag = true;
                break;
            }
            else if(averageAccuracy > tempAccuracy)
            {
                tempAccuracy = averageAccuracy;
                tempIndex = i;
                foundFlag = true;
            }
            else if(averageAccuracy < tempAccuracy - 1.5)
            {
                neededChannels << tempItem;
                cout << "new needed channel[" << i << "] = " << tempItem << endl;
            }

        }

        if(foundFlag)
        {
            cout << "removed channel " << channelsSet[tempIndex] << endl;
            cout << "current accuracy " << tempAccuracy << endl;

            channelsSetExclude.push_back(channelsSet[tempIndex]);
            channelsSet.removeAt(tempIndex);

        }
        else
        {
            NetLength = spLength * (NetLength/spLength + 1);
            dimensionality[0] = NetLength;
            break;
        }
    }


    helpString = dir->absolutePath() + "/optimalChannels.txt";
    outStream.open(helpString.toStdString().c_str(), ios_base::app);

    helpString = ExpName;
    helpString += "\r\nNumOfChannels " + QString::number(channelsSet.length()) + "\n";
    for(int i = 0; i < channelsSet.length(); ++i)
    {
        helpString += QString::number(channelsSet[i] + 1) + "\t"; //count from 1
    }
    helpString += "\n";
    for(int i = 0; i < channelsSet.length(); ++i)
    {
        helpString += QString(coords::lbl[channelsSet[i]]) + "\t";
    }
    helpString += "\n";

    autoFlag = 0;

    outStream << helpString.toStdString() << endl;
    outStream.close();
//    QMessageBox::information((QWidget * )this, tr("Optimization results"), helpString, QMessageBox::Ok);
}

void Net::adjustParamsGroup2(QAbstractButton * but)
{
    if(but->text().contains("Bayes", Qt::CaseInsensitive))
    {
        ui->highLimitSpinBox->setValue(400);
        ui->lowLimitSpinBox->setValue(300);
        ui->epochSpinBox->setValue(500);
        ui->rdcCoeffSpinBox->setValue(0.05);
    }
    else
    {
        ui->highLimitSpinBox->setValue(130);
        ui->lowLimitSpinBox->setValue(80);
        ui->epochSpinBox->setValue(250);
        ui->rdcCoeffSpinBox->setValue(5.);
    }
}
