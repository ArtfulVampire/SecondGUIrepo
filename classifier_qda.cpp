#include "classifier.h"

QDA::QDA()
{
    covMat.resize(numCl);
    centers.resize(numCl);
    dets.resize(numCl);
}

void QDA::learn(std::vector<int> & indices)
{
    for(int i = 0; i < numCl; ++i)
    {
        /// fill learning submatrix
        matrix oneClass{};
        for(int ind : indices)
        {
            if((*types)[ind] == i)
            {
                oneClass.push_back((*dataMatrix)[ind]);
            }
        }
        centers[i] = oneClass.averageRow();
        covMat[i] = matrix::transpose(oneClass) * oneClass;
        covMat[i].invert(&(dets[i]));
    }
}

void QDA::test(const std::vector<int> & indices)
{
    confusionMatrix.fill(0.);
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
//    averageClassification();
}

std::pair<int, double> QDA::classifyDatum(const int & vecNum)
{
    lineType output(numCl);

    for(uint i = 0; i < numCl; ++i)
    {
        lineType a = ((*dataMatrix)[vecNum] - centers[i]);
        matrix m1(a, 'r'); // row
        matrix m2(a, 'c'); // col
        output[i] = - (m1 * covMat[i] * m2)[0][0] - log(dets[i]);
    }
    int outClass = myLib::indexOfMax(output);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
