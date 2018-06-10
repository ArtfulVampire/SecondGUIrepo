#ifndef ICA_H
#define ICA_H

#include <other/matrix.h>
#include <utility>
#include <vector>
#include <valarray>
#include <functional>

namespace myLib
{

class icaResult;

std::valarray<double> deorthogonal(const matrix & inMat,
								   int numOfICs,
								   int currNum);

/// matrixW * dataICA = components ????
matrix calculateMatrixW(const matrix & dataICA,
						const int numOfICs,
						const double vectorWTreshold);


icaResult ica(const matrix & initialData,
			  double eigenValuesTreshold,
			  double vectorWTreshold);



std::pair<matrix, std::valarray<double>> eigenValuesSVD(const matrix & initialData,
											 double threshold = 1e-9,
											 int eigenVecNum = -1);



class icaResult
{
private:
	/// initialData = A * components
	matrix components{};
	matrix matrixA{};

public:
	icaResult() {}
	icaResult(const matrix & comps, const matrix & A): components(comps), matrixA(A) {}

	/// the difference in the initial norming and colsNorms filling
	void orderIcaDisp();	/// by dispersion
	void orderIcaLen();		/// by maps-length
	void order(std::function<double(int)> func);

	bool isEmpty() const				{ return components.isEmpty() || matrixA.isEmpty(); }
	operator bool() const			{ return !this->isEmpty(); }

	/// gets
	std::vector<double> getExplVar() const;
	const matrix & getMatrixA() const				{ return matrixA; }
	const matrix & getComponents() const			{ return components; }
};

class ICAclass
{
private:
	icaResult result{};

	matrix inputData{}; /// raw
	int numIC{};
	QString eigMatPath{};
	QString eigVecPath{};
	double eigValThreshold{std::pow(10., -9)};
	double vectWThreshold{std::pow(10., -12)};

public:
	ICAclass(const matrix & inData): inputData(inData), numIC(inData.rows()) {}

	/// main function
	icaResult ica();

	/// gets
	const icaResult & getResult() const				{ return result; }
	const matrix & getComponents() const			{ return result.getComponents(); }
	const matrix & getMatrixA() const				{ return result.getMatrixA(); }

	/// sets
	void setEigMatPath(const QString & in)			{ eigMatPath = in; }
	void setEigVecPath(const QString & in)			{ eigVecPath = in; }
	void setEigValThreshold(double in)				{ eigValThreshold = in; }
	void setVectWThreshold(double in)				{ vectWThreshold = in; }
	void setNumIC(int in)							{ numIC = in; }
};





} // end namespace myLib


#endif // ICA_H
