#ifndef CLASSIFIERDATA_H
#define CLASSIFIERDATA_H

#include <iostream>
#include <valarray>
#include <vector>
#include <map>

#include <other/matrix.h>

class ClassifierData
{
public:
	ClassifierData() {}
	~ClassifierData() {}

	ClassifierData(const QString & inPath, const QStringList & filters = QStringList());
	ClassifierData(const matrix & inData, const std::vector<uint> & inTypes);

	ClassifierData(const ClassifierData &) = default;
	ClassifierData(ClassifierData &&) = default;
	ClassifierData & operator=(const ClassifierData &) = default;

	void adjust();	/// sets proper numCl, indices, classCount
	void recountIndices();
	void calculateApriori();

	/// gets
	const matrix & getData() const								{ return dataMatrix; }
	bool isEmpty() const										{ return dataMatrix.isEmpty(); }
	int size() const											{ return dataMatrix.rows(); }
	uint getNumOfCl() const										{ return numOfCl; }
	const std::vector<uint> & getTypes() const					{ return types; }
	uint getTypes(int i) const									{ return types[i]; }
	const std::vector<std::vector<uint>> & getIndices() const	{ return indices; }
	const std::vector<QString> & getFileNames() const			{ return fileNames; }
	const QString & getFileNames(int i) const					{ return fileNames[i]; }
	const std::valarray<double> & getApriori() const			{ return apriori; }
	const QString & getFilesPath() const						{ return filesPath; }
	int getSpLength() const										{ return spLength; }
	int getClassCount(uint i) const;

	/// sets
	void setApriori(const std::valarray<double> & inApriori);

	/// matrix modifiers
	void erase(const uint index);
	void erase(const std::vector<uint> & eraseIndices);
	void print() const;
	void zeroChans(const std::vector<int> & chans);

	void push_back(const std::valarray<double> & inDatum,
				   uint inType,
				   const QString & inFileName);

	 /// apply centering, variancing and push_back, for Net::successiveLearning
	void addItem(const std::valarray<double> & inDatum,
				 uint inType,
				 const QString & inFileName);

	void removeFirstItemOfType(uint type);
	void pop_back();
	void pop_front()		{ this->erase(0); }
#if 0
	/// unused funcs
	void insert(const std::valarray<double> & inDatum, uint inType, uint index);
	void push_front(const std::valarray<double> & inDatum, uint inType); /// = insert(0)
	void resize(int rows, int cols, double val)	{ this->dataMatrix.resize(rows, cols, val); }
	void resizeRows(int newRows)				{ this->dataMatrix.resizeRows(newRows); }
	void resizeCols(int newCols)				{ this->dataMatrix.resizeCols(newCols); }
#endif
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
	std::valarray<double> getClassCount() const;

private:
	std::map<uint, uint> classMarkers{};		/// key - input marker, value - number from zero
	uint numOfCl{};								/// = classMarkers.size(), to deprecate
	matrix dataMatrix{};						/// the data
	matrix bcMatrix{};							/// backup data for zeroing
	std::vector<uint> types{};					/// vector of object types (may be any uints)
	std::vector<QString> fileNames{};			/// used in Classifier::peopleClassification
	int spLength{};								/// for 128 chans zeroing

	std::vector<std::vector<uint>> indices{};	/// arrays of indices for each class used WHERE ???
	std::valarray<double> apriori{};			/// for some classifiers like NBC
	QString filesPath{};

	std::valarray<double> averageDatum{};	/// used in addItem
	std::valarray<double> sigmaVector{};	/// used in addItem
	double variance{1.};					/// used in addItem
};

#endif /// CLASSIFIERDATA_H
