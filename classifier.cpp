#include "classifier.h"
//using namespace myLib;

Classifier::Classifier()
{
    numCl = def::numOfClasses();
    confusionMatrix = matrix(numCl, numCl, 0.);
    resultsPath = def::dir->absolutePath() + myLib::slash + "results.txt";
    workDir = def::dir->absolutePath() + myLib::slash + "PA";
    apriori = std::valarray<double>(1./3., 3);
}

Classifier::~Classifier()
{
}


void Classifier::setTestCleanFlag(bool inFlag)
{
    this->testCleanFlag = inFlag;
}

void Classifier::deleteFile(int vecNum, int predType)
{
    if(this->testCleanFlag && (predType != (*types)[vecNum]))
    {
        QFile::remove(filesPath + myLib::slash + (*fileNames)[vecNum]);
    }
}

void Classifier::printResult(const QString & fileName, int predType, int vecNum)
{
    std::ofstream outStr;
    outStr.open((workDir + myLib::slash + fileName).toStdString(), std::ios_base::app);

    QString pew;
    if(predType == (*types)[vecNum])
    {
        pew = "";
    }
    else
    {
        pew = QString::number(vecNum) + "\n";
    }

    outStr << pew.toStdString();

    outStr.close();
}

void Classifier::setData(matrix & inMat)
{
    this->dataMatrix = &inMat;
}

void Classifier::setTypes(std::vector<int> & inTypes)
{
    this->types = &inTypes;
}

void Classifier::setClassCount(std::valarray<double> & inClassCount)
{
    this->classCount = &inClassCount;
}

void Classifier::setFileNames(std::vector<QString> & inFileNames)
{
    this->fileNames = &inFileNames;
}

void Classifier::setFilesPath(const QString & inPath)
{
    this->filesPath = inPath;
}

void Classifier::setApriori(const std::valarray<double> & in)
{
    /// w/o normalization
    if(in.size() == 0)
    {
        apriori = *classCount;
    }
    else
    {
        apriori = in;
    }
    return;

#if 0
    /// with normalization
    std::valarray<double> * pew;
    if(in.empty())
    {
        pew = &classCount;
    }
    else
    {
        pew = &in;
    }
    double sum = std::accumulate(std::begin(*pew),
                                 std::end(*pew),
                                 0.);
    std::for_each(std::begin(*pew),
                  std::end(*pew),
                  [sum](double & in)
    {
        in /= sum;
    }
    );
    this->apriori = sum;
#endif
}

#if !CLASS_TEST_VIRTUAL
void Classifier::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

void Classifier::learnAll()
{
    std::vector<int> ind(dataMatrix->rows());
    std::iota(std::begin(ind), std::end(ind), 0);
    learn(ind);
}

void Classifier::testAll()
{
    std::vector<int> ind(dataMatrix->rows());
    std::iota(std::begin(ind), std::end(ind), 0);
    test(ind);
}

void Classifier::successiveRelearn()
{
    this->resetFlag = false;
    learnAll(); // relearn w/o weights reset
    this->resetFlag = true;
}



avType Classifier::averageClassification()
{
    std::ofstream res;
    res.open(resultsPath.toStdString(), std::ios_base::app);

    for(uint i = 0; i < numCl; ++i)
    {
        const double num = confusionMatrix[i].sum();
        if(num != 0.)
        {
            res << smallLib::doubleRound(confusionMatrix[i][i] * 100. / num, 2) << '\t';
        }
        else
        {
            res << "pew" << '\t';
        }
    }

    // count averages, kappas
    double corrSum = 0.;
    double wholeNum = 0.;

    for(uint i = 0; i < numCl; ++i)
    {
        corrSum += confusionMatrix[i][i];
        wholeNum += confusionMatrix[i].sum();
    }
    averageAccuracy = corrSum * 100. / wholeNum;

    // kappa
    double pE = 0.; // for Cohen's kappa
    const double S = confusionMatrix.sum();
    for(uint i = 0; i < numCl; ++i)
    {
        pE += (confusionMatrix[i].sum() * confusionMatrix.getCol(i).sum()) / pow(S, 2);
    }
    kappa = 1. - (1. - corrSum / wholeNum) / (1. - pE);


    res << smallLib::doubleRound(averageAccuracy, 2) << '\t';

    /// myLib namespace
//    res << smallLib::doubleRound(kappa, 3) << '\t' << def::ExpName << std::endl;

    res << smallLib::doubleRound(kappa, 3) << '\t';
    myLib::operator <<(res, def::ExpName);
    res << std::endl;

    res.close();

    /// cout
#if 1
    confusionMatrix.print();
    std::cout << "average accuracy = "
              << smallLib::doubleRound(averageAccuracy, 2) << std::endl;
    std::cout << "kappa = " << kappa << std::endl;
#endif

    confusionMatrix.fill(0.);

    return std::make_pair(averageAccuracy, kappa);
}

