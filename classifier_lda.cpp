
#include "classifier.h"

using namespace myLib;

LDA::LDA() : Classifier()
{
    covMat = matrix();
    centers.resize(numCl);
    shrinkage = 0.;
    myType = ClassifierType::LDA;
}

void LDA::setShrinkage(double in)
{
    this->shrinkage = in;
}

void LDA::learn(std::vector<int> & indices)
{
    /// fill learning submatrix
    matrix oneClass[numCl];
    for(int ind : indices)
    {
        oneClass[(*types)[ind]].push_back((*dataMatrix)[ind]);
    }
    for(int i = 0; i < numCl; ++i)
    {
        centers[i] = oneClass[i].averageRow();
        for(int j = 0; j < oneClass[i].rows(); ++j)
        {
            oneClass[i][j] -= centers[i];
        }
    }

    matrix subMatrix = oneClass[0];
    for(int i = 1; i < numCl; ++i)
    {
        subMatrix.vertCat(std::move(oneClass[i]));
    }


    covMat = matrix::transpose(subMatrix) * subMatrix;
    covMat /= subMatrix.rows();

    /// regularization
    covMat *= (1. - shrinkage);
    for(int i = 0; i < covMat.rows(); ++i) {covMat[i][i] += shrinkage;}

    covMat.invert();
}

#if CLASS_TEST_VIRTUAL
void LDA::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<int, double> LDA::classifyDatum(const int & vecNum)
{
    lineType output(numCl);

    for(uint i = 0; i < numCl; ++i)
    {
        lineType a = ((*dataMatrix)[vecNum] - centers[i]);
        matrix m1(a, 'r'); // row
        matrix m2(a, 'c'); // col
        double tmp = (m1 * covMat * m2)[0][0];
        output[i] = - tmp  + 2. * log(this->apriori[i]);
    }
    int outClass = myLib::indexOfMax(output);

    printResult("LDA.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
