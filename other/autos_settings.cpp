#include <other/autos.h>

//#include <thread>
//#include <chrono>
#include <ios>
#include <iostream>
#include <bits/ios_base.h>
#include <functional>

#include <myLib/drw.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>
#include <myLib/output.h>
#include <myLib/statistics.h>

using namespace myOut;


namespace autos
{

/// filters Singleton
std::vector<filterFreqs> makeFilterVector(double start, double finish, double width)
{
	std::vector<filterFreqs> res{};
	for(double a = start; a + width <= finish; a += width)
	{
		res.push_back({a, a + width});
	}
	return res;
}

std::vector<feature> AutosSettings::getAutosMaskArray() const
{
	std::vector<feature> res{};
	for(int i = 0; i < 10; ++i)
	{
		int a = std::pow(2, i);
		if(a & this->autosMask) { res.push_back(feature(a)); }
	}
	return res;
}

} /// end namespase autos

