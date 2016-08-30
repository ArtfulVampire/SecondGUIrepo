#include "classifier.h"

WORD::WORD() : Classifier()
{
    this->numOfClust = 15;
    centers.resize(numOfClust);
    this->myType = ClassifierType::WORD;
    typeString = "WORD";
}

double WORD::dist(const uint a, const uint b)
{
    double res = 0.;
    res = smallLib::distance((dataMatrix->subRows(clusts[a])).averageRow(),
                             (dataMatrix->subRows(clusts[b])).averageRow());
    /// Word distance
    res *= clusts[a].size() * clusts[b].size() / double((clusts[a].size() + clusts[b].size()));
    return res;
}

void WORD::setNumClust(int ii)
{
    this->numOfClust = ii;
    centers.resize(numOfClust);
    dists.clear();
}

std::pair<uint, uint> WORD::findNearest()
{
    std::pair<uint, uint> res{0, 1};
    double tmp = dists[0][1];
    for(uint i = 0; i < dists.size(); ++i)
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
void WORD::learn(std::vector<uint> & indices)
{
    if(!dists.empty())
    {
        return;
    }

    uint num = dataMatrix->rows();
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
//        myLib::operator <<(std::cout, clusts[i]); std::cout << std::endl;
        centers[i] = (dataMatrix->subRows(clusts[i])).averageRow();
    }
}

void WORD::merge(const uint one, const uint two)
{

//    std::cout << one << "\t" << two << std::endl;
    const uint A = std::min(one, two);
    const uint B = std::max(one, two);

//    std::cout << A << "\t" << B << std::endl;

    const uint U = clusts[A].size();
    const uint V = clusts[B].size();

//    std::cout << U << "\t" << V << std::endl << std::endl;

    for(uint i = 0; i < clusts.size(); ++i)
    {
        if(i == A || i == B) continue;
        const uint S = clusts[i].size();
        dists[A][i] = ((S + U) * dists[A][i]
                      + (S + V) * dists[B][i]
                      - S * dists[A][B]) / double(S + U + V);
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


std::pair<uint, double> WORD::classifyDatum(const uint & vecNum)
{

    std::vector<double> distances(numOfClust);
    for(uint j = 0; j < numOfClust; ++j)
    {
        /// Word distance
        distances[j] = -smallLib::distance((*dataMatrix)[vecNum],
                                           centers[j]) * clusts[j].size() / (clusts[j].size() + 1);
    }
    /// add fuzzy solving for first N clusters
    int ind = myLib::indexOfMax(distances);

    std::valarray<double> numOfClass(0., numCl);
    for(uint i = 0; i < clusts[ind].size(); ++i)
    {
        numOfClass[ (*types)[ clusts[ind][i] ] ] += 1;
    }
    numOfClass /= apriori;
    uint outClass = myLib::indexOfMax(numOfClass);


    printResult("WORD.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));

}
