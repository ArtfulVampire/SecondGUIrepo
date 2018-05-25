#ifndef CLASSIFIERDATA_H
#define CLASSIFIERDATA_H

#include <iostream>
#include <valarray>
#include <vector>

#include <other/matrix.h>
#include <other/defs.h>
#include <myLib/small.h>
#include <myLib/output.h>



class ClassifierData
{
public:
	ClassifierData() { numOfCl = 3; }
	~ClassifierData() {}

	ClassifierData(const QString & inPath, const QStringList & filters = QStringList());
	ClassifierData(const matrix & inData, const std::vector<uint> & inTypes);

	ClassifierData(const ClassifierData &) = default;
	ClassifierData(ClassifierData &&) = default;
	ClassifierData & operator=(const ClassifierData &) = default;
	ClassifierData & operator=(const matrix &);

	void adjust();

	/// gets
	uint getNumOfCl() const { return numOfCl; }
	const matrix & getData() const { return dataMatrix; }
	const std::vector<uint> & getTypes() const { return types; }
	const std::vector<std::vector<uint>> & getIndices() const { return indices; }
	const std::vector<QString> & getFileNames() const { return fileNames; }
	const std::valarray<double> & getClassCount() const { return classCount; }
	const std::valarray<double> & getApriori() const { return apriori; }
	const QString & getFilesPath() const { return filesPath; }
	int size() { return dataMatrix.rows(); }

	/// sets
	void setApriori(const std::valarray<double> & inApriori) { apriori = inApriori; }

	/// matrix modifiers
	void erase(const uint index);
	void erase(const std::vector<uint> & eraseIndices);
	void print();

	void push_back(const std::valarray<double> & inDatum,
				   uint inType,
				   const QString & inFileName);

	 /// apply centering & variancing & push_back, for successiveLearning
	void addItem(const std::valarray<double> & inDatum,
				 uint inType,
				 const QString & inFileName);

	void removeFirstItemOfType(uint type);
	void pop_back();
	void pop_front();
//	void insert(const std::valarray<double> & inDatum, uint inType, uint index); // unused
//	void push_front(const std::valarray<double> & inDatum, uint inType); // unused - insert(0)
	void resize(int rows, int cols, double val);
	void resizeRows(int newRows);
	void resizeCols(int newCols);
	void reduceSize(uint oneClass);
	void clean(uint size, const QString & filter = QString());

	/// data modifiers
	void centering();
	void variancing(double var = 10.);
	void z_transform(double var = 10.);

	/// for Classifier::peopleClassification
	void centeringSubset(const std::vector<uint> & rows);
	void variancingSubset(const std::vector<uint> & rows, double var = 10.);
	void z_transformSubset(const std::vector<uint> & rows, double var = 10.);

	ClassifierData toPca(int numOfPca = 30, double var = 80.) const;
	ClassifierData productLeft(const matrix & coeffs = matrix()) const;

private:
	uint numOfCl{};
	matrix dataMatrix{};						// biases for Net are imaginary
	std::vector<uint> types{};
	std::vector<std::vector<uint>> indices{};	// arrays of indices for each class
	std::vector<QString> fileNames{};			// used in Classifier::peopleClassification
	std::valarray<double> classCount{};			// really int but...
	std::valarray<double> apriori{};			// for some classifiers like NBC
	QString filesPath{};

	std::valarray<double> averageDatum{};	/// used in addItem
	std::valarray<double> sigmaVector{};	/// used in addItem
	double variance{1.};					/// used in addItem
};

#endif // CLASSIFIERDATA_H
