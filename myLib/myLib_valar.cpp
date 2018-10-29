#include <myLib/valar.h>

#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>

#include <QString>

using uint = unsigned int; /// a copy from other/defs.h

namespace smLib
{

template <>
std::vector<QString> range(int beg, int en)
{
	std::vector<QString> res{};
	res.reserve(en - beg);
	for(int i = beg; i < en; ++i)
	{
		res.push_back(QString::number(i));
	}
	return res;
}

template <class Container>
Container range(int beg, int en)
{
	if(en <= beg) { return Container{}; }

	Container res(en - beg);
	std::iota(std::begin(res), std::end(res), beg);
	return res;
}
template std::vector<uint> range(int beg, int en);
template std::vector<int> range(int beg, int en);
template std::vector<double> range(int beg, int en);
template std::valarray<int> range(int beg, int en);
template std::valarray<double> range(int beg, int en);

template <class Container>
Container unite(const std::vector<Container> & ranges)
{
	int siz = 0;
	for(const auto & in : ranges)
	{
		siz += in.size();
	}
	Container res(siz);
	siz = 0;
	for(const auto & in : ranges)
	{
		std::copy(std::begin(in),
				  std::end(in),
				  std::begin(res) + siz);
		siz += in.size();
	}
	return res;
}
template std::vector<int> unite(const std::vector<std::vector<int>> & ranges);
template std::vector<double> unite(const std::vector<std::vector<double>> & ranges);
template std::valarray<int> unite(const std::vector<std::valarray<int>> & ranges);
template std::valarray<double> unite(const std::vector<std::valarray<double>> & ranges);


template <class Container>
double median(const Container & arr)
{
	auto arr2 = arr;
	std::sort(std::begin(arr2), std::end(arr2));
	if(arr2.size() % 2 == 1)
	{
		return arr2[arr2.size() / 2];
	}
//	else
	return 0.5 * (arr2[arr2.size() / 2] + arr2[arr2.size() / 2 - 1]);
}
template double median(const std::valarray<double> & arr);
template double median(const std::vector<double> & arr);
//template double median(const Container & arr);

template <> double min(const std::valarray<double> & arr) { return arr.min(); }
template <class Container>
double min(const Container & arr)
{
	double res = *std::begin(arr);
	std::for_each(std::begin(arr), std::end(arr), [&res](double in) { res = std::min(res, in); });
	return res;
}
template double min(const std::vector<double> & arr);

template <> double max(const std::valarray<double> & arr) { return arr.max(); }
template <class Container>
double max(const Container & arr)
{
	double res = *std::begin(arr);
	std::for_each(std::begin(arr), std::end(arr), [&res](double in) { res = std::max(res, in); });
	return res;
}
template double max(const std::vector<double> & arr);


template <class Typ>
Typ mean(const std::valarray<Typ> & arr)
{
	return arr.sum() / Typ(arr.size());
}
template double mean(const std::valarray<double> & arr);
template std::complex<double> mean(const std::valarray<std::complex<double>> & arr);

//template <class Typ>
//Typ mean(const std::vector<Typ> & arr)
//{
//	return std::accumulate(std::begin(arr), std::end(arr), Typ{0.}) / Typ(arr.size());
//}
//template double mean(const std::vector<double> & arr);
//template std::complex<double> mean(const std::vector<std::complex<double>> & arr);

template <class Container>
double covariance(const Container & arr1, const Container & arr2)
{
	return prod(std::valarray<double>(arr1 - mean(arr1)),
				std::valarray<double>(arr2 - mean(arr2)));
}
template double covariance(const std::valarray<double> & arr1, const std::valarray<double> & arr2);
//template double covariance(const std::vector<double> & arr1, const std::vector<double> & arr2);

template <class Container>
double correlation(const Container & arr1, const Container & arr2)
{
	return covariance(arr1, arr2) / (sigma(arr1) * sigma(arr2) * arr1.size());
}
template double correlation(const std::valarray<double> & arr1, const std::valarray<double> & arr2);
//template double correlation(const std::vector<double> & arr1, const std::vector<double> & arr2);


std::valarray<double> softmax(const std::valarray<double> & in)
{
	/// -1 for bias
	std::valarray<double> tmp = exp(in);
	double sum = std::accumulate(std::begin(tmp),
								 std::end(tmp) - 1,
								 0.);
	return tmp / sum; /// dont care about the last(bias)

}

template <typename Container>
Container contReverse(const Container & in)
{
	Container res{in};
	std::reverse_copy(std::begin(in), std::end(in), std::begin(res));
	return res;
}
template std::valarray<double> contReverse(const std::valarray<double> & in);
template std::vector<double> contReverse(const std::vector<double> & in);
template std::valarray<int> contReverse(const std::valarray<int> & in);
template std::vector<int> contReverse(const std::vector<int> & in);


template <typename Container>
Container contSubsec(const Container & in, int beg, int en)
{
	Container res(en - beg);
	std::copy(std::begin(in) + beg,
			  std::begin(in) + en,
			  std::begin(res));
	return res;
}
template std::vector<double> contSubsec(const std::vector<double> & in, int beg, int en);
template std::valarray<double> contSubsec(const std::valarray<double> & in, int beg, int en);

template <class Container>
Container contPopFront(const Container & in, int numOfPop)
{
	Container res(in.size() - numOfPop);
	std::copy(std::begin(in) + numOfPop, std::end(in), std::begin(res));
	return res;
}
template std::valarray<double> contPopFront(const std::valarray<double> & in, int numOfPop);
template std::vector<double> contPopFront(const std::vector<double> & in, int numOfPop);





#if 0
/// unused yet (23-Sep-2018)
template <> std::valarray<double> centered(const std::valarray<double> & arr)
{
	return arr - arr.sum() / arr.size();
}
template <class Container>
Container centered(const Container & arr)
{
	Container res(arr);
	auto mn = mean(arr);
	std::for_each(std::begin(res), std::end(res),
				  [mn](auto & in) { in -= mn; });
	return res;
}
template std::vector<double> centered(const std::vector<double> & arr);
#endif

template <class Container>
double sigmaToMean(const Container & arr)
{
	return sigma(arr) / mean(arr);
}
template double sigmaToMean(const std::valarray<double> & arr);
//template double sigmaToMean(const std::vector<double> & arr);

template <class Container>
double norma(const Container & in)
{
	return std::sqrt(normaSq(in));
}
template double norma(const std::valarray<double> & arr);
//template double norma(const std::vector<double> & arr);



template <class Container>
double prod(const Container & in1, const Container & in2)
{
	return std::inner_product(std::begin(in1),
							  std::end(in1),
							  std::begin(in2),
							  0.);
}
template double prod(const std::valarray<double> & in1, const std::valarray<double> & in2);
template double prod(const std::vector<double> & in1, const std::vector<double> & in2);

template <class Container>
double normaSq(const Container & in)
{
	return std::inner_product(std::begin(in),
							  std::end(in),
							  std::begin(in),
							  0.);
}
template double normaSq(const std::valarray<double> & in);
template double normaSq(const std::vector<double> & in);


template <class Container>
double variance(const Container & arr)
{
	return normaSq(std::valarray<double>(arr - mean(arr))) / arr.size();
}
template double variance(const std::valarray<double> & arr);
//template double variance(const std::vector<double> & arr);

template <class Container>
double sigma(const Container & arr)
{
	return std::sqrt(variance(arr));
}
template double sigma(const std::valarray<double> & arr);
//template double sigma(const std::vector<double> & arr);


template <typename Typ>
std::valarray<Typ> vecToValar(const std::vector<Typ> & in)
{
	std::valarray<Typ> res(in.size());
	std::copy(std::begin(in),
			  std::end(in),
			  std::begin(res));
	return res;
}
template std::valarray<double> vecToValar(const std::vector<double> & in);
template std::valarray<std::complex<double>> vecToValar(const std::vector<std::complex<double>> & in);

template <typename Typ>
std::vector<Typ> valarToVec(const std::valarray<Typ> & in)
{
	std::vector<Typ> res(in.size());
	std::copy(std::begin(in),
			  std::end(in),
			  std::begin(res));
	return res;
}
template std::vector<double> valarToVec(const std::valarray<double> & in);



template <typename Cont>
Cont mixed(int siz)
{
	Cont res(siz);
	std::iota(std::begin(res), std::end(res), 0);
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(std::begin(res),
				 std::end(res),
				 std::default_random_engine(seed));
	return res;
}
template std::vector<uint> mixed(int);

template <typename Container>
void mix (Container & in)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(std::begin(in),
				 std::end(in),
				 std::default_random_engine(seed));
}
template void mix(std::vector<uint> &);
template void mix(std::vector<int> &);

std::valarray<double> randomValar(int size)
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

std::valarray<double> abs(const std::valarray<std::complex<double>> & in)
{
	std::valarray<double> res(in.size());
	std::transform(std::begin(in),
				   std::end(in),
				   std::begin(res),
				   [](std::complex<double> a){ return std::abs(a); });
	return res;
}

std::valarray<double> valarErase(const std::valarray<double> & in, int index)
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

std::valarray<double> valarPushBack(const std::valarray<double> & in, double val)
{
	std::valarray<double> res(in);
	valarResize(res, res.size() + 1);
	res[in.size()] = val;
	return res;
}

template <typename Typ>
void valarResize(std::valarray<Typ> & in, int num)
{
	std::valarray<Typ> temp(in);
	in.resize(num);
	std::copy(std::begin(temp),
			  std::begin(temp) + std::min(in.size(), temp.size()),
			  std::begin(in));
}
template void valarResize(std::valarray<double> & in, int num);
template void valarResize(std::valarray<std::complex<double>> & in, int num);

} /// end of namespace smLib
