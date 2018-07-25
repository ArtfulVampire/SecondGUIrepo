#ifndef HIGHLEVEL_H
#define HIGHLEVEL_H

#include <other/matrix.h>
#include <vector>
#include <valarray>

namespace myLib
{

/// [separator][numFile][[data]]
std::vector<std::vector<matrix>> sliceData(const matrix & inData,
										   const std::vector<std::pair<int, int> > & markers,
										   const std::vector<int> & separators
										   = std::vector<int>{241, 247, 254});

/// vector(data, type, filename)
std::vector<std::tuple<matrix, uint, QString>> sliceWindows(
		const matrix & inData,
		const std::vector<std::pair<int, int> > & markers,
		const int windLen,
		const double overlapPart,
		const int numSkipStartWinds = 2,
		const int numBinsSkipRest = 0,
		const std::vector<int> & separators = std::vector<int>{241, 247, 254});

} /// end namespace myLib

#endif /// HIGHLEVEL_H
