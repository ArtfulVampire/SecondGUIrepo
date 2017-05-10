#include <classifier/classifier.h>

Classifier::Classifier()
{
	myClassData = new ClassifierData();
	resultsPath = def::dir->absolutePath() + "/results.txt";
	workDir = def::dir->absolutePath() + "/Help/PA";
	/// by myClassData
	confusionMatrix = matrix(myClassData->getNumOfCl(), myClassData->getNumOfCl(), 0.);
	outputLayer = std::valarray<double>(0., myClassData->getNumOfCl());
#if INERTIA
	fbVal = std::valarray<double>(0., myClassData->getNumOfCl());
#endif
}

void Classifier::setClassifierData(ClassifierData & in)
{
	if(&in != myClassData)
	{
		myClassData = &in;

		confusionMatrix = matrix(myClassData->getNumOfCl(), myClassData->getNumOfCl(), 0.);
		outputLayer = std::valarray<double>(0., myClassData->getNumOfCl());

#if INERTIA
		fbVal = std::valarray<double>(0., myClassData->getNumOfCl());
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
	if(this->testCleanFlag && (predType != myClassData->getTypes()[vecNum]))
    {
		QFile::remove(filesPath + "/" + myClassData->getFileNames()[vecNum]);
    }
}

/// TEMPORARY, will be virtual=0
/// SHOULD OVERRIDE FOR ALL CLASSIFIERS
void Classifier::classifyDatum1(const uint & vecNum)
{
	/// ALWAYS TRUE WITH ZERO ERROR
	this->confusionMatrix[myClassData->getTypes()[vecNum]][myClassData->getTypes()[vecNum]] += 1.;
	outputLayer = clLib::oneHot(myClassData->getNumOfCl(),
								myClassData->getTypes()[vecNum]);
}

std::pair<uint, double> Classifier::classifyDatum(const uint & vecNum)
{
	this->classifyDatum1(vecNum); /// sometimes is not necessary, but won't be worse
	return std::make_pair(myLib::indexOfMax(outputLayer),
						  clLib::countError(outputLayer, myClassData->getTypes()[vecNum]));
}

std::pair<uint, double> Classifier::classifyDatumLast()
{
	classifyDatumLast1();

#if INERTIA
	/// myLib::indexOfMax(fbVal) or myLib::indexOfMax(outputLayer) ???
	return std::make_pair(myLib::indexOfMax(fbVal),
						  clLib::countError(fbVal, myClassData->getTypes().back()));
#else
	return std::make_pair(myLib::indexOfMax(outputLayer),
						  clLib::countError(outputLayer, myClassData->getTypes().back()));
#endif
}

void Classifier::classifyDatumLast1()
{
#if INERTIA
	classifyDatum1(myClassData->getData().rows() - 1);
	if(curType != myClassData->getTypes().back())
	{
		fbVal = 0.;
		curType = myClassData->getTypes().back();
	}
//	fbVal *= inertiaCoef;
	fbVal *= def::inertiaCoef;
	fbVal += outputLayer;
#else
	classifyDatum1(myClassData->getData().rows() - 1);
#endif
}

void Classifier::printResult(const QString & fileName, uint predType, uint vecNum)
{
    std::ofstream outStr;
	outStr.open((workDir + "/" + fileName).toStdString()
				, std::ios_base::app
				);


    QString pew;

	if(predType == myClassData->getTypes()[vecNum])
    {
        pew = "";
    }
    else
    {
		pew = QString::number(vecNum) + "\n";

		outStr << vecNum+2 << ":\ttrue = " << myClassData->getTypes()[vecNum] << "\tpred = " << predType << "\n";
    }

//    outStr << pew.toStdString();

    outStr.close();
}

void Classifier::test(const std::vector<uint> & indices)
{
    for(int ind : indices)
	{
		/// should affect confusionMatrix and outputLayer
		classifyDatum1(ind);
    }
}

void Classifier::learnAll()
{
	std::vector<uint> ind(myClassData->getData().rows());
	std::iota(std::begin(ind), std::end(ind), 0);
    learn(ind);
}

void Classifier::testAll()
{
	std::vector<uint> ind(myClassData->getData().rows());
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


Classifier::avType Classifier::averageClassification()
{
#if 01
	/// successive
	resultsPath =
			def::dir->absolutePath()
			+ "/results"
			+ "_" + QString::number(suc::numGoodNewLimit)
			+ "_" + QString::number(suc::learnSetStay)
			+ "_" + QString::number(suc::decayRate)
			+ ".txt";
#endif

	std::ofstream res;
    res.open(resultsPath.toStdString(), std::ios_base::app);

	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
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

	/// count averages, kappas
    double corrSum = 0.;
    double wholeNum = 0.;

	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
        corrSum += confusionMatrix[i][i];
        wholeNum += confusionMatrix[i].sum();
    }
    averageAccuracy = corrSum * 100. / wholeNum;

	/// kappa
	double pE = 0.;
    const double S = confusionMatrix.sum();
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
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
    confusionMatrix.print();
	std::cout << "average accuracy = "
			  << smLib::doubleRound(averageAccuracy, 2) << '\t';
	std::cout << "kappa = " << smLib::doubleRound(kappa, 3) << '\t';
	std::cout << std::endl << std::endl;

    confusionMatrix.fill(0.);

    return std::make_pair(averageAccuracy, kappa);
}

