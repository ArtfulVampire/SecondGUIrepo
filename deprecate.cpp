#include "library.h"



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
*/

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
            cout << doubleRound(mat[a][b], 4) << "\t";
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
/*
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
*/


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
            if(signal1[k] >= min1 + (max1-min1)/numOfInts * i   &&
               signal1[k] < min1 + (max1-min1)/numOfInts * (i+1)) counter += 1;
        }
        self1 = counter/length;
        for(int j = 0; j < numOfInts; ++j) //intervals of 2
        {
            counter = 0;
            for(int k = 0; k < length; ++ k)
            {
                if(signal2[k] >= min2 + (max2-min2)/numOfInts * j   &&
                   signal2[k] < min2 + (max2-min2)/numOfInts * (j+1)) counter += 1;
            }
            self2 = counter/length;


            counter = 0;
            for(int k = 0; k < length; ++ k)
            {
                if(signal2[k] >= min2 + (max2-min2)/numOfInts * j   &&
                   signal2[k] < min2 + (max2-min2)/numOfInts * (j+1) &&
                   signal1[k] >= min1 + (max1-min1)/numOfInts * i   &&
                   signal1[k] < min1 + (max1-min1)/numOfInts * (i+1)) counter += 1;
            }
            together = counter/length;

            if(1)//together != 0.)
            {
                cout << i << "\t";
                cout << j << "\t";
                cout << doubleRound(self1, 5) << "\t";
                cout << doubleRound(self2, 5) << "\t";
                cout << doubleRound(self1 * self2, 5) << "\t";
                cout << doubleRound(together, 5) << "\t";

                //                cout << (together - self1*self2) / (together);
                cout << endl;
            }

            haupt += fabs(together - self1 * self2);// / (together * pow(numOfInts, 2.));
        }

    }
    return haupt;
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
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
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



template void matrixProduct(const double ** const &inMat1, const double ** const &inMat2, double **& outMat, int dimH, int dimL);
template void matrixProduct(const double ** const &inMat1, const double ** const &inMat2, mat & outMat, int dimH, int dimL);
template void matrixProduct(const mat & inMat1, const mat & inMat2, double **& outMat, int dimH, int dimL);
template void matrixProduct(const mat & inMat1, const mat & inMat2, mat & outMat, int dimH, int dimL);


template void matrixProduct(const double ** const & inMat1, const double ** const & inMat2, mat & outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const double ** const & inMat1, const double ** const & inMat2, double **& outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const double ** const & inMat1, const mat & inMat2, double **& outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const mat & inMat1, const mat & inMat2, double **& outMat, int numRows1, int numCols2, int numCols1Rows2);
template void matrixProduct(const mat & inMat1, const mat & inMat2, mat & outMat, int numRows1, int numCols2, int numCols1Rows2);
