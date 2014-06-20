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
void kernelEst(double *arr, int num, QString picPath);

double mean(double *arr, int length);
double variance(double *arr, int length);
void countRCP(QString filename, QString picPath = "");
void svd(double ** inData, int dim, int length, double *** eigenVects, double ** eigenValues);

double doubleRound(double in, int numSigns);
double mean(int *arr, int length);
double variance(double *arr, int length);
double correlation(double *arr1, double *arr2, int length, int t = 0);
double quantile(double arg);
double maxValue(double * arr, int length);
double minValue(double * arr, int length);


void readDataFile(ifstream & file, QString filename, double *** outData, int ns, int * NumOfSlices, int fftLength);
void readDataFile(ifstream & file, QString filename, double *** outData, int ns, int * NumOfSlices);
void readSpectraFile(ifstream & file, QString filename, double ** outData, int ns, int spLength);
void readSpectraFileLine(ifstream & file, QString filename, double ** outData, int ns, int spLength);
void readPaFile(ifstream & paSrc, QString paFile, double *** matrix, int NetLength, int NumOfClasses, int * NumberOfVectors, char *** FileName);
void readICAMatrix(QString path, double *** matrixA, int ns);
void writeICAMatrix(QString path, double ** matrixA, int ns);

void splitZeros(double *** inData, int ns, int length, int * outLength);
void splitZerosEdges(double *** dataIn, int ns, int length, int * outLength);

void calcSpectre(double ** inData, double *** dataFFT, int ns, int fftLength, int Eyes, int NumOfSmooth);


double distance(double * vec1, double * vec2, int dim);
void matrixProduct(double ** A, double ** B, double *** out, int dimH, int dimL);  //matrix product: out = A(H*H) * B(H*L)
void matrixProduct(double ** A, double ** B, double *** out, int dimA1, int dimB2, int dimA2B1);  //matrix product: out = A(H*H) * B(H*L)
void matrixTranspose(double ** inMat, int size, double *** outMat);
void matrixCopy(double ** inMat, double *** outMat, int dimH, int dimL);
void matrixInvert(double ** inMat, int size, double *** outMat); //cofactors
double matrixDet(double ** matrix, int dim);
double matrixDetB(double ** matrix, int dim);
void matrixCofactor(double ** inMatrix, int size, int i, int j, double *** outMatrix);

double ** matrixCreate(int i, int j);
void matrixDelete(double *** matrix, int i, int j);
void matrixPrint(double ** mat, int i, int j);

void drawArray(double ***sp, int count, int *spL, QStringList colours, int type, double scaling, int left, int right, double spStep, QString outName, QString rangePicPath, QDir * dirBC);

inline double gaussian(double x)
{
    return 1./(sqrt(2. * pi)) * exp(-x*x / 2.);
}

inline double logistic(double &x, double t)
{
    return 1. / ( 1. + exp(-x/t) );
}

#endif // LIBRARY_H
