#include <myLib/drw.h>
#include <myLib/signalProcessing.h>

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
	if(myLib::drw::manualNs > 0) return myLib::drw::manualNs;

	for(const int nch : {19, 23, 20, 21})
	{
		if(inData.size() % nch == 0)
		{
			return nch;
		}
	}
}

QPixmap drawOneSignal(const std::valarray<double> & inData,
					  int picHeight)
{
	QPixmap pic(inData.size(), picHeight);
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	const double norm = 0.4 / abs(inData).max();
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

QPixmap drawOneTemplate(const int chanNum,
						const bool channelsFlag,
						const double leftF,
						const double rightF)
{

	QPixmap pic(myLib::drw::graphWidth + 2 * myLib::drw::gap,
				myLib::drw::graphHeight + 2 * myLib::drw::gap); // second gaps for chanNum and 20 Hz
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	QString helpString;
	const double X = myLib::drw::gap;
	const double Y = pic.height() - myLib::drw::gap;

	//draw axes
	paint.setPen("black");
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X,
						   Y - myLib::drw::graphHeight + myLib::drw::fontSizeChan + 4)); /// magic const
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X + myLib::drw::graphWidth,
						   Y));

	//draw Herzes
	const double unit = (rightF - leftF) / myLib::drw::graphWidth;
	auto currFreq = [unit, leftF](int in) -> double {return leftF + in * unit;};
	const int lineLen = 5;

	paint.setFont(QFont("Helvitica", myLib::drw::fontSizeHz));
	for(int k = 0; k < graphWidth + 1; ++k) /// +1 ?
	{
		const double cF = currFreq(k);
		if( std::abs(cF - std::round(cF)) <= unit / 2. )
		{
			paint.drawLine(QPointF(X + k,
								   Y),
						   QPointF(X + k,
								   Y + lineLen)); /// magic const

			helpString = QString::number(std::round(cF));

			if(helpString.toInt() % 5 != 0) continue;

			if(helpString.toInt() < 10)
			{
				paint.drawText(QPointF(X + k - myLib::drw::fontSizeHz / 3,
									   pic.height() - 1),
							   helpString);
			}
			else
			{
				paint.drawText(QPointF(X + k - myLib::drw::fontSizeHz / 3 * 2,
									   pic.height() - 1),
							   helpString);
			}
		}
	}
	/// draw channel name
	paint.setFont(QFont("Helvetica", int(myLib::drw::fontSizeChan), -1, false));
	if(chanNum >= 0)
	{
		if(channelsFlag)
		{
			helpString = coords::lbl21[chanNum]
						 + "(" + QString::number(chanNum + 1) + ")" // can be commented
						 ;
		}
		else
		{
			helpString = QString::number(chanNum + 1);

		}
		paint.drawText(QPointF(X - myLib::drw::fontSizeChan * 2/3,
							   Y - graphHeight + myLib::drw::fontSizeChan),
					   helpString);
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
		paint.drawPixmap(myLib::drw::xx[chanNum],
						 myLib::drw::yy[chanNum],
						 myLib::drw::drawOneTemplate(chanNum, channelsFlag, leftF, rightF));
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

	if(maxVal <= 0.) maxVal = inData.maxAbsVal();

	for(int chanNum = 0; chanNum < inData.size(); ++chanNum)  //exept markers channel
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

	QFont tmpF = QFont("Helvetica", myLib::drw::fontSizeChan);
	paint.setFont(tmpF);
	maxVal = smallLib::doubleRound(maxVal, 1);
	paint.drawText(QPointF(myLib::drw::templateWidth * (myLib::drw::c(4) + 1/32.) + 5,
						   myLib::drw::templateHeight * (myLib::drw::c(0) + 1/32.)
						   + myLib::drw::graphHeight / 2.),
				   QString::number(maxVal) + " mcV^2/Hz");
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
	QPainter paint;
	paint.begin(&pic);


	if(maxVal <= 0.)
	{
		maxVal = (inData + inSigma).max();
	}
	double norm = myLib::drw::graphHeight / maxVal;

	const auto lowLine = inData - inSigma;
	const auto highLine = inData + inSigma;

	int numOfChans = myLib::drw::numOfChans(inData);
	int lineWidth_ = myLib::drw::lineWidth;

	for(auto drawLine : std::vector<lineType>{inData, lowLine, highLine})
	{
		for(int chanNum = 0; chanNum < numOfChans; ++chanNum)  //exept markers channel
		{
			myLib::drw::drawArray(templatePic,
								  drawLine,
								  color,
								  false,
								  maxVal,
								  lineWidth_);

		}
		lineWidth_ = 1; // for lowLine and highLine
	}


	paint.setOpacity(0.5);
	for(int chanNum = 0; chanNum < numOfChans; ++chanNum)
	{
		const double X = myLib::drw::xc[chanNum];
		const double Y = myLib::drw::yc[chanNum];
		/// passage from drawOneArray
		if(inData.size() < myLib::drw::graphWidth)
		{
			const double indexScale = myLib::drw::graphWidth / inData.size();
			for(int k = 0; k < inData.size(); ++k)
			{
				paint.drawLine(QPointF(X + k * indexScale,
									   Y - lowLine[k] * norm),
							   QPointF(X + k * indexScale,
									   Y - highLine[k] * norm));
			}
		}
		else
		{
			const double indexScale = inData.size() / myLib::drw::graphWidth;
			for(int k = 0; k < myLib::drw::graphWidth; ++k)
			{
				paint.drawLine(QPointF(X + k,
									   Y - lowLine[k * indexScale] * norm),
							   QPointF(X + k,
									   Y - highLine[k * indexScale] * norm));
			}
		}
	}
	paint.setOpacity(1.0);



	//returning norm = max magnitude
	paint.setPen("black");
	paint.setFont(QFont("Helvetica", myLib::drw::fontSizeChan));
	paint.drawText(QPointF(myLib::drw::templateWidth * myLib::drw::c(4),
						   myLib::drw::templateHeight * myLib::drw::c(0)
						   + myLib::drw::graphHeight / 2),
				   QString::number(maxVal) + " mcV^2/Hz");

}

QPixmap drawArrays(const QPixmap & templatePic,
				   const matrix & inData,
				   double maxVal,
				   bool weightsFlag,
				   const std::vector<QColor> & colors,
				   std::vector<int> lineWidths)
{
	if(colors.size() < inData.size())
	{
		std::cout << "myLib::drw::drawArrays: not enough colors" << std::endl;
		return QPixmap();
	}

	if(lineWidths.empty())
	{
		lineWidths = std::vector<int> (inData.size(), myLib::drw::lineWidth);
	}

	QPixmap pic = templatePic;
	for(int numArr = 0; numArr < inData.size(); ++numArr)
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


QPixmap drawArraysInLine(const matrix & inMatrix,
						 const std::vector<QColor> & colors)
{
	QPixmap pic(inMatrix.cols(), inMatrix.cols() / 3);
	QPainter paint;
	pic.fill();
	paint.begin(&pic);

	const double offsetY = 1 - 0.1;
	const double norm = pic.height() * offsetY / inMatrix.maxAbsVal();

	for(uint k = 0; k < inMatrix.rows(); ++k)
	{
		paint.setPen(QPen(QBrush(colors[k]), myLib::drw::lineWidth));
		for(int i = 0; i < pic.width() - 1; ++i)
		{
			paint.drawLine(i,
						   pic.height() * (offsetY - inMatrix[k][i] * norm),
						   i + 1,
						   pic.height() * (offsetY - inMatrix[k][i + 1] * norm) );
		}
	}

	const int nch = myLib::drw::numOfChans(inMatrix[0]);

	paint.setPen(QPen(QBrush("black"), 1, Qt::DashLine));
	for(int i = 0; i < nch; ++i)
	{
		paint.drawLine(i * pic.width() / nch,
					   0,
					   i * pic.width() / nch,
					   offsetY * pic.height());
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
					outDir + "/" + fileName + "_" + QString::number(i) + ".jpg", nullptr, 100);
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


QPixmap drawOneSpectrum(const std::valarray<double> & inData,
						double leftFr,
						double rightFr,
						double srate,
						int numOfSmooth,
						const QColor & color)
{
	std::valarray<double> drawArr = myLib::subSpectrumR(
										myLib::smoothSpectre(myLib::spectreRtoR(inData),
															 numOfSmooth),
										leftFr,
										rightFr,
										srate);
	return myLib::drw::drawOneArray(myLib::drw::drawOneTemplate(-1, false, leftFr, rightFr),
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

	if(maxVal <= 0.) maxVal = abs(inData).max();
	const double norm = myLib::drw::graphHeight / maxVal / (weightsFlag + 1);
	Y -= weightsFlag * myLib::drw::graphHeight / 2;

	paint.setPen(QPen(QBrush(color), lineWidth_));

	if(inData.size() < myLib::drw::graphWidth)
	{
		const double indexScale = myLib::drw::graphWidth / inData.size();
		for(int k = 0; k < inData.size() - 1; ++k)
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

	paint.end();
	return pic;
}


void drawMannWitney(const QString & templPath,
					const trivector<int> & inMW,
					const std::vector<QColor> & inColors)
{

	QPixmap pic;
	QPainter paint;


	pic.load(templPath);
	paint.begin(&pic);


	const double barWidth = 1/2.;
	const int barHeight = 5; // pixels
	const int barHeightStart = 18; // pixels
	const int barHeightStep = 8; // pixels

	const double graphWidth = paint.device()->width() * coords::scale;
	const double ext = def::spLength() / graphWidth;

	for(int chanNum = 0; chanNum < def::nsWOM(); ++chanNum)  //exept markers channel
	{
		const double X = paint.device()->width() * coords::x[chanNum];
		const double Y = paint.device()->height() * coords::y[chanNum];
		const int offset = chanNum * def::spLength();

		//statistic difference bars
		int barCounter = 0;
		for(int h = 0; h < def::numOfClasses(); ++h) // class1
		{

			for(int l = h + 1; l < def::numOfClasses(); ++l) // class2
			{
				QColor color1 = QColor(inColors[h]);
				QColor color2 = QColor(inColors[l]);

				for(int j = offset; j < offset + def::spLength(); ++j)
				{
					if(inMW[h][l - h][j] == 0) continue;

					if(inMW[h][l - h][j] == 1) // if class1 spectre is bigger
					{
						paint.setPen(color1);
						paint.setBrush(QBrush(color1));
					}
					else
					{
						paint.setPen(color2);
						paint.setBrush(QBrush(color2));
					}

					paint.drawRect(X + (j % def::spLength() - barWidth) / ext,
								   Y + barHeightStart + barHeightStep * barCounter,
								   2 * barWidth / ext,
								   barHeight);
				}
				++barCounter; // height shift of bars
			}
		}
	}

	paint.end();
	pic.save(templPath, 0, 100);
}

void drawMannWitneyInLine(const QString & picPath,
						  const trivector<int> & inMW,
						  const std::vector<QColor> & inColors)
{
	QPixmap pic;
	pic.load(picPath);
	QPainter paint;
	paint.begin(&pic);

	/// same as drawArraysInLine
	const double offsetY = 1 - 0.1;

	int num = 0;
	for(uint k = 0; k < inMW.size(); ++k)
	{
		for(uint j = k + 1; j < inMW.size(); ++j)
		{
			const int currTop = pic.height() * (offsetY + (1. - offsetY) * num);
			const int currBot = pic.height() * (offsetY + (1. - offsetY) * (num + 1));
//            const int currHeight = currBot - currTop;


			QColor color1 = QColor(inColors[k]);
			QColor color2 = QColor(inColors[j]);

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
	pic.save(picPath, 0, 100);
	paint.end();

}


} // namespace drw
} // namespace myLib
