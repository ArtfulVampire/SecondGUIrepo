#include <myLib/draws.h>

#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <myLib/signalProcessing.h>

using namespace myOut;

namespace myLib
{

// hot-to-cold, http:// stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
double red1(int range, int j)
{
    double part = j / double(range);
    if(0.000 <= part && part <= 0.5) return 0.;
    else if(0.500 < part && part <= 0.800) return (part - 0.5) / (0.8 - 0.5);
    else if(0.800 < part && part <= 1.000) return 1.;
    else return 0.0;
}
double green1(int range, int j)
{
    double part = j / double(range);
    if(0.000 <= part && part <= 0.2) return part * 5;
    else if(0.200 < part && part <= 0.800) return 1.;
    else if(0.800 < part && part <= 1.000) return 1. - (part - 0.8) / (1.0 - 0.8);
    else return 0.0;
}
double blue1(int range, int j)
{
    double part = j / double(range);
    if(0.000 <= part && part <= 0.2) return 1.;
    else if(0.200 < part && part <= 0.500) return 1 - (part - 0.2) / (0.5 - 0.2);
    else if(0.500 < part && part <= 1.000) return 0.;
    else return 0.0;
}

// jet
double red(int range, int j, double V, double S)
{
    double part = j / double(range);
    // matlab
	if    (0. <= part && part <= myLib::colDots_2[0]) return V*(1.-S);
	else if(myLib::colDots_2[0] < part && part <= myLib::colDots_2[1]) return V*(1.-S);
	else if(myLib::colDots_2[1] < part && part <= myLib::colDots_2[2]) return V*(1.-S) + V*S*(part-myLib::colDots_2[1])/(myLib::colDots_2[2] - myLib::colDots_2[1]);
	else if(myLib::colDots_2[2] < part && part <= myLib::colDots_2[3]) return V;
	else if(myLib::colDots_2[3] < part && part <= 1.) return V - V*S*(part-myLib::colDots_2[3])/(1 - myLib::colDots_2[3])/2.;
    // old
    if    (0.000 <= part && part <= 0.167) return V*(1.-S); /// 2. - V*S/2. + V*S*(part)*3.;
    else if(0.167 < part && part <= 0.400) return V*(1.-S);
    else if(0.400 < part && part <= 0.500) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.500 < part && part <= 0.600) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.600 < part && part <= 0.833) return V;
    else if(0.833 < part && part <= 1.000) return V - V*S*(part-0.833)/(1.000-0.833)/2.;
    else return 0.0;
}
double green(int range, int j, double V, double S)
{
    double part = j / double(range);
    double hlp = 1.0;
    // matlab
	if    (0.0 <= part && part <= myLib::colDots_2[0]) return V*(1.-S);
	else if(myLib::colDots_2[0] < part && part <= myLib::colDots_2[1]) return V*(1.-S) + V*S*(part-myLib::colDots_2[0])/(myLib::colDots_2[1] - myLib::colDots_2[0]);
	else if(myLib::colDots_2[1] < part && part <= myLib::colDots_2[2]) return V;
	else if(myLib::colDots_2[2] < part && part <= myLib::colDots_2[3]) return V - V*S*(part-myLib::colDots_2[2])/(myLib::colDots_2[3] - myLib::colDots_2[2]);
	else if(myLib::colDots_2[3] < part && part <= 1.) return V*(1.-S);
    // old
    if    (0.000 <= part && part <= 0.167) return V*(1.-S);
    else if(0.167 < part && part <= 0.400) return V*(1.-S) + V*S*hlp*(part-0.167)/(0.400-0.167);
    else if(0.400 < part && part <= 0.500) return V-V*S*(1.-hlp);
    else if(0.500 < part && part <= 0.600) return V-V*S*(1.-hlp);
    else if(0.600 < part && part <= 0.833) return V-V*S*(1.-hlp) - V*S*hlp*(part-0.600)/(0.833-0.600);
    else if(0.833 < part && part <= 1.000) return V*(1.-S);
    else return 0.0;
}

double blue(int range, int j, double V, double S)
{
    double part = j / double(range);

	if    (0.0 <= part && part <= myLib::colDots_2[0]) return V -V*S/2. + V*S*(part)/(myLib::colDots_2[0] - 0.0)/2.;
	else if(myLib::colDots_2[0] < part && part <= myLib::colDots_2[1]) return V;
	else if(myLib::colDots_2[1] < part && part <= myLib::colDots_2[2]) return V - V*S*(part-myLib::colDots_2[1])/(myLib::colDots_2[2] - myLib::colDots_2[1]);
	else if(myLib::colDots_2[2] < part && part <= myLib::colDots_2[3]) return V*(1.-S);
	else if(myLib::colDots_2[3] < part && part <= 1.) return V*(1.-S);

    // old
    if    (0.000 <= part && part <= 0.167) return V -V*S/2. + V*S*(part)/(0.167-0.000)/2.;
    else if(0.167 < part && part <= 0.400) return V;
    else if(0.400 < part && part <= 0.500) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.500 < part && part <= 0.600) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.600 < part && part <= 0.833) return V*(1.-S);
    else if(0.833 < part && part <= 1.000) return V*(1.-S);


    else return 0.0;
}

QColor hueJet(int range, int j)
{
	if(j > range) j = range;
	if(j < 0) j = 0;
	return QColor(255. * red(range, j),
				  255. * green(range, j),
				  255. * blue(range, j));
}

// hot to cold
QColor hueOld(int range, int j)
{
	if(j > range) j = range;
	if(j < 0) j = 0;

	return QColor(255. * red1(range, j),
				  255. * green1(range, j),
				  255. * blue1(range, j));
}

QColor grayScale(int range, int j)
{
    if(j > range) j = range;
    if(j < 0) j = 0;
    return QColor(255. * (1. - double(j) / range),
                  255. * (1. - double(j) / range),
                  255. * (1. - double(j) / range));
}

QColor qcolor(int range, int j)
{
    double sigmaR = range * 0.35;
    double sigmaG = range * 0.25;
    double sigmaB = range * 0.4;

    double offR = range * (1.0 - 0.3);
    double offG = range * (0.5 + 0.25);
    double offB = range * (0.0 + 0.15);

    return QColor(255. * exp(- (j - offR) * (j - offR) / (2 * sigmaR * sigmaR)),
                  255. * exp(- (j - offG) * (j - offG) / (2 * sigmaG * sigmaG)),
                  255. * exp(- (j - offB) * (j - offB) / (2 * sigmaB * sigmaB)));
}

void drawMapsICA(const QString &mapsFilePath,
                 const QString &outDir,
                 const QString &outName,
                 const ColorScale colourTheme,
                 void (*draw1MapFunc)(const matrix &,
                                      const int,
                                      const QString &,
                                      const QString &,
                                      double,
                                      const int,
                                      const ColorScale))
{
	matrix matrixA  = readMatrixFile(QString(mapsFilePath).remove("_ica"));
    double maxAbs = matrixA.maxAbsVal();

	for(int i = 0; i < DEFS.nsWOM(); ++i)
    {
        draw1MapFunc(matrixA, i, outDir, QString(outName).remove("_ica"), maxAbs, 240, colourTheme);
    }
}


void drawMapsOnSpectra(const QString & inSpectraFilePath,
					   const QString & outSpectraFilePath,
					   const QString & mapsDirPath,
					   const QString & mapsNames)
{
    QPixmap pic;
    pic = QPixmap(inSpectraFilePath);
    QPainter paint;
    paint.begin(&pic);

    QPixmap pic1;
    QString helpString;
    QRect earRect;

    const double offsetX = 0.7;
    const int earSize = 8; // generality
    const double shitCoeff = 1.05; // smth about width of map on spectra pic

    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;

    const QString mapNam = QString(mapsNames).remove("_ica");

	for(int i = 0; i < DEFS.nsWOM(); ++i)
    {
        helpString = mapsDirPath
					 + "/" + mapNam
					 + "_map_" + nm(i) + "+.png";
        if(!QFile::exists(helpString))
        {
            helpString.replace("+.png", "-.png");
            if(!QFile::exists(helpString))
            {
				std::cout << "drawMapsOnSpectra: no map file found " << helpString << std::endl;
                return;
            }
        }
//        std::cout << 1 << std::endl;
        pic1 = QPixmap(helpString);
//        std::cout << 2 << std::endl;



        const double Y = paint.device()->height() * coords::y[i];
        const double X = paint.device()->width() * coords::x[i];

        paint.drawPixmap(QRect(X + offsetX * graphWidth,
                               Y - graphHeight,
                               (shitCoeff - offsetX) * graphWidth,
                               (shitCoeff - offsetX) * graphHeight),
                         pic1);

        paint.setPen(QPen(QBrush("black"), 2));

        // draw the nose
                       // left side
        paint.drawLine(X + offsetX * graphWidth + (shitCoeff - offsetX) * graphWidth/2 - 4,
                       Y - graphHeight + 2,
                       X + offsetX * graphWidth + (shitCoeff - offsetX) * graphWidth/2,
                       Y - graphHeight - 6);
                       // right side
        paint.drawLine(X + offsetX * graphWidth + (shitCoeff - offsetX) * graphWidth/2 + 4,
                       Y - graphHeight + 2,
                       X + offsetX * graphWidth + (shitCoeff - offsetX) * graphWidth/2,
                       Y - graphHeight - 6);


                       // left ear
        earRect = QRect(X + offsetX * graphWidth - 0.5 * earSize,
                        Y - graphHeight + (shitCoeff - offsetX) * graphHeight/2 - earSize,
                        earSize, 2*earSize);
        paint.drawArc(earRect, 60*16, 240*16);


        earRect = QRect(X + offsetX * graphWidth
                        + (shitCoeff - offsetX) * graphWidth - 0.5 * earSize,
                        Y - graphHeight + (shitCoeff - offsetX) * graphHeight/2 - earSize,
                        earSize, 2*earSize);
        paint.drawArc(earRect, 240*16, 240*16);

    }
    paint.end();
    pic.save(outSpectraFilePath, 0, 100);

#if 0
    // save both jpg and png
    helpString = outSpectraFilePath;
    if(helpString.contains(".png"))
    {
        helpString.replace(".png", ".jpg");
        pic.save(helpString, 0, 100);
    }
    else if(helpString.contains(".jpg"))
    {
        helpString.replace(".jpg", ".png");
        pic.save(helpString, 0, 100);
    }
#endif
}

void drawMapSpline(const matrix & matrixA,
                   const int numOfColoumn,
                   const QString & outDir,
                   const QString & outName,
                   double maxAbs,
                   const int picSize,
                   const ColorScale colorTheme)
{
    QPixmap pic = QPixmap(picSize, picSize);
    QPainter painter;
    pic.fill();
    painter.begin(&pic);
    QString savePath1 = outDir
						+ "/" + outName + "_map_"
						+ nm(numOfColoumn) + "+.png";

    QPixmap pic1 = QPixmap(picSize, picSize);
    QPainter painter1;
    pic1.fill();
    painter1.begin(&pic1);
    QString savePath2 = outDir
						+ "/" + outName + "_map_"
						+ nm(numOfColoumn) + "-.png";

    double val;
    double drawArg;
    int drawRange = 256;

    int dim = 7;
    double scale1 = double(dim-1)/picSize;

    matrix helpMatrix(dim, dim, 0.); // generality for ns = 19

    int currIndex = 0.;
    for(int i = 0; i < dim * dim; ++i)
    {
        const int rest = i % dim;
        const int quot = i / dim;

        if(quot % (dim - 1) == 0|| rest % (dim-1) == 0)  // set 0 to all edge values
        {
            helpMatrix[quot][rest] = 0.;
        }
        else if(quot == 1
                && (rest - 1) * (rest - 3) * (rest - 5) == 0) // Fp3, Fpz, Fp4
        {
            helpMatrix[quot][rest] = 0.;
        }
        else if(quot == 5
                && (rest - 1) * (rest - 3) * (rest - 5) == 0) // O3, Oz, O4
        {
            helpMatrix[quot][rest] = 0.;
        }
        else
        {
            helpMatrix[quot][rest] = matrixA[currIndex++][numOfColoumn];
        }
    }

    // 
//    for(int)
    // approximation for square - Fp3, Fpz, Fp, O3, Oz, O4
    helpMatrix[1][1] = (helpMatrix[1][2] + helpMatrix[2][1] + helpMatrix[2][2])/3.;
    helpMatrix[1][3] = (helpMatrix[1][2] + helpMatrix[1][4] + helpMatrix[2][2] + helpMatrix[2][3] + helpMatrix[2][4])/5.;
    helpMatrix[1][5] = (helpMatrix[1][4] + helpMatrix[2][4] + helpMatrix[2][5])/3.;;
    helpMatrix[5][1] = (helpMatrix[4][1] + helpMatrix[4][2] + helpMatrix[5][2])/3.;
    helpMatrix[5][3] = (helpMatrix[5][2] + helpMatrix[4][2] + helpMatrix[4][3] + helpMatrix[4][4] + helpMatrix[4][5])/5.;
    helpMatrix[5][5] = (helpMatrix[5][4] + helpMatrix[4][4] + helpMatrix[4][5])/3.;

    const double minMagn = helpMatrix.minVal();
    const double maxMagn = helpMatrix.maxVal();

    matrix Ah(5, 6);
    matrix Bh(5, 6);
	std::valarray<double> inX(dim);
	std::valarray<double> inY(dim);
	std::valarray<double> inYv(dim);
	std::valarray<double> Av(dim - 1);
	std::valarray<double> Bv(dim - 1);

    std::iota(std::begin(inX), std::end(inX), 0); // hope, it's constant

    for(int i = 1; i < dim - 1; ++i) // number of helpMatrix row
    {
        inY = helpMatrix[i];
		myLib::splineCoeffCount(inX, inY, dim, Ah[i - 1], Bh[i - 1]); // horizontal splines coeffs
    }

    for(int x = 0; x < picSize; ++x)
    {
        for(int k = 1; k < dim - 1; ++k)
        {
            inY = helpMatrix[k]; // set inX and inY for k'th row of helpMatrix
            inYv[k] = splineOutput(inX, inY, dim, Ah[k - 1], Bh[k - 1], x * scale1);
        }
        inYv[0] = 0.;
        inYv[dim - 1] = 0.;
        splineCoeffCount(inX, inYv, dim, Av, Bv);
        for(int y = 0; y < picSize; ++y)
        {
            /// round shape
			if(smLib::distance(x, y, picSize/2, picSize/2) >
               picSize * 2. * sqrt(2.) / (dim - 1) ) continue;

            val = splineOutput(inX, inYv, dim, Av, Bv, y*scale1);
            if(maxAbs == 0)
            {
                // "private" limits
                // each map frop deep blue to deep red
                drawArg = (val - minMagn)
                        / (maxMagn - minMagn) * drawRange;
            }
            else
            {
                // global limits
                // current variant
                drawArg = (val + maxAbs)
                        / (2 * maxAbs) * drawRange; // if common maxAbs
            }


            switch(colorTheme)
            {
			case ColorScale::jet:
            {
				painter.setBrush(QBrush(myLib::hueJet(drawRange, drawArg)));
				painter.setPen(myLib::hueJet(drawRange, drawArg));
				painter1.setBrush(QBrush(myLib::hueJet(drawRange, drawRange - drawArg)));
				painter1.setPen(myLib::hueJet(drawRange, drawRange - drawArg));
                break;
            }
			case ColorScale::htc:
            {
				painter.setBrush(QBrush(myLib::hueOld(drawRange, drawArg)));
				painter.setPen(myLib::hueOld(drawRange, drawArg));
				painter1.setBrush(QBrush(myLib::hueOld(drawRange, drawRange - drawArg)));
				painter1.setPen(myLib::hueOld(drawRange, drawRange - drawArg));
                break;
            }
			case ColorScale::gray:
            {
				painter.setBrush(QBrush(myLib::grayScale(drawRange, drawArg)));
				painter.setPen(myLib::grayScale(drawRange, drawArg));
				painter1.setBrush(QBrush(myLib::grayScale(drawRange, drawRange - drawArg)));
				painter1.setPen(myLib::grayScale(drawRange, drawRange - drawArg));
                break;
            }
			case ColorScale::pew:
            {
				painter.setBrush(QBrush(myLib::grayScale(drawRange, drawArg)));
				painter.setPen(myLib::grayScale(drawRange, drawArg));
				painter1.setBrush(QBrush(myLib::grayScale(drawRange, drawRange - drawArg)));
				painter1.setPen(myLib::grayScale(drawRange, drawRange - drawArg));
                break;
            }
            }
            painter.drawPoint(x,y);
            painter1.drawPoint(x,y);
        }
    }

    if(1) // draw channels locations
    {
        // zero for absent electrodes
        helpMatrix[1][1] = 0.;
        helpMatrix[1][3] = 0.;
        helpMatrix[1][5] = 0.;
        helpMatrix[5][1] = 0.;
        helpMatrix[5][3] = 0.;
        helpMatrix[5][5] = 0.;

        painter.setBrush(QBrush("black"));
        painter1.setBrush(QBrush("black"));
        painter.setPen("black");
        painter1.setPen("black");
        for(int i = 0; i < dim; ++i)
        {
            for(int j = 0; j < dim; ++j)
            {
                if(helpMatrix[i][j] != 0.)
                {
                    painter.drawEllipse(j/scale1-1, i/scale1-1, 2, 2);
                    painter1.drawEllipse(j/scale1-1, i/scale1-1, 2, 2);
                }
            }
        }
    }
    pic.save(savePath1, 0, 100);
    pic1.save(savePath2, 0, 100);


    double sum = 0.;
    for(uint i = 0; i < helpMatrix.rows(); ++i)
    {
        for(uint j = 0; j < helpMatrix.cols(); ++j)
        {
            sum += helpMatrix[i][j];
        }
    }

#if 1
    // +- solver
    if(std::abs(maxMagn) > 1.5 * std::abs(minMagn))
    {
        QFile::remove(savePath2);
    }
    else if(1.5 * std::abs(maxMagn) < std::abs(minMagn))
    {
        QFile::remove(savePath1);
    }
    else if(sum >= 0.)
    {
        QFile::remove(savePath2);
    }
    else if(sum < 0.)
    {
        QFile::remove(savePath1);
    }
#endif
}


QPixmap drawOneSignal(const std::valarray<double> & signal,
					  int picHeight,
					  QString outPath)
{

	const int penWidth = 2;

	QPixmap pic(signal.size(), picHeight);
	pic.fill();
	QPainter pnt;
	pnt.begin(&pic);

	const double norm = 0.4 / abs(signal).max();
//	const double norm = 0.4 / 20.;
	pnt.setPen(QPen(QBrush("black"), penWidth));
	for(int i = 0; i < pic.width() - 1; ++i)
	{
		pnt.drawLine(i,
					 pic.height() * (0.5 - signal[i] * norm),
					 i + 1,
					 pic.height() * (0.5 - signal[i + 1] * norm));
	}
	pnt.end();
	if(!outPath.isEmpty())
	{
		pic.save(outPath, 0, 100);
	}
	return pic;
}

QPixmap drawOneTemplate(const int chanNum,
						const bool channelsFlag,
						const QString & savePath,
						const double leftF,
						const double rightF)
{
	const int gap = 20;
	const int fontSize = 24;
	const int graphHeight = 250;
	const int graphWidth = 250;
	QString helpString;

	QPixmap pic( graphWidth + 2 * gap, graphHeight + 2 * gap); // second gap for chanNum and 20 Hz
	pic.fill();
	QPainter paint;
	paint.begin(&pic);

	const double X = gap;
	const double Y = pic.height() - gap;

	// draw axes
	paint.setPen("black");
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X,
						   Y - graphHeight));
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X + graphWidth,
						   Y));

	// draw Herzes
	const double hzFontSize = fontSize / 3;
	paint.setFont(QFont("Helvitica", int(hzFontSize)));

	const double unit = (rightF - leftF) / graphWidth;
	auto currFreq = [unit, leftF](int in) -> double { return leftF + in * unit; };

	for(int k = 0; k < graphWidth + 1; ++k)
	{
		const double cF = currFreq(k);
		if( std::abs(cF - std::round(cF)) <= unit / 2. )
		{
			paint.drawLine(QPointF(X + k,
								   Y),
						   QPointF(X + k,
								   Y + 5)); /// magic const

			helpString = nm(std::round(cF));

			/// Hz to draw - useless
			if(helpString.toInt() % 5 != 0) continue;


			if(helpString.toInt() < 10)
			{
				paint.drawText(QPointF(X + k - hzFontSize / 3,
									   Y + hzFontSize * 2),
							   helpString);
			}
			else
			{
				paint.drawText(QPointF(X + k - hzFontSize / 3 * 2,
									   Y + hzFontSize * 2),
							   helpString);
			}
		}
	}
	paint.setFont(QFont("Helvetica", int(fontSize), -1, false));
	if(chanNum >= 0)
	{
		if(channelsFlag)
		{
			helpString = coords::lbl21[chanNum]
						 + "(" + nm(chanNum + 1) + ")" // can be commented
						 ;
		}
		else
		{
			helpString = nm(chanNum + 1);

		}
		paint.drawText(QPointF(X - fontSize / 2,
							   Y - graphHeight + fontSize),
					   helpString);
	}
	paint.end();
	if(!savePath.isEmpty())
	{
		pic.save(savePath, nullptr, 100);
	}
	return pic;

}

QPixmap drawTemplate(const QString & outPath,
					 bool channelsFlag,
					 int width,
					 int height)
{
	QPixmap pic(width, height);
    QPainter paint;
	pic.fill();
	paint.begin(&pic);


    QString helpString;

    int numOfChan = coords::numOfChan;

	if(outPath.contains("_ica"))
    {
		channelsFlag = false; // only numbers for ICAs
    }





    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;
    const double graphScale = DEFS.spLength() / graphWidth;
    // some empirical values prepared for defauls 1600*1600
    const double scaleY = paint.device()->height() / 1600.;
    const double scaleX = paint.device()->width() / 1600.;


	const double unit = graphScale * DEFS.spStep();
	auto currFreq = [unit](int in) -> double
	{
		return DEFS.left() * DEFS.spStep() + in * unit;
	};
    for(int c2 = 0; c2 < numOfChan; ++c2)  // exept markers channel
    {
		/// replace with drawOneTemplate

        const double Y = paint.device()->height() * coords::y[c2];
        const double X = paint.device()->width() * coords::x[c2];

        // draw axes
        paint.setPen("black");
        paint.drawLine(QPointF(X,
                               Y),
                       QPointF(X,
                               Y - graphHeight + 28)); // 28 for font
        paint.drawLine(QPointF(X,
                               Y),
                       QPointF(X + graphWidth,
                               Y));

        // draw Herzes
        paint.setFont(QFont("Helvitica", int(12 * scaleY)));
        for(int k = 0; k < graphWidth; ++k) // for every Hz generality
        {

			const double cF = currFreq(k);
			if( std::abs(cF - std::round(cF)) <= unit / 2. )
            {
                paint.drawLine(QPointF(X + k,
                                       Y),
                               QPointF(X + k,
                                       Y + 5 * scaleY));

				helpString = nm(int((DEFS.left() + k * graphScale) * DEFS.spStep() + 0.5));

                /// Hz to draw
                if(helpString.toInt() % 5 != 0) continue;


                if(helpString.toInt() < 10)
                {
                    paint.drawText(QPointF(X + k - 4 * scaleX,
                                           Y + 20 * scaleY),
                                   helpString);
                }
                else
                {
                    paint.drawText(QPointF(X + k - 9 * scaleX,
                                           Y + 20 * scaleY),
                                   helpString);
                }
            }
        }
        paint.setFont(QFont("Helvetica", int(24 * scaleY), -1, false));
        if(channelsFlag)
        {
            helpString = coords::lbl21[c2]
						 + "(" + nm(c2 + 1) + ")" // can be commented
                         ;
        }
        else
        {
			helpString = nm(c2 + 1);

        }
        paint.drawText(QPointF(X - 16 * scaleX,
                               Y - graphHeight + 24 * scaleY),
                       helpString);

    }
    // write channels labels

	paint.end();
	if(!outPath.isEmpty() && !outPath.contains(".svg"))
    {
        pic.save(outPath, 0, 100);
	}
	return pic;
}

void drawArray(const QString & templPath,
               const matrix & inData,
               const QString & color,
               double scaling,
               int lineWidth)
{
    QPixmap pic;
    QPainter paint;
    QString helpString;
    int numOfChan = inData.rows();
    if(numOfChan > 21)
    {
        return;
    }

	pic.load(templPath);
	paint.begin(&pic);


    double norm = inData.maxVal();

#if 0
    // for weights
    norm /= 2;
#endif


    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;

//    const double graphScale = DEFS.spLength() / graphWidth;
    const double graphScale = inData.cols() / graphWidth;

    // initial fonts prepared for 1600*1600
    const double scaleY = paint.device()->height() / 1600.;
    const double scaleX = paint.device()->width() / 1600.;

    norm = graphHeight / norm ; // 250 - pixels per graph, generality
    norm *= scaling;

    for(int c2 = 0; c2 < numOfChan; ++c2)  // exept markers channel
    {

        const double Y = paint.device()->height() * coords::y[c2];
        const double X = paint.device()->width() * coords::x[c2];

#if 0
        // for weigths
        paint.setPen(QPen(QBrush("black"), 1))
        paint.drawLine(QPointF(X,
                               Y - graphHeight / 2),
                QPointF(X + graphWidth,
                        Y - graphHeight / 2));
#endif

        // draw spectra
        for(int k = 0; k < graphWidth - 1; ++k)
        {
            paint.setPen(QPen(QBrush(QColor(color)), lineWidth));

#if 1
            // usual
            paint.drawLine(QPointF(X + k,
                                   Y - inData[c2][k * graphScale] * norm),
                    QPointF(X + k + 1,
                            Y - inData[c2][(k + 1) * graphScale] * norm));
#else
            // weights
            paint.drawLine(QPointF(X + k,
                                   Y - graphHeight / 2.
                                   - inData[c2][k * graphScale] * norm),
                    QPointF(X + k + 1,
                            Y - graphHeight / 2.
                            - inData[c2][(k + 1) * graphScale] * norm));
#endif

        }
    }

    // returning norm = max magnitude
    norm /= scaling;
    norm = graphHeight / norm;
    norm /= scaling;  // scaling generality
	norm = smLib::doubleRound(norm,
					   std::min(1., 2 - floor(log10(norm)) )
                       );

    helpString.setNum(norm);
    helpString += QObject::tr(" mcV^2/Hz");
    paint.setPen("black");
    paint.setFont(QFont("Helvetica", int(24 * scaleY)));
    paint.drawText(QPointF(pic.width() * coords::x[6] + 5 * scaleX,
                   pic.height() * coords::y[1] - graphHeight / 2),
            helpString);

    pic.save(templPath, 0, 100);
}

/// overload matrix/std::valarray<double>
void drawArray(const QString & templPath,
			   const std::valarray<double> & inData,
               const QString & color,
               double scaling,
               int lineWidth)
{
    matrix dataS;
    const int len = inData.size();
    for(const int nch : {19, 20, 21}) /// order important as 63 % 21 == 0
    {
        if(len % nch == 0)
        {
			dataS.resize(nch, len / nch, 0.);
            for(int i = 0; i < nch; ++i)
            {
                std::copy(std::begin(inData) + len / nch * i,
                          std::begin(inData) + len / nch * (i + 1),
                          std::begin(dataS[i]));
            }
            break;
        }
    }

    if(dataS.size() <= 0)
    {
        return;
    }

    drawArray(templPath,
              dataS,
              color,
              scaling,
              lineWidth);
}

int BaklushevChans = 19;

void drawArrayWithSigma(const QString &templPath,
						const std::valarray<double> &inData,
						const std::valarray<double> &inSigma,
						double norm,
						const QString &color,
						int lineWidth)
{
	QPixmap pic;
	QPainter paint;
	pic.load(templPath);
	paint.begin(&pic);

	QString helpString;
	int numOfChan = 19;

	if(inData.size() != inSigma.size())
	{
		std::cout << "data and sigma different length" << std::endl;
		return;
	}

	if(norm <= 0.)
	{
		norm = (inData + inSigma).max();
	}

	const double graphHeight = paint.device()->height() * coords::scale;
	const double graphWidth = paint.device()->width() * coords::scale;

	const int spL = inData.size() / numOfChan;
	const double graphScale =  spL / graphWidth;

	// initial fonts prepared for 1600*1600
	const double scaleY = paint.device()->height() / 1600.;
	const double scaleX = paint.device()->width() / 1600.;

	norm = graphHeight / norm ; // 250 - pixels per graph, generality

	const auto lowLine = inData - inSigma;
	const auto highLine = inData + inSigma;


	for(auto drawLine : std::vector<std::valarray<double>>{inData, lowLine, highLine})
	{
		for(int c2 = 0; c2 < BaklushevChans; ++c2)  // exept markers channel
		{
//			auto index = [=](int in) -> int
//			{
//				return c2 * spL + in * graphScale;
//			};

			auto index2 = [=](int in) -> int
			{
				return c2 * spL + in;
			};

			const double Y = paint.device()->height() * coords::y[c2];
			const double X = paint.device()->width() * coords::x[c2];



			// draw spectra
//			for(int k = 0; k < graphWidth - 1; ++k)
			for(int k = 0; k < spL - 1; ++k)
			{
				paint.setPen(QPen(QBrush(QColor(color)), lineWidth));

				// usual

//				paint.drawLine(QPointF(X + k,
//									   Y - drawLine[index(k)] * norm),
//						QPointF(X + k + 1,
//								Y - drawLine[index(k+1)] * norm));

				paint.drawLine(QPointF(X + k / graphScale,
									   Y - drawLine[index2(k)] * norm),
						QPointF(X + (k + 1)  / graphScale,
								Y - drawLine[index2(k+1)] * norm));

			}

		}

		lineWidth = 1;
	}



	paint.setOpacity(0.5);
	for(int c2 = 0; c2 < BaklushevChans; ++c2)
	{
		auto index2 = [=](int in) -> int
		{
			return c2 * spL + in;
		};
//		auto index = [=](int in) -> int
//		{
//			return c2 * spL + in * graphScale;
//		};

		const double Y = paint.device()->height() * coords::y[c2];
		const double X = paint.device()->width() * coords::x[c2];
		for(int k = 0; k < spL; ++k)
//		for(int k = 0; k < graphWidth; ++k)
		{
			paint.drawLine(QPointF(X + k / graphScale,
								   Y - highLine[index2(k)] * norm),
					QPointF(X + k / graphScale,
							Y - lowLine[index2(k)] * norm));
//			paint.drawLine(QPointF(X + k,
//								   Y - highLine[index(k)] * norm),
//					QPointF(X + k,
//							Y - lowLine[index(k)] * norm));
		}
	}
	paint.setOpacity(1.0);



	// returning norm = max magnitude
	norm = graphHeight / norm;
	norm = smLib::doubleRound(norm,
								 std::min(1., 2 - floor(log10(norm)) )
								 );

	helpString.setNum(norm);
	helpString += QObject::tr(" mcV^2/Hz");
	paint.setPen("black");
	paint.setFont(QFont("Helvetica", int(24 * scaleY)));
	paint.drawText(QPointF(pic.width() * coords::x[6] + 5 * scaleX,
				   pic.height() * coords::y[1] - graphHeight / 2),
			helpString);

	pic.save(templPath, 0, 100);
}

QPixmap drawArrays(const QPixmap & templPixmap,
				  const matrix & inMatrix,
				  const bool weightsFlag,
				  const spectraNorming normType,
				  double norm,
				  const std::vector<QColor> & colors,
				  const double scaling,
				  const int lineWidth)
{


	QString helpString;
	int numOfChan = inMatrix.cols() / DEFS.spLength();

	// test size
	int shouldSize = numOfChan * DEFS.spLength();

	for(const auto & inData : inMatrix)
	{
		if(inData.size() > shouldSize)
		{
			std::cout << "drawArrays: inappropriate array size = " << inData.size() << std::endl;
			return templPixmap;
		}
	}

	if(norm <= 0.)
	{
		std::for_each(inMatrix.begin(),
					  inMatrix.end(),
					  [&norm](std::valarray<double> inData)
		{
			norm = std::max(norm, std::abs(inData).max()); // fabs for negative weights e.g.
		});

		if(weightsFlag)
		{
			norm *= 2;
		}
	}


	QPixmap pic = templPixmap;
	QPainter paint;
	paint.begin(&pic);

	const double graphHeight = paint.device()->height() * coords::scale;
	const double graphWidth = paint.device()->width() * coords::scale;
	const double graphScale = DEFS.spLength() / graphWidth;
	// initial fonts prepared for 1600*1600
	const double scaleY = paint.device()->height() / 1600.;
	const double scaleX = paint.device()->width() / 1600.;

	norm = graphHeight / norm ; // 250 - pixels per graph, generality
	norm *= scaling;

	const double normBC = norm;

	for(int c2 = 0; c2 < numOfChan; ++c2)
	{
		const double Y = paint.device()->height() * coords::y[c2];
		const double X = paint.device()->width() * coords::x[c2];

		if(weightsFlag)
		{
			// for weigths
			paint.setPen(QPen(QBrush("black"), 1));
			paint.drawLine(QPointF(X, Y - graphHeight / 2),
						   QPointF(X + graphWidth, Y - graphHeight / 2));
		}

		norm = normBC;
		if(normType == spectraNorming::each)
		{
			// norm each channel by max peak
			norm = 0;
			for(uint i = 0; i < inMatrix.size(); ++i)
			{
				for(int j = 0; j < DEFS.spLength(); ++j)
				{
					norm = fmax(norm,
								std::abs(inMatrix[i][DEFS.spLength() * c2 + j])
						   * (1. + (j > 0.7 * DEFS.spLength()) * 0.7) );
				}
			}

			paint.setPen("black");
			paint.setFont(QFont("Helvetica", int(20 * scaleY)));

							   #if 0
			paint.drawText(QPointF(X + graphWidth * 0.4,
								   Y - graphHeight - 5 * scaleY),
						   nm(doubleRound(norm)));
							   #endif

			// make drawNorm
			norm = graphHeight / norm ; // 250 - pixels per graph, generality
			norm *= scaling;
		}


		const int offset = c2 * DEFS.spLength();
		for(uint numVec = 0; numVec < inMatrix.size(); ++numVec)
		{
			const std::valarray<double> & inData = inMatrix[numVec];
			// draw spectra
//			for(int k = 0; k < graphWidth - 1; ++k)
			for(int k = 0; k < DEFS.spLength() - 1; ++k)
			{
				paint.setPen(QPen(QBrush(QColor(colors[numVec])), lineWidth));
				if(weightsFlag)
				{
					// weights
					paint.drawLine(QPointF(X + k,
										   Y - graphHeight / 2.
										   - inData[offset + k * graphScale] * norm),
							QPointF(X + k + 1,
									Y - graphHeight / 2.
									- inData[offset + (k + 1) * graphScale] * norm));

				}
				else
				{
					// usual
//					paint.drawLine(QPointF(X + k,
//										   Y - inData[offset + k * graphScale] * norm),
//							QPointF(X + k + 1,
//									Y - inData[offset + (k + 1) * graphScale] * norm));
					/// smooth
					paint.drawLine(QPointF(X + k / graphScale,
										   Y - inData[offset + k] * norm),
							QPointF(X + (k + 1) / graphScale,
									Y - inData[offset + k + 1] * norm));
				}
			}
		}
	}

	// returning norm = max magnitude
	norm /= scaling;
	norm = graphHeight / norm;
	norm /= scaling;  // scaling generality
	norm = smLib::doubleRound(norm,
								 std::min(1., 2 - floor(log10(norm)) )
								 );
	if(normType == spectraNorming::all)
	{
		paint.drawLine(QPointF(paint.device()->width() * coords::x[6],
					   paint.device()->height() * coords::y[1]),
				QPointF(paint.device()->width() * coords::x[6],
				paint.device()->height() * coords::y[1] - graphHeight));

		helpString.setNum(norm);
		helpString += QObject::tr(" mcV^2/Hz");
		paint.setPen("black");
		paint.setFont(QFont("Helvetica", int(24 * scaleY)));
		paint.drawText(QPointF(pic.width() * coords::x[6] + 5 * scaleX,
					   pic.height() * coords::y[1] - graphHeight / 2),
				helpString);
	}
	paint.end();
	return pic;
}

double drawArrays(const QString & templPath,
				  const matrix & inMatrix,
				  const bool weightsFlag,
				  const spectraNorming normType,
				  double norm,
				  const std::vector<QColor> & colors,
				  const double scaling,
				  const int lineWidth)
{
    QPixmap pic;
    QPainter paint;
    QString helpString;
    int numOfChan = inMatrix.cols() / DEFS.spLength();

	pic.load(templPath);
	paint.begin(&pic);

    // test size
    int shouldSize = numOfChan * DEFS.spLength();
//	std::cout << "numOfChan = " << numOfChan << "\t"
//			  << "spLength = " << DEFS.spLength() << std::endl;

    std::for_each(inMatrix.begin(),
                  inMatrix.end(),
				  [shouldSize](std::valarray<double> inData)
    {
        if(inData.size() > shouldSize)
        {
//			std::cout << "drawArrays: inappropriate array size = " << inData.size()
//					  << "should be = " << shouldSize << std::endl;
            return;
        }
    });

    if(norm <= 0.)
    {
		norm = inMatrix.maxAbsVal();
        if(weightsFlag)
        {
            // for weights
            norm *= 2;
        }
    }
    const double res = norm;

    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;
    const double graphScale = DEFS.spLength() / graphWidth;
    // initial fonts prepared for 1600*1600
    const double scaleY = paint.device()->height() / 1600.;
    const double scaleX = paint.device()->width() / 1600.;

    norm = graphHeight / norm ; // 250 - pixels per graph, generality
    norm *= scaling;

    const double normBC = norm;

    for(int c2 = 0; c2 < numOfChan; ++c2)  // exept markers channel
    {
        const double Y = paint.device()->height() * coords::y[c2];
        const double X = paint.device()->width() * coords::x[c2];
        const int offset = c2 * DEFS.spLength();

        if(weightsFlag)
        {
            // for weigths
            paint.setPen(QPen(QBrush("black"), 1));
            paint.drawLine(QPointF(X, Y - graphHeight / 2),
                           QPointF(X + graphWidth, Y - graphHeight / 2));
        }

        norm = normBC;
		if(normType == spectraNorming::each)
        {
            // norm each channel by max peak
            norm = 0;
            for(uint i = 0; i < inMatrix.size(); ++i)
            {
                for(int j = 0; j < DEFS.spLength(); ++j)
                {
                    norm = fmax(norm,
                                std::abs(inMatrix[i][DEFS.spLength() * c2 + j])
                           * (1. + (j > 0.7 * DEFS.spLength()) * 0.7) );
                }
            }

            paint.setPen("black");
            paint.setFont(QFont("Helvetica", int(20 * scaleY)));

                               #if 0
            paint.drawText(QPointF(X + graphWidth * 0.4,
                                   Y - graphHeight - 5 * scaleY),
						   nm(doubleRound(norm)));
                               #endif

            // make drawNorm
            norm = graphHeight / norm ; // 250 - pixels per graph, generality
            norm *= scaling;
        }


        for(uint numVec = 0; numVec < inMatrix.size(); ++numVec)
        {
			std::valarray<double> inData = inMatrix[numVec];
            // draw spectra
            for(int k = 0; k < graphWidth - 1; ++k)
            {
                paint.setPen(QPen(QBrush(QColor(colors[numVec])), lineWidth));
                if(weightsFlag)
                {
                    // weights
                    paint.drawLine(QPointF(X + k,
                                           Y - graphHeight / 2.
                                           - inData[offset + k * graphScale] * norm),
                            QPointF(X + k + 1,
                                    Y - graphHeight / 2.
                                    - inData[offset + (k + 1) * graphScale] * norm));

                }
                else
                {
                    // usual
                    paint.drawLine(QPointF(X + k,
                                           Y - inData[offset + k * graphScale] * norm),
                            QPointF(X + k + 1,
                                    Y - inData[offset + (k + 1) * graphScale] * norm));
                }



            }
        }
    }

	// returning norm = max magnitude
    norm /= scaling;
    norm = graphHeight / norm;
    norm /= scaling;  // scaling generality
	norm = smLib::doubleRound(norm,
								 std::min(1., 2 - floor(log10(norm)) )
								 );
	if(normType == spectraNorming::all)
    {
        paint.drawLine(QPointF(paint.device()->width() * coords::x[6],
                       paint.device()->height() * coords::y[1]),
                QPointF(paint.device()->width() * coords::x[6],
                paint.device()->height() * coords::y[1] - graphHeight));

        helpString.setNum(norm);
        helpString += QObject::tr(" mcV^2/Hz");
        paint.setPen("black");
        paint.setFont(QFont("Helvetica", int(24 * scaleY)));
        paint.drawText(QPointF(pic.width() * coords::x[6] + 5 * scaleX,
                       pic.height() * coords::y[1] - graphHeight / 2),
                helpString);
    }


    paint.end();
	if(!templPath.isEmpty())
	{
		pic.save(templPath, 0, 100);
	}
//	return pic;

	return res;
}

void drawArraysInLine(const QString & picPath,
                      const matrix & inMatrix,
					  const std::vector<QColor> & colors,
                      const double scaling,
                      const int lineWidth)
{
    QPixmap pic(inMatrix.cols(), inMatrix.cols() / 3); // generality
    pic.fill();
    QPainter pnt;
    pnt.begin(&pic);


    const double offsetY = 1 - 0.1;
    const double norm = inMatrix.maxVal() * scaling * offsetY; /// check scaling * or /

    for(uint k = 0; k < inMatrix.rows(); ++k)
    {
		pnt.setPen(QPen(QBrush(colors[k]), lineWidth));
        for(int i = 0; i < pic.width() - 1; ++i)
        {
            pnt.drawLine(i, (offsetY - inMatrix[k][i] / norm) * pic.height(),
                         i + 1, (offsetY - inMatrix[k][i + 1] / norm) * pic.height());
        }
    }
	pnt.setPen(QPen(QBrush("black"), 1));
	for(int i = 0; i < DEFS.nsWOM(); ++i)
    {
		pnt.drawLine(i * pic.width() / DEFS.nsWOM(), offsetY * pic.height(),
					 i * pic.width() / DEFS.nsWOM(), 0);
    }

    pic.save(picPath, 0, 100);
    pnt.end();

}



///////////////////////// should add scales and shit
void drawCutOneChannel(const QString & inSpectraPath,
                       const int numChan)
{
    QString outPath = inSpectraPath;
	outPath.replace(".", QString("_" + nm(numChan) + "."));

    QPixmap pic = QPixmap(inSpectraPath);
    QPixmap cut = pic.copy(QRect(coords::x[numChan] * pic.width() - 20,
                                 coords::y[numChan] * pic.height() - 250 - 10,
                                 250 + 38,
								 250 + 53)); /// magic constants

    cut.save(outPath, 0, 100);

}

///// the same magic constants as in drawCutOneChannel
QPixmap drawOneSpectrum(const std::valarray<double> & signal,
						const QString & outPath,
						double leftFr,
						double rightFr,
						double srate,
						int numOfSmooth,
						const QString & color,
						int lineWidth)
{
	/// to namespace
	const int gap = 20;
	const int graphHeight = 250;
	const int graphWidth = 250;


	QPixmap pic = drawOneTemplate(-1, false, {}, leftFr, rightFr);
	QPainter pnt;
	pnt.begin(&pic);


	std::valarray<double> drawArr = myLib::subSpectrumR(
										myLib::smoothSpectre(myLib::spectreRtoR(signal),
															 numOfSmooth),
										leftFr,
										rightFr,
										srate);

	const double X = gap;
	const double Y = pnt.device()->height() - gap;
	const double norm = graphHeight / drawArr.max();
//	const double norm = graphHeight / 3.7e6;
	const double normX = double(drawArr.size()) / graphWidth;

	pnt.setPen(QPen(QBrush(QColor(color)), lineWidth));
	for(int k = 0; k < graphWidth - 1; ++k)
	{
		pnt.drawLine(QPoint(X + k,
							Y - drawArr[k * normX] * norm),
					 QPoint(X + k + 1,
							Y - drawArr[(k + 1) * normX] * norm));
	}
	pnt.end();
	if(!outPath.isEmpty())
	{
		pic.save(outPath, nullptr, 100);
	}
	return pic;
}

///// the same magic constants as in drawCutOneChannel
QPixmap drawOneArray(const QString & templPath,
					 const std::valarray<double> & arr,
					 const QString & outPath,
					 const QString & color,
					 int lineWidth)
{
	/// to namespace
	const int gap = 20;
	const int graphHeight = 250;
	const int graphWidth = 250;
	if(arr.size() != graphWidth)
	{
		std::cout << "drawOneArray: wrong input array size" << std::endl;
		return {};
	}

	QPixmap pic;
	QPainter pnt;
	pic.load(templPath);
	pnt.begin(&pic);

	const double X = gap;
	const double Y = pnt.device()->height() - gap;
	const double norm = graphHeight / arr.max();

	pnt.setPen(QPen(QBrush(QColor(color)), lineWidth));
	for(int k = 0; k < graphWidth - 1; ++k)
	{
		pnt.drawLine(QPoint(X + k,
							Y - arr[k] * norm),
					 QPoint(X + k + 1,
							Y - arr[k + 1] * norm));
	}
	pnt.end();
	if(!outPath.isEmpty())
	{
		pic.save(outPath, nullptr, 100);
	}
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
    const double ext = DEFS.spLength() / graphWidth;

	for(int c2 = 0; c2 < DEFS.nsWOM(); ++c2)  // exept markers channel
    {
        const double X = paint.device()->width() * coords::x[c2];
        const double Y = paint.device()->height() * coords::y[c2];
        const int offset = c2 * DEFS.spLength();

        // statistic difference bars
        int barCounter = 0;
        for(int h = 0; h < DEFS.numOfClasses(); ++h) // class1
        {

            for(int l = h + 1; l < DEFS.numOfClasses(); ++l) // class2
            {
                QColor color1 = QColor(inColors[h]);
                QColor color2 = QColor(inColors[l]);

                for(int j = offset; j < offset + DEFS.spLength(); ++j)
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

                    paint.drawRect(X + (j % DEFS.spLength() - barWidth) / ext,
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
    QPainter pnt;
    pnt.begin(&pic);

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
                    pnt.setPen(color1);
                    pnt.setBrush(QBrush(color1));
                }
                else
                {
                    pnt.setPen(color2);
                    pnt.setBrush(QBrush(color2));
                }
                pnt.drawLine(i,
                             currTop,
                             i,
                             currBot);
            }

            ++num;
        }
    }
    pic.save(picPath, 0, 100);
    pnt.end();

}

void drawColorScale(QString filePath, int range, ColorScale type, bool full)
{

    //    double sigmaR = range*0.35;
    //    double sigmaG = range*0.25;
    //    double sigmaB = range*0.4;

    //    double offR = range*(1.0 - 0.3);
    //    double offG = range*(0.5 + 0.25);
    //    double offB = range*(0.0 + 0.15);

    // type == 0 - hueJet
    // type == 1 - hueOld
    // type == 2 - 3gauss
    // type == 3 - grayScale
    QPixmap pic;
    if(full)
    {
        pic = QPixmap(744, 520);
    }
    else
    {
        pic = QPixmap(20, 300);
    }
    pic.fill();
    QPainter painter;
    painter.begin(&pic);


    for(int i = 0; i < range; ++i)
    {
        switch(type)
        {
		case ColorScale::jet:
        {
			painter.setBrush(QBrush(myLib::hueJet(range, i)));
			painter.setPen(myLib::hueJet(range, i));
            break;
        }
		case ColorScale::htc:
        {
			painter.setBrush(QBrush(myLib::hueOld(range, i)));
			painter.setPen(myLib::hueOld(range, i));
            break;
        }
		case ColorScale::gray:
        {
			painter.setBrush(QBrush(myLib::grayScale(range, i)));
			painter.setPen(myLib::grayScale(range, i));
            break;
        }
		case ColorScale::pew:
        {
			painter.setBrush(QBrush(myLib::grayScale(range, i)));
			painter.setPen(myLib::grayScale(range, i));
            break;
        }
        }

        if(full)
        {
            painter.drawRect(i*pic.width()/double(range),
                             0,
                             (i+1)*pic.width()/double(range),
                             pic.height() * 0.07);
        }
        else
        {
            painter.drawRect(0,
                             (1. - (i+1)/double(range)) * pic.height(),
                             pic.width(),
                             (1./double(range)) * pic.height()); // vertical
        }

    }
    if(full)
    {
        for(int i = 0; i < range; ++i)
        {
            switch(type)
            {
			case ColorScale::jet:
            {
                painter.setPen(QPen(QBrush("red"), 2));
                painter.drawLine(i*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * red(range, i),
                                 (i+1)*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * red(range, int(i+1)));

                painter.setPen(QPen(QBrush("green"), 2));
                painter.drawLine(i*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * green(range, i),
                                 (i+1)*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * green(range, int(i+1)));

                painter.setPen(QPen(QBrush("blue"), 2));
                painter.drawLine(i*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * blue(range, i),
                                 (i+1)*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * blue(range, int(i+1)));
                break;
            }
			case ColorScale::htc:
            {
                painter.setPen(QPen(QBrush("red"), 2));
                painter.drawLine(i*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * red1(range, i),
                                 (i+1)*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * red1(range, int(i+1)));

                painter.setPen(QPen(QBrush("green"), 2));
                painter.drawLine(i*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * green1(range, i),
                                 (i+1)*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * green1(range, int(i+1)));

                painter.setPen(QPen(QBrush("blue"), 2));
                painter.drawLine(i*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * blue1(range, i),
                                 (i+1)*pic.width()/double(range),
                                 pic.height()*0.95 - (pic.height() * 0.85) * blue1(range, int(i+1)));
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
    pic.save(filePath, 0, 100);

}







void drawRealisation(const QString & inPath)
{
	matrix inData = readPlainData(inPath);

	QString outPath = inPath;
	outPath.resize(outPath.lastIndexOf('.'));
	outPath += ".jpg";

	drawEeg(inData,
			inData.rows(),
			inData.cols(),
			DEFS.getFreq(),
			outPath);
}

QPixmap drawEeg(const matrix & dataD,
                int ns,
                int NumOfSlices,
                int freq,
                const QString & picPath,
                double norm,
                int blueChan,
                int redChan)
{
    QPixmap pic = QPixmap(NumOfSlices, 600);
    pic.fill();

    QPainter paint;
    paint.begin(&pic);

    QString colour;
    int lineWidth = 2;


    for(int c2 = 0; c2 < ns; ++c2)
    {
        if(c2 == blueChan)
        {
            colour = "blue";
        }
        else if(c2 == redChan)
        {
            colour = "red";
        }
        else
        {
            colour = "black";
        }

        paint.setPen(QPen(QBrush(QColor(colour)), lineWidth));
#if 1
        for(int c1 = 0; c1 < pic.width() - 1; ++c1)
        {
            paint.drawLine(c1, (c2+1) * pic.height() / (ns+2) + dataD[c2][c1] * norm,
                           c1+1, (c2+1) * pic.height() / (ns+2) + dataD[c2][c1+1] * norm);
        }
#else
        // same speed
        int c1 = 0;
        auto it = std::begin(dataD[c2]) + 1;
        for(auto itt = std::begin(dataD[c2]); itt != std::end(dataD[c2]) - 1; ++itt, ++it, ++c1)
        {
            paint.drawLine(c1    , (c2 + 1) * pic.height() / (ns + 2) + *itt * norm,
                           c1 + 1, (c2 + 1) * pic.height() / (ns + 2) + *it  * norm);
        }
#endif
    }
    norm = 1.;
    paint.setPen(QPen(QBrush("black"), lineWidth));
    for(int c3 = 0; c3 < NumOfSlices * 10 / 250; ++c3)
    {
        if(c3%10 == 0) norm = 20.;
        else if(c3%5  == 0) norm = 15.;

		paint.drawLine(c3 * freq/5, pic.height() - 2, c3 * freq/5, pic.height() - 2*norm);
		paint.drawText(c3 * freq, pic.height() - 35, nm(c3));
        norm = 10.;
    }
    pic.save(picPath, 0, 100);

    paint.end();

    return pic;
}

QPixmap drawEeg(const matrix & dataD,
                 int ns,
                 int startSlice,
                 int endSlice,
                 int freq,
                 const QString & picPath,
                 double norm,
                 int blueChan,
                 int redChan)
{
    int NumOfSlices = endSlice - startSlice;
    QPixmap pic = QPixmap(NumOfSlices, ns * 30); /// general size
    pic.fill();

    QPainter paint;
    paint.begin(&pic);

    QString colour;
    int lineWidth = 2;


    for(int c2 = 0; c2 < ns; ++c2)
    {
        if(c2 == blueChan)
        {
            colour = "blue";
        }
        else if(c2 == redChan)
        {
            colour = "red";
        }
        else
        {
            colour = "black";
        }

        paint.setPen(QPen(QBrush(QColor(colour)), lineWidth));

        for(int c1 = 0; c1 < pic.width(); ++c1)
        {
            paint.drawLine(c1,
                           (c2+1) * pic.height() / (ns + 2) + dataD[c2][c1 + startSlice] * norm,
                    c1 + 1,
                    (c2 + 1) * pic.height() / (ns + 2) + dataD[c2][c1 + startSlice + 1] * norm);
        }
    }
    norm = 1.;
    paint.setPen(QPen(QBrush("black"), lineWidth));
	for(int c3 = 0; c3 < NumOfSlices * 10 / freq; ++c3)
    {
        if(c3%10 == 0) norm = 20.;
        else if(c3%5  == 0) norm = 15.;

		paint.drawLine(c3 * freq/5, pic.height() - 2, c3 * freq/5, pic.height() - 2 * norm);
		paint.drawText(c3 * freq, pic.height() - 35, nm(c3));
        norm = 10.;
    }
    pic.save(picPath, 0, 100);
    paint.end();
    return pic;
}




} // namespace myLib
