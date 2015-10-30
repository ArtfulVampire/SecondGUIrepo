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
#include "library.h"
#include <fstream>
#include <ios>
#include <unistd.h>
#include <QTime>

using namespace std;

#include <QMessageBox>


namespace Ui {
    class Net;
}

class Net : public QWidget
{
    Q_OBJECT

//    friend class MainWindow;

public:
    explicit Net();
    ~Net();
    bool ClassificateVector(const int & vecNum);
    void closeLogFile();
    void setAutoProcessingFlag(bool);
    void prelearnDeepBelief();

    double adjustReduceCoeff(QString spectraDir,
                             int lowLimit,
                             int highLimit,
                             QString paFileName = "1");
    int getEpoch();
    void setErrcrit(double);
    double getErrcrit();

    void Sammon(double ** distArr, int size, int * colors);
    void Kohonen(double ** input, double ** eigenVects, double * averageProjection, int size, int length);
    void moveCoordsGradient(double ** coords, double ** distOld, double ** distNew, int size);
    double thetalpha(int bmu_, int j_, int step_, double ** arr, int length_);

    void readCfgByName(const QString & cfgFilePath);
    double setPercentageForClean();
    double mouseClick(QLabel * label, QMouseEvent * ev);
    void leaveOneOut();
    double getAverageAccuracy();
    void setReduceCoeff(double coeff);
    double getReduceCoeff();
    void setNumOfPairs(int num);
    void writeWts(const QString &wtsPath = def::dir->absolutePath()
                                           + slash() + def::ExpName + ".wts");
    void PaIntoMatrixByName(const QString & fileName);
    void autoClassification(const QString & spectraDir);
    void averageClassification();

    void drawWts(QString wtsPath = QString(),
                 QString picPath = QString());

protected:
    void mousePressEvent(QMouseEvent * event);

public slots:
    void readCfg();
    void readWts();

    void LearNetIndices(vector<int> mixNum);
    void LearnNet();

    void tall();
    void tallIndices(const vector<int> & indices);


    void reset();
    void writeWtsSlot();
    void stopActivity();
    void drawWtsSlot();
    void PaIntoMatrix();
    void leaveOneOutSlot();
    void neuronGas();
    void pca();
    void drawSammon();
    void autoClassificationSimple();
    void autoPCAClassification();
    void SVM();
    void Hopfield();
    void methodSetParam(int, bool);
    void memoryAndParamsAllocation();
    void testDistances();
    void optimizeChannelsSet();
    void adjustParamsGroup2(QAbstractButton*);

private:
    Ui::Net *ui;

    QButtonGroup  * group1,  * group2,  * group3;
    QDir *dirBC;
    matrix dataMatrix;

    double *** weight;
    int * dimensionality; //for backprop
//    double * output;
    char * helpCharArr;
    vector<double> classCount;

    matrix tempRandomMatrix; //test linear transform

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
    double Error;

    int * NumberOfErrors;
    vector<QString> fileNames;

    QPixmap columns;

    int numOfTall;
    int loadPAflag;
    bool stopFlag;
    QString tmp, paFileBC;

    double ** coords; //new coords for Sammon method
    double mouseShit;

public:

    void readWtsByName(const QString & fileName,
                       double * *** wtsMatrix = nullptr);
};

#endif // NET_H
