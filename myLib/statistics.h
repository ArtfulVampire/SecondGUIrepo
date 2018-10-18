#ifndef STATISTICS_H
#define STATISTICS_H

#include <valarray>

#include <other/matrix.h>
#include <myLib/small.h>

#include <QString>
#include <QPixmap>

namespace myLib
{


void kernelEst(const std::valarray<double> & arr, const QString & picPath);
void kernelEst(const QString & filePath, const QString & picPath);
QPixmap kernelEst(const std::valarray<double> & arr);

QPixmap histogram(const std::valarray<double> & arr,
				  int numSteps,
				  std::pair<double, double> xMinMax = {},
				  const QString & color = "darkgray",
				  int valueMax = 0);


double quantile(double alphaPeak);
double normalCumulative(double zVal);

double rankit(int i, int length, double k = 0.375);

#if 0
/// normality tests
bool gaussApproval(double * arr, int length);	/// not finished?
bool gaussApproval(QString filePath);			/// not finished?
bool gaussApproval2(double * arr, int length);	/// not finished?
#endif




/// Mann-Whitney
int MannWhitney(const std::valarray<double> & arr1,
				const std::valarray<double> & arr2,
				const double p);
enum class whichGreater {first, second};
std::pair<double, whichGreater> MannWhitney(const std::valarray<double> & arr1,
											const std::valarray<double> & arr2);

trivector<int> countMannWhitney(const QString & spectraPath,
								matrix * averageSpectraOut,
								matrix * distancesOut);
trivector<double> countMannWhitneyD(const QString & spectraPath);

template <typename Typ>
void writeMannWhitney(const trivector<Typ> & MW,
					  const QString & outPath,
					  const QString & separator);

/// to deprecate
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
			 const QString & picPath); /// what is it?
void countRCP(const QString & filename,
			  const QString & picPath  = QString(),
			  double * outMean = nullptr,
			  double * outSigma = nullptr);

double binomialOneTailed(int num1, int num2, int numAll1, int numAll2 = 0);

int64_t combination(int n, int k);
double combinationLog(int n, int k);
double factorialLog(int n);
double binomialPvalue(int n, int k, double successRate);
int binomialLimitOfSignificance(int n, double successRate, double pval);

}

#endif /// STATISTICS_H
