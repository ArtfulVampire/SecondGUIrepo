#ifndef DRAWS_H
#define DRAWS_H


#include <valarray>
#include <vector>

#include <other/matrix.h>
#include <other/defs.h>
#include <other/consts.h>

#include <QPixmap>
#include <QString>

namespace myLib
{

const int BaklushevChans = 19;
/// jet
const double defV = 1.;
const std::vector<double> colDots_2 = {1/9., 3.25/9., 5.5/9., 7.75/9.};

double red(int range, int j, double V = defV, double S = 1.0);
double green(int range, int j, double V = defV, double S = 1.0);
double blue(int range, int j, double V = defV, double S = 1.0);
QColor hueJet(int range, int j);

double red1(int range, int j);
double green1(int range, int j);
double blue1(int range, int j);
QColor hueOld(int range, int j);

QColor grayScale(int range, int j);

enum class ColorScale {jet,
					   htc,
					   gray,
					   pew};



void drawColorScale(const QString & filename, int range, ColorScale type = ColorScale::jet, bool full = false);


QPixmap drawEeg(const matrix & dataD,
				int ns,
				int NumOfSlices,
				int freq = DEFS.getFreq(),
				const QString & picPath = QString(),
				double norm = 1.,
				int blueChan = -1,
				int redChan = -1);

QPixmap drawEeg(const matrix & dataD,
				int ns,
				int startSlice,
				int endSlice,
				int freq,
				const QString & picPath = QString(),
				double norm = 1.,
				int blueChan = -1,
				int redChan = -1);

QPixmap drawOneSignal(const std::valarray<double> & signal,
					  int picHeight = 600,
					  const QString & outPath = QString());


QPixmap drawOneTemplate(const int chanNum = -1,
						const bool channelsFlag = true,
						const QString & savePath = QString(),
						const double leftF = DEFS.getLeftFreq(),
						const double rightF = DEFS.getRightFreq());

QPixmap drawOneArray(const QString & templPath,
					 const std::valarray<double> & arr,
					 const QString & outPath =  QString(),
					 const QString & color = "black",
					 int lineWidth = 2);

QPixmap drawTemplate(const QString & outPath = QString(),
				  bool channelsFlag = true,
				  int width = 1600,
				  int height = 1600);

///               const spectraNorming normType = 0, ////// TO DO
void drawArray(const QString & templPath,
			   const matrix & inData,
			   const QString & color = "black",
			   double scaling = 1.,
			   int lineWidth = 3);


///               const spectraNorming normType = 0, ////// TO DO
void drawArray(const QString & templPath,
			   const std::valarray<double> & inData,
			   const QString & color = "black",
			   double scaling = 1.,
			   int lineWidth = 3);

void drawArrayWithSigma(const QString & templPath,
						const std::valarray<double> & inData,
						const std::valarray<double> & inSigma,
						double norm = 0.,
						const QString & color = "blue",
						int lineWidth = 3);

/// inMatrix supposed to be spLength * 19 size
double drawArrays(const QString & templPath,
				const matrix & inMatrix,
				const bool weightsFlag = false,
				const spectraNorming normType = spectraNorming::all,
				double norm = 0.,
				const std::vector<QColor> & colors = def::colours,
				const double scaling = 1.,
				const int lineWidth = 3);

QPixmap drawArrays(const QPixmap & templPixmap,
				const matrix & inMatrix,
				const bool weightsFlag = false,
				const spectraNorming normType = spectraNorming::all,
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
						double leftFr = DEFS.getLeftFreq(),
						double rightFr = DEFS.getRightFreq(),
						double srate = DEFS.getFreq(),
						int numOfSmooth = 10,
						const QString & color = "black",
						int lineWidth = 2);

void drawMannWitney(const QString & templPath,
					const trivector<int> & inMW,
					const std::vector<QColor> & inColors = def::colours);

void drawMannWitneyInLine(const QString & picPath,
						  const trivector<int> & inMW,
						  const std::vector<QColor> & inColors = def::colours);







/// maps + drawings
void drawMapSpline(const matrix & matrixA,
				   const int numOfColoumn,
				   const QString & outDir,
				   const QString & outName,
				   double maxAbs,
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
									  double,
									  const int,
									  const ColorScale) = &drawMapSpline);

void drawMapsOnSpectra(const QString & inSpectraFilePath,
					   const QString & outSpectraFilePath,
					   const QString & mapsDirPath,
					   const QString & mapsNames);
}

#endif /// DRAWS_H
