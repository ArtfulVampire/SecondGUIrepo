#include "classifier.h"
using namespace myLib;

ANN::ANN() : Classifier()
{
    myType = ClassifierType::ANN;
    typeString = "ANN";
}

void ANN::setResetFlag(bool inFlag)
{
    this->resetFlag = inFlag;
}

void ANN::setCritError(double in)
{
    this->critError = in;
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


/// not always for this->weight for drawWts
void ANN::allocParams(weightType & inMat)
{
    inMat.resize(dim.size() - 1);
    for(uint i = 0; i < inMat.size(); ++i) // weights to layer i+1 from i
    {
#if WEIGHT_MATRIX
        inMat[i] = matrix(dim[i + 1], dim[i] + 1, 0.);
#else
        inMat[i].resize(dim[i + 1]);
        for(auto & b : inMat[i]) // to j'th in i+1 layer
        {
            // resizing lineType -> fill zeros
            b.resize(dim[i] + 1); // from k'th in i layer + bias
        }        
#endif
    }

    if(&inMat != &(this->weight)) /// happens only in drawings
    {
        return;
    }

    output.resize(dim.size());
    int i = 0;
    for(auto & b : output)
    {
        b.resize(dim[i] + 1); // +1 for biases
        ++i;
    }

    if(learnStyl == learnStyle::backprop)
    {
        deltaWeights.resize(dim.size());
        for(uint i = 0; i < dim.size(); ++i)
        {
            deltaWeights[i].resize(dim[i]); // fill zeros
        }
    }
}

void ANN::zeroParams()
{
#if WEIGHT_MATRIX
    for(auto & a : weight)
    {
        a.fill(0.);
    }
#else
    for(auto & a : weight)
    {
        for(auto & b : a)
        {
            std::fill(std::begin(b),
                      std::end(b),
                      0.);
        }
    }
#endif

    for(auto & b : output)
    {
        std::fill(std::begin(b),
                  std::end(b),
                  0.);
    }
}

void ANN::setDim(const std::vector<int> & inDim)
{
    dim.clear();
    dim.push_back(dataMatrix->cols());
    for(uint i = 0; i < inDim.size(); ++i)
    {
        dim.push_back(inDim[i]);
    }
    dim.push_back(numCl);

    allocParams(weight);

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



void ANN::loadVector(uint vecNum, uint & type)
{
    /// out.size() == dataMatrix->cols() + 1
    std::copy(std::begin((*dataMatrix)[vecNum]),
              std::end((*dataMatrix)[vecNum]),
              std::begin(output[0]));
    output[0][output[0].size() - 1] = 1.; //bias
    type = (*types)[vecNum]; // true class
}

void ANN::countOutput()
{
    /// switch case
    if(learnStyl == learnStyle::delta)
    {
        countOutputDelta();
    }
    else if(learnStyl == learnStyle::backprop)
    {
        countOutputBackprop();
    }
}

void ANN::countOutputDelta()
{
    for(uint i = 1; i < dim.size(); ++i)
    {
//        std::cout << "pew" << std::endl;
#if WEIGHT_MATRIX
        output[i] = weight[i - 1] * output[i - 1];
        smallLib::resizeValar(output[i], output[i].size() + 1);
        output[i][output[i].size() - 1] = 1;
#else
        for(int j = 0; j < dim[i]; ++j) //higher level, w/o bias
        {
            output[i][j] = smallLib::prod(weight[i - 1][j], output[i - 1]); // bias included
        }
#endif
//        std::cout << "pewpew" << std::endl;
        output[i] = activation(output[i]);
        output[i][ dim[i] ] = 1.; //bias, unused for the highest layer
    }
}

void ANN::countOutputBackprop()
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

void ANN::moveWeights(const std::vector<double> & normCoeff,
                      const uint type)
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

void ANN::countError(uint type,
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

void ANN::learn(std::vector<uint> & indices)
{
    QTime myTime;
    myTime.start();

    if(this->weight.empty() || this->output.empty())
    {
        allocParams(weight);
    }

    if(this->resetFlag)
    {
        zeroParams();
    }

//    std::cout << "asdkjfjkrgwegb" << std::endl;

    double currentError = critError + 0.1;
    uint type;

    /// edit due to Indices
    std::vector<double> normCoeff;
    const double helpMin = *std::min_element(std::begin(*classCount),
                                             std::end(*classCount));

    for(uint i = 0; i < numCl; ++i)
    {
        normCoeff.push_back(helpMin / (*classCount)[i]);
    }

    epoch = 0;

//    std::cout << "asdkjfjkrgwegb" << std::endl;
    while(currentError > critError && epoch < epochLimit)
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        currentError = 0.0;
        // mix the sequence of input vectors
        std::shuffle(std::begin(indices),
                     std::end(indices),
                     std::default_random_engine(seed));


//        std::cout << "asdkjfjkrgwegb" << std::endl;
        for(int index : indices)
        {

//            std::cout << 123 << std::endl;
            loadVector(index, type);
//            std::cout << 124 << std::endl;
            countOutput();
//            std::cout << 125 << std::endl;
            countError(type, currentError);
//            std::cout << 126 << std::endl;
            moveWeights(normCoeff, type);
//            std::cout << 127 << std::endl;
        }

//        std::cout << "asdkjfjkrgwegb" << std::endl;
        ++epoch;
        //count error
        if(errType == errorNetType::SME)
        {
            currentError /= indices.size();
        }
    }
    /// cout epoch
//    std::cout << "epoch = " << epoch << "\t"
//              << "error = " << smallLib::doubleRound(currentError, 4) << "\t"
//              << "time elapsed = " << myTime.elapsed()/1000. << " sec"  << std::endl;
}


void ANN::successiveRelearn()
{
    // decay weights
    const double rat = suc::decayRate;
    for(uint i = 0; i < dim.size() - 1; ++i)
    {
        std::for_each(std::begin(weight[i]),
                      std::end(weight[i]),
                      [rat](lineType & in)
        {
            in *= 1. - rat;
        });
    }

    this->resetFlag = false;
    learnAll(); // relearn w/o weights reset
    this->resetFlag = true;
}

int ANN::getEpoch()
{
    return epoch;
}


#if CLASS_TEST_VIRTUAL
void ANN::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<uint, double> ANN::classifyDatum(const uint & vecNum)
{
    uint type;

    loadVector(vecNum, type);
    countOutput();

    /// effect on successive procedure
    double res = 0.;
    countError(type, res);

    std::valarray<double> forRes = output.back();
    smallLib::resizeValar(forRes, numCl);
    uint outClass = myLib::indexOfMax(forRes);

#if 01
    /// cout results
    const int numOfLayers = dim.size();
    std::ofstream resFile;
    resFile.open((def::dir->absolutePath() +
                  slash + "class.txt").toStdString(),
                 std::ios_base::app);

    ///uncomment to write to file
//    auto tmp = std::cout.rdbuf();
//    std::cout.rdbuf(resFile.rdbuf());

    std::cout << "type = " << type << '\t' << "(";
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        std::cout << smallLib::doubleRound(output[numOfLayers - 1][i], 3) << '\t';
    }
    std::cout << ") " << ((type == outClass) ? "+ " : "- ") << "\t"
              << (*fileNames)[vecNum] << std::endl;

//    std::cout.rdbuf(tmp);

    resFile.close();
#endif

    deleteFile(vecNum, outClass);

    printResult("ANN.txt", outClass, vecNum);
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
    allocParams((*wtsMatrix));

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


void ANN::drawWeight(QString wtsPath,
                     QString picPath)
{
    if( dim.size() != 2 ) return;

    if(!QFile::exists(wtsPath))
    {
        wtsPath = def::dir->absolutePath()
                  + slash + def::ExpName + ".wts";
        if(!QFile::exists(wtsPath))
        {
            std::cout << "ANN::drawWeight: cant find wtsFile" << std::endl;
            return;
        }
    }
    twovector<lineType> tempWeights;
    readWeight(wtsPath, &tempWeights);
    matrix drawWts; // 3 arrays of weights

    drawWts = tempWeights[0];
    drawWts.resizeCols(drawWts.cols() - 1); // fck the bias?

    if(picPath.isEmpty())
    {
        picPath = wtsPath;
        picPath.replace(".wts", "_wts.jpg"); /// make default suffixes
    }
    drawTemplate(picPath);
    drawArrays(picPath,
               drawWts,
               true);
}


double ANN::adjustLearnRate()
{
    std::vector<uint> mixNum;
    mixNum.resize(dataMatrix->rows());
    std::iota(mixNum.begin(), mixNum.end(), 0);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(mixNum.begin(),
                 mixNum.end(),
                 std::default_random_engine(seed));
    /// const
    const int folds = 3;
    mixNum.resize(mixNum.size() * (folds - 1) / folds);


    double res = 1.;
    int counter = 0;
    do
    {
        /// remake with indices
        const double currVal = learnRate;

        this->learn(mixNum);

        /// check limits
        if(epoch > ANN::epochHighLimit ||
           epoch < ANN::epochLowLimit)
        {
            learnRate = (currVal * sqrt(epoch /
                                        ((ANN::epochLowLimit + ANN::epochHighLimit) / 2.)));
        }
        else
        {
            res = currVal;
            break;
        }

        /// check lrate values
        if(learnRate <= 0.001)
        {
            learnRate = 0.001; break;
        }
        else if(learnRate >= 1.)
        {
            learnRate = 1.; break;
        }
        ++counter;
    } while (counter < 10);

    std::cout << "ANN::adjustLearnRate: lrate = " << res << "\t"
              << "epoch = " << epoch << std::endl;

    return res;
}


