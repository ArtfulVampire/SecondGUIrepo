#include "classifier.h"

using namespace myLib;

QDA::QDA() : Classifier()
{
    covMat.resize(numCl, matrix());
    centers.resize(numCl);
    dets.resize(numCl);
    myType = ClassifierType::QDA;
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

        for(int j = 0; j < oneClass.rows(); ++j)
        {
            oneClass[j] -= centers[i];
        }

        covMat[i] = matrix::transpose(oneClass) * oneClass;
        covMat[i] /= oneClass.rows();

//        std::cout << "covMat = " << std::endl;
//        covMat[i].print();
        covMat[i].invert(&(dets[i]));
//        std::cout << "invMat = " << std::endl;
//        covMat[i].print();
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
//        std::cout << tmp << std::endl;
        output[i] = - tmp - log(dets[i]);
    }
//    std::cout << (*types)[vecNum] << ":\t" << output << std::endl;
    int outClass = myLib::indexOfMax(output);

    printResult("QDA.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}
