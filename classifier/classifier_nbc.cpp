#include "classifier.h"

using namespace myLib;

NBC::NBC() : Classifier()
{
    centers.resize(numCl);
    sigmas.resize(numCl);
    myType = ClassifierType::NBC;
}

void NBC::learn(std::vector<uint> & indices)
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
        sigmas[i] = oneClass.sigmaOfCols();
    }
}

#if CLASS_TEST_VIRTUAL
void NBC::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<uint, double> NBC::classifyDatum(const uint & vecNum)
{
    std::vector<double> res(numCl);
    std::valarray<double> vec[numCl];
    for(uint i = 0; i < numCl; ++i)
    {
        vec[i] = (*dataMatrix)[vecNum] - centers[i];
        res[i] = apriori[i];
        std::valarray<double> pewpew = 1. / sigmas[i]
                                       * exp( - pow(vec[i], 2.) / (2. * pow(sigmas[i], 2.)));
        for(double item : pewpew)
        {
            res[i] *= item;
        }
        res[i] *= this->apriori[i];
    }
    uint outClass = myLib::indexOfMax(res);

    printResult("NBC.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
