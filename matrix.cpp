#include "matrix.h"

template <typename Typ>
matrix<Typ>::matrix()
{
    this = matrix(1, 1);
    data[1][1] = 0;
}

template <typename Typ>
matrix<Typ>::matrix(double **inData, int rows, int cols)
{
    this = matrix(rows, cols);

    for(typename vector< vector<Typ> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        for(typename vector<Typ>::iterator itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            (*itt) = inData[it-data.begin()][itt - (*it).begin];
        }
    }
}

template <typename Typ>
matrix<Typ>::~matrix()
{
    for(typename vector< vector<Typ> >::iterator it = data.begin(); it < data.end(); ++it)
    {
//        ~(*it);
    }
//    ~data;
}

template <typename Typ>
matrix<Typ>::matrix(int rows, int cols)
{
    this = matrix(rows, cols);
}

template <typename Typ>
matrix<Typ>::matrix(int rows, int cols, Typ value)
{
    this = matrix(rows, cols);
    this->fill(value);
}

template <typename Typ>
void matrix<Typ>::fill(Typ value)
{
    for(typename vector< vector<Typ> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        for(typename vector<Typ>::iterator itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            (*itt) = value;
        }
    }
}

template <typename Typ>
void matrix<Typ>::resize(int rows, int cols)
{
    data.resize(rows);
    for(typename vector< vector<Typ> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}

template <typename Typ>
void matrix<Typ>::resizeRows(int rows)
{
    int cols = data[0].size();
    data.resize(rows);
    for(typename vector< vector<Typ> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}

template <typename Typ>
void matrix<Typ>::resizeCols(int cols)
{
    for(typename vector< vector<Typ> >::iterator it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}

template <typename Typ>
void matrix<Typ>::print(int rows, int cols)
{
    for(typename vector< vector<Typ> >::iterator it = data.begin(); it < (rows == 0) ? data.end() : (data.begin() + rows); ++it)
    {
        for(typename vector<Typ>::iterator itt = (*it).begin(); itt <(cols == 0) ? (*it).end() : ((*it).begin() + cols); ++itt)
        {
            cout << (*itt) << endl;
        }
    }
    cout << endl;
}

template <typename Typ>
int matrix<Typ>::rows()
{
   return data.size();
}

template <typename Typ>
int matrix<Typ>::cols()
{
    return data[0].size();
}

template <typename Typ>
ostream & operator << (ostream &os, matrix<Typ> toOut)
{
    for(typename vector< vector<Typ> >::iterator it = toOut.data.begin(); it < toOut.data.end(); ++it)
    {
        for(typename vector<Typ>::iterator itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            os << (*itt) << "  ";
        }
        os << endl;
    }
    return os;
}
