#include "classifierdata.h"
#include <iostream>
#include "library.h"

//using namespace myLib;

void ClassifierData::adjust()
{
	numOfCl = *std::max_element(std::begin(this->types), std::end(this->types)) + 1;
	this->indices.resize(numOfCl, std::vector<uint>{});
	for(uint i = 0; i < this->types.size(); ++i)
	{
		this->indices[this->types[i]].push_back(i);
	}

	classCount.resize(numOfCl);
	for(uint i = 0; i < numOfCl; ++i)
	{
		classCount[i] = indices[i].size();
	}

	fileNames.resize(this->types.size(), "pewpew_fileName");

	apriori = classCount;
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

	adjust();
}


ClassifierData::ClassifierData(const QString & inPath, const QStringList & filters)
{
#if 0
	std::vector<QStringList> leest;
	myLib::makeFileLists(inPath, leest, filters);
	this->numOfCl = leest.size();
#else

	QStringList lst;
	myLib::makeFullFileList(inPath, lst, filters);
	this->numOfCl = def::fileMarkers.length();
#endif



	this->dataMatrix = matrix();
	this->classCount.resize(this->numOfCl);
	this->types.clear();
	fileNames.clear();
	this->filesPath = inPath;
	this->indices.resize(numOfCl);

	std::valarray<double> tempArr;

#if 0
	/// std::vector<QStringList>
	for(uint i = 0; i < leest.size(); ++i)
	{
		classCount[i] = 0.;
		for(const QString & fileName : leest[i])
		{
			myLib::readFileInLine(inPath + slash + fileName,
								  tempArr);
			this->push_back(tempArr, i, fileName);
		}
	}
#else
	classCount = 0;
	for(const QString & fileName : lst)
	{
		myLib::readFileInLine(inPath + slash + fileName,
							  tempArr);
		this->push_back(tempArr,
						myLib::getTypeOfFileName(fileName),
						fileName);
	}
#endif

	this->apriori = classCount;

	/// paskdnfajfkqnwjklerqw
//	this->dataMatrix.resizeCols(60);

	this->z_transform();

}

void ClassifierData::erase(uint index)
{
	dataMatrix.eraseRow(index);
	classCount[ types[index] ] -= 1.;

	/// indices - or just recount?
	/// check empty classes?
#if 1
	auto & al = indices[types[index]];
	al.erase(std::find(std::begin(al), std::end(al), index));
	for(auto & ind : indices)
	{
		std::for_each(std::begin(ind), std::end(ind), [index](uint & in)
		{
			if(in > index) --in;
		});
	}
#else
	indices.resize(numOfCl, std::vector<double>{});
	for(uint i = 0; i < inTypes.size(); ++i)
	{
		indices[inTypes[i]].push_back(i);
	}
#endif

	types.erase(std::begin(types) + index);
	fileNames.erase(fileNames.begin() + index);
}

void ClassifierData::erase(const std::vector<uint> & indices)
{

	dataMatrix.eraseRows(indices);
	smLib::eraseItems(fileNames, indices);

	for(int index : indices)
	{
		classCount[ types[index] ] -= 1.;
	}
	smLib::eraseItems(types, indices);

	/// indices - just recount
	/// check empty classes?
	this->indices.resize(numOfCl, std::vector<uint>{});
	for(uint i = 0; i < this->types.size(); ++i)
	{
		this->indices[this->types[i]].push_back(i);
	}
}

void ClassifierData::removeFirstItemOfType(uint type)
{
	uint num = std::distance(std::begin(types),
							 std::find(std::begin(types),
									   std::end(types),
									   type));
	this->erase(num);
}

void ClassifierData::reduceSize(uint oneClass)
{
	std::vector<uint> eraseIndices{};
	std::valarray<double> count = classCount;
	for(uint i = 0; i < dataMatrix.rows(); ++i)
	{
		if(count[ types[i] ] > oneClass)
		{
			eraseIndices.push_back(i);
			count[ types[i] ] -= 1.;
		}
	}
	this->erase(eraseIndices);
}

void ClassifierData::clean(uint size, const QString & filter)
{
	std::vector<uint> eraseIndices{};
	std::vector<uint> tmpClCo(this->getNumOfCl());
	std::vector<uint> tmpTypes;
	std::vector<uint> inds;

	for(uint i = 0; i < fileNames.size(); ++i)
	{
		if(fileNames[i].contains(filter))
		{
			++tmpClCo[types[i]];
			inds.push_back(i);
			tmpTypes.push_back(types[i]);
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
	std::valarray<double> newDatum = (inDatum - averageDatum) / (sigmaVector * variance);
//	std::valarray<double> newDatum = inDatum / 20.;
//	newDatum /= 20.; /// same as in z_transform();
	this->push_back(newDatum, inType, inFileName);
}

void ClassifierData::push_back(const std::valarray<double> & inDatum,
							   uint inType,
							   const QString & inFileName)
{
	indices[inType].push_back(dataMatrix.rows()); // index of a new

	dataMatrix.push_back(inDatum);
	classCount[inType] += 1.;
	types.push_back(inType);
	fileNames.push_back(inFileName);
}

void ClassifierData::pop_back()
{
#if 1
	this->erase(dataMatrix.rows() - 1);
#else
	const uint typ = types.back();
	auto & ind = indices[typ];
	ind.erase(std::max(std::begin(ind), std::end(ind)));

	dataMatrix.pop_back();
	classCount[typ] -= 1.;
	types.pop_back();
	fileNames.pop_back();
#endif
}


void ClassifierData::pop_front()
{
	this->erase(0);
}

void ClassifierData::resize(int rows, int cols, double val)
{
	this->dataMatrix.resize(rows, cols, val);
}

void ClassifierData::resizeRows(int newRows)
{
	this->dataMatrix.resizeRows(newRows);
}


/// aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
void ClassifierData::resizeCols(int newCols)
{

}


void ClassifierData::centering()
{
	averageDatum = dataMatrix.averageRow();
	for(uint i = 0; i < dataMatrix.rows(); ++i)
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
	for(uint i = 0; i < tmp.rows(); ++i)
	{
		sigmaVector[i] = smLib::sigma(tmp[i]);
		if(sigmaVector[i] != 0.)
		{
			// to equal variance, 10 for reals, 5 winds
			tmp[i] /= sigmaVector[i] * var;
		}
	}
	tmp.transpose();

	dataMatrix = std::move(tmp);
}

void ClassifierData::z_transform(double var)
{
//	this->dataMatrix /= 20; return;
	this->centering();
	this->variancing(var);
}

ClassifierData ClassifierData::toPca(int numOfPca, double var) const
{
	matrix centeredMatrix;
	centeredMatrix = matrix::transpose(dataMatrix);

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

	matrix eigenVectors;
	std::valarray<double> eigenValues;
	const double eigenValuesTreshold = pow(10., -8.);

	myLib::svd(centeredMatrix,
			   eigenVectors,
			   eigenValues,
			   centeredMatrix.rows(),
			   eigenValuesTreshold,
			   numOfPca);

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
