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
struct matrix
{
    matrix();
    ~matrix();
    matrix(int rows, int cols);
    matrix(int rows, int cols, double value);
    matrix(double ** inData, int rows, int cols);
    matrix(const matrix & other);
    matrix(vector <double> vec, bool orientH);
    void resize(int rows, int cols);
    void resizeRows(int rows);
    void resizeCols(int cols);
    void fill(double value);
    void print(int rows = 0, int cols = 0) const;
    int cols() const;
    int rows() const;

    vector <double> & operator [](int i)
    {
        return data[i];
    }
    const vector <double> & operator [](int i) const
    {
        return data[i];
    }

    vector < vector <double> > data;



    matrix operator = (const matrix & other);



    //"static"
    // auto type
    matrix transpose(const matrix & input);
#if 0
    matrix invert(const matrix & input);
    matrix det(const matrix & input);
    matrix cofactor(const matrix & input);
    matrix systemGaussSolve(const matrix & input);
    matrix product(const matrix & in1, const matrix & in2);

    //"private"
    void transpose(const matrix & result);
    void invert(const matrix & result);
    void det(const matrix & result);
    void cofactor(const matrix & result);
    void systemGaussSolve(const matrix & result);
    void product(const matrix & in2, matrix & result);
#endif
};




#endif // MATRIX_H
