
#ifndef ICA_CPP
#define ICA_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"



//products for ICA
void product1(double ** const arr, int length, int ns, double * vec, double ** outVector)
{
    //<X*g(Wt*X)>
    //vec = Wt
    //X[j] = arr[][j] dimension = ns
    //average over j

    for(int i = 0; i < ns; ++i)
    {
        (*outVector)[i] = 0.;
    }

    double sum = 0.;


    for(int j = 0; j < length; ++j)
    {
        sum = 0.;

        for(int i = 0; i < ns; ++i)
        {
            sum += vec[i] * arr[i][j];
        }
        for(int i = 0; i < ns; ++i)
        {
            (*outVector)[i] += tanh(sum) * arr[i][j];
        }
    }
    for(int i = 0; i < ns; ++i)
    {
        (*outVector)[i] /= length;
    }
}



void product2(double ** const arr, int length, int ns, double * vec, double ** outVector)
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
            sum += vec[i] * arr[i][j];
        }
        sum1 += 1 - tanh(sum)*tanh(sum);
    }
    sum1 /= length;

    for(int i = 0; i < ns; ++i)
    {
        (*outVector)[i] = sum1 * vec[i];
    }

}



void product3(double ** vec, int ns, int currNum, double ** outVector)
{
    //sum(Wt*Wi*Wi)

    for(int k = 0; k < ns; ++k)
    {
        (*outVector)[k] = 0.;
    }
    double sum = 0.;

    for(int j = 0; j < currNum; ++j)
    {
        sum = 0.;
        for(int k = 0; k < ns; ++k)
        {
            sum += vec[currNum][k]*vec[j][k];
        }
        for(int k = 0; k < ns; ++k)
        {
            (*outVector)[k] += vec[j][k] * sum;
        }

    }


}

double * randomVector(int ns)
{

    double * tempVector2 = new double [ns];
    srand(time(NULL));
    double sum = 0.;
    for(int i = 0; i < ns; ++i)
    {
        tempVector2[i] = rand()%30 - 15;
        sum += tempVector2[i] * tempVector2[i];
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

    //count components = matrixW*data and write to ExpName_ICA.edf
    //count inverse matrixA = matrixW^-1 and draw maps of components
    //write automatization for classification different sets of components, find best set, explain
    QTime wholeTime;
    wholeTime.start();
    QTime myTime;
    myTime.start();

    QStringList lst;
    QString helpString;

    helpString = dir->absolutePath() + slash() + ExpName + ".edf";
    cout << "Ica started: " << helpString.toStdString() << endl;

    readData();
    //check reduceChannelsLineEdit for write edf

    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
//    for(int i = 0; i < lst.length(); ++i)
//    {
//        cout << label[lst[i].toInt()-1] << endl;
//    }

    if(!QString(label[lst[lst.length() - 1].toInt() - 1]).contains("Markers"))
    {
//        QMessageBox::critical(this, tr("Error"), tr("bad channels list"), QMessageBox::Ok);
        cout << "ICA: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }

    ns = ui->numOfIcSpinBox->value(); //generality. Bind to reduceChannelsLineEdit?

    double eigenValuesTreshold = pow(10., -ui->svdDoubleSpinBox->value());
    double vectorWTreshold = pow(10., -ui->vectwDoubleSpinBox->value());

    const int & fr = def::freq;


//    dataICA - matrix of data
//    centeredMatrix - matrix of centered data: data[i][j] -= average[j]
//    random quantity is a amplitude-vector of ns dimension
//    there are ndr*nr samples of this random quantity
//        covMatrix - matrix of covariations of different components of the random quantity. Its size is ns*ns
//        sampleCovMatrix - matrix of sample covariations - UNUSED
//        similarMatrix - matrix of similarity
//        differenceMatrix - matrix of difference, according to some metric

//    cout << "ns = " << ns << endl;
//    cout << "ndr*fr = " << ndr*fr << endl;

    double ** covMatrix = new double * [ns];
    double ** centeredMatrix = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        covMatrix[i] = new double [ns];
    }
    for(int i = 0; i < ns; ++i)
    {
        centeredMatrix[i] = new double [ndr*fr];
    }

    //vectors for the las stage
    double ** vectorW = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        vectorW[i] = new double [ns];
    }
    double * vector1 = new double [ns];
    double * vector2 = new double [ns];
    double * vector3 = new double [ns];
    double * vectorOld = new double [ns];

    //for full A-matrix count
    double ** matrixA = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        matrixA[i] = new double [ns];
    }
    double * tempVector = new double [ns];

    //components time-flow
    mat components;
    components.resize(ns+1);
//    double ** components = new double * [ns + 1]; //+1 for markers channel
    for(int i = 0; i < ns + 1; ++i)
    {
        components[i].resize(ndr*fr);
//        components[i] = new double [ndr*fr];
    }
    double ** dataICA;
    double * averages;
    double * eigenValues;
    double ** eigenVectors;
    double * tempA;
    double * tempB;


    //wtf is this?
    components[ns-1] = globalEdf.getData()[globalEdf.getMarkChan()];




    //count covariations
    //count averages
    averages = new double [ns];
    for(int i = 0; i < ns; ++i)
    {
        averages[i] = mean(globalEdf.getData()[i].data(), globalEdf.getDataLen());
    }

    //count zeros
    int h = 0;
    int Eyes = 0;
    for(int i = 0; i < ndr*fr; ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(fabs(globalEdf.getData()[i][j]) == 0.) ++h;
        }
        if(h == ns) Eyes += 1;
    }
    double realSignalFrac = (double(ndr*fr - Eyes)/(ndr*fr));

    double helpDouble = 0.;
    //subtract averages
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            //////
            /// remake with lambdas
            helpDouble = globalEdf.getData()[i][j] - averages[i] / realSignalFrac;
            globalEdf.setData(i,j, helpDouble);
        }
    }

    //covariation between different spectra-bins
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            covMatrix[i][j] = 0.;
            covMatrix[i][j] = covariance(globalEdf.getData()[i].data(),
                                         globalEdf.getData()[j].data(),
                                         globalEdf.getDataLen());
//            covMatrix[i][j] /= (ndr*fr); //should be -1 ?
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


    //count eigenvalues & eigenvectors of covMatrix
    eigenValues = new double [ns];
    eigenVectors = new double * [ns]; //vector is a coloumn
    for(int j = 0; j < ns; ++j)
    {
        eigenVectors[j] = new double [ns];
    }

    tempA = new double [ns]; //i
    tempB = new double [ndr*fr]; //j
    double sum1, sum2; //temporary help values
    double dF, F;
    int counter;
    double trace = 0.;

    for(int j = 0; j < ns; ++j)
    {
        trace += covMatrix[j][j];
    }
//    cout << "trace covMatrix = " << trace << endl;


//    cout << "start eigenValues processing" << endl;
    //count eigenValues & eigenVectors


    //array for components
    dataICA = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        memcpy(dataICA[i],
               globalEdf.getData()[i].data(),
               globalEdf.getDataLen() * sizeof(double));
    }
    int numOfPc = 0;



    //counter j - for B, i - for A
    for(int k = 0; k < ns; ++k)
    {
        myTime.restart();
        dF = 1.0;
        F = 1.0;

        //set 1-normalized vector tempA
        for(int i = 0; i < ns; ++i)
        {
            tempA[i] = 1./sqrt(ns);
        }
        for(int j = 0; j < ndr*fr; ++j)
        {
            tempB[j] = 1./sqrt(ndr*fr);
        }



        //approximate P[i] = tempA x tempB;
        counter = 0;
//        cout<<"curr val = "<<k<<endl;
        while(1) //when stop approximate?
        {
            //countF - error
            F = 0.;
            for(int i = 0; i < ns; ++i)
            {
                for(int j = 0; j < ndr*fr; ++j)
                {
                    F += 0.5*(dataICA[i][j]-tempB[j]*tempA[i])*(dataICA[i][j]-tempB[j]*tempA[i]);
                }
            }
            //count vector tempB
            for(int j = 0; j < ndr*fr; ++j)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int i = 0; i < ns; ++i)
                {
                    sum1 += dataICA[i][j]*tempA[i];
                    sum2 += tempA[i]*tempA[i];
                }
                tempB[j] = sum1/sum2;
            }

            //count vector tempA
            for(int i = 0; i < ns; ++i)
            {
                sum1 = 0.;
                sum2 = 0.;
                for(int j = 0; j < ndr*fr; ++j)
                {
                    sum1 += tempB[j]*dataICA[i][j];
                    sum2 += tempB[j]*tempB[j];
                }
                tempA[i] = sum1/sum2;
            }

            dF = 0.;
            for(int i = 0; i < ns; ++i)
            {
                for(int j = 0; j < ndr*fr; ++j)
                {
                    dF += 0.5 * pow((dataICA[i][j] - tempB[j] * tempA[i]), 2.);
                }
            }
            dF = (F-dF)/F;
            ++counter;
            if(counter == 150)
            {
                break;
            }
            if(fabs(dF) < eigenValuesTreshold) break; //crucial cap

            qApp->processEvents();
            if(stopFlag == 1)
            {
                cout << "ICA stopped by user" << endl;
                stopFlag = 0;
                if(1)
                {
                    //clear memory
                    for(int i = 0; i < ns; ++i)
                    {
                        delete [] covMatrix[i];
                        delete [] centeredMatrix[i];
                        delete [] eigenVectors[i];
                        delete [] vectorW[i];
                        delete [] matrixA[i];
                        delete [] dataICA[i];
                    }
                    delete [] centeredMatrix;
                    delete [] covMatrix;
                    delete [] eigenVectors;
                    delete [] averages;
                    delete [] eigenValues;
                    delete [] tempA;
                    delete [] tempB;
                    delete [] vectorOld;
                    delete [] tempVector;
                    delete [] vectorW;
                    delete [] matrixA;
                    delete [] dataICA;

                }
                return;
            }
        }
//        cout<<"val № " << k <<" dF = "<<abs(dF)<< " counter = " << counter << endl;

        //edit covMatrix
        for(int i = 0; i < ns; ++i)
        {
            for(int j = 0; j < ndr*fr; ++j)
            {
                dataICA[i][j] -= tempB[j] * tempA[i];
            }
        }

        //count eigenVectors && eigenValues
        sum1 = 0.;
        sum2 = 0.;
        for(int i = 0; i < ns; ++i)
        {
            sum1 += pow(tempA[i], 2.);
        }
        for(int j = 0; j < ndr*fr; ++j)
        {
            sum2 += pow(tempB[j], 2.);
        }
        for(int i = 0; i < ns; ++i)
        {
            tempA[i] /= sqrt(sum1);
            //test equality of left and right singular vectors
//            if((abs((tempB[i]-tempA[i])/tempB[i]))>threshold) cout << k << " " << i << " warning" << endl;  //till k==19 - OK
        }
        for(int j = 0; j < ndr*fr; ++j)
        {
            tempB[j] /= sqrt(sum2);
        }

        eigenValues[k] = sum1 * sum2 / double(ndr*fr - 1.);

        sum1 = 0.;

        for(int i = 0; i <= k; ++i)
        {
            sum1 += eigenValues[i];
        }

        cout << "numOfPC = " << k << "\t";
        cout << "value = " << eigenValues[k] << "\t";
        cout << "disp = " << 100. * eigenValues[k]/trace << "\t";
        cout << "total = " << 100. * sum1/trace << "\t";
        cout << "iterations = " << counter << "\t";
        cout << myTime.elapsed()/1000. << " sec" << endl;


        for(int i = 0; i < ns; ++i)
        {
            eigenVectors[i][k] = tempA[i]; //1-normalized
        }

    }
    numOfPc = ns;



    ofstream outStream;
    helpString = dir->absolutePath() + slash() + "Help" + slash() + ExpName + "_eigenMatrix.txt";
    outStream.open(helpString.toStdString().c_str());
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            outStream << eigenVectors[i][j] << "  ";
        }
        outStream << endl;
    }
    outStream << endl;
    outStream.close();



    helpString = dir->absolutePath() + slash() + "Help" + slash() + ExpName + "_eigenValues.txt";
    outStream.open(helpString.toStdString().c_str());
    for(int i = 0; i < ns; ++i)
    {
        outStream << eigenValues[i] << '\n';
    }
    outStream.close();



//    cout << "eigenVectors dot product= " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            sum1 = 0.;
//            for(int k = 0; k < ns; ++k)
//            {
//                sum1 += eigenVectors[i][k] * eigenVectors[j][k];

//            }
//            sum1 = int(sum1*100)/100.;
//            cout << sum1 << "\t";
//        }
//        cout << endl;
//    }
//    cout<<endl;




//    //matrix E * D^-0.5 * Et
//    double ** tempMatrix = new double * [ns];
//    for(int i = 0; i < ns; ++i)
//    {
//        tempMatrix[i] = new double [ns];
//    }

//    double * tempVector = new double [ns];

//    for(int i = 0; i < ns; ++i) //columns Et
//    {
//        for(int k = 0; k < ns; ++k) //rows E
//        {

//            tempVector[k] = 0.;
//            for(int s = 0; s < ns; ++s) //counter
//            {
//                tempVector[k] += /*eigenVectors[k][s] * */ eigenVectors[s][i] / sqrt(eigenValues[k]);
//            }
//        }
//        for(int s = 0; s < ns; ++s) //counter
//        {
//            tempMatrix[s][i] = tempVector[s];
//        }

//    }

    //count linear decomposition on PCAs
    for(int j = 0; j < ndr*fr; ++j) //columns initData
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



    for(int j = 0; j < ndr*fr; ++j) //columns X
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
//    cout << "linear decomposition counted" << endl;



    //now dataICA are uncovariated signals with variance 1
    //test of covMatrix dataICA
//    cout << "covMatrixICA = " << endl;
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ns; ++j)
//        {
//            covMatrix[i][j] = 0.;
//            for(int k = 0; k < ndr*fr; ++k)
//            {
//                covMatrix[i][j] += dataICA[i][k] * dataICA[j][k];
//            }
//            covMatrix[i][j] /= ( ndr*fr - 1 );
//            covMatrix[i][j] = int(covMatrix[i][j]*100)/100.;
//            cout << covMatrix[i][j] << " ";
//        }
//        cout<<endl;
//    }
//    cout<<"covMatrixICA counted"<<endl<<endl;

    //ICA itself!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //fastIca wiki - first function
    for(int i = 0; i < ns; ++i) //number of current vectorW
    {
        myTime.restart();
        counter = 0;

        memcpy(vectorW[i], randomVector(ns), ns*sizeof(double));

        while(1)
        {
            for(int j = 0; j < ns; ++j)
            {
                vectorOld[j] = vectorW[i][j];
            }
            product1(dataICA, ndr*fr, ns, vectorW[i], &vector1);
            product2(dataICA, ndr*fr, ns, vectorW[i], &vector2);
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] = vector1[j] - vector2[j];
            }

            //orthogonalization
            product3(vectorW, ns, i, &vector3);
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
                    sum1 += vectorW[k][h]  *vectorW[k][h];
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
                sum1 += vectorW[i][j] * vectorW[i][j];
            }
            for(int j = 0; j < ns; ++j)
            {
                vectorW[i][j] /= sqrt(sum1);
            }


            //check norma difference
            for(int j = 0; j < ns; ++j)
            {
                sum2 += (vectorOld[j] - vectorW[i][j]) * (vectorOld[j] - vectorW[i][j]);
            }
            sum2 = sqrt(sum2);
            ++counter;
            if(sum2 < vectorWTreshold || 2 - sum2 < vectorWTreshold) break;
            if(counter == 300) break;

            /*
            qApp->processEvents();
            if(stopFlag == 1)
            {
                cout << "ICA stopped by user" << endl;
                stopFlag = 0;
                if(1)
                {
                    //clear memory
                    for(int i = 0; i < ns; ++i)
                    {
                        delete [] covMatrix[i];
                        delete [] centeredMatrix[i];
                        delete [] eigenVectors[i];
                        delete [] matrixA[i];
                        delete [] dataICA[i];
                    }
                    delete [] centeredMatrix;
                    delete [] covMatrix;
                    delete [] eigenVectors;
                    delete [] averages;
                    delete [] eigenValues;
                    delete [] tempA;
                    delete [] tempB;
                    delete [] vector1;
                    delete [] vector2;
                    delete [] vector3;
                    delete [] vectorOld;
                    delete [] tempVector;
                    delete [] matrixA;
                    delete [] dataICA;

                }
                return;
            }
            */
        }
        cout << "NumOf vectorW component = " << i << "\t";
        cout << "iterations = " << counter << "\t";
        cout << "error = " << fabs(sum2 - int(sum2+0.5)) << "\t";
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
    matrixProduct(matrix(vectorW, ns, ns),
                  matrix(dataICA, ns, ndr*fr),
                  components, ns);
//    for(int i = 0; i < ns; ++i)
//    {
//        for(int j = 0; j < ndr*fr; ++j)
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
    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] = vectorW[k][i]; //A = Wt
        }
    }

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
    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(eigenValues[i]);//A = D^0.5 * Et * Wt
        }
    }

    for(int i = 0; i < ns; ++i)
    {
        for(int k = 0; k < ns; ++k)
        {
            tempVector[k] = 0.; //new i-th coloumn for matrix A
            for(int s = 0; s < ns; ++s)
            {
                tempVector[k] += eigenVectors[k][s] * matrixA[s][i]; //A = Et * Wt
            }
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] = tempVector[k];
        }
    }


    //test matrix A

    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
        {
            sum1 = 0.;
            for(int k = 0; k < ns; ++k)
            {
                sum1 += matrixA[i][k] * components[k][j];
            }
            if(fabs((globalEdf.getData()[i][j] - sum1) / globalEdf.getData()[i][j]) > 0.05
                    && fabs(globalEdf.getData()[i][j]) > 0.5)
            {
                cout << i << "\t" << j << "\t";
                cout << fabs((globalEdf.getData()[i][j] - sum1)/globalEdf.getData()[i][j]) << "\t";
                cout << globalEdf.getData()[i][j] << endl;
            }
        }
    }

//    cout << "correlations of A*comps[i] and data[i]:" << endl;
//    matrixProduct(matrixA, components, &dataICA, ns, ndr*fr);
//    for(int i = 0; i < ns; ++i)
//    {
//        cout << correlation(dataICA[i], data[i], ndr*fr) << endl;
//    }







//    //norm components - by 1-length vector of mixing matrix
//    for(int i = 0; i < ns; ++i)
//    {
//        sum1 = 0.;

//        for(int k = 0; k < ns; ++k)
//        {
//            sum1 += matrixA[k][i] * matrixA[k][i];
//        }
//        sum1 = sqrt(sum1);
//        for(int k = 0; k < ns; ++k)
//        {
//            matrixA[k][i] /= sum1;
//        }

//        for(int j = 0; j < ndr*fr; ++j)
//        {
//            components[i][j] *= sum1;
//        }
//    }


    //norm components - by equal dispersion ????????????????????
    double coeff = 1.5;
    for(int i = 0; i < ns; ++i)
    {
        sum1 = 0.;
        sum2 = 0.;

        sum1 = mean(components[i], ndr*fr);
        sum2 = variance(components[i], ndr*fr);

        for(int j = 0; j < ndr*fr; ++j)
        {
            if(components[i][j] != 0.) components[i][j] -= sum1 / realSignalFrac;
            components[i][j] /= sqrt(sum2);
            components[i][j] *= coeff;
        }
        for(int k = 0; k < ns; ++k)
        {
            matrixA[i][k] *= sqrt(sum2);
            matrixA[i][k] /= coeff;
        }
    }

    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ndr*fr; ++j)
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
                cout << fabs((globalEdf.getData()[i][j] - sum1)/globalEdf.getData()[i][j]) << "\t";
                cout << globalEdf.getData()[i][j] << endl;
            }
        }
    }



    //now matrixA is true Mixing matrix A*components = initialSignals

//    //test - ok
//    for(int i = 0; i < 30; ++i)
//    {
//        sum1 = 0.;
//        for(int k = 0; k < ns; ++k)
//        {
//            sum1 += matrixA[0][k] * components[k][i];
//        }
//        cout<<sum1<<" ";
//    }
//    cout<<endl<<endl;

//    for(int i = 0; i < 30; ++i)
//    {
//        cout<<data[0][i]<<" ";
//    }
//    cout<<endl;

    //now should draw amplitude maps OR write to file
    helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "Help" + slash() + ExpName + "_maps.txt");
    writeICAMatrix(helpString, matrixA, ns); //generality 19-ns


    helpString = dir->absolutePath() + slash() + ExpName + "_ica.edf";
    /// remake with dataType
//    writeEdf(ui->filePathLineEdit->text(), components, helpString, ndr*def::freq);

    ////to test
    QList <int> chanList;
    makeChanList(chanList);
    globalEdf.writeOtherData(components, helpString, chanList);

    cout << "ICA ended. time = " << wholeTime.elapsed()/1000. << " sec" << endl;

    ns = ui->numOfIcSpinBox->value();
    for(int i = 0; i < ns; ++i)
    {
        delete [] covMatrix[i];
        delete [] centeredMatrix[i];
        delete [] eigenVectors[i];
        delete [] vectorW[i];
        delete [] matrixA[i];
        delete [] dataICA[i];
    }
    delete [] centeredMatrix;
    delete [] covMatrix;
    delete [] eigenVectors;
    delete [] averages;
    delete [] eigenValues;
    delete [] tempA;
    delete [] tempB;
    delete [] vectorOld;
    delete [] tempVector;
    delete [] vectorW;
    delete [] matrixA;
    delete [] dataICA;
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
    sp = new Spectre(dir, ns_, ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
    for(int i = 0; i < 3; ++i)
    {
        helpString = dir->absolutePath() + slash() + "Help" + slash() + ExpName;
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
    sp = new Spectre(dir, ns_, ExpName);
    sp->countSpectra();
    sp->compare();
    sp->compare();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
    for(int i = 0; i < 3; ++i)
    {
        helpString = dir->absolutePath() + slash() + "Help" + slash() + ExpName;
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
        //    sp = new Spectre(dir, ns_, ExpName);
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
    sp = new Spectre(dir, ns_, ExpName);
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
    sp = new Spectre(dir, ns_, ExpName);
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
    outStream << ExpName.left(3).toStdString() << endl;
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
void MainWindow::icaClassTest() //non-optimized
{
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

    helpString2 = ExpName;
    helpString2.replace("_ica", "");
    helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "Help" + slash() + helpString2 + "_maps.txt");
    if(!readICAMatrix(helpString, matrixA, numOfIC))
    {
        return;
    }

    Spectre * spectr;// = Spectre(dir, numOfIC, ExpName);
    helpString = dir->absolutePath() + slash() + "SpectraSmooth";
    Net * ANN = new Net(dir, numOfIC, left, right, spStep, ExpName);
    helpString = dir->absolutePath() + slash() + "16sec19ch.net";
    ANN->loadCfgByName(helpString);
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
    sliceOneByOneNew(numOfIC);

    spectr = new Spectre(dir, numOfIC, ExpName);

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
            sliceOneByOneNew(numOfIC);

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
}

void MainWindow::throwIC()
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
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
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


    double ** dataICA = new double * [ns]; //with markers
    for(int i = 0; i < ns; ++i)
    {
        dataICA[i] = new double [ndr*fr];
        memcpy(dataICA[i],
               globalEdf.getData()[i].data(),
               globalEdf.getDataLen() * sizeof(double));
    }

    int numOfIC = ui->numOfIcSpinBox->value(); // = 19

    double ** matrixA = new double * [numOfIC];
    for(int i = 0; i < numOfIC; ++i)
    {
        matrixA[i] = new double [numOfIC];
    }

    helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "Help" + slash() + ExpName + "_maps.txt");
    readICAMatrix(helpString, matrixA, numOfIC);

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
    sliceOneByOneNew(ns);
    cout << "sliced" << endl;

    constructEDFSlot();

}


////////////////////////// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA need test
void MainWindow::transformEDF(QString inEdfPath, QString mapsPath, QString newEdfPath) // for 19 channels generality
{
    setEdfFile(inEdfPath);
    readData();

    matrix mat1(19, 19);
    readICAMatrix(mapsPath, mat1, 19); // data = mat1 * comps
    mat1.invert(); // mat1 * data = comps

    matrix newData;
    newData.resize(19, ndr*def::freq);

    matrixProduct(mat1, globalEdf.getData(), newData);

    newData.resizeRows(20); // for markers
    newData[19] = globalEdf.getData()[19]; //copy markers

    QList<int> chanList;
    chanList.clear();
    for(int i = 0; i < 20; ++i)
    {
        chanList << i;
    }
    globalEdf.writeOtherData(newData.data, newEdfPath, chanList);
}

void MainWindow::transformReals() //move to library
{
    // I dont care, I dont need this shit anymore
#if 0
    QStringList lst;
    QString helpString;

    helpString = QFileDialog::getOpenFileName(this, tr("choose maps file"), dir->absolutePath(), tr("*.txt"));
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
    dir->cd(procDir);
    lst = dir->entryList(QDir::Files);
    dir->cdUp();

    qApp->processEvents();

//    ifstream inStream;
//    ofstream outStream;
    matrixCreate(&mat1, 19, 100500);
    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = dir->absolutePath() + slash() + procDir + slash() + lst[i];
        readPlainData(helpString, mat1, 19, NumOfSlices);

        matrixProduct(mat3, globalEdf.getData(), mat2, 19, NumOfSlices);

        helpString = dir->absolutePath() + slash() + procDir + slash() + lst[i];
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

void MainWindow::randomDecomposition()
{
    QStringList lst;
    QString helpString;

    QString initName = ExpName;
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

    Spectre * spectr = new Spectre(dir, compNum, ExpName);
    Net * ANN = new Net(dir, compNum, left, right, spStep, ExpName); //generality parameters
    helpString = dir->absolutePath() + slash() + "16sec19ch.net";
    ANN->loadCfgByName(helpString);

    ANN->setReduceCoeff(15.); //generality
    ANN->setNumOfPairs(50);   ////////////////////////////////////////////////////////////////////////////////////////////
    FILE * outFile;


    double ** eigenMatrix = matrixCreate(compNum, compNum);

    /*
    helpString = dir->absolutePath() + slash() + "Help" + slash() + ExpName + "_eigenMatrix.txt";
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
    helpString = dir->absolutePath() + slash() + "Help" + slash() + ExpName + "_eigenValues.txt";
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
        helpString = dir->absolutePath() + slash() + initName;
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
        helpString = dir->absolutePath() + slash() + ExpName + "_randIca.edf";
        globalEdf.writeOtherData(newData.data, helpString, chanList);
//        writeEdf(ui->filePathLineEdit->text(), newData, helpString, ndr*def::freq);

        helpString = dir->absolutePath() + slash() + ExpName + "_randIca.edf";
        setEdfFile(helpString);
        readData(); //read newData

        ui->cleanRealisationsCheckBox->setChecked(true);
        ui->cleanRealsSpectraCheckBox->setChecked(true);
        cleanDirs();

        sliceOneByOneNew(19); //sliceAll()
        spectr->defaultState();
        spectr->countSpectra();
        ANN->autoClassificationSimple();
        helpString = dir->absolutePath() + slash() + initName + "_randIca.txt";
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

void MainWindow::spoc()
{
    QString helpString;

    readData();
    nsBackup = ns;
    ns = ui->numComponentsSpinBox->value(); //test

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

    helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "spocVar.txt");
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
