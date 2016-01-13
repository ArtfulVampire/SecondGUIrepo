#ifndef SMALLFUNCS_H
#define SMALLFUNCS_H

#include "coord.h"
#include <cmath>
#include <set>
#include <valarray>
#include <iostream>
#include <algorithm>
#include <numeric>

using namespace std;

typedef std::valarray<double> lineType;

const long double pi = 3.14159265358979323846L;

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

inline double gaussian(const double & x, const double & sigma = 1.) //N(0,1)
{
    return 1./(sigma * sqrt(2. * pi)) * exp(-x * x / (2. * sigma * sigma) );
}

inline double logistic(const double & x, const double & t = 10.)
{
    return 1. / ( 1. + exp(-x/t) );
}

std::valarray<double> softmax(const std::valarray<double> & in)
{
    // -1 for bias
    double sum = std::accumulate(begin(in), end(in) - 1, 0.,
                                 [](double init, double val){return init + exp(val);});
    return exp(in) / sum;

}



inline int fftL(const int & in)
{
    return pow(2., ceil(log2(in)));
}



inline double prod(const lineType & in1, const lineType & in2)
{
    return std::inner_product(begin(in1),
                              end(in1),
                              begin(in2),
                              0.);
}

inline double normaSq(const lineType & in)
{
    return std::inner_product(begin(in),
                              end(in),
                              begin(in),
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
    return covariance(arr1, arr2) / (sigma(arr1) * sigma(arr2));
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
        cout << "distance: lineTypes of different size" << endl;
        return 0.; /// exception
    }
    return norma(in1 - in2);
}

template <typename T>
void eraseItems(vector<T> & inVect,
                const vector<int> & indices)
{
    const int initSize = inVect.size();
    std::set<int, std::less<int>> excludeSet; // less first
    for(auto item : indices)
    {
        excludeSet.emplace(item);
    }
    vector<int> excludeVector;
    for(auto a : excludeSet)
    {
        excludeVector.push_back(a);
    }
    excludeVector.push_back(initSize);

    for(int i = 0; i < excludeVector.size() - 1; ++i)
    {
        for(int j = excludeVector[i] - i; j < excludeVector[i + 1] - i - 1; ++j)
        {
            inVect[j] = std::move(inVect[j + 1 + i]);
        }
    }
    inVect.resize(initSize - excludeSet.size());
}
template void eraseItems(vector<lineType> & inVect, const vector<int> & indices);
template void eraseItems(vector<int> & inVect, const vector<int> & indices);
template void eraseItems(vector<double> & inVect, const vector<int> & indices);



#endif // SMALLFUNCS_H
