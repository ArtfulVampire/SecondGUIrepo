#include "classifier.h"

using namespace myLib;

KNN::KNN() : Classifier()
{
    numOfNear = 10;
    myType = ClassifierType::KNN;    
}

void KNN::setNumOfNear(int ii)
{
    numOfNear = ii;
}

/// recount if dataMatrixChanged!!!!11
void KNN::learn(std::vector<uint> & indices)
{
    if(!distances.empty()) return;

    uint num = dataMatrix->rows();
    distances = matrix(num, num, 0.);
    for(uint i = 0; i < num; ++i)
    {
        for(uint j = i + 1; j < num; ++j)
        {
           distances[i][j] = smallLib::distance((*dataMatrix)[i],
                    (*dataMatrix)[j]);
           distances[j][i] = distances[i][j];
        }
    }
}

#if CLASS_TEST_VIRTUAL
void KNN::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<int, double> KNN::classifyDatum(const int & vecNum)
{
    std::vector<std::pair<double, int>> toSort;

    for(uint i = 0; i < dataMatrix->rows(); ++i)
    {
        if(i == vecNum) continue;
        toSort.push_back(std::make_pair(
                             distances[i][vecNum],
                             (*types)[i])
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
    std::vector<double> numOfClass(numCl, 0);
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
    int outClass = myLib::indexOfMax(numOfClass);

    printResult("KNN.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
