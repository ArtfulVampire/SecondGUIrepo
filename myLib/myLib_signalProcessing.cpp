#include <myLib/signalProcessing.h>


#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <myLib/valar.h>
#include <other/defs.h>

#include <QPixmap>
#include <QPainter>

using namespace myOut;

namespace butter
{

const int ORDER_band = 48;
const int ORDER_cut = 32;

std::valarray<double> butterworthBandStop(const std::valarray<double> & in,
										  double fLow,
										  double fHigh,
										  double srate,
										  int n)
{
	//n = filter order 4,8,12,...
	//s = sampling frequency
	//f1 = upper half power frequency
	//f2 = lower half power frequency


	if(n % 4 != 0)
	{
		std::cout << "Order must be 4,8,12,16,..." << std::endl;
		return {};
	}

	double a = std::cos(M_PI * (fHigh + fLow) / srate) /
			   std::cos(M_PI * (fHigh - fLow) / srate);
	double a2 = std::pow(a, 2);

	double b = std::tan(M_PI * (fHigh - fLow) / srate);
	double b2 = std::pow(b, 2);


	n /= 4;
	std::valarray<double> A(n);
	std::valarray<double> d1(n);
	std::valarray<double> d2(n);
	std::valarray<double> d3(n);
	std::valarray<double> d4(n);
	std::valarray<double> w0(n);
	std::valarray<double> w1(n);
	std::valarray<double> w2(n);
	std::valarray<double> w3(n);
	std::valarray<double> w4(n);
	double r;

	for(int i = 0; i < n; ++i)
	{
		r = sin(M_PI * (2.0 * i + 1.0) / (4.0 * n));
		srate = b2 + 2.0 * b * r + 1.0;

		A[i] = 1.0 / srate;
		d1[i] = 4.0 * a * (1.0 + b * r) / srate;
		d2[i] = 2.0 * (b2 - 2.0 * a2 - 1.0) / srate;
		d3[i] = 4.0 * a * (1.0 - b * r) / srate;
		d4[i] = -(b2 - 2.0 * b * r + 1.0) / srate;
	}
	r = 4.0 * a;
	srate = 4.0 * a2 + 2.0;

	std::valarray<double> res(in.size());
	int count = 0;
	for(double x : in)
	{
		for(int i = 0; i < n; ++i)
		{
			w0[i] = d1[i] * w1[i] + d2[i] * w2[i]+ d3[i] * w3[i]+ d4[i] * w4[i] + x;
			x = A[i] * (w0[i] - r * w1[i] + srate * w2[i]- r * w3[i] + w4[i]);

			w4[i] = w3[i];
			w3[i] = w2[i];
			w2[i] = w1[i];
			w1[i] = w0[i];
		}
		res[count++] = x;
	}

	return res;
}


std::valarray<double> butterworthBandPass(const std::valarray<double> & in,
										  double fLow,
										  double fHigh,
										  double srate,
										  int n)
{
	///	Butterworth bandpass filter
	///	n = filter order 4,8,12
	///	s = sampling frequency
	///	f1 = upper half power frequency
	///	fLow = lower half power frequency

	if(n % 4 != 0)
	{
		std::cout << "Order must be 4,8,12,16,..." << std::endl;
		return {};
	}

	const double a = std::cos(M_PI * (fHigh + fLow) / srate) /
			   std::cos(M_PI * (fHigh - fLow) / srate);
	const double a2 = std::pow(a, 2);

	const double b = std::tan(M_PI * (fHigh - fLow) / srate);
	const double b2 = std::pow(b, 2);


	n /= 4;
	std::valarray<double> A(n);
	std::valarray<double> d1(n);
	std::valarray<double> d2(n);
	std::valarray<double> d3(n);
	std::valarray<double> d4(n);
	std::valarray<double> w0(n);
	std::valarray<double> w1(n);
	std::valarray<double> w2(n);
	std::valarray<double> w3(n);
	std::valarray<double> w4(n);

	double r;

	for(int i = 0; i < n; ++i)
	{
		r = std::sin(M_PI * (2.0 * i + 1.0) / (4.0 * n));
		double val = b2 + 2.0 * b * r + 1.0;

		A[i] = b2 / val;
		d1[i] = 4.0 * a * (1.0 + b * r) / val;
		d2[i] = 2.0 * (b2 - 2.0 * a2 - 1.0) / val;
		d3[i] = 4.0 * a * (1.0 - b * r) / val;
		d4[i] = -(b2 - 2.0 * b * r + 1.0) / val;
	}

	std::valarray<double> res(in.size());
	for(uint num = 0; num < in.size(); ++num)
	{
		double x = in[num];

		for(int i = 0; i < n; ++i)
		{
			w0[i] = d1[i] * w1[i] + d2[i] * w2[i]+ d3[i] * w3[i]+ d4[i] * w4[i] + x;
			x = A[i] * (w0[i] - 2.0 * w2[i] + w4[i]);

			w4[i] = w3[i];
			w3[i] = w2[i];
			w2[i] = w1[i];
			w1[i] = w0[i];
		}
		res[num] = x;
	}
	return res;
}

std::valarray<double> butterworthHighPass(const std::valarray<double> & in,
										  double cutoff,
										  double srate,
										  int n)
{
	///	Butterworth highpass filter
	///	n = filter order 2,4,6
	///	srate = sampling frequency
	///	cutoff = half power frequency

	if(n % 2 != 0)
	{
		std::cout << "Order must be 2,4,6,..." << std::endl;
		return {};
	}

	double a = std::tan(M_PI * cutoff / srate);
	double a2 = std::pow(a, 2);

	n /= 2;
	std::valarray<double> A(n);
	std::valarray<double> d1(n);
	std::valarray<double> d2(n);
	std::valarray<double> w0(n);
	std::valarray<double> w1(n);
	std::valarray<double> w2(n);
	double r;

	for(int i = 0; i < n; ++i)
	{
		r = std::sin(M_PI * (2.0 * i + 1.0) / (4.0 * n));
		srate = a2 + 2.0 * a * r + 1.0;
		A[i] = 1.0 / srate;
		d1[i] = 2.0 * (1 - a2) / srate;
		d2[i] = -(a2 - 2.0 * a * r + 1.0) / srate;
	}

	std::valarray<double> res(in.size());
	int count = 0;
	for(double x : in)
	{
		for(int i = 0; i < n; ++i)
		{
			w0[i] = d1[i] * w1[i] + d2[i] * w2[i] + x;
			x = A[i] * (w0[i] - 2.0 * w1[i] + w2[i]);

			w2[i] = w1[i];
			w1[i] = w0[i];
		}
		res[count++] = x;
	}
	return res;
}

std::valarray<double> butterworthLowPass(const std::valarray<double> & in,
										 double cutoff,
										 double srate,
										 int n)
{
	///	Butterworth lowpass filter
	///	n = filter order 2,4,6
	///	srate = sampling frequency
	///	cutoff = half power frequency

	if(n % 2 != 0)
	{
		std::cout << "Order must be 2,4,6,..." << std::endl;
		return {};
	}

	double a = std::tan(M_PI * cutoff / srate);
	double a2 = std::pow(a, 2);


	n /= 2;
	std::valarray<double> A(n);
	std::valarray<double> d1(n);
	std::valarray<double> d2(n);
	std::valarray<double> w0(n);
	std::valarray<double> w1(n);
	std::valarray<double> w2(n);
	double r;

	for(int i = 0; i < n; ++i)
	{
		r = std::sin(M_PI * (2.0 * i + 1.0) / (4.0 * n));
		srate = a2 + 2.0 * a * r + 1.0;
		A[i] = a2 / srate;
		d1[i] = 2.0 * (1 - a2) / srate;
		d2[i] = -(a2 - 2.0 * a * r + 1.0) / srate;
	}

	std::valarray<double> res(in.size());
	int count = 0;
	for(double x : in)
	{
		for(int i = 0; i < n; ++i)
		{
			w0[i] = d1[i] * w1[i] + d2[i] * w2[i] + x;
			x = A[i] * (w0[i] + 2.0 * w1[i] + w2[i]);

			w2[i] = w1[i];
			w1[i] = w0[i];
		}
		res[count++] = x;
	}
	return res;
}

std::valarray<double> butterworthBandStopTwoSided(const std::valarray<double> & in,
												  double fLow,
												  double fHigh,
												  double srate)
{
	std::valarray<double> res1 = butterworthBandStop(in, fLow, fHigh, srate);
	std::valarray<double> res2 = smLib::contReverse(res1);
	res1 = butterworthBandStop(res2, fLow, fHigh, srate);
	return smLib::contReverse(res1);
}

std::valarray<double> butterworthBandPassTwoSided(const std::valarray<double> & in,
												  double fLow,
												  double fHigh,
												  double srate)
{
	if(fLow == 0. && fHigh == 0.) { return in; }
	std::valarray<double> res1 = butterworthBandPass(in, fLow, fHigh, srate);
	std::valarray<double> res2 = smLib::contReverse(res1);
	res1 = butterworthBandPass(res2, fLow, fHigh, srate);
	return smLib::contReverse(res1);
}

std::valarray<double> butterworthHighPassTwoSided(const std::valarray<double> & in,
												  double cutoff,
												  double srate)
{
	std::valarray<double> res1 = butterworthHighPass(in, cutoff, srate);
	std::valarray<double> res2 = smLib::contReverse(res1);
	res1 = butterworthHighPass(res2, cutoff, srate);
	return smLib::contReverse(res1);
}

std::valarray<double> butterworthLowPassTwoSided(const std::valarray<double> & in,
												  double cutoff,
												  double srate)
{
	std::valarray<double> res1 = butterworthLowPass(in, cutoff, srate);
	std::valarray<double> res2 = smLib::contReverse(res1);
	res1 = butterworthLowPass(res2, cutoff, srate);
	return smLib::contReverse(res1);
}

std::valarray<double> refilter(const std::valarray<double> & inputSignal,
							   double lowFreq,
							   double highFreq,
							   bool isNotch,
							   double srate)
{
	if(isNotch)
	{
		return butter::butterworthBandStopTwoSided(inputSignal,
												   lowFreq,
												   highFreq,
												   srate);
	}
	/// else
	return butter::butterworthBandPassTwoSided(inputSignal,
											   lowFreq,
											   highFreq,
											   srate);

}

} /// end of namespace butter

namespace btr
{
std::valarray<double> butterworth(const std::valarray<double> & in,
								  int order,
								  double srate,
								  double centerFreq,
								  double halfBand)
{
	std::valarray<double> res(in.size());

	std::complex<double> U, P;
	std::vector<std::complex<double>> B;
	std::vector<std::complex<double>> G;

	B.resize(order + 1);
	G.resize(order + 1);
	double timeBin = 1. / srate;

	auto U0 = 2 * pi * halfBand * timeBin;
	U = U0;
	auto U1 = 1. - exp(-U0);

	for(int i = 1; i <= order; ++i)
	{
		P = exp(std::complex<double>(0., (pi * (order + 2 * i - 1)) / (2 * order)))
				+ std::complex<double>(0., centerFreq / halfBand);
		B[i] = exp(U * P);
		G[i] = std::complex<double>(0., 0.);
	}

	for(uint i = 1; i <= in.size(); ++i)
	{
		G[1] = U1 * in[i - 1] + B[1] * G[1];


		for(int j = 2; j <= order; ++j)
		{
			G[j] = G[j - 1] * U1 + B[j] * G[j];
		}
		res[i - 1] = G[order].real();
	}
	return res;
}
std::valarray<double> refilterButter(const std::valarray<double> & in,
									 int order,
									 double srate,
									 double lowFreq,
									 double highFreq)
{
	std::valarray<double> res(in.size());
	res = btr::butterworth(in, order, srate, (lowFreq + highFreq) / 2., (highFreq - lowFreq) / 2.);
	res = smLib::contReverse(res);
	res = btr::butterworth(res, order, srate, (lowFreq + highFreq) / 2., (highFreq - lowFreq) / 2.);
	res = smLib::contReverse(res);
	return res;
}
} /// end of namespace btr

#if 0
namespace myDsp
{
std::valarray<double> lowPassOneSide(const std::valarray<double> & inputSignal,
									 double cutoffFreq,
									 double srate)
{
	const int order = 10;

	double * tempArr;
	try
	{
		 tempArr = new double [inputSignal.size()];
	}
	catch(std::bad_alloc)
	{
		std::cout << "bad alloc array" << std::endl;
		exit(0);
	}
	catch(...)
	{
		throw;
	}
	std::copy(std::begin(inputSignal), std::end(inputSignal), tempArr);


	Dsp::Params params;
	params[0] = srate; /// sample rate
	params[1] = order; /// order
	params[2] = cutoffFreq; /// cutoff


	Dsp::Filter * f;
	try
	{
		f = new Dsp::FilterDesign
			<Dsp::Butterworth::Design::LowPass <order>, 1, Dsp::DirectFormII>;
	}
	catch(std::bad_alloc)
	{
		std::cout << "bad alloc filter" << std::endl;
		exit(0);
	}
	catch(...)
	{
		throw;
	}
	f->setParams(params);
	f->process(inputSignal.size(), &tempArr);

	std::valarray<double> res(inputSignal.size());
	std::copy(tempArr-1, tempArr + inputSignal.size()-1, std::begin(res));


	delete f;
	delete[] tempArr;

	return res;
}

std::valarray<double> lowPass(const std::valarray<double> & inputSignal,
							  double cutoffFreq,
							  double srate)
{
	std::valarray<double> tmp = lowPassOneSide(inputSignal,
											   cutoffFreq,
											   srate);
	tmp = smLib::reverseArray(tmp);
	tmp = lowPassOneSide(tmp,
						 cutoffFreq,
						 srate);

	/// reverse back
	tmp = smLib::reverseArray(tmp);
	return tmp;
}

std::valarray<double> refilterOneSide(const std::valarray<double> & inputSignal,
									  double lowFreq,
									  double highFreq,
									  bool isNotch,
									  double srate)
{
	const int order = 6;

	double * tempArr= new double [inputSignal.size()];
	std::copy(std::begin(inputSignal), std::end(inputSignal), tempArr);

	Dsp::Params params;
	params[0] = srate; /// sample rate
	params[1] = order; /// order
	params[2] = (lowFreq + highFreq) / 2; /// center frequency
	params[3] = highFreq - lowFreq; /// band width

	Dsp::Filter * f;
	if(isNotch)
	{
		f = new Dsp::FilterDesign
		<Dsp::Butterworth::Design::BandStop <order>, 1, Dsp::DirectFormII>;
	}
	else
	{
		f = new Dsp::FilterDesign
		<Dsp::Butterworth::Design::BandPass <order>, 1, Dsp::DirectFormII>;
	}
	f->setParams(params);
	f->process(inputSignal.size(), &tempArr);

	std::valarray<double> res(inputSignal.size());
	std::copy(tempArr, tempArr + inputSignal.size(), std::begin(res));
	delete f;
	delete[] tempArr;

	return res;
}

std::valarray<double> refilter(const std::valarray<double> & inputSignal,
							   double lowFreq,
							   double highFreq,
							   bool isNotch,
							   double srate)
{
	std::valarray<double> tmp = refilterOneSide(inputSignal,
												lowFreq,
												highFreq,
												isNotch,
												srate);
	tmp = smLib::reverseArray(tmp);

	tmp = refilterOneSide(tmp,
						  lowFreq,
						  highFreq,
						  isNotch,
						  srate);
	/// reverse back
	tmp = smLib::reverseArray(tmp);
	return tmp;
}

} /// end namespace myDsp
#endif

namespace myLib
{

void four1(double * dataF, int nn, int isign)
{
    int n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    double tempr,tempi;

	n = nn << 1; /// n = 2 * fftLength
    j = 1;
    for (i = 1; i < n; i += 2)
    {
        if (j > i)
        {
            std::swap(dataF[j], dataF[i]);
			std::swap(dataF[j + 1], dataF[i + 1]);
        }
        m = n >> 1; /// m = n / 2;
        while (m >= 2 && j > m)
        {
            j -= m;
			m >>= 1; /// m /= 2;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax)
    {
		istep = mmax << 1; /// istep = mmax * 2;
        theta = isign * (2 * pi / mmax);
        wtemp = sin(0.5 * theta);

        wpr = - 2.0 * wtemp * wtemp;
        wpi = sin(theta);

        wr = 1.0;
        wi = 0.0;
        for(m = 1; m < mmax; m += 2)
        {
            for(i = m; i <= n; i += istep)
            {
                j = i + mmax;

                /// tempCompl = wCompl * dataFCompl[j/2]
                tempr = wr * dataF[j] - wi * dataF[j + 1];
                tempi = wr * dataF[j + 1] + wi * dataF[j];

                /// dataFCompl[j/2] = dataFCompl[i/2] - tempCompl
                dataF[j] = dataF[i] - tempr;
                dataF[j + 1] = dataF[i + 1] - tempi;

                /// dataFCompl[i/2] += tempCompl
                dataF[i] += tempr;
                dataF[i + 1] += tempi;
            }
            wr = (wtemp = wr) * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax = istep;
    }
}

std::valarray<double> spectreRtoR(const std::valarray<double> & inputSignal,
								  int fftLen)
{
	if(fftLen <= 0)
	{
		fftLen = smLib::fftL(inputSignal.size());
	}

    double * pew = new double [2 * fftLen];
	std::fill(pew, pew + 2 * fftLen, 0.);

	/// hope, size() is not too big
	for(int i = 0; i < std::min(fftLen, static_cast<int>(inputSignal.size())); ++i)
    {
		pew[2 * i] = inputSignal[i];
    }

	four1(pew - 1, fftLen, 1);

	std::valarray<double> res(fftLen);
    for(int i = 0; i < fftLen; ++i)
    {
		res[i] = (pew[2 * i] * pew[2 * i] + pew[2 * i + 1] * pew[2 * i + 1]) ;
	}

	delete[] pew;
    return res;
}

std::valarray<std::complex<double>> spectreWelchRtoC(const std::valarray<double> & inputSignal,
													 double overlap,
													 double srate,
													 myLib::windowName window,
													 int fftLen)
{
	using specType = std::valarray<std::complex<double>>;

	const int windStep = fftLen * (1. - overlap);
	const std::valarray<double> wnd = myLib::fftWindow(fftLen, window);

	specType res;
	int num = 0;
	for(uint windStart = 0;
		windStart < inputSignal.size() - fftLen;
		windStart += windStep, ++num)
	{
		std::valarray<double> part1 = smLib::contSubsec(inputSignal,
														 windStart,
														 windStart + fftLen) * wnd;
		const specType spec1 = myLib::spectreRtoC2(part1, fftLen, srate);
		specType tmp = spec1 * spec1.apply(std::conj);
		if(res.size() != tmp.size()) { res.resize(tmp.size()); }
		res += tmp;
	}
	res /= num;

	return res;
}

std::valarray<double> spectreWelchRtoR(const std::valarray<double> & inputSignal,
									   double overlap,
									   double srate,
									   myLib::windowName window,
									   int fftLen)
{
	return smLib::abs(spectreWelchRtoC(inputSignal,
									   overlap,
									   srate,
									   window,
									   fftLen));
}

std::valarray<std::complex<double>> spectreCross(const std::valarray<double> & inputSignal1,
												 const std::valarray<double> & inputSignal2,
												 double srate,
												 const std::valarray<double> & wnd,
												 int fftLen)
{
	if(inputSignal1.size() != inputSignal2.size())
	{
		return {};
	}
	using specType = std::valarray<std::complex<double>>;

	const specType spec1 = myLib::spectreRtoC2(inputSignal1 * wnd, fftLen, srate);
	const specType spec2 = myLib::spectreRtoC2(inputSignal2 * wnd, fftLen, srate);
	specType res = spec1 * spec2.apply(std::conj);
	return res;
}

std::valarray<std::complex<double>> spectreWelchCross(const std::valarray<double> & inputSignal1,
													  const std::valarray<double> & inputSignal2,
													  double overlap,
													  double srate,
													  myLib::windowName window,
													  int fftLen)
{
	if(inputSignal1.size() != inputSignal2.size())
	{
		return {};
	}

	using specType = std::valarray<std::complex<double>>;

	const int windStep = fftLen * (1. - overlap);
	const std::valarray<double> wnd = myLib::fftWindow(fftLen, window);

	specType res;
	int num = 0;
	for(uint windStart = 0;
		windStart < inputSignal1.size() - fftLen;
		windStart += windStep, ++num)
	{
		std::valarray<double> part1 = smLib::contSubsec(inputSignal1,
														 windStart,
														 windStart + fftLen) * wnd;
		std::valarray<double> part2 = smLib::contSubsec(inputSignal2,
														 windStart,
														 windStart + fftLen) * wnd;
		const specType spec1 = myLib::spectreRtoC2(part1, fftLen, srate);
		const specType spec2 = myLib::spectreRtoC2(part2, fftLen, srate);
		specType tmp = spec1 * spec2.apply(std::conj);
		if(res.size() != tmp.size()) { res.resize(tmp.size()); }
		res += tmp;
	}
	res /= num;

	return res;
}

std::valarray<double> spectreRtoC(const std::valarray<double> & inputSignal,
								  int fftLen)
{
	if(fftLen <= 0)
	{
		fftLen = smLib::fftL(inputSignal.size());
	}

	double * pew = new double [2 * fftLen];
	std::fill(pew, pew + 2 * fftLen, 0.);

	/// hope, size() is not too big
	for(int i = 0; i < std::min(fftLen, static_cast<int>(inputSignal.size())); ++i)
	{
		pew[2 * i] = inputSignal[i];
	}

	four1(pew - 1, fftLen, 1);

	std::valarray<double> res(2 * fftLen);
	std::copy(pew, pew + 2 * fftLen, std::begin(res));

	delete[] pew;
	return res;
}

/// return size = fftLen
std::valarray<std::complex<double>> spectreRtoC2(const std::valarray<double> & inputSignal,
												 int fftLen,
												 double srate)
{
	if(fftLen <= 0)
	{
		fftLen = smLib::fftL(inputSignal.size());
	}

	double * pew = new double [2 * fftLen];
	std::fill(pew, pew + 2 * fftLen, 0.);

	/// hope, size() is not too big
	for(int i = 0; i < std::min(fftLen, static_cast<int>(inputSignal.size())); ++i)
	{
		pew[2 * i] = inputSignal[i];
	}

	four1(pew - 1, fftLen, 1);

	const double norm = std::sqrt(myLib::spectreNorm(fftLen,
													 inputSignal.size(),
													 srate));

	std::valarray<std::complex<double>> res(fftLen);
	for(uint i = 0; i < res.size(); ++i)
	{
		res[i] = std::complex<double>(pew[2 * i], pew[2 * i + 1]);
	}
	res *= std::complex<double>(norm, 0.);

	delete[] pew;
	return res;
}

std::valarray<double> spectreCtoR(const std::valarray<double> & inputSignal,
								  int fftLen)
{
	{
		std::cout << "spectreCtoR: should deal with fftLen, return" <<std::endl;
		return {};
	}

	if(fftLen <= 0)
	{
		fftLen = smLib::fftL(inputSignal.size());
	}

	double * pew = new double [2 * fftLen];
	std::fill(pew, pew + 2 * fftLen, 0.);

	for(uint i = 0; i < inputSignal.size(); ++i)
	{
		pew[i] = inputSignal[i];
	}

	four1(pew - 1, fftLen, 1);

	std::valarray<double> res(fftLen);
	for(int i = 0; i < fftLen; ++i)
	{
		res[i] = (pew[2 * i] * pew[2 * i] + pew[2 * i + 1] * pew[2 * i + 1]);
	}
	delete []pew;

	return res;
}

/// complex signal of fftLen = 2*fftLen numbers
std::valarray<double> spectreCtoC(const std::valarray<double> & inputSignal,
								  int fftLen) /// what is fftLen here?
{
	if(fftLen <= 0)
	{
		fftLen = smLib::fftL(inputSignal.size() / 2);
	}

	double * pew = new double [2 * fftLen];
	std::fill(pew, pew + 2 * fftLen, 0.);

	for(uint i = 0; i < inputSignal.size(); ++i)
	{
		pew[i] = inputSignal[i];
	}

	four1(pew - 1, fftLen, 1);

	std::valarray<double> res(2 * fftLen);
	std::copy(pew, pew + 2 * fftLen, std::begin(res));

	delete []pew;
	return res;
}


std::valarray<double> subSpectrumR(const std::valarray<double> & inputSpectre,
								  double leftFreq,
								  double rightFreq,
								  double srate)
{
	double a = log2(inputSpectre.size());
	if(a != static_cast<double>(static_cast<int>(a)))
	{
		std::cout << "subSpectrum: inputSpectre.size() is not power of 2" << std::endl;
		return {};
	}
	int fftLen = inputSpectre.size();

	int left = smLib::fftLimit(leftFreq, srate, fftLen);
	int right = smLib::fftLimit(rightFreq, srate, fftLen) + 1;

	std::valarray<double> res(right - left);
	std::copy(std::begin(inputSpectre) + left,
			  std::begin(inputSpectre) + right,
			  std::begin(res));
	return res;
}

std::valarray<double> spectreCtoRrev(const std::valarray<double> & inputSpectre)
{
	/// check size?
	int fftLen = inputSpectre.size() / 2;
	double * pew = new double [inputSpectre.size()];
	std::fill(pew, pew + 2 * fftLen, 0.);
	std::copy(std::begin(inputSpectre), std::end(inputSpectre), pew);

	four1(pew - 1, fftLen, -1);

	std::valarray<double> res(fftLen);
	for(uint i = 0; i < res.size(); ++i)
	{
		res[i] = pew[2 * i];
	}
	res /= fftLen;

	delete[] pew;
	return res;
}

std::valarray<double> spectreCtoRrev(const std::valarray<std::complex<double>> & inputSpectre)
{
	/// check size == std::pow(2, N) ?
	int fftLen = inputSpectre.size();

	double * pew = new double [inputSpectre.size() * 2];
	for(uint i = 0; i < inputSpectre.size(); ++i)
	{
		pew[2 * i] = inputSpectre[i].real();
		pew[2 * i + 1] = inputSpectre[i].imag();
	}

	four1(pew - 1, fftLen, -1);

	std::valarray<double> res(fftLen);
	for(uint i = 0; i < res.size(); ++i)
	{
		res[i] = pew[2 * i];
	}
	res /= fftLen;

	delete[] pew;
	return res;
}


std::valarray<double> spectreCtoCrev(const std::valarray<double> & inputSpectre)
{
	/// check size?
	int fftLen = inputSpectre.size() / 2;
	double * pew = new double [inputSpectre.size()];
	std::fill(pew, pew + 2 * fftLen, 0.);
	std::copy(std::begin(inputSpectre), std::end(inputSpectre), pew);

	four1(pew - 1, fftLen, -1);
	std::valarray<double> res(2 * fftLen);
	std::copy(pew, pew + 2 * fftLen, std::begin(res));
	res /= fftLen;


	delete[] pew;
	return res;
}

double spectreNorm(int fftLen, int realSig, double srate)
{
	return 2. / (std::min(realSig, fftLen) * srate);
}

/// works
std::valarray<double> refilterFFT(const std::valarray<double> & inputSignal,
								  double lowFreq,
								  double highFreq,
								  bool isNotch,
								  double srate)
{
	int fftLen = smLib::fftL(inputSignal.size());
	std::valarray<double> spectr = spectreRtoC(inputSignal, fftLen);
	const double spStep = srate / fftLen;
	/// 2 because complex values
	const int lowLim = 2. * std::floor( lowFreq / spStep);
	const int highLim = 2. * std::ceil( highFreq / spStep);

	refilterSpectre(spectr, lowLim, highLim, isNotch);

	/// constant component
	spectr[0] = 0.;
	spectr[1] = 0.;
	spectr[fftLen] = 0.;
	spectr[fftLen + 1] = 0.;

	return spectreCtoRrev(spectr);
}


std::valarray<double> lowPassFFT(const std::valarray<double> & inputSignal,
								 double cutoffFreq,
								 double srate)
{
	return refilterFFT(inputSignal, cutoffFreq, srate / 2., true, srate);
}


matrix refilterMat(const matrix & inputMatrix,
				   double lowFreq,
				   double highFreq,
				   bool isNotch,
				   double srate,
				   bool notTheLast)
{
	matrix res = matrix();
	for(int i = 0; i < inputMatrix.rows() - static_cast<int>(notTheLast); ++i)
	{
		res.push_back(myLib::refilter(inputMatrix[i],
									  lowFreq,
									  highFreq,
									  isNotch,
									  srate));
	}
	if(notTheLast) { res.push_back(inputMatrix.back()); }
	return res;
}


std::valarray<double> upsample(const std::valarray<double> & inSignal,
							   double oldFreq,
							   double newFreq)
{
	int rat = newFreq / oldFreq;
	std::valarray<double> res(inSignal.size() * rat);
	for(uint i = 0; i < inSignal.size(); ++i)
	{
		res[i * rat] = inSignal[i];
	}

	res = myLib::lowPass(res,
						 oldFreq / 2, /// whaaaaaaat
						 newFreq);
	res *= rat;
	return res;
}


std::valarray<double> downsample(const std::valarray<double> & inSignal,
								 double oldFreq,
								 double newFreq)
{
	int rat = oldFreq / newFreq;
	std::valarray<double> res = myLib::lowPass(inSignal, newFreq / 2, oldFreq);

	for(uint i = 0; i < inSignal.size() / rat; ++i)
	{
		res[i] = res[i * rat];
	}
	smLib::valarResize(res, inSignal.size() / rat);
	return res;
}

int findJump(const std::valarray<double> & inSignal, int startSearch, double numOfSigmas)
{
	const int lenForSigma = 100;
	for(uint i = startSearch; i < inSignal.size() - 1; ++i)
	{
		if(std::abs(inSignal[i + 1] - inSignal[i]) >
		   numOfSigmas * smLib::sigma(std::valarray<double>(
										  inSignal[std::slice(i - lenForSigma, lenForSigma, 1)] )
									  )
		   )
		{
			return i;
		}
	}
	return -1;
}

void refilterSpectre(std::valarray<double> & spectr,
					 int lowLim,
					 int highLim,
					 bool isNotch)
{
	/// generality
	int fftLen = spectr.size() / 2;

	/// both lowLim and highLim are in/ex
	if(!isNotch)
	{
		std::fill(std::begin(spectr),
				  std::begin(spectr) + lowLim,
				  0.);
		std::fill(std::begin(spectr) + highLim,
				  std::begin(spectr) + fftLen,
				  0.);
		std::fill(std::begin(spectr) + fftLen,
				  std::begin(spectr) + 2 * fftLen + 2 - highLim,
				  0.);
		std::fill(std::begin(spectr) + 2 * fftLen + 2 - lowLim,
				  std::begin(spectr) + 2 * fftLen,
				  0.);
	}
	else
	{
		std::fill(std::begin(spectr) + lowLim,
				  std::begin(spectr) + highLim,
				  0.);
		std::fill(std::begin(spectr) + 2 * fftLen + 2 - highLim ,
				  std::begin(spectr) + 2 * fftLen + 2 - lowLim,
				  0.);
	}
}



/// Rosetta stone - to check
std::valarray<std::complex<double>> spectreCtoCcomplex(
		const std::valarray<std::complex<double>> & inputArray,
		int fftLen)
{
	/// prepare
	if(fftLen <= 0)
	{
		fftLen = smLib::fftL(inputArray.size());
	}
	std::valarray<std::complex<double>> res(std::complex<double>{}, fftLen);

	if(fftLen >= inputArray.size())
	{
		/// short signal
		std::copy(std::begin(inputArray),
				  std::end(inputArray),
				  std::begin(res));
//		res *= fftLen / static_cast<double>(inputArray.size());
	}
	else
	{
		/// long signal
#if 0
		/// take last
		int offset = inputArray.size() - res.size();
#elif 0
		/// take mid
		int offset = (inputArray.size() - res.size()) / 2;
#else
		/// take first
		int offset = 0;
#endif

		std::copy(std::begin(inputArray) + offset,
				  std::begin(inputArray) + offset + fftLen,
				  std::begin(res));
	}

	/// apply window
//	res *= smLib::toComplex(myLib::fftWindow(fftLen, myLib::windowName::Blackman));


	/// DFT
	unsigned int N = res.size();
    unsigned int k = N;
    unsigned int n;
    const double thetaT = pi / N;
    std::complex<double> phiT = std::polar<double>(1., thetaT);
    std::complex<double> T;
    while (k > 1)
    {
        n = k;
        k >>= 1;
        phiT = phiT * phiT; /// remake ???
        T = 1.0L;
        for (unsigned int l = 0; l < k; ++l)
        {
            for (unsigned int a = l; a < N; a += n)
            {
                unsigned int b = a + k;
				std::complex<double> t = res[a] - res[b];
				res[a] += res[b];
				res[b] = t * T;
            }
            T *= phiT;
        }
    }

    /// Decimate
	auto m = static_cast<unsigned int>(log2(N));
    for (unsigned int a = 0; a < N; ++a)
    {
        unsigned int b = a;
        /// Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a)
		{
			std::swap(res[a], res[b]);
        }
    }
	return res;
}

std::valarray<std::complex<double>> spectreRtoCcomplex(
		const std::valarray<double> & inputArray,
		int fftLen)
{
	std::valarray<std::complex<double>> res(inputArray.size());
	std::transform(std::begin(inputArray),
				   std::end(inputArray),
				   std::begin(res),
				   [](double in)
	{
		return std::complex<double>(in);
	}
	);

	return spectreCtoCcomplex(res, fftLen);
}

std::valarray<double> spectreRtoRcomplex(
		const std::valarray<double> & inputArray,
		int fftLen)
{
	return smLib::abs(spectreRtoCcomplex(inputArray, fftLen));
}

double fractalDimension(const std::valarray<double> & arr,
						int Kmax,
						const QString & picPath)
{
    /// what are the limits?
	int N = arr.size();

	std::vector<double> drawK_{};
	std::vector<double> drawL_{};

	/// make collection of timeShifts
	auto timeShifts = smLib::range<std::vector<int>>(1, Kmax);

#if 0
	/// for long scale signals
	std::vector<int> timeShifts;
	timeShifts = {1, 2, 3, 4}; /// initialize
	for(int i = 11; i < log2(N / 4) * 4 + 1 ; ++i)
	{
		timeShifts.push_back(std::floor(std::pow(2, (i - 1)/4.)));
	}
#endif

	for(int timeShift : timeShifts)
	{
		double L = 0.;

        for(int m = 0; m < timeShift; ++m) /// m = startShift
        {
			const double coeff = (N - 1) / static_cast<double>(timeShift)
						   / std::floor( (N - m) / static_cast<double>(timeShift) )
						   ; /// ~1

			double Lm = 0.;
			for(int i = 1; i < std::floor( (N - m) / timeShift); ++i)
            {
				Lm += std::abs(arr[m + i * timeShift] - arr[m + (i - 1) * timeShift]);
			}
			L += Lm * coeff;
        }
		L /= timeShift; /// big "/ k"
		L /= timeShift; /// average Lm

		drawK_.push_back(std::log(timeShift));
		drawL_.push_back(std::log(L));
    }

	const std::valarray<double> drawK = smLib::vecToValar(drawK_);
	const std::valarray<double> drawL = smLib::vecToValar(drawL_);

	/// least square approximation
	const double slope = smLib::covariance(drawK, drawL) / smLib::covariance(drawK, drawK);

    double drawX = 0.;
    double drawY = 0.;
    if(!picPath.isEmpty())
    {
        QPixmap pic = QPixmap(800, 600);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);

        pnt.setPen("black");
        pnt.setBrush(QBrush("black"));

		double minX = drawK.min();
		double maxX = drawK.max();
		double minY = drawL.min();
		double maxY = drawL.max();
        double lenX = maxX - minX;
        double lenY = maxY - minY;

		/// draw squares
		int frame = 10;
		int rectSize = 3;
		for(uint h = 0; h < timeShifts.size(); ++h) /// drawK, drawL [last] is bottom-left
		{
			drawX = frame + std::abs(drawK[h] - minX) / lenX
					* (pic.width() - 2 * frame) - rectSize;
			drawY = frame + (1. - std::abs(drawL[h] - minY) / lenY)
					* (pic.height() - 2 * frame) - rectSize;
			pnt.drawRect(QRect(drawX, drawY, rectSize, rectSize));
        }

		/// draw line (passes (meanX, meanY))
		double add = smLib::mean(drawL) - slope * smLib::mean(drawK);
        pnt.setPen("red");
        pnt.setBrush(QBrush("red"));

		const double startY	= (1. - (slope * minX + add - minY) / lenY) * pic.height();
		const double endY	= (1. - (slope * maxX + add - minY) / lenY) * pic.height();

        pnt.drawLine(0,
					 startY,
                     pic.width(),
					 endY
                     );

		/// draw results
		pnt.setPen("black");
		pnt.setBrush(QBrush("black"));
		pnt.setFont(QFont("Helvetica", 18));
		pnt.drawText(pic.width() * 0.7,
					 pic.height() * 0.2,
					 QString("FD = " + nm(smLib::doubleRound(-slope, 3)))
					 );
		pnt.drawText(pic.width() * 0.7,
					 pic.height() * 0.2 + 25,
					 QString("R^2 = " + nm(smLib::doubleRound(
											   std::pow(smLib::correlation(drawL, drawK), 2), 3
											   )
										   )
							 )
					 );


        pnt.end();
        pic.save(picPath, nullptr, 100);
	}
    return -slope;
}


double fractalDimensionForTest(const std::valarray<double> & arr,
							   int Kmax,
							   const QString & picPath)
{
	std::vector<double> drawK_{};
	std::vector<double> drawL_{};
	const int N = arr.size();

	/// make collection of timeShifts
//	std::vector<int> timeShifts = smLib::range<std::vector<int>>(1, Kmax);

	/// for long scale signals
	std::vector<int> timeShifts;
	timeShifts = {1, 2, 3, 4}; /// initialize
	for(int i = 11; i < log2(N / 4.) * 4 + 1 ; ++i)
	{
		timeShifts.push_back(std::floor(std::pow(2, (i - 1) / 4.)));
	}

	for(int timeShift : timeShifts)
	{
		double L = 0.;
		for(int m = 0; m < timeShift; ++m) /// m = startShift
		{
			const double coeff = (N - 1) / static_cast<double>(timeShift)
						   / std::floor( (N - m) / static_cast<double>(timeShift) )
						   ; /// ~1

			double Lm = 0.;
			for(int i = 1; i < std::floor( (N - m) / timeShift); ++i)
			{
				Lm += std::abs(arr[m + i * timeShift] - arr[m + (i - 1) * timeShift]);
			}
			L += Lm * coeff;
		}
		L /= timeShift; /// big "/ k"
		L /= timeShift; /// average Lm

		drawK_.push_back(std::log(timeShift));
		drawL_.push_back(std::log(L));
	}

	const std::valarray<double> drawK = smLib::vecToValar(drawK_);
	const std::valarray<double> drawL = smLib::vecToValar(drawL_);

	const std::valarray<double> drawK8 = drawK[std::slice(0, 8, 1)];
	const std::valarray<double> drawL8 = drawL[std::slice(0, 8, 1)];

	/// least square approximation
	const double slope = smLib::covariance(drawK8,
										   drawL8)
						 / smLib::covariance(drawK8,
											 drawK8);


	double drawX = 0.;
	double drawY = 0.;
	if(!picPath.isEmpty())
	{
		QPixmap pic = QPixmap(800, 600);
		QPainter pnt;
		pic.fill();
		pnt.begin(&pic);

		pnt.setPen("black");
		pnt.setBrush(QBrush("black"));

		/// draw axes
		double gap = 0.06;
		int frame = 10; /// from top and right
		pnt.drawLine(pic.width() * gap, pic.height() * (1. - gap),
					 pic.width() - frame, pic.height() * (1. - gap)); /// x

		pnt.drawLine(pic.width() * gap, frame,
					 pic.width() * gap, pic.height() * (1. - gap)); /// y

		double minX = drawK.min(); std::cout << minX << std::endl;
		double maxX = drawK.max(); std::cout << maxX << std::endl;
		double minY = drawL.min(); std::cout << minY << std::endl;
		double maxY = drawL.max(); std::cout << maxY << std::endl;
		double lenX = maxX - minX; std::cout << lenX << std::endl;
		double lenY = maxY - minY; std::cout << lenY << std::endl;

		/// draw values
		/// magic consts
		pnt.setFont(QFont("Helvetica", 12));
		/// y
		pnt.drawText(0, pic.height() * (1. - gap) + 12. / 2,
					 nm(smLib::doubleRound(minY, 2)));
		pnt.drawText(0, + 12 + 12 / 2,
					 nm(smLib::doubleRound(maxY, 2)));
		/// x
		pnt.drawText(pic.width() * gap, pic.height() * (1. - gap) + 12 * 1.5,
					 nm(smLib::doubleRound(minX, 2)));
		pnt.drawText(pic.width() - frame - 20, pic.height() * (1. - gap) + 12 * 1.5,
					 nm(smLib::doubleRound(maxX, 2)));

		/// draw squares
		int rectSize = 3;
		for(uint h = 0; h < timeShifts.size(); ++h) /// drawK, drawL [last] is bottom-left
		{
			drawX = pic.width() * gap + std::abs(drawK[h] - minX) / lenX
					* (pic.width() * (1. - gap) - frame) - rectSize;
			drawY = frame + (1. - std::abs(drawL[h] - minY) / lenY)
					* (pic.height() * (1. - gap) - frame) - rectSize;
			pnt.drawRect(QRect(drawX, drawY, rectSize, rectSize));
		}

		/// draw line (passes (meanX, meanY))
		double add = smLib::mean(drawL8) - slope * smLib::mean(drawK8);
		pnt.setPen("red");
		pnt.setBrush(QBrush("red"));

		const double startY	= frame + (1. - (slope * minX + add - minY) / lenY) * (pic.height() * (1. - gap) - frame);
		const double endY	= frame + (1. - (slope * maxX + add - minY) / lenY) * (pic.height() * (1. - gap) - frame);

		pnt.drawLine(pic.width() * gap,
					 startY,
					 pic.width(),
					 endY
					 );

		/// draw results
		pnt.setPen("black");
		pnt.setBrush(QBrush("black"));
		pnt.setFont(QFont("Helvetica", 18));
		pnt.drawText(pic.width() * 0.7,
					 pic.height() * 0.2,
					 QString("FD = " + nm(smLib::doubleRound(-slope, 3)))
					 );
		pnt.drawText(pic.width() * 0.7,
					 pic.height() * 0.2 + 25,
					 QString("R^2 = " + nm(smLib::doubleRound(
											   std::pow(smLib::correlation(drawL, drawK), 2), 3
											   )
										   )
							 )
					 );


		pnt.end();
		pic.save(picPath, nullptr, 100);
	}
	return -slope;
}

std::pair<double, double> signalBand(const std::valarray<double> & inSignal)
{
	return {0., 0.};
}

double alphaPeakFreq(const std::valarray<double> & spectreR,
					 int initSigLen, ///
					 double srate,
					 double leftLimFreq,
					 double rightLimFreq)
{
	double helpDouble = 0.;
	int helpInt = 0;
	const int hlp = smLib::fftL(initSigLen);
	for(int k = smLib::fftLimit(leftLimFreq,
								srate,
								hlp);
		k <= smLib::fftLimit(rightLimFreq,
							 srate,
							 hlp);
		++k)
	{
		if(spectreR[k] > helpDouble)
		{
			helpDouble = spectreR[k];
			helpInt = k;
		}
	}
	return helpInt * srate / hlp;
}

std::valarray<double> integrateSpectre(const std::valarray<double> & spectreR,
									   double srate,
									   std::vector<std::pair<double, double>> limits)
{
	const int & initSigLen = spectreR.size(); /// really?

	std::valarray<double> res(limits.size());
	int counter = 0;
	for(const auto & limit : limits)
	{
		const int leftLimit = smLib::fftLimit(limit.first,
											  srate,
											  smLib::fftL(initSigLen));
		const int rightLimit = smLib::fftLimit(limit.second,
											   srate,
											   smLib::fftL(initSigLen)) + 1;

		res[counter++] = std::accumulate(std::begin(spectreR) + leftLimit,
										 std::begin(spectreR) + rightLimit,
										 0.)
						 / (rightLimit - leftLimit) /// average in the band
						 ;
	}

#if 0
	/// normalize spectre for unit sum
	/// 20. for not too small values
	double helpDouble = 20. / std::accumulate(std::begin(res),
											  std::end(res),
											  0.) ;
	for(auto & in : fullSpectre)
	{
		in *= helpDouble;
	}
#endif
	return res;
}

matrix integrateSpectra(const matrix & spectraR,
						double srate,
						const std::vector<std::pair<double, double>> & limits)
{
	matrix res(spectraR.rows(), 1);
	/// kinda res.apply(myLib::integrateSpectre(_1, srate, limits));
	for(int i = 0; i < spectraR.rows(); ++i)
	{
		res[i] = myLib::integrateSpectre(spectraR[i], srate, limits);
	}
	return res;
}

std::valarray<double> integrateSpectre(const std::valarray<double> & spectreR,
									 double srate,
									 double leftFreqLowLim,
									 double leftFreqHighLim,
									 double spectreStepFreq)
{
	std::vector<std::pair<double, double>> lims{};
	for(double j = leftFreqLowLim;
		j <= leftFreqHighLim;
		j += spectreStepFreq)
	{
		lims.push_back({j - spectreStepFreq / 2., j + spectreStepFreq / 2.});
	}
	return integrateSpectre(spectreR, srate, lims);
}

#if 0
double fractalDimensionBySpectre(const std::valarray<double> & arr,
								 const QString & picPath)
{
	const double srate = 250.;


	int N = arr.size();
	const int windowSize = 512;
	const int timeShift = 128;
	const double freqStep = srate / windowSize;

	std::valarray<double> avSpectre = myLib::spectreRtoR(smLib::valarSubsec(arr,
																			0,
																			windowSize));
	int numWinds = (N - windowSize) / timeShift;
	for(int i = 0; i < numWinds; ++i)
	{
		auto windSpec = myLib::spectreRtoR(smLib::valarSubsec(arr,
															  i * timeShift,
															  i * timeShift + windowSize),
						   windowSize);
		avSpectre += windSpec;
	}
	avSpectre /= numWinds;
	avSpectre *= myLib::spectreNorm(windowSize, windowSize, srate);

	std::vector<double> drawK_{};
	std::vector<double> drawL_{};

#if 0
	/// ???
	for(double fr = freqStep; fr < 15.; fr += freqStep)
	{
		drawK_.push_back(std::log(fr));
		drawL_.push_back(std::log(avSpectre[int(fr / freqStep)]));
	}
#endif

	for(int fr = 1; fr < 150; ++fr)
	{
		drawK_.push_back(std::log(fr));
		drawL_.push_back(std::log(avSpectre[fr]));
	}

	std::valarray<double> drawK = smLib::vecToValar(drawK_);
	std::valarray<double> drawL = smLib::vecToValar(drawL_);

	for(int i = 0; i < 20; ++i)
	{
		std::cout << drawK[i] << " " << drawL[i] << std::endl;
	}

	/// least square approximation
	double slope = smLib::covariance(drawK, drawL) / smLib::covariance(drawK, drawK);

	double drawX = 0.;
	double drawY = 0.;
	if(!picPath.isEmpty())
	{
		QPixmap pic = QPixmap(800, 600);
		QPainter pnt;
		pic.fill();
		pnt.begin(&pic);

		pnt.setPen("black");
		pnt.setBrush(QBrush("black"));

		double minX = drawK.min();
		double maxX = drawK.max();
		double minY = drawL.min();
		double maxY = drawL.max();
		double lenX = maxX - minX;
		double lenY = maxY - minY;

		int frame = 10; /// pixels
		int rectSize = 3;
		for(int h = 0; h < drawK.size(); ++h) /// drawK, drawL [last] is bottom-left
		{
			drawX = frame + std::abs(drawK[h] - minX) / lenX
					* (pic.width() - 2 * frame) - rectSize;
			drawY = frame + (1. - std::abs(drawL[h] - minY) / lenY)
					* (pic.height() - 2 * frame) - rectSize;
			pnt.drawRect(QRect(drawX, drawY, rectSize, rectSize));
		}

		pnt.setPen("red");
		pnt.setBrush(QBrush("red"));

		/// line passes (meanX, meanY)
		/// check this with frame
		double add = smLib::mean(drawL) - slope * smLib::mean(drawK);

		drawX = (1. - (slope * minX + add - minY) / lenY) * pic.height(); /// startY
		drawY = (1. - (slope * maxX + add - minY) / lenY) * pic.height(); /// endY

		pnt.drawLine(0,
					 drawX,
					 pic.width(),
					 drawY
					 );

		pnt.end();
		pic.save(picPath, nullptr, 100);
	}
	return (5. + slope) / 2.;
}
#endif

/// replace DEFS.getFreq()
double morletCos(double const freq1, const double timeShift, const double pot, const double time)
{
	double freq = freq1 * 2. * pi / DEFS.getFreq();
	return cos(freq * (time - timeShift)) * exp( - std::pow(freq * (time-timeShift) / pot, 2));
}

/// replace DEFS.getFreq()
double morletSin(double const freq1, const double timeShift, const double pot, const double time)
{
	double freq = freq1 * 2. * pi / DEFS.getFreq();
	return sin(freq * (time - timeShift)) * exp( - std::pow(freq * (time-timeShift) / pot, 2));
}


void splineCoeffCount(const std::valarray<double> & inX,
					  const std::valarray<double> & inY,
                      int dim,
					  std::valarray<double> & outA,
					  std::valarray<double> & outB)
{

	/// [inX[i-1]...inX[i]] - q[i-1] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
    matrix coefsMatrix(dim, dim, 0);
	std::valarray<double> rightVec(dim);
	std::valarray<double> vectorK(dim);

	/// set coefs and rightVec
    coefsMatrix[0][0] = 2.*(inX[1] - inX[0]);
    coefsMatrix[0][1] = (inX[1] - inX[0]);
    rightVec[0] = 3. * (inY[1] - inY[0]);

    coefsMatrix[dim-1][dim-1] = 2.*(inX[dim-1] - inX[dim-2]);
    coefsMatrix[dim-1][dim-2] = (inX[dim-1] - inX[dim-2]);
    rightVec[dim-1] = 3. * (inY[dim-1] - inY[dim-2]);
	for(int i = 1; i < dim-1; ++i) /// besides first and last rows
    {
        coefsMatrix[i][i-1] = 1 / (inX[i] - inX[i-1]);
        coefsMatrix[i][i] = 2 * (1 / (inX[i] - inX[i-1]) + 1 / (inX[i+1] - inX[i]));
        coefsMatrix[i][i+1] = 1 / (inX[i+1] - inX[i]);

        rightVec[i] = 3 * ( (inY[i] - inY[i-1]) / ( (inX[i] - inX[i-1]) * (inX[i] - inX[i-1]) ) + (inY[i+1] - inY[i]) / ( (inX[i+1] - inX[i]) * (inX[i+1] - inX[i]) ));
    }
#if 0
	/// cout matrix and rightvec - OK
	for(int i = 0; i < dim; ++i)
	{
		for(int j = 0; j < dim; ++j)
		{
			std::cout << coefsMatrix[i][j] << "\t ";
		}
		std::cout << rightVec[i] << std::endl;
	}
#endif
	/// count K's
    vectorK = coefsMatrix.matrixSystemSolveGauss(rightVec);
	/// count outA and outB
	for(int i = 1; i < dim; ++i) /// there is dim-1 intervals between dim points
    {
        outA[i-1] =  vectorK[i - 1] * (inX[i] - inX[i - 1]) - (inY[i] - inY[i - 1]);
        outB[i-1] = -vectorK    [i] * (inX[i] - inX[i - 1]) + (inY[i] - inY[i - 1]);
    }
}

double splineOutput(const std::valarray<double> & inX,
					const std::valarray<double> & inY,
                    int dim,
					const std::valarray<double> & A,
					const std::valarray<double> & B,
                    double probeX)
{
	/// [inX[i-1]...inX[i]] - q[i] = (1-t)*inY[i1] + t*inY[i] + t(1-t)(outA[i](1-t) + outB[i]t));
    double t;
	/// which interval we have?
	int num = -1; /// number of interval
    for(int i = 0; i < dim-1; ++i)
    {
        if(inX[i] <= probeX && probeX < inX[i + 1])
        {
            num = i;
            break;
        }
    }
    t = (probeX - inX[num]) / (inX[num + 1] - inX[num]);
    double res = (1 - t) * inY[num]
                 +   t   * inY[num + 1]
                 + t * (1 - t) * (A[num] * (1 - t) + B[num] * t);
    return res;
}


/// UNUSED - MUST CHECK BEFORE USE
std::valarray<double> hilbert(const std::valarray<double> & arr,
							  double lowFreq,
							  double highFreq,
							  const QString & picPath)
{

    const int inLength = arr.size();
	const int fftLen = smLib::fftL(inLength);
	const double spStep = DEFS.getFreq() / fftLen;
	const double normCoef = std::sqrt(fftLen / static_cast<double>(inLength));

	std::valarray<double> out; /// result
	out.resize(2 * fftLen);
	std::fill(std::begin(out), std::end(out), 0.);

	std::vector<double> tempArr;
    tempArr.resize(fftLen, 0.);

	std::vector<double> filteredArr;
    filteredArr.resize(fftLen, 0.);

    for(int i = 0; i < inLength; ++i)
    {
        out[ 2 * i + 0] = arr[i] * normCoef;
    }

	spectreCtoC(out, fftLen);

	/// start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2. * lowFreq / spStep
		   || i > 2. * highFreq / spStep) { out[i] = 0.; }
    }
    for(int i = fftLen; i < 2 * fftLen; ++i)
    {
        if(((2 * fftLen - i) < 2. * lowFreq / spStep)
		   || (2 * fftLen - i > 2. * highFreq / spStep)) { out[i] = 0.; }
    }
	/// constant component
    out[0] = 0.;
    out[1] = 0.;
    out[fftLen] = 0.;
    out[fftLen+1] = 0.;
	/// end filtering

	spectreCtoRrev(out);
    for(int i = 0; i < inLength; ++i)
    {
        filteredArr[i] = out[2 * i] / fftLen / normCoef;
    }


	/// Hilbert via FFT
    for(int i = 0; i < inLength; ++i)
    {
        out[2 * i + 0] = filteredArr[i] * normCoef;
        out[2 * i + 1] = 0.;
    }
    for(int i = inLength; i < fftLen; ++i)
    {
        out[2 * i + 0] = 0.;
        out[2 * i + 1] = 0.;
	}
	spectreCtoC(out, fftLen);

    for(int i = 0; i < fftLen / 2; ++i)
    {
        out[2 * i + 0] = 0.;
        out[2 * i + 1] = 0.;
	}
	/// whaaaat?
	spectreCtoCrev(out);

    for(int i = 0; i < inLength; ++i)
    {
		tempArr[i] = out[2 * i + 1] / fftLen * 2; /// hilbert
    }
	/// end Hilbert via FFT


    for(int i = 0; i < fftLen; ++i)
    {
        out[i] = std::sqrt(std::pow(tempArr[i], 2) + std::pow(filteredArr[i], 2));
    }


    if(!picPath.isEmpty())
    {

		/// start check draw - OK
        QPixmap pic(fftLen, 600);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);
        ///    double sum, sum2;
        double enlarge = 10.;

        pnt.setPen("black");
        for(int i = 0; i < pic.width() - 1; ++i)
        {
            pnt.drawLine(i,
                         pic.height() / 2. - enlarge * filteredArr[i],
                         i+1,
                         pic.height() / 2. - enlarge * filteredArr[i + 1]);
        }
#if 0
        pnt.setPen("blue");
        for(int i = 0; i < pic.width() - 1; ++i)
        {
            pnt.drawLine(i,
                         pic.height() / 2. - enlarge * tempArr[i],
                         i+1,
                         pic.height() / 2. - enlarge * tempArr[i + 1]);
        }
#endif
        pnt.setPen("green");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt.drawLine(i,
                         pic.height() / 2. - enlarge * out[i],
                         i+1,
                         pic.height() / 2. - enlarge * out[i + 1]);
        }

        pic.save(picPath, nullptr, 100);
        pic.fill();
        pnt.end();
		std::cout << "hilber drawn" << std::endl;
		/// end check draw
    }
    return out;
}


std::valarray<double> hilbertPieces(const std::valarray<double> & inArr,
									const QString & picPath)
{
	/// do hilbert transform for the first inLength bins
	const int inLength = inArr.size();
	const int fftLen = smLib::fftL(inLength) / 2;

	std::valarray<double> outHilbert(inLength); /// result
	std::array<std::valarray<double>, 2> tempArr{};
	int start{0};
	for(int i = 0; i < 2; ++i)
	{
		if(i == 1)
		{
			start = inLength - fftLen;
		}

		/// Hilbert via FFT
		tempArr[i].resize(fftLen);
		std::copy(std::begin(inArr) + start,
				  std::begin(inArr) + start + fftLen,
				  std::begin(tempArr[i]));

		tempArr[i] = spectreRtoC(tempArr[i], fftLen);

		std::fill(std::begin(tempArr[i]), std::begin(tempArr[i]) + fftLen, 0.);

		const std::valarray<double> temp = spectreCtoCrev(tempArr[i]);
		tempArr[i].resize(inLength);
		for(int j = 0; j < fftLen; ++j)
		{
			tempArr[i][start + j] = temp[2 * j + 1]; /// hilbert. Why +1?
		}
		tempArr[i] *= 2.;
		/// end Hilbert via FFT
	}

	for(int i = 0; i < fftLen; ++i)
	{
		outHilbert[i] = std::sqrt(std::pow(tempArr[0][i], 2.) + std::pow(inArr[i], 2.));
	}

    int startReplace = 0;
	double minD = 100.; /// difference between two possible envelopes
    double helpDouble = 0.;

    for(int i = inLength - fftLen; i < fftLen; ++i)
    {
		helpDouble = outHilbert[i] - std::pow(std::pow(tempArr[1][i], 2.) +
										 std::pow(inArr[i], 2.), 0.5);
		/// could add derivative
        if(std::abs(helpDouble) <= std::abs(minD))
        {
            minD = helpDouble;
            startReplace = i;
        }
    }

    for(int i = startReplace; i < inLength; ++i)
    {
		outHilbert[i] = std::sqrt(std::pow(tempArr[1][i], 2.) + std::pow(inArr[i], 2.));
    }


    if(!picPath.isEmpty())
    {

		/// start check draw - OK
		QPixmap pic(inLength, 120);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);
		double maxVal = *std::max_element(std::begin(inArr),
										  std::end(inArr)) * 1.1;

        pnt.setPen("black");
		/// initial signal
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt.drawLine(i,
						 pic.height()/2. * (1. - inArr[i] / maxVal),
                         i+1,
						 pic.height()/2. * (1. - inArr[i + 1] / maxVal)
                    );
		}



#if 0
		pnt.setPen("gray");
		/// imaginary
		/// first part
		for(int i = 0; i < startReplace; ++i)
		{
			pnt.drawLine(i,
						 pic.height()/2. * (1. - tempArr[0][i] / maxVal),
						 i+1,
						 pic.height()/2. * (1. - tempArr[0][i + 1] / maxVal)
					);
		}
		/// second part
		for(int i = startReplace; i < inLength; ++i)
		{
			/// draw the imaginary part of the signal
			pnt.drawLine(i,
						 pic.height()/2. * (1. - tempArr[1][i] / maxVal),
						 i+1,
						 pic.height()/2. * (1. - tempArr[1][i + 1] / maxVal)
					);
        }
#endif


		pnt.setPen(QPen(QBrush("black"), 2));
		/// envelope
		for(int i = 0; i < pic.width() - 1; ++i)
        {
			/// draw the envelope?
			pnt.drawLine(i,
						 pic.height()/2. * (1. - outHilbert[i] / maxVal),
						 i+1,
						 pic.height()/2. * (1. - outHilbert[i + 1] / maxVal)
					);

        }

#if 01
		/// draw the envelope mean
		const double M = smLib::mean(outHilbert);
		const double S = smLib::sigma(outHilbert);

		pnt.setPen(QPen(QBrush("black"), 1, Qt::DashLine));
		pnt.drawLine(0,
					 pic.height()/2. * (1. - M / maxVal),
					 pic.width(),
					 pic.height()/2. * (1. - M / maxVal)
				);

		pnt.setPen(QPen(QBrush("black"), 1, Qt::DotLine));
		pnt.drawLine(0,
					 pic.height()/2. * (1. - (M + S) / maxVal),
					 pic.width(),
					 pic.height()/2. * (1. - (M + S) / maxVal)
				);

		pnt.drawLine(0,
					 pic.height()/2. * (1. - (M - S) / maxVal),
					 pic.width(),
					 pic.height()/2. * (1. - (M - S) / maxVal)
				);
#endif


        pnt.end();
        pic.save(picPath, nullptr, 100);
		/// end check draw
    }
    return outHilbert;
}

double hilbertCarr(const std::valarray<double> & arr, double srate)
{
	static const double hilbertLowLimit = 0.;
	static const double hilbertHighLimit = 40.;

	auto envSpec = myLib::spectreRtoR(myLib::hilbertPieces(arr));
	envSpec[0] = 0.;

	double res = 0.;
	double sumSpec = 0.;
	for(int j = smLib::fftLimit(hilbertLowLimit,
								srate,
								smLib::fftL( arr.size() ));
		j <= smLib::fftLimit(hilbertHighLimit,
							 srate,
							 smLib::fftL( arr.size() ));
		++j)
	{
		res += envSpec[j] * j;
		sumSpec += envSpec[j];
	}
	res /= sumSpec;
	res /= smLib::fftLimit(1.,
					srate,
					smLib::fftL( arr.size() ));
	return res;
}


double hilbertSD(const std::valarray<double> & arr, double srate)
{
	auto a = myLib::hilbertPieces(arr);
	return smLib::sigma(a) / smLib::mean(a);
}

std::valarray<double> makeSine(int numPoints,
							   double freq,
							   double srate,
							   double startPhase)
{
	std::valarray<double> res(numPoints);
	for(int i = 0; i < numPoints; ++i)
	{
		res[i] = sin(freq * 2. * pi * (i / srate) + startPhase);
	}
	return res;
}


std::valarray<double> makeNoise(int numPoints)
{
	srand(time(nullptr));
	std::valarray<double> res(numPoints);
	for(int i = 0; i < numPoints; ++i)
	{
		res[i] = (rand() % 1000 - 500) / 500.;
	}
	return res;
}




std::valarray<double> derivative(const std::valarray<double> & inSignal)
{
	const int step = 2;
	std::valarray<double> res(inSignal.size());

	/// first some points - one-sided deriv
	for(int i = 0; i < step; ++i)
	{
		res[i] = (inSignal[i + step] - inSignal[i]) / step;
	}

	for(uint i = step; i < inSignal.size() - step; ++i)
	{
		res[i] = (inSignal[i + step] - inSignal[i - step]) / (2. * step);
	}

	/// last some points - one-sided deriv
	for(auto i = inSignal.size() - step; i < inSignal.size(); ++i)
	{
		res[i] = (inSignal[i] - inSignal[i - step]) / step;
	}
	return res;

}

double hjorthActivity(const std::valarray<double> & inSignal)
{
	return smLib::variance(inSignal);
}
double hjorthMobility(const std::valarray<double> & inSignal)
{
	return std::sqrt(smLib::variance(derivative(inSignal)) /
				smLib::variance(inSignal));
}

double hjorthComplexity(const std::valarray<double> & inSignal)
{
	/// second derivative
	return std::sqrt(smLib::variance(derivative(derivative(inSignal))) /
				smLib::variance(inSignal));
}



std::valarray<double> bayesCount(const std::valarray<double> & dataIn,
								 int numOfIntervals)
{
	double maxAmpl = 80.; /// generality
	std::valarray<double> out(0., numOfIntervals);
    for(uint j = 0; j < dataIn.size(); ++j)
    {
		int helpInt = std::floor((dataIn[j] + maxAmpl) / (2. * maxAmpl / numOfIntervals));

		if(helpInt != std::min(std::max(0, helpInt), numOfIntervals - 1))
        {
			continue; /// if helpInt not in range
        }
        out[helpInt] += 1;
    }
	out /= dataIn.size() * 10.; /// 10 is norm coef for perceptron

    return out;
}

#if 0
/// for fftWindow
double bes0(double in)
{
	/// https:/// en.wikipedia.org/wiki/Bessel_function#Modified_Bessel_functions
	return in;
}
#endif

std::valarray<double> fftWindow(int N, windowName name)
{
	std::valarray<double> res(N);
	const double arg = 2. * pi / (N - 1.);
	/// Hann/Hanning
	switch(name)
	{
	case windowName::Hann:
	{
		for(int i = 0; i < N; ++i)
		{
			res[i] = 0.5 * (1. - cos (i * arg));
		}
		break;
	}
	case windowName::Hamming:
	{
		for(int i = 0; i < N; ++i)
		{
			res[i] = 0.53836 - 0.46164 * cos (i * arg);
		}
		break;
	}
	case windowName::Blackman:
	{
		const double a = 0.16;
		const double a0 = (1. - a) / 2.;
		const double a1 = 0.5;
		const double a2 = a / 2.;
		for(int i = 0; i < N; ++i)
		{
			res[i] = a0 - a1 * cos(i * arg) + a2 * cos(2 * i * arg);
		}
		break;
	}
	case windowName::Nuttal:
	{

		const double a0 = 0.355768;
		const double a1 = 0.487396;
		const double a2 = 0.144232;
		const double a3 = 0.012604;
		for(int i = 0; i < N; ++i)
		{
			res[i] = a0 - a1 * cos(i * arg) + a2 * cos(2 * i * arg) - a3 * cos(3 * i * arg);
		}
		break;
	}
	case windowName::BlackmanNuttal:
	{

		const double a0 = 0.3635819;
		const double a1 = 0.4891775;
		const double a2 = 0.1365995;
		const double a3 = 0.0106411;
		for(int i = 0; i < N; ++i)
		{
			res[i] = a0 - a1 * cos(i * arg) + a2 * cos(2 * i * arg) - a3 * cos(3 * i * arg);
		}
		break;
	}
	case windowName::BlackmanHarris:
	{

		const double a0 = 0.35875;
		const double a1 = 0.48829;
		const double a2 = 0.14128;
		const double a3 = 0.01168;
		for(int i = 0; i < N; ++i)
		{
			res[i] = a0 - a1 * cos(i * arg) + a2 * cos(2 * i * arg) - a3 * cos(3 * i * arg);
		}
		break;
	}
	case windowName::FlatTop:
	{

		const double a0 = 1.;
		const double a1 = 1.93;
		const double a2 = 1.29;
		const double a3 = 0.388;
		const double a4 = 0.028;
		for(int i = 0; i < N; ++i)
		{
			res[i] = a0
					 - a1 * cos(i * arg)
					 + a2 * cos(2 * i * arg)
					 - a3 * cos(3 * i * arg)
					 + a4 * cos(4 * i * arg);
		}
		break;
	}
	case windowName::RifeVincent:
	{
		const double a0 = 1.;
		const double a1 = 4 / 3.;
		const double a2 = 1 / 3.;
		for(int i = 0; i < N; ++i)
		{
			res[i] = a0 - a1 * cos(i * arg) + a2 * cos(2 * i * arg);
		}
		break;
	}
	case windowName::Gaussian:
	{
		const double s = 0.4; /// < 0.5
		for(int i = 0; i < N; ++i)
		{
			res[i] = std::exp(-0.5 * std::pow((i - (N - 1) / 2.) / (s * (N - 1) / 2.) , 2));
		}
		break;
	}
	case windowName::Lanczos:
	{
		for(int i = 0; i < N; ++i)
		{
			res[i] = std::sin(M_PI * (2. * i / (N - 1) - 1)) / (M_PI * (2. * i / (N - 1) - 1));
		}
		break;
	}
	case windowName::rect:
	{
		res = 1;
		break;
	}
	case windowName::triang:
	{
		for(int i = 0; i < N; ++i)
		{
			res[i] = 1. - std::abs(2. * (i - (N - 1) / 2.) / N);
		}
		break;
	}
	case windowName::Parzen:
	{
		auto val = [N](int i) { return (2. * i) / N; };
		for(int i = 0; i <= N / 4; ++i)
//		for(int i = N / 4 + 1; i <= N / 2; ++i)
		{
			res[i] = 6. * std::pow(val(i), 2.) * (1. - val(i));
			res[N - 1 - i] = res[i];
		}
		for(int i = N / 4 + 1; i <= N / 2; ++i)
//		for(int i = 0; i <= N / 4; ++i)
		{
			res[i] = 1. - 2. * std::pow(1. - val(i), 3.);
			res[N - 1 - i] = res[i];
		}
		break;
	}
	case windowName::Welch:
	{
		for(int i = 0; i < N; ++i)
		{
			res[i] = 1. - std::pow((i - (N - 1.) / 2.) / ((N - 1.) / 2.), 2.);
		}
		break;
	}
	case windowName::sine:
	{
		for(int i = 0; i < N; ++i)
		{
			res[i] = std::sin(M_PI * i / (N - 1.));
		}
		break;
	}
	default:
	{
		std::cout << "default window" << std::endl;
		res = 1;
		break;
	}
	}
	return res;
}

double RDfreq(const std::valarray<double> & inSignal, int fftWind)
{
	auto RDspec = myLib::smoothSpectre(myLib::spectreRtoR(inSignal, fftWind), -1);
	auto RDargmax = myLib::indexOfMax(RDspec);
	auto RDfreq_ = RDargmax * globalEdf.getFreq() / fftWind;
	return RDfreq_;
}

double PPGrange(const std::valarray<double> & inSignal)
{
	std::valarray<double> PPGdata = inSignal - smLib::mean(inSignal);

	auto sign = [](double in) -> int
	{
		return (in > 0) ? 1 : -1;
	};
	int start{0};

	/// start with negative sign
	for(uint i = 0; i < PPGdata.size() - 1; ++i)
	{
		if(sign(PPGdata[i]) == -1) { start = i; break; } /// +1 for max, -1 for min
	}
	int currSign = sign(PPGdata[start]);

	std::vector<double> maxs{};	maxs.reserve(200);
	std::vector<double> mins{};	mins.reserve(200);

//	int counter = 0;
	for(uint i = start; i < PPGdata.size() - 1; ++i)
	{
		if(sign(PPGdata[i]) != currSign)
		{
			int end = i - 1;
			std::valarray<double> val = smLib::contSubsec(PPGdata, start, end);
			if(currSign == +1)		{ maxs.push_back(val.max()); }
			else if(currSign == -1)	{ mins.push_back(val.min()); }

			start = i;
			currSign *= -1; /// currSign = sign(chan[start]);
		}
	}
	int num = std::min(mins.size(), maxs.size());
	mins.resize(num);
	maxs.resize(num);

	double PPGrange_{0.};
	for(int i = 0; i < num; ++i)
	{
		PPGrange_ += (maxs[i] - mins[i]);
	}
	PPGrange_ /= num;
	return PPGrange_;
}

std::pair<double, double> EDAmax(const std::valarray<double> & inSignal,
								 const std::valarray<double> & baseSignal) /// max, latency
{
	double a = inSignal.max() - smLib::mean(baseSignal);
	double b = myLib::indexOfMax(inSignal);
	return std::make_pair(a, b);

}

std::pair<double, double> EDAmax(const std::valarray<double> & inSignal,
								 double baseMean)
{
	double a = inSignal.max() - baseMean;
	double b = myLib::indexOfMax(inSignal);
	return std::make_pair(a, b);

}

std::valarray<double> smoothSpectre(const std::valarray<double> & inSpectre,
									int numOfSmooth)
{
	if(numOfSmooth < 0)
	{
		numOfSmooth = std::ceil(15 * std::sqrt(inSpectre.size() / 4096)); /// magic constant
	}
	std::valarray<double> result = inSpectre;
    double help1, help2;

	/// generalized
//	static const std::vector<double> smoothC{1., 1., 1., 1./3.};

    for(int num = 0; num < numOfSmooth; ++num)
    {
        help1 = result[0];
        for(uint i = 1; i < result.size() - 1; ++i)
        {
            help2 = result[i];
			result[i] = (help1 + help2 + result[i + 1]) / 3.;
			/// generalized
//			result[i] = (help1 * smoothC[0] + help2 * smoothC[1] + result[i + 1] * smoothC[2]) * smoothC[3];
            help1 = help2;
        }
    }
    return result;
}

matrix countSpectre(const matrix & inData,
					int fftLen,
					int numSmooth)
{
	matrix data2 = inData;
	if(data2.cols() < fftLen)
	{
		data2.resizeCols(fftLen);
	}
	else
	{
		data2.pop_front(data2.cols() - fftLen);
	}

	const double threshold = 0.125;
	int eyes = 0;
	int h = 0;
	for(int i = 0; i < fftLen; ++i)
	{
		h = 0;
		for(int j = 0; j < data2.rows(); ++j)
		{
			if(std::abs(data2[j][i]) <= threshold) { ++h; } /// generality 1/8.
		}
		if(h == data2.rows()) { eyes += 1; }
	}

	if(
	   ( (fftLen - eyes) < DEFS.getFreq() * 3. ) /// real signal shorter than 3 sec
//	   || (eyes > 0.3 * fftLen) /// too much eyes
	   )
	{
		return {};
	}

	matrix res(data2.rows(), 1);
	for(auto i = 0; i < data2.rows(); ++i)
	{
		res[i] = myLib::calcSpectre(data2[i],
									fftLen,
									numSmooth,
									eyes);
	}
	return res;
}


std::valarray<double> calcSpectre(const std::valarray<double> & inSignal,
								  int fftLength,
								  int NumOfSmooth,
								  int Eyes,
								  double powArg)
{
    if(inSignal.size() != fftLength)
    {
		std::cout << "calcSpectre: inappropriate signal length" << std::endl;
		return {};
    }

	std::valarray<double> outSpectre = spectreRtoR(inSignal, fftLength) *
									   spectreNorm(fftLength, fftLength - Eyes, DEFS.getFreq());
//	outSpectre = std::pow(outSpectre, powArg);

	const double normSmooth = std::sqrt(fftLength / static_cast<double>(fftLength - Eyes));

	/// smooth spectre
	const int leftSmoothLimit = 2; /// doesn't effect on zero component
    const int rightSmoothLimit = fftLength / 2 - 1;
	double help1, help2;
	for(int a = 0; a < std::ceil(NumOfSmooth / normSmooth); ++a)
    {
        help1 = outSpectre[leftSmoothLimit - 1];
        for(int k = leftSmoothLimit; k < rightSmoothLimit; ++k)
        {
            help2 = outSpectre[k];
			outSpectre[k] = (help1 + help2 + outSpectre[k + 1]) / 3.;
            help1 = help2;
        }
    }
	return outSpectre;
}




void eyesProcessingStatic(const std::vector<int> & eogChannels,
						  const std::vector<int> & eegChannels,
						  const QString & windsDir,
						  const QString & outFilePath)
{
	QTime myTime;
	myTime.start();

	QStringList leest = QDir(windsDir).entryList({"*.edf"});

	matrix dataE(edfFile(windsDir + "/" + leest[0]).getNs(), 0);
	for(const auto & filePath : leest)
	{
		matrix tmp = edfFile(windsDir + "/" + filePath).getData();
		dataE.horzCat(tmp);
	}

	const uint Size = eogChannels.size() + 1; /// usually 3
	std::vector<int> signalNums;
	signalNums.reserve(Size);
	for(int eogNum : eogChannels)
	{
		signalNums.push_back(eogNum);
	}
	signalNums.push_back(0);


	matrix matrixInit(Size, Size);
	matrix coefficients(eegChannels.size(), eogChannels.size());

	for(uint k = 0; k < eegChannels.size(); ++k)
	{
		signalNums.back() = eegChannels[k];
		for(uint j = 0; j < Size; ++j)
		{
			for(uint z = j; z < Size; ++z)
			{
				matrixInit[j][z] = smLib::prod(
										dataE[ signalNums[j] ],
										dataE[ signalNums[z] ])
								   / dataE.cols();
				/// maybe (dataE.cols() - 1), but it's not important here
				if(j != z)
				{
					matrixInit[z][j] = matrixInit[j][z];
				}
			}
		}
		matrixInit.invert();
		for(uint i = 0; i < eogChannels.size(); ++i)
		{
			coefficients[k][i] = - matrixInit[i][eogChannels.size()]
								 / matrixInit[eogChannels.size()][eogChannels.size()];
		}
	}

	writeMatrixFile(outFilePath,
					coefficients,
					"NumOfEegChannels",
					"NumOfEogChannels");
	std::cout << "eyesProcessing: time elapsed = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}


std::valarray<double> (* refilter)(const std::valarray<double> & inputSignal,
								  double lowFreq,
								  double highFreq,
								  bool isNotch,
								  double srate) =
		&butter::refilter;

} /// end of namespace myLib
