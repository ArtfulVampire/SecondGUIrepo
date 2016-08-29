#include "net.h"
#include "ui_net.h"
using namespace std;
using namespace myLib;
using namespace smallLib;


void Net::autoClassification(const QString & spectraDir)
{
    QTime myTime;
    myTime.start();

#if 0
    //set random matrix - add in PaIntoMatrixByName
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        for(int j = 0; j < def::nsWOM(); ++j)
        {
            tempRandomMatrix[i][j] = (i == j); //identity matrix
//            tempRandomMatrix[i][j] = (i == j) * (5 + rand()%36) / 10.; //random diagonal
//            tempRandomMatrix[i][j] = (5. + rand()%36) / 50.; //full random
//            tempRandomMatrix[i][j] = (i == j) * (1. + (rand()%8 == 0) * (5. + rand()%35) / 5.); //random diagonal
        }
    }
#endif

    loadData(spectraDir);

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
    cout <<  "AutoClass: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
}


avType Net::autoClassification()
{
    QTime myTime;
    myTime.start();

    switch(Mode)
    {
    case myMode::k_fold:
    {
        crossClassification();
//        cout << "cross classification:" << endl;
        break;
    }
    case myMode::N_fold:
    {
        leaveOneOutClassification();
//        cout << "N-fold cross-validation:" << endl;
        break;
    }
    case myMode::train_test:
    {
        trainTestClassification();
//        cout << "train-test classification:" << endl;
        break;
    }
    case myMode::half_half:
    {
        halfHalfClassification();
//        cout << "half-half classification:" << endl;
        break;
    }
    default: {break;}
    }
//    cout <<  "AutoClassOnData: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
    return myClassifier->averageClassification();
}


void Net::autoClassificationSimple()
{
    QString helpString = def::dir->absolutePath()
                         + slash + "SpectraSmooth";

    switch(Source)
    {
    case source::winds:
    {
        helpString += slash + "windows"; break;
    }
    case source::bayes:
    {
        helpString += slash + "Bayes"; break;
    }
    case source::pca:
    {
        helpString += slash + "PCA"; break;
    }
    default: {break;}
    }

    if(!helpString.isEmpty())
    {
        autoClassification(helpString);
    }
}


void Net::leaveOneOutClassification()
{
    std::vector<uint> learnIndices;
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
//        cout << i + 1 << " "; cout.flush();

        learnIndices.clear();
        learnIndices.resize(dataMatrix.rows() - 1);
        std::iota(std::begin(learnIndices),
                  std::begin(learnIndices) + i,
                  0);
        std::iota(std::begin(learnIndices) + i,
                  std::end(learnIndices),
                  i + 1);

        myClassifier->learn(learnIndices);
        myClassifier->test({i});
    }
//    cout << endl;
}

void Net::crossClassification()
{
    const int numOfPairs = ui->numOfPairsBox->value();
    const int fold = ui->foldSpinBox->value();

    std::vector<std::vector<uint>> arr; // [class][index]
    arr.resize(def::numOfClasses());
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        arr[ types[i] ].push_back(i);
    }
    cout << "Net: crossClass (max " << numOfPairs << "):" << endl;

    for(int i = 0; i < numOfPairs; ++i)
    {
        cout << i + 1 << " "; cout.flush();

        // mix arr for one "pair"-iteration
        for(int i = 0; i < def::numOfClasses(); ++i)
        {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(arr[i].begin(),
                         arr[i].end(),
                         std::default_random_engine(seed));
        }

        for(int numFold = 0; numFold < fold; ++numFold)
        {
            auto sets = makeIndicesSetsCross(arr, numFold); // on const arr
            myClassifier->learn(sets.first);
            myClassifier->test(sets.second);
        }

        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = 0;
            return;
        }
    }
    cout << endl;
}

std::pair<std::vector<uint>,
std::vector<uint>> Net::makeIndicesSetsCross(
                       const std::vector<std::vector<uint> > & arr,
                       const int numOfFold)
{
    std::vector<uint> learnInd;
    std::vector<uint> tallInd;

    const int fold = ui->foldSpinBox->value();

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        for(int j = 0; j < classCount[i]; ++j)
        {
            if(j >= (classCount[i] * numOfFold / fold) &&
               j < (classCount[i] * (numOfFold + 1) / fold))
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

void Net::halfHalfClassification()
{
    std::vector<uint> learnIndices;
    std::vector<uint> tallIndices;

    for(uint i = 0; i < dataMatrix.rows() / 2; ++i)
    {
        learnIndices.push_back(i);
        tallIndices.push_back(i + dataMatrix.rows() / 2);
    }
    if(learnIndices.empty() || tallIndices.empty())
    {
        cout << "Net::halfHalfClassification: indicesArray empty, return" << endl;
        return;
    }
    myClassifier->learn(learnIndices);
    myClassifier->test(tallIndices);
}

void Net::trainTestClassification(const QString & trainTemplate,
                                  const QString & testTemplate)
{
    std::vector<uint> learnIndices;
    std::vector<uint> tallIndices;
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
        cout << "Net::trainTestClassification: indicesArray empty, return" << endl;
        return;
    }
    myClassifier->learn(learnIndices);
    myClassifier->test(tallIndices);
}

void Net::customF()
{

#if 0
    /// RDA
    ui->pcaNumberSpinBox->setValue(60);
    ui->traceDoubleSpinBox->setValue(1.);
//    pca();
    loadData(def::dir->absolutePath() + "/SpectraSmooth/PCA");

    /// single file _ train, RDA
    /// AAX opt 32 pcas, lambda 0.9, shrinkage 0.1
    /// AAU opt 26 pcas, lambda 1.0, shrinkage 0.5-0.7
    /// BEA opt 32 pcas, lambda 0.9-1.0, shrinkage 0.4
    /// CAA opt 34 pcas, lambda 0.6-0.7, shrinkage 0.0-0.2
    /// GAS opt 60 pcas, lambda 0.7-0.9, shrinkage 0.1-0.2
    /// SUA opt 40 pcas, lambda 0.7-0.8, shrinkage 0.3-0.5


    std::vector<std::vector<double>> results{};
    for(int i = 60; i >= 10; i -= 2)
    {
        dataMatrix.resizeCols(i);
        for(double lambda = 0.0; lambda <= 1.0; lambda += 0.1)
        {
            for(double shr = 0.0; shr <= 1.0; shr += 0.1)
            {
                setRdaLambdaSlot(lambda);
                setRdaShrinkSlot(shr);

                auto a = autoClassification();
                results.push_back(std::vector<double>{i, lambda, shr, a.first, a.second});
            }
        }
    }
    std::sort(std::begin(results), std::end(results),
              [](const std::vector<double> & in1, const std::vector<double> & in2)
    {
        return in1[3] > in2[3];
    });



    ofstream outStr;
    outStr.open((def::dir->absolutePath()
                 + slash + def::ExpName + "_"
                 + "pcaRes.txt").toStdString(), std::ios_base::app);

    for(auto vec : results)
    {
        if(vec[3] < results[0][3] - 1.5) break;
        outStr << vec << endl;
    }

    outStr.close();
#endif


#if 1
    /// ANN
//    loadData(def::dir->absolutePath() + "/SpectraSmooth/PCA");

    /// single file _ train, ANN
    std::vector<std::vector<double>> results{};
//    for(int i = 60; i >= 20; i -= 2)
//    {
//        dataMatrix.resizeCols(i);
//        for(double lrat = 0.005; lrat <= 0.01; lrat += 0.001)
        for(double lrat : {0.002, 0.005, 0.01, 0.02})
        {
            setLrate(lrat);
            auto a = autoClassification();
            results.push_back(std::vector<double>{lrat, a.first, a.second});
        }
//    }

    const int accNum = 1;
    std::sort(std::begin(results), std::end(results),
              [](const std::vector<double> & in1, const std::vector<double> & in2)
    {
        return in1[accNum] > in2[accNum];
    });

    ofstream outStr;
    outStr.open((def::dir->absolutePath()
                 + slash + def::ExpName + "_w_"
                 + "annRes.txt").toStdString(), std::ios_base::app);

    for(auto vec : results)
    {
        if(vec[accNum] < results[0][accNum] - 1.5) break;
        outStr << vec << endl;
    }

    outStr.close();
#endif


}
