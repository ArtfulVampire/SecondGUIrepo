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



double fractalDimension(const std::valarray<double> &arr,
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

std::valarray<double> bayesCount(const std::valarray<double> & dataIn, int numOfIntervals);

/// whaaaaat???
void splineCoeffCount(const std::valarray<double> & inX,
					  const std::valarray<double> & inY,
					  int dim,
					  std::valarray<double> & outA,
					  std::valarray<double> & outB); //[inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
double splineOutput(const std::valarray<double> & inX,
					const std::valarray<double> & inY,
					int dim,
					const std::valarray<double> & A,
					const std::valarray<double> & B,
					double probeX);

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


const std::vector<int> leest19 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
void eyesProcessingStatic(const std::vector<int> eogChannels = {21, 22}, // 19 eeg, 2 help, form zero
						  const std::vector<int> eegChannels = leest19,
						  const QString & windsDir = def::dir->absolutePath()
													   + "/winds",
						  const QString & outFilePath = def::dir->absolutePath()
														+ "/eyes.txt");


} // namespace myLib

namespace btr
{
/// from GA fortran
std::valarray<double> butterworth(const std::valarray<double> & in,
								  int order,
								  double srate,
								  double centerFreq,
								  double halfBand);
std::valarray<double> refilterButter(const std::valarray<double> & in,
									 int order,
									 double srate,
									 double lowFreq,
									 double highFreq);
} // namespace btr

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
