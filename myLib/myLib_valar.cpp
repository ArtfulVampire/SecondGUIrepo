#include <myLib/valar.h>

namespace smallLib
{

std::valarray<double> valarFromRange(int beg, int en)
{
	std::valarray<double> res(en - beg + 1);
	std::iota(std::begin(res), std::end(res), beg);
	return res;
}

std::valarray<double> valarSubsec(const std::valarray<double> & in, int beg, int en)
{
	std::valarray<double> res(en - beg);
	std::copy(std::begin(in) + beg,
			  std::begin(in) + en,
			  std::begin(res));
	return res;
}


} // end of namespace
