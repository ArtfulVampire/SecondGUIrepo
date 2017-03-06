#include "classifier.h"

namespace clLib
{

std::valarray<double> oneHot(uint siz, uint hotIndex)
{
	std::valarray<double> res(siz);
	res[hotIndex] = 1;
	return res;
}

double countError(const std::valarray<double> & vals, int trueType)
{
	return sqrt(pow(vals - clLib::oneHot(vals.size(), trueType), 2.).sum());
}

} /// end of namespace clLib