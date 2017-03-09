#include <classifier/classifier.h>

using namespace myOut;

/// lowest distance to cluster center

DIST::DIST() : Classifier()
{
    myType = ClassifierType::DIST;
    typeString = "DIST";
}

void DIST::adjustToNewData()
{
	centers.resize(myClassData->getNumOfCl());
}

void DIST::learn(std::vector<uint> & indices)
{
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
        matrix oneClass{};
        for(int ind : indices)
        {
			if(myClassData->getTypes()[ind] == i)
            {
				oneClass.push_back(myClassData->getData()[ind]);
            }
        }
        centers[i] = oneClass.averageRow();
    }
}

std::pair<uint, double> DIST::classifyDatum(const uint & vecNum)
{

	std::vector<double> distances(myClassData->getNumOfCl());
	for(uint j = 0; j < myClassData->getNumOfCl(); ++j)
    {
		distances[j] = -smLib::distance(myClassData->getData()[vecNum],
                                           centers[j]);
    }
    uint outClass = myLib::indexOfMax(distances);

    printResult("DIST.txt", outClass, vecNum);

    return std::make_pair(outClass,
						  double(outClass != myClassData->getTypes()[vecNum]));
}
