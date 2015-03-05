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

template <typename Typ = double> class matrix
{
public:
    matrix();
    ~matrix();
    matrix(int rows, int cols);
    matrix(int rows, int cols, Typ value);
    matrix(double ** inData, int rows, int cols);
    void resize(int rows, int cols);
    void resizeRows(int rows);
    void resizeCols(int cols);
    void fill(Typ value);
    void print(int rows = 0, int cols = 0);
    int cols();
    int rows();


private:
    vector < vector <Typ> > data;


};

#endif // MATRIX_H
