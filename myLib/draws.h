#ifndef DRAWS_H
#define DRAWS_H

#include <QString>
#include <valarray>
#include <vector>

#include "matrix.h"
#include "coord.h"

namespace myLib
{

// jet
const double defV = 1.;
const std::vector<double> colDots = {1/9., 3.25/9., 5.5/9., 7.75/9.};
double red(const int &range, double j, double V = defV, double S = 1.0);
double green(const int &range, double j, double V = defV, double S = 1.0);
double blue(const int &range, double j, double V = defV, double S = 1.0);
QColor hueJet(const int &range, double j);
// hot-to-cold
double red1(int range, int j);
double green1(int range, int j);
double blue1(int range, int j);

QColor hueOld(int range, double j, int numOfContours = 0, double V = 0.95, double S = 1.0);
QColor grayScale(int range, int j);


enum class ColorScale {jet,
					   htc,
					   gray,
					   pew};
void drawColorScale(QString filename, int range, ColorScale type = ColorScale::jet, bool full = false);

void drawRealisation(const QString & inPath);

QPixmap drawEeg(const matrix & dataD,
				int ns,
				int NumOfSlices,
				int freq = def::freq,
				const QString & picPath = QString(),
				double norm = 1.,
				int blueChan = -1,
				int redChan = -1);

QPixmap drawEeg(const matrix & dataD,
				int ns,
				int startSlice,
				int endSlice,
				int freq = def::freq,
				const QString & picPath = QString(),
				double norm = 1.,
				int blueChan = -1,
				int redChan = -1);

QPixmap drawOneSignal(const std::valarray<double> & signal,
					  int picHeight = 600,
					  QString outPath = QString());


QPixmap drawOneTemplate(const int chanNum = -1,
						const bool channelsFlag = true,
						const QString & savePath = QString(),
						const double leftF = def::leftFreq,
						const double rightF = def::rightFreq);

QPixmap drawOneArray(const QString & templPath,
					 const std::valarray<double> & arr,
					 const QString & outPath =  QString(),
					 const QString & color = "black",
					 const int & lineWidth = 2);

QPixmap drawTemplate(const QString & outPath = QString(),
				  bool channelsFlag = true,
				  int width = 1600,
				  int height = 1600);

void drawArray(const QString & templPath,
			   const matrix & inData,
//               const spectraGraphsNormalization normType = 0, ////// TO DO
			   const QString & color = "black",
			   const double & scaling = 1.,
			   const int & lineWidth = 3);

void drawArray(const QString & templPath,
			   const std::valarray<double> & inData,
//               const spectraGraphsNormalization normType = 0, ////// TO DO
			   const QString & color = "black",
			   const double & scaling = 1.,
			   const int & lineWidth = 3);

void drawArrayWithSigma(const QString & templPath,
						const std::valarray<double> & inData,
						const std::valarray<double> & inSigma,
						double norm = 0.,
						const QString & color = "blue",
						int lineWidth = 3);

//inMatrix supposed to be def::spLength() * 19 size
double drawArrays(const QString & templPath,
				const matrix & inMatrix,
				const bool weightsFlag = false,
				const spectraGraphsNormalization normType = spectraGraphsNormalization::all,
				double norm = 0.,
				const std::vector<QColor> & colors = def::colours,
				const double scaling = 1.,
				const int lineWidth = 3);

QPixmap drawArrays(const QPixmap & templPixmap,
				const matrix & inMatrix,
				const bool weightsFlag = false,
				const spectraGraphsNormalization normType = spectraGraphsNormalization::all,
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

QPixmap drawOneSpectrum(const std::valarray<double> & signal,
						const QString & outPath = QString(),
						double leftFr = def::leftFreq,
						double rightFr = def::rightFreq,
						double srate = def::freq,
						int numOfSmooth = 10,
						const QString & color = "black",
						const int & lineWidth = 2);

void drawMannWitney(const QString & templPath,
					const trivector<int> & inMW,
					const std::vector<QColor> & inColors = def::colours);

void drawMannWitneyInLine(const QString & picPath,
						  const trivector<int> & inMW,
						  const std::vector<QColor> & inColors = def::colours);







/// maps + drawings

QColor mapColor(double minMagn, double maxMagn,
				const matrix & helpMatrix,
				int numX, int numY,
				double partX, double partY,
				bool colour = true);
// old unused

void drawMapSpline(const matrix & matrixA,
				   const int numOfColoumn,
				   const QString & outDir,
				   const QString & outName,
				   const double & maxAbs,
				   const int picSize = 240,
				   const ColorScale colorTheme = ColorScale::jet);

void drawMapsICA(const QString & mapsFilePath,
				 const QString & outDir,
				 const QString & outName,
				 const ColorScale colourTheme = ColorScale::jet,
				 void (*draw1MapFunc)(const matrix &,
									  const int,
									  const QString &,
									  const QString &,
									  const double &,
									  const int,
									  const ColorScale) = &drawMapSpline);

void drawMapsOnSpectra(const QString & inSpectraFilePath,
					   const QString & outSpectraFilePath,
					   const QString & mapsDirPath,
					   const QString & mapsNames);
}

#endif // DRAWS_H
