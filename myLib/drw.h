#ifndef DRW_H
#define DRW_H
#include <matrix.h>
#include <QColor>
#include <QString>
#include <valarray>
#include <vector>
#include <QPixmap>
#include <QPainter>


namespace myLib {

namespace drw {

///consts
const int templateWidth = 1600;
const int templateHeight = 1600;
const int gap = 20; // for template - from bottom to Hz axis
const int fontSizeChan = 24;
const int fontSizeHz = 12;
const double graphHeight = 250.;
const double graphWidth = 250.;
const int lineWidth = 3; // for arrays
const int penWidth = 2; // for signals
// Mann-Witney
const double barWidth = 1/2.;
const int barHeight = 5;
const int barHeightStart = 18;
const int barHeightStep = 8;
const std::vector<QColor> defaultColors{ QColor("blue"),
			QColor("red"),
			QColor("green"),
			QColor("black"),
			QColor("gray")};

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

/// for drawTemplate
const std::valarray<double> xx {(myLib::drw::x + 1/32.) * double(myLib::drw::templateWidth)
			- myLib::drw::gap};
const std::valarray<double> yy {(myLib::drw::y + 6/32.) * double(myLib::drw::templateHeight)
			- myLib::drw::graphHeight - myLib::drw::gap};

/// for drawOneArray
const std::valarray<double> xc {(myLib::drw::x + 1/32.) * double(myLib::drw::templateWidth)};
const std::valarray<double> yc {(myLib::drw::y + 6/32.) * double(myLib::drw::templateHeight)};




/// small funcs
extern int manualNs;
void setNumOfChans(int in);
int numOfChans(const std::valarray<double> & inData);

/// big funcs
/// ones
QPixmap drawOneTemplate(const int chanNum,
						const bool channelsFlag,
						const double leftF,
						const double rightF);

QPixmap drawOneArray(const QPixmap & templatePic,
					 const std::valarray<double> & inData,
					 double maxVal,
					 const QColor & color,
					 bool weightsFlag = false,
					 int lineWidth_ = myLib::drw::lineWidth,
					 double X = myLib::drw::gap,
					 double Y = myLib::drw::graphHeight - myLib::drw::gap);

QPixmap drawOneSignal(const std::valarray<double> & inData,
					  int picHeight = 600);

QPixmap drawOneSpectrum(const std::valarray<double> & inData,
						double leftFr,
						double rightFr,
						double srate,
						int numOfSmooth,
						const QColor & color);
/// many
QPixmap drawTemplate(const bool channelsFlag = true,
					 const double leftF = 5.0,
					 const double rightF = 20.0,
					 const int numOfChannels = 19);

// inData[numOfChans][chanLength]
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


QPixmap drawArrayWithSigma(const QPixmap & templatePic,
						   const std::valarray<double> & inData,
						   const std::valarray<double> & inSigma,
						   double maxVal = 0.,
						   const QColor & color = QColor("blue"));




} // namespace drw

} // namespace myLib

#endif // DRW_H
