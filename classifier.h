#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "smallLib.h"
#include "library.h"
#include "coord.h"
#include "matrix.h"
#include "classifierdata.h"

#include <QString>
#include <vector>
#include <utility>
#include <ios>
#include <fstream>
#include <iostream>
#include <random>
#include <QTime>
#include <chrono>

enum class ClassifierType {ANN, RDA, SVM, DIST, NBC, KNN, WARD};
typedef std::pair<double, double> avType;

#define CLASS_TEST_VIRTUAL 0
class Classifier
{
protected:
    ClassifierType myType;
    QString typeString;

	/// dataClass
    const matrix * dataMatrix = nullptr; // biases for Net are imaginary
    const std::vector<uint> * types = nullptr;
    const std::vector<QString> * fileNames = nullptr;
    const std::valarray<double> * classCount = nullptr; // really int but...
    std::valarray<double> apriori;

	/// to do
//	ClassifierData * myData;

    matrix confusionMatrix; // rows - realClass, cols - outClass
    double averageAccuracy;
    double kappa;
    uint numCl;

	bool testCleanFlag  = false; /// delete wrong classified files
    bool resetFlag = true; /// reset learning values before new learning


/// sheeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeet
    QString resultsPath = def::dir->absolutePath() + myLib::slash + "results.txt";
    QString workDir = def::dir->absolutePath() + myLib::slash + "PA";
    QString filesPath = def::dir->absolutePath()
                        + myLib::slash + "SpectraSmooth"
                        + myLib::slash + "windows";

public:
    const ClassifierType & getType() {return myType;}
    const QString & getTypeString() {return typeString;}
    void setTestCleanFlag(bool inFlag);
    void deleteFile(uint vecNum, uint predClass);
    void printResult(const QString & fileName, uint predType, uint vecNum);
    void setData(matrix & inMat);
    void setTypes(std::vector<uint> & inTypes);
    void setClassCount(std::valarray<double> & inClassCount);
    void setFileNames(std::vector<QString> & inFileNames);
    void setFilesPath(const QString & inPath);
    void setApriori(const std::valarray<double> & in = std::valarray<double>());

    /// crutch
    void confMatInc(int trueClass, int predClass){confusionMatrix[trueClass][predClass] += 1.;}





public:
    Classifier();
    virtual ~Classifier();

    avType averageClassification(); /// on confusionMatrix
    void learnAll();
    virtual void learn(std::vector<uint> & indices) = 0;
#if CLASS_TEST_VIRTUAL
    virtual void test(const std::vector<uint> & indices) = 0;
#else
    void testAll();
    void test(const std::vector<uint> & indices);
#endif
    virtual std::pair<uint, double> classifyDatum(const uint & vecNum) = 0;
    virtual void successiveRelearn();
	virtual void printParams();
};



#define WEIGHT_MATRIX 0
class ANN : public Classifier
{
#if WEIGHT_MATRIX
    typedef std::vector<matrix> weightType;
#else
    typedef std::vector<std::vector<std::valarray<double>>> weightType;
#endif
    typedef std::vector<std::valarray<double>> outputType;

private:
    int epoch = 0;
    const int epochLimit = 250;
    static const int epochHighLimit = 120;
    static const int epochLowLimit = 80;

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
    void allocParams(weightType & inMat);
    void zeroParams();
    std::valarray<double> (*activation)(const std::valarray<double> & in) = smallLib::softmax;
    void loadVector(const uint vecNum, uint & type);
    void countOutput();
    void countOutputDelta();
    void countOutputBackprop();
    void countError(uint type,
                    double & currentError);
    void moveWeights(const std::vector<double> & normCoeff,
                     const uint type);

public:
    ANN();


    void setCritError(double in);
    void setResetFlag(bool inFlag);
    void setDim(const std::vector<int> & inDim);
    void setLrate(double inRate);

    int getEpoch();
    double getLrate();
    const weightType & getWeight();

    void readWeight(const QString & fileName,
                    weightType * wtsMatrix = nullptr);
    void writeWeight(const QString & wtsPath = QString());
    void drawWeight(QString wtsPath = QString(),
                    QString picPath = QString());
    double adjustLearnRate();


protected:
    void learn(std::vector<uint> & indices) override;
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<uint> & indices) override;
#endif
    std::pair<uint, double> classifyDatum(const uint & vecNum) override;
    /// successive
    void successiveRelearn() override;
};


class RDA : public Classifier
{
private:
    std::vector<matrix> covMat;
    std::vector<std::valarray<double>> centers;
    std::vector<double> dets;
    double gamma; /// gamma
    double lambda;

public:
    RDA();
    void setShrinkage(double);
    void setLambda(double); /// 0 - QDA, 1 - LDA
	void printParams() override;

protected:
    void learn(std::vector<uint> & indices) override;
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<uint> & indices) override;
#endif
	std::pair<uint, double> classifyDatum(const uint & vecNum) override;
};


class SVM : public Classifier
{
private:
    /// these two are really static
    const QString learnFileName = "svmLearn";
    const QString testFileName = "svmTest";
    const QString outputFileName = "output";
    int kernelNum = 0;
    int svmType = 0;
    void makeFile(const std::vector<uint> & indices,
                  const QString & fileName);

public:
    SVM();
    void setKernelNum(int inNum);
    void setSvmType(int inNum);

protected:
    void learn(std::vector<uint> & indices) override;
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<uint> & indices) override;
#endif
    std::pair<uint, double> classifyDatum(const uint & vecNum) override;
};



class DIST : public Classifier
{
private:
    std::vector<std::valarray<double>> centers;

public:
    DIST();

protected:
    void learn(std::vector<uint> & indices) override;
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<uint> & indices) override;
#endif
    std::pair<uint, double> classifyDatum(const uint & vecNum) override;
};

/// gauss NBC
class NBC : public Classifier
{
private:
    std::vector<std::valarray<double>> centers;
    std::vector<std::valarray<double>> sigmas; //[class][feature]

public:
    NBC();

protected:
    void learn(std::vector<uint> & indices) override;
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<uint> & indices) override;
#endif
    std::pair<uint, double> classifyDatum(const uint & vecNum) override;
};



class KNN : public Classifier
{
private:
    matrix distances{};
    int numOfNear;

public:
    KNN();

    void setNumOfNear(int);

protected:
    void learn(std::vector<uint> & indices) override;
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<uint> & indices) override;
#endif
    std::pair<uint, double> classifyDatum(const uint & vecNum) override;
};


class WARD : public Classifier
{
private:
    std::vector<std::vector<uint>> clusts; /// vectors of indices of dataMatrix
    std::vector<std::valarray<double>> centers;
    matrix dists{};
    uint numOfClust;

    double dist(const uint a, const uint b);
    void merge(const uint one, const uint two);
    std::pair<uint, uint> findNearest();

public:
    WARD();

    void setNumClust(int ii);

protected:
    void learn(std::vector<uint> & indices) override;
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<uint> & indices) override;
#endif
    std::pair<uint, double> classifyDatum(const uint & vecNum) override;
};


#endif // CLASSIFIER_H
