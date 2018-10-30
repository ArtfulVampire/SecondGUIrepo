#ifndef VALAR_H
#define VALAR_H

#include <vector>
#include <valarray>
#include <complex>

namespace smLib
{

/// any Container
/// usually std::vector<double> and std::valarray<double>
template <class Container>
Container range(double beg, double en, double step = 1.); /// [beg, en)

template <class Container>
Container slice(double sta, int siz, double step);

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
Container contPopFront(const Container & in, int numOfPop);

template <class Typ>
Typ mean(const std::valarray<Typ> & in1);

//template <class Typ>
//Typ mean(const std::vector<Typ> & in1);


/// Container<double>

template <class Container>
double min(const Container & arr);

template <class Container>
double max(const Container & arr);

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

#if 0
/// unused yet (23-Sep-2018)
template <class Container>
Container centered(const Container & arr);
#endif



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

std::valarray<double> randomValar(int size);
inline std::valarray<double> logistic(const std::valarray<double> & in)
{
	const double temp = 10.;
	return 1. / (1. + std::exp(-in / temp));
}


std::valarray<double> softmax(const std::valarray<double> & in);

std::valarray<double> abs(const std::valarray<std::complex<double>> & in);

std::valarray<double> valarErase(const std::valarray<double> & in, int index);

template <typename Typ>
void valarResize(std::valarray<Typ> & in, int num);

std::valarray<double> valarPushBack(const std::valarray<double> & in, double val);

inline void normalize(std::valarray<double> & in)
{
	in /= norma(in);
}

inline std::valarray<double> normalized(const std::valarray<double> & in)
{
	return in / norma(in);
}

inline std::valarray<double> normalized(std::valarray<double> && in)
{
	return in / norma(in);
}

inline double distance(const std::valarray<double> & in1,
					   const std::valarray<double> & in2)
{
	return norma(std::valarray<double>(in1 - in2));
}


} /// end of namespace

#endif /// VALAR_H
