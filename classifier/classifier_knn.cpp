#include <classifier/classifier.h>

using namespace myOut;

KNN::KNN()
{
    numOfNear = 10;
    myType = ClassifierType::KNN;    
    typeString = "KNN";
}


void KNN::setNumOfNear(int ii)
{
    numOfNear = ii;
}

/// recount if dataMatrixChanged!!!!11
void KNN::learn(std::vector<uint> & indices)
{
	if(!distances.isEmpty()) return;

	uint num = myClassData->getData().rows();
    distances = matrix(num, num, 0.);
    for(uint i = 0; i < num; ++i)
    {
        for(uint j = i + 1; j < num; ++j)
        {
		   distances[i][j] = smLib::distance(myClassData->getData()[i],
											 myClassData->getData()[j]);
           distances[j][i] = distances[i][j];
        }
    }
}

void KNN::classifyDatum1(const uint & vecNum)
{
	std::vector<std::pair<double, int>> toSort; // [otherVecNum][distance, otherVecType]

	/// fill toSort
	for(uint i = 0; i < myClassData->getData().rows(); ++i)
	{
		if(i == vecNum) continue;
		toSort.push_back(std::make_pair(
							 distances[i][vecNum],
							 myClassData->getTypes()[i])
						 );
	}

	/// sort toSort
	std::sort(std::begin(toSort),
			  std::end(toSort),
			  [](std::pair<double, int> par1, std::pair<double, int> par2)
	{
		return par1.first < par2.first;
	}
	);

	/// solving part
	std::valarray<double> numOfClass(myClassData->getNumOfCl());
	double sgm = toSort[numOfNear].first / 3.;

	for(int i = 0; i < numOfNear; ++i)
	{
#if 0
		/// simple
		numOfClass[toSort[i].second] += 1;
#else
		/// weighted
		numOfClass[toSort[i].second] += exp(-pow(toSort[i].first, 2) / (2 * sgm * sgm));
#endif
	}
	outputLayer = numOfClass / double(numOfClass.sum());
	confusionMatrix[myClassData->getTypes()[vecNum]][myLib::indexOfMax(outputLayer)] += 1.;
}

std::pair<uint, double> KNN::classifyDatum(const uint & vecNum)
{
    std::vector<std::pair<double, int>> toSort;

	for(uint i = 0; i < myClassData->getData().rows(); ++i)
    {
        if(i == vecNum) continue;
        toSort.push_back(std::make_pair(
                             distances[i][vecNum],
							 myClassData->getTypes()[i])
                         );
    }
    std::sort(std::begin(toSort),
              std::end(toSort),
              [](std::pair<double, int> par1, std::pair<double, int> par2)
    {
        return par1.first < par2.first;
    }
    );

    /// solving part
	std::vector<double> numOfClass(myClassData->getNumOfCl(), 0);
    double sgm = toSort[numOfNear].first / 3.;

    for(int i = 0; i < numOfNear; ++i)
    {
#if 0
        /// simple
        numOfClass[toSort[i].second] += 1;
#else
        /// weighted
        numOfClass[toSort[i].second] += exp(-pow(toSort[i].first, 2) / (2 * sgm * sgm));
#endif
    }
    uint outClass = myLib::indexOfMax(numOfClass);

    printResult("KNN.txt", outClass, vecNum);

    return std::make_pair(outClass,
						  double(outClass != myClassData->getTypes()[vecNum]));
}
