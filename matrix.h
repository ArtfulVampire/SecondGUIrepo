#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cmath>
#include <QString>
#include <QDir>
#include <ios>
#include <vector>
#include <iostream>

using namespace std;

typedef vector<vector<double>> dataType;

struct matrix
{
    matrix();
    ~matrix();
    matrix(int dim);
    matrix(int rows, int cols);
    matrix(int rows, int cols, double value);
    matrix(double ** inData, int rows, int cols);
    matrix(const matrix & other);
    matrix(const dataType & other);
    matrix(vector <double> vec, bool orientH);

    matrix(vector <double> vec, char orient);
    void resize(int rows, int cols);
    void resizeRows(int rows);
    void resizeCols(int cols);
    void fill(double value);
    void print(int rows = 0, int cols = 0) const;
    int cols() const;
    int rows() const;
    double maxVal() const;
    double minVal() const;

    // for compability with vector < vector<Type> >
    int size() const {return data.size();}
    void resize(int i) {data.resize(i);}


    vector <double> & operator [](int i)
    {
        return data[i];

    }
    const vector <double> & operator [](int i) const
    {
        return data[i];
    }
    matrix operator = (const matrix & other);
    matrix operator = (const dataType & other);


    dataType data;

    //"static"
    // auto type
    matrix transpose(const matrix & input);

    // "private"
    void transpose();
    void invert();
    void swapCols(int i, int j);
    void swapRows(int i, int j);
    void zero();
    void one();
//    double det();
//    void cofactor();
//    void systemGaussSolve();

};

template <typename Typ1, typename Typ2, typename Typ3>
void matrixProduct(const Typ1 (&in1), const Typ2 (&in2), Typ3 (&result),
                   int dim = -1, int rows1 = -1, int cols2 = -1);



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
