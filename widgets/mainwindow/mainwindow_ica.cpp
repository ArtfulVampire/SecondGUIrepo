#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/signalProcessing.h>
#include <myLib/output.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

void MainWindow::ICA() //fastICA
{

#define MATRICES_ICA_0 1
#define MATRICES_ICA_1 1
#define MATRICES_ICA_2 1
#define MATRICES_ICA_3 1
#define MATRICES_ICA_4 1
#define MATRICES_ICA_5 1
#define MATRICES_ICA_6 1

    //we have data[ns][ndr*nr], ns, ndr, nr
    //at first - whiten signals using eigen linear superposition to get E as covMatrix
    //then count matrixW

    //data = A * comps, comps = W * data

    //count components = matrixW*data and write to def::ExpName_ICA.edf
    //count inverse matrixA = matrixW^-1 and draw maps of components
    //write automatization for classification different sets of components, find best set, explain

    QTime wholeTime;
    wholeTime.start();

    QTime myTime;
    myTime.start();

    QString helpString;
    int counter;

    helpString = def::dir->absolutePath()
				 + "/" + def::ExpName + ".edf";
	std::cout << "Ica started: " << helpString << std::endl;
    readData();

    const uint ns = ui->numOfIcSpinBox->value(); //generality. Bind to reduceChannelsLineEdit?
    const int dataLength = globalEdf.getDataLen();


    const double eigenValuesTreshold = pow(10., - ui->svdDoubleSpinBox->value());
    const double vectorWTreshold = pow(10., - ui->vectwDoubleSpinBox->value());
    const QString pathForAuxFiles = def::dir->absolutePath()
									+ "/Help/ica";

#define NEW_9_3_17 1

	matrix centeredMatrix = globalEdf.getData();
	matrix resMatBackup = centeredMatrix.subRows(
							  smLib::range<std::vector<uint>>(ns,
															  centeredMatrix.rows()));
	centeredMatrix.resizeRows(ns);

	matrix components(ns, dataLength, 0.);


    // count eigenvalue decomposition
    matrix eigenVectors;
    std::valarray<double> eigenValues;

	myLib::dealWithEyes(centeredMatrix,
						ns);

    const QString eigMatPath = pathForAuxFiles
							   + "/" + def::ExpName
							   + "_eigenMatrix.txt";
    const QString eigValPath = pathForAuxFiles
							   + "/" + def::ExpName
							   + "_eigenValues.txt";
    /// careful !
    if(!QFile::exists(eigMatPath) &&
       !QFile::exists(eigValPath))
    {
		myLib::svd(centeredMatrix,
				   eigenVectors,
				   eigenValues,
				   ns,
				   eigenValuesTreshold);

        // write eigenVectors
		myLib::writeMatrixFile(eigMatPath, eigenVectors);

        // write eigenValues
		myLib::writeFileInLine(eigValPath, eigenValues);
    }
    else /// read
    {
        // write eigenVectors
		myLib::readMatrixFile(eigMatPath, eigenVectors);

        // write eigenValues
		myLib::readFileInLine(eigValPath, eigenValues);
    }

	std::cout << "ICA: svd read = " << myTime.elapsed() / 1000. << " sec" << std::endl;
    myTime.restart();


    // components = EigenValues^-0.5 * Et * data
    matrix D_minus_05(ns, ns, 0.);
    for(uint i = 0; i < ns; ++i)
    {
        D_minus_05[i][i] = 1. / sqrt(eigenValues[i]);
    }

    matrix tmpMat{};
    tmpMat = D_minus_05 * matrix::transpose(eigenVectors);

#if NEW_9_3_17
	components = tmpMat * centeredMatrix;
#else
    matrixProduct(tmpMat,
                  centeredMatrix,
                  components,
                  ns);
#endif


    matrix dataICA{};
#if NEW_9_3_17
	dataICA = eigenVectors * components;
#else
    matrixProduct(eigenVectors,
                  components,
                  dataICA,
                  ns);
#endif


    //now dataICA are uncovariated signals with variance 1
    // ICA itself!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // fastIca wiki - first function


    matrix vectorW(ns, ns);
	myLib::countVectorW(vectorW,
						dataICA,
						ns,
						dataLength,
						vectorWTreshold);
	std::cout << "ICA: vectorW ready = " << myTime.elapsed() / 1000. << " sec" << std::endl;
    myTime.restart();

//    helpString = pathForAuxFiles
//                 + "/" + def::ExpName + "_vectorW.txt";
//    writeMatrixFile(helpString,
//                     vectorW);


    //count components
#if NEW_9_3_17
	components = vectorW * dataICA;
#else
    matrixProduct(vectorW,
                  dataICA,
                  components,
                  ns);
#endif


    //count full mixing matrix A = E * D^0.5 * Et * Wt
    matrix matrixA(ns, ns, 0.);
    matrix D_05(ns, ns, 0.);
    for(uint i = 0; i < ns; ++i)
    {
        D_05[i][i] = sqrt(eigenValues[i]);
    }
#if 1
    /// test
    matrixA = eigenVectors * D_05 * matrix::transpose(eigenVectors) * matrix::transpose(vectorW);
#else

    matrixA = matrix::transpose(vectorW); // A = Wt

    matrixA = matrix::transpose(eigenVectors) * matrixA; //A = Et * Wt

    //A = D^0.5 * Et * Wt
    matrixA = D_05 * matrixA;

    matrixA = eigenVectors * matrixA;
#endif



#if 0
	/// remake with valarrays
	// norm components - to equal dispersion
	// sort by maps length

    double coeff = 1.5;
    for(int i = 0; i < ns; ++i)
    {
        sum1 = 0.;
        sum2 = 0.;

        sum2 = variance(components[i], dataLength);

        for(int j = 0; j < dataLength; ++j)
        {
            components[i][j] /= sqrt(sum2);
            components[i][j] *= coeff;
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(sum2);
            matrixA[i][k] /= coeff;
        }
    }

    //ordering components by sum of squares of the matrixA coloumn
    std::vector<std::pair <double, int>> colsNorms;
    double sumSquares = 0.;
    std::vector<double> explainedVariance;

    for(int i = 0; i < ns; ++i) // w/o markers
    {
        helpDouble = 0.;
        for(int j = 0; j < ns; ++j)
        {
            helpDouble += pow(matrixA[j][i], 2.);
        }
        sumSquares += helpDouble;
        colsNorms.push_back(std::make_pair(helpDouble, i));
    }
    std::sort(colsNorms.begin(),
              colsNorms.end(),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {return i.first > j.first;});

    for(int i = 0; i < ns; ++i)
    {
		std::cout << colsNorms[i].first << "\t" << colsNorms[i].second << std::endl;
    }

    double * tempCol = new double [ns];
    int tempIndex;
    std::vector<double> tempComp;
    for(int i = 0; i < ns - 1; ++i) // dont move the last
    {
        //swap cols and components

        for(int j = 0; j < ns; ++j) // swap j'th elements in i'th and colsNorms[i].second'th cols
        {
            tempCol[j] = matrixA[j][i];
            matrixA[j][i] = matrixA[j][ colsNorms[i].second ];
            matrixA[j][ colsNorms[i].second ] = tempCol[j];
        }

		/// swap rows i and colsNorms[i].second
        tempComp = components[i];
        components[i] = components[ colsNorms[i].second ];
        components[ colsNorms[i].second ] = tempComp;

		/// swap cols i and colsNorms[i].second
		/// find first
        auto it1 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [i](std::pair <double, int> in)
        {return in.second == i;});
		/// find second
        auto it2 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [colsNorms, i](std::pair <double, int> in)
        {return in.second == colsNorms[i].second;});

		/// swap
        tempIndex = (*it1).second;
        (*it1).second = (*it2).second;
        (*it2).second = tempIndex;
    }
    delete []tempCol;

    helpString = (def::dir->absolutePath()
										  + "/Help"
										  + "/" + def::ExpName + "_maps_after_var.txt");
    writeMatrixFile(helpString, matrixA, ns); //generality 19-ns

    for(int i = 0; i < ns; ++i)
    {
        explainedVariance.push_back(colsNorms[i].first / sumSquares * 100.);
		std::cout << "comp = " << i+1 << "\t";
		std::cout << "explVar = " << doubleRound(explainedVariance[i], 2) << std::endl;
    }
    //end componets ordering
#else
    // norm components to 1-length of mapvector, order by dispersion

    double sum1;
    for(uint i = 0; i < ns; ++i) // for each component
    {
		sum1 = smLib::norma(matrixA.getCol(i));

        for(uint k = 0; k < ns; ++k)
        {
            matrixA[k][i] /= sum1;
        }
        components[i] *= sum1;
    }

    // ordering components by dispersion
    std::vector<std::pair <double, int>> colsNorms; // dispersion, numberOfComponent
    double sumSquares = 0.; // sum of all dispersions
    std::vector<double> explainedVariance;

    for(uint i = 0; i < ns; ++i)
    {
		sum1 = smLib::variance(components[i]);
        sumSquares += sum1;
        colsNorms.push_back(std::make_pair(sum1, i));
    }

	std::sort(std::begin(colsNorms),
			  std::end(colsNorms),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {
        return i.first > j.first;
    });

    int tempIndex;
    for(uint i = 0; i < ns - 1; ++i) // dont move the last
    {
        // swap matrixA cols
#if MATRICES_ICA_4
        matrixA.swapCols(i, colsNorms[i].second);
#else
        std::vector<double> tempCol(ns);
        for(int j = 0; j < ns; ++j) // swap j'th elements in i'th and colsNorms[i].second'th cols
        {
            tempCol[j] = matrixA[j][i];
            matrixA[j][i] = matrixA[j][ colsNorms[i].second ];
            matrixA[j][ colsNorms[i].second ] = tempCol[j];
        }
#endif

        // swap components
#if MATRICES_ICA_5
        components.swapRows(i, colsNorms[i].second);
#else
        std::vector<double> tempComp;
        tempComp = components[i];
        components[i] = components[ colsNorms[i].second ];
        components[ colsNorms[i].second ] = tempComp;
#endif

        // swap i and colsNorms[i].second values in colsNorms
        auto it1 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [i](std::pair <double, int> in)
        {return in.second == i;});
        auto it2 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [colsNorms, i](std::pair <double, int> in)
        {return in.second == colsNorms[i].second;});

		std::swap((*it1).second, (*it2).second);
    }

    for(uint i = 0; i < ns; ++i)
    {
        explainedVariance.push_back(colsNorms[i].first / sumSquares * 100.);
		std::cout << "comp = " << i+1 << "\t";
		std::cout << "explVar = " << smLib::doubleRound(explainedVariance[i], 2) << std::endl;
    }
	helpString = (pathForAuxFiles
				  + "/" + def::ExpName + "_explainedVariance.txt");
	myLib::writeFileInLine(helpString, explainedVariance);
    //end componets ordering
#endif


#if 1
    // test  data = matrixA * comps;
    // again to check reordering and normalizations

    counter = 0;
    for(int j = 0; j < dataLength; ++j)
    {
        std::valarray<double> currCol = components.getCol(j, ns);
        for(uint i = 0; i < ns; ++i)
        {
			sum1 = std::abs((centeredMatrix[i][j] - smLib::prod(currCol, matrixA[i]))
                       / centeredMatrix[i][j]);
            if(sum1 > 0.05
			   && std::abs(centeredMatrix[i][j]) > 0.5)
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


    //now should draw amplitude maps OR write to file
        helpString = pathForAuxFiles
					 + "/" + def::ExpName + "_maps.txt";
		myLib::writeMatrixFile(helpString, matrixA); //generality 19-ns
		myLib::drawMapsICA(helpString,
						   def::dir->absolutePath() + "/Help/ica",
						   def::ExpName);


    // save components
    helpString = def::dir->absolutePath()
				 + "/" + def::ExpName + "_ica.edf";

    std::vector<int> chanList(ns);
    std::iota(chanList.begin(), chanList.end(), 0);
    chanList.push_back(globalEdf.getMarkChan());

	/// drop all non-EEG channels ?
//	components.vertCat(resMatBackup);
	components.push_back(globalEdf.getMarkArr());

    globalEdf.writeOtherData(components, helpString, chanList);
    def::ns = ns + 1; // numOfICs + markers

	std::cout << "ICA ended. time = " << wholeTime.elapsed()/1000. << " sec" << std::endl;
}