#include "net.h"
#include "ui_net.h"
using namespace std;
using namespace myLib;
using namespace smallLib;

std::vector<int> Net::makeLearnIndexSet()
{
    std::vector<int> mixNum;
    if(Mode == myMode::train_test)
    {
        for(uint i = 0; i < fileNames.size(); ++i)
        {
            if(fileNames[i].contains("_train"))
            {
                mixNum.push_back(i);
            }
        }
    }
    else if(Mode == myMode::N_fold)
    {
        for(uint i = 0; i < dataMatrix.rows(); ++i)
        {
            mixNum.push_back(i);
        }
    }
    else if(Mode == myMode::k_fold)
    {
        mixNum.resize(dataMatrix.rows());
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::iota(mixNum.begin(), mixNum.end(), 0);
        std::shuffle(mixNum.begin(),
                     mixNum.end(),
                     std::default_random_engine(seed));
        const int folds = ui->foldSpinBox->value();
        mixNum.resize(mixNum.size() * (folds - 1) / folds);
    }
    else if(Mode == myMode::half_half)
    {
        mixNum.resize(dataMatrix.rows() / 2);
        std::iota(mixNum.begin(), mixNum.end(), 0);
    }
    return mixNum;
}


double Net::adjustLearnRate(int lowLimit,
                            int highLimit)
{

    QTime myTime;
    myTime.start();

    cout << "adjustLearnRate: start" << endl;

    const std::vector<int> mixNum = makeLearnIndexSet();

    double res = 1.;
    int counter = 0;
    do
    {
        /// remake with indices
        const double currVal = ui->learnRateBox->value();
        cout << "lrate = " << currVal << '\t';

        learnNetIndices(mixNum);

        /// check limits
        if(this->getEpoch() > highLimit ||
           this->getEpoch() < lowLimit)
        {
            ui->learnRateBox->setValue(currVal
                                       * sqrt(this->getEpoch()
                                              /  ((lowLimit + highLimit) / 2.)
                                              )
                                       );
        }
        else
        {
            res = currVal;
            break;
        }

        /// check lrate values
        if(ui->learnRateBox->value() < 0.005)
        {
            ui->learnRateBox->setValue(0.005); break;
        }
        else if(ui->learnRateBox->value() >= 1.)
        {
            ui->learnRateBox->setValue(1.); break;
        }
        ++counter;
    } while (counter < 15);
    cout << "adjustLearnRate: lrate = " << res << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
    return res;
}


double Net::adjustReduceCoeff(QString spectraDir,
                              int lowLimit,
                              int highLimit,
                              QString paFileName)
{
    QTime myTime;
    myTime.start();


    double res;
    double currVal;
    const double threshold = 5e-2;

    cout << "adjustReduceCoeff: start" << endl;

    while(1)
    {
        currVal = ui->rdcCoeffSpinBox->value();

        /// remake with indices
        makePaStatic(spectraDir,
                     ui->foldSpinBox->value(),
                     currVal);
        cout << "coeff = " << currVal << "\t";

        if(Mode == myMode::train_test)
        {
            PaIntoMatrixByName(paFileName);
            learnNet();
        }
        else
        {
            PaIntoMatrixByName("all");

//            cout << dataMatrix.rows() << endl;
//            for(int i = 0; i < def::numOfClasses(); ++i)
//            {
//                cout << classCount[i] << "\t";
//            }
//            cout << endl;

            vector<int> learnIndices;
            for(uint i = 0; i < dataMatrix.rows(); ++i)
            {
//                cout << i << "\t" << fileNames[i] << endl;
                if(fileNames[i].contains("train"))
                {
                    learnIndices.push_back(i);
                }
            }
//            for(int item : learnIndices)
//            {
//                cout << item << "\t" << fileNames[item] << endl;
//            }
            learnNetIndices(learnIndices);
        }

        if(this->getEpoch() > highLimit
           || this->getEpoch() < lowLimit)
        {
            ui->rdcCoeffSpinBox->setValue(currVal
                                          / sqrt(this->getEpoch()
                                                 /  ((lowLimit + highLimit) / 2.)
                                                 )
                                          );
            if(currVal <= threshold) //possible minimum
            {
                cout << "adjustReduceCoeff: stop, too little" << endl;
                res = threshold;
                break;
            }
        }
        else
        {
            res = currVal;
            break;
        }
    }
    cout << "adjustReduceCoeff: reduceCoeff = " << res << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
    return res;
}


void Net::autoClassificationSimple()
{

//    ui->deltaRadioButton->setChecked(true); //generality
    QString helpString;
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash + "SpectraSmooth");

    if(Source == source::winds) //generality
    {
        helpString += slash + "windows";
    }
    else if(Source == source::bayes)
    {
        helpString += slash + "Bayes";
    }
    else if(Source == source::pca)
    {
        helpString += slash + "PCA";
    }

    if(!helpString.isEmpty())
    {
        autoClassification(helpString);
    }
}


std::pair<std::vector<int>, std::vector<int>> Net::makeIndicesSetsCross(
        const std::vector<std::vector<int>> & arr,
        const int numOfFold)
{
    std::vector<int> learnInd;
    std::vector<int> tallInd;

    const int fold = ui->foldSpinBox->value();

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        for(int j = 0; j < classCount[i]; ++j)
        {
            if(j >= (classCount[i] * numOfFold / fold) &&
               j <= (classCount[i] * (numOfFold + 1) / fold))
            {
                tallInd.push_back(arr[i][j]);
            }
            else
            {
                learnInd.push_back(arr[i][j]);
            }
        }
    }
    return make_pair(learnInd, tallInd);
}

void Net::autoClassification(const QString & spectraDir)
{
    QTime myTime;
    myTime.start();

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash + "log.txt");
    QFile::remove(helpString);
#if 0
    //set random matrix - add in PaIntoMatrixByName
    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            tempRandomMatrix[i][j] = (i == j); //identity matrix
//            tempRandomMatrix[i][j] = (i == j) * (5 + rand()%36) / 10.; //random diagonal
//            tempRandomMatrix[i][j] = (5. + rand()%36) / 50.; //full random
//            tempRandomMatrix[i][j] = (i == j) * (1. + (rand()%8 == 0) * (5. + rand()%35) / 5.); //random diagonal
        }
    }
#endif

//    cout << spectraDir << endl;
    loadData(spectraDir);

    if(ui->adjustLeanrRateCheckBox->isChecked())
    {
        adjustLearnRate(ui->lowLimitSpinBox->value(),
                        ui->highLimitSpinBox->value()); // or reduce coeff ?
    }

    confusionMatrix.fill(0.);
    switch(Mode)
    {
    case myMode::k_fold:
    {
        crossClassification(); break;
    }
    case myMode::N_fold:
    {
        leaveOneOutClassification(); break;
    }
    case myMode::train_test:
    {
        trainTestClassification(); break;
    }
    case myMode::half_half:
    {
        halfHalfClassification(); break;
    }
    default: {break;}
    }

    learnNet();
    writeWts();
    drawWts();
    cout <<  "AutoClass: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
}

void Net::averageClassification(matrix * inMat)
{
    if(inMat == nullptr)
    {
        inMat = &confusionMatrix;
    }
    /// deal with confusionMatrix

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash + "results.txt");
    ofstream res;
    res.open(helpString.toStdString(), ios_base::app);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        const double num = (*inMat)[i].sum();
        if(num != 0.)
        {
            res << doubleRound((*inMat)[i][i] * 100. / num, 2) << '\t';
        }
        else
        {
            res << "pew" << '\t';
        }
    }

    // count averages, kappas
    double corrSum = 0.;
    double wholeNum = 0.;

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        corrSum += (*inMat)[i][i];
        wholeNum += (*inMat)[i].sum();
    }
    averageAccuracy = corrSum * 100. / wholeNum;

    // kappa
    double pE = 0.; // for Cohen's kappa
    const double S = (*inMat).sum();
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        pE += ((*inMat)[i].sum() * (*inMat).getCol(i).sum()) / pow(S, 2);
    }
    kappa = 1. - (1. - corrSum / wholeNum) / (1. - pE);

    res << doubleRound(averageAccuracy, 2) << '\t';
    res << doubleRound(kappa, 3) << '\t';
    res << def::ExpName << endl;
    res.close();

    (*inMat).print();
    cout << "average accuracy = " << doubleRound(averageAccuracy, 2) << endl;
    cout << "kappa = " << kappa << endl;
}

void Net::leaveOneOutClassification()
{
    if(Source == source::pca)
    {
        ofstream outStr;
        outStr.open((def::dir->absolutePath()
                    + slash + "pcaRes.txt").toStdString());
        // auto pca classification
        for(int i = ui->autoPCAMaxSpinBox->value();
            i >= ui->autoPCAMinSpinBox->value();
            i -= ui->autoPCAStepSpinBox->value())
        {
            cout << "numOfPc = " << i  << " \t";
            dataMatrix.resizeCols(i);

            adjustLearnRate(ui->lowLimitSpinBox->value(),
                            ui->highLimitSpinBox->value());

            leaveOneOut();
            outStr << i << "\t" << averageAccuracy << endl;
        }
        outStr.close();
    }
    else
    {
        cout << "Net: autoclass (max " << dataMatrix.rows() << "):" << endl;
        leaveOneOut();
    }

    return;
    if(!ui->openclCheckBox->isChecked())
    {
        leaveOneOut();
    }
    else
    {
//        leaveOneOutCL();
    }
}

void Net::crossClassification()
{
    const int numOfPairs = ui->numOfPairsBox->value();
    const int fold = ui->foldSpinBox->value();

    std::vector<std::vector<int>> arr;
    arr.resize(def::numOfClasses(), {});
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        arr[ types[i] ].push_back(i);
    }
    cout << "Net: autoclass (max " << numOfPairs << "):" << endl;

    for(int i = 0; i < numOfPairs; ++i)
    {
        cout << i + 1;
        cout << " "; cout.flush();

        // mix arr for one "pair"-iteration
        for(int i = 0; i < def::numOfClasses(); ++i)
        {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(arr[i].begin(),
                         arr[i].end(),
                         std::default_random_engine(seed));
        }

        /// new with indices
        for(int numFold = 0; numFold < fold; ++numFold)
        {
//            if(tallCleanFlag)
//            {
//                /// remake for k-fold tallCleanFlag
//                arr.resize(def::numOfClasses(), {});
//                for(int i = 0; i < dataMatrix.rows(); ++i)
//                {
//                    arr[ types[i] ].push_back(i);
//                }
//                for(int i = 0; i < def::numOfClasses(); ++i)
//                {
//                    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//                    std::shuffle(arr[i].begin(),
//                                 arr[i].end(),
//                                 std::default_random_engine(seed));
//                }
//            }

            auto sets = makeIndicesSetsCross(arr, numFold);
            learnNetIndices(sets.first);
            tallNetIndices(sets.second);
        }

        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = 0;
            return;
        }
    }
    cout << endl;
    cout << "cross classification - ";
    averageClassification();
}

void Net::halfHalfClassification()
{
    std::vector<int> learnIndices;
    std::vector<int> tallIndices;

    for(uint i = 0; i < dataMatrix.rows() / 2; ++i)
    {
        learnIndices.push_back(i);
        tallIndices.push_back(i + dataMatrix.rows() / 2);
    }
    if(learnIndices.empty() || tallIndices.empty())
    {
        cout << "teainTest: indicesArray empty, return" << endl;
        return;
    }
    learnNetIndices(learnIndices);
    tallNetIndices(tallIndices);
    cout << "half-half classification - ";
    averageClassification();
}

void Net::trainTestClassification(const QString & trainTemplate,
                                  const QString & testTemplate)
{
    std::vector<int> learnIndices;
    std::vector<int> tallIndices;
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        if(fileNames[i].contains(trainTemplate))
        {
            learnIndices.push_back(i);
        }
        else if(fileNames[i].contains(testTemplate))
        {
            tallIndices.push_back(i);
        }
    }
    if(learnIndices.empty() || tallIndices.empty())
    {
        cout << "teainTest: indicesArray empty, return" << endl;
        return;
    }
    learnNetIndices(learnIndices);
    tallNetIndices(tallIndices);
    cout << "train-test classification - ";
    averageClassification();
}

void Net::leaveOneOut()
{
    std::vector<int> learnIndices;
    uint i = 0;
    adjustLearnRate(ui->lowLimitSpinBox->value(),
                    ui->highLimitSpinBox->value()); /// ~40-60
    while(i < dataMatrix.rows())
    {
        cout << i + 1;
        cout << " "; cout.flush();

        /// iota ?
        learnIndices.clear();
#if 0
        learnIndices.resize(dataMatrix.rows() - 1);
        std::iota(std::begin(learnIndices),
                  std::begin(learnIndices) + i,
                  0);
        std::iota(std::begin(learnIndices) + i,
                  std::end(learnIndices),
                  i + 1);
#else
        for(uint j = 0; j < dataMatrix.rows(); ++j)
        {
            if(j == i) continue;
            learnIndices.push_back(j);
        }
#endif
        learnNetIndices(learnIndices);
        tallNetIndices({i});

        /// not so fast
        /// what with softmax/logistic ?
//        if(tallCleanFlag && epoch < ui->lowLimitSpinBox->value() && activation == logistic)
//        {
//            adjustLearnRate(ui->lowLimitSpinBox->value(),
//                            ui->highLimitSpinBox->value());
//        }
        ++i;
    }
    cout << endl;
    cout << "N-fold cross-validation:" << endl;
    averageClassification();
}

void Net::learnNet(const bool resetFlag)
{
    std::vector<int> mixNum(dataMatrix.rows());
    std::iota(mixNum.begin(), mixNum.end(), 0);
    learnNetIndices(mixNum, resetFlag);
}

void Net::tallNet()
{
    std::vector<int> indices;
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        indices.push_back(i);
    }
    tallNetIndices(indices);
}

void Net::learnNetIndices(std::vector<int> mixNum,
                          const bool resetFlag)
{
    QTime myTime;
    myTime.start();

    if(resetFlag)
    {
//        cout << "reset" << endl;
        reset();
    }


    const int numOfLayers = dimensionality.size();
    std::vector<std::valarray<double>> deltaWeights(numOfLayers);
    std::vector<std::valarray<double>> output(numOfLayers);
    for(int i = 0; i < numOfLayers; ++i)
    {
        deltaWeights[i].resize(dimensionality[i]); // fill zeros
        output[i].resize(dimensionality[i] + 1); // for biases
    }

    const double critError = ui->critErrorDoubleSpinBox->value();
    const double temp = ui->tempBox->value();
    const double learnRate = ui->learnRateBox->value();
    const bool deltaFlag = ui->deltaRadioButton->isChecked();
//    const double momentum = ui->momentumDoubleSpinBox->value(); //unused yet
    double currentError = critError + 0.1;

    int type = 0;



    /// edit due to Indices
    vector <double> normCoeff;
    const double helpMin = *std::min_element(classCount.begin(),
                                             classCount.end());
//    const double helpMin = std::accumulate(classCount.begin(),
//                                           classCount.end(),
//                                           0.);


    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        normCoeff.push_back(helpMin / classCount[i]);
    }


    if(ui->deepBeliefRadioButton->isChecked())
    {
//        prelearnDeepBelief();
    }


    epoch = 0;
    while(currentError > critError && epoch < ui->epochSpinBox->value())
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        currentError = 0.0;
        // mix the sequence of input vectors
        std::shuffle(mixNum.begin(),
                     mixNum.end(),
                     std::default_random_engine(seed));

        for(const int & index : mixNum)
        {
            // add data
            std::copy(begin(dataMatrix[index]),
                      end(dataMatrix[index]),
                      begin(output[0]));


            // add bias
            output[0][output[0].size() - 1] = 1.;
            type = types[index];

            //obtain outputs
            for(int i = 1; i < numOfLayers; ++i)
            {
                for(int j = 0; j < dimensionality[i]; ++j)
                {
                    output[i][j] = prod(weight[i - 1][j], output[i-1]); // bias included
                }
//                output[i] = smallLib::softmax(output[i]);
                output[i] = activation(output[i]);

                output[i][ dimensionality[i] ] = 1.; //bias, unused for the highest layer
            }

            //error in the last layer
            {
                double err = 0.;
                for(int j = 0; j < dimensionality.back(); ++j)
                {
                    err += pow((output.back()[j] - int(type == j) ), 2.);
                }
                err = sqrt(err);
                if(errType == errorNetType::SME)
                {
                    currentError += err;
                }
                else if(errType == errorNetType::maxDist)
                {
                    currentError = max(err, currentError);
                }
            }
#if 0
            /// check weight
            if(!deltaFlag) /// enum !
            {
                //count deltaweights (used for backprop only)
                //for the last layer
                for(int j = 0; j < dimensionality[numOfLayers-1]; ++j)
                {
                    deltaWeights[numOfLayers-1][j] = -1. / temp
                                                     * output[numOfLayers-1][j]
                                                     * (1. - output[numOfLayers-1][j])
                            * ((type == j) - output[numOfLayers-1][j]); //~0.1
                }

                //for the other layers, besides the input one, upside->down
                for(int i = numOfLayers - 2; i >= 1; --i)
                {
                    for(int j = 0; j < dimensionality[i] + 1; ++j) //+1 for bias
                    {
                        deltaWeights[i][j] = 0.;
                        for(int k = 0; k < dimensionality[i + 1]; ++k) //connected to the higher-layer
                        {
                            deltaWeights[i][j] += deltaWeights[i + 1][k] * weight[i][j][k];
                        }
                        deltaWeights[i][j] *= 1. / temp
                                              * output[i][j]
                                              * (1. - output[i][j]);
                    }
                }
            }
#endif



            if(deltaFlag)
            {
                // numOfLayers = 2 and i == 0 in this case
                // simplified

                for(int j = 0; j < def::numOfClasses(); ++j)
                {
                    weight[0][j] += output[0]
                            * (learnRate * normCoeff[type]
                               * ((type == j) - output[1][j])
//                            * (output[1][j] * (1. - output[1][j])) * 6. // derivative
                            );
                }

            }
#if 0
            else /// backprop check weight
            {

                // count new weights using deltas
                for(int i = 0; i < numOfLayers - 1; ++i)
                {
                    for(int j = 0; j < dimensionality[i] + 1; ++j) // +1 for bias? 01.12.2014
                    {
                        for(int k = 0; k < dimensionality[i + 1]; ++k)
                        {
                            weight[i][j][k] -= learnRate
                                               * normCoeff[type]
                                               * output[i][j]
                                               * deltaWeights[i + 1][k];
                        }
                    }
                }
            }
#endif
        }
        ++epoch;
        //count error
        if(errType == errorNetType::SME)
        {
            currentError /= mixNum.size();
        }
        this->ui->currentErrorDoubleSpinBox->setValue(currentError);

//        cout << "epoch = " << epoch << "\terror = " << doubleRound(currentError, 4) << endl;
    }
    cout << "epoch = " << epoch << "\terror = " << doubleRound(currentError, 4) << "\ttime elapsed = " << myTime.elapsed()/1000. << " sec"  << endl;

//    writeWts();
}


void Net::tallNetIndices(const vector<int> & indices)
{
    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash + "badFiles.txt");
    matrix localConfusionMatrix(def::numOfClasses(), def::numOfClasses());


    ofstream badFilesStr;
    badFilesStr.open(helpString.toStdString(), ios_base::app);
    for(uint i = 0; i < indices.size(); ++i)
    {
        const int outClass = classifyDatum(indices[i]).first;
        if(types[ indices[i] ] != outClass )
        {
            badFilesStr << fileNames[ indices[i] ] << endl;
            if(tallCleanFlag)
            {
                if(Source == source::reals)
                {
                    QFile::remove(def::dir->absolutePath()
                                  + slash + "SpectraSmooth"
                                  + slash + fileNames[ indices[i] ]);
                }
                else if(Source == source::winds)
                {
                    QFile::remove(def::dir->absolutePath()
                                  + slash + "SpectraSmooth"
                                  + slash + "windows"
                                  + slash + fileNames[ indices[i] ]);
                }
                eraseDatum(indices[i]);
            }

        }
        localConfusionMatrix[ types[ indices[i] ] ][ outClass ] += 1.;
        confusionMatrix[ types[ indices[i] ] ][ outClass ] += 1.;
    }
    badFilesStr.close();


    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash + "log.txt");
    ofstream logStream;
    logStream.open(helpString.toStdString(), ios_base::app);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        const double num = localConfusionMatrix[i].sum();
        if(num != 0.)
        {
            logStream << doubleRound( localConfusionMatrix[i][i] * 100. / num, 2) << '\t';
        }
        else
        {
            // no errors if there werent such files - N-fold only
            logStream << "pew" << '\t';
        }
    }

    double corrSum = 0.;
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        corrSum += localConfusionMatrix[i][i];
    }
    averageAccuracy = corrSum / indices.size() * 100.;

    logStream << doubleRound(averageAccuracy, 2) << endl;
    logStream.close();
}

void Net::learnLDAIndices(const std::vector<int> & indices)
{
    const int numCl = def::numOfClasses();


    for(int i = 0; i < numCl; ++i)
    {
        /// fill learning submatrix
        matrix oneClass{};
        for(int ind : indices)
        {
            if(types[ind] == i)
            {
                oneClass.push_back(dataMatrix[ind]);
            }
        }
        centers[i] = oneClass.averageRow();
        covMat[i] = matrix::transpose(oneClass) * oneClass;
        covMat[i].invert(&(dets[i]));
    }
}
void Net::tallLDAIndices(const std::vector<int> & indices)
{
    const int numCl = def::numOfClasses();

    matrix localConfusionMatrix(numCl, numCl);
    lineType output(numCl);
    int res;
    for(int ind : indices)
    {
        for(int i = 0; i < numCl; ++i)
        {
            lineType a = (dataMatrix[ind] - centers[i]);
            matrix m1(a, 'r');
            matrix m2(a, 'c');
            output[i] = - (m1 * covMat[i] * m2)[0][0] - log(dets[i]);
        }
        res = indexOfMax(output);

        localConfusionMatrix[types[ind]][res] += 1.;
    }
    averageClassification(&localConfusionMatrix);
}



std::pair<int, double> Net::classifyDatum(const int & vecNum)
{
    const int type = types[vecNum]; // true class
    const int numOfLayers = dimensionality.size();
    const double temp = ui->tempBox->value();

    std::vector<std::valarray<double>> output(numOfLayers);
    output[0].resize(dimensionality[0] + 1); // +1 for biases

    std::copy(begin(dataMatrix[vecNum]),
              end(dataMatrix[vecNum]),
              begin(output[0]));
    output[0][output[0].size() - 1] = 1.; //bias

    for(int i = 1; i < numOfLayers; ++i)
    {
        output[i].resize(dimensionality[i] + 1);
        for(int j = 0; j < dimensionality[i]; ++j) //higher level, w/o bias
        {
            output[i][j] = prod(weight[i-1][j], output[i-1]); // bias included
        }
//        output[i] = smallLib::softmax(output[i]);
        output[i] = activation(output[i]);
        output[i][ dimensionality[i] ] = 1.; //bias, unused for the highest layer
    }

    resizeValar(output.back(), def::numOfClasses());
    int outClass = indexOfMax(output.back());


    /// effect on successive procedure
    double res = 0.;
    for(int j = 0; j < dimensionality.back(); ++j)
    {
        res += pow((output.back()[j] - int(type == j) ), 2.);
    }
    res = sqrt(res);

#if 1
    /// cout results
    std::ofstream resFile;
    resFile.open((def::dir->absolutePath() +
                  slash + "class.txt").toStdString(),
                 ios_base::app);

    auto tmp = std::cout.rdbuf();
    cout.rdbuf(resFile.rdbuf());

    cout << "type = " << type << '\t' << "(";
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        cout << doubleRound(output[numOfLayers - 1][i], 3) << '\t';
    }
    cout << ") " << ((type == outClass) ? "+ " : "- ") << fileNames[vecNum] << endl;
    cout.rdbuf(tmp);

    resFile.close();
#endif

    return std::make_pair(outClass,
                          res);


    // more general
//    return std::distance(output.back().begin(),
//                         std::max_element(output.back().begin(),
//                                          output.back().end()));

//    for(int k = 0; k < dimensionality[numOfLayers - 1]; ++k)
//    {
//        if(k != type && output[numOfLayers - 1] [k] >= output[numOfLayers - 1] [type])
//        {
//            return false;
//        }
//    }
//    return true;
}


void Net::crossSVM(std::pair<std::vector<int>, std::vector<int>> sets)
{
    QString helpString;
    helpString = def::dir->absolutePath()
                 + slash + "PA"
                 + slash + "svmAll";
    ofstream outStream(helpString.toStdString());
    for(int j = 0; j < 2; ++j)
    {
//        helpString = def::dir->absolutePath()
//                     + slash + "PA"
//                     + slash + "svm" + QString::number(j+1);
//        ofstream outStream(helpString.toStdString());

        std::vector<int> & currentSet = (j == 0) ? sets.first : sets.second;
        for(uint i = 0; i < currentSet.size(); ++i)
        {
            outStream << myLib::typeOfFileName(fileNames[currentSet[i]]) << ' ';
            for(uint l = 0; l < dataMatrix.cols(); ++l)
            {
                outStream << l << ':' << doubleRound(dataMatrix[currentSet[i]][l], 4) << ' ';
            }
            outStream << endl;
        }
//        outStream.close();
    }
    outStream.close();


    helpString = "cd "
                 + def::dir->absolutePath() + slash + "PA"
                 + " && svm-train -q "
                 + " -v " + QString::number(ui->foldSpinBox->value())
                 + " -t " + QString::number(ui->svmKernelSpinBox->value())
                 + " svmAll >> output1";
    system(helpString.toStdString().c_str());
return;




    helpString = def::dir->absolutePath()
                 + slash + "PA"
                 + slash + "svmCommands";
//    ofstream outStr(helpString.toStdString());
    helpString = "cd "
                 + def::dir->absolutePath() + slash + "PA"
                 + " && svm-train -q -t " + QString::number(ui->svmKernelSpinBox->value())
                 + " svm1"
                 + " && svm-predict svm2 svm1.model output >> output1";
//    outStr << helpString << endl << endl;
    system(helpString.toStdString().c_str());

    helpString = "cd "
                 + def::dir->absolutePath() + slash + "PA"
                 + " && svm-train -q -t " + QString::number(ui->svmKernelSpinBox->value())
                 + " svm2"
                 + " && svm-predict svm1 svm2.model output >> output1";
//    outStr << helpString << endl << endl;
    system(helpString.toStdString().c_str());
//    outStr.close();
}

void Net::SVM()
{
    /// create/clear PA/output1
    QString helpString = def::dir->absolutePath()
                         + slash + "PA"
                         + slash + "output1";
    FILE * out = fopen(QDir::toNativeSeparators(helpString), "w");
    fclose(out);



    const int numOfPairs = ui->numOfPairsBox->value();
    const int fold = ui->foldSpinBox->value();

    std::vector<std::vector<int>> arr;
    arr.resize(def::numOfClasses(), {});
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        arr[ types[i] ].push_back(i);
    }
    cout << "SVM: cross (max " << numOfPairs << "):" << endl;

    for(int i = 0; i < numOfPairs; ++i)
    {
        cout << i + 1;
        cout << " "; cout.flush();

        // mix arr for one "pair"-iteration
        for(int i = 0; i < def::numOfClasses(); ++i)
        {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(arr[i].begin(),
                         arr[i].end(),
                         std::default_random_engine(seed));
        }

        /// new with indices
        for(int numFold = 0; numFold < fold; ++numFold)
        {
            auto sets = makeIndicesSetsCross(arr, numFold);
            crossSVM(sets);
        }

        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = 0;
            return;
        }
    }

    double helpDouble, average = 0.;
    helpString = def::dir->absolutePath() + slash + "PA" + slash + "output1";
    QFile file(helpString);
    if(!file.open(QIODevice::ReadOnly)) return;

    int lines = 0;
    while(!file.atEnd())
    {
        helpString = file.readLine();
        if(!helpString.contains(QRegExp("[% = ]"))) break;
        helpString = helpString.split(QRegExp("[% = ]"),
                                      QString::SkipEmptyParts)[1]; //generality [1]
        helpDouble = helpString.toDouble();
        average += helpDouble;
        ++lines;
    }
    average /= lines;
    cout << average << endl;
    file.close();

    ofstream outStr;
    helpString = QDir::toNativeSeparators(def::dir->absolutePath() + slash + "results.txt");
    outStr.open(helpString.toStdString(), ios_base::app);
    outStr << "\nSVM\t";
    outStr << doubleRound(average, 1) << " %" << endl;
    outStr.close();
}
