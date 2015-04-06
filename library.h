#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QtGui>
#include <QtSvg>
#include "coord.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cmath>
#include <QFile>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <cerrno>
#include <ios>
#include <QDoubleSpinBox>
#include <vector>
#include <typeinfo>


#ifdef _OPENMP
#include <omp.h>
#endif
/////// *** -> **&

using namespace std;
//using namespace itpp;
//using namespace mlpack;

const double pi = 3.141592653589;

//void QDoubleSpinBox::mySlot(double val)
int len(QString s); // string length for EDF+ annotations
QString rightNumber(const unsigned int input, int N); // prepend zeros
QString fitNumber(const double & input, int N); // append spaces
QString fitString(const QString & input, int N); // append spaces

int findChannel(char ** const labelsArr, QString chanName, int ns = 21);
QString setFileName(const QString & initNameOrPath); //-> initName_i.ext
QString getPicPath(const QString & dataPath, QDir *ExpNameDir, int ns);
QString getFileName(QString filePath, bool withExtension = true);
bool areEqualFiles(QString path1, QString path2);

QString getExpNameLib(const QString filePath);
QString getDirPathLib(const QString filePath);
QString getExt(QString filePath);
QString slash();
ostream & operator << (ostream &os, QString toOut);
ostream & operator << (ostream &os, vector<double> toOut); // template!
//vector< vector<double> > operator=(const vector< vector<double> > & other);
char * strToChar(const QString & input);
FILE *fopen(QString filePath, const char *__modes);
char * QStrToCharArr(const QString & input);

//wavelets
void drawColorScale(QString filename, int range, int type = 0);

double red(int range, double j, double V = 0.95, double S = 1.0);
double green(int range, double j, double V = 0.95, double S = 1.0);
double blue(int range, double j, double V = 0.95, double S = 1.0);
QColor hueJet(int range, double j, int numOfContours = 0, double V = 0.95, double S = 1.0);
QColor hueOld(int range, int j);
QColor grayScale(int range, int j);

double morletCos(double const freq1, double timeShift, double pot, double time);
double morletSin(double const freq1, double timeShift, double pot, double time);
void wavelet(QString out, FILE * file, int ns = 19, int channelNumber = 0, double freqMax = 20., double freqMin = 5., double freqStep = 0.99, double pot = 32.);
void waveletPhase(QString out, FILE * file, int ns, int channelNumber1, int channelNumber2, double freqMax, double freqMin, double freqStep, double pot);

//signal processing
double fractalDimension(double *arr, int N, QString picPath = ""); // piPath deprecated
double enthropy(double *arr, int N, QString picPath, int numOfRanges); // not finished?
void four1(double * dataF, int nn, int isign);
void hilbert(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath = "");
void hilbertPieces(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath = "");
void bayesCount(double * dataIn, int length, int numOfIntervals, double **out);
void kernelEst(double *arr, int length, QString picPath);
bool gaussApproval(double * arr, int length); // not finished?
bool gaussApproval(QString filePath); // not finished?
bool gaussApproval2(double * arr, int length); // not finished?
double vectorLength(double * arr, int len);
double quantile(double arg);
double mean(int *arr, int length);
double mean(double *arr, int length);
double variance(int *arr, int length);
double variance(double *arr, int length);
double varianceFromZero(int *arr, int length);
double varianceFromZero(double *arr, int length);
double sigma(int *arr, int length);
double sigma(double *arr, int length);
double sigmaFromZero(int *arr, int length);
double sigmaFromZero(double *arr, int length);
double skewness(double *arr, int length);
double kurtosis(double *arr, int length);
double rankit(int i, int length, double k = 0.375);
double covariance(double * const arr1, double * const arr2, int length);
double correlation(double * const arr1, double * const arr2, int length, int t = 0);
double correlationFromZero(double * const arr1, double * const arr2, int length, int t = 0);
double maxValue(double * arr, int length);
double minValue(double * arr, int length);
void splitZeros(double *** inData, const int &ns, const int &length, int * outLength, const QString &logFile = "", QString dataName = "");
void splitZerosEdges(double *** dataIn, int ns, int length, int * outLength);
void splineCoeffCount(double * const inX, double * const inY, int dim, double ** outA, double ** outB); //[inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
void zeroData(double *** inData, const int & ns, const int & leftLimit, const int & rightLimit, const bool & withMarkersFlag);
double splineOutput(double * const inX, double * const inY, int dim, double * A, double *B, double probeX);
double independence(double * const signal1, double * const signal2, int length);


bool MannWhitney(double * arr1, int len1, double * arr2, int len2, double p);
int typeOfFileName(QString fileName);
void makePaFile(QString spectraDir, QStringList fileNames, int ns, int spLength, int NumOfClasses, double coeff, QString outFile);
void makeMatrixFromFiles(QString spectraDir, QStringList fileNames, int ns, int spLength, double coeff, double *** outMatrix);
void cleanDir(QString dirPath, QString nameFilter = "", bool ext = true);

void drawRCP(double *values, int length);
void countRCP(QString filename, QString picPath = "", double *outMean = NULL, double *outSigma = NULL);
void svd(double ** inData, int dim, int length, double *** eigenVects, double ** eigenValues);
void makeCfgStatic(QString outFileDir, int NetLength = 19*247, QString FileName = "16sec19ch", int numOfOuts = 3, double lrate = 0.1, double error = 0.1, int temp = 10);


void readDataFile(QString filePath, double *** outData, int ns, int * NumOfSlices, int fftLength);
void readDataFile(QString filePath, double *** outData, int ns, int * NumOfSlices);
void writePlainData(double ** data, int ns, int numOfSlices, QString outPath);
void writePlainData(vector< vector <double> > data, QString outPath);
void readPlainData(double **&data, int ns, int & numOfSlices, QString inPath);
QPixmap drawEeg(double **dataD, int ns, int NumOfSlices, int freq, const QString picPath = "", double norm = 1., int blueChan = -1, int redChan = -1);
QPixmap drawEeg(double **dataD, int ns, double startTime, double endTime, int freq, QString const picPath = "", double norm = 1.); // haven't checked? now unused
void readSpectraFile(QString filePath, double *** outData, int ns, int spLength);
void readSpectraFileLine(QString filePath, double ** outData, int ns, int spLength);
void readFileInLine(QString filePath, double ** outData, int len);
void readPaFile(QString paFile, double *** matrix, int NetLength, int NumOfClasses, int * NumberOfVectors, char *** FileName, double **classCount);
bool readICAMatrix(QString path, double *** matrixA, int ns);
void writeICAMatrix(QString path, double ** matrixA, const int ns);

QColor mapColor(double minMagn, double maxMagn, double ** helpMatrix, int numX, int numY, double partX, double partY, bool colour = true);
void drawMap      (double ** const matrixA, double maxAbs, QString outDir, QString outName, int num, int size = 240, bool colourFlag = true);
void drawMapSpline(double ** const matrixA, double maxAbs, QString outDir, QString outName, int num, int size = 240, bool colourFlag = true);
void drawMapsICA(QString mapsPath, int ns, QString outDir, QString outName, bool colourFlag = true,
                 void (*draw1MapFunc)(double ** const matrixA, double maxAbs, QString outDir, QString outName, int num, int size, bool colourFlag) = &drawMapSpline);
void drawMapsOnSpectra(QString spectraFilePath, QString outSpectraFilePath, QString mapsPath, QString mapsNames);


void calcSpectre(double ** const inData, double *** dataFFT, int const ns, int const fftLength, const int Eyes, int const NumOfSmooth = 15, const double powArg = 1.);
void calcSpectre(double ** const inData, int leng, int const ns, double *** dataFFT, int * fftLength, int const NumOfSmooth = 15, const double powArg = 1.);
void calcRawFFT(double ** const inData, double *** dataFFT, int const ns, int const fftLength, int Eyes, int const NumOfSmooth);

double distance(vector<double> vec1, vector<double> vec2, const int dim);
double distance(double * const vec1, double * const vec2, int const dim);
double distance(double const x1, double const y1, double const x2, double const y2);
double distanceMah(double * const vect, double ** const covMatrixInv, double * const groupMean, int dimension);
double distanceMah(double * const vect, double ** const group, int dimension, int number);
double distanceMah(double ** const group1, double ** const group2, int dimension, int number1, int number2);
void matrixMahCount(double ** const matrix, int number, int dimension, double *** outMat, double **meanVect);
void matrixProduct(double ** const inMat1, double ** const inMat2, double *** outMat, int const dimH, int const dimL);  //matrix product: out = A(H*H) * B(H*L)
void matrixProduct(double ** const inMat1, double ** const inMat2, double *** outMat, int const numRows1, int const numCols2, int const numCols1Rows2);  //matrix product: out = A(K*H) * B(H*L)
void matrixProduct(double * const vect, double ** const mat, double ** outVect, int dimVect, int dimMat); //outVect = vect * mat
void matrixProduct(double ** const mat, double * const vect, double ** outVect, int dimVect, int dimMat); //outVect = mat * vect
void matrixProduct(double * const vect1, double * const vect2, int dim, double * out);

void matrixTranspose(double ** const inMat, int const numRows, int const numCols, double *** outMat);
void matrixTranspose(double *** inMat, int const numRowsCols);
void matrixCopy(double ** const inMat, double *** outMat, int const dimH, int const dimL);
void matrixInvert(double ** const inMat, int const size, double *** outMat);
void matrixInvert(double *** mat, int const size);
void matrixInvertGauss(double *** mat, int const size);
void matrixInvertGauss(double ** const mat, int const size, double *** outMat);
double matrixDet(double ** const matrix, int const dim);
double matrixDetB(double ** const matrix, int const dim);
void matrixCofactor(double ** const inMatrix, int size, int numRows, int numCols, double *** outMatrix);
void matrixSystemSolveGauss(double ** const inMat, double * const inVec, int size, double ** outVec);

double matrixInnerMaxCorrelation(double ** const inMatrix, const int numRows, const int numCols,
                                 double (*corrFunc)(double * const arr1, double * const arr2, int length, int t) = &correlationFromZero);

double matrixMaxCorrelation(double ** const inMat1, double ** const inMat2, int const numRows, int const numCols);
void matrixCorrelations(double ** const inMat1, double ** const inMat2, int const numRows, int const numCols, double **resCorr);

double ** matrixCreate(int i, int j);
void matrixCreate(double *** matrix, int i, int j);
void matrixDelete(double *** matrix, int i);
void matrixDelete(int *** matrix, int i);
void matrixPrint(double ** const mat, int i, int j);

void drawArray(double * array, int length, QString outPath); ///////////////////////////////////////////to do
void drawArray(double ***sp, int count, int *spL, QStringList colours, int type, double scaling, int left, int right, double spStep, QString outName, QString rangePicPath, QDir * dirBC);


vector<bool> matiCountByte(const double & marker);
QString matiCountByteStr(const double & marker);
void matiPrintMarker(double const & marker, QString pre = "");
void matiFixMarker(double & marker);
int matiCountDecimal(vector<bool> byteMarker);
int matiCountDecimal(QString byteMarker);



inline double doubleRound(const double & in, int numSigns)
{
    return int(  ceil(in*pow(10., numSigns) - 0.5)  ) / pow(10., numSigns);
}

inline double doubleRoundFraq(const double & in, int denom)
{
    return ceil(in * denom - 0.5) / denom;
}

inline double gaussian(double x, double sigma = 1.) //N(0,1)
{
    return 1./(sigma * sqrt(2. * pi)) * exp(-x * x / (2. * sigma * sigma) );
}

inline double logistic(double &x, double t)
{
    return 1. / ( 1. + exp(-x/t) );
}

inline int fftLimit(double inFreq, double freq, int fftL)
{
    return floor(0.5 + inFreq/freq*fftL);
}

inline int fftL(int in)
{
    return pow(2., ceil(log2(in)));
}


inline bool matiCountBit(double const & marker, int num)
{
//    return (int(marker) >> num) % 2;
    return (int(marker) / int(pow(2, num))) % 2;
}

#endif // LIBRARY_H
