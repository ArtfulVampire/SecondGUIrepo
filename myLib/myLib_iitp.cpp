#include "iitp.h"
#include <myLib/signalProcessing.h>


namespace iitp
{
std::complex<double> gFunc(const std::complex<double> & in)
{

	return (1 - pow(std::abs(in), 2)) * pow(atanh(std::abs(in)), 2) / std::norm(in);
}

std::valarray<std::complex<double>> crossSpectrum(
		const std::vector<std::valarray<double>> & in1,
		const std::vector<std::valarray<double>> & in2)
{
	if(in1.size() != in2.size())
	{
		std::cout << "crossSpectrum: data sizes inequal" << std::endl;
		return {};
	}
	const int siz = in1.size();

	/// determine fftLength
	int fftLen = 0;
	for(int i = 0; i < siz; ++i)
	{
		fftLen = std::max(smallLib::fftL(in1[i].size()),
						  fftLen);
		fftLen = std::max(smallLib::fftL(in2[i].size()),
						  fftLen);
	}

	/// count spectra
	std::valarray<std::complex<double>> spec1 = myLib::spectreRtoCcomplex(in1[0], fftLen);
	std::valarray<std::complex<double>> spec2 = myLib::spectreConj(
													myLib::spectreRtoCcomplex(in2[0], fftLen));
	std::valarray<std::complex<double>> res = spec1 * spec2; /// to determine size
	for(int i = 1; i < in1.size(); ++i)
	{
		spec1 = myLib::spectreRtoCcomplex(in1[i], fftLen);
		spec2 = myLib::spectreConj(myLib::spectreRtoCcomplex(in2[i], fftLen));
		res += spec1 * spec2;
	}
	res /= siz;
	return res;
}
} // namespace iitp
