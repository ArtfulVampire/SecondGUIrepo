#include <myLib/ica.h>
#include <myLib/output.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>

#include <QTime>

#include <iostream>

using namespace myOut;

namespace myLib
{

/// functions
std::valarray<double> deorthogonal(const matrix & inMat,
								   int currNum)
{
	/// calculate the sum of projections of currNum vector to previous ones
	std::valarray<double> outVector(inMat.cols());

	for(int j = 0; j < currNum; ++j)
	{
		outVector += inMat[j] * smLib::prod(inMat[j], inMat[currNum]);
	}
	return outVector;
}


/// make a class for two functions and derivatives

matrix calculateMatrixW(const matrix & dataICA,
						const int numOfICs,
						const double vectorWTreshold)
{
	QTime myTime;
	myTime.restart();
	const matrix dataICAtransposed = matrix::transposed(dataICA);

	matrix res(numOfICs, dataICA.rows(), 0.); /// matrix of row-vectors wt

	for(int numVec = 0; numVec < numOfICs; ++numVec) // number of current vectorW
	{
		myTime.restart();

		std::valarray<double> & wt = res[numVec];
		wt = smLib::randomValar(wt.size());

		double squareError = 1.;
		int counter = 0;
		while((((squareError > vectorWTreshold) && (2. - squareError > vectorWTreshold))
			  && counter < 100 ) || counter < 3)
		{
			const std::valarray<double> vecOld = wt;

			std::valarray<double> tmpVec(0., dataICA.rows());
			double sum1 = 0.;
			for(int numCol = 0; numCol < dataICA.cols(); ++numCol)
			{
				const std::valarray<double> & x = dataICAtransposed[numCol];

				/// first term
				const double temp = std::tanh( smLib::prod(vecOld, x) );
				tmpVec += x * temp;

				/// for second term (vector 2)
				sum1 += (1. - temp * temp);
			}
			wt = (tmpVec - sum1 * vecOld) / dataICA.cols();
			/// orthogonalization
			auto deort = deorthogonal(res, numVec);
			wt -= deort;
			smLib::normalize(wt);

			/// calculate convergence
			squareError = smLib::norma(std::valarray<double>(vecOld - wt));
			++counter;
		}
		std::cout << "vectW num = " << numVec << "\t";
		std::cout << "iters = " << counter << "\t";
		std::cout << "error = " << std::abs(squareError - std::round(squareError)) << "\t";
		std::cout << "time = " << smLib::doubleRound(myTime.elapsed() / 1000., 1) << " sec" << std::endl;
	}
	return res;
}

std::pair<matrix, std::valarray<double>>
eigenValuesSVD(const matrix & initialData,
			   int eigenVecNum,
			   double threshold)
{
	/// wiki(rus) - Principal Component Analysis -> simple iterative algorithm
	/// https://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%82%D0%BE%D0%B4_%D0%B3%D0%BB%D0%B0%D0%B2%D0%BD%D1%8B%D1%85_%D0%BA%D0%BE%D0%BC%D0%BF%D0%BE%D0%BD%D0%B5%D0%BD%D1%82#%D0%9F%D1%80%D0%BE%D1%81%D1%82%D0%BE%D0%B9_%D0%B8%D1%82%D0%B5%D1%80%D0%B0%D1%86%D0%B8%D0%BE%D0%BD%D0%BD%D1%8B%D0%B9_%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D1%81%D0%B8%D0%BD%D0%B3%D1%83%D0%BB%D1%8F%D1%80%D0%BD%D0%BE%D0%B3%D0%BE_%D1%80%D0%B0%D0%B7%D0%BB%D0%BE%D0%B6%D0%B5%D0%BD%D0%B8%D1%8F

	if(eigenVecNum <= 0)
	{
		eigenVecNum = initialData.rows();
	}

	const int iterationsThreshold = 100;
	const int errorStep = 5;

	/// auxiliary data
	matrix inData(initialData);
	inData.centerRows();
	const matrix inDataTrans = matrix::transposed(inData);

	const double trace = inData.traceCov();

	/// results
	std::valarray<double> eigenValues(eigenVecNum);
	matrix eigenVectors(initialData.rows(), eigenVecNum);


	/// error function
	auto mse = [](const matrix & mat,
			   const std::valarray<double> & vecB,
			   const std::valarray<double> & vecA) -> double
	{
		double res = 0.;
		for(int i = 0; i < mat.rows(); ++i)
		{
			res += 0.5 * smLib::normaSq(
					 std::valarray<double>(mat[i] - vecB[i] * vecA)
					 );
		}
		return res;
	};

	QTime myTime;
	myTime.start();

	double currTrace = 0;
	for(int k = 0; k < eigenVecNum; ++k)
	{
		myTime.restart();

		std::valarray<double> tempA = smLib::randomValar(initialData.cols());
		std::valarray<double> tempB = smLib::randomValar(initialData.rows());

		/// approximate P[i] = tempB x tempA;
		int counter = 0;
		double dF = 2 * threshold;	/// relative error
		while((std::abs(dF) > threshold) && (counter < iterationsThreshold))
		{
			/// sometimes recalculate F
			double F{};
			if(counter % errorStep == 0) { F = mse(inData, tempB, tempA); }

			/// calculate tempA = tempB^t * inData
			double nB = 1. / smLib::normaSq(tempB);
			for(int j = 0; j < initialData.cols(); ++j)
			{
				tempA[j] = nB * smLib::prod(tempB, inDataTrans[j]);
			}

			/// calculate tempB = inData * tempA
			double nA = 1. / smLib::normaSq(tempA);
			for(int i = 0; i < initialData.rows(); ++i)
			{
				tempB[i] = nA * smLib::prod(inData[i], tempA);
			}

			/// sometimes recount error after adjust
			if(counter % errorStep == 0) { dF = 1. - mse(inData, tempB, tempA) / F; }
			++counter;
		}

		/// edit currMatrix
		for(int i = 0; i < initialData.rows(); ++i)
		{
			inData[i] -= tempB[i] * tempA;
		}

		/// count eigenVectors && eigenValues
		/// singular value = narma(tempA) * norma(tempB)
		/// eigenValue = sqr(singular value);
		/// look https://en.wikipedia.org/wiki/Singular-value_decomposition
		eigenValues[k] = smLib::normaSq(tempB) *
						 smLib::normaSq(tempA)
						 / double(initialData.cols() - 1.);
		currTrace += eigenValues[k];

		smLib::normalize(tempB);
		for(int i = 0; i < initialData.rows(); ++i)
		{
			eigenVectors[i][k] = tempB[i];
		}

		std::cout << k << "\t";
		std::cout << "val = " << smLib::doubleRound(eigenValues[k], 3) << "\t";
		std::cout << "disp = " << smLib::doubleRound(100. * eigenValues[k] / trace, 2) << "\t";
		std::cout << "total = " << smLib::doubleRound(100. * currTrace / trace, 2) << "\t";
		std::cout << "iters = " << counter << "\t";
		std::cout << smLib::doubleRound(myTime.elapsed() / 1000., 1) << " s" << std::endl;
	}
	return std::make_pair(eigenVectors, eigenValues);
}


/// icaResult class
void icaResult::order(std::function<double(int)> func)
{
	std::vector<std::pair <double, int>> colsNorms; // dispersion, numberOfComponent
	for(int i = 0; i < matrixA.rows(); ++i)
	{
		double val = func(i);
		colsNorms.push_back(std::make_pair(val, i));
	}

	std::sort(std::begin(colsNorms),
			  std::end(colsNorms),
			  [](const auto & i, const auto & j)
	{ return i.first > j.first; });

	for(int i = 0; i < matrixA.cols() - 1; ++i) // the last is already on place
	{
		matrixA.swapCols(i, colsNorms[i].second);
		components.swapRows(i, colsNorms[i].second);

		// swap i and colsNorms[i].second values in colsNorms
		auto it1 = std::find_if(std::begin(colsNorms),
								std::end(colsNorms),
								[i](const auto & in)
		{ return in.second == i; });
		auto it2 = std::begin(colsNorms) + i;
		std::swap((*it1).second, (*it2).second);
	}
	/// recalculate after unmixing
	calculateExplVar();
}


void icaResult::calculateExplVar()
{
	explVar.clear();
	double sum = 0.;
	for(int i = 0; i < components.rows(); ++i)
	{
		double a = smLib::variance(components[i]) * smLib::normaSq(matrixA.getCol(i));
		explVar.push_back(a);
		sum += a;
	}
	std::for_each(std::begin(explVar), std::end(explVar), [sum](double & in) { in /= sum; });
}

void icaResult::orderIcaLen()
{
	// norm components - to equal dispersion
	// sort by maps length

	for(int i = 0; i < components.rows(); ++i)
	{
		double sgm = smLib::sigma(components[i]) * 0.05; /// it is some coeff
		components[i] /= sgm;

		for(int j = 0; j < matrixA.rows(); ++j)
		{
			matrixA[j][i] *= sgm;
		}
	}

	/// ordering components by sum of squares of the matrixA coloumn
	auto func = [this](int i)->double { return smLib::normaSq(matrixA.getCol(i)); };
	order(func);
}

void icaResult::orderIcaDisp()
{
	/// cols of matrixA to 1-length, adjust corresponding row
	for(int j = 0; j < matrixA.cols(); ++j)
	{
		double colNorm = smLib::norma(matrixA.getCol(j)); /// here was /2. WHY?

		for(int i = 0; i < matrixA.rows(); ++i)
		{
			matrixA[i][j] /= colNorm;
		}
		components[j] *= colNorm; /// components.rows() = matrixA.cols()
	}

	/// ordering itself
	auto func = [this](int i)->double { return smLib::variance(components[i]); };
	order(func);
}



/// ICAclass
void ICAclass::calculateICA()
{
	matrix centeredData(inputData);
	centeredData.centerRows();

	if(!QFile::exists(eigMatPath) ||
	   !QFile::exists(eigValPath)
	   || 1 /// always recalculate
	   )
	{
		/// auto [eigenVectors, eigenValues] =
		auto a = myLib::eigenValuesSVD(centeredData,
									   numIC,
									   eigValThreshold);

		eigenVectors = a.first;
		eigenValues = a.second;
	}
	else
	{
		eigenVectors = myLib::readMatrixFile(eigMatPath);
		eigenValues = myLib::readFileInLine(eigValPath);
	}

	/// write whitenedData to components
	/// whitenedData = Eig * D^-0.5 * Eig^t * centeredData
	matrix components = eigenVectors
						* matrix(std::pow(eigenValues, -0.5))
						* matrix::transposed(eigenVectors)
						* centeredData;

	matrix rotation = myLib::calculateMatrixW(components,
											  numIC,
											  vectWThreshold);
	/// components = rotation * whitenedData
	/// components = W * centeredData
	/// W = rotation * Eig * D^-0.5 * Eig^t
	components = rotation * components;

	/// A * components = centeredData
	/// A = inverted( W )
	/// A = Eig * D^0.5 * Eig^t * rotation^t
	matrix matrixA = eigenVectors
					 * matrix(std::pow(eigenValues, +0.5))
					 * matrix::transposed(eigenVectors)
					 * matrix::transposed(rotation);

	this->result = icaResult(components, matrixA);
	this->result.orderIcaDisp();
}

void ICAclass::calculateSVD()
{

	auto a = myLib::eigenValuesSVD(inputData,
								   numIC,
								   eigValThreshold);

	eigenVectors = a.first;
	eigenValues = a.second;
}

void ICAclass::printExplainedVariance() const
{
	if(!result || explVarPath.isEmpty()) { return; }
	myLib::writeFileInLine(explVarPath, this->getExplVar());
}

void ICAclass::printEigenVectors() const
{
	if(eigenVectors.isEmpty() || eigMatPath.isEmpty()) { return; }
	myLib::writeMatrixFile(eigMatPath, eigenVectors);
}

void ICAclass::printEigenValues() const
{
	if(eigenValues.size() == 0 || eigValPath.isEmpty()) { return; }
	myLib::writeFileInLine(eigValPath, eigenValues);
}

void ICAclass::printMapsFile() const
{
	if(!result || mapsFilePath.isEmpty()) { return; }
	myLib::writeMatrixFile(mapsFilePath, this->getMatrixA());
}

void ICAclass::drawMaps() const
{
	matrix matrixA  = this->getMatrixA();

	for(int i = 0; i < matrixA.cols(); ++i)
	{
		myLib::drawMapSpline(matrixA,
							 i,
							 drawMapsPath,
							 locExpName,
							 matrixA.maxAbsVal(),
							 240,					/// magic const - picture size
							 myLib::ColorScale::jet);
	}
}

void ICAclass::drawSpectraWithMaps() const
{
#if 0
	matrix centeredData(inputData);
	centeredData.centerRows();

	/// use placeholders
	std::bind myLib::spectreWelchRtoR()

	centeredData.apply()

	myLib::drawArrays(myLib::drawTemplate(),
					  drawMatrix).save(drawMapsWMPath, 0, 100);

	myLib::drawMapsOnSpectra();
	for(int i = 0; i < matrixA.cols(); ++i)
	{
		myLib::drawMapSpline(matrixA,
							 i,
							 drawMapsPath,
							 locExpName,
							 matrixA.maxAbsVal(),
							 240,					/// magic const - picture size
							 myLib::ColorScale::jet);
	}
#endif
}

int ICAclass::getNumOfErrors(std::ostream & os) const
{
	int counter = 0;
	matrix centeredData(inputData); centeredData.centerRows();
	matrix diff = result.getMatrixA() * result.getComponents() - centeredData;
	for(int i = 0; i < diff.rows(); ++i)
	{
		for(int j = 0; j < diff.cols(); ++j)
		{
			/// magic consts
			double error = std::abs(diff[i][j]) / centeredData[i][j];
			if( error > 0.05 && centeredData[i][j] > 0.5)
			{
				++counter;
				os << i << "\t" << j << "\t";
				os << "err = " << smLib::doubleRound(error, 3) << "\t";
				os << "initVal = " << smLib::doubleRound(centeredData[i][j], 3) << std::endl;
			}
		}
	}
	os << "num of errors = " << counter << std::endl;
	return counter;
}


void ICAclass::setOutPaths(const QString & inHelpPath)
{
	eigMatPath = inHelpPath + "/ica/" + locExpName + "_eigenMatrix.txt";
	eigValPath = inHelpPath + "/ica/" + locExpName + "_eigenValues.txt";
	explVarPath = inHelpPath + "/ica/" + locExpName + "_explainedVariance.txt";
	mapsFilePath = inHelpPath + "/ica/" + locExpName + "_maps.txt";
	drawMapsPath = inHelpPath + "/maps";
	drawMapsWMPath = inHelpPath + "/wm/" + locExpName + "_wm.txt";
}

} // end namespace myLib
