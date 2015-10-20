#ifndef MAKEPA_H
#define MAKEPA_H

//#include <QtWidgets>
#include <QWidget>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <cmath>
#include <QPainter>
#include <QTimer>
#include <QTime>
#include "library.h"

using namespace std;

namespace Ui {
    class MakePa;
}

class MakePa : public QWidget
{
    Q_OBJECT

public:
    explicit MakePa(QString spectraPath = def::dir->absolutePath() + slash() + "SpectraSmooth",
                    QVector<int> vect_ = QVector<int> ());
    ~MakePa();
    void setRdcCoeff(double);
    double getRdcCoeff();
    void kernelest(double * arr, int num, QString path, double minVal, double maxVal, int numOfRanges);
    double gaussian(double x);
    double drawSamples(double * drawArray, double leftLim, double rightLim);
    void setFold(int a);

public slots:
    void makePaSlot();
    void mwTest();
    void dirSlot();
    void kwTest();
    void dispersionAnalysis();
    void correlationDifference();
    void changeSpectraDir(QAbstractButton * button);



private slots:
    void on_rdcCoeffBox_valueChanged(double arg1);

private:
    Ui::MakePa *ui;
    QFileDialog *browser;
    char * helpCharArr;
    QVector<int> vect;
    QButtonGroup * group1;
};

#endif // MAKEPA_H
