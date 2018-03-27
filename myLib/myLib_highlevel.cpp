#include <myLib/highlevel.h>
#include <myLib/general.h>

namespace myLib
{

std::vector<std::vector<matrix>> sliceData(const matrix & inData,
										   const std::vector<std::pair<int, int>> & markers,
										   const std::vector<int> & separators)
{
	std::vector<std::vector<matrix>> res(separators.size());

	int sta = -1;
	int currMrk = -1;
	int prevMrk = -1;
	for(auto mrk : markers)
	{
		currMrk = myLib::indexOfVal(separators, mrk.second);

		if(currMrk == -1) { continue; }
		else
		{
			/// save real
			if(sta != -1) { res[prevMrk].push_back(inData.subCols(sta, mrk.first)); }

			/// set sta and currMrk
			prevMrk = currMrk;
			sta = mrk.first;
		}
	}

	/// add last rest from last 254 + 8 sec
	for(auto it = std::end(markers) - 1;  it != std::begin(markers); --it)
	{
		if((*it).second == 254)
		{
//			res[2].push_back(inData.subCols((*it).first, (*(it + 1)).first)); break;
			/// magic constant 8 seconds 250 srate
			res[2].push_back(inData.subCols((*it).first, (*it).first + 8 * 250.)); break;
		}
	}
	return res;
}

std::pair<std::vector<matrix>, std::vector<uint>> sliceWindows(
		const matrix & inData,
		const std::vector<std::pair<int, int> > & markers,
		const int windLen,
		const double overlapPart,
		const std::vector<int> & separators)
{
	const int windStep = windLen * (1. - overlapPart);
	const int maxNumWinds = (inData.cols() - windLen) / windStep;
	std::vector<matrix> res1{};	res1.reserve(maxNumWinds);
	std::vector<uint> res2{};	res2.reserve(maxNumWinds);


	int currType = 2; /// = int(fb::taskType::rest);
	int start = markers[0].first;
	for(const auto & mrk : markers)
	{
		int newType = myLib::indexOfVal(separators, mrk.second);
		if(newType == -1) { continue; }
		else
		{
			for(int windStart = start; windStart < mrk.first - windLen; windStart += windStep)
			{
				res1.push_back(inData.subCols(windStart, windStart + windLen));
				res2.push_back(currType);
			}
			currType = newType;
			start = mrk.first;
		}
	}
	return std::make_pair(res1, res2);
}

} // end namespace myLib