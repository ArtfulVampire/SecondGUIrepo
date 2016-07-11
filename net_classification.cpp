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


void Net::autoClassification()
{
    QTime myTime;
    myTime.start();

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
    cout <<  "AutoClassOnData: time elapsed = " << myTime.elapsed()/1000. << " sec" << endl;
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


/// on pca only
void Net::leaveOneOut()
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
        }
        outStr.close();
    }
}

void Net::leaveOneOutClassification()
{
    std::vector<int> learnIndices;
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        cout << i + 1 << " "; cout.flush();

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
    cout << endl;
    cout << "N-fold cross-validation:" << endl;
    myClassifier->averageClassification();
}

void Net::crossClassification()
{
    const int numOfPairs = ui->numOfPairsBox->value();
    const int fold = ui->foldSpinBox->value();

    std::vector<std::vector<int>> arr; // [class][index]
    arr.resize(def::numOfClasses());
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        arr[ types[i] ].push_back(i);
    }
    cout << "Net: autoclass (max " << numOfPairs << "):" << endl;

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
    cout << "cross classification:" << endl;
    myClassifier->averageClassification();
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
    std::vector<int> learnIndices;
    std::vector<int> tallIndices;

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
    cout << "half-half classification:" << endl;
    myClassifier->averageClassification();
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
        cout << "Net::trainTestClassification: indicesArray empty, return" << endl;
        return;
    }

    myClassifier->learn(learnIndices);
    myClassifier->test(tallIndices);
    cout << "train-test classification:" << endl;
    myClassifier->averageClassification();

}
