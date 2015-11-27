#ifndef SMALLLIB_H
#define SMALLLIB_H

#include "coord.h"
#include <cmath>

const double pi = 3.141592653589;


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

inline int fftLimit(const double & inFreq,
                    const double & sampleFreq = def::freq,
                    const int & fftL = def::fftLength)
{
    return ceil(inFreq / sampleFreq * fftL - 0.5);
}

inline int fftL(const int & in)
{
    return pow(2., ceil(log2(in)));
}


#endif // SMALLLIB_H
