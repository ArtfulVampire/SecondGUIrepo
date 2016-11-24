#include "library.h"


using namespace std;
using namespace std::chrono;
using namespace smallLib;

namespace wvlt
{
void drawWavelet(QString picPath,
                 const matrix & inData)
{
    int NumOfSlices = ceil(inData.cols() * wvlt::timeStep);
    QPixmap pic(NumOfSlices, 1000);
    pic.fill();
    QPainter painter;
    painter.begin(&pic);

	double numb = 0.;

	int currFreqNum = 0;
    int currSliceNum = 0;

	for(double freq = wvlt::freqMax;
	   freq > wvlt::freqMin;
	#if WAVELET_FREQ_STEP_TYPE==0
	   freq *= wvlt::freqStep
    #else

	   freq -= wvlt::freqStep)
#endif
    {
        currSliceNum = 0;
        for(int currSlice = 0; currSlice < NumOfSlices; currSlice += wvlt::timeStep)
        {
			numb = fmin(floor(inData[currFreqNum][currSliceNum] * wvlt::range), wvlt::range);

			// sligthly more than numb, may be dropped
			numb = pow(numb/wvlt::range, 0.6) * wvlt::range;

            painter.setBrush(QBrush(myLib::hueJet(wvlt::range, numb)));
            painter.setPen(myLib::hueJet(wvlt::range, numb));

#if WAVELET_FREQ_STEP_TYPE==0
            painter.drawRect( currSlice * pic.width() / NumOfSlices,
							  pic.height() * (wvlt::freqMax - freq
											  + 0.5 * freq *
											  (1. - wvlt::freqStep) / wvlt::freqStep)
							  / (wvlt::freqMax-wvlt::freqMin),
                              pic.width() * wvlt::timeStep / NumOfSlices,
							  pic.height() * -0.5 * freq * (1. / wvlt::freqStep - wvlt::freqStep)
							  / (wvlt::freqMax - wvlt::freqMin) );
#else
            painter.drawRect( currSlice * pic.width() / NumOfSlices,
							  pic.height() * (wvlt::freqMax - freq  - 0.5 * wvlt::freqStep)
							  / (wvlt::freqMax - wvlt::freqMin),
                              pic.width() * wvlt::timeStep / NumOfSlices,
							  pic.height() * wvlt::freqStep / (wvlt::freqMax - wvlt::freqMin));
#endif
            ++currSliceNum;
        }
		++currFreqNum;

    }
    painter.setPen("black");


    painter.setFont(QFont("Helvetica", 28, -1, -1));
    painter.setPen(Qt::DashLine);
	for(int i = wvlt::freqMax; i > wvlt::freqMin; --i)
    {

        painter.drawLine(0,
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin),
                         pic.width(),
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin));
        painter.drawText(0,
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin) - 2,
                         QString::number(i));

    }
    painter.setPen(Qt::SolidLine);
    for(int i = 0; i < int(NumOfSlices / def::freq); ++i)
    {
        painter.drawLine(pic.width() * i * def::freq / NumOfSlices,
                         pic.height(),
                         pic.width() * i * def::freq / NumOfSlices,
                         pic.height() - 20);
        painter.drawText(pic.width() * i * def::freq / NumOfSlices - 8,
                         pic.height() - 2,
                         QString::number(i));

    }
    pic.save(picPath, 0, 100);
    painter.end();
}
}

namespace myLib
{

//hot-to-cold, http://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
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

//jet
double red(const int & range, double j, double V, double S)
{
    double part = j / double(range);
    // matlab
    if    (0. <= part && part <= colDots[0]) return V*(1.-S);
    else if(colDots[0] < part && part <= colDots[1]) return V*(1.-S);
    else if(colDots[1] < part && part <= colDots[2]) return V*(1.-S) + V*S*(part-colDots[1])/(colDots[2] - colDots[1]);
    else if(colDots[2] < part && part <= colDots[3]) return V;
    else if(colDots[3] < part && part <= 1.) return V - V*S*(part-colDots[3])/(1 - colDots[3])/2.;
    // old
    if    (0.000 <= part && part <= 0.167) return V*(1.-S); ///2. - V*S/2. + V*S*(part)*3.;
    else if(0.167 < part && part <= 0.400) return V*(1.-S);
    else if(0.400 < part && part <= 0.500) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.500 < part && part <= 0.600) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.600 < part && part <= 0.833) return V;
    else if(0.833 < part && part <= 1.000) return V - V*S*(part-0.833)/(1.000-0.833)/2.;
    else return 0.0;
}
double green(const int & range, double j, double V, double S)
{
    double part = j / double(range);
    double hlp = 1.0;
    // matlab
    if    (0.0 <= part && part <= colDots[0]) return V*(1.-S);
    else if(colDots[0] < part && part <= colDots[1]) return V*(1.-S) + V*S*(part-colDots[0])/(colDots[1] - colDots[0]);
    else if(colDots[1] < part && part <= colDots[2]) return V;
    else if(colDots[2] < part && part <= colDots[3]) return V - V*S*(part-colDots[2])/(colDots[3] - colDots[2]);
    else if(colDots[3] < part && part <= 1.) return V*(1.-S);
    // old
    if    (0.000 <= part && part <= 0.167) return V*(1.-S);
    else if(0.167 < part && part <= 0.400) return V*(1.-S) + V*S*hlp*(part-0.167)/(0.400-0.167);
    else if(0.400 < part && part <= 0.500) return V-V*S*(1.-hlp);
    else if(0.500 < part && part <= 0.600) return V-V*S*(1.-hlp);
    else if(0.600 < part && part <= 0.833) return V-V*S*(1.-hlp) - V*S*hlp*(part-0.600)/(0.833-0.600);
    else if(0.833 < part && part <= 1.000) return V*(1.-S);
    else return 0.0;
}

double blue(const int & range, double j, double V, double S)
{
    double part = j / double(range);

    if    (0.0 <= part && part <= colDots[0]) return V -V*S/2. + V*S*(part)/(colDots[0] - 0.0)/2.;
    else if(colDots[0] < part && part <= colDots[1]) return V;
    else if(colDots[1] < part && part <= colDots[2]) return V - V*S*(part-colDots[1])/(colDots[2] - colDots[1]);
    else if(colDots[2] < part && part <= colDots[3]) return V*(1.-S);
    else if(colDots[3] < part && part <= 1.) return V*(1.-S);

    // old
    if    (0.000 <= part && part <= 0.167) return V -V*S/2. + V*S*(part)/(0.167-0.000)/2.;
    else if(0.167 < part && part <= 0.400) return V;
    else if(0.400 < part && part <= 0.500) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.500 < part && part <= 0.600) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.600 < part && part <= 0.833) return V*(1.-S);
    else if(0.833 < part && part <= 1.000) return V*(1.-S);


    else return 0.0;
}

// hot to cold
QColor hueOld(int range, double j, int numOfContours, double V, double S)
{
    if(j > range) j = range; //bicycle for no black colour
    if(j < 0) j = 0; //bicycle for no black colour

    for(int i = 1; i < numOfContours + 1; ++i)
    {
        if(fabs(j/range - double(i)/(numOfContours + 1)) < 0.003)
        {
            return QColor("black");
        }
    }
    return QColor(255.*red1(range,j), 255.*green1(range,j), 255.*blue1(range,j));
}

QColor hueJet(const int & range, double j)
{
    if(j > range) j = range; //bicycle for no black colour
    if(j < 0) j = 0; //bicycle for no black colour
    //    return QColor(255.*red1(range,j), 255.*green1(range,j), 255.*blue1(range,j));
    return QColor(255. * red(range, j),
                  255. * green(range, j),
                  255. * blue(range, j));
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

QColor mapColor(double minMagn,
                double maxMagn,
                const matrix & helpMatrix,
                int numX, int numY,
                double partX, double partY,
                bool colour)
{
    double a[4];
    a[0] = helpMatrix[numY][numX];
    a[1] = helpMatrix[numY][numX+1];
    a[2] = helpMatrix[numY+1][numX];
    a[3] = helpMatrix[numY+1][numX+1];


    double val = 0.;

    //mean harmonic
    if(0)
    {
        val = 0.;
        val += a[0] / (partX * partX + partY * partY) ;
        val += a[1] / ( (1. - partX) * (1. - partX) + partY * partY ) ;
        val += a[2] / ( partX * partX + (1. - partY) * (1. - partY) );
        val += a[3] / ( (1. - partX) * (1. - partX) + (1. - partY) * (1. - partY) );

        val /=    1. / (partX * partX + partY * partY)
                  + 1. / ( (1. - partX) * (1. - partX) + partY * partY )
                  + 1. / ( partX * partX + (1. - partY) * (1. - partY) )
                  + 1. / ( (1. - partX) * (1. - partX) + (1. - partY) * (1. - partY) );
    }

    //bilinear approximation - OK
    if(0)
    {
        val = 0.;
        val += a[3] * ( partX * partY );
        val += a[2] * ( (1. - partX) * partY );
        val += a[1] * ( partX * (1. - partY) );
        val += a[0] * ( (1. - partX) * (1. - partY) );
    }

    //gaussian approximation
    if(0)
    {
        double sigma = 0.45;
        val = 0;
        double deltaX;
        double deltaY;
        double probeX = numX + partX;
        double probeY = numY + partY;
        for(int i = 0; i < 25; ++i) //25 the number of values in helpMatrix
        {
            deltaX = (i%5) - probeX;
            deltaY = (i/5) - probeY;
            val += helpMatrix[i/5][i%5] * gaussian(sqrt(deltaX*deltaX + deltaY*deltaY), sigma);
        }
    }
    if(!colour)
    {
        return grayScale(256, ((val - minMagn) / (maxMagn - minMagn))*256.);
    }
    else
    {
        return hueJet(256, ((val - minMagn) / (maxMagn - minMagn))*256.);
    }
}




void drawMap(const matrix & matrixA,
             double maxAbs,
             QString outDir,
             QString outName,
             int num,
             int size,
             bool colourFlag)
{
    QPixmap pic = QPixmap(size, size);
    QPainter painter;
    pic.fill();
    painter.begin(&pic);

    matrix helpMatrix(5, 5);

    int currIndex = 0.;
    for(int i = 0; i < 25; ++i) //generality for ns = 19
    {
        if(i == 0 || i == 2 || i == 4 || i == 20 || i == 22 || i == 24)  //generality for ns = 19
        {
            helpMatrix[i/5][i%5] = 0.;
        }
        else
        {
            helpMatrix[i/5][i%5] = matrixA[currIndex++][num]; //here was fabs()
        }
    }

    //approximation for square
    helpMatrix[0][0] = (helpMatrix[0][1] + helpMatrix[1][0] + helpMatrix[1][1])/3.;
    helpMatrix[0][2] = (helpMatrix[0][1] + helpMatrix[1][1] + helpMatrix[1][2] + helpMatrix[1][2] + helpMatrix[1][3] + helpMatrix[0][3])/6.;
    helpMatrix[0][4] = (helpMatrix[0][3] + helpMatrix[1][3] + helpMatrix[1][4])/3.;;
    helpMatrix[4][0] = (helpMatrix[3][0] + helpMatrix[3][1] + helpMatrix[4][1])/3.;
    helpMatrix[4][2] = (helpMatrix[4][1] + helpMatrix[3][1] + helpMatrix[3][2] + helpMatrix[3][2] + helpMatrix[3][3] + helpMatrix[4][3])/6.;
    helpMatrix[4][4] = (helpMatrix[4][3] + helpMatrix[3][3] + helpMatrix[3][4])/3.;

    int numX, numY;
    double leftCoeff = 0.0; //gap from left
    double rightCoeff = 4.0; // gap from right
    double scale1 = size/(leftCoeff + rightCoeff);

    //generality 5 -> ns=19
    //usual approximations
    for(int x = floor(scale1) * leftCoeff; x < floor(scale1) * rightCoeff; ++x)
    {
        for(int y = floor(scale1) * leftCoeff; y < floor(scale1) * rightCoeff; ++y)
        {
            if(myLib::distance(x, y, size/2, size/2) > size/2 ) continue; // make it round

            numX = floor(x/int(scale1)) ; //1 2
            numY = floor(y/int(scale1)) ; //3 4

            painter.setPen(mapColor(-maxAbs, maxAbs, helpMatrix, numX, numY, double(double(x%int(scale1))/scale1 + 0.003/scale1), double(double(y%int(scale1))/scale1) + 0.003/scale1, colourFlag)); // why 0.003
            painter.drawPoint(x,y);
        }
    }


    QString helpString = outDir + slash + outName + "_map_" + QString::number(num) + ".png";
    pic.save(helpString, 0, 100);
}

void drawMapsICA(const QString &mapsFilePath,
                 const QString &outDir,
                 const QString &outName,
                 const ColorScale colourTheme,
                 void (*draw1MapFunc)(const matrix &,
                                      const int,
                                      const QString &,
                                      const QString &,
                                      const double &,
                                      const int,
                                      const ColorScale))
{
    matrix matrixA(def::nsWOM(), def::nsWOM());
    readMatrixFile(QString(mapsFilePath).remove("_ica"), matrixA);
    double maxAbs = matrixA.maxAbsVal();

    for(int i = 0; i < def::nsWOM(); ++i)
    {
        draw1MapFunc(matrixA, i, outDir, QString(outName).remove("_ica"), maxAbs, 240, colourTheme);
    }
}


void drawMapsOnSpectra(const QString &inSpectraFilePath,
                       const QString &outSpectraFilePath,
                       const QString &mapsDirPath,
                       const QString &mapsNames)
{
//    cout << def::ExpName << endl;
//    cout << inSpectraFilePath << endl;
//    cout << outSpectraFilePath << endl;
//    cout << mapsDirPath << endl;
//    cout << mapsNames << endl;

    QPixmap pic;
    pic = QPixmap(inSpectraFilePath);
    QPainter paint;
    paint.begin(&pic);

    QPixmap pic1;
    QString helpString;
    QRect earRect;

    const double offsetX = 0.7;
    const int earSize = 8; //generality
    const double shitCoeff = 1.05; //smth about width of map on spectra pic

    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;

    const QString mapNam = QString(mapsNames).remove("_ica");

    for(int i = 0; i < def::nsWOM(); ++i)
    {
        helpString = mapsDirPath
                     + slash + mapNam
                     + "_map_" + QString::number(i) + "+.png";
        if(!QFile::exists(helpString))
        {
            helpString.replace("+.png", "-.png");
            if(!QFile::exists(helpString))
            {
                cout << "drawMapsOnSpectra: no map file found " << helpString.toStdString() << endl;
                return;
            }
        }
//        cout << 1 << endl;
        pic1 = QPixmap(helpString);
//        cout << 2 << endl;



        const double Y = paint.device()->height() * coords::y[i];
        const double X = paint.device()->width() * coords::x[i];

        paint.drawPixmap(QRect(X + offsetX * graphWidth,
                               Y - graphHeight,
                               (shitCoeff - offsetX) * graphWidth,
                               (shitCoeff - offsetX) * graphHeight),
                         pic1);

        paint.setPen(QPen(QBrush("black"), 2));

        //draw the nose
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
                   const double & maxAbs,
                   const int picSize,
                   const ColorScale colorTheme)
{
    QPixmap pic = QPixmap(picSize, picSize);
    QPainter painter;
    pic.fill();
    painter.begin(&pic);
    QString savePath1 = outDir
                        + slash + outName + "_map_"
                        + QString::number(numOfColoumn) + "+.png";

    QPixmap pic1 = QPixmap(picSize, picSize);
    QPainter painter1;
    pic1.fill();
    painter1.begin(&pic1);
    QString savePath2 = outDir
                        + slash + outName + "_map_"
                        + QString::number(numOfColoumn) + "-.png";

    double val;
    double drawArg;
    int drawRange = 256;

    int dim = 7;
    double scale1 = double(dim-1)/picSize;

    matrix helpMatrix(dim, dim, 0.); //generality for ns = 19

    int currIndex = 0.;
    for(int i = 0; i < dim * dim; ++i)
    {
        const int rest = i % dim;
        const int quot = i / dim;

        if(quot % (dim - 1) == 0|| rest % (dim-1) == 0)  //set 0 to all edge values
        {
            helpMatrix[quot][rest] = 0.;
        }
        else if(quot == 1
                && (rest - 1) * (rest - 3) * (rest - 5) == 0) //Fp3, Fpz, Fp4
        {
            helpMatrix[quot][rest] = 0.;
        }
        else if(quot == 5
                && (rest - 1) * (rest - 3) * (rest - 5) == 0) //O3, Oz, O4
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
    //approximation for square - Fp3, Fpz, Fp, O3, Oz, O4
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
    lineType inX(dim);
    lineType inY(dim);
    lineType inYv(dim);
    lineType Av(dim - 1);
    lineType Bv(dim - 1);

    std::iota(std::begin(inX), std::end(inX), 0); //hope, it's constant

    for(int i = 1; i < dim - 1; ++i) // number of helpMatrix row
    {
        inY = helpMatrix[i];
        splineCoeffCount(inX, inY, dim, Ah[i - 1], Bh[i - 1]); // horizontal splines coeffs
    }

    for(int x = 0; x < picSize; ++x)
    {
        for(int k = 1; k < dim - 1; ++k)
        {
            inY = helpMatrix[k]; //set inX and inY for k'th row of helpMatrix
            inYv[k] = splineOutput(inX, inY, dim, Ah[k - 1], Bh[k - 1], x * scale1);
        }
        inYv[0] = 0.;
        inYv[dim - 1] = 0.;
        splineCoeffCount(inX, inYv, dim, Av, Bv);
        for(int y = 0; y < picSize; ++y)
        {
            /// round shape
            if(myLib::distance(x, y, picSize/2, picSize/2) >
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
            case jet:
            {
                painter.setBrush(QBrush(hueJet(drawRange, drawArg)));
                painter.setPen(hueJet(drawRange, drawArg));
                painter1.setBrush(QBrush(hueJet(drawRange, drawRange - drawArg)));
                painter1.setPen(hueJet(drawRange, drawRange - drawArg));
                break;
            }
            case htc:
            {
                painter.setBrush(QBrush(hueOld(drawRange, drawArg)));
                painter.setPen(hueOld(drawRange, drawArg));
                painter1.setBrush(QBrush(hueOld(drawRange, drawRange - drawArg)));
                painter1.setPen(hueOld(drawRange, drawRange - drawArg));
                break;
            }
            case gray:
            {
                painter.setBrush(QBrush(grayScale(drawRange, drawArg)));
                painter.setPen(grayScale(drawRange, drawArg));
                painter1.setBrush(QBrush(grayScale(drawRange, drawRange - drawArg)));
                painter1.setPen(grayScale(drawRange, drawRange - drawArg));
                break;
            }
            case pew:
            {
                painter.setBrush(QBrush(grayScale(drawRange, drawArg)));
                painter.setPen(grayScale(drawRange, drawArg));
                painter1.setBrush(QBrush(grayScale(drawRange, drawRange - drawArg)));
                painter1.setPen(grayScale(drawRange, drawRange - drawArg));
                break;
            }
            }
            painter.drawPoint(x,y);
            painter1.drawPoint(x,y);
        }
    }

    if(1) //draw channels locations
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
    //+- solver
    if(fabs(maxMagn) > 1.5 * fabs(minMagn))
    {
        QFile::remove(savePath2);
    }
    else if(1.5 * fabs(maxMagn) < fabs(minMagn))
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

	//draw axes
	paint.setPen("black");
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X,
						   Y - graphHeight));
	paint.drawLine(QPointF(X,
						   Y),
				   QPointF(X + graphWidth,
						   Y));

	//draw Herzes
	const double hzFontSize = fontSize / 3;
	paint.setFont(QFont("Helvitica", int(hzFontSize)));

	const double unit = (rightF - leftF) / graphWidth;
	auto currFreq = [unit, leftF](int in) -> double {return leftF + in * unit;};

	for(int k = 0; k < graphWidth + 1; ++k)
	{
		const double cF = currFreq(k);
		if( std::abs(cF - std::round(cF)) <= unit / 2. )
		{
			paint.drawLine(QPointF(X + k,
								   Y),
						   QPointF(X + k,
								   Y + 5)); /// magic const

			helpString = QString::number(std::round(cF));

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
						 + "(" + QString::number(chanNum + 1) + ")" // can be commented
						 ;
		}
		else
		{
			helpString = QString::number(chanNum + 1);

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

    if(outPath.contains("_ica") || def::ExpName.contains("_ica"))
    {
        channelsFlag = false; // numbers for ICAs
    }





    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;
    const double graphScale = def::spLength() / graphWidth;
    // some empirical values prepared for defauls 1600*1600
    const double scaleY = paint.device()->height() / 1600.;
    const double scaleX = paint.device()->width() / 1600.;


	const double unit = graphScale * def::spStep();
	auto currFreq = [unit](int in) -> double
	{
		return def::left() * def::spStep() + in * unit;
	};
    for(int c2 = 0; c2 < numOfChan; ++c2)  //exept markers channel
    {
		/// replace with drawOneTemplate

        const double Y = paint.device()->height() * coords::y[c2];
        const double X = paint.device()->width() * coords::x[c2];

        //draw axes
        paint.setPen("black");
        paint.drawLine(QPointF(X,
                               Y),
                       QPointF(X,
                               Y - graphHeight + 28)); // 28 for font
        paint.drawLine(QPointF(X,
                               Y),
                       QPointF(X + graphWidth,
                               Y));

        //draw Herzes
        paint.setFont(QFont("Helvitica", int(12 * scaleY)));
        for(int k = 0; k < graphWidth; ++k) //for every Hz generality
        {

			const double cF = currFreq(k);
			if( abs(cF - std::round(cF)) <= unit / 2. )
            {
                paint.drawLine(QPointF(X + k,
                                       Y),
                               QPointF(X + k,
                                       Y + 5 * scaleY));

                helpString = QString::number(int((def::left() + k * graphScale) * def::spStep() + 0.5));

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
                         + "(" + QString::number(c2 + 1) + ")" // can be commented
                         ;
        }
        else
        {
            helpString = QString::number(c2 + 1);

        }
        paint.drawText(QPointF(X - 16 * scaleX,
                               Y - graphHeight + 24 * scaleY),
                       helpString);

    }
    //write channels labels

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
               const double & scaling,
               const int & lineWidth)
{
    QSvgGenerator svgGen;
    QSvgRenderer svgRen;
    QPixmap pic;
    QPainter paint;
    QString helpString;
    int numOfChan = inData.rows();
    if(numOfChan > 21)
    {
        return;
    }

    if(templPath.contains(".svg"))
    {
        return;
        //// TO FIX
        ///

#if 0
        svgRen = QSvgRenderer(templPath);
        svgGen.setSize(QSize(width, height));
        svgGen.setViewBox(QRect(QPoint(0,0), svgGen.size()));
        svgGen.setFileName(outPath);
        paint.begin(&svgGen);
        paint.setBrush(QBrush("white"));
        paint.drawRect(QRect(QPoint(0,0), svgGen.size()));
#endif


    }
    else if(templPath.contains(".jpg") || templPath.contains(".png"))
    {
        pic.load(templPath);
        paint.begin(&pic);
    }

//    if(inData.size() != numOfChan * def::spLength())
//    {
//        cout << "drawArray: inappropriate array size" << endl;
//        return;
//    }

    double norm = inData.maxVal();

#if 0
    // for weights
    norm /= 2;
#endif


    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;

//    const double graphScale = def::spLength() / graphWidth;
    const double graphScale = inData.cols() / graphWidth;

    // initial fonts prepared for 1600*1600
    const double scaleY = paint.device()->height() / 1600.;
    const double scaleX = paint.device()->width() / 1600.;

    norm = graphHeight / norm ; //250 - pixels per graph, generality
    norm *= scaling;

    for(int c2 = 0; c2 < numOfChan; ++c2)  //exept markers channel
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

        //draw spectra
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

    //returning norm = max magnitude
    norm /= scaling;
    norm = graphHeight / norm;
    norm /= scaling;  //scaling generality
    norm = doubleRound(norm,
                       min(1., 2 - floor(log10(norm)) )
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

/// overload matrix/lineType
void drawArray(const QString & templPath,
               const lineType & inData,
               const QString & color,
               const double & scaling,
               const int & lineWidth)
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
						const lineType &inData,
						const lineType &inSigma,
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
		cout << "data and sigma different length" << endl;
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

	norm = graphHeight / norm ; //250 - pixels per graph, generality

	const auto lowLine = inData - inSigma;
	const auto highLine = inData + inSigma;


	for(auto drawLine : std::vector<lineType>{inData, lowLine, highLine})
	{
		for(int c2 = 0; c2 < BaklushevChans; ++c2)  //exept markers channel
		{
			auto index = [=](int in) -> int
			{
				return c2 * spL + in * graphScale;
			};

			auto index2 = [=](int in) -> int
			{
				return c2 * spL + in;
			};

			const double Y = paint.device()->height() * coords::y[c2];
			const double X = paint.device()->width() * coords::x[c2];



			//draw spectra
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
		auto index = [=](int in) -> int
		{
			return c2 * spL + in * graphScale;
		};

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



	//returning norm = max magnitude
	norm = graphHeight / norm;
	norm = doubleRound(norm,
					   min(1., 2 - floor(log10(norm)) )
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
				  const spectraGraphsNormalization normType,
				  double norm,
				  const std::vector<QColor> & colors,
				  const double scaling,
				  const int lineWidth)
{
	QPixmap pic = templPixmap;
	QPainter paint;
	paint.begin(&pic);

	QString helpString;
	int numOfChan = inMatrix.cols() / def::spLength();

	// test size
	int shouldSize = numOfChan * def::spLength();

	std::for_each(inMatrix.begin(),
				  inMatrix.end(),
				  [shouldSize](lineType inData)
	{
		if(inData.size() > shouldSize)
		{
			cout << "drawArrays: inappropriate array size = " << inData.size() << endl;
			return;
		}
	});



	if(norm <= 0.)
	{
		std::for_each(inMatrix.begin(),
					  inMatrix.end(),
					  [&norm](lineType inData)
		{
			norm = max(norm, abs(inData).max()); // fabs for negative weights e.g.
		});

		if(weightsFlag)
		{
			norm *= 2;
		}
	}

	const double graphHeight = paint.device()->height() * coords::scale;
	const double graphWidth = paint.device()->width() * coords::scale;
	const double graphScale = def::spLength() / graphWidth;
	// initial fonts prepared for 1600*1600
	const double scaleY = paint.device()->height() / 1600.;
	const double scaleX = paint.device()->width() / 1600.;

	norm = graphHeight / norm ; //250 - pixels per graph, generality
	norm *= scaling;

	const double normBC = norm;

	for(int c2 = 0; c2 < BaklushevChans; ++c2)  //exept markers channel
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
		if(normType == spectraGraphsNormalization::each)
		{
			// norm each channel by max peak
			norm = 0;
			for(uint i = 0; i < inMatrix.size(); ++i)
			{
				for(int j = 0; j < def::spLength(); ++j)
				{
					norm = fmax(norm,
								fabs(inMatrix[i][def::spLength() * c2 + j])
						   * (1. + (j > 0.7 * def::spLength()) * 0.7) );
				}
			}

			paint.setPen("black");
			paint.setFont(QFont("Helvetica", int(20 * scaleY)));

							   #if 0
			paint.drawText(QPointF(X + graphWidth * 0.4,
								   Y - graphHeight - 5 * scaleY),
						   QString::number(doubleRound(norm)));
							   #endif

			// make drawNorm
			norm = graphHeight / norm ; //250 - pixels per graph, generality
			norm *= scaling;
		}


		const int offset = c2 * def::spLength();
		for(uint numVec = 0; numVec < inMatrix.size(); ++numVec)
		{
			const lineType & inData = inMatrix[numVec];
			//draw spectra
//			for(int k = 0; k < graphWidth - 1; ++k)
			for(int k = 0; k < def::spLength() - 1; ++k)
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
	norm /= scaling;  //scaling generality
	norm = doubleRound(norm,
					   min(1., 2 - floor(log10(norm)) )
					   );
	if(normType == spectraGraphsNormalization::all)
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
                  const spectraGraphsNormalization normType,
                  double norm,
				  const std::vector<QColor> & colors,
                  const double scaling,
                  const int lineWidth)
{
    QSvgGenerator svgGen;
    QSvgRenderer svgRen;
    QPixmap pic;
    QPainter paint;
    QString helpString;
    int numOfChan = inMatrix.cols() / def::spLength();
//	cout << inMatrix.cols() << "\t" << def::spLength() << "\t" << numOfChan << endl;
//	exit(0);

    if(templPath.contains(".svg"))
    {
        cout << "will do nothing, look into library.h" << endl;
        return -1;
        //// TO FIX ///

#if 0
        svgRen = QSvgRenderer(templPath);
        svgGen.setSize(QSize(width, height));
        svgGen.setViewBox(QRect(QPoint(0,0), svgGen.size()));
        svgGen.setFileName(outPath);
        paint.begin(&svgGen);
        paint.setBrush(QBrush("white"));
        paint.drawRect(QRect(QPoint(0,0), svgGen.size()));
#endif


    }
    else if(templPath.contains(".jpg") || templPath.contains(".png"))
    {
        pic.load(templPath);
        paint.begin(&pic);
    }

    // test size
    int shouldSize = numOfChan * def::spLength();

    std::for_each(inMatrix.begin(),
                  inMatrix.end(),
                  [shouldSize](lineType inData)
    {
        if(inData.size() > shouldSize)
        {
            cout << "drawArrays: inappropriate array size = " << inData.size() << endl;
            return;
        }
    });

    if(norm <= 0.)
    {
        std::for_each(inMatrix.begin(),
                      inMatrix.end(),
                      [&norm](lineType inData)
        {
            norm = max(norm, abs(inData).max()); // fabs for negative weights e.g.
        });

        if(weightsFlag)
        {
            // for weights
            norm *= 2;
        }
    }
    const double res = norm;

    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;
    const double graphScale = def::spLength() / graphWidth;
    // initial fonts prepared for 1600*1600
    const double scaleY = paint.device()->height() / 1600.;
    const double scaleX = paint.device()->width() / 1600.;

    norm = graphHeight / norm ; //250 - pixels per graph, generality
    norm *= scaling;

    const double normBC = norm;

    for(int c2 = 0; c2 < numOfChan; ++c2)  //exept markers channel
    {
        const double Y = paint.device()->height() * coords::y[c2];
        const double X = paint.device()->width() * coords::x[c2];
        const int offset = c2 * def::spLength();

        if(weightsFlag)
        {
            // for weigths
            paint.setPen(QPen(QBrush("black"), 1));
            paint.drawLine(QPointF(X, Y - graphHeight / 2),
                           QPointF(X + graphWidth, Y - graphHeight / 2));
        }

        norm = normBC;
        if(normType == spectraGraphsNormalization::each)
        {
            // norm each channel by max peak
            norm = 0;
            for(uint i = 0; i < inMatrix.size(); ++i)
            {
                for(int j = 0; j < def::spLength(); ++j)
                {
                    norm = fmax(norm,
                                fabs(inMatrix[i][def::spLength() * c2 + j])
                           * (1. + (j > 0.7 * def::spLength()) * 0.7) );
                }
            }

            paint.setPen("black");
            paint.setFont(QFont("Helvetica", int(20 * scaleY)));

                               #if 0
            paint.drawText(QPointF(X + graphWidth * 0.4,
                                   Y - graphHeight - 5 * scaleY),
                           QString::number(doubleRound(norm)));
                               #endif

            // make drawNorm
            norm = graphHeight / norm ; //250 - pixels per graph, generality
            norm *= scaling;
        }


        for(uint numVec = 0; numVec < inMatrix.size(); ++numVec)
        {
            lineType inData = inMatrix[numVec];
            //draw spectra
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
    norm /= scaling;  //scaling generality
    norm = doubleRound(norm,
                       min(1., 2 - floor(log10(norm)) )
                       );
    if(normType == spectraGraphsNormalization::all)
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
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        pnt.drawLine(i * pic.width() / def::nsWOM(), offsetY * pic.height(),
                     i * pic.width() / def::nsWOM(), 0);
    }

    pic.save(picPath, 0, 100);
    pnt.end();

}



///////////////////////// should add scales and shit
void drawCutOneChannel(const QString & inSpectraPath,
                       const int numChan)
{
    QString outPath = inSpectraPath;
    outPath.replace(".", QString("_" + QString::number(numChan) + "."));

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
						const int & lineWidth)
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
					 const int & lineWidth)
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
    QSvgGenerator svgGen;
    QSvgRenderer svgRen;
    QPixmap pic;
    QPainter paint;

    if(templPath.contains(".svg"))
    {
        cout << "will do nothing, look into library.h" << endl;
        return;
        //// TO FIX ///

#if 0
        svgRen = QSvgRenderer(templPath);
        svgGen.setSize(QSize(width, height));
        svgGen.setViewBox(QRect(QPoint(0,0), svgGen.size()));
        svgGen.setFileName(outPath);
        paint.begin(&svgGen);
        paint.setBrush(QBrush("white"));
        paint.drawRect(QRect(QPoint(0,0), svgGen.size()));
#endif


    }
    else if(templPath.contains(".jpg") || templPath.contains(".png"))
    {
        pic.load(templPath);
        paint.begin(&pic);
    }

    const double barWidth = 1/2.;
    const int barHeight = 5; // pixels
    const int barHeightStart = 18; // pixels
    const int barHeightStep = 8; // pixels

    const double graphWidth = paint.device()->width() * coords::scale;
    const double ext = def::spLength() / graphWidth;

    for(int c2 = 0; c2 < def::nsWOM(); ++c2)  //exept markers channel
    {
        const double X = paint.device()->width() * coords::x[c2];
        const double Y = paint.device()->height() * coords::y[c2];
        const int offset = c2 * def::spLength();

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

    //type == 0 - hueJet
    //type == 1 - hueOld
    //type == 2 - 3gauss
    //type == 3 - grayScale
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
        case 0:
        {
            painter.setBrush(QBrush(hueJet(range, i)));
            painter.setPen(hueJet(range, i));
            break;
        }
        case 1:
        {
            painter.setBrush(QBrush(hueOld(range, i)));
            painter.setPen(hueOld(range, i));
            break;
        }
        case 2:
        {
            painter.setBrush(QBrush(grayScale(range, i)));
            painter.setPen(grayScale(range, i));
            break;
        }
        case 3:
        {
            painter.setBrush(QBrush(grayScale(range, i)));
            painter.setPen(grayScale(range, i));
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
            case 0:
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
            case 1:
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





template <typename signalType>
void histogram(const signalType & arr, int numSteps, QString picPath)
{
    vectType values(numSteps, 0.);
    int length = arr.size();

    QPixmap pic(1000, 400);
    QPainter pnt;
    pic.fill();
    pnt.begin(&pic);

    pnt.setPen("black");
    pnt.setBrush(QBrush("black"));

    double xMin, xMax;

    xMin = *std::min(arr.begin(),
                    arr.end());
    xMax = *std::max(arr.begin(),
                    arr.end());

    for(int j = 0; j < length; ++j)
    {
        values[ int(floor((arr[j] - xMin) / ((xMax-xMin) / numSteps))) ] += 1.;
    }
    double valueMax = *std::max(values.begin(),
                               values.end());

    for(int i = 0; i < numSteps; ++i)
    {
        pnt.drawRect (QRect(QPoint(i * pic.width() / numSteps,
                                   pic.height() * 0.9 * ( 1. - values[i] / valueMax)),
                            QPoint((i+1) * pic.width() / numSteps,
                                   pic.height() * 0.9)
                            )
                      );
    }
    pnt.drawLine(0,
                 pic.height() * 0.9,
                 pic.width(),
                 pic.height() * 0.9);

    pic.save(picPath, 0, 100);
}


void drawSammon(const vector< pair <double, double> > & plainCoords,
                const vector<int> & types,
                const QString & picPath) //uses coords array
{

    const int NumberOfVectors = plainCoords.size();
    //draw the points
    QPixmap pic(1200, 1200);
    pic.fill();
    QPainter painter;
    painter.begin(&pic);

    painter.setPen("black");
    painter.drawLine(QPointF(0, pic.height()/2.),
                     QPointF(pic.width(), pic.height()/2.));
    painter.drawLine(QPointF(pic.width()/2., 0),
                     QPointF(pic.width()/2, pic.height()));

    double minX = 0., minY = 0., maxX = 0., maxY = 0., avX, avY, rangeX, rangeY;
    const double rectSize = 4;

    double sum1 = 0., sum2 = 0.;

    minX = plainCoords.front().first;
    minY = plainCoords.front().second;
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        maxX = fmax(maxX, plainCoords[i].first);
        maxY = fmax(maxY, plainCoords[i].second);

        minX = fmin(minX, plainCoords[i].first);
        minY = fmin(minY, plainCoords[i].second);
    }
    avX = (minX + maxX)/2.;
    avY = (minY + maxY)/2.;

    rangeX = (maxX - minX)/2.;
    rangeY = (maxY - minY)/2.;

    rangeX *= 1.02;
    rangeY *= 1.02;

    double range = fmax(rangeX, rangeY);

    double initX = 0.;
    double initY = 0.;
    double leng = 0.;
    double angle = 0.;
    double drawX = 0.;
    double drawY = 0.;

    // count half of points for right angle
    double sumAngle1 = 0.;
    double sumAngle2 = 0.;
    double maxLeng = 0;
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        sum1 = plainCoords[i].first;
        sum2 = plainCoords[i].second;

        // relative coordinates (centroid)
        initX = sum1 - avX;
        initY = sum2 - avY;

        leng = pow(pow(initX, 2.) + pow(initY, 2.), 0.5);
        maxLeng = fmax(leng, maxLeng);

        angle = countAngle(initX, initY);

        if(i < NumberOfVectors/2)
        {
            sumAngle1 += angle;
        }
        if(i >= NumberOfVectors/4 && i < NumberOfVectors*3/4)
        {
            sumAngle2 += angle;
        }
    }
    sumAngle1 /= (NumberOfVectors/2);
    sumAngle2 /= (NumberOfVectors/2);
    range = maxLeng * 1.02;

    int mirror = 1;
    if(cos(sumAngle1) * sin(sumAngle2) - cos(sumAngle2) * sin(sumAngle1) < 0.)
    {
        mirror = -1;
    }

    int pew = 0;
    for(int i = 0; i < NumberOfVectors; ++i)
    {
        sum1 = plainCoords[i].first;
        sum2 = plainCoords[i].second;

        if( i<3 )
        {
            painter.setBrush(QBrush("red"));
            painter.setPen("red");

        }
        else if(0)
        {
            switch(types[i])
            {
            case 0:
            {
                painter.setBrush(QBrush("blue"));
                painter.setPen("blue");
                break;
            }
            case 1:
            {
                painter.setBrush(QBrush("red"));
                painter.setPen("red");
                break;
            }
            case 2:
            {
                painter.setBrush(QBrush("green"));
                painter.setPen("green");
                break;
            }
            default:
            {
                painter.setBrush(QBrush("black"));
                painter.setPen("black");
                break;
            }
            }
        }
        else
        {
            //            painter.setBrush(QBrush("red"));
            //            painter.setPen("red");

            pew = int(255.*i/NumberOfVectors);
            painter.setBrush(QBrush(QColor(pew,0,pew)));
            painter.setPen(QColor(pew,0,pew));
        }





        initX = sum1 - avX;
        initY = -(sum2 - avY);

        leng = pow(pow(initX, 2.) + pow(initY, 2.), 0.5);
        angle = countAngle(initX, initY);

        angle -= sumAngle1;
        //        initX = leng * cos(angle);
        //        initY = leng * sin(angle);
        mirror = 1;


        drawX = pic.width()  * 0.5 * (1. + (initX / range));
        drawY = pic.height() * 0.5 * (1. + (initY / range) * mirror);

        //        cout << drawX << '\t' << drawY << endl;

        painter.drawRect(QRectF(QPointF(drawX - rectSize,
                                        drawY - rectSize),
                                QPointF(drawX + rectSize,
                                        drawY + rectSize)
                                )
                         );

    }
    pic.save(picPath, 0, 100);
    painter.end();
    cout << "Sammon projection done" << endl;
}

void drawShepard(const mat & distOld,
                 const mat & distNew,
                 const QString & picPath)
{
    //    cout << distNew << endl;
    const int num = distOld.size();
    //draw the points
    QPixmap pic(1200, 1200);
    pic.fill();
    QPainter painter;
    painter.begin(&pic);

    painter.setPen("black");
    painter.setBrush(QBrush("black"));
    painter.drawLine(QPointF(pic.width() * 0.1, pic.height() * 0.9),
                     QPointF(pic.width() * 1.0, pic.height() * 0.9)
                     );
    painter.drawLine(QPointF(pic.width() * 0.1, pic.height() * 0.9),
                     QPointF(pic.width() * 0.1, pic.height() * 0.0)
                     );
    painter.drawLine(QPointF(pic.width() * 0.1, pic.height() * 0.9),
                     QPointF(pic.width() * 1.0, pic.height() * 0.0)
                     );

    double drawX = 0.;
    double drawY = 0.;
    double maxDistOld = 0.;
    double maxDistNew = 0.;
    for(int i = 0; i < num; ++i)
    {
        for(int j = i+1; j < num; ++j)
        {
            maxDistOld = fmax(distOld[i][j], maxDistOld);
            maxDistNew = fmax(distNew[i][j], maxDistNew);
        }
    }
    maxDistNew *= 1.02;
    maxDistOld *= 1.02;
    //    cout << "maxDistNew = " << maxDistNew << endl;
    //    cout << "maxDistOld = " << maxDistOld << endl;

    const int rectSize = 4;
    for(int i = 0; i < num; ++i)
    {
        for(int j = i+1; j < num; ++j)
        {
            drawX = pic.width()  * (0.1 + 0.9 * distOld[i][j] / maxDistOld);
            drawY = pic.height() * (0.9 - 0.9 * distNew[i][j] / maxDistNew);

            painter.drawRect(QRectF(QPoint(drawX - rectSize, drawY - rectSize),
                                    QPoint(drawX + rectSize, drawY + rectSize)
                                    )
                             );
        }
    }
    painter.end();
    pic.save(picPath, 0, 100);

}

void drawRealisation(const QString & inPath)
{
        matrix inData;
        int num = 0;
        readPlainData(inPath, inData, num);

        QString outPath = inPath;
        outPath.resize(outPath.lastIndexOf('.'));
        outPath += ".jpg";

        drawEeg(inData,
                inData.rows(),
                inData.cols(),
                def::freq,
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
        for(auto itt = std::begin(dataD[c2]); itt < std::end(dataD[c2]) - 1; ++itt, ++it, ++c1)
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
		paint.drawText(c3 * freq, pic.height() - 35, QString::number(c3));
        norm = 10.;
    }


    norm = 1;
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
		paint.drawText(c3 * freq, pic.height() - 35, QString::number(c3));
        norm = 10.;
    }
    pic.save(picPath, 0, 100);
    paint.end();
    return pic;
}


void drawRCP(const lineType & values, const QString & picPath)
{
    QPixmap pic(1000, 400);
    QPainter pnt;
    pic.fill();
    pnt.begin(&pic);
    pnt.setPen("black");
    int length = values.size();



    int numOfDisp = 4;
//    double xMin, xMax;
//    xMin = -numOfDisp;
//    xMax = numOfDisp;

    lineType line(pic.width());

    for(int i = 0; i < pic.width(); ++i)
    {
        line[i] = gaussian( (i - pic.width()/2) / (pic.width()/2.) * numOfDisp );
    }

    line /= line.max();

    for(int i = 0; i < pic.width() - 1; ++i)
    {
        pnt.drawLine(i,
                     pic.height() * 0.9 * (1. - line[i]),
                     i+1,
                     pic.height() * 0.9 * (1. - line[i+1]));
    }
    pnt.drawLine(0,
                 pic.height() * 0.9,
                 pic.width(),
                 pic.height() * 0.9);


    int coordinate;

    for(int i = 0; i < length; ++i) //draw the values
    {
        coordinate = pic.width()/2. * (1. + values[i] / numOfDisp);
        if(i%2 == 0) //raw data
        {
            pnt.setPen("blue");
        }
        else //ica data
        {
            pnt.setPen("red");
        }
        pnt.drawLine(coordinate,
                     pic.height() * 0.9 * ( 1. - line[coordinate]),
                     coordinate,
                     pic.height() * 0.9 * ( 1. - line[coordinate]) - 50);
    }
    pic.save(picPath, 0, 100);
}

} // nemspace myLib
