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


#include "coord.cpp"
using namespace coords;

namespace Ui {
    class MakePa;
}

class MakePa : public QWidget
{
    Q_OBJECT

public:
    explicit MakePa(QString spectraPath, QString ExpName_, int ns_=20, int left_=82, int right_=328, double spStep_=250./4096., QVector<int> vect_ = QVector<int> ());
    ~MakePa();
    void setRdcCoeff(double);
    double getRdcCoeff();
    void kernelest(double * arr, int num, QString path, double minVal, double maxVal, int numOfRanges);
    double gaussian(double x);
    double drawSamples(double * drawArray, double leftLim, double rightLim);

public slots:
    void makePaSlot();
    void setSpLength();
    void setNs();
    void mwTest();
    void dirSlot();
    void kwTest();
    void setNumOfClasses(int a);
    void dispersionAnalysis();
    void correlationDifference();


private:
    Ui::MakePa *ui;
    QDir *dir;
    int ns, spLength, left, right, helpInt;
    double spStep;
    QString helpString;
    QFileDialog *browser;
    int NumOfClasses;
    char * helpCharArr;
    QString ExpName;
    QVector<int> vect;
};

#endif // MAKEPA_H
