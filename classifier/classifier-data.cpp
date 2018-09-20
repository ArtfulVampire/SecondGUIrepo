#include <classifier/classifierdata.h>

#include <myLib/dataHandlers.h>
#include <myLib/general.h>
#include <myLib/signalProcessing.h>
#include <myLib/ica.h>

using namespace myOut;

/// use a map of class marker - class number
#define MAP 01

void ClassifierData::adjust()
{	
	std::set<int> typesSet{};
	for(auto in : this->types) { typesSet.emplace(in); }

#if MAP
	/// fill the map:
	/// classMarkers[ typesSet[0] ] = 0
	/// classMarkers[ typesSet[1] ] = 1 etc

	int clCounter = 0;
	for(auto in : typesSet)
	{
		this->classMarkers[in] = clCounter;
		++clCounter;
	}
	numOfCl = classMarkers.size();
#else
	numOfCl = typesSet.size();
#endif
	recountIndices();
	calculateApriori();
}

std::valarray<double> ClassifierData::getClassCount() const
{
	std::valarray<double> res(classMarkers.size());
	int co = 0;
	for(const auto & in : indices)
	{
		res[co++] = in.size();
	}
	return res;
}

void ClassifierData::calculateApriori()
{
	apriori = smLib::normalized(getClassCount());
}

void ClassifierData::recountIndices()
{
	this->indices.resize(numOfCl);
	for(auto & in : indices) { in.clear(); }

	for(uint i = 0; i < this->types.size(); ++i)
	{
#if MAP
		this->indices[ this->classMarkers[ this->types[i] ] ].push_back(i);
#else
		this->indices[this->types[i]].push_back(i);
#endif
	}
}

ClassifierData::ClassifierData(const matrix & inData, const std::vector<uint> & inTypes)
{
	if(inData.rows() != inTypes.size())
	{
		std::cout << "ClassifierData::ClassifierData: arguments of wrong size" << std::endl;
		return;
	}

	this->types = inTypes;
	this->dataMatrix = inData;
	this->fileNames.resize(this->types.size(), QString()); /// empty names

	adjust();
	this->z_transform();
}


ClassifierData::ClassifierData(const QString & inPath, const QStringList & filters)
{

	this->dataMatrix = matrix();
	this->types.clear();
	this->fileNames.clear();
	this->filesPath = inPath;


#if MAP
	/// construct types vector, dataMatrix and fileNames, then adjust
	for(const QString & fileName : myLib::makeFullFileList(inPath, filters))
	{
		auto tempArr = myLib::readFileInLine(inPath + "/" + fileName);
		dataMatrix.push_back(tempArr);
		types.push_back(myLib::getTypeOfFileName(fileName, DEFS.getFileMarks()));
		fileNames.push_back(fileName);
	}
	adjust();
#else
	this->numOfCl = DEFS.numOfClasses();
	this->indices.resize(numOfCl);

	for(const QString & fileName : myLib::makeFullFileList(inPath, filters))
	{
		auto tempArr = myLib::readFileInLine(inPath + "/" + fileName);
		this->push_back(tempArr,
						myLib::getTypeOfFileName(fileName, DEFS.getFileMarks()),
						fileName);
	}
	calculateApriori();
#endif

	this->z_transform();
}

void ClassifierData::erase(const uint index)
{
	dataMatrix.eraseRow(index);

	/// check empty classes?
#if MAP
	auto & al = indices[ classMarkers[ types[index] ] ];
#else
	auto & al = indices[ types[index] ];
#endif
	al.erase(std::find(std::begin(al), std::end(al), index));

	for(auto & ind : indices)
	{
		for(auto & inInd : ind)
		{
			if(inInd > index) --inInd;
		}
	}

	types.erase(std::begin(types) + index);
	fileNames.erase(std::begin(fileNames) + index);
}

void ClassifierData::print()
{
	std::cout << "numOfClasses = " << numOfCl << std::endl;
	std::cout << "data rows = " << dataMatrix.rows() << std::endl;
	std::cout << "types size = " << types.size() << std::endl;

#if MAP
	for(auto in : classMarkers)
	{
		std::cout << in.first << " -> " << in.second << std::endl;
	}
#endif
	std::cout << "indices sizes = ";
	for(const auto & in : indices)
	{
		std::cout << in.size() << "\t";
	}
	std::cout << std::endl;
	std::cout << "fileNames size = " << fileNames.size() << std::endl;
	std::cout << std::endl;
}

void ClassifierData::erase(const std::vector<uint> & eraseIndices)
{

	dataMatrix.eraseRows(eraseIndices);
	smLib::eraseItems(fileNames, eraseIndices);
	smLib::eraseItems(types, eraseIndices);

	/// indices - just recount from scratch
	recountIndices();
}

/// is it type from zero or random marker

int ClassifierData::getClassCount(uint i) const
{
#if MAP
	return indices[ classMarkers.at(i) ].size();
#else
	return indices[i].size();
#endif
}

/// is it type from zero or random marker
void ClassifierData::removeFirstItemOfType(uint type)
{
	/// check if such type exists ???

#if MAP
	/////////////////////////// WARNING!
	uint num = *std::min_element(std::begin(indices[ classMarkers[type] ]),
								 std::end(indices[ classMarkers[type] ]));

#else
	uint num = *std::min_element(std::begin(indices[type]),
								 std::end(indices[type]));
#endif
	this->erase(num);
}

void ClassifierData::reduceSize(uint oneClass)
{
	std::vector<uint> eraseIndices{};
	std::valarray<double> localCount = getClassCount();
	for(int i = 0; i < dataMatrix.rows(); ++i)
	{
#if MAP
		int typeIndex = classMarkers[ types[i] ];
#else
		int typeIndex = types[i];
#endif

		if(localCount[ typeIndex ] > oneClass)
		{
			eraseIndices.push_back(i);
			localCount[ typeIndex ] -= 1.;
		}
	}
	this->erase(eraseIndices);
}

/// retain last 'size' vectors of each class, whose names contain 'filter'
/// is used once in Net::customF
void ClassifierData::clean(uint size, const QString & filter)
{
	std::vector<uint> eraseIndices{};
	std::vector<uint> tmpClCo(this->getNumOfCl());
	std::vector<uint> tmpTypes{};
	std::vector<uint> inds{};

	for(uint i = 0; i < fileNames.size(); ++i)
	{
#if MAP
		int typeIndex = classMarkers[ types[i] ];
#else
		int typeIndex = types[i];
#endif
		if(fileNames[i].contains(filter))
		{
			++tmpClCo[typeIndex];
			inds.push_back(i);
			tmpTypes.push_back(typeIndex);
		}
	}


	for(uint i = 0; i < tmpTypes.size(); ++i)
	{
		if(tmpClCo[tmpTypes[i]] > size)
		{
			eraseIndices.push_back(inds[i]);
			--tmpClCo[tmpTypes[i]];
		}
	}

	this->erase(eraseIndices);
}


void ClassifierData::addItem(const std::valarray<double> & inDatum,
							 uint inType,
							 const QString & inFileName)
{
	std::valarray<double> newDatum;
	if((averageDatum.size() == inDatum.size()) && (sigmaVector.size() == inDatum.size()))
	{
		newDatum = (inDatum - averageDatum) / (sigmaVector * variance);
	}
	else
	{
		newDatum = inDatum;
	}
	this->push_back(newDatum, inType, inFileName);
}

/// std::optional<uint> inType for?
void ClassifierData::push_back(const std::valarray<double> & inDatum,
							   uint inType,
							   const QString & inFileName)
{
	dataMatrix.push_back(inDatum);
	types.push_back(inType);
	fileNames.push_back(inFileName);

	/// process if it is a new class
#if MAP
	if(classMarkers.count(inType) == 0) /// this is a new type
	{
		adjust(); /// can rearrange the sequence of classes
	}
	else
	{
		indices[ classMarkers[inType] ].push_back(dataMatrix.rows() - 1); /// index of a new
	}
#else
	if(inType >= indices.size()) { indices.resize(inType + 1); }
	indices[inType].push_back(dataMatrix.rows() - 1);
#endif
}

void ClassifierData::pop_back()
{
#if 0
	/// costs more, but works
	this->erase(dataMatrix.rows() - 1);
#else
	const int index = dataMatrix.rows() - 1;

#if MAP
	const int type = classMarkers[ types[index] ];
#else
	const int type = types[index];
#endif

	/// inversed push_back
	dataMatrix.pop_back();
	types.pop_back();
	fileNames.pop_back();

	indices[type].pop_back();
#endif
}

void ClassifierData::centeringSubset(const std::vector<uint> & rows)
{
	std::valarray<double> avRow(dataMatrix.cols());
	for(auto in : rows)
	{
		avRow += dataMatrix[in];
	}
	avRow /= rows.size();

	for(auto in : rows)
	{
		dataMatrix[in] -= avRow;
	}
}

void ClassifierData::variancingSubset(const std::vector<uint> & rows, double var)
{
	this->variance = var;
	matrix tmp = dataMatrix.subRows(rows);
	tmp.transpose();

	std::valarray<double> sgmVec(tmp.rows());
	for(int i = 0; i < tmp.rows(); ++i)
	{
		sgmVec[i] = smLib::sigma(tmp[i]);
		if(sgmVec[i] != 0.)
		{
			tmp[i] /= sgmVec[i] * var;
		}
	}
	tmp.transpose();

	for(uint i = 0; i < rows.size(); ++i)
	{
		dataMatrix[rows[i]] = tmp[i];
	}
}

void ClassifierData::z_transformSubset(const std::vector<uint> & rows, double var)
{
	this->centeringSubset(rows);
	this->variancingSubset(rows, var);
}

void ClassifierData::centering()
{
	averageDatum = dataMatrix.averageRow();
	for(int i = 0; i < dataMatrix.rows(); ++i)
	{
		dataMatrix[i] -= averageDatum;
	}
}

void ClassifierData::variancing(double var)
{
	this->variance = var;
	matrix tmp = dataMatrix;
	tmp.transpose();

	sigmaVector.resize(tmp.rows());
	for(int i = 0; i < tmp.rows(); ++i)
	{
		sigmaVector[i] = smLib::sigma(tmp[i]);
		if(sigmaVector[i] != 0.)
		{
			tmp[i] /= sigmaVector[i] * var;
		}
	}
	tmp.transpose();

	dataMatrix = tmp;
}

void ClassifierData::z_transform(double var)
{
	this->centering();
	this->variancing(var);
}

ClassifierData ClassifierData::toPca(int numOfPca, double var) const
{
	matrix centeredMatrix;
	centeredMatrix = matrix::transposed(dataMatrix);

	/// is useless if centering is on
	for(int i = 0; i < centeredMatrix.rows(); ++i)
	{
		centeredMatrix[i] -= smLib::mean(centeredMatrix[i]);
	}

	double trace = 0.;
	for(int i = 0; i < centeredMatrix.rows(); ++i)
	{
		trace += smLib::variance(centeredMatrix[i]);
	}

	const double eigenValuesTreshold = std::pow(10., -8.);

	/// auto [eigenVectors, eigenValues] =
	matrix eigenVectors;
	std::valarray<double> eigenValues;
	auto a = myLib::eigenValuesSVD(centeredMatrix,
								   numOfPca,
								   eigenValuesTreshold);
	eigenVectors = a.first;
	eigenValues = a.second;

	centeredMatrix.transpose();
	matrix pcaMatrix = centeredMatrix * eigenVectors;

	ClassifierData ret(*this);
	ret.dataMatrix = pcaMatrix;
	return ret;
}



ClassifierData ClassifierData::productLeft(const matrix & coeffs) const
{
	if(coeffs.cols() != this->dataMatrix.rows())
	{
		std::cout << "ClassifierData::productLeft: arguments of wrong size" << std::endl;
		return {};
	}

	ClassifierData ret(*this);
	ret.dataMatrix = coeffs * this->dataMatrix;
	return ret;
}
