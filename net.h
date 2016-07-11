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

    std::vector<QButtonGroup *> myButtonGroup;

    Classifier * myClassifier = nullptr;
    /// change everywhere
    enum class myMode {N_fold, k_fold, train_test,  half_half};
    enum class source {winds, reals, pca, bayes};
    myMode Mode = myMode::N_fold;
    source Source = source::reals;

    matrix dataMatrix{}; // biases and types separately
    std::vector<int> types{};
    std::vector<QString> fileNames{};
    std::vector<double> classCount{}; // really int but...

    double loadDataNorm = 10.;
    lineType averageDatum;
    lineType sigmaVector;

//    matrix tempRandomMatrix; //test linear transform

    enum class errorNetType {SME, maxDist};
    const errorNetType errType = errorNetType::SME;
    const double errorThreshold = 1.0;

    bool resetFlag = true;
    bool stopFlag = false;
    bool autoFlag = false;
    bool tallCleanFlag  = false;

    matrix coords; //new coords for Sammon method


    /// private methods
    void setClassifierParams();
    void aaDefaultSettings();

    /// data
    void normalizeDataMatrix();
    void popBackDatum();
    void pushBackDatum(const lineType & inDatum,
                     const int & inType,
                     const QString & inFileName);
    void eraseDatum(const int & index);
    void eraseData(const std::vector<int> & indices);
    std::pair<std::vector<int>, std::vector<int>> makeIndicesSetsCross(
            const std::vector<std::vector<int> > & arr,
            const int numOfFold);

    /// class
    void crossClassification();
    void leaveOneOutClassification();
    void halfHalfClassification();
    void trainTestClassification(const QString & trainTemplate = "_train",
                                 const QString & testTemplate = "_test");
    void leaveOneOut();

public:
    explicit Net();
    ~Net();

    /// classification
    void autoClassification(const QString & spectraDir);

    /// setsgets
    void setErrCrit(double in);
    void setLrate(double in);
    double getLrate();
    void setNumOfPairs(int num);
    void setFold(int in);
    void setAutoProcessingFlag(bool);
    void setMode(const QString & in = "N-fold");
    void setSource(const QString & in = "reals");

    /// data
    void loadData(const QString & spectraPath = def::dir->absolutePath()
                                                + myLib::slash + "SpectraSmooth",
                  const QStringList & filters = {},
                  double rdcCoeff = 1.);
    void loadData(const matrix & inMat,
                  const std::vector<int> & inTypes);


    ///wts
    void readWtsByName(const QString & fileName,
                       twovector<lineType> * wtsMatrix = nullptr);
    void writeWts(const QString & wtsPath = QString());
    void drawWts(QString wtsPath = QString(),
                 QString picPath = QString());

public slots:
    void readWtsSlot();
    void drawWtsSlot();
    void writeWtsSlot();

    void loadDataSlot();
    void stopActivity();
    void pca();

    void autoClassificationSimple(); /// on SpectraSmooth + Source
    void autoClassification(); /// on dataMatrix + types

    void setSourceSlot(QAbstractButton*);
    void setModeSlot(QAbstractButton*, bool i);
    void setClassifier(QAbstractButton*);



};

#endif // NET_H
