#ifndef SIGNALPROCESSING_H
#define SIGNALPROCESSING_H

#include <valarray>

namespace myLib
{

/// srate for norm
std::valarray<double> spectreRtoR(const std::valarray<double> & inputSignal,
//								  const double srate = 250.,
								  int fftLen = -1);

std::valarray<double> spectreRtoC(const std::valarray<double> & inputSignal,
//								  const double srate = 250.,
								  int fftLen = -1);

std::valarray<double> spectreCtoR(const std::valarray<double> & inputSignal,
//								  const double srate = 250.,
								  int fftLen = -1);

std::valarray<double> spectreCtoC(const std::valarray<double> & inputSignal,
//								  const double srate = 250.,
								  int fftLen = -1);

std::valarray<double> subSpectrumR(const std::valarray<double> & inputSpectre,
								   double leftFreq,
								   double rightFreq,
								   double srate = 250.);

std::valarray<double> spectreCtoRrev(const std::valarray<double> & inputSpectre);

std::valarray<double> spectreCtoCrev(const std::valarray<double> & inputSpectre);


std::valarray<double> smoothSpectre(const std::valarray<double> & inSpectre,
									const int numOfSmooth);

/// original FFT
void four1(double * dataF, int nn, int isign);

/// from Rosetta stone - the same
void four3(std::valarray<std::complex<double>> & inputArray);

/// by FFT
void refilterSpectre(std::valarray<double> & spectr,
					 int lowLim,
					 int highLim,
					 bool isNotch);

std::valarray<double> refilter(const std::valarray<double> & inputSignal,
							   double lowFreq,
							   double highFreq,
							   bool isNotch = false,
							   double srate = 250.);


std::valarray<double> fftWindow(int length, const QString & name = "Hann");

std::valarray<double> upsample(const std::valarray<double> & inSignal,
							   double oldFreq,
							   double newFreq);

std::valarray<double> downsample(const std::valarray<double> & inSignal,
								 double oldFreq,
								 double newFreq);

int findJump(const std::valarray<double> & inSignal, int startSearch, double numOfSigmas = 3.5);


void calcSpectre(const std::valarray<double> & inSignal,
				 std::valarray<double> & outSpectre,
				 const int & fftLength = def::fftLength,
				 const int & NumOfSmooth = 0,
				 const int & Eyes = 0,
				 const double & powArg = 1.);


} // namespace myLib


namespace myDsp
{
std::valarray<double> reverseArray(const std::valarray<double> & in);
std::valarray<double> lowPassOneSide(const std::valarray<double> & inputSignal,
									 double cutoffFreq,
									 double srate = 250.);
std::valarray<double> lowPass(const std::valarray<double> & inputSignal,
							  double cutoffFreq,
							  double srate = 250.);


/// bandPass/bandStop
std::valarray<double> refilter(const std::valarray<double> & inputSignal,
							   double lowFreq,
							   double highFreq,
							   bool isNotch = false,
							   double srate = 250.);
matrix refilter(const matrix & inputMatrix,
				double lowFreq,
				double highFreq,
				bool isNotch = false,
				double srate = 250.);
std::valarray<double> refilterOneSide(const std::valarray<double> & inputSignal,
									  double lowFreq,
									  double highFreq,
									  bool isNotch = false,
									  double srate = 250.);
} // namespace myDsp


#endif // SIGNALPROCESSING_H
