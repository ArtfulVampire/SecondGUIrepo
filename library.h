#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QtGui>
#include <QtSvg>
#include <QFile>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>

#include "coord.h"
#include "matrix.h"
#include "smallLib.h"
#include <DspFilters/Dsp.h>

#include <ios>
#include <iostream>
#include <fstream>

#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <cstdio>

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <valarray>
#include <set>
#include <algorithm>
#include <complex>
#include <utility>

#include <typeinfo>
#include <chrono>
#include <random>
#include <cerrno>
#include <thread>

#ifdef _OPENMP
#include <omp.h>
#endif

#define OLD_DATA 0


#include <myLib/signalProcessing.h>
#include <myLib/wavelet.h>
#include <myLib/dataHandlers.h>
#include <myLib/draws.h>



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
}

namespace myLib
{

std::string funcName(std::string in);
#define TIME(arg)\
    do{\
        auto t0 = std::chrono::high_resolution_clock::now();\
        arg;\
        auto t1 = std::chrono::high_resolution_clock::now();\
        std::cout << myLib::funcName(#arg) \
        << ": time elapsed = "\
        << std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count()/1000.\
        << " nsec" << std::endl;\
    }while(false)


// consts
const double pi_min_025 = pow(pi, -0.25);
const double pi_sqrt = sqrt(pi);
const QString slash = "/"; // QString(QDir::separator());

template <typename Typ>
inline QString nm(Typ in) {return QString::number(in);}
template QString nm(int in);
template QString nm(double in);





// small shit
void writeWavFile(const std::vector<double> & inData, const QString & outPath);
int len(const QString & s); // string length for EDF+ annotations

QString rightNumber(const unsigned int input, int N); // prepend zeros
QString fitNumber(const double & input, int N); // append spaces
QString fitString(const QString & input, int N); // append spaces

QString setFileName(const QString & initNameOrPath); //-> initName_i.ext
QString getPicPath(const QString & dataPath,
                   const QDir * ExpNameDir = def::dir,
                   const int & ns = def::ns);
QString getFileName(const QString & filePath, bool withExtension = true);

bool areEqualFiles(QString path1, QString path2);
double areSimilarFiles(const QString & path1,
					   const QString & path2);
int countSymbolsInFile(const QString & filePath, char inChar);


QString getExpNameLib(const QString & filePath, bool shortened = false);
QString getDirPathLib(const QString & filePath);
QString getExt(const QString & filePath);
QString getLabelName(const QString & label);



std::istream & operator>> (std::istream &is, QString & in);
std::ostream & operator<< (std::ostream &os, const QString & toOut);
std::ostream & operator<< (std::ostream &os, QChar toOut);
std::ostream & operator<< (std::ostream &os, const matrix & toOut);

// containers with no allocators
template <typename Typ, template <typename> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);

template <typename Typ, template <typename, typename = std::allocator<Typ>> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);

char * strToChar(const QString & input);
FILE * fopen(QString filePath, const char *__modes);
char * QStrToCharArr(const QString & input, const int & len = -1);

int getTypeOfFileName(const QString & fileName);
QString getFileMarker(const QString & fileName);


template <typename Container>
uint indexOfMax(const Container & cont);


void makeSine(std::valarray<double> & in,
			  double freq = 10.,
			  double phaseInRad = 0.,
			  int numPoints = -1,
			  double srate = 250.);


QString rerefChannel(const QString & initialName,
					 const QString & targetRef = "Ar",
					 const QString & currentNum = "1",
					 const QString & earsChan = "20",
					 const QString & groundChan = "21",
					 const std::vector<QString> & sign = {"-", "+"});



/// colorscales

// jet
const double defV = 1.;
const std::vector<double> colDots = {1/9., 3.25/9., 5.5/9., 7.75/9.};
double red(const int &range, double j, double V = defV, double S = 1.0);
double green(const int &range, double j, double V = defV, double S = 1.0);
double blue(const int &range, double j, double V = defV, double S = 1.0);
QColor hueJet(const int &range, double j);
// hot-to-cold
double red1(int range, int j);
double green1(int range, int j);
double blue1(int range, int j);

QColor hueOld(int range, double j, int numOfContours = 0, double V = 0.95, double S = 1.0);
QColor grayScale(int range, int j);









/// "static" functions
const std::vector<int> leest19 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
void eyesProcessingStatic(const std::vector<int> eogChannels = {21, 22}, // 19 eeg, 2 help, form zero
						  const std::vector<int> eegChannels = leest19,
						  const QString & windsDir = def::dir->absolutePath()
													   + slash + "winds",
                          const QString & outFilePath = def::dir->absolutePath()
                                                        + slash + "eyes.txt");





// drawings





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


/// signal processing




/// non-spectral
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

template <typename signalType = std::valarray<double>>
void kernelEst(const signalType & arr, QString picPath);

template <typename signalType = std::valarray<double>>
void histogram(const signalType & arr,
			   int numSteps,
			   const QString & picPath = QString(),
			   std::pair<double, double> xMinMax = {},
			   const QString & color = "drakgray",
			   int valueMax = 0);


double quantile(double arg);
void kernelEst(QString filePath, QString picPath);

double rankit(int i, int length, double k = 0.375);
bool gaussApproval(double * arr, int length); // not finished?
bool gaussApproval(QString filePath); // not finished?
bool gaussApproval2(double * arr, int length); // not finished?



template <typename Typ>
double mean(const Typ &arr, int length, int shift = 0);

template <typename Typ>
double variance(const Typ &arr, int length, int shift = 0, bool fromZero = false);

template <typename Typ>
double sigma(const Typ &arr, int length, int shift = 0, bool fromZero = false);

/// remake with
template <typename Typ>
double covariance (const Typ & arr1,
                   const Typ & arr2,
                   int length,
                   int shift = 0,
                   bool fromZero = false);

template <typename Typ>
double correlation(const Typ &arr1,
                   const Typ &arr2,
                   int length,
                   int shift = 0,
                   bool fromZero = false);


template <typename T>
double distance(const std::vector<T> & vec1,
				const std::vector<T> & vec2,
                const int &dim);

double distance(double const x1, double const y1,
                double const x2, double const y2);


double countAngle(double initX, double initY);




/// what is RCP ???
void drawRCP(const std::valarray<double> & values,
             const QString & picPath);
void countRCP(QString filename,
              QString picPath  = QString(),
              double * outMean = nullptr,
              double * outSigma = nullptr);

void splitZeros(matrix & inData,
                const int & inLength,
                int * outLength,
                const QString & logFile = QString(),
                const QString & dataName = def::ExpName);

void splitZerosEdges(matrix & dataIn, const int & ns, const int & length, int * outLength);
void zeroData(matrix & inData, const int & leftLimit, const int & rightLimit);





/// products for ICA
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

void dealWithEyes(matrix & inData,
                  const int dimension);

void ica(const matrix & initialData,
         matrix & matrixA,
         double eigenValuesTreshold, double vectorWTreshold);

void svd(const matrix & initialData,
         matrix & eigenVectors,
		 std::valarray<double> & eigenValues,
         const int dimension,
         const double & threshold = 1e-9,
         int eigenVecNum = -1);




template <typename Typ>
void calcRawFFT(const Typ & inData, mat & dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth);





// mati
/// use bitset
std::vector<bool> matiCountByte(const double & marker);
QString matiCountByteStr(const double & marker);
void matiPrintMarker(double const & marker, QString pre  = QString());
void matiFixMarker(double & marker);
int matiCountDecimal(std::vector<bool> byteMarker);
int matiCountDecimal(QString byteMarker);
inline bool matiCountBit(double const & marker, int num)
{
    return (int(marker) / int(pow(2, num))) % 2;
}




// do sammon class
//sammon
void drawSammon(const coordType & plainCoords,
				const std::vector<int> & types,
                const QString & picPath);

void drawShepard(const mat & distOld,
                 const mat & distNew,
                 const QString & picPath);

void sammonProj(const mat & distOld,
				const std::vector<int> & types,
                const QString & picPath);
double errorSammon(const mat & distOld,
                   const mat & distNew);

void moveCoordsGradient(coordType & plainCoords,
                        const mat & distOld,
                        mat & distNew);
void refreshDistAll(mat & distNew,
                    const coordType & plainCoords);

void refreshDist(mat & dist,
                 const coordType & testCoords,
                 const int input);

void countGradient(const coordType & plainCoords,
                   const mat &distOld,
                   mat &distNew,
				   std::vector<double> &gradient);

void sammonAddDot(const mat & distOld,
                  mat & distNew, // change only last coloumn
                  coordType & plainCoords,
				  const std::vector<int> & placedDots);

void countDistNewAdd(mat & distNew, // change only last coloumn
                     const coordType &crds,
					 const std::vector<int> & placedDots);

void countGradientAddDot(const mat & distOld,
                         const mat & distNew,
                         const coordType & crds,
						 const std::vector<int> & placedDots,
						 std::vector<double>  & gradient);

void countInvHessianAddDot(const mat & distOld,
                           const mat & distNew,
                           const coordType & crds,
						   const std::vector<int> & placedDots,
                           mat & invHessian);

double errorSammonAdd(const mat & distOld,
                      const mat & distNew,
					  const std::vector<int> & placedDots);



} // myLib namespace

namespace deprecate
{

}


#endif // LIBRARY_H
