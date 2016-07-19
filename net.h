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
    /// ui things
    std::vector<QButtonGroup *> myButtonGroup;
    bool stopFlag = false;
    bool autoFlag = false;
    /// ui private methods
    void aaDefaultSettings();

    /// data
    enum class myMode {N_fold, k_fold, train_test,  half_half};
    enum class source {winds, reals, pca, bayes};
    myMode Mode = myMode::N_fold;
    source Source = source::reals;

    matrix dataMatrix{}; // biases and types separately
    std::vector<int> types{};
    std::vector<QString> fileNames{};
    std::vector<double> classCount{}; // really int but...
//    matrix tempRandomMatrix; //test linear transform
    double loadDataNorm = 10.;
    lineType averageDatum;
    lineType sigmaVector;

    /// classification
    Classifier * myClassifier = nullptr;
    void setClassifierParams();
    /// succesiive
    int numGoodNew;
    void successiveLearning(const std::valarray<double> & newSpectre,
                            const int newType,
                            const QString & newFileName);


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
    QString filesPath;

    /// class
    void crossClassification();
    void leaveOneOutClassification();
    void halfHalfClassification();
    void trainTestClassification(const QString & trainTemplate = "_train",
                                 const QString & testTemplate = "_test");

public:
    explicit Net();
    ~Net();


    void customF();

    /// classification
    void autoClassification(const QString & spectraDir);
    /// NEED CHECK
    void successiveProcessing();
    void successivePreclean(const QString & spectraPath);

    /// setsgets
    void setErrCrit(double in);
    void setLrate(double in);
    double getLrate();
    void setNumOfPairs(int num);
    void setFold(int in);
    void setAutoProcessingFlag(bool);
    void setMode(const QString & in = "N-fold");
    void setSource(const QString & in = "reals");
    void setClassifier(const QString &);
    void setClassifier(ClassifierType);

    /// data
    void setVariancing(bool);
    void setCentering(bool);
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
    void loadDataSlot();
    void stopActivity();
    void pca();

    void autoClassificationSimple(); /// on SpectraSmooth + Source
    avType autoClassification(); /// on dataMatrix + types

    void setSourceSlot(QAbstractButton*);
    void setModeSlot(QAbstractButton*, bool);
    void setClassifier(QAbstractButton*, bool);


    /// ANN
    void setLrateSlot(double in);
    void setErrCritSlot(double in);
    void setDimensionalitySlot();

    void readWtsSlot();
    void drawWtsSlot();
    void writeWtsSlot();

    /// SVM
    void setSvmTypeSlot(int);
    void setKernelNumSlot(int);

    /// KNN
    void setKnnNumSlot(int);

    /// WORD
    void setWordNumSlot(int);

    /// LDA/QDA
    void setRdaShrinkSlot(double);
    void setRdaLambdaSlot(double);


};

#endif // NET_H
