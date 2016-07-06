#include "classifier.h"
using namespace myLib;

ANN::ANN()
{
    allocWeight(weight); /// dim == {} here
}

void ANN::setResetFlag(bool inFlag)
{
    this->resetFlag = inFlag;
}

void ANN::setTestCleanFlag(bool inFlag)
{
    this->testCleanFlag = inFlag;
}

#if 0
void ANN::forEachWeight(weightType & inWeight,
                        void (*func)(double &, Args...))
{
    for(uint i = 0; i < weight.size(); ++i)
    {
        for(uint j = 0; j < weight[i].size(); ++j)
        {
            for(uint k = 0; k < weight[i][j].size(); ++k)
            {
                func(inWeight[i][j][k], Args...);
            }
        }
    }
}
#endif

void ANN::allocWeight(weightType & inMat)
{
    inMat.resize(dim.size() - 1);
    for(uint i = 0; i < inMat.size(); ++i) // weights to layer i+1 from i
    {
        inMat[i].resize(dim[i + 1]);
        for(uint j = 0; j < inMat[i].size(); ++j) // to j'th in i+1 layer
        {
            // resizing lineType -> fill zeros
            inMat[i][j].resize(dim[i] + 1); // from k'th in i layer + bias
        }
    }
}

void ANN::setDim(const std::vector<int> & inDim)
{
    dim.clear();
    dim.push_back((*dataMatrix).cols());
    for(uint i = 0; i < inDim.size(); ++i)
    {
        dim.push_back(inDim[i]);
    }
    dim.push_back(numCl);

    allocWeight(weight);

#if 0
    if(0) // if backprop
    {
        std::default_random_engine engine;
        std::uniform_int_distribution<int> dist(0, 1000 - 1);
        for(uint i = 0; i < weight.size(); ++i)
        {
            for(uint j = 0; j < weight[i].size(); ++j)
            {
                for(uint k = 0; k < weight[i][j].size(); ++k)
                {
                    weight[i][j][k] = (-500 + dist(engine)) / 50000.;  // backprop ~0
                }
            }
        }
    }
#endif
}

double ANN::getLrate()
{
    return learnRate;
}
void ANN::setLrate(double inRate)
{
    learnRate = inRate;
}



void ANN::loadVector(const int vecNum, std::valarray<double> & out, int & type)
{
    /// out.size() == (*dataMatrix).cols() + 1
    std::copy(std::begin((*dataMatrix)[vecNum]),
              std::end((*dataMatrix)[vecNum]),
              std::begin(out));
    out[out.size() - 1] = 1.; //bias
    type = (*types)[vecNum]; // true class
}

void ANN::countOutput(std::vector<std::valarray<double>> & output)
{
    /// switch case
    if(learnStyl == learnStyle::delta)
    {
        countOutputDelta(output);
    }
    else if(learnStyl == learnStyle::backprop)
    {
        countOutputBackprop(output);
    }
}

void ANN::countOutputDelta(std::vector<std::valarray<double>> & output)
{
    for(int i = 1; i < dim.size(); ++i)
    {
        for(int j = 0; j < dim[i]; ++j) //higher level, w/o bias
        {
            output[i][j] = smallLib::prod(weight[i - 1][j], output[i - 1]); // bias included
        }
        output[i] = activation(output[i]);
        output[i][ dim[i] ] = 1.; //bias, unused for the highest layer
    }
}

void ANN::countOutputBackprop(std::vector<std::valarray<double>> & output)
{
#if 0
    /// count deltaWeights
    /// CHECK
    //count deltaweights (used for backprop only)
    //for the last layer
    for(uint j = 0; j < dim[numOfLayers - 1]; ++j)
    {
        deltaWeights[numOfLayers-1][j] = -1. / temp
                                         * output[numOfLayers-1][j]
                                         * (1. - output[numOfLayers-1][j])
                * ((type == j) - output[numOfLayers-1][j]); //~0.1
    }

    //for the other layers, besides the input one, upside->down
    for(int i = numOfLayers - 2; i >= 1; --i)
    {
        for(int j = 0; j < dim[i] + 1; ++j) //+1 for bias
        {
            deltaWeights[i][j] = 0.;
            for(int k = 0; k < dim[i + 1]; ++k) //connected to the higher-layer
            {
                deltaWeights[i][j] += deltaWeights[i + 1][k] * weight[i][j][k];
            }
            deltaWeights[i][j] *= 1. / temp
                                  * output[i][j]
                                  * (1. - output[i][j]);
        }
    }

#endif

}

void ANN::moveWeights(const std::vector<std::valarray<double>> & output,
                      const std::vector<double> & normCoeff,
                      const int type)
{
    if(learnStyl == learnStyle::delta)
    {
        for(uint j = 0; j < numCl; ++j)
        {
            weight[0][j] += output[0]
                    * (learnRate * normCoeff[type]
                       * ((type == j) - output[1][j])
//                    * (output[1][j] * (1. - output[1][j])) * 6. // derivative
                    );
        }

    }
#if 0
    else if(learnStyl == learnStyle::backprop)
    {
        /// check this sheet
        // count new weights using deltas
        for(int i = 0; i < numOfLayers - 1; ++i)
        {
            for(int j = 0; j < dim[i] + 1; ++j) // +1 for bias? 01.12.2014
            {
                for(int k = 0; k < dim[i + 1]; ++k)
                {
                    weight[i][j][k] -= learnRate
                                       * normCoeff[type]
                                       * output[i][j]
                                       * deltaWeights[i + 1][k];
                }
            }
        }
    }
#endif
}

void ANN::countError(std::vector<std::valarray<double>> & output,
                     int type,
                     double & currentError)
{
    double err = 0.;
    for(int j = 0; j < dim.back(); ++j)
    {
        err += pow((output.back()[j] - int(type == j) ), 2.);
    }
    err = sqrt(err);
    if(errType == errorNetType::SME)
    {
        currentError += err;
    }
    else if(errType == errorNetType::maxDist)
    {
        currentError = std::max(err, currentError);
    }
}

void ANN::learn(std::vector<int> & indices)
{
    QTime myTime;
    myTime.start();

    if(this->resetFlag)
    {
        allocWeight(weight);
    }

    const uint numOfLayers = dim.size();
    std::vector<std::valarray<double>> output(numOfLayers);
    for(uint i = 0; i < numOfLayers; ++i)
    {
        output[i].resize(dim[i] + 1); // +1 for biases
    }

    if(learnStyl == learnStyle::backprop)
    {
        deltaWeights.resize(numOfLayers);
        for(uint i = 0; i < numOfLayers; ++i)
        {
            deltaWeights[i].resize(dim[i]); // fill zeros
        }
    }

    double currentError = critError + 0.1;
    int type = -1;

    /// edit due to Indices
    std::vector<double> normCoeff;
    const double helpMin = *std::min_element(std::begin(*classCount),
                                             std::end(*classCount));
    for(uint i = 0; i < numCl; ++i)
    {
        normCoeff.push_back(helpMin / (*classCount)[i]);
    }

    epoch = 0;
    while(currentError > critError && epoch < epochLimit)
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        currentError = 0.0;
        // mix the sequence of input vectors
        std::shuffle(std::begin(indices),
                     std::end(indices),
                     std::default_random_engine(seed));

        for(const int index : indices)
        {
            loadVector(index, output[0], type);
            countOutput(output);
            countError(output, type, currentError);
            moveWeights(output, normCoeff, type);
        }
        ++epoch;
        //count error
        if(errType == errorNetType::SME)
        {
            currentError /= indices.size();
        }
    }
    std::cout << "epoch = " << epoch << "\t"
              << "error = " << smallLib::doubleRound(currentError, 4) << "\t"
              << "time elapsed = " << myTime.elapsed()/1000. << " sec"  << std::endl;
}

int ANN::getEpoch()
{
    return epoch;
}


void ANN::test(const std::vector<int> & indices)
{
    confusionMatrix.fill(0.);
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
//    averageClassification();
}

std::pair<int, double> ANN::classifyDatum(const int & vecNum)
{
    const int numOfLayers = dim.size();
    int type = -1;

    std::vector<std::valarray<double>> output(numOfLayers);
    for(int i = 0; i < numOfLayers; ++i)
    {
        output[i].resize(dim[i] + 1); // +1 for biases
    }

    loadVector(vecNum, output[0], type);
    countOutput(output);

    /// effect on successive procedure
    double res = 0.;
    countError(output, type, res);

    smallLib::resizeValar(output.back(), numCl);
    int outClass = indexOfMax(output.back());


#if 0
    /// cout results
    std::ofstream resFile;
    resFile.open((def::dir->absolutePath() +
                  slash + "class.txt").toStdString(),
                 ios_base::app);

    auto tmp = std::cout.rdbuf();
    cout.rdbuf(resFile.rdbuf());

    cout << "type = " << type << '\t' << "(";
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        cout << doubleRound(output[numOfLayers - 1][i], 3) << '\t';
    }
    cout << ") " << ((type == outClass) ? "+ " : "- ") << fileNames[vecNum] << endl;
    cout.rdbuf(tmp);

    resFile.close();
#endif

    return std::make_pair(outClass,
                          res);
}

const ANN::weightType & ANN::getWeight()
{
    return weight;
}

void ANN::writeWeight(const QString & wtsPath)
{
    static int wtsCounter = 0;
    std::ofstream weightsFile;
    if(wtsPath.isEmpty())
    {
        weightsFile.open((def::dir->absolutePath() + myLib::slash +
                         def::ExpName + "_" +
                         QString::number(wtsCounter++) + ".wts").toStdString());
    }
    else
    {
        weightsFile.open(wtsPath.toStdString());
    }

    if(!weightsFile.good())
    {
        /// namespace myLib
        std::cout << "ANN::writeWeight: cannot open file = " << wtsPath << std::endl;
        return;
    }

    for(uint i = 0; i < weight.size(); ++i) // numOfLayers
    {
        for(uint j = 0; j < weight[i].size(); ++j) // top layer
        {
            for(uint k = 0; k < weight[i][j].size(); ++k) // bot layer
            {
                weightsFile << weight[i][j][k] << '\n';
            }
            weightsFile << '\n';
        }
        weightsFile << '\n';
    }
    weightsFile.close();
}

void ANN::readWeight(const QString & fileName,
                     weightType * wtsMatrix)
{
    std::ifstream wtsStr;
    wtsStr.open(fileName.toStdString());
    if(!wtsStr.good())
    {
        std::cout << "ANN::readWeight: wtsPath is not good() " << std::endl;
        return;
    }

    if(wtsMatrix == nullptr)
    {
        wtsMatrix = &(this->weight);
    }
    allocWeight((*wtsMatrix));

    for(uint i = 0; i < weight.size(); ++i) // numOfLayers
    {
        for(uint j = 0; j < weight[i].size(); ++j) // top layer
        {
            for(uint k = 0; k < weight[i][j].size(); ++k) // bot layer (bias already inside)
            {
                wtsStr >> (*wtsMatrix)[i][j][k];
            }
        }
    }
    wtsStr.close();
}
