#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <vector>
#include <utility>
#include <ios>
#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include <set>

#include <myLib/small.h>
#include <myLib/output.h>
#include <myLib/draws.h>
#include <myLib/drw.h>
#include <myLib/general.h>

#include <other/consts.h>
#include <other/defs.h>
#include <other/matrix.h>
#include <classifier/classifierdata.h>

#include <QString>
#include <QTime>

enum class ModelType {ANN, RDA, SVM, DIST, NBC, KNN, WARD};


namespace clLib
{
/// useful functions for classification
double countError(const std::valarray<double> & vals, int trueType);

std::valarray<double> oneHot(uint siz, uint hotIndex);

} /// end of namespace clLib


class Classifier
{
public:
	using avType = std::pair<double, double>;				/// average accuracy, Cohen's kappa
	using classOneType = std::tuple<bool, int, double>;		/// correctness, outClass, error
protected:
	ModelType myType;
    QString typeString;

	ClassifierData * myClassData;

	matrix confusionMatrix; // [realClass] [predictedClass]
	double averageAccuracy;				/// to deprecate
	double kappa;						/// to deprecate
	std::valarray<double> outputLayer;	/// output of classification

	bool testCleanFlag  = false;		/// delete wrong classified files
	bool resetFlag = true;				/// reset learning weights before new learning


/// sheeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeet
	QString resultsPath = DEFS.dirPath() + "/results.txt";
	QString workDir = DEFS.dirPath() + "/Help/PA";
	QString filesPath = DEFS.windsSpectraDir();

#if INERTIA
	std::valarray<double> fbVal;
//	const double inertiaCoef = suc::inertiaCoef;
	uint curType = 15; // not any of types
#endif


public:
	const ModelType & getType() { return myType; }
    const QString & getTypeString() { return typeString; }
    void setTestCleanFlag(bool inFlag);
    void deleteFile(uint vecNum, uint predClass);
	void printResult(const QString & fileName, uint predType, uint vecNum);

	void setClassifierData(ClassifierData & in);
	ClassifierData * getClassifierData()			{ return myClassData; }
	std::valarray<double> getOutputLayer() const	{ return outputLayer; }
	double getOutputLayer(int i) const				{ return outputLayer[i]; }

	/// 
	void peopleClassification(bool indZ, std::ostream & os = std::cout);
	void leaveOneOutClassification(std::ostream & os = std::cout);
	void crossClassification(int numOfPairs, int fold, std::ostream & os = std::cout);
	void trainTestClassification(const QString & trainTemplate = "_train",
								 const QString & testTemplate = "_test");
	void halfHalfClassification();
	void cleaningNfold(int num = 3);
	void cleaningKfold(int num = 3, int fold = 5);

private:
	std::pair<std::vector<uint>, std::vector<uint>> makeIndicesSetsCross(
			const std::vector<std::vector<uint>> & arr,
			int numOfFolds,
			const int currentFold);

public:
    Classifier();
    virtual ~Classifier();

	avType averageClassification(std::ostream & os = std::cout); /// on confusionMatrix

	virtual void learn(std::vector<uint> & indices) = 0;
	void learnAll();

	void test(const std::vector<uint> & indices);
	classOneType test(uint index) { return classifyDatum(index); }
    void testAll();

    virtual void successiveRelearn();
	virtual void printParams();
	virtual void adjustToNewData() {}

//	std::pair<uint, double> classifyDatumLast();
	classOneType classifyDatumLast();

protected:
//	std::pair<uint, double> classifyDatum(uint vecNum); // return class and error, effect on confMat
	classOneType classifyDatum(uint vecNum); // effect on confMat
	virtual void classifyDatum1(uint vecNum) = 0;		// just count outputLayer
	void classifyDatumLast1();
};



#define WEIGHT_MATRIX 0

class ANN : public Classifier
{
#if WEIGHT_MATRIX
	using weightType = std::vector<matrix> ;
#else
	using weightType = std::vector<std::vector<std::valarray<double>>>;
#endif


	using outputType = std::vector<std::valarray<double>>;

private:
    int epoch = 0;
	const int epochLimit = 249;
    static const int epochHighLimit = 120;
    static const int epochLowLimit = 80;

    enum class learnStyle {backprop, delta};
    const learnStyle learnStyl = learnStyle::delta;

    enum class errorNetType {SME, maxDist};
    const errorNetType errType = errorNetType::SME;
    double critError = 0.04;
    double learnRate = 0.05;

	mutable weightType weight{};
    outputType output{};
    std::vector<int> dim{}; /// only intermediate layers
    std::vector<std::valarray<double>> deltaWeights{};

    /// deleberately private functions
    void allocParams(weightType & inMat);
    void zeroParams();
    std::valarray<double> (*activation)(const std::valarray<double> & in) = smLib::softmax;
	void loadVector(uint vecNum, uint & type);
    void countOutput();
    void countOutputDelta();
    void countOutputBackprop();
	void countError(uint trueType,
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
	void writeWeight(const QString & wtsPath = QString()) const;
    void drawWeight(QString wtsPath = QString(),
					QString picPath = QString());
	double adjustLearnRate(std::ostream & os = std::cout);

	/// returning output layer values
	/// should be made virtual = 0


protected:
	void learn(std::vector<uint> & indices) override;
	void classifyDatum1(uint vecNum) override;
    /// successive
    void successiveRelearn() override;
	void adjustToNewData() override;

public:
//	static void cleaningNfold(matrix & inData, std::vector<uint> & inTypes);
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
	void classifyDatum1(uint vecNum) override;
	void adjustToNewData() override;
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
	void classifyDatum1(uint vecNum) override;
//	void adjustToNewData() override;
};



class DIST : public Classifier
{
private:
    std::vector<std::valarray<double>> centers;

public:
    DIST();

protected:
	void learn(std::vector<uint> & indices) override;
	void classifyDatum1(uint vecNum) override;
	void adjustToNewData() override;
};

/// gauss NBC
class NBC : public Classifier
{
private:
    std::vector<std::valarray<double>> centers;
    std::vector<std::valarray<double>> sigmas; // [class][feature]

public:
    NBC();

protected:
	void learn(std::vector<uint> & indices) override;
	void classifyDatum1(uint vecNum) override;
	void adjustToNewData() override;
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
	void classifyDatum1(uint vecNum) override;
//	void adjustToNewData() override;
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
	void classifyDatum1(uint vecNum) override;
//	void adjustToNewData() override;
};


#endif // CLASSIFIER_H
