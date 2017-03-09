#include "classifier.h"

using namespace myLib;
using namespace myOut;

NBC::NBC() : Classifier()
{
//	std::cout << myClassData->getNumOfCl() << std::endl;

    myType = ClassifierType::NBC;
    typeString = "NBC";
}

void NBC::adjustToNewData()
{
	centers.resize(myClassData->getNumOfCl());
	sigmas.resize(myClassData->getNumOfCl());
}

void NBC::learn(std::vector<uint> & indices)
{
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
        matrix oneClass{};
        for(int ind : indices)
        {
			if(myClassData->getTypes()[ind] == i)
            {
				oneClass.push_back(myClassData->getData()[ind]);
            }
        }
        centers[i] = oneClass.averageRow();
        sigmas[i] = oneClass.sigmaOfCols();
    }
}

/// what with apriori???
std::pair<uint, double> NBC::classifyDatum(const uint & vecNum)
{
	std::vector<double> res(myClassData->getNumOfCl());
	std::valarray<double> vec[myClassData->getNumOfCl()];
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
		vec[i] = myClassData->getData()[vecNum] - centers[i];
		res[i] = myClassData->getApriori()[i]; /// ???
        std::valarray<double> pewpew = 1. / sigmas[i]
                                       * exp( - pow(vec[i], 2.) / (2. * pow(sigmas[i], 2.)));
        for(double item : pewpew)
        {
            res[i] *= item;
        }
		res[i] *= myClassData->getApriori()[i]; /// ???
    }
    uint outClass = myLib::indexOfMax(res);

    printResult("NBC.txt", outClass, vecNum);

    return std::make_pair(outClass,
						  double(outClass != myClassData->getTypes()[vecNum]));
}
