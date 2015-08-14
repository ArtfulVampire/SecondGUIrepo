#include "cfg.h"
#include "ui_cfg.h"


cfg::cfg(double error_, double lrate_, QString FileName_) :
    ui(new Ui::cfg)
{
    ui->setupUi(this);

    this->setWindowTitle("Make CFG");
    ui->nsBox->setMaximum(300);
    ui->nsBox->setMinimum(1);
    ui->nsBox->setValue(def::ns - 1 * def::withMarkersFlag); // affects NetLength, w/o markers

    ui->spLBox->setMaximum(1000);
    ui->spLBox->setMinimum(1);
    ui->spLBox->setValue(def::spLength);

    ui->numOfOutsBox->setValue(def::numOfClasses);
    ui->epsilonSpinBox->setValue(lrate_);
    ui->errorSpinBox->setValue(error_);
    ui->tempSpinBox->setValue(10);
    ui->nameEdit->setText(FileName_);
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(makeCfg()));
    QObject::connect(ui->nameEdit, SIGNAL(returnPressed()), this, SLOT(makeCfg()));
    this->setAttribute(Qt::WA_DeleteOnClose);
}

cfg::~cfg()
{
    delete ui;
}

void cfg::makeCfg()
{
//    inputs    19*247
//    outputs     3
//    lrate       0.1
//    ecrit       0.1
//    temp       10
//    srand      77
    int helpInt = ui->spLBox->value() * ui->nsBox->value();

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + QDir::separator() + ui->nameEdit->text() + ".net");
    FILE * cfgFile = fopen(helpString.toStdString().c_str(), "w");
    if(cfgFile == NULL)
    {
        return;
    }

    fprintf(cfgFile, "inputs    %d\n", helpInt);
    fprintf(cfgFile, "outputs    %d\n", ui->numOfOutsBox->value());
    fprintf(cfgFile, "lrate    %.2lf\n", ui->epsilonSpinBox->value());
    fprintf(cfgFile, "ecrit    %.2lf\n", ui->errorSpinBox->value());
    fprintf(cfgFile, "temp    %d\n", ui->tempSpinBox->value());
    fprintf(cfgFile, "srand    %d\n", int(time (NULL))%12345);
    fclose(cfgFile);
    this->close();
}



