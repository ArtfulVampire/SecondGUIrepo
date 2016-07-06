#include "net.h"
#include "ui_net.h"
using namespace std;
using namespace myLib;
using namespace smallLib;

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

const matrix & Net::getConfusionMatrix()
{
    return confusionMatrix;
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

void Net::setTallCleanFlag(bool in)
{
    this->tallCleanFlag = in;
}

void Net::setFold(int in)
{
    ui->foldSpinBox->setValue(in);
}

void Net::stopActivity()
{
    stopFlag = 1;
}


void Net::setActFunc(const QString & in)
{
    if(in.contains("log", Qt::CaseInsensitive) ||
       in.startsWith('l', Qt::CaseInsensitive))
    {
        ui->logisticRadioButton->setChecked(true);
        activation = smallLib::logistic;
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
        if(activation == smallLib::softmax)
        {
            ui->highLimitSpinBox->setValue(60); /// highLimit
            ui->lowLimitSpinBox->setValue(40);  /// lowLimit
        }
        else if(activation == smallLib::logistic)
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
