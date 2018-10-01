#include <classifier/classifier.h>

#include <set>

#include <myLib/output.h>

using namespace myOut;

Classifier::Classifier()
{
	myClassData = new ClassifierData();
	resultsPath = DEFS.dirPath() + "/results.txt";
	workDir = DEFS.dirPath() + "/Help/PA";
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
		myClassData = &in; //// pointer may overlive object in !!!!

		confusionMatrix = matrix(myClassData->getNumOfCl(), myClassData->getNumOfCl(), 0.);
		outputLayer = std::valarray<double>(0., myClassData->getNumOfCl());

#if INERTIA
		fbVal = std::valarray<double>(0., myClassData->getNumOfCl());
#endif

		this->adjustToNewData();
	}
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

#if 0
/// TEMPORARY, will be virtual=0
/// SHOULD OVERRIDE FOR ALL CLASSIFIERS
void Classifier::classifyDatum1(uint vecNum)
{
	/// ALWAYS TRUE WITH ZERO ERROR
	this->confusionMatrix[myClassData->getTypes()[vecNum]][myClassData->getTypes()[vecNum]] += 1.;
	outputLayer = clLib::oneHot(myClassData->getNumOfCl(),
								myClassData->getTypes()[vecNum]);
}
#endif

Classifier::classOneType Classifier::classifyDatum(uint vecNum)
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
	return std::make_tuple(myClassData->getTypes()[vecNum] == outClass,
						   outClass,
						   clLib::countError(outputLayer, myClassData->getTypes()[vecNum]));
}

Classifier::classOneType Classifier::classifyDatumLast()
{
	return classifyDatum(myClassData->getData().rows() - 1);
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
	fbVal *= suc::inertiaCoef;
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

		outStr << vecNum + 2 << ":\ttrue = " << myClassData->getTypes()[vecNum] << "\tpred = " << predType << "\n";
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
	learnAll(); /// relearn w/o weights reset
    this->resetFlag = true;
}

void Classifier::printParams()
{
}

Classifier::avType Classifier::averageClassification(std::ostream & os)
{
#if 01
	/// successive
	resultsPath =
			DEFS.dirPath()
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
		pE += (confusionMatrix[i].sum() * confusionMatrix.getCol(i).sum()) / std::pow(S, 2);
    }
    kappa = 1. - (1. - corrSum / wholeNum) / (1. - pE);

    res << smLib::doubleRound(averageAccuracy, 2) << '\t';
    res << smLib::doubleRound(kappa, 3) << '\t';
	res << std::endl;
    res.close();


	os << confusionMatrix;
	os << "average accuracy = " << smLib::doubleRound(averageAccuracy, 1) << "\t";
	os << "kappa = " << smLib::doubleRound(kappa, 3) << "\t";
	os << std::endl << std::endl;

    confusionMatrix.fill(0.);

    return std::make_pair(averageAccuracy, kappa);
}

void Classifier::leaveOneOutClassification(std::ostream & os)
{
	const auto & dataMatrix = this->myClassData->getData();
	std::vector<uint> learnIndices;
	os << "LOO: max = " << dataMatrix.rows() << std::endl;
	for(int i = 0; i < dataMatrix.rows(); ++i)
	{
		if((i + 1) % 10 == 0) { os << i + 1 << " "; os.flush(); }

		learnIndices.clear();
		learnIndices.resize(dataMatrix.rows() - 1);
		std::iota(std::begin(learnIndices),
				  std::begin(learnIndices) + i,
				  0);
		std::iota(std::begin(learnIndices) + i,
				  std::end(learnIndices),
				  i + 1);

		this->learn(learnIndices);
		this->test(uint(i));
	}
}


std::pair<std::vector<uint>,
std::vector<uint>> Classifier::makeIndicesSetsCross(
		const std::vector<std::vector<uint> > & arr,
		int numOfFolds,
		const int currentFold)
{
	const double numOfClasses = this->myClassData->getNumOfCl();

	std::vector<uint> learnInd;
	std::vector<uint> tallInd;

	for(int i = 0; i < numOfClasses; ++i)
	{
		const int clSize = this->myClassData->getClassCount(i);
		for(int j = 0; j < clSize; ++j)
		{
			if(j >= (clSize * currentFold / numOfFolds) &&
			   j < (clSize * (currentFold + 1) / numOfFolds))
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

void Classifier::peopleClassification(bool indZ, std::ostream & os)
{
	const int size = this->myClassData->size();

	/// compose a set of people
	std::set<QString> people{};
	std::map<QString, std::vector<uint>> zSets{};
	for(int i = 0; i < size; ++i)
	{
		const QString a = this->myClassData->getFileNames()[i];
		const QString guy = a.left(a.indexOf('_'));
		people.emplace(guy);

		zSets[guy].push_back(i);
	}

#if 0
	for(const QString & guy : people)
	{
		for(auto in : zSets[guy])
		{
			std::cout << this->myClassData->getFileNames()[in] << std::endl;
		}
		std::cout << std::endl;
	}
#endif

	if(indZ)
	{
		/// z-transform for each guy separately?
		for(const QString & guy : people)
		{
			this->myClassData->z_transformSubset(zSets[guy]);
		}
	}

	std::vector<uint> learnSet{};	learnSet.reserve(size);
	std::vector<uint> testSet{};	testSet.reserve(size);

	for(const QString & guy : people)
	{
		std::cout << guy << "\t";
	}
	os << "peopleClassification: " << std::endl;
	std::cout << std::endl;

	for(const QString & guy : people)
	{
		learnSet.clear();
		testSet.clear();
		for(int i = 0; i < size; ++i)
		{
			if(this->myClassData->getFileNames()[i].startsWith(guy))
			{
				testSet.push_back(i);
			}
			else
			{
				learnSet.push_back(i);
			}
		}
		this->learn(learnSet);
		this->test(testSet);

		this->averageClassification(DEVNULL);
		os << guy
		   << "\t" << smLib::doubleRound(this->averageAccuracy, 2)
		   << "\t" << smLib::doubleRound(this->kappa, 3)
		   << std::endl;
	}
}

void Classifier::crossClassification(int numOfPairs, int fold, std::ostream & os)
{
	const matrix & dataMatrix = this->myClassData->getData();
	const auto & types = this->myClassData->getTypes();

	std::vector<std::vector<uint>> arr; /// [class][index]
	arr.resize(this->myClassData->getNumOfCl());
	for(int i = 0; i < dataMatrix.rows(); ++i)
	{
		arr[ types[i] ].push_back(i);
	}

	os << "Cross-Class: max = " << numOfPairs << std::endl;
	for(int i = 0; i < numOfPairs; ++i)
	{
		os << i + 1 << ":  "; std::cout.flush();

		for(uint i = 0; i < this->myClassData->getNumOfCl(); ++i)
		{
			smLib::mix(arr[i]);
		}
		for(int numFold = 0; numFold < fold; ++numFold)
		{
			os << numFold + 1 << " ";  os.flush();
			auto sets = this->makeIndicesSetsCross(arr, fold, numFold); /// on const arr
			this->learn(sets.first);
			this->test(sets.second);
		}
		os << std::endl;
	}
}

void Classifier::trainTestClassification(const QString & trainTemplate,
										 const QString & testTemplate)
{
	const matrix & dataMatrix = this->myClassData->getData();
	const auto & fileNames = this->myClassData->getFileNames();

	std::vector<uint> learnIndices;
	std::vector<uint> tallIndices;
	for(int i = 0; i < dataMatrix.rows(); ++i)
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

	for(int i = 0; i < dataMatrix.rows() / 2; ++i)
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

/// check by Meyers - local variables in lambda
void Classifier::cleaningNfold(int num)
{
	int counter = 0;
	bool hazWrong = false;

	std::function<bool(void)> pred;
	if(num > 0)
	{
		pred = [&counter, num]() { return counter < num; };
	}
	else
	{
		pred = [&hazWrong]() { return hazWrong; };
	}

	do
	{
		hazWrong = false;
		std::vector<uint> learnIndices{};
		std::vector<uint> exclude{};
		for(int i = 0; i < this->myClassData->size(); ++i)
		{
			learnIndices.clear();
			learnIndices.resize(this->myClassData->size() - 1);
			std::iota(std::begin(learnIndices),
					  std::begin(learnIndices) + i,
					  0);
			std::iota(std::begin(learnIndices) + i,
					  std::end(learnIndices),
					  i + 1);

			this->learn(learnIndices);
			auto a = this->test(i);
			if(!std::get<0>(a))
			{
				exclude.push_back(i);
				hazWrong = true;
			}
		}
		this->myClassData->erase(exclude);
		std::cout << counter << "\t";
		++counter;
	} while (pred());
	std::cout << std::endl;
}

void Classifier::cleaningKfold(int num, int fold)
{
	int counter = 0;
	bool hazWrong = false;

	std::function<bool(void)> continueIf;
	if(num > 0)
	{
		continueIf = [&counter, &hazWrong, num](){ return (counter < num) ? hazWrong : false; };
	}
	else
	{
		continueIf = [&hazWrong](){ return hazWrong; };
	}

	do
	{
		hazWrong = false;
		/// fill arr
		auto arr = this->myClassData->getIndices();
		for(auto & inArr : arr) { smLib::mix(inArr); }

		auto sets = this->makeIndicesSetsCross(arr, fold, 0);
		this->learn(sets.first);

		/// remove some indices
		std::vector<uint> exclude{};
		for(uint i : sets.second)
		{
			auto a = this->test(i);
			if(!std::get<0>(a))
			{
				exclude.push_back(i);
			}
		}
		if(!exclude.empty())
		{
			this->myClassData->erase(exclude);
			hazWrong = true;
		}
		++counter;
	} while(continueIf());

	/// reset params
	confusionMatrix.fill(0.);
	outputLayer = 0.;

#if 0
	/// classify - what for?
	this->crossClassification(3, fold, DEVNULL);
#endif
}
