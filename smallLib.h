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

typedef std::valarray<double> lineType;
typedef std::vector<double> vectType;
typedef std::vector<std::vector<double>> mat;
typedef std::vector<std::pair<double, double>> coordType;

template <typename Typ>
class trivector : public std::vector<std::vector<std::vector<Typ>>>
{};
template <typename Typ>
class twovector : public std::vector<std::vector<Typ>>
{};

const long double pi = 3.14159265358979323846L;

namespace smallLib
{


inline double doubleRound(const double & in, const int & numSigns)
{
    return int(  ceil(in * pow(10., numSigns) - 0.5)  ) / pow(10., numSigns);
}

inline double doubleRound(const double & in)
{
    return doubleRound(in, 2 - floor(log10(in))); // 2 significant numbers
}

inline double doubleRoundFraq(const double & in, const int & denom)
{
    return ceil(in * denom - 0.5) / denom;
}

inline double gaussian(const double & x, const double & sigma = 1.) // N(0,1)
{
    return 1./(sigma * sqrt(2. * pi)) * exp(-x * x / (2. * sigma * sigma) );
}

inline double sigmoid(const double & x, const double & t = 10.)
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



inline int fftL(const int & in)
{
    return pow(2., ceil(log2(in)));
}


inline void resizeValar(std::valarray<double> & in, int num)
{
    lineType temp = in;
    in.resize(num);
    std::copy(std::begin(temp),
              std::begin(temp) + std::min(in.size(), temp.size()),
              std::begin(in));
}

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


inline double prod(const lineType & in1, const lineType & in2)
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

inline double normaSq(const lineType & in)
{
    return std::inner_product(std::begin(in),
                              std::end(in),
                              std::begin(in),
                              0.);
}
inline double mean(const lineType & arr)
{
    return arr.sum() / arr.size();
}

inline double variance(const lineType & arr)
{
    return normaSq(arr - mean(arr)) / arr.size();
}

inline double sigma(const lineType & arr)
{
    return sqrt(variance(arr));
}

inline double covariance(const lineType & arr1, const lineType & arr2)
{
    return prod(arr1 - mean(arr1), arr2 - mean(arr2));
}

inline double correlation(const lineType & arr1, const lineType & arr2)
{
    return covariance(arr1, arr2) / (sigma(arr1) * sigma(arr2) * arr1.size());
}

inline double norma(const lineType & in)
{
    return sqrt(normaSq(in));
}

inline void normalize(lineType & in)
{
    in /= norma(in);
}

inline double distance(const lineType & in1,
                       const lineType & in2)
{
    if(in1.size() != in2.size())
    {
        std::cout << "distance: lineTypes of different size" << std::endl;
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
template void eraseItems(std::vector<lineType> & inVect, const std::vector<uint> & indices);
template void eraseItems(std::vector<int> & inVect, const std::vector<uint> & indices);
template void eraseItems(std::vector<double> & inVect, const std::vector<uint> & indices);

} // namespace smallLib


#endif // SMALLLIB_H
