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


double fractalDimension(double *arr, int N, QString picPath)
{
    int k; //timeShift
    double L; //average length
//    double sum; //tempSumm

    //NumOfSlices = 8000;
    //max k = 1000; log2(kmax) = 10
    //log(Lmax) = ?

    int minLimit = 2;
//    int maxLimit = int(N/10);
    int maxLimit = floor(log(sqrt(N)) * 4. - 5.);

//    cout<<"minLimit = "<<minLimit<<endl;
//    cout<<"maxLimit = "<<maxLimit<<endl<<endl;

    double * drawK = new double [maxLimit - minLimit];
    double * drawL = new double [maxLimit - minLimit];
//    for(int i = 0; i < N; ++i)
//    {
//        cout<<arr[i]<<" ";
//    }
//    cout<<endl;

//    cout<<"1"<<endl;
//    pic = QPixmap(800, 600);
//    QPainter *pnt     = new QPainter;
//    pic.fill();
//    pnt->begin(&pic);
//    pnt->setPen("black");
//    pnt->setBrush(QBrush("black"));

    for(int h = minLimit; h < maxLimit; ++h)
    {
        k=h;
//        if(h < 5) k = h;
//        else k = exp(log(2) * (h + 5.)/4.);

        L = 0.;
        //count L(k) by averaging Lm(k) over m
//        cout<<"2"<<endl;
        for(int m = 0; m < k; ++m)
        {
            for(int i = 1; i < floor((N - m) / k); ++i)
            {
//                cout<<"3.5 k="<<k<<" m="<<m<<" i="<<i<<" L="<<L<<endl;
//                cout<<arr[m + i * k]
                L += fabs(arr[m + i * k] - arr[m + (i - 1) * k]);
            }
            L = L * (N - 1) / (floor((N - m) / k) * k);
        }
        L /= k;
        drawK[h - minLimit] = log(k);
        drawL[h - minLimit] = log(L);
    }
    for(int h = 0; h < maxLimit - minLimit; ++h)
    {
//        pnt->drawRect(abs((drawK[h] - drawK[0])/(drawK[0] - drawK[maxLimit - minLimit - 1])) * pic.width() - 1, (1. - abs(drawL[h] - drawL[maxLimit - minLimit - 1])/(drawL[0] - drawL[maxLimit - minLimit - 1])) * pic.height() - 1, 2, 2);
    }



//    for(int h = minLimit; h < maxLimit; ++h)
//    {
//        cout<<drawK[h - minLimit]<<"\t";
//    }
//    cout<<endl;
//    for(int h = minLimit; h < maxLimit; ++h)
//    {
//        cout<<drawL[h - minLimit]<<"\t";
//    }
//    cout<<endl;



    //least square approximation
    double slope;
    double add;
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
    add = temp[2] - slope*temp[1]; ///???

//    pnt->setPen("red");
//    pnt->setBrush(QBrush("red"));
//    pnt->drawLine(0, ( (slope*drawK[0]+add - drawL[0])/(drawL[maxLimit - minLimit - 1] - drawL[0])) * pic.height(), pic.width(), ( (slope*drawK[maxLimit - minLimit - 1]+add - drawL[0])/(drawL[maxLimit - minLimit - 1] - drawL[0])) * pic.height());


    slope = -slope;
//    if(picPath.contains("_12.png")) pic.save(picPath);
//    pnt->end();
//    delete pnt;
    delete []temp;
    delete []drawK;
    delete []drawL;
    return slope;
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

double *hilbert(double * arr, int fftLen, double sampleFreq, double lowFreq, double highFreq )
{
    double *spectre = new double [2*fftLen]; //Hilbert
    double spStep = sampleFreq/fftLen;

    for(int i = 0; i < fftLen; ++i)
    {
        spectre[ 2 * i + 0] = arr[i];
        spectre[ 2 * i + 1] = 0.;
    }
    four1(spectre-1, fftLen, 1);

    //start filtering
    for(int i = 0; i < fftLen; ++i)
    {
        if(i < 2.*lowFreq/spStep || i > 2.*highFreq/spStep)
            spectre[i] = 0.;
    }
    for(int i = fftLen; i < 2*fftLen; ++i)
    {
        if(((2*fftLen - i) < 2.*lowFreq/spStep) || (2*fftLen - i > 2.*highFreq/spStep))
            spectre[i] = 0.;
    }
    spectre[0] = 0.;
    spectre[1] = 0.;
    spectre[fftLen] = 0.;
    spectre[fftLen+1] = 0.;
    //end filtering

    four1(spectre-1, fftLen, -1);
    for(int i = 0; i < fftLen; ++i)
    {
        arr[i] = spectre[2*i]*2./fftLen;
    }


    //Hilbert via FFT
    for(int i = 0; i < fftLen; ++i)
    {
        spectre[ 2 * i + 0] = arr[i];
        spectre[ 2 * i + 1] = 0.;
    }
    four1(spectre-1, fftLen, 1);

    for(int i = 0; i < fftLen/2; ++i)
    {
        spectre[2*i + 0] = 0.;
        spectre[2*i + 1] = 0.;
    }
    four1(spectre-1, fftLen, -1);

    for(int i = 0; i < fftLen; ++i)
    {
        spectre[i] = spectre[2*i+1]/fftLen*2;
    }
    //end Hilbert via FFT



/*
    //start check draw - OK
    QPixmap pic(fftLen,600);
    QPainter *pnt = new QPainter;
    pic.fill();
    pnt->begin(&pic);
    double sum, sum2;

    pnt->setPen("black");
    for(int i = 0; i < pic.width()-1; ++i)
    {
        pnt->drawLine(i, pic.height()/2. - arr[i * fftLen/pic.width()], i+1, pic.height()/2. - arr[(i+1) * fftLen/pic.width()]);
    }
    pnt->setPen("blue");
    pnt->setPen("green");
    for(int i = 0; i < pic.width()-1; ++i)
    {
        sum = arr[i * fftLen/pic.width()] * arr[i * fftLen/pic.width()] + spectre[i * fftLen/pic.width()]*spectre[i * fftLen/pic.width()];
        sum = sqrt(sum);
        sum2 = arr[(i+1) * fftLen/pic.width()] * arr[(i+1) * fftLen/pic.width()] + spectre[(i+1) * fftLen/pic.width()] * spectre[(i+1) * fftLen/pic.width()];
        sum2 = sqrt(sum2);
        pnt->drawLine(i, pic.height()/2. - sum, i+1, pic.height()/2. - sum2);
    }

    pic.save("/media/Files/Data/Hilbert.png", 0, 100);
    pic.fill();
    pnt->end();
    delete pnt;
    //end check draw
*/




    return spectre;
}


#include "wavelet.h"

//offset hue
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

void drawColorScale(QString filename, int range)
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
    pic.save(filename, 0, 100);

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

