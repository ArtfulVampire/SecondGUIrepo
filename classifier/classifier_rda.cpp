#include <classifier/classifier.h>

#include <myLib/output.h>

using namespace myOut;

RDA::RDA()
{
    gamma = 0.;
    lambda = 0.;
    myType = ModelType::RDA;
    typeString = "RDA";
}

void RDA::adjustToNewData()
{
	covMat.resize(myClassData->getNumOfCl() + 1, matrix());
	centers.resize(myClassData->getNumOfCl() + 1);
	dets.resize(myClassData->getNumOfCl());
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
#if 0
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
	{
		std::cout << centers[i] << std::endl;
	}
#endif
}


/// check!
void RDA::learn(std::vector<uint> & indices)
{
	matrix oneClass[myClassData->getNumOfCl()];
	covMat[myClassData->getNumOfCl()] = matrix(myClassData->getData().cols(), myClassData->getData().cols(), 0);
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
        for(int ind : indices)
        {
			if(myClassData->getTypes(ind) == i)
            {
				oneClass[i].push_back(myClassData->getData(ind));
            }
        }
        covMat[i] = oneClass[i].covMatCols(&(centers[i]));
		covMat[myClassData->getNumOfCl()] += covMat[i];
	}

	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
        /// regularization
		covMat[i] = covMat[i] * (1. - lambda) + covMat[myClassData->getNumOfCl()] * lambda;
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

void RDA::classifyDatum1(uint vecNum)
{
	outputLayer.resize(myClassData->getNumOfCl()); outputLayer = 0;

	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
	{
		std::valarray<double> a = (myClassData->getData(vecNum) - centers[i]);
        matrix m1(a, 'r'); /// row
        matrix m2(a, 'c'); /// col
		double tmp = (m1 * covMat[i] * m2)[0][0];
		outputLayer[i] = - tmp - std::log(dets[i]) + 2 * std::log(myClassData->getApriori()[i]);
	}
}
