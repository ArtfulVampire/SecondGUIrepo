#include <myLib/valar.h>

namespace smLib
{

template <typename Cont>
Cont range(int beg, int en)
{
	if(en <= beg) return Cont{};

	Cont res(en - beg);
	std::iota(std::begin(res), std::end(res), beg);
	return res;
}
template std::vector<uint> range(int beg, int en);
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

template <typename Cont>
Cont mixed(int num)
{
	Cont res(num);
	std::iota(std::begin(res), std::end(res), 0);
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(std::begin(res),
				 std::end(res),
				 std::default_random_engine(seed));
	return res;
}
template std::vector<uint> mixed(int);

template <typename Cont>
void mix (Cont & in)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(std::begin(in),
				 std::end(in),
				 std::default_random_engine(seed));
}
template void mix(std::vector<uint>&);

} // end of namespace
