#include "library.h"

//template <typename Typ>
//Typ trackTime(Typ (*runFunc)())
//{
//    QTime myTime;
//    myTime.start();
//    Typ res = runFunc;
//    cout << stringify(runFunc) << ": time elapsed " << myTime.elapsed() / 1000. << " sec" << endl;
//    return res;
//}
//template void trackTime(void (*)());


//void trackTim(void (*runFunc)(void), char * funcNam)
//{
//    QTime myTime;
//    myTime.start();
//    runFunc();
//    cout << funcNam << ": time elapsed " << myTime.elapsed() / 1000. << " sec" << endl;
//}

//template <typename... Args>
//void trackTim(void (*runFunc)(Args...),
//              char * funcNam)
//{
//    QTime myTime;
//    myTime.start();
//    runFunc;
//    cout << funcNam << ": time elapsed " << myTime.elapsed() / 1000. << " sec" << endl;
//}
//template void trackTim(void (*runFunc)(int), char * funcNam);
//template void trackTim(void (*runFunc)(double), char * funcNam);




void writeByte(FILE * fil, int num)
{
    char tempChar = num;
    fwrite(&tempChar, sizeof(char), 1, fil);
}

void writeBytes(FILE * fil, int value, int numBytes)
{
    int tempInt;
    for(int i = 0; i < numBytes; ++i)
    {
        tempInt = (value / int(pow(256, i)))%256;
        writeByte(fil, tempInt);
    }
}

void writeWavFile(const vec & inData, const QString & outPath)
{
    // http://soundfile.sapp.org/doc/WaveFormat/


    FILE * outFile;
    outFile = fopen(outPath, "wb");
    if(outFile == NULL)
    {
        cout << "cant open file to write" << endl;
        return;
    }

    const int numChannels = 1;
    const int bitsPerSample = 16;
    const int sampleFreq = 44100;
    const int numSamples = inData.size();
    const double maxAmpl = *(std::max_element(inData.begin(), inData.end())) + 1e-3;
    const int subchunk2size = numSamples * numChannels * bitsPerSample / 8;
    const int chunkSize = 4 + (8 + 16) + (8 + subchunk2size);
    const int byteRate = sampleFreq * numChannels * bitsPerSample / 8;
    const int blockAlign = numChannels * bitsPerSample / 8;

    //RIFF
    writeByte(outFile, 0x52);
    writeByte(outFile, 0x49);
    writeByte(outFile, 0x46);
    writeByte(outFile, 0x46);

    //chunksize = 44 + ns * numSamples * bytesPerSample
    writeBytes(outFile, chunkSize, 4);

    //WAVE
    writeByte(outFile, 0x57);
    writeByte(outFile, 0x41);
    writeByte(outFile, 0x56);
    writeByte(outFile, 0x45);

    // fmt
    writeByte(outFile, 0x66);
    writeByte(outFile, 0x6d);
    writeByte(outFile, 0x74);
    writeByte(outFile, 0x20);

    //Subchunk1Size = 16 for pcm
    writeBytes(outFile, 16, 4);

    //audioFormat = 1 PCM
    writeBytes(outFile, 1, 2);

    //numChannels
    writeBytes(outFile, numChannels, 2);

    //sampleRate
    writeBytes(outFile, sampleFreq, 4);

    //BYTErate
    writeBytes(outFile, byteRate, 4);

    //block align
    writeBytes(outFile, blockAlign, 2);

    //bitsPerSample
    writeBytes(outFile, bitsPerSample, 2);

    //data
    writeByte(outFile, 0x64);
    writeByte(outFile, 0x61);
    writeByte(outFile, 0x74);
    writeByte(outFile, 0x61);

    //subchunk2size
    writeBytes(outFile, subchunk2size, 4);

    //the data itself
    int currVal;
    for(int i = 0; i < numSamples; ++i)
    {
        for(int j = 0; j < numChannels; ++j)
        {
            currVal = int(inData[i] * pow(256, bitsPerSample/8/numChannels) / maxAmpl);
            writeBytes(outFile, currVal, int(bitsPerSample/8/numChannels));
        }
    }
    fclose(outFile);
}


double const morletFall = 9.; // coef in matlab = mF^2 / (2 * pi^2);
double morletCosNew(double const freq1, // Hz
                    const double timeShift,
                    const double time)
{
    double freq = freq1 * 2. * pi;
    double res =  sqrt(2. * freq / pi_sqrt / morletFall)
                  * cos(freq * (time - timeShift) / def::freq)
                  * exp(-0.5 * pow(freq / morletFall * (time - timeShift) / def::freq, 2));
    return res;
}

double morletSinNew(double const freq1,
                    const double timeShift,
                    const double time)
{
    double freq = freq1 * 2. * pi;
    double res =  sqrt(2. * freq / pi_sqrt / morletFall)
                  * sin(freq * (time - timeShift) / def::freq)
                  * exp(-0.5 * pow(freq / morletFall * (time - timeShift) / def::freq, 2));
    return res;
}


QColor mapColor(double minMagn, double maxMagn, double ** helpMatrix, int numX, int numY, double partX, double partY, bool colour)
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
                        + slash() + outName + "_map_"
                        + QString::number(numOfColoumn) + "+.png";

    QPixmap pic1 = QPixmap(picSize, picSize);
    QPainter painter1;
    pic1.fill();
    painter1.begin(&pic1);
    QString savePath2 = outDir
                        + slash() + outName + "_map_"
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

    double ** Ah;
    matrixCreate(&Ah, 5, 6);
    double ** Bh;
    matrixCreate(&Bh, 5, 6);
    double * inX = new double [dim];
    double * inY = new double [dim];
    double * inYv = new double [dim];
    double * Av = new double [dim - 1];
    double * Bv = new double [dim - 1];

    for(int k = 0; k < dim; ++k)
    {
        inX[k] = k; //hope, it's constant
    }
    for(int i = 1; i < dim - 1; ++i) // number of helpMatrix row
    {
        for(int k = 0; k < dim; ++k)
        {
            inX[k] = k; //not necessary
            inY[k] = helpMatrix[i][k];
        }
        splineCoeffCount(inX, inY, dim, &(Ah[i-1]), &(Bh[i-1])); // horizontal splines coeffs
    }

    for(int x = 0; x < picSize; ++x)
    {
        for(int k = 1; k < dim-1; ++k)
        {
            for(int h = 0; h < dim; ++h) //set inX and inY for k'th row of helpMatrix
            {
                inX[h] = h; // not necessary
                inY[h] = helpMatrix[k][h];
            }
            inYv[k] = splineOutput(inX, inY, dim, Ah[k-1], Bh[k-1], x*scale1);
        }
        inYv[0] = 0.;
        inYv[dim-1] = 0.;
        splineCoeffCount(inX, inYv, dim, &Av, &Bv);
        for(int y = 0; y < picSize; ++y)
        {
            if(distance(x, y, picSize/2, picSize/2) > picSize * 2. * sqrt(2.)/(dim-1) ) continue;
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
            case 0:
            {
                painter.setBrush(QBrush(hueJet(drawRange, drawArg)));
                painter.setPen(hueJet(drawRange, drawArg));
                painter1.setBrush(QBrush(hueJet(drawRange, drawRange - drawArg)));
                painter1.setPen(hueJet(drawRange, drawRange - drawArg));
                break;
            }
            case 1:
            {
                painter.setBrush(QBrush(hueOld(drawRange, drawArg)));
                painter.setPen(hueOld(drawRange, drawArg));
                painter1.setBrush(QBrush(hueOld(drawRange, drawRange - drawArg)));
                painter1.setPen(hueOld(drawRange, drawRange - drawArg));
                break;
            }
            case 2:
            {
                painter.setBrush(QBrush(grayScale(drawRange, drawArg)));
                painter.setPen(grayScale(drawRange, drawArg));
                painter1.setBrush(QBrush(grayScale(drawRange, drawRange - drawArg)));
                painter1.setPen(grayScale(drawRange, drawRange - drawArg));
                break;
            }
            case 3:
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
    for(int i = 0; i < helpMatrix.rows(); ++i)
    {
        for(int j = 0; j < helpMatrix.cols(); ++j)
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



    matrixDelete(&Ah, 5);
    matrixDelete(&Bh, 5);
    delete []inX;
    delete []inY;
    delete []inYv;
    delete []Av;
    delete []Bv;
}




void drawMap(const double ** const &matrixA,
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

    double ** helpMatrix;
    matrixCreate(&helpMatrix, 5, 5); //generality for ns = 19

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
    for(int x = floor(scale1)*leftCoeff; x < floor(scale1)*rightCoeff; ++x)
    {
        for(int y = floor(scale1)*leftCoeff; y < floor(scale1)*rightCoeff; ++y)
        {
            if(distance(x, y, size/2, size/2) > size/2 ) continue; // make it round

            numX = floor(x/int(scale1)) ; //1 2
            numY = floor(y/int(scale1)) ; //3 4

            painter.setPen(mapColor(-maxAbs, maxAbs, helpMatrix, numX, numY, double(double(x%int(scale1))/scale1 + 0.003/scale1), double(double(y%int(scale1))/scale1) + 0.003/scale1, colourFlag)); // why 0.003
            painter.drawPoint(x,y);
        }
    }


    QString helpString = outDir + slash() + outName + "_map_" + QString::number(num) + ".png";
    pic.save(helpString, 0, 100);


    matrixDelete(&helpMatrix, 5);
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
    readICAMatrix(mapsFilePath, matrixA);
    double maxAbs = matrixA[0][0];
    for(int i = 0; i < matrixA.rows(); ++i)
    {
        for(int j = 0; j < matrixA.cols(); ++j)
        {
            maxAbs = fmax(maxAbs, fabs(matrixA[i][j]));
        }
    }

    for(int i = 0; i < def::nsWOM(); ++i)
    {
        draw1MapFunc(matrixA, i, outDir, outName, maxAbs, 240, colourTheme);
    }
}


void drawMapsOnSpectra(const QString &inSpectraFilePath,
                       const QString &outSpectraFilePath,
                       const QString &mapsDirPath,
                       const QString &mapsNames)
{
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

    for(int i = 0; i < def::nsWOM(); ++i)
    {
        helpString = mapsDirPath
                     + slash() + mapsNames
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
        pic1 = QPixmap(helpString);



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

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
void four1(double * dataF, int nn, int isign)
{
    int n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    double tempr,tempi;

    n = nn << 1; //n = 2 * fftLength
    j = 1;
    for (i = 1; i < n; i += 2)
    {
        if (j > i)
        {
            SWAP(dataF[j], dataF[i]);
            SWAP(dataF[j+1],dataF[i+1]);
        }
        m = n >> 1; // m = n / 2;
        while (m >= 2 && j > m)
        {
            j -= m;
            m >>= 1; //m /= 2;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax)
    {
        istep = mmax << 1; //istep = mmax * 2;
        theta = isign * (2 * pi / mmax);
        wtemp = sin(0.5 * theta);
        wpr = - 2.0 * wtemp * wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for(m = 1; m < mmax; m += 2)
        {
            for(i = m; i <= n; i += istep)
            {
                j = i + mmax;
                tempr = wr * dataF[j] - wi * dataF[j + 1];
                tempi = wr * dataF[j + 1] + wi * dataF[j];
                dataF[j] = dataF[i] - tempr;
                dataF[j + 1] = dataF[i + 1] - tempi;
                dataF[i] += tempr;
                dataF[i + 1] += tempi;
            }
            wr = (wtemp = wr) * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax = istep;
    }
}
#undef SWAP

//should test
void four1(vec & dataF, int fftLen, int isign)
{
    double * pew = new double [2 * fftLen];
    for(int i = 0; i < 2 * fftLen; ++i)
    {
        pew[i] = dataF[i];
    }
    four1(pew - 1, fftLen, isign);
//    four1(dataF.data() - 1, fftLen, isign);
    for(int i = 0; i < 2 * fftLen; ++i)
    {
        dataF[i] = pew[i];
    }
    delete []pew;
}

int len(QString s) //lentgh till double \0-byte for EDF+annotations
{
    int l = 0;
    for(int i = 0; i < 100500; ++i)
    {
        if(s[i]!='\0') ++l;
        else
        {
            if(s[i+1]!='\0') ++l;
            if(s[i+1]=='\0')
            {
                ++l;
                return l;
            }
        }
    }
    return -1;
}


// what is N? signalLength ?
double fractalDimension(const vec & arr, const QString &picPath)
{
    int timeShift; //timeShift
    long double L = 0.; //average length
    long double tempL = 0.;
    long double coeff = 0.;

    /// what are the limits?
    int N = arr.size();
    int maxLimit = floor( log(N) * 4. - 5.);

    maxLimit = 100;

    int minLimit = max(maxLimit - 10, 3);

    minLimit = 15;

    double * drawK = new double [maxLimit - minLimit];
    double * drawL = new double [maxLimit - minLimit];

    for(int h = minLimit; h < maxLimit; ++h)
    {
        timeShift = h;
        L = 0.;
        for(int m = 0; m < timeShift; ++m) // m = startShift
        {
            tempL = 0.;
            coeff = (N - 1) / (floor((N - m) / timeShift)) / timeShift;
            for(int i = 1; i < floor((N - m) / timeShift); ++i)
            {
                tempL += fabs(arr[m + i * timeShift] - arr[m + (i - 1) * timeShift]) * coeff;

            }
            L += tempL / timeShift;
        }
        drawK[h - minLimit] = log(timeShift);
        drawL[h - minLimit] = log(L);
        //        cout << drawK[h - minLimit] << '\t' << drawL[h - minLimit] << endl;
    }

    //least square approximation
    double slope = covariance(drawK, drawL, maxLimit - minLimit) / covariance(drawK, drawK, maxLimit - minLimit);
    //    cout << "slope = " << slope << endl;

    double drawX = 0.;
    double drawY = 0.;
    if(!picPath.isEmpty())
    {
        QPixmap pic = QPixmap(800, 600);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);

        pnt.setPen("black");
        pnt.setBrush(QBrush("black"));

        double minX = fmin(drawK[0], drawK[maxLimit - minLimit - 1]);
        double maxX = fmax(drawK[0], drawK[maxLimit - minLimit - 1]);
        double minY = fmin(drawL[0], drawL[maxLimit - minLimit - 1]);
        double maxY = fmax(drawL[0], drawL[maxLimit - minLimit - 1]);
        double lenX = maxX - minX;
        double lenY = maxY - minY;

        for(int h = 0; h < maxLimit - minLimit; ++h) // drawK, drawL [last] is bottom-left
        {
            drawX = fabs(drawK[h] - minX) / lenX * pic.width() - 3;
            drawY = (1. - fabs(drawL[h] - minY) / lenY) * pic.height() - 3;
            pnt.drawRect(QRect(int(drawX), int(drawY), 3, 3));
        }

        pnt.setPen("red");
        pnt.setBrush(QBrush("red"));

        // line passes (meanX, meanY)
        double add = mean(drawL, maxLimit - minLimit) - slope * mean(drawK, maxLimit - minLimit);
        //        cout << "add = " << add << endl;

        drawX = (1. - (slope * minX + add - minY) / lenY) * pic.height(); // startY
        drawY = (1. - (slope * maxX + add - minY) / lenY) * pic.height(); // endY
        //        cout << drawX << '\t' << drawY << endl;

        pnt.drawLine(0,
                     drawX,
                     pic.width(),
                     drawY
                     );

        pnt.end();
        pic.save(picPath, 0, 100);


    }

    delete []drawK;
    delete []drawL;
    return -slope;
}


int findChannel(char ** const labelsArr, QString chanName, int ns)
{
    for(int i = 0; i < ns; ++i)
    {
        if(QString(labelsArr[i]).contains(chanName))
        {
            return i;
        }
    }
}

QString setFileName(const QString & initNameOrPath) // append _num before the dot
{
    QString beforeDot = initNameOrPath;
    beforeDot.resize(beforeDot.lastIndexOf('.'));

    QString afterDot = initNameOrPath; //with the dot
    afterDot = afterDot.right(afterDot.length() - afterDot.lastIndexOf('.'));

    QString helpString;
    helpString = beforeDot + afterDot;
    int counter = 0;
    while (QFile::exists(helpString))
    {
        helpString = beforeDot + "_" + QString::number(counter++) + afterDot;
    }
    return helpString;
}

QString getExpNameLib(const QString & filePath) // getFileName
{
    QString hlp;
    hlp = QDir::toNativeSeparators(filePath);
    hlp = hlp.right(hlp.length() - hlp.lastIndexOf(slash()) - 1); // ExpName.edf
    hlp = hlp.left(hlp.lastIndexOf('.')); // ExpName
    return hlp;
}

QString getDirPathLib(const QString & filePath)
{
    QString str = filePath;
    return str.left(str.lastIndexOf(slash()));
}

QString getExt(QString filePath)
{
    QString helpString = QDir::toNativeSeparators(filePath);
    if(helpString.contains('.'))
    {
        helpString = helpString.right(helpString.length() - helpString.lastIndexOf('.') - 1);
        return helpString;
    }
    else
    {
        return QString();
    }
}

QString getFileName(const QString & filePath, bool withExtension)
{
    QString helpString = QDir::toNativeSeparators(filePath);
    helpString = helpString.right(helpString.length() - helpString.lastIndexOf(slash()) - 1);
    if(!withExtension)
    {
        helpString = helpString.left(helpString.lastIndexOf("."));
    }
    return helpString;
}

bool areEqualFiles(QString path1, QString path2)
{
    QTime myTime;
    myTime.start();

    typedef size_t byte;
    FILE * fil1 = fopen(path1, "rb");
    FILE * fil2 = fopen(path2, "rb");
    if(fil1 == NULL || fil2 == NULL)
    {
        cout << "areEqualFiles: some of the files == NULL" << endl;
    }
    byte byt1, byt2;
    while(!feof(fil1) && !feof(fil2))
    {
        fread(&byt1, sizeof(byte), 1, fil1);
        fread(&byt2, sizeof(byte), 1, fil2);
        if(byt1 != byt2)
        {
            fclose(fil1);
            fclose(fil2);
            cout << "equalFiles: time = " << myTime.elapsed() / 1000. << " sec" << endl;
            return false;
        }

    }
    fclose(fil1);
    fclose(fil2);
    cout << "equalFiles: time = " << myTime.elapsed() / 1000. << " sec" << endl;
    return true;
}

QString getPicPath(const QString & dataPath,
                   const QDir * ExpNameDir,
                   const int & ns)
{
    QString fileName = QDir::toNativeSeparators(dataPath);
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf(slash()) - 1);
    fileName.replace('.', '_');

    QString helpString = QDir::toNativeSeparators(ExpNameDir->absolutePath() + slash());

    if(dataPath.contains("Realisations"))
    {
        helpString += "Signals";
    }
    else if(dataPath.contains("windows"))
    {
        if(!dataPath.contains("fromreal"))
        {
            helpString += "Signals" + QString(slash()) + "windows";
        }
        else /// fromreals need other path?
        {
            helpString += "Signals" + QString(slash()) + "windows";
        }
    }
    else if(dataPath.contains("cut"))
    {
        helpString += "SignalsCut";
    }
    helpString += slash();
    if(ns == 19)
    {
        helpString += "after";
    }
    else if(ns == 21)
    {
        helpString += "before";
    }
    else
    {
        helpString += "other";
    }
    helpString += slash() + fileName + ".jpg";
    return helpString;
}
QString slash()
{
    return QString(QDir::separator());
}

char * strToChar(const QString & input)
{
    char * array = new char [input.length() + 1];
    memcpy(array, input.toStdString().c_str(), input.length());
    array[input.length()] = '\0';
    return array;
}

char * QStrToCharArr(const QString & input, const int &len)
{
    // fixes problem with labels length

    int leng = input.length();
    if(len != -1)
    {
        leng = len;
    }
    char * array = new char [leng + 1];
    memcpy(array, input.toStdString().c_str(), input.length());

    if(len != -1)
    {
        for(int i = input.length(); i < leng; ++i)
        {
            array[i] = ' ';
        }
    }
    array[leng] = '\0';
    return array;
}

FILE * fopen(QString filePath, const char *__modes)
{
    return fopen(filePath.toStdString().c_str(), __modes);
}

ostream & operator << (ostream &os, QString toOut)
{
    os << toOut.toStdString();
    return os;
}


template <typename T>
ostream & operator << (ostream &os, vector<T> toOut) // template!
{
    for(int i = 0; i < toOut.size(); ++i)
    {
        os << toOut[i] << '\t';
    }
    return os;
}

ostream & operator << (ostream &os, vector < vector < double > > toOut)
{
    for(unsigned int i = 0; i < toOut.size(); ++i)
    {
        os << toOut[i] << endl;
    }
    return os;

}

ostream & operator << (ostream &os, QList<int> toOut)
{
    for(int i = 0; i < toOut.length(); ++i)
    {
        os << toOut[i] << " ";
    }
    return os;
}

ostream & operator << (ostream &os, matrix toOut)
{
    for(auto it = toOut.data.begin(); it < toOut.data.end(); ++it)
    {
        for(auto itt = (*it).begin(); itt < (*it).end(); ++itt)
        {
            os << doubleRound((*itt), 4) << "\t";
        }
        os << endl;
    }
    return os;
}

//mat operator=(const mat & other)
//{
//    mat temp;
//    temp.resize(other.size());
//    for(int i = 0; i < other.size(); ++i)
//    {
//        temp[i].resize(other[i].size());
//        for(int j = 0; j < other[i].size(); ++j)
//        {
//            temp[i][j] = other[i][j];
//        }
//    }
//    return temp;
//}

double vectorLength(double * arr, int len)
{
    double a = 0.;
    for(int i = 0; i < len; ++i)
    {
        a += arr[i]*arr[i];
    }
    return sqrt(a);
}

double quantile(double arg)
{
    double a, b;
    //    a = exp(0.14*log(arg));
    //    b = exp(0.14*log(1-arg));
    a = pow(arg, 0.14);
    b = pow(1. - arg, 0.14);
    return (4.91*(a-b));
}

template <typename Typ>
double mean(const Typ &arr, int length, int shift)
{
    double sum = 0.;
    for(int i = 0; i < length; ++i)
    {
        sum += arr[i + shift];
    }
    sum /= length;
    return sum;
}
template double mean(const double * const &arr, int length, int shift);
template double mean(const int * const &arr, int length, int shift);
template double mean(const vector<int> &arr, int length, int shift);
template double mean(const vector<double> &arr, int length, int shift);

template <typename Typ>
double variance(const Typ &arr, int length, int shift, bool fromZero)
{
    double sum1 = 0.;
    double m = mean(arr, length, shift);
    double sign = (fromZero)?0.:1.;
    for(int i = 0; i < length; ++i)
    {
        sum1 += pow((arr[i + shift] - m * sign), 2.);
    }
    sum1 /= length; // needed ?
    return sum1;
}
template double variance(const double * const &arr, int length, int shift, bool fromZero);
template double variance(const int * const &arr, int length, int shift, bool fromZero);
template double variance(const vector<int> &arr, int length, int shift, bool fromZero);
template double variance(const vector<double> &arr, int length, int shift, bool fromZero);

template <typename Typ>
double sigma(const Typ &arr, int length, int shift, bool fromZero)
{
    return sqrt(variance(arr, length, shift, fromZero));
}
template double sigma(const double * const &arr, int length, int shift, bool fromZero);
template double sigma(const int * const &arr, int length, int shift, bool fromZero);
template double sigma(const vector<int> &arr, int length, int shift, bool fromZero);
template double sigma(const vector<double> &arr, int length, int shift, bool fromZero);

template <typename Typ>
double covariance(const Typ &arr1, const Typ &arr2, int length, int shift, bool fromZero)
{
    double res = 0.;
    double m1 = mean(arr1, length, shift);
    double m2 = mean(arr2, length, shift);
    double sign = (fromZero)?0.:1.;
    for(int i = 0; i < length; ++i)
    {
        res += (arr1[i + shift] - m1 * sign) *
                (arr2[i + shift] - m2 * sign);
    }
    return res;
}
template double covariance(const double * const &arr1, const double * const &arr2, int length, int shift, bool fromZero);
template double covariance(const int * const &arr1, const int * const &arr2, int length, int shift, bool fromZero);
template double covariance(const vector<int> &arr1, const vector<int> &arr2, int length, int shift, bool fromZero);
template double covariance(const vector<double> &arr1, const vector<double> &arr2, int length, int shift, bool fromZero);

template <typename Typ>
double correlation(const Typ &arr1, const Typ &arr2, int length, int shift, bool fromZero)
{
    double res = 0.;
    double m1, m2;
    int T = abs(shift);
    double sigmas;
    int signM = (shift >= 0)?1:0;
    int signL = (shift <= 0)?1:0;

    m1 = mean(arr1, length - T, T * signL);
    m2 = mean(arr2, length - T, T * signM);
    for(int i = 0; i < length - T; ++i)
    {
        res += (arr1[i + T * signL] - m1) * (arr2[i + T * signM] - m2);
    }
    sigmas = sigma(arr1, length - T, T * signL, fromZero) *
             sigma(arr2, length - T, T * signM, fromZero);
    if(sigmas != 0.)
    {
        res /= sigmas;
    }
    else
    {
        cout << "correlation const signal" << endl;
        return 0.;
    }

    res /= double(length - T);
    return res;
}
template double correlation(const double * const &arr1, const double * const &arr2, int length, int shift, bool fromZero);
template double correlation(const int * const  &arr1, const int * const &arr2, int length, int shift, bool fromZero);
template double correlation(const vector<int> &arr1, const vector<int> &arr2, int length, int shift, bool fromZero);
template double correlation(const vector<double> &arr1, const vector<double> &arr2, int length, int shift, bool fromZero);

double skewness(double *arr, int length)
{
    double sum = 0.;
    double av = mean(arr, length);
    double disp = variance(arr, length);
    for(int i = 0; i < length; ++i)
    {
        sum += pow((arr[i] - av), 3);
    }
    sum /= double(length);
    sum /= pow(disp, 1.5);
    return sum;
}

double kurtosis(double *arr, int length)
{
    double sum = 0.;
    double av = mean(arr, length);
    double disp = variance(arr, length);
    for(int i = 0; i < length; ++i)
    {
        sum += pow((arr[i] - av), 4);
    }
    sum /= double(length);
    sum /= pow(disp, 2.);
    sum -= 3.;
    return sum;
}


double rankit(int i, int length, double k)
{
    return quantile( (i-k) / (length + 1. - 2. * k) );
}

int MannWhitney(double * arr1, int len1,
                 double * arr2, int len2,
                 double p)
{
    vec vect1;
    vec vect2;
    for(int i = 0; i < len1; ++i)
    {
        vect1.push_back(arr1[i]);
    }
    for(int i = 0; i < len2; ++i)
    {
        vect2.push_back(arr2[i]);
    }
    return MannWhitney(vect1, vect2, p);

}


int MannWhitney(const vec & arr1,
                const vec & arr2,
                const double p)
{
    vector <pair <double, int>> arr;

    // fill first array
    std::for_each(arr1.begin(),
                  arr1.end(),
                  [&arr](double in)
    {arr.push_back(std::make_pair(in, 0));});

    // fill second array
    std::for_each(arr2.begin(),
                  arr2.end(),
                  [&arr](double in)
    {arr.push_back(std::make_pair(in, 1));});

    std::sort(arr.begin(),
              arr.end(),
              [](std::pair<double, int> i,
              std::pair<double, int> j) {return i.first > j.first;});

    int sum0 = 0;
    int sumAll;
    const int N1 = arr1.size();
    const int N2 = arr2.size();

    const double average = N1 * N2 / 2.;
    const double dispersion = sqrt(N1 * N2 * ( N1 + N2 ) / 12.);

    double U = 0.;


    //count sums
    for(unsigned int i = 0; i < arr.size(); ++i)
    {
        if(arr[i].second == 0)
        {
            sum0 += (i+1);
        }
    }

    //    cout << "vec " << sum0 << endl;

    sumAll = ( N1 + N2 )
             * (N1 + N2 + 1) / 2;

    if(sum0 > sumAll/2 )
    {
        U = double(N1 * N2
                   + N1 * (N1 + 1) /2. - sum0);
    }
    else
    {

        U = double(N1 * N2
                   + N2 * (N2 + 1) /2. - (sumAll - sum0));
    }

    const double beliefLimit = quantile( (1.00 + (1. - p) ) / 2.);
    const double ourValue = (U - average) / dispersion;

//    const double s1 = mean(arr1, N1);
//    const double s2 = mean(arr2, N2);

    // old
    if(fabs(ourValue) > beliefLimit)
    {
//        if(s1 > s2)
        // new
        if(sum0 < sumAll / 2 )
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return 0;
    }

    // new try DONT WORK??? to test
    if(ourValue > beliefLimit)
    {
        return 1;
    }
    else if (ourValue < -beliefLimit)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

int typeOfFileName(const QString & fileName)
{
    QStringList leest;
    int res = 0;
    for(const QString & marker : def::fileMarkers)
    {
        leest.clear();
        leest = marker.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(const QString & filter : leest)
        {
            if(fileName.contains(filter))
            {
                return res;
            }
        }
        ++res;
    }
    return -1;
}

QString getFileMarker(const QString & fileName)
{
    for(const QString & fileMark : def::fileMarkers)
    {
        QStringList lst = fileMark.split(' ', QString::SkipEmptyParts);
        for(const QString & filter : lst)
        {
            if(fileName.contains(filter))
            {
                return filter.right(3); // generality markers appearance
            }
        }
    }
}



void makePaFile(const QString & spectraDir,
                const QStringList & fileNames,
                const double & coeff,
                const QString & outFile,
                const bool svmFlag) /// make enum
{
    //    QTime myTime;
    //    myTime.start();

    ofstream outStream(outFile.toStdString());
    if(!outStream.good())
    {
        cout << "bad out pa-file" << endl;
        return;
    }
    matrix data4;

    int type;
    QString helpString;

    for(const QString & fileName: fileNames)
    {
        type = typeOfFileName(fileName);
        helpString = spectraDir
                     + slash() + fileName;
        readSpectraFile(helpString, data4);



        if(!svmFlag)
        {
            outStream << fileName << endl;
            for(int l = 0; l < def::nsWOM(); ++l) // write PA files without markers
            {
                for(int k = 0; k < def::spLength; ++k)
                {
                    outStream << doubleRound(data4[l][k] / coeff, 5) << '\t';
                    if(k%10 == 9)
                    {
                        outStream << '\n';
                    }
                }
                outStream << '\n';
            }
            for(int k = 0; k < def::numOfClasses; ++k)
            {

                outStream << (k==type) << ' ';
            }
            outStream << "\n\n";
            outStream.flush();
        }
        else // svm
        {
            outStream << type << ' ';
            for(int l = 0; l < def::nsWOM(); ++l) // write PA files without markers
            {
                for(int k = 0; k < def::spLength; ++k)
                {
                    outStream << l * def::spLength + k << ':' << doubleRound(data4[l][k] / coeff, 5) << ' ';
                }
            }
            outStream << endl;
        }





    }
    outStream.close();
    //    cout << "makePaFile: time elapsed = " << myTime.elapsed()/1000. << " sec" <<endl;
}

void makeFileLists(const QString & path,
                   vector<QStringList> & lst)
{
    QDir localDir(path);
    QStringList nameFilters, leest;
    QString helpString;
    for(const QString & fileMark : def::fileMarkers)
    {
        nameFilters.clear();
        leest.clear();
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(const QString & filter : leest)
        {
            helpString = "*" + filter + "*";
            nameFilters << helpString;
        }
        lst.push_back(localDir.entryList(nameFilters, QDir::Files, QDir::Name));
    }

}

void makePaStatic(const QString & spectraDir,
                  const int & fold,
                  const double & coeff,
                  const bool svmFlag) /// make enum
{

    QString helpString;
    const QString paPath = def::dir->absolutePath()
                          + slash() + "PA";
    vector<QStringList> lst;
    makeFileLists(spectraDir, lst);

    int len[def::numOfClasses];
    for(int i = 0; i < def::numOfClasses; ++i)
    {
        len[i] = lst[i].length();
        if(len[i] == 0)
        {
            cout << "no spectra files found of type " << i << " found" << endl;
            return;
        }
    }

    int Length;
    Length = len[0];
    for(int i = 0; i < def::numOfClasses; ++i)
    {
        Length = min(Length, len[i]);
    }

    vector<vector<int>> arr;
    arr.resize(def::numOfClasses);
    for(int i = 0; i < def::numOfClasses; ++i)
    {
        arr[i].resize(len[i]);

        for(int j = 0; j < len[i]; ++j)
        {
            arr[i][j] = j;
        }
    }

    //generality
    if(def::nsWOM() == -1) return;
    if(def::spLength == -1) return;
    //mix list

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    for(int i = 0; i < def::numOfClasses; ++i)
    {
        std::shuffle(arr[i].begin(),
                     arr[i].end(),
                     std::default_random_engine(seed));
    }



    QStringList listToWrite;
    listToWrite.clear();
    for(int j = 0; j < def::numOfClasses; ++j)
    {
        for(int i = 0; i < (len[j] / fold) * (fold - 1); ++i)
        {
            listToWrite << lst[j][arr[j][i]];
        }
    }
    helpString = QDir::toNativeSeparators(paPath + slash());
    if(!svmFlag)
    {
        helpString += "1.pa";
        makePaFile(spectraDir, listToWrite, coeff, helpString);
    }
    else
    {
        helpString += "svm1";
        makePaFile(spectraDir, listToWrite, coeff, helpString, true);
    }




    listToWrite.clear();
    for(int j = 0; j < def::numOfClasses; ++j)
    {
        for(int i = (len[j] / fold) * (fold - 1); i < (len[j] / fold) * fold; ++i)
        {
            listToWrite << lst[j][arr[j][i]];
        }
    }
    helpString = QDir::toNativeSeparators(paPath + slash());
    if(!svmFlag)
    {
        helpString += "2.pa";
        makePaFile(spectraDir, listToWrite, coeff, helpString);
    }
    else
    {
        helpString += "svm2";
        makePaFile(spectraDir, listToWrite, coeff, helpString, true);
    }


    if(!svmFlag)
    {
        /// sorting by classes !!!!!!!!!!!!!!!!!!!!!!!!!!
        /// very important for auto cross-validation
        listToWrite.clear();
        for(int j = 0; j < def::numOfClasses; ++j)
        {
            for(int i = 0; i < (len[j] / fold) * fold; ++i)
            {
                listToWrite << lst[j][arr[j][i]];
            }
        }
        helpString = QDir::toNativeSeparators(paPath + slash() + "all.pa");
        makePaFile(spectraDir, listToWrite, coeff, helpString);
    }
}

void makeMatrixFromFiles(QString spectraDir,
                         QStringList fileNames,
                         double coeff,
                         double *** outMatrix)
{
    //    QTime myTime;
    //    myTime.start();

    matrix data4;

    int type;
    QString helpString;

    for(int i = 0; i < fileNames.length(); ++i)
    {
        type = typeOfFileName(fileNames[i]);
        helpString = spectraDir + slash() + fileNames[i];
        readSpectraFile(helpString, data4);

        for(int l = 0; l < def::nsWOM(); ++l)
        {
            for(int k = 0; k < def::spLength; ++k)
            {
                (*outMatrix)[i][l * def::spLength + k] = data4[l][k] / coeff;
            }
        }
        (*outMatrix)[i][def::nsWOM() * def::spLength] = 1.;
        (*outMatrix)[i][def::nsWOM() * def::spLength + 1] = type;

    }
    //    cout << "makeMatrixFromFiles: time elapsed = " << myTime.elapsed()/1000. << " sec" <<endl;
}

void cleanDir(QString dirPath, QString nameFilter, bool ext)
{
    QDir * tmpDir = new QDir(dirPath);

    QStringList lst;

    if(nameFilter.isEmpty()) lst = tmpDir->entryList(QDir::Files);
    else
    {
        QStringList filter;
        QString hlp;
        filter.clear();
        if(ext)
        {
            hlp = "*." + nameFilter;
        }
        else
        {
            hlp = "*" + nameFilter + "*";
        }
        filter << hlp;
        lst = tmpDir->entryList(filter, QDir::Files);
    }

    for(int h = 0; h < lst.length(); ++h)
    {
        QFile::remove(tmpDir->absolutePath() + slash() + lst[h]);
    }
    delete tmpDir;

}

void drawRCP(const vec & values, const QString & picPath)
{
    QPixmap pic(1000, 400);
    QPainter pnt;
    pic.fill();
    pnt.begin(&pic);
    pnt.setPen("black");
    int length = values.size();


    double xMin, xMax;
    //    //generality

    int numOfDisp = 4;
    xMin = -numOfDisp;
    xMax = numOfDisp;

    double * line = new double [pic.width()];

    for(int i = 0; i < pic.width(); ++i)
    {
        line[i] = gaussian( (i - pic.width()/2) / (pic.width()/2.) * numOfDisp );
    }

    double valueMax;
    valueMax = maxValue(line, pic.width());

    for(int i = 0; i < pic.width() - 1; ++i)
    {
        pnt.drawLine(i,
                     pic.height() * 0.9 * (1. - line[i] / valueMax),
                     i+1,
                     pic.height() * 0.9 * (1. - line[i+1] / valueMax));
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
                     pic.height() * 0.9 * ( 1. - line[coordinate] / valueMax),
                     coordinate,
                     pic.height() * 0.9 * ( 1. - line[coordinate] / valueMax) - 50);
    }
    pic.save(picPath, 0, 100);
}

double maxValue(double * arr, int length)
{
    double res = arr[0];
    for(int i = 0; i < length; ++i)
    {
        res = fmax(res, arr[i]);
    }
    return res;
}

double maxAbsValue(double * arr, int length)
{
    double res = fabs(arr[0]);
    for(int i = 0; i < length; ++i)
    {
        res = fmax(res, fabs(arr[i]));
    }
    return res;
}

double minValue(double * arr, int length)
{
    double res = arr[0];
    for(int i = 0; i < length; ++i)
    {
        res = fmin(res, arr[i]);
    }
    return res;
}

double enthropy(const double * arr, const int N, const int numOfRanges) // ~30 is ok
{
    double max_ = 0.;
    double min_ = 0.;
    double result = 0.;

    for(int i = 0; i < N; ++i)
    {
        //        maxAbs = fmax(maxAbs, fabs(arr[i]));
        max_ = fmax(max_, arr[i]);
        min_ = fmin(min_, arr[i]);
    }
    max_ *= 1.0001;
    int tempCount = 0;
    for(int j = 0; j < numOfRanges; ++j)
    {
        tempCount = 0;
        for(int i = 0; i < N; ++i)
        {
            //            if((-maxAbs + 2. * maxAbs/double(numOfRanges) * j < arr[i]) && (-maxAbs + 2. * maxAbs/double(numOfRanges) * (j + 1) > arr[i]))
            if((min_ + (max_ - min_)/double(numOfRanges) * j <= arr[i]) && (min_ + (max_ - min_)/double(numOfRanges) * (j + 1) > arr[i]))
            {
                ++tempCount;
            }
        }
        //        cout<< tempCount << endl;
        if(tempCount!=0)
        {
            result -= (tempCount/double(N)) * log(tempCount/double(N));
        }
    }
    return result;
}

template <typename Typ1, typename Typ2, typename Typ3>
void matrixProduct(const Typ1 & inMat1, const Typ2 & inMat2, Typ3 &outMat, int dimH, int dimL)
{
    double result;

    for(int j = 0; j < dimL; ++j)
    {
        for(int i = 0; i < dimH; ++i)
        {
            result = 0.;
            for(int k = 0; k < dimH; ++k)
            {
                result += inMat1[i][k] * inMat2[k][j];
            }
            outMat[i][j] = result;
        }
    }
}
template void matrixProduct(const double ** const &inMat1, const double ** const &inMat2, double **& outMat, int dimH, int dimL);
template void matrixProduct(const double ** const &inMat1, const double ** const &inMat2, mat & outMat, int dimH, int dimL);
template void matrixProduct(const mat & inMat1, const mat & inMat2, double **& outMat, int dimH, int dimL);
template void matrixProduct(const mat & inMat1, const mat & inMat2, mat & outMat, int dimH, int dimL);

template <typename Typ1, typename Typ2, typename Typ3>
void matrixProduct(const Typ1 & inMat1, const Typ2 & inMat2, Typ3 &outMat, int numRows1, int numCols2, int numCols1Rows2)
{
    double result;

    for(int j = 0; j < numRows1; ++j)
    {
        for(int i = 0; i < numCols2; ++i)
        {
            result = 0.;
            for(int k = 0; k < numCols1Rows2; ++k)
            {
                result += inMat1[j][k] * inMat2[k][i];
            }
            outMat[j][i] = result;
        }
    }
}
template void matrixProduct(const double ** const & inMat1, const double ** const & inMat2, mat & outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const double ** const & inMat1, const double ** const & inMat2, double **& outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const double ** const & inMat1, const mat & inMat2, double **& outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const mat & inMat1, const mat & inMat2, double **& outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const mat & inMat1, const mat & inMat2, mat & outMat, int numRows1, int numCols2, int numCols1Rows2);

template <typename T>
double distance(const vector<T> &vec1, const vector<T> &vec2, const int &dim)
{
    double dist = 0.;
    //Euclid
    for(int i = 0; i < dim; ++i)
    {
        dist += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return dist;
}

void matrixProduct(double * &vect, double ** &mat, double *& outVect, int dimVect, int dimMat) //outVect(dimMat) = vect(dimVect) * mat(dimVect * dimMat)
{
    for(int i = 0; i < dimMat; ++i)
    {
        outVect[i] = 0.;
        for(int j = 0; j < dimVect; ++j)
        {
            outVect[i] += vect[j] * mat[j][i];
        }
    }
}

void matrixProduct(double ** &mat, double * &vect, double *& outVect, int dimVect, int dimMat) //outVect(dimMat) = mat(dimMat*dimVect) * vect(dimVect)
{
    for(int i = 0; i < dimMat; ++i)
    {
        outVect[i] = 0.;
        for(int j = 0; j < dimVect; ++j)
        {
            outVect[i] += mat[i][j] * vect[j];
        }
    }
}

void matrixProduct(double * &vect1, double * &vect2, int dim, double & out)
{
    out = 0.;
    for(int j = 0; j < dim; ++j)
    {
        out += vect1[j] * vect2[j];
    }

}

double distance(double * vec1, double * vec2, const int &dim)
{
    double dist = 0.;
    //Euclid
    for(int i = 0; i < dim; ++i)
    {
        dist += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return dist;

}

double distance(double const x1, double const y1, double const x2, double const y2)
{
    return pow(pow(x1 - x2, 2.) + pow(y1 - y2, 2.), 0.5);
}

void matrixMahCount(double ** &matrix, int number, int dimension, double ** &outMat, double *& meanVect) //matrix(number * dimension)
{
    double ** newMat;
    matrixCreate(&newMat, dimension, number);
    matrixTranspose(matrix, number, dimension, newMat);

    for(int i = 0; i < dimension; ++i)
    {
        meanVect[i] = 0.;
        for(int j = 0; j < number; ++j)
        {
            meanVect[i] += newMat[i][j];
        }
        meanVect[i] /= number;
    }

    for(int i = 0; i < dimension; ++i)
    {
        for(int j = 0; j < dimension; ++j)
        {
            outMat[i][j] = covariance(newMat[i], newMat[j], number);
        }
    }
    matrixDelete(&newMat, dimension);

}

double distanceMah(double * &vect, double ** &covMatrixInv, double * &groupMean, int dimension)
{
    double * tempVec = new double [dimension];
    double * difVec = new double [dimension];
    for(int i = 0; i < dimension; ++i)
    {
        difVec[i] = vect[i] - groupMean[i];
    }

    double res;
    matrixProduct(difVec, covMatrixInv, tempVec, dimension, dimension);
    matrixProduct(tempVec, difVec, dimension, res);

    delete []tempVec;
    delete []difVec;
    return res;
}

double distanceMah(double * &vect, double ** &group, int dimension, int number) //group[number][dimension]
{
    double ** covMatrix;
    matrixCreate(&covMatrix, dimension, dimension);
    double * meanGroup = new double [dimension];

    matrixMahCount(group, number, dimension, covMatrix, meanGroup);
    matrixInvertGauss(covMatrix, dimension);

    double res = distanceMah(vect, covMatrix, meanGroup, dimension);
    matrixDelete(&covMatrix, dimension);
    delete []meanGroup;
    return res;
}

QString fitNumber(const double &input, int N) // append spaces
{
    QString h;
    h.setNum(input);
    h += QString(N, ' ');
    return h.left(N);
}
QString fitString(const QString & input, int N) // append spaces
{
    QString h(input);
    h += QString(N, ' ');
    return h.left(N);
}

QString rightNumber(const unsigned int input, int N) // prepend zeros
{
    QString h;
    h.setNum(input);
    for(int i = 0; i < N; ++i)
    {
        h.prepend("0");
    }
    return h.right(N);
}


void drawArray(double * array, int length, QString outPath)
{
    if(outPath.isEmpty()) return;

    const double picH = 600;
    const int penWidth = 2;

    QPixmap pic(length, picH);
    pic.fill();
    QPainter pnt;
    pnt.begin(&pic);

    double maxVal = maxValue(array, length);
    pnt.setPen(QPen(QBrush("black"), penWidth));
    for(int i = 0; i < pic.width() - 1; ++i)
    {
        pnt.drawLine(i,
                     pic.height() * (1. - array[i] / maxVal),
                     i + 1,
                     pic.height() * (1. - array[i + 1] / maxVal));
    }
    pnt.end();
    pic.save(outPath, 0, 100);
}

//void drawArray(double ***sp, int count, int *spL, QStringList colours, int type, double scaling, int left, int right, double spStep, QString outName, QString rangePicPath, QDir * dirBC)

void drawTemplate(const QString & outPath,
                  bool channelsFlag,
                  int width,
                  int height)
{
    QSvgGenerator svgGen;
    QPixmap pic;
    QPainter paint;
    QString helpString;
    int numOfChan = 19;
    if(outPath.contains("_ica") || def::ExpName.contains("_ica"))
    {
        channelsFlag = false; // numbers for ICAs
    }

    if(outPath.contains(".svg"))
    {
        svgGen.setSize(QSize(width, height));
        svgGen.setViewBox(QRect(QPoint(0,0), svgGen.size()));
        svgGen.setFileName(outPath);
        paint.begin(&svgGen);
        paint.setBrush(QBrush("white"));
        paint.drawRect(QRect(QPoint(0,0), svgGen.size()));
    }
    else if(outPath.contains(".jpg") || outPath.contains(".png"))
    {
        pic = QPixmap(width, height);
        pic.fill();
        paint.begin(&pic);
    }



    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;
    const double graphScale = def::spLength / graphWidth;
    // some empirical values prepared for defauls 1600*1600
    const double scaleY = paint.device()->height() / 1600.;
    const double scaleX = paint.device()->width() / 1600.;

    for(int c2 = 0; c2 < numOfChan; ++c2)  //exept markers channel
    {

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
        paint.setFont(QFont("Helvitica", int(10 * scaleY)));
        for(int k = 0; k < graphWidth; ++k) //for every Hz generality
        {
            if( abs((def::left + k * graphScale) * def::spStep
                    - ceil((def::left + k * graphScale) * def::spStep - 0.5))
                < graphScale * def::spStep / 2. )
            {
                paint.drawLine(QPointF(X + k,
                                       Y),
                               QPointF(X + k,
                                       Y + 5 * scaleY));

                helpString = QString::number(int((def::left + k * graphScale) * def::spStep + 0.5));

                if(helpString.toInt() % 5 != 0) continue;

                if(helpString.toInt() < 10)
                {
                    paint.drawText(QPointF(X + k - 3 * scaleX,
                                           Y + 15 * scaleY),
                                   helpString);
                }
                else
                {
                    paint.drawText(QPointF(X + k - 5 * scaleX,
                                           Y + 15 * scaleY),
                                   helpString);
                }
            }
        }
        paint.setFont(QFont("Helvetica", int(24 * scaleY), -1, false));
        if(channelsFlag)
        {
            helpString = QString(coords::lbl[c2]);
        }
        else
        {
            helpString = QString::number(c2);

        }
        paint.drawText(QPointF(X - 16 * scaleX,
                               Y - graphHeight + 24 * scaleY),
                       helpString);

    }
    //write channels labels

    paint.end();
    if(!outPath.contains(".svg"))
    {
        pic.save(outPath, 0, 100);
    }
}

void drawArray(const QString & templPath,
               const vec & inData,
               const QString & color,
               const double & scaling,
               const int & lineWidth)
{
    QSvgGenerator svgGen;
    QSvgRenderer svgRen;
    QPixmap pic;
    QPainter paint;
    QString helpString;
    int numOfChan = 19;

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

    if(inData.size() != numOfChan * def::spLength)
    {
        cout << "inappropriate array size" << endl;
        return;
    }

    double norm = 0.;
    std::for_each(inData.begin(),
                  inData.end(),
                  [&norm](double inn)
    {
        norm = fmax(norm, inn);
    });

#if 0
    // for weights
    norm /= 2;
#endif


    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;
    const double graphScale = def::spLength / graphWidth;
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
                                   Y - inData[c2 * def::spLength + k * graphScale] * norm),
                    QPointF(X + k + 1,
                            Y - inData[c2 * def::spLength + (k + 1) * graphScale] * norm));
#else
            // weights
            paint.drawLine(QPointF(X + k,
                                   Y - graphHeight / 2.
                                   - inData[c2 * def::spLength + k * graphScale] * norm),
                    QPointF(X + k + 1,
                            Y - graphHeight / 2.
                            - inData[c2 * def::spLength + (k + 1) * graphScale] * norm));
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


// make valarray
void readFileInLine(const QString & filePath, vec & outData)
{
    ifstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "readFileInLine: bad file " << filePath << endl;
        return;
    }
    outData.clear();
    double tmp;
    int num = 0;
    while(!file.eof())
    {
        file >> tmp;
        outData.push_back(tmp);

        // test
        ++num;
        if(num > def::fftLength * def::nsWOM())
        {
            cout << 23864 << endl;
            exit(0);
        }
    }
    outData.pop_back(); ///// prevent doubling last item (eof) bicycle
    file.close();
}

void writeFileInLine(const QString & filePath, const vec & outData)
{
    ofstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    for(auto out : outData)
    {
        file << doubleRound(out, 4) << '\t'; // \t or \n
    }
    file.close();
}

template <typename Typ>
void drawArrays(const QString & templPath,
                const Typ & inMatrix, // [] [] []
                const bool weightsFlag,
                const spectraGraphsNormalization normType,
                const QStringList & colors,
                const double scaling,
                const int lineWidth)
{
    QSvgGenerator svgGen;
    QSvgRenderer svgRen;
    QPixmap pic;
    QPainter paint;
    QString helpString;
    int numOfChan = 19;

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

    // test size
    int shouldSize = numOfChan * def::spLength;

    std::for_each(inMatrix.begin(),
                  inMatrix.end(),
                  [shouldSize](vec inData)
    {
        if(inData.size() != shouldSize)
        {
            cout << "inappropriate array size" << endl;
            return;
        }
    });

    // norm <- maxValue;
    // norm = inMatrix.maxVal();
    double norm = 0.;
    std::for_each(inMatrix.begin(),
                  inMatrix.end(),
                  [&norm](vec inData)
    {
        std::for_each(inData.begin(),
                      inData.end(),
                      [&norm](double in)
        {
            norm = fmax(norm, fabs(in)); // fabs for negative weights e.g.
        });
    });

    if(weightsFlag)
    {
        // for weights
        norm *= 2;
    }

    const double graphHeight = paint.device()->height() * coords::scale;
    const double graphWidth = paint.device()->width() * coords::scale;
    const double graphScale = def::spLength / graphWidth;
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
            for(int i = 0; i < inMatrix.size(); ++i)
            {
                for(int j = 0; j < def::spLength; ++j)
                {
                    norm = fmax(norm,
                                fabs(inMatrix[i][def::spLength * c2 + j])
                           * (1. + (j > 0.7 * def::spLength) * 0.7) );
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


        for(int numVec = 0; numVec < inMatrix.size(); ++numVec)
        {
            vec inData = inMatrix[numVec];
            //draw spectra
            for(int k = 0; k < graphWidth - 1; ++k)
            {
                paint.setPen(QPen(QBrush(QColor(colors[numVec])), lineWidth));
                if(weightsFlag)
                {
                    // weights
                    paint.drawLine(QPointF(X + k,
                                           Y - graphHeight / 2.
                                           - inData[c2 * def::spLength + k * graphScale] * norm),
                            QPointF(X + k + 1,
                                    Y - graphHeight / 2.
                                    - inData[c2 * def::spLength + (k + 1) * graphScale] * norm));

                }
                else
                {
                    // usual
                    paint.drawLine(QPointF(X + k,
                                           Y - inData[c2 * def::spLength + k * graphScale] * norm),
                            QPointF(X + k + 1,
                                    Y - inData[c2 * def::spLength + (k + 1) * graphScale] * norm));
                }



            }
        }
    }

    //returning norm = max magnitude
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
    pic.save(templPath, 0, 100);

}

void drawArraysInLine(const QString & picPath,
                      const matrix & inMatrix,
                      const QStringList & colors,
                      const double scaling,
                      const int lineWidth)
{
    QPixmap pic(inMatrix.cols(), inMatrix.cols() / 3); // generality
    pic.fill();
    QPainter pnt;
    pnt.begin(&pic);


    const double offsetY = 1 - 0.1;
    const double norm = inMatrix.maxVal() * scaling * offsetY; /// check scaling * or /

    for(int k = 0; k < inMatrix.rows(); ++k)
    {
        pnt.setPen(QPen(QBrush(QColor(colors[k])), lineWidth));
        for(int i = 0; i < pic.width() - 1; ++i)
        {
            pnt.drawLine(i, (offsetY - inMatrix[k][i] / norm) * pic.height(),
                         i + 1, (offsetY - inMatrix[k][i + 1] / norm) * pic.height());
        }
    }
    pnt.setPen(QPen(QBrush(QColor("black")), 1));
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        pnt.drawLine(i * pic.width() / def::nsWOM(), offsetY * pic.height(),
                     i * pic.width() / def::nsWOM(), 0);
    }

    pic.save(picPath, 0, 100);
    pnt.end();

}



///////////////////////// scales and shit
///
///
void drawCutOneChannel(const QString & inSpectraPath,
                       const int numChan)
{
    QString outPath = inSpectraPath;
    outPath.replace(".", QString("_" + QString::number(numChan) + "."));

    QPixmap pic = QPixmap(inSpectraPath);
    QPixmap cut = pic.copy(QRect(coords::x[numChan] * pic.width() - 20,
                                 coords::y[numChan] * pic.height() - 250 - 10,
                                 250 + 38,
                                 250 + 53));

    cut.save(outPath, 0, 100);

}



void countMannWhitney(trivector<int> & outMW,
                      const QString & spectraPath,
                      matrix * averageSpectraOut,
                      matrix * distancesOut)
{
//    const int numOfClasses = def::fileMarkers.length();
    const int numOfClasses = def::numOfClasses;

    const int NetLength = def::nsWOM() * def::spLength;

    QString helpString;
    const QDir dir_(spectraPath);
    QStringList lst[numOfClasses]; //0 - Spatial, 1 - Verbal, 2 - Rest
    matrix spectra[numOfClasses];

    matrix averageSpectra(numOfClasses, NetLength, 0);
    matrix distances(numOfClasses, numOfClasses, 0);

    for(int i = 0; i < numOfClasses; ++i)
    {
        QStringList nameFilters;
        QStringList leest;
        leest = def::fileMarkers[i].split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(int j = 0; j < leest.length(); ++j)
        {
            helpString = "*" + leest[j] + "*";
            nameFilters << helpString;
        }
        lst[i] = dir_.entryList(nameFilters, QDir::Files);


        spectra[i].resize(lst[i].length());
        for(int j = 0; j < lst[i].length(); ++j)
        {
            helpString = dir_.absolutePath() + slash() + lst[i][j];
            readFileInLine(helpString, spectra[i][j]);
        }
        averageSpectra[i] = spectra[i].averageRow();

        spectra[i].transpose();
    }


#if 1
    // trivector
    outMW.resize(numOfClasses);
    for(int i = 0; i < numOfClasses; ++i)
    {
        outMW[i].resize(numOfClasses);
        for(int j = i + 1; j < numOfClasses; ++j)
        {
            outMW[i][j - i].resize(NetLength);
            int dist1 = 0;
            for(int k = 0; k < NetLength; ++k)
            {
                outMW[i][j - i][k] = MannWhitney(spectra[i][k],
                                                 spectra[j][k]);
                if(outMW[i][j - i][k] != 0)
                {
                    ++dist1;
                }
            }
            distances[i][j - i] = dist1 / double(NetLength);
        }
    }
#else
    /// twovector not ready
    outMW.resize((numOfClasses * (numOfClasses - 1)) / 2 );
    for(int i = 0; i < outMW.size(); ++i)
    {
        outMW[i].resize(NetLength);
        int dist1 = 0;
        for(int k = 0; k < NetLength; ++k)
        {
            outMW[i][k] = MannWhitney(spectra[i][k],
                                      spectra[j][k]);
            if(outMW[i][j - i][k] != 0)
            {
                ++dist1;
            }
        }
        distances[i][j - i] = dist1 / double(NetLength);

    }
#endif

    if(averageSpectraOut != nullptr)
    {
        (*averageSpectraOut) = std::move(averageSpectra);
    }
    if(distancesOut != nullptr)
    {
        (*distancesOut) = std::move(distances);
    }
}

void drawMannWitney(const QString & templPath,
                    const trivector<int> & inMW,
                    const QStringList & inColors)
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
    const double ext = def::spLength / graphWidth;

    for(int c2 = 0; c2 < def::nsWOM(); ++c2)  //exept markers channel
    {
        const double X = paint.device()->width() * coords::x[c2];
        const double Y = paint.device()->height() * coords::y[c2];

        //statistic difference bars
        int barCounter = 0;
        for(int h = 0; h < def::numOfClasses; ++h) // class1
        {

            for(int l = h + 1; l < def::numOfClasses; ++l) // class2
            {
                QColor color1 = QColor(inColors[h]);
                QColor color2 = QColor(inColors[l]);

                for(int j = c2 * def::spLength; j < (c2 + 1) * def::spLength; ++j)
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

                    paint.drawRect(X + (j % def::spLength - barWidth) / ext,
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
                          const QStringList & inColors)
{
    QPixmap pic;
    pic.load(picPath);
    QPainter pnt;
    pnt.begin(&pic);

    /// same as drawArraysInLine
    const double offsetY = 1 - 0.1;

    int num = 0;
    for(int k = 0; k < inMW.size(); ++k)
    {
        for(int j = k + 1; j < inMW.size(); ++j)
        {
            const int currTop = pic.height() * (offsetY + (1. - offsetY) * num);
            const int currBot = pic.height() * (offsetY + (1. - offsetY) * (num + 1));
            const int currHeight = currBot - currTop;


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


void kernelEst(QString filePath, QString picPath)
{
    vec arr;
    readFileInLine(filePath, arr);
    kernelEst(arr, picPath);
}

void svd(const mat & inData,
         matrix & eigenVectors,
         vec & eigenValues,
         const double & threshold)
{
    const int iterationsThreshold = 100;
    const int ns = def::nsWOM();
    const int dataLen = inData[0].size();
    const int errorStep = 10;


    matrix tempData = matrix(inData);
    matrix covMatrix(ns, ns, 0.);

//    double ** tempData = new double * [ns];
//    double ** covMatrix = new double * [ns];
//    for(int i = 0; i < ns; ++i)
//    {
//        covMatrix[i] = new double [ns];
//        tempData[i] = new double [dataLen];
//        memcpy(tempData[i], inData[i].data(), sizeof(double) * dataLen);
//    }

    vec averages;
//    double * averages = new double [ns];
    for(int i = 0; i < ns; ++i)
    {
        averages.push_back(mean(inData[i].data(), dataLen));
//        averages[i] = mean(inData[i].data(), dataLen);
    }
    //count zeros
    int h = 0;
    int Eyes = 0;
    for(int i = 0; i < dataLen; ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(fabs(inData[j][i]) == 0.) ++h;
        }
        if(h == ns) Eyes += 1;
    }
    double realSignalFrac = dataLen / double(dataLen - Eyes); ///deprecate!!!!!!

    //subtract averages
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < dataLen; ++j)
        {
            if(tempData[i][j] != 0.)
            {
                tempData[i][j] = (tempData[i][j] - averages[i]) * realSignalFrac;
            }
        }
    }

    //count covMatrix
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            covMatrix[i][j] = 0.;
            covMatrix[i][j] = covariance(tempData[i],
                                         tempData[j],
                                         dataLen);
            covMatrix[i][j] /= dataLen; //should be -1 ? needed for trace
        }
    }

    double trace = 0.;
    for(int i = 0; i < ns; ++i)
    {
        trace += covMatrix[i][i];
    }

    eigenValues.resize(ns);
    eigenVectors.resize(ns, ns);

    vec tempA(ns);
    vec tempB(dataLen);
//    double * tempA = new double [ns];
//    double * tempB = new double [dataLen];

    double sum1, sum2; //temporary help values
    double dF, F;
    int counter;

    QTime myTime;
    myTime.start();
    //counter j - for B, i - for A
    for(int k = 0; k < ns; ++k)
    {
        myTime.restart();
        dF = 0.5;
        F = 1.0;

        //set 1-normalized vector tempA
        sum1 = 1. / sqrt(ns);
        for(int i = 0; i < ns; ++i)
        {
            tempA[i] = sum1;
        }
        sum2 = 1. / sqrt(dataLen);
        for(int j = 0; j < dataLen; ++j)
        {
            tempB[j] = sum2;
        }


        //approximate P[i] = tempA x tempB;
        counter = 0;

        while(1) //when stop approximate?
        {

            if((counter + 1) % errorStep == 0)
            {
                //countF - error
                F = 0.;
                for(int i = 0; i < ns; ++i)
                {
                    for(int j = 0; j < dataLen; ++j)
                    {
                        F += 0.5 * pow(tempData[i][j] - tempB[j] * tempA[i], 2.);
                    }
                }
            }

            //count vector tempB
            sum2 = 0.;
            for(int i = 0; i < ns; ++i)
            {
                sum2 += pow(tempA[i], 2);
            }
            sum2 = 1. / sum2; // to save time, multiplication faster than division

            for(int j = 0; j < dataLen; ++j)
            {
                sum1 = 0.;
                for(int i = 0; i < ns; ++i)
                {
                    sum1 += tempData[i][j] * tempA[i];
                }
                tempB[j] = sum1 * sum2;
            }

            //count vector tempA

            sum2 = 0.;
            for(int j = 0; j < dataLen; ++j)
            {
                sum2 += tempB[j] * tempB[j];
            }
            sum2 = 1. / sum2; // to save time, multiplication faster than division

            for(int i = 0; i < ns; ++i)
            {
                sum1 = 0.;
                for(int j = 0; j < dataLen; ++j)
                {
                    sum1 += tempB[j] * tempData[i][j];
                }
                tempA[i] = sum1 * sum2;
            }

            if((counter + 1) % errorStep == 0)
            {
                dF = 0.;
                for(int i = 0; i < ns; ++i)
                {
                    for(int j = 0; j < dataLen; ++j)
                    {
                        dF += 0.5 * pow((tempData[i][j] - tempB[j] * tempA[i]), 2.);
                    }
                }

                dF = (F-dF)/F;
            }


            if(counter == iterationsThreshold)
            {
                break;
            }
            ++counter;
            if(fabs(dF) < threshold) break; //crucial cap

        }

        //edit covMatrix
        for(int i = 0; i < ns; ++i)
        {
            for(int j = 0; j < dataLen; ++j)
            {
                tempData[i][j] -= tempB[j] * tempA[i];
            }
        }

        //count eigenVectors && eigenValues
        sum1 = 0.;
        sum2 = 0.;
        for(int i = 0; i < ns; ++i)
        {
            sum1 += pow(tempA[i], 2.);
        }
        for(int j = 0; j < dataLen; ++j)
        {
            sum2 += pow(tempB[j], 2.);
        }
        eigenValues[k] = sum1 * sum2 / double(dataLen - 1.);

        sum1 = 1. / sqrt(sum1);
        for(int i = 0; i < ns; ++i)
        {
            tempA[i] *= sum1;
        }

        sum1 = 0.;
        for(int i = 0; i <= k; ++i)
        {
            sum1 += eigenValues[i];
        }

        cout << k << "\t";
        cout << "val = " << doubleRound(eigenValues[k], 3) << "\t";
        cout << "disp = " << doubleRound(100. * eigenValues[k] / trace, 2) << "\t";
        cout << "total = " << doubleRound(100. * sum1 / trace, 2) << "\t";
        cout << "iters = " << counter << "\t";
        cout << doubleRound(myTime.elapsed()/1000., 1) << " s" << endl;

        for(int i = 0; i < ns; ++i)
        {
            eigenVectors[i][k] = tempA[i]; //1-normalized coloumns
        }
    }
}

void wavelet(QString filePath,
             QString picPath,
             int channelNumber,
             int ns)
{
    // continious
    int NumOfSlices;
    double helpDouble;

    matrix fileData;
    readPlainData(filePath, fileData, ns, NumOfSlices);

    vec input = fileData[channelNumber];

    QPixmap pic(NumOfSlices, 1000);
    pic.fill();
    QPainter painter;
    painter.begin(&pic);

    matrix temp;
    temp.resize(wvlt::numberOfFreqs,
                ceil(NumOfSlices / wvlt::timeStep) + 1);
    temp.fill(0.);

    //    mat temp;
    //    temp.resize(wvlt::numberOfFreqs);
    //    const int num = ceil(NumOfSlices / wvlt::timeStep);
    //    std::for_each(temp.begin(),
    //                  temp.end(),
    //                  [num](vec & in){in.resize(num, 0.);});

    double tempR = 0., tempI = 0.;
    int kMin = 0, kMax = 0;

    double numb;

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
        //        timeStep = def::freq/freq / 2.5;  //in time-bins 250 Hz
        currSliceNum = 0;
        for(int currSlice = 0; currSlice < NumOfSlices; currSlice += wvlt::timeStep)
        {
            temp[currFreqNum][currSliceNum] = 0.;
            tempR = 0.;
            tempI = 0.;

            /////// TO LOOK
            //set left & right limits of counting - should be 2.5 * morletFall... but works so
            kMin = max(0, int(currSlice - 3 * morletFall * def::freq / freq));
            kMax = min(NumOfSlices, int(currSlice + 3 * morletFall * def::freq / freq));

            for(int k = kMin; k < kMax; ++k)
            {
                tempI += (morletSinNew(freq, currSlice, k) * input[k]);
                tempR += (morletCosNew(freq, currSlice, k) * input[k]);
            }
            temp[currFreqNum][currSliceNum] = pow(tempI, 2) + pow(tempR, 2);
            ++currSliceNum;
        }
        ++currFreqNum;
    }

    // maximal value from temp matrix
    helpDouble = temp.maxVal();
    //    helpDouble = 800000;

    //    cout << helpDouble << endl;
    //    helpDouble = 1e5;

    //    /// test for maxVal
    //    ofstream str;
    //    str.open("/media/Files/Data/wav.txt", ios_base::app);
    //    str << helpDouble << endl;
    //    str.close();
    //    return;


    currFreqNum = 0;
    for(double freq = wvlt::freqMax;
        freq > wvlt::freqMin;
    #if WAVELET_FREQ_STEP_TYPE==0
        freq *= wvlt::freqStep
    #else
        freq -= wvlt::freqStep)
#endif
    {
        //        timeStep = def::freq/freq / 2.5;  //in time-bins 250 Hz

        currSliceNum = 0;
        for(int currSlice = 0; currSlice < NumOfSlices; currSlice += wvlt::timeStep)
        {
            //            cout << temp[currFreqNum][currSliceNum] << endl;
            numb = fmin(floor(temp[currFreqNum][currSliceNum] / helpDouble * wvlt::range),
                        wvlt::range);

            //             numb = pow(numb/wvlt::range, 0.8) * wvlt::range; // sligthly more than numb, may be dropped

            painter.setBrush(QBrush(hueJet(wvlt::range, numb)));
            painter.setPen(hueJet(wvlt::range, numb));

#if WAVELET_FREQ_STEP_TYPE==0
            painter.drawRect( currSlice * pic.width() / NumOfSlices,
                              pic.height() * (wvlt::freqMax - freq
                                              + 0.5 * freq *
                                              (1. - wvlt::freqStep) / wvlt::freqStep)
                              / (wvlt::freqMax-wvlt::freqMin),
                              pic.width() wvlt::timeStep / NumOfSlices,
                              pic.height() * -0.5 * freq * (1. / wvlt::freqStep - wvlt::freqStep)
                              / (wvlt::freqMax - wvlt::freqMin) );
#else

            painter.drawRect( pic.width() * currSlice / NumOfSlices,
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



matrix waveletDiscrete(const vec & inData)
{
    const int numOfFreqs = log2(def::rightFreq / def::leftFreq) + 1;
    matrix res(numOfFreqs, inData.size());

    int numOfShifts;
    for(int j = 0; j < numOfFreqs; ++j)
    {
        numOfShifts = 0;
        for(int k = 0; k < numOfShifts; ++k)
        {
            res[j][k] = 0.;
        }
    }
}

vec signalFromFile(QString filePath,
                   int channelNumber,
                   int ns)
{
    matrix tempMat;
    int tempInt;
    readPlainData(filePath,
                  tempMat,
                  ns,
                  tempInt);
    vec res = tempMat[channelNumber];
    return res;
}
matrix countWavelet(vec inSignal)
{
    // continious
    int NumOfSlices = inSignal.size();

    matrix res;
    res.resize(wvlt::numberOfFreqs,
               ceil(NumOfSlices / wvlt::timeStep) + 1);
    res.fill(0.);

    double tempR = 0., tempI = 0.;
    int kMin = 0, kMax = 0;

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
        //        timeStep = def::freq/freq / 2.5;  //in time-bins 250 Hz
        currSliceNum = 0;
        for(int currSlice = 0; currSlice < NumOfSlices; currSlice += wvlt::timeStep)
        {
            res[currFreqNum][currSliceNum] = 0.;
            tempR = 0.;
            tempI = 0.;

            /////// TO LOOK
            //set left & right limits of counting - should be 2.5 * morletFall... but works so
            kMin = max(0, int(currSlice - 3 * morletFall * def::freq / freq));
            kMax = min(NumOfSlices, int(currSlice + 3 * morletFall * def::freq / freq));

            for(int k = kMin; k < kMax; ++k)
            {
                tempI += (morletSinNew(freq, currSlice, k) * inSignal[k]);
                tempR += (morletCosNew(freq, currSlice, k) * inSignal[k]);
            }
            res[currFreqNum][currSliceNum] = (pow(tempI, 2) + pow(tempR, 2)) / 1e5; // ~1

            ++currSliceNum;
        }
        ++currFreqNum;
    }
    // no normalization
    return res;
}
void drawWavelet(QString picPath,
                 const matrix & inData)
{
    int NumOfSlices = ceil(inData.cols() * wvlt::timeStep);
    QPixmap pic(NumOfSlices, 1000);
    pic.fill();
    QPainter painter;
    painter.begin(&pic);

    double numb;

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

            //             numb = pow(numb/wvlt::range, 0.8) * wvlt::range; // sligthly more than numb, may be dropped

            painter.setBrush(QBrush(hueJet(wvlt::range, numb)));
            painter.setPen(hueJet(wvlt::range, numb));

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



vec hilbert(const vec & arr,
            double lowFreq,
            double highFreq,
            QString picPath)
{

    int inLength = arr.size();
    int fftLen = fftL(inLength); // int(pow(2., ceil(log(inLength)/log(2.))));
    double spStep = def::freq / fftLen;
    double normCoef = sqrt(fftLen / double(inLength));

    vec out; // result
    out.resize(2*fftLen);

    vec tempArr;
    tempArr.resize(fftLen, 0.);

    vec filteredArr;
    filteredArr.resize(fftLen, 0.);


    for(int i = 0; i < inLength; ++i)
    {
        out[ 2 * i + 0] = arr[i] * normCoef;
        out[ 2 * i + 1] = 0.;
    }
    for(int i = inLength; i < fftLen; ++i)
    {
        out[ 2 * i + 0] = 0.;
        out[ 2 * i + 1] = 0.;
    }
    four1(out, fftLen, 1);
    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2. * lowFreq / spStep
           || i > 2. * highFreq / spStep)
            out[i] = 0.;
    }
    for(int i = fftLen; i < 2 * fftLen; ++i)
    {
        if(((2 * fftLen - i) < 2. * lowFreq / spStep)
           || (2 * fftLen - i > 2. * highFreq / spStep))
            out[i] = 0.;
    }
    out[0] = 0.;
    out[1] = 0.;
    out[fftLen] = 0.;
    out[fftLen+1] = 0.;
    //end filtering

    four1(out, fftLen, -1);
    for(int i = 0; i < inLength; ++i)
    {
        filteredArr[i] = out[2 * i] / fftLen / normCoef;
    }


    //Hilbert via FFT
    for(int i = 0; i < inLength; ++i)
    {
        out[2 * i + 0] = filteredArr[i] * normCoef;
        out[2 * i + 1] = 0.;
    }
    for(int i = inLength; i < fftLen; ++i)
    {
        out[2 * i + 0] = 0.;
        out[2 * i + 1] = 0.;
    }
    four1(out, fftLen, 1);

    for(int i = 0; i < fftLen / 2; ++i)
    {
        out[2 * i + 0] = 0.;
        out[2 * i + 1] = 0.;
    }
    four1(out, fftLen, -1);

    for(int i = 0; i < inLength; ++i)
    {
        tempArr[i] = out[2 * i + 1] / fftLen * 2; //hilbert
    }
    //end Hilbert via FFT


    for(int i = 0; i < fftLen; ++i)
    {
        out[i] = sqrt(pow(tempArr[i], 2) + pow(filteredArr[i], 2));
    }


    if(!picPath.isEmpty())
    {

        //start check draw - OK
        QPixmap pic(fftLen, 600);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);
        //    double sum, sum2;
        double enlarge = 10.;

        pnt.setPen("black");
        for(int i = 0; i < pic.width() - 1; ++i)
        {
            pnt.drawLine(i,
                         pic.height() / 2. - enlarge * filteredArr[i],
                         i+1,
                         pic.height() / 2. - enlarge * filteredArr[i + 1]);
        }
#if 0
        pnt.setPen("blue");
        for(int i = 0; i < pic.width() - 1; ++i)
        {
            pnt.drawLine(i,
                         pic.height() / 2. - enlarge * tempArr[i],
                         i+1,
                         pic.height() / 2. - enlarge * tempArr[i + 1]);
        }
#endif
        pnt.setPen("green");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt.drawLine(i,
                         pic.height() / 2. - enlarge * out[i],
                         i+1,
                         pic.height() / 2. - enlarge * out[i + 1]);
        }

        pic.save(picPath, 0, 100);
        pic.fill();
        pnt.end();
        cout << "hilber drawn" << endl;
        //end check draw
    }
    return out;
}


vec hilbertPieces(const vec & arr,
                   int inLength,
                   double sampleFreq,
                   double lowFreq,
                   double highFreq,
                   QString picPath)
{
    /// do hilbert transform for the last fftLen bins ???
    if( inLength <= 0)
    {
        inLength = arr.size();
    }
    vec outHilbert(inLength);
    int fftLen = fftL(inLength) / 2;

    double spStep = sampleFreq/fftLen;
    vec out(2*fftLen); // temp
    vec tempArr(inLength);
    vec filteredArr(inLength);

    for(int i = 0; i < fftLen; ++i)
    {
        out[ 2 * i + 0] = arr[i];
        out[ 2 * i + 1] = 0.;
    }
    four1(out, fftLen, 1);
    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2.*lowFreq/spStep || i > 2.*highFreq/spStep)
            out[i] = 0.;
    }
    for(int i = fftLen; i < 2*fftLen; ++i)
    {
        if(((2*fftLen - i) < 2.*lowFreq/spStep) || (2*fftLen - i > 2.*highFreq/spStep))
            out[i] = 0.;
    }
    out[0] = 0.;
    out[1] = 0.;
    out[fftLen] = 0.;
    out[fftLen+1] = 0.;

    four1(out, fftLen, -1);
    for(int i = 0; i < fftLen; ++i)
    {
        filteredArr[i] = out[2*i] / fftLen*2;
    }
    //end filtering



    //Hilbert via FFT
    for(int i = 0; i < fftLen; ++i)
    {
        out[ 2 * i + 0] = filteredArr[i];
        out[ 2 * i + 1] = 0.;
    }
    four1(out, fftLen, 1);

    for(int i = 0; i < fftLen/2; ++i)
    {
        out[2*i + 0] = 0.;
        out[2*i + 1] = 0.;
    }
    four1(out, fftLen, -1);

    for(int i = 0; i < fftLen; ++i)
    {
        tempArr[i] = out[2*i+1] / fftLen*2; //hilbert
    }
    //end Hilbert via FFT

    for(int i = 0; i < fftLen; ++i)
    {
        outHilbert[i] = pow(pow(tempArr[i], 2.) + pow(filteredArr[i], 2.), 0.5);
    }





    //second piece
    for(int i = 0; i < fftLen; ++i)
    {
        out[ 2 * i + 0] = arr[i + inLength-fftLen];
        out[ 2 * i + 1] = 0.;
    }
    four1(out, fftLen, 1);
    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2.*lowFreq/spStep || i > 2.*highFreq/spStep)
            out[i] = 0.;
    }
    for(int i = fftLen; i < 2*fftLen; ++i)
    {
        if(((2*fftLen - i) < 2.*lowFreq/spStep) || (2*fftLen - i > 2.*highFreq/spStep))
            out[i] = 0.;
    }
    out[0] = 0.;
    out[1] = 0.;
    out[fftLen] = 0.;
    out[fftLen+1] = 0.;

    four1(out, fftLen, -1);
    for(int i = 0; i < fftLen; ++i)
    {
        filteredArr[i + inLength - fftLen] = out[2*i] / fftLen*2;
    }
    //end filtering


    //Hilbert via FFT
    for(int i = 0; i < fftLen; ++i)
    {
        out[ 2 * i + 0] = filteredArr[i + inLength - fftLen];
        out[ 2 * i + 1] = 0.;
    }
    four1(out, fftLen, 1);
    for(int i = 0; i < fftLen/2; ++i)
    {
        out[2*i + 0] = 0.;
        out[2*i + 1] = 0.;
    }
    four1(out, fftLen, -1);

    for(int i = 0; i < fftLen; ++i)
    {
        tempArr[i + inLength - fftLen] = out[2*i+1] / fftLen*2; //hilbert
    }
    //end Hilbert via FFT


    int startReplace = 0;
    double minD = 100.;

    double helpDouble = 0.;
    for(int i = inLength - fftLen; i < fftLen; ++i)
    {
        helpDouble = outHilbert[i] - pow(pow(tempArr[i], 2.) +
                                         pow(filteredArr[i], 2.), 0.5);
        if(fabs(helpDouble) <= fabs(minD))
        {
            minD = helpDouble;
            startReplace = i;
        }
    }

    for(int i = startReplace; i < inLength; ++i)
    {
        outHilbert[i] = pow(pow(tempArr[i], 2.) + pow(filteredArr[i], 2.), 0.5);
    }


    if(!picPath.isEmpty())
    {

        //start check draw - OK
        QPixmap pic(inLength,600);
        QPainter pnt;
        pic.fill();
        pnt.begin(&pic);
        double maxVal = *std::max_element(filteredArr.begin(), filteredArr.end()) * 1.1;

        pnt.setPen("black");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt.drawLine(i,
                         pic.height()/2. * (1. - filteredArr[i] / maxVal),
                         i+1,
                         pic.height()/2. * (1. - filteredArr[i + 1] / maxVal)
                    );
        }

        //        pnt.setPen("blue");
        //        for(int i = 0; i < pic.width()-1; ++i)
        //        {
        //            pnt.drawLine(i, pic.height()/2. - enlarge * tempArr[i], i+1, pic.height()/2. - enlarge * tempArr[i+1]);
        //        }

        pnt.setPen("green");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt.drawLine(i,
                         pic.height()/2. * (1. - outHilbert[i] / maxVal),
                         i+1,
                         pic.height()/2. * (1. - outHilbert[i + 1] / maxVal)
                    );
        }

        pnt.setPen("blue");
        pnt.drawLine(startReplace,
                     pic.height(),
                     startReplace,
                     0.);

        pnt.end();
        pic.save(picPath, 0, 100);
        //end check draw
    }
    return outHilbert;
}

vec bayesCount(const vec & dataIn,
               int numOfIntervals)
{
    double maxAmpl = 80.; //generality
    int helpInt;
    vec out(numOfIntervals, 0.);

    for(int j = 0; j < dataIn.size(); ++j)
    {
        helpInt = int(floor((dataIn[j] + maxAmpl) / (2. * maxAmpl / double(numOfIntervals))));

        if(helpInt != min(max(0, helpInt), numOfIntervals - 1))
        {
            continue; //if helpInt not in range
        }

        out[helpInt] += 1;
    }
    for(int k = 0; k < numOfIntervals; ++k)
    {
        out[k] /= double(dataIn.size()) * 10.; // 10 is norm coef for perceptron
    }
    return out;
}

void histogram(const vec & arr, int numSteps, QString picPath)
{
    vec values(numSteps, 0.);
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


void kernelEst(const vec &arr, QString picPath)
{
    double sigma = 0.;
    int length = arr.size();

    sigma = variance(arr, length);
    sigma = sqrt(sigma);
    double h = 1.06 * sigma * pow(length, -0.2);

    QPixmap pic(1000, 400);
    QPainter pnt;
    pic.fill();
    pnt.begin(&pic);
    pnt.setPen("black");

    vec values(pic.width(), 0.);

    double xMin, xMax;

    xMin = *std::min(arr.begin(),
                    arr.end());
    xMax = *std::max(arr.begin(),
                    arr.end());

    xMin = floor(xMin);
    xMax = ceil(xMax);

    //    sigma = (xMax - xMin);
    //    xMin -= 0.1 * sigma;
    //    xMax += 0.1 * sigma;

    //    //generality
    //    xMin = -20;
    //    xMax = 20;

    //    xMin = 65;
    //    xMax = 100;


    for(int i = 0; i < pic.width(); ++i)
    {
        for(int j = 0; j < length; ++j)
        {
            values[i] += 1 / (length * h)
                         * gaussian((xMin + (xMax - xMin) / double(pic.width()) * i - arr[j]) / h);
        }
    }

    double valueMax;
    valueMax = *std::max(values.begin(),
                        values.end());

    for(int i = 0; i < pic.width() - 1; ++i)
    {
        pnt.drawLine(i,
                     pic.height() * 0.9 * ( 1. - values[i] / valueMax),
                     i + 1,
                     pic.height() * 0.9 * (1. - values[i + 1] / valueMax));

    }
    pnt.drawLine(0,
                 pic.height() * 0.9,
                 pic.width(),
                 pic.height() * 0.9);

    // x markers - 10 items
    for(int i = xMin; i <= xMax; i += (xMax - xMin) / 10)
    {
        pnt.drawLine((i - xMin) / (xMax - xMin) * pic.width(),
                     pic.height() * 0.9,
                     (i - xMin) / (xMax - xMin) * pic.width(),
                     pic.height());
        pnt.drawText((i - xMin) / (xMax - xMin) * pic.width(),
                     pic.height() * 0.95,
                     QString::number(i));
    }
    pic.save(picPath, 0, 100);
}


bool gaussApproval(double * arr, int length) //kobzar page 239
{
    double z;
    int m = int(length/2);
    double a[m+1];
    double disp = variance(arr, length) * length;
    double B = 0.;
    double W;

    a[0] = 0.899/pow(length-2.4, 0.4162) - 0.02;
    for(int j = 1; j <= m; ++j)
    {
        z = (length - 2*j + 1.) / (length - 0.5);
        a[j] = a[0] * (z + 1483 / pow(3.-z, 10.845) + pow(71.61, -10.) / pow(1.1-z, 8.26));
        B += a[j] * (arr[length-j+1] - arr[j-1]); //or without +1
    }
    B *= B;
    W = (1 - 0.6695 / pow(length, 0.6518)) * disp / B;

    if(W < 1.) return true;
    return false;
}


bool gaussApproval(QString filePath)
{
    vec arr;
    readFileInLine(filePath, arr);
    return gaussApproval(arr.data(), arr.size());
}

bool gaussApproval2(double * arr, int length) //kobzar page 238
{
    double W = 0.;
    double disp = variance(arr, length) * length;
    double c[length+1];
    double sum = 0.;
    for(int i = 1; i <= length; ++i)
    {
        sum += pow(rankit(i, length), 2.);
    }
    sum = sqrt(sum);
    for(int j = 1; j < length; ++j)
    {
        c[j] = rankit(length - j + 1, length) / sum;
        W += c[j] * (arr[length - j] - arr[j - 1]);
    }
    W /= disp;

    /// NOT READY
    return false;


}

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
    return QColor(255.*red(range,j), 255.*green(range,j), 255.*blue(range,j));
}

QColor grayScale(int range, int j)
{
    if(j > range) j = range;
    if(j < 0) j = 0;
    return QColor(255. * (1. - double(j)/range), 255. * (1. - double(j)/range), 255. * (1. -  double(j)/range));
}

QColor qcolor(int range, int j)
{
    double sigmaR = range*0.35;
    double sigmaG = range*0.25;
    double sigmaB = range*0.4;

    double offR = range*(1.0 - 0.3);
    double offG = range*(0.5 + 0.25);
    double offB = range*(0.0 + 0.15);

    return QColor(255*exp(-(j-offR)*(j-offR)/(2*sigmaR*sigmaR)), 255*exp(-(j-offG)*(j-offG)/(2*sigmaG*sigmaG)), 255*exp((-(j-offB)*(j-offB)/(2*sigmaB*sigmaB))));
    //    return QColor(255*red(part),255* green(part), 255*blue(part));
}

double morletCos(double const freq1, const double timeShift, const double pot, const double time)
{
    double freq = freq1 * 2. * pi / def::freq;
    return cos(freq * (time - timeShift)) * exp( - pow(freq * (time-timeShift) / pot, 2));
}

double morletSin(double const freq1, const double timeShift, const double pot, const double time)
{
    double freq = freq1 * 2. * pi / def::freq;
    return sin(freq * (time - timeShift)) * exp( - pow(freq * (time-timeShift) / pot, 2));
}


void matrixTranspose(double **&inMat, const int &numRowsCols)
{
    double ** tmp;
    matrixCreate(&tmp, numRowsCols, numRowsCols);

    //transpose to tmp
    for(int i = 0; i < numRowsCols; ++i)
    {
        for(int j = 0; j < numRowsCols; ++j)
        {
            tmp[i][j] = inMat[j][i];
        }
    }


    //tmp to inMat
    for(int i = 0; i < numRowsCols; ++i)
    {
        for(int j = 0; j < numRowsCols; ++j)
        {
            inMat[i][j] = tmp[i][j];
        }
    }
    matrixDelete(&tmp, numRowsCols);
}
/*
void waveletPhase(QString out, FILE * file, int ns=19, int channelNumber1=0, int channelNumber2=1, double freqMax=20., double freqMin=5., double freqStep=0.99, double pot=32.)
{
    int NumOfSlices;
    double * input1;
    double * input2;
    double helpDouble;
    QString helpString;

    if(file==NULL)
    {
        cout<<"file==NULL"<<endl;
        return;
    }
    fscanf(file, "NumOfSlices %d", &NumOfSlices);
    if(NumOfSlices<200) return;


    input1 = new double [NumOfSlices]; //first channel signal
    input2 = new double [NumOfSlices]; //second channel signal

    //read data
    for(int i=0; i<NumOfSlices; ++i)
    {
        for(int j=0; j<ns; ++j)
        {
            if(j==channelNumber1) fscanf(file, "%lf", &input1[i]);
            else if(j==channelNumber2) fscanf(file, "%lf", &input2[i]);
            else fscanf(file, "%lf", &helpDouble);
        }
    }
//    fclose(file);
    QPixmap pic(150*NumOfSlices/250,800); //150 pixels/sec
    pic.fill();
    QPainter painter;
    painter.begin(&pic);


    double freq=20.;
    double timeStep=0.;
    double * temp = new double[NumOfSlices];
    double tempR=0., tempI=0.;
    int i=0;
    int jMin=0, jMax=0;


//    cout<<"1"<<endl;
    for(freq=freqMax; freq>freqMin; freq*=freqStep)
    {
        timeStep = 1. / ( freq ) * 250./1.5;  //250 Hz
        i=0;
        while(i<NumOfSlices)
        {
            temp[i]=0.; //phase difference
            jMin=max(0, int(i - sqrt(5*pot*pot*250.*250./4/pi/pi/freq/freq)));
            jMax=min(int(i + sqrt(5*pot*pot*250.*250./4/pi/pi/freq/freq)), NumOfSlices);

            //count phase in the first channel
            tempR=0.;
            tempI=0.;
            for(int j=jMin; j<jMax; ++j)
            {
                tempI+=(morletSin(freq, i, pot, j)*input1[j]);
                tempR+=(morletCos(freq, i, pot, j)*input1[j]);
            }
            if(tempR != 0.) temp[i] = atan(tempI / tempR);
            else if(tempI > 0) temp[i] = pi/2.;
            else temp[i] = -pi/2.;

            //count phase in the second channel
            tempR=0.;
            tempI=0.;
            for(int j=jMin; j<jMax; ++j)
            {
                tempI+=(morletSin(freq, i, pot, j)*input2[j]);
                tempR+=(morletCos(freq, i, pot, j)*input2[j]);
            }
            if(tempR != 0.) helpDouble = atan(tempI / tempR);
            else if(tempI > 0) helpDouble = pi/2.;
            else helpDouble = -pi/2.;

            //count the difference
//            temp[i] -= helpDouble;
            i+=timeStep;
        }

        int range=500;

        i=0;
         while(i<NumOfSlices)
        {
             painter.setBrush(QBrush(hueJet(range, (temp[i] + pi)/2./pi * range)));
             painter.setPen(hueJet(range, (temp[i] + pi)/2./pi * range));

             painter.drawRect(i*pic.width()/NumOfSlices, int(pic.height()*(freqMax-freq  + 0.5*freq*(1. - freqStep)/freqStep)/(freqMax-freqMin)), timeStep*pic.width()/NumOfSlices,     int(pic.height()*(- 0.5*freq*(1./freqStep - freqStep))/(freqMax-freqMin)));
             i+=timeStep;
        }

    }
//    cout<<"2"<<endl;
    painter.setPen("black");


    painter.setFont(QFont("Helvetica", 32, -1, -1));
    for(int i=freqMax; i>freqMin; --i)
    {
        painter.drawLine(0, pic.height()*(freqMax-i)/(freqMax-freqMin), pic.width(), pic.height()*(freqMax-i)/(freqMax-freqMin));
        painter.drawText(0, pic.height()*(freqMax-i)/(freqMax-freqMin)-2, helpString.setNum(i));

    }
    for(int i=0; i<int(NumOfSlices/250); ++i)
    {
        painter.drawLine(pic.width()*i*250/NumOfSlices, pic.height(), pic.width()*i*250/NumOfSlices, pic.height()-20);
        painter.drawText(pic.width()*i*250/NumOfSlices-8, pic.height()-2, helpString.setNum(i));

    }


    delete []input1;
    delete []input2;
    delete []temp;

    pic.save(out, 0, 100);
    rewind(file);

    painter.end();
}
                   */

void writePlainData(const QString outPath,
                    const matrix &data,
                    const int & ns,
                    int numOfSlices,
                    const int & start)
{
    numOfSlices = min(numOfSlices,
                      data.cols() - start);

    if(numOfSlices < 250) return;

    ofstream outStr;
    outStr.open(outPath.toStdString());
    outStr << "NumOfSlices " << numOfSlices;
    outStr << "\tNumOfChannels " << ns;
    outStr << endl;
    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            outStr << doubleRound(data[j][i + start], 3) << '\t';
        }
        outStr << '\n';
    }
    outStr.flush();
    outStr.close();
}


void readPlainData(const QString & inPath,
                   matrix & data,
                   const int & ns,
                   int & numOfSlices,
                   const int & start)
{
    ifstream inStr;
    inStr.open(inPath.toStdString());
    if(!inStr.good())
    {
        cout << "readPlainData: cannot open file" << endl;
        return;
    }
    inStr.ignore(64, ' '); // "NumOfSlices "
    inStr >> numOfSlices;
    inStr.ignore(64, '\n'); // "NumOfChannels N\n"

    data.resize(ns, numOfSlices + start);

    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {

            inStr >> data[j][i + start];

            /// Ossadtchi
//            if(j == ns - 1 && def::OssadtchiFlag)
//            {
//                if(i == 0) data[j][i + start] = inPath.right(3).toDouble();
//                else if(i == numOfSlices-1) data[j][i + start] = 254;
//                else data[j][i + start] = 0.;
//            }

        }
    }
    inStr.close();
}

void spectre(const double * data, const int & length, double * & spectr)
{
    int fftLen = fftL(length); // nearest exceeding power of 2
    double norm = sqrt(fftLen / double(length));

    double * tempSpectre = new double [2*fftLen];
    for(int i = 0; i < length; ++i)
    {
        tempSpectre[ 2 * i + 0] = data[i] * norm;
        tempSpectre[ 2 * i + 1] = 0.;
    }
    for(int i = length; i < fftLen; ++i)
    {
        tempSpectre[ 2 * i + 0] = 0.;
        tempSpectre[ 2 * i + 1] = 0.;
    }
    four1(tempSpectre-1, fftLen, 1);


    spectr = new double [fftLen / 2];

    norm = 2. / (def::freq * fftLen);
    for(int i = 0; i < fftLen/2; ++i )      //get the absolute value of FFT
    {
        spectr[ i ] = (pow(tempSpectre[ i * 2 ], 2.) +
                      pow(tempSpectre[ i * 2 + 1 ], 2.)) * norm;
    }
}

vec spectre(const vec & data)
{
    int length = data.size();
    int fftLen = fftL(length); // nearest exceeding power of 2
    double norm = sqrt(fftLen / double(length));

    vec tempSpectre(2 * fftLen, 0.);
    for(int i = 0; i < length; ++i)
    {
        tempSpectre[ 2 * i + 0 ] = data[i] * norm;
    }
    four1(tempSpectre.data() - 1, fftLen, 1);

    vec spectr(fftLen / 2);
    norm = 2. / (def::freq * fftLen);
    for(int i = 0; i < fftLen / 2; ++i )      //get the absolute value of FFT
    {
        spectr[ i ] = (pow(tempSpectre[ i * 2 ], 2.)
                      + pow(tempSpectre[ i * 2 + 1 ], 2.)) * norm;
    }
    return spectr;
}

vec smoothSpectre(const vec & inSpectre, const int numOfSmooth)
{
    vec result = inSpectre;
    double help1, help2;
    for(int num = 0; num < numOfSmooth; ++num)
    {
        help1 = result[0];
        for(int i = 1; i < result.size() - 1; ++i)
        {
            help2 = result[i];
            result[i] = (help1 + help2 + result[i+1]) / 3.;
            help1 = help2;
        }
    }
    return result;
}

template <typename Typ>
void readSpectraFile(const QString & filePath,
                     Typ & outData,
                     int inNs,
                     int spL)
{
    ifstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "readSpectreFile: bad input file " << filePath << endl;
        return;
    }
    outData.resize(inNs);
    std::for_each(outData.begin(),
                  outData.end(),
                  [spL](vec & in){in.resize(spL);});

    for(int i = 0; i < inNs; ++i)
    {
        for(int j = 0; j < spL; ++j)
        {
            file >> outData[i][j];
        }
    }
    file.close();
}


template <typename Typ>
void writeSpectraFile(QString filePath,
                      const Typ & outData,
                      int inNs,
                      int spL)

{
    if(inNs > outData.size() ||
       spL > outData[0].size())
    {
        cout << "bad inputs while writing matrix" << endl;
        return;
    }

    ofstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "bad output file:\n" << filePath.toStdString() << endl;
        return;
    }

    for(int i = 0; i < inNs; ++i)
    {
        for(int j = 0; j < spL; ++j)
        {
            file << doubleRound(outData[i][j], 4) << '\t';
        }
        file << endl;
    }
    file.close();
}
template void writeSpectraFile(QString filePath, const mat & outData, int inNs, int spL);
template void writeSpectraFile(QString filePath, const matrix & outData, int inNs, int spL);



template <typename Typ>
void zeroData(Typ & inData, const int & leftLimit, const int & rightLimit)
{
    for(int i = leftLimit; i < rightLimit; ++i)
    {
        for(int k = 0; k < def::nsWOM(); ++k)
        {
            inData[k][i] = 0.;
        }
    }
}

template <typename Typ>
void splitZeros(Typ & dataIn,
                const int & inLength,
                int * outLength,
                const QString & logFilePath,
                const QString & dataName)
{
    //remake with std::list of slices
    vector<bool> flags(inLength + 1, false); //1 if usual, 0 if eyes, 1 for additional one point

    bool startFlag = false;
    int start = -1;
    int finish = -1;
    int allEyes = 0;

    ofstream outStream;
    int markChan = def::ns - 1; // generality markers channel

    const double firstMarker = dataIn[markChan][0];
    const double lastMarker =  dataIn[markChan][inLength - 1];

//    ofstream of("/media/Files/Data/Mati/BSA/flags.txt");
    for(int i = 0; i < inLength; ++i)
    {
        for(int j = 0; j < def::nsWOM(); ++j) // dont consider markers
        {
            if(dataIn[j][i] != 0.)
            {
                flags[i] = true;
                break;
            }
        }
//        of << flag[i] << endl;
    }
    flags[inLength] = true; // for terminate zero piece
//    of.close();




    if(!logFilePath.isEmpty())
    {
        outStream.open(logFilePath.toStdString(), ios_base::app);
        if(!outStream.good())
        {
            cout << "splitZeros: outStream is not good" << endl;
            return;
        }
    }
    else
    {
        cout << "splitZeros: logFilePath is empty" << endl;
        return;
    }

    //flag[i] == 0 if it's eyes-cut interval
    int i = 0;
    do
    {
        if(startFlag == false) //if startFlag == false
        {
            if(flags[i] == false) // set start
            {
                start = i; // first bin to exclude
                startFlag = true;
            }
        }
        else //if startFlag is set
        {
            if(flags[i]) // if we meet the end of eyes-interval OR reached end of a data
            {

                finish = i; // first bin NOT TO exclude
                outStream << dataName.toStdString() << "\t";
                outStream << start + allEyes << "\t";
                outStream << finish + allEyes << "\t";
                outStream << finish - start << "\t"; // length

                outStream << (start + allEyes) / def::freq << "\t"; // start time
                outStream << (finish + allEyes) / def::freq << "\t"; // finish time
                outStream << (finish - start) / def::freq << endl; // length

                //split. vector.erase();
                for(int j = 0; j < def::ns; ++j) //shift with markers and flags
                {
                    dataIn[j].erase(dataIn[j].begin() + start,
                                    dataIn[j].begin() + finish);
                }
                flags.erase(flags.begin() + start,
                            flags.begin() + finish);

                allEyes += finish - start;
                i -= (finish - start);
                startFlag = false;
            }
        }
        ++i;
    } while (i <= inLength - allEyes); // = for the last zero piece
    (*outLength) = inLength - allEyes;
    outStream.close();

//    cout << "allEyes = " << allEyes << endl;

    dataIn[markChan][0] = firstMarker;
    dataIn[markChan][(*outLength) - 1] = lastMarker;
}


template <typename Typ>
void splitZerosEdges(Typ & dataIn, const int & ns, const int & length, int * outLength)
{
    bool flag[length];
    (*outLength) = length;
    for(int i = 0; i < length; ++i)
    {
        flag[i] = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(dataIn[j][i] != 0.)
            {
                flag[i] = 1;
                break;
            }
        }
    }
    if(flag[0] == 0)
    {
        for(int i = 0; i < length; ++i)
        {
            if(flag[i] == 1)
            {
                //generality, use relocate pointer
                for(int k = 0; k < i; ++k)
                {
                    for(int j = 0; j < ns; ++j)
                    {
                        dataIn[j][k] = dataIn[j][k + i];
                    }
                }
                (*outLength) -= i;
                break;
            }

        }
    }
    if(flag[(*outLength) - 1] == 0)
    {
        for(int i = (*outLength) - 1; i > 0; --i)
        {
            if(flag[i] == 1)
            {
                (*outLength) = i;
                break;
            }

        }
    }

}

void splineCoeffCount(double * const inX, double * const inY, int dim, double ** outA, double ** outB)
{

    //[inX[i-1]...inX[i]] - q[i-1] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
    double ** coefsMatrix;
    matrixCreate(&coefsMatrix, dim, dim);
    for(int i = 0; i < dim; ++i)
    {
        for(int j = 0; j < dim; ++j)
        {
            coefsMatrix[i][j] = 0.;
        }
    }

    double * rightVec = new double [dim];
    double * vectorK = new double [dim];

    //set coefs and rightVec
    coefsMatrix[0][0] = 2.*(inX[1] - inX[0]);
    coefsMatrix[0][1] = (inX[1] - inX[0]);
    rightVec[0] = 3. * (inY[1] - inY[0]);

    coefsMatrix[dim-1][dim-1] = 2.*(inX[dim-1] - inX[dim-2]);
    coefsMatrix[dim-1][dim-2] = (inX[dim-1] - inX[dim-2]);
    rightVec[dim-1] = 3. * (inY[dim-1] - inY[dim-2]);
    for(int i = 1; i < dim-1; ++i) //besides first and last rows
    {
        coefsMatrix[i][i-1] = 1 / (inX[i] - inX[i-1]);
        coefsMatrix[i][i] = 2 * (1 / (inX[i] - inX[i-1]) + 1 / (inX[i+1] - inX[i]));
        coefsMatrix[i][i+1] = 1 / (inX[i+1] - inX[i]);

        rightVec[i] = 3 * ( (inY[i] - inY[i-1]) / ( (inX[i] - inX[i-1]) * (inX[i] - inX[i-1]) ) + (inY[i+1] - inY[i]) / ( (inX[i+1] - inX[i]) * (inX[i+1] - inX[i]) ));
    }
    if(0) //cout matrix and rightvec - OK
    {
        for(int i = 0; i < dim; ++i)
        {
            for(int j = 0; j < dim; ++j)
            {
                cout << coefsMatrix[i][j] << "\t ";
            }
            cout << rightVec[i] << endl;
        }
    }
    //count K's
    matrixSystemSolveGauss(coefsMatrix, rightVec, dim, vectorK);
    //count outA and outB
    for(int i = 1; i < dim; ++i) //there is dim-1 intervals between dim points
    {
        (*outA)[i-1] = vectorK[i-1] * (inX[i] - inX[i-1]) - (inY[i] - inY[i-1]);
        (*outB)[i-1] =  -vectorK[i] * (inX[i] - inX[i-1]) + (inY[i] - inY[i-1]);
    }

    matrixDelete(&coefsMatrix, dim);
    delete []rightVec;
    delete []vectorK;
}

double splineOutput(double * const inX, double * const inY, int dim, double * A, double * B, double probeX)
{
    //[inX[i-1]...inX[i]] - q[i] = (1-t)*inY[i1] + t*inY[i] + t(1-t)(outA[i](1-t) + outB[i]t));
    double t;
    //which interval we have?
    int num = -1; //number of interval
    for(int i = 0; i < dim-1; ++i)
    {
        if(inX[i] <= probeX && probeX < inX[i+1])
        {
            num = i;
            break;
        }
    }
    t = (probeX - inX[num]) / (inX[num+1] - inX[num]);
    double res;
    res = (1-t) * inY[num] + t * inY[num+1] + t * (1-t) * (A[num] * (1-t) + B[num] * t);
    return res;
}

double independence(double * const signal1, double * const signal2, int length)
{
    //determine amplitudes
    double min1 = 0.;
    double min2 = 0.;
    double max1 = 0.;
    double max2 = 0.;
    for(int i = 0; i < length; ++i)
    {
        min1 = fmin(min1, signal1[i]);
        min2 = fmin(min2, signal2[i]);
        max1 = fmax(max1, signal1[i]);
        max2 = fmax(max2, signal2[i]);
    }
    int numOfInts = 20;
    double self1 = 0.;
    double self2 = 0.;
    double together = 0.;
    double counter = 0.;
    double haupt = 0.;
    cout << "i\t" << "j\t" << "self1\t" << "self2\t" << "1*2\t" << "tog\t" << "error\t" << endl;

    for(int i = 0; i < numOfInts; ++i) //intervals of 1
    {
        counter = 0;
        for(int k = 0; k < length; ++ k)
        {
            if(signal1[k] >= min1 + (max1-min1)/numOfInts * i   &&   signal1[k] < min1 + (max1-min1)/numOfInts * (i+1)) counter += 1;
        }
        self1 = counter/length;
        for(int j = 0; j < numOfInts; ++j) //intervals of 2
        {
            counter = 0;
            for(int k = 0; k < length; ++ k)
            {
                if(signal2[k] >= min2 + (max2-min2)/numOfInts * j   &&   signal2[k] < min2 + (max2-min2)/numOfInts * (j+1)) counter += 1;
            }
            self2 = counter/length;


            counter = 0;
            for(int k = 0; k < length; ++ k)
            {
                if(signal2[k] >= min2 + (max2-min2)/numOfInts * j   &&   signal2[k] < min2 + (max2-min2)/numOfInts * (j+1) && signal1[k] >= min1 + (max1-min1)/numOfInts * i   &&   signal1[k] < min1 + (max1-min1)/numOfInts * (i+1)) counter += 1;
            }
            together = counter/length;

            if(1)//together != 0.)
            {
                cout << i << "\t";
                cout << j << "\t";
                cout << doubleRound(self1, 5) << "\t";
                cout << doubleRound(self2, 5) << "\t";
                cout << doubleRound(self1*self2, 5) << "\t";
                cout << doubleRound(together, 5) << "\t";

                //                cout << (together - self1*self2) / (together);
                cout << endl;
            }

            haupt += fabs(together - self1*self2);// / (together * pow(numOfInts, 2.));
        }

    }
    return haupt;
}

double countAngle(double initX, double initY)
{
    if(initX == 0.)
    {
        return (initY > 0.)?(pi/2.):(-pi/2);
    }

    if(initX > 0.)
    {
        return atan(initY/initX);
    }
    else
    {
        return atan(initY/initX) + pi;
    }
}

void calcSpectre(const vector<double> & inSignal,
                 vector<double> & outSpectre,
                 const int & fftLength,
                 const int & NumOfSmooth,
                 const int & Eyes,
                 const double & powArg)
{
    if(inSignal.size() != fftLength)
    {
        cout << "calcSpectre: inappropriate signal length" << endl;
        return;
    }

    const double norm1 = sqrt(fftLength / double(fftLength - Eyes));
    vector<double> spectre (fftLength * 2, 0.);

    double help1, help2;

    for(int i = 0; i < fftLength; ++i)
    {
        spectre[ i * 2 ] = inSignal[ i ] * norm1;
    }
    four1(spectre, fftLength, 1);

    const double norm2 = 2. / (def::freq * fftLength);
    for(int i = 0; i < fftLength / 2; ++i )
    {
        outSpectre[ i ] = (pow(spectre[ i * 2 ], 2) + pow(spectre[ i * 2 + 1 ], 2)) * norm2;
//        outSpectre[ i ] = pow ( outSpectre[ i ], powArg );
    }

    const int leftSmoothLimit = 0;
    const int rightSmoothLimit = fftLength / 2. - 1;

    //smooth spectre
    for(int a = 0; a < (int)(NumOfSmooth / norm1); ++a)
    {
        help1 = outSpectre[leftSmoothLimit-1];
        for(int k = leftSmoothLimit; k < rightSmoothLimit; ++k)
        {
            help2 = outSpectre[k];
            outSpectre[k] = (help1 + help2 + outSpectre[k+1]) / 3.;
            help1 = help2;
        }
    }
}




template <typename Typ>
void calcRawFFT(const Typ & inData,
                mat & dataFFT,
                const int &ns,
                const int &fftLength,
                const int &Eyes,
                const int &NumOfSmooth)
{

    const double norm1 = sqrt(fftLength / double(fftLength-Eyes));
    const double norm2 = 2. / def::freq / fftLength;
    double * spectre = new double [fftLength*2];
    dataFFT.resize(ns);
    std::for_each(dataFFT.begin(),
                  dataFFT.end(),
                  [fftLength](vec & in){in.resize(fftLength/2);});

    double help1, help2;
    int leftSmoothLimit, rightSmoothLimit;

    for(int j = 0; j < ns; ++j)
    {
        for(int i = 0; i < fftLength; ++i)            //make appropriate array
        {
            spectre[ i * 2 + 0 ] = (double)(inData[j][ i ] * norm1);
            spectre[ i * 2 + 1 ] = 0.;
        }
        four1(spectre-1, fftLength, 1);       //Fourier transform

        for(int i = 0; i < fftLength; ++i )      //get the absolute value of FFT
        {
            dataFFT[j][ i ] = spectre[ i ] * norm2; //0.004 = 1/250 generality
        }

        leftSmoothLimit = 0;
        rightSmoothLimit = fftLength / 2 - 1;

        //smooth spectre - odd and even split
        /*
        for(int a = 0; a < (int)(NumOfSmooth / sqrt(norm1)); ++a)
        {
            help1 = (*dataFFT)[j][leftSmoothLimit-1];
            for(int k = leftSmoothLimit; k < rightSmoothLimit; ++k)
            {
                help2 = (*dataFFT)[j][k];
                (*dataFFT)[j][k] = (help1 + help2 + (*dataFFT)[j][k+1]) / 3.;
                help1 = help2;
            }
        }
*/
    }
}

void readPaFile(QString paFile,
                matrix & dataMatrix,
                int & NumberOfVectors,
                vector<QString> & FileName,
                vector<double> & classCount)
{
    const int NetLength = def::nsWOM() * def::spLength;

    ifstream paSrc;
    paSrc.open(paFile.toStdString());
    if(!paSrc.good())
    {
        cout << "readPaFile: bad file" << endl;
        cout << "is_open:\t" << paSrc.is_open() << endl;
        cout << "eof:\t" << paSrc.eof() << endl;
        cout << "good:\t" << paSrc.good() << endl;
        cout << "bad:\t" << paSrc.bad() << endl;
        cout << "fail:\t" << paSrc.fail() << endl;
        cout << "errno = " << errno << endl;

        return;
    }

    double tempClass[def::numOfClasses];  //generality
    double tempVal;
    classCount = vector<double>(def::numOfClasses, 0);
    FileName.clear();
    dataMatrix = matrix();

    std::string tempStr;
    vector<double> tempVec(NetLength + 2); // bias and class

    NumberOfVectors = 0;

    while(!paSrc.eof())
    {
        paSrc >> tempStr;

        /// no need
        QString tempQStr = QString(tempStr.c_str());
        if(!tempQStr.contains(def::ExpName))
        {
            break;
        }
        FileName.push_back(tempQStr);

        for(int i = 0; i < NetLength; ++i)
        {
            paSrc >> tempVec[i];
        }

        tempVal = 0.;
        for(int i = 0; i < def::numOfClasses; ++i)
        {
            paSrc >> tempClass[i];
            tempVal += tempClass[i] * i;
        }

        while(paSrc.peek() == int('\n') || paSrc.peek() == int(' '))
        {
            paSrc.get();
        }

        tempVec[NetLength] = 1.; //bias
        tempVec[NetLength + 1] = tempVal;

        if(int(tempVal) != tempVal)
        {
            cout << "type is wrong " << tempVal << endl;
            return;
        }

        classCount[int(tempVal)] += 1.; // set number of vectors of each class
        dataMatrix.push_back(tempVec);
        ++NumberOfVectors;
    }
    paSrc.close();
}

template <typename Typ>
bool readICAMatrix(const QString & path, Typ &matrixA)
{
    readSpectraFile(path, matrixA, def::nsWOM(), def::nsWOM());
    return 1;
}
template bool readICAMatrix(const QString & path, matrix & matrixA);
template bool readICAMatrix(const QString & path, mat & matrixA);

template <typename Typ>
void writeICAMatrix(const QString & path, Typ &matrixA)
{
    writeSpectraFile(path,
                     matrixA,
                     matrixA.size(),
                     matrixA[0].size());
}
template void writeICAMatrix(const QString & path, matrix & matrixA);
template void writeICAMatrix(const QString & path, mat & matrixA);

void matrixCofactor(double ** &inMatrix, const int &size, const int &numRows, const int &numCols, double **& outMatrix)
{
//    cout << "matrixCof: start" << endl;
int indexA, indexB;
for(int a = 0; a < size; ++a)
{
                   if(a == numRows) continue;
                   indexA = a - (a > numRows);
for(int b = 0; b < size; ++b)
{
    if(b == numCols) continue;
    indexB = b - (b > numCols);

    outMatrix[indexA][indexB] = inMatrix[a][b];
}
}
//    cout << "matrixCof: end" << endl;
}

void matrixSystemSolveGauss(double ** &inMat, double * &inVec, int size, double * &outVec)
{
    double ** initMat;
    matrixCreate(&initMat, size, size);
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            initMat[i][j] = inMat[i][j];
        }
    }
    ////////////////////////////////////////////////////////////
    //too lazy to rewrite via double * tempVec
    double ** tempMat;
    matrixCreate(&tempMat, size, size);
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            tempMat[i][j] = (j==0) * inVec[i];
        }
    }
    double coeff;


    //1) make higher-triangular
    for(int i = 0; i < size-1; ++i) //which line to substract
    {
        for(int j = i+1; j < size; ++j) //FROM which line to substract
        {
            coeff = initMat[j][i]/initMat[i][i];
            //row[j] = row[j] - coeff * row[i] for both matrices
            for(int k = i; k < size; ++k) //k = i because all previous values in a row are already 0
            {
                initMat[j][k] -= coeff * initMat[i][k];
            }
            for(int k = 0; k < size; ++k) //k = 0 because default
            {
                tempMat[j][k] -= coeff * tempMat[i][k];
            }
        }
    }

    //2) make lower-triangular
    for(int i = size-1; i > 0; --i) //which line to substract
    {
        for(int j = i-1; j >= 0; --j) //FROM which line to substract
        {
            coeff = initMat[j][i]/initMat[i][i];
            //row[j] = row[j] - coeff * row[i] for both matrices
            initMat[j][i] -= coeff * initMat[i][i]; //optional subtraction
            for(int k = 0; k < size; ++k) //k = 0 because default
            {
                tempMat[j][k] -= coeff * tempMat[i][k];
            }
        }
    }

    //3) divide on diagonal elements
    for(int i = 0; i < size; ++i) //which line to substract
    {
        for(int k = 0; k < size; ++k) //k = 0 because default
        {
            tempMat[i][k] /= initMat[i][i];
        }
    }


    for(int i = 0; i < size; ++i)
    {
        outVec[i] = tempMat[i][0];
    }

    matrixDelete(&initMat, size);
    matrixDelete(&tempMat, size);
}

void matrixTranspose(double ** &inMat, const int &numRows, const int &numCols, double ** &outMat)
{
    for(int i = 0; i < numCols; ++i)
    {
        for(int j = 0; j < numRows; ++j)
        {
            outMat[i][j] = inMat[j][i];
        }
    }
}

void matrixCopy(double ** &inMat, double ** &outMat, const int &dimH, const int &dimL)
{
    for(int i = 0; i < dimH; ++i)
    {
        //        memcpy((*outMat), inMat, dimL * sizeof(double));
        for(int j = 0; j < dimL; ++j)
        {
            outMat[i][j] = inMat[i][j];
        }
    }
}

void matrixInvert(double ** &inMat, const int &size, double **& outMat) //cofactors
{
    double ** cof = new double * [size - 1];
    for(int i = 0; i < size - 1; ++i)
    {
        cof[i] = new double [size - 1];
    }
    double Det = matrixDet(inMat, size);

    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            matrixCofactor(inMat, size, j, i, cof);
            //            cout << "matrixInvert: cofactor\n";
            //            matrixPrint(cof, size-1, size-1);
            outMat[i][j] = pow(-1, i+j) * matrixDet(cof, size - 1)/Det;
        }
    }
    for(int i = 0; i < size - 1; ++i)
    {
        delete []cof[i];
    }
    delete []cof;
}

void matrixInvert(double ** &mat, const int &size) // by definition - cofactors
{
    double ** tempMat;
    matrixCreate(&tempMat, size, size);
    matrixInvert(mat, size, tempMat);
    matrixCopy(tempMat, mat, size, size);
    matrixDelete(&tempMat, size);
}

void matrixInvertGauss(double **& mat, const int &size) // by definition - cofactors
{
    double ** tempMat;
    matrixCreate(&tempMat, size, size);
    matrixInvertGauss(mat, size, tempMat);
    matrixCopy(tempMat, mat, size, size);
    matrixDelete(&tempMat, size);
}

void matrixInvertGauss(double ** &mat, const int &size, double ** &outMat)
{
    double ** initMat;
    matrixCreate(&initMat, size, size);
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            initMat[i][j] = mat[i][j];
        }
    }

    double ** tempMat;
    matrixCreate(&tempMat, size, size);
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            tempMat[i][j] = (j==i);
        }
    }
    double coeff;


    //1) make higher-triangular
    for(int i = 0; i < size - 1; ++i) //which line to substract
    {
        for(int j = i+1; j < size; ++j) //FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i]; // coefficient

            //row[j] -= coeff * row[i] for both matrices
            for(int k = i; k < size; ++k) //k = i because all previous values in a row are already 0
            {
                initMat[j][k] -= coeff * initMat[i][k];
            }
            for(int k = 0; k < size; ++k) //k = 0 because default
            {
                tempMat[j][k] -= coeff * tempMat[i][k];
            }
        }
    }

    //2) make diagonal
    for(int i = size - 1; i > 0; --i) //which line to substract (bottom -> up)
    {
        for(int j = i - 1; j >= 0; --j) //FROM which line to substract
        {
            coeff = initMat[j][i] / initMat[i][i];

            //row[j] -= coeff * row[i] for both matrices
            for(int k = i; k < size; ++k) // //k = i because all previous values in a row are already 0
            {
                initMat[j][k] -= coeff * initMat[i][k]; //optional subtraction
            }
            for(int k = 0; k < size; ++k) //k = 0 because default
            {
                tempMat[j][k] -= coeff * tempMat[i][k];
            }
        }
    }

    //3) divide on diagonal elements
    for(int i = 0; i < size; ++i) //which line to substract
    {
        for(int k = 0; k < size; ++k) //k = 0 because default
        {
            tempMat[i][k] /= initMat[i][i];
        }
    }

    //4) outmat = tempMat
    for(int i = 0; i < size; ++i) //which line to substract
    {
        for(int k = 0; k < size; ++k) //k = 0 because default
        {
            outMat[i][k] = tempMat[i][k];
        }
    }
    matrixDelete(&initMat, size);
    matrixDelete(&tempMat, size);
}

double matrixDet(double ** &matrix, const int &dim) //- Det
{
    //    cout << "matrixDet: start" << endl;
    if(dim == 1)
    {
        //        cout << "matrixDet: end" << endl;
        return matrix[0][0];
    }


    double ** matrixDet_;
    matrixCreate(&matrixDet_, dim,dim);
    /*
    = new double * [dim];
    for(int i = 0; i < dim; ++i)
    {
        matrixDet_[i] = new double [dim];
    }
    */
    matrixCopy(matrix, matrixDet_, dim, dim);

    double coef;
    for(int i = 1; i < dim; ++i)
    {
        for(int k = 0; k < i; ++k)
        {
            if(matrixDet_[k][k] != 0.)
            {
                coef = matrixDet_[i][k]/matrixDet_[k][k];
            }
            else
            {
                continue;
            }

            for(int j = 0; j < dim; ++j)
            {
                matrixDet_[i][j] -= coef * matrixDet_[k][j];
            }
        }
    }


    coef = 1.;
    for(int i = 0; i < dim; ++i)
    {
        coef *= matrixDet_[i][i];
    }

    for(int k = 0; k < dim; ++k)
    {
        delete []matrixDet_[k];
    }
    delete []matrixDet_;

    //    cout << "matrixDet: end" << endl;
    return coef;
}


double matrixDetB(double ** &matrix, const int &dim) // Det
{
    if(dim == 1) return matrix[0][0];

    double ** cof = matrixCreate(dim-1, dim-1);

    double coef = 0.;
    cout << dim << endl;
    for(int i = 0; i < dim; ++i)
    {
        matrixCofactor(matrix, dim, 0, i, cof);
        coef += matrix[0][i] * pow(-1, i) * matrixDet(cof, dim-1);
    }
    matrixDelete(&cof, dim-1);
    return coef;
}

double ** matrixCreate(const int &i, const int &j)
{
    double ** mat = new double * [i];
    for(int k = 0; k < i; ++k)
    {
        mat[k] = new double [j];
    }
    return mat;
}

void matrixCreate(double *** matrix, const int &i, const int &j)
{
    (*matrix) = new double * [i];
    for(int k = 0; k < i; ++k)
    {
        (*matrix)[k] = new double [j];
    }
}

void matrixDelete(double *** matrix, const int &i)
{
    for(int k = 0; k < i; ++k)
    {
        delete [](*matrix)[k];
    }
    delete [](*matrix);
}

void matrixDelete(int *** matrix, const int &i)
{
    for(int k = 0; k < i; ++k)
    {
        delete [](*matrix)[k];
    }
    delete [](*matrix);
}

void matrixPrint(const double ** const &mat, const int &i, const int &j)
{
    for(int a = 0; a < i; ++a)
    {
        for(int b = 0; b < j; ++b)
        {
            //            cout << mat[a][b] << "\t";
            cout << doubleRound(mat[a][b], 3) << "\t";
        }
        cout << endl;
    }
    cout << endl;
}



double matrixInnerMaxCorrelation(double ** &inMatrix, const int &numRows, const int &numCols, double (*corrFunc)(double * const arr1, double * const arr2, int length, int t))
{
    double res = 0.;
    double temp;
    double ** tempMat;
    matrixCreate(&tempMat, numCols, numRows);
    matrixTranspose(inMatrix, numRows, numCols, tempMat);
    for(int i = 0; i < numCols; ++i)
    {
        for(int j = 0; j < numCols; ++j)
        {
            if(j==i) continue;
            temp = corrFunc(tempMat[i], tempMat[j], numRows, 0); ///////////////////////////////////
            if(fabs(temp) > fabs(res))
            {
                res = temp;
            }
        }
    }
    return res;
}

double matrixMaxCorrelation(double ** &inMat1, double ** &inMat2, const int &numRows, const int &numCols)
{
    double res = 0.;
    double temp;
    double ** tempMat1;
    double ** tempMat2;
    matrixCreate(&tempMat1, numCols, numRows);
    matrixTranspose(inMat1, numRows, numCols, tempMat1);
    matrixCreate(&tempMat2, numCols, numRows);
    matrixTranspose(inMat2, numRows, numCols, tempMat2);

    for(int i = 0; i < numCols; ++i)
    {
        for(int j = 0; j < numCols; ++j)
        {
            temp = correlation(tempMat1[i], tempMat2[j], numRows);
            if(fabs(temp) > fabs(res))
            {
                res = temp;
            }
        }
    }
    return res;
}


void matrixCorrelations(double ** &inMat1, double ** &inMat2, const int &numRows, const int &numCols, double *& resCorr)
{
    double res = 0.;
    QList<int> tempNum;
    tempNum.clear();
    double temp;
    int index;
    double ** tempMat1;
    double ** tempMat2;
    matrixCreate(&tempMat1, numCols, numRows);
    matrixTranspose(inMat1, numRows, numCols, tempMat1);
    matrixCreate(&tempMat2, numCols, numRows);
    matrixTranspose(inMat2, numRows, numCols, tempMat2);

    cout << "ready" << endl;

    for(int i = 0; i < numCols; ++i)
    {
        res = 0.;
        for(int j = 0; j < numCols; ++j)
        {
            temp = correlation(tempMat1[i], tempMat2[j], numRows);
            if(fabs(temp) > fabs(res) && !tempNum.contains(j))
            {
                res = temp;
                if(!tempNum.isEmpty()) tempNum.pop_back();
                tempNum << j;
                index = j;
            }
        }
        tempNum << index;
        resCorr[i] = res;
    }
    for(int i = 0; i < numCols; ++i)
    {
        cout << tempNum[i] + 1 << " ";
    }
    cout << endl;
}

void countRCP(QString filePath, QString picPath, double * outMean, double * outSigma)
{
    vec arr;
    readFileInLine(filePath, arr);

    (*outMean) = mean(arr.data(),
                      arr.size());
    (*outSigma) = sigma(arr.data(),
                        arr.size());

    if(!picPath.isEmpty())
    {
        kernelEst(arr, picPath);
    }
}

void makeCfgStatic(const QString & FileName,
                   const int & NetLength,
                   const QString & outFileDir,
                   const int & numOfOuts,
                   const double & lrate,
                   const double & error,
                   const int & temp)
{
    QString helpString = QDir::toNativeSeparators(outFileDir
                                                  + slash() + FileName);
    if(!helpString.contains(".net"))
    {
        helpString += ".net";

    }
    ofstream outStr;
    outStr.open(helpString.toStdString());
    if(!outStr.good())
    {
        cout << "static MakeCfg: cannot open file: " << helpString << endl;
        return;
    }

    outStr << "inputs\t" << NetLength << '\n';
    outStr << "outputs\t" << numOfOuts << '\n';
    outStr << "lrate\t" << lrate << '\n';
    outStr << "ecrit\t" << error << '\n';
    outStr << "temp\t" << temp << '\n';
    outStr << "srand\t" << int(time (NULL))%12345 << '\n';
    outStr.close();
}

/*
void svd(double ** inData, int size, int length, double *** eigenVects, double ** eigenValues) // not finished
{
    double dF, F;

    double eigenValuesTreshold = pow(10., -9.);
    double * tempA = new double [size];
    double * tempB = new double [length];

    double sum1, sum2;

    //counter j - for B, i - for A
    for(int k = 0; k < size; ++k)
    {
        dF = 1.0;
        F = 1.0;

        //set 1-normalized vector tempA
        for(int i = 0; i < size; ++i)
        {
            tempA[i] = 1./sqrt(size);
        }
        for(int j = 0; j < length; ++j)
        {
            tempB[j] = 1./sqrt(length);
        }



        //approximate P[i] = tempA x tempB;
        int counter = 0;
//        cout<<"curr val = "<<k<<endl;
        while(1) //when stop approximate?
        {
            //countF - error
            F = 0.;
            for(int i = 0; i < size; ++i)
            {
                for(int j = 0; j < length; ++j)
                {
                    F += 0.5*(inData[i][j]-tempB[j]*tempA[i])*(inData[i][j]-tempB[j]*tempA[i]);
                }
            }
            //count vector tempB
            for(int j = 0; j < length; ++j)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int i = 0; i < size; ++i)
                {
                    sum1 += inData[i][j]*tempA[i];
                    sum2 += tempA[i]*tempA[i];
                }
                tempB[j] = sum1/sum2;
            }

            //count vector tempA
            for(int i = 0; i < size; ++i)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int j = 0; j < length; ++j)
                {
                    sum1 += tempB[j]*inData[i][j];
                    sum2 += tempB[j]*tempB[j];
                }
                tempA[i] = sum1/sum2;
            }

            dF = 0.;
            for(int i = 0; i < size; ++i)
            {
                for(int j = 0; j < length; ++j)
                {
                    dF += 0.5*(inData[i][j]-tempB[j]*tempA[i])*(inData[i][j]-tempB[j]*tempA[i]);

                }
//                cout << dF << " ";
            }
            dF = (F-dF)/F;
            ++counter;
            if(counter==150)
            {
//                cout<<"dF = "<<abs(dF)<<endl;
                break;
            }
            if(fabs(dF) < eigenValuesTreshold) break; //crucial cap
        }

        //edit covMatrix
        for(int i = 0; i < size; ++i)
        {
            for(int j = 0; j < length; ++j)
            {
                inData[i][j] -= tempB[j]*tempA[i];
            }
        }

        //count eigenVectors && eigenValues
        sum1 = 0.;
        sum2 = 0.;
        for(int i = 0; i < size; ++i)
        {
            sum1 += tempA[i] * tempA[i];
        }
        for(int j = 0; j < length; ++j)
        {
            sum2 += tempB[j] * tempB[j];
        }
        for(int i = 0; i < size; ++i)
        {
            tempA[i] /= sqrt(sum1);
        }
        for(int j = 0; j < length; ++j)
        {
            tempB[j] /= sqrt(sum2);
        }

        (*eigenValues)[k] = sum1 * sum2 / double(length - 1.);
        cout << "numOfPC = " << k << "\tvalue = " << (*eigenValues)[k] << "\t iterations = " << counter << endl;
        for(int i = 0; i < size; ++i)
        {
            (*eigenVects)[i][k] = tempA[i]; //1-normalized
        }
    }

}

                       */

QString matiCountByteStr(const double & marker)
{
    QString result;
    vector<bool> byteMarker;
    byteMarker = matiCountByte(marker);

    for(int h = 15; h >= 0; --h)
    {
        result += QString::number(int(byteMarker[h]));
        if(h == 8) result += " ";
    }
    return result;

}
void matiPrintMarker(const double &marker, QString pre)
{
    vector<bool> byteMarker;
    byteMarker = matiCountByte(marker);

    if(!pre.isEmpty())
    {
        cout << pre.toStdString() << " = ";
    }
    cout << marker << "\t" << matiCountByteStr(marker) << endl;
}

vector<bool> matiCountByte(double const &  marker)
{
    vector<bool> byteMarker;
    for(int h = 0; h < 16; ++h)
    {
        byteMarker.push_back(matiCountBit(marker, h));
    }
    return byteMarker;
}

void matiFixMarker(double & marker)
{
    //throw 10000000 00000000 and 00000000 10000000 and 00000000 00000000
    if(marker == pow(2, 15) || marker == pow(2, 7) || marker == 0)
    {
        marker = 0;
        return;
    }

    vector<bool> byteMarker = matiCountByte(marker);
    bool boolBuf;

    if(!byteMarker[7]) //elder byte should start with 0 and younger - with 1
    {
        //swap bytes if wrong order
        for(int h = 0; h < 8; ++h)
        {
            boolBuf = byteMarker[h];
            byteMarker[h] = byteMarker[h + 8];
            byteMarker[h + 8] = boolBuf;
        }
        byteMarker[7] = 1;
    }
    marker = double(matiCountDecimal(byteMarker));
}

int matiCountDecimal(vector<bool> byteMarker)
{
    int res = 0;
    for(int h = 0; h < 16; ++h)
    {
        res += byteMarker[h] * pow(2, h);
    }
    return res;
}

int matiCountDecimal(QString byteMarker)
{
    byteMarker.remove(' ');
    if(byteMarker.length() != 16) return 0;

    int res = 0;
    for(int h = 0; h < 16; ++h)
    {
        res += ((byteMarker[h]==QChar('1'))?1:0) * pow(2, 15-h);
    }
    return res;
}



template <typename Typ>
QPixmap drawEeg( Typ dataD,
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

        for(int c1 = 0; c1 < pic.width(); ++c1)
        {
            paint.drawLine(c1, (c2+1) * pic.height() / (ns+2) + dataD[c2][c1] * norm,
                           c1+1, (c2+1) * pic.height() / (ns+2) + dataD[c2][c1+1] * norm);
        }
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


template
QPixmap drawEeg(double ** dataD,
int ns,
int NumOfSlices,
int freq,
const QString & picPath = QString(),
double norm = 1.,
int blueChan = -1,
int redChan = -1);

template
QPixmap drawEeg( mat dataD,
int ns,
int NumOfSlices,
int freq,
const QString & picPath,
double norm,
int blueChan,
int redChan);

template
QPixmap drawEeg( matrix dataD,
int ns,
int NumOfSlices,
int freq,
const QString & picPath,
double norm,
int blueChan,
int redChan);


template <typename Typ>
QPixmap drawEeg( Typ dataD,
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
template QPixmap drawEeg(double ** dataD,
int ns,
int startSlice,
int endSlice,
int freq,
const QString & picPath = QString(),
double norm = 1.,
int blueChan = -1,
int redChan = -1);
template
QPixmap drawEeg( mat dataD,
int ns,
int startSlice,
int endSlice,
int freq,
const QString & picPath,
double norm,
int blueChan,
int redChan);










#if 0



void refreshDist(mat & dist,
const vector <pair <double, double> > & testCoords,
const int input)
{

    // numRow * (numRow + 1) / 2 = distSize
    int size = testCoords.size();

    double helpDist = 0;
    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            if(i != input && j != input) continue;

            helpDist = pow( pow(testCoords[i].first - testCoords[j].first, 2)
                            + pow(testCoords[i].second - testCoords[j].second, 2), 0.5);
            dist[i][j] = helpDist;
            dist[j][i] = helpDist; // unneeded?
        }
    }
}

void refreshDistAll(mat & distNew,
                    const vector <pair <double, double> > & plainCoords)
{
    // numRow * (numRow + 1) / 2 = distSize

    int numRow = plainCoords.size();
    //    cout << "numRow = " << numRow << endl;



    double helpDist = 0.;
    for(int i = 0; i < numRow; ++i)
    {
        for(int j = i+1; j < numRow; ++j)
        {
            helpDist = pow( pow(plainCoords[i].first - plainCoords[j].first, 2)
                            + pow(plainCoords[i].second - plainCoords[j].second, 2), 0.5);
            distNew[i][j] = helpDist;
            distNew[j][i] = helpDist;  // unneeded?
        }
    }
}

void countGradient(const vector <pair <double, double> > & plainCoords,
                   const vector <vector <double> >  & distOld,
                   vector <vector <double> > & distNew,
                   vector <double> & gradient)
{
    const int size = plainCoords.size();
    const double delta = 0.1;
    vector <pair <double, double> > tempCoords = plainCoords;

    for(int i = 0; i < size; ++i)
    {
        tempCoords[i].first += delta/2.;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i] = errorSammon(distOld, distNew);

        tempCoords[i].first -= delta;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i] -= errorSammon(distOld, distNew);
        gradient[2 * i] /= delta;
        tempCoords[i].first += delta/2.;

        if(fabs(tempCoords[i].first - plainCoords[i].first) > 1e-5)
        {
            cout << "coords1 changed " << plainCoords[i].first - tempCoords[i].first << endl;
        }

        tempCoords[i].second += delta/2.;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i + 1] = errorSammon(distOld, distNew);

        tempCoords[i].second -= delta;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i + 1] -= errorSammon(distOld, distNew);
        gradient[2 * i + 1] /= delta;
        tempCoords[i].second += delta/2.;

        if(fabs(tempCoords[i].second - plainCoords[i].second) > 1e-5)
        {
            cout << "coords2 changed " << plainCoords[i].second - tempCoords[i].second << endl;
        }

        refreshDist(distNew, tempCoords, i);
    }
}

void moveCoordsGradient(vector <pair <double, double> > & plainCoords,
                        const mat & distOld,
                        mat & distNew)
{
    int size = plainCoords.size();

    vector <double> gradient;
    gradient.resize(size * 2);
    double lambda = 0.1;

    double errorBackup;

    countGradient(plainCoords, distOld, distNew, gradient);
    int numSteps = 0;
    while(1)
    {
        errorBackup = errorSammon(distOld, distNew);
        // a bit move coords
        for(int i = 0; i < size; ++i)
        {
            plainCoords[i].first -= gradient[2 * i] * lambda;
            plainCoords[i].second -= gradient[2 * i+1] * lambda;
        }
        // count new dists
        refreshDistAll(distNew, plainCoords);

        // if became worse - go back
        if(errorBackup < errorSammon(distOld, distNew))
        {
            for(int i = 0; i < size; ++i)
            {
                plainCoords[i].first += gradient[2 * i] * lambda;
                plainCoords[i].second += gradient[2 * i+1] * lambda;
            }
            refreshDistAll(distNew, plainCoords);
            break;
        }
        ++numSteps;
        if(numSteps % 5 == 4) lambda *= 2;
    }
    //    cout << "gradient steps = " << numSteps  << endl;
}

double errorSammon(const mat & distOld,
                   const mat & distNew) // square matrices
{
    int size = distOld.size();
    //    cout << "errorSammon size = " << size << endl;
    double res = 0.;
    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            res += pow(distOld[i][j] - distNew[i][j], 2.) / pow(distOld[i][j], 2.);
        }
    }
    return res;
}
#endif


double errorSammonAdd(const mat & distOld,
                      const mat & distNew,
                      const vector <int> & placedDots) // square matrices
{
    double res = 0.;
    for(unsigned int i = 0; i < placedDots.size(); ++i)
    {
        for(unsigned int j = i+1; j < placedDots.size(); ++j)
        {
            res += pow(distOld[i][j] - distNew[i][j], 2.) / pow(distOld[i][j], 2.);
        }
    }
    return res;
}

void countInvHessianAddDot(const mat & distOld,
                           const mat & distNew,
                           const vector <pair <double, double> > & crds,
                           const vector <int> & placedDots,
                           mat & invHessian)
{
    invHessian[0][0] = 0.;
    invHessian[0][1] = 0.;
    invHessian[1][0] = 0.;
    invHessian[1][1] = 0.;

    const int & b = placedDots.back();
    for(unsigned int j = 0; j < placedDots.size() - 1; ++j)
    {
        const int & i = placedDots[j];
        //dydy
        invHessian[0][0] +=
                2. * (distOld[i][b] *
                      (pow(distNew[i][b], -3.) *
                       pow(crds.back().second - crds[j].second, 2.) -
                       pow(distNew[i][b], -1.)
                       )
                      + 1.)
                * pow(distOld[i][b], -2.);
        //dxdx
        invHessian[1][1] +=
                2. * (distOld[i][b] *
                      (pow(distNew[i][b], -3.) *
                       pow(crds.back().first - crds[j].first, 2.) -
                       pow(distNew[i][b], -1.)
                       )
                      + 1.)
                * pow(distOld[i][b], -2.);
        invHessian[0][1] +=
                -2. * distOld[i][b] *
                pow(distNew[i][b], -3.) *
                (crds.back().first - crds[j].first) *
                (crds.back().second - crds[j].second)
                * pow(distOld[i][b], -2.);
    }
    invHessian[1][0] = invHessian[0][1];

    double det = invHessian[1][1] * invHessian[0][0] - invHessian[1][0] * invHessian[0][1];
    //    cout << "det = " << det << endl;
    invHessian[0][0] /= det;
    invHessian[0][1] /= det;
    invHessian[1][0] /= det;
    invHessian[1][1] /= det;


}


void countGradientAddDot(const mat & distOld,
                         const mat & distNew,
                         const vector <pair <double, double> > & crds,
                         const vector <int> & placedDots,
                         vector <double>  & gradient) // gradient for one dot
{
    const int & b = placedDots.back();
    gradient[0] = 0.;
    gradient[1] = 0.;
    for(unsigned int j = 0; j < placedDots.size() - 1; ++j)
    {

        const int & i = placedDots[j];
        gradient[0] +=
                2. * (1. - distOld[i][b] /
                      distNew[i][b]) *
                (crds.back().first - crds[j].first)
                * pow(distOld[i][b], -2.);
        gradient[1] +=
                2. * (1. - distOld[i][b] /
                      distNew[i][b]) *
                (crds.back().second - crds[j].second)
                * pow(distOld[i][b], -2.);
    }

}

void countDistNewAdd(mat & distNew, // change only last coloumn
                     const vector < pair <double, double> > & crds,
                     const vector <int> & placedDots)
{
    const int & b = placedDots.back(); // placedDots[placedDots.size() - 1];
    for(unsigned int i = 0; i < placedDots.size() - 1; ++i)
    {
        const int & a = placedDots[i];
        distNew[a][b] = pow(pow(crds[i].first  - crds.back().first , 2) +
                            pow(crds[i].second - crds.back().second, 2),
                            0.5);
        distNew[b][a] = distNew[a][b];
    }

}

void sammonAddDot(const mat & distOld,
                  mat & distNew, // change only last coloumn
                  vector < pair <double, double> > & plainCoords,
                  const vector <int> & placedDots)
{
    const int addNum = placedDots.size() - 1;
    // set initial place
    double helpX = 0.;
    double helpY = 0.;
    double sumW = 0.;
    double currW = 0.;
    for(int i = 0; i < addNum; ++i)
    {
        currW = pow(distOld[placedDots[i]][placedDots.back()], -2.);
        sumW += currW;
        helpX += plainCoords[i].first  * currW;
        helpY += plainCoords[i].second * currW;
    }
    helpX /= sumW;
    helpY /= sumW;

    plainCoords.push_back(make_pair(helpX, helpY));

    //cout all the dots



    // gradien descent
    vector <double> gradient;
    gradient.resize(2);
    mat invHessian; // matrix 2x2
    invHessian.resize(2);
    invHessian[0].resize(2);
    invHessian[1].resize(2);

    double tmpError1 = 0.;
    double tmpError2 = 0.;

    // count initial error
    countDistNewAdd(distNew,
                    plainCoords,
                    placedDots);
    tmpError2 = errorSammonAdd(distOld,
                               distNew,
                               placedDots);

    double lambda = 0.2;
    int iterationsCount = 0;
    double deltaX = 0.;
    double deltaY = 0.;





    while(1)
    {
        tmpError1 = tmpError2;
        countGradientAddDot(distOld,
                            distNew,
                            plainCoords,
                            placedDots,
                            gradient);
        if(1)
        {
            countInvHessianAddDot(distOld,
                                  distNew,
                                  plainCoords,
                                  placedDots,
                                  invHessian);
        }
        deltaX = lambda * (invHessian[0][0] * gradient[0] + invHessian[0][1] * gradient[1]);
        deltaY = lambda * (invHessian[1][0] * gradient[0] + invHessian[1][1] * gradient[1]);

        plainCoords[addNum].first  -= deltaX;
        plainCoords[addNum].second -= deltaY;

        countDistNewAdd(distNew,
                        plainCoords,
                        placedDots);

        tmpError2 = errorSammonAdd(distOld,
                                   distNew,
                                   placedDots);

        ++iterationsCount;

        if(tmpError2 < 1e-10
           || (fabs(tmpError1 - tmpError2) / tmpError1) < 1e-6
           || iterationsCount > 100) break;
    }
    //    cout << "NewDot = " << plainCoords[addNum].first << '\t' << plainCoords[addNum].second << endl;
    //    cout << "NumOfIterations addDot = " << iterationsCount << " error = " << tmpError2 << endl;

    //    if(addNum == 4) exit(1);
}

void sammonProj(const mat & distOld,
                const vector <int> & types,
                const QString & picPath)
{
    srand(time(NULL));
    int size = distOld.size();

    mat distNew; // use only higher triangle
    distNew.resize(size);
    for(int i = 0; i < size; ++i)
    {
        distNew[i].resize(size);
    }

    vector < pair <double, double> > plainCoords;
    //    plainCoords.resize(size);

    // find three most distant points
    // precise
    int num1 = -1;
    int num2 = -1;
    int num3 = -1;
    vector <int> placedDots;
    double maxDist = 0.;

    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            if(distOld[i][j] > maxDist)
            {
                maxDist = distOld[i][j];
                num1 = i;
                num2 = j;
            }
        }
    }
    //    cout << "maxDist = " << maxDist << endl;
    plainCoords.push_back(make_pair(0., 0.));
    plainCoords.push_back(make_pair(maxDist, 0.));
    maxDist = 0.;

    for(int i = 0; i < size; ++i)
    {
        if(i == num1 || i == num2) continue;
        if(fmin(distOld[i][num1], distOld[i][num2]) > maxDist)
        {
            maxDist = fmin(distOld[i][num1], distOld[i][num2]);
            num3 = i;
        }
    }
    //    cout << "maxDist = " << maxDist << endl;
    //count third dot coords
    double tm = (pow(distOld[num1][num3], 2.) +
                 pow(distOld[num1][num1], 2.) -
                 pow(distOld[num2][num3], 2.)
                 ) * 0.5 / distOld[num1][num2];
    //    cout << "tm = " << tm << endl;
    plainCoords.push_back(make_pair(tm,
                                    pow( pow(distOld[num1][num3], 2.) -
                                         pow(tm, 2.),
                                         0.5)
                                    )
                          );
    placedDots.push_back(num1);
    placedDots.push_back(num2);
    placedDots.push_back(num3);

    for(int i = 0; i < 3; ++i)
    {
        distNew[placedDots[i]][placedDots[(i+1)%3]] = distOld[placedDots[i]][placedDots[(i+1)%3]];
        distNew[placedDots[(i+1)%3]][placedDots[i]] = distNew[placedDots[i]][placedDots[(i+1)%3]];
    }

    //    cout << distNew << endl;


    double helpDist = 0.;
    for(int addNum = 3; addNum < size; ++addNum)
    {
        maxDist = 0.;
        //search max distant dot
        for(int i = 0; i < size; ++i)
        {
            if(std::find(placedDots.begin(),
                         placedDots.end(),
                         i) != placedDots.end()) {continue;}

            helpDist = distOld[placedDots[0]][placedDots[1]];
            for(int j = 0; j > placedDots.size(); ++j)
            {
                helpDist = fmin(helpDist, distOld[i][placedDots[j]]);
            }

            if(helpDist > maxDist)
            {
                maxDist = helpDist;
                num3 = i;
            }
        }
        //        cout << "newDotNum = " << num3 << endl;

        //place this dot
        placedDots.push_back(num3);
        sammonAddDot(distOld, distNew, plainCoords, placedDots);
        if(addNum >= int(sqrt(size)))
        {
            ////TODO
            //            adjustSkeletonDots(distOld, distNew, plainCoords, placedDots);
        }
    }
    //    cout << distNew << endl;

    for(int i = 0; i < size; ++i)
    {
        cout << "dot[" << i << "] = " << plainCoords[i].first << '\t' << plainCoords[i].second << endl;
    }
    cout << endl;


    drawSammon(plainCoords, types, picPath);
    QString helpString = picPath;
    helpString.replace(".jpg", "_.jpg");
    drawShepard(distOld, distNew, helpString);
}



void drawSammon(const vector < pair <double, double> > & plainCoords,
                const vector <int> & types,
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

template void drawArrays(const QString & templPath,
const matrix & inMatrix,
const bool weightsFlag,
const spectraGraphsNormalization normType,
const QStringList & colors,
double scaling,
int lineWidth);

template void drawArrays(const QString & templPath,
const mat & inMatrix,
const bool weightsFlag,
const spectraGraphsNormalization normType,
const QStringList & colors,
double scaling,
int lineWidth);

//template void readPlainData(QString inPath,
//mat & data,
//int ns,
//int & numOfSlices,
//int start);
//template void readPlainData(QString inPath,
//matrix & data,
//int ns,
//int & numOfSlices,
//int start);


//template
//void writePlainData(QString outPath,
//const mat & data,
//int ns,
//int numOfSlices,
//int start);

//template
//void writePlainData(QString outPath,
//const matrix & data,
//int ns,
//int numOfSlices,
//int start);

//template void writePlainData(QString outPath,
//double ** const &data,
//int ns,
//int numOfSlices,
//int start);

template void readSpectraFile(const QString & filePath, matrix & outData, int inNs, int spL);
template void readSpectraFile(const QString & filePath, mat & outData, int inNs, int spL);

template void splitZerosEdges(mat & dataIn, const int & ns, const int & length, int * outLength);
template void splitZerosEdges(matrix & dataIn, const int & ns, const int & length, int * outLength);

template void zeroData(mat & inData, const int & leftLimit, const int & rightLimit);
template void zeroData(matrix & inData, const int & leftLimit, const int & rightLimit);


template void calcRawFFT(const mat & inData, mat & dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth);
template void calcRawFFT(const matrix & inData, mat & dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth);

template void splitZeros(mat & inData,
                const int & length,
                int * outLength,
                const QString & logFile,
                const QString & dataName);
template void splitZeros(matrix & inData,
                const int & length,
                int * outLength,
                const QString & logFile,
                const QString & dataName);

