#include "library.h"


using namespace std;
using namespace std::chrono;

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

                // tempCompl = wCompl * dataFCompl[j/2]
                tempr = wr * dataF[j] - wi * dataF[j + 1];
                tempi = wr * dataF[j + 1] + wi * dataF[j];

                // dataFCompl[j/2] = dataFCompl[i/2] - tempCompl
                dataF[j] = dataF[i] - tempr;
                dataF[j + 1] = dataF[i + 1] - tempi;

                // dataFCompl[i/2] += tempCompl
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

template <typename signalType>
void four1(signalType & inComplexData, int fftLen, int isign)
{
    double * pew = new double [2 * fftLen];
    for(int i = 0; i < 2 * fftLen; ++i)
    {
        pew[i] = inComplexData[i];
    }
    four1(pew - 1, fftLen, isign);
//    four1(dataF.data() - 1, fftLen, isign);
    for(int i = 0; i < 2 * fftLen; ++i)
    {
        inComplexData[i] = pew[i];
    }
    delete []pew;
}

template <typename signalType>
signalType four2(const signalType & inRealData, int fftLen, int isign)
{
    double * pew = new double [2 * fftLen];
    for(int i = 0; i < fftLen; ++i)
    {
        pew[2 * i] = inRealData[i];
        pew[2 * i + 1] = 0.;
    }
//    for(int i = inRealData.size(); i < 2 * fftLen; ++i)
//    {
//        pew[i] = 0.;
//    }

    four1(pew - 1, fftLen, isign);

    signalType res(fftLen);
    for(int i = 0; i < fftLen; ++i)
    {
        res[i] = (pew[2 * i] * pew[2 * i] + pew[2 * i + 1] * pew[2 * i + 1]);
    }
    delete []pew;
    return res;
}

void four3(std::valarray<std::complex<double>> & inputArray)
{
    // DFT
    unsigned int N = inputArray.size();
    unsigned int k = N;
    unsigned int n;
    const double thetaT = pi / N;
    std::complex<double> phiT = std::polar<double>(1., thetaT);
    std::complex<double> T;
    while (k > 1)
    {
        n = k;
        k >>= 1;
        phiT = phiT * phiT; /// remake ???
        T = 1.0L;
        for (unsigned int l = 0; l < k; ++l)
        {
            for (unsigned int a = l; a < N; a += n)
            {
                unsigned int b = a + k;
                std::complex<double> t = inputArray[a] - inputArray[b];
                inputArray[a] += inputArray[b];
                inputArray[b] = t * T;
            }
            T *= phiT;
        }
    }

    // Decimate
    unsigned int m = (unsigned int)log2(N);
    for (unsigned int a = 0; a < N; ++a)
    {
        unsigned int b = a;
        // Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a)
        {
#if 1
            std::swap(inputArray[a], inputArray[b]);
#else
            std::complex<double> t = inputArray[a];
            inputArray[a] = inputArray[b];
            inputArray[b] = t;
#endif
        }
    }
}

void four3(std::valarray<double> & inputRealArray)
{
    std::valarray<std::complex<double>> inputComplexArray(inputRealArray.size());
    std::transform(begin(inputRealArray),
                   end(inputRealArray),
                   begin(inputComplexArray),
                   [](const double & in)
    { return std::complex<double>(in);});
//    for(int i = 0; i < inputRealArray.size(); ++i)
//    {
//        inputComplexArray[i] = std::complex<double>(inputRealArray[i]);
//    }
    four3(inputComplexArray);
//    inputRealArray = inputComplexArray.apply(std::real<double>());
    std::transform(std::begin(inputComplexArray),
                   std::end(inputComplexArray),
                   std::begin(inputRealArray),
                   [](const std::complex<double> & in)
    { return std::real<double>(in);});
//    for(int i = 0; i < inputRealArray.size(); ++i)
//    {
//        inputRealArray[i] = std::real<double>(inputComplexArray[i]);
//    }
}



template <typename signalType>
double fractalDimension(const signalType & arr,
                        const QString & picPath)
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
    double m1 = 0.;
    double m2 = 0.;
    if(!fromZero)
    {
        m1 = mean(arr1, length, shift);
        m2 = mean(arr2, length, shift);
    }
    for(int i = 0; i < length; ++i)
    {
        res += (arr1[i + shift] - m1) *
                (arr2[i + shift] - m2);
    }
    return res;
}

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


int MannWhitney(double * arr1, int len1,
                 double * arr2, int len2,
                 double p)
{
    vectType vect1;
    vectType vect2;
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

template <typename signalType>
int MannWhitney(const signalType & arr1,
                const signalType & arr2,
                const double p)
{
    vector <pair <double, int>> arr;

    // fill first array
    std::for_each(begin(arr1),
                  end(arr1),
                  [&arr](double in)
    {arr.push_back(std::make_pair(in, 0));});

    // fill second array
    std::for_each(begin(arr2),
                  end(arr2),
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

    /// new try DONT WORK??? to test
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

void countMannWhitney(trivector<int> & outMW,
                      const QString & spectraPath,
                      matrix * averageSpectraOut,
                      matrix * distancesOut)
{

    const int NetLength = def::nsWOM() * def::spLength();
    const int numOfClasses = def::numOfClasses();

    QString helpString;
    const QDir dir_(spectraPath);
    vector<QStringList> lst; //0 - Spatial, 1 - Verbal, 2 - Rest
    vector<matrix> spectra(numOfClasses);

    matrix averageSpectra(numOfClasses, NetLength, 0);
    matrix distances(numOfClasses, numOfClasses, 0);

    makeFileLists(spectraPath, lst);

    for(int i = 0; i < numOfClasses; ++i)
    {
        spectra[i].resize(lst[i].length());
        for(int j = 0; j < lst[i].length(); ++j) /// remake : lst[i]
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







//products for ICA
void product1(const matrix & arr,
              const int length,
              const int ns,
              const lineType & vect,
              lineType & outVector)
{
    //<X*g(Wt*X)>
    //vec = Wt
    //X[j] = arr[][j] dimension = ns
    //average over j

    outVector.resize(ns); // and fill zeros

    double sum = 0.;

    for(int j = 0; j < length; ++j)
    {
        sum = prod(vect, arr[j]);
        outVector +=  tanh(sum) * arr[j];
    }
    outVector /= length;
}



void product2(const matrix & arr,
              const int length,
              const int ns,
              const lineType & vect,
              lineType & outVector)
{
    //g'(Wt*X)*1*W
    //vec = Wt
    //X = arr[][j]
    //average over j

    double sum = 0.;
    double sum1 = 0.;

    for(int j = 0; j < length; ++j)
    {
        sum = prod(vect, arr[j]);
        sum1 += 1 - tanh(sum) * tanh(sum);
    }
    sum1 /= length;
    outVector = vect * sum1;
}



//void product3(double ** vec, int ns, int currNum, double ** outVector)
void product3(const matrix & inMat,
              const int ns,
              const int currNum,
              lineType & outVector)
{
    //sum(Wt*Wi*Wi)
    outVector.resize(ns);

    double sum = 0.;
    for(int j = 0; j < currNum; ++j)
    {
        sum = prod(inMat[currNum], inMat[j]); // short valarray, no difference
        outVector += inMat[j] * sum;
    }
}

void randomizeValar(lineType & valar)
{

    srand(time(NULL));
    for(int i = 0; i < valar.size(); ++i)
    {
        valar[i] = rand() % 50 - 25;
    }
    normalize(valar);
}

void countVectorW(matrix & vectorW,
                  const matrix & dataICA,
                  const int ns,
                  const int dataLen,
                  const double vectorWTreshold)
{
    QTime myTime;
    myTime.restart();

    double sum1;
    double sum2;
    lineType vector1(ns);
    lineType vector2(ns);
    lineType vector3(ns);
    lineType vectorOld(ns);

    int counter;

    const matrix tempMatrix = matrix::transpose(dataICA);

    for(int i = 0; i < ns; ++i) //number of current vectorW
    {
        myTime.restart();
        counter = 0;
        randomizeValar(vectorW[i]);

        while(1)
        {
            vectorOld = vectorW[i]; // save previous vect

            /// local, dataICA transposed to tempMatrix
            vector1 = 0.;
            sum1 = 0.;
            for(int j = 0; j < dataLen; ++j)
            {
                const double temp = tanh(prod(vectorW[i], tempMatrix[j]));

                vector1 += tempMatrix[j] * temp;
                sum1 += (1. - temp * temp);
            }
            vector1 /= dataLen;

            sum1 /= dataLen;
            vector2 = vectorW[i] * sum1;


            vectorW[i] = vector1 - vector2;
            //orthogonalization
            product3(vectorW, ns, i, vector3);
            vectorW[i] -= vector3;
            normalize(vectorW[i]);

            sum2 = norma(vectorOld - vectorW[i]);

            ++counter;
            if(sum2 < vectorWTreshold || 2. - sum2 < vectorWTreshold) break;
            if(counter == 100) break;
        }
        cout << "vectW num = " << i << "\t";
        cout << "iters = " << counter << "\t";
        cout << "error = " << fabs(sum2 - int(sum2 + 0.5)) << "\t";
        cout << "time = " << doubleRound(myTime.elapsed() / 1000., 1) << " sec" << endl;
    }
}

void dealWithEyes(matrix & inData,
                  const int dimension)
{

    const int dataLen = inData.cols();
    int eyes = 0;
    for(int i = 0; i < dataLen; ++i)
    {
        const lineType temp = inData.getCol(i, dimension);
        if(abs(temp).max() == 0.)
        {
            ++eyes;
        }
    }
    const double realSignalFrac =  double(dataLen - eyes) / dataLen; /// deprecate?!! splitZeros


//    auto t0 = high_resolution_clock::now();
    // subtract averages
    for(int i = 0; i < dimension; ++i)
    {
        const double temp = - mean(inData[i]) * realSignalFrac;

        std::for_each(begin(inData[i]),
                      end(inData[i]),
                      [temp](double & in)
        {
            if(in != 0.)
            {
                in += temp;
            }
        }); // retain zeros
    }
//    auto t1 = high_resolution_clock::now();
//    cout << duration_cast<microseconds>(t1-t0).count() << endl;
}

void ica(const matrix & initialData,
         matrix & matrixA,
         double eigenValuesTreshold,
         const double vectorWTreshold)
{
    const int ns = initialData.rows();

    matrix centeredMatrix = initialData;
    dealWithEyes(centeredMatrix,
                 ns);

    matrix eigenVectors;
    lineType eigenValues;

    svd(centeredMatrix,
        eigenVectors,
        eigenValues,
        ns,
        eigenValuesTreshold);

    matrix D_minus_05(ns, ns, 0.);
    for(int i = 0; i < ns; ++i)
    {
        D_minus_05[i][i] = 1. / sqrt(eigenValues[i]);
    }

    matrix dataICA = (eigenVectors * (D_minus_05 * matrix::transpose(eigenVectors))) *
                     centeredMatrix;

    matrix vectorW(ns, ns);
    countVectorW(vectorW,
                 dataICA,
                 ns,
                 initialData.cols(),
                 vectorWTreshold);
    dataICA = vectorW * dataICA;

    matrix D_05(ns, ns, 0.);
    for(int i = 0; i < ns; ++i)
    {
        D_05[i][i] = sqrt(eigenValues[i]);
    }
    matrixA = eigenVectors * D_05 * matrix::transpose(eigenVectors) * matrix::transpose(vectorW);


    // norm components to 1-length of mapvector, order by dispersion
    double sum1{};
    for(int i = 0; i < ns; ++i)
    {
        sum1 = norma(matrixA.getCol(i)) / 2.;

        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] /= sum1;
        }
        dataICA[i] *= sum1;
    }


#if 0
    // ordering components by dispersion
    std::vector <std::pair <double, int>> colsNorms; // dispersion, numberOfComponent
    double sumSquares = 0.; // sum of all dispersions

    for(int i = 0; i < ns; ++i)
    {
        sum1 = variance(dataICA[i]);
        sumSquares += sum1;
        colsNorms.push_back(std::make_pair(sum1, i));
    }

    std::sort(colsNorms.begin(),
              colsNorms.end(),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {
        return i.first > j.first;
    });

    for(int i = 0; i < ns - 1; ++i) // dont move the last
    {
        matrixA.swapCols(i, colsNorms[i].second);

        // swap i and colsNorms[i].second values in colsNorms
        auto it1 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [i](std::pair <double, int> in)
        {
                   return in.second == i;
    });
        auto it2 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [colsNorms, i](std::pair <double, int> in)
        {return in.second == colsNorms[i].second;});

//        std::swap((*it1).second, (*it2).second);

        int tempIndex = (*it1).second;
        (*it1).second = (*it2).second;
        (*it2).second = tempIndex;
    }
#endif
}

void svd(const matrix & initialData,
         matrix & eigenVectors,
         lineType & eigenValues,
         const int dimension, // length of the vectors
         const double & threshold,
         int eigenVecNum) // num of eigenVectors to count
{
    if(eigenVecNum <= 0)
    {
        eigenVecNum = dimension;
    }
    const int dataLen = initialData.cols();

    const int iterationsThreshold = 100;
    const int errorStep = 5;

    double trace = 0.;
    for(int i = 0; i < dimension; ++i)
    {
        trace += variance(initialData[i]);
    }

    eigenValues.resize(eigenVecNum);
    eigenVectors.resize(dimension, eigenVecNum);

    lineType tempA(dimension);
    lineType tempB(dataLen);

    double sum1, sum2; //temporary help values
    double dF, F;
    int counter;

#if 0
    /// ICA test short

    const QString pathForAuxFiles = def::dir->absolutePath()
                                    + slash() + "Help"
                                    + slash() + "ica";
    QString helpString = pathForAuxFiles
                         + slash() + def::ExpName + "_eigenMatrix.txt";
    readMatrixFile(helpString,
                    eigenVectors);


    // write eigenValues
    helpString = pathForAuxFiles
                 + slash() + def::ExpName + "_eigenValues.txt";
    readFileInLine(helpString, eigenValues);
    return;
#endif

    // maybe lines longer than dimension but OK

    // lineType tempLine(dataLen); // for debug acceleration

    matrix inData = initialData;
    const matrix inDataTrans = matrix::transpose(initialData);
    QTime myTime;
    myTime.start();
    //counter j - for B, i - for A
    for(int k = 0; k < eigenVecNum; ++k)
    {
        myTime.restart();

        dF = 0.5;
        F = 1.0;

        tempA = 1. / sqrt(dimension);
        tempB = 1. / sqrt(dataLen);

        //approximate P[i] = tempA x tempB;
        counter = 0;
        while(1) //when stop approximate?
        {
            if((counter) % errorStep == 0)
            {
                //countF - error
                F = 0.;
                for(int i = 0; i < dimension; ++i)
                {
#if 0
                    F += 0.5 * pow(inData[i] - tempB * tempA[i], 2.).sum();
#elif 1
                    F += 0.5 * normaSq(inData[i] - tempB * tempA[i]);
#elif 1
                    // much faster in debug
                    const double coef = tempA[i];
                    std::transform(begin(inData[i]),
                                   end(inData[i]),
                                   begin(tempB),
                                   begin(tempLine),
                                   [coef](const double & in1, const double & in2)
                    {
                        return in1 - in2 * coef;
                    });
                    F += 0.5 * normaSq(tempLine);
#else
                    for(int j = 0; j < dataLen; ++j)
                    {
                        F += 0.5 * pow(inData[i][j] - tempB[j] * tempA[i], 2.);
                    }


#endif
                }
            }

            //count vector tempB
            sum2 = 1. / normaSq(tempA);
            for(int j = 0; j < dataLen; ++j)
            {
//                tempB[j] = sum2 * (inDataTrans[j] * tempA).sum();
                // slightly faster
//                tempB[j] = sum2 * std::inner_product(begin(tempA),
//                                                     end(tempA), // begin + dimension
//                                                     begin(inDataTrans[j]),
//                                                     0.);
                tempB[j] = sum2 * prod(tempA, inDataTrans[j]);
            }

            //count vector tempA
            sum2 = 1. / normaSq(tempB);
            for(int i = 0; i < dimension; ++i)
            {
//                tempA[i] = sum2 * (tempB * inData[i]).sum();
                // slightly faster
//                tempA[i] = sum2 * std::inner_product(begin(tempB),
//                                                     end(tempB),
//                                                     begin(inData[i]),
//                                                     0.);
                tempA[i] = sum2 * prod(tempB, inData[i]);
            }

            if((counter) % errorStep == 0)
            {
                dF = 0.;
                for(int i = 0; i < dimension; ++i)
                {
#if 0
                    dF += 0.5 * pow((inData[i] - tempB * tempA[i]), 2.).sum();
#elif 1
                    dF += 0.5 * normaSq(inData[i] - tempB * tempA[i]);
#elif 1
                    // much faster in debug
                    const double coef = tempA[i];
                    std::transform(begin(inData[i]),
                                   end(inData[i]),
                                   begin(tempB),
                                   begin(tempLine),
                                   [coef](const double & in1, const double & in2)
                    {
                        return in1 - in2 * coef;
                    });
                    dF += 0.5 * normaSq(tempLine);
#else
                    for(int j = 0; j < dataLen; ++j)
                    {
                        dF += 0.5 * pow(inData[i][j] - tempB[j] * tempA[i], 2.);
                    }
#endif
                }
                dF = 1. - dF / F;

            }

            if(counter == iterationsThreshold)
            {
                break;
            }
            ++counter;
            if(fabs(dF) < threshold) break; //crucial cap
        }

        //edit currMatrix
        /// test!
        for(int i = 0; i < dimension; ++i)
        {
#if 1
            // better in release
            inData[i] -= tempA[i] * tempB;
#else
            // better in debug
            const double coef = tempA[i];
            std::transform(begin(inData[i]),
                           end(inData[i]),
                           begin(tempB),
                           begin(tempLine),
                           [coef](const double & in1, const double & in2)
            {
                return in1 - in2 * coef;
            });
#endif
        }


        //count eigenVectors && eigenValues
        sum1 = normaSq(tempA);
        sum2 = normaSq(tempB);
        eigenValues[k] = sum1 * sum2 / double(dataLen - 1.);
        tempA /= sqrt(sum1);

        sum1 = std::accumulate(begin(eigenValues),
                               begin(eigenValues) + k + 1,
                               0.);

        cout << k << "\t";
        cout << "val = " << doubleRound(eigenValues[k], 3) << "\t";
        cout << "disp = " << doubleRound(100. * eigenValues[k] / trace, 2) << "\t";
        cout << "total = " << doubleRound(100. * sum1 / trace, 2) << "\t";
        cout << "iters = " << counter << "\t";
        cout << doubleRound(myTime.elapsed()/1000., 1) << " s" << endl;

        for(int i = 0; i < dimension; ++i)
        {
            eigenVectors[i][k] = tempA[i]; // 1-normalized coloumns
        }
    }
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

void wavelet(QString filePath,
             QString picPath,
             int channelNumber,
             int ns)
{
    // continious
    int NumOfSlices;
    double helpDouble;

    matrix fileData;
    readPlainData(filePath, fileData, NumOfSlices);

    lineType input = fileData[channelNumber];

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



matrix waveletDiscrete(const vectType & inData)
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
    return res;
}


template <typename signalType>
matrix countWavelet(const signalType & inSignal)
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


template <typename signalType, typename retType>
retType hilbert(const signalType & arr,
                double lowFreq,
                double highFreq,
                QString picPath)
{

    const int inLength = arr.size();
    const int fftLen = fftL(inLength); // int(pow(2., ceil(log(inLength)/log(2.))));
    const double spStep = def::freq / fftLen;
    const double normCoef = sqrt(fftLen / double(inLength));

    retType out; // result
    out.resize(2 * fftLen);

    vectType tempArr;
    tempArr.resize(fftLen, 0.);

    vectType filteredArr;
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


template <typename signalType, typename retType>
retType hilbertPieces(const signalType & arr,
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
    retType outHilbert(inLength);
    int fftLen = fftL(inLength) / 2;

    double spStep = sampleFreq/fftLen;
    vectType out(2*fftLen); // temp
    vectType tempArr(inLength);
    vectType filteredArr(inLength);

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

template <typename signalType, typename retType>
retType bayesCount(const signalType & dataIn,
               int numOfIntervals)
{
    double maxAmpl = 80.; //generality
    int helpInt;
    retType out(0., numOfIntervals);

    for(int j = 0; j < dataIn.size(); ++j)
    {
        helpInt = int(floor((dataIn[j] + maxAmpl) / (2. * maxAmpl / double(numOfIntervals))));

        if(helpInt != min(max(0, helpInt), numOfIntervals - 1))
        {
            continue; //if helpInt not in range
        }
        out[helpInt] += 1;
    }
    out /= double(dataIn.size()) * 10.; // 10 is norm coef for perceptron

    return out;
}



void kernelEst(QString filePath, QString picPath)
{
    vectType arr;
    readFileInLine(filePath, arr);
    kernelEst(arr, picPath);
}

template <typename signalType>
void kernelEst(const signalType & arr, QString picPath)
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

    vectType values(pic.width(), 0.);

    double xMin, xMax;

    xMin = *std::min(begin(arr),
                    end(arr));
    xMax = *std::max(begin(arr),
                    end(arr));

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


template <typename signalType, typename retType>
retType spectre(const signalType & data)
{
    int length = data.size();
    int fftLen = fftL(length); // nearest exceeding power of 2
    double norm = sqrt(fftLen / double(length));

    vectType tempSpectre(2 * fftLen, 0.);
    for(int i = 0; i < length; ++i)
    {
        tempSpectre[ 2 * i + 0 ] = data[i] * norm;
    }
    four1(tempSpectre.data() - 1, fftLen, 1);

    retType spectr(fftLen / 2);
    norm = 2. / (def::freq * fftLen);
    for(int i = 0; i < fftLen / 2; ++i )      //get the absolute value of FFT
    {
        spectr[ i ] = (pow(tempSpectre[ i * 2 ], 2.)
                      + pow(tempSpectre[ i * 2 + 1 ], 2.)) * norm;
    }
    return spectr;
}

template <typename signalType, typename retType>
retType smoothSpectre(const signalType & inSpectre, const int numOfSmooth)
{
    retType result = inSpectre;
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


void zeroData(matrix & inData, const int & leftLimit, const int & rightLimit)
{
    for(int k = 0; k < def::nsWOM(); ++k) /// don't affect markers
    {
        std::for_each(begin(inData[k]) + leftLimit,
                      begin(inData[k]) + rightLimit,
                      [](double & in){in = 0.;});
    }
}

void splitZeros(matrix & dataIn,
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
                    // vector
//                    dataIn[j].erase(dataIn[j].begin() + start,
//                                    dataIn[j].begin() + finish);
                    // valarray, resizing further
                    std::remove_if(begin(dataIn[j]) + start,
                                   begin(dataIn[j]) + finish,
                                   [](double){return true;});
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

    // valarray
    dataIn.resizeCols(*outLength);

    outStream.close();

//    cout << "allEyes = " << allEyes << endl;

    dataIn[markChan][0] = firstMarker;
    dataIn[markChan][(*outLength) - 1] = lastMarker;
}


void splitZerosEdges(matrix & dataIn, const int & ns, const int & length, int * outLength)
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
                  [fftLength](vectType & in){in.resize(fftLength/2);});

//    double help1, help2;
//    int leftSmoothLimit, rightSmoothLimit;

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


        //smooth spectre - odd and even split
        /*
        leftSmoothLimit = 0;
        rightSmoothLimit = fftLength / 2 - 1;
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


template <typename signalType = lineType>
void calcSpectre(const signalType & inSignal,
                 signalType & outSpectre,
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
#if 0
    const double norm2 = 2. / (def::freq * fftLength);
    vector<double> spectre (fftLength * 2, 0.); // can be valarray, but not important
    for(int i = 0; i < fftLength; ++i)
    {
        spectre[ i * 2 ] = inSignal[ i ] * norm1;
    }
    four1(spectre, fftLength, 1);
    for(int i = 0; i < fftLength / 2; ++i )
    {
        outSpectre[ i ] = (pow(spectre[ i * 2 ], 2) + pow(spectre[ i * 2 + 1 ], 2)) * norm2;
//        outSpectre[ i ] = pow ( outSpectre[ i ], powArg );
    }
#else
    const double nrm = 2. / (double(fftLength - Eyes) * def::freq);
    outSpectre = four2(inSignal, fftLength, 1) * nrm;
#endif
//    outSpectre = pow(four2(inSignal, fftLength, 1) * nrm, powArg);



    //smooth spectre
    const int leftSmoothLimit = 1;
    const int rightSmoothLimit = fftLength / 2 - 1;
    double help1, help2;
    for(int a = 0; a < (int)(NumOfSmooth / norm1); ++a)
    {
        help1 = outSpectre[leftSmoothLimit - 1];
        for(int k = leftSmoothLimit; k < rightSmoothLimit; ++k)
        {
            help2 = outSpectre[k];
            outSpectre[k] = (help1 + help2 + outSpectre[k+1]) / 3.;
            help1 = help2;
        }
    }
}


template double covariance(const double * const &arr1, const double * const &arr2, int length, int shift, bool fromZero);
template double covariance(const int * const &arr1, const int * const &arr2, int length, int shift, bool fromZero);
template double covariance(const vector<int> &arr1, const vector<int> &arr2, int length, int shift, bool fromZero);
template double covariance(const vector<double> &arr1, const vector<double> &arr2, int length, int shift, bool fromZero);
template double covariance(const lineType &arr1, const lineType &arr2, int length, int shift, bool fromZero);

template double correlation(const double * const &arr1, const double * const &arr2, int length, int shift, bool fromZero);
template double correlation(const int * const  &arr1, const int * const &arr2, int length, int shift, bool fromZero);
template double correlation(const vector<int> &arr1, const vector<int> &arr2, int length, int shift, bool fromZero);
template double correlation(const vector<double> &arr1, const vector<double> &arr2, int length, int shift, bool fromZero);
template double correlation(const lineType &arr1, const lineType &arr2, int length, int shift, bool fromZero);

template matrix countWavelet(const lineType & inSignal);
template matrix countWavelet(const vectType & inSignal);

template void four1(lineType & dataF, int nn, int isign);
template void four1(vectType & dataF, int nn, int isign);

template lineType four2(const lineType & inRealData, int fftLen, int isign);

template lineType spectre(const vectType & data);
template lineType spectre(const lineType & data);

template lineType smoothSpectre(const lineType & inSpectre, const int numOfSmooth);

template lineType hilbert(const lineType & arr, double lowFreq, double highFreq, QString picPath);
template lineType hilbert(const vectType & arr, double lowFreq, double highFreq, QString picPath);

template lineType hilbertPieces(const lineType & arr, int inLength, double sampleFreq, double lowFreq, double highFreq, QString picPath);
template lineType hilbertPieces(const vectType & arr, int inLength, double sampleFreq, double lowFreq, double highFreq, QString picPath);

template lineType bayesCount(const vectType & dataIn, int numOfIntervals);
template lineType bayesCount(const lineType & dataIn, int numOfIntervals);

template double fractalDimension(const lineType &arr, const QString &picPath = QString());
template double fractalDimension(const vectType &arr, const QString &picPath = QString());

template void calcRawFFT(const mat & inData, mat & dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth);
template void calcRawFFT(const matrix & inData, mat & dataFFT, const int &ns, const int &fftLength, const int &Eyes, const int &NumOfSmooth);

template void calcSpectre(const lineType & inSignal, lineType & outSpectre, const int & fftLength, const int & NumOfSmooth, const int & Eyes, const double & powArg);
//template void calcSpectre(const vectType & inSignal, vectType & outSpectre, const int & fftLength, const int & NumOfSmooth, const int & Eyes, const double & powArg);

template void kernelEst(const vectType & arr, QString picPath);
template void kernelEst(const lineType & arr, QString picPath);
