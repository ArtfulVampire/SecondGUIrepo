#include "classifier.h"

using namespace myLib;
using namespace myOut;

RDA::RDA() : Classifier()
{
    gamma = 0.;
    lambda = 0.;
    myType = ClassifierType::RDA;
    typeString = "RDA";
}

void RDA::adjustToNewData()
{
	covMat.resize(myData->getNumOfCl() + 1, matrix());
	centers.resize(myData->getNumOfCl() + 1);
	dets.resize(myData->getNumOfCl());
}


void RDA::setShrinkage(double in)
{
    this->gamma = in;
}

void RDA::setLambda(double in)
{
    this->lambda = in;
}

void RDA::printParams()
{
	for(int i = 0; i < myData->getNumOfCl(); ++i)
	{
//		std::cout << centers[i] << std::endl;
	}
}


/// check!
void RDA::learn(std::vector<uint> & indices)
{
//	std::cout << lambda << "\t" << gamma << std::endl;
	matrix oneClass[myData->getNumOfCl()];
//	std::cout << myData->getNumOfCl() << "\t" << covMat.size() << std::endl;
	covMat[myData->getNumOfCl()] = matrix(myData->getData().cols(), myData->getData().cols(), 0);
	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
        for(int ind : indices)
        {
			if(myData->getTypes()[ind] == i)
            {
				oneClass[i].push_back(myData->getData()[ind]);
            }
        }
        covMat[i] = oneClass[i].covMatCols(&(centers[i]));
		covMat[myData->getNumOfCl()] += covMat[i];
	}

	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
        /// regularization
		covMat[i] = covMat[i] * (1. - lambda) + covMat[myData->getNumOfCl()] * lambda;
        covMat[i] /= (1. - lambda) * oneClass[i].rows() + lambda * indices.size();

        /// shrinkage
        double tmpTrace = covMat[i].trace();
        covMat[i] *= (1. - gamma);
//#pragma omp parallel for
        for(uint j = 0; j < covMat[i].rows(); ++j)
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
		confusionMatrix[myData->getTypes()[ind]][res.first] += 1.;
    }
}
#endif

std::pair<uint, double> RDA::classifyDatum(const uint & vecNum)
{
	std::valarray<double> output(myData->getNumOfCl());

	for(uint i = 0; i < myData->getNumOfCl(); ++i)
	{
		std::valarray<double> a = (myData->getData()[vecNum] - centers[i]);
        matrix m1(a, 'r'); // row
        matrix m2(a, 'c'); // col
		double tmp = (m1 * covMat[i] * m2)[0][0];
		output[i] = - tmp - log(dets[i]) + 2 * log(myData->getApriori()[i]);
	}
    uint outClass = myLib::indexOfMax(output);
    printResult("RDA.txt", outClass, vecNum);
    return std::make_pair(outClass,
						  double(outClass != myData->getTypes()[vecNum]));
}
