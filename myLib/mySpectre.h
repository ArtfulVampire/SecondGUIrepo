#ifndef MYSPECTRE_H
#define MYSPECTRE_H

#include <complex>
#include <valarray>
#include <myLib/signalProcessing.h>
#include <myLib/small.h>

namespace myLib
{

class mySpectre
{
private:
	std::valarray<std::complex<double>> spec; /// complex
	std::valarray<double> psd; /// real

	const std::valarray<double> sig; /// just copy
//	std::valarray<double> filt; /// filtered

	double spStep = 0.;
	int fftLength = -1;
	double srate = 250.;
	int eyes = 0;
	mySpectre() = default;


public:
	mySpectre(const std::valarray<double> & inData,
			  double inSrate = 250,
			  int inFftLength = -1,
			  int inEyes = 0) : sig(inData), srate(inSrate), fftLength(inFftLength), eyes(inEyes)
	{
		if(fftLength == -1) fftLength = smLib::fftL(inData.size());
		spStep = srate / fftLength;

		spec = myLib::spectreRtoCcomplex(sig, fftLength);
		spec *=  sqrt(2. / ((fftLength - eyes) * srate)); /// check norm

		psd.resize(spec.size());
		std::transform(std::begin(spec),
					   std::end(spec),
					   std::begin(psd),
					   [](std::complex<double> in)
		{
			return std::abs(in);
		};

	}


	const std::valarray<std::complex<double>> & getSpectre() const {return spec;}
	const std::valarray<double> & getPsd() const {return psd;}
	const std::valarray<double> & getSignal() const {return sig;}
//	const std::valarray<double> & getFiltered() {return filt;}

	std::valarray<std::complex<double>> getConj() const
	{
		auto res = spec;
		res.apply(std::conj);
		return res;
	}

//	void filterBand(double lowFreq, double highFreq);
//	void filterNotch(double lowFreq, double highFreq);

	const std::complex<double> & operator[](double freq) const
	{
		return spec(std::round(freq/spStep));
	}
};

}

#endif // MYSPECTRE_H
