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
matrix::matrix(vec vect, bool orientH)
{
    if(orientH)
    {
        this->resize(1, vect.size());
        this->data[0] = vect;
    }
    else
    {
        this->resize(vect.size(), 1);
        for(int i = 0; i < vect.size(); ++i)
        {
            this->data[i][0] = vect[i];
        }
    }
}
matrix::matrix(vec vect, char orient)
{
    if(orient == 'h' || orient == 'H')
    {
        this->resize(1, vect.size());
        this->data[0] = vect;
    }
    else if(orient == 'v' || orient == 'V')
    {
        this->resize(vect.size(), 1);
        for(int i = 0; i < vect.size(); ++i)
        {
            this->data[i][0] = vect[i];
        }
    }
    else
    {
        matrix();
    }
}

matrix::matrix(vec vect, int inRows)
{
    if(vect.size() % inRows != 0)
    {
        cout << "not appropriate size" << endl;
        return;
    }
    int newCols = vect.size() / inRows;

    this->resize(inRows, newCols);
    for(int i = 0; i < inRows; ++i)
    {
        std::copy(vect.begin() + i * newCols,
                  vect.begin() + (i + 1) * newCols,
                  data[i].begin());
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

matrix::matrix(std::initializer_list<double> lst) // diagonal
{
    this->resize(lst.size(), lst.size());
    vec tmp;
    std::for_each(lst.begin(),
                  lst.end(),
                  [&tmp](double in)
    {
        tmp.push_back(in);
    });

    vec tempVec(lst.size(), 0.);
    for(int i = 0; i < lst.size(); ++i)
    {
        tempVec[i] = tmp[i];
        this->data.push_back(tempVec);
        tempVec[i] = 0.;
    }
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
matrix matrix::operator * (const matrix & other)
{
    if(this->cols() != other.rows())
    {
        cout << "matrixProduct (operator *): input matrices are not productable" << endl;
        return (*this);
    }

    const int size = this->cols();
    const int dim1 = this->rows();
    const int dim2 = other.cols();


    matrix result(dim1, dim2, 0.);

    double helpDouble = 0.;
    for(int i = 0; i < dim1; ++i)
    {
        for(int j = 0; j < dim2; ++j)
        {
            helpDouble = 0.;
            for(int k = 0; k < size; ++k)
            {
                helpDouble += (*this)[i][k] * other[k][j];
            }
            result[i][j] = helpDouble;
        }
    }
    return result;
}

matrix matrix::operator *= (const matrix & other)
{
    if(this->cols() != other.rows())
    {
        cout << "matrixProduct (operator *): input matrices are not productable" << endl;
        return (*this);
    }

    const int size = this->cols();
    const int dim1 = this->rows();
    const int dim2 = other.cols();


    matrix result(dim1, dim2, 0.);

    double helpDouble = 0.;
    for(int i = 0; i < dim1; ++i)
    {
        for(int j = 0; j < dim2; ++j)
        {
            helpDouble = 0.;
            for(int k = 0; k < size; ++k)
            {
                helpDouble += (*this)[i][k] * other[k][j];
            }
            result[i][j] = helpDouble;
        }
    }
    return result;
}

//dataType * matrix::operator &()
//{
//    return &data;
//}

//matrix * matrix::operator &()
//{
//    return this;
//}


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
    vector<double> res; // = data[0][0];
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](const vector<double> & in)
    {
//        res = fmax(res, *(std::max_element(in.begin(), in.end())));
        res.push_back( *(std::max_element(in.begin(), in.end())) );
    });
//    return res;
    return *(std::max_element(res.begin(), res.end()));
}
double matrix::minVal() const
{
    double res = data[0][0];
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](const vector<double> & in)
    {
        res = fmin(res, *(std::min_element(in.begin(), in.end())));
    });
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


vector<double> matrix::averageRow() const
{
    vector<double> res(this->cols(), 0);
    std::for_each(this->begin(),
                  this->end(),
                  [&res](const vector<double> & in)
    {
        std::transform(in.begin(),
                       in.end(),
                       res.begin(),
                       res.begin(),
                       [](const double & in1, const double & in2)
        {
            return in1 + in2;
        });
    });

    std::for_each(res.begin(),
                  res.end(),
                  [this](double & in)
    {
        in /= this->rows();
    });
    return res;
}

vector<double> matrix::averageCol() const
{

    vector<double> res(this->rows(), 0);
    for(int i = 0; i < this->cols(); ++i)
    {
        vector<double> col = this->getCol(i);
        std::transform(col.begin(),
                       col.end(),
                       res.begin(),
                       res.begin(),
                       [](const double & in1, const double & in2)
        {
            return in1 + in2;
        });

    }
    std::for_each(res.begin(),
                  res.end(),
                  [this](double & in)
    {
        in /= this->cols();
    });

    return res;
}

vector<double> matrix::getCol(int i) const
{
    vector<double> res;
    for(int j = 0; j < this->rows(); ++j)
    {
        res.push_back((*this)[j][i]);
    }
    return res;
}

void matrix::pop_back()
{
    this->data.pop_back();
}

void matrix::print(int rows, int cols) const
{
    if(rows == 0) rows = this->rows();
    if(cols == 0) cols = this->cols();

    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            cout << data[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

void matrix::push_back(const vector<double> & in)
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
        for(int j = i + 1; j < this->cols(); ++j)
        {
            std::swap(this->data[j][i], this->data[i][j]);
        }
    }
    this->resize(oldCols,
                 oldRows);
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

void matrix::eraseRow(int i)
{
    if(i > this->rows()) return;
    this->data.erase(data.begin() + i);
}

void matrixProduct(const matrix & in1,
                   const matrix & in2,
                   matrix & result,
                   int dim,
                   int rows1,
                   int cols2)
{
    int dim1 = 0;
    int dim2 = 0;
    int size = 0;



    if(rows1 != -1)
    {
        dim1 = rows1;
    }
    else
    {
        dim1 = in1.rows();
    }

    if(cols2 != -1)
    {
        dim2 = cols2;
    }
    else
    {
        dim2 = in2.cols();
    }

    if(dim != -1)
    {
        size = dim;
    }
    else if(in1.cols() != in2.rows())
    {
        cout << "matrixProduct: input matrices are not productable" << endl;
        result = matrix();
        return;
    }
    else
    {
        size = in1.cols();
    }

    result.resize(max(dim1, result.rows()),
                  max(dim2, result.cols()));

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

void matrixProduct(const vec &in1,
                   const matrix &in2,
                   matrix & result,
                   int dim,
                   int rows1,
                   int cols2)
{
    matrixProduct(matrix(in1, 'h'),
                  in2,
                  result,
                  dim,
                  rows1,
                  cols2);
}

void matrixProduct(const matrix &in1,
                   const vec &in2,
                   matrix & result,
                   int dim,
                   int rows1,
                   int cols2)
{
    matrixProduct(in1,
                  matrix(in2, 'v'),
                  result,
                  dim,
                  rows1,
                  cols2);
}



