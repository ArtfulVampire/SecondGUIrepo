#include <myLib/ica.h>
#include <myLib/output.h>
#include <QTime>

#include <iostream>

using namespace myOut;

namespace myLib
{

/// functions
icaResult ica(const matrix & initialData,
			  double eigenValuesTreshold,
			  double vectorWTreshold)
{
	const int numOfIcs = initialData.rows();

	matrix centeredMatrix = initialData;
	centeredMatrix.centerRows(numOfIcs);

	/// auto [eigenVectors, eigenValues] =
	matrix eigenVectors;
	std::valarray<double> eigenValues;
	auto a = myLib::svd(centeredMatrix,
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

	matrix rotation = myLib::countVectorW(components,
										  numOfIcs,
										  initialData.cols(),
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
svd(const matrix & initialData,
	const int dimension, // length of the vectors
	double threshold,
	int eigenVecNum) /// num of eigenVectors to count - add variance
{
	if(eigenVecNum <= 0)
	{
		eigenVecNum = dimension;
	}
	const int dataLen = initialData.cols();

	const int iterationsThreshold = 100;
	const int errorStep = 5;

	double trace = 0.;
	for(int i = 0; i < dimension; ++i)
	{
		trace += smLib::variance(initialData[i]);
	}

	/// results
	std::valarray<double> eigenValues(eigenVecNum);
	matrix eigenVectors(dimension, eigenVecNum);

	std::valarray<double> tempA(dimension);
	std::valarray<double> tempB(dataLen);

	double sum1, sum2; // temporary help values
	double dF, F;
	int counter;

#if 0
	/// ICA test short

	const QString pathForAuxFiles = DEFS.dirPath()
									+ "/Help"
									+ "/ica";
	QString helpString = pathForAuxFiles
						 + "/EigenMatrix.txt";
	eigenVectors = readMatrixFile(helpString);


	// write eigenValues
	helpString = pathForAuxFiles
				 + "/EigenValues.txt";
	eigenValues = readFileInLine(helpString);
	return;
#endif

	// maybe lines longer than dimension but OK
	// std::valarray<double> tempLine(dataLen); // for debug acceleration
	matrix inData = initialData;
	inData.centerRows(dimension);
	const matrix inDataTrans = matrix::transposed(initialData);
	QTime myTime;
	myTime.start();
	// counter j - for B, i - for A
	for(int k = 0; k < eigenVecNum; ++k)
	{
		myTime.restart();

		dF = 0.5;
		F = 1.0;

		tempA = 1. / sqrt(dimension);
		tempB = 1. / sqrt(dataLen);

		// approximate P[i] = tempA x tempB;
		counter = 0;
		while(1) // when stop approximate?
		{
			if((counter) % errorStep == 0)
			{
				// countF - error
				F = 0.;
				for(int i = 0; i < dimension; ++i)
				{
#if 0
					F += 0.5 * pow(inData[i] - tempB * tempA[i], 2.).sum();
#elif 1
					F += 0.5 * smLib::normaSq(
							 std::valarray<double>(inData[i] - tempB * tempA[i]));
#elif 1
					// much faster in debug
					const double coef = tempA[i];
					std::transform(begin(inData[i]),
								   end(inData[i]),
								   begin(tempB),
								   begin(tempLine),
								   [coef](double in1, double in2)
					{
						return in1 - in2 * coef;
					});
					F += 0.5 * normaSq(tempLine);
#else
					for(int j = 0; j < dataLen; ++j)
					{
						F += 0.5 * pow(inData[i][j] - tempB[j] * tempA[i], 2.);
					}


#endif
				}
			}

			// count vector tempB
			sum2 = 1. / smLib::normaSq(tempA);
			for(int j = 0; j < dataLen; ++j)
			{
//                tempB[j] = sum2 * (inDataTrans[j] * tempA).sum();
				// slightly faster
//                tempB[j] = sum2 * std::inner_product(begin(tempA),
//                                                     end(tempA), // begin + dimension
//                                                     begin(inDataTrans[j]),
//                                                     0.);
				tempB[j] = sum2 * smLib::prod(tempA, inDataTrans[j]);
			}

			// count vector tempA
			sum2 = 1. / smLib::normaSq(tempB);
			for(int i = 0; i < dimension; ++i)
			{
//                tempA[i] = sum2 * (tempB * inData[i]).sum();
				// slightly faster
//                tempA[i] = sum2 * std::inner_product(begin(tempB),
//                                                     end(tempB),
//                                                     begin(inData[i]),
//                                                     0.);
				tempA[i] = sum2 * smLib::prod(tempB, inData[i]);
			}

			if((counter) % errorStep == 0)
			{
				dF = 0.;
				for(int i = 0; i < dimension; ++i)
				{
#if 0
					dF += 0.5 * pow((inData[i] - tempB * tempA[i]), 2.).sum();
#elif 1
					dF += 0.5 * smLib::normaSq(std::valarray<double>(inData[i] - tempB * tempA[i]));
#elif 1
					// much faster in debug
					const double coef = tempA[i];
					std::transform(begin(inData[i]),
								   end(inData[i]),
								   begin(tempB),
								   begin(tempLine),
								   [coef](double in1, double in2)
					{
						return in1 - in2 * coef;
					});
					dF += 0.5 * normaSq(tempLine);
#else
					for(int j = 0; j < dataLen; ++j)
					{
						dF += 0.5 * pow(inData[i][j] - tempB[j] * tempA[i], 2.);
					}
#endif
				}
				dF = 1. - dF / F;

			}

			if(counter == iterationsThreshold)
			{
				break;
			}
			++counter;
			if(std::abs(dF) < threshold) break; // crucial cap
		}

		// edit currMatrix
		/// test!
		for(int i = 0; i < dimension; ++i)
		{
#if 1
			// better in release
			inData[i] -= tempA[i] * tempB;
#else
			// better in debug
			const double coef = tempA[i];
			std::transform(begin(inData[i]),
						   end(inData[i]),
						   begin(tempB),
						   begin(tempLine),
						   [coef](double in1, double in2)
			{
				return in1 - in2 * coef;
			});
#endif
		}

		// count eigenVectors && eigenValues
		sum1 = smLib::normaSq(tempA);
		sum2 = smLib::normaSq(tempB);
		eigenValues[k] = sum1 * sum2 / double(dataLen - 1.);
		tempA /= sqrt(sum1); /// smLib::normalize(tempA);


		sum1 = std::accumulate(std::begin(eigenValues),
							   std::begin(eigenValues) + k + 1,
							   0.);

		std::cout << k << "\t";
		std::cout << "val = " << smLib::doubleRound(eigenValues[k], 3) << "\t";
		std::cout << "disp = " << smLib::doubleRound(100. * eigenValues[k] / trace, 2) << "\t";
		std::cout << "total = " << smLib::doubleRound(100. * sum1 / trace, 2) << "\t";
		std::cout << "iters = " << counter << "\t";
		std::cout << smLib::doubleRound(myTime.elapsed() / 1000., 1) << " s" << std::endl;

		for(int i = 0; i < dimension; ++i)
		{
			eigenVectors[i][k] = tempA[i]; // 1-normalized coloumns
		}
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
	return {};
}

} // end namespace myLib