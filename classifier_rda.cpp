#include "classifier.h"

using namespace myLib;

RDA::RDA() : Classifier()
{
    covMat.resize(numCl + 1, matrix());
    centers.resize(numCl + 1);
    dets.resize(numCl);
    gamma = 0.;
    lambda = 0.;
    myType = ClassifierType::RDA;
}


void RDA::setShrinkage(double in)
{
    this->gamma = in;
}

void RDA::setLambda(double in)
{
    this->lambda = in;
}

void RDA::learn(std::vector<int> & indices)
{
//    std::cout << lambda << "\t" << gamma << std::endl;
    matrix oneClass[numCl];
    covMat[numCl] = matrix(dataMatrix->cols(), dataMatrix->cols(), 0);
    for(int i = 0; i < numCl; ++i)
    {
        for(int ind : indices)
        {
            if((*types)[ind] == i)
            {
                oneClass[i].push_back((*dataMatrix)[ind]);
            }
        }
        covMat[i] = oneClass[i].covMatCols(&(centers[i]));
        covMat[numCl] += covMat[i];
    }

    for(int i = 0; i < numCl; ++i)
    {
        /// regularization
        covMat[i] = covMat[i] * (1. - lambda) + covMat[numCl] * lambda;
        covMat[i] /= (1. - lambda) * oneClass[i].rows() + lambda * indices.size();

        /// shrinkage
        double tmpTrace = covMat[i].trace();
        covMat[i] *= (1. - gamma);
        for(int j = 0; j < covMat[i].rows(); ++j)
        {
            covMat[i][j][j] += gamma * tmpTrace / covMat[i].rows();
        }
        covMat[i].invert(&(dets[i]));
    }
}

#if CLASS_TEST_VIRTUAL
void RDA::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<int, double> RDA::classifyDatum(const int & vecNum)
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

    printResult("RDA.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
