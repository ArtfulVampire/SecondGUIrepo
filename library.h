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
#define pi 3.141592653589

using namespace std;

QString rightNumber(int &input, int N);

void wavelet(QString out, FILE * file, int ns, int channelNumber, double freqMax, double freqMin, double freqStep, double pot);
void drawColorScale(QString filename, int range);
QColor hue(int range, int j, double V, double S);
void waveletPhase(QString out, FILE * file, int ns, int channelNumber1, int channelNumber2, double freqMax, double freqMin, double freqStep, double pot);

double fractalDimension(double *arr, int N, QString picPath);
double chaosDimension(double *arr, int N, QString picPath);
double enthropy(double *arr, int N, QString picPath, int numOfRanges);
void four1(double *dataF, int nn, int isign);
double * hilbert(double * arr, int fftLen, double sampleFreq, double lowFreq, double highFreq);

double mean(double *arr, int length);
double variance(double *arr, int length);


double mean(int *arr, int length);
double variance(int *arr, int length);
double correlation(double *arr1, double *arr2, int length, int t = 0);
double quantile(double arg);


void readDataFile(QString filename, double *** outData, int ns, int * NumOfSlices, int fftLength);
void readDataFile(QString filename, double *** outData, int ns, int * NumOfSlices);
void readSpectraFile(QString filename, double ** outData, int ns, int spLength);
void readPaFile(QString paFile, double *** matrix, int NetLength, int NumOfClasses, int * NumberOfVectors, char *** FileName);
void readICAMatrix(QDir * dir, double *** matrixA, int ns);

void calcSpectre(double ** inData, double ** dataFFT, int ns, int fftLength, int Eyes, int NumOfSmooth);


double distance(double * vec1, double * vec2, int dim);
void matrixProduct(double ** A, double ** B, double *** out, int dimH, int dimL);  //matrix product = A(H*H) * B(H*L)




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
