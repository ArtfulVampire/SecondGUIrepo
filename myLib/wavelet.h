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

const double pi_sqrt = std::sqrt(M_PI);
const int range = 1024;			/// smoothnest of the picture
double const morletFall = 9.;	/// coef in matlab = mF^2 / (2 * pi^2);
extern const int timeStep;			/// time bins

/// wavelets
double morletCosNew(double const freq1,
					const double timeShift,
					const double time);
double morletSinNew(double const freq1,
					const double timeShift,
					const double time);

std::vector<matrix> myCWT(const matrix & fileData, const std::vector<double> & freqs);
matrix myCWT(const std::valarray<double> & inSignal, const std::vector<double> & freqs);

/// drawWavelet
const double defV = 1.;
const std::vector<double> colDots = {1/9., 3.25/9., 5.5/9., 7.75/9.};
double red(int range, double j, double V = defV, double S = 1.0);
double green(int range, double j, double V = defV, double S = 1.0);
double blue(int range, double j, double V = defV, double S = 1.0);
QColor hueJet(int range, double j);


QPixmap drawWavelet(const matrix &inData);


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
