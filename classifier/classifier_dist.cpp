#include <classifier/classifier.h>

#include <myLib/output.h>
#include <myLib/valar.h>

using namespace myOut;

/// lowest distance to cluster center

DIST::DIST()
{
    myType = ModelType::DIST;
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

void DIST::classifyDatum1(uint vecNum)
{
	outputLayer.resize(myClassData->getNumOfCl()); outputLayer = 0;
	for(uint j = 0; j < myClassData->getNumOfCl(); ++j)
    {
		outputLayer[j] = -smLib::distance(myClassData->getData()[vecNum],
                                           centers[j]);
	}
}
