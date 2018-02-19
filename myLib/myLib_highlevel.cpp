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

	/// add last rest
	for(auto it = std::end(markers) - 1;  it != std::begin(markers); --it)
	{
		if((*it).second == 254)
		{
			res[2].push_back(inData.subCols((*it).first, (*(it + 1)).first)); break;
		}
	}
	return res;
}

} // end namespace myLib