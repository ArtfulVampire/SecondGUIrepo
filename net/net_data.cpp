#include "net.h"

#include "ui_net.h"

void Net::pushBackDatum(const lineType & inDatum,
					  const uint & inType,
					  const QString & inFileName)
{
#if !OLD_DATA
	myClassifierData.push_back(inDatum, inType, inFileName);
#else
	dataMatrix.push_back(inDatum);
	classCount[inType] += 1.;
	types.push_back(inType);
	fileNames.push_back(inFileName);
#endif
}

void Net::popBackDatum()
{
#if !OLD_DATA
	myClassifierData.pop_back();
#else
	dataMatrix.pop_back();
	classCount[types.back()] -= 1.;
	types.pop_back();
	fileNames.pop_back();
#endif
}

void Net::eraseDatum(const uint & index)
{
#if !OLD_DATA
	myClassifierData.erase(index);
#else
	dataMatrix.eraseRow(index);
	classCount[ types[index] ] -= 1.;
	types.erase(types.begin() + index);
	fileNames.erase(fileNames.begin() + index);
#endif
}

void Net::eraseData(const std::vector<uint> & indices)
{
#if !OLD_DATA
	myClassifierData.erase(indices);
#else
	dataMatrix.eraseRows(indices);
	eraseItems(fileNames, indices);
	for(int index : indices)
	{
		classCount[ types[index] ] -= 1.;
	}
	eraseItems(types, indices);
#endif
}


void Net::normalizeDataMatrix()
{
#if !OLD_DATA
	myClassifierData.z_transform();
#else
#if 1
	if(ui->centerCheckBox->isChecked())
	{
		averageDatum = dataMatrix.averageRow();
		for(uint i = 0; i < dataMatrix.rows(); ++i)
		{
			dataMatrix[i] -= averageDatum;
		}
	}
	else
	{
		averageDatum = std::valarray<double> (0., dataMatrix.cols());
	}


	if(ui->varianceCheckBox->isChecked())
	{

		dataMatrix.transpose();
		sigmaVector.resize(dataMatrix.rows());
		for(uint i = 0; i < dataMatrix.rows(); ++i)
		{
			sigmaVector[i] = sigma(dataMatrix[i]);
			if(sigmaVector[i] != 0.)
			{
				// to equal variance, 10 for reals, 5 winds
				dataMatrix[i] /= sigmaVector[i] * loadDataNorm;
			}
		}
		dataMatrix.transpose();
	}
	else
	{
		sigmaVector = std::valarray<double> (1., dataMatrix.cols());
	}
#endif
#if 0
	dataMatrix.transpose();
	for(int i = 0; i < dataMatrix.rows(); ++i)
	{
		const double a = dataMatrix[i].max();
		const double b = dataMatrix[i].min();
		dataMatrix[i] -= (a + b) / 2.;
		dataMatrix[i] /= (a - b);
	}
	dataMatrix.transpose();

#endif
#endif /// old_data
}

void Net::applyPCA(const QString & pcaMatFilePath)
{
	matrix pca{};
	myLib::readMatrixFile(pcaMatFilePath, pca);
#if !OLD_DATA
	myClassifierData = myClassifierData.productLeft(pca);
#else
	dataMatrix *= pca;
#endif
}
#if OLD_DATA
void Net::resizeData(uint newCols)
{
	if(newCols >= dataMatrix.cols()) return;
	dataMatrix.resizeCols(newCols);
}
#endif

void Net::loadDataUCI(const QString & setName)
{
	matrix uciData{};
	std::vector<uint> uciTypes{};
	myLib::readUCIdataSet(setName, uciData, uciTypes);
	/// set numOfClasses !
	loadData(uciData, uciTypes);
}

void Net::loadData(const matrix & inMat,
				   const std::vector<uint> & inTypes)
{
#if !OLD_DATA
	/// new
	myClassifierData = ClassifierData(inMat, inTypes);
	return;
#else

	/// old
	dataMatrix = matrix();
	dataMatrix = inMat;
	types = inTypes;
	classCount.resize(def::numOfClasses());
	for(int typ : types)
	{
		classCount[typ] += 1.;
	}
	normalizeDataMatrix();
	filesPath.clear();
#endif
}

// like readPaFile from library.cpp
void Net::loadData(const QString & spectraPath,
				   const QStringList & filters,
				   double rdcCoeff)
{
#if !OLD_DATA
	/// new
//	std::cout << "spectraPath = " << spectraPath.toStdString() << std::endl;
	myClassifierData = ClassifierData(spectraPath, filters);
//	std::cout << "subset size = " << myClassifierData.getData().rows() << std::endl;
	return;
#else

	/// old
	std::vector<QStringList> leest;
	makeFileLists(spectraPath, leest, filters);

	dataMatrix = matrix();
	classCount.resize(def::numOfClasses());
	types.clear();
	fileNames.clear();
	filesPath = spectraPath;

	lineType tempArr;
//    cout << spectraPath << endl;
	for(uint i = 0; i < leest.size(); ++i)
	{
		classCount[i] = 0.;
		for(const QString & fileName : leest[i])
		{
			readFileInLine(spectraPath + slash + fileName,
						   tempArr);
			pushBackDatum(tempArr, i, fileName);
		}
	}
	if(rdcCoeff != 1.)
	{
		dataMatrix /= rdcCoeff;
	}
	normalizeDataMatrix();
#endif
}

void Net::pca()
{
	QTime myTime;
	myTime.start();
	const matrix & dataMatrix = myClassifier->getClassifierData()->getData();
	const std::vector<QString> & fileNames = myClassifier->getClassifierData()->getFileNames();

	const int NumberOfVectors = dataMatrix.rows();
	const int NetLength = dataMatrix.cols();

	matrix centeredMatrix;
	centeredMatrix = matrix::transpose(dataMatrix);
	// now rows = spectral points/features, cols - vectors

	// count covariations
	// centered matrix
	for(int i = 0; i < NetLength; ++i)
	{
		// should be already zero because if loadData centering is on
		centeredMatrix[i] -= smallLib::mean(centeredMatrix[i]);
	}

	//covariation between different spectra-bins
	double trace = 0.;
	for(int i = 0; i < NetLength; ++i)
	{
		trace += smallLib::variance(centeredMatrix[i]);
	}
	std::cout << "trace covMatrix = " << trace << std::endl;

	// count eigenvalue decomposition
	matrix eigenVectors;
	lineType eigenValues;
	const double eigenValuesTreshold = pow(10., -8.);
	const int numOfPc = this->ui->pcaNumberSpinBox->value();

	myLib::svd(centeredMatrix,
			   eigenVectors,
			   eigenValues,
			   centeredMatrix.rows(),
			   eigenValuesTreshold,
			   numOfPc);


	double sum1 = 0.;
	for(int k = 0; k < numOfPc; ++k)
	{
		sum1 += eigenValues[k];
	}
	std::cout << "Part of dispersion explained = " << sum1 * 100./double(trace) << " %" << std::endl;
	std::cout << "Number of Components = " << numOfPc << std::endl;

	/// pcaMatrix
	centeredMatrix.transpose();
	matrix pcaMatrix(NumberOfVectors, numOfPc);
	pcaMatrix = centeredMatrix * eigenVectors;

	/// save pca matrix
	QString helpString;
	helpString = def::dir->absolutePath()
				 + myLib::slash + "Help"
				 + myLib::slash + "ica"
				 + myLib::slash + def::ExpName + "_pcaMat.txt";
	myLib::writeMatrixFile(helpString, eigenVectors);


	/// pewpewpewpewpewpewpewpewpewpewpewpewpewpewpewpew
//	dataMatrix = pcaMatrix;


	for(int j = 0; j < NumberOfVectors; ++j)
	{
		helpString = def::dir->absolutePath()
					 + myLib::slash + "SpectraSmooth"
					 + myLib::slash + "PCA"
					 + myLib::slash + fileNames[j];
		myLib::writeFileInLine(helpString,
						pcaMatrix[j]);
	}

	eigenVectors.transpose();
	eigenVectors.resizeRows(3); /// ???
	helpString = def::dir->absolutePath()
				 + myLib::slash + "Help"
				 + myLib::slash + "ica"
				 + myLib::slash + def::ExpName + "_pcas.jpg";
	myLib::drawTemplate(helpString);
	myLib::drawArrays(helpString,
			   eigenVectors,
			   true);

	std::cout << "pca: time elapsed = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}
