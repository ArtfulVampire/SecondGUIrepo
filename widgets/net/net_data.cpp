#include <widgets/net.h>
#include "ui_net.h"

#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/output.h>
#include <myLib/ica.h>

using namespace myOut;

void Net::loadDataUCI(const QString & setName)
{
	matrix uciData{};
	std::vector<uint> uciTypes{};
	myLib::readUCIdataSet(setName, uciData, uciTypes);
	myClassifierData = ClassifierData(uciData, uciTypes);
}

// like readPaFile from library.cpp
void Net::loadData(const QString & spectraPath,
				   const QStringList & filters)
{
	myClassifierData = ClassifierData(spectraPath, filters);
}

void Net::loadDataXenia(const QString & filesPath, const QString & type)
{
	int num = QString(type[ type.indexOf("var") - 1]).toInt();

	matrix xeniaData = myLib::readMatrixFileRaw(filesPath + "/" + type + ".txt");
	std::valarray<double> xeniaTypesTemp = myLib::readFileInLineRaw(filesPath
																	+ "/Groups"
																	+ "_" + nm(num) + ".txt");
	std::vector<uint> xeniaTypes{};
	xeniaTypesTemp -= 1;
	xeniaTypes.resize(xeniaTypesTemp.size());
	std::copy(std::begin(xeniaTypesTemp), std::end(xeniaTypesTemp), std::begin(xeniaTypes));

	myClassifierData = ClassifierData(xeniaData, xeniaTypes);
}


void Net::pca()
{
	QTime myTime;
	myTime.start();
	const matrix & dataMatrix = myModel->getClassifierData()->getData();
	const std::vector<QString> & fileNames = myModel->getClassifierData()->getFileNames();

	const int NumberOfVectors = dataMatrix.rows();
	const int NetLength = dataMatrix.cols();

	matrix centeredMatrix;
	centeredMatrix = matrix::transposed(dataMatrix);
	// now rows = spectral points/features, cols - vectors

	// count covariations
	// centered matrix
	for(int i = 0; i < NetLength; ++i)
	{
		// should be already zero because if loadData centering is on
		centeredMatrix[i] -= smLib::mean(centeredMatrix[i]);
	}

	// covariation between different spectra-bins
	double trace = 0.;
	for(int i = 0; i < NetLength; ++i)
	{
		trace += smLib::variance(centeredMatrix[i]);
	}
	std::cout << "trace covMatrix = " << trace << std::endl;

	// count eigenvalue decomposition

	const double eigenValuesTreshold = pow(10., -8.);
	const int numOfPc = this->ui->pcaNumberSpinBox->value();

	/// auto [eigenVectors, eigenValues] =
	matrix eigenVectors;
	std::valarray<double> eigenValues;
	auto a = myLib::eigenValuesSVD(centeredMatrix,
								   numOfPc,
								   eigenValuesTreshold);
	eigenVectors = a.first;
	eigenValues = a.second;


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
	helpString = DEFS.dirPath()
				 + "/Help"
				 + "/ica"
				 + "/" + DEFS.getExpName() + "_pcaMat.txt";
	myLib::writeMatrixFile(helpString, eigenVectors);


	/// pewpewpewpewpewpewpewpewpewpewpewpewpewpewpewpew
//	dataMatrix = pcaMatrix;


	for(int j = 0; j < NumberOfVectors; ++j)
	{
		helpString = DEFS.dirPath()
					 + "/SpectraSmooth"
					 + "/PCA"
					 + "/" + fileNames[j];
		myLib::writeFileInLine(helpString,
						pcaMatrix[j]);
	}

	eigenVectors.transpose();
	eigenVectors.resizeRows(3); /// ???
	helpString = DEFS.dirPath()
				 + "/Help"
				 + "/ica"
				 + "/" + DEFS.getExpName() + "_pcas.jpg";
	myLib::drawTemplate(helpString);
	myLib::drawArrays(helpString,
			   eigenVectors,
			   true);

	std::cout << "pca: time elapsed = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}
