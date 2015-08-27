
#ifndef ICA_CPP
#define ICA_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"



//products for ICA
//void product1(double ** const arr, int length, int ns, double * vec, double ** outVector)
void product1(const matrix & arr, int length, int ns, const vec & vect, vec & outVector)
{
    //<X*g(Wt*X)>
    //vec = Wt
    //X[j] = arr[][j] dimension = ns
    //average over j

    for(int i = 0; i < ns; ++i)
    {
        outVector[i] = 0.;
    }

    double sum = 0.;


    for(int j = 0; j < length; ++j)
    {
        sum = 0.;

        for(int i = 0; i < ns; ++i)
        {
            sum += vect[i] * arr[i][j];
        }
        for(int i = 0; i < ns; ++i)
        {
            outVector[i] += tanh(sum) * arr[i][j];
        }
    }
    for(int i = 0; i < ns; ++i)
    {
        outVector[i] /= length;
    }
}



//void product2(double ** const arr, int length, int ns, double * vec, double ** outVector)
void product2(const matrix & arr, int length, int ns, const vec & vect, vec & outVector)
{
    //g'(Wt*X)*1*W
    //vec = Wt
    //X = arr[][j]
    //average over j

    double sum = 0.;
    double sum1 = 0.;


    for(int j = 0; j < length; ++j)
    {
        sum = 0.;
        for(int i = 0; i < ns; ++i)
        {
            sum += vect[i] * arr[i][j];
        }
        sum1 += 1 - tanh(sum)*tanh(sum);
    }
    sum1 /= length;

    for(int i = 0; i < ns; ++i)
    {
        outVector[i] = sum1 * vect[i];
    }

}



//void product3(double ** vec, int ns, int currNum, double ** outVector)
void product3(const matrix & inMat, int ns, int currNum, vec & outVector)
{
    //sum(Wt*Wi*Wi)

    for(int k = 0; k < ns; ++k)
    {
        outVector[k] = 0.;
    }
    double sum = 0.;

    for(int j = 0; j < currNum; ++j)
    {
        sum = 0.;
        for(int k = 0; k < ns; ++k)
        {
            sum += inMat[currNum][k] * inMat[j][k];
        }
        for(int k = 0; k < ns; ++k)
        {
            outVector[k] += inMat[j][k] * sum;
        }

    }
}

vec randomVector(int ns)
{

    vec tempVector2(ns);
    srand(time(NULL));
    double sum = 0.;
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] = rand()%30 - 15;

        sum += pow(tempVector2[i], 2);
    }
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] /= sqrt(sum);
    }
    return tempVector2;
}


void MainWindow::ICA() //fastICA
{
    //we have data[ns][ndr*nr], ns, ndr, nr
    //at first - whiten signals using eigen linear superposition to get E as covMatrix
    //then count matrixW

    //data = A * comps, comps = W * data

    //count components = matrixW*data and write to def::ExpName_ICA.edf
    //count inverse matrixA = matrixW^-1 and draw maps of components
    //write automatization for classification different sets of components, find best set, explain

    QTime wholeTime;
    wholeTime.start();
    QTime myTime;
    myTime.start();

    QStringList lst;
    QString helpString;

    helpString = def::dir->absolutePath()
            + slash() + def::ExpName + ".edf";
    cout << "Ica started: " << helpString << endl;
    readData();
    const int & dataLength = globalEdf.getDataLen();
    //check reduceChannelsLineEdit for write edf

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);

    if(!QString(label[lst.last().toInt() - 1]).contains("Markers"))
    {
        cout << "ICA: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }

    //ns = 19
    int ns = ui->numOfIcSpinBox->value(); //generality. Bind to reduceChannelsLineEdit?


    double eigenValuesTreshold = pow(10., - ui->svdDoubleSpinBox->value());
    double vectorWTreshold = pow(10., - ui->vectwDoubleSpinBox->value());

    const int & fr = def::freq;


//    dataICA - matrix of data
//    random quantity is a amplitude-vector of ns dimension
//    there are ndr*nr samples of this random quantity
//        covMatrix - matrix of covariations of different components of the random quantity. Its size is ns*ns
//        sampleCovMatrix - matrix of sample covariations - UNUSED
//        similarMatrix - matrix of similarity
//        differenceMatrix - matrix of difference, according to some metric

//    cout << "ns = " << ns << endl;
//    cout << "ndr*fr = " << ndr*fr << endl;

    matrix covMatrix(ns, ns, 0.);
//    double ** covMatrix = new double * [ns];
//    for(int i = 0; i < ns; ++i)
//    {
//        covMatrix[i] = new double [ns];
//    }

    //vectors for the las stage
    matrix vectorW(ns, ns, 0.);
//    double ** vectorW = new double * [ns];
//    for(int i = 0; i < ns; ++i)
//    {
//        vectorW[i] = new double [ns];
//    }
    vec vector1(ns, 0.);
    vec vector2(ns, 0.);
    vec vector3(ns, 0.);
    vec vectorOld(ns, 0.);
//    double * vector1 = new double [ns];
//    double * vector2 = new double [ns];
//    double * vector3 = new double [ns];
//    double * vectorOld = new double [ns];

    //for full A-matrix count
    matrix matrixA(ns, ns);
    vec tempVector(ns);

    //components time-flow
    matrix components(ns + 1, globalEdf.getDataLen()); // needed readData();
//    components.resize(ns+1);
//    for(int i = 0; i < ns + 1; ++i)
//    {
//        components[i].resize(dataLength);
//    }

    // save markers
    components[ns] = globalEdf.getData()[globalEdf.getMarkChan()];

    //count covariations
    //count averages

//    double * averages = new double [ns];
    vec averages(ns);
    for(int i = 0; i < ns; ++i)
    {
        averages[i] = mean(globalEdf.getData()[i].data(), dataLength);
    }

    //count zeros
    int h = 0;
    int Eyes = 0;
    for(int i = 0; i < dataLength; ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(fabs(globalEdf.getData()[j][i]) == 0.) ++h;
        }
        if(h == ns) Eyes += 1;
    }

    const double & realSignalFrac = (double(dataLength - Eyes) / dataLength);

    double helpDouble = 0.;
    //subtract averages
    /*
    std::for_each(globalEdf.getData().begin(),
                  globalEdf.getData().end(),
                  [realSignalFrac, averages](vec & dataVec)
    {
        std::for_each(dataVec.begin(),
                      dataVec.end(),
                      [realSignalFrac, averages](double & in)
        {

        });
    });
    */

    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < dataLength; ++j)
        {
            if(globalEdf.getData()[i][j] == 0.) continue;
            helpDouble = globalEdf.getData()[i][j] - averages[i] / realSignalFrac;
            globalEdf.setData(i, j, helpDouble);
        }
    }

    //covariation between different channels
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            covMatrix[i][j] = 0.;
            covMatrix[i][j] = covariance(globalEdf.getData()[i].data(),
                                         globalEdf.getData()[j].data(),
                                         dataLength);
            covMatrix[i][j] /= dataLength; //should be -1 ? needed for trace
        }
    }

    //test covMatrix symmetric
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            if(covMatrix[i][j] != covMatrix[j][i]) cout << i << " " << j << " warning covariances !=" << endl;
        }
    }

    double sum1, sum2; //temporary help values
    int counter = 0;
    matrix dataICA(ns, dataLength, 0);
//    double ** dataICA = new double * [ns];
//    for(int i = 0; i < ns; ++i)
//    {
//        dataICA[i] = new double [dataLength];
//    }


    // count eigenvalue decomposition
    matrix eigenVectors;
    vector <double> eigenValues;

    svd(globalEdf.getData(),
        eigenVectors,
        eigenValues,
        eigenValuesTreshold);


    helpString = def::dir->absolutePath()
                 + slash() + "Help"
                 + slash() + def::ExpName + "_eigenMatrix.txt";
    writeSpectraFile(helpString,
                     eigenVectors,
                     ns, ns);



    helpString = def::dir->absolutePath()
                 + slash() + "Help"
                 + slash() + def::ExpName + "_eigenValues.txt";
    writeFileInLine(helpString, eigenValues);



    cout << "time svd = " << wholeTime.elapsed()/1000. << " sec" << endl;

    //count linear decomposition on PCAs
    for(int j = 0; j < dataLength; ++j) //columns initData
    {
        for(int i = 0; i < ns; ++i) //rows tempMatrix
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k) //rows initData = coloumns tempMatrix
            {
                sum1 += eigenVectors[k][i] * globalEdf.getData()[k][j] / sqrt(eigenValues[i]);
            }
            components[i][j] = sum1;
        }
    }

    for(int j = 0; j < dataLength; ++j) //columns X
    {
        for(int i = 0; i < ns; ++i) //rows tempMatrix
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += eigenVectors[i][k] * components[k][j];
            }
            dataICA[i][j] = sum1;
        }
    }

    //now dataICA are uncovariated signals with variance 1



    //test of covMatrix dataICA
//    cout << "covMatrixICA = " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            covMatrix[i][j] = 0.;
//            for(int k = 0; k < dataLength; ++k)
//            {
//                covMatrix[i][j] += dataICA[i][k] * dataICA[j][k];
//            }
//            covMatrix[i][j] /= ( dataLength - 1 );
//            covMatrix[i][j] = int(covMatrix[i][j]*100)/100.;
//            cout << covMatrix[i][j] << " ";
//        }
//        cout<<endl;
//    }
//    cout<<"covMatrixICA counted"<<endl<<endl;

    // ICA itself!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // fastIca wiki - first function
    for(int i = 0; i < ns; ++i) //number of current vectorW
    {
        myTime.restart();
        counter = 0;


        vectorW[i] = randomVector(ns);

        while(1)
        {
            for(int j = 0; j < ns; ++j)
            {
                vectorOld[j] = vectorW[i][j];
            }
            product1(dataICA, dataLength, ns, vectorW[i], vector1);
            product2(dataICA, dataLength, ns, vectorW[i], vector2);
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] = vector1[j] - vector2[j];
            }

            //orthogonalization
            product3(vectorW, ns, i, vector3);
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] -= vector3[j];
            }

            //check norma
            for(int k = 0; k < i; ++k)
            {
                sum1 = 0.;
                for(int h = 0; h < ns; ++h)
                {
                    sum1 += pow(vectorW[k][h], 2);
                }
                if(fabs(sum1 - 1.) > 0.01)
                {
                    cout << i << "'th vector not 1-l" << endl;
                }
            }

            //check ortho
            for(int k = 0; k < i; ++k)
            {
                sum1 = 0.;
                for(int h = 0; h < ns; ++h)
                {
                    sum1 += vectorW[k][h] * vectorW[i][h];
                }
                if(sum1 > 0.01)
                {
                    cout << i << "'th and " << k << "'th  vectors not ortho" << endl;
                }

            }

            sum2 = 0.;
            sum1 = 0.;

            //normalization
            for(int j = 0; j < ns; ++j)
            {
                sum1 += pow(vectorW[i][j], 2);
            }
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] /= sqrt(sum1);
            }


            //check norma difference
            for(int j = 0; j < ns; ++j)
            {
                sum2 += pow(vectorOld[j] - vectorW[i][j], 2);
            }
            sum2 = sqrt(sum2);
            ++counter;
            if(sum2 < vectorWTreshold || 2 - sum2 < vectorWTreshold) break;
            if(counter == 100) break;
        }
        cout << "NumOf vectorW component = " << i << "\t";
        cout << "iterations = " << counter << "\t";
        cout << "error = " << fabs(sum2 - int(sum2 + 0.5)) << "\t";
        cout << "time = " << myTime.elapsed()/1000. << " sec" << endl;

    }
//    cout << "VectorsW counted" << endl;



//    //test vectorsW - ok
//    cout << "Mixing matrix = " << endl; //A, W^-1
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            cout << vectorW[j][i] << "\t";
//        }
//        cout << endl;
//    }

    //test orthoNorm VectorsW  -OK
//    cout << "test W*W^t = " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            sum1 = 0.;
//            for(int k = 0; k < ns; ++k)
//            {
//                sum1 += vectorW[i][k] * vectorW[j][k];
//            }
//            sum1 = int(sum1*100)/100.;
//            cout << sum1 << " ";
//        }
//        cout << endl;
//    }
//    cout<<endl;

    //count components
    matrixProduct(vectorW,
                  dataICA,
                  components,
                  ns);

//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < dataLength; ++j)
//        {
//            sum1 = 0.;
//            for(int k = 0; k < ns; ++k)
//            {
//                sum1 += vectorW[i][k] * dataICA[k][j]; //initial
//            }
//            components[i][j] = sum1;
//        }
//    }


    //count full mixing matrix A = E * D^0.5 * Et * Wt
    //X = AS (sensor data = A*components)
#define MATRICES 0

#if MATRICES
    matrixA = matrix::transpose(vectorW); // A = Wt
#else
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            matrixA[i][j] = eigenVectors[j][i];
        }
    }
#endif



///////////////////////////////////////////////////////////////////////////////////////////////
#if MATRICES
    matrixA = matrix::transpose(eigenVectors) * matrixA; //A = Et * Wt
#else
    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            tempVector[k] = 0.; //new i-th coloumn for matrix A
            for(int s = 0; s < ns; ++s)
            {
                tempVector[k] += eigenVectors[s][k] * matrixA[s][i]; //A = Et * Wt
            }
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] = tempVector[k];
        }
    }
#endif

#if MATRICES
    //A = D^0.5 * Et * Wt
    matrix D_05(ns, ns, 0.);
    for(int i = 0; i < ns; ++i)
    {
        D_05[i][i] = sqrt(eigenValues[i]);
    }
    matrixA = D_05 * matrixA;
#else
    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(eigenValues[i]);//A = D^0.5 * Et * Wt
        }
    }
#endif

#if MATRICES
    matrixA = eigenVectors * matrixA;
#else

    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            tempVector[k] = 0.; //new i-th coloumn for matrix A
            for(int s = 0; s < ns; ++s)
            {
                tempVector[k] += eigenVectors[k][s] * matrixA[s][i]; //A = E * D^0.5 * Et * Wt
            }
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] = tempVector[k];
        }
    }
#endif



#if 1
    //test  data = matrixA * comps;

    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < dataLength; ++j)
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += matrixA[i][k] * components[k][j];
            }
            if(fabs((globalEdf.getData()[i][j] - sum1) / globalEdf.getData()[i][j]) > 0.05
                    && fabs(globalEdf.getData()[i][j]) > 0.5)
            {
                cout << "before" << "\t";
                cout << i << "\t" << j << "\t";
                cout << "err = " <<fabs((globalEdf.getData()[i][j] - sum1)/globalEdf.getData()[i][j]) << "\t";
                cout << "init value = " << globalEdf.getData()[i][j] << endl;
            }
        }
    }
#endif



#if 0
    // norm components - by equal dispersion ????????????????????
    // and then order by squared sum of maps coeffs

    double coeff = 1.5;
    for(int i = 0; i < ns; ++i)
    {
        sum1 = 0.;
        sum2 = 0.;

        sum2 = variance(components[i], dataLength);

        for(int j = 0; j < dataLength; ++j)
        {
            components[i][j] /= sqrt(sum2);
            components[i][j] *= coeff;
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(sum2);
            matrixA[i][k] /= coeff;
        }
    }

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName + "_maps_before_var.txt");
    writeICAMatrix(helpString, matrixA, ns); //generality 19-ns


    //ordering components by sum of squares of the matrixA coloumn
    std::vector <std::pair <double, int>> colsNorms;
    double sumSquares = 0.;
    std::vector <double> explainedVariance;

    for(int i = 0; i < ns; ++i) // w/o markers
    {
        helpDouble = 0.;
        for(int j = 0; j < ns; ++j)
        {
            helpDouble += pow(matrixA[j][i], 2.);
        }
        sumSquares += helpDouble;
        colsNorms.push_back(std::make_pair(helpDouble, i));
    }
    std::sort(colsNorms.begin(),
              colsNorms.end(),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {return i.first > j.first;});

    for(int i = 0; i < ns; ++i)
    {
        cout << colsNorms[i].first << "\t" << colsNorms[i].second << endl;
    }

    double * tempCol = new double [ns];
    int tempIndex;
    std::vector <double> tempComp;
    for(int i = 0; i < ns - 1; ++i) // dont move the last
    {
        //swap cols and components

        for(int j = 0; j < ns; ++j) // swap j'th elements in i'th and colsNorms[i].second'th cols
        {
            tempCol[j] = matrixA[j][i];
            matrixA[j][i] = matrixA[j][ colsNorms[i].second ];
            matrixA[j][ colsNorms[i].second ] = tempCol[j];
        }

        tempComp = components[i];
        components[i] = components[ colsNorms[i].second ];
        components[ colsNorms[i].second ] = tempComp;

        // swap i and colsNorms[i].second values in colsNorms
        auto it1 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [i](std::pair <double, int> in)
        {return in.second == i;});
        auto it2 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [colsNorms, i](std::pair <double, int> in)
        {return in.second == colsNorms[i].second;});
        tempIndex = (*it1).second;
        (*it1).second = (*it2).second;
        (*it2).second = tempIndex;
    }
    delete []tempCol;

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName + "_maps_after_var.txt");
    writeICAMatrix(helpString, matrixA, ns); //generality 19-ns

    for(int i = 0; i < ns; ++i)
    {
        explainedVariance.push_back(colsNorms[i].first / sumSquares * 100.);
        cout << "comp = " << i+1 << "\t";
        cout << "explVar = " << explainedVariance[i] << endl;
    }
    //end componets ordering
#else
    // norm components to 1-length of mapvector, order by dispersion
    for(int i = 0; i < ns; ++i) // for each component
    {
        sum1 = 0.;
        for(int k = 0; k < ns; ++k)
        {
            sum1 += pow(matrixA[k][i], 2);
        }
        sum1 = sqrt(sum1);

        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] /= sum1;
        }
        std::transform(components[i].begin(),
                       components[i].end(),
                       components[i].begin(),
                       [sum1](double in) {return in * sum1;});
    }

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName + "_maps_1-len.txt");
    writeICAMatrix(helpString, matrixA);


    // ordering components by dispersion
    std::vector <std::pair <double, int>> colsNorms; // dispersion, numberOfComponent
    double sumSquares = 0.; // sum of all dispersions
    vec explainedVariance;

    for(int i = 0; i < ns; ++i)
    {
        sum1 = 0.;
        std::for_each(components[i].begin(),
                      components[i].end(),
                      [&sum1](double in){sum1 += pow(in, 2);});
        sumSquares += sum1;
        colsNorms.push_back(std::make_pair(sum1, i));
    }

    std::sort(colsNorms.begin(),
              colsNorms.end(),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {return i.first > j.first;});

    for(int i = 0; i < ns; ++i)
    {
        cout << colsNorms[i].first << "\t" << colsNorms[i].second << endl;
    }

    int tempIndex;
    vec tempComp;
    for(int i = 0; i < ns - 1; ++i) // dont move the last
    {
        // swap matrixA cols
#if MATRICES
        matrixA.swapCols(i, colsNorms[i].second);
#else
        vec tempCol(ns);
        for(int j = 0; j < ns; ++j) // swap j'th elements in i'th and colsNorms[i].second'th cols
        {
            tempCol[j] = matrixA[j][i];
            matrixA[j][i] = matrixA[j][ colsNorms[i].second ];
            matrixA[j][ colsNorms[i].second ] = tempCol[j];
        }
#endif

        // swap components
#if MATRICES
        components.swapRows(i, colsNorms[i].second);
#else
        tempComp = components[i];
        components[i] = components[ colsNorms[i].second ];
        components[ colsNorms[i].second ] = tempComp;
#endif

        // swap i and colsNorms[i].second values in colsNorms
        auto it1 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [i](std::pair <double, int> in)
        {return in.second == i;});
        auto it2 = std::find_if(colsNorms.begin(),
                                colsNorms.end(),
                                [colsNorms, i](std::pair <double, int> in)
        {return in.second == colsNorms[i].second;});

        tempIndex = (*it1).second;
        (*it1).second = (*it2).second;
        (*it2).second = tempIndex;
    }

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName + "_maps_after_len.txt");
    writeICAMatrix(helpString, matrixA); //generality 19-ns

    for(int i = 0; i < ns; ++i)
    {
        explainedVariance.push_back(colsNorms[i].first / sumSquares * 100.);
        cout << "comp = " << i+1 << "\t";
        cout << "explVar = " << explainedVariance[i] << endl;
    }
    //end componets ordering
#endif

#if 1
    // test data = matrixA * components
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < dataLength; ++j)
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += matrixA[i][k] * components[k][j];
            }
            if(fabs((globalEdf.getData()[i][j] - sum1)/globalEdf.getData()[i][j]) > 0.05
                    && fabs(globalEdf.getData()[i][j]) > 0.5)
            {
                cout << "after norm\t" << i << "\t" << j << "\t";
                cout << "err = " << fabs((globalEdf.getData()[i][j] - sum1) / globalEdf.getData()[i][j]) << "\t";
                cout << "init value = " << globalEdf.getData()[i][j] << endl;
            }
        }
    }
#endif

    //now should draw amplitude maps OR write to file
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName + "_maps.txt");
    writeICAMatrix(helpString, matrixA); //generality 19-ns

    drawMapsICA(helpString,
                helpString.left(helpString.lastIndexOf(slash())), // same dir as mapsFile
                def::ExpName);


    helpString = def::dir->absolutePath()
            + slash() + def::ExpName + "_ica.edf";
    QList <int> chanList;
    makeChanList(chanList);
    cout << chanList << endl;

    globalEdf.writeOtherData(components, helpString, chanList);

    cout << "ICA ended. time = " << wholeTime.elapsed()/1000. << " sec" << endl;

    def::ns = ns + 1; // numOfICs + markers
}



void MainWindow::ICsSequence(QString EDFica1, QString EDFica2, QString maps1Path, QString maps2Path, int mode)
{
#if 0
    /////////////////////// REWORK read spectra in line
    //mode == 0 -> sequency by most stability
    //mode == 1 -> sequency by first File & no overwrite
    if(mode != 0 && mode != 1)
    {
        cout << "bad mode" << endl;
        return;
    }

    QTime myTime;
    myTime.start();
    //count cross-correlation by maps and spectra
    int ns_ = 19;

    double corrMap;
    double corrSpectr[3];
    int offset5hz;
    int offset20hz;
    QString helpString2;
    QString helpString;

    double ** dataFFT1;
    matrixCreate(&dataFFT1, 3, 247*19);
    double ** dataFFT2;
    matrixCreate(&dataFFT2, 3, 247*19);


    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");
    ui->reduceChannelsCheckBox->setChecked(false);
    ui->sliceWithMarkersCheckBox->setChecked(false);

    Spectre * sp;

    setEdfFile(EDFica1);
    cleanDirs();
    readData();
    sliceAll();
    sp = new Spectre(dir, ns_, def::ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
    for(int i = 0; i < 3; ++i)
    {
        helpString = def::dir->absolutePath() + slash() + "Help" + slash() + def::ExpName;
        switch(i)
        {
        case 0: {helpString += "_241.psa"; break;}
        case 1: {helpString += "_247.psa"; break;}
        case 2: {helpString += "_254.psa"; break;}
        }
        readSpectraFileLine(helpString, dataFFT1, 19, 247);
    }



    setEdfFile(EDFica2);
    cleanDirs();
    readData();
    sliceAll();
    sp = new Spectre(dir, ns_, def::ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
    for(int i = 0; i < 3; ++i)
    {
        helpString = def::dir->absolutePath() + slash() + "Help" + slash() + def::ExpName;
        switch(i)
        {
        case 0: {helpString += "_241.psa"; break;}
        case 1: {helpString += "_247.psa"; break;}
        case 2: {helpString += "_254.psa"; break;}
        }
        readSpectraFileLine(helpString, dataFFT2, 19, 247);
    }


    //sequence ICs
    double ** mat1;
    double ** mat2;
    matrixCreate(&mat1, ns_, ns_);
    matrixCreate(&mat2, ns_, ns_);

    //read matrices
    readICAMatrix(maps1Path, mat1, ns_);
    readICAMatrix(maps2Path, mat2, ns_);

    //transpose ICA maps
    matrixTranspose(mat1, ns_);
    matrixTranspose(mat2, ns_);

    QList<int> list1;
    QList<int> list2;
    list1.clear();
    list2.clear();

    int fftLength = 4096;
    offset5hz = 5./ (def::freq/fftLength) - 1;
    offset20hz = 20./ (def::freq/fftLength) + 1;

    struct ICAcoeff
    {
        double cMap;
        double cSpectr[3];
        double sumCoef;
    };
    ICAcoeff coeffs[ns_][ns_];

    struct ICAcorr
    {
        int num1;
        int num2;
        ICAcoeff coeff;
    };
    ICAcorr ICAcorrArr[ns_];

    double ** corrs;
    matrixCreate(&corrs, ns_, ns_);

    double tempDouble;
    int maxShift = 2; ////////////////////////////////////////////////////////////

    cout << "1" << endl;
    double helpDouble;

    helpString.clear();
    for(int k = 0; k < ns_; ++k)
    {
        for(int j = 0; j < ns_; ++j)
        {
            corrMap = (correlation(mat1[k], mat2[j], ns_, 0, true));
            corrMap = corrMap * corrMap;

            helpDouble = corrMap; /////////////////////////////////////////////////////////////////////////////////////
            coeffs[k][j].cMap = corrMap;

            for(int h = 0; h < 3; ++h)
            {
                corrSpectr[h] = 0.;
                for(int shift = -maxShift; shift <= maxShift; ++shift)
                {
                    corrSpectr[h] = fmax( fabs(correlation(dataFFT1[h] + k*247, dataFFT2[h] + j*247, 247, shift)), corrSpectr[h]);
                }
                corrSpectr[h] = corrSpectr[h] * corrSpectr[h];
                helpDouble += corrSpectr[h]; /////////////////////////////////////////////////////////////////////////////////////

                coeffs[k][j].cSpectr[h] = corrSpectr[h];
            }
            helpDouble = sqrt(helpDouble);
            corrs[k][j] = helpDouble;
            coeffs[k][j].sumCoef = helpDouble;
        }
    }

    //find best correlations
    int temp1;
    int temp2;
    for(int j = 0; j < ns_; ++j) //j pairs
    {
        tempDouble = 0.;
        for(int i = 0; i < ns_; ++i) //rows
        {
            if(list1.contains(i)) continue;
            for(int k = 0; k < ns_; ++k) //cols
            {
                if(list2.contains(k)) continue;
//                if(i == k) continue;  ////////////////////////////////////////////////////////////////////////////////////////////////////////
                if(corrs[i][k] > tempDouble)
                {
                    tempDouble = corrs[i][k];
                    temp1 = i;
                    temp2 = k;
                }
            }
        }



        if(j == ns_ - 1 && EDFica1 == EDFica2) ////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            for(int i = 0; i < ns_; ++i) //rows
            {
                if(list1.contains(i)) continue;
                list1 << i;
                list2 << i;

                ICAcorrArr[j].num1 = i;
                ICAcorrArr[j].num2 = i;


            }
            ICAcorrArr[j].coeff.cMap = 1;
            for(int h = 0; h < 3; ++h)
            {
                ICAcorrArr[j].coeff.cSpectr[h] = 1;
            }
            ICAcorrArr[j].coeff.sumCoef = 1;
            break;

        }




        list1 << temp1;
        list2 << temp2;
        ICAcorrArr[j].num1 = temp1;
        ICAcorrArr[j].num2 = temp2;
        ICAcorrArr[j].coeff.cMap = coeffs[temp1][temp2].cMap;
        for(int h = 0; h < 3; ++h)
        {
            ICAcorrArr[j].coeff.cSpectr[h] = coeffs[temp1][temp2].cSpectr[h];
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        tempDouble = ICAcorrArr[j].coeff.cMap;
        for(int h = 0; h < 3; ++h)
        {
            tempDouble += ICAcorrArr[j].coeff.cSpectr[h];
        }
        tempDouble = sqrt(tempDouble);


        ICAcorrArr[j].coeff.sumCoef = tempDouble;
    }







    double ** newMaps;
    matrixCreate(&newMaps, ns_, ns_);

    ui->sliceWithMarkersCheckBox->setChecked(true);
    ui->reduceChannelsCheckBox->setChecked(true);

    //sequence
    //1st file
    if(mode == 0)
    {
        helpString.clear();
        for(int k = 0; k < ns_; ++k)
        {
            helpString += QString::number( ICAcorrArr[k].num1 + 1) + " "; ///////////////////////////////by most stability
//            helpString += QString::number( k + 1 ) + " "; /////////////////////////////by first file
            for(int j = 0; j < ns_; ++j)
            {
                newMaps[k][j] = mat1[ICAcorrArr[k].num1][j]; /////////////////////////////////////////by most stability
//                newMaps[k][j] = mat1[k][j]; ////////////////////////////////////////////////////by first file
            }
            //        memcpy(newMaps[k], mat1[ICAcorrArr[k].num1], ns_*sizeof(double));
        }
        matrixTranspose(newMaps, ns_);
        helpString2 = maps1Path;
        helpString2.replace("_maps.txt", "_newSeq_maps.txt");
        writeICAMatrix(helpString2, newMaps, ns_);

        helpString += "20";
        ui->reduceChannelsLineEdit->setText(helpString);
        cout << helpString.toStdString() << endl;

        setEdfFile(EDFica1);
        cleanDirs();
        sliceAll();
        helpString2 = EDFica1;
        helpString2.replace(".edf", "_newSeq.edf");
        constructEDF(helpString2);
        //    sp = new Spectre(dir, ns_, def::ExpName);
        //    sp->countSpectra();
        //    sp->compare();
        //    sp->compare();
        //    sp->compare();
        //    sp->psaSlot();
        //    sp->close();
        //    delete sp;
    }

    //second file
    helpString.clear();
    for(int k = 0; k < ns_; ++k)
    {
        if(mode == 0)
        {
            helpString += QString::number( ICAcorrArr[k].num2 + 1) + " "; ///////////////////////////////////////////////////////////by most stability
        }
        else if(mode == 1)
        {
            helpString += QString::number( list2[ list1.indexOf(k) ] + 1) + " "; /////////////////////////////by first file
        }

        for(int j = 0; j < ns_; ++j)
        {        if(mode == 0)
            {
                newMaps[k][j] = mat2[ICAcorrArr[k].num2][j]; ///////////////////////////////////////////////////////////by most stability
            }
            else if(mode == 1)
            {
                newMaps[k][j] = mat2[ list2[list1.indexOf(k)] ][j]; /////////////////////////////by first file
            }
        }
//        memcpy(newMaps[k], mat1[ICAcorrArr[k].num2], ns_*sizeof(double));
    }
    matrixTranspose(newMaps, ns_);
    helpString2 = maps2Path;
    helpString2.replace("_maps.txt", "_newSeq_maps.txt");
    writeICAMatrix(helpString2, newMaps, ns_);

    helpString += "20";
    ui->reduceChannelsLineEdit->setText(helpString);
    cout << helpString.toStdString() << endl;
    setEdfFile(EDFica2);
    cleanDirs();
    sliceAll();
    helpString2 = EDFica2;
    helpString2.replace(".edf", "_newSeq.edf");
    constructEDF(helpString2);





    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);
    ui->reduceChannelsComboBox->setCurrentText("20");
    helpString2 = EDFica1;
    if(mode == 0)
    {
        helpString2.replace(".edf", "_newSeq.edf");
    }
    setEdfFile(helpString2);
    cleanDirs();
    sliceAll();
    sp = new Spectre(dir, ns_, def::ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;

    helpString2 = EDFica2;
    helpString2.replace(".edf", "_newSeq.edf");
    setEdfFile(helpString2);
    cleanDirs();
    sliceAll();
    sp = new Spectre(dir, ns_, def::ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;



    cout << endl;
    ofstream outStream;
    outStream.open("/media/Files/Data/AB/12", ios_base::out|ios_base::app);
    outStream << def::ExpName.left(3).toStdString() << endl;
    for(int k = 0; k < ns_; ++k)
    {
        cout << k+1 << "\t";
        cout << ICAcorrArr[k].num1 + 1 << "\t";
        cout << ICAcorrArr[k].num2 + 1 <<  "\t";
        cout << doubleRound(sqrt(ICAcorrArr[k].coeff.cMap), 3) <<  "\t";

        for(int h = 0; h < 3; ++h)
        {
            cout << doubleRound(sqrt(ICAcorrArr[k].coeff.cSpectr[h]), 3) <<  "\t";
        }
        cout << doubleRound(ICAcorrArr[k].coeff.sumCoef, 3) << endl;


        outStream << k+1 << "\t";
        outStream << ICAcorrArr[k].num1 + 1 << "\t";
        outStream << ICAcorrArr[k].num2 + 1 <<  "\t";
        outStream << doubleRound(sqrt(ICAcorrArr[k].coeff.cMap), 3) <<  "\t";

        for(int h = 0; h < 3; ++h)
        {
            outStream << doubleRound(sqrt(ICAcorrArr[k].coeff.cSpectr[h]), 3) <<  "\t";
        }
        outStream << doubleRound(ICAcorrArr[k].coeff.sumCoef, 3) << endl;
    }


    outStream.close();

    //leave only 7 high-score components


    matrixDelete(&mat1, ns_);
    matrixDelete(&mat2, ns_);
    matrixDelete(&newMaps, ns_);
    matrixDelete(&corrs, ns_);

    matrixDelete(&dataFFT1, 3);
    matrixDelete(&dataFFT2, 3);

    cout << "ICsSequence ended. time = = " << myTime.elapsed()/1000. << " sec" << endl;
#endif
}


void MainWindow::icaClassTest() /// CAREFUL sliceOneByOneNew() numOfIC
{
#if 0
    QString helpString2;
    QString helpString;

    int tempIndex = -1;
    //load ica file
    readData();

    int fr = def::freq; // generality


    double ** dataICA = new double * [ns]; //with markers
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        memcpy(dataICA[i],
               globalEdf.getData()[i].data(),
               globalEdf.getDataLen() * sizeof(double));
    }

    int numOfIC = ui->numOfIcSpinBox->value();

    double ** matrixA = new double * [numOfIC];
    for(int i = 0; i < numOfIC; ++i)
    {
        matrixA[i] = new double [numOfIC];
    }

    helpString2 = def::ExpName;
    helpString2.replace("_ica", "");
    helpString = QDir::toNativeSeparators(def::dir->absolutePath() + slash() + "Help" + slash() + helpString2 + "_maps.txt");
    if(!readICAMatrix(helpString, matrixA, numOfIC))
    {
        return;
    }

    Spectre * spectr;// = Spectre(dir, numOfIC, def::ExpName);
    helpString = def::dir->absolutePath() + slash() + "SpectraSmooth";
    Net * ANN = new Net(dir, numOfIC, left, right, spStep, def::ExpName);
    helpString = def::dir->absolutePath() + slash() + "16sec19ch.net";
    ANN->readCfgByName(helpString);
    ANN->setReduceCoeff(10.);
    double tempAccuracy;
    double currentAccuracy;
    double initAccuracy;
    this->ui->cleanRealisationsCheckBox->setChecked(true);
    QList<int> thrownComp;
    thrownComp.clear();
    QList<int> stayComp;
    stayComp.clear();
    double helpDouble;


    for(int i = 0; i < thrownComp.length(); ++i)
    {
        cout << thrownComp[i] << endl;
    }



    //remake via reduce channels
    for(int j = 0; j < fr*ndr; ++j)
    {
        for(int i = 0; i < numOfIC; ++i)
        {
            helpDouble = 0.;
            for(int k = 0; k < numOfIC; ++k)
            {
                if(!thrownComp.contains(k)) helpDouble += matrixA[i][k] * dataICA[k][j];
            }
            globalEdf.setData(i, j, helpDouble);
        }
    }

    cleanDirs();
    sliceOneByOneNew();

    spectr = new Spectre(dir, numOfIC, def::ExpName);

    spectr->countSpectra();

    ANN->setNumOfPairs(30);
    ANN->autoClassificationSimple();
    tempAccuracy = ANN->getAverageAccuracy();
    cout << "initAccuracy = " << tempAccuracy << endl;

    tempIndex = -1;
    for(int j = numOfIC; j > 0; --j) //num of components left
    {
        tempIndex = -1;
        initAccuracy = tempAccuracy;
        for(int i = 0; i < numOfIC; ++i)
        {
            if(thrownComp.contains(i)) continue;
            if(stayComp.contains(i)) continue;

            thrownComp.push_back(i);

            for(int j = 0; j < fr*ndr; ++j)
            {
                for(int i = 0; i < numOfIC; ++i)
                {
                    helpDouble = 0.;
                    for(int k = 0; k < numOfIC; ++k)
                    {
                        if(!thrownComp.contains(k)) helpDouble += matrixA[i][k] * dataICA[k][j];
                    }
                    globalEdf.setData(i, j, helpDouble);
                }
            }

            cleanDirs();
            sliceOneByOneNew();

            spectr->defaultState();
            spectr->countSpectra();

            ANN->autoClassificationSimple();

            currentAccuracy = ANN->getAverageAccuracy();
            cout << "AverageAccuracy " << i << " = " << currentAccuracy << endl;
            thrownComp.removeLast();

            if(currentAccuracy > initAccuracy + 1.5)
            {
                tempIndex = i;
                tempAccuracy = currentAccuracy;
                break;
            }
            else if(currentAccuracy > initAccuracy + 0.3 && currentAccuracy > tempAccuracy)
            {
                tempIndex = i;
                cout << tempIndex << "'th component saved as temp" <<endl;
                tempAccuracy = currentAccuracy;
            }
            else if(currentAccuracy < initAccuracy - 0.5)
            {
                stayComp.push_back(i);
                cout << tempIndex << "'th component is necessary" <<endl;
            }

        }
        if(tempIndex != -1)
        {
            cout << tempIndex << "'th component thrown" <<endl;
            thrownComp.push_back(tempIndex);
        }
        else
        {
            cout << "optimal components set:" << "\n";
            for(int i = 0; i < numOfIC; ++i)
            {
                if(!thrownComp.contains(i)) cout << i << "  ";
            }
            cout << "\n";
            cout << endl;
            break;
        }
    }
    delete ANN;
    delete spectr;
#endif
}

/// TEST
void MainWindow::throwIC() /// CAREFUL sliceOneByOneNew()
{
    QStringList lst;
    QString helpString;

    //makes a signals-file from opened ICA file and appropriate matrixA file
    if(!ui->filePathLineEdit->text().contains("ica", Qt::CaseInsensitive))
    {
        cout << "bad ica file" << endl;
        return;
    }

    //load ica file
    readData();

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst.last().toInt() - 1]).contains("Markers"))
    {
        QMessageBox::critical(this, tr("Error"), tr("bad channels list"), QMessageBox::Ok);
        return;
    }


    lst = ui->throwICLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    cout << "throwIC: going to throw " << lst.length() << " components - ";
    for(int i = 0; i < lst.length(); ++i)
    {
        cout << lst[i].toInt() << " ";
    }
    cout << "count from 1" << endl;




    int fr = def::freq; // generality
    int ns = def::ns;


    double ** dataICA = new double * [ns]; //with markers
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        memcpy(dataICA[i],
               globalEdf.getData()[i].data(),
               globalEdf.getDataLen() * sizeof(double));
    }

    int numOfIC = ui->numOfIcSpinBox->value(); // = 19

    matrix matrixA(numOfIC, numOfIC);

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName + "_maps.txt");
    readICAMatrix(helpString, matrixA);

    QList<int> thrownComp;
    thrownComp.clear();

    double helpDouble;


    for(int i = 0; i < lst.length(); ++i)
    {
        thrownComp << (lst[i].toInt() - 1);
    }

    for(int j = 0; j < fr*ndr; ++j)
    {
        for(int i = 0; i < numOfIC; ++i)
        {
            helpDouble = 0.;
            for(int k = 0; k < numOfIC; ++k)
            {
                if(!thrownComp.contains(k)) helpDouble += matrixA[i][k] * dataICA[k][j]; //generality -1
            }
            globalEdf.setData(i, j, helpDouble);
        }
    }

    ui->cleanRealisationsCheckBox->setChecked(true);
    cleanDirs();
    sliceOneByOneNew();
    cout << "sliced" << endl;

    constructEDFSlot();

}


////////////////////////// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA need test
void MainWindow::transformEDF(QString inEdfPath, QString mapsPath, QString newEdfPath) // for 19 channels generality
{
    setEdfFile(inEdfPath);
    readData();

    matrix mat1(def::nsWOM(), def::nsWOM());
    readICAMatrix(mapsPath, mat1); // data = mat1 * comps
    mat1.invert(); // mat1 * data = comps

    matrix newData(def::nsWOM(), ndr * def::freq);

    matrixProduct(mat1, globalEdf.getData(), newData);

    newData.resizeRows(def::ns); // for markers
    newData[def::ns - 1] = globalEdf.getData()[def::ns - 1]; //copy markers

    globalEdf.writeOtherData(newData.data, newEdfPath);
}

void MainWindow::transformReals() //move to library
{
    // I dont care, I dont need this shit anymore
#if 0
    QStringList lst;
    QString helpString;

    helpString = QFileDialog::getOpenFileName(this, tr("choose maps file"), def::dir->absolutePath(), tr("*.txt"));
    if(helpString.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No file chosen"), QMessageBox::Ok);
        return;
    }


    double ** mat1;
    matrixCreate(&mat1, 19,19);
    double ** mat3;
    matrixCreate(&mat3, 19, 19);

    readICAMatrix(helpString, mat1, 19);
    matrixInvert(mat1, 19, &mat3);
    matrixDelete(&mat1, 19);

    double ** mat2;
    matrixCreate(&mat2, 19, 250*50);

    QString procDir = ui->drawDirBox->currentText();
    def::dir->cd(procDir);
    lst = def::dir->entryList(QDir::Files);
    def::dir->cdUp();

    qApp->processEvents();

//    ifstream inStream;
//    ofstream outStream;
    matrixCreate(&mat1, 19, 100500);
    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = def::dir->absolutePath() + slash() + procDir + slash() + lst[i];
        readPlainData(helpString, mat1, 19, NumOfSlices);

        matrixProduct(mat3, globalEdf.getData(), mat2, 19, NumOfSlices);

        helpString = def::dir->absolutePath() + slash() + procDir + slash() + lst[i];
        writePlainData(helpString, mat2, 19, NumOfSlices);

        ui->progressBar->setValue(i*100./lst.length());
        qApp->processEvents();
    }
    ui->progressBar->setValue(0);

    matrixDelete(&mat1, 19);
    matrixDelete(&mat2, 19);
    matrixDelete(&mat3, 19);
#endif

}

void MainWindow::randomDecomposition() // CAREFUL sliceOneByOneNew
{

// careful compNum

#if 0
    QStringList lst;
    QString helpString;

    QString initName = def::ExpName;
    readData(); //should have opened initial data-file

    cout << "data read" << endl;

    int h = 0;
    int Eyes = 0;
    for(int i = 0; i < ndr*def::freq; ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(fabs(globalEdf.getData()[j][i]) == 0.) ++h;
        }
        if(h == ns) Eyes += 1;
    }

    lst.clear();
    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst.last().toInt() - 1]).contains("Markers"))
    {
        QMessageBox::critical(this, tr("Doge"), tr("Bad Markers channel in rdc channel lineEdit"), QMessageBox::Ok);
        return;
    }


    int compNum = ui->numOfIcSpinBox->value();
//    double ** randMatrix = matrixCreate(compNum, compNum);
    matrix randMatrix(compNum, compNum);
    double ** matrixW = matrixCreate(compNum, compNum);
//    double ** newData = matrixCreate(ns, ndr*def::freq);
    matrix newData(ns, globalEdf.getDataLen());

    double sum1;
    double sum2;

    Spectre * spectr = new Spectre(dir, compNum, def::ExpName);
    Net * ANN = new Net(dir, compNum, left, right, spStep, def::ExpName); //generality parameters
    helpString = def::dir->absolutePath() + slash() + "16sec19ch.net";
    ANN->readCfgByName(helpString);

    ANN->setReduceCoeff(15.); //generality
    ANN->setNumOfPairs(50);   ////////////////////////////////////////////////////////////////////////////////////////////
    FILE * outFile;


    double ** eigenMatrix = matrixCreate(compNum, compNum);

    /*
    helpString = def::dir->absolutePath() + slash() + "Help" + slash() + def::ExpName + "_eigenMatrix.txt";
    inStream.open(helpString.toStdString().c_str());
    if(!inStream.good())
    {
        cout << "bad inStream eigenMatrix" << endl;
        return;
    }
    for(int i = 0; i < compNum; ++i)
    {
        for(int j = 0; j < compNum; ++j)
        {
            inStream >> eigenMatrix[i][j];
        }
    }
    inStream.close();
*/
    double ** eigenMatrixInv = matrixCreate(compNum, compNum);
    matrixTranspose(eigenMatrix, compNum, compNum, eigenMatrixInv);

    double * eigenValues = new double [compNum];

    /*
    helpString = def::dir->absolutePath() + slash() + "Help" + slash() + def::ExpName + "_eigenValues.txt";
    inStream.open(helpString.toStdString().c_str());
    if(!inStream.good())
    {
        cout << "bad inStream eigenValues" << endl;
        return;
    }
    for(int i = 0; i < compNum; ++i)
    {
        inStream >> eigenValues[i];
    }
    inStream.close();
*/

    QTime myTime;
    myTime.start();

    QList<int> chanList;
    makeChanList(chanList);

    for(int i = 0; i < 55; ++i) /////////////////////////////////////////////////////////////////////////////////
    {
        myTime.restart();
        helpString = def::dir->absolutePath() + slash() + initName;
        setEdfFile(helpString);
        readData(); //should have opened initial data-file


        //set random matrix
        srand(time(NULL));

        for(int i = 0; i < compNum; ++i) //i'th coloumn
        {
            sum1 = 0.;
            for(int j = 0; j < compNum; ++j)
            {
                randMatrix[j][i] = (-20 + rand()%41)/20.;
                sum1 += randMatrix[j][i] * randMatrix[j][i];
            }
            sum1 = sqrt(sum1);

            //norm raandMatrix by 1-length coloumns
            for(int j = 0; j < compNum; ++j)
            {
                randMatrix[j][i] /= sum1;
            }
        }



  /*
   * //make randon matrix in terms of matrixW after whitening the initial signals
        matrixProduct(randMatrix, eigenMatrix, &matrixW, compNum, compNum); //W = rand * Eig

        for(int i = 0; i < compNum; ++i)
        {
            sum1 = 0.;
            for(int j = 0; j < compNum; ++j)
            {
                matrixW[j][i] /= sqrt(eigenValues[i]);
            }
        }
        //W = rand * Eig * D^-0.5

        matrixProduct(matrixW, eigenMatrixInv, &randMatrix, compNum, compNum); //randMatrix = randW * Eig * d^-0.5 *Eig^t
*/
        //decompose
        matrixProduct(randMatrix, globalEdf.getData(), newData, compNum);
        newData[ns-1] = globalEdf.getData()[globalEdf.getMarkChan()];

        //norm components and rand matrix
        for(int i = 0; i < compNum; ++i)
        {
            sum1 = mean(newData[i], ndr*def::freq);
            for(int j = 0; j < ndr*def::freq; ++j)
            {
                if(newData[i][j] != 0.) newData[i][j] -= sum1 * (ndr*def::freq / (ndr*def::freq - Eyes));
            }

            sum2 = sqrt(variance(newData[i], ndr*def::freq));
            for(int j = 0; j < ndr*def::freq; ++j)
            {
                newData[i][j] /= sum2;
                newData[i][j] *= 10.;  //10 generality
            }

            for(int k = 0; k < compNum; ++k)
            {
                randMatrix[k][i] *= sum2;
                randMatrix[k][i] /= 10.;
            }

        }

        //write newData
        helpString = def::dir->absolutePath() + slash() + def::ExpName + "_randIca.edf";
        globalEdf.writeOtherData(newData.data, helpString, chanList);
//        writeEdf(ui->filePathLineEdit->text(), newData, helpString, ndr*def::freq);

        helpString = def::dir->absolutePath() + slash() + def::ExpName + "_randIca.edf";
        setEdfFile(helpString);
        readData(); //read newData

        ui->cleanRealisationsCheckBox->setChecked(true);
        ui->cleanRealsSpectraCheckBox->setChecked(true);
        cleanDirs();

        sliceOneByOneNew(); //sliceAll()
        spectr->defaultState();
        spectr->countSpectra();
        ANN->autoClassificationSimple();
        helpString = def::dir->absolutePath() + slash() + initName + "_randIca.txt";
        outFile = fopen(helpString.toStdString().c_str(), "a");
        fprintf(outFile, "%.2lf\n", ANN->getAverageAccuracy());
        fclose(outFile);
    }




    matrixDelete(&matrixW, compNum);
    matrixDelete(&eigenMatrix, compNum);
    matrixDelete(&eigenMatrixInv, compNum);

    delete []eigenValues;
    delete ANN;
    delete spectr;
#endif
}
//


double objFunc(double *W_, double ***Ce_, double **Cz_, double **Cav_, double ns_, double numOfEpoches_)
{
    //count new r2
    double sum1 = 0.;
    double sum2 = 0.;
    double sum3 = 0.;

    for(int h = 0; h < numOfEpoches_; ++h)
    {
        sum1 = 0.;
        for(int j = 0; j < ns_; ++j)
        {
            for(int k = 0; k < ns_; ++k)
            {
                sum1 += ((Ce_[h][j][k] - Cav_[j][k]) * W_[j] * W_[k]);
            }
        }
        sum3 += pow(sum1, 2.);

    }
    sum3 /= numOfEpoches_;


    sum1 = 0.;
    sum2 = 0.;
    for(int j = 0; j < ns_; ++j)
    {
        for(int k = 0; k < ns_; ++k)
        {
            sum2 += (Cz_[j][k] * W_[j] * W_[k]);
        }
    }
    sum1 = pow(sum2, 2.);

    return sum1/sum3;
}


// holy fck, how long it is
void MainWindow::spoc()
{
    QString helpString;

    readData();
    nsBackup = def::ns;
    int ns = ui->numComponentsSpinBox->value(); //test

    double * W = new double  [ns];
    double * WOld = new double  [ns];
    int epochLength = ui->windowLengthSpinBox->value();
    int timeShift = ui->timeShiftSpinBox->value();
    int numOfEpoches = (ndr*def::freq - epochLength)/timeShift;

    double * Z = new double [numOfEpoches];

    double *** Ce = new double ** [numOfEpoches]; //covariance matrix
    for(int i = 0; i < numOfEpoches; ++i)
    {
        Ce[i] = new double * [ns];
        for(int j = 0; j < ns; ++j)
        {
            Ce[i][j] = new double [ns];
        }
    }

    double * averages = new double [ns];

    double ** Cav = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        Cav[i] = new double [ns];
    }

    double ** Cz = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        Cz[i] = new double [ns];
    }

    double * Znew = new double [numOfEpoches];

    helpString = QDir::toNativeSeparators(def::dir->absolutePath() + slash() + "spocVar.txt");
    FILE * in = fopen(helpString.toStdString().c_str(), "r");
    int helpInt = 0;
    while(!feof(in) && helpInt < numOfEpoches)
    {
        fscanf(in, "%lf", &Z[helpInt++]);
    }
    fclose(in);

    //zero mean
    double helpDouble = 0.;
    for(int i = 0; i < numOfEpoches; ++i)
    {
        helpDouble += Z[i];
    }
    helpDouble /= numOfEpoches;
    for(int i = 0; i < numOfEpoches; ++i)
    {
        Z[i] -= helpDouble;
    }

    //unit variance
    helpDouble = variance(Z, numOfEpoches);
    helpDouble = sqrt(helpDouble);
    for(int i = 0; i < numOfEpoches; ++i)
    {
        Z[i] /= helpDouble;
    }


    //count Ce
    for(int i = 0; i < numOfEpoches; ++i)
    {
        //count averages
        for(int j = 0; j < ns; ++j)
        {
//            averages[j] = 0.;
//            for(int h = 0; h < epochLength; ++h)
//            {
//                averages[j] += data[j][i * timeShift + h];
//            }
//            averages[j] /= epochLength;

            averages[j] = mean((globalEdf.getData()[j].data() + i*timeShift), epochLength);
        }

        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
//                helpDouble = 0.;
//                for(int h = 0; h < epochLength; ++h)
//                {
//                    helpDouble += (data[j][i * timeShift + h] - averages[j]) * (data[k][i * timeShift + h] - averages[k]);
//                }
//                helpDouble /= epochLength;
//                Ce[i][j][k] = helpDouble;

                Ce[i][j][k] = variance(globalEdf.getData()[j].data() + i*timeShift, epochLength);
            }
        }
    }

    //count Cav
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cav[j][k] = 0.;
        }
    }

    for(int i = 0; i < numOfEpoches; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                Cav[j][k] += Ce[i][j][k];
            }
        }
    }
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cav[j][k] /= numOfEpoches;
        }
    }

    //count Cz
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cz[j][k] = 0.;
        }
    }

    for(int i = 0; i < numOfEpoches; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                Cz[j][k] += Ce[i][j][k] * Z[i];
            }
        }
    }
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < ns; ++k)
        {
            Cz[j][k] /= numOfEpoches;
        }
    }

    double * gradientW = new double [ns];
    double sum1, sum2;
    double value1, value2;
    int counter1, counter2;
    double coeff = pow(10., -ui->spocCoeffDoubleSpinBox->value());
//    srand(time(NULL));


    if(ui->r2RadioButton->isChecked())
    {

        //W = randomVector
        for(int i = 0; i < ns; ++i)
        {
            W[i] = rand()%30 - 15.;
            sum1 += W[i] * W[i];
        }
        for(int i = 0; i < ns; ++i)
        {
            W[i] /= sqrt(sum1);
        }

        counter1 = 0;
        do
        {

            srand(time(NULL));
            sum1 = 1.;
            sum2 = 1.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 *= WOld[i];
                sum2 *= W[i];
            }

            for(int i = 0; i < ns; ++i)
            {
                WOld[i] = W[i];
            }

            if(sum1 == sum2)
            {
                srand(time(NULL));
                for(int i = 0; i < ns; ++i)
                {
                    W[i] = rand()%30 - 15.;
                    sum1 += W[i] * W[i];
                }
                for(int i = 0; i < ns; ++i)
                {
                    W[i] /= sqrt(sum1);
                }
            }

            //count gradient
            for(int i = 0; i < ns; ++i)
            {
                gradientW[i] = 0.;
            }


            for(int i = 0; i < ns; ++i)
            {
                //count <(wt*(Ce-Cav)w)^2>
                sum1 = 0.;

                for(int h = 0; h < numOfEpoches; ++h)
                {
                    sum2 = 0.;
                    for(int j = 0; j < ns; ++j)
                    {
                        for(int k = 0; k < ns; ++k)
                        {
                            sum2 += ((Ce[h][j][k] - Cav[j][k]) * W[j] * W[k]);
                        }
                    }
                    sum1 += pow(sum2, 2.);
                }
                sum1 /= numOfEpoches; //sum1 = <(wt*(Ce-Cav)w)^2>

                //count (wt*Cz*w)'
                sum2 = 0.;
                for(int j = 0; j < ns; ++j)
                {
                    sum2 += Cz[i][j] * W[j];
                }
                gradientW[i] = sum1 * sum2; //gradientW[i] = <(wt*(Ce-Cav)w)^2> * (wt*Cz*w)'

                //count <(wt*(Ce-Cav)*w) * (wt*(Ce-Cav)*w)'> * (wt * Cz * w)

                //1)
                //count <(wt*(Ce-Cav)*w) * (wt*(Ce-Cav)*w)'>
                helpDouble = 0.;
                for(int h = 0; h < numOfEpoches; ++h)
                {

                    sum1 = 0.;
                    sum2 = 0.;
                    for(int j = 0; j < ns; ++j)
                    {
                        for(int k = 0; k < ns; ++k)
                        {
                            sum1 += ((Ce[h][j][k] - Cav[j][k]) * W[j] * W[k]); //(wt*(Ce-Cav)*w)
                        }
                        sum2 += ((Ce[h][i][j] - Cav[i][j]) * W[j] ); //(wt*(Ce-Cav)*w)'
                    }
                    helpDouble += sum1 * sum2;

                }
                helpDouble /= numOfEpoches; // = <(wt*(Ce-Cav)*w) * (wt*(Ce-Cav)*w)'>

                //count (wt*Cz*w)
                sum1 = 0.;
                for(int j = 0; j < ns; ++j)
                {
                    for(int k = 0; k < ns; ++k)
                    {
                        sum1 += (Cz[j][k] * W[j] * W[k]);
                    }
                }
                gradientW[i] -= sum1 * helpDouble;

                sum2 = ((sum1 > 0.) - 0.5) * 2.; //sum2 = sign(sum1)
                gradientW[i] *= sum2;
            }



            //another count gradient
//            helpDouble = 1e-4;
//            for(int i = 0; i < ns; ++i)
//            {
//                W[i] += helpDouble;
//                gradientW[i] = objFunc(W, Ce,Cz,Cav,ns,numOfEpoches);
//                W[i] -= helpDouble;
//                gradientW[i] -= objFunc(W, Ce,Cz,Cav,ns,numOfEpoches);
//                gradientW[i] /= helpDouble;
//            }


            //1-norm gradient
            sum1 = 0.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 += gradientW[i] * gradientW[i];
            }
            sum1 = sqrt(sum1);
            for(int i = 0; i < ns; ++i)
            {
                gradientW[i] /= sum1;
            }

            //count objFunc before move
            value1 = objFunc(W, Ce, Cz, Cav, ns, numOfEpoches);

            value2 = value1;

            counter2 = 0;
            do
            {
                value1 = value2;
//                cout << value1 << " -> ";
                coeff = pow(10., -fmax(ui->spocCoeffDoubleSpinBox->value(), (value1 - 6.5)));

                for(int i = 0; i < ns; ++i)
                {
                    W[i] += gradientW[i] * coeff;
                }

                //count objFunc after move
                value2 = objFunc(W, Ce, Cz, Cav, ns, numOfEpoches);
//                cout << value2 << endl;

                ++counter2;


                qApp->processEvents();
                if(stopFlag)
                {

                    for(int i = 0; i < numOfEpoches; ++i)
                    {
                        for(int j = 0; j < ns; ++j)
                        {
                            delete []Ce[i][j];
                        }
                        delete []Ce[i];
                    }
                    delete []Ce;

                    for(int i = 0; i < ns; ++i)
                    {
                        delete []Cav[i];
                        delete []Cz[i];
                    }
                    delete []W;
                    delete []WOld;
                    delete []Cav;
                    delete []Cz;
                    delete []Z;
                    delete []Znew;
                    delete []averages;
                    delete []gradientW;
                    stopFlag = 0;
                    cout << "STOPPED" << endl;
                    return;
                }

            } while (value2 > value1 * 1.00);
            //step back
            for(int i = 0; i < ns; ++i)
            {
                W[i] -= gradientW[i] * coeff;
            }

            if(counter2 > 1)
            {
                cout << "value = " << value2 << "\t" << counter2 << " iterations" << endl;
            }

            sum1 = 0.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 += W[i] * W[i];
            }
            sum1 = sqrt(sum1);
            for(int i = 0; i < ns; ++i)
            {
                W[i] /= sum1 * ((W[2] > 0.)?1.:-1.); //generality
            }


            qApp->processEvents();
            if(stopFlag)
            {


                for(int i = 0; i < numOfEpoches; ++i)
                {
                    for(int j = 0; j < ns; ++j)
                    {
                        delete []Ce[i][j];
                    }
                    delete []Ce[i];
                }
                delete []Ce;

                for(int i = 0; i < ns; ++i)
                {
                    delete []Cav[i];
                    delete []Cz[i];
                }
                delete []W;
                delete []WOld;
                delete []Cav;
                delete []Cz;
                delete []Z;
                delete []Znew;
                delete []averages;
                delete []gradientW;
                stopFlag = 0;
                cout << "STOPPED" << endl;
                return;
            }


            sum1 = 0.;
            for(int i = 0; i < ns; ++i)
            {
                sum1 += (W[i] - WOld[i]) * (W[i] - WOld[i]);
            }
            sum1 = sqrt(sum1);
            ++counter1;
        } while(sum1 > pow(10., -1.5) || value2 < ui->spocTresholdDoubleSpinBox->value());
        cout << "final correlation = " << value2 << "\t" << counter1 << " iterations" << endl;


        //1-norm W
        sum1 = 0.;
        for(int i = 0; i < ns; ++i)
        {
            sum1 += W[i] * W[i];
        }
        sum1 = sqrt(sum1);
        for(int i = 0; i < ns; ++i)
        {
            W[i] /= sum1 * ((W[2] > 0.)?1.:-1.); //generality
        }


        for(int i = 0; i < ns; ++i)
        {
            cout << W[i] << "   ";
        }
        cout << endl;

        spocWVector = new double [ns];
        for(int i = 0; i < ns; ++i)
        {
            spocWVector[i] = W[i];
        }

        for(int i = 0; i < ns; ++i)
        {
            W[i] = 0.;
            for(int j = 0; j < ns; ++j)
            {
                W[i] += spocWVector[j] * spocMixMatrix[j][i];
            }
        }

        sum1 = 0.;
        sum2 = 0.;
        for(int i = 0; i < ns; ++i)
        {
            if(fabs(W[i]) > sum1)
            {
                sum1 = fabs(W[i]);
                sum2 = (W[i] > 0.)?1:-1;
            }
        }
        for(int i = 0; i < ns; ++i)
        {
            W[i] /= sum1*sum2;
        }


        cout << "wt*MixMatrix = " << endl;
        for(int i = 0; i < ns; ++i)
        {
            cout << W[i] << "   ";
        }
        cout << endl;



    }
    /*
    else if (ui->lambdaRadioButton->isChecked())
    {
        double ** detMatrix = new double * [ns];
        for(int i = 0; i < ns; ++i)
        {
            detMatrix[i] = new double [ns];
        }
        double * dets = new double [ndr*def::freq];

        //(Cz-l*Cav)*w = 0
        //matrixDet(Cz-l*Cav) = 0
        for(double L = 0.; L < ndr*def::freq; L+=1.)
        {
            for(int j = 0; j < ns; ++j)
            {
                for(int k = 0; k < ns; ++k)
                {
                    detMatrix[j][k] = Cz[j][k] - L*Cav[j][k];
                }
            }
            dets[int(L)] = matrixDet(detMatrix, ns);
            if(dets[int(L)] * dets[int(L-1)] < 0 || ( L > 2. && (dets[int(L)] > dets[int(L-1)]) && (dets[int(L-2)] > dets[int(L-1)]) && dets[int(L-1)] < 10.))
            {
                cout << "L = " << L << endl;
            }
        }

        for(int i = 0; i < ns; ++i)
        {
            delete []detMatrix[i];
        }
        delete []detMatrix;
        delete []dets;
    }
*/






    for(int i = 0; i < numOfEpoches; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            delete []Ce[i][j];
        }
        delete []Ce[i];
    }
    delete []Ce;

    for(int i = 0; i < ns; ++i)
    {
        delete []Cav[i];
        delete []Cz[i];
    }
    delete []W;
    delete []WOld;
    delete []Cav;
    delete []Cz;
    delete []Z;
    delete []Znew;
    delete []averages;
    delete []gradientW;

}



#endif
