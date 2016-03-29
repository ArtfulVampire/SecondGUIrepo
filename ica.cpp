#include "mainwindow.h"
#include "ui_mainwindow.h"


using namespace std;




void MainWindow::ICA() //fastICA
{

#define MATRICES_ICA_0 1
#define MATRICES_ICA_1 1
#define MATRICES_ICA_2 1
#define MATRICES_ICA_3 1
#define MATRICES_ICA_4 1
#define MATRICES_ICA_5 1
#define MATRICES_ICA_6 1


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

    QString helpString;
    int counter;

    helpString = def::dir->absolutePath()
                 + slash() + def::ExpName + ".edf";
    cout << "Ica started: " << helpString << endl;
    readData();

    const int ns = ui->numOfIcSpinBox->value(); //generality. Bind to reduceChannelsLineEdit?
    const int dataLength = globalEdf.getDataLen();


    const double eigenValuesTreshold = pow(10., - ui->svdDoubleSpinBox->value());
    const double vectorWTreshold = pow(10., - ui->vectwDoubleSpinBox->value());
    const QString pathForAuxFiles = def::dir->absolutePath()
                                    + slash() + "Help"
                                    + slash() + "ica";

    matrix centeredMatrix = globalEdf.getData();

    matrix components(ns + 1, dataLength, 0.);
    components[ns] = globalEdf.getData()[globalEdf.getMarkChan()];


    // count eigenvalue decomposition
    matrix eigenVectors;
    lineType eigenValues;

    dealWithEyes(centeredMatrix,
                 ns);

    const QString eigMatPath = pathForAuxFiles
                               + slash() + def::ExpName + "_eigenMatrix.txt";
    const QString eigValPath = pathForAuxFiles
                               + slash() + def::ExpName + "_eigenValues.txt";
    /// careful !
    if(!QFile::exists(eigMatPath) &&
       !QFile::exists(eigValPath))
    {

        svd(centeredMatrix,
            eigenVectors,
            eigenValues,
            ns,
            eigenValuesTreshold);

        // write eigenVectors
        writeMatrixFile(eigMatPath, eigenVectors);

        // write eigenValues
        writeFileInLine(eigValPath, eigenValues);
    }
    else /// read
    {
        // write eigenVectors
        readMatrixFile(eigMatPath, eigenVectors);

        // write eigenValues
        readFileInLine(eigValPath, eigenValues);
    }

    cout << "ICA: svd read = " << myTime.elapsed() / 1000. << " sec" << endl;
    myTime.restart();


    // components = EigenValues^-0.5 * Et * data
    matrix D_minus_05(ns, ns, 0.);
    for(int i = 0; i < ns; ++i)
    {
        D_minus_05[i][i] = 1. / sqrt(eigenValues[i]);
    }

    matrix tmpMat{};
    tmpMat = D_minus_05 * matrix::transpose(eigenVectors);

//    components = tmpMat * centeredMatrix;
    matrixProduct(tmpMat,
                  centeredMatrix,
                  components,
                  ns);


    matrix dataICA{};
//    dataICA = eigenVectors * components;
    matrixProduct(eigenVectors,
                  components,
                  dataICA,
                  ns);


    //now dataICA are uncovariated signals with variance 1
    // ICA itself!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // fastIca wiki - first function

    double sum1;

    matrix vectorW(ns, ns);
    countVectorW(vectorW,
                 dataICA,
                 ns,
                 dataLength,
                 vectorWTreshold);
    cout << "ICA: vectorW ready = " << myTime.elapsed() / 1000. << " sec" << endl;
    myTime.restart();

//    helpString = pathForAuxFiles
//                 + slash() + def::ExpName + "_vectorW.txt";
//    writeMatrixFile(helpString,
//                     vectorW);


    //count components
//    components = vectorW * dataICA;
    matrixProduct(vectorW,
                  dataICA,
                  components,
                  ns);


    //count full mixing matrix A = E * D^0.5 * Et * Wt
    matrix matrixA(ns, ns, 0.);
    matrix D_05(ns, ns, 0.);
    for(int i = 0; i < ns; ++i)
    {
        D_05[i][i] = sqrt(eigenValues[i]);
    }
#if 1
    /// test
    matrixA = eigenVectors * D_05 * matrix::transpose(eigenVectors) * matrix::transpose(vectorW);
#else

    matrixA = matrix::transpose(vectorW); // A = Wt

    matrixA = matrix::transpose(eigenVectors) * matrixA; //A = Et * Wt

    //A = D^0.5 * Et * Wt
    matrixA = D_05 * matrixA;

    matrixA = eigenVectors * matrixA;
#endif

#if 0
    //test  data = matrixA * comps;

    counter = 0;
    for(int j = 0; j < dataLength; ++j)
    {
        lineType currCol = components.getCol(j, ns);
        for(int i = 0; i < ns; ++i)
        {
            sum1 = abs((centeredMatrix[i][j] - prod(currCol, matrixA[i]))
                       / centeredMatrix[i][j]);
            if(sum1 > 0.05
               && abs(centeredMatrix[i][j]) > 0.5)
            {
                ++counter;
#if 0
                cout << "before norm" << "\t";
                cout << i << "\t" << j << "\t";
                cout << "err = " << doubleRound(sum1, 3) << "\t";
                cout << "init value = " << doubleRound(centeredMatrix[i][j], 4) << endl;
#endif
            }
        }
    }
    cout << "num of errors = " << counter << endl;
#endif



#if 0
    /// remake
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
    writeMatrixFile(helpString, matrixA, ns); //generality 19-ns

    for(int i = 0; i < ns; ++i)
    {
        explainedVariance.push_back(colsNorms[i].first / sumSquares * 100.);
        cout << "comp = " << i+1 << "\t";
        cout << "explVar = " << doubleRound(explainedVariance[i], 2) << endl;
    }
    //end componets ordering
#else
    // norm components to 1-length of mapvector, order by dispersion
    for(int i = 0; i < ns; ++i) // for each component
    {
        sum1 = norma(matrixA.getCol(i));

        for(int k = 0; k < ns; ++k)
        {
            matrixA[k][i] /= sum1;
        }
        components[i] *= sum1;
    }

    // ordering components by dispersion
    std::vector <std::pair <double, int>> colsNorms; // dispersion, numberOfComponent
    double sumSquares = 0.; // sum of all dispersions
    vectType explainedVariance;

    for(int i = 0; i < ns; ++i)
    {
        sum1 = variance(components[i]);
        sumSquares += sum1;
        colsNorms.push_back(std::make_pair(sum1, i));
    }

    std::sort(colsNorms.begin(),
              colsNorms.end(),
              [](std::pair <double, int> i, std::pair <double, int> j)
    {
        return i.first > j.first;
    });

    int tempIndex;
    for(int i = 0; i < ns - 1; ++i) // dont move the last
    {
        // swap matrixA cols
#if MATRICES_ICA_4
        matrixA.swapCols(i, colsNorms[i].second);
#else
        vectType tempCol(ns);
        for(int j = 0; j < ns; ++j) // swap j'th elements in i'th and colsNorms[i].second'th cols
        {
            tempCol[j] = matrixA[j][i];
            matrixA[j][i] = matrixA[j][ colsNorms[i].second ];
            matrixA[j][ colsNorms[i].second ] = tempCol[j];
        }
#endif

        // swap components
#if MATRICES_ICA_5
        components.swapRows(i, colsNorms[i].second);
#else
        vectType tempComp;
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

//        std::swap((*it1).second, (*it2).second);
        tempIndex = (*it1).second;
        (*it1).second = (*it2).second;
        (*it2).second = tempIndex;
    }

    for(int i = 0; i < ns; ++i)
    {
        explainedVariance.push_back(colsNorms[i].first / sumSquares * 100.);
        cout << "comp = " << i+1 << "\t";
        cout << "explVar = " << doubleRound(explainedVariance[i], 2) << endl;
    }
    helpString = QDir::toNativeSeparators(pathForAuxFiles
                                          + slash() + def::ExpName + "_explainedVariance.txt");
    writeFileInLine(helpString, explainedVariance);
    //end componets ordering
#endif


#if 1
    // test  data = matrixA * comps;
    // again to check reordering and normalizations

    counter = 0;
    for(int j = 0; j < dataLength; ++j)
    {
        lineType currCol = components.getCol(j, ns);
        for(int i = 0; i < ns; ++i)
        {
            sum1 = abs((centeredMatrix[i][j] - prod(currCol, matrixA[i]))
                       / centeredMatrix[i][j]);
            if(sum1 > 0.05
               && abs(centeredMatrix[i][j]) > 0.5)
            {
                ++counter;
#if 0
                cout << "after norm" << "\t";
                cout << i << "\t" << j << "\t";
                cout << "err = " << doubleRound(sum1, 3) << "\t";
                cout << "init value = " << doubleRound(centeredMatrix[i][j], 4) << endl;
#endif
            }
        }
    }
    cout << "num of errors = " << counter << endl;
#endif


    //now should draw amplitude maps OR write to file
        helpString = pathForAuxFiles
                     + slash() + def::ExpName + "_maps.txt";
    writeMatrixFile(helpString, matrixA); //generality 19-ns
    drawMapsICA(helpString);


    // save components
    helpString = def::dir->absolutePath()
                 + slash() + def::ExpName + "_ica.edf";

    vector<int> chanList(ns);
    std::iota(chanList.begin(), chanList.end(), 0);
    chanList.push_back(globalEdf.getMarkChan());

    components.push_back(globalEdf.getData()[globalEdf.getMarkChan()]);
    globalEdf.writeOtherData(components, helpString, chanList);
    def::ns = ns + 1; // numOfICs + markers

    cout << "ICA ended. time = " << wholeTime.elapsed()/1000. << " sec" << endl;
}


void MainWindow::reorderIcaFile(const QString & icaPath,
                                vector<int> chanList,
                                QString icaOutPath,
                                QString mapsPath,
                                QString mapsOutPath)
{
    QString fileName = getFileName(icaPath, false);
    fileName.remove("_ica");

    if(mapsPath.isEmpty())
    {
        mapsPath = icaPath;
        mapsPath.resize(mapsPath.lastIndexOf(slash()));
        mapsPath += slash() + "Help"
                    + slash() + fileName + "_maps.txt";
        cout << mapsPath << endl;
    }

    if(icaOutPath.isEmpty())
    {
        icaOutPath = icaPath;
        icaOutPath.replace(".edf", "_ro.edf");
        cout << icaOutPath << endl;
    }

    if(mapsOutPath.isEmpty())
    {
        mapsOutPath = icaPath;
        mapsOutPath.resize(mapsOutPath.lastIndexOf(slash()));
        mapsOutPath += slash() + "Help"
                    + slash() +  getFileName(icaOutPath, false) + "_maps.txt";
        cout << mapsOutPath << endl;
    }

    edfFile localEdf;
    localEdf.setMatiFlag(def::matiFlag);
    localEdf.setNtFlag(def::ntFlag);

    localEdf.readEdfFile(icaPath);
    const int localNs = localEdf.getNs();
    /// remake set
    if(std::find(chanList.begin(),
                  chanList.end(),
                  int(localNs - 1)) != chanList.end())
    {
        chanList.push_back(localNs - 1);
    }

    matrix maps;
    readMatrixFile(mapsPath, maps);
    maps.transpose();

    localEdf.reduceChannels(chanList);

    matrix newMaps(chanList.size() - 1, 0);
    for(int i = 0; i < chanList.size() - 1; ++i) // -1 for markers
    {
        newMaps[i] = maps[chanList[i]];
    }
    newMaps.transpose();


    localEdf.writeEdfFile(icaOutPath);
    writeMatrixFile(mapsOutPath, newMaps);

}


void MainWindow::ICsSequence(const QString & EDFica1,
                             const QString & EDFica2,
                             const int mode,
                             QString maps1Path,
                             QString maps2Path)
{

    QTime myTime;
    myTime.start();


    QString helpString;
    /// make good mapsPathes
    if(maps1Path.isEmpty())
    {
        helpString = getFileName(EDFica1, false);
        helpString.remove("_ica");

        maps1Path = EDFica1;
        maps1Path.resize(maps1Path.lastIndexOf(slash()));
        maps1Path += slash() + "Help"
                     + slash() + "ica"
                     + slash() + helpString + "_maps.txt";
        cout << "maps1Path = " << maps1Path << endl;
    }
    if(maps2Path.isEmpty())
    {
        helpString = getFileName(EDFica2, false);
        helpString.remove("_ica");

        maps2Path = EDFica2;
        maps2Path.resize(maps2Path.lastIndexOf(slash()));
        maps2Path += slash() + "Help"
                     + slash() + "ica"
                     + slash() + helpString + "_maps.txt";
        cout << "maps2Path = " << maps2Path << endl;
    }
    // mode == 0 -> sequency by most stability
    // mode == 1 -> sequency by first File & no overwrite

    /// make enum
    if(mode != 0 && mode != 1)
    {
        cout << "bad mode" << endl;
        return;
    }


    def::ns = 20;
    const int ns_ = 19;
    const double corrMapCoeff = 2.;

    QString helpString2;

    matrix dataFFT1 (def::numOfClasses(), 0);
    matrix dataFFT2 (def::numOfClasses(), 0);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->reduceChannelsComboBox->setCurrentText("20");
    ui->reduceChannelsCheckBox->setChecked(false);

    /// count and remember averagge spectra
    setEdfFile(EDFica1);
    cleanDirs();
    readData();
    sliceAll();
    countSpectraSimple(4096);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        helpString = def::dir->absolutePath()
                     + slash() + "Help"
                     + slash() + "psa"
                     + slash() + def::ExpName + "_class_" + QString::number(i + 1) + ".psa";
        readFileInLine(helpString, dataFFT1[i]);
    }

    setEdfFile(EDFica2);
    cleanDirs();
    readData();
    sliceAll();
    countSpectraSimple(4096);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        helpString = def::dir->absolutePath()
                     + slash() + "Help"
                     + slash() + "psa"
                     + slash() + def::ExpName + "_class_" + QString::number(i + 1) + ".psa";
        readFileInLine(helpString, dataFFT2[i]);
    }

    //sequence ICs
    matrix mat1 (ns_, ns_);
    matrix mat2 (ns_, ns_);

    //read matrices
    readMatrixFile(maps1Path, mat1);
    readMatrixFile(maps2Path, mat2);

    //transpose ICA maps
    mat1.transpose();
    mat2.transpose();

    QVector<int> list1;
    QVector<int> list2;

    struct ICAcoeff
    {
        double cMap;
        double cSpectr[5];
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

    matrix corrs (ns_, ns_);
    double corrSpectr[def::numOfClasses()];
    double corrMap;

    int maxShift = 2; ////// num of spectra bins to count spectra correlations

    double tempDouble;
    double helpDouble;

    /// count matrix of correlation coefficients
    for(int k = 0; k < ns_; ++k)
    {
        for(int j = 0; j < ns_; ++j)
        {
            corrMap = correlation(mat1[k], mat2[j], ns_, 0, true);
            corrMap = pow(corrMap, 2);

            coeffs[k][j].cMap = corrMap;
            coeffs[k][j].cSpectr[def::numOfClasses()] = 0.; // summary
            for(int h = 0; h < def::numOfClasses(); ++h)
            {
                corrSpectr[h] = 0.;
                for(int shift = -maxShift; shift <= maxShift; ++shift)
                {
                    vectType tempVec1(def::spLength());
                    vectType tempVec2(def::spLength());
                    std::copy(begin(dataFFT1[h]) + k * def::spLength(),
                              begin(dataFFT1[h]) + (k + 1) * def::spLength(),
                              begin(tempVec1));
                    std::copy(begin(dataFFT2[h]) + j * def::spLength(),
                              begin(dataFFT2[h]) + (j + 1) * def::spLength(),
                              begin(tempVec2));

                    corrSpectr[h] = fmax( fabs(correlation(tempVec1,
                                                           tempVec2,
                                                           def::spLength(),
                                                           shift)),
                                          corrSpectr[h]);
                }
                corrSpectr[h] = pow(corrSpectr[h], 2);
                helpDouble += corrSpectr[h]; //////

                coeffs[k][j].cSpectr[h] = corrSpectr[h];
                coeffs[k][j].cSpectr[def::numOfClasses()] += corrSpectr[h];
            }
            helpDouble = sqrt(coeffs[k][j].cMap * corrMapCoeff)
                         + sqrt(coeffs[k][j].cSpectr[def::numOfClasses()]);
            corrs[k][j] = helpDouble;
            coeffs[k][j].sumCoef = helpDouble;
        }
    }
//    cout << "wanted:" << endl;
//    cout << coeffs[1][3].cMap << "\t" << coeffs[1][3].cSpectr[def::numOfClasses()] << endl;
//    cout << "real:" << endl;
//    cout << coeffs[1][11].cMap << "\t" << coeffs[1][11].cSpectr[def::numOfClasses()] << endl;


    //find best correlations - check corrs matrix
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
//                if(i == k) continue;  ////////////////////
                if(corrs[i][k] > tempDouble)
                {
                    tempDouble = corrs[i][k];
                    temp1 = i;
                    temp2 = k;
                }
            }
        }



        if(j == ns_ - 1 && EDFica1 == EDFica2) ///////////////////// whaaaaat?
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
            for(int h = 0; h < def::numOfClasses(); ++h)
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
        for(int h = 0; h < def::numOfClasses(); ++h)
        {
            ICAcorrArr[j].coeff.cSpectr[h] = coeffs[temp1][temp2].cSpectr[h];
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        tempDouble = ICAcorrArr[j].coeff.cMap;
        for(int h = 0; h < def::numOfClasses(); ++h)
        {
            tempDouble += ICAcorrArr[j].coeff.cSpectr[h];
        }
        tempDouble = sqrt(tempDouble);


        ICAcorrArr[j].coeff.sumCoef = tempDouble;
    }

    matrix newMaps (ns_, ns_);

    ui->reduceChannelsCheckBox->setChecked(true);

    //sequence
    //1st file
    // mode == 0) by most stable components (which have the best matching in second file)

    for(int k = 0; k < ns_; ++k)
    {
        if(mode == 0)
        {
            newMaps[k] = mat1[ICAcorrArr[k].num1]; ///////////////////by most stability
        }
        else if (mode == 1)
        {
            newMaps[k] = mat1[k]; //////////////////////////by first file
        }

    }
    newMaps.transpose();

    helpString2 = maps1Path;
    helpString2.replace("_maps.txt", "_ord_maps.txt");
    writeMatrixFile(helpString2, newMaps);


    if(mode == 0)
    {
        helpString.clear();
        for(int k = 0; k < ns_; ++k)
        {
            helpString += QString::number( ICAcorrArr[k].num1 + 1 ) + " "; /////////by most stability
        }
        helpString += "20";
        ui->reduceChannelsLineEdit->setText(helpString);

        setEdfFile(EDFica1);
        helpString2 = EDFica1;
        helpString2.replace(".edf", "_ord.edf");
        reduceChannelsEDF(helpString2);
    }
    else
    {
        helpString2 = EDFica1;
        helpString2.replace(".edf", "_ord.edf");
        QFile::remove(helpString2);
        QFile::copy(EDFica1, helpString2);
    }


    //second file
    for(int k = 0; k < ns_; ++k)
    {
        if(mode == 0)
        {
            newMaps[k] = mat2[ICAcorrArr[k].num2]; /////////////////////////////by most stability
        }
        else if(mode == 1)
        {
            newMaps[k] = mat2[ list2[list1.indexOf(k)] ]; ////////////////by first file
        }

    }
    newMaps.transpose();
    helpString2 = maps2Path;
    helpString2.replace("_maps.txt", "_ord_maps.txt");
    writeMatrixFile(helpString2, newMaps);


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
    }
    helpString += "20";
    ui->reduceChannelsLineEdit->setText(helpString);

    setEdfFile(EDFica2);
    helpString2 = EDFica2;
    helpString2.replace(".edf", "_ord.edf");
    reduceChannelsEDF(helpString2);






    ui->reduceChannelsCheckBox->setChecked(false);
    ui->reduceChannelsComboBox->setCurrentText("20");

    helpString2 = EDFica1;
    helpString2.replace(getFileName(EDFica1), "Help/" + getFileName(EDFica1, false) + "_ord_all.jpg");
    if(!QFile(helpString2).exists())
    {
        helpString2 = EDFica1;
        helpString2.replace(".edf", "_ord.edf");
        setEdfFile(helpString2);
        cleanDirs();
        sliceAll();
        countSpectraSimple(4096);
    }

    helpString2 = EDFica2;
    helpString2.replace(".edf", "_ord.edf");
    setEdfFile(helpString2);
    cleanDirs();
    sliceAll();
    countSpectraSimple(4096);


#if 0
    ofstream outStream;
    outStream.open("/media/Files/Data/AB/12", ios_base::out|ios_base::app);
    outStream << def::ExpName.left(3).toStdString() << endl;
    for(int k = 0; k < ns_; ++k)
    {
        cout << k+1 << "\t";
        cout << ICAcorrArr[k].num1 + 1 << "\t";
        cout << ICAcorrArr[k].num2 + 1 <<  "\t";
        cout << doubleRound(sqrt(ICAcorrArr[k].coeff.cMap), 3) <<  "\t";

        for(int h = 0; h < def::numOfClasses(); ++h)
        {
            cout << doubleRound(sqrt(ICAcorrArr[k].coeff.cSpectr[h]), 3) <<  "\t";
        }
        cout << doubleRound(ICAcorrArr[k].coeff.sumCoef, 3) << endl;


        outStream << k+1 << "\t";
        outStream << ICAcorrArr[k].num1 + 1 << "\t";
        outStream << ICAcorrArr[k].num2 + 1 <<  "\t";
        outStream << doubleRound(sqrt(ICAcorrArr[k].coeff.cMap), 3) <<  "\t";

        for(int h = 0; h < def::numOfClasses(); ++h)
        {
            outStream << doubleRound(sqrt(ICAcorrArr[k].coeff.cSpectr[h]), 3) <<  "\t";
        }
        outStream << doubleRound(ICAcorrArr[k].coeff.sumCoef, 3) << endl;
    }
    outStream.close();
#endif


    cout << "ICsSequence ended. time = " << myTime.elapsed()/1000. << " sec" << endl;
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
    if(!readMatrixFile(helpString, matrixA, numOfIC))
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
#if 0
    /// unused
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
    readMatrixFile(helpString, matrixA);

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
#endif

}


////////////////////////// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA need test
void MainWindow::transformEdfMaps(const QString & inEdfPath,
                                  const QString & mapsPath,
                                  const QString & newEdfPath)
{
    QTime myTime;
    myTime.start();

    setEdfFile(inEdfPath);
    readData();

    matrix mat1(def::nsWOM(), def::nsWOM());
    readMatrixFile(mapsPath, mat1); // data = mat1 * comps
    mat1.invert(); // mat1 * data = comps

    matrix newData(def::nsWOM(), globalEdf.getDataLen());

    matrixProduct(mat1,
                  globalEdf.getData(),
                  newData,
                  def::nsWOM());

    newData.push_back(globalEdf.getData()[globalEdf.getMarkChan()]); //copy markers

    globalEdf.writeOtherData(newData, newEdfPath);
    cout << "transformEdfMaps: time elapsed = " << myTime.elapsed() / 1000. << " sec" << endl;
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

    readMatrixFile(helpString, mat1, 19);
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
        readPlainData(helpString, mat1, NumOfSlices);

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


// holy fck, how long and bad it is
void MainWindow::spoc()
{

#if 0
    QString helpString;

    readData();
    nsBackup = def::ns;
    int ns = ui->numComponentsSpinBox->value(); //test

    double * W = new double  [ns];
    double * WOld = new double  [ns];

    /// remade 30.10.15
    double timeShift = ui->timeShiftSpinBox->value() * def::freq;
    double epochLength = ui->windowLengthSpinBox->value() * def::freq;

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

            averages[j] = mean(globalEdf.getData()[j].data()
                                + i * int(timeShift),
                               epochLength);
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

                Ce[i][j][k] = variance(globalEdf.getData()[j].data()
                                       + i * int(timeShift),
                                       epochLength);
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
#endif

}
