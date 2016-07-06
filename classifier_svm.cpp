#include "classifier.h"

SVM::SVM()
{
    workDir = def::dir->absolutePath() + myLib::slash + "PA";
    kernelNum = 0;
    fold = 5;
}

void SVM::learn(std::vector<int> & indices)
{
    makeFile(indices, learnFileName);
    QString helpString = "cd "
                         + workDir
                         + " && svm-train -q "
//                         + " -v " + fold
                         + " -t " + kernelNum
                         + learnFileName
                         + " output >> " + resultsPath;
    system(helpString.toStdString().c_str());
}

void SVM::test(const std::vector<int> & indices)
{
    makeFile(indices, testFileName);
    QString helpString = "cd "
                         + workDir
                         + " && svm-predict -q "
                         + testFileName + " "
                         + learnFileName + ".model "
                         + " output >> " + resultsPath;
    system(helpString.toStdString().c_str());
}

std::pair<int, double> SVM::classifyDatum(const int & vecNum)
{
    /// to write!
}


void SVM::setKernelNum(int inNum)
{
    kernelNum = inNum;
}

void SVM::setNumPairs(int inNum)
{
    numOfPairs = inNum;
}

void SVM::setFold(int inFold)
{
    fold = inFold;
}

void SVM::makeFile(const std::vector<int> & indices,
                   const QString & fileName)
{
    QString helpString = workDir + myLib::slash + fileName;
    /// create/clear
    std::ofstream outStream(helpString.toStdString());

    for(int ind : indices)
    {
        outStream << (*types)[ind] << ' ';
        for(uint l = 0; l < (*dataMatrix).cols(); ++l)
        {
            outStream << l << ':'
                      << smallLib::doubleRound((*dataMatrix)[ind][l], 4) << ' ';
        }
        outStream << endl;

    }
    outStream.close();
}

