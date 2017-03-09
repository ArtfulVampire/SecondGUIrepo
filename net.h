#ifndef NET_H
#define NET_H

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
#include "tempthread.h"
#include "library.h"
#include <fstream>
#include <ios>
#include <unistd.h>
#include <QTime>
#include "classifier.h"
#include "classifierdata.h"
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

    /// classification
    Classifier * myClassifier = nullptr;
	ClassifierData myClassifierData{};



	/// succesiive
	std::valarray<int> passed{};
    matrix pcaMat{};
	void successiveLearning(const std::valarray<double> & newSpectre,
							const uint newType,
							const QString & newFileName);


    /// data
    std::pair<std::vector<uint>, std::vector<uint>> makeIndicesSetsCross(
            const std::vector<std::vector<uint> > & arr,
            const int numOfFold);

    QString filesPath;

    /// class
    void crossClassification();
    void leaveOneOutClassification();
    void halfHalfClassification();
    void trainTestClassification(const QString & trainTemplate = "_train",
                                 const QString & testTemplate = "_test");
    void cycleParams(std::vector<std::vector<double>> & in); /// for customF

public:
    explicit Net();
    ~Net();


    void customF();

    /// classification
    void autoClassification(const QString & spectraDir);
    /// NEED CHECK
    void successiveProcessing();
	void successiveByEDF(const QString & edfPath1, const QString & edfPath2);
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
	const Classifier & getClassifier() const {return *myClassifier;}
	const ClassifierData & getClassifierData() const {return myClassifierData;}

	/// data
    void loadData(const QString & spectraPath = def::dir->absolutePath()
                                                + slash + "SpectraSmooth",
				  const QStringList & filters = {});
	void loadDataUCI(const QString & setName);


    ///wts
    void readWtsByName(const QString & fileName,
                       twovector<std::valarray<double>> * wtsMatrix = nullptr);
    void writeWts(const QString & wtsPath = QString());
    void drawWts(QString wtsPath = QString(),
                 QString picPath = QString());

public slots:
	void loadDataSlot();
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
    void setSvmKernelNumSlot(int);

    /// KNN
    void setKnnNumSlot(int);

    /// WARD
    void setWordNumSlot(int);

    /// LDA/QDA
    void setRdaShrinkSlot(double);
    void setRdaLambdaSlot(double);


};

#endif // NET_H
