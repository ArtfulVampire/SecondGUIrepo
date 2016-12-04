#ifndef STATISTICS_H
#define STATISTICS_H

#include <valarray>
#include <QString>
#include "smallLib.h"
#include "matrix.h"


namespace myLib
{


void kernelEst(const std::valarray<double> & arr, QString picPath);
void kernelEst(QString filePath, QString picPath);

template <typename signalType = std::valarray<double>>
void histogram(const signalType & arr,
			   int numSteps,
			   const QString & picPath = QString(),
			   std::pair<double, double> xMinMax = {},
			   const QString & color = "drakgray",
			   int valueMax = 0);


double quantile(double arg);

double rankit(int i, int length, double k = 0.375);
//bool gaussApproval(double * arr, int length); // not finished?
//bool gaussApproval(QString filePath); // not finished?
//bool gaussApproval2(double * arr, int length); // not finished?

/// Mann-Whitney
int MannWhitney(const std::valarray<double> & arr1,
				 const std::valarray<double> & arr2,
				 const double p = 0.05);
void countMannWhitney(trivector<int> & outMW,
					  const QString & spectraPath,
					  matrix * averageSpectraOut = nullptr,
					  matrix * distancesOut = nullptr);
void MannWhitneyFromMakepa(const QString & spectraDir,
						   const QString & outPicPath);



template <typename Typ>
double mean(const Typ & arr, int length, int shift = 0);

/// needed for fractal dimension
template <typename Typ>
double covariance (const Typ & arr1,
				   const Typ & arr2,
				   int length,
				   int shift = 0,
				   bool fromZero = false);


void drawRCP(const std::valarray<double> & values,
			 const QString & picPath);
void countRCP(QString filename,
			  QString picPath  = QString(),
			  double * outMean = nullptr,
			  double * outSigma = nullptr);

}

#endif // STATISTICS_H
