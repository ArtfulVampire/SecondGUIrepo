//#pragma once
#ifndef SMALLLIB_H
#define SMALLLIB_H

#include <cmath>
#include <set>
#include <valarray>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <complex>

#include "coord.h"
#include "valar.h"


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

inline int fftL(int in)
{
	return pow(2., ceil(log2(in)));
}

inline double distance(double const x1, double const y1,
					   double const x2, double const y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
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

} // namespace smallLib


#endif // SMALLLIB_H
