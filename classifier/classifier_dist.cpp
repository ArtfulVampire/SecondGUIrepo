#include "classifier.h"

using namespace myLib;
using namespace myOut;

/// lowest distance to cluster center

DIST::DIST() : Classifier()
{
    myType = ClassifierType::DIST;
    typeString = "DIST";
}

void DIST::adjustToNewData()
{
	centers.resize(myData->getNumOfCl());
}

void DIST::learn(std::vector<uint> & indices)
{
	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
        matrix oneClass{};
        for(int ind : indices)
        {
			if(myData->getTypes()[ind] == i)
            {
				oneClass.push_back(myData->getData()[ind]);
            }
        }
        centers[i] = oneClass.averageRow();
    }
}

#if CLASS_TEST_VIRTUAL
void DIST::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
		confusionMatrix[myData->getTypes()[ind]][res.first] += 1.;
    }
}
#endif

std::pair<uint, double> DIST::classifyDatum(const uint & vecNum)
{

	std::vector<double> distances(myData->getNumOfCl());
	for(uint j = 0; j < myData->getNumOfCl(); ++j)
    {
		distances[j] = -smLib::distance(myData->getData()[vecNum],
                                           centers[j]);
    }
    uint outClass = myLib::indexOfMax(distances);

    printResult("DIST.txt", outClass, vecNum);

    return std::make_pair(outClass,
						  double(outClass != myData->getTypes()[vecNum]));
}
