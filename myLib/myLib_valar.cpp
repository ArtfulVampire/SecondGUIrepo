#include <myLib/valar.h>

namespace smallLib
{

std::valarray<double> valarFromRange(int beg, int en)
{
	std::valarray<double> res(en - beg + 1);
	std::iota(std::begin(res), std::end(res), beg);
	return res;
}


} // end of namespace
