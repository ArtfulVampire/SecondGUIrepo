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
#if 0
struct matrix
{
    matrix();
    ~matrix();
    matrix(int rows, int cols);
    matrix(int rows, int cols, double value);
    matrix(double ** inData, int rows, int cols);
    void resize(int rows, int cols);
    void resizeRows(int rows);
    void resizeCols(int cols);
    void fill(double value);
    void print(int rows = 0, int cols = 0);
    int cols();
    int rows();

    vector <double> & operator [](int i)
    {
        return data[i];
    }

    vector < vector <double> > data;
    friend ostream & operator << (ostream &os, matrix toOut);
};
#endif



#endif // MATRIX_H
