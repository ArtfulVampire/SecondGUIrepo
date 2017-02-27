#include <myLib/valar.h>

namespace smLib
{

template <typename Cont>
Cont range(int beg, int en)
{
	Cont res(en - beg + 1);
	std::iota(std::begin(res), std::end(res), beg);
	return res;
}

template std::vector<int> range(int beg, int en);
template std::vector<double> range(int beg, int en);
template std::valarray<int> range(int beg, int en);
template std::valarray<double> range(int beg, int en);

template <typename Cont>
Cont unite(const std::vector<Cont> & ranges)
{
	int siz = 0;
	for(const auto & in : ranges)
	{
		siz += in.size();
	}
	Cont res(siz);
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






std::valarray<double> valarSubsec(const std::valarray<double> & in, int beg, int en)
{
	std::valarray<double> res(en - beg);
	std::copy(std::begin(in) + beg,
			  std::begin(in) + en,
			  std::begin(res));
	return res;
}


} // end of namespace
