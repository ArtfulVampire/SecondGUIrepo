#include <stdio.h>
#include <complex>
#include <cmath>
#include <iostream>
#include <vector>
#include "library.h"

namespace btr
{
std::valarray<double> butterworth(const std::valarray<double> & in,
								  int order,
								  double srate,
								  double centerFreq,
								  double halfBand)
{
	std::valarray<double> res(in.size());

	std::complex<double> U, P;
	std::vector<std::complex<double>> B;
	std::vector<std::complex<double>> G;

	B.resize(order + 1);
	G.resize(order + 1);
	double timeBin = 1. / srate;

	auto U0 = 2 * pi * halfBand * timeBin;
	U = U0;
	auto U1 = 1. - exp(-U0);

	for(int i = 1; i <= order; ++i)
	{
		P = exp(std::complex<double>(0., (pi * (order + 2 * i - 1)) / (2 * order)))
				+ std::complex<double>(0., centerFreq / halfBand);
		B[i] = exp(U * P);
		G[i] = std::complex<double>(0., 0.);
	}

	for(int i = 1; i <= in.size(); ++i)
	{
		G[1] = U1 * in[i - 1] + B[1] * G[1];


		for(int j = 2; j <= order; ++j)
		{
			G[j] = G[j - 1] * U1 + B[j] * G[j];
		}
		res[i - 1] = G[order].real();
	}
	return res;
}
std::valarray<double> refilterButter(const std::valarray<double> & in,
									 int order,
									 double srate,
									 double lowFreq,
									 double highFreq)
{
	std::valarray<double> res(in.size());
	res = btr::butterworth(in, order, srate, (lowFreq + highFreq) / 2., (highFreq - lowFreq) / 2.);
	res = myDsp::reverseArray(res);
	res = btr::butterworth(res, order, srate, (lowFreq + highFreq) / 2., (highFreq - lowFreq) / 2.);
	res = myDsp::reverseArray(res);
	return res;
}
}
