#include "classifier.h"

SVM::SVM() : Classifier()
{
    workDir = def::dir->absolutePath() + myLib::slash + "PA";
    kernelNum = 0;
    fold = 5;
    myType = ClassifierType::SVM;
}

void SVM::learn(std::vector<int> & indices)
{
    makeFile(indices, learnFileName);
//    std::cout << std::endl << "LEARN start" << std::endl << std::endl;
    QString helpString = "cd "
                         + workDir
                         + " && svm-train "
                         + " -s 1 "
//                         + " -v " + fold
                         + " -t " + QString::number(kernelNum) + " -q "
                         + learnFileName;
    system(helpString.toStdString().c_str());
//    std::cout << std::endl << "LEARN finish" << std::endl << std::endl;
}

#if CLASS_TEST_VIRTUAL
void SVM::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<int, double> SVM::classifyDatum(const int & vecNum)
{
    /// to write!
    const QString fileName = "oneVec";
    makeFile({vecNum}, fileName);
//    std::cout << std::endl << "Test start" << std::endl << std::endl;
    QString helpString = "cd "
                         + workDir
                         + " && svm-predict "
                         + fileName + " "
                         + learnFileName + ".model "
                         + outputFileName + " >> /dev/null";
    system(helpString.toStdString().c_str());
//    std::cout << std::endl << "Test finish" << std::endl << std::endl;

    int outClass;
    std::ifstream inStr;
    inStr.open((workDir + myLib::slash + outputFileName).toStdString());
    inStr >> outClass;
    inStr.close();

    printResult("SVM.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
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
        for(uint l = 0; l < dataMatrix->cols(); ++l)
        {
            outStream << l + 1 << ':'
                      << smallLib::doubleRound((*dataMatrix)[ind][l], 4) << ' ';
        }
        outStream << std::endl;
    }
    outStream.close();
}

