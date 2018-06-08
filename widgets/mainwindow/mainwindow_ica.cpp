#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/signalProcessing.h>
#include <myLib/output.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

void MainWindow::ICA() // fastICA
{
	// we have data[ns][ndr*nr], ns, ndr, nr
	// at first - whiten signals using eigen linear superposition to get E as covMatrix
	// then count matrixW

	// data = A * comps, comps = W * data

	// count components = matrixW*data and write to FileName_ICA.edf
	// count inverse matrixA = matrixW^-1 and draw maps of components
	// write automatization for classification different sets of components, find best set, explain

    QTime wholeTime;
    wholeTime.start();

    QTime myTime;
    myTime.start();
	std::cout << "Ica started: " << std::endl;

    readData();

	const uint ns = ui->numOfIcSpinBox->value(); /// usually 19
    const int dataLength = globalEdf.getDataLen();

	const double eigenValuesTreshold = std::pow(10., - ui->svdDoubleSpinBox->value());
	const double vectorWTreshold = std::pow(10., - ui->vectwDoubleSpinBox->value());
	const QString pathForAuxFiles = DEFS.dirPath() + "/Help/ica";

#define NEW_9_3_17 1

	matrix centeredData = globalEdf.getData();

	/// remember all the rest channels (eog, veget, mark, etc)
	matrix resMatBackup = centeredData.subRows(
							  smLib::range<std::vector<uint>>(ns,
															  centeredData.rows()));
	centeredData.resizeRows(ns);
	centeredData.centerRows(ns);


    const QString eigMatPath = pathForAuxFiles
							   + "/" + globalEdf.getExpName()
							   + "_eigenMatrix.txt";
    const QString eigValPath = pathForAuxFiles
							   + "/" + globalEdf.getExpName()
							   + "_eigenValues.txt";
	// count eigenvalue decomposition
	matrix eigenVectors;
	std::valarray<double> eigenValues;

    /// careful !
    if(!QFile::exists(eigMatPath) &&
       !QFile::exists(eigValPath))
    {
		/// complicated calculations
		/// auto [eigenVectors, eigenValues] =
		auto a = myLib::svd(centeredData,
							ns,
							eigenValuesTreshold);
		eigenVectors = a.first;
		eigenValues = a.second;

        // write eigenVectors
		myLib::writeMatrixFile(eigMatPath, eigenVectors);

        // write eigenValues
		myLib::writeFileInLine(eigValPath, eigenValues);
    }
    else /// read
    {
		// read eigenVectors
		eigenVectors = myLib::readMatrixFile(eigMatPath);

		// read eigenValues
		eigenValues = myLib::readFileInLine(eigValPath);
    }

	std::cout << "ICA: svd read = " << myTime.elapsed() / 1000. << " sec" << std::endl;
    myTime.restart();

	/// write whiteningData to components
	/// whitenedData = Eig * D^-0.5 * Eig^t * centeredData
	matrix components = eigenVectors
						  * matrix(std::pow(eigenValues, -0.5))
						  * !eigenVectors
						  * centeredData;

	matrix rotation = myLib::countVectorW(components,
										  ns,
										  dataLength,
										  vectorWTreshold);

	std::cout << "ICA: rotation ready = " << myTime.elapsed() / 1000. << " sec" << std::endl;
	myTime.restart();

	/// components = rotation * whitenedData
	/// components = W * centeredData
	/// W = rotation * Eig * D^-0.5 * Eig^t
	components = rotation * components;

	/// A * components = centeredData
	/// A = inverted( W )
	/// A = Eig * D^0.5 * Eig^t * rotation^t
	matrix matrixA = eigenVectors
					 * matrix(std::pow(eigenValues, +0.5))
					 * !eigenVectors
					 * !rotation;


	/// move to a function

    // norm components to 1-length of mapvector, order by dispersion
    for(uint i = 0; i < ns; ++i) // for each component
    {
		double nrm = smLib::norma(matrixA.getCol(i));

		for(auto & row : matrixA)
		{
			row[i] /= nrm;
		}
		components[i] *= nrm;
    }


    // ordering components by dispersion
    std::vector<std::pair <double, int>> colsNorms; // dispersion, numberOfComponent
    double sumSquares = 0.; // sum of all dispersions
    std::vector<double> explainedVariance;

    for(uint i = 0; i < ns; ++i)
    {
		double var = smLib::variance(components[i]);
		sumSquares += var;
		colsNorms.push_back(std::make_pair(var, i));
    }

	std::sort(std::begin(colsNorms),
			  std::end(colsNorms),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {
        return i.first > j.first;
    });

    for(uint i = 0; i < ns - 1; ++i) // dont move the last
    {
		// swap matrixA cols
		matrixA.swapCols(i, colsNorms[i].second);

		// swap components
		components.swapRows(i, colsNorms[i].second);

        // swap i and colsNorms[i].second values in colsNorms
		auto it1 = std::find_if(std::begin(colsNorms),
								std::end(colsNorms),
                                [i](std::pair <double, int> in)
		{ return in.second == i; });
		auto it2 = std::find_if(std::begin(colsNorms),
								std::end(colsNorms),
                                [colsNorms, i](std::pair <double, int> in)
		{ return in.second == colsNorms[i].second; });

		std::swap((*it1).second, (*it2).second);
    }






    for(uint i = 0; i < ns; ++i)
    {
        explainedVariance.push_back(colsNorms[i].first / sumSquares * 100.);
		std::cout << "comp = " << i+1 << "\t";
		std::cout << "explVar = " << smLib::doubleRound(explainedVariance[i], 2) << std::endl;
    }

#if 01
	{
		QString helpString = (pathForAuxFiles
							  + "/" + globalEdf.getExpName() + "_explainedVariance.txt");
		myLib::writeFileInLine(helpString, explainedVariance);
	}
#endif






#if 1
	/// test centeredMatrix == A * components;
	int counter = 0;
	matrix diff = matrixA * components - centeredData;
	for(int i = 0; i < diff.rows(); ++i)
	{
		for(int j = 0; j < diff.cols(); ++j)
		{
			if( std::abs(diff[i][j]) / centeredData[i][j] > 0.05 && centeredData[i][j] > 0.5)
			{
				++counter;
#if 0
				std::cout << "after norm" << "\t";
				std::cout << i << "\t" << j << "\t";
				std::cout << "err = " << doubleRound(sum1, 3) << "\t";
				std::cout << "init value = " << doubleRound(centeredMatrix[i][j], 4) << std::endl;
#endif
			}
		}
	}
	std::cout << "num of errors = " << counter << std::endl;
#endif







	// now should draw amplitude maps OR write to file
#if 01
	{
		/// write maps to file
		QString helpString = pathForAuxFiles
							 + "/" + globalEdf.getExpName() + "_maps.txt";
		myLib::writeMatrixFile(helpString, matrixA);
		/// draw maps
		myLib::drawMapsICA(helpString,
						   DEFS.dirPath() + "/Help/ica",
						   globalEdf.getExpName());
	}
#endif

	/// save ICA file as a new edf
    std::vector<int> chanList(ns);
	std::iota(std::begin(chanList), std::end(chanList), 0);
    chanList.push_back(globalEdf.getMarkChan());

//	components.vertCat(resMatBackup); /// comment to drop all non-EEG channels
	components.push_back(globalEdf.getMarkArr());

#if 01
	{
		/// write new edf
		QString helpString = DEFS.dirPath()
							 + "/" + globalEdf.getExpName() + "_ica.edf";
		globalEdf.writeOtherData(components, helpString, chanList);
	}
#endif
	std::cout << "ICA ended. time = " << wholeTime.elapsed() / 1000. << " sec" << std::endl;
}
