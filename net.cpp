#include "net.h"
#include "ui_net.h"
//#include <CL/cl.h>
using namespace std;

Net::Net() :
    ui(new Ui::Net)
{
    ui->setupUi(this);
    this->setWindowTitle("Net");

    //clean log file
    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "log.txt");
    QFile::remove(helpString);

//    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
//                                          + slash() + "badFiles.txt");
//    QFile::remove(helpString);

    stopFlag = 0;
    confusionMatrix.resize(def::numOfClasses(), def::numOfClasses(), 0.);

//    tempRandomMatrix = matrix(def::nsWOM(), def::nsWOM());

    group1 = new QButtonGroup();
    group1->addButton(ui->leaveOneOutRadioButton);
    group1->addButton(ui->crossRadioButton);
    group1->addButton(ui->trainTestRadioButton);
    group1->addButton(ui->halfHalfRadioButton);
    group2 = new QButtonGroup();
    group2->addButton(ui->realsRadioButton);
    group2->addButton(ui->windowsRadioButton);
    group2->addButton(ui->pcaRadioButton);
    group2->addButton(ui->bayesRadioButton);
    group3 = new QButtonGroup();
    group3->addButton(ui->deltaRadioButton);
    group3->addButton(ui->backpropRadioButton);
    group3->addButton(ui->deepBeliefRadioButton);
    group4 = new QButtonGroup();
    group4->addButton(ui->logisticRadioButton);
    group4->addButton(ui->softmaxRadioButton);

    ui->softmaxRadioButton->setChecked(true); /// activation

    ui->crossRadioButton->setChecked(true); /// k-fold
    ui->leaveOneOutRadioButton->setChecked(true); /// N-fold
//    ui->trainTestRadioButton->setChecked(true); /// train-test
    if(def::OssadtchiFlag)
    {
        ui->trainTestRadioButton->setChecked(true); /// train-test
    }

//    ui->realsRadioButton->setChecked(true); /// reals
    ui->windowsRadioButton->setChecked(true); /// windows
//    ui->pcaRadioButton->setChecked(true); /// PCA

    ui->backpropRadioButton->setChecked(false);
    ui->deltaRadioButton->setChecked(false);

    ui->dropoutDoubleSpinBox->setMinimum(0.);
    ui->dropoutDoubleSpinBox->setMaximum(1.);
    ui->dropoutDoubleSpinBox->setValue(0.15);
    ui->dropoutDoubleSpinBox->setSingleStep(0.05);

    ui->tempBox->setValue(10);
    ui->tempBox->setSingleStep(1);

    ui->critErrorDoubleSpinBox->setValue(0.10);
    ui->critErrorDoubleSpinBox->setSingleStep(0.01);
    ui->critErrorDoubleSpinBox->setDecimals(4);
    ui->critErrorDoubleSpinBox->setValue(0.05); /// errcrit PEWPEW

    ui->learnRateBox->setMinimum(0.001);
    ui->learnRateBox->setMaximum(1.0);
    ui->learnRateBox->setValue(0.05); /// lrate
    ui->learnRateBox->setSingleStep(0.01);
    ui->learnRateBox->setDecimals(3);

    ui->epochSpinBox->setMaximum(1000);
    ui->epochSpinBox->setSingleStep(50);
    ui->epochSpinBox->setValue(300);

    ui->numOfPairsBox->setMaximum(100);
    ui->numOfPairsBox->setMinimum(1);
    ui->numOfPairsBox->setValue(10); //// pairs

#define INDICES 1
    ui->foldSpinBox->setMaximum(10);
    ui->foldSpinBox->setMinimum(1);
    ui->foldSpinBox->setValue(4); /////// fold

    ui->rdcCoeffSpinBox->setMaximum(100);
    ui->rdcCoeffSpinBox->setDecimals(3);
    ui->rdcCoeffSpinBox->setMinimum(0.001);
    ui->rdcCoeffSpinBox->setSingleStep(0.5);

    ui->rdcCoeffSpinBox->setValue(4.5); ///  rdc coeff

    ui->highLimitSpinBox->setMaximum(500);
    ui->highLimitSpinBox->setMinimum(100);
    ui->highLimitSpinBox->setValue(120); /// highLimit

    ui->lowLimitSpinBox->setMaximum(500);
    ui->lowLimitSpinBox->setMinimum(50);
    ui->lowLimitSpinBox->setValue(70);  /// lowLimit



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

    ui->autoPCAMaxSpinBox->setMinimum(1);
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
//    QObject::connect(ui->loadNetButton, SIGNAL(clicked()), this, SLOT(readCfg()));

    QObject::connect(ui->loadWtsButton, SIGNAL(clicked()), this, SLOT(readWts()));

//    QObject::connect(ui->loadPaButton, SIGNAL(clicked()), this, SLOT(PaIntoMatrix()));
    QObject::connect(ui->loadPaButton, SIGNAL(clicked()), this, SLOT(loadDataSlot()));

    QObject::connect(ui->learnButton, SIGNAL(clicked()), this, SLOT(learnNet()));

    QObject::connect(ui->testAllButton, SIGNAL(clicked()), this, SLOT(tallNet()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopActivity()));

    QObject::connect(ui->saveWtsButton, SIGNAL(clicked()), this, SLOT(writeWtsSlot()));

    QObject::connect(ui->drawWtsButton, SIGNAL(clicked()), this, SLOT(drawWtsSlot()));

    QObject::connect(ui->neuronGasPushButton, SIGNAL(clicked()), this, SLOT(neuronGas()));

    QObject::connect(ui->pcaPushButton, SIGNAL(clicked()), this, SLOT(pca()));

    QObject::connect(ui->drawSammonPushButton, SIGNAL(clicked()), this, SLOT(drawSammon()));

    QObject::connect(ui->autoClassButton, SIGNAL(clicked()), this, SLOT(autoClassificationSimple()));

    QObject::connect(ui->svmPushButton, SIGNAL(clicked()), this, SLOT(SVM()));

//    QObject::connect(ui->hopfieldPushButton, SIGNAL(clicked()), this, SLOT(Hopfield()));



    QObject::connect(ui->dimensionalityLineEdit, SIGNAL(returnPressed()), this, SLOT(reset()));

    QObject::connect(ui->distancesPushButton, SIGNAL(clicked()), this, SLOT(testDistances()));

    QObject::connect(ui->optimizeChannelsPushButton, SIGNAL(clicked()), this, SLOT(optimizeChannelsSet()));

    QObject::connect(group1, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(setModeSlot(QAbstractButton*, bool)));
    QObject::connect(group2, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(setSourceSlot(QAbstractButton*)));
    QObject::connect(group3, SIGNAL(buttonToggled(int,bool)), this, SLOT(methodSetParam(int,bool)));
    QObject::connect(group4, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(setActFuncSlot(QAbstractButton*)));
    this->setAttribute(Qt::WA_DeleteOnClose);

    aaDefaultSettings();
}

void Net::aaDefaultSettings()
{
    ui->deltaRadioButton->setChecked(true);

    /// mode
    ui->crossRadioButton->setChecked(true); /// k-fold
//    ui->leaveOneOutRadioButton->setChecked(true); /// N-fold
//    ui->trainTestRadioButton->setChecked(true); /// train-test

    /// source
        ui->realsRadioButton->setChecked(true); /// reals
//        ui->windowsRadioButton->setChecked(true); /// windows
    //    ui->pcaRadioButton->setChecked(true); /// PCA

    /// activation
//    ui->logisticRadioButton->setChecked(true);
//    ui->softmaxRadioButton->setChecked(true);
    activation = softmax;
    ui->highLimitSpinBox->setValue(60); /// highLimit
    ui->lowLimitSpinBox->setValue(40);  /// lowLimit

//    activation = logistic;
//    ui->highLimitSpinBox->setValue(120); /// highLimit
//    ui->lowLimitSpinBox->setValue(60);  /// lowLimit

    ui->rdcCoeffSpinBox->setValue(7.); ///  rdc coeff
    ui->foldSpinBox->setValue(4); /////// fold
    ui->numOfPairsBox->setValue(10); //// pairs
    ui->critErrorDoubleSpinBox->setValue(0.04); /// errcrit PEWPEW
}



const char * kernelFromFile(char * path)
{
    char * tempString = new char [300];
    char * shaderString = new char [30000];
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
        for(unsigned int i = 0; i < strlen(tempString); ++i)
        {
            shaderString[currentIndex++] = tempString[i];
        }
    }
    shaderString[currentIndex] = '\0';
    fclose(shad);

    delete []tempString;
    return shaderString; // static pewpew ololo
}


const char *  errorMessage(int err_)
{
    return QString::number(err_).toStdString().c_str();
}

Net::~Net()
{
    delete group1;
    delete group2;
    delete group3;
    delete group4;
    delete ui;
}

void Net::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

int Net::getEpoch()
{
    return epoch;
}

double Net::getLrate()
{
    return ui->learnRateBox->value();
}

void Net::setActFunc(const QString & in)
{
    if(in.contains("log", Qt::CaseInsensitive) ||
       in.startsWith('l', Qt::CaseInsensitive))
    {
        ui->logisticRadioButton->setChecked(true);
        activation = logistic;
        ui->learnRateBox->setValue(0.05);
//        ui->critErrorDoubleSpinBox->setValue(0.1);
        ui->adjustLeanrRateCheckBox->setChecked(true);
//        ui->adjustLeanrRateCheckBox->setChecked(false);
    }
    else if(in.contains("soft", Qt::CaseInsensitive) ||
            in.startsWith('s', Qt::CaseInsensitive))
    {
        ui->softmaxRadioButton->setChecked(true);
        activation = softmax;
        ui->learnRateBox->setValue(0.05);
        ui->critErrorDoubleSpinBox->setValue(0.05);
        ui->adjustLeanrRateCheckBox->setChecked(false);
    }
    else
    {
        cout << "setActFunc: wrong input" << endl;
        exit(0);
    }
}

void Net::setMode(const QString & in)
{
    if(in.contains("k-fold", Qt::CaseInsensitive)
       || in.contains("cross", Qt::CaseInsensitive)
       || in.startsWith('k', Qt::CaseInsensitive)
       || in.startsWith('c', Qt::CaseInsensitive))
    {
        ui->crossRadioButton->setChecked(true);
    }
    else if(in.contains("N-fold", Qt::CaseInsensitive)
            || in.startsWith('N', Qt::CaseInsensitive))
    {
        ui->leaveOneOutRadioButton->setChecked(true);
    }
    else if(in.contains("train", Qt::CaseInsensitive)
            || in.contains("test", Qt::CaseInsensitive)
            || in.startsWith('t', Qt::CaseInsensitive))
    {
        ui->trainTestRadioButton->setChecked(true);
    }
    else if(in.contains("half", Qt::CaseInsensitive)
            || in.startsWith('h', Qt::CaseInsensitive))
    {
        ui->halfHalfRadioButton->setChecked(true);
    }
    else
    {
        cout << "setMode: wrong mode" << endl;
        exit(0);
    }
}

void Net::setTallCleanFlag(bool in)
{
    this->tallCleanFlag = in;
}

void Net::setFold(int in)
{
    ui->foldSpinBox->setValue(in);
}

void Net::setSource(const QString & in)
{
    if(in.contains("real", Qt::CaseInsensitive) || in.startsWith('r'))
    {
        ui->realsRadioButton->setChecked(true);
        loadDataNorm = 10.;
    }
    else if(in.contains("wind", Qt::CaseInsensitive) || in.startsWith('w'))
    {
        ui->windowsRadioButton->setChecked(true);
        loadDataNorm = 5.;
    }
}

void Net::setActFuncSlot(QAbstractButton * but)
{
    if(but->text().contains("logistic", Qt::CaseInsensitive))
    {
        setActFunc("logistic");
    }
    else if(but->text().contains("softmax", Qt::CaseInsensitive))
    {
        setActFunc("softmax");
    }
}

void Net::setSourceSlot(QAbstractButton * but)
{
    if(but->text().contains("Bayes", Qt::CaseInsensitive))
    {
        ui->highLimitSpinBox->setValue(400);
        ui->lowLimitSpinBox->setValue(300);
        ui->epochSpinBox->setValue(500);
        ui->rdcCoeffSpinBox->setValue(0.05);
        Source = source::bayes;
    }
    else
    {
        if(activation == softmax)
        {
            ui->highLimitSpinBox->setValue(60); /// highLimit
            ui->lowLimitSpinBox->setValue(40);  /// lowLimit
        }
        else if(activation == logistic)
        {
            ui->highLimitSpinBox->setValue(120); /// highLimit
            ui->lowLimitSpinBox->setValue(60);  /// lowLimit
        }

        ui->highLimitSpinBox->setValue(150);
        ui->lowLimitSpinBox->setValue(80);

        ui->epochSpinBox->setValue(250);
        ui->rdcCoeffSpinBox->setValue(7.);
        ui->foldSpinBox->setValue(5.);
        if(but->text().contains("wind", Qt::CaseInsensitive))
        {
            loadDataNorm = 5.;
            Source = source::winds;
        }
        else if(but->text().contains("real", Qt::CaseInsensitive))
        {
            loadDataNorm = 10.;
            Source = source::reals;
        }
        else if(but->text().contains("pca", Qt::CaseInsensitive))
        {
            Source = source::pca;
        }
    }
}


/// ids from group1->addButton()
void Net::setModeSlot(QAbstractButton * but, bool i)
{
    if(i == false) return;
    int a = group1->checkedId();
    switch(a)
    {
    case -2:
    {
        Mode = myMode::N_fold; break;
    }
    case -3:
    {
        Mode = myMode::k_fold; break;
    }
    case -4:
    {
        Mode = myMode::train_test; break;
    }
    case -5:
    {
        Mode = myMode::half_half; break;
    }
    default: {break;}
    }
}

vector<int> Net::makeLearnIndexSet()
{
    vector<int> mixNum;
    if(Mode == myMode::train_test)
    {
        for(int i = 0; i < fileNames.size(); ++i)
        {
            if(fileNames[i].contains("_train"))
            {
                mixNum.push_back(i);
            }
        }
    }
    else if(Mode == myMode::N_fold)
    {
        for(int i = 0; i < dataMatrix.rows(); ++i)
        {
            mixNum.push_back(i);
        }
    }
    else if(Mode == myMode::k_fold)
    {
        mixNum.resize(dataMatrix.rows());
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::iota(mixNum.begin(), mixNum.end(), 0);
        std::shuffle(mixNum.begin(), mixNum.end(),
                     std::default_random_engine(seed));
        const int folds = ui->foldSpinBox->value();
        mixNum.resize(mixNum.size() * (folds - 1) / folds);
    }
    else if(Mode == myMode::half_half)
    {
        mixNum.resize(dataMatrix.rows() / 2);
        std::iota(mixNum.begin(), mixNum.end(), 0);
    }
    return mixNum;
}


double Net::adjustLearnRate(int lowLimit,
                            int highLimit)
{

    QTime myTime;
    myTime.start();

    cout << "adjustLearnRate: start" << endl;

    const vector<int> mixNum = makeLearnIndexSet();

    double res = 1.;
    int counter = 0;
    do
    {
        /// remake with indices
        const double currVal = ui->learnRateBox->value();
        cout << "lrate = " << currVal << '\t';

        learnNetIndices(mixNum);

        /// check limits
        if(this->getEpoch() > highLimit
           || this->getEpoch() < lowLimit)
        {
            ui->learnRateBox->setValue(currVal
                                       * sqrt(this->getEpoch()
                                              /  ((lowLimit + highLimit) / 2.)
                                              )
                                       );
        }
        else
        {
            res = currVal;
            break;
        }

        /// check lrate values
        if(ui->learnRateBox->value() < 0.005)
        {
            ui->learnRateBox->setValue(0.005); break;
        }
        else if(ui->learnRateBox->value() >= 1.)
        {
            ui->learnRateBox->setValue(1.); break;
        }
        ++counter;
    } while (counter < 15);
    cout << "adjustLearnRate: lrate = " << res << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
    return res;
}


double Net::adjustReduceCoeff(QString spectraDir,
                              int lowLimit,
                              int highLimit,
                              QString paFileName)
{
    QTime myTime;
    myTime.start();


    double res;
    double currVal;
    const double threshold = 5e-2;

    cout << "adjustReduceCoeff: start" << endl;

    while(1)
    {
        currVal = ui->rdcCoeffSpinBox->value();

        /// remake with indices
        makePaStatic(spectraDir,
                     ui->foldSpinBox->value(),
                     currVal);
        cout << "coeff = " << currVal << "\t";

        if(Mode == myMode::train_test)
        {
            PaIntoMatrixByName(paFileName);
            learnNet();
        }
        else
        {
            PaIntoMatrixByName("all");

//            cout << dataMatrix.rows() << endl;
//            for(int i = 0; i < def::numOfClasses(); ++i)
//            {
//                cout << classCount[i] << "\t";
//            }
//            cout << endl;

            vector<int> learnIndices;
            for(int i = 0; i < dataMatrix.rows(); ++i)
            {
//                cout << i << "\t" << fileNames[i] << endl;
                if(fileNames[i].contains("train"))
                {
                    learnIndices.push_back(i);
                }
            }
//            for(int item : learnIndices)
//            {
//                cout << item << "\t" << fileNames[item] << endl;
//            }
            learnNetIndices(learnIndices);
        }

        if(this->getEpoch() > highLimit
           || this->getEpoch() < lowLimit)
        {
            ui->rdcCoeffSpinBox->setValue(currVal
                                          / sqrt(this->getEpoch()
                                                 /  ((lowLimit + highLimit) / 2.)
                                                 )
                                          );
            if(currVal <= threshold) //possible minimum
            {
                cout << "adjustReduceCoeff: stop, too little" << endl;
                res = threshold;
                break;
            }
        }
        else
        {
            res = currVal;
            break;
        }
    }
    cout << "adjustReduceCoeff: reduceCoeff = " << res << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
    return res;
}


void Net::autoClassificationSimple()
{

//    ui->deltaRadioButton->setChecked(true); //generality
    QString helpString;
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "SpectraSmooth");

    if(Source == source::winds) //generality
    {
        helpString += slash() + "windows";
    }
    else if(Source == source::bayes)
    {
        helpString += slash() + "Bayes";
    }
    else if(Source == source::pca)
    {
        helpString += slash() + "PCA";
    }

    if(!helpString.isEmpty())
    {
        autoClassification(helpString);
    }
}


pair<vector<int>, vector<int>> Net::makeIndicesSetsCross(const vector<vector<int>> & arr,
                                                         const int numOfFold)
{
    vector<int> learnInd;
    vector<int> tallInd;

    const int fold = ui->foldSpinBox->value();

    for(int i = 0; i < def::numOfClasses(); ++i)
    {

        for(int j = 0; j < classCount[i]; ++j)
        {
            if(j >= (classCount[i] * numOfFold / fold) &&
               j <= (classCount[i] * (numOfFold + 1) / fold))
            {
                tallInd.push_back(arr[i][j]);
            }
            else
            {
                learnInd.push_back(arr[i][j]);
            }
        }
    }
    return make_pair(learnInd, tallInd);
}

void Net::autoClassification(const QString & spectraDir)
{
    QTime myTime;
    myTime.start();

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "log.txt");
    QFile::remove(helpString);
#if 0
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
#endif




//    cout << spectraDir << endl;
    loadData(spectraDir);

    if(ui->adjustLeanrRateCheckBox->isChecked())
    {
        adjustLearnRate(ui->lowLimitSpinBox->value(),
                        ui->highLimitSpinBox->value()); // or reduce coeff ?
    }

    confusionMatrix.fill(0.);
    switch(Mode)
    {
    case myMode::k_fold:
    {
        crossClassification();
        break;
    }
    case myMode::N_fold:
    {

        leaveOneOutClassification();
        break;
    }
    case myMode::train_test:
    {
        trainTestClassification();
        break;
    }
    case myMode::half_half:
    {
        halfHalfClassification();
        break;
    }
    default: {break;}
    }

    learnNet();
    writeWts();
    drawWts();
    cout <<  "AutoClass: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
}

void Net::autoPCAClassification()
{

}

double Net::getAverageAccuracy()
{
    return this->averageAccuracy;
}

double Net::getKappa()
{
    return this->kappa;
}


void Net::setReduceCoeff(double coeff)
{
    this->ui->rdcCoeffSpinBox->setValue(coeff);
}

void Net::setErrCrit(double in)
{
    ui->critErrorDoubleSpinBox->setValue(in);
}

void Net::setLrate(double in)
{
    ui->learnRateBox->setValue(in);
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
    /// deal with confusionMatrix

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "results.txt");
    ofstream res;
    res.open(helpString.toStdString(), ios_base::app);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
//        for(int j = 0; j < def::numOfClasses(); ++j)
//        {
//            cout << confusionMatrix[i][j] << '\t';
//        }
//        cout << endl;


        const double num = confusionMatrix[i].sum();
        if(num != 0.)
        {
            res << doubleRound(confusionMatrix[i][i] * 100. / num, 2) << '\t';
        }
        else
        {
            res << "pew" << '\t';
        }
    }

    // count averages, kappas
    double corrSum = 0.;
    double wholeNum = 0.;

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        corrSum += confusionMatrix[i][i];
        wholeNum += confusionMatrix[i].sum();
    }
    averageAccuracy = corrSum * 100. / wholeNum;

    // kappa
    double pE = 0.; // for Cohen's kappa
    const double S = confusionMatrix.sum();
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        pE += (confusionMatrix[i].sum() * confusionMatrix.getCol(i).sum()) /
              (S * S);
    }
    kappa = 1. - (1. - corrSum / wholeNum) / (1. - pE);

    res << doubleRound(averageAccuracy, 2) << '\t';
    res << doubleRound(kappa, 3) << '\t';
    res << def::ExpName << endl;
    res.close();

    confusionMatrix.print();
    cout << "average accuracy = " << doubleRound(averageAccuracy, 2) << endl;
    cout << "kappa = " << kappa << endl;
}


void Net::drawWtsSlot()
{
    QString helpString = QFileDialog::getOpenFileName((QWidget * )this,
                                                      tr("wts to draw"),
                                                      def::dir->absolutePath(),
                                                      tr("wts files (*.wts)"));
    if(helpString.isEmpty())
    {
        return;
    }
    drawWts(helpString);
}


void Net::drawWts(QString wtsPath,
                  QString picPath)  //generality
{
    if( dimensionality.size() != 2 ) return;

    if(!QFile::exists(wtsPath))
    {
        wtsPath = def::dir->absolutePath()
                  + slash() + def::ExpName + ".wts";
        if(!QFile::exists(wtsPath))
        {
            cout << "drawWts: bad filePath" << endl;
            return;
        }
    }
    twovector<lineType> tempWeights;
    readWtsByName(wtsPath, &tempWeights);

    matrix drawWts; // 3 arrays of weights
#if 0
    vec tempVec;
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        tempVec.clear();
        for(int j = 0; j < dataMatrix.cols(); ++j)
        {
            tempVec.push_back(tempWeights[0][j][i]); // 0 is for 2 layers
        }
        drawWts.push_back(tempVec);
    }
#else
    drawWts = tempWeights[0];
    drawWts.resizeCols(drawWts.cols() - 1); // fck the bias?
#endif

    if(picPath.isEmpty())
    {
        picPath = wtsPath;
        picPath.replace(".wts", "_wts.jpg"); /// make default suffixes
    }
    drawTemplate(picPath);
    drawArrays(picPath,
               drawWts,
               true);
}

void Net::stopActivity()
{
    stopFlag = 1;
}

void Net::writeWts(const QString & wtsPath)
{
    static int wtsCounter = 0;
    std::ofstream weightsFile;
    if(wtsPath.isEmpty())
    {
        weightsFile.open((def::dir->absolutePath() + slash() +
                         def::ExpName + "_" +
                         QString::number(wtsCounter++) + ".wts").toStdString());


    }
    else
    {
        weightsFile.open(wtsPath.toStdString());
    }

    if(!weightsFile.good())
    {
        cout << "saveWts: cannot open file = " << wtsPath.toStdString() << endl;
        return;
    }

    for(int i = 0; i < dimensionality.size() - 1; ++i) // numOfLayers
    {
        for(int j = 0; j < dimensionality[i + 1]; ++j) // NetLength+1 for bias
        {
            for(int k = 0; k < dimensionality[i] + 1; ++k) // NumOfClasses
            {
                weightsFile << weight[i][j][k] << '\n';
            }
            weightsFile << '\n';
        }
        weightsFile << '\n';
    }
    weightsFile.close();
}

void Net::writeWtsSlot()
{
    //automatization
    int wtsCounter = 0;
    QString helpString;
    if(!autoFlag)
    {
        helpString = QFileDialog::getSaveFileName((QWidget * )this,
                                                  tr("wts to save"),
                                                  def::dir->absolutePath(),
                                                  tr("wts files (*.wts)"));
        if(!helpString.endsWith(".wts", Qt::CaseInsensitive))
        {
            helpString += ".wts";
        }
    }
    else /////////wtf?
    {
        do
        {
            helpString = def::dir->absolutePath()
                         + slash() + def::ExpName + QString::number(wtsCounter) + ".wts";
            ++wtsCounter;
        } while(QFile::exists(helpString));
    }

    if(helpString.isEmpty())
    {
        cout << "saveWtsSlot: no file is chosen to save" << endl;
        return;
    }
    writeWts(helpString);

}

void Net::reset()
{
    QString helpString = ui->dimensionalityLineEdit->text();
    QStringList lst = helpString.split(QRegExp("[., ;]"), QString::SkipEmptyParts);

    const int numOfLayers = lst.length();

    dimensionality.resize(numOfLayers);
    dimensionality[0] = dataMatrix.cols(); ///////////// need to be read before!
    for(int i = 1; i < numOfLayers - 1; ++i)
    {
        dimensionality[i] = lst[i].toInt();
    }
    dimensionality[numOfLayers - 1] = def::numOfClasses();

    weight.resize(numOfLayers - 1);
    for(int i = 0; i < numOfLayers - 1; ++i) // weights from layer i to i+1
    {
        weight[i].resize(dimensionality[i + 1]);
        for(int j = 0; j < dimensionality[i + 1]; ++j) // to j'th in i+1 layer
        {
            // resizing lineType -> fill zeros
            weight[i][j].resize(dimensionality[i] + 1); // from k'th in i layer
        }
    }

    // for delta there are already zeros
    if(ui->backpropRadioButton->isChecked())
    {
        default_random_engine engine;
        std::uniform_int_distribution<int> dist(0, 1000 - 1);
        for(int i = 0; i < dimensionality.size() - 1; ++i) // numOfLayers - 1
        {
            for(int j = 0; j < dimensionality[i + 1]; ++j) // +bias
            {
                for(int k = 0; k < dimensionality[i] + 1; ++k)
                {
                    weight[i][j][k] = (-500 + dist(engine)) / 50000.;  // backprop ~0
                }
            }
        }
    }

}

void Net::testDistances()
{
    PaIntoMatrixByName("1");

    const int NumberOfVectors = dataMatrix.rows();
    const int NetLength = dataMatrix.cols();

    vector<int> NumberOfErrors(def::numOfClasses(), 0);
    matrix averageSpectra(def::numOfClasses(), NetLength, 0.);

    for(int i = 0; i < NumberOfVectors; ++i)
    {
        for(int j = 0; j < NetLength; ++j)
        {
            averageSpectra[ types[i] ] += dataMatrix[i];
        }
    }

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        averageSpectra[i] /= classCount[i];
    }
    PaIntoMatrixByName("2");


    vector<double> distances(def::numOfClasses());
    int outType;
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        for(int j = 0; j < def::numOfClasses(); ++j)
        {
            distances[j] = distance(dataMatrix[i],
                                    averageSpectra[j]);
        }
        outType = std::distance(std::min_element(distances.begin(), distances.end()),
                                distances.begin());
        if(outType != types[i]) ++NumberOfErrors[ types[i] ];
    }
    cout << "NumberOfVectors = " << NumberOfVectors << endl;
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        cout << "NumberOfErrors = " << NumberOfErrors[j] << endl;
    }
    int sum = 0;
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        sum += NumberOfErrors[j];
    }
    cout << "Percentage = " <<  100. * (1. - double(sum)/NumberOfVectors) << endl;
}


void Net::tallNet()
{
    vector<int> indices;
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        indices.push_back(i);
    }
    tallNetIndices(indices);
}

void Net::tallNetIndices(const vector<int> & indices)
{
    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "badFiles.txt");
    matrix localConfusionMatrix(def::numOfClasses(), def::numOfClasses());


    ofstream badFilesStr;
    badFilesStr.open(helpString.toStdString(), ios_base::app);
    for(int i = 0; i < indices.size(); ++i)
    {
        const int outClass = classifyDatum(indices[i]).first;
        if(types[ indices[i] ] != outClass )
        {
            badFilesStr << fileNames[ indices[i] ] << endl;
            if(tallCleanFlag)
            {
                if(Source == source::reals)
                {
                    QFile::remove(def::dir->absolutePath()
                                  + slash() + "SpectraSmooth"
                                  + slash() + fileNames[ indices[i] ]);
                }
                else if(Source == source::winds)
                {
                    QFile::remove(def::dir->absolutePath()
                                  + slash() + "SpectraSmooth"
                                  + slash() + "windows"
                                  + slash() + fileNames[ indices[i] ]);
                }
                eraseDatum(indices[i]);
            }

        }
        localConfusionMatrix[ types[ indices[i] ] ][ outClass ] += 1.;
        confusionMatrix[ types[ indices[i] ] ][ outClass ] += 1.;
    }
    badFilesStr.close();


    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "log.txt");
    ofstream logStream;
    logStream.open(helpString.toStdString(), ios_base::app);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        const double num = localConfusionMatrix[i].sum();
        if(num != 0.)
        {
            logStream << doubleRound( localConfusionMatrix[i][i] * 100. / num, 2) << '\t';
        }
        else
        {
            // no errors if there werent such files - N-fold only
            logStream << "pew" << '\t';
        }
    }

    double corrSum = 0.;
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        corrSum += localConfusionMatrix[i][i];
    }
    averageAccuracy = corrSum / indices.size() * 100.;

    logStream << doubleRound(averageAccuracy, 2) << endl;
    logStream.close();
}




vector<int> exIndices{};
int numGoodNew;

void Net::successiveProcessing()
{
    QString helpString = def::dir->absolutePath()
                         + slash() + "SpectraSmooth"
                         + slash() + "windows";

    vector<int> eraseIndices{};

    numGoodNew = 0;
    confusionMatrix.fill(0.);
    exIndices.clear();

    /// check for no test items
    loadData(helpString);
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        if(fileNames[i].contains("_test"))
        {
            eraseIndices.push_back(i);
        }
    }
    eraseData(eraseIndices);
    eraseIndices.clear();

    /// reduce learning set to (NumClasses * suc::learnSetStay)
    vector<double> count = classCount;
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        if(count[ types[i] ] > suc::learnSetStay)
        {
            eraseIndices.push_back(i);
            count[ types[i] ] -= 1.;
        }
    }
    eraseData(eraseIndices);
    eraseIndices.clear();

    /// consts
    setErrCrit(0.05);
    setLrate(0.05);
    learnNet(); /// get initial weights on train set
    setErrCrit(0.01);
    setLrate(0.01);
//    setErrCrit(ui->critErrorDoubleSpinBox->value() / 5.);
//    setLrate(ui->learnRateBox->value() / 5.);

    cout << "successive: initial learn done" << endl;
//    return;


    lineType tempArr;
    int type = -1;

    QStringList leest = QDir(helpString).entryList({"*_test*"}); /// special generality

//    helpString = "/media/michael/Files/Data/RealTime/windows/SpectraSmooth";
//    QStringList leest = QDir(helpString).entryList(QDir::Files); /// special generality

    for(const QString & fileNam : leest)
    {
        readFileInLine(helpString + slash() + fileNam,
                       tempArr);
        type = typeOfFileName(fileNam);
        successiveLearning(tempArr, type, fileNam);
    }
    averageClassification();
}

void Net::successivePreclean(const QString & spectraPath)
{
    QStringList leest;
    makeFullFileList(spectraPath, leest, {"*train*"});
    // clean from first 2 winds
    cout << "clean first 2 winds" << endl;
    for(auto str : leest)
    {
        if(str.endsWith(".00") || str.endsWith(".01"))
        {
            QFile::remove(spectraPath + slash() + str);
        }
    }

    // clean by learnSetStay
    cout << "clean by learnSetStay" << endl;
    vector<QStringList> leest2;
    makeFileLists(spectraPath, leest2);

    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        auto it = leest2[j].begin();
        for(int i = 0;
            i < leest2[j].size() - suc::learnSetStay * 1.3; /// consts generality
            ++i, ++it)
        {
            QFile::remove(spectraPath + slash() + (*it));
        }
    }
    Source = source::winds;
    Mode = myMode::N_fold;

    // N-fold cleaning
    cout << "N-fold cleaning" << endl;
    tallCleanFlag = true;
    for(int i = 0; i < 3; ++i)
    {
        this->autoClassification(spectraPath);
    }
    tallCleanFlag = false;
}

void Net::successiveLearning(const lineType & newSpectre,
                             const int newType,
                             const QString & newFileName)
{
    /// consider loaded wts
    /// dataMatrix is learning matrix


//    const double errorThreshold = 0.8; /// add to learning set or not - logistic
    const double errorThreshold = 0.5; /// add to learning set or not - softmax

    lineType newData = (newSpectre - averageDatum) / (sigmaVector * loadDataNorm);

    pushBackDatum(newData, newType, newFileName);

    const std::pair<int, double> outType = classifyDatum(dataMatrix.rows() - 1); // take the last

//    for(int i = 0; i < 5; ++i)
//    {
//        cout << doubleRound(newSpectre[i], 2) << "\t";
//    }
//    cout << endl;
//    for(int i = 0; i < 5; ++i)
//    {
//        cout << doubleRound(newData[i], 2) << "\t";
//    }
//    cout << endl;

    confusionMatrix[newType][outType.first] += 1.;
    if(outType.first == newType && outType.second < errorThreshold) /// if good coincidence
    {
        const int num = std::find(types.begin(), types.end(), newType) - types.begin();
        eraseDatum(num);
        ++numGoodNew;
    }
    else
    {
        popBackDatum();
    }

    if(numGoodNew == suc::numGoodNewLimit)
    {
        successiveRelearn();
        numGoodNew = 0;
    }
}

void Net::successiveRelearn()
{
    // decay weights
    const double rat = suc::decayRate;
    for(int i = 0; i < dimensionality.size() - 1; ++i)
    {
        std::for_each(weight[i].begin(),
                      weight[i].end(),
                      [rat](lineType & in)
        {
            in *= 1. - rat;
        });
    }

    learnNet(false); // relearn w/o weights reset
}

void Net::readWtsByName(const QString & fileName,
                        twovector<lineType> * wtsMatrix) //
{
    ifstream wtsStr;
    wtsStr.open(fileName.toStdString());
    if(!wtsStr.good())
    {
        cout << "readWtsByName: wtsStr is not good() " << endl;
        return;
    }
    if(wtsMatrix == nullptr)
    {
        wtsMatrix = &(this->weight);
    }
    else
    {
        (*wtsMatrix).resize(dimensionality.size() - 1);
        for(int i = 0; i < dimensionality.size() - 1; ++i)
        {
            (*wtsMatrix)[i].resize(dimensionality[i + 1]);
            for(int j = 0; j < dimensionality[i + 1]; ++j)
            {
                (*wtsMatrix)[i][j].resize(dimensionality[i] + 1);
            }
        }
    }

    for(int i = 0; i < dimensionality.size() - 1; ++i)
    {
        for(int j = 0; j < dimensionality[i + 1]; ++j)
        {
            for(int k = 0; k < dimensionality[i] + 1; ++k)
            {
                wtsStr >> (*wtsMatrix)[i][j][k];
            }
        }
    }
    wtsStr.close();
}


void Net::readWts()
{
    QString helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget * )NULL,
                                                                       tr("load wts"),
                                                                       def::dir->absolutePath(),
                                                                       tr("wts files (*.wts)")));
    if(helpString == "")
    {
        QMessageBox::information((QWidget * )this, tr("Warning"), tr("No wts-file was chosen"), QMessageBox::Ok);
        return;
    }
    readWtsByName(helpString, &(this->weight));
}


void Net::leaveOneOutClassification()
{
    if(Source == source::pca)
    {
        ofstream outStr;
        outStr.open((def::dir->absolutePath()
                    + slash() + "pcaRes.txt").toStdString());
        // auto pca classification
        for(int i = ui->autoPCAMaxSpinBox->value();
            i >= ui->autoPCAMinSpinBox->value();
            i -= ui->autoPCAStepSpinBox->value())
        {
            cout << "numOfPc = " << i  << " \t";
            dataMatrix.resizeCols(i);

            adjustLearnRate(ui->lowLimitSpinBox->value(),
                            ui->highLimitSpinBox->value());

            leaveOneOut();
            outStr << i << "\t" << averageAccuracy << endl;
        }
        outStr.close();
    }
    else
    {
        cout << "Net: autoclass (max " << dataMatrix.rows() << "):" << endl;
        leaveOneOut();
    }

    return;
    if(!ui->openclCheckBox->isChecked())
    {
        leaveOneOut();
    }
    else
    {
//        leaveOneOutCL();
    }
}

void Net::crossClassification()
{
    const int numOfPairs = ui->numOfPairsBox->value();
    const int fold = ui->foldSpinBox->value();

    vector<vector<int>> arr;
    arr.resize(def::numOfClasses(), {});
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        arr[ types[i] ].push_back(i);
    }
    cout << "Net: autoclass (max " << numOfPairs << "):" << endl;

    for(int i = 0; i < numOfPairs; ++i)
    {
        cout << i + 1;
        cout << " "; cout.flush();

        // mix arr for one "pair"-iteration
        for(int i = 0; i < def::numOfClasses(); ++i)
        {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(arr[i].begin(),
                         arr[i].end(),
                         std::default_random_engine(seed));
        }

        /// new with indices
        for(int numFold = 0; numFold < fold; ++numFold)
        {
//            if(tallCleanFlag)
//            {
//                /// remake for k-fold tallCleanFlag
//                arr.resize(def::numOfClasses(), {});
//                for(int i = 0; i < dataMatrix.rows(); ++i)
//                {
//                    arr[ types[i] ].push_back(i);
//                }
//                for(int i = 0; i < def::numOfClasses(); ++i)
//                {
//                    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//                    std::shuffle(arr[i].begin(),
//                                 arr[i].end(),
//                                 std::default_random_engine(seed));
//                }
//            }

            auto sets = makeIndicesSetsCross(arr, numFold);
            learnNetIndices(sets.first);
            tallNetIndices(sets.second);
        }

        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = 0;
            return;
        }
    }
    cout << endl;
    cout << "cross classification - ";
    averageClassification();
}

void Net::halfHalfClassification()
{
    vector<int> learnIndices;
    vector<int> tallIndices;

    for(int i = 0; i < dataMatrix.rows() / 2; ++i)
    {
        learnIndices.push_back(i);
        tallIndices.push_back(i + dataMatrix.rows() / 2);
    }
    if(learnIndices.empty() || tallIndices.empty())
    {
        cout << "teainTest: indicesArray empty, return" << endl;
        return;
    }
    learnNetIndices(learnIndices);
    tallNetIndices(tallIndices);
    cout << "half-half classification - ";
    averageClassification();
}

void Net::trainTestClassification(const QString & trainTemplate,
                                  const QString & testTemplate)
{
    vector<int> learnIndices;
    vector<int> tallIndices;
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        if(fileNames[i].contains(trainTemplate))
        {
            learnIndices.push_back(i);
        }
        else if(fileNames[i].contains(testTemplate))
        {
            tallIndices.push_back(i);
        }
    }
    if(learnIndices.empty() || tallIndices.empty())
    {
        cout << "teainTest: indicesArray empty, return" << endl;
        return;
    }
    learnNetIndices(learnIndices);
    tallNetIndices(tallIndices);
    cout << "train-test classification - ";
    averageClassification();
}

void Net::leaveOneOut()
{
    vector<int> learnIndices;
    int i = 0;
    adjustLearnRate(ui->lowLimitSpinBox->value(),
                    ui->highLimitSpinBox->value()); /// ~40-60
    while(i < dataMatrix.rows())
    {
        cout << i + 1;
        cout << " "; cout.flush();

        /// iota ?
        learnIndices.clear();
        for(int j = 0; j < dataMatrix.rows(); ++j)
        {
            if(j == i) continue;
            learnIndices.push_back(j);
        }
        learnNetIndices(learnIndices);
        tallNetIndices({i});

        /// not so fast
        /// what with softmax/logistic ?
        if(tallCleanFlag && epoch < ui->lowLimitSpinBox->value() && activation == logistic)
        {
            adjustLearnRate(ui->lowLimitSpinBox->value(),
                            ui->highLimitSpinBox->value());
        }
        ++i;

    }
    cout << endl;
    cout << "N-fold cross-validation:" << endl;
    averageClassification();
}


void Net::PaIntoMatrix()
{
    QString helpString = QDir::toNativeSeparators(QFileDialog::getOpenFileName((QWidget * )NULL, tr("load PA"), def::dir->absolutePath(), tr("PA files (*.pa)")));
    if(helpString == "")
    {
        QMessageBox::information((QWidget * )this, tr("Information"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }
    readPaFile(helpString,
               dataMatrix,
               types,
               fileNames,
               classCount);

}


void Net::PaIntoMatrixByName(const QString & fileName)
{

    QString helpString = def::dir->absolutePath()
                         + slash() + "PA"
                         + slash() + fileName;
    if(!fileName.contains(".pa"))
    {
        helpString += ".pa";
    }

    if(!QFile::exists(helpString))
    {
        cout << "PaIntoMatrixByName: cannot open file\n" << helpString << endl;
    }

    readPaFile(helpString,
               dataMatrix,
               types,
               fileNames,
               classCount);
}

void Net::loadDataSlot()
{
    QString helpString = QFileDialog::getExistingDirectory((QWidget * )NULL,
                                                           tr("load data"),
                                                           def::dir->absolutePath());
    if(helpString.isEmpty())
    {
        QMessageBox::information((QWidget * )this,
                                 tr("Information"),
                                 tr("No directory was chosen"),
                                 QMessageBox::Ok);
        return;
    }
    loadData(helpString,
             ui->rdcCoeffSpinBox->value());
}

void Net::pushBackDatum(const lineType & inDatum,
                      const int & inType,
                      const QString & inFileName)
{
    dataMatrix.push_back(inDatum);
    classCount[inType] += 1.;
    types.push_back(inType);
    fileNames.push_back(inFileName);
}

void Net::popBackDatum()
{
    dataMatrix.pop_back();
    classCount[types.back()] -= 1.;
    types.pop_back();
    fileNames.pop_back();
}

void Net::eraseDatum(const int & index)
{
    dataMatrix.eraseRow(index);
    classCount[ types[index] ] -= 1.;
    types.erase(types.begin() + index);
    fileNames.erase(fileNames.begin() + index);
}

void Net::eraseData(const vector<int> & indices)
{
    dataMatrix.eraseRows(indices);
    eraseItems(fileNames, indices);
    for(int index : indices)
    {
        classCount[ types[index] ] -= 1.;
    }
    eraseItems(types, indices);
}


// like readPaFile from library.cpp
void Net::loadData(const QString & spectraPath,
                   double rdcCoeff)
{
    vector<QStringList> leest;
    makeFileLists(spectraPath, leest);

    dataMatrix = matrix();
    classCount.resize(def::numOfClasses(), 0.);
    types.clear();
    fileNames.clear();

    lineType tempArr;
    cout << spectraPath << endl;
    for(int i = 0; i < leest.size(); ++i)
    {
        classCount[i] = 0.;
        for(const QString & fileName : leest[i])
        {
            readFileInLine(spectraPath + slash() + fileName,
                           tempArr);
            if(rdcCoeff != 1.)
            {
                tempArr /= rdcCoeff;
            }
            pushBackDatum(tempArr, i, fileName);
        }
    }
    cout << "loadDataNorm = " << loadDataNorm << endl;
#if 1
    averageDatum = dataMatrix.averageRow();
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        dataMatrix[i] -= averageDatum;
    }
    dataMatrix.transpose();
    sigmaVector.resize(dataMatrix.rows());
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        sigmaVector[i] = sigma(dataMatrix[i]);
        if(sigmaVector[i] != 0.)
        {
            // to equal variance, 10 for reals, 5 winds
            dataMatrix[i] /= sigmaVector[i] * loadDataNorm;
        }
    }
    dataMatrix.transpose();
#endif
#if 0
    dataMatrix.transpose();
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        const double a = dataMatrix[i].max();
        const double b = dataMatrix[i].min();
        dataMatrix[i] -= (a + b) / 2.;
        dataMatrix[i] /= (a - b);
    }
    dataMatrix.transpose();

#endif
}

//void Net::loadDataFromFolder(const QString & spectraPath)
//{
//    makePaFile(spectraDir, listToWrite, coeff, helpString);
//}

double HopfieldActivation(double x, double temp)
{
    return ((1./(1. + exp(-x/temp/300.)) * 2.) - 1.) * temp;
}

void Net::methodSetParam(int a, bool ch)
{
    if(!ch) return;
    if(group3->button(a)->text().contains("delta")) ///generality
    {
        ui->epochSpinBox->setValue(250);
        ui->tempBox->setValue(10);
        ui->learnRateBox->setValue(0.05);
//        ui->critErrorDoubleSpinBox->setValue(0.1);

        ui->dimensionalityLineEdit->setText("0 0");
        ui->dimensionalityLineEdit->setReadOnly(true);

        ui->numOfLayersSpinBox->setValue(2);
        ui->numOfLayersSpinBox->setReadOnly(true);

    }
    else if(group3->button(a)->text().contains("backprop"))  ///generality
    {
//        cout << "backprop button pressed" << endl;
        ui->epochSpinBox->setValue(300);
        ui->tempBox->setValue(2);
        ui->learnRateBox->setValue(1.0);
        ui->critErrorDoubleSpinBox->setValue(0.05);

        ui->numOfLayersSpinBox->setValue(3);
        ui->numOfLayersSpinBox->setReadOnly(false);

        ui->dimensionalityLineEdit->setText("0 20 0");
        ui->dimensionalityLineEdit->setReadOnly(false);
    }
    else if(group3->button(a)->text().contains("deepBelief"))  ///generality
    {
        ui->epochSpinBox->setValue(300);
        ui->tempBox->setValue(2);
        ui->learnRateBox->setValue(1.0);
        ui->critErrorDoubleSpinBox->setValue(0.05);

        ui->numOfLayersSpinBox->setValue(3);
        ui->numOfLayersSpinBox->setReadOnly(false);

        ui->dimensionalityLineEdit->setText("0 22 0");
        ui->dimensionalityLineEdit->setReadOnly(false);
    }
//    reset();
}

void Net::learnNet(const bool resetFlag)
{
    vector<int> mixNum(dataMatrix.rows());
    std::iota(mixNum.begin(), mixNum.end(), 0);
    learnNetIndices(mixNum, resetFlag);
}

void Net::learnNetIndices(vector<int> mixNum,
                          const bool resetFlag)
{
    QTime myTime;
    myTime.start();

    if(resetFlag)
    {
//        cout << "reset" << endl;
        reset();
    }


    const int numOfLayers = dimensionality.size();
    vector<valarray<double>> deltaWeights(numOfLayers);
    vector<valarray<double>> output(numOfLayers);
    for(int i = 0; i < numOfLayers; ++i)
    {
        deltaWeights[i].resize(dimensionality[i]); // fill zeros
        output[i].resize(dimensionality[i] + 1); // for biases
    }

    const double critError = ui->critErrorDoubleSpinBox->value();
    const double temp = ui->tempBox->value();
    const double learnRate = ui->learnRateBox->value();
    const bool deltaFlag = ui->deltaRadioButton->isChecked();
//    const double momentum = ui->momentumDoubleSpinBox->value(); //unused yet
    double currentError = critError + 0.1;

    int type = 0;



    /// edit due to Indices
    vector <double> normCoeff;
    const double helpMin = *std::min_element(classCount.begin(),
                                             classCount.end());
//    const double helpMin = std::accumulate(classCount.begin(),
//                                           classCount.end(),
//                                           0.);


    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        normCoeff.push_back(helpMin / classCount[i]);
    }


    if(ui->deepBeliefRadioButton->isChecked())
    {
//        prelearnDeepBelief();
    }


    epoch = 0;
    while(currentError > critError && epoch < ui->epochSpinBox->value())
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        currentError = 0.0;
        // mix the sequence of input vectors
        std::shuffle(mixNum.begin(),
                     mixNum.end(),
                     std::default_random_engine(seed));

        for(const int & index : mixNum)
        {
            // add data
            std::copy(begin(dataMatrix[index]),
                      end(dataMatrix[index]),
                      begin(output[0]));


            // add bias
            output[0][output[0].size() - 1] = 1.;
            type = types[index];

            //obtain outputs
            for(int i = 1; i < numOfLayers; ++i)
            {
                for(int j = 0; j < dimensionality[i]; ++j)
                {
                    output[i][j] = prod(weight[i - 1][j], output[i-1]); // bias included
                }
                output[i] = activation(output[i], temp);

                output[i][ dimensionality[i] ] = 1.; //bias, unused for the highest layer
            }

            //error in the last layer
            {
                double err = 0.;
                for(int j = 0; j < dimensionality.back(); ++j)
                {
                    err += pow((output.back()[j] - int(type == j) ), 2.);
                }
                err = sqrt(err);
                if(def::errType == errorNetType::SME)
                {
                    currentError += err;
                }
                else if(def::errType == errorNetType::maxDist)
                {
                    currentError = max(err, currentError);
                }
            }
#if 0
            /// check weight
            if(!deltaFlag) /// enum !
            {
                //count deltaweights (used for backprop only)
                //for the last layer
                for(int j = 0; j < dimensionality[numOfLayers-1]; ++j)
                {
                    deltaWeights[numOfLayers-1][j] = -1. / temp
                                                     * output[numOfLayers-1][j]
                                                     * (1. - output[numOfLayers-1][j])
                            * ((type == j) - output[numOfLayers-1][j]); //~0.1
                }

                //for the other layers, besides the input one, upside->down
                for(int i = numOfLayers - 2; i >= 1; --i)
                {
                    for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
                    {
                        deltaWeights[i][j] = 0.;
                        for(int k = 0; k < dimensionality[i + 1]; ++k) //connected to the higher-layer
                        {
                            deltaWeights[i][j] += deltaWeights[i + 1][k] * weight[i][j][k];
                        }
                        deltaWeights[i][j] *= 1. / temp
                                              * output[i][j]
                                              * (1. - output[i][j]);
                    }
                }
            }
#endif



            if(deltaFlag)
            {
                // numOfLayers = 2 and i == 0 in this case
                // simplified

                for(int j = 0; j < def::numOfClasses(); ++j)
                {
                    weight[0][j] += output[0]
                            * (learnRate * normCoeff[type]
                               * ((type == j) - output[1][j])
//                            * (output[1][j] * (1. - output[1][j])) * 6. // derivative
                            );
                }

            }
#if 0
            else /// backprop check weight
            {

                // count new weights using deltas
                for(int i = 0; i < numOfLayers - 1; ++i)
                {
                    for(int j = 0; j < dimensionality[i] + 1; ++j) // +1 for bias? 01.12.2014
                    {
                        for(int k = 0; k < dimensionality[i + 1]; ++k)
                        {
                            weight[i][j][k] -= learnRate
                                               * normCoeff[type]
                                               * output[i][j]
                                               * deltaWeights[i + 1][k];
                        }
                    }
                }
            }
#endif
        }
        ++epoch;
        //count error
        if(def::errType == errorNetType::SME)
        {
            currentError /= mixNum.size();
        }
        this->ui->currentErrorDoubleSpinBox->setValue(currentError);

//        cout << "epoch = " << epoch << "\terror = " << doubleRound(currentError, 4) << endl;
    }
    cout << "epoch = " << epoch << "\terror = " << doubleRound(currentError, 4) << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec"  << endl;

//    writeWts();
}



std::pair<int, double> Net::classifyDatum(const int & vecNum)
{
    const int type = types[vecNum]; // true class
    const int numOfLayers = dimensionality.size();
    const double temp = ui->tempBox->value();

    vector<valarray<double>> output(numOfLayers);
    output[0].resize(dimensionality[0] + 1); // +1 for biases

    std::copy(begin(dataMatrix[vecNum]),
              end(dataMatrix[vecNum]),
              begin(output[0]));
    output[0][output[0].size() - 1] = 1.; //bias

    for(int i = 1; i < numOfLayers; ++i)
    {
        output[i].resize(dimensionality[i] + 1);
        for(int j = 0; j < dimensionality[i]; ++j) //higher level, w/o bias
        {
            output[i][j] = prod(weight[i-1][j], output[i-1]); // bias included
        }
        output[i] = activation(output[i], temp);
        output[i][ dimensionality[i] ] = 1.; //bias, unused for the highest layer
    }

#if 0
    /// cout results
    cout << "type = " << type << '\t' << "(";
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        cout << doubleRound(output[numOfLayers - 1][i], 3) << '\t';
    }
    cout << ") " << fileNames[vecNum] << "   ";
    for(int i = 0; i < 5; ++i)
    {
        cout << doubleRound(dataMatrix[vecNum][i], 2) << "\t";
    }
    cout << endl;
#endif

    /// effect on successive procedure
    double res = 0.;
    for(int j = 0; j < dimensionality.back(); ++j)
    {
        res += pow((output.back()[j] - int(type == j) ), 2.);
    }
    res = sqrt(res);

    return std::make_pair(std::max_element(begin(output.back()),
                                           end(output.back()) - 1)  // -bias
                          - begin(output.back()),
                          res);


    // more general
//    return std::distance(output.back().begin(),
//                         std::max_element(output.back().begin(),
//                                          output.back().end()));

//    for(int k = 0; k < dimensionality[numOfLayers - 1]; ++k)
//    {
//        if(k != type && output[numOfLayers - 1] [k] >= output[numOfLayers - 1] [type])
//        {
//            return false;
//        }
//    }
//    return true;
}


void Net::SVM()
{
    QString helpString = def::dir->absolutePath()
            + slash() + "PA"
            + slash() + "output1";
    FILE * out = fopen(QDir::toNativeSeparators(helpString), "w");
    fclose(out);
//    QString spectraDir = QDir::toNativeSeparators(def::dir->absolutePath() + slash() + "SpectraSmooth"));
    QString spectraDir = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose spectra dir"),
                                                           def::dir->absolutePath());
    if(spectraDir.isEmpty())
    {
        spectraDir = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "SpectraSmooth");
    }
    if(spectraDir.isEmpty())
    {
        cout << "spectraDir for SVM is empty" << endl;
        return;
    }

    for(int i = 0; i < ui->numOfPairsBox->value(); ++i)
    {
        makePaStatic(spectraDir,
                     ui->foldSpinBox->value(),
                     ui->rdcCoeffSpinBox->value(), true);

        helpString = def::dir->absolutePath() + slash() + "PA";
        helpString.prepend("cd ");
        helpString += " && svm-train -t "
                      + QString::number(ui->svmKernelSpinBox->value())
                      + " svm1 && svm-predict svm2 svm1.model output >> output1";
        system(helpString.toStdString().c_str());

        helpString = def::dir->absolutePath() + slash() + "PA";
        helpString.prepend("cd ");
        helpString += " && svm-train -t "
                      + QString::number(ui->svmKernelSpinBox->value())
                      + " svm2 && svm-predict svm1 svm2.model output >> output1";
        system(helpString.toStdString().c_str());
    }

    helpString = def::dir->absolutePath() + slash() + "PA" + slash() + "output1";

    double helpDouble, average = 0.;

    QFile file(helpString);
    if(!file.open(QIODevice::ReadOnly)) return;
    int lines = 0;
    while(!file.atEnd())
    {
        helpString = file.readLine();
        if(!helpString.contains(QRegExp("[% = ]"))) break;
        helpString = helpString.split(QRegExp("[% = ]"),
                                      QString::SkipEmptyParts)[1]; //generality [1]
        helpDouble = helpString.toDouble();
        average += helpDouble;
        ++lines;
    }
    average /= lines;
    cout << average << endl;
    file.close();


    ofstream outStr;
    helpString = QDir::toNativeSeparators(def::dir->absolutePath() + slash() + "results.txt");
    outStr.open(helpString.toStdString(), ios_base::app);
    outStr << "\nSVM\t";
    outStr << doubleRound(average, 2) << " %" << endl;
    outStr.close();
}


/// principal component analisys
/// remake
void Net::pca()
{
    QTime myTime;
    myTime.start();

    const int NumberOfVectors = dataMatrix.rows();
    const int NetLength = dataMatrix.cols();

    matrix centeredMatrix;
    centeredMatrix = matrix::transpose(dataMatrix); // rows = spectral points, cols - numOfPattern

    QString helpString;

    //count covariations
    //centered matrix
    for(int i = 0; i < NetLength; ++i)
    {
        centeredMatrix[i] -= mean(centeredMatrix[i]);
    }


    //covariation between different spectra-bins
    double trace = 0.;
    for(int i = 0; i < NetLength; ++i)
    {
        trace += variance(centeredMatrix[i]);
    }
    cout << "trace covMatrix = " << trace << endl;

    // count eigenvalue decomposition
    matrix eigenVectors;
    lineType eigenValues;
    const double eigenValuesTreshold = pow(10., -8.);
    const int numOfPc = ui->pcaNumberSpinBox->value();

    svd(centeredMatrix,
        eigenVectors,
        eigenValues,
        centeredMatrix.rows(),
        eigenValuesTreshold,
        numOfPc);


    double sum1 = 0.;
    for(int k = 0; k < numOfPc; ++k)
    {
        sum1 += eigenValues[k];
    }
    cout << "Part of dispersion explained = " << sum1 * 100./double(trace) << " %" << endl;
    cout << "Number of Components = " << numOfPc << endl;

    //memory for pcaProjections
    centeredMatrix.transpose();
    matrix pcaMatrix(NumberOfVectors, numOfPc);

    pcaMatrix = centeredMatrix * eigenVectors;

    for(int j = 0; j < NumberOfVectors; ++j) //i->j
    {
        helpString = def::dir->absolutePath()
                     + slash() + "SpectraSmooth"
                     + slash() + "PCA"
                     + slash() + fileNames[j];
        writeFileInLine(helpString,
                        pcaMatrix[j]);

    }

    eigenVectors = matrix::transpose(eigenVectors);
    eigenVectors.resizeRows(3);


    drawTemplate(def::dir->absolutePath()
                 + slash() + "Help"
                 + slash() + def::ExpName + "_pcas.jpg");
    drawArrays(def::dir->absolutePath()
               + slash() + "Help"
               + slash() + def::ExpName + "_pcas.jpg",
               eigenVectors,
               true);

    cout << "pca: time elapsed = " << myTime.elapsed() / 1000. << " sec" << endl;
#if 0

    matrix differenceMatrix (NumberOfVectors, NumberOfVectors);
    //count distances between different spectre-vectors (projections on first numOfPc PCs)
    for(int h = 0; h < NumberOfVectors; ++h)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            if(ui->sammonComboBox->currentIndex() == 0)
            {
                differenceMatrix[h][j] = distance(dataMatrix[h].data(),
                                                  dataMatrix[j].data(),
                                                  NetLength);  //wet data
            }
            else if(ui->sammonComboBox->currentIndex() == 1)
            {
                differenceMatrix[h][j] = distance(pcaMatrix[h],
                                                  pcaMatrix[j],
                                                  numOfPc); //by some PC
            }
        }
    }


    if(ui->pcaCheckBox->isChecked())
    {
        int * vecTypes = new int [NumberOfVectors];
        for(int i = 0; i < NumberOfVectors; ++i)
        {
            vecTypes[i] = dataMatrix[i][NetLength+1];
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

        ////////////////// matrix vs double **

        // Kohonen(dataMatrix, eigenVectors, avProj, NumberOfVectors, NetLength);

        delete [] avProj;
        ui->sammonLineEdit->clear();
    }
#endif
}

/// naive Bayes classifier
void Net::learnBayesIndices(vector<int> mixNum)
{
    QTime myTime;
    myTime.start();

    int type = 0;

    // apriori probability of classes
    vector <double> aprioriClass;
    const double helpMax = *std::max_element(classCount.begin(),
                                             classCount.end());

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        aprioriClass.push_back(classCount[i] / helpMax);
    }

}










/// Sammon and sheet
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
    double lambda = ui->lambdaDoubleSpinBox->value(); // how to estimate - add into UI
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
#if 0
    srand(time(NULL));
    double ** distNew = new double * [size];
    for(int i = 0; i < size; ++i)
    {
        distNew[i] = new double [size];
    }

    coords.resize(size, 3);

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
#endif
}

void Net::drawSammon() //uses coords array
{
#if 0
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

    QString helpString = def::dir->absolutePath() + slash() + "Help" + slash() + "Sammon-" + ui->sammonLineEdit->text() + ".jpg";

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
#endif
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
#if 0
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

#endif

}

void Net::neuronGas()
{
}


void Net::optimizeChannelsSet() /// CAREFUL
{
#if 0
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
            NetLength = spLength * (NetLength/spLength + 1); //// CAREFUL
            dimensionality[0] = NetLength;
            break;
        }
    }


    helpString = def::dir->absolutePath() + "/optimalChannels.txt";
    outStream.open(helpString.toStdString().c_str(), ios_base::app);

    helpString = def::ExpName;
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


    outStream << helpString.toStdString() << endl;
    outStream.close();
//    QMessageBox::information((QWidget * )this, tr("Optimization results"), helpString, QMessageBox::Ok);
#endif
}


#if 0
void Net::leaveOneOutCL()
{

    QTime myTime;
    myTime.start();
    cout << "leaveOneOutCL started" << endl;
    NumberOfErrors = new int[def::numOfClasses()];
    helpString = "";
    for(int i = 0; i < def::numOfClasses(); ++i)
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
    const char  * kernel_Source == (const char * )kernelFromFile("/home/michael/Qt/Projects/SecondGUI/kernels.cl"); //generality
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
//    __global int numOfClasses,
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
                              &temp,
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
                              &def::numOfClasses(),
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
                              sizeof(cl_double) * def::numOfClasses() * (NetLength + 1),
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
                              sizeof(cl_double) * def::numOfClasses(),
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
                              sizeof(cl_double) * def::numOfClasses(),
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
                              sizeof(cl_int) * def::numOfClasses(),
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
#endif


void Net::Hopfield()
{
#if 0
    double maxH = 0.;
    double * output1 = new double [NetLength];
    double * output2 = new double [NetLength];
    const int NumberOfVectors = dataMatrix.rows();
    MakePa  * mkPa = new MakePa(def::dir->absolutePath() + slash() + "SpectraSmooth");
    mkPa->setRdcCoeff(10);
    mkPa->makePaSlot();

    PaIntoMatrixByName("all");
    for(int i = 0; i < NetLength; ++i)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            maxH = fmax(fabs(dataMatrix[j][i]), maxH);
        }
    }
    cout << "maxH = " << maxH << endl;
//    learnNet();

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
                weightH[i][j] += dataMatrix[k][i] * dataMatrix[k][j];
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

    double * outputClass = new double [def::numOfClasses()];
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
            output1[j] = dataMatrix[i][j];
            output2[j] = dataMatrix[i][j] + 1;
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
        attractorsH[i][NetLength] = dataMatrix[i][NetLength+1];

    }

    for(int i = 0; i < NumberOfVectors; ++i)
    {
        for(int j = 0; j < NumberOfVectors; ++j)
        {
            if( distance(attractorsH[i], attractorsH[j], NetLength) > 1.) cout << " x  ";
            else cout << "000 ";
        }
        cout << dataMatrix[i][NetLength+1] << endl;
    }
//    return;
//    learnNet();


    PaIntoMatrixByName("2");

    for(int i = 0; i < NumberOfVectors; ++i) //number of vector to classify
    {
        for(int j = 0; j < NetLength; ++j)
        {
            output1[j] = dataMatrix[i][j];
            output2[j] = dataMatrix[i][j] + 1;
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
        if(dataMatrix[i][NetLength+1] == attractorsH[buff][NetLength]) answer = true;
        else answer = false;
        cout << "classification: dist = " << sumH << " attrNum = " << buff << " class coincidence = " << answer  << endl;

//        //classifying perceptron
//        type = int(dataMatrix[i][NetLength+1]);
//        for(int j = 0; j < def::numOfClasses(); ++j) //calculate output //2 = numberOfTypes
//        {
//            outputClass[j] = 0.;
//            for(int h = 0; h < NetLength; ++h)
//            {
//                outputClass[j] += weight[j][h] * output1[h];
//            }
//            outputClass[j] += weight[j][NetLength] * dataMatrix[i][NetLength];
//            outputClass[j] = softmax(outputClass[j], temp); // unlinear conformation
//        }
//        answer = true;
//        for(int k = 0; k<def::numOfClasses(); ++k)
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
#endif

}


void Net::prelearnDeepBelief() //uses weights, matrix, dimensionality, numOfLayers
{
#if 0
    /// I dont use this, NEED CHECK weight
//    int  * mixNum = new int [NumberOfVectors];
    vector<int> mixNum;
    const int NumberOfVectors = dataMatrix.rows();
    const int numOfLayers = dimensionality.size();

    for(int i = 0; i < NumberOfVectors; ++i)
    {
        mixNum.push_back(i);
    }
    int a1, a2, buffer; //for mixNum mixing
    int index;

    //weights between current hidden layer and virtual/temp layer
    double ** tempWeight;
    matrixCreate(&tempWeight, dimensionality[0] + 1, dimensionality[0] + 1); // +1 for bias

    double ** output = new double * [numOfLayers]; //data and global output together
    for(int i = 0; i < numOfLayers; ++i)
    {
        output[i] = new double [dimensionality[i] + 1];
    }
    double ** tempOutput;
    matrixCreate(&tempOutput, 3, dimensionality[0] + 1);


    double ** tempDeltaWeights = new double * [3];
    for(int i = 0; i < 3; ++i)
    {
        tempDeltaWeights[i] = new double [dimensionality[0] + 1];
    }

//    double * normCoeff = new double [def::numOfClasses()];
    vector < double > normCoeff;
    double helpMin = classCount[0];
    for(int i = 1; i < def::numOfClasses(); ++i)
    {
        helpMin = min(helpMin, classCount[i]);
    }
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        normCoeff.push_back(helpMin/classCount[i]);
    }




    double currentError = 2 * ui->critErrorDoubleSpinBox->value();
    const double temp = ui->tempBox->value();
    const double learnRate = ui->learnRateBox->value();
    for(int numLayer = 1; numLayer < numOfLayers - 1; ++numLayer) //for every hidden layer
    {
        cout << "layer " << numLayer << " starts to learn" << endl;
        epoch = 0;
        //reset tempWeight
        for(int i = 0; i < dimensionality[0] + 1; ++i)
        {
            for(int j = 0; j < dimensionality[0]; ++j)
            {
                tempWeight[i][j] = (-500 + rand()%1000)/50000.;
            }
        }
        while(currentError > ui->critErrorDoubleSpinBox->value()
              && epoch < ui->epochSpinBox->value()) //while not good approximation
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
//                cout << num << " "; cout.flush();
                index = mixNum[num];

                //count input using all previously learned hidden layers
                //1) set initial Layer
                for(int j = 0; j < dimensionality[0]; ++j)
                {
                    output[0][j] = dataMatrix[index][j];
                }
                output[0][dimensionality[0]] = 1.; //bias

                //2) count ascend only on precomputed hidden layers
                for(int i = 1; i < numLayer; ++i)
                {
    //#pragma omp parallel for schedule(dynamic) num_threads(dimensionality[i]) shared(output)
                    for(int j = 0; j < dimensionality[i]; ++j)
                    {
                        output[i][j] = 0.;

                        for(int k = 0; k < dimensionality[i-1] + 1; ++k) //-1 for prev.layer, +1 for bias
                        {
                            output[i][j] += weight[i-1][k][j] * output[i-1][k];
                        }

                        output[i][j] = softmax(output[i][j], temp);
                    }
                    output[i][dimensionality[i]] = 1.; //bias, unused for the highest layer
                }

                memcpy(tempOutput[0], output[numLayer-1], (dimensionality[numLayer-1] + 1) * sizeof(double)); //+1 for bias

                //count outputs in current hidden layer
                for(int j = 0; j < dimensionality[numLayer]; ++j) //for hidden layer dim[numLayer]
                {
                    tempOutput[1][j] = 0.;
                    for(int k = 0; k < dimensionality[numLayer - 1] + 1; ++k) //for input layer dim[numLayer-1] + bias
                    {
                        tempOutput[1][j] += weight[numLayer - 1][k][j] * tempOutput[0][k];
                    }

                    tempOutput[1][j] = softmax(tempOutput[1][j], temp);
                }
                tempOutput[1][dimensionality[numLayer]] = 1.; //bias in hidden layer


                //count outputs in virtual/temp layer
                for(int j = 0; j < dimensionality[numLayer - 1]; ++j) //for virtual/temp output dim[numLayer - 1]
                {
                    tempOutput[2][j] = 0.;
                    for(int k = 0; k < dimensionality[numLayer] + 1; ++k) //for hidden learning layer dim[numLayer] + bias
                    {
                        tempOutput[2][j] += tempWeight[k][j] * tempOutput[1][k]; //tempWeight is important
                    }

                    tempOutput[2][j] = softmax(tempOutput[2][j], temp);
                }
//                matrixPrint(tempOutput, 3, 10);

//                cout << "outputs ready" << endl;



                //error in the last layer
                for(int j = 0; j < dimensionality[numLayer-1]; ++j) //virtual/temp output layer
                {
                    currentError += distance(tempOutput[2], tempOutput[0], dimensionality[numLayer - 1]);
                }


                //count deltaweights
                for(int j = 0; j < dimensionality[numLayer - 1]; ++j)//for the last virtual/temp layer
                {
                    tempDeltaWeights[2][j] = -1./temp * tempOutput[2][j] * (1. - tempOutput[2][j]) * (tempOutput[0][j] - tempOutput[2][j]);
                }

                //for the only hidden layer
                for(int j = 0; j < dimensionality[numLayer] + 1; ++j) // + bias weight
                {
                    tempDeltaWeights[1][j] = 0.;
                    for(int k = 0; k < dimensionality[numLayer - 1]; ++k) //connected to the higher-layer
                    {
                        tempDeltaWeights[1][j] += tempDeltaWeights[2][k] * tempWeight[j][k]; //tempWeigth
                    }
                    tempDeltaWeights[1][j]  *= 1./temp * tempOutput[1][j] * (1. - tempOutput[1][j]);
                }
//                cout << "deltaweights ready" << endl;

                //adjust tempWeight
                for(int j = 0; j < dimensionality[numLayer] + 1; ++j) //hidden
                {
                    for(int k = 0; k < dimensionality[numLayer - 1]; ++k) //virtual/temp
                    {
                        tempWeight[j][k] -= learnRate * tempDeltaWeights[2][k] * tempOutput[1][j];// * normCoeff[type]; //normCoeff or not?
                    }
                }
                //adjust weight
                for(int j = 0; j < dimensionality[numLayer - 1] + 1; ++j) //input
                {
                    for(int k = 0; k < dimensionality[numLayer]; ++k) //hidden
                    {
                        weight[numLayer-1][j][k] -= learnRate  * tempDeltaWeights[1][k] * tempOutput[0][j];// * normCoeff[type]; //normCoeff or not?
                    }
                }
//                cout << "weights ready" << endl;
                qApp->processEvents();
                if(stopFlag)
                {
                    matrixDelete(&output, numOfLayers);
                    matrixDelete(&tempDeltaWeights, 3);
                    matrixDelete(&tempOutput, 3);
                    matrixDelete(&tempWeight, dimensionality[0] + 1);

                    stopFlag = 0;
                    return;
                }
            }

            ++epoch;
            currentError /= NumberOfVectors;
            currentError = sqrt(currentError);
            cout << endl;
            cout << "epoch = " << epoch << "\terror = " << currentError << endl;
            this->ui->currentErrorDoubleSpinBox->setValue(currentError);
        }

        cout << "layer " << numLayer << " finished to learn" << endl;
        cout << "epoches = " << epoch << " error = " << currentError << endl;
    }

    matrixDelete(&output, numOfLayers);
    matrixDelete(&tempDeltaWeights, 3);
    matrixDelete(&tempOutput, 3);
    matrixDelete(&tempWeight, dimensionality[0] + 1);
#endif
}
