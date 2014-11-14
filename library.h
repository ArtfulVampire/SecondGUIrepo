#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QtGui>
#include <QtSvg>
#include <coord.cpp>
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

#ifdef _OPENMP
#include <omp.h>
#endif

//#include <itpp/itbase.h>
//#include <mlpack/core.hpp>
//#include <mlpack/methods/pca/pca.hpp>
#define pi 3.141592653589



using namespace std;
//using namespace itpp;
//using namespace mlpack;

int len(QString s);
QString rightNumber(int &input, int N);
double doubleRound(double in, int numSigns);

//wavelets
void drawColorScale(QString filename, int range);
QColor hueJet(int range, int j, double V = 0.95, double S = 1.0);
double morletCos(double const freq1, double timeShift, double pot, double time);
double morletSin(double const freq1, double timeShift, double pot, double time);
void wavelet(QString out, FILE * file, int ns=19, int channelNumber=0, double freqMax=20., double freqMin=5., double freqStep=0.99, double pot=32.);
void waveletPhase(QString out, FILE * file, int ns, int channelNumber1, int channelNumber2, double freqMax, double freqMin, double freqStep, double pot);

//signal processing
double fractalDimension(double *arr, int N, QString picPath);
double enthropy(double *arr, int N, QString picPath, int numOfRanges);
void four1(double * dataF, int nn, int isign);
void hilbert(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath);
void hilbertPieces(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath);
void bayesCount(double * dataIn, int length, int numOfIntervals, double **out);
void kernelEst(double *arr, int length, QString picPath);
bool gaussApproval(double * arr, int length);
bool gaussApproval(QString filePath);
bool gaussApproval2(double * arr, int length);
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
double correlation(double * const arr1, double * const arr2, int length, int t = 0);
double correlationFromZero(double * const arr1, double * const arr2, int length, int t = 0);
double maxValue(double * arr, int length);
double minValue(double * arr, int length);
void splitZeros(double *** inData, int ns, int length, int * outLength);
void splitZerosEdges(double *** dataIn, int ns, int length, int * outLength);

bool MannWhitney(double * arr1, int len1, double * arr2, int len2, double p);
int typeOfFileName(QString fileName);
void makePaFile(QString spectraDir, QStringList fileNames, int ns, int spLength, int NumOfClasses, double coeff, QString outFile);
void makeMatrixFromFiles(QString spectraDir, QStringList fileNames, int ns, int spLength, double coeff, double *** outMatrix);

void drawRCP(double *values, int length);
void countRCP(QString filename, QString picPath = "", double *outMean = NULL, double *outSigma = NULL);
void svd(double ** inData, int dim, int length, double *** eigenVects, double ** eigenValues);


void readDataFile(QString filePath, double *** outData, int ns, int * NumOfSlices, int fftLength);
void readDataFile(QString filePath, double *** outData, int ns, int * NumOfSlices);
void readSpectraFile(QString filePath, double *** outData, int ns, int spLength);
void readSpectraFileLine(QString filePath, double ** outData, int ns, int spLength);
void readFileInLine(QString filePath, double ** outData, int len);
void readPaFile(QString paFile, double *** matrix, int NetLength, int NumOfClasses, int * NumberOfVectors, char *** FileName, double **classCount);
bool readICAMatrix(QString path, double *** matrixA, int ns);
void writeICAMatrix(QString path, double ** matrixA, const int ns);

QColor mapColor(double minMagn, double maxMagn, double ** helpMatrix, int numX, int numY, double partX, double partY);
void drawMap(double ** const matrixA, QString outDir, QString outName, int num, int size = 240);
void drawICAMaps(QString mapsPath, int ns, QString outDir, QString outName);
void drawMapsOnSpectra(QString spectraFilePath, QString outSpectraFilePath, QString mapsPath, QString mapsNames);


void calcSpectre(double ** const inData, double *** dataFFT, int const ns, int const fftLength, const int Eyes, int const NumOfSmooth = 15, const double powArg = 1.);
void calcSpectre(double ** const inData, int leng, int const ns, double *** dataFFT, int * fftLength, int const NumOfSmooth = 15, const double powArg = 1.);
void calcRawFFT(double ** const inData, double *** dataFFT, int const ns, int const fftLength, int Eyes, int const NumOfSmooth);


double distance(double * const vec1, double * const vec2, int const dim);
double distance(double const x1, double const y1, double const x2, double const y2);
double distanceMah(double * const vect, double ** const covMatrix, double * const groupMean, int dimension);
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
double matrixDet(double ** const matrix, int const dim);
double matrixDetB(double ** const matrix, int const dim);
void matrixCofactor(double ** const inMatrix, int size, int numRows, int numCols, double *** outMatrix);

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

inline double gaussian(double x) //N(0,1)
{
    return 1./(sqrt(2. * pi)) * exp(-x*x / 2.);
}

inline double logistic(double &x, double t)
{
    return 1. / ( 1. + exp(-x/t) );
}

#endif // LIBRARY_H
