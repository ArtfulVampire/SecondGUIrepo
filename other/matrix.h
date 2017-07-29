#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <chrono>
#include <ios>
#include <iostream>

#include <string>
#include <vector>
#include <valarray>
#include <set>

#if _OPENMP
#include <omp.h>
#endif

#include <myLib/small.h>
#include <myLib/valar.h>

#include <QString>
#include <QDir>

// omp not effective
#define MATRIX_OMP _OPENMP&&0

using matrixType = std::vector<std::valarray<double>>;

class matrix
{
public:
	matrix();
	~matrix();
	matrix(int dim);
	matrix(int rows, int cols);
	matrix(int rows, int cols, double value);

	matrix(const matrix & other);
	matrix(const matrixType & other);

	matrix(matrix && other)
		:myData{other.myData}
	{
		other.myData = matrixType();
	}


	matrix(const std::valarray<double> & vect1, const std::valarray<double> & vect2);
	matrix(const std::valarray<double> & vect, bool orientH);
	matrix(const std::valarray<double> & vect, char orient);
	matrix(const std::valarray<double> & vect, uint rows);

	matrix(const std::valarray<double> & vect); // diagonal
	matrix(std::initializer_list<double> lst); // diagonal

	matrix(std::initializer_list<std::valarray<double>> lst);



	matrix & resizeRows(int rows);
	matrix & resizeCols(int newCols);
	matrix & fill(double value);
	void print(uint rows = 0, uint cols = 0) const;
	uint cols() const;
	uint rows() const;
	double maxVal() const;
	double minVal() const;
	double maxAbsVal() const;
	double minAbsVal() const;

	double sum() const;
	matrixType::iterator begin();
	matrixType::iterator end();
	matrixType::const_iterator begin() const;
	matrixType::const_iterator end() const;
	bool isEmpty() const;
	std::valarray<double> & last() { return myData.back(); }
	std::valarray<double> & back() { return myData.back(); }
	std::valarray<double> & front() { return myData.front(); }
	std::valarray<double> & first() { return myData.front(); }

	std::valarray<double> toVectorByRows() const;
	std::valarray<double> toVectorByCols() const;
	std::valarray<double> getCol(uint i, uint numCols = 0) const;
	std::valarray<double> averageRow() const;
	std::valarray<double> averageCol() const;
	std::valarray<double> sigmaOfCols() const;
	std::valarray<double> maxOfRows() const;
	std::valarray<double> maxOfCols() const;
	void pop_back();
	void push_back(const std::valarray<double> &in);
	void push_back(const std::vector<double> &in);

	// for compability with vector< std::vector<Type> >
	void clear() {this->myData.clear(); }
	uint size() const { return myData.size(); }

	void resize(int rows, int cols, double val);
	void resize(int rows, int cols);
	void resize(int i);


	std::valarray<double> & operator [](int i)
	{
		return myData[i];

	}
	const std::valarray<double> & operator [](int i) const
	{
		return myData[i];
	}

	matrix operator = (const matrix & other);
	matrix operator = (const matrixType & other);

	matrix operator += (const matrix & other);
	matrix operator += (double val);

	matrix operator -= (const matrix & other);
	matrix operator -= (double val);

	matrix operator *= (const matrix & other);
	matrix operator *= (double val);

	matrix operator /= (double other);
	matrix operator -();

	bool operator == (const matrix & other);
	bool operator != (const matrix & other);


	// "static"
	static matrix transpose(const matrix & input);
	static matrix ident(int dim);
	static matrix vertCat(const matrix & upper, const matrix & lower);
	static matrix horzCat(const matrix & left, const matrix & right);

	// "private"
	double trace() const;
	matrix & transpose();
	matrix & invert(double * det = nullptr);
	matrix covMatCols(std::valarray<double> * avRow = nullptr) const;
	matrix & swapCols(uint i, uint j);
	matrix & swapRows(uint i, uint j);
	matrix & zero();
	matrix & one();
	matrix & eraseRow(uint i);
	matrix & eraseCol(uint j);
	//    matrix & eraseRows(const std::vector<int> & indices);
	matrix & eraseRows(const std::vector<uint> & indices);
	matrix & random(double low = 0., double high = 1.);
	matrix & vertCat(matrix && other);
	matrix & horzCat(const matrix & other);
	matrix & pop_front(uint numOfCols);
	matrix subCols(int beginCol, int endCol) const; /// submatrix
	matrix subRows(const std::vector<int> & inds) const; /// submatrix
	matrix subRows(const std::vector<uint> & inds) const; /// submatrix

	std::valarray<double> matrixSystemSolveGauss(const std::valarray<double> & inVec) const;
public:
	matrixType myData {matrixType()};

public:
	friend std::ostream & operator<<(std::ostream & os, const matrix & in);
};


matrix operator + (const matrix & lhs, const matrix & rhs);
matrix operator + (const matrix & lhs, double val);
matrix operator / (const matrix & lhs, double val);
matrix operator * (const matrix & lhs, const matrix & rhs);
matrix operator * (const matrix & lhs, double val);
std::valarray<double> operator * (const matrix & lhs, const std::valarray<double> & rhs);
std::valarray<double> operator * (const std::valarray<double> & lhs, const matrix & rhs);
matrix operator - (const matrix & lhs, const matrix & rhs);
matrix operator - (const matrix & lhs, double val);

// template <typename matType1, typename matType2>
void matrixProduct(const matrix & in1,
				   const matrix & in2,
				   matrix & result,
				   uint dim = 0,
				   uint rows1 = 0,
				   uint cols2 = 0);
#endif // MATRIX_H
