#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QtGui>
#include <QtSvg>
#include "coord.h"
#include "matrix.h"
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
#include <ctime>


#ifdef _OPENMP
#include <omp.h>
#endif
/////// *** -> **&

using namespace std;
//using namespace itpp;
//using namespace mlpack;

const double pi = 3.141592653589;
const double pi_min_025 = pow(pi, -0.25);
const double pi_sqrt = sqrt(pi);

typedef std::vector<std::vector<double>> mat;
typedef std::vector<double> vec;

void writeWavFile(const vec & inData, const QString & outPath);

//void QDoubleSpinBox::mySlot(double val)
int len(QString s); // string length for EDF+ annotations
QString rightNumber(const unsigned int input, int N); // prepend zeros
QString fitNumber(const double & input, int N); // append spaces
QString fitString(const QString & input, int N); // append spaces

int findChannel(char ** const labelsArr, QString chanName, int ns = 21); // const fail
QString setFileName(const QString & initNameOrPath); //-> initName_i.ext
QString getPicPath(const QString & dataPath, QDir *ExpNameDir, int ns);
QString getFileName(QString filePath, bool withExtension = true);
bool areEqualFiles(QString path1, QString path2);

QString getExpNameLib(const QString filePath);
QString getDirPathLib(const QString filePath);
QString getExt(QString filePath);
QString slash();
ostream & operator << (ostream &os, QString toOut);
ostream & operator << (ostream &os, vector < vector < double > > toOut);
ostream & operator << (ostream &os, QList<int> toOut);
ostream & operator << (ostream &os, matrix toOut);
template <typename T>
ostream & operator << (ostream &os, vector<T> toOut); // template!
//vector< vector<double> > operator=(const vector< vector<double> > & other);
char * strToChar(const QString & input);
FILE *fopen(QString filePath, const char *__modes);
char * QStrToCharArr(const QString & input, const int &len = -1);
int typeOfFileName(QString fileName);

vec vectorFromMatrix(double ** inMat,
                     int inNs = def::nsWOM(),
                     int spL = def::spLength);


//wavelets
enum ColorScale {jet = 0,
           htc = 1,
           gray = 2,
           pew = 3};
void drawColorScale(QString filename, int range, ColorScale type = jet, bool full = false);

const vector <double> colDots = {1/9., 3.25/9., 5.5/9., 7.75/9.};
// jet
const double defV = 1.;
double red(const int &range, double j, double V = defV, double S = 1.0);
double green(const int &range, double j, double V = defV, double S = 1.0);
double blue(const int &range, double j, double V = defV, double S = 1.0);
// hot-to-cold
double red1(int range, int j);
double green1(int range, int j);
double blue1(int range, int j);

QColor hueOld(int range, double j, int numOfContours = 0, double V = 0.95, double S = 1.0);
QColor hueJet(const int &range, double j);
QColor grayScale(int range, int j);

double morletCos(double const freq1, double const timeShift, double const pot, double const time);
double morletSin(double const freq1, double const timeShift, double const pot, double const time);
double morletCosNew(double const freq1,
                    const double timeShift,
                    const double time);
double morletSinNew(double const freq1,
                    const double timeShift,
                    const double time);

//void wavelet(QString out, FILE * file, int ns = 19, int channelNumber = 0, double freqMax = 20., double freqMin = 5., double freqStep = 0.99, double pot = 32.);


#define WAVELET_FREQ_STEP_TYPE 1 // 0 for multiplicative 1 for additive
namespace wvlt
{
const int timeStep = ceil(0.1 * def::freq);
const double freqMax = 20.; // def::rightFreq
const double freqMin = 5.; // def::leftFreq
const double freqStep = 0.2;
const int range = 256;

#if WAVELET_FREQ_STEP_TYPE==0
const int numberOfFreqs = int(log(wvlt::freqMin/wvlt::freqMax) / log(wvlt::freqStep)) + 1;
#else
const int numberOfFreqs = int((wvlt::freqMax - wvlt::freqMin) / wvlt::freqStep) + 1;
#endif

}
void wavelet(QString filePath,
             QString picPath,
             int channelNumber = 0,
             int ns = 20);
matrix waveletDiscrete(const vec & inData);

vec signalFromFile(QString filePath,
                   int channelNumber,
                   int ns = def::ns);
matrix countWavelet(vec inSignal);
void drawWavelet(QString picPath,
                 const matrix &inData);

//void waveletPhase(QString out, FILE * file, int ns, int channelNumber1, int channelNumber2, double freqMax, double freqMin, double freqStep, double pot);

//signal processing
double fractalDimension(const vec &arr, const QString &picPath = QString());
double enthropy(const double *arr, const int N, const int numOfRanges = 30); // not finished?
void four1(double * dataF, int nn, int isign);
void four1(vec &dataF, int nn, int isign);

vec hilbert(const vec & arr,
            double lowFreq = def::leftFreq,
            double highFreq = def::rightFreq,
            QString picPath  = QString());

vec hilbertPieces(const vec & arr,
                   int inLength,
                   double sampleFreq,
                   double lowFreq,
                   double highFreq,
                   QString picPath = QString());

vec bayesCount(const vec & dataIn, int numOfIntervals);
void kernelEst(const vec & arr, QString picPath);
void kernelEst(QString filePath, QString picPath);

void histogram(const vec & arr, int numSteps, QString picPath);

bool gaussApproval(double * arr, int length); // not finished?
bool gaussApproval(QString filePath); // not finished?
bool gaussApproval2(double * arr, int length); // not finished?
double vectorLength(double * arr, int len);
double quantile(double arg);

template <typename Typ>
double mean(const Typ &arr, int length, int shift = 0);
template <typename Typ>
double variance(const Typ &arr, int length, int shift = 0, bool fromZero = false);
template <typename Typ>
double sigma(const Typ &arr, int length, int shift = 0, bool fromZero = false);

template <typename Typ>
double correlation(const Typ &arr1, const Typ &arr2, int length, int shift = 0, bool fromZero = false);
template <typename Typ>
double covariance (const Typ &arr1, const Typ &arr2, int length, int shift = 0, bool fromZero = false);

double skewness(double *arr, int length);
double kurtosis(double *arr, int length);
double rankit(int i, int length, double k = 0.375);
double maxValue(double * arr, int length);
double minValue(double * arr, int length);

void splitZeros(mat & inData,
                const int &length,
                int * outLength,
                const QString & logFile = QString(),
                const QString & dataName = def::ExpName);

template <typename Typ>
void splitZerosEdges(Typ & dataIn, const int & ns, const int & length, int * outLength);
template <typename Typ>
void zeroData(Typ & inData, const int & leftLimit, const int & rightLimit);


double independence(double * const signal1, double * const signal2, int length);
double countAngle(double initX, double initY);

bool MannWhitney(double * arr1, int len1, double * arr2, int len2, double p = 0.05);
bool MannWhitney(vec arr1,
                 vec arr2,
                 double p = 0.05);

void makePaFile(QString spectraDir, QStringList fileNames, double coeff, QString outFile);
void makeMatrixFromFiles(QString spectraDir, QStringList fileNames, double coeff, double *** outMatrix);
void cleanDir(QString dirPath, QString nameFilter = QString(), bool ext = true);

void drawRCP(const vec & values,
             const QString & picPath = "/media/Files/Data/AA/rcp.png");
void countRCP(QString filename,
              QString picPath  = QString(),
              double *outMean = nullptr,
              double *outSigma = nullptr);

void svd(const mat &inData,
         matrix &eigenVectors,
         vec &eigenValues,
         const double &threshold = 1e-9);

void makeCfgStatic(QString outFileDir = def::dir->absolutePath(),
                   int NetLength = def::nsWOM() * def::spLength,
                   QString FileName = "16sec19ch",
                   int numOfOuts = 3,
                   double lrate = 0.1,
                   double error = 0.1,
                   int temp = 10);

template <typename Typ>
void readPlainData(QString inPath,
                   Typ & data,
                   int ns,
                   int & numOfSlices,
                   int start = 0);

template <typename Typ>
void writePlainData(QString outPath,
                    const Typ &data,
                    int ns,
                    int numOfSlices,
                    int start = 0);

template <typename Typ>
QPixmap drawEeg(Typ dataD,
                int ns,
                int NumOfSlices,
                int freq = def::freq,
                const QString & picPath = QString(),
                double norm = 1.,
                int blueChan = -1,
                int redChan = -1);

template <typename Typ>
QPixmap drawEeg( Typ dataD,
                 int ns,
                 int startSlice,
                 int endSlice,
                 int freq = def::freq,
                 const QString & picPath = QString(),
                 double norm = 1.,
                 int blueChan = -1,
                 int redChan = -1);


void drawTemplate(const QString & outPath,
                  int width = 1600,
                  int height = 1600);

void drawArray(const QString & templPath,
               const vec & inData,
               QString color = "black",
               double scaling = 1.,
               int lineWidth = 2);

//inMatrix supposed to be def::spLength * 19 size
template <typename Typ>
void drawArrays(const QString & templPath,
                const Typ & inMatrix,
                QStringList colors = def::colours,
                double scaling = 1.,
                int lineWidth = 3);

void drawArray(double * array, int length, QString outPath);

template <typename Typ>
void readSpectraFile(QString filePath,
                     Typ & outData,
                     int inNs = def::nsWOM(),
                     int spL = def::spLength);

template <typename Typ>
void writeSpectraFile(QString filePath,
                      const Typ & outData,
                      int inNs = def::nsWOM(),
                      int spL = def::spLength);
void readFileInLine(QString filePath,
                    vec & outData);
void writeFileInLine(QString filePath,
                     const vec & outData);

void readPaFile(QString paFile,
                double *** matrix,
                int NetLength,
                int NumOfClasses,
                int * NumberOfVectors,
                char *** FileName,
                double **classCount);

template <typename Typ>
bool readICAMatrix(const QString & path, Typ &matrixA);
template <typename Typ>
void writeICAMatrix(const QString & path, Typ &matrixA);




void splineCoeffCount(double * const inX, double * const inY, int dim, double ** outA, double ** outB); //[inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
void splineCoeffCount(const vec & inX,
                      const vec & inY,
                      int dim,
                      vec & outA,
                      vec & outB); //[inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
double splineOutput(double * const inX, double * const inY, int dim, double * A, double *B, double probeX);

QColor mapColor(double minMagn, double maxMagn, double ** helpMatrix, int numX, int numY, double partX, double partY, bool colour = true);
// old unused
void drawMap      (double ** &matrixA,
                   double maxAbs,
                   QString outDir,
                   QString outName,
                   int num,
                   int size = 240,
                   bool colourFlag = true);

void drawMapSpline(const matrix & matrixA,
                   const int numOfColoumn,
                   const QString & outDir,
                   const QString & outName,
                   const double & maxAbs,
                   const int picSize = 240,
                   const ColorScale colorTheme = jet);

void drawMapsICA(const QString & mapsFilePath,
                 const QString & outDir = QString(def::dir->absolutePath()
                                                  + slash() + "Help"
                                                  + slash() + "Maps"),
                 const QString & outName = def::ExpName,
                 const ColorScale colourTheme = jet,
                 void (*draw1MapFunc)(const matrix &,
                                      const int,
                                      const QString &,
                                      const QString &,
                                      const double &,
                                      const int,
                                      const ColorScale) = &drawMapSpline);

void drawMapsOnSpectra(const QString & spectraFilePath,
                       const QString & outSpectraFilePath,
                       const QString & mapsDirPath,
                       const QString & mapsNames);



void spectre(const double * data,
             const int &length,
             double *& spectr);
vec spectre(const vec & data);
vec smoothSpectre(const vec & inSpectre, const int numOfSmooth);

template <typename inTyp, typename outTyp>
void calcSpectre(const inTyp & inSignal,
                 int length,
                 outTyp & outSpectre,
                 const int & Eyes = 0,
                 int * fftLength = nullptr,
                 const int & NumOfSmooth = 0,
                 const double & powArg = 1.);

//void calcSpectre(double ** &inData, double **& dataFFT, const int &ns, const int &inDataLen, const int &NumOfSmooth = 15, const double &powArg = 1.);
//void calcSpectre(double ** &inData, double **& dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth = 15, const double &powArg = 1.);
//void calcSpectre(double ** &inData, int leng, const int &ns, double **& dataFFT, int * fftLength, const int &NumOfSmooth = 15, const double &powArg = 1.);

template <typename Typ>
void calcSpectre(const Typ & inData,
                 mat & dataFFT,
                 const int &ns,
                 const int &fftLength,
                 const int &Eyes,
                 const int &NumOfSmooth,
                 double const &powArg);

template <typename Typ>
void calcRawFFT(const Typ & inData, mat & dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth);

template <typename T>
double distance(const vector <T> &vec1, const vector <T> &vec2, const int &dim);

double distance(double *vec1, double *vec2, const int &dim);
double distance(double const x1, double const y1, double const x2, double const y2);
double distanceMah(double * &vect, double ** &covMatrixInv, double *&groupMean, int dimension);
double distanceMah(double * &vect, double ** &group, int dimension, int number);
double distanceMah(double ** &group1, double ** &group2, int dimension, int number1, int number2);
void matrixMahCount(double ** &matrix, int number, int dimension, double **&outMat, double *&meanVect);

template <typename Typ1, typename Typ2, typename Typ3>
void matrixProduct(const Typ1 & inMat1, const Typ2 & inMat2, Typ3 & outMat, int dimH, int dimL);  //matrix product: out = A(H*H) * B(H*L)

template <typename Typ1, typename Typ2, typename Typ3>
void matrixProduct(const Typ1 &inMat1, const Typ2 &inMat2, Typ3 & outMat, const int &numRows1, const int &numCols2, const int &numCols1Rows2);  //matrix product: out = A(K*H) * B(H*L)

void matrixProduct(double * &vect, double ** &mat, double * &outVect, int dimVect, int dimMat); //outVect = vect * mat
void matrixProduct(double ** &mat, double * &vect, double * &outVect, int dimVect, int dimMat); //outVect = mat * vect
void matrixProduct(double * &vect1, double * &vect2, int dim, double &out);
void matrixTranspose(double ** &inMat, const int &numRows, const int &numCols, double ** &outMat);
void matrixTranspose(double ** &inMat, const int &numRowsCols);
void matrixCopy(double ** &inMat, double ** &outMat, const int &dimH, const int &dimL);
void matrixInvert(double ** &inMat, const int &size, double **&outMat);
void matrixInvert(double ** &mat, const int &size);
void matrixInvertGauss(double ** &mat, const int &size);
void matrixInvertGauss(double ** &mat, const int &size, double ** &outMat);
double matrixDet(double ** &matrix, const int &dim);
double matrixDetB(double ** &matrix, const int &dim);
void matrixCofactor(double ** &inMatrix, const int &size, const int &numRows, const int &numCols, double ** &outMatrix);
void matrixSystemSolveGauss(double ** &inMat, double * &inVec, int size, double * &outVec);

double matrixInnerMaxCorrelation(double ** &inMatrix, const int numRows, const int numCols,
                                 double (*corrFunc)(const double * const &arr1, const double * const &arr2, int length, int t, bool fromZero)
                                 = &correlation
        );

double matrixMaxCorrelation(double ** &inMat1, double ** &inMat2, const int &numRows, const int &numCols);
void matrixCorrelations(double ** &inMat1, double ** &inMat2, const int &numRows, const int &numCols, double *&resCorr);

double ** matrixCreate( const int &i, const int &j);
void matrixCreate(double *** matrix, const int &i, const int &j);
void matrixDelete(double *** matrix, const int &i);
void matrixDelete(int *** matrix, const int &i);
void matrixPrint(double ** &mat, const int &i, const int &j);


vector<bool> matiCountByte(const double & marker);
QString matiCountByteStr(const double & marker);
void matiPrintMarker(double const & marker, QString pre  = QString());
void matiFixMarker(double & marker);
int matiCountDecimal(vector<bool> byteMarker);
int matiCountDecimal(QString byteMarker);




// do sammon class
//sammon
void drawSammon(const vector < pair <double, double> > & plainCoords,
                const vector <int> & types,
                const QString & picPath);

void drawShepard(const vector < vector <double> > & distOld,
                 const vector < vector <double> > & distNew,
                 const QString & picPath);

void sammonProj(const vector < vector <double> > & distOld,
                const vector <int> & types,
                const QString & picPath);
double errorSammon(const vector < vector <double> > & distOld,
                   const vector < vector <double> > & distNew);

void moveCoordsGradient(vector < pair<double, double> > & plainCoords,
                        const vector < vector <double> > & distOld,
                        vector<vector<double> > & distNew);
void refreshDistAll(vector < vector <double> > & distNew,
                    const vector <pair <double, double> > & plainCoords);

void refreshDist(vector < vector<double> > & dist,
                 const vector <pair <double, double> > & testCoords,
                 const int input);

void countGradient(const vector <pair <double, double> > & plainCoords,
                   const vector<vector<double> > &distOld,
                   vector<vector<double> > &distNew,
                   vector<double> &gradient);

void sammonAddDot(const vector < vector <double> > & distOld,
                  vector < vector <double> > & distNew, // change only last coloumn
                  vector < pair <double, double> > & plainCoords,
                  const vector<int> & placedDots);

void countDistNewAdd(vector < vector <double> > & distNew, // change only last coloumn
                     const vector<pair<double, double> > &crds,
                     const vector <int> & placedDots);

void countGradientAddDot(const vector < vector <double> > & distOld,
                         const vector < vector <double> > & distNew,
                         const vector <pair <double, double> > & crds,
                         const vector <int> & placedDots,
                         vector <double>  & gradient);

void countInvHessianAddDot(const vector < vector <double> > & distOld,
                           const vector < vector <double> > & distNew,
                           const vector <pair <double, double> > & crds,
                           const vector <int> & placedDots,
                           vector < vector <double> > & invHessian);

double errorSammonAdd(const vector < vector <double> > & distOld,
                      const vector < vector <double> > & distNew,
                      const vector <int> & placedDots);




inline double doubleRound(const double & in, const int & numSigns)
{
    return int(  ceil(in * pow(10., numSigns) - 0.5)  ) / pow(10., numSigns);
}

inline double doubleRoundFraq(const double & in, const int & denom)
{
    return ceil(in * denom - 0.5) / denom;
}

inline double gaussian(const double & x, const double & sigma = 1.) //N(0,1)
{
    return 1./(sigma * sqrt(2. * pi)) * exp(-x * x / (2. * sigma * sigma) );
}

inline double logistic(const double & x, const double & t = 10.)
{
    return 1. / ( 1. + exp(-x/t) );
}

inline int fftLimit(const double & inFreq, const double & sampleFreq, const int & fftL)
{
    return ceil(inFreq / sampleFreq * fftL - 0.5);
}

inline int fftL(const int & in)
{
    return pow(2., ceil(log2(in)));
}


inline bool matiCountBit(double const & marker, int num)
{
//    return (int(marker) >> num) % 2;
    return (int(marker) / int(pow(2, num))) % 2;
}

#endif // LIBRARY_H
