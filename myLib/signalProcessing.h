#ifndef SIGNALPROCESSING_H
#define SIGNALPROCESSING_H

#include <valarray>
#include <complex>
#include "matrix.h"

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



template <typename signalType = std::valarray<double>>
double fractalDimension(const signalType &arr,
						const QString & picPath = QString());

std::valarray<double> hilbert(const std::valarray<double> & arr,
							  double lowFreq = def::leftFreq,
							  double highFreq = def::rightFreq,
							  QString picPath  = QString());

std::valarray<double> hilbertPieces(const std::valarray<double> & arr,
									double sampleFreq,
									double lowFreq,
									double highFreq,
									QString picPath = QString());

template <typename signalType = std::valarray<double>, typename retType = std::valarray<double>>
retType bayesCount(const signalType & dataIn, int numOfIntervals);

/// Mann-Whitney
int MannWhitney(const std::valarray<double> & arr1,
				 const std::valarray<double> & arr2,
				 const double p = 0.05);
void countMannWhitney(trivector<int> & outMW,
					  const QString & spectraPath,
					  matrix * averageSpectraOut = nullptr,
					  matrix * distancesOut = nullptr);
void MannWhitneyFromMakepa(const QString & spectraDir);

/// ICA
void product1(const matrix & arr,
			  const int length,
			  const int ns,
			  const std::valarray<double> & vect,
			  std::valarray<double> & outVector);

void product2(const matrix & arr,
			  const int length,
			  const int ns,
			  const std::valarray<double> & vect,
			  std::valarray<double> & outVector);

void product3(const matrix & inMat,
			  const int ns,
			  const int currNum,
			  std::valarray<double> & outVector);

void randomizeValar(std::valarray<double> & valar);

void countVectorW(matrix & vectorW,
				  const matrix & dataICA,
				  const int ns,
				  const int dataLen,
				  const double vectorWTreshold);

void ica(const matrix & initialData,
		 matrix & matrixA,
		 double eigenValuesTreshold, double vectorWTreshold);

void svd(const matrix & initialData,
		 matrix & eigenVectors,
		 std::valarray<double> & eigenValues,
		 const int dimension,
		 const double & threshold = 1e-9,
		 int eigenVecNum = -1);



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
