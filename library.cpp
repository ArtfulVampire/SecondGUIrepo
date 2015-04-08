#include "library.h"



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
        return hueJet(256, ((val - minMagn) / (maxMagn - minMagn))*256., 1., 1.);
    }
}


void drawMapSpline(double ** const matrixA, double maxAbs, QString outDir, QString outName, int num, int size, bool colourFlag)
{

    QPixmap pic = QPixmap(size, size);
    QPainter * painter = new QPainter;
    pic.fill();
    painter->begin(&pic);
    QString savePath1 = outDir + QDir::separator() + outName + "_map_" + QString::number(num) + "+.png";
    double val;

    QPixmap pic1 = QPixmap(size, size);
    QPainter * painter1 = new QPainter;
    pic1.fill();
    painter1->begin(&pic1);
    QString savePath2 = outDir + QDir::separator() + outName + "_map_" + QString::number(num) + "-.png";
    double val1;
    double drawArg;
    int drawRange = 256;

    int dim = 7;
    double scale1 = double(dim-1)/size;

    double ** helpMatrix;
    matrixCreate(&helpMatrix, dim, dim); //generality for ns = 19

    int currIndex = 0.;
    for(int i = 0; i < dim*dim; ++i) //generality for ns = 19
    {
        if((i/dim)%(dim-1) == 0 || (i%dim)%(dim-1) == 0)  //set 0 to all edge values
        {
            helpMatrix[i/dim][i%dim] = 0.;
        }
        else if(i/dim == 1 && ((i%dim) - 1) * ((i%dim) - 3) * ((i%dim) - 5) == 0) //Fp3, Fpz, Fp4
        {
            helpMatrix[i/dim][i%dim] = 0.;
        }
        else if(i/dim == 5 && ((i%dim) - 1) * ((i%dim) - 3) * ((i%dim) - 5) == 0) //O3, Oz, O4
        {
            helpMatrix[i/dim][i%dim] = 0.;
        }
        else
        {
            helpMatrix[i/dim][i%dim] = matrixA[currIndex++][num];
        }
    }



    //approximation for square - Fp3, Fpz, Fp, O3, Oz, O4
    helpMatrix[1][1] = (helpMatrix[1][2] + helpMatrix[2][1] + helpMatrix[2][2])/3.;
    helpMatrix[1][3] = (helpMatrix[1][2] + helpMatrix[1][4] + helpMatrix[2][2] + helpMatrix[2][3] + helpMatrix[2][4])/5.;
    helpMatrix[1][5] = (helpMatrix[1][4] + helpMatrix[2][4] + helpMatrix[2][5])/3.;;
    helpMatrix[5][1] = (helpMatrix[4][1] + helpMatrix[4][2] + helpMatrix[5][2])/3.;
    helpMatrix[5][3] = (helpMatrix[5][2] + helpMatrix[4][2] + helpMatrix[4][3] + helpMatrix[4][4] + helpMatrix[4][5])/5.;
    helpMatrix[5][5] = (helpMatrix[5][4] + helpMatrix[4][4] + helpMatrix[4][5])/3.;

    double minMagn = helpMatrix[1][1];
    double maxMagn = helpMatrix[1][1];
    for(int i = 1; i < dim-1; ++i)
    {
        for(int j = 1; j < dim-1; ++j)
        {
            minMagn = min(minMagn, helpMatrix[i][j]);
            maxMagn = max(maxMagn, helpMatrix[i][j]);
        }
    }
//    matrixTranspose(&helpMatrix, dim);

    double ** Ah;
    matrixCreate(&Ah, 5, 6);
    double ** Bh;
    matrixCreate(&Bh, 5, 6);
    double * inX = new double [dim];
    double * inY = new double [dim];
    double * inYv = new double [dim];
    double * Av = new double [dim-1];
    double * Bv = new double [dim-1];

    for(int k = 0; k < dim; ++k)
    {
        inX[k] = k; //hope, it's constant
    }
    for(int i = 1; i < dim-1; ++i) // number of helpMatrix row
    {
        for(int k = 0; k < dim; ++k)
        {
            inX[k] = k; //not necessary
            inY[k] = helpMatrix[i][k];
        }
        splineCoeffCount(inX, inY, dim, &(Ah[i-1]), &(Bh[i-1])); // horizontal splines coeffs

    }

    for(int x = 0; x < size; ++x)
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
        for(int y = 0; y < size; ++y)
        {
            if(distance(x, y, size/2, size/2) > size * 2. * sqrt(2.)/(dim-1) ) continue;
            val = splineOutput(inX, inYv, dim, Av, Bv, y*scale1);


            if(maxAbs == 0) drawArg = (val - minMagn) / (maxMagn - minMagn)*drawRange; //if private maxAbs
            else drawArg = (val + maxAbs) / (2 * maxAbs)*drawRange; //if common maxAbs

            if(!colourFlag)
            {
                painter->setPen(grayScale(drawRange, drawArg));
                painter1->setPen(grayScale(drawRange, drawRange - drawArg));
            }
            else
            {
                painter->setPen(hueJet(drawRange, drawArg));
                painter1->setPen(hueJet(drawRange, drawRange - drawArg));
            }
            painter->drawPoint(x,y);
            painter1->drawPoint(x,y);
        }
    }

    //for transposed helpMatrix
//    painter->end();
//    QTransform transform1(0, 1, 1, 0, 0, 0);
//    pic = QPixmap(pic.transformed(transform1));
//    painter->begin(&pic);


//    painter1->end();
//    pic1 = QPixmap(pic1.transformed(transform1));
//    painter1->begin(&pic1);

    if(1) //draw channels locations
    {
        helpMatrix[1][1] = 0.;
        helpMatrix[1][3] = 0.;
        helpMatrix[1][5] = 0.;
        helpMatrix[5][1] = 0.;
        helpMatrix[5][3] = 0.;
        helpMatrix[5][5] = 0.;
        painter->setBrush(QBrush("black"));
        painter1->setBrush(QBrush("black"));
        painter->setPen("black");
        painter1->setPen("black");
        for(int i = 0; i < dim; ++i)
        {
            for(int j = 0; j < dim; ++j)
            {
                if(helpMatrix[i][j] != 0.)
                {
                    painter->drawEllipse(j/scale1-1, i/scale1-1, 2, 2);
                    painter1->drawEllipse(j/scale1-1, i/scale1-1, 2, 2);
                }
            }
        }
    }
    pic.save(savePath1, 0, 100);
    pic1.save(savePath2, 0, 100);




    matrixDelete(&Ah, 5);
    matrixDelete(&Bh, 5);
    delete []inX;
    delete []inY;
    delete []inYv;
    delete []Av;
    delete []Bv;

    matrixDelete(&helpMatrix, 5);
    delete painter;
    delete painter1;
}


void drawMap(double ** const matrixA, double maxAbs, QString outDir, QString outName, int num, int size, bool colourFlag)
{
    QPixmap pic = QPixmap(size, size);
    QPainter * painter = new QPainter;
    pic.fill();
    painter->begin(&pic);

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

            painter->setPen(mapColor(-maxAbs, maxAbs, helpMatrix, numX, numY, double(double(x%int(scale1))/scale1 + 0.003/scale1), double(double(y%int(scale1))/scale1) + 0.003/scale1, colourFlag)); // why 0.003
            painter->drawPoint(x,y);
        }
    }


    QString helpString = outDir + QDir::separator() + outName + "_map_" + QString::number(num) + ".png";
    pic.save(helpString, 0, 100);


    matrixDelete(&helpMatrix, 5);
    delete painter;
}

void drawMapsICA(QString mapsPath, int ns, QString outDir, QString outName, bool colourFlag, void (*draw1Map)(double ** const matrixA, double maxAbs, QString outDir, QString outName, int num, int size, bool colourFlag))
{
    double ** matrixA;
    matrixCreate(&matrixA, ns, ns);
    readICAMatrix(mapsPath, &matrixA, ns);
    double maxAbs = matrixA[0][0];
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            maxAbs = fmax(maxAbs, fabs(matrixA[i][j]));
        }
    }

//    maxAbs = 0.;
    for(int i = 0; i < ns; ++i)
    {
        draw1Map(matrixA, maxAbs, outDir, outName, i, 240, colourFlag); //240 generality
    }
    matrixDelete(&matrixA, ns);
}


void drawMapsOnSpectra(QString spectraFilePath, QString outSpectraFilePath, QString mapsPath, QString mapsNames)
{
    QPixmap pic;
    pic = QPixmap(spectraFilePath);
    QPainter * pnt = new QPainter;
    pnt->begin(&pic);

    QPixmap pic1;
    QString helpString;
    double offsetX = 0.7;
    QRect earRect;
    int earSize = 14; //generality
    double shitCoeff = 1.10; //smth about width of map on spectra pic

    for(int i = 0; i < 19; ++i) /////////////////////////// generality 19
    {
        //+- maps handler
        helpString = mapsPath + QDir::separator() + mapsNames + "_map_" + QString::number(i) + "+.png";
        if(!QFile::exists(helpString))
        {
            helpString = mapsPath + QDir::separator() + mapsNames + "_map_" + QString::number(i) + "-.png";
            if(!QFile::exists(helpString))
            {
                cout << "drawMapsOnSpectra: no map file found " << helpString.toStdString() << endl;
                return;
            }
        }
        pic1 = QPixmap(helpString);

        pnt->drawPixmap(QRect(coords::x[i] * pic.width() + offsetX * coords::scale * pic.width(),
                              coords::y[i] * pic.height() - coords::scale * pic.height(),
                              (shitCoeff - offsetX) * coords::scale * pic.width(),
                              (shitCoeff - offsetX) * coords::scale * pic.height()),
                        pic1);

        pnt->setPen(QPen(QBrush("black"), 2));
        //draw the nose
        pnt->drawLine(coords::x[i] * pic.width() + offsetX * coords::scale * pic.width() + (shitCoeff - offsetX) * coords::scale * pic.width()/2 - 8,
                      coords::y[i] * pic.height() - coords::scale * pic.height(),
                      coords::x[i] * pic.width() + offsetX * coords::scale * pic.width() + (shitCoeff - offsetX) * coords::scale * pic.width()/2,
                      coords::y[i] * pic.height() - coords::scale * pic.height() - 13);

        pnt->drawLine(coords::x[i] * pic.width() + offsetX * coords::scale * pic.width() + (shitCoeff - offsetX) * coords::scale * pic.width()/2 + 8,
                      coords::y[i] * pic.height() - coords::scale * pic.height(),
                      coords::x[i] * pic.width() + offsetX * coords::scale * pic.width() + (shitCoeff - offsetX) * coords::scale * pic.width()/2,
                      coords::y[i] * pic.height() - coords::scale * pic.height() - 13);

        earRect = QRect(coords::x[i] * pic.width() + offsetX * coords::scale * pic.width() - 0.75 * earSize,
                        coords::y[i] * pic.height() - coords::scale * pic.height() + (shitCoeff - offsetX) * coords::scale * pic.height()/2 - earSize,
                        earSize, 2*earSize);
        pnt->drawArc(earRect, 60*16, 240*16);
//        pnt->drawRect(earRect);


        earRect = QRect(coords::x[i] * pic.width() + offsetX * coords::scale * pic.width() + (shitCoeff - offsetX) * coords::scale * pic.width() - 0.25 * earSize,
                        coords::y[i] * pic.height() - coords::scale * pic.height() + (shitCoeff - offsetX) * coords::scale * pic.height()/2 - earSize,
                        earSize, 2*earSize);
        pnt->drawArc(earRect, 240*16, 240*16);
//        pnt->drawRect(earRect);



    }
    pic.save(outSpectraFilePath, 0, 100);
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

    pnt->end();
    delete pnt;
}


#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
void four1(double * dataF, int nn, int isign)
{
    int n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    double tempr,tempi;

    n = nn << 1; //n = nn * 2;
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


int readDefaults(QString filePath, char * dataFolder, int fftLength, int numOfClasses, bool opencl, bool openmp)
{
    FILE * inFile;
    inFile = fopen(filePath.toStdString().c_str(), "r");
    if(inFile == NULL)
    {
        cout << "cannot open defaults file" << endl;
        return 0;
    }

    fscanf(inFile, "%*s %s", dataFolder);
    if(&fftLength != NULL) fscanf(inFile, "%*s %d", &fftLength);
    else fscanf(inFile, "%*s %*s");
    if(&numOfClasses != NULL) fscanf(inFile, "%*s %d", &numOfClasses);
    else fscanf(inFile, "%*s %*s");
    if(&opencl != NULL) fscanf(inFile, "%*s %d", &opencl);
    else fscanf(inFile, "%*s %*s");
    if(&openmp != NULL) fscanf(inFile, "%*s %d", &openmp);
    else fscanf(inFile, "%*s %*s");

    fclose(inFile);
    return 1;
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

double fractalDimension(double *arr, int N, QString picPath)
{
    int timeShift; //timeShift
    double L; //average length

    int minLimit = 2;
    int maxLimit = floor(log(sqrt(N)) * 4. - 5.);


    double * drawK = new double [maxLimit - minLimit];
    double * drawL = new double [maxLimit - minLimit];

    for(int h = minLimit; h < maxLimit; ++h)
    {
        timeShift = h;
        L = 0.;
        for(int m = 0; m < timeShift; ++m)
        {
            for(int i = 1; i < floor((N - m) / timeShift); ++i)
            {
                L += fabs(arr[m + i * timeShift] - arr[m + (i - 1) * timeShift]);
            }
            L = L * (N - 1) / (floor((N - m) / timeShift) * timeShift);
        }
        L /= timeShift;
        drawK[h - minLimit] = log(timeShift);
        drawL[h - minLimit] = log(L);
    }

    //least square approximation
    double slope;
    double *temp = new double [5];
    for(int i = 0; i < 5; ++i)
    {
        temp[i] = 0.;
    }
    for(int h = 0; h < maxLimit - minLimit; ++h)
    {
        temp[0] += drawK[h]*drawL[h];
        temp[1] += drawK[h];
        temp[2] += drawL[h];
        temp[3] += drawK[h]*drawK[h];
        temp[4] += drawK[h];
    }
    for(int i = 0; i < 5; ++i)
    {
        temp[i] /= (maxLimit - minLimit);
    }

    slope = (temp[0] - temp[1]*temp[2]) / (temp[3] - temp[1]*temp[1]); // ???????????????????
    slope = -slope;

    delete []temp;
    delete []drawK;
    delete []drawL;
    return slope;
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

QString getExpNameLib(QString const filePath) // getFileName
{
    QString hlp;
    hlp = QDir::toNativeSeparators(filePath);
    hlp = hlp.right(hlp.length() - hlp.lastIndexOf(slash()) - 1);
    hlp = hlp.left(hlp.lastIndexOf('.'));
    return hlp;
}

QString getDirPathLib(const QString filePath)
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

QString getFileName(QString filePath, bool withExtension)
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
//    typedef unsigned char byte;
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
            return false;
        }

    }
    fclose(fil1);
    fclose(fil2);
    cout << "equalFiles: time = " << myTime.elapsed() / 1000. << " sec" << endl;
    return true;
}

QString getPicPath(const QString & dataPath, QDir * ExpNameDir, int ns)
{
    QString fileName = QDir::toNativeSeparators(dataPath);
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf(QDir::separator()) - 1);
    fileName.replace('.', '_');

    QString helpString = QDir::toNativeSeparators(ExpNameDir->absolutePath() + QDir::separator());

    if(dataPath.contains("Realisations"))
    {
        helpString += "Signals";
    }
    else if(dataPath.contains("windows") && !dataPath.contains("fromreal"))
    {
        helpString += "Signals" + QString(QDir::separator()) + "windows";
    }
    else if(dataPath.contains("cut"))
    {
        helpString += "SignalsCut";
    }
    helpString += QDir::separator();
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
    helpString += QDir::separator() + fileName + ".jpg";
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

char * QStrToCharArr(const QString & input)
{
    char * array = new char [input.length() + 1];
    memcpy(array, input.toStdString().c_str(), input.length());
    array[input.length()] = '\0';
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


ostream & operator << (ostream &os, vector<double> toOut)
{
    for(int i = 0; i < 10; ++i)
    {
        os << toOut[i] << " ";
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

//vector< vector<double> > operator=(const vector< vector<double> > & other)
//{
//    vector< vector<double> > temp;
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

double mean(const int *arr, int length)
{
    double sum = 0.;
    for(int i = 0; i < length; ++i)
    {
        sum += arr[i];
    }
    sum /= (double)length;
    return sum;
}

double mean(const double * arr, int length)
{
    double sum = 0.;
    for(int i = 0; i < length; ++i)
    {
        sum += arr[i] / double(length);
    }
    return sum;
}

double variance(const int *arr, int length)
{
    double sum1 = 0.;
    double m = mean(arr, length);
    for(int i = 0; i < length; ++i)
    {
        sum1 += (arr[i] - m) * (arr[i] - m);
    }
    sum1 /= (double)length;
    return sum1;
}


double variance(const double * arr, int length)
{
    double sum1 = 0.;
    double m = mean(arr, length);
    for(int i = 0; i < length; ++i)
    {
        sum1 += (arr[i] - m) * (arr[i] - m);
    }
    sum1 /= (double)length;
    return sum1;
}

double varianceFromZero(const int *arr, int length)
{
    double sum1 = 0.;
    for(int i = 0; i < length; ++i)
    {
        sum1 += arr[i] * arr[i];
    }
    sum1 /= (double)length;
    return sum1;
}


double varianceFromZero(const double * arr, int length)
{
    double sum1 = 0.;
    for(int i = 0; i < length; ++i)
    {
        sum1 += arr[i]* arr[i];
    }
    sum1 /= (double)length;
    return sum1;
}


double sigma(int *arr, int length)
{
    return sqrt(variance(arr, length));
}

double sigma(const double *arr, int length)
{
    return sqrt(variance(arr, length));
}

double sigmaFromZero(int *arr, int length)
{
    return sqrt(varianceFromZero(arr, length));
}

double sigmaFromZero(const double *arr, int length)
{
    return sqrt(varianceFromZero(arr, length));
}


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

bool MannWhitney(double * arr1, int len1, double * arr2, int len2, double p)
{

    int * n = new int[2];
    n[0] = len1;
    n[1] = len2;

    double ** array = new double * [2];
    array[0] = new double [n[0] + n[1]];
    array[1] = new double [n[0] + n[1]];
    for(int i = 0; i < n[0]; ++i)
    {
        array[0][i] = arr1[i];
    }
    for(int i = 0 + n[0]; i < n[1] + n[0]; ++i)
    {
        array[1][i] = arr2[i];
    }


    for(int i = 0; i < n[0]; ++i)
    {
        array[1][i] = 0.;
    }
    for(int i = n[0]; i < n[0] + n[1]; ++i)
    {
        array[0][i] = 0.;
    }


    double temp;
    int sum0;
    int sumAll;
    double average = n[0]*n[1]/2.;
    double dispersion = sqrt(n[0]*n[1]*(n[0]+n[1])/12.);


    double U = 0.;

    double tmp1, tmp2;
    for(int k = 0; k < n[0] + n[1]; ++k)
    {
        for(int i = 0; i < n[0] + n[1] - 1; ++i)
        {
            if( array[0][i] != 0. ) tmp1 = array[0][i];
            else tmp1 = array[1][i];

            if( array[0][i+1] != 0. ) tmp2 = array[0][i+1];
            else tmp2 = array[1][i+1];

            if( tmp1 > tmp2 )
            {
                for(int j = 0; j < 2; ++j)
                {
                    temp = array[j][i];
                    array[j][i] = array[j][i+1];
                    array[j][i+1] = temp;
                }
            }
        }
    }

    //count sums

    sum0 = 0;
    for(int i = 0; i<n[0] + n[1]; ++i)
    {
        if(array[0][i]!=0) sum0 += (i+1);
    }

    //if sum0 is bigger

    sumAll = (n[0]+n[1])*(n[0]+n[1]+1)/2;
    if(sum0 > sumAll/2 )
    {
        U = double(n[0]*n[1] + n[0]*(n[0]+1)/2. - sum0);
    }
    else
    {
        U = double(n[1]*n[0] + n[1]*(n[1]+1)/2. - (sumAll - sum0));
    }

    delete []n;
    matrixDelete(&array, 2);

    if(abs((U-average)/double(dispersion)) > quantile((1.00 + (1-p))/2.))
    {
//        cout<<"different"<<endl;
        return true;

    }
    else
    {
//        cout<<"not different"<<endl;
        return false;
    }
}

int typeOfFileName(QString fileName)
{
    if(fileName.contains("_241")) return 0;
    else if(fileName.contains("_247")) return 1;
    else if(fileName.contains("_254")) return 2;
    else return -1;
}


void makePaFile(QString spectraDir, QStringList fileNames, int ns, int spLength, int NumOfClasses, double coeff, QString outFile)
{
//    QTime myTime;
//    myTime.start();
    ofstream outStream(outFile.toStdString().c_str());
    if(!outStream.good())
    {
        cout << "bad out pa-file" << endl;
        return;
    }
    double ** data4;
    matrixCreate(&data4, ns, spLength);

    int type;
    QString helpString;

    for(int i = 0; i < fileNames.length(); ++i)
    {
        type = typeOfFileName(fileNames[i]);
        helpString = spectraDir + QDir::separator() + fileNames[i];
        readSpectraFile(helpString, &data4, ns, spLength);
        outStream << fileNames[i].toStdString() << endl;

        for(int l = 0; l < ns; ++l)
        {
            for(int k = 0; k < spLength; ++k)
            {
                outStream << doubleRound(data4[l][k]/coeff, 5) << '\t';
                if(k%10 == 9)
                {
                    outStream << '\n';
                }
            }
            outStream << '\n';
        }


        for(int k = 0; k < NumOfClasses; ++k)
        {

            outStream << (k==type) << ' ';
        }

        outStream << "\n\n";
        outStream.flush();
    }
    outStream.close();
    matrixDelete(&data4, ns);
//    cout << "makePaFile: time elapsed = " << myTime.elapsed()/1000. << " sec" <<endl;
}

void makeMatrixFromFiles(QString spectraDir, QStringList fileNames, int ns, int spLength, double coeff, double *** outMatrix)
{
//    QTime myTime;
//    myTime.start();

    double ** data4;
    matrixCreate(&data4, ns, spLength);

    int type;
    QString helpString;

    for(int i = 0; i < fileNames.length(); ++i)
    {
        type = typeOfFileName(fileNames[i]);
        helpString = spectraDir + QDir::separator() + fileNames[i];
        readSpectraFile(helpString, &data4, ns, spLength);

        for(int l = 0; l < ns; ++l)
        {
            for(int k = 0; k < spLength; ++k)
            {
                (*outMatrix)[i][l*spLength + k] = data4[l][k]/coeff;
            }
        }
        (*outMatrix)[i][ns*spLength] = 1.;
        (*outMatrix)[i][ns*spLength + 1] = type;

    }
    matrixDelete(&data4, ns);
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
        QFile::remove(tmpDir->absolutePath() + QDir::separator() + lst[h]);
    }
    delete tmpDir;

}

void drawRCP(double * values, int length)
{
    QPixmap pic(1000, 400);
    QPainter * pnt = new QPainter();
    pic.fill();
    pnt->begin(&pic);
    pnt->setPen("black");


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
        pnt->drawLine(i, pic.height() * 0.9 * ( 1. - line[i] / valueMax), i+1, pic.height() * 0.9 * (1. - line[i+1] / valueMax));
    }
    pnt->drawLine(0, pic.height()*0.9, pic.width(), pic.height()*0.9);


    int coordinate;

    for(int i = 0; i < length; ++i) //draw the values
    {
        coordinate = pic.width()/2. * (1. + values[i] / numOfDisp);
        if(i%2 == 0) //raw data
        {
            pnt->setPen("blue");
        }
        else //ica data
        {
            pnt->setPen("red");
        }
        pnt->drawLine(coordinate, pic.height() * 0.9 * ( 1. - line[coordinate] / valueMax) , coordinate, pic.height() * 0.9 * ( 1. - line[coordinate] / valueMax) - 50);
    }
    pic.save("/media/Files/Data/AA/rcp.png", 0, 100);

    delete pnt;
    delete []values;

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


double covariance(double * const arr1, double * const arr2, int length)
{
    double res = 0.;
    double m1, m2;
    m1 = mean(arr1, length);
    m2 = mean(arr2, length);
    for(int i = 0; i < length; ++i)
    {
        res += (arr1[i] - m1) * (arr2[i] - m2);
    }
    return res;
}

double correlation(const double * arr1, const double * arr2, int length, int shift)
{
    double res = 0.;
    double m1, m2;
    int T = abs(shift);
    double sigmas;
    if(shift >= 0) //start from arr1[0] and arr1[t]
    {
        m1 = mean(arr1, length-shift);
        m2 = mean(arr2+shift, length-shift);
        for(int i = 0; i < length - shift; ++i)
        {
            res += (arr1[i] - m1) * (arr2[i + shift] - m2);
        }
        sigmas = sigma(arr1, length-shift) * sigma(arr2+shift, length-shift);
        if(sigmas != 0.)
        {
            res /= sigmas;
        }
        else
        {
            cout << "const signal ";
            return 0.;
        }
    }
    else  //start from arr1[0] and arr1[t]
    {
        m1 = mean(arr1+T, length-T);
        m2 = mean(arr2, length-T);
        for(int i = 0; i < length - T; ++i)
        {
            res += (arr1[i + T] - m1) * (arr2[i] - m2);
        }
        sigmas = sigma(arr1 + T, length - T) * sigma(arr2, length - T);
        if(sigmas != 0.)
        {
            res /= sigmas;
        }
        else
        {
            cout << "const signal ";
            return 0.;
        }
    }
    res /= double(length - T);
    return res;
}


double correlationFromZero(double * const arr1, double * const arr2, int length, int shift)
{
    double res = 0.;
    int T = abs(shift);
    if(shift >= 0) //start from arr1[0] and arr1[t]
    {
        for(int i = 0; i < length - shift; ++i)
        {
            res += arr1[i] * arr2[i + shift];
        }

        res /= sigmaFromZero(arr1, length-shift) * sigmaFromZero(arr2+shift, length-shift);
    }
    else  //start from arr1[0] and arr1[t]
    {
        for(int i = 0; i < length - T; ++i)
        {
            res += arr1[i + T] * arr2[i];
        }

        res /= sigmaFromZero(arr1 + T, length - T) * sigmaFromZero(arr2, length - T);
    }

    res /= double(length - T);
    return res;
}


double enthropy(double *arr, int N, QString picPath, int numOfRanges) // ~30 is ok
{
//    numOfRanges = 50;
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

//matrix product out = A(H*H) * B(H*L)
void matrixProduct(double ** const A, double ** const inMat2, double *** outMat, int dimH, int dimL)
{
    double result;

    for(int j = 0; j < dimL; ++j)
    {
        for(int i = 0; i < dimH; ++i)
        {
            result = 0.;
            for(int k = 0; k < dimH; ++k)
            {
                result += A[i][k] * inMat2[k][j];
            }
            (*outMat)[i][j] = result;
        }
    }
}

void matrixProduct(double ** const inMat1, double ** const inMat2, double *** outMat, int numRows1, int numCols2, int numCols1Rows2)
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
            (*outMat)[j][i] = result;
        }
    }
}

void matrixProduct(double * const vect, double ** const mat, double ** outVect, int dimVect, int dimMat) //outVect(dimMat) = vect(dimVect) * mat(dimVect * dimMat)
{
    for(int i = 0; i < dimMat; ++i)
    {
        (*outVect)[i] = 0.;
        for(int j = 0; j < dimVect; ++j)
        {
            (*outVect)[i] += vect[j] * mat[j][i];
        }
    }
}
void matrixProduct(double ** const mat, double * const vect, double ** outVect, int dimVect, int dimMat) //outVect(dimMat) = mat(dimMat*dimVect) * vect(dimVect)
{
    for(int i = 0; i < dimMat; ++i)
    {
        (*outVect)[i] = 0.;
        for(int j = 0; j < dimVect; ++j)
        {
            (*outVect)[i] += mat[i][j] * vect[j];
        }
    }
}
void matrixProduct(double * const vect1, double * const vect2, int dim, double * out)
{
    (*out) = 0.;
    for(int j = 0; j < dim; ++j)
    {
        (*out) += vect1[j] * vect2[j];
    }

}

double distance(vector<double> vec1, vector<double> vec2, const int dim)
{
    double dist = 0.;
    //Euclid
    for(int i = 0; i < dim; ++i)
    {
        dist += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return dist;

}

double distance(double * const vec1, double * const vec2, const int dim)
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
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void matrixMahCount(double ** const matrix, int number, int dimension, double *** outMat, double ** meanVect) //matrix(number * dimension)
{
    double ** newMat;
    matrixCreate(&newMat, dimension, number);
    matrixTranspose(matrix, number, dimension, &newMat);

    for(int i = 0; i < dimension; ++i)
    {
        (*meanVect)[i] = 0.;
        for(int j = 0; j < number; ++j)
        {
            (*meanVect)[i] += newMat[i][j];
        }
        (*meanVect)[i] /= number;
    }

    for(int i = 0; i < dimension; ++i)
    {
        for(int j = 0; j < dimension; ++j)
        {
            (*outMat)[i][j] = covariance(newMat[i], newMat[j], number);
        }
    }
    matrixDelete(&newMat, dimension);

}

double distanceMah(double * const vect, double ** const covMatrixInv, double * const groupMean, int dimension)
{
    double * tempVec = new double [dimension];
    double * difVec = new double [dimension];
    for(int i = 0; i < dimension; ++i)
    {
        difVec[i] = vect[i] - groupMean[i];
    }

    double res;
    matrixProduct(difVec, covMatrixInv, &tempVec, dimension, dimension);
    matrixProduct(tempVec, difVec, dimension, &res);

    delete []tempVec;
    delete []difVec;
    return res;
}

double distanceMah(double * const vect, double ** const group, int dimension, int number) //group[number][dimension]
{
    double ** covMatrix;
    matrixCreate(&covMatrix, dimension, dimension);
    double * meanGroup = new double [dimension];

    matrixMahCount(group, number, dimension, &covMatrix, &meanGroup);
    matrixInvertGauss(&covMatrix, dimension);

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
    QPixmap pic;
    pic.fill();
    QPainter * pnt;
    pnt->begin(&pic);

    double maxVal = maxValue(array, length);

}

void drawArray(double ***sp, int count, int *spL, QStringList colours, int type, double scaling, int left, int right, double spStep, QString outName, QString rangePicPath, QDir * dirBC)
{
    QSvgGenerator svgGen;
    QPixmap pic;
    QPainter * paint;
    int ns = 19;
    QString helpString;
    int helpInt;

    if(type == 1)
    {
        svgGen.setSize(QSize(800, 800));
        svgGen.setViewBox(QRect(QPoint(0,0), svgGen.size()));
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(outName).append(".svg");
        svgGen.setFileName(helpString);
        paint->begin(&svgGen);
        paint->setBrush(QBrush("white"));
        paint->drawRect(QRect(QPoint(0,0), svgGen.size()));
    }
    else if(type == 0)
    {

        pic=QPixmap(1600,1600);
        pic.fill();
        paint->begin(&pic);
    }
    double norm = 0.;
    for(int j = 0; j < count; ++j)
    {
        for(int c2 = 0; c2 < ns; ++c2)
        {
            for(int k = 0; k < int(coords::scale * paint->device()->width()); ++k)
            {
                norm = fmax(norm, sp[j][c2][int(k*spL[j]/(coords::scale * paint->device()->width()))]);  //doesn't work
            }
        }
    }
    cout<<"max magnitude = "<<norm<<endl;

    norm = (coords::scale * paint->device()->height())/norm ; //250 - pixels per graph, generality
    norm *= scaling;


    //    cout<<"prep spectra draw"<<endl;
    for(int c2 = 0; c2 < ns; ++c2)  //exept markers channel
    {
        //draw spectra
        for(int k = 0; k < int(coords::scale * paint->device()->width())-1; ++k)
        {
            for(int j = 0; j < count; ++j)
            {
                paint->setPen(QPen(QBrush(QColor(colours[j])), 2));
                paint->drawLine(QPointF(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - sp[j][c2][int(k*spL[j]/(coords::scale * paint->device()->width()))]*norm), QPointF(paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - sp[j][c2][int((k+1)*spL[j]/(coords::scale * paint->device()->width()))]*norm));
            }
        }

        //draw axes
        paint->setPen("black");
        paint->drawLine(QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2] - coords::scale * paint->device()->height())); //250 - length of axes generality
        paint->drawLine(QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2] + coords::scale * paint->device()->width(), paint->device()->height() * coords::y[c2])); //250 - length of axes generality

        //draw Herzes
        paint->setFont(QFont("Helvitica", int(8*(paint->device()->height()/1600.))));
        for(int k=0; k<int(coords::scale * paint->device()->width()); ++k) //for every Hz generality
        {
            if( (left + k*(spL[0])/(coords::scale * paint->device()->width()))*spStep - floor((left + k*(spL[0])/(coords::scale * paint->device()->width()))*spStep) < spL[0]/(coords::scale * paint->device()->width())*spStep/2. || ceil((left + k*(spL[0])/(coords::scale * paint->device()->width()))*spStep) - (left + k*(spL[0])/(coords::scale * paint->device()->width()))*spStep < spL[0]/(coords::scale * paint->device()->width())*spStep/2.)  //why spLength - generality 250 - length of axes //generality spL[0] for these left and right
            {
                paint->drawLine(QPointF(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2] + 5 * (paint->device()->height()/1600.)));

                helpInt = int((left + k*(spL[0])/(coords::scale * paint->device()->width()))*spStep + 0.5); //generality spL[0] for these left and right
                helpString.setNum(helpInt);
                if(helpInt<10)
                {
                    paint->drawText(QPointF(paint->device()->width() * coords::x[c2] + k - 3 * (paint->device()->width()/1600.), paint->device()->height() * coords::y[c2] + 15 * (paint->device()->height()/1600.)), helpString);  //-3 getFont->size
                }
                else
                {
                    paint->drawText(QPointF(paint->device()->width() * coords::x[c2] + k - 5 * (paint->device()->width()/1600.), paint->device()->height() * coords::y[c2] + 15 * (paint->device()->height()/1600.)), helpString);  //-5 getFont->size
                }
            }
        }

    }
    //write channels labels
    paint->setFont(QFont("Helvetica", int(24*paint->device()->height()/1600.), -1, false));
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText(QPointF((paint->device()->width() * coords::x[c2] - 20 * (paint->device()->width()/1600.)), (paint->device()->height() * coords::y[c2] - (coords::scale * paint->device()->height()) - 2)), QString(coords::lbl[c2]));
    }

    //draw coords::scale
    paint->drawLine(QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1]), QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())));  //250 - graph height generality

    //returning norm = max magnitude
    norm /= scaling;
    norm = (coords::scale * paint->device()->height()) / norm;
    norm /= scaling;  //scaling generality
    norm = int(norm*10.)/10.;

    helpString.setNum(norm);
    helpString.append(QObject::tr(" mcV^2/Hz"));
    paint->drawText(QPointF(paint->device()->width() * coords::x[6]+5., paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())/2.), helpString);


    if(type == 0)
    {
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(outName).append(".jpg");
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(outName).append(".png");
        pic.save(helpString, 0, 100);
        rangePicPath = helpString;
    }
    paint->end();
}

void hilbert(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath)
{

    int fftLen = int(pow(2., ceil(log(inLength)/log(2.))));
    (*out) = new double [2*fftLen];
    double spStep = sampleFreq/fftLen;

    double * tempArr = new double [fftLen];
    double * filteredArr = new double [fftLen];
    for(int i = 0; i < fftLen; ++i)
    {
        tempArr[i] = 0.;
        filteredArr[i] = 0.;
    }

    for(int i = 0; i < inLength; ++i)
    {
        (*out)[ 2 * i + 0] = arr[i] * sqrt(fftLen/double(inLength));
        (*out)[ 2 * i + 1] = 0.;
    }
    for(int i = inLength; i < fftLen; ++i)
    {
        (*out)[ 2 * i + 0] = 0.;
        (*out)[ 2 * i + 1] = 0.;
    }
    four1((*out)-1, fftLen, 1);
    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2.*lowFreq/spStep || i > 2.*highFreq/spStep)
            (*out)[i] = 0.;
    }
    for(int i = fftLen; i < 2*fftLen; ++i)
    {
        if(((2*fftLen - i) < 2.*lowFreq/spStep) || (2*fftLen - i > 2.*highFreq/spStep))
            (*out)[i] = 0.;
    }
    (*out)[0] = 0.;
    (*out)[1] = 0.;
    (*out)[fftLen] = 0.;
    (*out)[fftLen+1] = 0.;
    //end filtering

    four1((*out)-1, fftLen, -1);
    for(int i = 0; i < inLength; ++i)
    {
        filteredArr[i] = (*out)[2*i]/fftLen/sqrt(fftLen/double(inLength));
    }


    //Hilbert via FFT
    for(int i = 0; i < inLength; ++i)
    {
        (*out)[ 2 * i + 0] = filteredArr[i] * sqrt(fftLen/double(inLength));
        (*out)[ 2 * i + 1] = 0.;
    }
    for(int i = inLength; i < fftLen; ++i)
    {
        (*out)[ 2 * i + 0] = 0.;
        (*out)[ 2 * i + 1] = 0.;
    }
    four1((*out)-1, fftLen, 1);

    for(int i = 0; i < fftLen/2; ++i)
    {
        (*out)[2*i + 0] = 0.;
        (*out)[2*i + 1] = 0.;
    }
    four1((*out)-1, fftLen, -1);

    for(int i = 0; i < inLength; ++i)
    {
        tempArr[i] = (*out)[2*i+1]/fftLen*2; //hilbert
    }
    //end Hilbert via FFT


    for(int i = 0; i < fftLen; ++i)
    {
        (*out)[i] = sqrt(tempArr[i]*tempArr[i] + filteredArr[i]*filteredArr[i]);
    }


    if(!picPath.isEmpty())
    {

        //start check draw - OK
        QPixmap pic(fftLen,600);
        QPainter *pnt = new QPainter;
        pic.fill();
        pnt->begin(&pic);
        //    double sum, sum2;
        double enlarge = 10.;

        pnt->setPen("black");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt->drawLine(i, pic.height()/2. - enlarge * filteredArr[i], i+1, pic.height()/2. - enlarge * filteredArr[i+1]);
        }
        pnt->setPen("blue");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            //        pnt->drawLine(i, pic.height()/2. - enlarge * tempArr[i], i+1, pic.height()/2. - enlarge * tempArr[i+1]);
        }
        pnt->setPen("green");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt->drawLine(i, pic.height()/2. - enlarge * (*out)[i], i+1, pic.height()/2. - enlarge * (*out)[i+1]);
        }

        pic.save(picPath, 0, 100);
        pic.fill();
        pnt->end();
        delete pnt;
        cout << "hilber drawn" << endl;
        //end check draw
    }

    delete []tempArr;
    delete []filteredArr;

}

void hilbertPieces(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath)
{
    int fftLen = int(pow(2., floor(log(inLength)/log(2.))));
    (*out) = new double [2*fftLen];
    double spStep = sampleFreq/fftLen;

    double * tempArr = new double [inLength];
    double * filteredArr = new double [inLength];

    for(int i = 0; i < fftLen; ++i)
    {
        (*out)[ 2 * i + 0] = arr[i];
        (*out)[ 2 * i + 1] = 0.;
    }
    four1((*out)-1, fftLen, 1);
    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2.*lowFreq/spStep || i > 2.*highFreq/spStep)
            (*out)[i] = 0.;
    }
    for(int i = fftLen; i < 2*fftLen; ++i)
    {
        if(((2*fftLen - i) < 2.*lowFreq/spStep) || (2*fftLen - i > 2.*highFreq/spStep))
            (*out)[i] = 0.;
    }
    (*out)[0] = 0.;
    (*out)[1] = 0.;
    (*out)[fftLen] = 0.;
    (*out)[fftLen+1] = 0.;
    //end filtering

    four1((*out)-1, fftLen, -1);
    for(int i = 0; i < fftLen; ++i)
    {
        filteredArr[i] = (*out)[2*i]/fftLen*2;
    }


    //Hilbert via FFT
    for(int i = 0; i < fftLen; ++i)
    {
        (*out)[ 2 * i + 0] = filteredArr[i];
        (*out)[ 2 * i + 1] = 0.;
    }
    four1((*out)-1, fftLen, 1);

    for(int i = 0; i < fftLen/2; ++i)
    {
        (*out)[2*i + 0] = 0.;
        (*out)[2*i + 1] = 0.;
    }
    four1((*out)-1, fftLen, -1);

    for(int i = 0; i < fftLen; ++i)
    {
        tempArr[i] = (*out)[2*i+1]/fftLen*2; //hilbert
    }
    //end Hilbert via FFT


    for(int i = 0; i < fftLen; ++i)
    {
        (*out)[i] = sqrt(tempArr[i]*tempArr[i] + filteredArr[i]*filteredArr[i]);
    }





    //second piece

    for(int i = 0; i < fftLen; ++i)
    {
        (*out)[ 2 * i + 0] = arr[i + inLength-fftLen];
        (*out)[ 2 * i + 1] = 0.;
    }
    four1((*out)-1, fftLen, 1);
    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2.*lowFreq/spStep || i > 2.*highFreq/spStep)
            (*out)[i] = 0.;
    }
    for(int i = fftLen; i < 2*fftLen; ++i)
    {
        if(((2*fftLen - i) < 2.*lowFreq/spStep) || (2*fftLen - i > 2.*highFreq/spStep))
            (*out)[i] = 0.;
    }
    (*out)[0] = 0.;
    (*out)[1] = 0.;
    (*out)[fftLen] = 0.;
    (*out)[fftLen+1] = 0.;
    //end filtering

    four1((*out)-1, fftLen, -1);
    for(int i = 0; i < fftLen; ++i)
    {
        filteredArr[i + inLength - fftLen] = (*out)[2*i]/fftLen*2;
    }


    //Hilbert via FFT
    for(int i = 0; i < fftLen; ++i)
    {
        (*out)[ 2 * i + 0] = filteredArr[i + inLength - fftLen];
        (*out)[ 2 * i + 1] = 0.;
    }
    four1((*out)-1, fftLen, 1);
    for(int i = 0; i < fftLen/2; ++i)
    {
        (*out)[2*i + 0] = 0.;
        (*out)[2*i + 1] = 0.;
    }
    four1((*out)-1, fftLen, -1);

    for(int i = 0; i < fftLen; ++i)
    {
        tempArr[i + inLength - fftLen] = (*out)[2*i+1]/fftLen*2; //hilbert
    }
    //end Hilbert via FFT


    for(int i = 0; i < inLength; ++i)
    {
        (*out)[i] = sqrt(tempArr[i]*tempArr[i] + filteredArr[i]*filteredArr[i]);
    }


    if(!picPath.isEmpty())
    {

        //start check draw - OK
        QPixmap pic(inLength,600);
        QPainter *pnt = new QPainter;
        pic.fill();
        pnt->begin(&pic);
        //    double sum, sum2;
        double enlarge = 10.;

        pnt->setPen("black");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt->drawLine(i, pic.height()/2. - enlarge * filteredArr[i], i+1, pic.height()/2. - enlarge * filteredArr[i+1]);
        }
        pnt->setPen("blue");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            //        pnt->drawLine(i, pic.height()/2. - enlarge * tempArr[i], i+1, pic.height()/2. - enlarge * tempArr[i+1]);
        }
        pnt->setPen("green");
        for(int i = 0; i < pic.width()-1; ++i)
        {
            pnt->drawLine(i, pic.height()/2. - enlarge * (*out)[i], i+1, pic.height()/2. - enlarge * (*out)[i+1]);
        }

        pic.save(picPath, 0, 100);
        pic.fill();
        pnt->end();
        delete pnt;
        cout << "hilber drawn" << endl;
        //end check draw
    }




    delete []tempArr;
    delete []filteredArr;

}

void bayesCount(double * dataIn, int length, int numOfIntervals, double ** out)
{
    double maxAmpl = 80.; //generality
    int helpInt;
    (*out) = new double [numOfIntervals];

    for(int k = 0; k < numOfIntervals; ++k)
    {
        (*out)[k] = 0;
    }
    for(int j = 0; j < length; ++j)
    {
        helpInt = int(floor((dataIn[j] + maxAmpl) / (2.*maxAmpl/double(numOfIntervals))));

        if(helpInt != min(max(0, helpInt), numOfIntervals-1)) continue; //if helpInt not in range

        (*out)[helpInt] += 1;
    }
    for(int k = 0; k < numOfIntervals; ++k)
    {
        (*out)[k] /= double(length)*10.;
    }
}

void kernelEst(double * arr, int length, QString picPath)
{
    double sigma = 0.;

    sigma = variance(arr, length);
    sigma = sqrt(sigma);


    double h = 1.06 * sigma * pow(length, -0.2);



    QPixmap pic(1000, 400);
    double * values = new double [pic.width()];
    QPainter * pnt = new QPainter();
    pic.fill();
    pnt->begin(&pic);
    pnt->setPen("black");


    double xMin, xMax;

    xMin = minValue(arr, length);
    xMax = maxValue(arr, length);

    xMin = floor(xMin)-1;
    xMax = ceil(xMax)+1;

//    //generality
    xMin = -20;
    xMax = 20;

    xMin = 65;
    xMax = 100;

    for(int i = 0; i < pic.width(); ++i)
    {
        values[i] = 0.;
        for(int j = 0; j < length; ++j)
        {
            values[i] += 1/(length*h) * gaussian((xMin + (xMax- xMin) / double(pic.width()) * i - arr[j])/h);
        }
    }

    double valueMax;
    valueMax = maxValue(values, pic.width());

    for(int i = 0; i < pic.width() - 1; ++i)
    {
        pnt->drawLine(i, pic.height() * 0.9 * ( 1. - values[i] / valueMax), i+1, pic.height() * 0.9 * (1. - values[i+1] / valueMax));
//        pnt->drawRect(i, pic.height() * 0.9, i+1, pic.height() * 0.9 * ( 1. - values[xMin + (xMax - Xmin)/pic.width() * (i+1)] / valueMax));
    }
    pnt->drawLine(0, pic.height()*0.9, pic.width(), pic.height()*0.9);

//    int power = log10(xMin);
//    int step = pow(10., floor(power));

    //+=step
    for(int i = ceil(xMin); i <= floor(xMax); ++i)
    {
        pnt->drawLine( (i - xMin) / (xMax - xMin) * pic.width(), pic.height()*0.9+1, (i - xMin) / (xMax - xMin) * pic.width(), pic.height());
        pnt->drawText((i - xMin) / (xMax - xMin) * pic.width(), pic.height()*0.95, QString::number(i));
    }
    pic.save(picPath, 0, 100);

    delete pnt;
    delete []values;
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
    int length = 0;

    ifstream inStream;
    double * arr = new double [250];
    inStream.open(filePath.toStdString().c_str());
    while(!inStream.eof())
    {
        inStream >> arr[length++];
    }
    --length;
    inStream.close();
    return gaussApproval(arr, length);

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
double red(int range, double j, double V, double S)
{
    double part = j / double(range);
    if    (0.000 <= part && part <= 0.167) return V*(1.-S); ///2. - V*S/2. + V*S*(part)*3.;
    else if(0.167 < part && part <= 0.400) return V*(1.-S);
    else if(0.400 < part && part <= 0.500) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.500 < part && part <= 0.600) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.600 < part && part <= 0.833) return V;
    else if(0.833 < part && part <= 1.000) return V - V*S*(part-0.833)/(1.000-0.833)/2.;
    else return 0.0;
}
double green(int range, double j, double V, double S)
{
    double part = j / double(range);
    double hlp = 0.95;
    if    (0.000 <= part && part <= 0.167) return V*(1.-S);
    else if(0.167 < part && part <= 0.400) return V*(1.-S) + V*S*hlp*(part-0.167)/(0.400-0.167);
    else if(0.400 < part && part <= 0.500) return V-V*S*(1.-hlp);
    else if(0.500 < part && part <= 0.600) return V-V*S*(1.-hlp);
    else if(0.600 < part && part <= 0.833) return V-V*S*(1.-hlp) - V*S*hlp*(part-0.600)/(0.833-0.600);
    else if(0.833 < part && part <= 1.000) return V*(1.-S);
    else return 0.0;
}
double blue(int range, double j, double V, double S)
{
    double part = j / double(range);
    if    (0.000 <= part && part <= 0.167) return V -V*S/2. + V*S*(part)/(0.167-0.000)/2.;
    else if(0.167 < part && part <= 0.400) return V;
    else if(0.400 < part && part <= 0.500) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.500 < part && part <= 0.600) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
    else if(0.600 < part && part <= 0.833) return V*(1.-S);
    else if(0.833 < part && part <= 1.000) return V*(1.-S);
    else return 0.0;
}

QColor hueJet(int range, double j, int numOfContours, double V, double S)
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
    return QColor(255.*red(range,j,V,S), 255.*green(range,j,V,S), 255.*blue(range,j,V,S));
}


QColor hueOld(int range, int j)
{
    if(j > range) j = range; //bicycle for no black colour
    if(j < 0) j = 0; //bicycle for no black colour
    return QColor(255.*red1(range,j), 255.*green1(range,j), 255.*blue1(range,j));
//    return QColor(255.*red(range,j,V,S), 255.*green(range,j,V,S), 255.*blue(range,j,V,S));
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


double morletCos(double const freq1, double timeShift, double pot, double time)
{
    double freq = freq1 * 2.*pi/250.;
    return cos(freq*(time-timeShift))*exp(-freq*freq*(time-timeShift)*(time-timeShift)/(pot*pot));
}

double morletSin(double const freq1, double timeShift, double pot, double time)
{
    double freq = freq1 * 2.*pi/250.;
    return sin(freq*(time-timeShift))*exp(-freq*freq*(time-timeShift)*(time-timeShift)/(pot*pot));
}

void drawColorScale(QString filePath, int range, int type)
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
    QPixmap pic(1800, 600);
    pic.fill();
    QPainter * painter = new QPainter;
    painter->begin(&pic);

    for(int i = 0; i < range; ++i)
    {
        switch(type)
        {
        case 0:
        {
            painter->setBrush(QBrush(hueJet(range, i)));
            painter->setPen(hueJet(range, i));
            break;
        }
        case 1:
        {
            painter->setBrush(QBrush(hueOld(range, i)));
            painter->setPen(hueOld(range, i));
            break;
        }
        case 2:
        {
            painter->setBrush(QBrush(grayScale(range, i)));
            painter->setPen(grayScale(range, i));
            break;
        }
        case 3:
        {
            painter->setBrush(QBrush(grayScale(range, i)));
            painter->setPen(grayScale(range, i));
            break;
        }
        }
        painter->drawRect(i*pic.width()/double(range), 0, (i+1)*pic.width()/double(range), 30);
    }
    for(int i = 0; i < range; ++i)
    {
        painter->setPen(QPen(QBrush("red"), 2));
//        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-(i-offR)*(i-offR)/(2*sigmaR*sigmaR)), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-((i+1)-offR)*((i+1)-offR)/(2*sigmaR*sigmaR)));
//        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * red(range, i, 0.95,1.0), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * red(range, int(i+1), 0.95, 1.0));
        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * red1(range, i), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * red1(range, int(i+1)));
        painter->setPen(QPen(QBrush("green"), 2));
//        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-(i-offG)*(i-offG)/(2*sigmaG*sigmaG)), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-((i+1)-offG)*((i+1)-offG)/(2*sigmaG*sigmaG)));
//        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * green(range, i, 0.95,1.0), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * green(range, int(i+1), 0.95, 1.0));
        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * green1(range, i), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * green1(range, int(i+1)));
        painter->setPen(QPen(QBrush("blue"), 2));
//        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-(i-offB)*(i-offB)/(2*sigmaB*sigmaB)), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-((i+1)-offB)*((i+1)-offB)/(2*sigmaB*sigmaB)));
//        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * blue(range, i, 0.95,1.0), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * blue(range, int(i+1), 0.95, 1.0));
        painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * blue1(range, i), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * blue1(range, int(i+1)));

    }
    painter->end();
    pic.save(filePath, 0, 100);

    delete painter;
}

void wavelet(QString out, FILE * file, int ns, int channelNumber, double freqMax, double freqMin, double freqStep, double pot)
{
    int NumOfSlices;
    double * input, helpDouble;
    QString helpString;

    if(file == NULL)
    {
        cout<<"file==NULL"<<endl;
        return;
    }
    fscanf(file, "NumOfSlices %d", &NumOfSlices);
    if(NumOfSlices < 200) return;

    input = new double [NumOfSlices];


    //read the appropriate channel
    for(int i = 0; i < NumOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            if(j!=channelNumber)
            {
                fscanf(file, "%*lf");
            }
            else
            {
                fscanf(file, "%lf", &input[i]);
//                cout << input[i] << endl;
            }
        }
    }



    QPixmap pic(NumOfSlices,800);
    pic.fill();
    QPainter * painter = new QPainter;
    painter->begin(&pic);


    double timeStep = 0.;

    int numberOfFreqs = int(log(freqMin/freqMax) / log(freqStep)) + 1;
//    cout << "numberOfFreqs = " << numberOfFreqs << endl;
    double ** temp = new double * [numberOfFreqs];
    for(int i = 0; i < numberOfFreqs; ++i)
    {
        temp[i] = new double [NumOfSlices];
        for(int j = 0; j < NumOfSlices; ++j)
        {
            temp[i][j] = 0.;
        }
    }


    double tempR = 0., tempI = 0.;
    int i = 0;
    int kMin = 0, kMax = 0;

    int range = 256;
    int numb;

    int j = 0;

    for(double freq = freqMax; freq > freqMin; freq *= freqStep)
    {
        timeStep = 1./freq * 250./1.5;  //in time-bins 250 Hz ////////////////////////////////////////////////////////////////////////////////////////////////
        i = 0;
        while(i < NumOfSlices)
        {
            temp[j][i] = 0.;
            tempR = 0.;
            tempI = 0.;
            //set left & right limits of counting - 5 variances
            kMin = max(0, int(i - sqrt(5 * pow(pot, 2) / pow((freq*2.*pi/250.), 2))));
            kMax = min(NumOfSlices, int(i + sqrt(5 * pow(pot, 2) / pow((freq*2.*pi/250.), 2)))); //i - kMin ~= kMax - i = 5 variances of morlet

            for(int k = kMin; k < kMax; ++k)
            {
                tempI += (morletSin(freq, i, pot, k) * input[k]);
                tempR += (morletCos(freq, i, pot, k) * input[k]);
            }
            temp[j][i] = tempI*tempI + tempR*tempR;
            i += timeStep;
        }
        ++j;
    }

    helpDouble = 0.;
    j = 0;
    for(double freq = freqMax; freq > freqMin; freq *= freqStep)
    {
        timeStep = 1./freq * 250./1.5;
        for(int i = 0; i < NumOfSlices; i += timeStep)
        {
            helpDouble = fmax (helpDouble, temp[j][i]);
//            cout << temp[j][i] << endl;
        }
        ++j;
    }

//    cout << "max = " << helpDouble << endl;


    j = 0;
    for(double freq = freqMax; freq > freqMin; freq *= freqStep)
    {
        timeStep = 1./freq * 250./1.5;  //in time-bins 250 Hz ////////////////////////////////////////////////////////////////////////////////////////////////
        i = 0;
        while(i < NumOfSlices)
        {
             numb = fmin( floor(temp[j][i]*range / double(helpDouble)), double(range));
             numb = pow(numb/double(range), 0.8) * range;

             painter->setBrush(QBrush(hueJet(range, numb)));
             painter->setPen(hueJet(range, numb));

             painter->drawRect( i*pic.width() / NumOfSlices, int(pic.height()*(freqMax-freq  + 0.5*freq*(1. - freqStep)/freqStep) / (freqMax-freqMin) ), timeStep*pic.width()/NumOfSlices, int(pic.height()*( - 0.5*freq*(1./freqStep - freqStep)) / (freqMax-freqMin)));
             i += timeStep;
        }
        ++j;

    }
    painter->setPen("black");


    painter->setFont(QFont("Helvetica", 32, -1, -1));
    for(int i = freqMax; i > freqMin; --i)
    {
        painter->drawLine(0, pic.height()*(freqMax-i)/(freqMax-freqMin), pic.width(), pic.height()*(freqMax-i)/(freqMax-freqMin));
        painter->drawText(0, pic.height()*(freqMax-i)/(freqMax-freqMin)-2, helpString.setNum(i));

    }
    for(int i = 0; i < int(NumOfSlices/250); ++i)
    {
        painter->drawLine(pic.width()*i*250/NumOfSlices, pic.height(), pic.width()*i*250/NumOfSlices, pic.height()-20);
        painter->drawText(pic.width()*i*250/NumOfSlices-8, pic.height()-2, helpString.setNum(i));

    }

    rewind(file);
    delete []input;
    for(int i = 0; i < numberOfFreqs; ++i)
    {
        delete[] temp[i];\
    }
    delete[] temp;
    pic.save(out, 0, 100);
    painter->end();
    delete painter;
}

void matrixTranspose(double ***inMat, int const numRowsCols)
{
    double ** tmp;
    matrixCreate(&tmp, numRowsCols, numRowsCols);

     //transpose to tmp
    for(int i = 0; i < numRowsCols; ++i)
    {
        for(int j = 0; j < numRowsCols; ++j)
        {
            tmp[i][j] = (*inMat)[j][i];
        }
    }


    //tmp to inMat
    for(int i = 0; i < numRowsCols; ++i)
    {
        for(int j = 0; j < numRowsCols; ++j)
        {
            (*inMat)[i][j] = tmp[i][j];
        }
    }
    matrixDelete(&tmp, numRowsCols);
}

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
    QPainter * painter = new QPainter;
    painter->begin(&pic);


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
             painter->setBrush(QBrush(hueJet(range, (temp[i] + pi)/2./pi * range, 0.95, 1.)));
             painter->setPen(hueJet(range, (temp[i] + pi)/2./pi * range, 0.95, 1.));

             painter->drawRect(i*pic.width()/NumOfSlices, int(pic.height()*(freqMax-freq  + 0.5*freq*(1. - freqStep)/freqStep)/(freqMax-freqMin)), timeStep*pic.width()/NumOfSlices,     int(pic.height()*(- 0.5*freq*(1./freqStep - freqStep))/(freqMax-freqMin)));
             i+=timeStep;
        }

    }
//    cout<<"2"<<endl;
    painter->setPen("black");


    painter->setFont(QFont("Helvetica", 32, -1, -1));
    for(int i=freqMax; i>freqMin; --i)
    {
        painter->drawLine(0, pic.height()*(freqMax-i)/(freqMax-freqMin), pic.width(), pic.height()*(freqMax-i)/(freqMax-freqMin));
        painter->drawText(0, pic.height()*(freqMax-i)/(freqMax-freqMin)-2, helpString.setNum(i));

    }
    for(int i=0; i<int(NumOfSlices/250); ++i)
    {
        painter->drawLine(pic.width()*i*250/NumOfSlices, pic.height(), pic.width()*i*250/NumOfSlices, pic.height()-20);
        painter->drawText(pic.width()*i*250/NumOfSlices-8, pic.height()-2, helpString.setNum(i));

    }


    delete []input1;
    delete []input2;
    delete []temp;

    pic.save(out, 0, 100);
    rewind(file);

    painter->end();
    delete painter;
}

void readDataFile(QString filePath, double *** outData, int ns, int * NumOfSlices)
{

    ifstream file;
    file.open(filePath.toStdString().c_str());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    file.ignore(64, ' '); // "NumOfSlices "
    file >> *NumOfSlices;
    *outData = new double * [*NumOfSlices];
    for(int i = 0; i < *NumOfSlices; ++i)
    {
        (*outData)[i] = new double [ns];
        for(int j = 0; j < ns; ++j)
        {
            file >> (*outData)[i][j];
        }
    }
    file.close();
}

template <typename Typ>
void writePlainData(QString outPath,
                    Typ data,
                    int ns,
                    int numOfSlices,
                    int start)
{
    ofstream outStr;
    outStr.open(outPath.toStdString().c_str());
    outStr << "NumOfSlices " << numOfSlices << endl;
    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            outStr << fitNumber(doubleRound(data[j][i + start], 4), 7) << '\t';
        }
        outStr << '\n';
    }
    outStr.flush();
    outStr.close();
}
template void writePlainData(QString outPath,
                            double ** data,
                            int ns,
                            int numOfSlices,
                            int start = 0);
template void writePlainData(QString outPath,
                            vector < vector <double> > data,
                            int ns,
                            int numOfSlices,
                            int start = 0);

template <typename Typ>
void readPlainData(QString inPath,
                   Typ & data,
                   int ns,
                   int & numOfSlices,
                   int start) // data already allocated
{
    ifstream inStr;
    inStr.open(inPath.toStdString().c_str());
    if(!inStr.good())
    {
        cout << "readPlainData: cannot open file" << endl;
        return;
    }
    inStr.ignore(64, ' '); // "NumOfSlices "
    inStr >> numOfSlices;
    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            inStr >> data[j][i + start];
        }
    }
    inStr.close();
}
template void readPlainData(QString inPath,
                            double **& data,
                            int ns,
                            int & numOfSlices,
                            int start);
template void readPlainData(QString inPath,
                            vector < vector <double> > & data,
                            int ns,
                            int & numOfSlices,
                            int start);

void readDataFile(QString filePath, double *** outData, int ns, int * NumOfSlices, int fftLength)
{
    ifstream file;
    file.open(filePath.toStdString().c_str());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    file.ignore(64, ' '); // "NumOfSlices "
    file >> *NumOfSlices;
    *outData = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        (*outData)[i] = new double [fftLength];
    }

    if(*NumOfSlices > fftLength)   //too long - take the end of realisation
    {
        for(int i = fftLength; i < *NumOfSlices; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
                file >> (*outData)[0][0]; //ignore
            }
        }
        for(int i = 0; i < fftLength; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
                file >> (*outData)[j][i];
            }
        }
    }
    else
    {
        for(int i = 0; i < *NumOfSlices; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
                file >> (*outData)[j][i];
            }
        }
        //fill the rest with zeros
        for(int i = *NumOfSlices; i < fftLength; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
                (*outData)[j][i] = 0.;
            }
        }
    }
    file.close();
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

    QPainter * paint = new QPainter();
    paint->begin(&pic);

    QString colour;
    QString lab;
    int lineWidth = 2;


    for(int c2 = 0; c2 < ns; ++c2)
    {
//        if(ns >= 21 && ns < 25)
//        {
//            if(c2 == 19)        colour = "red";
//            else if(c2 == 20)   colour = "blue";
//            else colour = "black";
//        }
//        else
//        {
//            colour = "black";
//        }
//        if(ns == 23 && c2 == 21)
//        {
//            colour = "green";
//        }
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

        paint->setPen(QPen(QBrush(QColor(colour)), lineWidth));

        for(int c1 = 0; c1 < pic.width(); ++c1)
        {
            paint->drawLine(c1, (c2+1)*pic.height()/(ns+2) + dataD[c2][c1] / norm, c1+1, (c2+1)*pic.height()/(ns+2) + dataD[c2][c1+1] / norm);
        }
    }
    norm = 1.;
    paint->setPen(QPen(QBrush("black"), lineWidth));
    for(int c3 = 0; c3 < NumOfSlices * 10 / 250; ++c3)
    {
        if(c3%10 == 0) norm = 20.;
        else if(c3%5  == 0) norm = 15.;

        paint->drawLine(c3 * freq/5, pic.height() - 2, c3 * freq/5, pic.height() - 2*norm);
        paint->drawText(c3 * freq, pic.height() - 35, QString::number(c3));
        norm = 10.;
    }


    norm = 1;
    pic.save(picPath, 0, 100);

    paint->end();
    delete paint;
    return pic;
}


template QPixmap drawEeg(double ** dataD,
                         int ns,
                         int NumOfSlices,
                         int freq,
                         const QString & picPath = "",
                         double norm = 1.,
                         int blueChan = -1,
                         int redChan = -1);

template
QPixmap drawEeg( vector < vector <double> > dataD,
                 int ns,
                 int NumOfSlices,
                 int freq,
                 const QString & picPath,
                 double norm,
                 int blueChan,
                 int redChan);

QPixmap drawEeg(double **dataD, int ns, double startTime, double endTime, int freq, const QString picPath, double norm) // haven't checked
{
    double stretch = ceil(freq/512); // wtf????
    QPixmap pic = QPixmap((endTime-startTime)*freq/stretch, 600);  //cut.cpp 851
    pic.fill();
    QPainter * paint = new QPainter;
    paint->begin(&pic);

    QString colour;
    int lineWidth = 2;

    for(int c1 = 0; c1 < pic.width(); ++c1)
    {
        for(int c2 = 0; c2 < ns; ++c2)
        {
            if(ns > 20 && ns < 25)
            {
                if(c2 == 19)        colour = "red";
                else if(c2 == 20)   colour = "blue";
                else colour = "black";
            }            
            else colour = "black";

            if(ns == 23 && c2 == 21) colour = "green";

            paint->setPen(QPen(QBrush(QColor(colour)), lineWidth));
            paint->drawLine(c1, (c2+1)*pic.height()/(ns+2) +dataD[c2][int(startTime*freq+c1*stretch)]/norm, c1+1, (c2+1)*pic.height()/(ns+2) +dataD[c2][int(startTime*freq+(c1+1)*stretch)]/norm);
        }
    }

    paint->setPen("black");
    for(int c3 = 0; c3 < 25*5+10; ++c3)
    {
        if(c3%5  == 0) norm=15.;
        if(c3%10 == 0) norm=20.;

        paint->drawLine(c3*freq/5, pic.height() - 2, c3*freq/5, pic.height() - 2*norm);
        paint->drawText(c3*freq, pic.height() - 35, QString::number(c3));
        norm = 10.;
    }

    pic.save(picPath, 0, 100);

    paint->end();
    delete paint;
    return pic;
}

void readSpectraFile(QString filePath, double *** outData, int ns, int spLength)
{
    ifstream file(filePath.toStdString().c_str());
    if(!file.good())
    {
        cout << "bad input spectra file:\n" << filePath.toStdString() << endl;
        return;
    }
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < spLength; ++j)
        {
            file >> (*outData)[i][j];
        }
    }
    file.close();
}

void readSpectraFileLine(QString filePath, double **outData, int ns, int spLength)
{
    ifstream file(filePath.toStdString().c_str());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < spLength; ++j)
        {
            file >> (*outData)[i*spLength + j];
        }
    }
    file.close();
}

void readFileInLine(QString filePath, double ** outData, int len)
{
    ifstream file(filePath.toStdString().c_str());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    for(int j = 0; j < len; ++j)
    {
        file >> (*outData)[j];
    }

    file.close();
}

void zeroData(double **& inData, const int &ns, const int &leftLimit, const int &rightLimit, const bool &withMarkersFlag)
{
    for(int i = leftLimit; i < rightLimit; ++i)
    {
        for(int k = 0; k < ns - 1; ++k)
        {
            inData[k][i] = 0.;
        }
        if(!withMarkersFlag) inData[ns - 1][i] = 0.; /// should deprecate
    }
}

void splitZeros(double *** dataIn, const int & ns, const int & length, int * outLength, const QString &logFile, QString dataName)
{
    //remake with std::list of slices

    bool flag[length + 1]; //1 if usual, 0 if eyes, 1 for additional one point
    bool startFlag = false;
    int start = -1;
    int finish = -1;
    int allEyes = 0;
    ofstream outStream;
    double firstMarker = (*dataIn)[ns-1][0];
    double lastMarker =  (*dataIn)[ns-1][length - 1];


    for(int i = 0; i < length; ++i)
    {
        flag[i] = false;
        for(int j = 0; j < ns - 1; ++j) ////////dont consider markers
        {
            if((*dataIn)[j][i] != 0.)
            {
                flag[i] = true;
                break;
            }
        }
    }
    flag[length] = true; // for terminate zero piece

    if(!logFile.isEmpty())
    {
        outStream.open(logFile.toStdString().c_str(), ios_base::app);
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
        if(!startFlag) //if startFlag == false
        {
            if(!flag[i]) //if eyes-slice - set start
            {
                start = i; // first bin to exclude
                startFlag = true;
            }
        }
        else //if startFlag is set
        {
            if(flag[i]) // if we meet the end of eyes-interval OR reached end of a data
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
                for(int k = start; k < start + (length-1 - allEyes) - finish; ++k)
                {
                    for(int j = 0; j < ns; ++j) //shift with markers and flags
                    {
                        (*dataIn)[j][k] = (*dataIn)[j][k + (finish - start)];
                        flag[k] = flag[k + (finish - start)];
                    }
                }

                allEyes += finish - start;
                i -= finish - start;
                startFlag = 0;
            }
        }
        ++i;
    } while (i <= length - allEyes); // = for the last zero piece
    (*outLength) = length - allEyes;
    outStream.close();

    (*dataIn)[ns-1][0] = firstMarker;
//    cout << "splitZeros: lastMarker" << lastMarker << "\t" << matiCountByteStr(lastMarker) << endl;
    (*dataIn)[ns-1][(*outLength) - 1] = lastMarker;
}


void splitZerosEdges(double *** dataIn, int ns, int length, int * outLength)
{
    bool flag[length];
    (*outLength) = length;
    for(int i = 0; i < length; ++i)
    {
        flag[i] = 0;
        for(int j = 0; j < ns; ++j)
        {
            if((*dataIn)[j][i] != 0.)
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
                        (*dataIn)[j][k] = (*dataIn)[j][k + i];
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
    matrixSystemSolveGauss(coefsMatrix, rightVec, dim, &vectorK);
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

void calcSpectre(double ** const inData, int leng, int const ns, double *** dataFFT, int * fftLength, int const NumOfSmooth, const double powArg)
{
    //allocates memory for dataFFT
    //counts best-fit fftLength, Eyes and spectra

    int Eyes;

    if((*fftLength) <= 0)
    {
        if(log(leng)/log(2.) != int(log(leng)/log(2.)))
        {
            (*fftLength) = pow(2, int(log(leng)/log(2.))+1); //nearest exceeding power of 2
        }
        else
        {
            (*fftLength) = leng;
        }
    }
//    cout << (*fftLength) << endl;

    matrixCreate(dataFFT, ns, (*fftLength));

    double ** newData;
    matrixCreate(&newData, ns, (*fftLength));
    for(int j = 0; j < ns; ++j)
    {
        for(int i = 0; i < leng; ++i)
        {
            newData[j][i] = inData[j][i];
        }
//        memcpy(newData[j], inData[j], sizeof(double) * leng);
        for(int i = leng; i < (*fftLength); ++i)
        {
            newData[j][i] = 0.;
        }
    }

    Eyes = 0;
    int h = 0;
    for(int i = 0; i < (*fftLength); ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(fabs(newData[j][i]) <= 0.125) ++h;
        }
        if(h == ns) Eyes += 1;
    }


    double norm1 = (*fftLength) / double((*fftLength)-Eyes);
    double * spectre = new double [(*fftLength)*2];

    double help1, help2;
    int leftSmoothLimit, rightSmoothLimit;

    for(int j = 0; j < ns; ++j)
    {
        for(int i = 0; i < (*fftLength); ++i)            //make appropriate array
        {
            spectre[ i * 2 + 0 ] = (double)(newData[j][ i ] * sqrt(norm1));
            spectre[ i * 2 + 1 ] = 0.;
        }
        four1(spectre-1, (*fftLength), 1);       //Fourier transform
        for(int i = 0; i < (*fftLength)/2; ++i )      //get the absolute value of FFT
        {
            (*dataFFT)[j][ i ] = ( spectre[ i * 2 ] * spectre[ i * 2 ] + spectre[ i * 2 + 1 ]  * spectre[ i * 2 + 1 ] ) * 2 /250. / (*fftLength); //0.004 = 1/250 generality
//            (*dataFFT)[j][ i ] = pow ( (*dataFFT)[j][ i ], powArg );

        }

        leftSmoothLimit = 0;
        rightSmoothLimit = (*fftLength)/2-1;
        //smooth spectre
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
    }
    delete []spectre;
    matrixDelete(&newData, ns);
}


void calcSpectre(double ** const inData, double *** dataFFT, int const ns, int const fftLength, int const Eyes, int const NumOfSmooth, double const powArg)
{

    double norm1 = fftLength / double(fftLength-Eyes);
    double * spectre = new double [fftLength*2];

    double help1, help2;
    int leftSmoothLimit, rightSmoothLimit;

    for(int j = 0; j < ns; ++j)
    {
        for(int i = 0; i < fftLength; ++i)            //make appropriate array
        {
            spectre[ i * 2 + 0 ] = (double)(inData[j][ i ] * sqrt(norm1));
            spectre[ i * 2 + 1 ] = 0.;
        }
        four1(spectre-1, fftLength, 1);       //Fourier transform
        for(int i = 0; i < fftLength/2; ++i )      //get the absolute value of FFT
        {
            (*dataFFT)[j][ i ] = ( spectre[ i * 2 ] * spectre[ i * 2 ] + spectre[ i * 2 + 1 ]  * spectre[ i * 2 + 1 ] ) * 2 /250. / fftLength; //0.004 = 1/250 generality
//            (*dataFFT)[j][ i ] = pow ( (*dataFFT)[j][ i ], powArg );

        }

        leftSmoothLimit = 0;
        rightSmoothLimit = fftLength/2-1;

        //smooth spectre
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
    }
    delete []spectre;
}


void calcRawFFT(double ** const inData, double *** dataFFT, int const ns, int const fftLength, int Eyes, int const NumOfSmooth)
{

    double norm1 = fftLength / double(fftLength-Eyes);
    double * spectre = new double [fftLength*2];

    double help1, help2;
    int leftSmoothLimit, rightSmoothLimit;

    for(int j = 0; j < ns; ++j)
    {
        for(int i = 0; i < fftLength; ++i)            //make appropriate array
        {
            spectre[ i * 2 + 0 ] = (double)(inData[j][ i ] * sqrt(norm1));
            spectre[ i * 2 + 1 ] = 0.;
        }
        four1(spectre-1, fftLength, 1);       //Fourier transform
        for(int i = 0; i < fftLength; ++i )      //get the absolute value of FFT
        {
            (*dataFFT)[j][ i ] = spectre[ i ] * 2. /250. / fftLength; //0.004 = 1/250 generality
        }

        leftSmoothLimit = 0;
        rightSmoothLimit = fftLength/2-1;

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
    delete []spectre;
}

void readPaFile(QString paFile, double *** matrix, int NetLength, int NumOfClasses, int * NumberOfVectors, char *** FileName, double ** classCount)
{
    ifstream paSrc;
    paSrc.open(paFile.toStdString().c_str());
    if(!paSrc.is_open())
    {
        cout << "bad Pa File:" << endl;
        cout << "is_open:\t" << paSrc.is_open() << endl;
        cout << "eof:\t" << paSrc.eof() << endl;
        cout << "good:\t" << paSrc.good() << endl;
        cout << "bad:\t" << paSrc.bad() << endl;
        cout << "fail:\t" << paSrc.fail() << endl;
        cout << "errno = " << errno << endl;

        return;
    }


    for(int i = 0; i < (*NumberOfVectors); ++i)
    {
        if((*matrix)[i]  != NULL) delete [](*matrix)[i];
        if((*FileName)[i]  != NULL) delete [](*FileName)[i];
    }
    if((*matrix) != NULL) delete [](*matrix);
    if((*FileName) != NULL) delete [](*FileName);


    (*NumberOfVectors) = 6000; //generality

    (*matrix) = new double * [(*NumberOfVectors)];
    for(int i = 0; i < (*NumberOfVectors); ++i)
    {
        (*matrix)[i] = new double [NetLength+2]; //+bias +type
    }
//    cout << "readPaFile: data allocated" << endl;
    int num = 0;
    double g[3];  //generality

    (*FileName) = new char * [(*NumberOfVectors)];
    for(int i = 0; i < (*NumberOfVectors); ++i)
    {
        (*FileName)[i] = new char [64];
    }

//    cout << "readPaFile: mem ok" << endl;
    for(int i = 0; i < NumOfClasses; ++i)
    {
        (*classCount)[i] = 0.;
    }
    while(!paSrc.eof())
    {
        paSrc.getline((*FileName)[num], 64);

        for(int i = 0; i < NetLength; ++i)
        {
            paSrc >> (*matrix)[num][i];
        }

        paSrc >> g[0] >> g[1];
        if(NumOfClasses == 3)
        {
            paSrc >> g[2];
        }
        else if(NumOfClasses == 2)
        {
            g[2] = 0.;
        }

        while(paSrc.peek() == int('\n') || paSrc.peek() == int(' '))
        {
            paSrc.get();
        }

        (*matrix)[num][NetLength] = 1.; //bias
        (*matrix)[num][NetLength + 1] = 0. * g[0] + 1. * g[1] + 2. * g[2]; //type
        (*classCount)[int((*matrix)[num][NetLength + 1])] += 1.; //set number of vectors of each class

        if((*matrix)[num][NetLength + 1]  != 0. && (*matrix)[num][NetLength + 1]  != 1. && (*matrix)[num][NetLength + 1]  != 2. && (*matrix)[num][NetLength + 1]  != 1.5)
        {
            cout << "type is wrong " << (*matrix)[num][NetLength + 1] << endl;
            return;
        }
        ++num;
//        cout << "readPaFile: " << num << " file is read" << endl;
    }
    for(int i = num; i < (*NumberOfVectors); ++i)
    {
        delete [] (*matrix)[i];
        delete [] (*FileName)[i];
    }
    paSrc.close();
    (*NumberOfVectors) = num;
}

bool readICAMatrix(QString path, double *** matrixA, int ns)
{
    ifstream inStream;
    inStream.open(path.toStdString().c_str());
    if(!inStream.is_open())
    {
        cout << "cannot open maps File:" << endl << path.toStdString() << endl;
        return 0;
    }
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            inStream >> (*matrixA)[i][j];
        }
    }
    inStream.close();
    return 1;
}


void writeICAMatrix(QString path, double ** matrixA, int const ns)
{
    FILE * map = fopen(path.toStdString().c_str(), "w");
    double maxMagn = 0.;
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fprintf(map, "%.3lf\t", matrixA[i][j]);
            maxMagn = fmax(maxMagn, double(fabs(matrixA[i][j])));
        }
        fprintf(map, "\n");
    }
    fprintf(map, "max = %.3lf\n", maxMagn);
    fclose(map);
}

void matrixCofactor(double ** const inMatrix, int const size, int const numRows, int const numCols, double *** outMatrix)
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

            (*outMatrix)[indexA][indexB] = inMatrix[a][b];
        }
    }
//    cout << "matrixCof: end" << endl;
}

void matrixSystemSolveGauss(double ** const inMat, double * const inVec, int size, double ** outVec)
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
        (*outVec)[i] = tempMat[i][0];
    }

    matrixDelete(&initMat, size);
    matrixDelete(&tempMat, size);
}

void matrixTranspose(double ** const inMat, int const numRows, int const numCols, double *** outMat)
{
    for(int i = 0; i < numCols; ++i)
    {
        for(int j = 0; j < numRows; ++j)
        {
            (*outMat)[i][j] = inMat[j][i];
        }
    }
}

void matrixCopy(double ** const inMat, double *** outMat, int const dimH, int const dimL)
{
    for(int i = 0; i < dimH; ++i)
    {
//        memcpy((*outMat), inMat, dimL * sizeof(double));
        for(int j = 0; j < dimL; ++j)
        {
            (*outMat)[i][j] = inMat[i][j];
        }
    }
}

void matrixInvert(double ** const inMat, int const size, double *** outMat) //cofactors
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
            matrixCofactor(inMat, size, j, i, &cof);
//            cout << "matrixInvert: cofactor\n";
//            matrixPrint(cof, size-1, size-1);
            (*outMat)[i][j] = pow(-1, i+j) * matrixDet(cof, size - 1)/Det;
        }
    }
    for(int i = 0; i < size - 1; ++i)
    {
        delete []cof[i];
    }
    delete []cof;
}

void matrixInvert(double *** mat, int const size) // by definition - cofactors
{
    double ** tempMat;
    matrixCreate(&tempMat, size, size);
    matrixInvert(*mat, size, &tempMat);
    matrixCopy(tempMat, mat, size, size);
    matrixDelete(&tempMat, size);
}

void matrixInvertGauss(double *** mat, int const size) // by definition - cofactors
{
    double ** tempMat;
    matrixCreate(&tempMat, size, size);
    matrixInvertGauss((*mat), size, &tempMat);
    matrixCopy(tempMat, mat, size, size);
    matrixDelete(&tempMat, size);
}

void matrixInvertGauss(double ** const mat, int const size, double *** outMat)
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
            (*outMat)[i][k] = tempMat[i][k];
        }
    }
    matrixDelete(&initMat, size);
    matrixDelete(&tempMat, size);
}

double matrixDet(double ** const matrix, int const dim) //- Det
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
    matrixCopy(matrix, &matrixDet_, dim, dim);

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


double matrixDetB(double ** const matrix, int const dim) // Det
{
    if(dim == 1) return matrix[0][0];

    double ** cof = matrixCreate(dim-1, dim-1);

    double coef = 0.;
    cout << dim << endl;
    for(int i = 0; i < dim; ++i)
    {
        matrixCofactor(matrix, dim, 0, i, &cof);
        coef += matrix[0][i] * pow(-1, i) * matrixDet(cof, dim-1);
    }
    matrixDelete(&cof, dim-1);
    return coef;
}

double ** matrixCreate(int const i, int const j)
{
    double ** mat = new double * [i];
    for(int k = 0; k < i; ++k)
    {
        mat[k] = new double [j];
    }
    return mat;
}

void matrixCreate(double *** matrix, int const i, int const j)
{
    (*matrix) = new double * [i];
    for(int k = 0; k < i; ++k)
    {
        (*matrix)[k] = new double [j];
    }
}

void matrixDelete(double *** matrix, int const i)
{
    for(int k = 0; k < i; ++k)
    {
        delete [](*matrix)[k];
    }
    delete [](*matrix);
}

void matrixDelete(int *** matrix, int const i)
{
    for(int k = 0; k < i; ++k)
    {
        delete [](*matrix)[k];
    }
    delete [](*matrix);
}

void matrixPrint(double ** const mat, int const i, int const j)
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



double matrixInnerMaxCorrelation(double ** const inMatrix, int const numRows, int const numCols, double (*corrFunc)(double * const arr1, double * const arr2, int length, int t))
{
    double res = 0.;
    double temp;
    double ** tempMat;
    matrixCreate(&tempMat, numCols, numRows);
    matrixTranspose(inMatrix, numRows, numCols, &tempMat);
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

double matrixMaxCorrelation(double ** const inMat1, double ** const inMat2, int const numRows, int const numCols)
{
    double res = 0.;
    double temp;
    double ** tempMat1;
    double ** tempMat2;
    matrixCreate(&tempMat1, numCols, numRows);
    matrixTranspose(inMat1, numRows, numCols, &tempMat1);
    matrixCreate(&tempMat2, numCols, numRows);
    matrixTranspose(inMat2, numRows, numCols, &tempMat2);

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


void matrixCorrelations(double ** const inMat1, double ** const inMat2, int const numRows, int const numCols, double ** resCorr)
{
    double res = 0.;
    QList<int> tempNum;
    tempNum.clear();
    double temp;
    int index;
    double ** tempMat1;
    double ** tempMat2;
    matrixCreate(&tempMat1, numCols, numRows);
    matrixTranspose(inMat1, numRows, numCols, &tempMat1);
    matrixCreate(&tempMat2, numCols, numRows);
    matrixTranspose(inMat2, numRows, numCols, &tempMat2);

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
        (*resCorr)[i] = res;
    }
    for(int i = 0; i < numCols; ++i)
    {
        cout << tempNum[i] + 1 << " ";
    }
    cout << endl;
}

void countRCP(QString filePath, QString picPath, double * outMean, double * outSigma)
{
    int counter = 0;
    ifstream inStream;
    double * arr = new double [250];
    inStream.open(filePath.toStdString().c_str());
    if(!inStream.is_open())
    {
        cout << "cant open rcp File: " << filePath.toStdString() << endl;
    }
    while(!inStream.eof())
    {
        inStream >> arr[counter++];
    }
    --counter;

//    cout << filename.left(3).toStdString() << endl;

//    cout << mean(arr, counter) << endl;
//    cout << sigma(arr, counter) << endl;

    (*outMean) = mean(arr, counter);
    (*outSigma) = sigma(arr, counter);

    if(!picPath.isEmpty())
    {
        kernelEst(arr, counter, picPath);
    }

    delete []arr;
}

void makeCfgStatic(QString outFileDir, int NetLength, QString FileName, int numOfOuts, double lrate, double error, int temp)
{
    QString helpString = QDir::toNativeSeparators(outFileDir + QDir::separator() + FileName + ".net");
    FILE * cfgFile = fopen(helpString.toStdString().c_str(), "w");
    if(cfgFile == NULL)
    {
        cout << "static MakeCfg: cannot open file: " << helpString.toStdString() << endl;
        return;
    }
    fprintf(cfgFile, "inputs    %d\n", NetLength);
    fprintf(cfgFile, "outputs    %d\n", numOfOuts);
    fprintf(cfgFile, "lrate    %.2lf\n", lrate);
    fprintf(cfgFile, "ecrit    %.2lf\n", error);
    fprintf(cfgFile, "temp    %d\n", temp);
    fprintf(cfgFile, "srand    %d\n", int(time (NULL))%1234);
    fclose(cfgFile);
}

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
