#include "matrix.h"

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

matrix vertCat(const matrix & upper, const matrix & lower)
{
	const int maxLen = std::max(upper.cols(), lower.cols());
	matrix res;
	for(const matrix & mat : {upper, lower})
	{
		for(const auto & row : mat.myData)
		{
			res.myData.push_back(row);
		}
	}
	res.resizeCols(maxLen);
	return res;
}

matrix horzCat(const matrix & left, const matrix & right)
{
	if(left.rows() != right.rows())
	{
		std::cout << "matrix::horzCat: inequal rows = "
				  << left.rows() << " " << right.rows() << std::endl;
		return left;
	}
	const int sumLen = left.cols() + right.cols();
	matrix res = left;
	res.resizeCols(sumLen);

	int i = 0;
	for(const auto & row : right.myData)
	{
		std::copy(std::begin(row),
				  std::end(row),
				  std::begin(res[i]) + left.cols());
		++i;
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
	this->myData = other.myData;
}


matrix::matrix(const matrixType & other)
{
	this->myData = other;
}
matrix::matrix(const std::valarray<double> & vect, bool orientH)
{
    if(orientH)
    {
        this->resize(1, vect.size());
		this->myData[0] = vect;
    }
    else
    {
        this->resize(vect.size(), 1);
        for(uint i = 0; i < vect.size(); ++i)
        {
			this->myData[i][0] = vect[i];
        }
    }
}
matrix::matrix(const std::valarray<double> & vect, char orient)
{
    if(orient == 'h' || orient == 'H' || orient == 'r' || orient == 'R')
    {
        this->resize(1, vect.size());
		this->myData[0] = vect;
    }
    else if(orient == 'v' || orient == 'V' || orient == 'c' || orient == 'C')
    {
        this->resize(vect.size(), 1);
        for(uint i = 0; i < vect.size(); ++i)
        {
			this->myData[i][0] = vect[i];
        }
    }
    else
    {
        matrix();
    }
}

matrix::matrix(const std::valarray<double> & vect, uint inRows)
{
    if(vect.size() % inRows != 0)
    {
		std::cout << "not appropriate size" << std::endl;
        return;
    }
    int newCols = vect.size() / inRows;

    this->resize(inRows, newCols);
    for(uint i = 0; i < inRows; ++i)
    {
        std::copy(std::begin(vect) + i * newCols,
                  std::begin(vect) + (i + 1) * newCols,
				  std::begin(this->myData[i]));
    }
}

matrix::matrix(const std::valarray<double> & vect1, const std::valarray<double> & vect2)
{
	this->myData.clear();
    for(uint i = 0; i < vect1.size(); ++i)
    {
		this->myData.push_back(vect1[i] * vect2);
    }
}

matrix::matrix(std::initializer_list<std::valarray<double>> lst)
{
    this->resize(0, 0);
    std::for_each(lst.begin(),
                  lst.end(),
                  [this](std::valarray<double> in)
    {
		this->myData.push_back(in);
    });
}

/// these two are the same

matrix::matrix(const std::valarray<double> & vect) // diagonal
{
    this->resize(vect.size(), vect.size());
    this->fill(0.);
    int count = 0;
    for(int item : vect)
    {
		this->myData[count][count] = item;
        ++count;
    }
}

matrix::matrix(std::initializer_list<double> lst) // diagonal
{
    (*this) = matrix(std::valarray<double>{lst});
    return;
    this->resize(lst.size(), lst.size());
    this->fill(0.);
    int count = 0;
    for(int item : lst)
    {
		this->myData[count][count] = item;
        ++count;
    }
}






matrix matrix::operator = (const matrix & other)
{
	this->myData = other.myData;
    return *this;
}
matrix matrix::operator = (const matrixType & other)
{
	this->myData = other;

    return *this;
}




matrix operator + (const matrix & lhs, const matrix & rhs)
{
    if(lhs.rows() != rhs.rows()
       || lhs.cols() != rhs.cols())
    {
		std::cout << "matrix sum failed, dimensions" << std::endl;
        return lhs;
    }
    matrix result(lhs.rows(), lhs.cols());
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

matrix operator + (const matrix & lhs, double val)
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
		std::cout << "matrix sum failed" << std::endl;
        return *this;
    }
    for(uint i = 0; i < this->rows(); ++i)
    {
        (*this)[i] += other[i];
    }
    return *this;
}

matrix matrix::operator += (double val)
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
		std::cout << "matrix sum failed, dimensions" << std::endl;
        return lhs;
    }
    matrix result(lhs.rows(), lhs.cols());
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}

matrix operator - (const matrix & lhs, double val)
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
		std::cout << "matrix sum failed" << std::endl;
        return *this;
    }
    for(uint i = 0; i < this->rows(); ++i)
    {
        (*this)[i] -= other[i];
    }
    return *this;
}

matrix matrix::operator -= (double val)
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
		res[i] = -this->myData[i];
    }
    return res;
}



matrix operator * (const matrix & lhs, const matrix & rhs)
{
    if(lhs.cols() != rhs.rows())
    {
		std::cout << "matrixProduct (operator *): input matrices are not productable" << std::endl;
        return lhs;
    }

    const uint dim1 = lhs.rows();
    const uint dim2 = rhs.cols();


    matrix result(dim1, dim2, 0.);
#if 0
    for(uint j = 0; j < dim2; ++j)
    {
        std::valarray<double> currCol = rhs.getCol(j);
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

matrix operator * (const matrix & lhs, double val)
{
    matrix result(lhs.rows(), lhs.cols());
//#pragma omp parallel for
    for(uint i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] * val;
    }
    return result;
}

matrix matrix::operator *= (double other)
{
    for(uint i = 0; i < this->rows(); ++i)
    {
		this->myData[i] *= other;
    }
    return *this;
}

matrix matrix::operator *= (const matrix & other)
{
    /// OMG
    (*this) = std::move((*this) * other);
    return (*this);
}

std::valarray<double> operator * (const matrix & lhs, const std::valarray<double> & rhs)
{
    if(rhs.size() != lhs.cols())
    {
		std::cout << "operator * (matrix, valar) invalid sizes" << std::endl;
        return lhs.getCol(0);
    }
    std::valarray<double> res(lhs.rows());
#if MATRIX_OMP
#pragma omp parallel for
#endif
    for(uint i = 0; i < res.size(); ++i)
    {
		res[i] = smLib::prod(lhs[i], rhs);
    }
    return res;
}

std::valarray<double> operator * (const std::valarray<double> & lhs, const matrix & rhs)
{
    if(lhs.size() != rhs.rows())
    {
		std::cout << "operator * (valar, matrix) invalid sizes" << std::endl;
        return lhs;
    }

    std::valarray<double> res = rhs[0];
    for(uint i = 1; i < lhs.size(); ++i)
    {
        res += lhs[i] * rhs[i];
    }
    exit(0);
    return res;


}

matrix operator / (const matrix & lhs, double val)
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
		std::cout << "diff rows: " << this->rows() << " " << other.rows() << std::endl;
        return false;
    }
    if(this->cols() != other.cols())
    {
		std::cout << "diff cols: " << this->cols() << " " << other.cols() << std::endl;
        return false;
    }

    for(uint i = 0; i < this->rows(); ++i)
    {
        for(uint j = 0; j < this->cols(); ++j)
        {
            if((*this)[i][j] != other[i][j])
            {
				std::cout << "diff val, (row, col) = (" << i << "," << j << ")" << std::endl;
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


matrix matrix::operator /= (double other)
{
    for(uint i = 0; i < this->rows(); ++i)
    {
		this->myData[i] /= other;

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
	for(auto it = myData.begin(); it < myData.end(); ++it)
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

matrix & matrix::horzCat(const matrix & other)
{
	if(this->rows() != other.rows())
	{
		std::cout << "matrix::horzCat(): wrong dimensionality" << std::endl;
		return *this;
	}
	const int startCopy = this->cols();
	this->resizeCols(this->cols() + other.cols());

	for(uint i = 0; i < this->rows(); ++i)
	{
		std::copy(std::begin(other[i]),
				  std::end(other[i]),
				  std::begin(this->myData[i]) + startCopy);
	}
	return *this;

}


matrix & matrix::random(double low, double high)
{
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
//    std::default_random_engine gen;
    std::uniform_real_distribution<double> distr(low, high);
	for(auto it = myData.begin(); it < myData.end(); ++it)
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

matrix & matrix::pop_front(uint numOfCols)
{
	for(std::valarray<double> & row : myData)
	{
		row = smLib::pop_front_valar(row, numOfCols);
	}
	return *this;
}

matrix matrix::subCols(int beginCol, int endCol) const /// submatrix
{
    matrix res(this->rows(), endCol - beginCol);
    int row = 0;
	for(auto it = std::begin(this->myData);
		it != std::end(this->myData);
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
		res.myData.push_back(this->myData[i]);
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
		res.myData.push_back(this->myData[i]);
    }
    return res;
}

matrix matrix::covMatCols(std::valarray<double> * avRowIn) const
{
    matrix cop = *this;

    std::valarray<double> * avRow;
    std::valarray<double> avRowVal;
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
		cop.myData[j] -= *avRow;
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
	this->myData.resize(newRows);
	std::for_each(myData.begin(),
				  myData.end(),
                  [newCols](std::valarray<double> & in)
    {
		smLib::resizeValar(in, newCols);
    });

}

void matrix::resize(int i)
{
	this->myData.resize(i);
}


matrix & matrix::resizeRows(int newRows)
{
    int cols = this->cols();
    int oldRows = this->rows();
	myData.resize(newRows);
    if(oldRows < newRows)
    {
		std::for_each(myData.begin() + oldRows,
					  myData.end(),
                      [cols](std::valarray<double> & in)
        {
			smLib::resizeValar(in, cols);
        });
    }
    return *this;
}


matrix & matrix::resizeCols(int newCols)
{
	std::for_each(myData.begin(),
				  myData.end(),
                  [newCols](std::valarray<double> & in)
    {
		smLib::resizeValar(in, newCols);
    });
    return *this;
}

uint matrix::rows() const
{
   return myData.size();
}


double matrix::maxVal() const
{
	double res = myData[0][0];
	std::for_each(this->myData.begin(),
				  this->myData.end(),
                  [&res](const std::valarray<double> & in)
    {
		res = std::max(res, in.max());
    });
    return res;
}
double matrix::minVal() const
{
	double res = myData[0][0];
	std::for_each(this->myData.begin(),
				  this->myData.end(),
                  [&res](const std::valarray<double> & in)
    {
		res = std::min(res, in.min());
    });
    return res;
}

double matrix::maxAbsVal() const
{
	double res = myData[0][0];
	std::for_each(this->myData.begin(),
				  this->myData.end(),
                  [&res](const std::valarray<double> & in)
    {
		res = std::max(res, std::abs(in).max());
    });
    return res;
}
double matrix::minAbsVal() const
{
	double res = myData[0][0];
	std::for_each(this->myData.begin(),
				  this->myData.end(),
                  [&res](const std::valarray<double> & in)
    {
		res = std::min(res, std::abs(in).min());
    });
    return res;
}

std::valarray<double> matrix::maxOfRows() const
{
	std::valarray<double> res(this->rows(), 0);
	for(int i = 0; i < res.size(); ++i)
	{
		res[i] = this->myData[i].max();
	}
	return res;
}

std::valarray<double> matrix::maxOfCols() const
{
	std::valarray<double> res(this->cols(), 0);
	for(int i = 0; i < res.size(); ++i)
	{
		std::valarray<double> t = this->getCol(i);
		res[i] = t.max();
	}
	return res;
}

double matrix::sum() const
{
    double res = 0.;
	std::for_each(this->myData.begin(),
				  this->myData.end(),
                  [&res](const std::valarray<double> & in)
    {
       res += in.sum();
    });
    return res;
}


matrixType::iterator matrix::begin()
{
	return this->myData.begin();
}

matrixType::iterator matrix::end()
{
	return this->myData.end();
}

matrixType::const_iterator matrix::begin() const
{
	return this->myData.begin();
}

matrixType::const_iterator matrix::end() const
{
	return this->myData.end();
}

std::valarray<double> matrix::toVectorByRows() const
{
    std::valarray<double> res(this->rows() * this->cols());
    for(uint i = 0; i < this->rows(); ++i)
    {
		std::copy(std::begin(this->myData[i]),
				  std::end(this->myData[i]),
                  std::begin(res) + this->cols() * i);
    }
    return res;
}

std::valarray<double> matrix::toVectorByCols() const
{
    std::valarray<double> res(this->rows() * this->cols());
    int count = 0;
    for(uint i = 0; i < this->cols(); ++i)
    {
        for(uint j = 0; j < this->rows(); ++j)
        {
			res[count++] = this->myData[j][i];
        }
    }
    return res;
}

std::valarray<double> matrix::sigmaOfCols() const
{
    std::valarray<double> res(this->cols());
#if 1
    for(uint i = 0; i < this->cols(); ++i)
    {
        std::valarray<double> W = this->getCol(i);
        res[i] = smLib::sigma(W);
    }
#else

    std::valarray<double> avRow = this->averageRow();

    matrix M = *this;
	for(std::valarray<double> & row : M.myData)
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

std::valarray<double> matrix::averageRow() const
{
    std::valarray<double> res(0., this->cols());
    for(uint i = 0; i < this->rows(); ++i)
    {
		res += this->myData[i];
    }
    res /= this->rows();
    return res;
}

std::valarray<double> matrix::averageCol() const
{
    std::valarray<double> res(this->rows());
    for(uint i = 0; i < this->rows(); ++i)
    {
		res[i] = this->myData[i].sum() / this->myData[i].size();
    }
    return res;
}

std::valarray<double> matrix::getCol(uint i, uint numCols) const
{
    if(numCols == 0) numCols = this->rows();
    std::valarray<double> res(numCols);
    for(uint j = 0; j < numCols; ++j)
    {
		res[j] = this->myData[j][i];
    }
    return res;
}

void matrix::pop_back()
{
	this->myData.pop_back();
}

void matrix::print(uint rows, uint cols) const
{
    if(rows == 0) rows = this->rows();
    if(cols == 0) cols = this->cols();

    for(uint i = 0; i < rows; ++i)
    {
        for(uint j = 0; j < cols; ++j)
        {
			std::cout << smLib::doubleRound(myData[i][j], 3) << "\t";
        }
		std::cout << std::endl;
    }
//    std::cout << std::endl;
}

void matrix::push_back(const std::valarray<double> & in)
{
	this->myData.push_back(in);
}

void matrix::push_back(const std::vector<double> & in)
{
	std::valarray<double> temp{in.data(), in.size()};
	this->myData.push_back(temp);
}

uint matrix::cols() const
{
	return myData[0].size();
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
			std::swap(this->myData[j][i], this->myData[i][j]);
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
		std::cout << "matrix::invert: matrix is not square" << std::endl;
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

//    std::cout << "start first cycle" << std::endl;

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

//    std::cout << "start second cycle" << std::endl;
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
//    std::cout << "do the rest" << std::endl;

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
		std::swap(this->myData[k][i], this->myData[k][j]);
    }
    return *this;
}
matrix & matrix::swapRows(uint i, uint j)
{
	std::swap(this->myData[i], this->myData[j]);
    return *this;
}

matrix & matrix::eraseRow(uint i)
{
    if(i < this->rows())
    {
		this->myData.erase(myData.begin() + i);
    }
    return *this;
}


matrix & matrix::eraseCol(uint j)
{
    if(j < this->cols())
    {
		for(std::valarray<double> & each : this->myData)
        {
			each = smLib::eraseValar(each, j);
        }
    }
    return *this;
}


/// looks like okay
matrix & matrix::eraseRows(const std::vector<uint> & indices)
{
	smLib::eraseItems(this->myData, indices);
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
		std::cout << "matrixProduct: input matrices are not productable" << std::endl;
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


    std::valarray<double> temp{};
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

std::valarray<double> matrix::matrixSystemSolveGauss(const std::valarray<double> & inVec) const
{
    const matrix & inMat = (*this);
    const uint size = inMat.rows();

    matrix initMat(inMat);
    initMat.invert();

    std::valarray<double> res(size);
    res = initMat * inVec;
    return res;
}

std::ostream & operator<<(std::ostream & os, const matrix & toOut)
{
	for(auto it = std::begin(toOut.myData); it < std::end(toOut.myData); ++it)
	{
		for(auto itt = std::begin(*it); itt < std::end(*it); ++itt)
		{
			os << smLib::doubleRound((*itt), 4) << "\t";
		}
		os << std::endl;
	}
	return os;
}
