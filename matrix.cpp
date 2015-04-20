#include "matrix.h"
#if 0
matrix::matrix()
{
    this = matrix(1, 1);
    data[1][1] = 0;
}

matrix::matrix(double **inData, int rows, int cols)
{
    this = matrix(rows, cols);

    for(typename vector< vector<double> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        for(typename vector<double>::iterator itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            (*itt) = inData[it-data.begin()][itt - (*it).begin];
        }
    }
}


matrix::~matrix()
{
    for(typename vector< vector<double> >::iterator it = data.begin(); it < data.end(); ++it)
    {
//        ~(*it);
    }
//    ~data;
}


matrix::matrix(int rows, int cols)
{
    this = matrix(rows, cols);
}


matrix::matrix(int rows, int cols, double value)
{
    this = matrix(rows, cols);
    this->fill(value);
}


void matrix::fill(double value)
{
    for(typename vector< vector<double> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        for(typename vector<double>::iterator itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            (*itt) = value;
        }
    }
}


void matrix::resize(int rows, int cols)
{
    data.resize(rows);
    for(typename vector< vector<double> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}


void matrix::resizeRows(int rows)
{
    int cols = data[0].size();
    data.resize(rows);
    for(typename vector< vector<double> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}


void matrix::resizeCols(int cols)
{
    for(typename vector< vector<double> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}


void matrix::print(int rows, int cols)
{
    for(typename vector< vector<double> >::iterator it = data.begin(); it < (rows == 0) ? data.end() : (data.begin() + rows); ++it)
    {
        for(typename vector<double>::iterator itt = (*it).begin(); itt <(cols == 0) ? (*it).end() : ((*it).begin() + cols); ++itt)
        {
            cout << (*itt) << endl;
        }
    }
    cout << endl;
}


int matrix::rows()
{
   return data.size();
}


int matrix::cols()
{
    return data[0].size();
}


ostream & operator << (ostream &os, matrix toOut)
{
    for(typename vector< vector<double> >::iterator it = toOut.data.begin(); it < toOut.data.end(); ++it)
    {
        for(typename vector<double>::iterator itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            os << (*itt) << "  ";
        }
        os << endl;
    }
    return os;
}
#endif
