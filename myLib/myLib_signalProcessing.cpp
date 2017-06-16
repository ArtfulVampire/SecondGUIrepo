#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <DSP/DspFilters/Dsp.h>

using namespace myOut;

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

	for(int i = 1; i <= in.size(); ++i)
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
	res = smLib::reverseArray(res);
	res = btr::butterworth(res, order, srate, (lowFreq + highFreq) / 2., (highFreq - lowFreq) / 2.);
	res = smLib::reverseArray(res);
	return res;
}
} // namespace btr


#if DSP_LIB
namespace myDsp
{


std::valarray<double> lowPassOneSide(const std::valarray<double> & inputSignal,
									 double cutoffFreq,
									 double srate)
{
	const int order = 6;

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
	params[0] = srate; // sample rate
	params[1] = order; // order
	params[2] = cutoffFreq; // cutoff


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
	std::valarray<double> tmp;
	tmp = lowPassOneSide(inputSignal,
						 cutoffFreq,
						 srate);

//	myLib::operator <<(std::cout, tmp);
//	std::cout << std::endl;

	// reverse signal
	tmp = smLib::reverseArray(tmp);
	tmp = lowPassOneSide(tmp,
						 cutoffFreq,
						 srate);

	// reverse back
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
	params[0] = srate; // sample rate
	params[1] = order; // order
	params[2] = (lowFreq + highFreq) / 2; // center frequency
	params[3] = highFreq - lowFreq; // band width

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
	// reverse back
	tmp = smLib::reverseArray(tmp);
	return tmp;
}

}
#endif // DSP_LIB









namespace myLib
{



void four1(double * dataF, int nn, int isign)
{
    int n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    double tempr,tempi;

    n = nn << 1; //n = 2 * fftLength
    j = 1;
    for (i = 1; i < n; i += 2)
    {
        if (j > i)
        {
            std::swap(dataF[j], dataF[i]);
			std::swap(dataF[j+1], dataF[i+1]);
        }
        m = n >> 1; // m = n / 2;
        while (m >= 2 && j > m)
        {
            j -= m;
            m >>= 1; //m /= 2;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax)
    {
        istep = mmax << 1; //istep = mmax * 2;
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

                // tempCompl = wCompl * dataFCompl[j/2]
                tempr = wr * dataF[j] - wi * dataF[j + 1];
                tempi = wr * dataF[j + 1] + wi * dataF[j];

                // dataFCompl[j/2] = dataFCompl[i/2] - tempCompl
                dataF[j] = dataF[i] - tempr;
                dataF[j + 1] = dataF[i + 1] - tempi;

                // dataFCompl[i/2] += tempCompl
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
	for(int i = 0; i < std::min(fftLen, int(inputSignal.size())); ++i)
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
	for(int i = 0; i < std::min(fftLen, int(inputSignal.size())); ++i)
	{
		pew[2 * i] = inputSignal[i];
	}

	four1(pew - 1, fftLen, 1);

	std::valarray<double> res(2 * fftLen);
	std::copy(pew, pew + 2 * fftLen, std::begin(res));

	delete[] pew;
	return res;
}

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
	for(int i = 0; i < std::min(fftLen, int(inputSignal.size())); ++i)
	{
		pew[2 * i] = inputSignal[i];
	}

	four1(pew - 1, fftLen, 1);

	const double norm = sqrt(myLib::spectreNorm(fftLen,
												inputSignal.size(),
												srate));

	std::valarray<std::complex<double>> res(fftLen);
	for(int i = 0; i < res.size(); ++i)
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

	for(int i = 0; i < inputSignal.size(); ++i)
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
//	{
//		std::cout << "spectreCtoC: should deal with fftLen, return" <<std::endl;
//		return {};
//	}

	if(fftLen <= 0)
	{
		fftLen = smLib::fftL(inputSignal.size() / 2);
	}

	double * pew = new double [2 * fftLen];
	std::fill(pew, pew + 2 * fftLen, 0.);

	for(int i = 0; i < inputSignal.size(); ++i)
	{
		pew[i] = inputSignal[i];
	}

	four1(pew - 1, fftLen, 1);

	std::valarray<double> res(2 * fftLen);
	std::copy(pew, pew + 2 * fftLen, std::begin(res));

	delete []pew;
	return res;
}

std::valarray<double> spectreConj(
		const std::valarray<double> & inputSpectre)
{
	std::valarray<double> res{inputSpectre};
	for(int i = 0; i < res.size() / 2; ++i)
	{
		res[2 * i + 1] = - res[2 * i + 1];
	}
	return res;
}

std::valarray<std::complex<double>> spectreConj(
		const std::valarray<std::complex<double>> & inputSpectre)
{
	return inputSpectre.apply(std::conj);
}

std::valarray<double> subSpectrumR(const std::valarray<double> & inputSpectre,
								  double leftFreq,
								  double rightFreq,
								  double srate)
{
	double a = log2(inputSpectre.size());
	if(a != double(int(a)))
	{
		std::cout << "subSpectrum: inputSpectre.size() is not power of 2" << std::endl;
		return {};
	}
	int fftLen = inputSpectre.size();
	int left = fftLimit(leftFreq, srate, fftLen);
	int right = fftLimit(rightFreq, srate, fftLen);
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
	for(int i = 0; i < res.size(); ++i)
	{
		res[i] = pew[2 * i];
	}
	res /= fftLen;

	delete[] pew;
	return res;
}

std::valarray<double> spectreCtoRrev(const std::valarray<std::complex<double>> & inputSpectre)
{
	/// check size == pow(2, N) ?
	int fftLen = inputSpectre.size();

	double * pew = new double [inputSpectre.size() * 2];
	for(int i = 0; i < inputSpectre.size(); ++i)
	{
		pew[2 * i] = inputSpectre[i].real();
		pew[2 * i + 1] = inputSpectre[i].imag();
	}

	four1(pew - 1, fftLen, -1);

	std::valarray<double> res(fftLen);
	for(int i = 0; i < res.size(); ++i)
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

double spectreNorm(int fftLen, int realSig, int srate)
{
	return 2. / (double(std::min(realSig, fftLen)) * srate);
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
	const int lowLim = 2. * floor( lowFreq / spStep);
	const int highLim = 2. * ceil( highFreq / spStep);

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
				   double srate)
{
	matrix res = matrix();
	for(int i = 0; i < inputMatrix.rows(); ++i)
	{
		res.push_back(myLib::refilter(inputMatrix[i],
									  lowFreq,
									  highFreq,
									  isNotch,
									  srate));
	}
	return res;
}


std::valarray<double> upsample(const std::valarray<double> & inSignal,
							   double oldFreq,
							   double newFreq)
{
	int rat = newFreq / oldFreq;
	std::valarray<double> res(inSignal.size() * rat);
	for(int i = 0; i < inSignal.size(); ++i)
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

	for(int i = 0; i < inSignal.size() / rat; ++i)
	{
		res[i] = res[i * rat];
	}
	smLib::resizeValar(res, inSignal.size() / rat);
	return res;
}

int findJump(const std::valarray<double> & inSignal, int startSearch, double numOfSigmas)
{
	const int lenForSigma = 100;
	for(int i = startSearch; i < inSignal.size() - 1; ++i)
	{
		if(std::abs(inSignal[i + 1] - inSignal[i]) >
		   numOfSigmas * smLib::sigma(inSignal[std::slice(i - lenForSigma, lenForSigma, 1)]))
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

	// both lowLim and highLim are in/ex
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
//		res *= fftLen / double(inputArray.size());
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


	// DFT
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

    // Decimate
    unsigned int m = (unsigned int)log2(N);
    for (unsigned int a = 0; a < N; ++a)
    {
        unsigned int b = a;
        // Reverse bits
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
                        const QString & picPath)
{
    int timeShift; //timeShift
    long double L = 0.; //average length
    long double tempL = 0.;
    long double coeff = 0.;

    /// what are the limits?
    int N = arr.size();
    int maxLimit = floor( log(N) * 4. - 5.);

    maxLimit = 100;

	int minLimit = std::max(maxLimit - 10, 3);

    minLimit = 15;

	int arrSize = maxLimit - minLimit;

	std::valarray<double> drawK(arrSize);
	std::valarray<double> drawL(arrSize);

    for(int h = minLimit; h < maxLimit; ++h)
    {
        timeShift = h;
        L = 0.;
        for(int m = 0; m < timeShift; ++m) // m = startShift
        {
            tempL = 0.;
            coeff = (N - 1) / (floor((N - m) / timeShift)) / timeShift;
            for(int i = 1; i < floor((N - m) / timeShift); ++i)
            {
				tempL += std::abs(arr[m + i * timeShift] - arr[m + (i - 1) * timeShift]) * coeff;

            }
            L += tempL / timeShift;
        }
        drawK[h - minLimit] = log(timeShift);
		drawL[h - minLimit] = log(L);
    }

    //least square approximation
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

		double minX = std::min(drawK[0], drawK[drawK.size() - 1]);
		double maxX = std::max(drawK[0], drawK[drawK.size() - 1]);
		double minY = std::min(drawL[0], drawL[drawL.size() - 1]);
		double maxY = std::max(drawL[0], drawL[drawL.size() - 1]);
        double lenX = maxX - minX;
        double lenY = maxY - minY;

        for(int h = 0; h < maxLimit - minLimit; ++h) // drawK, drawL [last] is bottom-left
        {
			drawX = std::abs(drawK[h] - minX) / lenX * pic.width() - 3;
			drawY = (1. - std::abs(drawL[h] - minY) / lenY) * pic.height() - 3;
            pnt.drawRect(QRect(int(drawX), int(drawY), 3, 3));
        }

        pnt.setPen("red");
        pnt.setBrush(QBrush("red"));

        // line passes (meanX, meanY)
		double add = smLib::mean(drawL) - slope * smLib::mean(drawK);

        drawX = (1. - (slope * minX + add - minY) / lenY) * pic.height(); // startY
		drawY = (1. - (slope * maxX + add - minY) / lenY) * pic.height(); // endY

        pnt.drawLine(0,
                     drawX,
                     pic.width(),
                     drawY
                     );

        pnt.end();
        pic.save(picPath, 0, 100);
	}
    return -slope;
}








//products for ICA
void product1(const matrix & arr,
              const int length,
              const int ns,
			  const std::valarray<double> & vect,
			  std::valarray<double> & outVector)
{
    //<X*g(Wt*X)>
    //vec = Wt
    //X[j] = arr[][j] dimension = ns
    //average over j

    outVector.resize(ns); // and fill zeros

    double sum = 0.;

    for(int j = 0; j < length; ++j)
    {
		sum = smLib::prod(vect, arr[j]);
        outVector +=  tanh(sum) * arr[j];
    }
    outVector /= length;
}



void product2(const matrix & arr,
              const int length,
              const int ns,
			  const std::valarray<double> & vect,
			  std::valarray<double> & outVector)
{
    //g'(Wt*X)*1*W
    //vec = Wt
    //X = arr[][j]
    //average over j

    double sum = 0.;
    double sum1 = 0.;

    for(int j = 0; j < length; ++j)
    {
		sum = smLib::prod(vect, arr[j]);
        sum1 += 1 - tanh(sum) * tanh(sum);
    }
    sum1 /= length;
    outVector = vect * sum1;
}

void product3(const matrix & inMat,
              const int ns,
              const int currNum,
			  std::valarray<double> & outVector)
{
    //sum(Wt*Wi*Wi)
    outVector.resize(ns);

    double sum = 0.;
    for(int j = 0; j < currNum; ++j)
    {
		sum = smLib::prod(inMat[currNum], inMat[j]); // short valarray, no difference
        outVector += inMat[j] * sum;
    }
}

void randomizeValar(std::valarray<double> & valar)
{
    srand(time(NULL));
    for(uint i = 0; i < valar.size(); ++i)
    {
        valar[i] = rand() % 50 - 25;
    }
	smLib::normalize(valar);
}

void countVectorW(matrix & vectorW,
                  const matrix & dataICA,
                  const int ns,
                  const int dataLen,
                  const double vectorWTreshold)
{
    QTime myTime;
    myTime.restart();

    double sum1;
    double sum2;
	std::valarray<double> vector1(ns);
	std::valarray<double> vector2(ns);
	std::valarray<double> vector3(ns);
	std::valarray<double> vectorOld(ns);

    int counter;

    const matrix tempMatrix = matrix::transpose(dataICA);

    for(int i = 0; i < ns; ++i) //number of current vectorW
    {
        myTime.restart();
        counter = 0;
        randomizeValar(vectorW[i]);

        while(1)
        {
            vectorOld = vectorW[i]; // save previous vect

            /// local, dataICA transposed to tempMatrix
            vector1 = 0.;
            sum1 = 0.;
            for(int j = 0; j < dataLen; ++j)
            {
				const double temp = tanh(smLib::prod(vectorW[i], tempMatrix[j]));

                vector1 += tempMatrix[j] * temp;
                sum1 += (1. - temp * temp);
            }
            vector1 /= dataLen;

            sum1 /= dataLen;
            vector2 = vectorW[i] * sum1;


            vectorW[i] = vector1 - vector2;
            //orthogonalization
			myLib::product3(vectorW, ns, i, vector3);
            vectorW[i] -= vector3;
			smLib::normalize(vectorW[i]);

			sum2 = smLib::norma(vectorOld - vectorW[i]);

            ++counter;
            if(sum2 < vectorWTreshold || 2. - sum2 < vectorWTreshold) break;
            if(counter == 100) break;
        }
		std::cout << "vectW num = " << i << "\t";
		std::cout << "iters = " << counter << "\t";
		std::cout << "error = " << std::abs(sum2 - int(sum2 + 0.5)) << "\t";
		std::cout << "time = " << smLib::doubleRound(myTime.elapsed() / 1000., 1) << " sec" << std::endl;
    }
}

void dealWithEyes(matrix & inData,
                  const int dimension)
{

    const int dataLen = inData.cols();
    int eyes = 0;
    for(int i = 0; i < dataLen; ++i)
    {
		const std::valarray<double> temp = inData.getCol(i, dimension);
		if(std::abs(temp).max() == 0.)
        {
            ++eyes;
        }
    }
    const double realSignalFrac =  double(dataLen - eyes) / dataLen; /// deprecate?!! splitZeros

    for(int i = 0; i < dimension; ++i)
    {
		const double temp = - smLib::mean(inData[i]) * realSignalFrac;

        std::for_each(begin(inData[i]),
                      end(inData[i]),
                      [temp](double & in)
        {
            if(in != 0.)
            {
                in += temp;
            }
        }); // retain zeros
	}
}

void ica(const matrix & initialData,
         matrix & matrixA,
         double eigenValuesTreshold,
         double vectorWTreshold)
{
    const int ns = initialData.rows();

    matrix centeredMatrix = initialData;
    dealWithEyes(centeredMatrix,
                 ns);

    matrix eigenVectors;
	std::valarray<double> eigenValues;

    svd(centeredMatrix,
        eigenVectors,
        eigenValues,
        ns,
        eigenValuesTreshold);

    matrix D_minus_05(ns, ns, 0.);
    for(int i = 0; i < ns; ++i)
    {
        D_minus_05[i][i] = 1. / sqrt(eigenValues[i]);
    }

    matrix dataICA = (eigenVectors * (D_minus_05 * matrix::transpose(eigenVectors))) *
                     centeredMatrix;

    matrix vectorW(ns, ns);
    countVectorW(vectorW,
                 dataICA,
                 ns,
                 initialData.cols(),
                 vectorWTreshold);
    dataICA = vectorW * dataICA;

    matrix D_05(ns, ns, 0.);
    for(int i = 0; i < ns; ++i)
    {
        D_05[i][i] = sqrt(eigenValues[i]);
    }
    matrixA = eigenVectors * D_05 * matrix::transpose(eigenVectors) * matrix::transpose(vectorW);


    // norm components to 1-length of mapvector, order by dispersion
    double sum1 {};
    for(int i = 0; i < ns; ++i)
    {
		sum1 = smLib::norma(matrixA.getCol(i)) / 2.;

        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] /= sum1;
        }
        dataICA[i] *= sum1;
    }
#if 0
    // ordering components by dispersion
    std::vector<std::pair <double, int>> colsNorms; // dispersion, numberOfComponent
    double sumSquares = 0.; // sum of all dispersions

    for(int i = 0; i < ns; ++i)
    {
        sum1 = variance(dataICA[i]);
        sumSquares += sum1;
        colsNorms.push_back(std::make_pair(sum1, i));
    }

    std::sort(colsNorms.begin(),
              colsNorms.end(),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {
        return i.first > j.first;
    });

    for(int i = 0; i < ns - 1; ++i) // dont move the last
    {
        matrixA.swapCols(i, colsNorms[i].second);

        // swap i and colsNorms[i].second values in colsNorms
        auto it1 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [i](std::pair <double, int> in)
        {
                   return in.second == i;
    });
        auto it2 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [colsNorms, i](std::pair <double, int> in)
        { return in.second == colsNorms[i].second; });

//        std::swap((*it1).second, (*it2).second);

        int tempIndex = (*it1).second;
        (*it1).second = (*it2).second;
        (*it2).second = tempIndex;
    }
#endif
}

void svd(const matrix & initialData,
         matrix & eigenVectors,
		 std::valarray<double> & eigenValues,
         const int dimension, // length of the vectors
         double threshold,
		 int eigenVecNum) /// num of eigenVectors to count - add variance
{
    if(eigenVecNum <= 0)
    {
        eigenVecNum = dimension;
    }
    const int dataLen = initialData.cols();

    const int iterationsThreshold = 100;
    const int errorStep = 5;

    double trace = 0.;
    for(int i = 0; i < dimension; ++i)
    {
		trace += smLib::variance(initialData[i]);
    }

    eigenValues.resize(eigenVecNum);
    eigenVectors.resize(dimension, eigenVecNum);

	std::valarray<double> tempA(dimension);
	std::valarray<double> tempB(dataLen);

    double sum1, sum2; //temporary help values
    double dF, F;
    int counter;

#if 0
    /// ICA test short

    const QString pathForAuxFiles = def::dir->absolutePath()
									+ "/Help"
									+ "/ica";
    QString helpString = pathForAuxFiles
						 + "/" + def::ExpName + "_eigenMatrix.txt";
    readMatrixFile(helpString,
                    eigenVectors);


    // write eigenValues
    helpString = pathForAuxFiles
				 + "/" + def::ExpName + "_eigenValues.txt";
    readFileInLine(helpString, eigenValues);
    return;
#endif

    // maybe lines longer than dimension but OK

	// std::valarray<double> tempLine(dataLen); // for debug acceleration

    matrix inData = initialData;
    const matrix inDataTrans = matrix::transpose(initialData);
    QTime myTime;
    myTime.start();
    //counter j - for B, i - for A
    for(int k = 0; k < eigenVecNum; ++k)
    {
        myTime.restart();

        dF = 0.5;
        F = 1.0;

        tempA = 1. / sqrt(dimension);
        tempB = 1. / sqrt(dataLen);

        //approximate P[i] = tempA x tempB;
        counter = 0;
        while(1) //when stop approximate?
        {
            if((counter) % errorStep == 0)
            {
                //countF - error
                F = 0.;
                for(int i = 0; i < dimension; ++i)
                {
#if 0
                    F += 0.5 * pow(inData[i] - tempB * tempA[i], 2.).sum();
#elif 1
					F += 0.5 * smLib::normaSq(inData[i] - tempB * tempA[i]);
#elif 1
                    // much faster in debug
                    const double coef = tempA[i];
                    std::transform(begin(inData[i]),
                                   end(inData[i]),
                                   begin(tempB),
                                   begin(tempLine),
                                   [coef](double in1, double in2)
                    {
                        return in1 - in2 * coef;
                    });
                    F += 0.5 * normaSq(tempLine);
#else
                    for(int j = 0; j < dataLen; ++j)
                    {
                        F += 0.5 * pow(inData[i][j] - tempB[j] * tempA[i], 2.);
                    }


#endif
                }
            }

            //count vector tempB
			sum2 = 1. / smLib::normaSq(tempA);
            for(int j = 0; j < dataLen; ++j)
            {
//                tempB[j] = sum2 * (inDataTrans[j] * tempA).sum();
                // slightly faster
//                tempB[j] = sum2 * std::inner_product(begin(tempA),
//                                                     end(tempA), // begin + dimension
//                                                     begin(inDataTrans[j]),
//                                                     0.);
				tempB[j] = sum2 * smLib::prod(tempA, inDataTrans[j]);
            }

            //count vector tempA
			sum2 = 1. / smLib::normaSq(tempB);
            for(int i = 0; i < dimension; ++i)
            {
//                tempA[i] = sum2 * (tempB * inData[i]).sum();
                // slightly faster
//                tempA[i] = sum2 * std::inner_product(begin(tempB),
//                                                     end(tempB),
//                                                     begin(inData[i]),
//                                                     0.);
				tempA[i] = sum2 * smLib::prod(tempB, inData[i]);
            }

            if((counter) % errorStep == 0)
            {
                dF = 0.;
                for(int i = 0; i < dimension; ++i)
                {
#if 0
                    dF += 0.5 * pow((inData[i] - tempB * tempA[i]), 2.).sum();
#elif 1
					dF += 0.5 * smLib::normaSq(inData[i] - tempB * tempA[i]);
#elif 1
                    // much faster in debug
                    const double coef = tempA[i];
                    std::transform(begin(inData[i]),
                                   end(inData[i]),
                                   begin(tempB),
                                   begin(tempLine),
                                   [coef](double in1, double in2)
                    {
                        return in1 - in2 * coef;
                    });
                    dF += 0.5 * normaSq(tempLine);
#else
                    for(int j = 0; j < dataLen; ++j)
                    {
                        dF += 0.5 * pow(inData[i][j] - tempB[j] * tempA[i], 2.);
                    }
#endif
                }
                dF = 1. - dF / F;

            }

            if(counter == iterationsThreshold)
            {
                break;
            }
            ++counter;
            if(std::abs(dF) < threshold) break; //crucial cap
        }

        //edit currMatrix
        /// test!
        for(int i = 0; i < dimension; ++i)
        {
#if 1
            // better in release
            inData[i] -= tempA[i] * tempB;
#else
            // better in debug
            const double coef = tempA[i];
            std::transform(begin(inData[i]),
                           end(inData[i]),
                           begin(tempB),
                           begin(tempLine),
                           [coef](double in1, double in2)
            {
                return in1 - in2 * coef;
            });
#endif
        }


        //count eigenVectors && eigenValues
		sum1 = smLib::normaSq(tempA);
		sum2 = smLib::normaSq(tempB);
        eigenValues[k] = sum1 * sum2 / double(dataLen - 1.);
        tempA /= sqrt(sum1);

		sum1 = std::accumulate(std::begin(eigenValues),
							   std::begin(eigenValues) + k + 1,
                               0.);

		std::cout << k << "\t";
		std::cout << "val = " << smLib::doubleRound(eigenValues[k], 3) << "\t";
		std::cout << "disp = " << smLib::doubleRound(100. * eigenValues[k] / trace, 2) << "\t";
		std::cout << "total = " << smLib::doubleRound(100. * sum1 / trace, 2) << "\t";
		std::cout << "iters = " << counter << "\t";
		std::cout << smLib::doubleRound(myTime.elapsed()/1000., 1) << " s" << std::endl;

        for(int i = 0; i < dimension; ++i)
        {
            eigenVectors[i][k] = tempA[i]; // 1-normalized coloumns
        }
    }
}

double morletCos(double const freq1, const double timeShift, const double pot, const double time)
{
    double freq = freq1 * 2. * pi / def::freq;
	return cos(freq * (time - timeShift)) * exp( - pow(freq * (time-timeShift) / pot, 2));
}

double morletSin(double const freq1, const double timeShift, const double pot, const double time)
{
    double freq = freq1 * 2. * pi / def::freq;
	return sin(freq * (time - timeShift)) * exp( - pow(freq * (time-timeShift) / pot, 2));
}


void splineCoeffCount(const std::valarray<double> & inX,
					  const std::valarray<double> & inY,
                      int dim,
					  std::valarray<double> & outA,
					  std::valarray<double> & outB)
{

    //[inX[i-1]...inX[i]] - q[i-1] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
    matrix coefsMatrix(dim, dim, 0);
	std::valarray<double> rightVec(dim);
	std::valarray<double> vectorK(dim);

    //set coefs and rightVec
    coefsMatrix[0][0] = 2.*(inX[1] - inX[0]);
    coefsMatrix[0][1] = (inX[1] - inX[0]);
    rightVec[0] = 3. * (inY[1] - inY[0]);

    coefsMatrix[dim-1][dim-1] = 2.*(inX[dim-1] - inX[dim-2]);
    coefsMatrix[dim-1][dim-2] = (inX[dim-1] - inX[dim-2]);
    rightVec[dim-1] = 3. * (inY[dim-1] - inY[dim-2]);
    for(int i = 1; i < dim-1; ++i) //besides first and last rows
    {
        coefsMatrix[i][i-1] = 1 / (inX[i] - inX[i-1]);
        coefsMatrix[i][i] = 2 * (1 / (inX[i] - inX[i-1]) + 1 / (inX[i+1] - inX[i]));
        coefsMatrix[i][i+1] = 1 / (inX[i+1] - inX[i]);

        rightVec[i] = 3 * ( (inY[i] - inY[i-1]) / ( (inX[i] - inX[i-1]) * (inX[i] - inX[i-1]) ) + (inY[i+1] - inY[i]) / ( (inX[i+1] - inX[i]) * (inX[i+1] - inX[i]) ));
    }
	if(0) //std::cout matrix and rightvec - OK
    {
        for(int i = 0; i < dim; ++i)
        {
            for(int j = 0; j < dim; ++j)
            {
				std::cout << coefsMatrix[i][j] << "\t ";
            }
			std::cout << rightVec[i] << std::endl;
        }
    }
    //count K's
    vectorK = coefsMatrix.matrixSystemSolveGauss(rightVec);
    //count outA and outB
    for(int i = 1; i < dim; ++i) //there is dim-1 intervals between dim points
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
    //[inX[i-1]...inX[i]] - q[i] = (1-t)*inY[i1] + t*inY[i] + t(1-t)(outA[i](1-t) + outB[i]t));
    double t;
    //which interval we have?
    int num = -1; //number of interval
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
							  QString picPath)
{

    const int inLength = arr.size();
	const int fftLen = smLib::fftL(inLength); // int(pow(2., ceil(log(inLength)/log(2.))));
    const double spStep = def::freq / fftLen;
    const double normCoef = sqrt(fftLen / double(inLength));

	std::valarray<double> out; // result
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

    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2. * lowFreq / spStep
           || i > 2. * highFreq / spStep)
            out[i] = 0.;
    }
    for(int i = fftLen; i < 2 * fftLen; ++i)
    {
        if(((2 * fftLen - i) < 2. * lowFreq / spStep)
           || (2 * fftLen - i > 2. * highFreq / spStep))
            out[i] = 0.;
    }
	/// constant component
    out[0] = 0.;
    out[1] = 0.;
    out[fftLen] = 0.;
    out[fftLen+1] = 0.;
    //end filtering

	spectreCtoRrev(out);
    for(int i = 0; i < inLength; ++i)
    {
        filteredArr[i] = out[2 * i] / fftLen / normCoef;
    }


    //Hilbert via FFT
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
        tempArr[i] = out[2 * i + 1] / fftLen * 2; //hilbert
    }
    //end Hilbert via FFT


    for(int i = 0; i < fftLen; ++i)
    {
        out[i] = sqrt(pow(tempArr[i], 2) + pow(filteredArr[i], 2));
    }


    if(!picPath.isEmpty())
    {

        //start check draw - OK
        QPixmap pic(fftLen, 600);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);
        //    double sum, sum2;
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

        pic.save(picPath, 0, 100);
        pic.fill();
        pnt.end();
		std::cout << "hilber drawn" << std::endl;
        //end check draw
    }
    return out;
}


std::valarray<double> hilbertPieces(const std::valarray<double> & arr,
									double srate,
									double lowFreq,
									double highFreq,
									QString picPath)
{
	/// do hilbert transform for the first inLength bins
	const int inLength = arr.size();
	const int fftLen = smLib::fftL(inLength) / 2;

	std::valarray<double> outHilbert(inLength); /// result
	std::valarray<double> tempArr[2];

	const std::valarray<double> filteredArr = myLib::refilter(arr,
															  lowFreq,
															  highFreq,
															  false,
															  srate);

	int start;
	for(int i = 0; i < 2; ++i)
	{
		if(i == 0)
		{
			start = 0;
		}
		else if(i == 1)
		{
			start = inLength - fftLen;
		}

		//Hilbert via FFT
		tempArr[i].resize(fftLen);
		std::copy(std::begin(filteredArr) + start,
				  std::begin(filteredArr) + start + fftLen,
				  std::begin(tempArr[i]));

		tempArr[i] = spectreRtoC(tempArr[i], fftLen);

		std::fill(std::begin(tempArr[i]), std::begin(tempArr[i]) + fftLen, 0.);

		const std::valarray<double> temp = spectreCtoCrev(tempArr[i]);
		tempArr[i].resize(inLength);
		for(int j = 0; j < fftLen; ++j)
		{
			tempArr[i][start + j] = temp[2 * j + 1]; // hilbert. Why +1?
		}
		tempArr[i] *= 2.;
		//end Hilbert via FFT
	}

	for(int i = 0; i < fftLen; ++i)
	{
		outHilbert[i] = sqrt(pow(tempArr[0][i], 2.) + pow(filteredArr[i], 2.));
	}

    int startReplace = 0;
	double minD = 100.; // difference between two possible envelopes
    double helpDouble = 0.;

    for(int i = inLength - fftLen; i < fftLen; ++i)
    {
		helpDouble = outHilbert[i] - pow(pow(tempArr[1][i], 2.) +
                                         pow(filteredArr[i], 2.), 0.5);
		/// could add derivative
        if(std::abs(helpDouble) <= std::abs(minD))
        {
            minD = helpDouble;
            startReplace = i;
        }
    }

    for(int i = startReplace; i < inLength; ++i)
    {
		outHilbert[i] = sqrt(pow(tempArr[1][i], 2.) + pow(filteredArr[i], 2.));
    }


    if(!picPath.isEmpty())
    {

        //start check draw - OK
		QPixmap pic(inLength, 600);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);
		double maxVal = *std::max_element(std::begin(filteredArr),
										  std::end(filteredArr)) * 1.1;

        pnt.setPen("black");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt.drawLine(i,
						 pic.height()/2. * (1. - filteredArr[i] / maxVal),
                         i+1,
						 pic.height()/2. * (1. - filteredArr[i + 1] / maxVal)
                    );
        }

        //        pnt.setPen("blue");
        //        for(int i = 0; i < pic.width()-1; ++i)
        //        {
        //            pnt.drawLine(i, pic.height()/2. - enlarge * tempArr[i], i+1, pic.height()/2. - enlarge * tempArr[i+1]);
        //        }

        pnt.setPen("green");
		for(int i = 0; i < pic.width() - 1; ++i)
        {
			pnt.drawLine(i,
						 pic.height()/2. * (1. - outHilbert[i] / maxVal),
						 i+1,
						 pic.height()/2. * (1. - outHilbert[i + 1] / maxVal)
					);
//			pnt.drawLine(i,
//						 pic.height()/2. * (1. - fftLen * tempArr[0][i] / maxVal),
//						 i+1,
//						 pic.height()/2. * (1. - fftLen * tempArr[0][i + 1] / maxVal)
//					);
        }

        pnt.setPen("blue");
        pnt.drawLine(startReplace,
                     pic.height(),
                     startReplace,
                     0.);

        pnt.end();
        pic.save(picPath, 0, 100);
        //end check draw
    }
    return outHilbert;
}

void makeSine(std::valarray<double> & in,
			  double freq,
			  double phaseInRad,
			  int numPoints,
			  double srate)
{
	if(numPoints < 0)
	{
		numPoints = in.size();
	}
	else
	{
		in.resize(numPoints);
	}
	for(int i = 0; i < numPoints; ++i)
	{
		in[i] = sin(freq * 2 * pi * (i / srate) + phaseInRad);
	}
}

std::valarray<double> bayesCount(const std::valarray<double> & dataIn,
								 int numOfIntervals)
{
    double maxAmpl = 80.; //generality
    int helpInt;
	std::valarray<double> out(0., numOfIntervals);

    for(uint j = 0; j < dataIn.size(); ++j)
    {
        helpInt = int(floor((dataIn[j] + maxAmpl) / (2. * maxAmpl / double(numOfIntervals))));

		if(helpInt != std::min(std::max(0, helpInt), numOfIntervals - 1))
        {
            continue; //if helpInt not in range
        }
        out[helpInt] += 1;
    }
    out /= double(dataIn.size()) * 10.; // 10 is norm coef for perceptron

    return out;
}




/// for fftWindow
//double bes0(double in)
//{
//https://en.wikipedia.org/wiki/Bessel_function#Modified_Bessel_functions
//	return in;
//}


std::valarray<double> fftWindow(int length, windowName name)
{
	std::valarray<double> res(length);
	const double arg = 2. * pi / (length - 1.);
	/// Hann/Hanning
	switch(name)
	{
	case windowName::Hann:
	{
		for(int i = 0; i < length; ++i)
		{
			res[i] = 0.5 * (1. - cos (i * arg));
		}
		break;
	}
	case windowName::Hamming:
	{
		for(int i = 0; i < length; ++i)
		{
			res[i] = 0.53836 - 0.46164 * cos (i * arg);
		}
		break;
	}
	case windowName::Blackman:
	{
		double alpha = 1.;
		for(int i = 0; i < length; ++i)
		{
			res[i] = 0.5 * (1. - alpha) - 0.5 * cos(i * arg) + 0.5 * alpha * cos(2 * i * arg);
		}
		break;
	}
		/// Kaiser
	case windowName::Kaiser:
	{
		res = 1;

//		const double beta = 5.; /// [4; 9]
//		for(int i = 0; i < length; ++i)
//		{
//			res[i] = std::abs(bes0(beta * sqrt(1 - pow(2 * i - length + 1 / (length - 1), 2)))) /
//					 std::abs(bes0(beta));
//		}
		break;
	}
	case windowName::rect:
	{
		res = 1;
		break;
	}
	default:
	{
		res = 1;
	}
	}

	return res;
}



std::valarray<double> smoothSpectre(const std::valarray<double> & inSpectre,
									const int numOfSmooth)
{
	std::valarray<double> result = inSpectre;
    double help1, help2;

	std::vector<double> coefs{1., 1., 1.};
	coefs.push_back(1. / std::accumulate(std::begin(coefs), std::end(coefs), 0.));

    for(int num = 0; num < numOfSmooth; ++num)
    {
        help1 = result[0];
        for(uint i = 1; i < result.size() - 1; ++i)
        {
            help2 = result[i];
			result[i] = (help1 + help2 + result[i + 1]) / 3.;
//			result[i] = (help1 * coefs[0] +
//						help2 * coefs[1] +
//						result[i + 1] * coefs[2]) * coefs[3].;
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
			if(std::abs(data2[j][i]) <= threshold) ++h; // generality 1/8.
		}
		if(h == data2.rows()) eyes += 1;
	}

	if(
	   ( (fftLen - eyes) < def::freq * 3. )
//	   || (eyes > 0.3 * fftLen) /// too much eyes
	   )
	{
		return {};
	}

	matrix res(data2.rows(), 1);
	/// calculate spectra for all channels, but write not all ???
	for(uint i = 0; i < data2.rows(); ++i)
	{
		myLib::calcSpectre(data2[i],
						   res[i],
						   fftLen,
						   numSmooth,
						   eyes);
	}
	return res;
}




void splitZeros(matrix & dataIn,
                int inLength,
				int & outLength,
                const QString & logFilePath,
                const QString & dataName)
{
    //remake with std::list of slices
    std::vector<bool> flags(inLength + 1, false); //1 if usual, 0 if eyes, 1 for additional one point

    bool startFlag = false;
    int start = -1;
    int finish = -1;
    int allEyes = 0;

	std::ofstream outStream;
    int markChan = def::ns - 1; // generality markers channel

    const double firstMarker = dataIn[markChan][0];
    const double lastMarker =  dataIn[markChan][inLength - 1];

//    ofstream of("/media/Files/Data/Mati/BSA/flags.txt");
    for(int i = 0; i < inLength; ++i)
    {
        for(int j = 0; j < def::nsWOM(); ++j) // dont consider markers
        {
            if(dataIn[j][i] != 0.)
            {
                flags[i] = true;
                break;
            }
        }
//        of << flag[i] << std::endl;
    }
    flags[inLength] = true; // for terminate zero piece
//    of.close();




    if(!logFilePath.isEmpty())
    {
		outStream.open(logFilePath.toStdString(), std::ios_base::app);
        if(!outStream.good())
        {
			std::cout << "splitZeros: outStream is not good" << std::endl;
            return;
        }
    }
    else
    {
		std::cout << "splitZeros: logFilePath is empty" << std::endl;
        return;
    }

    //flag[i] == 0 if it's eyes-cut interval
    int i = 0;
    do
    {
        if(startFlag == false) //if startFlag == false
        {
            if(flags[i] == false) // set start
            {
                start = i; // first bin to exclude
                startFlag = true;
            }
        }
        else //if startFlag is set
        {
            if(flags[i]) // if we meet the end of eyes-interval OR reached end of a data
            {

                finish = i; // first bin NOT TO exclude
                outStream << dataName.toStdString() << "\t";
                outStream << start + allEyes << "\t";
                outStream << finish + allEyes << "\t";
                outStream << finish - start << "\t"; // length

                outStream << (start + allEyes) / def::freq << "\t"; // start time
                outStream << (finish + allEyes) / def::freq << "\t"; // finish time
				outStream << (finish - start) / def::freq << std::endl; // length

                //split. vector.erase();
                for(int j = 0; j < def::ns; ++j) //shift with markers and flags
                {
                    // vector
//                    dataIn[j].erase(dataIn[j].begin() + start,
//                                    dataIn[j].begin() + finish);
                    // valarray, resizing further
                    std::remove_if(begin(dataIn[j]) + start,
                                   begin(dataIn[j]) + finish,
                                   [](double){ return true; });
                }
                flags.erase(flags.begin() + start,
                            flags.begin() + finish);

                allEyes += finish - start;
                i -= (finish - start);
                startFlag = false;
            }
        }
        ++i;
    } while (i <= inLength - allEyes); // = for the last zero piece
	outLength = inLength - allEyes;

    // valarray
	dataIn.resizeCols(outLength);

    outStream.close();

//    std::cout << "allEyes = " << allEyes << std::endl;

    dataIn[markChan][0] = firstMarker;
	dataIn[markChan][outLength - 1] = lastMarker;
}


void calcSpectre(const std::valarray<double> & inSignal,
				 std::valarray<double> & outSpectre,
                 int fftLength,
                 int NumOfSmooth,
                 int Eyes,
                 double powArg)
{
    if(inSignal.size() != fftLength)
    {
		std::cout << "calcSpectre: inappropriate signal length" << std::endl;
        return;
    }

	const double nrm = 2. / (double(fftLength - Eyes) * def::freq);
	outSpectre = spectreRtoR(inSignal, fftLength) * nrm;
//	outSpectre = pow(outSpectre, powArg);


	const double norm1 = sqrt(fftLength / double(fftLength - Eyes));
    //smooth spectre
    const int leftSmoothLimit = 1;
    const int rightSmoothLimit = fftLength / 2 - 1;
	double help1, help2;
	for(int a = 0; a < (int)(NumOfSmooth / norm1); ++a)
    {
        help1 = outSpectre[leftSmoothLimit - 1];
        for(int k = leftSmoothLimit; k < rightSmoothLimit; ++k)
        {
            help2 = outSpectre[k];
            outSpectre[k] = (help1 + help2 + outSpectre[k+1]) / 3.;
            help1 = help2;
        }
    }
}




void eyesProcessingStatic(const std::vector<int> eogChannels,
						  const std::vector<int> eegChannels,
						  const QString & windsDir,
						  const QString & outFilePath)
{
	QTime myTime;
	myTime.start();


	QStringList leest;
	/// if cut from reals
//    makeFullFileList(windsDir, leest);
	/// if cut from edf
	leest = QDir(windsDir).entryList({"*" + def::plainDataExtension});

	for(QString & item : leest)
	{
		item.prepend(windsDir + "/");
	}

	const uint Size = eogChannels.size() + 1; // usually 3

	matrix dataE{};

	//make dataE array to count covariation matrix
	int NumOfSlices = 0;
	for(auto filePath : leest)
	{
		readPlainData(filePath, dataE, NumOfSlices);
		NumOfSlices = dataE.cols();
	}

	std::vector<int> signalNums;
	for(int eogNum : eogChannels)
	{
		signalNums.push_back(eogNum);
	}

	matrix matrixInit(Size, Size);
	matrix coefficients(eegChannels.size(), eogChannels.size());

	for(uint k = 0; k < eegChannels.size(); ++k)
	{
		signalNums.push_back(eegChannels[k]);
		for(uint j = 0; j < Size; ++j)
		{
			for(uint z = j; z < Size; ++z)
			{
				matrixInit[j][z] = smLib::prod(dataE[signalNums[j]],
						dataE[ signalNums[z]])
						/ NumOfSlices;
				// maybe (NumOfSlices-1), but it's not important here
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
		signalNums.pop_back();
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
		#if DSP_LIB
		&myDsp::refilter;
#else
		&myLib::refilterFFT;
#endif

}// namespace myLib
