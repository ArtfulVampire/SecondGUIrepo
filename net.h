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
#include"library.h"
#include "coord.h"
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

//    friend class MainWindow;

public:
    explicit Net(QDir *dir_ = new QDir("/"), int ns_ = 19., int left_ = 82, int right_ = 328, double spStep_=250./4096., QString ExpName_="nobody");
    ~Net();
    bool ClassificateVector(int &vecNum);
//    double ClassificateVectorError(int &vecNum);
    void closeLogFile();
    void setAutoProcessingFlag(bool);

    void adjustReduceCoeff(QString spectraDir, int lowLimit, int highLimit, MakePa * outMkPa);
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
//    void leaveOneOutCL();
    double getAverageAccuracy();
    void setReduceCoeff(double coeff);
    double getReduceCoeff();
    void setNumOfPairs(int num);
    void saveWts(QString wtsPath);

protected:
    bool event(QEvent * ev);
    void mousePressEvent(QMouseEvent * event);

public slots:
    void readCfg();
    void loadWts();
    void loadWtsByName(QString);

    void LearnNet();

    void tall();
    void reset();
    void saveWtsSlot();
    void stopActivity();
    void drawWts();
    void PaIntoMatrixByName(QString fileName);
    void PaIntoMatrix();
    void leaveOneOutSlot();
    void neuronGas();
    void pca();
    void drawSammon();
    void averageClassification();
    void autoClassification(QString spectraDir);
    void autoClassificationSimple();
    void autoPCAClassification();
    void SVM();
    void Hopfield();
    void methodSetParam(int, bool);
    void memoryAndParamsAllocation();
    void testDistances();
    void optimizeChannelsSet();
    void rcpSlot();

private:
    Ui::Net *ui;
    QString helpString;
    QString ExpName;
    QDir *dir, *dirBC;
    double **matrix;
    double *** weight;
    int * dimensionality; //for backprop
    double * output;
    char * helpCharArr;
    ifstream inStream;
    ofstream outStream;
    double * classCount;

//    double * tempRandoms;
    double ** tempRandomMatrix; //test linear transform

    QVector<int> channelsSet;
    QVector<int> channelsSetExclude;
    double averageAccuracy;

    QPixmap pic;
    QPainter *paint;
    int numOfLayers; //for backprop
    int numTest;
    int epoch;
    int  NetLength; //inputs
    int NumOfClasses; //outputs
    int NumberOfVectors;
    bool autoFlag;
    double critError;
    double currentError;
    double temperature;
    double learnRate;
    int * NumberOfErrors;
    char **FileName;
    int spLength;
    int left, right;
    int helpInt;
    double spStep;
    double Error;
    QPixmap columns;
    FILE * log;
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
