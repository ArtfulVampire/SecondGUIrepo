#include <classifier/classifier.h>

#include <myLib/output.h>

using namespace myOut;

NBC::NBC()
{
    myType = ModelType::NBC;
    typeString = "NBC";
}

void NBC::adjustToNewData()
{
	centers.resize(myClassData->getNumOfCl());
	sigmas.resize(myClassData->getNumOfCl());
#if 0
	//// make something with apriori
	myClassData->setApriori();
#endif
}

void NBC::learn(std::vector<uint> & indices)
{
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
        matrix oneClass{};
        for(int ind : indices)
        {
			if(myClassData->getTypes(ind) == i)
            {
				oneClass.push_back(myClassData->getData(ind));
            }
        }
        centers[i] = oneClass.averageRow();
        sigmas[i] = oneClass.sigmaOfCols();
    }
}

/// what with apriori???
void NBC::classifyDatum1(uint vecNum)
{
	outputLayer.resize(myClassData->getNumOfCl()); outputLayer = 0;
	std::valarray<double> vec[myClassData->getNumOfCl()];
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
		vec[i] = myClassData->getData(vecNum) - centers[i];
		outputLayer[i] = myClassData->getApriori()[i]; /// ???
        std::valarray<double> pewpew = 1. / sigmas[i]
                                       * exp( - std::pow(vec[i], 2.) / (2. * std::pow(sigmas[i], 2.)));
        for(double item : pewpew)
        {
			outputLayer[i] *= item;
        }
		outputLayer[i] *= myClassData->getApriori()[i]; /// ???
	}
}
