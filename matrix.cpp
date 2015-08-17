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
//    std::for_each(this->data.begin(),
//                  this->data.end(),
//                  [](vector<double> & in){in.~vector();});
//    this->data.~vector();
}

matrix::matrix(int dim)
{
    this->resize(dim, dim);
    this->fill(0.);
}

matrix::matrix(int rows, int cols)
{
    this->resize(rows, cols);
    this->fill(0.);
}

matrix::matrix(const matrix & other)
{
    this->data = other.data;
}
matrix::matrix(const dataType & other)
{
    this->data = other;
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
matrix::matrix(std::initializer_list<vector<double>> lst)
{
    this->resize(0, 0);
    std::for_each(lst.begin(),
                  lst.end(),
                  [this](vector<double> in)
    {
        this->data.push_back(in);
    });
}



matrix matrix::operator = (const matrix & other)
{
    this->data = other.data;

    return *this;
}
matrix matrix::operator = (const dataType & other)
{
    this->data = other;

    return *this;
}
matrix matrix::operator /= (const double & other)
{
    for(int i = 0; i < this->rows(); ++i)
    {
        for(int j = 0; j < this->cols(); ++j)
        {
            this->data[i][j] /= other;
        }
    }
    return *this;
}
matrix matrix::operator *= (const double & other)
{
    for(int i = 0; i < this->rows(); ++i)
    {
        for(int j = 0; j < this->cols(); ++j)
        {
            this->data[i][j] *= other;
        }
    }
    return *this;
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
    this->data.resize(rows);
    std::for_each(data.begin(),
                  data.end(),
                  [cols](vector<double> & in)
    {
        in.resize(cols);
    });

}


void matrix::resizeRows(int rows)
{
    int cols = data[0].size();
    data.resize(rows);
    std::for_each(data.begin(),
                  data.end(),
                  [cols](vector<double> & in)
    {
        in.resize(cols);
    });
}


void matrix::resizeCols(int cols)
{
    std::for_each(data.begin(),
                  data.end(),
                  [cols](vector<double> & in)
    {
        in.resize(cols);
    });
}

int matrix::rows() const
{
   return data.size();
}


double matrix::maxVal() const
{
    double res = data[0][0];
    for(auto it = data.begin(); it < data.end(); ++it)
    {
        for(auto itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            res = max(res, *itt);
        }
    }
    return res;
}
double matrix::minVal() const
{

    double res = data[0][0];
    for(auto it = data.begin(); it < data.end(); ++it)
    {
        for(auto itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            res = min(res, *itt);
        }
    }
    return res;
}


dataType::iterator matrix::begin()
{
    return this->data.begin();
}

dataType::iterator matrix::end()
{
    return this->data.end();
}

dataType::const_iterator matrix::begin() const
{
    return this->data.begin();
}

dataType::const_iterator matrix::end() const
{
    return this->data.end();
}

vector<double> matrix::toVectorByRows() const
{
    vector<double> res;
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](vector<double> in)
    {
        std::for_each(in.begin(),
                      in.end(),
                      [&res](double inn){res.push_back(inn);});
    });
    return res;
}

vector<double> matrix::toVectorByCols() const
{
    vector<double> res;
    for(int i = 0; i < this->cols(); ++i)
    {
        for(int j = 0; j < this->rows(); ++j)
        {
            res.push_back(this->data[j][i]);
        }
    }
    return res;
}

void matrix::pop_back()
{
    this->data.pop_back();
}

void matrix::push_back(vector<double> & in)
{
    this->data.push_back(in);
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
            std::swap((*this)[j][i], (*this)[i][j]);
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

    (*this) = tempMat;

//    //4) outmat = tempMat
//    for(int i = 0; i < size; ++i) //which line to substract
//    {
//        for(int k = 0; k < size; ++k) //k = 0 because default
//        {
//        }
//    }
}

void matrix::swapCols(int i, int j)
{
    for(int k = 0; k < this->rows(); ++k)
    {
        std::swap(this->data[k][i], this->data[k][j]);
    }
}
void matrix::swapRows(int i, int j)
{
    std::swap(this->data[i], this->data[j]);
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

//    cout << "dim1 = " << dim1 << endl;
//    cout << "dim2 = " << dim2 << endl;
//    cout << "size = " << size << endl;

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



