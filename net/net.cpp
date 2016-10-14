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

    stopFlag = 0;

//    tempRandomMatrix = matrix(def::nsWOM(), def::nsWOM());
    /// 0
    myButtonGroup.push_back(new QButtonGroup());
    myButtonGroup.back()->addButton(ui->leaveOneOutRadioButton);
    myButtonGroup.back()->addButton(ui->crossRadioButton);
    myButtonGroup.back()->addButton(ui->trainTestRadioButton);
    myButtonGroup.back()->addButton(ui->halfHalfRadioButton);
    /// 1
    myButtonGroup.push_back(new QButtonGroup());
    myButtonGroup.back()->addButton(ui->realsRadioButton);
    myButtonGroup.back()->addButton(ui->windowsRadioButton);
    myButtonGroup.back()->addButton(ui->pcaRadioButton);
    myButtonGroup.back()->addButton(ui->bayesRadioButton);
    /// 2
    myButtonGroup.push_back(new QButtonGroup());
    myButtonGroup.back()->addButton(ui->classANNRadioButton);
    myButtonGroup.back()->addButton(ui->classRDARadioButton);
    myButtonGroup.back()->addButton(ui->classSVMRadioButton);
    myButtonGroup.back()->addButton(ui->classDISTRadioButton);
    myButtonGroup.back()->addButton(ui->classNBCRadioButton);
    myButtonGroup.back()->addButton(ui->classKNNRadioButton);
    myButtonGroup.back()->addButton(ui->classWARDRadioButton);

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

    ui->critErrorDoubleSpinBox->setValue(0.10);
    ui->critErrorDoubleSpinBox->setSingleStep(0.01);
    ui->critErrorDoubleSpinBox->setDecimals(4);
    ui->critErrorDoubleSpinBox->setValue(0.05); /// errcrit PEWPEW

    ui->learnRateBox->setMinimum(0.001);
    ui->learnRateBox->setMaximum(1.0);
    ui->learnRateBox->setSingleStep(0.01);
    ui->learnRateBox->setDecimals(3);
    ui->learnRateBox->setValue(0.05); /// lrate

    ui->numOfPairsBox->setMaximum(100);
    ui->numOfPairsBox->setMinimum(1);
    ui->numOfPairsBox->setValue(10); //// pairs

    ui->foldSpinBox->setMaximum(50);
    ui->foldSpinBox->setMinimum(1);
    ui->foldSpinBox->setValue(4); /////// fold

    ui->reduceCoeffSpinBox->setMaximum(100);
    ui->reduceCoeffSpinBox->setDecimals(3);
    ui->reduceCoeffSpinBox->setMinimum(0.001);
    ui->reduceCoeffSpinBox->setSingleStep(0.5);
    ui->reduceCoeffSpinBox->setValue(4.5); ///  rdc coeff

    ui->pcaNumberSpinBox->setMinimum(2);
    ui->pcaNumberSpinBox->setMaximum(500);
    ui->pcaNumberSpinBox->setValue(100);
    ui->traceDoubleSpinBox->setMaximum(1.0);
    ui->traceDoubleSpinBox->setMinimum(0.05);
    ui->traceDoubleSpinBox->setSingleStep(0.01);
    ui->traceDoubleSpinBox->setValue(1.0);

    ui->autoPCAMaxSpinBox->setMinimum(1);
    ui->autoPCAMinSpinBox->setMinimum(1);
    ui->autoPCAStepSpinBox->setMinimum(0);
    ui->autoPCAStepSpinBox->setMaximum(5);

    ui->momentumDoubleSpinBox->setMaximum(1.0);
    ui->momentumDoubleSpinBox->setMinimum(0.0);
    ui->momentumDoubleSpinBox->setDecimals(2);
    ui->momentumDoubleSpinBox->setSingleStep(0.05);
    ui->momentumDoubleSpinBox->setValue(0.5);

    ui->dimensionalityLineEdit->setText("");

    ui->centerCheckBox->setChecked(true);
    ui->varianceCheckBox->setChecked(true);

    ui->knnNumOfNearSpinBox->setValue(10);
    ui->wordNumOfClustSpinBox->setValue(10);

    ui->rdaShrinkSpinBox->setMaximum(0.5);
    ui->rdaShrinkSpinBox->setDecimals(3);
    ui->rdaShrinkSpinBox->setSingleStep(0.005);
    ui->rdaShrinkSpinBox->setValue(0.1);

    ui->rdaLambdaSpinBox->setMaximum(1.0);
    ui->rdaLambdaSpinBox->setMinimum(-1.0);
    ui->rdaLambdaSpinBox->setDecimals(3);
    ui->rdaLambdaSpinBox->setSingleStep(0.005);
    ui->rdaLambdaSpinBox->setValue(0.1);

    myClassifier = new NBC();

    QObject::connect(ui->loadDataButton, SIGNAL(clicked()), this, SLOT(loadDataSlot()));
    QObject::connect(ui->pcaPushButton, SIGNAL(clicked()), this, SLOT(pca()));
    QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopActivity()));

    QObject::connect(ui->autoClassButton, SIGNAL(clicked()), this, SLOT(autoClassificationSimple()));
    QObject::connect(ui->autoClassDataPushButton, SIGNAL(clicked()), this, SLOT(autoClassification()));

    QObject::connect(myButtonGroup[0], SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(setModeSlot(QAbstractButton*, bool)));
    QObject::connect(myButtonGroup[1], SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(setSourceSlot(QAbstractButton*)));
    QObject::connect(myButtonGroup[2], SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(setClassifier(QAbstractButton*, bool)));

    /// ANN
    QObject::connect(ui->loadWtsButton, SIGNAL(clicked()), this, SLOT(readWtsSlot()));
    QObject::connect(ui->saveWtsButton, SIGNAL(clicked()), this, SLOT(writeWtsSlot()));
    QObject::connect(ui->drawWtsButton, SIGNAL(clicked()), this, SLOT(drawWtsSlot()));
    QObject::connect(ui->learnRateBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setLrateSlot(double)));
    QObject::connect(ui->critErrorDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setErrCritSlot(double)));
    QObject::connect(ui->dimensionalityLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(setDimensionalitySlot()));

    /// SVM
    QObject::connect(ui->svmKernelSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setSvmKernelNumSlot(int)));
    QObject::connect(ui->svmTypeSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setSvmTypeSlot(int)));

    /// KNN
    QObject::connect(ui->knnNumOfNearSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setKnnNumSlot(int)));

    /// WARD
    QObject::connect(ui->wordNumOfClustSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setWordNumSlot(int)));

    /// LDA/QDA
    QObject::connect(ui->rdaShrinkSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setRdaShrinkSlot(double)));
    QObject::connect(ui->rdaLambdaSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setRdaLambdaSlot(double)));


    this->setAttribute(Qt::WA_DeleteOnClose);


    aaDefaultSettings();
}


Net::~Net()
{
    for(auto item : myButtonGroup)
    {
        delete item;
    }
    delete ui;
}

void Net::drawWtsSlot()
{
    if(myClassifier->getType() != ClassifierType::ANN)
    {
        return;
    }
    QString helpString = QFileDialog::getOpenFileName((QWidget * )this,
                                                      tr("wts to draw"),
                                                      def::dir->absolutePath(),
                                                      tr("wts files (*.wts)"));
    if(helpString.isEmpty())
    {
        return;
    }
    ANN * myANN = dynamic_cast<ANN *>(myClassifier);
    myANN->drawWeight(helpString);
}


void Net::writeWtsSlot()
{
    if(myClassifier->getType() != ClassifierType::ANN)
    {
        return;
    }
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

    ANN * myANN = dynamic_cast<ANN *>(myClassifier);
    myANN->writeWeight(helpString);
}

void Net::readWtsSlot()
{
    if(myClassifier->getType() != ClassifierType::ANN)
    {
        return;
    }
    QString helpString = QFileDialog::getOpenFileName((QWidget * )NULL,
                                                      tr("load wts"),
                                                      def::dir->absolutePath(),
                                                      tr("wts files (*.wts)"));
    if(helpString.isEmpty())
    {
//        QMessageBox::information((QWidget * )this, tr("Warning"), tr("No wts-file was chosen"), QMessageBox::Ok);
        return;
    }
    ANN * myANN = dynamic_cast<ANN *>(myClassifier);
    myANN->readWeight(helpString);
}

void Net::pushBackDatum(const lineType & inDatum,
                      const uint & inType,
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

void Net::eraseDatum(const uint & index)
{
    dataMatrix.eraseRow(index);
    classCount[ types[index] ] -= 1.;
    types.erase(types.begin() + index);
    fileNames.erase(fileNames.begin() + index);
}

void Net::eraseData(const std::vector<uint> & indices)
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
        for(uint i = 0; i < dataMatrix.rows(); ++i)
        {
            dataMatrix[i] -= averageDatum;
        }
    }
    else
    {
        averageDatum = std::valarray<double> (0., dataMatrix.cols());
    }


    if(ui->varianceCheckBox->isChecked())
    {

        dataMatrix.transpose();
        sigmaVector.resize(dataMatrix.rows());
        for(uint i = 0; i < dataMatrix.rows(); ++i)
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
    else
    {
        sigmaVector = std::valarray<double> (1., dataMatrix.cols());
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

void Net::applyPCA(const QString & pcaMatFilePath)
{
    matrix pca{};
    readMatrixFile(pcaMatFilePath, pca);
    dataMatrix *= pca;
}

void Net::resizeData(uint newCols)
{
    if(newCols >= dataMatrix.cols()) return;
    dataMatrix.resizeCols(newCols);
}

void Net::loadDataUCI(const QString & setName)
{
    matrix uciData{};
    std::vector<uint> uciTypes{};
    myLib::readUCIdataSet(setName, uciData, uciTypes);
	/// set numOfClasses !
	loadData(uciData, uciTypes);
}

void Net::loadData(const matrix & inMat,
                   const std::vector<uint> & inTypes)
{
    dataMatrix = matrix();
    dataMatrix = inMat;
	types = inTypes;
	classCount.resize(def::numOfClasses());
    for(int typ : types)
    {
        classCount[typ] += 1.;
    }
    normalizeDataMatrix();
    filesPath.clear();
}

// like readPaFile from library.cpp
void Net::loadData(const QString & spectraPath,
                   const QStringList & filters,
                   double rdcCoeff)
{
    std::vector<QStringList> leest;
    makeFileLists(spectraPath, leest, filters);

    dataMatrix = matrix();
    classCount.resize(def::numOfClasses());
    types.clear();
    fileNames.clear();
    filesPath = spectraPath;

    lineType tempArr;
//    cout << spectraPath << endl;
    for(uint i = 0; i < leest.size(); ++i)
    {
        classCount[i] = 0.;
        for(const QString & fileName : leest[i])
        {
            readFileInLine(spectraPath + slash + fileName,
                           tempArr);
            pushBackDatum(tempArr, i, fileName);
        }
    }
    if(rdcCoeff != 1.)
    {
        dataMatrix /= rdcCoeff;
    }
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
    loadData(helpString, {}
//             , ui->reduceCoeffSpinBox->value()
             );
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
    centeredMatrix = matrix::transpose(dataMatrix);
    // now rows = spectral points/features, cols - vectors

    // count covariations
    // centered matrix
    for(int i = 0; i < NetLength; ++i)
    {
        // should be already zero because if loadData centering is on
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

	/// pcaMatrix
    centeredMatrix.transpose();
    matrix pcaMatrix(NumberOfVectors, numOfPc);
    pcaMatrix = centeredMatrix * eigenVectors;

    /// save pca matrix
    QString helpString;
    helpString = def::dir->absolutePath()
                 + slash + "Help"
                 + slash + "ica"
                 + slash + def::ExpName + "_pcaMat.txt";
    writeMatrixFile(helpString, eigenVectors);


    /// pewpewpewpewpewpewpewpewpewpewpewpewpewpewpewpew
    dataMatrix = pcaMatrix;


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
