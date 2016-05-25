#include "cfg.h"
#include "ui_cfg.h"


using namespace myLib;

cfg::cfg(double error_, double lrate_, QString FileName_) :
    ui(new Ui::cfg)
{
    ui->setupUi(this);

    this->setWindowTitle("Make CFG");
    ui->nsBox->setMaximum(300);
    ui->nsBox->setMinimum(1);
    ui->nsBox->setValue(def::nsWOM()); // affects NetLength, w/o markers

    ui->spLBox->setMaximum(1000);
    ui->spLBox->setMinimum(1);
    ui->spLBox->setValue(def::spLength());

    ui->numOfOutsBox->setValue(def::numOfClasses());
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

    makeCfgStatic(ui->nameEdit->text(),
                  ui->nsBox->value() * ui->spLBox->value(),
                  def::dir->absolutePath(),
                  ui->numOfOutsBox->value(),
                  ui->epsilonSpinBox->value(),
                  ui->errorSpinBox->value(),
                  ui->tempSpinBox->value());
    this->close();
}



