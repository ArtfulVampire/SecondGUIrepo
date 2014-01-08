#include "net.h"
#include "ui_net.h"
//#include "mainwindow.h"


Net::Net(QDir *dir_, int ns_, int left_, int right_, double spStep_, QString ExpName_) :
    ui(new Ui::Net)
{
    paFileBC="";
    ui->setupUi(this);
    dir = new QDir;
    dir->cd(dir_->absolutePath());
    dirBC = new QDir;
    dirBC->cd(dir_->absolutePath());


    this->setWindowTitle("Net");

    ExpName = ExpName_;

    epoch=150;


    log = fopen(dir->absolutePath().append(QDir::separator()).append("log.txt").toStdString().c_str(),"w");

    left = left_;
    right = right_;
    spLength = right_ - left_ + 1;
    spStep = spStep_;
    ns = ns_;

    cout<<"left = "<<left<<endl;
    cout<<"right = "<<right<<endl;
    cout<<"spStep = "<<spStep<<endl;
    cout<<"ns = "<<ns<<endl;
    cout<<"spLength = "<<spLength<<endl;

//    cout<<"spLength="<<spLength<<endl;
    stopFlag=0;
    numTest=0;
    numOfTall = 0;

    NumberOfVectors=-1;
    loadPAflag = 0;

    helpCharArr = new char [200];


    QButtonGroup *group1, *group2;
    group1 = new QButtonGroup();
    group1->addButton(ui->leaveOneOutRadioButton);
    group1->addButton(ui->crossRadioButton);
    group2 = new QButtonGroup();
    group2->addButton(ui->realsRadioButton);
    group2->addButton(ui->windowsRadioButton);
    group2->addButton(ui->pcRadioButton);

    ui->crossRadioButton->setChecked(true);
    ui->realsRadioButton->setChecked(true);

    ui->tempBox->setValue(10);
    ui->tempBox->setSingleStep(10);
    ui->errorBox->setValue(0.10);
    ui->errorBox->setSingleStep(0.01);
    ui->errorBox->setDecimals(8);
    ui->learnRateBox->setValue(0.1);
    ui->learnRateBox->setSingleStep(0.05);
    ui->epochSpinBox->setMaximum(500);
    ui->epochSpinBox->setSingleStep(50);
    ui->epochSpinBox->setValue(250);
    ui->numOfPairsBox->setMaximum(100);
    ui->numOfPairsBox->setValue(20);
    ui->rdcCoeffSpinBox->setMaximum(50);
    ui->rdcCoeffSpinBox->setMinimum(1);
    ui->rdcCoeffSpinBox->setValue(10);


    ui->pcaNumberSpinBox->setValue(15);
    ui->traceDoubleSpinBox->setMaximum(1.0);
    ui->traceDoubleSpinBox->setMinimum(0.05);
    ui->traceDoubleSpinBox->setSingleStep(0.01);
    ui->traceDoubleSpinBox->setValue(0.9);
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

    ui->sizeSpinBox->setValue(6);
    paint = new QPainter;
    tempEvent = new QTempEvent;

//    myThread.start();
    QObject::connect(this, SIGNAL(destroyed()), &myThread, SLOT(quit()));


    QObject::connect(ui->loadNetButton, SIGNAL(clicked()), this, SLOT(readCfg()));

    QObject::connect(ui->loadWtsButton, SIGNAL(clicked()), this, SLOT(loadWts()));

    QObject::connect(ui->loadPaButton, SIGNAL(clicked()), this, SLOT(PaIntoMatrix()));

    QObject::connect(ui->learnButton, SIGNAL(clicked()), this, SLOT(LearnNet()));

    QObject::connect(ui->testAllButton, SIGNAL(clicked()), this, SLOT(tall()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopActivity()));

    QObject::connect(ui->saveWtsButton, SIGNAL(clicked()), this, SLOT(saveWts()));

    QObject::connect(ui->testButton, SIGNAL(clicked()), this, SLOT(test()));

    QObject::connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));

    QObject::connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));

    QObject::connect(ui->drawWtsButton, SIGNAL(clicked()), this, SLOT(drawWts()));

    QObject::connect(ui->leaveOneOutButton, SIGNAL(clicked()), this, SLOT(leaveOneOut()));

    QObject::connect(ui->neuronGasPushButton, SIGNAL(clicked()), this, SLOT(neuronGas()));

    QObject::connect(ui->pcaPushButton, SIGNAL(clicked()), this, SLOT(pca()));

    QObject::connect(ui->drawSammonPushButton, SIGNAL(clicked()), this, SLOT(drawSammon()));

    QObject::connect(ui->autoClassButton, SIGNAL(clicked()), this, SLOT(autoClassificationSimple()));

    QObject::connect(ui->autoPCAButton, SIGNAL(clicked()), this, SLOT(autoPCAClassification()));

    QObject::connect(ui->drawWindowsButton, SIGNAL(clicked()), this, SLOT(drawWindows()));

    QObject::connect(ui->clearSetsButton, SIGNAL(clicked()), this, SLOT(clearSets()));

    QObject::connect(ui->svmPushButton, SIGNAL(clicked()), this, SLOT(SVM()));


    this->setAttribute(Qt::WA_DeleteOnClose);
}

Net::~Net()
{
//    delete ui;
//    delete dir;
//    delete dirBC;

//    for(int i=0; i<NumberOfVectors; ++i)
//    {
//        delete []matrix[i];
//    }
//    delete []matrix;


//    for(int i=0; i<NumberOfVectors; ++i)
//    {
//        delete []FileName[i];
//    }
//    delete []FileName;

    myThread.quit();
    myThread.wait();

}



double logistic(double &x, double t)
{
    if( x >   37.*t )  return 1.;
    if( x < -115.*t )  return 0.;
    return 1. / ( 1. + exp(-x/t) );
}

QString rightNumber2(int input)
{
    QString hlp1="", hlp2;
    hlp2.setNum(input);
    if(input<10) hlp1.append("000").append(hlp2);
    if((input>=10)&&(input<100)) hlp1.append("00").append(hlp2);
    if(input>=100 && input<1000) hlp1.append("0").append(hlp2);
    if(input>=1000 && input<10000) hlp1.append(hlp2);
    if(input>=10000 && input<100000) hlp1.append(hlp2);
    return hlp1;
}

//void Net::customEvent(QEvent *ev)
bool Net::event(QEvent *ev)
{
    if(ev->type() != 1200)
    {
        return QWidget::event(ev);
    }
    else
    {
        QTempEvent * tempEvent = static_cast<QTempEvent*>(ev);
        if(tempEvent->value() >= 90.)
        {
            cout<<"CRIT temp = "<<tempEvent->value()<<endl;
            sleep(2);///////////////////////////////////////////////////////////////wrong!!!!!!!1111111111
//            QApplication::sendEvent(this, tempEvent);
            return true;
        }
        else
        {
            cout<<"curr temp = "<<tempEvent->value()<<endl;
            return false;
        }
    }
}

void Net::mousePressEvent(QMouseEvent *event)
{
//    cout<<"global(X,Y)=("<<event->globalX()<<","<<event->globalY()<<")"<<endl;
//    cout<<"(X,Y)=("<<event->x()<<","<<event->y()<<")"<<endl;
//    cout<<"pos(x,y)=("<<ui->clearSetsLabel->pos().x()<<","<<ui->clearSetsLabel->pos().y()<<")"<<endl;
//    cout<<"size(w,h)=("<<ui->clearSetsLabel->width()<<","<<ui->clearSetsLabel->height()<<")"<<endl;

    if(event->x() > ui->clearSetsLabel->pos().x() && event->x() < ui->clearSetsLabel->pos().x() + ui->clearSetsLabel->width() && event->y() > ui->clearSetsLabel->pos().y() && event->y() < ui->clearSetsLabel->pos().y() + ui->clearSetsLabel->height() )
    {
//        cout<<"aimed!"<<endl;
        this->mouseShit = double(event->x() - ui->clearSetsLabel->pos().x())/double(ui->clearSetsLabel->width());
    }
    else
    {
//        cout<<"fail"<<endl;
    }
}

double Net::setPercentageForClean()
{
    cout<<"set for clean"<<endl;
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
    if(ui->realsRadioButton->isChecked())
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
        autoClassification(helpString);
    }
    else if(ui->windowsRadioButton->isChecked())
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows"));
        autoClassification(helpString);
    }
    else
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraPCA"));
        autoClassification(helpString);
    }
}

void Net::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

void Net::autoClassification(QString spectraDir)
{
    if(loadPAflag!=1)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No CFG-file loaded yet"), QMessageBox::Ok);
        return;
    }
    if(log == NULL)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("log.txt"));
        log = fopen(helpString.toStdString().c_str(),"w");
        if(log == NULL)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open log file to write"), QMessageBox::Ok);
            return;
        }
    }

    bool tempBool = autoFlag;
    autoFlag = 1;
    int numOfPairs = ui->numOfPairsBox->value();


    MakePa *mkPa = new MakePa(spectraDir, ExpName, ns, left, right, spStep);

    cout<<spectraDir.toStdString()<<endl;

    mkPa->setRdcCoeff(ui->rdcCoeffSpinBox->value());
    mkPa->setNumOfClasses(NumOfClasses);

    QString typeString;
    if(spectraDir.contains("windows", Qt::CaseInsensitive))
    {
        typeString = "_wnd";
    }
    else if(spectraDir.contains("pca", Qt::CaseInsensitive))
    {
        typeString = "_pca";
    }
    else
    {
        typeString = "";
    }
    cout << typeString.toStdString()<<endl;

    for(int i=0; i<numOfPairs; ++i)
    {
        cout<<i<<" iteration in process"<<endl;
        //make PA
        mkPa->makePaSlot();

        if(ui->crossRadioButton->isChecked())
        {
            helpString = "1" + typeString;

            PaIntoMatrixByName(helpString);

            reset();
            LearnNet();
            helpString = "2" + typeString;
            PaIntoMatrixByName(helpString);

            tall();
            reset();
            LearnNet();
            helpString = "1" + typeString;
            PaIntoMatrixByName(helpString);
            tall();
        }
        else if(ui->leaveOneOutRadioButton->isChecked())
        {
            helpString = "all" + typeString;
            PaIntoMatrixByName(helpString);
            leaveOneOut();
        }
//        qApp->processEvents();

    }
    //leaveOneOut
    if(ui->leaveOneOutRadioButton->isChecked())
    {
        numOfTall = ui->numOfPairsBox->value();
    }

    averageClassification();
    mkPa->close();
    delete mkPa;
    autoFlag = tempBool;

    if(autoFlag == 0) QMessageBox::information((QWidget*)this, tr("Info"), tr("Auto classification done"), QMessageBox::Ok);


}

void Net::autoPCAClassification()
{
    autoFlag = 1;
    cfg * config;
//    MakePa * mkPa;

//    helpString = QDir::toNativeSeparators(dirBC->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
//    MakePa * mkPaW;

    int nsBC = ns;
    int leftBC = left;
    int rightBC = right;
    double spStepBC = spStep;


//    ui->numOfPcSpinBox->setValue(30); //max count PCA
//    pca();

    for(int i=ui->autpPCAMaxSpinBox->value(); i>=ui->autoPCAMinSpinBox->value(); i-=ui->autoPCAStepSpinBox->value())
    {
        cout<<"numOfPc = "<<i<<" started"<<endl;
        config = new cfg(dirBC, 1, i, ui->errorBox->value(), ui->learnRateBox->value(), "pca");
        config->makeCfg();
        config->close();
        if(config != NULL) delete config;

        helpString = QDir::toNativeSeparators(dirBC->absolutePath().append(QDir::separator()).append("pca.net"));
        readCfgByName(helpString);

        ns = 1;
        left = 1;
        right = i;
        spStep = 0.1;

        helpString = QDir::toNativeSeparators(dirBC->absolutePath().append(QDir::separator()).append("SpectraPCA"));
        autoClassification(helpString);


        ns = nsBC;
        left = leftBC;
        right = rightBC;
        spStep = spStepBC;
        cout<<"numOfPc = "<<i<<" ended"<<endl;

        qApp->processEvents();
        if(stopFlag == 1)
        {
            stopFlag = 0;
            break;
        }

    }


}

void Net::averageClassification()
{
    fclose(log);

    FILE * logFile;
    helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("log.txt"));
    logFile = fopen(helpString.toStdString().c_str(),"r");
    if(logFile==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open logFile"), QMessageBox::Ok);
        cout<<"logFile==NULL"<<endl;
        return;
    }
    double *averagePercentage = new double [NumOfClasses+1];
    double *tempDouble = new double [NumOfClasses+1];

    for(int j=0; j<NumOfClasses+1; ++j)
    {
        averagePercentage[j]=0.;
        tempDouble[j]=0.;
    }

    int num=0;
    for(int i = 0; i<numOfTall; ++i)
    {
        for(int j=0; j<NumOfClasses+1; ++j)
        {
            fscanf(logFile, "%lf", &tempDouble[j]);
//            cout<<tempDouble[j]<<"  ";
            averagePercentage[j]+=tempDouble[j];
        }
//        cout<<endl;
        ++num;
    }

    for(int j=0; j<NumOfClasses+1; ++j)
    {
        averagePercentage[j] /= num;
    }
    fclose(logFile);

    log = fopen(dir->absolutePath().append(QDir::separator()).append("log.txt").toStdString().c_str(),"w");



    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    if(spStep != 250./1024.) fprintf(res, "\nPRR \t(");
    else fprintf(res, "\nPRR wnd \t(");
    for(int j=0; j<NumOfClasses-1; ++j)
    {
        fprintf(res, "%.2lf - ", averagePercentage[j]);
    }
    fprintf(res, "%.2lf", averagePercentage[NumOfClasses-1]);
    fprintf(res, ")  -  %.2lf ", averagePercentage[NumOfClasses]);
    fclose(res);
    delete []averagePercentage;
    delete []tempDouble;
}

void Net::clearSets1()
{
    helpString = QFileDialog::getExistingDirectory((QWidget*)this, tr("Choose dir"), dir->absolutePath());
    if(helpString=="") return;
    //make PA
    cout<<helpString.toStdString()<<endl;
    MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
    mkPa->setRdcCoeff(ui->rdcCoeffSpinBox->value());
    mkPa->setNumOfClasses(NumOfClasses);
//    return;
    mkPa->makePaSlot();
    dir->setPath(helpString);
    PaIntoMatrixByName("whole_wnd");


    cout<<NumberOfVectors<<endl;
    cout<<NetLength<<endl;


    double **arr = new double* [NumberOfVectors];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        arr[i] = new double [NumberOfVectors];
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            arr[i][j] = 0.;
        }
    }

    double **forWholeClass = new double* [NumberOfVectors];
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        forWholeClass[i] = new double [2];
    }

    for(int i = 0; i < NumberOfVectors; ++i)
    {
        for(int j = i+1; j < NumberOfVectors; ++j)
        {
            for(int k = 0; k < NetLength; ++k)
            {
                arr[i][j] += (matrix[i][k] - matrix[j][k])*(matrix[i][k] - matrix[j][k]);
//                arr[j][i] += (matrix[i][k] - matrix[j][k])*(matrix[i][k] - matrix[j][k]);
            }
            arr[i][j] = sqrt(arr[i][j]);
            arr[j][i] = arr[i][j];
        }
    }

    int current = 0.;
    double swapBuffer;
    double percent = 0.;
    double coeff1 = 0.;
    double coeff2 = 0.;

    QPixmap pic;
    pic = QPixmap(ui->clearSetsLabel->size());
    pic.fill();
    QPainter * pnt = new QPainter;
    pnt->begin(&pic);

    for(int clas = 0; clas < NumOfClasses; ++clas)
    {
        //make an array of sum distances to all other elements
        for(int i = 0; i < NumberOfVectors; ++i)
        {
            forWholeClass[i][0] = 0.;
            forWholeClass[i][1] = -1;
        }
        current = 0;
        for(int i = 0; i < NumberOfVectors; ++i)
        {
            if(matrix[i][NetLength+1] != clas) continue;
            forWholeClass[current][1] = i;


            for(int j = 0; j < NumberOfVectors; ++j)
            {
                if(matrix[j][NetLength+1] == clas)
                {
                    forWholeClass[current][0] += arr[i][j];
                }
            }
            ++current;
        }
        --current;

//        for(int i = 0; i < current; ++i)
//        {
//            cout<<forWholeClass[i][0]<<"\t"<<forWholeClass[i][1]<<endl;
//        }
//        cout<<endl;
        //sort this array bubbles
        for(int i = 0; i < current; ++i)
        {
            for(int j = 0; j < current - 1; ++j)
            {
                if(forWholeClass[j+1][0] > forWholeClass[j][0])
                {
                    swapBuffer = forWholeClass[j+1][0];
                    forWholeClass[j+1][0] = forWholeClass[j][0];
                    forWholeClass[j][0] = swapBuffer;

                    swapBuffer = forWholeClass[j+1][1];
                    forWholeClass[j+1][1] = forWholeClass[j][1];
                    forWholeClass[j][1] = swapBuffer;
                }
            }
        }
//        for(int i = 0; i < current; ++i)
//        {
//            cout<<forWholeClass[i][0]<<"\t"<<forWholeClass[i][1]<<endl;
//        }

        //delete 15% of mostly deviated
        //GUI
        pic.fill();
        for(int i = 0; i < ui->clearSetsLabel->size().width(); ++i)
        {
            coeff1 = (1. - (forWholeClass[int(current *  i  /ui->clearSetsLabel->size().width())][0] - forWholeClass[current][0]) / (forWholeClass[0][0] - forWholeClass[current][0])) * ui->clearSetsLabel->size().height();
            coeff2 = (1. - (forWholeClass[int(current * (i+1)/ui->clearSetsLabel->size().width())][0] - forWholeClass[current][0]) / (forWholeClass[0][0] - forWholeClass[current][0])) * ui->clearSetsLabel->size().height();
            pnt->drawLine(QPointF(i, coeff1), QPointF(i+1, coeff2));
        }
        ui->clearSetsLabel->setPixmap(pic);

        percent = setPercentageForClean();
        cout<<"percent = "<<percent<<endl;

//        switch(clas)
//        {
//        case 0: {percent = 0.12; break;}
//        case 1: {percent = 0.12; break;}
//        case 2: {percent = 0.12; break;}
//        }

        for(int i = 0; i < int(current*percent); ++i)
        {
            helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(QString(FileName[int(forWholeClass[i][1])])));
//            cout<<helpString.toStdString()<<endl;
            remove(helpString.toStdString().c_str());
        }
    }


    dir->setPath(dirBC->absolutePath());

    pnt->end();
    delete pnt;


    for(int i = 0; i < NumberOfVectors; ++i)
    {
        delete []arr[i];
    }
    delete []arr;

    for(int i = 0; i < NumberOfVectors; ++i)
    {
        delete []forWholeClass[i];
    }
    delete []forWholeClass;

    delete mkPa;
}

void Net::clearSets()
{
    helpString = QFileDialog::getExistingDirectory((QWidget*)this, tr("Get windows dir"), dir->absolutePath());
    if(helpString == "") return;
    cout<<helpString.toStdString()<<endl;
    dir->setPath(helpString);
    MakePa *mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
    mkPa->setRdcCoeff(ui->rdcCoeffSpinBox->value());
    mkPa->setNumOfClasses(NumOfClasses);

    int vecNum;
    double error;
    output = new double[NumOfClasses];
    int *mixNum;
    int type;
    int a1, a2, buffer;
    int h;
    int howMuchSuccess = 0;
    int howMuchSuccess2 = 0;
    int watIsThis;

    NumberOfErrors = new int[NumOfClasses];
    time_t duration = time(NULL);



    while(1)
    {
        mkPa->makePaSlot();
        PaIntoMatrixByName("1_wnd");
        mixNum = new int [NumberOfVectors];

        srand(time(NULL));


        //all.pa is loaded
        ecrit = ui->errorBox->value();
        temp = ui->tempBox->value();
        lrate = ui->learnRateBox->value();

        srand (time (NULL));

        error=2.*ecrit;
        type = 0;

        //set zero weights
        for(int i=0; i<NetLength+1; ++i)
        {
            for(int j=0; j<NumOfClasses; ++j)
            {
                weight[j][i]=0.;
            }
        }
        for(int i=0; i<NumberOfVectors; ++i)
        {
            mixNum[i]=i;
        }

        //part from tall();
        Error=0.;
        helpString="";
        for(int i=0; i<NumOfClasses; ++i)
        {
            NumberOfErrors[i] = 0;
        }

        epoch=0;
        //start learning


        while(error>ecrit)
        {
            error=0.0;
            //mix vectors
            for(int i=0; i<5*NumberOfVectors; ++i)
            {
                a1=rand()%(NumberOfVectors);
                a2=rand()%(NumberOfVectors);
                buffer=mixNum[a2];
                mixNum[a2]=mixNum[a1];
                mixNum[a1]=buffer;
            }

            for(int vecNum=0; vecNum<NumberOfVectors; ++vecNum)
            {

                type=int(matrix[mixNum[vecNum]][NetLength+1]);

                for(int j=0; j<NumOfClasses; ++j) //calculate output
                {
                    output[j]=0.;
                    for(int i=0; i<NetLength+1; ++i)   // +bias, coz +1
                    {
                        output[j]+=weight[j][i]*matrix[mixNum[vecNum]][i];
                    }
                    output[j]=logistic(output[j], temp); // unlinear logistic conformation
                }


                //error count + weight differ
                error+=(1.-output[type])*(1.-output[type]);
                for(int i=0; i<NumOfClasses; ++i)
                {
                    if(i!=type)
                    {
                        error+=output[i]*output[i];
                    }
                }
                watIsThis=0;
                while(watIsThis<(NetLength+1))
                {
                    //cbypass of channels not to consider
                    for(int z=0; z<zeroChanLength; ++z)
                    {
                        if((watIsThis/spLength+1)==zeroChan[z]) watIsThis+=spLength;
                    }

                    weight[type][watIsThis] += lrate * (1.-output[type]) * matrix[mixNum[vecNum]][watIsThis];
                    for(int k=0; k<NumOfClasses; ++k)
                    {
                        if (k!=type) weight[k][watIsThis] -= lrate * output[k] * matrix[mixNum[vecNum]][watIsThis];
                    }
                    ++watIsThis;
                }
            }


            error/=NumberOfVectors;
            error=sqrt(error);
            ++epoch;
            this->ui->errorSpinBox->setValue(error);
//            this->ui->epochSpinBox->setValue(epoch);
        }//endof all epoches, end of learning
        cout<<"learning ended "<<epoch<<" epoches"<<endl;

        delete [] mixNum;


        //classification

        PaIntoMatrixByName("2_wnd");

//        howMuchSuccess = 0;
        for(int vecNum=0; vecNum<NumberOfVectors; ++vecNum)
        {
            if(!ClassificateVector(vecNum))
            {
                helpString = dir->absolutePath().append(QDir::separator()).append(FileName[vecNum]);
                cout<<helpString.toStdString()<<endl;
                remove(QDir::toNativeSeparators(helpString).toStdString().c_str());
                howMuchSuccess = 1;
            }
        }
        if(howMuchSuccess == 0)
        {
            ++howMuchSuccess2;
        }
        else
        {
            howMuchSuccess2 = 0;
        }
//        cout<<"NumSuccess = "<<howMuchSuccess<<endl;

        error = 2.*ecrit;
        //set zero weights
        for(int i=0; i<NetLength+1; ++i)
        {
            for(int j=0; j<NumOfClasses; ++j)
            {
                weight[j][i]=0.;//(-10. + rand()%20)/100000.;
            }
        }
        cout<<"clean iteration finished"<<endl;

        qApp->processEvents();

        if(ui->clearSetsWaitBox->isChecked())
        {
            sleep(5);
        }


        if(stopFlag == 1)
        {
            stopFlag = 0;
            break;
        }

        if(howMuchSuccess2 == 5)
            break;

    }
    dir->setPath(dirBC->absolutePath());


    //time
    helpString="";
    duration = time(NULL) - duration;
    tmp="";
    tmp.setNum(int(duration)).prepend("Time elapsed = ").append(" sec\n");
    helpString.append(tmp);

    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Classification results"), helpString, QMessageBox::Ok);
    delete [] NumberOfErrors;
    delete [] output;



}

void Net::compressWts()
{
    if(weight!=NULL) saveWts();

    int compress=spLength/63;

    if(spLength!=63*compress)
    {
        cout<<"inappropriate spLength"<<endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("inappropriate spLength of full-wts-file"), QMessageBox::Ok);
        return;
    }


    double average;
    double temp;
    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)this, tr("wts to compress"), dirBC->absolutePath(), tr("wts files (*.wts)")));
    if(helpString=="")
    {
        QMessageBox::warning((QWidget*)this, tr("Warning"), tr("no wts file was chosen"), QMessageBox::Ok);
        return;
    }
    FILE * in = fopen(helpString.toStdString().c_str(),"r");
    FILE * out = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("weights4s_rdc.wts")).toStdString().c_str(),"w");
    for(int i=0; i<NumOfClasses*NetLength/compress; ++i)
    {
        average=0.;
        for(int j=0; j<compress; ++j)
        {
            fscanf(in, "%lf", &temp);
            average+=temp;
        }
        average/=compress;
        fprintf(out, "%lf\n", average);
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
        fscanf(in, "%lf", &temp);
        fprintf(out, "%lf\n", temp/double(compress));
    }

    fclose(in);
    fclose(out);
}

void Net::drawWts()  //generality
{
    QString out;

    QPixmap pic;
    QPainter * paint = new QPainter();
//    int NetLength=19*63, NumOfClasses=3, spLength=63;

    double ** weight = new double*[NumOfClasses];
    for(int i=0; i<NumOfClasses; ++i)
    {
        weight[i] = new double [NetLength+1];
    }

//automatization
    if(!autoFlag)
    {
        helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)this, tr("wts to draw"), dirBC->absolutePath(), tr("wts files (*.wts)")));
    }
    else
    {
        helpString = dir->absolutePath();
        helpString.append(QDir::separator()).append(ExpName).append("_weights.wts");
    }
    if(helpString=="")
    {
        cout<<"no wts-file to draw was opened"<<endl;
        QMessageBox::information((QWidget*)this, tr("Information"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }

    out = helpString;
    FILE * w = fopen(helpString.toStdString().c_str(),"r");
    if(w==NULL)
    {
        cout<<"cannot open file"<<endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open wts-file"), QMessageBox::Ok);
        return;
    }
    double maxWeight=0.;
    for(int i=0; i<NumOfClasses*(NetLength); ++i)
    {
        if(feof(w))
        {
            cout<<"wts-file too small"<<endl;

            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too small. Nothing happened"), QMessageBox::Ok);
            return;
        }
        fscanf(w, "%lf\n", &weight[i/(NetLength)][i%(NetLength)]);
        maxWeight = max(weight[i/(NetLength)][i%(NetLength)], maxWeight);
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
        if(feof(w))
        {
            cout<<"wts-file too small"<<endl;

            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too small. Nothing happened"), QMessageBox::Ok);
            return;
        }
        fscanf(w, "%lf\n", &weight[i][NetLength]);
    }
    if(!feof(w))
    {
        cout<<"wts-file too big"<<endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too long. Nothing happened"), QMessageBox::Ok);
        return;
    }
    fclose(w);




    pic = QPixmap(1600, 1600);
    pic.fill();
    paint->begin(&pic);

    double ext = spLength/250.;   //extension - graphical parameter
    for(int c2=0; c2<ns; ++c2)  //exept markers channel & Fp1,2
    {
        for(int k=0; k<250-1; ++k)
        {
            paint->setPen(QPen(QBrush("blue"), 2));
            paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - weight[0][int((c2)*spLength+k*ext)]*250/maxWeight, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - weight[0][int((c2)*spLength+(k+1)*ext)]*250/maxWeight);
            paint->setPen(QPen(QBrush("red"), 2));
            paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - weight[1][int((c2)*spLength+k*ext)]*250/maxWeight, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - weight[1][int((c2)*spLength+(k+1)*ext)]*250/maxWeight);
            if(NumOfClasses==3)
            {
                paint->setPen(QPen(QBrush("green"), 2));
                paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - weight[2][int((c2)*spLength+k*ext)]*250/maxWeight, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - weight[2][int((c2)*spLength+(k+1)*ext)]*250/maxWeight);
            }
        }
        paint->setPen("black");
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);

        paint->setFont(QFont("Helvitica", 8));
        for(int k=0; k<250-1; ++k) //for every Hz generality
        {
//            paint->drawLine(paint->device()->width() * coords::x[c2]+250*k/15, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250*k/15, paint->device()->height() * coords::y[c2]+10);
            if( (left + k*(spLength)/250.)*spStep - floor((left + k*(spLength)/250.)*spStep) < spLength/250.*spStep/2. || ceil((left + k*(spLength)/250.)*spStep) - (left + k*(spLength)/250.)*spStep < spLength/250.*spStep/2.)
            {
                paint->drawLine(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]+5);

                helpInt = int((left + k*(spLength)/250.)*spStep + 0.5);
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
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText((paint->device()->width() * coords::x[c2]-20), (paint->device()->height() * coords::y[c2]-252), QString(coords::lbl[c2]));
    }

    paint->drawLine(QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1]), QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())));  //250 - graph height generality

    //returning norm = max magnitude
    maxWeight = int(maxWeight*100.)/100.;

    helpString.setNum(maxWeight);
//    helpString.append(tr(" mcV^2/Hz"));
    paint->drawText(QPointF(paint->device()->width() * coords::x[6]+5., paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())/2.), helpString);


    paint->end();

//    QFileInfo inf(helpString);
//    helpString = inf.fileName();
//    helpString.resize(helpString.lastIndexOf('.'));
//    cout<<helpString.toStdString()<<endl;
    out.replace(".wts", ".jpg");
    cout<<out.toStdString()<<endl;
    pic.save(out, 0, 100);

    for(int i=0; i<NumOfClasses; ++i)
    {
        delete [] weight[i];
    }
    delete [] weight;

    //automatization
    if(!autoFlag)
    {
        QMessageBox::information((QWidget*)this, tr("Info"), tr("wts is drawn"), QMessageBox::Ok);
    }
}

void Net::drawWindows()
{
    int ns = 19;
    helpString = dir->absolutePath().append(QDir::separator()).append("windows.txt");
    cout<<helpString.toStdString()<<endl;
    FILE * windows = fopen(helpString.toStdString().c_str(), "r");

    int length;// = int(pic.width()/(NumOfClasses*2+1));
    double helpDouble;
    helpInt = 0;
    int tmp;
    while(!feof(windows))
    {

        pic = QPixmap(240, 200);
        pic.fill();
        paint->begin(&pic);
        fscanf(windows, "%d ", &tmp);
        cout<<helpInt<<endl;


        length = int(pic.width()/(NumOfClasses*2+1));
        paint->setPen("black");
        for(int i=0; i<NumOfClasses; ++i)
        {
            if(i==0) paint->setBrush(QBrush("blue"));
            if(i==1) paint->setBrush(QBrush("red"));
            if(i==2) paint->setBrush(QBrush("green"));
            if(i==3) paint->setBrush(QBrush("orange"));
            if(i==4) paint->setBrush(QBrush("lightgray"));
            fscanf(windows, "%lf ", &helpDouble);
            paint->drawRect(length*(2*i+1), pic.height()*(1-helpDouble), length, pic.height()*helpDouble);
        }
        helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("winKOD").append(QDir::separator()).append("win_").append(rightNumber2(helpInt)).append(".jpg");
        cout<<helpString.toStdString()<<endl;
        pic.save(helpString, 0, 100);

        paint->end();
        ++helpInt;
    }
    fclose(windows);

    dir->cd("SpectraSmooth");
    dir->cd("windows");
    QStringList lst = dir->entryList(QDir::Files, QDir::Name);
    FILE * wnd;
    double * vect = new double [ns*spLength];
    double * out = new double [NumOfClasses];
    for(int m = 0; m < lst.length(); ++m)
    {
        helpString = dir->absolutePath().append(QDir::separator()).append(lst[m]);
        wnd = fopen(helpString.toStdString().c_str(), "r");
        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < spLength; ++k)
            {
                fscanf(wnd, "%lf", &vect[j*spLength+k]);
            }
        }
        for(int j = 0; j < NumOfClasses; ++j)
        {
            out[j]=0.;
            for(int i=0; i<NetLength; ++i)
            {
                out[j] += weight[j][i] * vect[i]/20.;
            }
            out[j] += weight[j][NetLength];
            out[j] = logistic(out[j], 10.); // unlinear conformation
        }

        pic = QPixmap(240, 200);
        pic.fill();
        paint->begin(&pic);
        cout<<helpInt<<endl;
        length = int(pic.width()/(NumOfClasses*2+1));


        paint->setPen("black");
        for(int i = 0; i < NumOfClasses; ++i)
        {
            if(i==0) paint->setBrush(QBrush("blue"));
            if(i==1) paint->setBrush(QBrush("red"));
            if(i==2) paint->setBrush(QBrush("green"));
            if(i==3) paint->setBrush(QBrush("orange"));
            if(i==4) paint->setBrush(QBrush("lightgray"));
            paint->drawRect(length*(2*i+1), pic.height()*(1-out[i]), length, pic.height()*out[i]);
        }
        helpString = lst[m];
        helpString.replace(".txt", ".jpg");
        if(!helpString.contains(".jpg")) helpString.append(".jpg");
        helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("winMY").append(QDir::separator()).append(helpString);
        cout<<helpString.toStdString()<<endl;
        pic.save(helpString, 0, 100);

        paint->end();



        fclose(wnd);
        if(m==400) break;
    }
    delete []vect;
    delete []out;
    dir->cdUp();
    dir->cdUp();
}

void Net::test()
{
    if(numTest<0)
    {
        cout<<"end of vectors, too litle"<<endl;

        QMessageBox::information((QWidget*)this, tr("Information"), tr("end of vectors, too litle"), QMessageBox::Ok);
        numTest=0;
        return;
    }
    if(numTest>=NumberOfVectors)
    {
        cout<<"end of vectors, too many"<<endl;
        QMessageBox::information((QWidget*)this, tr("Information"), tr("end of vectors, too many"), QMessageBox::Ok);
        numTest=NumberOfVectors;
        return;
    }

    ClassificateVector(numTest);
    ui->lineEdit->setText(QString(FileName[numTest]));

    pic = QPixmap(240, 200);
    pic.fill();
    paint->begin(&pic);

    int length = int(pic.width()/(NumOfClasses*2+1));
    paint->setPen("black");
    for(int i=0; i<NumOfClasses; ++i)
    {
        if(i==0) paint->setBrush(QBrush("blue"));
        if(i==1) paint->setBrush(QBrush("red"));
        if(i==2) paint->setBrush(QBrush("green"));
        if(i==3) paint->setBrush(QBrush("orange"));
        if(i==4) paint->setBrush(QBrush("lightgray"));
        paint->drawRect(length*(2*i+1), pic.height()*(1-output[i]), length, pic.height()*output[i]);
    }
    ui->label_7->setPixmap(pic.scaled(ui->label_7->size()));    
    paint->end();

    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("winKOD/win_").append(rightNumber2(numTest)).append(".jpg");
    columns.load(helpString);
    ui->label_8->setPixmap(columns.scaled(ui->label_8->size()));

    delete [] output; //generality
}

void Net::next()
{
    numTest+=1;
    test();
}

void Net::prev()
{
    numTest-=1;
    test();
}

void Net::stopActivity()
{
    stopFlag=1;
}



void Net::saveWts()
{
    FILE * weights;
    //automatization

    if(!autoFlag)
    {
        helpString = QDir::toNativeSeparators(QFileDialog::getSaveFileName((QWidget*)this, tr("wts to save"), dirBC->absolutePath(), tr("wts files (*.wts)")));
        if(!helpString.endsWith(".wts") && !helpString.endsWith(".WTS"))
        {
            helpString.append(".wts");
        }
    }
    else
    {
        helpString = dir->absolutePath();
        helpString.append(QDir::separator()).append(ExpName).append("_weights.wts");
    }

    if(helpString=="")
    {
        QMessageBox::information((QWidget*)this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
    }
    weights=fopen(helpString.toStdString().c_str(),"w");

    if(weight==NULL)
    {
        cout<<"file to write==NULL"<<endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open target wts-file"), QMessageBox::Ok);
        return;
    }
    //save weights into files
    for(int j=0; j<NumOfClasses; ++j)
    {
        for(int i=0; i<NetLength; ++i)
        {
            fprintf(weights, "%lf\r\n", weight[j][i]);
        }
    }
    for(int j=0; j<NumOfClasses; ++j)
    {
        fprintf(weights, "%lf\r\n", weight[j][NetLength]);
    }
    fclose(weights);
}



void Net::reset()
{
    for(int i=0; i<NetLength+1; ++i)
    {
        for(int j=0; j<NumOfClasses; ++j)
        {
            weight[j][i]=0.;
        }
    }
}

void Net::tall()
{
    Error=0.;
    NumberOfErrors = new int[NumOfClasses];
    helpString="";
    double NumOfVectorsOfClass[3];
    for(int i=0; i<NumOfClasses; ++i)
    {
        NumberOfErrors[i]=0;
        NumOfVectorsOfClass[i]=0;
    }
    for(int i=0; i<NumberOfVectors; ++i)
    {
        ClassificateVector(i);
        //generality
        NumOfVectorsOfClass[int(matrix[i][NetLength+1])]+=1;
//        if(QString(FileName[i]).contains("_241")) NumOfVectorsOfClass[0]+=1;
//        if(QString(FileName[i]).contains("_247")) NumOfVectorsOfClass[1]+=1;
//        if(QString(FileName[i]).contains("_254")) NumOfVectorsOfClass[2]+=1;
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
//        fprintf(log, "%.2lf\t", double((1. - double(NumberOfErrors[i]*NumOfClasses/double(NumberOfVectors)))*100.));
        fprintf(log, "%.2lf\t", double((1. - double(NumberOfErrors[i]/double(NumOfVectorsOfClass[i])))*100.));
        helpString.append("Percentage[").append(tmp.setNum(i)).append("] = ");
//        /double(NumOfVectorsOfClass[i])
//        helpString.append(tmp.setNum((1. - double(NumberOfErrors[i]*NumOfClasses/double(NumberOfVectors)))*100.)).append(" % \n");
        helpString.append(tmp.setNum((1. - double(NumberOfErrors[i]/double(NumOfVectorsOfClass[i])))*100.)).append(" % \n");
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
        Error+=NumberOfErrors[i];
    }
    helpString.append("Percentage[all] = ").append(tmp.setNum((1. - double(Error/double(NumberOfVectors)))*100.)).append(" % \n");
    fprintf(log, "%.2lf\n", double((1. - double(Error/double(NumberOfVectors)))*100.));


    //automatization
    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Classification results"), helpString, QMessageBox::Ok);
    delete [] NumberOfErrors;
    ++numOfTall;
}

void Net::closeLogFile()
{
    if(log!=NULL)
    {
        fclose(log);
    }
}

void Net::readCfg()
{
    //automatization
    if(!autoFlag)
    {
        helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)NULL,tr("open cfg"), dirBC->absolutePath(), tr("cfg files (*.net)")));
        if(helpString=="")
        {
            QMessageBox::information((QWidget*)this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
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
        else
        {
            helpString = "pca.net";     //for PCAs
        }
        helpString.prepend(dirBC->absolutePath().append(QDir::separator()));
        cout<<"cfg auto path = "<<helpString.toStdString()<<endl;
    }
    readCfgByName(helpString);


}

void Net::readCfgByName(QString FileName)
{
    helpString = FileName;
    FILE * cfg=fopen(helpString.toStdString().c_str(),"r");
    if(cfg==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open cfg-file"), QMessageBox::Ok);
        return;
    }
    fscanf(cfg, "%*s    %d\n", &NetLength);
    fscanf(cfg, "%*s    %d\n", &NumOfClasses);
    fscanf(cfg, "%*s    %lf\n", &lrate);
    fscanf(cfg, "%*s    %lf\n", &ecrit);
    fscanf(cfg, "%*s    %lf\n", &temp);
    fclose(cfg);

    weight = new double * [NumOfClasses];
    for(int i=0; i<NumOfClasses; ++i)
    {
        weight[i] = new double [NetLength+1];
    }
    ui->errorBox->setValue(ecrit);
    ui->tempBox->setValue(temp);
    ui->learnRateBox->setValue(lrate);

    QStringList lst = ui->zeroChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    zeroChanLength = lst.length();
    zeroChan = new int [zeroChanLength];
    for(int i=0; i<zeroChanLength ; ++i)
    {
        zeroChan[i] = lst.at(i).toInt();   ///sequence sort
    }
    loadPAflag = 1;

}

void Net::loadWtsByName(QString filename)
{
    FILE * wts=fopen(filename.toStdString().c_str(),"r");

    for(int i=0; i<NumOfClasses*(NetLength); ++i)
    {
        fscanf(wts, "%lf", &weight[i/(NetLength)][i%(NetLength)]);
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
        fscanf(wts, "%lf", &weight[i][NetLength]);
    }
    fclose(wts);
}
//?????
void Net::loadWts()
{
    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)NULL,tr("load wts"), dir->absolutePath(), tr("wts files (*.wts)")));
    if(helpString=="")
    {
        QMessageBox::information((QWidget*)this, tr("Warning"), tr("No wts-file was chosen"), QMessageBox::Ok);
        return;
    }
    FILE * wts=fopen(helpString.toStdString().c_str(),"r");

    for(int i=0; i<NumOfClasses*(NetLength); ++i)
    {
        fscanf(wts, "%lf", &weight[i/(NetLength)][i%(NetLength)]);
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
        fscanf(wts, "%lf", &weight[i][NetLength]);
    }
    fclose(wts);
}

void Net::PaIntoMatrix()
{
    if(loadPAflag!=1)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No CFG-file loaded yet"), QMessageBox::Ok);
        return;
    }

    helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget*)NULL, tr("load PA"), dir->absolutePath(), tr("PA files (*.pa)")));
    if(helpString=="")
    {
        QMessageBox::information((QWidget*)this, tr("Information"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }



    FILE *paSrc=fopen(helpString.toStdString().c_str(), "r");
    if(paSrc==NULL)
    {
        cout<<"pa-file==NULL"<<endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open pa-file"), QMessageBox::Ok);
        return;
    }
    if(matrix!=NULL && NumberOfVectors>0)
    {
//        cout<<"delete matrix"<<endl;
        for(int i=0; i<NumberOfVectors; ++i)
        {
            if(matrix[i]!=NULL) delete []matrix[i];
        }
        delete []matrix;
    }

    NumberOfVectors=6000; //generality

    matrix = new double * [NumberOfVectors];
    for(int i=0; i<NumberOfVectors; ++i)
    {
        matrix[i] = new double[NetLength+2];
    }
    int num=0;
    double g[3];  //generality

    FileName = new char*[NumberOfVectors];
    for(int i=0; i<NumberOfVectors; ++i)
    {
        FileName[i] = new char[40];
    }

//    cout<<"start pa-reading"<<endl;
    while(!feof(paSrc))
    {
        fscanf(paSrc, "%s\n", FileName[num]);  //read FileName

        for(int i=0; i<NetLength; ++i)
        {
            fscanf(paSrc, "%lf", &matrix[num][i]);
//            matrix[num][i]*=20;
        }

        if(NumOfClasses==3) fscanf(paSrc, "%lf %lf %lf\n", &g[0], &g[1], &g[2]); //read the class
        if(NumOfClasses==2)
        {
            fscanf(paSrc, "%lf %lf\n", &g[0], &g[1]);
            g[2]=0.;
//            cout<<"g[0]="<<g[0]<<" g[1]="<<g[1]<<" g[2]="<<g[2]<<endl;
        }

        matrix[num][NetLength]=1.; //bias
        matrix[num][NetLength+1]=0.*g[0] + 1.*g[1] + 2.*g[2]; //type
        if(matrix[num][NetLength+1]!=0. && matrix[num][NetLength+1]!=1. && matrix[num][NetLength+1]!=2. && matrix[num][NetLength+1]!=1.5)
        {
            cout<<"type is wrong "<<matrix[num][NetLength+1]<<endl;
            return;
        }
        ++num;
    }
    for(int i=num; i<NumberOfVectors; ++i)
    {
        delete []matrix[i];
        delete []FileName[i];
    }
    fclose(paSrc);
    NumberOfVectors=num;
//    cout<<"NumberOfVectors="<<NumberOfVectors<<endl;
}

void Net::leaveOneOut()
{

    //all.pa is loaded
    time_t duration = time(NULL);
    ecrit = ui->errorBox->value();
    temp = ui->tempBox->value();
    lrate = ui->learnRateBox->value();

    srand (time (NULL));
    int vecNum;

    output = new double[NumOfClasses];
    double error=2.*ecrit;
    int type=0;
    int mixNum[NumberOfVectors];

//    cout<<"NumberOfVectors="<<NumberOfVectors<<endl;
//    cout<<"NumOfClasses="<<NumOfClasses<<endl;


    //set zero weights
    for(int i=0; i<NetLength+1; ++i)
    {
        for(int j=0; j<NumOfClasses; ++j)
        {
            weight[j][i]=0.;
        }
    }

    for(int i=0; i<NumberOfVectors; ++i)
    {
        mixNum[i]=i;
    }

    //part from tall();
    Error=0.;
    NumberOfErrors = new int[NumOfClasses];
    helpString="";
    for(int i=0; i<NumOfClasses; ++i)
    {
        NumberOfErrors[i]=0;
    }


    int a1, a2, buffer;

//    cout<<"LeaveOneOut start"<<endl;
    for(int h=0; h<NumberOfVectors; ++h) //learn & classificate every vector
    {
        cout<<h<<endl;

        epoch=0;
//        cout<<h<<" ";
        //start learning
        while(error>ecrit)
        {
            error=0.0;
            //mix vectors
            for(int i=0; i<5*NumberOfVectors; ++i)
            {
                a1=rand()%(NumberOfVectors);
                a2=rand()%(NumberOfVectors);
                buffer=mixNum[a2];
                mixNum[a2]=mixNum[a1];
                mixNum[a1]=buffer;
            }
            //        cout<<"epoch="<<epoch<<endl;

            for(vecNum=0; vecNum<NumberOfVectors; ++vecNum)
            {
                if(mixNum[vecNum] == h) continue; //not to learn with h'th vector

                type=int(matrix[mixNum[vecNum]][NetLength+1]);
                //            cout<<"type="<<type<<endl;

                for(int j=0; j<NumOfClasses; ++j) //calculate output
                {
                    output[j]=0.;
                    for(int i=0; i<NetLength+1; ++i)   // +bias, coz +1
                    {
                        output[j]+=weight[j][i]*matrix[mixNum[vecNum]][i];
                    }
                    output[j]=logistic(output[j], temp); // unlinear logistic conformation
                }

                //            cout<<"output[0]="<<output[0]<<"  output[1]="<<output[1]<<endl;

                //error count + weight differ
                error+=(1.-output[type])*(1.-output[type]);
                for(int i=0; i<NumOfClasses; ++i)
                {
                    if(i!=type)
                    {
                        error+=output[i]*output[i];
                    }
                }

                //            cout<<"errors"<<endl;

                //vary weights
                //            for(int i=0; i<NetLength+1; ++i)
                //            {
                //                weight[type][i] += lrate * (1.-output[type]) * matrix[mixNum[vecNum]][i];
                //                for(int k=0; k<NumOfClasses; ++k)
                //                {
                //                    if (k!=type) weight[k][i] -= lrate * output[k] * matrix[mixNum[vecNum]][i];
                //                }
                //            }

                int i=0;
                while(i<(NetLength+1))
                {
                    //cbypass of channels not to consider
                    for(int z=0; z<zeroChanLength; ++z)
                    {
                        if((i/spLength+1)==zeroChan[z]) i+=spLength;
                    }

                    weight[type][i] += lrate * (1.-output[type]) * matrix[mixNum[vecNum]][i];
                    for(int k=0; k<NumOfClasses; ++k)
                    {
                        if (k!=type) weight[k][i] -= lrate * output[k] * matrix[mixNum[vecNum]][i];
                    }
                    ++i;
                }
            }

            error/=NumberOfVectors;
            error=sqrt(error);
            ++epoch;
            this->ui->errorSpinBox->setValue(error);
//            this->ui->epochSpinBox->setValue(epoch);
        }//endof all epoches, end of learning


        //classification
        ClassificateVector(h);

        error = 2.*ecrit;
        //set zero weights
        for(int i=0; i<NetLength+1; ++i)
        {
            for(int j=0; j<NumOfClasses; ++j)
            {
                weight[j][i]=0.;//(-10. + rand()%20)/100000.;
            }
        }


        //heat control
//        if(h%2 == 0) QApplication::sendEvent(this, tempEvent);

//        qApp->processEvents();
//        if(stopFlag == 1)
//        {
//            stopFlag = 0;
//            delete [] NumberOfErrors;
//            delete [] output;
//            return;
//        }




    }

//    cout<<endl;


    if(log == NULL)
    {
        cout<<"log file = NULL"<<endl;
        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("log.txt"));
        log = fopen(helpString.toStdString().c_str(),"a+");
        if(log == NULL)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open log file to write"), QMessageBox::Ok);
            return;
        }
    }


    for(int i=0; i<NumOfClasses; ++i)
    {
//        cout<<"Percentage["<<i<<"] = "<<(1. - double(NumberOfErrors[i]*NumOfClasses/double(NumberOfVectors)))*100.<<" %"<<endl;
        fprintf(log, "%.2lf\t", double((1. - double(NumberOfErrors[i]*NumOfClasses/double(NumberOfVectors)))*100.));
//        helpString.append("Percentage[").append(tmp.setNum(i)).append("] = ");
//        helpString.append(tmp.setNum((1. - double(NumberOfErrors[i]*NumOfClasses/double(NumberOfVectors)))*100.)).append(" % \n");
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
        Error+=NumberOfErrors[i];
    }
//    cout<<"Percentage[all]= "<<(1. - double(Error/double(NumberOfVectors)))*100.<<" %"<<endl;
//    helpString.append("Percentage[all] = ").append(tmp.setNum((1. - double(Error/double(NumberOfVectors)))*100.)).append(" % \n");
    fprintf(log, "%.2lf\n", double((1. - double(Error/double(NumberOfVectors)))*100.));
    //automatization

    //results.txt-> PRR leave-one-out
//    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt"));
//    FILE * res = fopen(helpString.toStdString().c_str(), "a+");
//    error = (1. - double(Error/double(NumberOfVectors)))*100.;
//    fprintf(res, "PRR Leave-one-out \t(");
//    for(int i=0; i<NumOfClasses-1; ++i)
//    {
//        error = (1. - double(NumberOfErrors[i]*NumOfClasses/double(NumberOfVectors)))*100.;
//        fprintf(res, "%.2lf - ", error);
//    }
//    error = (1. - double(NumberOfErrors[NumOfClasses-1]*NumOfClasses/double(NumberOfVectors)))*100.;
//    fprintf(res, "%.2lf", error);

//    error = (1. - double(Error/double(NumberOfVectors)))*100.;
//    fprintf(res, ")  -  %.2lf", error);
//    fprintf(res, "\n");
//    fclose(res);

    //time
    helpString="";
    duration = time(NULL) - duration;
    tmp="";
    tmp.setNum(int(duration)).prepend("Time elapsed = ").append(" sec\n");
    helpString.append(tmp);

    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Classification results"), helpString, QMessageBox::Ok);
    delete [] NumberOfErrors;
    delete [] output;

}

void Net::PaIntoMatrixByName(QString fileName)
{
    if(loadPAflag!=1)
    {
        QMessageBox::critical((QWidget*)this, tr("net.cpp: PaIntoMatrixByName"), tr("No CFG-file loaded yet"), QMessageBox::Ok);
        return;
    }
//    cout<<"1"<<endl;

    if(fileName.contains("wnd"))
    {
        if(fileName.contains("1"))
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("1_wnd.pa");
            fprintf(log, "\n");
        }
        else if(fileName.contains("2"))
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("2_wnd.pa");
        }
        else
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("all_wnd.pa");
        }
    }
    else if(fileName.contains("pca"))
    {

        if(fileName.contains("1"))
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("1_pca.pa");
            fprintf(log, "\n");
        }
        else if(fileName.contains("2"))
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("2_pca.pa");
        }
        else
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("all_pca.pa");
        }
    }
    else
    {


        if(fileName=="1")
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("1.pa");
            fprintf(log, "\n");
        }
        else if(fileName=="2")
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("2.pa");
        }
        else
        {
            helpString = dirBC->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("all.pa");
        }
    }
//    cout<<helpString.toStdString()<<endl;
//    cout<<"2"<<endl;
//    cout<<helpString.toStdString()<<endl;
    paFileBC = helpString;

    FILE *paSrc=fopen(helpString.toStdString().c_str(), "r");
    if(paSrc==NULL)
    {
        cout<<"pa-file==NULL"<<endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open pa-file"), QMessageBox::Ok);
        return;
    }

//    cout<<"3"<<endl;

    if(matrix!=NULL && NumberOfVectors>0)
    {
//        cout<<"delete matrix"<<endl;
        for(int i=0; i<NumberOfVectors; ++i)
        {
            if(matrix[i]!=NULL) delete [] matrix[i];
        }
        delete [] matrix;
    }

//    cout<<"4"<<endl;

    NumberOfVectors=6000; //generality

    matrix = new double * [NumberOfVectors];
    for(int i=0; i<NumberOfVectors; ++i)
    {
        matrix[i] = new double[NetLength+2]; // + bias + type
    }
    int num=0;
    double g[3];  //generality

    FileName = new char*[NumberOfVectors];
    for(int i=0; i<NumberOfVectors; ++i)
    {
        FileName[i] = new char[40];
    }
//    cout<<"4"<<endl;

//    cout<<"start pa-reading"<<endl;
    while(!feof(paSrc))
    {
        fscanf(paSrc, "%s\n", FileName[num]);  //read FileName

        for(int i=0; i<NetLength; ++i)
        {
            fscanf(paSrc, "%lf", &matrix[num][i]);
//            matrix[num][i]*=20;
        }

        if(NumOfClasses==3) fscanf(paSrc, "%lf %lf %lf\n", &g[0], &g[1], &g[2]); //read the class
        if(NumOfClasses==2)
        {
            fscanf(paSrc, "%lf %lf\n", &g[0], &g[1]);
            g[2]=0.;
//            cout<<"g[0]="<<g[0]<<" g[1]="<<g[1]<<" g[2]="<<g[2]<<endl;
        }

        matrix[num][NetLength]=1.; //bias
        matrix[num][NetLength+1]=0.*g[0] + 1.*g[1] + 2.*g[2]; //type
        if(matrix[num][NetLength+1]!=0. && matrix[num][NetLength+1]!=1. && matrix[num][NetLength+1]!=2. && matrix[num][NetLength+1]!=1.5)
        {
            cout<<"type is wrong "<<matrix[num][NetLength+1]<<endl;
            return;
        }
        ++num;
    }
    for(int i=num; i<NumberOfVectors; ++i)
    {
        delete [] matrix[i];
        delete [] FileName[i];
    }
    fclose(paSrc);
    NumberOfVectors=num;
//    cout<<"5"<<endl;
//    cout<<"NumberOfVectors="<<NumberOfVectors<<endl;
}

void Net::LearnNet()
{
    time_t duration = time(NULL);
    ecrit = ui->errorBox->value();
    temp = ui->tempBox->value();
    lrate = ui->learnRateBox->value();

    srand (time (NULL));
    int vecNum;

//    if(output!=NULL) delete [] output;

    output = new double [NumOfClasses];
    double error=ecrit + 1.;
    int type=0;
    int mixNum[NumberOfVectors];

//    cout<<"NumberOfVectors="<<NumberOfVectors<<endl;
//    cout<<"NumOfClasses="<<NumOfClasses<<endl;


    //set random weights
    for(int i=0; i<NetLength+1; ++i)
    {
        for(int j=0; j<NumOfClasses; ++j)
        {
            weight[j][i]=0.;//(-10. + rand()%20)/100000.;
        }
    }


    //create a random sequence of vectors to learn the net
    for(int i=0; i<NumberOfVectors; ++i)
    {
        mixNum[i]=i;
    }
//    cout<<NetLength<<endl;

//    cout<<"now will learn"<<endl;
    //start learning

    epoch=0;

    int a1, a2, buffer;
    while(error>ecrit && epoch<ui->epochSpinBox->value())
    {
        error=0.0;
        //mix vectors
        for(int i=0; i<5*NumberOfVectors; ++i)
        {
            a1=rand()%(NumberOfVectors);
            a2=rand()%(NumberOfVectors);
            buffer=mixNum[a2];
            mixNum[a2]=mixNum[a1];
            mixNum[a1]=buffer;
        }
//        cout<<"epoch="<<epoch<<endl;

        for(vecNum=0; vecNum<NumberOfVectors; ++vecNum)
        {
            type=int(matrix[mixNum[vecNum]][NetLength+1]);
//            cout<<"type="<<type<<endl;

            for(int j=0; j<NumOfClasses; ++j) //calculate output
            {
                output[j]=0.;
                for(int i=0; i<NetLength+1; ++i)   // +bias, coz +1
                {
                    output[j]+=weight[j][i]*matrix[mixNum[vecNum]][i];
                }
                output[j]=logistic(output[j], temp); // unlinear logistic conformation
            }

//            cout<<"output[0]="<<output[0]<<"  output[1]="<<output[1]<<endl;

            //error count + weight differ
            error+=(1.-output[type])*(1.-output[type]);
            for(int i=0; i<NumOfClasses; ++i)
            {
                if(i!=type)
                {
                    error+=output[i]*output[i];
                }
            }

//            cout<<"errors"<<endl;

            //vary weights
//            for(int i=0; i<NetLength+1; ++i)
//            {
//                weight[type][i] += lrate * (1.-output[type]) * matrix[mixNum[vecNum]][i];
//                for(int k=0; k<NumOfClasses; ++k)
//                {
//                    if (k!=type) weight[k][i] -= lrate * output[k] * matrix[mixNum[vecNum]][i];
//                }
//            }
            int i=0;
            while(i<(NetLength+1))
            {
                //cbypass of channels not to consider
                for(int z=0; z<zeroChanLength; ++z)
                {
                    if((i/spLength+1)==zeroChan[z]) i+=spLength;
                }

                weight[type][i] += lrate * (1.-output[type]) * matrix[mixNum[vecNum]][i];
                for(int k=0; k<NumOfClasses; ++k)
                {
                    if (k!=type) weight[k][i] -= lrate * output[k] * matrix[mixNum[vecNum]][i];
                }
                ++i;
            }

//            cout<<"weights"<<endl;
        }

        error/=NumberOfVectors;
        error=sqrt(error);

        if(!autoFlag) cout<<"epoch="<<epoch<<" error="<<error<<endl;
        ++epoch;
        this->ui->errorSpinBox->setValue(error);
//        this->ui->epochSpinBox->setValue(epoch);

//        qApp->processEvents();
//        if(stopFlag == 1)
//        {
//            stopFlag = 0;
//            return;
//        }
    }//endof all epoches, end of learning
    cout<<"learning ended "<<epoch<<" epoches"<<endl;

    duration = time(NULL) - duration;
    tmp="";
    tmp.setNum(int(duration)).prepend("Time elapsed = ").append(" sec\n");
    helpString.setNum(epoch);
    helpString.prepend("Learning ended \nNumber of epoches = ").prepend(tmp);
    //automatization
    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);

    delete [] output;
    stopFlag = 0;
}

bool Net::ClassificateVector(int &vecNum)
{
    double * outputClass = new double [NumOfClasses];

    int type=int(matrix[vecNum][NetLength+1]);

    for(int j=0; j<NumOfClasses; ++j) //calculate output //2 = numberOfTypes
    {
        outputClass[j]=0.;
        for(int i=0; i<NetLength; ++i)
        {
            outputClass[j]+=weight[j][i] * matrix[vecNum][i];
        }
        outputClass[j]+=weight[j][NetLength] * matrix[vecNum][NetLength];
        outputClass[j]=logistic(outputClass[j], temp); // unlinear conformation
    }
    bool right = true;
//    int out=type;
    double outp=outputClass[type];
    for(int k=0; k<NumOfClasses; ++k)
    {
        if(k!=type && outputClass[k]>=outp)
        {
            right=false;
//            out=k;
            outp=outputClass[k];
        }
    }
    if(!right && matrix[vecNum][NetLength+1]!=1.5) ++NumberOfErrors[type]; //generality
    delete [] outputClass;
    return right;
    //automatization
//    if(log!=NULL) fprintf(log, "%s %d -> %d\n", FileName[vecNum], type, out);
}

int Net::getEpoch()
{
    return epoch;
}

double Net::getErrcrit()
{
    return this->ecrit;
}

void Net::setErrcrit(double a)
{
    this->ecrit = a;
}

double distance(double * vec1, double * vec2, int dim)
{
    double dist = 0.;
    //Euclid
    for(int i = 0; i < dim; ++i)
    {
        dist += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return dist;

}


//matrix product B = A*B
void matrixProduct(double ** A, double ** B, int dim)
{
    double * temp = new double [dim];
    for(int j=0; j<dim; ++j)
    {
        for(int i=0; i<dim; ++i)
        {
            temp[i]=0.;
            for(int k=0; k<dim; ++k)
            {
                temp[i]+=A[i][k]*B[k][j];
            }
        }
        for(int i=0; i<dim; ++i)
        {
            B[i][j]=temp[i];
        }
    }
    delete [] temp;
}

double errorSammon(double ** distOld, double ** distNew, int size)
{
    double sum1_=0., sum2_=0., ret;
    for(int i=0; i < size; ++i)
    {
        for(int j=0; j < i; ++j)
        {
            if(distOld[i][j]!=0.)
            {
                sum1_+=distOld[i][j];
                sum2_+=(distOld[i][j] - distNew[i][j])*(distOld[i][j] - distNew[i][j])/distOld[i][j];
            }
        }
    }
    ret = sum2_/double(sum1_);

    return ret;
}

void refreshDistAll( double ** dist, double ** coordsNew, int size)
{
    for(int i=0; i < size; ++i)
    {
        for(int j=0; j < i; ++j)
        {
            dist[i][j] = distance(coordsNew[i], coordsNew[j], 2);
            dist[j][i] = dist[i][j]; //distance(coordsNew[i], coordsNew[j], 2);
        }
    }


}

void refreshDist( double ** dist, double ** coordsNew, int size, int i)
{
    for(int j=0; j < size; ++j)
    {
        dist[i][j] = distance(coordsNew[i], coordsNew[j], 2);
        dist[j][i] = dist[i][j]; //distance(coordsNew[i], coordsNew[j], 2);
    }


}

void countGradient(double ** coords, double ** distOld, double ** distNew, int size, double * gradient)
{
    double delta = 0.05;
    double tmp1;

    for(int i=0; i < size; ++i)
    {
        tmp1 = coords[i][0];
        coords[i][0] = tmp1 + delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2*i] = errorSammon(distOld, distNew, size);

        coords[i][0] = tmp1 - delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2*i] -= errorSammon(distOld, distNew, size);
        gradient[2*i] /= delta;
        coords[i][0] = tmp1;


        tmp1 = coords[i][1];
        coords[i][1] = tmp1 + delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2*i+1] = errorSammon(distOld, distNew, size);

        coords[i][1] = tmp1 - delta/2.;
        refreshDist(distNew, coords, size, i);
        gradient[2*i+1] -= errorSammon(distOld, distNew, size);
        gradient[2*i+1] /= delta;
        coords[i][1] = tmp1;
        refreshDist(distNew, coords, size, i);
    }
}

void Net::moveCoordsGradient(double ** coords, double ** distOld, double ** distNew, int size)
{

    //gradient method
    double * gradient = new double [size*2];
    double lambda = ui->lambdaDoubleSpinBox->value(); //how to estimate - add into UI
    double num = 0;

    double errorBackup;

    countGradient(coords, distOld, distNew, size, gradient);
    int j = 0;
    while(1)
    {
        errorBackup = errorSammon(distOld, distNew, size);

        for(int i=0; i<size; ++i)
        {
            coords[i][0] -= gradient[2*i] * lambda;
            coords[i][1] -= gradient[2*i+1] * lambda;
        }
        refreshDistAll(distNew, coords, size);


        if(errorBackup > errorSammon(distOld, distNew, size))
        {
            num = j;
        }
        else
        {
            for(int i=0; i<size; ++i)
            {
                coords[i][0] += gradient[2*i] * lambda;
                coords[i][1] += gradient[2*i+1] * lambda;
            }
            refreshDistAll(distNew, coords, size);
            break;
        }
        ++j;
        if(j%5==4) lambda *= 2;
    }
    cout<<j<<endl;
}

void moveCoords(double ** coords, double ** distOld, double ** distNew, int size, int i, int iterationsCount)
{
    int num = 10. * (iterationsCount/5. + 1.);
    double coordTemp1, coordTemp2;

    coordTemp1 = coords[i][0];
    coordTemp2 = coords[i][1];

    double tmp1,tmp2;


    tmp1 = errorSammon(distOld, distNew, size);

    for(int j=0; j<num; ++j)
    {
        tmp2 = coords[i][0];
        coords[i][0] = 0. + j*coordTemp1*2./(num-1) ; //from 0 to 1.5*coords

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
    for(int j=0; j<num; ++j)
    {
        tmp2 = coords[i][1];
        coords[i][1] = 0. + j*coordTemp2*2./(num-1);

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
    for(int i=0; i<size; ++i)
    {
        distNew[i] = new double [size];
    }

    coords = new double * [size];
    for(int i=0; i<size; ++i)
    {
        coords[i] = new double [3]; //x and y - coords, type-color
    }

    //set random coordinates
    for(int i=0; i<size; ++i)
    {
        coords[i][0] = (rand()%20 + 10.);
        coords[i][1] = (rand()%20 + 10.);
        coords[i][2] = types[i];
    }

//    double maxDist=0.;
    for(int i=0; i<size; ++i)
    {
        for(int j=0; j<size; ++j)
        {
            distNew[i][j] = distance(coords[i], coords[j], 2); //plane euclid distance
//            maxDist = fmax(distNew[i][j], maxDist);
        }
    }
//    cout<<"maxDistanceRandomSet = "<<maxDist<<endl;




    //coordinate method
    double tmpError1 = 0.;
    double tmpError2 = errorSammon(distArr, distNew, size);
    int iterationsCount = 0;

    int * mixNum = new int [size];
    for(int i=0; i<size; ++i)
    {
        mixNum[i] = i;
    }
    int a1, a2, temp;
    for(int i=0; i<size*5; ++i)
    {
        a1 = rand()%size;
        a2 = rand()%size;
        temp = mixNum[a1];
        mixNum[a1] = mixNum[a2];
        mixNum[a2] = temp;
    }



    if(ui->optimizationMethodComboBox->currentIndex() == 0)
    {
        cout<<"start coordinate method, error = "<<tmpError2<<endl;
        while(1)
        {
            tmpError1 = tmpError2; //error before
            for(int i=0; i<size; ++i)
            {
                //            cout<<iterationsCount<<"   "<<i<<endl;
                moveCoords(coords, distArr, distNew, size, mixNum[i], iterationsCount);
                refreshDist(distNew, coords, size, mixNum[i]);
            }
            tmpError2 = errorSammon(distArr, distNew, size);

            cout<<iterationsCount<<" error = "<<tmpError2<<endl;
            ++iterationsCount;
            if(tmpError2 < 0.05 || (fabs(tmpError1-tmpError2)/tmpError1)<0.002) break;
        }
        cout<<"NumOfIterations = "<<iterationsCount<<" error = "<<tmpError2<<endl;
    }

    if(ui->optimizationMethodComboBox->currentIndex() == 1)
    {

        cout<<"start gradient method, error = "<<tmpError2<<endl;
        while(1)
        {
            tmpError1 = tmpError2; //error before
            moveCoordsGradient(coords, distArr, distNew, size);
            tmpError2 = errorSammon(distArr, distNew, size);

            cout<<iterationsCount<<" error = "<<tmpError2<<endl;
            ++iterationsCount;
            if(tmpError2 < 0.05 || (fabs(tmpError1-tmpError2)/tmpError1)<0.002) break;
        }
        cout<<"NumOfIterations = "<<iterationsCount<<" error = "<<tmpError2<<endl;
    }




    delete [] mixNum;




    for(int i=0; i<size; ++i)
    {
        delete [] distNew[i];
    }
    delete [] distNew;


    QMessageBox::information((QWidget*)this, tr("info"), tr("Sammon projection counted"), QMessageBox::Ok);


}

void Net::drawSammon() //uses coords array
{
    if(coords==NULL)
    {
        cout<<"coords array == NULL"<<endl;
        QMessageBox::information((QWidget*)this, tr("warning"), tr("Coords array == NULL"), QMessageBox::Ok);
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

    double minX=0., minY=0., maxX=0., maxY=0., avX,avY, rangeX, rangeY;
    int rectSize=ui->sizeSpinBox->value();

    double sum1=0., sum2=0.;
    minX = coords[0][0];
    minY = coords[0][1];
    for(int i=0; i<NumberOfVectors; ++i)
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


    for(int i=0; i<NumberOfVectors; ++i)
    {
        sum1 = coords[i][0];
        sum2 = coords[i][1];

//        cout<<sum1<<"  "<<sum2<<endl;

        painter->setBrush(QBrush("black"));
        painter->setPen("black");

        if(coords[i][2]==0)
        {
            painter->setBrush(QBrush("blue"));
            painter->setPen("blue");
        }
        if(coords[i][2]==1)
        {
            painter->setBrush(QBrush("red"));
            painter->setPen("red");
        }
        if(coords[i][2]==2)
        {
            painter->setBrush(QBrush("green"));
            painter->setPen("green");
        }
        painter->drawRect(QRectF(QPointF(pic.width()/2. + (sum1-avX)/rangeX * pic.width()/2. - rectSize - avX, pic.height()/2. + (sum2-avY)/rangeY * pic.height()/2. - rectSize - avY), QPointF(pic.width()/2. + (sum1-avX)/rangeX * pic.width()/2. + rectSize  - avX, pic.height()/2. + (sum2-avY)/rangeY * pic.height()/2. + rectSize - avY)));

    }

    helpString = dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("Sammon-").append(ui->sammonLineEdit->text()).append(".jpg");

    cout<<helpString.toStdString()<<endl;
    pic.save(helpString, 0, 100);
//    ui->sammonLineEdit->setText(rightNumber2(ui->sammonLineEdit->text().toInt()+1));

    painter->end();
//    delete [] painter;
    for(int i=0; i<3; ++i)
    {
        delete [] coords[i];
    }
    delete [] coords;



    cout<<"Sammon projection done"<<endl;
    QMessageBox::information((QWidget*)this, tr("info"), tr("Sammon projection drawn"), QMessageBox::Ok);
}

//principal component analisys
void Net::pca()
{
    QTime wholeTime;
    wholeTime.start();
//    spLength - 1 channel
//    spLength*ns = NetLength
//    matrix - matrix of data
//    centeredMatrix - matrix of centered data: matrix[i][j]-=average[j]
//    random quantity is a spectre-vector of spLength dimension
//    there are NumberOfVectors samples of this random quantity
//        similarMatrix - matrix of similarity
//        differenceMatrix - matrix of difference, according to some metric
    //projectedMatrix - vectors with just some PCs left
    cout<<"NetLength = "<<NetLength<<endl;
    cout<<"NumberOfVectors = "<<NumberOfVectors<<endl;
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
//        cout<<i<<"   "<<averages[i]<<endl; //test - OK
    }

    //centered matrix
    for(int i = 0; i < NetLength; ++i)
    {
        for(int j=0; j < NumberOfVectors; ++j)
        {
            centeredMatrix[j][i] -= averages[i];
        }
    }

    cout<<"centeredMatrix counted"<<endl;

    //NetLength ~= 45000
    //NumberOfVectors ~= 100
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
//            cout << "50 passed, time = " << initTime.elapsed() << " i = " << i <<endl;
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
//            if(covMatrix[i][k] != covMatrix[k][i]) cout<<i<<" "<<k<<" warning"<<endl;
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

    cout<<"start eigenValues processing"<<endl;
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
        counter=0.;
        while(1) //when stop approximate?
        {
            //countF
            F = 0.;
            for(int i = 0; i < NetLength; ++i)
            {
                for(int j = 0; j < NumberOfVectors; ++j)
                {
                    F += 0.5*(centeredMatrix[j][i] - tempB[j] * tempA[i]) * (centeredMatrix[j][i] - tempB[j] * tempA[i]);
                }
//                cout<<F<<" ";
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
//            if(k==0) cout<<endl;

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

            dF=0.;
            for(int i = 0; i < NetLength; ++i)
            {
                for(int j = 0; j < NumberOfVectors; ++j)
                {
                    dF += 0.5 * (centeredMatrix[j][i] - tempB[j] * tempA[i]) * (centeredMatrix[j][i] - tempB[j] * tempA[i]);
                }
            }
            dF = (F-dF)/F;
            ++counter;
            if(fabs(dF) < 1e-5) break;
        }
//        cout<<k<<"   "<<counter<<endl;

        //edit covMatrix
        for(int i = 0; i < NetLength; ++i)
        {
            for(int j = 0; j < NumberOfVectors; ++j)
            {
                centeredMatrix[j][i] -= tempB[j] * tempA[i];
            }
        }

        //count eigenVectors && eigenValues
        sum1=0.;
        sum2=0.;
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
//            if(((tempB[i]-tempA[i])/tempB[i])<-0.05 || ((tempB[i]-tempA[i])/tempB[i])>0.05) cout<<k<<" "<<i<<" warning"<<endl;  //till k==19 - OK
        }

        eigenValues[k] = sum1 * sum2 / double(NumberOfVectors-1.);


        sum1 = 0.;
        for(int j = 0; j < k+1; ++j)
        {
            sum1 += eigenValues[j];
        }
//        cout<<"Part of dispersion explained = "<<sum1*100./double(trace)<<" %"<<endl;

        cout << k << "\t" << eigenValues[k] << "\tDisp expl = " << sum1*100./double(trace) << " %\ttimeElapsed = " << initTime.elapsed()/1000. << " seconds" <<endl;
        for(int i = 0; i < NetLength; ++i)
        {
            eigenVectors[i][k] = tempA[i]; //1-normalized
        }

        //need a rule
        if(k+1 == ui->pcaNumberSpinBox->value() || sum1/trace>=ui->traceDoubleSpinBox->value())
        {
            cout<<"numOfEigenValues = "<<k+1<<endl;
            numOfPc = k+1;
            break;
        }

    }
    ui->autpPCAMaxSpinBox->setValue(numOfPc);

    sum1 = 0.;
    for(int k = 0; k < numOfPc; ++k)
    {
        sum1 += eigenValues[k];
    }
    cout<<"Part of dispersion explained = "<<sum1*100./double(trace)<<" %"<<endl;
    cout<<"Number of Components = "<<numOfPc<<endl;

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


    FILE * pcaFile;
    //count reduced Data - first some PC
    for(int j = 0; j < NumberOfVectors; ++j) //i->j
    {
        pcaFile = fopen(QDir::toNativeSeparators(dirBC->absolutePath().append(QDir::separator()).append("SpectraPCA").append(QDir::separator()).append(FileName[j])).toStdString().c_str(), "w");
        for(int k = 0; k < numOfPc; ++k) //j->k
        {
            fprintf(pcaFile, "%lf\n", double(10.*pcaMatrix[j][k])); //PC coefficients
        }
        fclose(pcaFile);
    }
    cout<<"some PC projections written"<<endl;


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
        for(int j=0; j<numOfPc; ++j)
        {
            avProj[j]=0.;
            for(int i=0; i<NumberOfVectors; ++i)
            {
                avProj[j]+=centeredMatrix[j][i];

            }
            avProj[j]/=NumberOfVectors;
            cout<<"averageProjection["<<j<<"] = "<<avProj[j]<<endl;
        }
        ui->sammonLineEdit->setText("kohonen");
        Kohonen(matrix, eigenVectors, avProj, NumberOfVectors, NetLength);
        delete [] avProj;
        ui->sammonLineEdit->clear();
    }



    cout<<"end"<<endl;
    for(int i=0; i<NumberOfVectors; ++i)
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

//    if(!this->autoFlag) QMessageBox::information((QWidget*)this, tr("info"), tr("PCA drawn"), QMessageBox::Ok);
}

double Net::thetalpha(int bmu_, int j_, int step_, double ** arr, int length_)
{
    //approximately 20-30 steps
    double neighbour=0., alpha=0., temp=0., sigma=0.;
    alpha = 5./(step_+5.);
    sigma = 10./(step_+10.);
    neighbour = exp(- distance(arr[bmu_], arr[j_], length_) * distance(arr[bmu_], arr[j_], length_) / (2. * sigma * sigma));
    temp = neighbour * alpha;
//    if(bmu_==j_) temp=1.;
    return temp;
}

//length - dimension of input vectors
void Net::Kohonen(double ** input, double ** eigenVects, double * averageProjection, int size, int length)
{
    srand(time(NULL));
//    int numOfComponents = SizeOfArray(averageProjection); //=2
//    cout<<"size = "<<size<<" SizeOfArray(input) = "<<SizeOfArray(input)<<endl;

//    int sqrLen = floor(sqrt(size)) + 1;
    int sqrLen = 10;
    int numOfNodes = sqrLen * sqrLen;

    //set the coords
    double ** coordsKohonen = new double * [numOfNodes];
    for(int i=0; i<numOfNodes; ++i)
    {
        coordsKohonen[i] = new double [2];

        coordsKohonen[i][0] = (i%sqrLen);
        coordsKohonen[i][1] = floor(i/sqrLen);
    }

    //set the weights
    double a1,a2;
    double ** weightsKohonen = new double * [numOfNodes];
    for(int i=0; i<numOfNodes; ++i)
    {
        weightsKohonen[i] = new double [length+1]; //+ type

        //+- 10%
        a1 = (0.9 + 0.05 * (rand()%41)/10.);
        a2 = (0.9 + 0.05 * (rand()%41)/10.);
        for(int j=0; j<length; ++j)
        {
            weightsKohonen[i][j] = averageProjection[0] * a1 * eigenVects[j][0]  + averageProjection[1] * a2 * eigenVects[j][1];
//            cout<<weightsKohonen[i][j]<<endl; //OK
        }
    }

    int * mixNum = new int [size];
    int tmp1, tmp2, buf;
    double minDist;
    int * bmu = new int [size]; //num of bet match neuron

    double * distances = new double [numOfNodes];
    for(int i=0; i<numOfNodes; ++i)
    {
        distances[i] = 0.;
    }

    double error=0., tempError=0.;

    int step=0;
    while(1)
    {

        //mix the order
        for(int i=0; i<size; ++i)
        {
            mixNum[i]=i;
        }


        for(int i=0; i<5*size; ++i)
        {
            tmp1=rand()%(size);
            tmp2=rand()%(size);
            buf=mixNum[tmp2];
            mixNum[tmp2]=mixNum[tmp1];
            mixNum[tmp1]=buf;
        }

        //for every inputVector
        for(int i=0; i<size; ++i)
        {

            //find BMU
            for(int j=0; j<numOfNodes; ++j)
            {
                distances[j] = distance(input[mixNum[i]], weightsKohonen[j], length);
                if(j==0)
                {
                    minDist = distances[j]; bmu[mixNum[i]]=j;
                }
                else
                {
                    if(distances[j] < minDist)
                    {
                        minDist = distances[j];
                        bmu[mixNum[i]]=j;
                        weightsKohonen[j][length] = input[mixNum[i]][NetLength+1];
                    }
                }
            }


            for(int j=0; j<numOfNodes; ++j)
            {
                //adjust the weights
                for(int k=0; k<length; ++k)
                {
                     weightsKohonen[j][k] += thetalpha(bmu[mixNum[i]], j, step, coordsKohonen, 2) * (input[mixNum[i]][k] - weightsKohonen[j][k]);
                }

                //adjust the coords???
                coordsKohonen[j][0] +=0;// theta(bmu[mixNum[i]], j, step)*alpha(step)*(input[mixNum[i]][k] - weightsKohonen[j][k]);
                coordsKohonen[j][1] +=0;// theta(bmu[mixNum[i]], j, step)*alpha(step)*(input[mixNum[i]][k] - weightsKohonen[j][k]);
            }
        }
        tempError = error;
        error=0.;
        //count error
        for(int i=0; i<size; ++i)
        {
            error += distance(input[i], weightsKohonen[bmu[i]], length);
        }
        error/=size;
        cout<<step<<" "<<"error = "<<error<<endl;



        ++step;
        if(step==100 || error<0.01 || fabs(tempError-error)/error <0.002) break;
    }



    double ** distKohonen = new double * [numOfNodes];
    for(int i=0; i < numOfNodes; ++i)
    {
        distKohonen[i] = new double [numOfNodes];
        for(int j=0; j < numOfNodes; ++j)
        {
            distKohonen[i][j] = distance(weightsKohonen[i], weightsKohonen[j], length);
//            cout<<distKohonen[i][j]<<endl; //OK
        }
    }

    int * typesKohonen = new int [numOfNodes];
    for(int i=0; i < numOfNodes; ++i)
    {
        typesKohonen[i] = weightsKohonen[i][length];
        if(typesKohonen[i]!=0 && typesKohonen[i]!=1 && typesKohonen[i]!=2)
        {
            cout<<"typeKohonen = "<<typesKohonen[i]<<endl;
        }
    }
    Sammon(distKohonen, numOfNodes, typesKohonen);


    delete [] mixNum;
    delete [] distances;
    delete [] bmu;
    for(int i=0; i < numOfNodes; ++i)
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
    helpString = dir->absolutePath();
    helpString.append(QDir::separator()).append("PA").append(QDir::separator()).append("output1");
    FILE * out = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");
    fclose(out);
//    QString spectraDir = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
    QString spectraDir = QFileDialog::getExistingDirectory(this, tr("Choose spectra dir"), dir->absolutePath());
    if(spectraDir.isEmpty()) spectraDir = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
    if(spectraDir.isEmpty())
    {
        cout<<"spectraDir for SVM is empty"<<endl;
        return;
    }
    MakePa * mkPa = new MakePa(spectraDir, ExpName, ns, left, right, spStep);

    for(int i = 0; i < ui->numOfPairsBox->value(); ++i)
    {
        mkPa->makePaSlot();

        helpString = dir->absolutePath();
        helpString.append(QDir::separator()).append("PA");
        helpString.prepend("cd ");
        helpString.append(" && svm-train -t 0 svm1 && svm-predict svm2 svm1.model output >> output1");
        system(helpString.toStdString().c_str());

        helpString = dir->absolutePath();
        helpString.append(QDir::separator()).append("PA");
        helpString.prepend("cd ");
        helpString.append(" && svm-train -t 0 svm2 && svm-predict svm1 svm2.model output >> output1");
        system(helpString.toStdString().c_str());
    }


    helpString = dir->absolutePath();
    helpString.append(QDir::separator()).append("PA").append(QDir::separator()).append("output1");

    double helpDouble, average = 0.;

    QFile file(helpString);
    if(!file.open(QIODevice::ReadOnly)) return;
    int lines = 0;
    while(!file.atEnd())
    {
        helpString = file.readLine();
        if(!helpString.contains(QRegExp("[%= ]"))) break;
        helpString = helpString.split(QRegExp("[%= ]"), QString::SkipEmptyParts)[1];
        helpDouble = helpString.toDouble();
        average += helpDouble;
        ++lines;
    }
    average /= lines;
    cout<<average<<endl;
    file.close();

//    helpString = dir->absolutePath();
//    helpString.append(QDir::separator()).append("PA").append(QDir::separator()).append("output1");
//    fstream in;
//    in.open(QDir::toNativeSeparators(helpString).toStdString().c_str(), fstream::in);

//    for(int i = 0; i < ui->numOfPairsBox->value()*2; ++i)
//    {
//        in.getline(helpCharArr, 200);
//        sscanf(helpCharArr, "Accuracy = %lf", &helpDouble);
//        average += helpDouble;
//    }
//    average /= ui->numOfPairsBox->value()*2;
//    cout<<average<<endl;
//    in.close();

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "\nSVM\t");
    fprintf(res, "%.2lf\%\n", average);
    fclose(res);

    delete mkPa;
}
