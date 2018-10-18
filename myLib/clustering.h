#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <vector>
#include <valarray>

#include <QString>
#include <QPixmap>
#include <QPainter>



using coordType = std::vector<std::pair<double, double>>;
using mat = std::vector<std::vector<double>>;

namespace clust
{

double countAngle(double initX, double initY);
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
				   const mat & distOld,
				   mat & distNew,
				   std::vector<double> & gradient);

void sammonAddDot(const mat & distOld,
				  mat & distNew, /// change only last coloumn
				  coordType & plainCoords,
				  const std::vector<int> & placedDots);

void countDistNewAdd(mat & distNew, /// change only last coloumn
					 const coordType & crds,
					 const std::vector<int> & placedDots);

void countGradientAddDot(const mat & distOld,
						 const mat & distNew,
						 const coordType & crds,
						 const std::vector<int> & placedDots,
						 std::vector<double> & gradient);

void countInvHessianAddDot(const mat & distOld,
						   const mat & distNew,
						   const coordType & crds,
						   const std::vector<int> & placedDots,
						   mat & invHessian);

double errorSammonAdd(const mat & distOld,
					  const mat & distNew,
					  const std::vector<int> & placedDots);

}

#endif /// CLUSTERING_H
