#include "matrix.h"


using namespace std;
using namespace smallLib;

matrix::matrix()
{

}

matrix::~matrix()
{

}

matrix::matrix(int dim)
{
    this->resize(dim, dim);
    this->fill(0.);
}


matrix matrix::ident(int dim)
{
    matrix res(dim, dim, 0.);
    for(int i = 0; i < dim; ++i)
    {
        res[i][i] = 1.;
    }
    return res;
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
matrix::matrix(const lineType & vect, bool orientH)
{
    if(orientH)
    {
        this->resize(1, vect.size());
        this->data[0] = vect;
    }
    else
    {
        this->resize(vect.size(), 1);
        for(uint i = 0; i < vect.size(); ++i)
        {
            this->data[i][0] = vect[i];
        }
    }
}
matrix::matrix(const lineType & vect, char orient)
{
    if(orient == 'h' || orient == 'H' || orient == 'r' || orient == 'R')
    {
        this->resize(1, vect.size());
        this->data[0] = vect;
    }
    else if(orient == 'v' || orient == 'V' || orient == 'c' || orient == 'C')
    {
        this->resize(vect.size(), 1);
        for(uint i = 0; i < vect.size(); ++i)
        {
            this->data[i][0] = vect[i];
        }
    }
    else
    {
        matrix();
    }
}

matrix::matrix(const lineType & vect, uint inRows)
{
    if(vect.size() % inRows != 0)
    {
        cout << "not appropriate size" << endl;
        return;
    }
    int newCols = vect.size() / inRows;

    this->resize(inRows, newCols);
    for(uint i = 0; i < inRows; ++i)
    {
        std::copy(std::begin(vect) + i * newCols,
                  std::begin(vect) + (i + 1) * newCols,
                  std::begin(this->data[i]));
    }
}

matrix::matrix(const lineType & vect1, const lineType & vect2)
{
    this->data.clear();
    for(uint i = 0; i < vect1.size(); ++i)
    {
        this->data.push_back(vect1[i] * vect2);
    }
}

matrix::matrix(std::initializer_list<lineType> lst)
{
    this->resize(0, 0);
    std::for_each(lst.begin(),
                  lst.end(),
                  [this](lineType in)
    {
        this->data.push_back(in);
    });
}

/// these two are the same

matrix::matrix(const lineType & vect) // diagonal
{
    this->resize(vect.size(), vect.size());
    this->fill(0.);
    int count = 0;
    for(int item : vect)
    {
        this->data[count][count] = item;
        ++count;
    }
}

matrix::matrix(std::initializer_list<double> lst) // diagonal
{
    (*this) = matrix(lineType{lst});
    return;
    this->resize(lst.size(), lst.size());
    this->fill(0.);
    int count = 0;
    for(int item : lst)
    {
        this->data[count][count] = item;
        ++count;
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




matrix operator + (const matrix & lhs, const matrix & rhs)
{
    if(lhs.rows() != rhs.rows()
       || lhs.cols() != rhs.cols())
    {
        cout << "matrix sum failed, dimensions" << endl;
        return lhs;
    }
    matrix result(lhs.rows(), lhs.cols());
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

matrix operator + (const matrix & lhs, const double & val)
{
    matrix result;
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result.push_back(lhs[i] + val);
    }
    return result;
}

matrix matrix::operator += (const matrix & other)
{
    if(this->rows() != other.rows()
       || this->cols() != other.cols())
    {
        cout << "matrix sum failed" << endl;
        return *this;
    }
    for(uint i = 0; i < this->rows(); ++i)
    {
        (*this)[i] += other[i];
    }
    return *this;
}

matrix matrix::operator += (const double & val)
{
    for(uint i = 0; i < this->rows(); ++i)
    {
        (*this)[i] += val;
    }
    return *this;
}



matrix operator - (const matrix & lhs, const matrix & rhs)
{
    if(lhs.rows() != rhs.rows()
       || lhs.cols() != rhs.cols())
    {
        cout << "matrix sum failed, dimensions" << endl;
        return lhs;
    }
    matrix result(lhs.rows(), lhs.cols());
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}

matrix operator - (const matrix & lhs, const double & val)
{
    matrix result;
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result.push_back(lhs[i] - val);
    }
    return result;
}

matrix matrix::operator -= (const matrix & other)
{
    if(this->rows() != other.rows()
       || this->cols() != other.cols())
    {
        cout << "matrix sum failed" << endl;
        return *this;
    }
    for(uint i = 0; i < this->rows(); ++i)
    {
        (*this)[i] -= other[i];
    }
    return *this;
}

matrix matrix::operator -= (const double & val)
{
    for(uint i = 0; i < this->rows(); ++i)
    {
        (*this)[i] -= val;
    }
    return *this;
}
matrix matrix::operator -()
{
    matrix res(this->rows(), this->cols());

    for(uint i = 0; i < this->rows(); ++i)
    {
        res[i] = -this->data[i];
    }
    return res;
}



matrix operator * (const matrix & lhs, const matrix & rhs)
{
    if(lhs.cols() != rhs.rows())
    {
        cout << "matrixProduct (operator *): input matrices are not productable" << endl;
        return lhs;
    }

    const uint dim1 = lhs.rows();
    const uint dim2 = rhs.cols();


    matrix result(dim1, dim2, 0.);
#if 0
    for(uint j = 0; j < dim2; ++j)
    {
        lineType currCol = rhs.getCol(j);
        for(uint i = 0; i < dim1; ++i)
        {
            result[i][j] = std::inner_product(begin(lhs[i]),
                                              end(lhs[i]),
                                              begin(currCol),
                                              0.);
        }
    }
#elif 1





#if MATRIX_OMP
#pragma omp parallel for
    for(uint i = 0; i < dim1; ++i)
    {
        for(uint j = 0; j < lhs.cols(); ++j)
        {
            result[i] += lhs[i][j] * rhs[j];
        }
    }
#else
    /// 15-20% faster than with currCol
    for(uint i = 0; i < dim1; ++i)
    {
        for(uint j = 0; j < lhs.cols(); ++j)
        {
            result[i] += lhs[i][j] * rhs[j];
        }
    }
#endif //omp


#endif
    return result;
}

matrix operator * (const matrix & lhs, const double & val)
{
    matrix result(lhs.rows(), lhs.cols());
//#pragma omp parallel for
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] * val;
    }
    return result;
}

matrix matrix::operator *= (const double & other)
{
    for(uint i = 0; i < this->rows(); ++i)
    {
        this->data[i] *= other;
    }
    return *this;
}

matrix matrix::operator *= (const matrix & other)
{
    /// OMG
    (*this) = std::move((*this) * other);
    return (*this);
}

lineType operator * (const matrix & lhs, const lineType & rhs)
{
    if(rhs.size() != lhs.cols())
    {
        cout << "operator * (matrix, valar) invalid sizes" << endl;
        return lhs.getCol(0);
    }
    lineType res(lhs.rows());
#if MATRIX_OMP
#pragma omp parallel for
#endif
    for(uint i = 0; i < res.size(); ++i)
    {
        res[i] = prod(lhs[i], rhs);
    }
    return res;
}

lineType operator * (const lineType & lhs, const matrix & rhs)
{
    if(lhs.size() != rhs.rows())
    {
        cout << "operator * (valar, matrix) invalid sizes" << endl;
        return lhs;
    }

    lineType res = rhs[0];
    for(uint i = 1; i < lhs.size(); ++i)
    {
        res += lhs[i] * rhs[i];
    }
    exit(0);
    return res;


}

matrix operator / (const matrix & lhs, const double & val)
{
    matrix result(lhs.rows(), lhs.cols());
//#pragma omp parallel for
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] / val;
    }
    return result;
}

bool matrix::operator == (const matrix & other)
{
    if(this->rows() != other.rows())
    {
        cout << "diff rows: " << this->rows() << " " << other.rows() << endl;
        return false;
    }
    if(this->cols() != other.cols())
    {
        cout << "diff cols: " << this->cols() << " " << other.cols() << endl;
        return false;
    }

    for(uint i = 0; i < this->rows(); ++i)
    {
        for(uint j = 0; j < this->cols(); ++j)
        {
            if((*this)[i][j] != other[i][j])
            {
                cout << "diff val, (row, col) = (" << i << "," << j << ")" << endl;
                return false;
            }
        }
    }
    return true;
}

bool matrix::operator != (const matrix & other)
{
	return !(this->operator==(other));
}

bool matrix::isEmpty() const
{
	if(this->rows() == 0 || this->cols() == 0) return true;
	return false;
}


matrix matrix::operator /= (const double & other)
{
    for(uint i = 0; i < this->rows(); ++i)
    {
        this->data[i] /= other;

    }
    return *this;
}


matrix::matrix(int rows, int cols, double value)
{
    this->resize(rows, cols);
    this->fill(value);
}


matrix & matrix::fill(double value)
{
    for(auto it = data.begin(); it < data.end(); ++it)
    {
        for(auto itt = std::begin(*it); itt < std::end(*it); ++itt)
        {
            (*itt) = value;
        }
    }
    return *this;
}

matrix & matrix::vertCat(matrix && other)
{
    if(this->cols() != other.cols())
    {
        std::cout << "matrix::vertCat(): wrong dimensionality" << std::endl;
        return *this;
    }
    for(uint i = 0; i < other.rows(); ++i)
    {
        this->push_back(std::move(other[i]));
    }
    return *this;
}

matrix & matrix::random(double low, double high)
{
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
//    std::default_random_engine gen;
    std::uniform_real_distribution<double> distr(low, high);
    for(auto it = data.begin(); it < data.end(); ++it)
    {
#if MATRIX_OMP
#pragma omp parallel for
#endif
        for(auto itt = std::begin(*it); itt < std::end(*it); ++itt)
        {
            (*itt) = distr(gen);
        }
    }
    return *this;
}

matrix matrix::subCols(int beginCol, int endCol) const /// submatrix
{
    matrix res(this->rows(), endCol - beginCol);
    int row = 0;
    for(auto it = std::begin(this->data);
        it != std::end(this->data);
        ++it)
    {
        std::copy(std::begin(*it) + beginCol,
                  std::begin(*it) + endCol,
                  std::begin(res[row++]));
    }
    return res;
}

matrix matrix::subRows(const std::vector<int> & inds) const /// submatrix
{
//    matrix res(inds.size(), this->cols());
    matrix res = matrix();
//    int col = 0;
    for(int i : inds)
    {
        res.data.push_back(this->data[i]);
    }
    return res;
}

matrix matrix::subRows(const std::vector<uint> & inds) const /// submatrix
{
//    matrix res(inds.size(), this->cols());
    matrix res = matrix();
//    int col = 0;
    for(int i : inds)
    {
        res.data.push_back(this->data[i]);
    }
    return res;
}

matrix matrix::covMatCols(lineType * avRowIn) const
{
    matrix cop = *this;

    lineType * avRow;
    lineType avRowVal;
    if(avRowIn != nullptr)
    {
        *avRowIn = cop.averageRow();
        avRow = avRowIn;
    }
    else
    {
        avRowVal =  cop.averageRow();
        avRow = &avRowVal;
    }

    for(uint j = 0; j < cop.rows(); ++j)
    {
        cop.data[j] -= *avRow;
    }

    matrix res = matrix::transpose(cop) * cop;
    return res;
}

void matrix::resize(int rows, int cols, double val)
{
    this->resize(rows, cols);
    this->fill(val);
}

void matrix::resize(int newRows, int newCols)
{
    this->data.resize(newRows);
    std::for_each(data.begin(),
                  data.end(),
                  [newCols](lineType & in)
    {
        resizeValar(in, newCols);
    });

}

void matrix::resize(int i)
{
	this->data.resize(i);
}


matrix & matrix::resizeRows(int newRows)
{
    int cols = this->cols();
    int oldRows = this->rows();
    data.resize(newRows);
    if(oldRows < newRows)
    {
        std::for_each(data.begin() + oldRows,
                      data.end(),
                      [cols](lineType & in)
        {
            resizeValar(in, cols);
        });
    }
    return *this;
}


matrix & matrix::resizeCols(int newCols)
{
    std::for_each(data.begin(),
                  data.end(),
                  [newCols](lineType & in)
    {
        resizeValar(in, newCols);
    });
    return *this;
}

uint matrix::rows() const
{
   return data.size();
}


double matrix::maxVal() const
{
    double res = data[0][0];
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](const lineType & in)
    {
        res = max(res, in.max());
    });
    return res;
}
double matrix::minVal() const
{
    double res = data[0][0];
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](const lineType & in)
    {
        res = min(res, in.min());
    });
    return res;
}

double matrix::maxAbsVal() const
{
    double res = data[0][0];
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](const lineType & in)
    {
        res = max(res, abs(in).max());
    });
    return res;
}
double matrix::minAbsVal() const
{
    double res = data[0][0];
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](const lineType & in)
    {
        res = min(res, abs(in).min());
    });
    return res;
}

lineType matrix::maxOfRows() const
{
	lineType res(this->rows(), 0);
	for(int i = 0; i < res.size(); ++i)
	{
		res[i] = this->data[i].max();
	}
	return res;
}

lineType matrix::maxOfCols() const
{
	lineType res(this->cols(), 0);
	for(int i = 0; i < res.size(); ++i)
	{
		lineType t = this->getCol(i);
		res[i] = t.max();
	}
	return res;
}

double matrix::sum() const
{
    double res = 0.;
    std::for_each(this->data.begin(),
                  this->data.end(),
                  [&res](const lineType & in)
    {
       res += in.sum();
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

lineType matrix::toVectorByRows() const
{
    lineType res(this->rows() * this->cols());
    for(uint i = 0; i < this->rows(); ++i)
    {
        std::copy(std::begin(this->data[i]),
                  std::end(this->data[i]),
                  std::begin(res) + this->cols() * i);
    }
    return res;
}

lineType matrix::toVectorByCols() const
{
    lineType res(this->rows() * this->cols());
    int count = 0;
    for(uint i = 0; i < this->cols(); ++i)
    {
        for(uint j = 0; j < this->rows(); ++j)
        {
            res[count++] = this->data[j][i];
        }
    }
    return res;
}

lineType matrix::sigmaOfCols() const
{
    lineType res(this->cols());
#if 1
    for(uint i = 0; i < this->cols(); ++i)
    {
        lineType W = this->getCol(i);
        res[i] = smallLib::sigma(W);
    }
#else

    lineType avRow = this->averageRow();

    matrix M = *this;
    for(std::valarray<double> & row : M.data)
    {
        row -= avRow;
        row *= row;
        res += row;
    }
    res /= this->rows();
    res = sqrt(res);
#endif

    return res;
}

lineType matrix::averageRow() const
{
    lineType res(0., this->cols());
    for(uint i = 0; i < this->rows(); ++i)
    {
        res += this->data[i];
    }
    res /= this->rows();
    return res;
}

lineType matrix::averageCol() const
{
    lineType res(this->rows());
    for(uint i = 0; i < this->rows(); ++i)
    {
        res[i] = this->data[i].sum() / this->data[i].size();
    }
    return res;
}

lineType matrix::getCol(uint i, uint numCols) const
{
    if(numCols == 0) numCols = this->rows();
    lineType res(numCols);
    for(uint j = 0; j < numCols; ++j)
    {
        res[j] = this->data[j][i];
    }
    return res;
}

void matrix::pop_back()
{
    this->data.pop_back();
}

void matrix::print(uint rows, uint cols) const
{
    if(rows == 0) rows = this->rows();
    if(cols == 0) cols = this->cols();

    for(uint i = 0; i < rows; ++i)
    {
        for(uint j = 0; j < cols; ++j)
        {
            cout << doubleRound(data[i][j], 3) << "\t";
        }
        cout << endl;
    }
//    cout << endl;
}

void matrix::push_back(const lineType & in)
{
    this->data.push_back(in);
}

void matrix::push_back(const vectType & in)
{
    lineType temp{in.data(), in.size()};
    this->data.push_back(temp);
}

uint matrix::cols() const
{
    return data[0].size();
}

matrix matrix::transpose(const matrix &input)
{
#if 1
    matrix res(input.cols(), input.rows());
    for(uint i = 0; i < input.rows(); ++i)
    {
        for(uint j = 0; j < input.cols(); ++j)
        {
            res[j][i] = input[i][j];
        }
    }
    return res;
#else
    matrix res;
    for(uint i = 0; i < input.cols(); ++i)
    {
        res.push_back(input.getCol(i));
    }
    return res;
#endif
}
matrix & matrix::transpose()
{
#if 1
    (*this) = matrix::transpose(*this);
#else
    int oldCols = this->cols();
    int oldRows = this->rows();
    this->resize(max(oldRows, oldCols),
                 max(oldRows, oldCols)); // make square

    for(uint i = 0; i < this->rows(); ++i)
    {
        for(int j = i + 1; j < this->cols(); ++j)
        {
            std::swap(this->data[j][i], this->data[i][j]);
        }
    }
    this->resize(oldCols,
                 oldRows);
#endif
    return *this;
}

double matrix::trace() const
{
    if(this->rows() != this->cols()) return 0.;
    double res = 0.;
    for(uint i = 0; i < this->rows(); ++i)
    {
        res += (*this)[i][i];
    }
    return res;
}

matrix & matrix::invert(double * det)
{
    if(this->rows() != this->cols())
    {
        cout << "matrix::invert: matrix is not square" << endl;
//        exit(1);
        return *this;
    }

    const uint size = this->rows();
    matrix initMat(size, size);
    initMat = (*this);

    matrix tempMat(size, size, 0.);
    for(uint i = 0; i < size; ++i)
    {
        tempMat[i][i] = 1.;
    }
    double coeff;

//    cout << "start first cycle" << endl;

    //1) make higher-triangular
    for(uint i = 0; i < size - 1; ++i) //which line to substract
    {
        for(uint j = i + 1; j < size; ++j) //FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i]; // coefficient

            //row[j] -= coeff * row[i] for both (temp & init) matrices
            initMat[j] -= initMat[i] * coeff;
            tempMat[j] -= tempMat[i] * coeff;
        }
    }

//    cout << "start second cycle" << endl;
    //2) make diagonal
    for(int i = size - 1; i > 0; --i) //which line to substract (bottom -> up)
    {
        for(int j = i - 1; j >= 0; --j) //FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i];

            //row[j] -= coeff * row[i] for both matrices
            initMat[j] -= initMat[i] * coeff;
            tempMat[j] -= tempMat[i] * coeff;
        }
    }
//    cout << "do the rest" << endl;

    if(det != nullptr)
    {
        (*det) = 1.;
        for(uint i = 0; i < size; ++i)
        {
            (*det) *= initMat[i][i];
        }
    }

    //3) divide on diagonal elements
    for(uint i = 0; i < size; ++i) //which line to divide
    {
        tempMat[i] /= initMat[i][i];
    }

    (*this) = tempMat;
    return *this;
}

matrix & matrix::swapCols(uint i, uint j)
{
    for(uint k = 0; k < this->rows(); ++k)
    {
        std::swap(this->data[k][i], this->data[k][j]);
    }
    return *this;
}
matrix & matrix::swapRows(uint i, uint j)
{
    std::swap(this->data[i], this->data[j]);
    return *this;
}

matrix & matrix::eraseRow(uint i)
{
    if(i < this->rows())
    {
        this->data.erase(data.begin() + i);
    }
    return *this;
}


matrix & matrix::eraseCol(uint j)
{
    if(j < this->cols())
    {
        for(lineType & each : this->data)
        {
            each = eraseValar(each, j);
        }
    }
    return *this;
}


/// looks like okay
matrix & matrix::eraseRows(const std::vector<uint> & indices)
{
    eraseItems(this->data, indices);
    return *this;
}

//template <typename matType1, typename matType2>
void matrixProduct(const matrix & in1,
                   const matrix & in2,
                   matrix & result,
                   uint dim,
                   uint rows1,
                   uint cols2)
{
    uint dim1 = 0;
    uint dim2 = 0;
    uint Size = 0;

    if(rows1 != 0)
    {
        dim1 = rows1;
    }
    else
    {
        dim1 = in1.rows();
    }

    if(cols2 != 0)
    {
        dim2 = cols2;
    }
    else
    {
        dim2 = in2.cols();
    }

    if(dim != 0)
    {
        Size = dim;
    }
    else if(in1.cols() != in2.rows())
    {
        cout << "matrixProduct: input matrices are not productable" << endl;
        result = matrix();
        return;
    }
    else
    {
        Size = in1.cols();
    }

//    result.resize(max(dim1, result.rows()),
//                  max(dim2, result.cols()));
    result.resize(dim1, dim2);


    lineType temp{};
    for(uint j = 0; j < dim2; ++j)
    {
        temp = in2.getCol(j, Size); // size for prod()
        for(uint i = 0; i < dim1; ++i)
        {
            result[i][j] = std::inner_product(std::begin(temp),
                                              std::end(temp),
                                              std::begin(in1[i]),
                                              0.);
        }
    }
}

lineType matrix::matrixSystemSolveGauss(const lineType & inVec) const
{
    const matrix & inMat = (*this);
    const uint size = inMat.rows();

    matrix initMat(inMat);
    initMat.invert();

    lineType res(size);
    res = initMat * inVec;
    return res;
}
//template
//void matrixProduct(const matrix & in1,
//                   const matrix & in2,
//                   matrix & result,
//                   int dim,
//                   int rows1,
//                   int cols2);
//template
//void matrixProduct(const matrix & in1,
//                   const dataType & in2,
//                   matrix & result,
//                   int dim,
//                   int rows1,
//                   int cols2);
//template
//void matrixProduct(const dataType & in1,
//                   const matrix & in2,
//                   matrix & result,
//                   int dim,
//                   int rows1,
//                   int cols2);
//template
//void matrixProduct(const dataType & in1,
//                   const dataType & in2,
//                   matrix & result,
//                   int dim,
//                   int rows1,
//                   int cols2);

//void matrixProduct(const lineType & in1,
//                   const matrix &in2,
//                   matrix & result,
//                   int dim,
//                   int rows1,
//                   int cols2)
//{
//    matrixProduct(matrix(in1, 'h'),
//                  in2,
//                  result,
//                  dim,
//                  rows1,
//                  cols2);
//}

//void matrixProduct(const matrix &in1,
//                   const lineType &in2,
//                   matrix & result,
//                   int dim,
//                   int rows1,
//                   int cols2)
//{
//    matrixProduct(in1,
//                  matrix(in2, 'v'),
//                  result,
//                  dim,
//                  rows1,
//                  cols2);
//}



