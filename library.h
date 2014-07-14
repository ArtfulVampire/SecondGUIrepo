#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QtGui>
#include <QtSvg>
#include "coord.cpp"
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
#include <QPainter>
#include <QMessageBox>
#include <cerrno>
#include <ios>
#include <omp.h>
//#include <itpp/itbase.h>
//#include <mlpack/core.hpp>
//#include <mlpack/methods/pca/pca.hpp>
#define pi 3.141592653589



using namespace std;
//using namespace itpp;
//using namespace mlpack;


QString rightNumber(int &input, int N);

void wavelet(QString out, FILE * file, int ns, int channelNumber, double freqMax, double freqMin, double freqStep, double pot);
void drawColorScale(QString filename, int range);
QColor hue(int range, int j, double V, double S);
void waveletPhase(QString out, FILE * file, int ns, int channelNumber1, int channelNumber2, double freqMax, double freqMin, double freqStep, double pot);

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
double sigma(int *arr, int length);
double sigma(double *arr, int length);
double skewness(double *arr, int length);
double kurtosis(double *arr, int length);
double rankit(int i, int length, double k = 0.375);

void drawRCP(double *values, int length);

void countRCP(QString filename, QString picPath = "", double *outMean = NULL, double *outSigma = NULL);
void svd(double ** inData, int dim, int length, double *** eigenVects, double ** eigenValues);

double doubleRound(double in, int numSigns);
double correlation(double *arr1, double *arr2, int length, int t = 0);
double maxValue(double * arr, int length);
double minValue(double * arr, int length);


void readDataFile(QString filename, double *** outData, int ns, int * NumOfSlices, int fftLength);
void readDataFile(QString filename, double *** outData, int ns, int * NumOfSlices);
void readSpectraFile(QString filename, double ** outData, int ns, int spLength);
void readSpectraFileLine(QString filename, double ** outData, int ns, int spLength);
void readPaFile(QString paFile, double *** matrix, int NetLength, int NumOfClasses, int * NumberOfVectors, char *** FileName);
void readICAMatrix(QString path, double *** matrixA, int ns);
void writeICAMatrix(QString path, double ** matrixA, const int ns);

void splitZeros(double *** inData, int ns, int length, int * outLength);
void splitZerosEdges(double *** dataIn, int ns, int length, int * outLength);

void calcSpectre(double ** const inData, double *** dataFFT, int const ns, int const fftLength, const int Eyes, int const NumOfSmooth, const double powArg);
void calcRawFFT(double ** const inData, double *** dataFFT, int const ns, int const fftLength, int Eyes, int const NumOfSmooth);


double distance(double * const vec1, double * const vec2, int const dim);
void matrixProduct(double ** const inMat1, double ** const inMat2, double *** outMat, int const dimH, int const dimL);  //matrix product: out = A(H*H) * B(H*L)
void matrixProduct(double ** const inMat1, double ** const inMat2, double *** outMat, int const numRows1, int const numCols2, int const numCols1Rows2);  //matrix product: out = A(H*H) * B(H*L)
void matrixTranspose(double ** const inMat, int const numRows, int const numCols, double *** outMat);
void matrixCopy(double ** const inMat, double *** outMat, int const dimH, int const dimL);
void matrixInvert(double ** const inMat, int const size, double *** outMat); //cofactors
double matrixDet(double ** const matrix, int const dim);
double matrixDetB(double ** const matrix, int const dim);
void matrixCofactor(double ** const inMatrix, int size, int numRows, int numCols, double *** outMatrix);

double matrixInnerMaxCorrelation(double ** const inMatrix, const int numRows, const int numCols);
double matrixMaxCorrelation(double ** const inMat1, double ** const inMat2, int const numRows, int const numCols);
void matrixCorrelations(double ** const inMat1, double ** const inMat2, int const numRows, int const numCols, double **resCorr);

double ** matrixCreate(int i, int j);
void matrixCreate(double *** matrix, int i, int j);
void matrixDelete(double *** matrix, int i, int j);
void matrixPrint(double ** const mat, int i, int j);

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
