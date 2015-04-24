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
matrix::matrix(vector <double> vec, char orient)
{
    if(orient == 'h' || orient == 'H')
    {
        this->resize(1, vec.size());
        this->data[0] = vec;
    }
    else if(orient == 'v' || orient == 'V')
    {
        this->resize(vec.size(), 1);
        for(int i = 0; i < vec.size(); ++i)
        {
            this->data[i][0] = vec[i];
        }
    }
    else
    {
        matrix();
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
void matrix::transpose()
{
    int oldCols = this->cols();
    int oldRows = this->rows();
    this->resize(max(oldRows, oldCols),
                 max(oldRows, oldCols)
                 );
    for(int i = 0; i < this->rows(); ++i)
    {
        for(int j = 0; j < this->cols(); ++j)
        {
            swap((*this)[j][i], (*this)[i][j]);
        }
    }
    this->resize(oldCols,
                 oldRows
                 );
}
void matrix::invert()
{
    if(this->rows() != this->cols())
    {
        cout << "matrix::invert: matrix is not square" << endl;
        return;
    }

    int size = this->rows();
    matrix initMat(size, size);
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            initMat = (*this);
        }
    }

    matrix tempMat(size, size);
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            tempMat[i][j] = (j==i);
        }
    }
    double coeff;

    //1) make higher-triangular
    for(int i = 0; i < size - 1; ++i) //which line to substract
    {
        for(int j = i+1; j < size; ++j) //FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i]; // coefficient

            //row[j] -= coeff * row[i] for both (temp & init) matrices
            std::transform(initMat[j].begin() + i,
                           initMat[j].end(),
                           initMat[i].begin() + i,
                           initMat[j].begin() + i,
                           [&](double A, double B){return A - B * coeff;}
            );

            std::transform(tempMat[j].begin(),
                           tempMat[j].end(),
                           tempMat[i].begin(),
                           tempMat[j].begin(),
                           [&](double A, double B){return A - B * coeff;}
            );
        }
    }

    //2) make diagonal
    for(int i = size - 1; i > 0; --i) //which line to substract (bottom -> up)
    {
        for(int j = i - 1; j >= 0; --j) //FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i];


            //row[j] -= coeff * row[i] for both matrices
            std::transform(initMat[j].begin() + i,
                           initMat[j].end(),
                           initMat[i].begin() + i,
                           initMat[j].begin() + i,
                           [&](double A, double B){return A - B * coeff;}
            );

            std::transform(tempMat[j].begin(),
                           tempMat[j].end(),
                           tempMat[i].begin(),
                           tempMat[j].begin(),
                           [&](double A, double B){return A - B * coeff;}
            );
        }
    }

    //3) divide on diagonal elements
    for(int i = 0; i < size; ++i) //which line to substract
    {
        std::transform(tempMat[i].begin(),
                       tempMat[i].end(),
                       tempMat[i].begin(),
                       [&](double A){return A / initMat[i][i];}
        );
    }

    //4) outmat = tempMat
    for(int i = 0; i < size; ++i) //which line to substract
    {
        for(int k = 0; k < size; ++k) //k = 0 because default
        {
            (*this) = tempMat;
        }
    }
}

template <typename Typ1, typename Typ2, typename Typ3>
void matrixProduct(const Typ1 (&in1), const Typ2 (&in2), Typ3 (&result),
                   int dim, int rows1, int cols2)
{
    int dim1 = 0;
    int dim2 = 0;
    int size = 0;

    dim1 = in1.size();
    dim2 = in2[0].size();

    if(rows1 != -1) {dim1 = rows1;}
    if(cols2 != -1) {dim2 = cols2;}

    // count size
    if(dim != -1) {size = dim;}
    else if(in1[0].size() != in2.size())
    {
        cout << "matrixProduct: input matrices are not productable" << endl;
        result = Typ3();
        return;
    }
    else
    {
        size = in1[0].size();
    }

    double helpDouble = 0.;
    for(int i = 0; i < dim1; ++i)
    {
        for(int j = 0; j < dim2; ++j)
        {
            helpDouble = 0.;
            for(int k = 0; k < size; ++k)
            {
                helpDouble += in1[i][k] * in2[k][j];
            }
            result[i][j] = helpDouble;
        }
    }
}

template void matrixProduct(const matrix &in1, const matrix &in2, matrix & result, int dim, int rows1, int cols2);
template void matrixProduct(const matrix &in1, const matrix &in2, dataType & result, int dim, int rows1, int cols2);
template void matrixProduct(const matrix &in1, const dataType & in2, matrix & result, int dim, int rows1, int cols2);
template void matrixProduct(const dataType &in1, const dataType & in2, matrix & result, int dim, int rows1, int cols2);
template void matrixProduct(const dataType &in1, const matrix & in2, matrix & result, int dim, int rows1, int cols2);



