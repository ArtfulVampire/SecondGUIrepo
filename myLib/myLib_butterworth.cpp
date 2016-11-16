#include <stdio.h>
#include <complex>
#include <cmath>
#include <iostream>
#include <vector>
#include "library.h"

namespace btr
{

void zeros2coeffs(std::vector<std::complex<double>> & zeros,
				  std::vector<std::complex<double>> & coeffs,
				  int size) // symmetric polynoms
{

	std::vector<int> b(size + 1);
	int i = 0;

	while( b[size] == 0 )
	{
		i = 0;
		while(b[i] != 0)
		{
			b[i++] = 0;
		}
		b[i] = 1;

		int subset_size = 0;
		std::complex<double> summand = 1.0;
		for(int i = 0; i < size; ++i)
		{
			if(b[i] != 0)
			{
				summand *= zeros[i];
				++subset_size;
			}
		}
		coeffs[size - subset_size] += summand;
	}

	for(int i = (size + 1) % 2; i < size; i += 2)
	{
		coeffs[i] = -coeffs[i];
	}

	coeffs[size] = std::complex<double>(1.0, 0.0);
}

void butterworth_poles(std::vector<std::complex<double>> & poles,
					   int order,
					   double cutoff) // cutoff Hz
{
	double arg;
	for(int i = 0; i < order; ++i)
	{
		//arg = (M_PI*(2*i+1))/order;
		arg = M_PI * (2 * i + order + 1) / 2 / order;
		poles[i] = std::complex<double>(std::cos(arg), std::sin(arg));
		poles[i] *= 2 * M_PI * cutoff;
	}
}

void butterworth_z_zeros(std::vector<std::complex<double>> & zeros, int order)
{
	for(int i = 0; i < order; ++i)
	{
		zeros[i] = std::complex<double>(-1.0, 0.0);
	}
}

double warp_freq(double freq, double Fs)
{
	return Fs * tan(M_PI * freq / Fs) / M_PI;
}

void p2z(std::vector<std::complex<double>> & p,
		 std::vector<std::complex<double>> & z,
		 int size,
		 double Fs)
{
	for(int i = 0; i < size; ++i)
	{
		z[i] = (std::complex<double>(2 * Fs, 0.0) + p[i]) /
			   (std::complex<double>(2 * Fs, 0.0) - p[i]);
	}
}


// recalc coeffs from x to 1/x polynomial variable
void inverse_poly(std::vector<std::complex<double>> & coeffs, int size)
{
	std::complex<double> denom = coeffs[size-1];
	for(int i = 0; i < size; ++i)
	{
		coeffs[i] /= denom;
	}
//	reverse(coeffs, coeffs + size); /// ???
	for(int i = 0; i < size / 2; ++i)
	{
		std::swap(coeffs[i], coeffs[size-1 - i]);
	}

}

void print_line(int len)
{
	std::cout << "<";
	for(int i = 0; i < len - 2; ++i)
	{
		std::cout << "=";
	}
	std::cout << ">" << std::endl;
}


int test()
{
	int max_size = 30;
	std::vector<std::complex<double>> p_poles(max_size);
	std::vector<std::complex<double>> z_poles(max_size);
	std::vector<std::complex<double>> b_coeffs(max_size);

	std::vector<std::complex<double>> z_zeros(max_size);
	std::vector<std::complex<double>> a_coeffs(max_size);

	/// params
	int order = 6;
	double Fs = 250;
	double analog_cutoff = Fs / 2 * 0.7; // analog freq
	double cutoff = warp_freq(analog_cutoff, Fs); // corresponding digital freq

	print_line();
	std::cout << "Butterworth low-pass filter design" << std::endl;
	std::cout << "Order: " << order << std::endl;
	std::cout << "Cutoff freq: " << analog_cutoff << " Hz" << std::endl;
	std::cout << "Digital cutoff freq: " << cutoff << " Hz" << std::endl;
	print_line();

	butterworth_poles(p_poles, order, cutoff);
	std::cout << "poles in p-domain:" << std::endl;
	for(int i = 0; i < order; ++i)
	{
		std::cout << p_poles[i] << std::endl;
	}

	p2z(p_poles, z_poles, order, Fs);
	std::cout << "poles in z-domain:" << std::endl;
	for(int i = 0; i < order; ++i)
	{
		std::cout << z_poles[i] << std::endl;
	}

	zeros2coeffs(z_poles, b_coeffs, order);
	std::cout << "z denominator coeffs:" << std::endl;
	for(int i = 0; i < order + 1; ++i)
	{
		std::cout << b_coeffs[i].real() << std::endl;
	}

//	std::complex<double> denom = coeffs[order];

//	inverse_poly(b_coeffs, order+1);
//	std::cout << "1/z denominator coeffs:" << std::endl;
//	for(int i=0; i<order+1; ++i)
//	{
//		std::cout << "\t" << b_coeffs[i].real() << std::endl;
//	}
	print_line();

	butterworth_z_zeros(z_zeros, order);
	std::cout << "zeros in z-domain:" << std::endl;
	for(int i = 0; i < order; ++i)
	{
		std::cout << z_zeros[i] << std::endl;
	}

	zeros2coeffs(z_zeros, a_coeffs, order);
	std::cout << "z numerator coeffs:" << std::endl;
	for(int i = 0; i < order + 1; ++i)
	{
		std::cout << a_coeffs[i].real() << std::endl;
	}

//	inverse_poly(a_coeffs, order+1);
//	std::cout << "1/z numerator coeffs:" << std::endl;
//	for(int i=0; i<order+1; ++i)
//	{
//		std::cout << "\t" << a_coeffs[i].real() << std::endl;
//	}
	print_line();

	double k0_numer = 0.0;
	double k0_denom = 0.0;
	for(int i = 0; i < order + 1; ++i)
	{
		k0_numer += a_coeffs[i].real();
		k0_denom += b_coeffs[i].real();
	}
	std::cout << "k0: " << k0_numer/k0_denom << std::endl;
	print_line();

	return 0;
}
}
