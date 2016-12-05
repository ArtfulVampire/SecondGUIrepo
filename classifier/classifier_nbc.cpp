#include "classifier.h"

using namespace myLib;
using namespace myOut;

NBC::NBC() : Classifier()
{
//	std::cout << myData->getNumOfCl() << std::endl;

    myType = ClassifierType::NBC;
    typeString = "NBC";
}

void NBC::adjustToNewData()
{
	centers.resize(myData->getNumOfCl());
	sigmas.resize(myData->getNumOfCl());
}

void NBC::learn(std::vector<uint> & indices)
{
	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
        matrix oneClass{};
        for(int ind : indices)
        {
			if(myData->getTypes()[ind] == i)
            {
				oneClass.push_back(myData->getData()[ind]);
            }
        }
        centers[i] = oneClass.averageRow();
        sigmas[i] = oneClass.sigmaOfCols();
    }
}

#if CLASS_TEST_VIRTUAL
void NBC::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
		confusionMatrix[myData->getTypes()[ind]][res.first] += 1.;
    }
}
#endif

/// what with apriori???
std::pair<uint, double> NBC::classifyDatum(const uint & vecNum)
{
	std::vector<double> res(myData->getNumOfCl());
	std::valarray<double> vec[myData->getNumOfCl()];
	for(uint i = 0; i < myData->getNumOfCl(); ++i)
    {
		vec[i] = myData->getData()[vecNum] - centers[i];
		res[i] = myData->getApriori()[i]; /// ???
        std::valarray<double> pewpew = 1. / sigmas[i]
                                       * exp( - pow(vec[i], 2.) / (2. * pow(sigmas[i], 2.)));
        for(double item : pewpew)
        {
            res[i] *= item;
        }
		res[i] *= myData->getApriori()[i]; /// ???
    }
    uint outClass = myLib::indexOfMax(res);

    printResult("NBC.txt", outClass, vecNum);

    return std::make_pair(outClass,
						  double(outClass != myData->getTypes()[vecNum]));
}
