#ifndef DRW_H
#define DRW_H

#include <valarray>
#include <vector>

#include <other/matrix.h>

#include <QColor>
#include <QString>
#include <QPixmap>
#include <QPainter>


namespace myLib {

namespace drw {

/// consts
const int templateWidth = 1600;
const int templateHeight = 1600;
const int gap = 20; /// for template - from bottom to Hz axis
const int fontSizeChan = 24;
const int fontSizeHz = 12;
const double graphHeight = 250.;
const double graphWidth = 250.;
const int lineWidth = 3; /// for arrays
const int penWidth = 3; /// for signals
const double offsetYinLine = 1 - 0.1;
const int eegPicHeight = 600;
const int mapSize = 240;

/// sec sticks, time num text
const int wholeSecStick = 20;
const int timeTextY = wholeSecStick - 2;
const int fifthSecStick = wholeSecStick * 0.8;

/// Mann-Witney
const double barWidth = 1/2.;
const int barHeight = 5;
const int barHeightStart = 18;
const int barHeightStep = 8;

const std::vector<QColor> defaultColors
{
	QColor("blue"),
			QColor("red"),
			QColor("green"),
			QColor("black"),
			QColor("gray"),
};

/// coordinates
auto c = [](int in) -> double
{
	return in * 6. / 32.;
};
const std::valarray<double> x {c(1), c(3),
			c(0), c(1), c(2), c(3), c(4),
			c(0), c(1), c(2), c(3), c(4),
			c(0), c(1), c(2), c(3), c(4),
			c(1), c(3)};
const std::valarray<double> y {c(0), c(0),
			c(1), c(1), c(1), c(1), c(1),
			c(2), c(2), c(2), c(2), c(2),
			c(3), c(3), c(3), c(3), c(3),
			c(4), c(4)};

/// for drawTemplate - left upper corner of oneTemplate
const std::valarray<double> xx {(myLib::drw::x + 1/32.) * myLib::drw::templateWidth
			- myLib::drw::gap};
const std::valarray<double> yy {(myLib::drw::y + 6/32.) * myLib::drw::templateHeight
			- myLib::drw::graphHeight - myLib::drw::gap};

/// for drawOneArray - start of signal drawing
const std::valarray<double> xc {(myLib::drw::x + 1/32.) * myLib::drw::templateWidth};
const std::valarray<double> yc {(myLib::drw::y + 6/32.) * myLib::drw::templateHeight};




/// small funcs
extern int manualNs;
void setNumOfChans(int in);
int numOfChans(const std::valarray<double> & inData);
int numOfChans(int inDataSize);

/// big funcs
/// ones
QPixmap drawOneTemplate(const QString & chanName,
						const double leftF,
						const double rightF);

QPixmap drawOneArray(const QPixmap & templatePic,
					 const std::valarray<double> & inData,
					 double maxVal = 0.,
					 const QColor & color = "black",
					 bool weightsFlag = false,
					 int lineWidth_ = myLib::drw::lineWidth,
					 double X = myLib::drw::gap,
					 double Y = myLib::drw::graphHeight + myLib::drw::gap); /// + top gap

QPixmap drawOneSignal(const std::valarray<double> & inData,
					  int picHeight = 600);

QPixmap drawOneGraph(const std::valarray<double> & inData,
					 int picHeight = 600,
					 std::pair<double, double> lims = std::pair<double, double>{});

QPixmap drawOneSpectrum(const std::valarray<double> & inSignal,
						double leftFr,
						double rightFr,
						double srate,
						int numOfSmooth,
						const QColor & color = QColor("black"));

/// many
QPixmap drawTemplate(const bool channelsFlag = true,
					 const double leftF = 5.0,
					 const double rightF = 20.0,
					 const int numOfChannels = 19);

/// inData[numOfChans][chanLength]
QPixmap drawArray(const QPixmap & templatePic,
				  const matrix & inData,
				  const QColor & color = QColor("black"),
				  bool weightsFlag = false,
				  double maxVal = 0.,
				  int lineWidth_ = myLib::drw::lineWidth);

QPixmap drawArray(const QPixmap & templatePic,
				  const std::valarray<double> & inData,
				  const QColor & color = QColor("black"),
				  bool weightsFlag = false,
				  double maxVal = 0.,
				  int lineWidth_ = myLib::drw::lineWidth);

QPixmap drawArrays(const QPixmap & templatePic,
				   const matrix & inData,
				   double maxVal = 0.,
				   bool weightsFlag = false,
				   const std::vector<QColor> & colors = myLib::drw::defaultColors,
				   std::vector<int> lineWidths = std::vector<int>());


QPixmap drawArraysInLine(const matrix & inMatrix,
						 const std::vector<QColor> & colors = myLib::drw::defaultColors);

QPixmap drawCutOneChannel(const QPixmap & inSpectraPic,
						  const int numChan);

void cutToChannels(const QPixmap & inSpectraPic,
				   int numOfChans,
				   const QString & outDir,
				   const QString & fileName);
void cutToChannels(const QString & inSpectraPath,
				   int numOfChans,
				   const QString & outDir,
				   const QString & fileName);

void cutCentralChannels(const QString & inPath);

/// for iitp
QPixmap drawArrays(const QPixmap & templatePic, const std::vector<QString> & filesPaths);
extern std::vector<int> zeroChans;
extern std::vector<int> trueChans;
std::vector<QPixmap> drawArraysSameScale(const QPixmap & templatePic, const std::vector<QString> & filesPaths);

/// for Baklushev
QPixmap drawArrayWithSigma(const QPixmap & templatePic,
						   const std::valarray<double> & inData,
						   const std::valarray<double> & inSigma,
						   double maxVal = 0.,
						   const QColor & color = QColor("blue"));

/// Mann-Witney
QPixmap drawMannWitney(const QPixmap & templatePic,
					   const trivector<int> & inMW,
					   const std::vector<QColor> & inColors);
QPixmap drawMannWitneyInLine(const QPixmap & templatePic,
							 const trivector<int> & inMW,
							 const std::vector<QColor> & inColors);


/// EEGs
QPixmap drawRealisation(const QString & inPath, int srate);
QPixmap redrawEegCopy(const QPixmap & prev,
					  int sta,
					  int fin,
					  const matrix & inData,
					  int srate, double Xnorm,
					  const std::vector<std::pair<int, QColor> > & colouredChans = {});
void redrawEeg(QPixmap & prev,
			   int sta,
			   int fin,
			   const matrix & inData,
			   int srate,
			   double Xnorm,
			   const std::vector<std::pair<int, QColor> > & colouredChans = {});
QPixmap drawEeg(const matrix & inData,
				double srate,
				int picHeight,
				const std::vector<std::pair<int, QColor>> & colouredChans = {});
void drawTime(QPainter & paint, int start, int end,  int srate);

/// colorScales
enum class ColorScale {jet, htc, gray, matlab};
const double defV = 1.;
const double defS = 1.;
const std::vector<double> colDots = {1/9., 3.25/9., 5.5/9., 7.75/9.};

double redJet(int range, int j, double V = defV, double S = defS);
double greenJet(int range, int j, double V = defV, double S = defS);
double blueJet(int range, int j, double V = defV, double S = defS);
QColor hueJet(int range, int j);

double redOld(int range, int j);
double greenOld(int range, int j);
double blueOld(int range, int j);
QColor hueOld(int range, int j);

QColor grayScale(int range, int j);

double redMatlab(int range, int j, double V = defV, double S = defS);
double greenMatlab(int range, int j, double V = defV, double S = defS);
double blueMatlab(int range, int j, double V = defV, double S = defS);
QColor hueMatlab(int range, int j);

auto colorFunction(ColorScale in) -> QColor (*)(int, int);

/// ica maps
QPixmap drawOneMap(const std::valarray<double> & inData,
				   double maxAbs,
				   const ColorScale & colorTheme,
				   bool drawScale = false);
void drawMapsICA(const QString & mapsFilePath,
				 const QString & outDir,
				 const QString & outName,
				 const ColorScale colourTheme);
QPixmap drawMapsOnSpectra(const QPixmap & inSpectraPic,
						  const QString & mapsDirPath,
						  const QString & mapsName);

QPixmap drawColorScale(int range, ColorScale type, bool full);

} /// namespace drw

} /// namespace myLib

#endif /// DRW_H
