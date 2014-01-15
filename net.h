#ifndef NET_H
#define NET_H

//#include <QtWidgets>
#include <QWidget>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <QFileDialog>
#include <cmath>
#include <time.h>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include "makepa.h"
#include "cfg.h"
#include "qtempevent.h"
#include "tempthread.h"
#include <fstream>
#include <ios>
#include <unistd.h>
#include <QTime>

using namespace std;

#include <QMessageBox>

#include "coord.cpp"
using namespace coords;

namespace Ui {
    class Net;
}

class Net : public QWidget
{
    Q_OBJECT

public:
    explicit Net(QDir *dir_ = new QDir("/"), int ns_ = 19., int left_ = 82, int right_ = 328, double spStep_=250./4096., QString ExpName_="nobody");
    ~Net();
    bool ClassificateVector(int &vecNum);
    void closeLogFile();
    void setAutoProcessingFlag(bool);
    int getEpoch();
    void setErrcrit(double);
    double getErrcrit();
    void Sammon(double ** distArr, int size, int * colors);
    void Kohonen(double ** input, double ** eigenVects, double * averageProjection, int size, int length);
    void moveCoordsGradient(double ** coords, double ** distOld, double ** distNew, int size);
    double thetalpha(int bmu_, int j_, int step_, double ** arr, int length_);
    void readCfgByName(QString FileName);
    double setPercentageForClean();
    double mouseClick(QLabel * label, QMouseEvent * ev);
    void leaveOneOut();
    void leaveOneOutCL();

protected:
    bool event(QEvent * ev);
    void mousePressEvent(QMouseEvent * event);
//    void customEvent(QEvent * ev);
//    bool tempEvent(QTempEvent * ev);

public slots:
    void readCfg();
    void loadWts();
    void loadWtsByName(QString);
    void LearnNet();
    void tall();
    void reset();
    void saveWts();
    void stopActivity();
    void test();
    void next();
    void prev();
    void drawWts();
    void compressWts();
    void PaIntoMatrixByName(QString fileName);
    void PaIntoMatrix();
    void leaveOneOutSlot();
    void neuronGas();
    void pca();
    void drawSammon();
    void averageClassification();
    void autoClassification(QString spectraDir);
    void autoClassificationSimple();
    void drawWindows();
    void autoPCAClassification();
    void clearSets();
    void clearSets1();    
//    void clearSets2();
    void SVM();

private:
    Ui::Net *ui;
    QString helpString;
    QString ExpName;
    QDir *dir, *dirBC;
    double **matrix;
    double **weight; //perceptron weights
    double * output;
    char * helpCharArr;
    QPixmap pic;
    QPainter *paint;
    int numTest;
    int epoch;
    int  NetLength; //inputs
    int NumOfClasses; //outputs
    int NumberOfVectors;
    bool autoFlag;
    double lrate;
    double ecrit;
    double temp;
    int *NumberOfErrors;
    char **FileName;
    int spLength;
    int left, right;
    int helpInt;
    double spStep;
    double Error;
    QPixmap columns;
    FILE * log;
    int *zeroChan;
    int zeroChanLength;
    int numOfTall;
    int loadPAflag;
    bool stopFlag;
    QString tmp, paFileBC;
    int ns;
    QTempEvent * tempEvent;
    TempThread myThread;
    double ** coords; //new coords for Sammon method
    double mouseShit;
};

#endif // NET_H
