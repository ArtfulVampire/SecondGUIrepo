#include "matrix.h"

matrix::matrix()
{
}

matrix::matrix(double **inData, int rows, int cols)
{
    this->resize(rows, cols);
    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            this->data[i][j] = inData[i][j];
        }
    }
}


matrix::~matrix()
{
}


matrix::matrix(int rows, int cols)
{
    this->resize(rows, cols);
}

matrix::matrix(const matrix & other)
{
    this->data = other.data;
}
matrix::matrix(vector <double> vec, bool orientH)
{
    if(orientH)
    {
        this->resize(1, vec.size());
        this->data[0] = vec;
    }
    else
    {
        this->resize(vec.size(), 1);
        for(int i = 0; i < vec.size(); ++i)
        {
            this->data[i][0] = vec[i];
        }
    }
}


matrix matrix::operator = (const matrix & other)
{
    this->data = other.data;
}


matrix::matrix(int rows, int cols, double value)
{
    this->resize(rows, cols);
    this->fill(value);
}


void matrix::fill(double value)
{
    for(auto it = data.begin(); it < data.end(); ++it)
    {
        for(auto itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            (*itt) = value;
        }
    }
}


void matrix::resize(int rows, int cols)
{
    data.resize(rows);
    for(auto it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}


void matrix::resizeRows(int rows)
{
    int cols = data[0].size();
    data.resize(rows);
    for(auto it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}


void matrix::resizeCols(int cols)
{
    for(auto it = data.begin(); it < data.end(); ++it)
    {
        (*it).resize(cols);
    }
}

int matrix::rows() const
{
   return data.size();
}


int matrix::cols() const
{
    return data[0].size();
}

matrix matrix::transpose(const matrix &input)
{
    matrix res;
    res.resize(input.cols(), input.rows());
    for(int i = 0; i < input.rows(); ++i)
    {
        for(int j = 0; j < input.cols(); ++j)
        {
            res[j][i] = input[i][j];
        }
    }
    return res;
}





