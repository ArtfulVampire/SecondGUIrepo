#include <myLib/wavelet.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>

using namespace myOut;

namespace wvlt
{

#if WAVELET_MATLAB
bool isInit = false;

int initMtlb()
{
	if(isInit) return 1;
	auto t0 = std::chrono::high_resolution_clock::now();
	std::cout << "matlab lib load starts" << std::endl;
	if (!libcwt_rInitializeWithHandlers(NULL, 0)){
		std::cout << "Could not initialize the application." << std::endl;
		return -1;
	}
	if (!libcwt_rInitialize()){
		std::cout << "Could not initialize the library." << std::endl;
		return -2;
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "matlab lib load ends" << std::endl
		 << "time elapsed = "
		 << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() / 1000.
		 << " sec" << std::endl;
	isInit = true;
	return 1;
}

int termMtlb()
{
	if(!isInit) return 1;
	try
	{
		libcwt_rTerminate();
	}
	catch(...)
	{
		return -1;
	}
	isInit = false;
	return 1;
}

matrix cwt(const std::valarray<double> & signal, double freq)
{
	if(!isInit)
	{
		initMtlb();
	}
	const int numFreqs = 19; /// number of frequencies in cwt.m (2:1:20)

	mxArray * srate = mxCreateDoubleScalar(freq);
	mxArray * inData = mxCreateDoubleMatrix(1, signal.size(), mxREAL);
	double * it = mxGetPr(inData);
	for(auto datum : signal)
	{
		*it = datum;
		++it;
	}
	mxArray * res = mxCreateDoubleMatrix(numFreqs, signal.size(), mxCOMPLEX);

	mlfCWT(1, &res, inData, srate);
	double * itt = mxGetPr(res);

	matrix result(numFreqs, signal.size());

	for(int col = 0; col < result.cols(); ++col)
	{
		for(int row = 0; row < result.rows(); ++row)
		{
			result[row][col] = *itt;
			++itt;
		}

	}
	mxDestroyArray(res);
	mxDestroyArray(inData);
	mxDestroyArray(srate);

	return result;
}

void drawWaveletMtlb(const matrix & inData, QString picPath)
{
	/// height and length in pixels
	const int barH = 30;
	const int barL = 1;

	QPixmap pic(inData.cols() * barL,
				inData.rows() * barH);
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	/// normalization to [0, 1]
	matrix drawData = inData;
	drawData += drawData.minVal();
	drawData *= wvlt::range / drawData.maxVal();

	for(int i = 0; i < inData.rows(); ++i)
	{
		for(int j = 0; j < inData.cols(); ++j)
		{
			painter.setBrush(QBrush(wvlt::hueJet(wvlt::range, drawData[i][j])));
			painter.setPen(wvlt::hueJet(wvlt::range, drawData[i][j]));

			painter.drawRect( j * barL,
							  pic.height() - (i + 1) * barH, // +1 'cause left UPPER
							  barL,
							  barH);
		}
	}
	painter.setPen("black");

	painter.setFont(QFont("Helvetica", 28, -1, -1));

	painter.setPen(Qt::DashLine);
	for(int i = 0; i < inData.rows(); ++i)
	{
		painter.drawLine(0,
						 i * barH,
						 pic.width(),
						 i * barH);
		painter.drawText(0,
						 pic.height() - (i + 0.5) * barH,
						 nm(i + 2)); /// +2 from scales [2:1:20]
	}

	painter.setPen(Qt::SolidLine);
	for(int i = 0; i < int(inData.cols() / def::freq); ++i)
	{
		painter.drawLine(i * def::freq * barL,
						 pic.height(),
						 i * def::freq * barL,
						 pic.height() - 20);
		painter.drawText(i * def::freq * barL - 8,
						 pic.height() - 2,
						 nm(i));

	}
	pic.save(picPath, 0, 100);
	painter.end();
}
#endif




const int timeStep = ceil(0.02 * def::freq);
const double freqMax = 20.; // def::rightFreq
const double freqMin = 2.; // def::leftFreq
const double freqStep = 1.;
const int range = 1024;
const double pi_sqrt = sqrt(M_PI);

#if !WAVELET_FREQ_STEP_TYPE
const int numberOfFreqs = int(log(wvlt::freqMin/wvlt::freqMax) / log(wvlt::freqStep)) + 1;
#else
const int numberOfFreqs = int((wvlt::freqMax - wvlt::freqMin) / wvlt::freqStep) + 1;
#endif



double const morletFall = 9.; // coef in matlab = mF^2 / (2 * pi^2);
double morletCosNew(double const freq1, // Hz
					const double timeShift,
					const double time)
{
	double freq = freq1 * 2. * pi;
	double res =  sqrt(2. * freq / pi_sqrt / morletFall)
				  * cos(freq * (time - timeShift) / def::freq)
				  * exp(-0.5 * pow(freq / morletFall * (time - timeShift) / def::freq, 2));
	return res;
}

double morletSinNew(double const freq1,
					const double timeShift,
					const double time)
{
	double freq = freq1 * 2. * pi;
	double res =  sqrt(2. * freq / pi_sqrt / morletFall)
				  * sin(freq * (time - timeShift) / def::freq)
				  * exp(-0.5 * pow(freq / morletFall * (time - timeShift) / def::freq, 2));
	return res;
}

void wavelet(QString filePath,
			 QString picPath,
			 int channelNumber,
			 int ns)
{
	// continious
	double helpDouble;

	matrix fileData;
	myLib::readPlainData(filePath, fileData);

	std::valarray<double> input = fileData[channelNumber];

	QPixmap pic(fileData.cols(), 1000);
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	matrix temp;
	temp.resize(wvlt::numberOfFreqs,
				ceil(fileData.cols() / wvlt::timeStep) + 1);
	temp.fill(0.);

	//    mat temp;
	//    temp.resize(wvlt::numberOfFreqs);
	//    const int num = ceil(fileData.cols() / wvlt::timeStep);
	//    std::for_each(temp.begin(),
	//                  temp.end(),
	//                  [num](vec & in){in.resize(num, 0.); });

	double tempR = 0., tempI = 0.;
	int kMin = 0, kMax = 0;

	double numb;

	int currFreqNum = 0;
	int currSliceNum = 0;

	for(double freq = wvlt::freqMax;
	   freq > wvlt::freqMin;
	#if WAVELET_FREQ_STEP_TYPE==0
	   freq *= wvlt::freqStep)
	#else
	   freq -= wvlt::freqStep)
#endif
	{
		//        timeStep = def::freqFreq / 2.5;  // in time-bins 250 Hz
		currSliceNum = 0;
		for(int currSlice = 0; currSlice < fileData.cols(); currSlice += wvlt::timeStep)
		{
			temp[currFreqNum][currSliceNum] = 0.;
			tempR = 0.;
			tempI = 0.;

			/////// TO LOOK
			// set left & right limits of counting - should be 2.5 * morletFall... but works so
			kMin = std::max(0, int(currSlice - 3 * morletFall * def::freq / freq));
			kMax = std::min(int(fileData.cols()), int(currSlice + 3 * morletFall * def::freq / freq));

			for(int k = kMin; k < kMax; ++k)
			{
				tempI += (morletSinNew(freq, currSlice, k) * input[k]);
				tempR += (morletCosNew(freq, currSlice, k) * input[k]);
			}
			temp[currFreqNum][currSliceNum] = pow(tempI, 2) + pow(tempR, 2);
			++currSliceNum;
		}
		++currFreqNum;
	}

	// maximal value from temp matrix
	helpDouble = temp.maxVal();
	//    helpDouble = 800000;

	//    std::cout << helpDouble << std::endl;
	//    helpDouble = 1e5;

	//    /// test for maxVal
	//    ofstream str;
	//    str.open("/media/Files/Data/wav.txt", ios_base::app);
	//    str << helpDouble << std::endl;
	//    str.close();
	//    return;


	currFreqNum = 0;
	for(double freq = wvlt::freqMax;
	   freq > wvlt::freqMin;
	#if WAVELET_FREQ_STEP_TYPE==0
	   freq *= wvlt::freqStep)
	#else
	   freq -= wvlt::freqStep)
#endif
	{
		//        timeStep = def::freqFreq / 2.5;  // in time-bins 250 Hz

		currSliceNum = 0;
		for(int currSlice = 0; currSlice < fileData.cols(); currSlice += wvlt::timeStep)
		{
			//            std::cout << temp[currFreqNum][currSliceNum] << std::endl;
			numb = fmin(floor(temp[currFreqNum][currSliceNum] / helpDouble * wvlt::range),
						wvlt::range);

			//             numb = pow(numb/wvlt::range, 0.8) * wvlt::range; // sligthly more than numb, may be dropped

			painter.setBrush(QBrush(wvlt::hueJet(wvlt::range, numb)));
			painter.setPen(wvlt::hueJet(wvlt::range, numb));

#if WAVELET_FREQ_STEP_TYPE==0
			painter.drawRect( currSlice * pic.width() / fileData.cols(),
							  pic.height() * (wvlt::freqMax - freq
											  + 0.5 * freq *
											  (1. - wvlt::freqStep) / wvlt::freqStep)
							  / (wvlt::freqMax - wvlt::freqMin),
							  pic.width() * wvlt::timeStep / fileData.cols(),
							  pic.height() * -0.5 *Freq * (1. / wvlt::freqStep - wvlt::freqStep)
							  / (wvlt::freqMax - wvlt::freqMin) );
#else

			painter.drawRect( pic.width() * currSlice / fileData.cols(),
							  pic.height() * (wvlt::freqMax - freq  - 0.5 * wvlt::freqStep)
							  / (wvlt::freqMax - wvlt::freqMin),
							  pic.width() * wvlt::timeStep / fileData.cols(),
							  pic.height() * wvlt::freqStep / (wvlt::freqMax - wvlt::freqMin));
#endif
			++currSliceNum;
		}
		++currFreqNum;

	}
	painter.setPen("black");

	painter.setFont(QFont("Helvetica", 28, -1, -1));
	painter.setPen(Qt::DashLine);
	for(int i = wvlt::freqMax; i > wvlt::freqMin; --i)
	{

		painter.drawLine(0,
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin),
						 pic.width(),
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin));
		painter.drawText(0,
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin) - 2,
						 nm(i));

	}
	painter.setPen(Qt::SolidLine);
	for(int i = 0; i < int(fileData.cols() / def::freq); ++i)
	{
		painter.drawLine(pic.width() * i * def::freq / fileData.cols(),
						 pic.height(),
						 pic.width() * i * def::freq / fileData.cols(),
						 pic.height() - 20);
		painter.drawText(pic.width() * i * def::freq / fileData.cols() - 8,
						 pic.height() - 2,
						 nm(i));

	}
	pic.save(picPath, 0, 100);
	painter.end();
}



template <typename signalType>
matrix countWavelet(const signalType & inSignal)
{
	// continious
	int NumOfSlices = inSignal.size();

	matrix res;
	res.resize(wvlt::numberOfFreqs,
			   ceil(NumOfSlices / wvlt::timeStep) + 1);
	res.fill(0.);

	double tempR = 0., tempI = 0.;
	int kMin = 0, kMax = 0;

	int currFreqNum = 0;
	int currSliceNum = 0;

	for(double freq = wvlt::freqMax;
	   freq > wvlt::freqMin;
	#if WAVELET_FREQ_STEP_TYPE==0
	   freq *= wvlt::freqStep
	#else
	   freq -= wvlt::freqStep)
#endif
	{
		//        timeStep = def::freqFreq / 2.5;  // in time-bins 250 Hz
		currSliceNum = 0;
		for(int currSlice = 0; currSlice < NumOfSlices; currSlice += wvlt::timeStep)
		{
			res[currFreqNum][currSliceNum] = 0.;
			tempR = 0.;
			tempI = 0.;

			/////// TO LOOK
			// set left & right limits of counting - should be 2.5 * morletFall... but works so
			kMin = std::max(0, int(currSlice - 3 * morletFall * def::freq / freq));
			kMax = std::min(NumOfSlices, int(currSlice + 3 * morletFall * def::freq / freq));

			for(int k = kMin; k < kMax; ++k)
			{
				tempI += (morletSinNew(freq, currSlice, k) * inSignal[k]);
				tempR += (morletCosNew(freq, currSlice, k) * inSignal[k]);
			}
			res[currFreqNum][currSliceNum] = (pow(tempI, 2) + pow(tempR, 2)) / 1e5; // ~1

			++currSliceNum;
		}
		++currFreqNum;
	}
	// no normalization
	return res;
}



double red(int range, double j, double V, double S)
{
	double part = j / double(range);
	// matlab
	if    (0. <= part && part <= wvlt::colDots[0]) return V*(1.-S);
	else if(wvlt::colDots[0] < part && part <= wvlt::colDots[1]) return V*(1.-S);
	else if(wvlt::colDots[1] < part && part <= wvlt::colDots[2]) return V*(1.-S) + V*S*(part-wvlt::colDots[1])/(wvlt::colDots[2] - wvlt::colDots[1]);
	else if(wvlt::colDots[2] < part && part <= wvlt::colDots[3]) return V;
	else if(wvlt::colDots[3] < part && part <= 1.) return V - V*S*(part-wvlt::colDots[3])/(1 - wvlt::colDots[3])/2.;
	// old
	if    (0.000 <= part && part <= 0.167) return V*(1.-S); /// 2. - V*S/2. + V*S*(part)*3.;
	else if(0.167 < part && part <= 0.400) return V*(1.-S);
	else if(0.400 < part && part <= 0.500) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
	else if(0.500 < part && part <= 0.600) return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.;
	else if(0.600 < part && part <= 0.833) return V;
	else if(0.833 < part && part <= 1.000) return V - V*S*(part-0.833)/(1.000-0.833)/2.;
	else return 0.0;
}
double green(int range, double j, double V, double S)
{
	double part = j / double(range);
	double hlp = 1.0;
	// matlab
	if    (0.0 <= part && part <= wvlt::colDots[0]) return V*(1.-S);
	else if(wvlt::colDots[0] < part && part <= wvlt::colDots[1]) return V*(1.-S) + V*S*(part-wvlt::colDots[0])/(wvlt::colDots[1] - wvlt::colDots[0]);
	else if(wvlt::colDots[1] < part && part <= wvlt::colDots[2]) return V;
	else if(wvlt::colDots[2] < part && part <= wvlt::colDots[3]) return V - V*S*(part-wvlt::colDots[2])/(wvlt::colDots[3] - wvlt::colDots[2]);
	else if(wvlt::colDots[3] < part && part <= 1.) return V*(1.-S);
	// old
	if    (0.000 <= part && part <= 0.167) return V*(1.-S);
	else if(0.167 < part && part <= 0.400) return V*(1.-S) + V*S*hlp*(part-0.167)/(0.400-0.167);
	else if(0.400 < part && part <= 0.500) return V-V*S*(1.-hlp);
	else if(0.500 < part && part <= 0.600) return V-V*S*(1.-hlp);
	else if(0.600 < part && part <= 0.833) return V-V*S*(1.-hlp) - V*S*hlp*(part-0.600)/(0.833-0.600);
	else if(0.833 < part && part <= 1.000) return V*(1.-S);
	else return 0.0;
}

double blue(int range, double j, double V, double S)
{
	double part = j / double(range);

	if    (0.0 <= part && part <= wvlt::colDots[0]) return V -V*S/2. + V*S*(part)/(wvlt::colDots[0] - 0.0)/2.;
	else if(wvlt::colDots[0] < part && part <= wvlt::colDots[1]) return V;
	else if(wvlt::colDots[1] < part && part <= wvlt::colDots[2]) return V - V*S*(part-wvlt::colDots[1])/(wvlt::colDots[2] - wvlt::colDots[1]);
	else if(wvlt::colDots[2] < part && part <= wvlt::colDots[3]) return V*(1.-S);
	else if(wvlt::colDots[3] < part && part <= 1.) return V*(1.-S);

	// old
	if    (0.000 <= part && part <= 0.167) return V -V*S/2. + V*S*(part)/(0.167-0.000)/2.;
	else if(0.167 < part && part <= 0.400) return V;
	else if(0.400 < part && part <= 0.500) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
	else if(0.500 < part && part <= 0.600) return V - V*S*(part-0.400)/(0.500-0.400)/2.;
	else if(0.600 < part && part <= 0.833) return V*(1.-S);
	else if(0.833 < part && part <= 1.000) return V*(1.-S);


	else return 0.0;
}

QColor hueJet(int range, double j)
{
	if(j > range) j = range; // bicycle for no black colour
	if(j < 0) j = 0; // bicycle for no black colour
	//    return QColor(255.*red1(range,j), 255.*green1(range,j), 255.*blue1(range,j));
	return QColor(255. * red(range, j),
				  255. * green(range, j),
				  255. * blue(range, j));
}

void drawWavelet(QString picPath,
				 const matrix & inData)
{
	int NumOfSlices = ceil(inData.cols() * wvlt::timeStep);
	QPixmap pic(NumOfSlices, 1000);
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	double numb = 0.;

	int currFreqNum = 0;
	int currSliceNum = 0;

	for(double freq = wvlt::freqMax;
	   freq > wvlt::freqMin;
	#if WAVELET_FREQ_STEP_TYPE==0
	   freq *= wvlt::freqStep
	#else

	   freq -= wvlt::freqStep)
#endif
	{
		currSliceNum = 0;
		for(int currSlice = 0; currSlice < NumOfSlices; currSlice += wvlt::timeStep)
		{
			numb = fmin(floor(inData[currFreqNum][currSliceNum] * wvlt::range), wvlt::range);

			// sligthly more than numb, may be dropped
			numb = pow(numb/wvlt::range, 0.6) * wvlt::range;

			painter.setBrush(QBrush(wvlt::hueJet(wvlt::range, numb)));
			painter.setPen(wvlt::hueJet(wvlt::range, numb));

#if WAVELET_FREQ_STEP_TYPE==0
			painter.drawRect( currSlice * pic.width() / NumOfSlices,
							  pic.height() * (wvlt::freqMax - freq
											  + 0.5 * freq *
											  (1. - wvlt::freqStep) / wvlt::freqStep)
							  / (wvlt::freqMax-wvlt::freqMin),
							  pic.width() * wvlt::timeStep / NumOfSlices,
							  pic.height() * -0.5 * freq * (1. / wvlt::freqStep - wvlt::freqStep)
							  / (wvlt::freqMax - wvlt::freqMin) );
#else
			painter.drawRect( currSlice * pic.width() / NumOfSlices,
							  pic.height() * (wvlt::freqMax - freq  - 0.5 * wvlt::freqStep)
							  / (wvlt::freqMax - wvlt::freqMin),
							  pic.width() * wvlt::timeStep / NumOfSlices,
							  pic.height() * wvlt::freqStep / (wvlt::freqMax - wvlt::freqMin));
#endif
			++currSliceNum;
		}
		++currFreqNum;

	}
	painter.setPen("black");


	painter.setFont(QFont("Helvetica", 28, -1, -1));
	painter.setPen(Qt::DashLine);
	for(int i = wvlt::freqMax; i > wvlt::freqMin; --i)
	{

		painter.drawLine(0,
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin),
						 pic.width(),
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin));
		painter.drawText(0,
						 pic.height() * (wvlt::freqMax - i) / (wvlt::freqMax - wvlt::freqMin) - 2,
						 nm(i));

	}
	painter.setPen(Qt::SolidLine);
	for(int i = 0; i < int(NumOfSlices / def::freq); ++i)
	{
		painter.drawLine(pic.width() * i * def::freq / NumOfSlices,
						 pic.height(),
						 pic.width() * i * def::freq / NumOfSlices,
						 pic.height() - 20);
		painter.drawText(pic.width() * i * def::freq / NumOfSlices - 8,
						 pic.height() - 2,
						 nm(i));

	}
	pic.save(picPath, 0, 100);
	painter.end();
}


template matrix countWavelet(const std::valarray<double> & inSignal);
template matrix countWavelet(const std::vector<double> & inSignal);




}
