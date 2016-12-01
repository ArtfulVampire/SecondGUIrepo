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

} // namespace myLib

#endif // SIGNALPROCESSING_H
