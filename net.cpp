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

    QObject::connect(ui->loadDataButton, SIGNAL(clicked()), this, SLOT(loadDataSlot()));

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
    activation = smallLib::softmax;
    ui->highLimitSpinBox->setValue(60); /// highLimit
    ui->lowLimitSpinBox->setValue(40);  /// lowLimit

//    activation = smallLib::logistic;
//    ui->highLimitSpinBox->setValue(120); /// highLimit
//    ui->lowLimitSpinBox->setValue(60);  /// lowLimit

    ui->rdcCoeffSpinBox->setValue(7.); ///  rdc coeff
    ui->foldSpinBox->setValue(4); /////// fold
    ui->numOfPairsBox->setValue(10); //// pairs
    ui->critErrorDoubleSpinBox->setValue(0.04); /// errcrit PEWPEW
}

Net::~Net()
{
    delete group1;
    delete group2;
    delete group3;
    delete group4;
    delete ui;
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







/// data part
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
    if(ui->centerCheckBox->isChecked())
    {
        averageDatum = dataMatrix.averageRow();
        for(int i = 0; i < dataMatrix.rows(); ++i)
        {
            dataMatrix[i] -= averageDatum;
        }
    }
    if(ui->varianceCheckBox->isChecked())
    {

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
    }
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
        // should be already zero because of loadData - ok
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


    /// pewpewpewpewpewpewpewpewpewpewpewpewpewpewpewpew
    dataMatrix = pcaMatrix;


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
