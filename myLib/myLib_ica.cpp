#include <myLib/ica.h>
#include <myLib/output.h>
#include <QTime>

#include <iostream>

using namespace myOut;

namespace myLib
{

/// functions
std::valarray<double> deorthogonal(const matrix & inMat,
								   int numOfICs,
								   int currNum)
{
	/// calculate the sum of projections of currNum vector to previous ones
	std::valarray<double> outVector(numOfICs);
	for(int j = 0; j < currNum; ++j)
	{
		outVector += inMat[j] * smLib::prod(inMat[currNum], inMat[j]);
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

	matrix res{}; /// matrix of row-vectors wt

	for(int numVec = 0; numVec < numOfICs; ++numVec) // number of current vectorW
	{
		myTime.restart();
		int counter = 0;
		std::valarray<double> wt(dataICA.rows());
		wt = smLib::randomValar(wt.size());			/// randomize + normalize wt

		double squareError{};
		while(1)
		{
			std::valarray<double> vecOld = wt; // save previous vect
			std::valarray<double> vector1 (numOfICs);
			vector1 = 0.;

			double sum1 = 0.;
			for(int numCol = 0; numCol < dataICA.cols(); ++numCol)
			{
				const std::valarray<double> x = dataICA.getCol(numCol);

				/// first term
				const double temp = std::tanh( smLib::prod(wt, x) );
				vector1 += x * temp;

				/// for second term (vector 2)
				sum1 += (1. - temp * temp);
			}
			vector1 /= dataICA.cols();
			sum1 /= dataICA.cols();

			wt = smLib::normalized(vector1 - sum1 * wt);

			/// orthogonalization
			wt -= myLib::deorthogonal(res, numOfICs, numVec);
			smLib::normalize(wt);

			/// calculate convergence
			squareError = smLib::norma(std::valarray<double>(vecOld - wt));
			++counter;
			if(squareError < vectorWTreshold || 2. - squareError < vectorWTreshold) break;
			if(counter == 100) break;
		}
		std::cout << "vectW num = " << numVec << "\t";
		std::cout << "iters = " << counter << "\t";
		std::cout << "error = " << std::fdim(squareError, std::round(squareError)) << "\t";
		std::cout << "time = " << smLib::doubleRound(myTime.elapsed() / 1000., 1) << " sec" << std::endl;
		res.push_back(wt);
	}
	return res;
}

icaResult ica(const matrix & initialData,
			  double eigenValuesTreshold,
			  double vectorWTreshold)
{
	const int numOfIcs = initialData.rows();		/// max of possible ICs

	matrix centeredMatrix(initialData);
	centeredMatrix.centerRows();

	/// auto [eigenVectors, eigenValues] =
	matrix eigenVectors;
	std::valarray<double> eigenValues;
	auto a = myLib::eigenValuesSVD(centeredMatrix,
						numOfIcs,
						eigenValuesTreshold);
	eigenVectors = a.first;
	eigenValues = a.second;

	/// write whitenedData to components
	/// whitenedData = Eig * D^-0.5 * Eig^t * centeredData
	matrix components = eigenVectors
						* matrix(std::pow(eigenValues, -0.5))
						* !eigenVectors
						* centeredMatrix;

	matrix rotation = myLib::calculateMatrixW(components,
											  numOfIcs,
											  vectorWTreshold);
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

	icaResult res(components, matrixA);
	res.orderIcaDisp();
	return res;
}



std::pair<matrix, std::valarray<double>>
eigenValuesSVD(const matrix & initialData,
	double threshold,
	int eigenVecNum) /// num of eigenVectors to count - add variance
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


///icaResult
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
}


std::vector<double> icaResult::getExplVar() const
{
	std::vector<double> res{};
	double sum = 0.;
	for(int i = 0; i < components.rows(); ++i)
	{
		double a = smLib::variance(components[i]) * smLib::normaSq(matrixA.getCol(i));
		res.push_back(a);
		sum += a;
	}
	std::for_each(std::begin(res), std::end(res), [sum](double & in) { in /= sum; });
	return res;
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
icaResult ICAclass::ica()
{
	matrix centeredData(inputData);
	centeredData.centerRows();

	/// auto [eigenVectors, eigenValues] =
	matrix eigenVectors;
	std::valarray<double> eigenValues;
	auto a = myLib::eigenValuesSVD(centeredData,
						numIC,
						eigValThreshold);
	eigenVectors = a.first;
	eigenValues = a.second;

	/// write whitenedData to components
	/// whitenedData = Eig * D^-0.5 * Eig^t * centeredData
	matrix components = eigenVectors
						* matrix(std::pow(eigenValues, -0.5))
						* !eigenVectors
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
					 * !eigenVectors
					 * !rotation;

	icaResult res(components, matrixA);
	res.orderIcaDisp();
	return res;
}

} // end namespace myLib
