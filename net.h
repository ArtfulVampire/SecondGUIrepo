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
#include "qtempevent.h"
#include "tempthread.h"
#include "library.h"
#include <fstream>
#include <ios>
#include <unistd.h>
#include <QTime>
#include "classifier.h"

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
    QButtonGroup  * group5;

    Classifier * myClassifier;

    matrix dataMatrix{}; // biases and types separately
    std::vector<int> types;
    std::vector<QString> fileNames;
    std::vector<double> classCount; // really int but...
    matrix confusionMatrix; // rows - realClass, cols - outClass

    double loadDataNorm = 10.;
    lineType averageDatum;
    lineType sigmaVector;

    std::vector<int> channelsSet;
    std::vector<int> channelsSetExclude;

    /// pewpewpewpewpewpewpewpewpewpewpew
    matrix covMat[3];
    lineType centers[3];
    double dets[3];

    twovector<lineType> weight;
    std::vector<int> dimensionality; // for backprop - to deprecate
    std::valarray<double> (*activation)(const std::valarray<double> & in) = smallLib::softmax;


//    matrix tempRandomMatrix; //test linear transform

    enum class errorNetType {SME, maxDist};
    const errorNetType errType = errorNetType::SME;
    const double errorThreshold = 1.0;

    double averageAccuracy;
    double kappa; // Cohen's
    int epoch;

    bool resetFlag = true;
    bool stopFlag = false;
    bool autoFlag = false;
    bool tallCleanFlag  = false;

    matrix coords; //new coords for Sammon method

public:
    explicit Net();
    ~Net();

    /// classification
    void autoClassification(const QString & spectraDir);
    void averageClassification(matrix * inMat = nullptr); /// on confusionMatrix
    void crossClassification();
    void leaveOneOutClassification();
    void leaveOneOut();
    void halfHalfClassification();
    void trainTestClassification(const QString & trainTemplate = "_train",
                                 const QString & testTemplate = "_test");
    /// change everywhere
    enum class myMode {N_fold, k_fold, train_test,  half_half};
    enum class source {winds, reals, pca, bayes};

    myMode Mode = myMode::N_fold;
    source Source = source::winds;

    /// make a class
    void learnIndicesNet(std::vector<int> & indices);
    std::pair<int, double> classifyDatumNet(const int & vecNum);
    void tallIndicesNet(const std::vector<int> & indices);

    void learnIndicesQDA(std::vector<int> & indices); // really const vec &, but for *func
    std::pair<int, double> classifyDatumQDA(const int & vecNum);
    void tallIndicesQDA(const std::vector<int> & indices);

    void (*learnIndicesFunc)(std::vector<int> indices);
    std::pair<int, double> (*classifyDatumFunc)(const int & vecNum);
    void (*tallIndicesFunc)(const std::vector<int> & indices);



    /// successive
    void successiveProcessing();

    void successiveLearning(const lineType & newSpectre,
                           const int newType,
                           const QString & newFileName);
    void successiveRelearn();
    void successivePreclean(const QString & spectraPath);





    /// setsgets
    void setReduceCoeff(double coeff);
    double getReduceCoeff();
    void setErrCrit(double in);
    void setLrate(double in);
    double getLrate();
    void setNumOfPairs(int num);
    void setFold(int in);
    void setTallCleanFlag(bool in);
    int getEpoch();
    double getAverageAccuracy();
    double getKappa();
    void setAutoProcessingFlag(bool);
    const matrix & getConfusionMatrix();
    void setMode(const QString & in = "N-fold");
    void setActFunc(const QString & in = "softmax");
    void setSource(const QString & in = "reals");
    void aaDefaultSettings();

    /// data
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
    std::pair<std::vector<int>, std::vector<int>> makeIndicesSetsCross(
            const std::vector<std::vector<int> > & arr,
            const int numOfFold);

    std::vector<int> makeLearnIndexSet(); /// for adjust learn rate only
    double adjustLearnRate(int lowLimit,
                           int highLimit);
    double adjustReduceCoeff(QString spectraDir,
                             int lowLimit,
                             int highLimit,
                             QString paFileName = "all");

    ///wts
    void readWtsByName(const QString & fileName,
                       twovector<lineType> * wtsMatrix = nullptr);
    void writeWts(const QString & wtsPath = QString());
    void drawWts(QString wtsPath = QString(),
                 QString picPath = QString());




    /// too old
    void Sammon(double ** distArr, int size, int * colors);
    void Kohonen(double ** input, double ** eigenVects, double * averageProjection, int size, int length);
    void moveCoordsGradient(double ** coords, double ** distOld, double ** distNew, int size);
    double thetalpha(int bmu_, int j_, int step_, double ** arr, int length_);


    double mouseClick(QLabel * label, QMouseEvent * ev);


    /// not finished
    void crossSVM(std::pair<std::vector<int>, std::vector<int>> sets);

public slots:
    void readWts();

    void learnClassifierSlot(const bool resFlag = true);
    void tallClassifierSlot();


    void resetSlot();
    void writeWtsSlot();
    void stopActivity();
    void drawWtsSlot();

    void PaIntoMatrix();
    void loadDataSlot();

    void pca();
    void autoClassificationSimple();
    void doSVM();
    void methodSetParam(int, bool);
    void testDistances();
    void setSourceSlot(QAbstractButton*);
    void setModeSlot(QAbstractButton*, bool i);
    void setActFuncSlot(QAbstractButton*);
    void setClassifier(QAbstractButton*);



};

#endif // NET_H
