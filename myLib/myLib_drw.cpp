#include <myLib/drw.h>

#include <other/defs.h>

#include <other/coords.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <myLib/valar.h>

using namespace myOut;

namespace myLib
{
namespace drw
{
int manualNs = -1;

void setNumOfChans(int in)
{
	myLib::drw::manualNs = in;
}

int numOfChans(const std::valarray<double> & inData)
{
	return numOfChans(inData.size());
}

int numOfChans(int inDataSize)
{
	if(myLib::drw::manualNs > 0) { return myLib::drw::manualNs; }

	for(const int nch : {19, 23, 20, 21})
	{
		if(inDataSize % nch == 0)
		{
			return nch;
		}
	}
	return -1;
}

QPixmap drawOneSignal(const std::valarray<double> & inData,
					  int picHeight)
{
	QPixmap pic(inData.size(), picHeight);
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	const double norm = 0.4 / std::abs(inData).max();
	paint.setPen(QPen(QBrush("black"), myLib::drw::penWidth));
	for(int i = 0; i < pic.width() - 1; ++i)
	{
		paint.drawLine(i,
					   pic.height() * (0.5 - inData[i] * norm),
					   i + 1,
					   pic.height() * (0.5 - inData[i + 1] * norm));
	}
	paint.end();
	return pic;
}

QPixmap drawOneGraph(const std::valarray<double> & inData,
					 int picHeight,
					 std::pair<double, double> lims)
{
	QPixmap pic(inData.size(), picHeight);
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	if(lims == std::pair<double, double>{})
	{
		lims.first = inData.min();
		lims.second = inData.max();
	}

	const double norm =  1. / (lims.second - lims.first);
	paint.setPen(QPen(QBrush("black"), myLib::drw::penWidth));
	for(int i = 0; i < pic.width() - 1; ++i)
	{
		paint.drawLine(i,
					   pic.height() * (1. - (inData[i] - lims.first) * norm),
					   i + 1,
					   pic.height() * (1. - (inData[i + 1] - lims.first) * norm));
	}
	paint.end();
	return pic;
}

QPixmap drawOneTemplate(const QString & chanName,
						const double leftF,
						const double rightF)
{

	QPixmap pic(myLib::drw::graphWidth + 2 * myLib::drw::gap,
				myLib::drw::graphHeight + 2 * myLib::drw::gap); /// second gaps for chanNum and 20 Hz
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	QString helpString;
	const double X = myLib::drw::gap;
	const double Y = pic.height() - myLib::drw::gap;

	/// draw axes
	paint.setPen("black");
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X,
						   Y - myLib::drw::graphHeight + myLib::drw::fontSizeChan + 4)); /// magic const
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X + myLib::drw::graphWidth,
						   Y));

	/// draw Herzes
	const double unit = (rightF - leftF) / myLib::drw::graphWidth;
	auto currFreq = [unit, leftF](int in) -> double { return leftF + in * unit; };
	const int lineLen = 5;

	paint.setFont(QFont("Helvitica", myLib::drw::fontSizeHz));
	for(int k = 0; k < myLib::drw::graphWidth + 1; ++k) /// +1 ?
	{
		const double cF = currFreq(k);
		if( std::abs(cF - std::round(cF)) <= unit / 2. )
		{
			paint.drawLine(QPointF(X + k,
								   Y),
						   QPointF(X + k,
								   Y + lineLen)); /// magic const

			helpString = nm(std::round(cF));

			if(helpString.toInt() % 5 != 0) { continue; }

			if(helpString.toInt() < 10)
			{
				paint.drawText(QPointF(X + k - myLib::drw::fontSizeHz / 3.,
									   pic.height() - 1),
							   helpString);
			}
			else
			{
				paint.drawText(QPointF(X + k - myLib::drw::fontSizeHz / 3. * 2,
									   pic.height() - 1),
							   helpString);
			}
		}
	}
	/// draw channel name
	paint.setFont(QFont("Helvetica", myLib::drw::fontSizeChan, -1, false));
	if(!chanName.isEmpty())
	{
		paint.drawText(QPointF(X - myLib::drw::fontSizeChan * 2. / 3.,
							   Y - myLib::drw::graphHeight + myLib::drw::fontSizeChan),
					   chanName);
	}
	paint.end();
	return pic;
}

QPixmap drawTemplate(const bool channelsFlag,
					 const double leftF,
					 const double rightF,
					 const int numOfChannels)
{
	QPixmap pic(myLib::drw::templateWidth, myLib::drw::templateHeight);
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	for(int chanNum = 0; chanNum < numOfChannels; ++chanNum)
	{
		QString chanName = nm(chanNum + 1);
		if(channelsFlag)	{ chanName.prepend(coords::lbl21[chanNum] + " ("); chanName += ")"; }

		paint.drawPixmap(myLib::drw::xx[chanNum],
						 myLib::drw::yy[chanNum],
						 myLib::drw::drawOneTemplate(chanName, leftF, rightF));
	}
	paint.end();
	return pic;
}

QPixmap drawArray(const QPixmap & templatePic,
				  const matrix & inData,
				  const QColor & color,
				  bool weightsFlag,
				  double maxVal,
				  int lineWidth_)
{
	QPixmap pic = templatePic;
	QPainter paint;

	if(maxVal <= 0)
	{
		maxVal = inData.maxAbsVal();
	}

	for(int chanNum = 0; chanNum < inData.rows(); ++chanNum)  /// exept markers channel
	{

		pic = myLib::drw::drawOneArray(pic,
									   inData[chanNum],
									   maxVal,
									   color,
									   weightsFlag,
									   lineWidth_,
									   myLib::drw::xc[chanNum],
									   myLib::drw::yc[chanNum]);
	}


	/// draw amplitude
	paint.begin(&pic);
	paint.setPen("black");
	paint.setBrush(QBrush("black"));


	maxVal = smLib::doubleRound(maxVal, 1);
	QFont tmpF = QFont("Helvetica", myLib::drw::fontSizeChan);
	paint.setFont(tmpF);
	paint.drawText(QPointF(myLib::drw::templateWidth * (myLib::drw::c(4) + 1/32.) + 5,
						   myLib::drw::templateHeight * (myLib::drw::c(0) + 1/32.)
						   + myLib::drw::graphHeight / 2.),
				   nm(maxVal) + " mcV^2/Hz");
	paint.end();

	return pic;
}

QPixmap drawArray(const QPixmap & templatePic,
				  const std::valarray<double> & inData,
				  const QColor & color,
				  bool weightsFlag,
				  double maxVal,
				  int lineWidth_)
{
	matrix dataMat;
	const int nch = myLib::drw::numOfChans(inData);
	const int len = inData.size();
	dataMat.resize(nch, len / nch, 0.);
	for(int i = 0; i < nch; ++i)
	{
		std::copy(std::begin(inData) + len / nch * i,
				  std::begin(inData) + len / nch * (i + 1),
				  std::begin(dataMat[i]));
	}

	return myLib::drw::drawArray(templatePic,
								 dataMat,
								 color,
								 weightsFlag,
								 maxVal,
								 lineWidth_);
}


QPixmap drawArrayWithSigma(const QPixmap & templatePic,
						   const std::valarray<double> & inData,
						   const std::valarray<double> & inSigma,
						   double maxVal,
						   const QColor & color)
{
	QPixmap pic = templatePic;

	if(maxVal <= 0.)
	{
		maxVal = (inData + inSigma).max();
	}
	const double norm = myLib::drw::graphHeight / maxVal;

	const auto lowLine = inData - inSigma;
	const auto highLine = inData + inSigma;

	int numOfChans = myLib::drw::numOfChans(inData);
	int lineWidth_ = myLib::drw::lineWidth;


	for(const auto & drawLine : std::vector<std::valarray<double>>{inData, lowLine, highLine})
	{
		pic = myLib::drw::drawArray(pic,
									drawLine,
									color,
									false,
									maxVal,
									lineWidth_);
		lineWidth_ = 1; /// for lowLine and highLine
	}


	QPainter paint;
	paint.begin(&pic);
	paint.setPen(QPen(QBrush(color), 1));

	paint.setOpacity(0.5);
	for(int chanNum = 0; chanNum < numOfChans; ++chanNum)
	{
		const double X = myLib::drw::xc[chanNum];
		const double Y = myLib::drw::yc[chanNum];
		const int spL = inData.size() / numOfChans;
		const int offsetX = chanNum * spL;
		const auto drawData = inData[std::slice(offsetX, spL, 1)];

		/// passage from drawOneArray
		if(drawData.size() < myLib::drw::graphWidth)
		{
			const double indexScale = myLib::drw::graphWidth / drawData.size();

			for(uint k = 0; k < drawData.size(); ++k)
			{
				paint.drawLine(QPointF(X + k * indexScale,
									   Y - lowLine[spL * chanNum + k] * norm),
							   QPointF(X + k * indexScale,
									   Y - highLine[spL * chanNum + k] * norm));
			}
		}
		else
		{
			const double indexScale = drawData.size() / myLib::drw::graphWidth;
			for(int k = 0; k < myLib::drw::graphWidth; ++k)
			{
				paint.drawLine(QPointF(X + k,
									   Y - lowLine[spL * chanNum + k * indexScale] * norm),
						QPointF(X + k,
								Y - highLine[spL * chanNum + k * indexScale] * norm));
			}
		}
	}
	paint.setOpacity(1.0);

#if 0
	/// print magnitude on the picture
	paint.setPen("black");
	paint.setFont(QFont("Helvetica", myLib::drw::fontSizeChan));
	paint.drawText(QPointF(myLib::drw::templateWidth * myLib::drw::c(4),
						   myLib::drw::templateHeight * myLib::drw::c(0)
						   + myLib::drw::graphHeight / 2),
				   nm(maxVal) + " mcV^2/Hz");
#endif

	paint.end();
	return pic;
}

QPixmap drawArrays(const QPixmap & templatePic,
				   const matrix & inData,
				   double maxVal,
				   bool weightsFlag,
				   const std::vector<QColor> & colors,
				   std::vector<int> lineWidths)
{
	if(colors.size() < inData.rows())
	{
		std::cout << "myLib::drw::drawArrays: not enough colors" << std::endl;
		return QPixmap();
	}

	if(lineWidths.empty())
	{
		lineWidths = std::vector<int> (inData.rows(), myLib::drw::lineWidth);
	}

	if(maxVal == 0.)
	{
		maxVal = inData.maxAbsVal();
	}

	QPixmap pic = templatePic;
	for(int numArr = 0; numArr < inData.rows(); ++numArr)
	{
		pic = myLib::drw::drawArray(pic,
									inData[numArr],
									colors[numArr],
									weightsFlag,
									maxVal,
									lineWidths[numArr]);

	}

	return pic;
}

QPixmap drawArrays(const QPixmap & templatePic, const std::vector<QString> & filesPaths)
{
	matrix dat(filesPaths.size(), 1);

	for(uint i = 0; i < filesPaths.size(); ++i)
	{
		if(!QFile::exists(filesPaths[i]))
		{
		   return {};
		}
		dat[i] = myLib::readFileInLine(filesPaths[i]);
	}
	return drawArrays(templatePic, dat);
}


std::vector<int> zeroChans;
std::vector<int> trueChans;

std::vector<QPixmap> drawArraysSameScale(const QPixmap & templatePic,
										 const std::vector<QString> & filesPaths)
{
	std::vector<QPixmap> res;

	matrix dat(filesPaths.size(), 1);

	for(uint i = 0; i < filesPaths.size(); ++i)
	{
		if(!QFile::exists(filesPaths[i]))
		{
			std::cout << "drawArraysSameScale: no such file - " <<  filesPaths[i] << std::endl;
			return {}; /// this should never happen
		}
		dat[i] = myLib::readFileInLine(filesPaths[i]);
	}

	/// zero some channels - IITP
	for(std::valarray<double> & row : dat)
	{
		zeroChans = smLib::range<std::vector<int>>(0, 19);
		for(int trueNum : trueChans)
		{
			zeroChans.erase(std::find(std::begin(zeroChans),
									  std::end(zeroChans),
									  trueNum));
		}

		for(int zeroNum : zeroChans)
		{
			int spLength = row.size() / 19; /// 36 for 256 fftLenW, 18 for 128
			std::fill(std::begin(row) + zeroNum * spLength,
					  std::begin(row) + (zeroNum + 1) * spLength,
					  0.);
		}
	}

	/// draw each separate in black
	for(uint i = 0; i < filesPaths.size(); ++i)
	{
		res.push_back(myLib::drw::drawArray(templatePic,
											dat[i],
											QColor("black"),
											false,
											dat.maxAbsVal()));
	}
	return res;
}

QPixmap drawArraysInLine(const matrix & inMatrix,
						 const std::vector<QColor> & colors)
{
	QPixmap pic(inMatrix.cols(), inMatrix.cols() / 3);
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	const double norm = pic.height() * offsetYinLine / inMatrix.maxAbsVal();

	for(int k = 0; k < inMatrix.rows(); ++k)
	{
		paint.setPen(QPen(QBrush(colors[k]), myLib::drw::lineWidth));
		for(int i = 0; i < pic.width() - 1; ++i)
		{
			paint.drawLine(i,
						   pic.height() * (offsetYinLine - inMatrix[k][i] * norm),
						   i + 1,
						   pic.height() * (offsetYinLine - inMatrix[k][i + 1] * norm) );
		}
	}

	const int nch = myLib::drw::numOfChans(inMatrix[0]);

	paint.setPen(QPen(QBrush("black"), 1, Qt::DashLine));
	for(int i = 0; i < nch; ++i)
	{
		paint.drawLine(i * pic.width() / nch,
					   0,
					   i * pic.width() / nch,
					   offsetYinLine * pic.height());
	}
	paint.end();
	return pic;
}

QPixmap drawCutOneChannel(const QPixmap & inSpectraPic,
						  const int numChan)
{
	return inSpectraPic.copy(QRect(myLib::drw::xx[numChan],
								   myLib::drw::yy[numChan],
								   myLib::drw::graphWidth + 2 * myLib::drw::gap,
								   myLib::drw::graphHeight + 2 * myLib::drw::gap));
}


void cutToChannels(const QPixmap & inSpectraPic,
				   int numOfChans,
				   const QString & outDir,
				   const QString & fileName)
{
	for(int i = 0; i < numOfChans; ++i)
	{
		myLib::drw::drawCutOneChannel(inSpectraPic, i).save(
					outDir + "/" + fileName + "_" + nm(i) + ".jpg", nullptr, 100);
	}
}

void cutToChannels(const QString & inSpectraPath,
				   int numOfChans,
				   const QString & outDir,
				   const QString & fileName)
{
	QPixmap pic(inSpectraPath);
	myLib::drw::cutToChannels(pic,
							  numOfChans,
							  outDir,
							  fileName);

}


void cutCentralChannels(const QString & inPath)
{
	const QStringList files = QDir(inPath).entryList({"*.jpg", "*.png"});
	for(const QString & picPath : files)
	{
		QPixmap pic(inPath + "/" + picPath);
		QPixmap cut = pic.copy(QRect(330, 350, 900, 900));
		QString out = inPath + "/" + picPath;
		out.replace(".jpg", "_cut.jpg");
		out.replace(".png", "_cut.png");
		cut.save(out, nullptr, 100);
	}
}


QPixmap drawOneSpectrum(const std::valarray<double> & inSignal,
						double leftFr,
						double rightFr,
						double srate,
						int numOfSmooth,
						const QColor & color)
{
	std::valarray<double> drawArr = myLib::subSpectrumR(
										myLib::smoothSpectre(
											myLib::spectreRtoR(inSignal)
											* myLib::spectreNorm(smLib::fftL(inSignal.size()),
																 inSignal.size(),
																 srate)
											,
											numOfSmooth),
										leftFr,
										rightFr,
										srate);
	return myLib::drw::drawOneArray(myLib::drw::drawOneTemplate("", leftFr, rightFr),
									drawArr,
									drawArr.max(),
									color);
}

QPixmap drawOneArray(const QPixmap & templatePic,
					 const std::valarray<double> & inData,
					 double maxVal,
					 const QColor & color,
					 bool weightsFlag,
					 int lineWidth_,
					 double X,
					 double Y)
{
	QPixmap pic = templatePic;
	QPainter paint;
	paint.begin(&pic);

	if(maxVal <= 0.) { maxVal = std::abs(inData).max(); }
	const double norm = myLib::drw::graphHeight / maxVal / (weightsFlag + 1);


	Y -= weightsFlag * myLib::drw::graphHeight / 2;

	paint.setPen(QPen(QBrush(color), lineWidth_));

	if(inData.size() < myLib::drw::graphWidth)
	{
		const double indexScale = myLib::drw::graphWidth / inData.size();
		for(uint k = 0; k < inData.size() - 1; ++k)
		{
			paint.drawLine(QPointF(X + k * indexScale,
								   Y - inData[k] * norm),
						   QPointF(X + (k+1) * indexScale,
								   Y - inData[k+1] * norm));
		}
	}
	else
	{
		const double indexScale = inData.size() / myLib::drw::graphWidth;
		for(int k = 0; k < myLib::drw::graphWidth - 1; ++k)
		{
			paint.drawLine(QPointF(X + k,
								   Y - inData[k * indexScale] * norm),
					QPointF(X + (k+1),
							Y - inData[(k+1) * indexScale] * norm));
		}
	}



	/// zero line for weights
	if(weightsFlag)
	{
		paint.setPen(QPen(QBrush("black"), 1, Qt::DashLine));
		paint.drawLine(QPointF(X,
							   Y),
					   QPointF(X + myLib::drw::graphWidth,
							   Y));
	}
	else
	{
#if 0
		/// print max amplitude
		QFont tmpF = QFont("Helvetica", 12);
		paint.setFont(tmpF);
		paint.drawText(QPointF(myLib::drw::graphWidth,
							   myLib::drw::gap + myLib::drw::graphHeight / 2),
					   nm(maxVal) + " mcV^2/Hz");
#endif
	}
	paint.end();
	return pic;
}


QPixmap drawMannWitney(const QPixmap & templatePic,
					   const trivector<int> & inMW,
					   const std::vector<QColor> & inColors)
{

	QPixmap pic = templatePic;
	QPainter paint;
	paint.begin(&pic);

	const int numOfClasses = inMW.size();
	const int numOfChannels = numOfChans(inMW[0][0].size());
	const int spLength = inMW[0][0].size() / numOfChannels;
	const double normX = myLib::drw::graphWidth / spLength;

	for(int chanNum = 0; chanNum < numOfChannels; ++chanNum)  /// exept markers channel
	{
		const int offset = chanNum * spLength;

		/// statistic difference bars
		int barCounter = 0;
		for(int h = 0; h < numOfClasses; ++h) /// class1
		{

			for(int l = h + 1; l < numOfClasses; ++l) /// class2
			{
				QColor color1 = inColors[h];
				QColor color2 = inColors[l];

				/// check it is goog with spLength w/o DEFS
				for(int j = offset; j < offset + spLength; ++j)
				{
					if(inMW[h][l - h][j] == 0) { continue; }

					if(inMW[h][l - h][j] == 1) /// if class1 spectre is bigger
					{
						paint.setPen(color1);
						paint.setBrush(QBrush(color1));
					}
					else
					{
						paint.setPen(color2);
						paint.setBrush(QBrush(color2));
					}

					paint.drawRect(myLib::drw::xc[chanNum] + (j % spLength - barWidth) * normX,
								   myLib::drw::yc[chanNum] +
								   barHeightStart + barHeightStep * barCounter,
								   2 * barWidth * normX,
								   barHeight);
				}
				++barCounter; /// height shift of bars
			}
		}
	}

	paint.end();
	return pic;
}

QPixmap drawMannWitneyInLine(const QPixmap & templatePic,
							 const trivector<int> & inMW,
							 const std::vector<QColor> & inColors)
{
	QPixmap pic = templatePic;
	QPainter paint;
	paint.begin(&pic);


	int num = 0; /// barCounter
	for(uint k = 0; k < inMW.size(); ++k)
	{
		for(uint j = k + 1; j < inMW.size(); ++j) /// inMW[0].size()
		{
			const int currTop = pic.height() * (offsetYinLine + (1. - offsetYinLine) * num);
			const int currBot = pic.height() * (offsetYinLine + (1. - offsetYinLine) * (num + 1));

			QColor color1 = inColors[k];
			QColor color2 = inColors[j];

			for(int i = 0; i < pic.width(); ++i)
			{
				if(inMW[k][j - k][i] == 1)
				{
					paint.setPen(color1);
					paint.setBrush(QBrush(color1));
				}
				else
				{
					paint.setPen(color2);
					paint.setBrush(QBrush(color2));
				}
				paint.drawLine(i,
							   currTop,
							   i,
							   currBot);
			}

			++num;
		}
	}
	paint.end();
	return pic;
}

QPixmap redrawEegCopy(const QPixmap & prev,
					  int sta,
					  int fin,
					  const matrix & inData,
					  int srate,
					  double Xnorm,
					  const std::vector<std::pair<int, QColor> > & colouredChans)
{
	QPixmap pic = prev;
	redrawEeg(pic,
			  sta,
			  fin,
			  inData,
			  srate,
			  Xnorm,
			  colouredChans);
	return pic;
}

/// [sta, fin]
void redrawEeg(QPixmap & pic,
			   int sta,
			   int fin,
			   const matrix & inData,
			   int srate,
			   double Xnorm,
			   const std::vector<std::pair<int, QColor> > & colouredChans)
{

#if 01
	/// integer xNorm



	QPainter paint;
	paint.begin(&pic);

	/// white before redraw
	paint.setPen("white");
	paint.setBrush(QBrush("white"));

	 /// magic +-1
	auto rect1 = QRect(QPoint(sta - 1,	0),
					   QPoint(fin,		pic.height()));
	paint.drawRect(rect1);

	for(int chanNum = 0; chanNum < inData.rows(); ++chanNum)
	{
		/// set color
		auto it = std::find_if(std::begin(colouredChans), std::end(colouredChans),
							   [chanNum](const std::pair<int, QColor> & in)
		{
			return in.first == chanNum;
		}
		);

		if(it != std::end(colouredChans))
		{
			paint.setPen(QPen(QBrush((*it).second), myLib::drw::penWidth));
		}
		else
		{

			paint.setPen(QPen(QBrush("black"), myLib::drw::penWidth));
		}

		const double offsetY = (chanNum + 1) * pic.height() / static_cast<double>(inData.rows() + 2);

		/// magic +-2
		for(int currX = rect1.left() - 1; currX <= rect1.right() + 1; ++currX)
		{
			paint.drawLine(currX,
						   /// horzNorm WHAAAAAAAAAAAAT
						   offsetY + inData[chanNum][currX],
					(currX + 1),
					/// horzNorm WHAAAAAAAAAAAAAAT
					offsetY + inData[chanNum][currX + 1]
					);
		}
	}

	/// paint seconds marks - TO DO
	drawTime(paint, sta, fin, srate);
	paint.end();

#else
	/// double xNorm
#define PIECE_DRAW 0
	QPainter paint;
	paint.begin(&pic);

	/// white before redraw
	paint.setPen("white");
	paint.setBrush(QBrush("white"));

	/// horzNorm
	int staNorm = std::floor(sta / Xnorm);
	int finNorm = std::ceil(fin / Xnorm);

	 /// magic +-1
	auto rect1 = QRect(QPoint(staNorm - 1, 0),
					  QPoint(finNorm + 1, pic.height()));
	paint.drawRect(rect1);


#if PIECE_DRAW
	auto rect2 = QRect(QPoint(sta, 0),
					  QPoint(fin, pic.height()));
	QPixmap piece(rect2.size());
	piece.fill("white");
	QPainter pnt;
	pnt.begin(&piece);
#endif

	for(int chanNum = 0; chanNum < inData.rows(); ++chanNum)
	{
		/// set color
		auto it = std::find_if(std::begin(colouredChans), std::end(colouredChans),
							   [chanNum](const std::pair<int, QColor> & in)
		{
			return in.first == chanNum;
		}
		);
		if(it != std::end(colouredChans))
		{

#if PIECE_DRAW
			pnt.setPen(QPen(QBrush((*it).second), myLib::drw::penWidth));
#else
			paint.setPen(QPen(QBrush((*it).second), myLib::drw::penWidth));
#endif
		}
		else
		{
#if PIECE_DRAW
			pnt.setPen(QPen(QBrush("black"), myLib::drw::penWidth));
#else
			paint.setPen(QPen(QBrush("black"), myLib::drw::penWidth));
#endif
		}

		const double offsetY = (chanNum + 1) * pic.height() / static_cast<double>(inData.rows() + 2);

		/// horzNorm
		/// magic +-2
#if PIECE_DRAW
		int x = 0;
		for(int currX = rect2.left(); currX <= rect2.right() ; ++currX, ++x)
		{
			pnt.drawLine(x,
						   /// horzNorm WHAAAAAAAAAAAAT
						   offsetY + inData[chanNum][currX],
					x + 1,
					/// horzNorm WHAAAAAAAAAAAAAAT
					offsetY + inData[chanNum][currX + 1]
					);
		}
#else
		for(int currX = rect1.left(); currX <= rect1.right() + 1; ++currX)
		{
			paint.drawLine(currX,
						   /// horzNorm WHAAAAAAAAAAAAT
						   offsetY + inData[chanNum][currX * Xnorm],
					(currX + 1),
					/// horzNorm WHAAAAAAAAAAAAAAT
					offsetY + inData[chanNum][(currX + 1) * Xnorm]
					);
		}
#endif

	}
#if PIECE_DRAW
	pnt.end();
	paint.drawPixmap(rect1, piece, piece.rect());
#endif

#if PIECE_DRAW
	/// to do markers
#else
	/// paint seconds marks - TO DO

	paint.setPen(QPen(QBrush("black"), myLib::drw::penWidth));
	for(int numSec = staNorm / srate; numSec <= finNorm / srate ; ++numSec)
	{
		paint.drawLine(numSec * srate / Xnorm, pic.height(),
					   numSec * srate / Xnorm, pic.height() - 32 / Xnorm); /// magic const
		paint.drawText(numSec * srate / Xnorm,
					   pic.height() - 35 / Xnorm, /// magic const
					   nm(numSec));

		for(int subSec = 1; subSec < 5; ++subSec) /// const 5
		{
			paint.drawLine((numSec + subSec / 5.) * srate / Xnorm, pic.height(),
						   (numSec + subSec / 5.) * srate / Xnorm, pic.height() - 25); /// magic const
		}
	}
	paint.end();
#endif

#endif /// double xNorm
}

void drawTime(QPainter & pnt, int start, int end, int srate)
{
	pnt.setPen(QPen(QBrush("black"), myLib::drw::penWidth));
	pnt.setFont(QFont("", 10)); /// magic const

	for(int numSec = start / srate; numSec <= end / srate; ++numSec)
	{
		/// whole sec stick
		pnt.drawLine(numSec * srate, pnt.device()->height(),
					   numSec * srate, pnt.device()->height() - myLib::drw::wholeSecStick);

		/// number
		pnt.drawText(numSec * srate,
					   pnt.device()->height() - myLib::drw::timeTextY,  /// magic const
					   nm(numSec));

		/// 0.2 sec sticks
		for(int subSec = 1; subSec < 5; ++subSec) /// const 5
		{
			pnt.drawLine((numSec + subSec / 5.) * srate,
						 pnt.device()->height(),
						 (numSec + subSec / 5.) * srate,
						 pnt.device()->height() - myLib::drw::fifthSecStick);
		}
	}
}

QPixmap drawEeg(const matrix & inData,
				double srate,
				int picHeight,
				const std::vector<std::pair<int, QColor>> & colouredChans)
{
	QPixmap pic = QPixmap(inData.cols(), picHeight);
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	for(int chanNum = 0; chanNum < inData.rows(); ++chanNum)
	{
		auto it = std::find_if(std::begin(colouredChans), std::end(colouredChans),
							   [chanNum](const std::pair<int, QColor> & in)
		{
			return (in.first == chanNum);
		}
		);
		if(it != std::end(colouredChans))
		{
			paint.setPen(QPen(QBrush((*it).second), myLib::drw::penWidth));
		}
		else
		{
			paint.setPen(QPen(QBrush("black"), myLib::drw::penWidth)); /// default color
		}

		const double offsetY = (chanNum + 1) * pic.height() / (inData.rows() + 2.);
		for(int currX = 0; currX < pic.width() - 1; ++currX)
		{
			paint.drawLine(currX,
						   offsetY + inData[chanNum][currX],
						   currX + 1,
						   offsetY + inData[chanNum][currX + 1]);
		}
	}
	/// paint seconds marks
	drawTime(paint, 0, inData.cols(), srate);
	paint.end();
	return pic;
}


/// hot-to-cold, http:/// stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
double redOld(int range, int j)
{
	double part = j / static_cast<double>(range);
	if(0.000 <= part && part <= 0.5)	{ return 0.; }
	if(0.500 < part && part <= 0.800)	{ return (part - 0.5) / (0.8 - 0.5); }
	if(0.800 < part && part <= 1.000)	{ return 1.; }
	return 0.0;
}
double greenOld(int range, int j)
{
	double part = j / static_cast<double>(range);
	if(0.000 <= part && part <= 0.2)	{ return part * 5; }
	if(0.200 < part && part <= 0.800)	{ return 1.; }
	if(0.800 < part && part <= 1.000)	{ return 1. - (part - 0.8) / (1.0 - 0.8); }
	return 0.0;
}
double blueOld(int range, int j)
{
	double part = j / static_cast<double>(range);
	if(0.000 <= part && part <= 0.2)	{ return 1.; }
	if(0.200 < part && part <= 0.500)	{ return 1 - (part - 0.2) / (0.5 - 0.2); }
	if(0.500 < part && part <= 1.000)	{ return 0.; }
	return 0.0;
}
QColor hueOld(int range, int j)
{
	if(j > range)	{ j = range; }
	if(j < 0)		{ j = 0; }

	return QColor(255. * myLib::drw::redOld(range, j),
				  255. * myLib::drw::greenOld(range, j),
				  255. * myLib::drw::blueOld(range, j));
}

double redMatlab(int range, int j, double V, double S)
{
	double part = j / static_cast<double>(range);
	/// matlab
	if(0. <= part && part <= myLib::drw::colDots[0])					{ return V*(1.-S); }
	if(myLib::drw::colDots[0] < part && part <= myLib::drw::colDots[1])	{ return V*(1.-S); }
	if(myLib::drw::colDots[1] < part && part <= myLib::drw::colDots[2])	{ return V*(1.-S) + V*S*(part-myLib::drw::colDots[1]) / (myLib::drw::colDots[2] - myLib::drw::colDots[1]); }
	if(myLib::drw::colDots[2] < part && part <= myLib::drw::colDots[3])	{ return V; }
	if(myLib::drw::colDots[3] < part && part <= 1.)						{ return V - V*S*(part-myLib::drw::colDots[3])/(1 - myLib::drw::colDots[3])/2.; }
	return 0.; /// never happens
}

double greenMatlab(int range, int j, double V, double S)
{
	double part = j / static_cast<double>(range);
	/// matlab
	if(0.0 <= part && part <= myLib::drw::colDots[0])					{ return V*(1.-S); }
	if(myLib::drw::colDots[0] < part && part <= myLib::drw::colDots[1])	{ return V*(1.-S) + V*S*(part-myLib::drw::colDots[0])/(myLib::drw::colDots[1] - myLib::drw::colDots[0]); }
	if(myLib::drw::colDots[1] < part && part <= myLib::drw::colDots[2])	{ return V; }
	if(myLib::drw::colDots[2] < part && part <= myLib::drw::colDots[3])	{ return V - V*S*(part-myLib::drw::colDots[2])/(myLib::drw::colDots[3] - myLib::drw::colDots[2]); }
	if(myLib::drw::colDots[3] < part && part <= 1.)						{ return V*(1.-S); }
	return 0.; /// never happens
}

double blueMatlab(int range, int j, double V, double S)
{
	double part = j / static_cast<double>(range);
	/// matlab
	if(0.0 <= part && part <= myLib::drw::colDots[0])					{ return V -V*S/2. + V*S*(part)/(myLib::drw::colDots[0] - 0.0)/2.; }
	if(myLib::drw::colDots[0] < part && part <= myLib::drw::colDots[1])	{ return V; }
	if(myLib::drw::colDots[1] < part && part <= myLib::drw::colDots[2])	{ return V - V*S*(part-myLib::drw::colDots[1])/(myLib::drw::colDots[2] - myLib::drw::colDots[1]); }
	if(myLib::drw::colDots[2] < part && part <= myLib::drw::colDots[3])	{ return V*(1.-S); }
	if(myLib::drw::colDots[3] < part && part <= 1.)						{ return V*(1.-S); }
	return 0.; /// never happens
}


QColor hueMatlab(int range, int j)
{
	if(j > range)	{ j = range; }
	if(j < 0)		{ j = 0; }
	return QColor(255. * myLib::drw::redMatlab(range, j),
				  255. * myLib::drw::greenMatlab(range, j),
				  255. * myLib::drw::blueMatlab(range, j));
}

double redJet(int range, int j, double V, double S)
{
	double part = j / static_cast<double>(range);

	/// old
	if(0.000 <= part && part <= 0.167)		{ return V*(1.-S); } /// 2. - V*S/2. + V*S*(part)*3.;
	if(0.167 < part && part <= 0.400)		{ return V*(1.-S); }
	if(0.400 < part && part <= 0.500)		{ return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.500 < part && part <= 0.600)		{ return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.600 < part && part <= 0.833)		{ return V; }
	if(0.833 < part && part <= 1.000)		{ return V - V*S*(part-0.833)/(1.000-0.833)/2.; }
	return 0.0;
}

double greenJet(int range, int j, double V, double S)
{
	double part = j / static_cast<double>(range);
	double hlp = 1.0;

	/// old
	if(0.000 <= part && part <= 0.167)		{ return V*(1.-S); }
	if(0.167 < part && part <= 0.400)		{ return V*(1.-S) + V*S*hlp*(part-0.167)/(0.400-0.167); }
	if(0.400 < part && part <= 0.500)		{ return V-V*S*(1.-hlp); }
	if(0.500 < part && part <= 0.600)		{ return V-V*S*(1.-hlp); }
	if(0.600 < part && part <= 0.833)		{ return V-V*S*(1.-hlp) - V*S*hlp*(part-0.600)/(0.833-0.600); }
	if(0.833 < part && part <= 1.000)		{ return V*(1.-S); }
	return 0.0;
}

double blueJet(int range, int j, double V, double S)
{
	double part = j / static_cast<double>(range);
	/// old
	if(0.000 <= part && part <= 0.167)		{ return V -V*S/2. + V*S*(part)/(0.167-0.000)/2.; }
	if(0.167 < part && part <= 0.400)		{ return V; }
	if(0.400 < part && part <= 0.500)		{ return V - V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.500 < part && part <= 0.600)		{ return V - V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.600 < part && part <= 0.833)		{ return V*(1.-S); }
	if(0.833 < part && part <= 1.000)		{ return V*(1.-S); }
	return 0.0;
}
QColor hueJet(int range, int j)
{
	if(j > range)	{ j = range; }
	if(j < 0)		{ j = 0; }
	return QColor(255. * myLib::drw::redJet(range, j),
				  255. * myLib::drw::greenJet(range, j),
				  255. * myLib::drw::blueJet(range, j));
}

QColor grayScale(int range, int j)
{
	if(j > range)	{ j = range; }
	if(j < 0)		{ j = 0; }
	double t = 255. * (1. - j / static_cast<double>(range));
	return QColor(t, t, t);
}

auto colorFunction (ColorScale in) -> QColor (*)(int, int)
{
	switch(in)
	{
	case ColorScale::jet:
	{
		return myLib::drw::hueJet;
	}
	case ColorScale::htc:
	{
		return myLib::drw::hueOld;
	}
	case ColorScale::gray:
	{
		return myLib::drw::grayScale;
	}
	case ColorScale::matlab:
	{
		return myLib::drw::hueMatlab;
	}
//	default: { /* can't get here */ return myLib::drw::grayScale; }
	}
	return myLib::drw::grayScale;
}

QPixmap drawColorScale(int range, ColorScale type, bool full)
{
	QPixmap pic;
	if(full)
	{
		pic = QPixmap(744, 520); /// magic const
	}
	else
	{
		pic = QPixmap(20, 300); /// magic const
	}
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	/// draw the very scale

	QColor (*colorFunc)(int, int) = myLib::drw::colorFunction(type);

	for(int i = 0; i < range; ++i)
	{
		painter.setBrush(QBrush(colorFunc(range, i)));
		painter.setPen(colorFunc(range, i));

		if(full)
		{
			painter.drawRect(i * pic.width() / static_cast<double>(range),
							 0,
							 (i + 1) * pic.width() / static_cast<double>(range),
							 pic.height() * 0.07); /// magic const
		}
		else
		{
			painter.drawRect(0,
							 (1. - (i + 1)/ static_cast<double>(range)) * pic.height(),
							 pic.width(),
							 (1. / static_cast<double>(range)) * pic.height()); /// vertical
		}

	}

	/// draw curves
	if(full)
	{
		for(int i = 0; i < range; ++i)
		{
			switch(type)
			{
			case ColorScale::jet:
			{
				painter.setPen(QPen(QBrush("red"), 2));
				painter.drawLine(i * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * redJet(range, i),
								 (i + 1) * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * redJet(range, i + 1));

				painter.setPen(QPen(QBrush("green"), 2));
				painter.drawLine(i * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * greenJet(range, i),
								 (i + 1) * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * greenJet(range, i + 1));

				painter.setPen(QPen(QBrush("blue"), 2));
				painter.drawLine(i * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * blueJet(range, i),
								 (i + 1) * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * blueJet(range, i + 1));
				break;
			}
			case ColorScale::htc:
			{
				painter.setPen(QPen(QBrush("red"), 2));
				painter.drawLine(i * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * redOld(range, i),
								 (i + 1) * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * redOld(range, i + 1));

				painter.setPen(QPen(QBrush("green"), 2));
				painter.drawLine(i * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * greenOld(range, i),
								 (i + 1) * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * greenOld(range, i + 1));

				painter.setPen(QPen(QBrush("blue"), 2));
				painter.drawLine(i * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * blueOld(range, i),
								 (i + 1) * pic.width() / static_cast<double>(range),
								 pic.height() * 0.95 - (pic.height() * 0.85) * blueOld(range, i + 1));
				break;
			}
			default:
			{
				break;
			}
			}

		}
	}
	painter.end();
	return pic;
}

QPixmap drawOneMap(const std::valarray<double> & inData,
				   double minVal,
				   double maxVal,
				   const ColorScale & colorTheme,
				   bool drawScale,
				   bool notSolve)
{

	/// inData.size() == 19

	if(inData.size() != 19)
	{
		std::cout << "drw::drawOneMap: bad input size, return QPixmap{}" << std::endl;
		return QPixmap{};
	}

	const int colorsWidth = 30;
	const int valsWidth = 40;

	/// whole pic
	QPixmap pic1(myLib::drw::mapSize + (colorsWidth + valsWidth) * static_cast<int>(drawScale),
				 myLib::drw::mapSize);
	QPainter paint1;
	pic1.fill();
	paint1.begin(&pic1);

	/// the map itself
	QPixmap pic2(myLib::drw::mapSize + (colorsWidth + valsWidth) * static_cast<int>(drawScale),
				 myLib::drw::mapSize);
	pic2.fill();
	QPainter paint2;
	paint2.begin(&pic2);

	double val;
	double drawArg;
	int drawRange = 256;

	int dim = 7;
	double scale = (dim-1) / static_cast<double>(mapSize);

	matrix helpMatrix(dim, dim, 0.);


	int currIndex = 0.;
	for(int i = 0; i < dim * dim; ++i)
	{
		const int rest = i % dim;
		const int quot = i / dim;

		if(quot % (dim - 1) == 0|| rest % (dim-1) == 0)  /// set 0 to all edge values
		{
			helpMatrix[quot][rest] = 0.;
		}
		else if(quot == 1
				&& (rest - 1) * (rest - 3) * (rest - 5) == 0) /// ~Fp3, Fpz, Fp4
		{
			helpMatrix[quot][rest] = 0.;
		}
		else if(quot == 5
				&& (rest - 1) * (rest - 3) * (rest - 5) == 0) /// ~O3, Oz, O4
		{
			helpMatrix[quot][rest] = 0.;
		}
		else
		{
			helpMatrix[quot][rest] = inData[currIndex++];
		}
	}

	/// some approximation for square - Fp3, Fpz, Fp, O3, Oz, O4
	auto & a = helpMatrix;
	const double c1 = 1 / 2.;
	const double c2 = 1 / 1.2;
	a[1][1] = (a[1][2] + a[2][1] + a[2][2]) / 3. * c1;
	a[1][3] = (a[1][2] + a[1][4] + a[2][2] + a[2][3] + a[2][4]) / 5. * c2;
	a[1][5] = (a[1][4] + a[2][4] + a[2][5]) / 3. * c1;
	a[5][1] = (a[4][1] + a[4][2] + a[5][2]) / 3. * c1;
	a[5][3] = (a[5][2] + a[4][2] + a[4][3] + a[4][4] + a[4][5]) / 5. * c2;
	a[5][5] = (a[5][4] + a[4][4] + a[4][5]) / 3. * c1;

	/// lambda initialization (if maxAbs == 0)
	if(minVal == 0. && maxVal == 0.)
	{
		minVal = helpMatrix.minVal();
		maxVal = helpMatrix.maxVal();
	}
	const double minMagn = minVal;
	const double maxMagn = maxVal;

	/// splines
	matrix Ah(5, 6);
	matrix Bh(5, 6);
	std::valarray<double> inX(dim);
	std::valarray<double> inY(dim);
	std::valarray<double> inYv(dim);
	std::valarray<double> Av(dim - 1);
	std::valarray<double> Bv(dim - 1);
	std::iota(std::begin(inX), std::end(inX), 0); /// hope, it's constant

	for(int i = 1; i < dim - 1; ++i) /// number of helpMatrix row
	{
		inY = helpMatrix[i];
		myLib::splineCoeffCount(inX, inY, dim, Ah[i - 1], Bh[i - 1]); /// horizontal splines coeffs
	}

	for(int x = 0; x < mapSize; ++x)
	{
		for(int k = 1; k < dim - 1; ++k)
		{
			inY = helpMatrix[k]; /// set inX and inY for k'th row of helpMatrix
			inYv[k] = myLib::splineOutput(inX, inY, dim, Ah[k - 1], Bh[k - 1], x * scale);
		}
		inYv[0] = 0.;
		inYv[dim - 1] = 0.;

		splineCoeffCount(inX, inYv, dim, Av, Bv);
		for(int y = 0; y < mapSize; ++y)
		{
			/// round shape
			if(smLib::distance(x, y, mapSize / 2., mapSize / 2.) >
			   mapSize * 2. * std::sqrt(2.) / (dim - 1.) ) { continue; }

			val = myLib::splineOutput(inX, inYv, dim, Av, Bv, y * scale);

			/// minMagn ~ blue, maxMagn ~ red
			drawArg = (val - minMagn)
					  / (maxMagn - minMagn) * drawRange;

#if 0
			/// old version 23-Nov-18
			if(maxVal == 0.)
			{
				/// "private" limits
				/// each map from deep blue to deep red
				drawArg = (val - minMagn)
						  / (maxMagn - minMagn) * drawRange;
			}
			else
			{
				/// global limits
#if 0
				/// oldest variant - not really employed
				/// deep blue ~ -maxAbs, deep red ~ +maxAbs
				drawArg = (val + maxAbs)
						  / (2 * maxAbs) * drawRange;
#else
				/// deep blue ~ 0, deep red ~ +maxAbs
				drawArg = val / maxVal * drawRange;
#endif

			}
#endif

			QColor (*colorFunc)(int, int) = myLib::drw::colorFunction(colorTheme);
			paint1.setBrush(QBrush(colorFunc(drawRange, drawArg)));
			paint1.setPen(colorFunc(drawRange, drawArg));
			paint2.setBrush(QBrush(colorFunc(drawRange, drawRange - drawArg)));
			paint2.setPen(colorFunc(drawRange, drawRange - drawArg));

			paint1.drawPoint(x,y);
			paint2.drawPoint(x,y);
		}
	}

	if(1) /// draw channels locations
	{
		/// zeros for absent electrodes
		helpMatrix[1][1] = 0.;
		helpMatrix[1][3] = 0.;
		helpMatrix[1][5] = 0.;
		helpMatrix[5][1] = 0.;
		helpMatrix[5][3] = 0.;
		helpMatrix[5][5] = 0.;

		paint1.setBrush(QBrush("black"));
		paint1.setPen("black");
		paint2.setBrush(QBrush("black"));
		paint2.setPen("black");
		for(int i = 0; i < dim; ++i)
		{
			for(int j = 0; j < dim; ++j)
			{
				if(helpMatrix[i][j] != 0.)
				{
					paint1.drawEllipse(j / scale - 1, i / scale - 1, 2, 2);
					paint2.drawEllipse(j / scale - 1, i / scale - 1, 2, 2);
				}
			}
		}
	}
	double sum = helpMatrix.sum();

	/// draw colour scale with numbers
	if(drawScale)
	{
		QPixmap scalePic = drw::drawColorScale(256, colorTheme, false).scaled(colorsWidth, mapSize);
		QPixmap values(valsWidth, mapSize);
		values.fill();
		QPainter pnt;
		pnt.begin(&values);

		int fontSize = 12;
		pnt.setFont(QFont("Times", fontSize));
		if(maxVal == 0.)
		{
			pnt.drawText(2, fontSize + 2, nm(smLib::doubleRound(maxMagn, 3)));
		}
		else
		{
			pnt.drawText(2, fontSize + 2, nm(smLib::doubleRound(maxVal, 3)));

			/// draw maxMagn line
			pnt.setPen(QPen(QBrush("black"), 2));
			const int h = values.height() * (1. - maxMagn / maxVal);

			pnt.drawLine(0, h, pnt.device()->width(), h);		/// what about minMagn ?
//			pnt.drawText(2, h + fontSize + 2, nm(smLib::doubleRound(maxMagn, 3)));
		}

		if(minMagn != 0.)
		{
			pnt.drawText(2, values.height() - 2, nm(smLib::doubleRound(minMagn, 3)));
		}
		else
		{
			pnt.drawText(2, values.height() - 2, "0.0");
		}

		paint1.drawPixmap(mapSize, 0, colorsWidth, mapSize, scalePic);
		paint1.drawPixmap(mapSize + colorsWidth, 0, valsWidth, mapSize, values);

		paint2.drawPixmap(mapSize, 0, colorsWidth, mapSize, scalePic);
		paint2.drawPixmap(mapSize + colorsWidth, 0, valsWidth, mapSize, values);
	}


	paint1.end();
	paint2.end();

	if(notSolve) { return pic1; }

	/// +- solver
	if(std::abs(maxMagn) > 1.5 * std::abs(minMagn))
	{
		return pic1;
	}
	if(1.5 * std::abs(maxMagn) < std::abs(minMagn))
	{
		return pic2;
	}	
	if(sum >= 0.)
	{
		return pic1;
	}
	return pic2;
}

QString mapPath(const QString & dir,
				const QString & name,
				int i)
{
	return dir + "/" + name + "_" + nm(i) + ".png";
}

void drawMapsICA(const QString & mapsFilePath,
				 const QString & outDir,
				 const QString & outName,
				 const ColorScale colourTheme)
{
	matrix matrixA = myLib::readMatrixFile(mapsFilePath);
	double maxAbs = matrixA.maxAbsVal();

	for(int i = 0; i < matrixA.cols(); ++i)
	{
		drawOneMap(matrixA.getCol(i),
				   0,
				   maxAbs,
				   colourTheme).save(
					mapPath(outDir, outName, i),
					nullptr,
					0);
	}
}

QPixmap drawMapsOnSpectra(const QPixmap & inSpectraPic,
						  const QString & mapsDirPath,
						  const QString & mapsName)
{
	QPixmap pic = inSpectraPic;
	QPainter paint;
	paint.begin(&pic);

	QPixmap pic1;
	QString helpString;
	QRect earRect;

	const double offsetX = 0.7;
	const int earSize = 8; /// generality
	const double shitCoeff = 1.05; /// smth about width of map on spectra pic

	for(int i = 0; i < 21; ++i) /// not more than
	{
		helpString = mapPath(mapsDirPath, mapsName, i);
		if(!QFile::exists(helpString))
		{
			std::cout << "drawMapsOnSpectra: no map file found " << helpString << std::endl;
			break;
		}
		pic1 = QPixmap(helpString);

		const double X = myLib::drw::xc[i] + offsetX * myLib::drw::graphWidth;
		const double Y = myLib::drw::yy[i];
		const double coeff = (shitCoeff - offsetX) * graphWidth / 2;

		paint.drawPixmap(QRect(X,
							   Y,
							   (shitCoeff - offsetX) * myLib::drw::graphWidth,
							   (shitCoeff - offsetX) * myLib::drw::graphHeight),
						 pic1);

		paint.setPen(QPen(QBrush("black"), 2));

		/// draw the nose
		/// left side
		paint.drawLine(X + coeff - 4,
					   Y + 2,
					   X + coeff,
					   Y - 6);
		/// right side
		paint.drawLine(X + coeff + 4,
					   Y + 2,
					   X + coeff,
					   Y - 6);


		/// left ear
		earRect = QRect(X - 0.5 * earSize,
						Y + coeff - earSize,
						earSize,
						2 * earSize);
		paint.drawArc(earRect, 60 * 16, 240 * 16); /// degrees

		/// right ear
		earRect = QRect(X + 2 * coeff - 0.5 * earSize,
						Y + coeff - earSize,
						earSize,
						2 * earSize);
		paint.drawArc(earRect, 240 * 16, 240 * 16); /// degrees

	}
	paint.end();
	return pic;
}

} /// end of namespace drw
} /// end of namespace myLib
