#include "library.h"

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr



void four1(double *dataF, int nn, int isign)
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
        theta = isign * (6.28318530717959 / mmax);
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

double doubleRound(double in, int numSigns)
{
    return int(in*pow(10., numSigns))/pow(10., numSigns);
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

double mean(int *arr, int length)
{
    double sum = 0.;
    for(int i = 0; i < length; ++i)
    {
        sum += arr[i];
    }
    sum /= (double)length;
    return sum;
}

double mean(double *arr, int length)
{
    double sum = 0.;
    for(int i = 0; i < length; ++i)
    {
        sum += arr[i] / double(length);
    }
    return sum;
}

double variance(int *arr, int length)
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


double variance(double * arr, int length)
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


double sigma(int *arr, int length)
{
    return sqrt(variance(arr, length));
}

double sigma(double *arr, int length)
{
    return sqrt(variance(arr, length));
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

double minValue(double * arr, int length)
{
    double res = arr[0];
    for(int i = 0; i < length; ++i)
    {
        res = fmin(res, arr[i]);
    }
    return res;
}


double correlation(double * const arr1, double * const arr2, int length, int t)
{
    double res = 0.;
    double m1, m2;
    int T = abs(t);
    if(t >= 0) //start from arr1[0] and arr1[t]
    {
        m1 = mean(arr1, length-t);
        m2 = mean(arr2+t, length-t);
        for(int i = 0; i < length - t; ++i)
        {
            res += (arr1[i] - m1) * (arr2[i + t] - m2);
        }

        res /= sqrt(variance(arr1, length-t) * variance(arr2+t, length-t));
    }
    else  //start from arr1[0] and arr1[t]
    {
        m1 = mean(arr1+T, length-T);
        m2 = mean(arr2, length-T);
        for(int i = 0; i < length - T; ++i)
        {
            res += (arr1[i + T] - m1) * (arr2[i] - m2);
        }

        res /= sqrt(variance(arr1 + T, length - T) * variance(arr2, length - T));
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

QString rightNumber(int &input, int N)
{
    QString h;
    h.setNum(input);
    if(input== 0)
    {
        h="";
        for(int j = 0; j < N; ++j)
        {
            h.prepend("0");
        }
        return h;
    }
    for(int i=1; i < N; ++i)
    {
        if(log10(input)>=i-1 && log10(input) < i)
            for(int j=i; j < N; ++j)
            {
                h.prepend("0");
            }
    }
    return h;
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

void hilbert(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath="")
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

void hilbertPieces(double * arr, int inLength, double sampleFreq, double lowFreq, double highFreq, double ** out, QString picPath="")
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
    xMin = 70;
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

double red(int range, int j, double V, double S)
{
    if(0.000 <= j/double(range) && j/double(range) <= 0.167) return V*(1.-S); ///2. - V*S/2. + V*S*(j/double(range))*3.;
    if(0.167 < j/double(range) && j/double(range) <= 0.400) return V*(1.-S);
    if(0.400 < j/double(range) && j/double(range) <= 0.500) return V*(1.-S) + V*S*(j/double(range)-0.400)/(0.500-0.400)/2.;
    if(0.500 < j/double(range) && j/double(range) <= 0.600) return V*(1.-S) + V*S*(j/double(range)-0.400)/(0.500-0.400)/2.;
    if(0.600 < j/double(range) && j/double(range) <= 0.833) return V;
    if(0.833 < j/double(range) && j/double(range) <= 1.000) return V - V*S*(j/double(range)-0.833)/(1.000-0.833)/2.;
    return 0.0;
}
double green(int range, int j, double V, double S)
{
    double hlp = 0.95;
    if(0.000 <= j/double(range) && j/double(range) <= 0.167) return V*(1.-S);
    if(0.167 < j/double(range) && j/double(range) <= 0.400) return V*(1.-S) + V*S*hlp*(j/double(range)-0.167)/(0.400-0.167);
    if(0.400 < j/double(range) && j/double(range) <= 0.500) return V-V*S*(1.-hlp);
    if(0.500 < j/double(range) && j/double(range) <= 0.600) return V-V*S*(1.-hlp);
    if(0.600 < j/double(range) && j/double(range) <= 0.833) return V-V*S*(1.-hlp) - V*S*hlp*(j/double(range)-0.600)/(0.833-0.600);
    if(0.833 < j/double(range) && j/double(range) <= 1.000) return V*(1.-S);
    return 0.0;
}
double blue(int range, int j, double V, double S)
{
    if(0.000 <= j/double(range) && j/double(range) <= 0.167) return V -V*S/2. + V*S*(j/double(range))/(0.167-0.000)/2.;
    if(0.167 < j/double(range) && j/double(range) <= 0.400) return V;
    if(0.400 < j/double(range) && j/double(range) <= 0.500) return V - V*S*(j/double(range)-0.400)/(0.500-0.400)/2.;
    if(0.500 < j/double(range) && j/double(range) <= 0.600) return V - V*S*(j/double(range)-0.400)/(0.500-0.400)/2.;
    if(0.600 < j/double(range) && j/double(range) <= 0.833) return V*(1.-S);
    if(0.833 < j/double(range) && j/double(range) <= 1.000) return V*(1.-S);
    return 0.0;
}

QColor hue(int range, int j, double V, double S)
{
    S = 1.0;
    V = 0.95;
    return QColor(255.*red(range,j,V,S), 255.*green(range,j,V,S), 255.*blue(range,j,V,S));
}

QColor qcolor(int range, int j)
{
    double sigmaR=range*0.35;
    double sigmaG=range*0.25;
    double sigmaB=range*0.4;

    double offR=range*(1.0 - 0.3);
    double offG=range*(0.5 + 0.25);
    double offB=range*(0.0 + 0.15);

    return QColor(255*exp(-(j-offR)*(j-offR)/(2*sigmaR*sigmaR)), 255*exp(-(j-offG)*(j-offG)/(2*sigmaG*sigmaG)), 255*exp((-(j-offB)*(j-offB)/(2*sigmaB*sigmaB))));
//    return QColor(255*red(j/double(range)),255* green(j/double(range)), 255*blue(j/double(range)));
}

double morletCos(double freq, double timeShift, double pot, double time)
{
    freq*=2.*pi/250.;
    return cos(freq*(time-timeShift))*exp(-freq*freq*(time-timeShift)*(time-timeShift)/(pot*pot));
}

double morletSin(double freq, double timeShift, double pot, double time)
{
    freq*=2.*pi/250.;
    return sin(freq*(time-timeShift))*exp(-freq*freq*(time-timeShift)*(time-timeShift)/(pot*pot));
}

void drawColorScale(QString filePath, int range)
{
//    double sigmaR=range*0.35;
//    double sigmaG=range*0.25;
//    double sigmaB=range*0.4;

//    double offR=range*(1.0 - 0.3);
//    double offG=range*(0.5 + 0.25);
//    double offB=range*(0.0 + 0.15);

    QPixmap pic(1800, 600);
    pic.fill();
    QPainter * painter = new QPainter;
    painter->begin(&pic);

    for(int i=0; i<range; ++i)
    {
        painter->setBrush(QBrush(hue(range, i, 0.95, 1.0)));
        painter->setPen(hue(range, i, 0.95, 1.0));
        painter->drawRect(i*pic.width()/double(range), 0, (i+1)*pic.width()/double(range), 30);
    }
    for(int i=0; i<range; ++i)
    {
        painter->setPen(QPen(QBrush("red"), 2));
//            painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-(i-offR)*(i-offR)/(2*sigmaR*sigmaR)), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-((i+1)-offR)*((i+1)-offR)/(2*sigmaR*sigmaR)));
            painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * red(range, i, 0.95,1.0), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * red(range, int(i+1), 0.95, 1.0));
        painter->setPen(QPen(QBrush("green"), 2));
//            painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-(i-offG)*(i-offG)/(2*sigmaG*sigmaG)), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-((i+1)-offG)*((i+1)-offG)/(2*sigmaG*sigmaG)));
            painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * green(range, i, 0.95,1.0), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * green(range, int(i+1), 0.95, 1.0));
        painter->setPen(QPen(QBrush("blue"), 2));
//            painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-(i-offB)*(i-offB)/(2*sigmaB*sigmaB)), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * exp(-((i+1)-offB)*((i+1)-offB)/(2*sigmaB*sigmaB)));
            painter->drawLine(i*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * blue(range, i, 0.95,1.0), (i+1)*pic.width()/double(range), pic.height()-20 - (pic.height()-20 - 50) * blue(range, int(i+1), 0.95, 1.0));

    }
    painter->end();
    pic.save(filePath, 0, 100);

    delete painter;
}

void wavelet(QString out, FILE * file, int ns=19, int channelNumber=0, double freqMax=20., double freqMin=5., double freqStep=0.99, double pot=32.)
{
    int NumOfSlices;
    double * input, helpDouble;
    QString helpString;

    if(file==NULL)
    {
        cout<<"file==NULL"<<endl;
        return;
    }
    fscanf(file, "NumOfSlices %d", &NumOfSlices);
    if(NumOfSlices<200) return;
    input = new double [NumOfSlices];

    for(int i=0; i<NumOfSlices; ++i)
    {
        for(int j=0; j<ns; ++j)
        {
            if(j!=channelNumber) fscanf(file, "%lf", &helpDouble);
            else fscanf(file, "%lf", &input[i]);
        }
    }
//    fclose(file);
    QPixmap pic(150*NumOfSlices/250,800); //150 pixels/sec generality
    pic.fill();
    QPainter * painter = new QPainter;
    painter->begin(&pic);


    double freq=20.;
    double timeStep=0.;

    int numberOfFreqs = int(log(freqMin/freqMax) / log(freqStep)) + 1;
    double **temp = new double * [numberOfFreqs];
    for(int i = 0; i < numberOfFreqs; ++i)
    {
        temp[i] = new double [NumOfSlices];
        for(int j = 0; j < NumOfSlices; ++j)
        {
            temp[i][j] = 0.;
        }
    }
    double tempR=0., tempI=0.;
    int i=0;
    int jMin=0, jMax=0;

    int range = 256;
    int numb;
//    cout<<"1"<<endl;
    int j=0;
    for(freq=freqMax; freq>freqMin; freq*=freqStep)
    {
        timeStep = 1./freq * 250./1.5;  //250 Hz
        i=0;
        while(i<NumOfSlices)
        {
            temp[j][i]=0.;
            tempR=0.;
            tempI=0.;
            jMin=max(0, int(i - sqrt(5*pot*pot*250.*250./4/pi/pi/freq/freq)));
            jMax=min(int(i + sqrt(5*pot*pot*250.*250./4/pi/pi/freq/freq)), NumOfSlices);
            for(int j=jMin; j<jMax; ++j)
            {
                tempI+=(morletSin(freq, i, pot, j)*input[j]);
                tempR+=(morletCos(freq, i, pot, j)*input[j]);
            }
            temp[j][i]=tempI*tempI + tempR*tempR;
            i+=timeStep;
        }
        ++j;
    }
    helpDouble = 0.;
    for(int j = 0; j < numberOfFreqs; ++j)
    {
        for(int i = 0; i < NumOfSlices; ++i)
        {
            helpDouble = fmax (helpDouble, temp[j][i]);
        }
    }


    j = 0;
    for(freq=freqMax; freq>freqMin; freq*=freqStep)
    {
        i=0;
        while(i<NumOfSlices)
        {
             numb = min(floor(temp[j][i]*range/double(helpDouble)), double(range));

             painter->setBrush(QBrush(qcolor(range, numb)));
             painter->setPen(qcolor(range, numb));

             painter->drawRect(i*pic.width()/NumOfSlices, int(pic.height()*(freqMax-freq  + 0.5*freq*(1. - freqStep)/freqStep)/(freqMax-freqMin)), timeStep*pic.width()/NumOfSlices,     int(pic.height()*(- 0.5*freq*(1./freqStep - freqStep))/(freqMax-freqMin)));
             i+=timeStep;
        }
        ++j;

    }
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
             painter->setBrush(QBrush(hue(range, (temp[i] + pi)/2./pi * range, 0.95, 1.)));
             painter->setPen(hue(range, (temp[i] + pi)/2./pi * range, 0.95, 1.));

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
    file.ignore(12); // "NumOfSlices "
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

void readDataFile(QString filePath, double *** outData, int ns, int * NumOfSlices, int fftLength)
{
    ifstream file;
    file.open(filePath.toStdString().c_str());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    file.ignore(12); // "NumOfSlices "
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

void readSpectraFile(QString filePath, double *** outData, int ns, int spLength)
{
    ifstream file(filePath.toStdString().c_str());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    (*outData) = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        (*outData)[i] = new double [spLength];
        for(int j = 0; j < spLength; ++j)
        {
            file >> (*outData[i][j]);
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

void splitZeros(double *** dataIn, int ns, int length, int * outLength)
{
    bool flag[length];
    bool startFlag = 0;
    int start = -1;
    int finish = -1;
    int allEyes = 0;
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
    for(int i = 0; i < length; ++i)
    {
        if(flag[i] == 0 && startFlag == 0)
        {
            start = i;
            startFlag = 1;
        }

        if((flag[i] == 1 && startFlag == 1) || (i == length-1 && startFlag == 1))
        {
            finish = i;
            startFlag = 0;
            //split
            for(int k = start; k < finish; ++k)
            {
                for(int j = 0; j < ns; ++j)
                {
                    (*dataIn)[j][k] = (*dataIn)[j][k + finish - start - allEyes];
                }
            }
            allEyes += finish-start;
        }
    }
    (*outLength) = length - allEyes;
//    cout << length << "\t" << allEyes << "\t" << (*outLength) << "\t";
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
    cout << (*fftLength) << endl;

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

void readPaFile(QString paFile, double *** matrix, int NetLength, int NumOfClasses, int * NumberOfVectors, char *** FileName)
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
        cout << errno << endl;

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
    while(!paSrc.eof())
    {
        paSrc.getline((*FileName)[num], 64);
//        cout << (*FileName)[num] << endl;

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

void readICAMatrix(QString path, double *** matrixA, int ns)
{
    FILE * map = fopen(path.toStdString().c_str(), "r");
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fscanf(map, "%lf", &((*matrixA)[i][j]));
        }
    }
    fclose(map);
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

void matrixInvert(double *** mat, int const size)
{
    double ** tempMat;
    matrixCreate(&tempMat, size, size);
    matrixInvert(*mat, size, &tempMat);
    matrixCopy(tempMat, mat, size, size);
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


double matrixInnerMaxCorrelation(double ** const inMatrix, int const numRows, int const numCols)
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
            temp = correlation(tempMat[i], tempMat[j], numRows);
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

void svd(double ** inData, int size, int length, double *** eigenVects, double ** eigenValues)
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
            sum1 += tempA[i]*tempA[i];
        }
        for(int j = 0; j < length; ++j)
        {
            sum2 += tempB[j]*tempB[j];
        }
        for(int i = 0; i < size; ++i)
        {
            tempA[i]/=sqrt(sum1);
        }
        for(int j = 0; j < length; ++j)
        {
            tempB[j] /= sqrt(sum2);
        }

        (*eigenValues)[k] = sum1*sum2/double(length - 1.);
        cout << "numOfPC = " << k << "\tvalue = " << (*eigenValues)[k] << "\t iterations = " << counter << endl;
        for(int i = 0; i < size; ++i)
        {
            (*eigenVects)[i][k] = tempA[i]; //1-normalized
        }
    }

}
