
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
