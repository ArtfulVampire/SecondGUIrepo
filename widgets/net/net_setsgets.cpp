#include <widgets/net.h>
#include "ui_net.h"

void Net::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

double Net::getLrate() const
{
	if(myModel->getType() != ModelType::ANN)
    {
        return -1;
    }
	ANN * myANN = dynamic_cast<ANN *>(myModel);
    return myANN->getLrate();
}

void Net::setDimensionalitySlot()
{
	if(myModel->getType() == ModelType::ANN)
    {
		ANN * myANN = dynamic_cast<ANN *>(myModel);
        std::vector<int> pewpew;
        auto strList = ui->dimensionalityLineEdit->text().split(' ', QString::SkipEmptyParts);
        for(QString peww : strList)
        {
            pewpew.push_back(peww.toInt());
        }
        myANN->setDim(pewpew);
    }
}

void Net::setErrCrit(double in)
{
	ui->critErrorDoubleSpinBox->setValue(in);
}

void Net::setLrate(double in)
{
    ui->learnRateBox->setValue(in);
}

void Net::setNumOfPairs(int num)
{
    this->ui->numOfPairsBox->setValue(num);
}

void Net::setFold(int in)
{
    ui->foldSpinBox->setValue(in);
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
		std::cout << "setMode: wrong mode" << std::endl;
        exit(0);
    }
}

void Net::setSource(const QString & in)
{
    if(in.contains("real", Qt::CaseInsensitive) || in.startsWith('r'))
    {
        ui->realsRadioButton->setChecked(true);
//        loadDataNorm = 10.;
    }
    else if(in.contains("wind", Qt::CaseInsensitive) || in.startsWith('w'))
    {
        ui->windsRadioButton->setChecked(true);
//        loadDataNorm = 5.;
    }
}

void Net::setClassifier(const QString & in)
{
    if(in.contains("ANN", Qt::CaseInsensitive))
    {
        ui->classANNRadioButton->setChecked(true);
    }
    else if(in.contains("RDA", Qt::CaseInsensitive))
    {
        ui->classRDARadioButton->setChecked(true);
    }
    else if(in.contains("SVM", Qt::CaseInsensitive))
    {
        ui->classSVMRadioButton->setChecked(true);
    }
    else if(in.contains("DIST", Qt::CaseInsensitive))
    {
        ui->classDISTRadioButton->setChecked(true);
    }
    else if(in.contains("NBC", Qt::CaseInsensitive))
    {
        ui->classNBCRadioButton->setChecked(true);
    }
    else if(in.contains("KNN", Qt::CaseInsensitive))
    {
        ui->classKNNRadioButton->setChecked(true);
    }
    else if(in.contains("WARD", Qt::CaseInsensitive))
    {
        ui->classWARDRadioButton->setChecked(true);
    }
}

void Net::setClassifier(ModelType typ)
{
	if(typ == ModelType::ANN)
    {
        ui->classANNRadioButton->setChecked(true);
    }
	else if(typ == ModelType::RDA)
    {
        ui->classRDARadioButton->setChecked(true);
    }
	else if(typ == ModelType::SVM)
    {
        ui->classSVMRadioButton->setChecked(true);
    }
	else if(typ == ModelType::DIST)
    {
        ui->classDISTRadioButton->setChecked(true);
    }
	else if(typ == ModelType::NBC)
    {
        ui->classNBCRadioButton->setChecked(true);
    }
	else if(typ == ModelType::KNN)
    {
        ui->classKNNRadioButton->setChecked(true);
    }
	else if(typ == ModelType::WARD)
    {
        ui->classWARDRadioButton->setChecked(true);
    }
}

void Net::setClassifier(QAbstractButton * but, bool i)
{
    if(!i) return;
	if(myModel != nullptr)
	{
		delete myModel;
	}

    if(but->text() == "ANN")
    {
		myModel = new ANN();
		ANN * myANN = dynamic_cast<ANN *>(myModel);

		myModel->setClassifierData(myClassifierData);

        myANN->setLrate(ui->learnRateBox->value());
        myANN->setCritError(ui->critErrorDoubleSpinBox->value());
		setDimensionalitySlot();
    }
    else if(but->text() == "RDA")
	{
		myModel = new RDA();
		RDA * myRDA = dynamic_cast<RDA *>(myModel);
        myRDA->setShrinkage(ui->rdaShrinkSpinBox->value());
        myRDA->setLambda(ui->rdaLambdaSpinBox->value());

    }
    else if(but->text() == "SVM")
    {
		myModel = new SVM();
		SVM * mySVM = dynamic_cast<SVM *>(myModel);
        mySVM->setKernelNum(ui->svmKernelSpinBox->value());
    }
    else if(but->text() == "DIST")
    {
		myModel = new DIST();
//        DIST * myDIST = dynamic_cast<DIST *>(myClassifier);
    }
    else if(but->text() == "NBC")
    {
		myModel = new NBC();
//        NBC * myNBC = dynamic_cast<NBC *>(myClassifier);
    }
    else if(but->text() == "KNN")
    {
		myModel = new KNN();
		KNN * myKNN = dynamic_cast<KNN *>(myModel);
        myKNN->setNumOfNear(ui->knnNumOfNearSpinBox->value());
    }
    else if(but->text() == "WARD")
    {
		myModel = new WARD();
		WARD * myWARD = dynamic_cast<WARD *>(myModel);
		myWARD->setNumClust(ui->knnNumOfNearSpinBox->value());
	}

	myModel->setClassifierData(myClassifierData);
}

void Net::setSourceSlot(QAbstractButton * but)
{
    if(but->text().contains("Bayes", Qt::CaseInsensitive))
	{
        Source = source::bayes;
    }
    else
	{
        ui->foldSpinBox->setValue(2);
        if(but->text().contains("wind", Qt::CaseInsensitive))
		{
            Source = source::winds;
        }
        else if(but->text().contains("real", Qt::CaseInsensitive))
		{
            Source = source::reals;
        }
        else if(but->text().contains("pca", Qt::CaseInsensitive))
		{
            Source = source::pca;
        }
    }
}

void Net::setModeSlot(QAbstractButton * but, bool i)
{
	if(i == false) return;

	if(but->text().contains("N-fold"))
	{
		Mode = myMode::N_fold;
    }
	else if(but->text().contains("k-fold"))
    {
		Mode = myMode::k_fold;
    }
	else if(but->text().contains("train"))
    {
		Mode = myMode::train_test;
    }
	else if(but->text().contains("half"))
    {
		Mode = myMode::half_half;
    }
	else
	{
		Mode = myMode::N_fold;
		std::cout << "Net::setModeSlot: bad but->text(), N-fold set" << std::endl;
    }
}

void Net::aaDefaultSettings()
{
    /// mode
    ui->crossRadioButton->setChecked(true); /// k-fold
    ui->leaveOneOutRadioButton->setChecked(true); /// N-fold
//    ui->trainTestRadioButton->setChecked(true); /// train-test

    /// source
		ui->realsRadioButton->setChecked(true); /// reals
//        ui->windsRadioButton->setChecked(true); /// winds
//    ui->pcaRadioButton->setChecked(true); /// PCA

    ui->foldSpinBox->setValue(2); /////// fold
    ui->numOfPairsBox->setValue(1); //// pairs
    ui->critErrorDoubleSpinBox->setValue(0.04); /// errcrit PEWPEW
    ui->learnRateBox->setValue(0.05); /// lrate PEWPEW
}
