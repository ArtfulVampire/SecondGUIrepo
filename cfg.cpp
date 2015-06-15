#include "cfg.h"
#include "ui_cfg.h"


cfg::cfg(QDir *dir_, int ns_, int spLength_, double error_, double lrate_, QString FileName_) :
    ui(new Ui::cfg)
{
    ui->setupUi(this);
    dir=dir_;

    this->setWindowTitle("Make CFG");
    ui->nsBox->setMaximum(300);
    ui->nsBox->setMinimum(1);
    ui->nsBox->setValue(ns_ - 1 * def::withMarkersFlag); // affects NetLength

    ui->spLBox->setMaximum(1000);
    ui->spLBox->setMinimum(1);
    ui->spLBox->setValue(spLength_);

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
    int helpInt=ui->spLBox->value() * ui->nsBox->value();
//    cout<<"SpL = "<<ui->spLBox->value()<<" ns = "<<ui->nsBox->value()<<endl;

    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(ui->nameEdit->text()).append(".net");
    FILE * cfgFile = fopen(helpString.toStdString().c_str(), "w");
    if(cfgFile==NULL) {return;}
    /////////////generality//////////////



    fprintf(cfgFile, "inputs    %d\n", helpInt);
    fprintf(cfgFile, "outputs    %d\n", ui->numOfOutsBox->value());
    fprintf(cfgFile, "lrate    %.2lf\n", ui->epsilonSpinBox->value());
    fprintf(cfgFile, "ecrit    %.2lf\n", ui->errorSpinBox->value());
    fprintf(cfgFile, "temp    %d\n", ui->tempSpinBox->value());
    fprintf(cfgFile, "srand    %d\n", int(time (NULL))%1234);
    fclose(cfgFile);
    this->close();
}



