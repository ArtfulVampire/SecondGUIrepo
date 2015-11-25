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

private:
    Ui::Net *ui;

    QButtonGroup  * group1,  * group2,  * group3;

    matrix dataMatrix; // biases and types separately
    vector<int> types;
    vector<QString> fileNames;
    vector<double> classCount; // really int but...

    matrix confusionMatrix; // rows - realClass, cols - outClass

    twovector<lineType> weight;
    vector<uint> dimensionality; // for backprop

//    matrix tempRandomMatrix; //test linear transform

    vector<int> channelsSet;
    vector<int> channelsSetExclude;

    double averageAccuracy;
    int epoch;
    int numOfTall;

    int loadPAflag;
    bool stopFlag;
    bool autoFlag;

    matrix coords; //new coords for Sammon method

public:
    explicit Net();
    ~Net();
    int ClassificateVector(const int & vecNum);
    void closeLogFile();
    void setAutoProcessingFlag(bool);
    void prelearnDeepBelief();
    double adjustLearnRate(int lowLimit,
                           int highLimit);
    double adjustReduceCoeff(QString spectraDir,
                             int lowLimit,
                             int highLimit,
                             QString paFileName = "all");
    int getEpoch();

    void Sammon(double ** distArr, int size, int * colors);
    void Kohonen(double ** input, double ** eigenVects, double * averageProjection, int size, int length);
    void moveCoordsGradient(double ** coords, double ** distOld, double ** distNew, int size);
    double thetalpha(int bmu_, int j_, int step_, double ** arr, int length_);

//    void readCfgByName(const QString & cfgFilePath);
    double mouseClick(QLabel * label, QMouseEvent * ev);
    void trainTestClassification(const QString & trainTemplate = "train",
                                 const QString & testTemplate = "test");
    void leaveOneOut();
    double getAverageAccuracy();
    void setReduceCoeff(double coeff);
    double getReduceCoeff();
    void setNumOfPairs(int num);
    void writeWts(const QString &wtsPath = def::dir->absolutePath()
                                           + slash() + def::ExpName + ".wts");
    void PaIntoMatrixByName(const QString & fileName);
    void loadData(const QString & spectraPath = def::dir->absolutePath()
                                                + slash() + "SpectraSmooth",
                  double rdcCoeff = 1.);
    void makeIndicesVectors(vector<int> & learnInd,
                            vector<int> & tallInd,
                            vector<vector<int> > & arr,
                            const int numOfFold);
    void autoClassification(const QString & spectraDir);
    void averageClassification();



    void readWtsByName(const QString & fileName,
                       twovector<lineType> * wtsMatrix = nullptr);
    void drawWts(const QString & wtsPath = QString(),
                 QString picPath = QString());


public slots:
//    void readCfg();
    void readWts();

    void LearNetIndices(vector<int> mixNum);
    void LearnNet();

    void tall();
    void tallNetIndices(const vector<int> & indices);


    void reset();
    void writeWtsSlot();
    void stopActivity();
    void drawWtsSlot();

    void PaIntoMatrix();
    void loadDataSlot();

    void leaveOneOutSlot();
    void neuronGas();
    void pca();
    void drawSammon();
    void autoClassificationSimple();
    void autoPCAClassification();
    void SVM();
    void Hopfield();
    void methodSetParam(int, bool);
//    void memoryAndParamsAllocation();
    void testDistances();
    void optimizeChannelsSet();
    void adjustParamsGroup2(QAbstractButton*);



};

#endif // NET_H
