#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "smallLib.h"
#include "library.h"
#include "coord.h"
#include "matrix.h"

#include <QString>
#include <vector>
#include <utility>
#include <ios>
#include <fstream>
#include <iostream>
#include <random>
#include <QTime>
#include <chrono>

class Classifier
{
protected:
    matrix * dataMatrix = nullptr; // biases for Net are imaginary
    std::vector<int> * types = nullptr;
    std::vector<QString> * fileNames = nullptr;
    std::vector<double> * classCount = nullptr; // really int but...

    matrix confusionMatrix{}; // rows - realClass, cols - outClass
    QString resultsPath = def::dir->absolutePath() + myLib::slash + "results.txt";
    double averageAccuracy = 0;
    double kappa = 0;
    uint numCl = 0;

public:
    Classifier();
    ~Classifier();

    void setData(matrix & inMat);
    void setTypes(std::vector<int> & inTypes);
    void setClassCount(std::vector<double> & inClassCount);
    void setFileNames(std::vector<QString> & inFileNames);
    double averageClassification(); /// on confusionMatrix

    virtual void learn(std::vector<int> & indices) = 0;
    virtual void test(const std::vector<int> & indices) = 0;
    virtual std::pair<int, double> classifyDatum(const int & vecNum) = 0;
};


class ANN : public Classifier
{
    typedef std::vector<std::vector<std::valarray<double>>> weightType;
    typedef std::vector<std::valarray<double>> outputType;

private:
    bool resetFlag = true;
    bool testCleanFlag  = false;
    int epoch = 0;
    const int epochLimit = 250;

    enum class learnStyle {backprop, delta};
    const learnStyle learnStyl = learnStyle::delta;

    enum class errorNetType {SME, maxDist};
    const errorNetType errType = errorNetType::SME;
    double critError = 0.04;


    double learnRate = 0.05;
    weightType weight{};
    outputType output{};
    std::vector<int> dim{}; /// only intermediate layers
    std::vector<std::valarray<double>> deltaWeights{};

    /// deleberately private functions
    void allocWeight(weightType & inMat);
    void zeroParams();
    std::valarray<double> (*activation)(const std::valarray<double> & in) = smallLib::softmax;
    void loadVector(const int vecNum, int & type);
    void countOutput();
    void countOutputDelta();
    void countOutputBackprop();
    void countError(int type,
                    double & currentError);
    void moveWeights(const std::vector<double> & normCoeff,
                     const int type);

public:
    ANN();
    ~ANN();

    void setResetFlag(bool inFlag);
    void setTestCleanFlag(bool inFlag);
    void setDim(const std::vector<int> & inDim);
    int getEpoch();
    double getLrate();
    void setLrate(double inRate);
    const weightType & getWeight();

    void readWeight(const QString & fileName,
                    weightType * wtsMatrix = nullptr);
    void writeWeight(const QString & wtsPath = QString());
    void drawWeight(QString wtsPath = QString(),
                    QString picPath = QString());


    void successiveProcessing();
    void successiveLearning(const std::valarray<double> & newSpectre,
                           const int newType,
                           const QString & newFileName);
    void successiveRelearn();
    void successivePreclean(const QString & spectraPath);

protected:
    void learn(std::vector<int> & indices);
    void test(const std::vector<int> & indices);
    std::pair<int, double> classifyDatum(const int & vecNum);
};

class QDA : public Classifier
{
private:
    std::vector<matrix> covMat;
    std::vector<std::valarray<double>> centers;
    std::vector<double> dets;

public:
    QDA();
    ~QDA();

protected:
    void learn(std::vector<int> & indices);
    void test(const std::vector<int> & indices);
    std::pair<int, double> classifyDatum(const int & vecNum);
};

class SVM : public Classifier
{
private:
    /// these two are really static
    const QString learnFileName = "svmLearn";
    const QString testFileName = "svmTest";

    QString workDir = def::dir->absolutePath() + myLib::slash + "PA";
    int kernelNum = 0;
    int numOfPairs = 15;
    int fold = 4;

public:
    SVM();
    ~SVM();
    void setKernelNum(int inNum);
    void setNumPairs(int inNum);
    void setFold(int inFold);
    void makeFile(const std::vector<int> & indices,
                  const QString & fileName);

protected:
    void learn(std::vector<int> & indices);
    void test(const std::vector<int> & indices);
    std::pair<int, double> classifyDatum(const int & vecNum);
};

#endif // CLASSIFIER_H
