#include <widgets/net.h>
#include "ui_net.h"

#include <other/defs.h>

#include <QFileDialog>
#include <QButtonGroup>

using namespace myOut;

Net::Net() :
    ui(new Ui::Net)
{
    ui->setupUi(this);
	this->setWindowTitle("Net");

	std::vector<QButtonGroup*> myButtonGroup;
	/// 0
	myButtonGroup.push_back(new QButtonGroup(this));
	myButtonGroup.back()->addButton(ui->leaveOneOutRadioButton);
	myButtonGroup.back()->addButton(ui->crossRadioButton);
	myButtonGroup.back()->addButton(ui->trainTestRadioButton);
	myButtonGroup.back()->addButton(ui->peopleClassRadioButton);
	myButtonGroup.back()->addButton(ui->halfHalfRadioButton);
	/// 1
	myButtonGroup.push_back(new QButtonGroup(this));
	myButtonGroup.back()->addButton(ui->realsRadioButton);
	myButtonGroup.back()->addButton(ui->windsRadioButton);
	myButtonGroup.back()->addButton(ui->pcaRadioButton);
	myButtonGroup.back()->addButton(ui->bayesRadioButton);
	/// 2
	myButtonGroup.push_back(new QButtonGroup(this));
	myButtonGroup.back()->addButton(ui->classANNRadioButton);
	myButtonGroup.back()->addButton(ui->classRDARadioButton);
	myButtonGroup.back()->addButton(ui->classSVMRadioButton);
	myButtonGroup.back()->addButton(ui->classDISTRadioButton);
	myButtonGroup.back()->addButton(ui->classNBCRadioButton);
	myButtonGroup.back()->addButton(ui->classKNNRadioButton);
	myButtonGroup.back()->addButton(ui->classWARDRadioButton);

	ui->crossRadioButton->setChecked(true); /// k-fold
	ui->leaveOneOutRadioButton->setChecked(true); /// N-fold
//	ui->trainTestRadioButton->setChecked(true); /// train-test
	if(DEFS.isUser(username::Ossadtchi))
	{
		ui->trainTestRadioButton->setChecked(true); /// train-test
	}

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
	myModel = new NBC();



	QObject::connect(ui->loadDataButton, SIGNAL(clicked()), this, SLOT(loadDataSlot()));
	QObject::connect(ui->chooseChansPushButton, &QPushButton::clicked,
					 this, &Net::showReduce);
	QObject::connect(ui->pcaPushButton, SIGNAL(clicked()), this, SLOT(pca()));
	QObject::connect(ui->stopButton, &QPushButton::clicked, [this](){ this->stopFlag = true; });

	QObject::connect(ui->autoClassButton, &QPushButton::clicked,
					 [this](){ autoClassificationSimple(); });
	QObject::connect(ui->autoClassDataPushButton, &QPushButton::clicked,
					 [this](){ autoClassification(); });

	QObject::connect(myButtonGroup[0], SIGNAL(buttonToggled(QAbstractButton*, bool)),
			this, SLOT(setModeSlot(QAbstractButton*, bool)));
	QObject::connect(myButtonGroup[1], SIGNAL(buttonToggled(QAbstractButton*, bool)),
			this, SLOT(setSourceSlot(QAbstractButton*)));
	QObject::connect(myButtonGroup[2], SIGNAL(buttonToggled(QAbstractButton*, bool)),
			this, SLOT(setClassifier(QAbstractButton*, bool)));

	QObject::connect(ui->learnAllButton, &QPushButton::clicked,
					 [this]()
	{
		if(myModel)
		{
			std::cout << "Net: learnAll" << std::endl;
			myModel->learnAll();
		}
	});

	/// SHOLD I CHECK BAD CAST IF I CHECK ModelType?

	/// ANN
	QObject::connect(ui->loadWtsButton, SIGNAL(clicked()), this, SLOT(readWtsSlot()));
	QObject::connect(ui->saveWtsButton, SIGNAL(clicked()), this, SLOT(writeWtsSlot()));
	QObject::connect(ui->drawWtsButton, SIGNAL(clicked()), this, SLOT(drawWtsSlot()));
	QObject::connect(ui->learnRateBox,
					 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					 [this](double in)
	{ if(myModel->getType() == ModelType::ANN) { dynamic_cast<ANN*>(myModel)->setLrate(in); } });

	QObject::connect(ui->critErrorDoubleSpinBox,
					 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					 [this](double in)
	{ if(myModel->getType() == ModelType::ANN) { dynamic_cast<ANN*>(myModel)->setCritError(in); } });

	QObject::connect(ui->dimensionalityLineEdit, SIGNAL(returnPressed()),
					 this, SLOT(setDimensionalitySlot()));

	/// SVM
	QObject::connect(ui->svmKernelSpinBox,
					 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 [this](int in)
	{ if(myModel->getType() == ModelType::SVM) { dynamic_cast<SVM*>(myModel)->setKernelNum(in); } });

	QObject::connect(ui->svmTypeSpinBox,
					 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 [this](int in)
	{ if(myModel->getType() == ModelType::SVM) { dynamic_cast<SVM*>(myModel)->setSvmType(in); } });

	/// KNN
	QObject::connect(ui->knnNumOfNearSpinBox,
					 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 [this](int in)
	{ if(myModel->getType() == ModelType::KNN) { dynamic_cast<KNN*>(myModel)->setNumOfNear(in); } });

	/// WARD
	QObject::connect(ui->wordNumOfClustSpinBox,
					 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 [this](int in)
	{ if(myModel->getType() == ModelType::WARD) { dynamic_cast<WARD*>(myModel)->setNumClust(in); } });

	/// LDA/QDA
	QObject::connect(ui->rdaShrinkSpinBox,
					 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					 [this](double in)
	{ if(myModel->getType() == ModelType::RDA) { dynamic_cast<RDA*>(myModel)->setShrinkage(in); } });

	QObject::connect(ui->rdaLambdaSpinBox,
					 static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					 [this](double in)
	{ if(myModel->getType() == ModelType::RDA) { dynamic_cast<RDA*>(myModel)->setLambda(in); } });

	this->setAttribute(Qt::WA_DeleteOnClose);


	aaDefaultSettings();

}


Net::~Net()
{
    delete ui;
}

void Net::drawWtsSlot()
{
	if(myModel->getType() != ModelType::ANN)
    {
        return;
    }
	QString helpString = QFileDialog::getOpenFileName(
							 this,
							 tr("wts to draw"),
							 DEFS.dirPath(),
							 tr("wts files (*.wts)"));
    if(helpString.isEmpty())
    {
        return;
    }
	dynamic_cast<ANN *>(myModel)->drawWeight(helpString);
}


void Net::writeWtsSlot()
{
	if(myModel->getType() != ModelType::ANN)
    {
        return;
    }
    int wtsCounter = 0;
	QString helpString= QFileDialog::getSaveFileName(this,
													 tr("wts to save"),
													 DEFS.dirPath(),
													 tr("wts files (*.wts)"));
	if(helpString.isEmpty())
	{
		std::cout << "saveWtsSlot: no file is chosen to save" << std::endl;
		return;
	}

	if(!helpString.endsWith(".wts", Qt::CaseInsensitive))
	{
		helpString += ".wts";
	}

	/// check bad cast?
	dynamic_cast<ANN *>(myModel)->writeWeight(helpString);
}

void Net::readWtsSlot()
{
	if(myModel->getType() != ModelType::ANN)
    {
        return;
    }
	QString helpString = QFileDialog::getOpenFileName(
							 this,
							 tr("load wts"),
							 DEFS.dirPath(),
							 tr("wts files (*.wts)"));
    if(helpString.isEmpty())
	{
        return;
    }
	dynamic_cast<ANN *>(myModel)->readWeight(helpString);
}


void Net::loadDataSlot()
{
	QString helpString = QFileDialog::getExistingDirectory(nullptr,
                                                           tr("load data"),
														   DEFS.dirPath());
    if(helpString.isEmpty())
    {
        return;
	}
	myClassifierData = ClassifierData(helpString);
}
