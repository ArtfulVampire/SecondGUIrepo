#include <classifier/classifier.h>
#include <myLib/output.h>

using namespace myOut;

Classifier::Classifier()
{
	myClassData = new ClassifierData();
	resultsPath = def::dirPath() + "/results.txt";
	workDir = def::dirPath() + "/Help/PA";
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
// void Classifier::classifyDatum1(uint vecNum)
// {
//	/// ALWAYS TRUE WITH ZERO ERROR
//	this->confusionMatrix[myClassData->getTypes()[vecNum]][myClassData->getTypes()[vecNum]] += 1.;
//	outputLayer = clLib::oneHot(myClassData->getNumOfCl(),
//								myClassData->getTypes()[vecNum]);
// }

std::pair<uint, double> Classifier::classifyDatum(uint vecNum)
{
	this->classifyDatum1(vecNum);
	const int outClass = myLib::indexOfMax(outputLayer);

	if(this->testCleanFlag) /// here or inside deleteFile ?
	{
		this->deleteFile(vecNum, outClass); /// delete if wrong class
	}
	confusionMatrix[myClassData->getTypes()[vecNum]][outClass] += 1.;
	printResult(typeString + ".txt", outClass, vecNum);
	smLib::normalize(outputLayer); /// for clLib::countError
	return std::make_pair(outClass,
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
		pew = nm(vecNum) + "\n";

		outStr << vecNum+2 << ":\ttrue = " << myClassData->getTypes()[vecNum] << "\tpred = " << predType << "\n";
    }
    outStr.close();
}

void Classifier::test(const std::vector<uint> & indices)
{
    for(int ind : indices)
	{
		/// should affect confusionMatrix and outputLayer
		classifyDatum(ind);
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
//	std::cout << typeString << std::endl;
}


Classifier::avType Classifier::averageClassification()
{
#if 01
	/// successive
	resultsPath =
			def::dirPath()
			+ "/results"
			+ "_" + nm(suc::numGoodNewLimit)
			+ "_" + nm(suc::learnSetStay)
			+ "_" + nm(suc::decayRate)
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
	res << def::ExpName;
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

void Classifier::leaveOneOutClassification()
{
	const auto & dataMatrix = this->myClassData->getData();
	std::vector<uint> learnIndices;
	std::cout << "LOO: max = " << dataMatrix.rows() << std::endl;
	for(uint i = 0; i < dataMatrix.rows(); ++i)
	{
		if((i + 1) % 10 == 0) { std::cout << i + 1 << " "; std::cout.flush(); }

		learnIndices.clear();
		learnIndices.resize(dataMatrix.rows() - 1);
		std::iota(std::begin(learnIndices),
				  std::begin(learnIndices) + i,
				  0);
		std::iota(std::begin(learnIndices) + i,
				  std::end(learnIndices),
				  i + 1);

		this->learn(learnIndices);
		this->test({i});
	}
}


std::pair<std::vector<uint>,
std::vector<uint>> Classifier::makeIndicesSetsCross(
		const std::vector<std::vector<uint> > & arr,
		int numOfFolds,
		const int currentFold)
{
	const std::valarray<double> & classCount = this->myClassData->getClassCount();
	const double numOfClasses = this->myClassData->getNumOfCl();

	std::vector<uint> learnInd;
	std::vector<uint> tallInd;

	for(int i = 0; i < numOfClasses; ++i)
	{
		for(int j = 0; j < classCount[i]; ++j)
		{
			if(j >= (classCount[i] * currentFold / numOfFolds) &&
			   j < (classCount[i] * (currentFold + 1) / numOfFolds))
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

void Classifier::crossClassification(int numOfPairs, int fold)
{
	const matrix & dataMatrix = this->myClassData->getData();
	const auto & types = this->myClassData->getTypes();

	std::vector<std::vector<uint>> arr; // [class][index]
	arr.resize(def::numOfClasses());
	for(uint i = 0; i < dataMatrix.rows(); ++i)
	{
		arr[ types[i] ].push_back(i);
	}

	std::cout << "Cross-Class: max = " << numOfPairs << std::endl;
	for(int i = 0; i < numOfPairs; ++i)
	{
		std::cout << i + 1 << ":  "; std::cout.flush();

		for(int i = 0; i < def::numOfClasses(); ++i)
		{
			smLib::mix(arr[i]);
		}
		for(int numFold = 0; numFold < fold; ++numFold)
		{
			std::cout << numFold + 1 << " ";  std::cout.flush();
			auto sets = this->makeIndicesSetsCross(arr, fold, numFold); // on const arr
			this->learn(sets.first);
			this->test(sets.second);
		}
		std::cout << std::endl;
	}
}

void Classifier::trainTestClassification(const QString & trainTemplate,
										 const QString & testTemplate)
{
	const matrix & dataMatrix = this->myClassData->getData();
	const auto & fileNames = this->myClassData->getFileNames();

	std::vector<uint> learnIndices;
	std::vector<uint> tallIndices;
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
		return;
	}
	this->learn(learnIndices);
	this->test(tallIndices);
}

void Classifier::halfHalfClassification()
{
	const auto & dataMatrix = this->myClassData->getData();
	std::vector<uint> learnIndices;
	std::vector<uint> tallIndices;

	for(uint i = 0; i < dataMatrix.rows() / 2; ++i)
	{
		learnIndices.push_back(i);
		tallIndices.push_back(i + dataMatrix.rows() / 2);
	}
	if(learnIndices.empty() || tallIndices.empty())
	{
		return;
	}
	this->learn(learnIndices);
	this->test(tallIndices);
}

