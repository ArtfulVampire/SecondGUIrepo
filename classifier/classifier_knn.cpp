#include <classifier/classifier.h>

#include <myLib/output.h>
#include <myLib/valar.h>

using namespace myOut;

KNN::KNN()
{
    numOfNear = 10;
    myType = ModelType::KNN;    
    typeString = "KNN";
}


void KNN::setNumOfNear(int ii)
{
    numOfNear = ii;
}

/// recount if dataMatrixChanged!!!!11
void KNN::learn(std::vector<uint> & indices)
{
	if(!distances.isEmpty()) { return; }

	uint num = myClassData->getData().rows();
    distances = matrix(num, num, 0.);
    for(uint i = 0; i < num; ++i)
    {
        for(uint j = i + 1; j < num; ++j)
        {
		   distances[i][j] = smLib::distance(myClassData->getData()[i],
											 myClassData->getData()[j]);
           distances[j][i] = distances[i][j];
        }
    }
}

void KNN::classifyDatum1(uint vecNum)
{
	std::vector<std::pair<double, int>> toSort; /// [otherVecNum][distance, otherVecType]
	toSort.reserve(myClassData->getData().rows());

	/// fill toSort
	for(int i = 0; i < myClassData->getData().rows(); ++i)
	{
		if(i == vecNum) { continue; }
		toSort.push_back(std::make_pair(
							 distances[i][vecNum],
							 myClassData->getTypes(i))
						 );
	}

	/// sort toSort
	std::sort(std::begin(toSort),
			  std::end(toSort),
			  [](std::pair<double, int> par1, std::pair<double, int> par2)
	{
		return par1.first < par2.first;
	}
	);

	/// solving part
	outputLayer.resize(myClassData->getNumOfCl()); outputLayer = 0;
	double sgm = toSort[numOfNear].first / 3.;

	for(int i = 0; i < numOfNear; ++i)
	{
#if 0
		/// simple
		outputLayer[toSort[i].second] += 1;
#else
		/// weighted
		outputLayer[toSort[i].second] += std::exp(-std::pow(toSort[i].first, 2) / (2 * sgm * sgm));
#endif
	}
}
