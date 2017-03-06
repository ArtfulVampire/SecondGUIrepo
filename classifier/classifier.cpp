#include "classifier.h"
//using namespace myLib;



Classifier::Classifier()
{
	myData = new ClassifierData();
	resultsPath = def::dir->absolutePath() + slash + "results.txt";
	workDir = def::dir->absolutePath() + slash + "Help" + slash + "PA";
	/// by myData
	confusionMatrix = matrix(myData->getNumOfCl(), myData->getNumOfCl(), 0.);
	outputLayer = std::valarray<double>(0., myData->getNumOfCl());
#if INERTIA
	fbVal = std::valarray<double>(0., myData->getNumOfCl());
#endif
}

void Classifier::setClassifierData(ClassifierData & in)
{
	if(&in != myData)
	{
		delete myData;
		myData = &in;

		confusionMatrix = matrix(myData->getNumOfCl(), myData->getNumOfCl(), 0.);
		outputLayer = std::valarray<double>(0., myData->getNumOfCl());

#if INERTIA
		fbVal = std::valarray<double>(0., myData->getNumOfCl());
#endif

		this->adjustToNewData();
	}
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
	if(this->testCleanFlag && (predType != myData->getTypes()[vecNum]))
    {
		QFile::remove(filesPath + slash + myData->getFileNames()[vecNum]);
    }
}

/// TEMPORARY, will be virtual=0
void Classifier::classifyDatum1(const uint & vecNum)
{
	/// ALWAYS TRUE WITH ZERO ERROR
	this->confusionMatrix[myData->getTypes()[vecNum]][myData->getTypes()[vecNum]] += 1.;
	outputLayer = clLib::oneHot(myData->getNumOfCl(),
								myData->getTypes()[vecNum]);
}

std::pair<uint, double> Classifier::classifyDatum(const uint & vecNum)
{
	this->classifyDatum1(vecNum); /// sometimes is not necessary, but won't be worse
	return std::make_pair(myLib::indexOfMax(outputLayer),
						  clLib::countError(outputLayer, myData->getTypes()[vecNum]));
}

std::pair<uint, double> Classifier::classifyDatumLast()
{
	classifyDatumLast1();

#if INERTIA
	return std::make_pair(myLib::indexOfMax(fbVal),
						  clLib::countError(fbVal, myData->getTypes().back()));
#else
	return std::make_pair(myLib::indexOfMax(outputLayer),
						  clLib::countError(outputLayer, myData->getTypes().back()));
#endif
}

void Classifier::classifyDatumLast1()
{
#if INERTIA
	classifyDatum1(myData->getData().rows() - 1);
	if(curType != myData->getTypes().back())
	{
		fbVal = 0.;
		curType = myData->getTypes().back();
	}
//	fbVal *= inertiaCoef;
	fbVal *= def::inertiaCoef;
	fbVal += outputLayer;
#else
	classifyDatum1(myData->getData().rows() - 1);
#endif
}

void Classifier::printResult(const QString & fileName, uint predType, uint vecNum)
{
    std::ofstream outStr;
	outStr.open((workDir + slash + fileName).toStdString()
				, std::ios_base::app
				);


    QString pew;

	if(predType == myData->getTypes()[vecNum])
    {
        pew = "";
    }
    else
    {
		pew = QString::number(vecNum) + "\n";

		outStr << vecNum+2 << ":\ttrue = " << myData->getTypes()[vecNum] << "\tpred = " << predType << "\n";
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

void Classifier::test(const std::vector<uint> & indices)
{
    for(int ind : indices)
	{
		classifyDatum1(ind); /// confusionMatrix inside
    }
}

void Classifier::learnAll()
{
	std::vector<uint> ind(myData->getData().rows());
	std::iota(std::begin(ind), std::end(ind), 0);
    learn(ind);
}

void Classifier::testAll()
{
	std::vector<uint> ind(myData->getData().rows());
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
//	std::cout << typeString.toStdString() << std::endl;
}


avType Classifier::averageClassification()
{
    std::ofstream res;
	/// for successive
	resultsPath =
//			def::dir->absolutePath()
			"/media/Files/Dropbox/Successive/"
				  + slash + "results"
				  + "_" + QString::number(suc::numGoodNewLimit)
				  + "_" + QString::number(suc::learnSetStay)
				  + "_" + QString::number(suc::decayRate)
				  + ".txt";
    res.open(resultsPath.toStdString(), std::ios_base::app);

	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
        const double num = confusionMatrix[i].sum();
        if(num != 0.)
        {
            res << smLib::doubleRound(confusionMatrix[i][i] * 100. / num, 2) << '\t';
        }
        else
        {
            res << "pew" << '\t';
        }
    }

    // count averages, kappas
    double corrSum = 0.;
    double wholeNum = 0.;

	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
        corrSum += confusionMatrix[i][i];
        wholeNum += confusionMatrix[i].sum();
    }
    averageAccuracy = corrSum * 100. / wholeNum;

    // kappa
    double pE = 0.; // for Cohen's kappa
    const double S = confusionMatrix.sum();
	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
        pE += (confusionMatrix[i].sum() * confusionMatrix.getCol(i).sum()) / pow(S, 2);
    }
    kappa = 1. - (1. - corrSum / wholeNum) / (1. - pE);


    res << smLib::doubleRound(averageAccuracy, 2) << '\t';
    res << smLib::doubleRound(kappa, 3) << '\t';
	res << def::ExpName.toStdString();
	res << std::endl;
    res.close();


	/// std::cout
#if 1
    confusionMatrix.print();
	std::cout << "average accuracy = "
			  << smLib::doubleRound(averageAccuracy, 2) << '\t';
	std::cout << "kappa = " << kappa << '\t';
	std::cout << std::endl << std::endl;
#endif

    confusionMatrix.fill(0.);

    /// generality avType
    return std::make_pair(averageAccuracy, kappa);
}

