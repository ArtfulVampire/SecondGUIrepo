#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/output.h>

using namespace myOut;



void Cut::smartFindLearnSlot()
{
	if( !fileOpened ) { return; }

	/// take marked range
	matrix sub = dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
									  ui->rightLimitSpinBox->value());

	/// cut windows
	for(int i = 0; i < sub.cols() / smartFindWindLen; ++i)
	{
		smartFindLearnData.push_back(sub.subCols(smartFindWindLen * i, smartFindWindLen * (i + 1)));
	}

	/// count params
	if(smartFindLearnData.size() >= 50) /// if dataset if big enough
	{
		smartFindThresholds.resize(smartFindNumCh);
		for(int ch = 0; ch < smartFindNumCh; ++ch)
		{
			smartFindThresholds[ch].resize(smartFindFuncs.size());
			for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
			{
				thrParam param;

				std::valarray<double> vals(smartFindLearnData.size());
				for(int windNum = 0; windNum < smartFindLearnData.size(); ++windNum)
				{
					vals[windNum] = smartFindFuncs[numFunc] (smartFindLearnData[windNum][ch]);
				}

//				param.name = paramNames[numFunc];
				param.numParam = numFunc;
				param.mean = smLib::mean(vals);
				param.sigma = (smLib::sigma(vals) != 0) ? smLib::sigma(vals) : 1.;
				param.numChan = ch;
				smartFindThresholds[ch][numFunc] = param;
			}
		}
		std::cout << "smartFindLearnSlot: learning done" << std::endl;
		smartFindCountParams();
	}
	else
	{
		std::cout << "smartFindLearnSlot: learning set size = " << smartFindLearnData.size()
				  << " (need 50)" << std::endl;
	}
	resetLimits();
	paint();
}

void Cut::smartFindCountParams()
{
	// counts for all windows in record

	if( !fileOpened ) { return; }

	smartFindWindParams.resize(dataCutLocal.cols() / smartFindWindLen); // each window
	for(int windNum = 0; windNum < smartFindWindParams.size(); ++windNum)
	{
		smartFindWindParams[windNum].resize(smartFindNumCh);

		matrix localData = dataCutLocal.subCols(smartFindWindLen * windNum,
												smartFindWindLen * (windNum + 1));
		for(int ch = 0; ch < smartFindNumCh; ++ch)
		{
			smartFindWindParams[windNum][ch].resize(smartFindFuncs.size());
			for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
			{
				smartFindWindParams[windNum][ch][numFunc] = smartFindFuncs[numFunc] (localData[ch]);
			}
		}
	}

#if 0
	/// cout max values
	for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
	{
		double maxVal = 0.;
		int maxCh = -1;
		int maxWind = -1;
		for(int ch = 0; ch < smartFindNumCh; ++ch)
		{
			for(int windNum = 0; windNum < smartFindWindParams.size(); ++windNum)
			{
				if(smartFindWindParams[windNum][ch][numFunc] > maxVal)
				{
					maxVal = smartFindWindParams[windNum][ch][numFunc];
					maxCh = ch;
					maxWind = windNum;
				}
			}
		}
		std::cout << "max " << paramNames[numFunc] << " = " << maxVal << "\t"
				  << "ch = " << maxCh << "\t"
				  << "wind = " << maxWind << "\t"
				  << "time = " << maxWind * smartFindWindLen / edfFil.getFreq() << "\t"
				  << std::endl;
	}
#endif


	std::cout << "smartFindCountParams: done" << std::endl;
}

void Cut::smartFindSetFuncs()
{
	static const double norm = myLib::spectreNorm(smLib::fftL(smartFindWindLen),
												  smartFindWindLen,
												  edfFil.getFreq());
	/// theta
	if(1)
	{
		smartFindFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, smartFindWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(4, edfFil.getFreq(), smartFindWindLen),
								   std::begin(spec) + fftLimit(8, edfFil.getFreq(), smartFindWindLen),
								   0.);
		});
		paramNames.push_back("theta");
		paramSigmaThreshold.push_back(15); /// check and adjust
		paramAbsThreshold.push_back(100); /// check and adjust
	}

	/// alpha
	if(1)
	{
		smartFindFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, smartFindWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(8, edfFil.getFreq(), smartFindWindLen),
								   std::begin(spec) + fftLimit(13, edfFil.getFreq(), smartFindWindLen),
								   0.);
		});
		paramNames.push_back("alpha");
		paramSigmaThreshold.push_back(10); /// check and adjust
		paramAbsThreshold.push_back(100); /// check and adjust
	}


	/// beta
	if(1)
	{
		smartFindFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, smartFindWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(13, edfFil.getFreq(), smartFindWindLen),
								   std::begin(spec) + fftLimit(25, edfFil.getFreq(), smartFindWindLen),
								   0.);
		});
		paramNames.push_back("beta");
		paramSigmaThreshold.push_back(10); /// check and adjust
		paramAbsThreshold.push_back(70); /// check and adjust
	}

	/// gamma
	if(1)
	{
		smartFindFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, smartFindWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(25, edfFil.getFreq(), smartFindWindLen),
								   std::begin(spec) + fftLimit(40, edfFil.getFreq(), smartFindWindLen),
								   0.);
		});
		paramNames.push_back("gamma");
		paramSigmaThreshold.push_back(15); /// check and adjust
		paramAbsThreshold.push_back(80); /// check and adjust
	}

	/// max ampl
	if(1)
	{
		smartFindFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			return std::max(std::abs(in.max()), std::abs(in.min()));
		});
		paramNames.push_back("maxAmp");
		paramSigmaThreshold.push_back(10); /// check and adjust
		paramAbsThreshold.push_back(35); /// check and adjust
	}

	/// integral
	if(1)
	{
#if 01
		smartFindFuncs.push_back(smLib::variance<std::valarray<double>>);
#else
		smartFindFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, smartFindWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(4, edfFil.getFreq(), smartFindWindLen),
								   std::begin(spec) + fftLimit(40, edfFil.getFreq(), smartFindWindLen),
								   0.);
		});
#endif
		paramNames.push_back("intgrl");
		paramSigmaThreshold.push_back(10); /// check and adjust
		paramAbsThreshold.push_back(100); /// check and adjust
	}

}


void Cut::smartFindShowValues()
{

	int windNum = ui->leftLimitSpinBox->value() / smartFindWindLen;
	ui->leftLimitSpinBox->setValue(windNum * smartFindWindLen);
	ui->rightLimitSpinBox->setValue((windNum + 1) * smartFindWindLen);
#if 0
	for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
	{
		double maxAbs = 0;
		int indexAbs = 0;
		double maxSgm = 0;
		int indexSgm = 0;

		for(int ch = 0; ch < smartFindNumCh; ++ch)
		{
			double abs_ = smartFindWindParams[windNum][ch][numFunc];
			double sgm_ = std::abs(smartFindWindParams[windNum][ch][numFunc]
								  - smartFindThresholds[ch][numFunc].mean)
			 /  smartFindThresholds[ch][numFunc].sigma;

			if(abs_ > maxAbs) { maxAbs = abs_; indexAbs = ch; }
			if(sgm_ > maxSgm) { maxSgm = sgm_; indexSgm = ch; }

		}
		std::cout << paramNames[numFunc] << "\t"
				  << "maxAbs = " << maxAbs << " in " << edfFil.getLabels(indexAbs)
				  << "maxSgm = " << maxSgm << " in " << edfFil.getLabels(indexSgm)
				  << std::endl;
	}
	std::cout << std::endl;
#else
	std::cout << std::fixed;
	std::cout.precision(2);

	std::cout << "         " << "\t";
	for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
	{
		std::cout << paramNames[numFunc] << "\t";
		std::cout << paramNames[numFunc] << "\t";
	}
	std::cout << std::endl;


	for(int ch = 0; ch < smartFindNumCh; ++ch)
	{
		std::cout << edfFil.getLabels(ch);
		for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
		{
			double abs_ = smartFindWindParams[windNum][ch][numFunc];
			double sgm_ = std::abs(smartFindWindParams[windNum][ch][numFunc]
								  - smartFindThresholds[ch][numFunc].mean)
			 /  smartFindThresholds[ch][numFunc].sigma;

			std::cout << abs_ << "\t"
					  << sgm_ << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif
}

void Cut::smartFindFind(bool forward)
{
	if( !fileOpened ) { return; }

	const bool checkSigmas = true;

	if(smartFindLearnData.empty() || smartFindThresholds.empty() || smartFindWindParams.empty())
	{
		std::cout << "smartFindFind: data not ready" << std::endl;
		return;
	}
	double timeLeftGap = 1.0;
	int windNum = std::round((leftDrawLimit + timeLeftGap * edfFil.getFreq()) / smartFindWindLen)
						 + (forward ? 1 : -1);
	bool proceed = true;
	while(proceed)
	{
		for(int ch = 0; ch < smartFindNumCh; ++ch)
		{
			for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
			{
//				std::cout << "numFunc = " << numFunc << std::endl;
//				std::cout << "mean = " << thrParams[ch][numFunc].mean << std::endl;
//				std::cout << "sigma = " << thrParams[ch][numFunc].sigma << std::endl;
//				std::cout << "val = " << windParams[windNum][ch][numFunc] << std::endl;
				double numSigmas = std::abs(smartFindWindParams[windNum][ch][numFunc]
											- smartFindThresholds[ch][numFunc].mean)
								   /  smartFindThresholds[ch][numFunc].sigma;

				if(numSigmas > paramSigmaThreshold[numFunc] && checkSigmas) /// add spin box
//				if(numSigmas > 10. && checkSigmas) /// add spin box
				{
					std::cout << paramNames[numFunc] << "\t"
							  << edfFil.getLabels(ch) << "\t"
							  << "sgm" << "\t"
							  << numSigmas << "\t"
							  << std::endl;
					proceed = false;
				}
				else if(smartFindWindParams[windNum][ch][numFunc] > paramAbsThreshold[numFunc])
				{
					std::cout << paramNames[numFunc] << "\t"
							  << edfFil.getLabels(ch) << "\t"
							  << "abs" << "\t"
							  << smartFindWindParams[windNum][ch][numFunc] << "\t"
							  << std::endl;
					proceed = false;
				}
			}
		}
		if(proceed) { if(forward) { ++windNum; } else { --windNum; } }

		if(windNum == dataCutLocal.cols() / smartFindWindLen - 1
		   || windNum == 1
		   )
		{
			std::cout << "smartFindFind: end of file" << std::endl;
			return;
		}
	}
	if(!proceed) std::cout << std::endl;

	resetLimits();
	ui->paintStartDoubleSpinBox->setValue(
				windNum * smartFindWindLen / edfFil.getFreq() - timeLeftGap);
	ui->leftLimitSpinBox->setValue(windNum * smartFindWindLen);
	paint();
}


void Cut::smartFindPrevSlot()
{
	this->smartFindFind(false);
}

void Cut::smartFindNextSlot()
{

	this->smartFindFind(true); return;



	if( !fileOpened ) { return; }

	if(smartFindLearnData.empty() || smartFindThresholds.empty() || smartFindWindParams.empty())
	{
		std::cout << "smartFindNextSlot: data not ready" << std::endl;
		return;
	}
	double timeLeftGap = 0.5;
	int windNum = leftDrawLimit / smartFindWindLen + 2;
	bool proceed = true;
	while(proceed)
	{
		for(int ch = 0; ch < smartFindNumCh; ++ch)
		{
			for(int numFunc = 0; numFunc < smartFindFuncs.size(); ++numFunc)
			{
//				std::cout << "numFunc = " << numFunc << std::endl;
//				std::cout << "mean = " << thrParams[ch][numFunc].mean << std::endl;
//				std::cout << "sigma = " << thrParams[ch][numFunc].sigma << std::endl;
//				std::cout << "val = " << windParams[windNum][ch][numFunc] << std::endl;
				double numSigmas = std::abs(smartFindWindParams[windNum][ch][numFunc]
											- smartFindThresholds[ch][numFunc].mean)
								   /  smartFindThresholds[ch][numFunc].sigma;

				if(numSigmas > paramSigmaThreshold[numFunc]) /// add spin box
				{
					std::cout << "nextBad: param = " << paramNames[numFunc] << " "
							  << "chan = " << edfFil.getLabels(ch) << " "
//							  << "windNum = " << windNum << " "
							  << "numSigmas = " << numSigmas << " "
							  << std::endl;
					proceed = false;
					break; /// from while(proceed)
				}
				else if(smartFindWindParams[windNum][ch][numFunc] > paramAbsThreshold[numFunc])
				{
					std::cout << "nextBad: param = " << paramNames[numFunc] << " "
							  << "chan = " << edfFil.getLabels(ch) << " "
//							  << "windNum = " << windNum << " "
							  << "value = " << smartFindWindParams[windNum][ch][numFunc] << " "
							  << std::endl;
					proceed = false;
					break; /// from while(proceed)
				}
			}
		}
		if(proceed) ++windNum;

		if(windNum == dataCutLocal.cols() / smartFindWindLen - 1)
		{
			std::cout << "smartFindNextSlot: end of file" << std::endl;
			return;
		}
	}

	if(!proceed) std::cout << std::endl;

	resetLimits();
	ui->paintStartDoubleSpinBox->setValue(
				windNum * smartFindWindLen / double(edfFil.getFreq()) - timeLeftGap);
	ui->leftLimitSpinBox->setValue(windNum * smartFindWindLen);
	paint();
}
