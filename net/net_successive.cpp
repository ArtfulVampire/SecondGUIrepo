#include "net.h"
using namespace std;
using namespace myLib;

void Net::successiveProcessing()
{
    cout << "successive: started" << endl;



    const QString trainMarker = "_train";
    const QString testMarker = "_test";

    std::vector<uint> eraseIndices{};
    numGoodNew = 0;

    /// check for no test items
    /// use ExpName
    QString helpString = def::dir->absolutePath()
                         + slash + "SpectraSmooth"
                         + slash + "PCA";
    loadData(helpString, {def::ExpName.left(3) + "*" + trainMarker + "*"});

    cout << "successive: data loaded" << endl;

    /// reduce learning set to (NumClasses * suc::learnSetStay)
    std::valarray<double> count = classCount;
    for(uint i = 0; i < dataMatrix.rows(); ++i)
    {
        if(count[ types[i] ] > suc::learnSetStay)
        {
            eraseIndices.push_back(i);
            count[ types[i] ] -= 1.;
        }
    }
    eraseData(eraseIndices);
    eraseIndices.clear();

    cout << "successive: data cleaned" << endl;

    /// consts - set prelearn
    setErrCrit(0.05);
    setLrate(0.05);

    myClassifier->learnAll(); /// get initial weights on train set

    /// consts - set postlearn
    setErrCrit(0.01);
    setLrate(0.01);

    cout << "successive: initial learn done" << endl;

    helpString = def::dir->absolutePath()
                 + slash + "Help"
                 + slash + "ica"
                 + slash + def::ExpName.left(3) + "_train"
                 + "_pcaMat.txt";
    readMatrixFile(helpString, pcaMat);

    helpString = def::dir->absolutePath()
                 + slash + "SpectraSmooth"
                 + slash + "windows";
    QStringList leest = QDir(helpString).entryList(
    {def::ExpName.left(3) + "*" + testMarker + "*"}); /// special generality

    lineType tempArr;
    int type = -1;

    /// here to remember old averageDatum and sigmaVector

    /// temp for test
    int count2 = 0;
    for(const QString & fileNam : leest)
    {
        readFileInLine(helpString + slash + fileNam,
                       tempArr);
        type = typeOfFileName(fileNam);
        successiveLearning(tempArr, type, fileNam);
        ++count2; if(count2 == 30) break;
    }
    cout << "successive: all done" << endl;
    myClassifier->averageClassification();
}

void Net::successivePreclean(const QString & spectraPath)
{
    QStringList leest;
    makeFullFileList(spectraPath, leest, {"*train*"});
    // clean from first 2 winds from each realisation
    cout << "clean first 2 winds" << endl;

    for(const QString & str : leest)
    {
        if(str.contains(QRegExp(".0[0-1]$"))) /// change to 0-x for x first windows to delete
        {
            QFile::remove(spectraPath + slash + str);
        }
    }

    // clean by learnSetStay
    cout << "clean by learnSetStay" << endl;
    std::vector<QStringList> leest2;
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

    myClassifier->setTestCleanFlag(true);
    for(int i = 0; i < 0; ++i)
    {
        autoClassification(spectraPath);
        if(myClassifier->averageClassification().first == 100.) break;
    }
    myClassifier->setTestCleanFlag(false);
}

/// on one incoming vector
void Net::successiveLearning(const std::valarray<double> & newSpectre,
                             const int newType,
                             const QString & newFileName)
{
    /// consider loaded wts
    /// dataMatrix is learning matrix

    lineType newData = (newSpectre - averageDatum) / (sigmaVector * loadDataNorm);

    /// apply pca
    cout << newData.size() << "\t"
         << pcaMat.rows() << "\t"
         << pcaMat.cols() << endl;
    newData = newData * pcaMat;
    for(int i = 0; i < 5; ++i)
    {
        cout << newData[i] << endl;
    }
    exit(0);

    cout << newData.size() << endl;



    pushBackDatum(newData, newType, newFileName);

    const std::pair<int, double> outType = myClassifier->classifyDatum(dataMatrix.rows() - 1); // take the last
    /// adding into confusionMatrix
    myClassifier->confMatInc(newType, outType.first);

    if(outType.first == newType)
    {
        /// if accurate classification
        if(outType.second < suc::errorThreshold)
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
        myClassifier->successiveRelearn();
        numGoodNew = 0;
    }
}
