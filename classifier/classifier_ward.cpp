#include <classifier/classifier.h>

#include <myLib/output.h>
#include <myLib/general.h>
#include <myLib/valar.h>

using namespace myOut;

WARD::WARD()
{
    this->numOfClust = 15;
    centers.resize(numOfClust);
    this->myType = ModelType::WARD;
    typeString = "WARD";
}


double WARD::dist(const uint a, const uint b)
{
    double res = 0.;
	res = smLib::distance((myClassData->getData().subRows(clusts[a])).averageRow(),
							 (myClassData->getData().subRows(clusts[b])).averageRow());
    /// Word distance
	res *= clusts[a].size() * clusts[b].size() / static_cast<double>((clusts[a].size() + clusts[b].size()));
    return res;
}

void WARD::setNumClust(int ii)
{
    this->numOfClust = ii;
    centers.resize(numOfClust);
    dists.clear();
}

std::pair<uint, uint> WARD::findNearest()
{
    std::pair<uint, uint> res{0, 1};
    double tmp = dists[0][1];
	for(int i = 0; i < dists.rows(); ++i)
    {
        for(uint j = i + 1; j < dists[i].size(); ++j)
        {
            if(dists[i][j] < tmp)
            {
                tmp = this->dists[i][j];
                res.first = i;
                res.second = j;
            }
        }
    }
    return res;
}

/// recount if dataMatrixChanged!!!!11
void WARD::learn(std::vector<uint> & indices)
{
	if(!dists.isEmpty())
    {
        return;
    }

	uint num = myClassData->getData().rows();
    clusts.resize(num);
    for(uint i = 0; i < clusts.size(); ++i)
    {
        clusts[i] = std::vector<uint>{i};
    }

    dists = matrix(num, num, 0.);
    for(uint i = 0; i < num; ++i)
    {
        for(uint j = i + 1; j < num; ++j)
        {
           dists[i][j] = this->dist(i, j);
           dists[j][i] = dists[i][j];
        }
    }

    std::pair<int, int> lowest;
    for(uint iters = clusts.size(); iters > numOfClust; --iters)
    {
        lowest = this->findNearest();
        this->merge(lowest.first, lowest.second);
    }

    for(uint i = 0; i < clusts.size(); ++i)
	{
		centers[i] = (myClassData->getData().subRows(clusts[i])).averageRow();
    }
}

void WARD::merge(const uint one, const uint two)
{

    const uint A = std::min(one, two);
    const uint B = std::max(one, two);

    const uint U = clusts[A].size();
    const uint V = clusts[B].size();

    for(uint i = 0; i < clusts.size(); ++i)
    {
		if(i == A || i == B) { continue; }
        const uint S = clusts[i].size();
        dists[A][i] = ((S + U) * dists[A][i]
                      + (S + V) * dists[B][i]
					  - S * dists[A][B]) / static_cast<double>(S + U + V);
        dists[i][A] = dists[A][i];
    }

    /// merge itself
    uint oldSize = clusts[A].size();
    clusts[A].resize(oldSize + clusts[B].size());
    std::copy(std::begin(clusts[B]),
              std::end(clusts[B]),
              std::begin(clusts[A]) + oldSize);


    dists.eraseRow(B);
    dists.eraseCol(B);
    clusts.erase(std::begin(clusts) + B);
}


void WARD::classifyDatum1(uint vecNum)
{

    std::vector<double> distances(numOfClust);
    for(uint j = 0; j < numOfClust; ++j)
    {
        /// Word distance
		distances[j] = -smLib::distance(myClassData->getData(vecNum),
                                           centers[j]) * clusts[j].size() / (clusts[j].size() + 1);
    }
    /// add fuzzy solving for first N clusters
    int ind = myLib::indexOfMax(distances);

	outputLayer.resize(myClassData->getNumOfCl()); outputLayer = 0;
	for(auto index : clusts[ind])
    {
		outputLayer[ myClassData->getTypes(index) ] += 1;
    }
	outputLayer /= myClassData->getApriori();
}
