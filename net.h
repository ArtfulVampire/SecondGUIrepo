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


#include <QMessageBox>


namespace Ui {
    class Net;
}

class Net : public QWidget
{
    Q_OBJECT

private:
    Ui::Net * ui;

    QButtonGroup  * group1;
    QButtonGroup  * group2;
    QButtonGroup  * group3;
    QButtonGroup  * group4;

    matrix dataMatrix{}; // biases and types separately
    std::vector<int> types;
    std::vector<QString> fileNames;
    std::vector<double> classCount; // really int but...

    double loadDataNorm = 10.;
    lineType averageDatum;
    lineType sigmaVector;

    matrix confusionMatrix; // rows - realClass, cols - outClass

    twovector<lineType> weight;
    std::vector<int> dimensionality; // for backprop

//    matrix tempRandomMatrix; //test linear transform

    std::vector<int> channelsSet;
    std::vector<int> channelsSetExclude;

    enum class errorNetType {SME, maxDist};
    const errorNetType errType = errorNetType::SME;
    const double errorThreshold = 1.0;

    double averageAccuracy;
    double kappa; // Cohen's
    int epoch;

    bool stopFlag = false;
    bool autoFlag = false;
    bool tallCleanFlag  = false;

    matrix coords; //new coords for Sammon method

public:
    explicit Net();
    ~Net();

    void closeLogFile();
    void setAutoProcessingFlag(bool);

    void prelearnDeepBelief();
    std::vector<int> learnIndicesSet();
    double adjustLearnRate(int lowLimit,
                           int highLimit);
    double adjustReduceCoeff(QString spectraDir,
                             int lowLimit,
                             int highLimit,
                             QString paFileName = "all");
    std::pair<int, double> classifyDatum(const int & vecNum);

    void leaveOneOut();

    std::vector<int> makeLearnIndexSet();
    std::pair<std::vector<int>, std::vector<int>> makeIndicesSetsCross(
            const std::vector<std::vector<int> > & arr,
            const int numOfFold);
    std::valarray<double> (*activation)(const std::valarray<double> & in,
                                        double temp) = smallLib::softmax;
    void autoClassification(const QString & spectraDir);
    void averageClassification();


    double getAverageAccuracy();
    double getKappa();
    double getReduceCoeff();
    int getEpoch();
    double getLrate();
    matrix getConfusionMatrix();

    void setReduceCoeff(double coeff);
    void setErrCrit(double in);
    void setLrate(double in);
    void setNumOfPairs(int num);
    void setMode(const QString & in = "N-fold");
    void setActFunc(const QString & in = "softmax");
    void setSource(const QString & in = "reals");
    void setFold(int in);
    void setTallCleanFlag(bool in);
    void aaDefaultSettings();


    void Sammon(double ** distArr, int size, int * colors);
    void Kohonen(double ** input, double ** eigenVects, double * averageProjection, int size, int length);
    void moveCoordsGradient(double ** coords, double ** distOld, double ** distNew, int size);
    double thetalpha(int bmu_, int j_, int step_, double ** arr, int length_);


    double mouseClick(QLabel * label, QMouseEvent * ev);


    void crossClassification();
    void leaveOneOutClassification();
    void halfHalfClassification();
    void trainTestClassification(const QString & trainTemplate = "_train",
                                 const QString & testTemplate = "_test");


    void writeWts(const QString & wtsPath = QString());
    void PaIntoMatrixByName(const QString & fileName);
    void normalizeDataMatrix();
    void loadData(const QString & spectraPath = def::dir->absolutePath()
                                                + myLib::slash + "SpectraSmooth",
                  const QStringList & filters = {},
                  double rdcCoeff = 1.);
    void loadData(const matrix & inMat,
                  const std::vector<int> inTypes);
    void popBackDatum();
    void pushBackDatum(const lineType & inDatum,
                     const int & inType,
                     const QString & inFileName);
    void eraseDatum(const int & index);
    void eraseData(const std::vector<int> & indices);

    void drawWts(QString wtsPath = QString(),
                 QString picPath = QString());
    void readWtsByName(const QString & fileName,
                       twovector<lineType> * wtsMatrix = nullptr);

    void learnNetIndices(std::vector<int> mixNum,
                         const bool resetFlag = true);
    void tallNetIndices(const std::vector<int> & indices);

    void learnBayesIndices(std::vector<int> mixNum);
    void tallBayesIndices(std::vector<int> mixNum);

    /// change everywhere
    enum class myMode {N_fold, k_fold, train_test,  half_half};
    enum class source {winds, reals, pca, bayes};

    myMode Mode = myMode::N_fold;
    source Source = source::winds;

    void successiveProcessing();

    void successiveLearning(const lineType & newSpectre,
                           const int newType,
                           const QString & newFileName);
    void successiveRelearn();
    void successivePreclean(const QString & spectraPath);


public slots:
//    void readCfg();
    void readWts();

    void learnNet(const bool resetFlag = true);
    void tallNet();


    void reset();
    void writeWtsSlot();
    void stopActivity();
    void drawWtsSlot();

    void PaIntoMatrix();
    void loadDataSlot();

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
    void setSourceSlot(QAbstractButton*);
    void setModeSlot(QAbstractButton*, bool i);
    void setActFuncSlot(QAbstractButton*);



};

#endif // NET_H
