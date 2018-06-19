#ifndef VALAR_H
#define VALAR_H

#include <cmath>
#include <vector>
#include <valarray>
#include <algorithm>
#include <numeric>
#include <complex>
#include <iostream>
#include <chrono>
#include <random>
#include <other/consts.h> // for uint

namespace smLib
{

/// any Container
/// usually std::vector<double> and std::valarray<double>
template <class Container = std::valarray<double>>
Container range(int beg, int en); // [beg, en)

template <class Container = std::valarray<double>>
Container unite(const std::vector<Container> & ranges);

template <typename Container>
Container mixed (int siz);

template <typename Container>
void mix (Container & in);

template <class Container>
Container contReverse(const Container & in);

template <typename Container>
Container contSubsec(const Container & in, int beg, int en);

template <class Container>
Container contPopFront(const Container & in, uint numOfPop);

template <class Typ>
Typ mean(const std::valarray<Typ> & in1);

//template <typename Typ, template <typename, typename = std::allocator<Typ>> class Cont>
//Typ mean(const Cont<Typ> & in1);

template <typename Typ>
Typ mean(const std::vector<Typ> & in1);




/// Container<double>

template <class Container>
double min(const Container & in1);

template <class Container>
double max(const Container & in1);

template <class Container>
double prod(const Container & in1, const Container & in2);

template <class Container>
double norma(const Container & in);

template <class Container>
double normaSq(const Container & in);

template <class Container>
double variance(const Container & arr);

template <class Container>
double sigma(const Container & arr);

template <class Container>
double covariance(const Container & arr1, const Container & arr2);

template <class Container>
double correlation(const Container & arr1, const Container & arr2);

template <class Container>
double sigmaToMean(const Container & arr);

template <class Container>
double median(const Container & arr);

template <class Container>
Container centered(const Container & arr);



/// other
inline std::complex<double> abs(std::complex<double> in)
{
	return std::complex<double>(std::abs(in));
}

template <typename Typ>
std::valarray<Typ> vecToValar(const std::vector<Typ> & in);


template <typename Typ>
std::vector<Typ> valarToVec(const std::vector<Typ> & in);




/// pure valarray
inline std::valarray<double> logistic(const std::valarray<double> & in)
{
	const double temp = 10.;
	return 1. / (1. + exp(-in / temp));
}


std::valarray<double> softmax(const std::valarray<double> & in);

inline std::valarray<double> abs(const std::valarray<std::complex<double>> & in)
{
	std::valarray<double> res(in.size());
	std::transform(std::begin(in),
				   std::end(in),
				   std::begin(res),
				   [](std::complex<double> a){ return std::abs(a); });
	return res;
}

inline std::valarray<double> valarErase(const std::valarray<double> & in, uint index)
{
	std::valarray<double> res(in.size() - 1);
	std::copy(std::begin(in),
			  std::begin(in) + index,
			  std::begin(res));
	std::copy(std::begin(in) + index + 1,
			  std::end(in),
			  std::begin(res) + index);
	return res;
}

inline void normalize(std::valarray<double> & in)
{
	in /= norma(in);
}

inline std::valarray<double> normalized(const std::valarray<double> & in)
{
	return in / norma(in);
}

inline std::valarray<double> randomValar(int size)
{
	std::valarray<double> res(size);
	std::uniform_real_distribution<double> dist(-25., 25.);
	std::default_random_engine eng{};

	for(auto & in : res)
	{
		in = dist(eng);
	}
	smLib::normalize(res);
	return res;
}

inline double distance(const std::valarray<double> & in1,
					   const std::valarray<double> & in2)
{
//	if(in1.size() != in2.size())
//	{
//		std::cout << "distance: std::valarray<double>s of different size" << std::endl;
//		return 0.; /// exception
//	}
	return norma(std::valarray<double>(in1 - in2));
}

template <typename Typ>
inline void valarResize(std::valarray<Typ> & in, int num)
{
	std::valarray<Typ> temp(in);
	in.resize(num);
	std::copy(std::begin(temp),
			  std::begin(temp) + std::min(in.size(), temp.size()),
			  std::begin(in));
}
template void valarResize(std::valarray<double> & in, int num);
template void valarResize(std::valarray<std::complex<double>> & in, int num);


inline std::valarray<double> valarPushBack(const std::valarray<double> & in, double val)
{
	std::valarray<double> res(in);
	valarResize(res, res.size() + 1);
	res[in.size()] = val;
	return res;
}


} /// end of namespace

#endif // VALAR_H
