#include <classifier/classifierdata.h>

#include <myLib/dataHandlers.h>
#include <myLib/general.h>
#include <myLib/signalProcessing.h>

using namespace myOut;

void ClassifierData::adjust()
{
	/// can be generilized via std::set and set.size()
	numOfCl = *std::max_element(std::begin(types), std::end(types)) + 1;

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

	fileNames.resize(this->types.size(), QString());

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

	this->z_transform();
}


ClassifierData::ClassifierData(const QString & inPath, const QStringList & filters)
{
	QStringList lst = myLib::makeFullFileList(inPath, filters);

	this->numOfCl = DEFS.numOfClasses();
	this->dataMatrix = matrix();
	this->types.clear();
	this->fileNames.clear();
	this->filesPath = inPath;
	this->indices.resize(numOfCl);
	this->classCount.resize(this->numOfCl, 0.);

	for(const QString & fileName : lst)
	{
		auto tempArr = myLib::readFileInLine(inPath + "/" + fileName);
		this->push_back(tempArr,
						myLib::getTypeOfFileName(fileName, DEFS.getFileMarks()),
						fileName);
	}
	this->apriori = classCount;

	this->z_transform();
}

void ClassifierData::erase(const uint index)
{
	dataMatrix.eraseRow(index);
	classCount[ types[index] ] -= 1.;

	/// check empty classes?
	auto & al = indices[types[index]];
	al.erase(std::find(std::begin(al), std::end(al), index));

	for(auto & ind : indices)
	{
		for(auto & inInd : ind)
		{
			if(inInd > index) --inInd;
		}
	}

	types.erase(std::begin(types) + index);
	fileNames.erase(fileNames.begin() + index);
}

void ClassifierData::print()
{
	std::cout << "numOfClasses = " << numOfCl << std::endl;
	std::cout << "data rows = " << dataMatrix.rows() << std::endl;
	std::cout << "types size = " << types.size() << std::endl;
	std::cout << "classCount = " << classCount << std::endl;
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

	for(int index : eraseIndices)
	{
		classCount[ types[index] ] -= 1.;
	}
	smLib::eraseItems(types, eraseIndices);

	/// indices - just recount
	indices.resize(numOfCl);
	for(int i = 0; i < numOfCl; ++i)
	{
		indices[i].clear();
	}
	for(uint i = 0; i < types.size(); ++i)
	{
		indices[ types[i] ].push_back(i);
	}
}

void ClassifierData::removeFirstItemOfType(uint type)
{
	uint num = *std::min_element(std::begin(indices[type]),
								 std::end(indices[type]));
	this->erase(num);

}

void ClassifierData::reduceSize(uint oneClass)
{
	std::vector<uint> eraseIndices{};
	std::valarray<double> localCount = classCount;
	for(uint i = 0; i < dataMatrix.rows(); ++i)
	{
		if(localCount[ types[i] ] > oneClass)
		{
			eraseIndices.push_back(i);
			localCount[ types[i] ] -= 1.;
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

void ClassifierData::push_back(const std::valarray<double> & inDatum,
							   uint inType,
							   const QString & inFileName)
{
	if(inType >= indices.size()) indices.resize(inType + 1);
	if(inType >= classCount.size()) smLib::valarResize(classCount, inType + 1);

	indices[inType].push_back(dataMatrix.rows()); // index of a new
	dataMatrix.push_back(inDatum);
	classCount[inType] += 1.;
	types.push_back(inType);
	fileNames.push_back(inFileName);
}

void ClassifierData::pop_back()
{
#if 0
	this->erase(dataMatrix.rows() - 1);
#else
	const int index = dataMatrix.rows() - 1;
	const int type = types[index];

	/// inversed push_back
	indices[type].pop_back();
	dataMatrix.pop_back();
	classCount[ type ] -= 1.;
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
	for(uint i = 0; i < tmp.rows(); ++i)
	{
		sgmVec[i] = smLib::sigma(tmp[i]);
		if(sgmVec[i] != 0.)
		{
			tmp[i] /= sgmVec[i] * var;
		}
	}
	tmp.transpose();

	for(int i = 0; i < rows.size(); ++i)
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
