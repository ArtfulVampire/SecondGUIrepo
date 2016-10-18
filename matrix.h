#ifndef MATRIX_H
#define MATRIX_H

#include "smallLib.h"

#include <QString>
#include <QDir>

#include <cmath>
#include <cstdlib>
#include <cstdio>
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


    matrix(const lineType & vect1, const lineType & vect2);
    matrix(const lineType & vect, bool orientH);
    matrix(const lineType & vect, char orient);
    matrix(const lineType & vect, uint rows);

    matrix(const lineType & vect); // diagonal
    matrix(std::initializer_list<double> lst); // diagonal

    matrix(std::initializer_list<lineType> lst);



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
	lineType & last() {return myData.back();}
	lineType & back() {return myData.back();}
	lineType & front() {return myData.front();}
	lineType & first() {return myData.front();}

    lineType toVectorByRows() const;
    lineType toVectorByCols() const;
    lineType getCol(uint i, uint numCols = 0) const;
    lineType averageRow() const;
    lineType averageCol() const;
    lineType sigmaOfCols() const;
	lineType maxOfRows() const;
	lineType maxOfCols() const;
    void pop_back();
    void push_back(const lineType &in);
    void push_back(const vectType &in);

    // for compability with vector < vector<Type> >
	void clear() {this->myData.clear();}
	uint size() const {return myData.size();}

    void resize(int rows, int cols, double val);
    void resize(int rows, int cols);
	void resize(int i);


    lineType & operator [](int i)
    {
		return myData[i];

    }
    const lineType & operator [](int i) const
    {
		return myData[i];
    }

    matrix operator = (const matrix & other);
	matrix operator = (const matrixType & other);

    matrix operator += (const matrix & other);
    matrix operator += (const double & val);

    matrix operator -= (const matrix & other);
    matrix operator -= (const double & val);

    matrix operator *= (const matrix & other);
    matrix operator *= (const double & val);

    matrix operator /= (const double & other);
    matrix operator -();

    bool operator == (const matrix & other);
	bool operator != (const matrix & other);


    //"static"
    static matrix transpose(const matrix & input);
    static matrix ident(int dim);

    // "private"
    double trace() const;
    matrix & transpose();
    matrix & invert(double * det = nullptr);
    matrix covMatCols(lineType * avRow = nullptr) const;
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
    matrix subCols(int beginCol, int endCol) const; /// submatrix
    matrix subRows(const std::vector<int> & inds) const; /// submatrix
    matrix subRows(const std::vector<uint> & inds) const; /// submatrix

    lineType matrixSystemSolveGauss(const lineType & inVec) const;

//    double det();
//    void cofactor();
//    void systemGaussSolve();

public:
	matrixType myData {matrixType()};

};

matrix operator + (const matrix & lhs, const matrix & rhs);
matrix operator + (const matrix & lhs, const double & val);
matrix operator / (const matrix & lhs, const double & val);
matrix operator * (const matrix & lhs, const matrix & rhs);
matrix operator * (const matrix & lhs, const double & val);
lineType operator * (const matrix & lhs, const lineType & rhs);
lineType operator * (const lineType & lhs, const matrix & rhs);
matrix operator - (const matrix & lhs, const matrix & rhs);
matrix operator - (const matrix & lhs, const double & val);

//template <typename matType1, typename matType2>
void matrixProduct(const matrix & in1,
                   const matrix & in2,
                   matrix & result,
                   uint dim = 0,
                   uint rows1 = 0,
                   uint cols2 = 0);

//void matrixProduct(const lineType &in1,
//                   const matrix &in2,
//                   matrix & result,
//                   int dim = -1,
//                   int rows1 = -1,
//                   int cols2 = -1);

//void matrixProduct(const matrix &in1,
//                   const lineType &in2,
//                   matrix & result,
//                   int dim = -1,
//                   int rows1 = -1,
//                   int cols2 = -1);

#if 0
    matrix invert(const matrix & input);
    matrix det(const matrix & input);
    matrix cofactor(const matrix & input);
    matrix systemGaussSolve(const matrix & input);
    matrix product(const matrix & in1, const matrix & in2);

    //"private"
    void product(const matrix & in2, matrix & result);
#endif





#endif // MATRIX_H
