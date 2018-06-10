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
	std::vector<double> explVar{};

public:
	icaResult() {}
	icaResult(const matrix & comps, const matrix & A): components(comps), matrixA(A)
	{
		calculateExplVar();
	}

	/// the difference in the initial norming and colsNorms filling
	void orderIcaDisp();	/// by dispersion
	void orderIcaLen();		/// by maps-length
	void order(std::function<double(int)> func);
	void calculateExplVar();

	bool isEmpty() const				{ return components.isEmpty() || matrixA.isEmpty(); }
	operator bool() const			{ return !this->isEmpty(); }

	/// gets
	const std::vector<double> & getExplVar() const	{ return explVar; }
	const matrix & getMatrixA() const				{ return matrixA; }
	const matrix & getComponents() const			{ return components; }
};



class ICAclass
{
private:
	icaResult result{};

	matrix inputData{}; /// not centered
	int numIC{};
	matrix eigenVectors;
	std::valarray<double> eigenValues;
	QString locExpName{};

	QString eigMatPath{};
	QString eigValPath{};
	QString explVarPath{};
	QString mapsFilePath{};
	QString drawMapsPath{};

	double eigValThreshold{std::pow(10., -10)};
	double vectWThreshold{std::pow(10., -12)};

public:
	ICAclass(const matrix & inData): inputData(inData), numIC(inData.rows()) {}
	ICAclass(matrix && inData): inputData(inData), numIC(inData.rows()) {}

	/// main function
	void calculateICA();

	/// output to files
	void printExplainedVariance() const;
	void printEigenVectors() const;
	void printEigenValues() const;
	void printMapsFile() const;
	void drawMaps() const;
	int getNumOfErrors(std::ostream & os = std::cout) const;

	/// gets
	const icaResult & getResult() const				{ return result; }
	const matrix & getComponents() const			{ return result.getComponents(); }
	const matrix & getMatrixA() const				{ return result.getMatrixA(); }
	const std::vector<double> & getExplVar() const	{ return result.getExplVar(); }


	/// sets
	void setExpName(const QString & in)				{ locExpName = in; }
	void setEigMatPath(const QString & in)			{ eigMatPath = in; }
	void setEigValPath(const QString & in)			{ eigValPath = in; }
	void setExplVarPath(const QString & in)			{ explVarPath = in; }
	void setMapsFilePath(const QString & in)		{ mapsFilePath = in; }
	void setDrawMapsPath(const QString & in)		{ drawMapsPath = in; }
	void setEigValThreshold(double in)				{ eigValThreshold = in; }
	void setVectWThreshold(double in)				{ vectWThreshold = in; }
	void setNumIC(int in)							{ numIC = in; }
};





} // end namespace myLib


#endif // ICA_H
