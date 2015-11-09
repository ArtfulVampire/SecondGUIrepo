#ifndef CFG_H
#define CFG_H

//#include <QtWidgets>
#include <QWidget>
#include <QDir>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include "coord.h"
#include "library.h"

using namespace std;

namespace Ui {
    class cfg;
}

class cfg : public QWidget
{
    Q_OBJECT

public:
    explicit cfg(double error_ = 0.10, double lrate_ = 0.10, QString FileName_ = "16sec19ch");
    ~cfg();

public slots:
    void makeCfg();

private:
    Ui::cfg *ui;
};

#endif // CFG_H
