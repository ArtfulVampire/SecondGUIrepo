#include "net.h"
#include "ui_net.h"
//#include <CL/cl.h>

using namespace std;
using namespace myLib;
using namespace smallLib;

Net::Net() :
    ui(new Ui::Net)
{
    ui->setupUi(this);
    this->setWindowTitle("Net");

    //clean log file
    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash + "log.txt");
    QFile::remove(helpString);

//    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
//                                          + slash + "badFiles.txt");
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
    QObject::connect(ui->loadWtsButton, SIGNAL(clicked()), this, SLOT(readWts()));

    QObject::connect(ui->loadPaButton, SIGNAL(clicked()), this, SLOT(loadDataSlot()));

    QObject::connect(ui->learnButton, SIGNAL(clicked()), this, SLOT(learnNet()));

    QObject::connect(ui->testAllButton, SIGNAL(clicked()), this, SLOT(tallNet()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopActivity()));

    QObject::connect(ui->saveWtsButton, SIGNAL(clicked()), this, SLOT(writeWtsSlot()));

    QObject::connect(ui->drawWtsButton, SIGNAL(clicked()), this, SLOT(drawWtsSlot()));

    QObject::connect(ui->pcaPushButton, SIGNAL(clicked()), this, SLOT(pca()));

    QObject::connect(ui->autoClassButton, SIGNAL(clicked()), this, SLOT(autoClassificationSimple()));

    QObject::connect(ui->svmPushButton, SIGNAL(clicked()), this, SLOT(SVM()));

    QObject::connect(ui->dimensionalityLineEdit, SIGNAL(returnPressed()), this, SLOT(reset()));

    QObject::connect(ui->distancesPushButton, SIGNAL(clicked()), this, SLOT(testDistances()));

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

matrix Net::getConfusionMatrix()
{
    return confusionMatrix;
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

std::vector<int> Net::makeLearnIndexSet()
{
    std::vector<int> mixNum;
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
                                          + slash + "SpectraSmooth");

    if(Source == source::winds) //generality
    {
        helpString += slash + "windows";
    }
    else if(Source == source::bayes)
    {
        helpString += slash + "Bayes";
    }
    else if(Source == source::pca)
    {
        helpString += slash + "PCA";
    }

    if(!helpString.isEmpty())
    {
        autoClassification(helpString);
    }
}


std::pair<std::vector<int>, std::vector<int>> Net::makeIndicesSetsCross(
        const std::vector<std::vector<int>> & arr,
        const int numOfFold)
{
    std::vector<int> learnInd;
    std::vector<int> tallInd;

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
                                                  + slash + "log.txt");
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
                                                  + slash + "results.txt");
    ofstream res;
    res.open(helpString.toStdString(), ios_base::app);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
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
                  + slash + def::ExpName + ".wts";
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
    vectType tempVec;
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
        weightsFile.open((def::dir->absolutePath() + slash +
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
                         + slash + def::ExpName + QString::number(wtsCounter) + ".wts";
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
                                                  + slash + "badFiles.txt");
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
                                  + slash + "SpectraSmooth"
                                  + slash + fileNames[ indices[i] ]);
                }
                else if(Source == source::winds)
                {
                    QFile::remove(def::dir->absolutePath()
                                  + slash + "SpectraSmooth"
                                  + slash + "windows"
                                  + slash + fileNames[ indices[i] ]);
                }
                eraseDatum(indices[i]);
            }

        }
        localConfusionMatrix[ types[ indices[i] ] ][ outClass ] += 1.;
        confusionMatrix[ types[ indices[i] ] ][ outClass ] += 1.;
    }
    badFilesStr.close();


    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash + "log.txt");
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
                         + slash + "SpectraSmooth"
                         + slash + "windows";

    const QString trainMarker = "_train";
    const QString testMarker = "_test";
//    const QString testMarker = "_3.";

    vector<int> eraseIndices{};

    numGoodNew = 0;
    confusionMatrix.fill(0.);
    exIndices.clear();

    /// check for no test items
    loadData(helpString, {"*" + trainMarker + "*"});
//    for(int i = 0; i < dataMatrix.rows(); ++i)
//    {
//        if(fileNames[i].contains(testMarker))
//        {
//            eraseIndices.push_back(i);
//        }
//    }
//    eraseData(eraseIndices);
//    eraseIndices.clear();

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

    setErrCrit(0.02);
    setLrate(0.02);


    lineType tempArr;
    int type = -1;

    QStringList leest = QDir(helpString).entryList({'*' + testMarker + '*'}); /// special generality

//    helpString = "/media/michael/Files/Data/RealTime/windows/SpectraSmooth";
//    QStringList leest = QDir(helpString).entryList(QDir::Files); /// special generality

    for(const QString & fileNam : leest)
    {
        readFileInLine(helpString + slash + fileNam,
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
            QFile::remove(spectraPath + slash + str);
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
            QFile::remove(spectraPath + slash + (*it));
        }
    }
    Source = source::winds;
    Mode = myMode::N_fold;

    setErrCrit(0.05);

    // N-fold cleaning
    cout << "N-fold cleaning" << endl;
    tallCleanFlag = true;
    for(int i = 0; i < 0; ++i)
    {
        autoClassification(spectraPath);
        if(averageAccuracy == 100.) break;
    }
    tallCleanFlag = false;
}

void Net::successiveLearning(const lineType & newSpectre,
                             const int newType,
                             const QString & newFileName)
{
    /// consider loaded wts
    /// dataMatrix is learning matrix

    lineType newData = (newSpectre - averageDatum) / (sigmaVector * loadDataNorm);

    pushBackDatum(newData, newType, newFileName);

    const std::pair<int, double> outType = classifyDatum(dataMatrix.rows() - 1); // take the last
    confusionMatrix[newType][outType.first] += 1.;

    if(outType.first == newType)
    {
        /// if accurate classification
        if(outType.second < errorThreshold)
        {
            const int num = std::find(types.begin(),
                                      types.end(),
                                      newType)
                            - types.begin();
            eraseDatum(num);
            ++numGoodNew;
        }
        else
        {
            popBackDatum();
        }

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
                    + slash + "pcaRes.txt").toStdString());
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

    std::vector<std::vector<int>> arr;
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
    std::vector<int> learnIndices;
    std::vector<int> tallIndices;

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
    std::vector<int> learnIndices;
    std::vector<int> tallIndices;
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
    std::vector<int> learnIndices;
    int i = 0;
    adjustLearnRate(ui->lowLimitSpinBox->value(),
                    ui->highLimitSpinBox->value()); /// ~40-60
    while(i < dataMatrix.rows())
    {
        cout << i + 1;
        cout << " "; cout.flush();

        /// iota ?
        learnIndices.clear();
#if 0
        learnIndices.resize(dataMatrix.rows() - 1);
        std::iota(std::begin(learnIndices),
                  std::begin(learnIndices) + i,
                  0);
        std::iota(std::begin(learnIndices) + i,
                  std::end(learnIndices),
                  i + 1);
#else
        for(int j = 0; j < dataMatrix.rows(); ++j)
        {
            if(j == i) continue;
            learnIndices.push_back(j);
        }
#endif
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
                         + slash + "PA"
                         + slash + fileName;
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
    loadData(helpString, {},
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


void Net::normalizeDataMatrix()
{
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

void Net::loadData(const matrix & inMat,
                   const std::vector<int> inTypes)
{
    dataMatrix = inMat;
    types = inTypes;
    classCount.resize(def::numOfClasses(), 0.);
    std::for_each(std::begin(types),
                  std::end(types),
                  [this](int typ)
    {
        this->classCount[typ] += 1;
    });
    normalizeDataMatrix();
}

// like readPaFile from library.cpp
void Net::loadData(const QString & spectraPath,
                   const QStringList & filters,
                   double rdcCoeff)
{
    vector<QStringList> leest;
    makeFileLists(spectraPath, leest, filters);

    dataMatrix = matrix();
    classCount.resize(def::numOfClasses(), 0.);
    types.clear();
    fileNames.clear();

    lineType tempArr;
//    cout << spectraPath << endl;
    for(int i = 0; i < leest.size(); ++i)
    {
        classCount[i] = 0.;
        for(const QString & fileName : leest[i])
        {
            readFileInLine(spectraPath + slash + fileName,
                           tempArr);
            if(rdcCoeff != 1.)
            {
                tempArr /= rdcCoeff;
            }
            pushBackDatum(tempArr, i, fileName);
        }
    }
//    cout << "loadDataNorm = " << loadDataNorm << endl;

    normalizeDataMatrix();
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

void Net::learnNetIndices(std::vector<int> mixNum,
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
    std::vector<std::valarray<double>> deltaWeights(numOfLayers);
    std::vector<std::valarray<double>> output(numOfLayers);
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
                if(errType == errorNetType::SME)
                {
                    currentError += err;
                }
                else if(errType == errorNetType::maxDist)
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
        if(errType == errorNetType::SME)
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

    resizeValar(output.back(), def::numOfClasses());
    int outClass = indexOfMax(output.back());


    /// effect on successive procedure
    double res = 0.;
    for(int j = 0; j < dimensionality.back(); ++j)
    {
        res += pow((output.back()[j] - int(type == j) ), 2.);
    }
    res = sqrt(res);

#if 1
    /// cout results
    std::ofstream resFile;
    resFile.open((def::dir->absolutePath() +
                  slash + "class.txt").toStdString(),
                 ios_base::app);

    auto tmp = std::cout.rdbuf();
    cout.rdbuf(resFile.rdbuf());

    cout << "type = " << type << '\t' << "(";
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        cout << doubleRound(output[numOfLayers - 1][i], 3) << '\t';
    }
    cout << ") " << ((type == outClass) ? "+ " : "- ") << fileNames[vecNum] << endl;
    cout.rdbuf(tmp);

    resFile.close();
#endif

    return std::make_pair(outClass,
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


void Net::crossSVM(std::pair<std::vector<int>, std::vector<int>> sets)
{
    QString helpString;
    helpString = def::dir->absolutePath()
                 + slash + "PA"
                 + slash + "svmAll";
    ofstream outStream(helpString.toStdString());
    for(int j = 0; j < 2; ++j)
    {
//        helpString = def::dir->absolutePath()
//                     + slash + "PA"
//                     + slash + "svm" + QString::number(j+1);
//        ofstream outStream(helpString.toStdString());

        std::vector<int> & currentSet = (j == 0) ? sets.first : sets.second;
        for(int i = 0; i < currentSet.size(); ++i)
        {
            outStream << myLib::typeOfFileName(fileNames[currentSet[i]]) << ' ';
            for(int l = 0; l < dataMatrix.cols(); ++l)
            {
                outStream << l << ':' << doubleRound(dataMatrix[currentSet[i]][l], 4) << ' ';
            }
            outStream << endl;
        }
//        outStream.close();
    }
    outStream.close();


    helpString = "cd "
                 + def::dir->absolutePath() + slash + "PA"
                 + " && svm-train -q "
                 + " -v " + QString::number(ui->foldSpinBox->value())
                 + " -t " + QString::number(ui->svmKernelSpinBox->value())
                 + " svmAll >> output1";
    system(helpString.toStdString().c_str());
return;




    helpString = def::dir->absolutePath()
                 + slash + "PA"
                 + slash + "svmCommands";
//    ofstream outStr(helpString.toStdString());
    helpString = "cd "
                 + def::dir->absolutePath() + slash + "PA"
                 + " && svm-train -q -t " + QString::number(ui->svmKernelSpinBox->value())
                 + " svm1"
                 + " && svm-predict svm2 svm1.model output >> output1";
//    outStr << helpString << endl << endl;
    system(helpString.toStdString().c_str());

    helpString = "cd "
                 + def::dir->absolutePath() + slash + "PA"
                 + " && svm-train -q -t " + QString::number(ui->svmKernelSpinBox->value())
                 + " svm2"
                 + " && svm-predict svm1 svm2.model output >> output1";
//    outStr << helpString << endl << endl;
    system(helpString.toStdString().c_str());
//    outStr.close();
}

void Net::SVM()
{
    /// create/clear PA/output1
    QString helpString = def::dir->absolutePath()
                         + slash + "PA"
                         + slash + "output1";
    FILE * out = fopen(QDir::toNativeSeparators(helpString), "w");
    fclose(out);



    const int numOfPairs = ui->numOfPairsBox->value();
    const int fold = ui->foldSpinBox->value();

    std::vector<std::vector<int>> arr;
    arr.resize(def::numOfClasses(), {});
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        arr[ types[i] ].push_back(i);
    }
    cout << "SVM: cross (max " << numOfPairs << "):" << endl;

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
            auto sets = makeIndicesSetsCross(arr, numFold);
            crossSVM(sets);
        }

        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = 0;
            return;
        }
    }

    double helpDouble, average = 0.;
    helpString = def::dir->absolutePath() + slash + "PA" + slash + "output1";
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
    helpString = QDir::toNativeSeparators(def::dir->absolutePath() + slash + "results.txt");
    outStr.open(helpString.toStdString(), ios_base::app);
    outStr << "\nSVM\t";
    outStr << doubleRound(average, 1) << " %" << endl;
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
    centeredMatrix = matrix::transpose(dataMatrix); // rows = spectral points, cols - vectors


    // count covariations
    // centered matrix
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


    QString helpString;
    for(int j = 0; j < NumberOfVectors; ++j)
    {
        helpString = def::dir->absolutePath()
                     + slash + "SpectraSmooth"
                     + slash + "PCA"
                     + slash + fileNames[j];
        writeFileInLine(helpString,
                        pcaMatrix[j]);
    }

    eigenVectors.transpose();
    eigenVectors.resizeRows(3); /// ???
    helpString = def::dir->absolutePath()
                 + slash + "Help"
                 + slash + "ica"
                 + slash + def::ExpName + "_pcas.jpg";
    drawTemplate(helpString);
    drawArrays(helpString,
               eigenVectors,
               true);

    cout << "pca: time elapsed = " << myTime.elapsed() / 1000. << " sec" << endl;
}
