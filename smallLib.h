#pragma once
#ifndef SMALLLIB_H
#define SMALLLIB_H

#include "coord.h"
#include <cmath>
#include <set>
#include <valarray>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <complex>


template <typename Typ>
class trivector : public std::vector<std::vector<std::vector<Typ>>>
{};
template <typename Typ>
class twovector : public std::vector<std::vector<Typ>>
{};

const long double pi = 3.14159265358979323846L;

namespace smallLib
{

inline bool isDouble(const QString & in)
{
	return QString::number(in.toDouble()) == in;
}

inline bool isInt(const QString & in)
{
	return QString::number(in.toInt()) == in;
}

inline double doubleRound(double in, int numSigns)
{
	return std::round(in * pow(10., numSigns)) / double(pow(10, numSigns));
}

inline std::complex<double> doubleRound(const std::complex<double> & in, int numSigns)
{
	return std::complex<double>(doubleRound(in.real(), numSigns),
								doubleRound(in.imag(), numSigns));
}

inline double doubleRound(double in)
{
    return doubleRound(in, 2 - floor(log10(in))); // 2 significant numbers
}

inline double doubleRoundFraq(double in, int denom)
{
    return ceil(in * denom - 0.5) / denom;
}

inline double gaussian(double x, double sigma = 1.) // N(0,1)
{
    return 1./(sigma * sqrt(2. * pi)) * exp(-x * x / (2. * sigma * sigma) );
}

inline double sigmoid(double x, double t = 10.)
{
    return 1. / ( 1. + exp(-x/t) );
}

inline std::valarray<double> logistic(const std::valarray<double> & in)
{
    const double temp = 10.;
    return 1. / (1. + exp(-in / temp));
}

inline std::valarray<double> softmax(const std::valarray<double> & in)
{
    // -1 for bias
    std::valarray<double> tmp = exp(in);
    double sum = std::accumulate(std::begin(tmp),
                                 std::end(tmp) - 1,
                                 0.
//                                 ,[](double init, double val){return init + val;}
    );
    return tmp / sum; // dont care about the last(bias)

}

template <typename Typ>
inline std::valarray<Typ> vecToValar(const std::vector<Typ> & in)
{
	std::valarray<Typ> res(in.size());
	std::copy(std::begin(in),
			  std::end(in),
			  std::begin(res));
	return res;
}

inline int fftL(int in)
{
    return pow(2., ceil(log2(in)));
}

inline std::complex<double> abs(std::complex<double> in)
{
	return std::complex<double>(std::abs(in));
}

inline std::valarray<double> abs(const std::valarray<std::complex<double>> & in)
{
	std::valarray<double> res(in.size());
	std::transform(std::begin(in),
				   std::end(in),
				   std::begin(res),
				   [](std::complex<double> a){ return std::abs(a);});
	return res;
}

inline std::valarray<double> pop_front_valar(const std::valarray<double> & in, uint numOfPop)
{
	std::valarray<double> res(in.size() - numOfPop);
	std::copy(std::begin(in) + numOfPop, std::end(in), std::begin(res));
	return res;
}

template <typename Typ>
inline void resizeValar(std::valarray<Typ> & in, int num)
{
	std::valarray<Typ> temp = in;
	in.resize(num);
	std::copy(std::begin(temp),
			  std::begin(temp) + std::min(in.size(), temp.size()),
			  std::begin(in));
}

//inline std::valarray<double> resizeValar(const std::valarray<double> & in, int num)
//{
//	std::valarray<double> temp(num);
//	std::copy(std::begin(in),
//			  std::begin(in) + std::min(in.size(), num),
//			  std::begin(temp));
//	return temp;
//}

inline std::valarray<double> eraseValar(const std::valarray<double> & in, uint num)
{
    std::valarray<double> res(in.size() - 1);
    std::copy(std::begin(in),
              std::begin(in) + num,
              std::begin(res));
    std::copy(std::begin(in) + num + 1,
              std::end(in),
              std::begin(res) + num);
    return res;
}


inline double prod(const std::valarray<double> & in1, const std::valarray<double> & in2)
{
//    if(in1.size() != in2.size())
//    {
//        std::cout << "smallLib::prod: sizes = " << in1.size() << "   " << in2.size() << std::endl;
//        exit(1);
//    }
    return std::inner_product(std::begin(in1),
                              std::end(in1),
                              std::begin(in2),
                              0.);
}

inline double normaSq(const std::valarray<double> & in)
{
    return std::inner_product(std::begin(in),
                              std::end(in),
                              std::begin(in),
                              0.);
}

template <typename Typ>
inline Typ mean(const std::valarray<Typ> & arr)
{
	return arr.sum() / Typ(arr.size());
}
template std::complex<double> mean(const std::valarray<std::complex<double>> & arr);
template double mean(const std::valarray<double> & arr);

inline double variance(const std::valarray<double> & arr)
{
    return normaSq(arr - mean(arr)) / arr.size();
}

inline double sigma(const std::valarray<double> & arr)
{
    return sqrt(variance(arr));
}

inline double min(const std::valarray<double> & arr)
{
	return arr.min();
}

inline double max(const std::valarray<double> & arr)
{
	return arr.max();
}

inline double median(const std::valarray<double> & arr)
{
	auto arr2 = arr;
	std::sort(std::begin(arr2), std::end(arr2));
	if(arr2.size() % 2 == 1)
	{
		return arr2[arr2.size() / 2];
	}
	else
	{
		return 0.5 * (arr2[arr2.size() / 2] + arr2[arr2.size() / 2 - 1]);
	}
}

inline double covariance(const std::valarray<double> & arr1, const std::valarray<double> & arr2)
{
    return prod(arr1 - mean(arr1), arr2 - mean(arr2));
}

inline double correlation(const std::valarray<double> & arr1, const std::valarray<double> & arr2)
{
    return covariance(arr1, arr2) / (sigma(arr1) * sigma(arr2) * arr1.size());
}

inline double norma(const std::valarray<double> & in)
{
    return sqrt(normaSq(in));
}

inline void normalize(std::valarray<double> & in)
{
    in /= norma(in);
}

inline double distance(double const x1, double const y1,
					   double const x2, double const y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}


inline double distance(const std::valarray<double> & in1,
					   const std::valarray<double> & in2)
{
    if(in1.size() != in2.size())
    {
		std::cout << "distance: std::valarray<double>s of different size" << std::endl;
        return 0.; /// exception
    }
    return norma(in1 - in2);
}

template <typename T>
void eraseItems(std::vector<T> & inVect,
                const std::vector<uint> & indices)
{
    const int initSize = inVect.size();
    std::set<int, std::less<int>> excludeSet; // less first
    for(auto item : indices)
    {
        excludeSet.emplace(item);
    }
    std::vector<int> excludeVector;
    for(auto a : excludeSet)
    {
        excludeVector.push_back(a);
    }
    excludeVector.push_back(initSize);

    for(int i = 0; i < int(excludeVector.size()) - 1; ++i)
    {
        for(int j = excludeVector[i] - i; j < excludeVector[i + 1] - i - 1; ++j)
        {
            inVect[j] = std::move(inVect[j + 1 + i]);
        }
    }
    inVect.resize(initSize - excludeSet.size());
}
template void eraseItems(std::vector<std::valarray<double>> & inVect, const std::vector<uint> & indices);
template void eraseItems(std::vector<int> & inVect, const std::vector<uint> & indices);
template void eraseItems(std::vector<double> & inVect, const std::vector<uint> & indices);
template void resizeValar(std::valarray<double> & in, int num);
template void resizeValar(std::valarray<std::complex<double>> & in, int num);
template std::valarray<double> vecToValar(const std::vector<double> & in);
template std::valarray<std::complex<double>> vecToValar(const std::vector<std::complex<double>> & in);

} // namespace smallLib


#endif // SMALLLIB_H
