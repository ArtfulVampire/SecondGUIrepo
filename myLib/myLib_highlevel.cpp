#include <myLib/highlevel.h>
#include <myLib/general.h>
#include <myLib/output.h>
using namespace myOut;

namespace myLib
{

std::pair<std::vector<std::vector<matrix>>, std::vector<std::vector<int>>>
sliceData(const matrix & inData,
		  const std::vector<std::pair<int, int>> & markers,
		  const std::vector<int> & separators)
{
	std::vector<std::vector<matrix>> res(separators.size());
	std::vector<std::vector<int>> starts(separators.size());

	int sta = -1;
	int currMrk = -1;
	int prevMrk = -1;
	for(const auto & mrk : markers)
	{
		currMrk = myLib::indexOfVal(separators, mrk.second);

		if(currMrk == -1) { continue; } /// not interesting marker
		else
		{
			/// save real
			if(sta != -1 && prevMrk != -1)
			{
				res[prevMrk].push_back(inData.subCols(sta, mrk.first));
				starts[prevMrk].push_back(sta);
			}

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
#if 0
			/// all the rest period
			res[2].push_back(inData.subCols((*it).first, (*(it + 1)).first));
			 break;
#else
			/// rest period 8 seconds 250 srate - magic constant
			res[2].push_back(inData.subCols((*it).first, (*it).first + 8 * 250.));
			starts[2].push_back((*it).first);
			break;
#endif
		}
	}
	return {res, starts};
}

/// introduce into FBedf::FBedf(...), Net::successiveByEDFfinal, MainWindow::sliceWinds()
/// CHECK THIS FUNCTION
/// (data, type, name)
std::vector<std::tuple<matrix, uint, QString>> sliceWindows(
		const matrix & inData,
		const std::vector<std::pair<int, int> > & markers,
		const int windLen,
		const double overlapPart,
		const int numSkipStartWinds,
		const int numBinsSkipRest,
		const std::vector<int> & separators)
{
	const int windStep = windLen * (1. - overlapPart);
	const int maxNumWinds = (inData.cols() - windLen) / windStep;

	std::vector<std::tuple<matrix, uint, QString>> res{};
	res.reserve(maxNumWinds);

	int numReal = 0;

	/// iterator pointing at first task marker
	auto itSta = std::find_if(std::begin(markers),
							  std::end(markers),
							  [&separators](const auto & in)
	{ return myLib::contains(separators, in.second); });

	for(auto itMark = itSta + 1; itMark != std::end(markers); /*nothing*/++numReal)
	{
		/// find nearest task/rest marker
		auto itEnd = std::find_if(itMark,
								  std::end(markers),
								  [&separators](const auto & in)
		{ return myLib::contains(separators, in.second); });

		if(itEnd == std::end(markers)) { break; } /// end of file (last rest), slice later

		int typ = myLib::indexOfVal(separators, (*itSta).second);	/// type of current real
		QString marker = nm((*itSta).second);

		int windowCounter = numSkipStartWinds;
		int skipStart = numSkipStartWinds * windStep;
		if((*itSta).second == 254) { skipStart = numBinsSkipRest; } /// magic constant

		for(int startWind = (*itSta).first + skipStart;
			startWind < ((*itEnd).first - windLen);
			startWind += windStep)
		{
			QString appendName = "." + rn(numReal, 4)
								 + "_" + marker
								 + "." + rn(windowCounter++, 2);
			res.push_back(std::make_tuple(inData.subCols(startWind,
														 startWind + windLen),
										  uint(typ),
										  appendName)
						  );
		}

		itSta = itEnd;											/// new real/rest start
		itMark = itSta + 1;										/// new start to look for itEnd
	}

	/// slice last rest
	auto itLast = std::end(markers) - 1;
	for(; itLast != std::begin(markers); --itLast)
	{
		if((*itLast).second == 254) { break; } /// should be == itSta but not necessary
	}

	int windowCounter = 0;
	for(int startWind = (*itLast).first + numBinsSkipRest;
		startWind < std::min(int(inData.cols()),
							 (*itLast).first + 8 * 250) - windLen; /// 8 sec 250 Hz magic const
		startWind += windStep)
	{
		QString appendName = "." + rn(numReal, 4)
							 + "_" + "254"
							 + "." + rn(windowCounter++, 2);
		res.push_back(std::make_tuple(inData.subCols(startWind,
													 startWind + windLen),
									  2,
									  appendName)
					  );
	}
	return res;
}

} /// end namespace myLib