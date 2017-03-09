#include "net.h"
#include "ui_net.h"
//#include <CL/cl.h>

using namespace myOut;

Net::Net() :
    ui(new Ui::Net)
{
    ui->setupUi(this);
	this->setWindowTitle("Net");

    stopFlag = 0;

    /// 0
    myButtonGroup.push_back(new QButtonGroup());
    myButtonGroup.back()->addButton(ui->leaveOneOutRadioButton);
    myButtonGroup.back()->addButton(ui->crossRadioButton);
    myButtonGroup.back()->addButton(ui->trainTestRadioButton);
    myButtonGroup.back()->addButton(ui->halfHalfRadioButton);
    /// 1
    myButtonGroup.push_back(new QButtonGroup());
    myButtonGroup.back()->addButton(ui->realsRadioButton);
    myButtonGroup.back()->addButton(ui->windsRadioButton);
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
    ui->windsRadioButton->setChecked(true); /// winds
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


	myClassifierData = ClassifierData();
	myClassifier = new NBC();



    QObject::connect(ui->loadDataButton, SIGNAL(clicked()), this, SLOT(loadDataSlot()));
    QObject::connect(ui->pcaPushButton, SIGNAL(clicked()), this, SLOT(pca()));
	QObject::connect(ui->stopButton, &QPushButton::clicked, [this](){ this->stopFlag = true; });

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
	else
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
		std::cout << "saveWtsSlot: no file is chosen to save" << std::endl;
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
        return;
    }
    ANN * myANN = dynamic_cast<ANN *>(myClassifier);
    myANN->readWeight(helpString);
}


void Net::loadDataSlot()
{
	QString helpString = QFileDialog::getExistingDirectory(nullptr,
                                                           tr("load data"),
                                                           def::dir->absolutePath());
    if(helpString.isEmpty())
    {
        return;
	}
	myClassifierData = ClassifierData(helpString);
}


