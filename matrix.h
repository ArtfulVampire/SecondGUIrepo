#ifndef MATRIX_H
#define MATRIX_H

#include "smallFuncs.h"

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


using dataType = std::vector<std::valarray<double>>;

class matrix : public dataType
{
public:
    matrix();
    ~matrix();
    matrix(int dim);
    matrix(int rows, int cols);
    matrix(int rows, int cols, double value);

    matrix(const matrix & other);
    matrix(const dataType & other);

    matrix(matrix && other)
        :data{other.data}
    {
        other.data = dataType();
    }


    matrix(const lineType & vect1, const lineType & vect2);
    matrix(const lineType & vect, bool orientH);
    matrix(const lineType & vect, char orient);
    matrix(const lineType & vect, int rows);

    matrix(std::initializer_list<lineType> lst);

    matrix(std::initializer_list<double> lst); // diagonal

    matrix & resizeRows(int rows);
    matrix & resizeCols(int newCols);
    matrix & fill(double value);
    void print(int rows = 0, int cols = 0) const;
    int cols() const;
    int rows() const;
    double maxVal() const;
    double minVal() const;
    double maxAbsVal() const;
    double minAbsVal() const;

    double sum() const;
    dataType::iterator begin();
    dataType::iterator end();
    dataType::const_iterator begin() const;
    dataType::const_iterator end() const;
    lineType toVectorByRows() const;
    lineType toVectorByCols() const;
    lineType getCol(int i, int numCols = -1) const;
    lineType averageRow() const;
    lineType averageCol() const;
    void pop_back();
    void push_back(const lineType &in);
    void push_back(const vectType &in);

    // for compability with vector < vector<Type> >
    void clear() {this->data.clear();}
    int size() const {return data.size();}

    void resize(int rows, int cols, double val);
    void resize(int rows, int cols);
    void resize(int i) {data.resize(i);}


    lineType & operator [](int i)
    {
        return data[i];

    }
    const lineType & operator [](int i) const
    {
        return data[i];
    }

    matrix operator = (const matrix & other);
    matrix operator = (const dataType & other);

    matrix operator += (const matrix & other);
    matrix operator += (const double & val);

    matrix operator -= (const matrix & other);
    matrix operator -= (const double & val);

    matrix operator *= (const matrix & other);
    matrix operator *= (const double & val);

    matrix operator /= (const double & other);

    bool operator == (const matrix & other);

    //"static"
    static matrix transpose(const matrix & input);

    // "private"
    matrix & transpose();
    matrix & invert();
    matrix & swapCols(int i, int j);
    matrix & swapRows(int i, int j);
    matrix & zero();
    matrix & one();
    matrix & eraseRow(int i);
    matrix & eraseRows(const std::vector<int> & indices);
    matrix & random(double low, double high);
    matrix subCols(int beginCol, int endCol) const; /// submatrix
//    double det();
//    void cofactor();
//    void systemGaussSolve();

public:
    dataType data = dataType();

};

matrix operator + (const matrix & lhs, const matrix & rhs);
matrix operator + (const matrix & lhs, const double & val);
matrix operator / (const matrix & lhs, const double & val);
matrix operator * (const matrix & lhs, const matrix & rhs);
matrix operator * (const matrix & lhs, const double & val);
matrix operator - (const matrix & lhs, const matrix & rhs);
matrix operator - (const matrix & lhs, const double & val);

//template <typename matType1, typename matType2>
void matrixProduct(const matrix & in1,
                   const matrix & in2,
                   matrix & result,
                   int dim = -1,
                   int rows1 = -1,
                   int cols2 = -1);

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
