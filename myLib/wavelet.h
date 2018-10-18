#ifndef WAVELET_H
#define WAVELET_H

#include <other/matrix.h>
#include <other/consts.h>

#if WAVELET_MATLAB
#include <matlab/for_testing/libcwt_r.h>
//#include <libcwt_r.h>
#endif

#include <QString>

namespace wvlt
{
#define WAVELET_FREQ_STEP_TYPE 1 /// 0 for multiplicative 1 for additive

extern const int timeStep;
extern const double freqMax; /// DEFS.getRightFreq()
extern const double freqMin; /// DEFS.getLeftFreq()
extern const double freqStep;
extern const int range;
extern const int numberOfFreqs;
/// wavelets

double morletCos(double const freq1, double const timeShift, double const pot, double const time);
double morletSin(double const freq1, double const timeShift, double const pot, double const time);
double morletCosNew(double const freq1,
					const double timeShift,
					const double time);
double morletSinNew(double const freq1,
					const double timeShift,
					const double time);

void wavelet(const QString & filePath,
			 const QString & picPath,
			 int channelNumber = 0,
			 int ns = 20);

template <typename signalType = std::valarray<double>>
matrix countWavelet(const signalType & inSignal);


const double defV = 1.;
const std::vector<double> colDots = {1/9., 3.25/9., 5.5/9., 7.75/9.};
double red(int range, double j, double V = defV, double S = 1.0);
double green(int range, double j, double V = defV, double S = 1.0);
double blue(int range, double j, double V = defV, double S = 1.0);
QColor hueJet(int range, double j);


void drawWavelet(const QString & picPath,
				 const matrix &inData);

#if WAVELET_MATLAB
/// cwt, imported from matlab
extern bool isInit;
int initMtlb();
int termMtlb();
matrix cwt(const std::valarray<double> & signal, double srate);
void drawWaveletMtlb(const matrix & inData,
					 const QString & picPath = QString());
#endif
}

#endif /// WAVELET_H
