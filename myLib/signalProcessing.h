#ifndef SIGNALPROCESSING_H
#define SIGNALPROCESSING_H

#include <valarray>
#include <complex>
#include <fstream>

#include <other/matrix.h>
#include <other/defaults.h>

#include <QPixmap>
#include <QPainter>
#include <QTime>

namespace myLib
{


std::valarray<double> makeSine(int numPoints = 4096,
							   double freq = 10.,
							   double srate = 250.,
							   double startPhase = 0.);

std::valarray<double> makeNoise(int numPoints = 4096);




enum class windowName {Hann, Hamming, Blackman, rect, triang, Parzen, Welch,
					   sine, Nuttal, BlackmanNuttal, BlackmanHarris, FlatTop, RifeVincent,
					   Gaussian, Lanczos};
std::valarray<double> fftWindow(int N, windowName name = windowName::Hann);

std::valarray<double> spectreRtoR(const std::valarray<double> & inputSignal,
								  int fftLen = -1);

std::valarray<double> spectreRtoC(const std::valarray<double> & inputSignal,
								  int fftLen = -1);

/// srate for norm
std::valarray<std::complex<double>> spectreRtoC2(const std::valarray<double> & inputSignal,
												 int fftLen,
												 double srate);

std::valarray<double> spectreCtoR(const std::valarray<double> & inputSignal,
								  int fftLen = -1);

std::valarray<double> spectreCtoR(const std::valarray<std::complex<double>> & inputSignal,
								  int fftLen = -1);

std::valarray<double> spectreCtoC(const std::valarray<double> & inputSignal,
								  int fftLen = -1);

std::valarray<double> spectreWelchRtoR(const std::valarray<double> & inputSignal,
									   double overlap,
									   double srate,
									   myLib::windowName window,
									   int fftLen);

std::valarray<std::complex<double>> spectreWelchRtoC(const std::valarray<double> & inputSignal,
													 double overlap,
													 double srate,
													 myLib::windowName window,
													 int fftLen);

std::valarray<std::complex<double>> spectreWelchCross(const std::valarray<double> & inputSignal1,
													  const std::valarray<double> & inputSignal2,
													  double overlap,
													  double srate,
													  myLib::windowName window,
													  int fftLen);

std::valarray<std::complex<double>> spectreCross(const std::valarray<double> & inputSignal1,
												 const std::valarray<double> & inputSignal2,
												 double srate,
												 const std::valarray<double> & wnd,
												 int fftLen);

std::valarray<double> subSpectrumR(const std::valarray<double> & inputSpectre,
								   double leftFreq,
								   double rightFreq,
								   double srate);

std::valarray<double> spectreCtoRrev(const std::valarray<double> & inputSpectre);
std::valarray<double> spectreCtoRrev(const std::valarray<std::complex<double>> & inputSpectre);

std::valarray<double> spectreCtoCrev(const std::valarray<double> & inputSpectre);


std::valarray<double> smoothSpectre(const std::valarray<double> & inSpectre,
									int numOfSmooth = 0);

matrix countSpectre(const matrix & inData,
					int fftLen,
					int numSmooth);

/// original FFT
void four1(double * dataF, int nn, int isign);
double spectreNorm(int fftLen, int realSig, double srate);



/// from Rosetta stone - norming on maxValue?
/// currently unused
std::valarray<std::complex<double>> spectreCtoCcomplex(
		const std::valarray<std::complex<double>> & inputArray,
		int fftLen = -1);

std::valarray<std::complex<double>> spectreRtoCcomplex(
		const std::valarray<double> & inputArray,
		int fftLen = -1);

std::valarray<double> spectreRtoRcomplex(
		const std::valarray<double> & inputArray,
		int fftLen = -1);



/// by FFT
void refilterSpectre(std::valarray<double> & spectr,
					 int lowLim,
					 int highLim,
					 bool isNotch);

std::valarray<double> refilterFFT(const std::valarray<double> & inputSignal,
								  double lowFreq,
								  double highFreq,
								  bool isNotch,
								  double srate);

std::valarray<double> lowPassFFT(const std::valarray<double> & inputSignal,
								 double cutoffFreq,
								 double srate);

/// add highPassFFT

matrix refilterMat(const matrix & inputMatrix,
				   double lowFreq,
				   double highFreq,
				   bool isNotch,
				   double srate);




std::valarray<double> upsample(const std::valarray<double> & inSignal,
							   double oldFreq,
							   double newFreq);

std::valarray<double> downsample(const std::valarray<double> & inSignal,
								 double oldFreq,
								 double newFreq);

int findJump(const std::valarray<double> & inSignal, int startSearch, double numOfSigmas = 3.5);


void calcSpectre(const std::valarray<double> & inSignal,
				 std::valarray<double> & outSpectre,
				 int fftLength = DEFS.getFftLen(),
				 int NumOfSmooth = 0,
				 int Eyes = 0,
				 double powArg = 1.);


///some features, double(const std::valarray<double> & in)

/// Higuchi
double fractalDimension(const std::valarray<double> & arr,
						int Kmax = 8,
						const QString & picPath = QString());

double fractalDimensionForTest(const std::valarray<double> & arr,
							   int Kmax,
							   const QString & picPath);

std::pair<double, double> signalBand(const std::valarray<double> & inSignal);

double alphaPeakFreq(const std::valarray<double> & spectreR,
					 int initSigLen,
					 double srate,
					 double leftLimFreq = 8.,
					 double rightLimFreq = 13.);
std::vector<double> integrateSpectre(const std::valarray<double> & spectreR,
									 int initSigLen,
									 double srate,
									 double leftFreqLim = 2.,
									 double rightFreqLim = 19.,
									 double spectreStepFreq = 1.);

double hilbertCarr(const std::valarray<double> & arr);
double hilbertSD(const std::valarray<double> & arr);


std::valarray<double> hilbertPieces(const std::valarray<double> & arr,
									QString picPath = QString());

std::valarray<double> bayesCount(const std::valarray<double> & dataIn, int numOfIntervals);

std::valarray<double> derivative(const std::valarray<double> & inSignal);

double hjorthActivity(const std::valarray<double> & inSignal);
double hjorthMobility(const std::valarray<double> & inSignal);
double hjorthComplexity(const std::valarray<double> & inSignal);

/// whaaaaat???
void splineCoeffCount(const std::valarray<double> & inX,
					  const std::valarray<double> & inY,
					  int dim,
					  std::valarray<double> & outA,
					  std::valarray<double> & outB); // [inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
double splineOutput(const std::valarray<double> & inX,
					const std::valarray<double> & inY,
					int dim,
					const std::valarray<double> & A,
					const std::valarray<double> & B,
					double probeX);


//double fractalDimensionBySpectre(const std::valarray<double> &arr,
//						const QString & picPath = QString());


/// UNUSED - MUST CHECK BEFORE USE
// std::valarray<double> hilbert(const std::valarray<double> & arr,
//							  double lowFreq = DEFS.getLeftFreq(),
//							  double highFreq = DEFS.getRightFreq(),
//							  QString picPath  = QString());


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
		 double threshold = 1e-9,
		 int eigenVecNum = -1);


const std::vector<int> leest19 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
void eyesProcessingStatic(const std::vector<int> eogChannels = {21, 22}, // 19 eeg, 2 help, form zero
						  const std::vector<int> eegChannels = leest19,
						  const QString & windsDir = DEFS.dirPath()
													   + "/winds",
						  const QString & outFilePath = DEFS.dirPath()
														+ "/eyes.txt");


} // namespace myLib



namespace btr
{
/// from GA fortran
/// unused, unchecked
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



#if 0
namespace myDsp
{
std::valarray<double> lowPassOneSide(const std::valarray<double> & inputSignal,
									 double cutoffFreq,
									 double srate);
std::valarray<double> lowPass(const std::valarray<double> & inputSignal,
							  double cutoffFreq,
							  double srate);


/// bandPass/bandStop
std::valarray<double> refilter(const std::valarray<double> & inputSignal,
							   double lowFreq,
							   double highFreq,
							   bool isNotch,
							   double srate);
matrix refilter(const matrix & inputMatrix,
				double lowFreq,
				double highFreq,
				bool isNotch,
				double srate);
std::valarray<double> refilterOneSide(const std::valarray<double> & inputSignal,
									  double lowFreq,
									  double highFreq,
									  bool isNotch,
									  double srate);
} // namespace myDsp
#endif

namespace butter
{
constexpr int ORDER_band = 40;
constexpr int ORDER_cut = 16;


/// one sided
std::valarray<double> butterworthBandStop(const std::valarray<double> & in,
										  double fLow,
										  double fHigh,
										  double srate,
										  int n = butter::ORDER_band);

std::valarray<double> butterworthBandPass(const std::valarray<double> & in,
										  double fLow,
										  double fHigh,
										  double srate,
										  int n = butter::ORDER_band);

std::valarray<double> butterworthHighPass(const std::valarray<double> & in,
										  double cutoff,
										  double srate,
										  int n = butter::ORDER_cut);

std::valarray<double> butterworthLowPass(const std::valarray<double> & in,
										 double cutoff,
										 double srate,
										 int n = butter::ORDER_cut);


/// two sided
std::valarray<double> butterworthBandStopTwoSided(const std::valarray<double> & in,
												  double fLow,
												  double fHigh,
												  double srate);

std::valarray<double> butterworthBandPassTwoSided(const std::valarray<double> & in,
												  double fLow,
												  double fHigh,
												  double srate);

std::valarray<double> butterworthHighPassTwoSided(const std::valarray<double> & in,
												  double cutoff,
												  double srate);

std::valarray<double> butterworthLowPassTwoSided(const std::valarray<double> & in,
												 double cutoff,
												 double srate);

std::valarray<double> refilter(const std::valarray<double> & inputSignal,
							   double lowFreq,
							   double highFreq,
							   bool isNotch,
							   double srate);



} // namespace butter

namespace myLib
{

extern std::valarray<double> (* refilter)(const std::valarray<double> & inputSignal,
								  double lowFreq,
								  double highFreq,
								  bool isNotch,
								  double srate);
//		&butter::refilter;
//		&myDsp::refilter;

std::valarray<double> (* const lowPass)(const std::valarray<double> & inputSignal,
								  double cutoffFreq,
								  double srate) =
		&butter::butterworthLowPassTwoSided;
//		&myDsp::lowPass;

std::valarray<double> (* const highPass)(const std::valarray<double> & inputSignal,
								  double cutoffFreq,
								  double srate) =
		&butter::butterworthHighPassTwoSided;
}


#endif // SIGNALPROCESSING_H
