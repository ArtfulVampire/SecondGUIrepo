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
#include "smallFuncs.h"

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

namespace myLib
{

#define TIME(arg)\
    do{\
        auto t0 = std::chrono::high_resolution_clock::now();\
        arg;\
        auto t1 = std::chrono::high_resolution_clock::now();\
        std::cout << funcName(#arg) \
        << ": time elapsed = "\
        << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count()/1000.\
        << " sec" << std::endl;\
    }while(false)

std::string funcName(std::string in);


// consts
const double pi_min_025 = pow(pi, -0.25);
const double pi_sqrt = sqrt(pi);
const QString slash = QString(QDir::separator());






// small shit
void writeWavFile(const vectType & inData, const QString & outPath);
int len(QString s); // string length for EDF+ annotations
QString rightNumber(const unsigned int input, int N); // prepend zeros
QString fitNumber(const double & input, int N); // append spaces
QString fitString(const QString & input, int N); // append spaces

int findChannel(char ** const labelsArr, QString chanName, int ns = 21); // const fail
QString setFileName(const QString & initNameOrPath); //-> initName_i.ext
QString getPicPath(const QString & dataPath,
                   const QDir * ExpNameDir = def::dir,
                   const int & ns = def::ns);
QString getFileName(const QString & filePath, bool withExtension = true);
bool areEqualFiles(QString path1, QString path2);
void deleteSpaces(const QString & dirPath, const QStringList & filters = QStringList());

QString getExpNameLib(const QString & filePath);
QString getDirPathLib(const QString & filePath);
QString getExt(QString filePath);
QString rerefChannel(const QString & initialName,
                     const QString & targetRef = "Ar",
                     const QString & currentNum = "1",
                     const QString & earsChan = "20",
                     const QString & groundChan = "21",
                     const std::vector<QString> & sign = {"-", "+"});

template <typename Typ, template <typename> class Cont>
std::ostream & operator<< (std::ostream &os, Cont <Typ> toOut); // template!

template <typename Typ, template <typename, typename = std::allocator<Typ>> class Cont>
std::ostream & operator<< (std::ostream &os, Cont <Typ> toOut); // template!

std::ostream & operator<< (std::ostream &os, QString toOut);
std::ostream & operator<< (std::ostream &os, matrix toOut);


char * strToChar(const QString & input);
FILE * fopen(QString filePath, const char *__modes);
char * QStrToCharArr(const QString & input, const int & len = -1);

int typeOfFileName(const QString & fileName);
QString getFileMarker(const QString & fileName);

template <typename Container>
int indexOfMax(const Container & cont);





//colorscales
enum ColorScale {jet = 0,
                 htc = 1,
                 gray = 2,
                 pew = 3};
void drawColorScale(QString filename, int range, ColorScale type = jet, bool full = false);
const std::vector<double> colDots = {1/9., 3.25/9., 5.5/9., 7.75/9.};
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






//wavelets

double morletCos(double const freq1, double const timeShift, double const pot, double const time);
double morletSin(double const freq1, double const timeShift, double const pot, double const time);
double morletCosNew(double const freq1,
                    const double timeShift,
                    const double time);
double morletSinNew(double const freq1,
                    const double timeShift,
                    const double time);

#define WAVELET_FREQ_STEP_TYPE 1 // 0 for multiplicative 1 for additive
namespace wvlt
{
const int timeStep = ceil(0.1 * def::freq);
const double freqMax = 20.; // def::rightFreq
const double freqMin = 5.; // def::leftFreq
const double freqStep = 0.2;
const int range = 256;

#if !WAVELET_FREQ_STEP_TYPE
const int numberOfFreqs = int(log(wvlt::freqMin/wvlt::freqMax) / log(wvlt::freqStep)) + 1;
#else
const int numberOfFreqs = int((wvlt::freqMax - wvlt::freqMin) / wvlt::freqStep) + 1;
#endif

}
void wavelet(QString filePath,
             QString picPath,
             int channelNumber = 0,
             int ns = 20);
matrix waveletDiscrete(const vectType & inData);

void drawWavelet(QString picPath,
                 const matrix &inData);






// "static" functions

const std::vector<int> leest19 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
void eyesProcessingStatic(const std::vector<int> eogChannels = {21, 22}, // 19 eeg, 2 help, form zero
                          const std::vector<int> eegChannels = leest19,
                          const QString & windowsDir = def::dir->absolutePath()
                                                       + slash + "windows",
                          const QString & outFilePath = def::dir->absolutePath()
                                                        + slash + "eyes.txt");

void makeCfgStatic(const QString & FileName = "16sec19ch",
                   const int & NetLength = def::nsWOM() * def::spLength(),
                   const QString & outFileDir = def::dir->absolutePath(),
                   const int & numOfOuts = def::numOfClasses(),
                   const double & lrate = 0.1,
                   const double & error = 0.1,
                   const int & temp = 10);

void makePaStatic(const QString & spectraDir,
                  const int & fold = 2,
                  const double & coeff = 7.,
                  const bool svmFlag = false);










// dataHandlers
void makePaFile(const QString & spectraDir,
                const QStringList & fileNames,
                const double & coeff,
                const QString & outFile,
                const bool svmFlag = false);

void makeFullFileList(const QString & path,
                      QStringList & lst,
                      const QStringList & auxFilters = QStringList());

void makeFileLists(const QString & path,
                   std::vector<QStringList> & lst,
                   const QStringList & auxFilters = QStringList());


void makeMatrixFromFiles(QString spectraDir,
                         QStringList fileNames,
                         double coeff,
                         double *** outMatrix);
void cleanDir(QString dirPath, QString nameFilter = QString(), bool ext = true);

lineType signalFromFile(QString filePath,
                   int channelNumber,
                   int ns = def::ns); // unused

void readPlainData(const QString & inPath,
                   matrix & data,
                   int & numOfSlices,
                   const int & start = 0);

void writePlainData(const QString outPath,
                    const matrix & data,
                    int numOfSlices = 0,
                    int start = 0);

void readMatrixFile(const QString & filePath,
                    matrix & outData);

void writeMatrixFile(const QString & filePath,
                     const matrix & outData,
                     const QString & rowsString = "NumOfRows",
                     const QString & colsString = "NumOfCols");


template <typename signalType>
void readFileInLine(const QString & filePath,
                    signalType & result);


template <typename signalType>
void writeFileInLine(const QString & filePath,
                     const signalType & outData);

void readPaFile(const QString & paFile,
                matrix & dataMatrix,
                std::vector<int> types,
                std::vector<QString> & FileName,
                std::vector<double> & classCount);







// drawings

void drawRealisation(const QString & inPath);

QPixmap drawEeg(const matrix & dataD,
                int ns,
                int NumOfSlices,
                int freq = def::freq,
                const QString & picPath = QString(),
                double norm = 1.,
                int blueChan = -1,
                int redChan = -1);

QPixmap drawEeg( const matrix & dataD,
                 int ns,
                 int startSlice,
                 int endSlice,
                 int freq = def::freq,
                 const QString & picPath = QString(),
                 double norm = 1.,
                 int blueChan = -1,
                 int redChan = -1);

void drawOneArray(const lineType & array, QString outPath);

void drawTemplate(const QString & outPath,
                  bool channelsFlag = true,
                  int width = 1600,
                  int height = 1600);


void drawArray(const QString & templPath,
               const matrix & inData,
//               const spectraGraphsNormalization normType = 0, ////// TODO
               const QString & color = "black",
               const double & scaling = 1.,
               const int & lineWidth = 3);
void drawArray(const QString & templPath,
               const lineType & inData,
//               const spectraGraphsNormalization normType = 0, ////// TODO
               const QString & color = "black",
               const double & scaling = 1.,
               const int & lineWidth = 3);

//inMatrix supposed to be def::spLength() * 19 size
double drawArrays(const QString & templPath,
                const matrix & inMatrix,
                const bool weightsFlag = false,
                const spectraGraphsNormalization normType = all,
                double norm = 0.,
                const std::vector<QColor> & colors = def::colours,
                const double scaling = 1.,
                const int lineWidth = 3);


void drawArraysInLine(const QString & picPath,
                      const matrix & inMatrix,
                      const std::vector<QColor> & colors = def::colours,
                      const double scaling = 1.,
                      const int lineWidth = 3);

void drawCutOneChannel(const QString & inSpectraPath,
                       const int numChan);

int MannWhitney(double * arr1, int len1, double * arr2, int len2, double p = 0.05);
template <typename signalType = lineType>
int MannWhitney(const signalType & arr1,
                 const signalType & arr2,
                 const double p = 0.05);
void countMannWhitney(trivector<int> & outMW,
                      const QString & spectraPath = def::dir->absolutePath()
                                                    + slash + "SpectraSmooth",
                      matrix * averageSpectraOut = nullptr,
                      matrix * distancesOut = nullptr);

void drawMannWitney(const QString & templPath,
                    const trivector<int> & inMW,
                    const std::vector<QColor> & inColors = def::colours);

void drawMannWitneyInLine(const QString & picPath,
                          const trivector<int> & inMW,
                          const std::vector<QColor> & inColors = def::colours);













// maps drawings
void splineCoeffCount(double * const inX, double * const inY, int dim, double ** outA, double ** outB); //[inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
void splineCoeffCount(const vectType & inX,
                      const vectType & inY,
                      int dim,
                      vectType & outA,
                      vectType & outB); //[inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
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

void drawMapsICA(const QString & mapsFilePath = def::dir->absolutePath()
                                                + slash + "Help"
                                                + slash + "ica"
                                                + slash + def::ExpName + "_maps.txt",
                 const QString & outDir = def::dir->absolutePath()
                                          + slash + "Help"
                                          + slash + "maps",
                 const QString & outName = def::ExpName,
                 const ColorScale colourTheme = jet,
                 void (*draw1MapFunc)(const matrix &,
                                      const int,
                                      const QString &,
                                      const QString &,
                                      const double &,
                                      const int,
                                      const ColorScale) = &drawMapSpline);

void drawMapsOnSpectra(const QString & inSpectraFilePath = def::dir->absolutePath()
                                                           + slash + "Help"
                                                           + slash + def::ExpName + "_all.jpg",
                       const QString & outSpectraFilePath = def::dir->absolutePath()
                                                            + slash + "Help"
                                                            + slash + def::ExpName + "_all_wm.jpg",
                       const QString & mapsDirPath = def::dir->absolutePath()
                                                     + slash + "Help"
                                                     + slash + "maps",
                       const QString & mapsNames = def::ExpName);






// signal processing
double enthropy(const double *arr, const int N, const int numOfRanges = 30); // not finished?
void four1(double * dataF, int nn, int isign);

template <typename signalType = lineType>
void four1(signalType & inComplexData, int nn = def::fftLength, int isign = 1);

template <typename signalType = lineType>
signalType four2(const signalType & inRealData, int nn = def::fftLength, int isign = 1);

void four3(std::valarray<std::complex<double>> & inputArray);
void four3(std::valarray<double> & inputRealArray);


template <typename signalType = lineType>
matrix countWavelet(const signalType & inSignal);

template <typename signalType = lineType>
double fractalDimension(const signalType &arr,
                        const QString &picPath = QString());


template <typename signalType = lineType, typename retType = lineType>
retType hilbert(const signalType & arr,
                double lowFreq = def::leftFreq,
                double highFreq = def::rightFreq,
                QString picPath  = QString());

template <typename signalType = lineType, typename retType = lineType>
retType hilbertPieces(const signalType & arr,
                   int inLength,
                   double sampleFreq,
                   double lowFreq,
                   double highFreq,
                   QString picPath = QString());

template <typename signalType = lineType, typename retType = lineType>
retType bayesCount(const signalType & dataIn, int numOfIntervals);

template <typename signalType = lineType>
void kernelEst(const signalType & arr, QString picPath);

template <typename signalType = lineType>
void histogram(const signalType & arr, int numSteps, QString picPath);



template <typename signalType = lineType, typename retType = lineType>
retType spectre(const signalType & data);

void spectre(const double * data,
             const int &length,
             double *& spectr);

template <typename signalType = lineType, typename retType = lineType>
retType smoothSpectre(const signalType & inSpectre, const int numOfSmooth);


template <typename signalType = lineType>
void calcSpectre(const signalType & inSignal,
                 signalType & outSpectre,
                 const int & fftLength = def::fftLength,
                 const int & NumOfSmooth = 0,
                 const int & Eyes = 0,
                 const double & powArg = 1.);


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


double independence(double * const signal1, double * const signal2, int length);
double countAngle(double initX, double initY);




/// what is RCP ???
void drawRCP(const lineType & values,
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





//products for ICA
void product1(const matrix & arr,
              const int length,
              const int ns,
              const lineType & vect,
              lineType & outVector);

void product2(const matrix & arr,
              const int length,
              const int ns,
              const lineType & vect,
              lineType & outVector);

void product3(const matrix & inMat,
              const int ns,
              const int currNum,
              lineType & outVector);

void randomizeValar(lineType & valar);

void countVectorW(matrix & vectorW,
                  const matrix & dataICA,
                  const int ns,
                  const int dataLen,
                  const double vectorWTreshold);

void dealWithEyes(matrix & inData,
                  const int dimension);
void ica(const matrix & initialData,
         matrix & matrixA, double eigenValuesTreshold, const double vectorWTreshold);

void svd(const matrix & initialData,
         matrix & eigenVectors,
         lineType & eigenValues,
         const int dimension,
         const double & threshold = 1e-9,
         int eigenVecNum = -1);




template <typename Typ>
void calcRawFFT(const Typ & inData, mat & dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth);

template <typename T>
double distance(const std::vector<T> &vec1, const std::vector<T> &vec2, const int &dim);

double distance(double * vec1, double * vec2, const int & dim);
double distance(double const x1, double const y1, double const x2, double const y2);

double distanceMah(double * & vect, double ** & covMatrixInv, double *&groupMean, int dimension);
double distanceMah(double * & vect, double ** & group, int dimension, int number);
double distanceMah(double ** &group1, double ** &group2, int dimension, int number1, int number2);
void matrixMahCount(double ** &matrix, int number, int dimension, double **&outMat, double *&meanVect);








/// old matrices
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

#endif // LIBRARY_H
