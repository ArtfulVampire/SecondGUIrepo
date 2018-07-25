#include <classifier/classifier.h>

using namespace myOut;

SVM::SVM() : Classifier()
{
    kernelNum = 0;
    myType = ModelType::SVM;
    typeString = "SVM";
}

void SVM::learn(std::vector<uint> & indices)
{
	makeFile(indices, learnFileName);
    QString helpString = "cd "
                         + workDir
                         + " && svm-train "
                         + " -s " + nm(svmType)
					 #if 0
						 + " -v " + fold
					 #endif
                         + " -t " + nm(kernelNum) + " -q "
                         + learnFileName;
	system(helpString.toStdString().c_str());
}

void SVM::classifyDatum1(uint vecNum)
{
    /// to write!
    const QString fileName = "oneVec";
	makeFile({vecNum}, fileName);
    QString helpString = "cd "
                         + workDir
                         + " && svm-predict "
                         + fileName + " "
                         + learnFileName + ".model "
                         + outputFileName + " >> /dev/null";
	system(helpString.toStdString().c_str());

    uint outClass;
    std::ifstream inStr;
	inStr.open((workDir + "/" + outputFileName).toStdString());
    inStr >> outClass;
    inStr.close();

	outputLayer = clLib::oneHot(myClassData->getNumOfCl(), outClass);
}


void SVM::setKernelNum(int inNum) /// 0-4
{
    kernelNum = inNum;
}

void SVM::setSvmType(int in) /// 0-4
{
    svmType = in;
}

void SVM::makeFile(const std::vector<uint> & indices,
                   const QString & fileName)
{
	QString helpString = workDir + "/" + fileName;
    /// create/clear
    std::ofstream outStream(helpString.toStdString());

    for(int ind : indices)
    {
		outStream << myClassData->getTypes()[ind] << ' ';
		for(int l = 0; l < myClassData->getData().cols(); ++l)
        {
            outStream << l + 1 << ':'
					  << smLib::doubleRound(myClassData->getData()[ind][l], 4) << ' ';
        }
		outStream << std::endl;
    }
    outStream.close();
}

