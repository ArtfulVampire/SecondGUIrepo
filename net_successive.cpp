#include "net.h"
#include "ui_net.h"
using namespace std;
using namespace myLib;
using namespace smallLib;

///successive part


std::vector<int> exIndices{};
int numGoodNew;

void Net::successiveProcessing()
{
    QString helpString = def::dir->absolutePath()
                         + slash + "SpectraSmooth"
                         + slash + "windows";

    const QString trainMarker = "_train";
    const QString testMarker = "_test";
//    const QString testMarker = "_3.";

    std::vector<int> eraseIndices{};

    numGoodNew = 0;
    confusionMatrix.fill(0.);
    exIndices.clear();

    /// check for no test items
    loadData(helpString, {"*" + trainMarker + "*"});
//    for(int i = 0; i < dataMatrix.rows(); ++i)
//    {
//        if(fileNames[i].contains(testMarker))
//        {
//            eraseIndices.push_back(i);
//        }
//    }
//    eraseData(eraseIndices);
//    eraseIndices.clear();

    /// reduce learning set to (NumClasses * suc::learnSetStay)
    std::vector<double> count = classCount;
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        if(count[ types[i] ] > suc::learnSetStay)
        {
            eraseIndices.push_back(i);
            count[ types[i] ] -= 1.;
        }
    }
    eraseData(eraseIndices);
    eraseIndices.clear();



    /// consts
    setErrCrit(0.05);
    setLrate(0.05);

    learnNet(); /// get initial weights on train set

    setErrCrit(0.02);
    setLrate(0.02);


    lineType tempArr;
    int type = -1;

    QStringList leest = QDir(helpString).entryList({'*' + testMarker + '*'}); /// special generality

//    helpString = "/media/michael/Files/Data/RealTime/windows/SpectraSmooth";
//    QStringList leest = QDir(helpString).entryList(QDir::Files); /// special generality

    for(const QString & fileNam : leest)
    {
        readFileInLine(helpString + slash + fileNam,
                       tempArr);
        type = typeOfFileName(fileNam);
        successiveLearning(tempArr, type, fileNam);
    }
    averageClassification();
}

void Net::successivePreclean(const QString & spectraPath)
{
    QStringList leest;
    makeFullFileList(spectraPath, leest, {"*train*"});
    // clean from first 2 winds
    cout << "clean first 2 winds" << endl;
    for(auto str : leest)
    {
        if(str.endsWith(".00") || str.endsWith(".01"))
        {
            QFile::remove(spectraPath + slash + str);
        }
    }

    // clean by learnSetStay
    cout << "clean by learnSetStay" << endl;
    vector<QStringList> leest2;
    makeFileLists(spectraPath, leest2);

    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        auto it = leest2[j].begin();
        for(int i = 0;
            i < leest2[j].size() - suc::learnSetStay * 1.3; /// consts generality
            ++i, ++it)
        {
            QFile::remove(spectraPath + slash + (*it));
        }
    }
    Source = source::winds;
    Mode = myMode::N_fold;

    setErrCrit(0.05);

    // N-fold cleaning
    cout << "N-fold cleaning" << endl;
    tallCleanFlag = true;
    for(int i = 0; i < 0; ++i)
    {
        autoClassification(spectraPath);
        if(averageAccuracy == 100.) break;
    }
    tallCleanFlag = false;
}

void Net::successiveLearning(const lineType & newSpectre,
                             const int newType,
                             const QString & newFileName)
{
    /// consider loaded wts
    /// dataMatrix is learning matrix

    lineType newData = (newSpectre - averageDatum) / (sigmaVector * loadDataNorm);

    pushBackDatum(newData, newType, newFileName);

    const std::pair<int, double> outType = classifyDatum(dataMatrix.rows() - 1); // take the last
    confusionMatrix[newType][outType.first] += 1.;

    if(outType.first == newType)
    {
        /// if accurate classification
        if(outType.second < errorThreshold)
        {
            const int num = std::find(types.begin(),
                                      types.end(),
                                      newType)
                            - types.begin();
            eraseDatum(num);
            ++numGoodNew;
        }
        else
        {
            popBackDatum();
        }

    }
    else
    {
        popBackDatum();
    }

    if(numGoodNew == suc::numGoodNewLimit)
    {
        successiveRelearn();
        numGoodNew = 0;
    }
}

void Net::successiveRelearn()
{
    // decay weights
    const double rat = suc::decayRate;
    for(int i = 0; i < dimensionality.size() - 1; ++i)
    {
        std::for_each(weight[i].begin(),
                      weight[i].end(),
                      [rat](lineType & in)
        {
            in *= 1. - rat;
        });
    }

    learnNet(false); // relearn w/o weights reset
}

void Net::readWtsByName(const QString & fileName,
                        twovector<lineType> * wtsMatrix) //
{
    ifstream wtsStr;
    wtsStr.open(fileName.toStdString());
    if(!wtsStr.good())
    {
        cout << "readWtsByName: wtsStr is not good() " << endl;
        return;
    }
    if(wtsMatrix == nullptr)
    {
        wtsMatrix = &(this->weight);
    }
    else
    {
        (*wtsMatrix).resize(dimensionality.size() - 1);
        for(int i = 0; i < dimensionality.size() - 1; ++i)
        {
            (*wtsMatrix)[i].resize(dimensionality[i + 1]);
            for(int j = 0; j < dimensionality[i + 1]; ++j)
            {
                (*wtsMatrix)[i][j].resize(dimensionality[i] + 1);
            }
        }
    }

    for(int i = 0; i < dimensionality.size() - 1; ++i)
    {
        for(int j = 0; j < dimensionality[i + 1]; ++j)
        {
            for(int k = 0; k < dimensionality[i] + 1; ++k)
            {
                wtsStr >> (*wtsMatrix)[i][j][k];
            }
        }
    }
    wtsStr.close();
}
