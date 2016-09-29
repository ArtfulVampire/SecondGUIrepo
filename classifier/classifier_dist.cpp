#include "classifier.h"

using namespace myLib;

/// lowest distance to cluster center

DIST::DIST() : Classifier()
{
    centers.resize(numCl);
    myType = ClassifierType::DIST;
    typeString = "DIST";
}

void DIST::learn(std::vector<uint> & indices)
{
    for(uint i = 0; i < numCl; ++i)
    {
        matrix oneClass{};
        for(int ind : indices)
        {
            if((*types)[ind] == i)
            {
                oneClass.push_back((*dataMatrix)[ind]);
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
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<uint, double> DIST::classifyDatum(const uint & vecNum)
{

    std::vector<double> distances(numCl);
    for(uint j = 0; j < numCl; ++j)
    {
        distances[j] = -smallLib::distance((*dataMatrix)[vecNum],
                                           centers[j]);
    }
    uint outClass = myLib::indexOfMax(distances);

    printResult("DIST.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
