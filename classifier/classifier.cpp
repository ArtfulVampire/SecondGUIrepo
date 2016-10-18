#include "classifier.h"
//using namespace myLib;



Classifier::Classifier()
{
#if OLD_DATA
	myData.getNumOfCl() = def::numOfClasses();
	confusionMatrix = matrix(myData.getNumOfCl(), myData.getNumOfCl(), 0.);
    resultsPath = def::dir->absolutePath() + myLib::slash + "results.txt";
    workDir = def::dir->absolutePath() + myLib::slash + "PA";
    apriori = std::valarray<double>(1./3., 3);
#else

#endif


}

Classifier::~Classifier()
{
}


void Classifier::setTestCleanFlag(bool inFlag)
{
    this->testCleanFlag = inFlag;
}

void Classifier::deleteFile(uint vecNum, uint predType)
{
	if(this->testCleanFlag && (predType != myData.getTypes()[vecNum]))
    {
		QFile::remove(filesPath + myLib::slash + myData.getFileNames()[vecNum]);
    }
}

void Classifier::printResult(const QString & fileName, uint predType, uint vecNum)
{
    std::ofstream outStr;
	outStr.open((workDir + myLib::slash + fileName).toStdString()
				, std::ios_base::app
				);


    QString pew;

	if(predType == myData.getTypes()[vecNum])

    {
        pew = "";
    }
    else
    {
		pew = QString::number(vecNum) + "\n";

		outStr << vecNum+2 << ":\ttrue = " << myData.getTypes()[vecNum] << "\tpred = " << predType << "\n";
    }

//    outStr << pew.toStdString();

    outStr.close();
}
#if OLD_DATA
void Classifier::setData(matrix & inMat)
{
    this->dataMatrix = &inMat;
}

void Classifier::setTypes(std::vector<uint> & inTypes)
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
#if 1
    /// w/o normalization
    if(in.size() == 0)
    {
        apriori = *classCount;
    }
    else
    {
        apriori = in;
	}
#else
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
	this->apriori = *pew / std::accumulate(std::begin(*pew),
										   std::end(*pew),
										   0.);
#endif
}

#endif // OLD_DATA

#if !CLASS_TEST_VIRTUAL
void Classifier::test(const std::vector<uint> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
		confusionMatrix[myData.getTypes()[ind]][res.first] += 1.;
    }
}
#endif

void Classifier::learnAll()
{
	std::vector<uint> ind(myData.getData().rows());
    std::iota(std::begin(ind), std::end(ind), 0);
    learn(ind);
}

void Classifier::testAll()
{
	std::vector<uint> ind(myData.getData().rows());
    std::iota(std::begin(ind), std::end(ind), 0);
    test(ind);
}

void Classifier::successiveRelearn()
{
    this->resetFlag = false;
    learnAll(); // relearn w/o weights reset
    this->resetFlag = true;
}

void Classifier::printParams()
{
	std::cout << typeString.toStdString() << std::endl;
}


avType Classifier::averageClassification()
{
    std::ofstream res;
    res.open(resultsPath.toStdString(), std::ios_base::app);

	for(uint i = 0; i < myData.getNumOfCl(); ++i)
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

	for(uint i = 0; i < myData.getNumOfCl(); ++i)
    {
        corrSum += confusionMatrix[i][i];
        wholeNum += confusionMatrix[i].sum();
    }
    averageAccuracy = corrSum * 100. / wholeNum;

    // kappa
    double pE = 0.; // for Cohen's kappa
    const double S = confusionMatrix.sum();
	for(uint i = 0; i < myData.getNumOfCl(); ++i)
    {
        pE += (confusionMatrix[i].sum() * confusionMatrix.getCol(i).sum()) / pow(S, 2);
    }
    kappa = 1. - (1. - corrSum / wholeNum) / (1. - pE);


    res << smallLib::doubleRound(averageAccuracy, 2) << '\t';
    res << smallLib::doubleRound(kappa, 3) << '\t';
    myLib::operator <<(res, def::ExpName);
    res << std::endl;

    res.close();

    /// cout
#if 1
    confusionMatrix.print();
    std::cout << "average accuracy = "
              << smallLib::doubleRound(averageAccuracy, 2) << std::endl;
    std::cout << "kappa = " << kappa << std::endl << std::endl;
#endif

    confusionMatrix.fill(0.);

    /// generality avType
    return std::make_pair(averageAccuracy, kappa);
}

