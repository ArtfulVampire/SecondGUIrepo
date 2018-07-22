#include <other/matrix.h>

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
	myData = other.myData;
}


matrix::matrix(const matrixType & other)
{
	myData = other;
}
matrix::matrix(const std::valarray<double> & vect, bool orientH)
{
    if(orientH)
    {
        this->resize(1, vect.size());
		myData[0] = vect;
    }
    else
    {
        this->resize(vect.size(), 1);
		for(uint i = 0; i < vect.size(); ++i)
        {
			myData[i][0] = vect[i];
        }
    }
}
matrix::matrix(const std::valarray<double> & vect, char orient)
{
    if(orient == 'h' || orient == 'H' || orient == 'r' || orient == 'R')
    {
        this->resize(1, vect.size());
		myData[0] = vect;
    }
    else if(orient == 'v' || orient == 'V' || orient == 'c' || orient == 'C')
    {
        this->resize(vect.size(), 1);
		for(uint i = 0; i < vect.size(); ++i)
        {
			myData[i][0] = vect[i];
        }
    }
    else
    {
        matrix();
    }
}

matrix::matrix(const std::valarray<double> & vect, int inRows)
{
    if(vect.size() % inRows != 0)
    {
		std::cout << "not appropriate size" << std::endl;
        return;
    }
    int newCols = vect.size() / inRows;

    this->resize(inRows, newCols);
	for(int i = 0; i < inRows; ++i)
    {
        std::copy(std::begin(vect) + i * newCols,
                  std::begin(vect) + (i + 1) * newCols,
				  std::begin(myData[i]));
    }
}

matrix::matrix(const std::valarray<double> & vect1, const std::valarray<double> & vect2)
{
	myData.clear();
	for(uint i = 0; i < vect1.size(); ++i)
    {
		myData.push_back(vect1[i] * vect2);
    }
}

matrix::matrix(std::initializer_list<std::valarray<double>> lst)
{
    this->resize(0, 0);
    std::for_each(lst.begin(),
                  lst.end(),
                  [this](std::valarray<double> in)
    {
		myData.push_back(in);
    });
}

/// these two are the same

matrix::matrix(const std::valarray<double> & vect) // diagonal
{
    this->resize(vect.size(), vect.size());
    this->fill(0.);
    int count = 0;
	for(auto item : vect)
    {
		myData[count][count] = item;
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
		myData[count][count] = item;
        ++count;
    }
}




matrix & matrix::operator = (double other)
{
	return this->fill(other);
}

matrix matrix::operator = (const matrix & other)
{
	myData = other.myData;
    return *this;
}
matrix matrix::operator = (const matrixType & other)
{
	myData = other;

    return *this;
}




std::ostream & operator<< (std::ostream & os, const matrix & toOut)
{
	for(const auto & row : toOut)
	{
		for(auto in : row)
		{
			os << in << "\t";
		}
		os << "\r\n";
	}
	os.flush();
	return os;
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
	for(int i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

matrix operator + (const matrix & lhs, double val)
{
    matrix result;
	for(int i = 0; i < lhs.rows(); ++i)
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
	for(int i = 0; i < this->rows(); ++i)
    {
        (*this)[i] += other[i];
    }
    return *this;
}

matrix matrix::operator += (double val)
{
	for(int i = 0; i < this->rows(); ++i)
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
	for(int i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}

matrix operator - (const matrix & lhs, double val)
{
    matrix result;
	for(int i = 0; i < lhs.rows(); ++i)
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
	for(int i = 0; i < this->rows(); ++i)
    {
        (*this)[i] -= other[i];
    }
    return *this;
}

matrix matrix::operator -= (double val)
{
	for(int i = 0; i < this->rows(); ++i)
    {
        (*this)[i] -= val;
    }
    return *this;
}
matrix matrix::operator -()
{
    matrix res(this->rows(), this->cols());

	for(int i = 0; i < this->rows(); ++i)
    {
		res[i] = -myData[i];
    }
    return res;
}



matrix operator * (const matrix & lhs, double val)
{
    matrix result(lhs.rows(), lhs.cols());
// #pragma omp parallel for
	for(int i = 0; i < lhs.rows(); ++i)
    {
        result[i] = lhs[i] * val;
    }
    return result;
}

matrix matrix::operator *= (double other)
{
	for(int i = 0; i < this->rows(); ++i)
    {
		myData[i] *= other;
    }
    return *this;
}

void matrix::operator *=(const matrix & other)
{
	(*this) = this->operator*(other);
}

matrix matrix::operator *(const matrix & other) const
{
	if(this->cols() != other.rows())
	{
		std::cout << "matrix::operator*: matrices are not productable" << std::endl;
		return *this;
	}

	const int dim1 = this->rows();
	const int dim2 = other.cols();

	matrix result(dim1, dim2, 0.);

	/// 15-20% faster than with currCol
	for(int i = 0; i < dim1; ++i)
	{
		for(int j = 0; j < this->cols(); ++j)
		{
			result[i] += myData[i][j] * other[j];
		}
	}
	return result;
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
// #pragma omp parallel for
	for(int i = 0; i < lhs.rows(); ++i)
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

	for(int i = 0; i < this->rows(); ++i)
    {
		for(int j = 0; j < this->cols(); ++j)
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


matrix matrix::apply(std::function<std::valarray<double>(const std::valarray<double> &)> func) const
{
	matrix res{};
	for(const auto & row : myData)
	{
		res.push_back(func(row));
	}
	return res;
}

matrix matrix::integrate(const std::vector<std::pair<int, int>> & intervals) const
{
	matrix res{};
	/// add intervals validation
	for(const auto & row : myData)
	{
		std::valarray<double> newRow(intervals.size());
		int counter = 0;
		for(const auto & in : intervals)
		{
			newRow[counter] = std::accumulate(std::begin(row) + in.first,
											  std::begin(row) + in.second,
											  0.);
			++counter;
		}
		res.push_back(newRow);
	}
	return res;
}


bool matrix::isEmpty() const
{
	if(this->rows() == 0 || this->cols() == 0) return true;
	return false;
}


matrix matrix::operator /= (double other)
{
	for(int i = 0; i < this->rows(); ++i)
    {
		myData[i] /= other;

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
	for(auto it = std::begin(myData); it != std::end(myData); ++it)
	{
		(*it) = value;
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
	for(int i = 0; i < other.rows(); ++i)
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

	for(int i = 0; i < this->rows(); ++i)
	{
		std::copy(std::begin(other[i]),
				  std::end(other[i]),
				  std::begin(myData[i]) + startCopy);
	}
	return *this;

}


matrix & matrix::random(double low, double high)
{
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
//    std::default_random_engine gen;
    std::uniform_real_distribution<double> distr(low, high);
	for(auto it = std::begin(myData); it != std::end(myData); ++it)
    {
#if MATRIX_OMP
#pragma omp parallel for
#endif
		for(auto itt = std::begin(*it); itt != std::end(*it); ++itt)
        {
            (*itt) = distr(gen);
        }
    }
    return *this;
}

matrix & matrix::pop_front(int numOfCols)
{
	for(std::valarray<double> & row : myData)
	{
		row = smLib::contPopFront(row, numOfCols);
	}
	return *this;
}

matrix matrix::subCols(int beginCol, int endCol) const /// submatrix
{
	if(endCol > this->cols())
	{
		std::cout << "matrix::subCols: too few cols" << std::endl;
		endCol = this->cols();
	}

    matrix res(this->rows(), endCol - beginCol);
	for(int i = 0; i < this->rows(); ++i)
	{
		res[i] = smLib::contSubsec(myData[i], beginCol, endCol);
	}
    return res;
}

matrix matrix::subCols(int newCol) const
{
	matrix res(*this);
	if(newCol < 0) { newCol += this->cols(); }
	return res.resizeCols(newCol);
}

matrix matrix::subCols(const std::vector<std::pair<int, int>> & intervals) const
{
	int newSize = 0;
	for(const auto & in : intervals) { newSize += in.second - in.first; }

	matrix res(this->rows(), newSize);
	int colCounter = 0;
	for(const auto & in : intervals)
	{
		for(int i = 0; i < this->rows(); ++i)
		{
			std::copy(std::begin(myData[i]) + in.first,
					  std::begin(myData[i]) + in.second,
					  std::begin(res[i]) + colCounter);
		}
		colCounter += in.second - in.first;
	}
	return res;
}

matrix matrix::subRows(const std::vector<int> & inds) const /// submatrix
{
	matrix res = matrix();
    for(int i : inds)
    {
		res.myData.push_back(myData[i]);
    }
    return res;
}

matrix matrix::subRows(const std::vector<uint> & inds) const /// submatrix
{
	matrix res{};
    for(int i : inds)
    {
		res.myData.push_back(myData[i]);
    }
    return res;
}

matrix matrix::subRows(int newRows) const /// submatrix
{
	matrix res(*this);
	if(newRows < 0) { newRows += this->rows(); }
	return res.resizeRows(newRows);
}


matrix & matrix::centerRows(int numRows)
{
	if(numRows == 0) { numRows = this->rows(); }

	/// count zero columns
	int zeroCols = 0;
	for(int i = 0; i < this->cols(); ++i)
	{
		const std::valarray<double> col = this->getCol(i, numRows);
		if((col == 0.).min() == true) { ++zeroCols; }
	}

	for(int i = 0; i < numRows; ++i)
	{
		/// centering
		const double meanVal = smLib::mean(myData[i]) * this->cols() / (this->cols() - zeroCols);
		std::for_each(std::begin(myData[i]),
					  std::end(myData[i]),
					  [meanVal](double & in)
		{
			if(in != 0.) { in -= meanVal; }
		});
	}
	return *this;
}


matrix matrix::covMatRows() const
{
	matrix cop(*this);
	cop.centerRows();
	return cop * matrix::transposed(cop) / cop.cols();
}

matrix & matrix::normColsLastRowOne()
{
	for(int j = 0; j < this->cols(); ++j)
	{
		for(int i = 0; i < this->rows(); ++i)
		{
			myData[i][j] /= myData[this->rows() - 1][j];
		}
	}
	return *this;
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

	for(int j = 0; j < cop.rows(); ++j)
    {
		cop.myData[j] -= *avRow;
    }

	matrix res = matrix::transposed(cop) * cop;
    return res;
}

matrix & matrix::resize(int rows, int cols, double val)
{
    this->resize(rows, cols);
    this->fill(val);
	return *this;
}

matrix & matrix::resize(int newRows, int newCols)
{
	myData.resize(newRows);
	std::for_each(std::begin(myData),
				  std::end(myData),
                  [newCols](std::valarray<double> & in)
    {
		smLib::valarResize(in, newCols);
    });

	return *this;
}

matrix & matrix::resize(int i)
{
	myData.resize(i);
	return *this;
}

matrix&  matrix::reserve(int i)
{
	myData.reserve(i);
	return *this;
}


matrix & matrix::resizeRows(int newRows)
{
    int cols = this->cols();
    int oldRows = this->rows();
	myData.resize(newRows);
    if(oldRows < newRows)
    {
		std::for_each(std::begin(myData) + oldRows,
					  std::end(myData),
                      [cols](std::valarray<double> & in)
        {
			smLib::valarResize(in, cols);
        });
    }
    return *this;
}


matrix & matrix::resizeCols(int newCols)
{
	std::for_each(std::begin(myData),
				  std::end(myData),
                  [newCols](std::valarray<double> & in)
    {
		smLib::valarResize(in, newCols);
    });
    return *this;
}

double matrix::maxVal() const
{
	double res = myData[0][0];
	std::for_each(std::begin(myData),
				  std::end(myData),
                  [&res](const std::valarray<double> & in)
    {
		res = std::max(res, in.max());
    });
    return res;
}
double matrix::minVal() const
{
	double res = myData[0][0];
	std::for_each(std::begin(myData),
				  std::end(myData),
                  [&res](const std::valarray<double> & in)
    {
		res = std::min(res, in.min());
    });
    return res;
}

double matrix::maxAbsVal() const
{
	double res = myData[0][0];
	std::for_each(std::begin(myData),
				  std::end(myData),
                  [&res](const std::valarray<double> & in)
    {
		res = std::max(res, std::abs(in).max());
    });
    return res;
}

double matrix::minAbsVal() const
{
	double res = myData[0][0];
	std::for_each(std::begin(myData),
				  std::end(myData),
                  [&res](const std::valarray<double> & in)
    {
		res = std::min(res, std::abs(in).min());
    });
    return res;
}

std::valarray<double> matrix::maxOfRows() const
{
	std::valarray<double> res(this->rows(), 0);
	for(uint i = 0; i < res.size(); ++i)
	{
		res[i] = myData[i].max();
	}
	return res;
}

std::valarray<double> matrix::maxOfCols() const
{
	std::valarray<double> res(this->cols(), 0);
	for(uint i = 0; i < res.size(); ++i)
	{
		std::valarray<double> t = this->getCol(i);
		res[i] = t.max();
	}
	return res;
}

double matrix::sum() const
{
    double res = 0.;
	std::for_each(std::begin(myData),
				  std::end(myData),
                  [&res](const std::valarray<double> & in)
    {
       res += in.sum();
    });
    return res;
}

double matrix::traceCov() const
{
	double res = 0.;
	for(const auto & row : myData)
	{
		res += smLib::variance(row);
	}
	return res;
}

std::vector<double> matrix::toVectorByRows() const
{
	std::vector<double> res;
	res.reserve(this->rows() * this->cols() + 100);
	res.resize(this->rows() * this->cols());
	for(int i = 0; i < this->rows(); ++i)
	{
		std::copy(std::begin(myData[i]),
				  std::end(myData[i]),
				  std::begin(res) + this->cols() * i);
	}
	return res;
}

std::valarray<double> matrix::toValarByRows() const
{
    std::valarray<double> res(this->rows() * this->cols());
	for(int i = 0; i < this->rows(); ++i)
    {
		std::copy(std::begin(myData[i]),
				  std::end(myData[i]),
                  std::begin(res) + this->cols() * i);
    }
    return res;
}

std::valarray<double> matrix::toValarByCols() const
{
    std::valarray<double> res(this->rows() * this->cols());
    int count = 0;
	for(int i = 0; i < this->cols(); ++i)
    {
		for(int j = 0; j < this->rows(); ++j)
        {
			res[count++] = myData[j][i];
        }
    }
    return res;
}

std::valarray<double> matrix::sigmaOfCols() const
{
    std::valarray<double> res(this->cols());
#if 1
	for(int i = 0; i < this->cols(); ++i)
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
	for(int i = 0; i < this->rows(); ++i)
    {
		res += myData[i];
    }
    res /= this->rows();
    return res;
}

std::valarray<double> matrix::averageCol() const
{
    std::valarray<double> res(this->rows());
	for(int i = 0; i < this->rows(); ++i)
    {
		res[i] = myData[i].sum() / myData[i].size();
    }
    return res;
}

std::valarray<double> matrix::getCol(int i, int numRows) const
{
	if(numRows == 0) { numRows = this->rows(); }

	std::valarray<double> res(numRows);
	for(int j = 0; j < numRows; ++j)
    {
		res[j] = myData[j][i];
    }
    return res;
}

matrix & matrix::pop_back()
{
	myData.pop_back();
	return *this;
}

void matrix::print(int rows, int cols) const
{
    if(rows == 0) rows = this->rows();
    if(cols == 0) cols = this->cols();

	for(int i = 0; i < rows; ++i)
    {
		for(int j = 0; j < cols; ++j)
        {
			std::cout << smLib::doubleRound(myData[i][j], 3) << "\t";
        }
		std::cout << std::endl;
    }
//    std::cout << std::endl;
}

void matrix::printWithBraces(int rows, int cols) const
{
	if(rows == 0) rows = this->rows();
	if(cols == 0) cols = this->cols();

	std::cout << "{";
	for(int i = 0; i < rows; ++i)
	{
		std::cout << "{";
		for(int j = 0; j < cols; ++j)
		{
			std::cout << smLib::doubleRound(myData[i][j], 3);
			if(j != cols - 1) { std::cout << ","; }
		}
		std::cout << "}";
		if(i != rows - 1) { std::cout << ","; }
	}
	std::cout << "}";
	std::cout << std::endl;
}

matrix & matrix::push_back(const std::valarray<double> & in)
{
	myData.push_back(in);
	return *this;
}

matrix & matrix::push_back(const std::vector<double> & in)
{
	std::valarray<double> temp{in.data(), in.size()};
	myData.push_back(temp);
	return *this;
}

matrix matrix::transposed(const matrix &input)
{
    matrix res(input.cols(), input.rows());
	for(int i = 0; i < input.rows(); ++i)
    {
		for(int j = 0; j < input.cols(); ++j)
        {
            res[j][i] = input[i][j];
        }
    }
	return res;
}

matrix & matrix::transpose()
{
#if 1
	(*this) = matrix::transposed(*this);
#else
    int oldCols = this->cols();
    int oldRows = this->rows();
    this->resize(max(oldRows, oldCols),
                 max(oldRows, oldCols)); // make square

	for(int i = 0; i < this->rows(); ++i)
    {
        for(int j = i + 1; j < this->cols(); ++j)
        {
			std::swap(myData[j][i], myData[i][j]);
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
	for(int i = 0; i < this->rows(); ++i)
    {
        res += (*this)[i][i];
    }
    return res;
}


static matrix inverted(const matrix & input)
{
	matrix res(input);
	return res.invert();
}

matrix & matrix::invert(double * det)
{
    if(this->rows() != this->cols())
    {
		std::cout << "matrix::invert: matrix is not square" << std::endl;
//        exit(1);
        return *this;
    }

	const int size = this->rows();
	matrix initMat(*this);

    matrix tempMat(size, size, 0.);
	for(int i = 0; i < size; ++i)
    {
        tempMat[i][i] = 1.;
    }
    double coeff;

//    std::cout << "start first cycle" << std::endl;

    // 1) make higher-triangular
	for(int i = 0; i < size - 1; ++i) // which line to substract
    {
		for(int j = i + 1; j < size; ++j) // FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i]; // coefficient

            // row[j] -= coeff * row[i] for both (temp & init) matrices
            initMat[j] -= initMat[i] * coeff;
            tempMat[j] -= tempMat[i] * coeff;
        }
    }

//    std::cout << "start second cycle" << std::endl;
    // 2) make diagonal
    for(int i = size - 1; i > 0; --i) // which line to substract (bottom -> up)
    {
        for(int j = i - 1; j >= 0; --j) // FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i];

            // row[j] -= coeff * row[i] for both matrices
            initMat[j] -= initMat[i] * coeff;
            tempMat[j] -= tempMat[i] * coeff;
        }
    }
//    std::cout << "do the rest" << std::endl;

    if(det != nullptr)
    {
        (*det) = 1.;
		for(int i = 0; i < size; ++i)
        {
            (*det) *= initMat[i][i];
        }
    }

    // 3) divide on diagonal elements
	for(int i = 0; i < size; ++i) // which line to divide
    {
        tempMat[i] /= initMat[i][i];
    }

    (*this) = tempMat;
    return *this;
}

matrix & matrix::swapCols(int i, int j)
{
	for(int k = 0; k < this->rows(); ++k)
    {
		std::swap(myData[k][i], myData[k][j]);
    }
    return *this;
}
matrix & matrix::swapRows(int i, int j)
{
	std::swap(myData[i], myData[j]);
    return *this;
}

matrix & matrix::eraseRow(int i)
{
    if(i < this->rows())
    {
		myData.erase(std::begin(myData) + i);
    }
    return *this;
}


matrix & matrix::eraseCol(int j)
{
    if(j < this->cols())
    {
		for(std::valarray<double> & each : myData)
        {
			each = smLib::valarErase(each, j);
        }
    }
    return *this;
}

std::valarray<double> matrix::matrixSystemSolveGauss(const std::valarray<double> & inVec) const
{
    const matrix & inMat = (*this);
	const int size = inMat.rows();

    matrix initMat(inMat);
    initMat.invert();

    std::valarray<double> res(size);
    res = initMat * inVec;
    return res;
}


/*
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
#endif // omp


#endif
	return result;
}
*/

