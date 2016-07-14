#include "classifier.h"

using namespace myLib;

QDA::QDA() : Classifier()
{
    covMat.resize(numCl + 1, matrix());
    centers.resize(numCl + 1);
    dets.resize(numCl);
    shrinkage = 0.;
    lambda = 0.;
    myType = ClassifierType::QDA;
}


void QDA::setShrinkage(double in)
{
    this->shrinkage = in;
}

void QDA::setLambda(double in)
{
    this->lambda = in;
}

void QDA::learn(std::vector<int> & indices)
{
    /// count general covMat
    covMat[numCl] = dataMatrix->subRows(indices).covMatCols(&(centers[numCl]));

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
        covMat[i] = oneClass.covMatCols(&(centers[i]));

        /// regularization
        covMat[i] = (1. - lambda) * covMat[i] + lambda * covMat[numCl];
        covMat[i] /= (1. - lambda) * oneClass.rows() + lambda * indices.size();

        /// shrinkage
        covMat[i] *= (1. - shrinkage);
        for(int j = 0; j < covMat[i].rows(); ++j)
        {
            covMat[i][j][j] += shrinkage;
        }
        covMat[i].invert(&(dets[i]));
    }
}

#if CLASS_TEST_VIRTUAL
void QDA::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<int, double> QDA::classifyDatum(const int & vecNum)
{
    lineType output(numCl);

    for(uint i = 0; i < numCl; ++i)
    {
        lineType a = ((*dataMatrix)[vecNum] - centers[i]);
        matrix m1(a, 'r'); // row
        matrix m2(a, 'c'); // col
        double tmp = (m1 * covMat[i] * m2)[0][0];
        output[i] = - tmp - log(dets[i]) + 2 * log(this->apriori[i]);
    }
    int outClass = myLib::indexOfMax(output);

    printResult("QDA.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
