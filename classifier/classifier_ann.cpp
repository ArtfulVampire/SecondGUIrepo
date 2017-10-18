#include <classifier/classifier.h>

using namespace myOut;

ANN::ANN() : Classifier()
{
    myType = ModelType::ANN;
    typeString = "ANN";
}

void ANN::adjustToNewData()
{
	/// generality
	this->setDim({});
}

void ANN::setResetFlag(bool inFlag)
{
    this->resetFlag = inFlag;
}

void ANN::setCritError(double in)
{
    this->critError = in;
}


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
            // resizing std::valarray<double> -> fill zeros
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
			b = 0;
        }
    }
#endif

    for(auto & b : output)
    {
		b = 0.;
    }
}

void ANN::setDim(const std::vector<int> & inDim)
{
    dim.clear();
	dim.push_back(myClassData->getData().cols());
    for(uint i = 0; i < inDim.size(); ++i)
    {
        dim.push_back(inDim[i]);
    }
	dim.push_back(myClassData->getNumOfCl());

    allocParams(weight);

#if 0
	/// CHECK
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
	/// out.size() == myClassData->getData().cols() + 1
	std::copy(std::begin(myClassData->getData()[vecNum]),
			  std::end(myClassData->getData()[vecNum]),
              std::begin(output[0]));
	output[0][output[0].size() - 1] = 1.; // bias
	type = myClassData->getTypes()[vecNum]; // true class
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
#if WEIGHT_MATRIX
        output[i] = weight[i - 1] * output[i - 1];
        smLib::resizeValar(output[i], output[i].size() + 1);
        output[i][output[i].size() - 1] = 1;
#else
        for(int j = 0; j < dim[i]; ++j) // higher level, w/o bias
        {
            output[i][j] = smLib::prod(weight[i - 1][j], output[i - 1]); // bias included
        }
#endif
        output[i] = activation(output[i]);
        output[i][ dim[i] ] = 1.; // bias, unused for the highest layer
    }
	outputLayer = output.back();
	smLib::resizeValar(outputLayer, outputLayer.size() - 1);
}

void ANN::countOutputBackprop()
{
#if 0
    /// count deltaWeights
    /// CHECK
    // count deltaweights (used for backprop only)
    // for the last layer
    for(uint j = 0; j < dim[numOfLayers - 1]; ++j)
    {
        deltaWeights[numOfLayers-1][j] = -1. / temp
                                         * output[numOfLayers-1][j]
                                         * (1. - output[numOfLayers-1][j])
                * ((type == j) - output[numOfLayers-1][j]); // ~0.1
    }

    // for the other layers, besides the input one, upside->down
    for(int i = numOfLayers - 2; i >= 1; --i)
    {
        for(int j = 0; j < dim[i] + 1; ++j) // +1 for bias
        {
            deltaWeights[i][j] = 0.;
            for(int k = 0; k < dim[i + 1]; ++k) // connected to the higher-layer
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
		for(uint j = 0; j < myClassData->getNumOfCl(); ++j)
        {
            weight[0][j] += output[0]
                    * (learnRate * normCoeff[type]
                       * ((type == j) - output[1][j])
//                    * (output[1][j] * (1. - output[1][j])) * 6. // derivative
                    );
        }

    }
#if 0
	/// CHECK
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

void ANN::countError(uint trueType,
                     double & currentError)
{
	double err = clLib::countError(outputLayer, trueType);

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

    double currentError = critError + 0.1;
    uint type;

    /// edit due to Indices
	std::vector<int> localClassCount(myClassData->getNumOfCl(), 0);
    for(int index : indices)
    {
		++localClassCount[myClassData->getTypes()[index]];
    }
    const double helpMin = *std::min_element(std::begin(localClassCount),
                                             std::end(localClassCount));
    std::vector<double> normCoeff;
	for(uint i = 0; i < myClassData->getNumOfCl(); ++i)
    {
        normCoeff.push_back(helpMin / double(localClassCount[i]));
    }

    epoch = 0;

    while(currentError > critError && epoch < epochLimit)
	{
        currentError = 0.0;
		// mix the sequence of input vectors
		smLib::mix(indices);

        for(int index : indices)
		{
			loadVector(index, type);
			countOutput();
			countError(type, currentError);
			moveWeights(normCoeff, type);
		}
        ++epoch;

        // count error
        if(errType == errorNetType::SME)
        {
            currentError /= indices.size();
        }
	}
	/// std::cout epoch
//    std::cout << "epoch = " << epoch << "\t"
//              << "error = " << smLib::doubleRound(currentError, 4) << "\t"
//              << "time elapsed = " << myTime.elapsed()/1000. << " sec"  << std::endl;
}


void ANN::successiveRelearn()
{
    // decay weights
    const double rat = suc::decayRate;
    for(uint i = 0; i < dim.size() - 1; ++i)
    {
		for(auto & in : weight[i])
		{
			in *= 1. - rat;
		}
    }

    this->resetFlag = false;
	learnAll(); // relearn w/o weights reset
    this->resetFlag = true;
}

int ANN::getEpoch()
{
    return epoch;
}

void ANN::classifyDatum1(uint vecNum)
{
	uint type;
	loadVector(vecNum, type);
	countOutput(); // provides outputLayer

#if 0
	/// std::cout results
	const int numOfLayers = dim.size();
	std::ofstream resFile;
	resFile.open((def::dir->absolutePath() +
				  "/class.txt").toStdString(),
				 std::ios_base::app);

	/// uncomment to write to file
//    auto tmp = std::cout.rdbuf();
//    std::cout.rdbuf(resFile.rdbuf());

	std::cout << "type = " << type << '\t' << "(";
	for(int i = 0; i < myClassData->getNumOfCl(); ++i)
	{
		std::cout << smLib::doubleRound(output[numOfLayers - 1][i], 3) << '\t';
	}
	std::cout << ") " << ((type == outClass) ? "+ " : "- ") << "\t"
			  << myClassData->getFileNames()[vecNum] << std::endl;

//    std::cout.rdbuf(tmp);

	resFile.close();
#endif
}

const ANN::weightType & ANN::getWeight()
{
    return weight;
}

void ANN::writeWeight(const QString & wtsPath) const
{
//    static int wtsCounter = 0;
    std::ofstream weightsFile;
    if(wtsPath.isEmpty())
    {
		weightsFile.open((def::dir->absolutePath()
						  + "/Help/wts/"
						  + def::ExpName
//						  + "_" + nm(wtsCounter++)
						  + ".wts").toStdString());
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

	weightsFile << std::fixed;
	weightsFile.precision(4);

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

	/// first row with "numOfRows N	numOfCols M"
//	wtsStr.ignore(128, '\n');

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
				  + "/Help/wts/"
				  + def::ExpName + ".wts";
        if(!QFile::exists(wtsPath))
        {
			std::cout << "ANN::drawWeight: cant find wtsFile" << std::endl;
            return;
        }
    }
	twovector<std::valarray<double>> tempWeights; /// [0][3][NetLen]
	readWeight(wtsPath, &tempWeights);
    matrix drawWts; // 3 arrays of weights

    drawWts = tempWeights[0];
	drawWts.resizeCols(drawWts.cols() - 1); // fck the bias?

    if(picPath.isEmpty())
    {
        picPath = wtsPath;
        picPath.replace(".wts", "_wts.jpg"); /// make default suffixes
    }
	myLib::drawTemplate(picPath);
	myLib::drawArrays(picPath,
					  drawWts,
					  true);
}


double ANN::adjustLearnRate()
{
	std::vector<uint> mixNum = smLib::mixed<std::vector<uint>> (myClassData->getData().rows());

	/// fold
	if(0)
	{
		const int folds = 3;
		mixNum.resize(mixNum.size() * (folds - 1) / folds);
	}


	double res = learnRate;
    int counter = 0;
    do
    {

        /// remake with indices
        const double currVal = learnRate;

        this->learn(mixNum);

//		std::cout << "ANN::adjustLearnRate: lrate = " << learnRate << "\t"
//				  << "epoch = " << epoch << std::endl;

        /// check limits
        if(epoch > ANN::epochHighLimit ||
           epoch < ANN::epochLowLimit)
        {
			learnRate = currVal * sqrt(epoch /
									   ((ANN::epochLowLimit + ANN::epochHighLimit) / 2.));

			res = learnRate;
        }
        else
        {
			res = currVal;
            break;
        }

        /// check lrate values
		const double lowThr = 1e-8;
		const double highThr = 2;
		if(learnRate <= lowThr)
        {
			learnRate = lowThr; break;
        }
		else if(learnRate >= highThr)
        {
			learnRate = highThr; break;
        }
        ++counter;

	} while (counter < 20);

	std::cout << "ANN::adjustLearnRate: lrate = " << res
			  << "\t" << "epoch = " << epoch
			  << "\t" << "numIters = " << counter
			  << std::endl;

    return res;
}


