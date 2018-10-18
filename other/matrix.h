#ifndef MATRIX_H
#define MATRIX_H

#include <ios>
#include <iostream>
#include <vector>
#include <valarray>
#include <functional>
#if _OPENMP
#include <omp.h>
#endif

#include <QString>
#include <QDir>

/// omp not effective
#define MATRIX_OMP _OPENMP&&0

using matrixType = std::vector<std::valarray<double>>;

class matrix
{
public:
	matrix()=default;
	~matrix()=default;
	matrix(matrix && other)=default;
	matrix(const matrix & other)=default;

	matrix & operator= (const matrix & other)=default;
	matrix & operator= (matrix && other)=default;



	matrix(const matrixType & other);
	matrix(int dim);
	matrix(int rows, int cols);
	matrix(int rows, int cols, double value);

	matrix(const std::valarray<double> & vect1, const std::valarray<double> & vect2);
	matrix(const std::valarray<double> & vect, bool orientH);
	matrix(const std::valarray<double> & vect, char orient);
	matrix(const std::valarray<double> & vect, int rows);

	matrix(const std::valarray<double> & vect); /// diagonal
	matrix(std::initializer_list<double> lst); /// diagonal

	matrix(std::initializer_list<std::valarray<double>> lst);



	matrix & resizeRows(int rows);
	matrix & resizeCols(int newCols);
	matrix & fill(double value);
	void print(int rows = 0, int cols = 0) const;
	void printWithBraces(int rows = 0, int cols = 0) const;
	int cols() const	{ return myData[0].size(); }
	int rows() const	{ return myData.size(); }
	double maxVal() const;
	double minVal() const;
	double maxAbsVal() const;
	double minAbsVal() const;

	double sum() const;
	double traceCov() const;
	bool isEmpty() const			{ return(this->rows() == 0 || this->cols() == 0); }
	matrixType::iterator begin()					{ return std::begin(myData); }
	matrixType::iterator end()						{ return std::end(myData); }
	matrixType::const_iterator begin() const		{ return std::begin(myData); }
	matrixType::const_iterator end() const			{ return std::end(myData); }
	std::valarray<double> & last()					{ return myData.back(); }
	std::valarray<double> & back()					{ return myData.back(); }
	const std::valarray<double> & last() const		{ return myData.back(); }
	const std::valarray<double> & back() const		{ return myData.back(); }

	std::valarray<double> & front()					{ return myData.front(); }
	std::valarray<double> & first()					{ return myData.front(); }
	const std::valarray<double> & front() const		{ return myData.front(); }
	const std::valarray<double> & first() const		{ return myData.front(); }

	std::valarray<double> toValarByRows() const;
	std::valarray<double> toValarByCols() const;
	std::vector<double> toVectorByRows() const;
	std::valarray<double> getCol(int i, int numRows = 0) const;
	std::valarray<double> averageRow() const;
	std::valarray<double> averageCol() const;
	std::valarray<double> sigmaOfCols() const;
	std::valarray<double> maxOfRows() const;
	std::valarray<double> maxOfCols() const;
	matrix & pop_back();
	matrix & push_back(const std::valarray<double> &in);
	matrix & push_back(const std::vector<double> &in);

	/// for compability with vector< std::vector<Type> >
	void clear()				{ myData.clear(); }

	matrix & resize(int rows, int cols, double val);
	matrix & resize(int rows, int cols);
	matrix & resize(int i)		{ myData.resize(i); return *this; }
	matrix & reserve(int i)		{ myData.reserve(i); return *this; }

	std::valarray<double> & operator[](int i)
	{
		return myData[i];
	}

	const std::valarray<double> & operator[](int i) const
	{
		return myData[i];
	}

	matrix & operator= (double other);
	matrix & operator= (const matrixType & other);

	matrix & operator +=(const matrix & other);
	matrix & operator +=(double val);

	matrix & operator -=(const matrix & other);
	matrix & operator -=(double val);

	matrix operator* (const matrix & other) const;
	matrix & operator *=(const matrix & other);
	matrix & operator *=(double val);

	matrix & operator /=(double other);
	matrix operator-() const;

	bool operator== (const matrix & other) const;
	bool operator!= (const matrix & other) const;

	/// transpose
	matrix operator!() const { return matrix::transposed(*this); } /// DANGER
	/// inverse
	matrix operator~() const { return matrix::inverted(*this); }

	matrix apply(std::function<double(double)> func) const;
	matrix apply(const std::function<std::valarray<double>(const std::valarray<double> &)> &) const;
	matrix integrate(const std::vector<std::pair<int, int>> & intervals) const;


	/// "static"
	static matrix transposed(const matrix & input);
	static matrix inverted(const matrix & input);
	static matrix ident(int dim);
	static matrix vertCat(const matrix & upper, const matrix & lower);
	static matrix horzCat(const matrix & left, const matrix & right);

	/// "private"
	double trace() const;
	matrix & transpose();
	matrix & invert(double * det = nullptr);
	matrix & normColsLastRowOne(); /// what is that?

	matrix & random(double low = 0., double high = 1.);
	matrix & vertCat(matrix && other);
	matrix & horzCat(const matrix & other);
	matrix & pop_front(int numOfCols);

	/// cols operations
	matrix & swapCols(int i, int j);
	matrix covMatCols(std::valarray<double> * avRow = nullptr) const;
	matrix & eraseCol(int j);
	matrix subCols(int beginCol, int endCol) const;			/// submatrix
	matrix subCols(int newCol) const;						/// submatrix
	matrix subCols(const std::vector<std::pair<int, int>> & intervals) const;
	matrix subColsStride(int start, int stride) const;

	/// rows operations
	matrix & swapRows(int i, int j);
	matrix covMatRows() const;
	matrix & centerRows(int numRows = 0);
	matrix & eraseRow(int i);
	matrix subRows(int newRows) const;						/// submatrix
//	matrix subRows(int begRow, int endRow) const;			//// to do
	matrix subRows(const std::vector<int> & inds) const;	/// submatrix
	matrix subRows(const std::vector<uint> & inds) const;	/// submatrix
//	matrix subRows(const std::vector<std::pair<int, int>> & intervals) const; //// to do

	template <typename Typ>
	matrix & eraseRows(const std::vector<Typ> & indices);

	std::valarray<double> matrixSystemSolveGauss(const std::valarray<double> & inVec) const;
public:
	matrixType myData {matrixType()};
};


std::ostream & operator<< (std::ostream & os, const matrix & toOut);
matrix operator+ (const matrix & lhs, const matrix & rhs);
matrix operator+ (const matrix & lhs, double val);
matrix operator/ (const matrix & lhs, double val);
//matrix operator* (const matrix & lhs, const matrix & rhs);
matrix operator* (const matrix & lhs, double val);
std::valarray<double> operator* (const matrix & lhs, const std::valarray<double> & rhs);
std::valarray<double> operator* (const std::valarray<double> & lhs, const matrix & rhs);
matrix operator- (const matrix & lhs, const matrix & rhs);
matrix operator- (const matrix & lhs, double val);

#endif /// MATRIX_H
