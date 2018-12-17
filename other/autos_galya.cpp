#include <other/autos.h>

#include <thread>
#include <chrono>
#include <ios>
#include <iostream>
#include <bits/ios_base.h>
#include <functional>
#include <unordered_map>

#include <other/coords.h>
#include <myLib/clustering.h>
#include <myLib/iitp.h>
#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/valar.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>
#include <myLib/output.h>
#include <myLib/statistics.h>

#include <QTextStream>

using namespace myOut;




namespace autos
{

void calculateFeatures(const QString & pathWithEdfs,
					   const std::vector<QString> & channs,
					   const QString & outPath)
{

	if(!QDir(outPath).exists())
	{
		QDir().mkpath(outPath);
	}

	const auto edfs = QDir(pathWithEdfs).entryList(def::edfFilters,
												   QDir::NoFilter,
												   QDir::Size | QDir::Reversed
												   );
	const auto filesVec = edfs.toVector();
	const int Mask = AUT_SETS.getAutosMask();

	if(Mask & feature::wavelet)
	{
#if WAVELET_MATLAB
		if(!wvlt::isInit)
		{
			wvlt::initMtlb();
		}
#endif
	}


#if 0
	/// parallel calculations - not really needed
#ifdef _OPENMP
	omp_set_dynamic(0);
	omp_set_num_threads(3);
#pragma omp parallel
#pragma omp for nowait
#endif

#endif

	for(int i = 0; i < filesVec.size(); ++i)
	{
		QString filePath = pathWithEdfs + "/" + filesVec[i];

		edfFile initEdf(filePath);

		/// different checks crutches
		if(initEdf.getNdr() == 0)
		{
			std::cout << "ndr = 0\t" << filesVec[i] << std::endl;
			continue;
		}

		if(initEdf.getNs() < channs.size())
		{
			std::cout << "calculateFeatures: too few channels - " << filePath << std::endl;
			continue;
		}

		/// cout fileSize
		std::cout
				<< edfs[i] << '\t'
				<< smLib::doubleRound(QFile(filePath).size() / std::pow(2, 10), 1) << " kB" << "\t"
				<< smLib::doubleRound(initEdf.getDataLen() / initEdf.getFreq(), 1) << " sec"
				<< std::endl;


		const QString preOutPath = outPath + "/" + initEdf.getExpName();

		matrix tmpData = initEdf.getData().subRows(initEdf.findChannels(channs));

		switch(AUT_SETS.getInitialCut())
		{
		case initialCut::first30:
		{
			tmpData = tmpData.subCols(0, 30 * initEdf.getFreq());	/// not resizeCols
			break;
		}
		case initialCut::first60:
		{
			tmpData = tmpData.subCols(0, 60 * initEdf.getFreq());	/// not resizeCols
			break;
		}
		case initialCut::second30:
		{
			if(tmpData.cols() >= 60 * initEdf.getFreq())
			{
				tmpData = tmpData.subCols(30 * initEdf.getFreq(), 60 * initEdf.getFreq());
			}
			else if (tmpData.cols() >= 30 * initEdf.getFreq())
			{
				tmpData = tmpData.subCols(tmpData.cols() - 30 * initEdf.getFreq(), tmpData.cols());
			}
			else
			{
				/// as is
			}
			break;
		}
		case initialCut::none: { break; /* do nothing */ }
		default: { /* do nothing */ } /// never get here
		}
		countAllFeatures(tmpData, initEdf.getFreq(), Mask, preOutPath);
	}
}

void countAllFeatures(const matrix & inData,
					  const double srate,
					  const int Mask,
					  const QString & preOutPath)
{
	/// spectre will be calculated thrice for alpha, FFT, logFFT but I dont care
	/// envelope will be calculated twice in countHilbert
	for(const auto & feat : FEATURES)
	{
		if(Mask & std::get<0>(feat))
		{
			const QString outPath = preOutPath + "_" + std::get<1>(feat) + ".txt";
			QFile::remove(outPath);

			std::ofstream outStr;
			outStr.open(outPath.toStdString());

			outStr << std::fixed;
			outStr.precision(4);

			const auto & f = std::get<2>(feat);
			f(inData,
			  srate,
			  outStr);

			outStr.close();
		}
	}
}


QString featToStr(feature in)
{
	return std::get<1>(
				*std::find_if(std::begin(autos::FEATURES),
							  std::end(autos::FEATURES),
							  [in](const auto & i) { return std::get<0>(i) == in; }));
}
int getFileLength(int in)
{
	return std::get<3>(
				*std::find_if(std::begin(autos::FEATURES),
							  std::end(autos::FEATURES),
							  [in](const auto & i) { return std::get<0>(i) == in; }));
}


std::vector<double> XeniaAverage(const std::vector<double> & res) /// alpha peak
{
#if XENIA_AVERAGE
	std::vector<double> resNew(AUT_SETS.getChansGroups().size());
	for(int i = 0; i < AUT_SETS.getChansGroups().size(); ++i)
	{
		auto group = AUT_SETS.getChansGroups()[i];
		for(auto ch : group.first)
		{
			resNew[i] += res[ch];
		}
		resNew[i] /= group.first.size();
	}
	return resNew;
#else
	return res;
#endif
}

matrix XeniaAverage(const matrix & res)
{
#if XENIA_AVERAGE
	matrix resNew(AUT_SETS.getChansGroups().size(), res.cols(), 0.);
	for(int i = 0; i < AUT_SETS.getChansGroups().size(); ++i)
	{
		auto group = AUT_SETS.getChansGroups()[i];
		for(auto ch : group.first)
		{
			resNew[i] += res[ch];
		}
		resNew[i] /= group.first.size();
	}
	return resNew;
#else
	return res;
#endif
}

void countFFT(const matrix & inData,
			  double srate,
			  std::ostream & outStr)
{

	matrix res(inData.rows(), 1); /// [chan][freq]
	for(int i = 0; i < inData.rows(); ++i)
	{
		/// norming is necessary
		auto helpSpectre = myLib::smoothSpectre(
							   myLib::spectreRtoR(inData[i]) *
							   myLib::spectreNorm(smLib::fftL(inData.cols()),
												  inData.cols(),
												  srate),
							   -1);

		res[i] = myLib::integrateSpectre(helpSpectre,
										 srate,
										 AUT_SETS.getFilter(feature::fft));

	}
	res = XeniaAverage(res);

	/// output part
	QString sep{""};
	QString sep1{"\t"};
	if(AUT_SETS.getOutputStyle() == outputStyle::Column) { sep1 = "\r\n"; }
	if(AUT_SETS.getOutputStyle() == outputStyle::Table) { sep = "\r\n"; }
	switch(AUT_SETS.getOutputSequence())
	{
	case outputSeq::ByChans:
	{
		for(int j = 0; j < res.cols(); ++j) /// freqs
		{
			for(int i = 0; i < res.rows(); ++i) /// chans
			{
				outStr << res[i][j] << sep1;
			}
			outStr << sep;
		}
		break;
	}
	case outputSeq::ByFilters:
	{
		for(int i = 0; i < res.rows(); ++i) /// chans
		{
			for(int j = 0; j < res.cols(); ++j) /// freqs
			{
				outStr << res[i][j] << sep1;
			}
			outStr << sep;
		}
		break;
	}
//	default: { /* can't get here */ }
	}
}

void countLogFFT(const matrix & inData,
				 double srate,
				 std::ostream & outStr)
{
	matrix res(inData.rows(), 1); /// [chan][freq]
	for(int i = 0; i < inData.rows(); ++i)
	{
		/// norming is necessary
		auto helpSpectre = myLib::smoothSpectre(
							   myLib::spectreRtoR(inData[i]) *
							   myLib::spectreNorm(smLib::fftL(inData.cols()),
												  inData.cols(),
												  srate),
							   -1);

		res[i] = myLib::integrateSpectre(helpSpectre,
										 srate,
										 AUT_SETS.getFilter(feature::logFFT));
	}
	res = res.apply(static_cast<double(*)(double)>(std::log10));

	res = XeniaAverage(res);


	/// output part
	QString sep{""};
	QString sep1{"\t"};
	if(AUT_SETS.getOutputStyle() == outputStyle::Column) { sep1 = "\r\n"; }
	if(AUT_SETS.getOutputStyle() == outputStyle::Table) { sep = "\r\n"; }
	switch(AUT_SETS.getOutputSequence())
	{
	case outputSeq::ByChans:
	{
		for(int j = 0; j < res.cols(); ++j) /// freqs
		{
			for(int i = 0; i < res.rows(); ++i) /// chans
			{
				outStr << res[i][j] << sep1;
			}
			outStr << sep;
		}
		break;
	}
	case outputSeq::ByFilters:
	{
		for(int i = 0; i < res.rows(); ++i) /// chans
		{
			for(int j = 0; j < res.cols(); ++j) /// freqs
			{
				outStr << res[i][j] << sep1;
			}
			outStr << sep;
		}
		break;
	}
//	default: { /* can't get here */ }
	}
}

void countAlphaPeak(const matrix & inData,
					double srate,
					std::ostream & outStr)
{
	std::vector<double> res{};
	for(int i = 0; i < inData.rows(); ++i)
	{
		/// is norming necessary?
		auto helpSpectre = myLib::smoothSpectre(
							   myLib::spectreRtoR(inData[i]) *
							   myLib::spectreNorm(smLib::fftL(inData.cols()),
												  inData.cols(),
												  srate),
							   -1);
		res.push_back(myLib::alphaPeakFreq(helpSpectre,
										   inData.cols(),
										   srate));
	}

	res = XeniaAverage(res);

	/// output part
	QString sep{""};
	QString sep1{"\t"};
	if(AUT_SETS.getOutputStyle() == outputStyle::Column) { sep1 = "\r\n"; }
	if(AUT_SETS.getOutputStyle() == outputStyle::Table) { sep = "\r\n"; }

	switch(AUT_SETS.getOutputSequence())
	{
	case outputSeq::ByChans:
	{
		for(int i = 0; i < res.size(); ++i) /// chans
		{
			outStr << res[i] << sep1;
		}
		outStr << sep;
		break;
	}
	case outputSeq::ByFilters:
	{
		for(int i = 0; i < res.size(); ++i) /// chans
		{
			outStr << res[i] << sep1;
			outStr << sep;
		}
		break;
	}
//	default: { /* can't get here */ }
	}
}

void countFracDim(const matrix & inData,
				  double srate,
				  std::ostream & outStr)
{
	matrix res(inData.rows(), AUT_SETS.getFilter(feature::fracDim).size());
	for(int i = 0; i < res.rows(); ++i)
	{
		for(int j = 0; j < res.cols(); ++j)
		{
			const auto & filt = AUT_SETS.getFilter(feature::fracDim)[j];
			res[i][j] = myLib::fractalDimension(
							myLib::refilter(inData[i],
											filt.first,
											filt.second,
											false,
											srate));
		}
	}
	res = XeniaAverage(res);


	/// output part
	QString sep{""};
	QString sep1{"\t"};
	if(AUT_SETS.getOutputStyle() == outputStyle::Column) { sep1 = "\r\n"; }
	if(AUT_SETS.getOutputStyle() == outputStyle::Table) { sep = "\r\n"; }
	switch(AUT_SETS.getOutputSequence())
	{
	case outputSeq::ByChans:
	{
		for(int j = 0; j < res.cols(); ++j) /// filters
		{
			for(int i = 0; i < res.rows(); ++i) /// chans
			{
				outStr << res[i][j] << sep1;
			}
			outStr << sep;
		}
		break;
	}
	case outputSeq::ByFilters:
	{
		for(int i = 0; i < res.rows(); ++i) /// chans
		{
			for(int j = 0; j < res.cols(); ++j) /// filters
			{
				outStr << res[i][j] << sep1;
			}
			outStr << sep;
		}
		break;
	}
//	default: { /* can't get here */ }
	}
}

void countHilbert(const matrix & inData,
				  double srate,
				  std::ostream & outStr)
{
	static const std::vector<std::function<double(const std::valarray<double>&, double)>> funcs
	{myLib::hilbertCarr, myLib::hilbertSD};

	const int resCols = AUT_SETS.getFilter(feature::Hilbert).size();
	const int resRows = inData.rows();

	/// [numFunc][chan][filter]
	std::vector<matrix> res(funcs.size());
	for(auto & in : res)
	{
		in.resize(resRows, resCols);
	}

	for(int j = 0; j < resCols; ++j)
	{
		const auto & filterLims = AUT_SETS.getFilter(feature::Hilbert)[j];
		matrix currMat{};
		if(filterLims != std::pair<double,double>(0, 0))
		{
			currMat = myLib::refilterMat(inData,
										 filterLims.first,
										 filterLims.second,
										 false,
										 srate);
		}
		else
		{
			currMat = inData;
		}

		for(int i = 0; i < resRows; ++i)
		{
			for(int funcNum = 0; funcNum < funcs.size(); ++funcNum)
			{
				const auto & f = funcs[funcNum];
				res[funcNum][i][j] = f(currMat[i], srate);
			}
		}
	}
	res[0] = XeniaAverage(res[0]);
	res[1] = XeniaAverage(res[1]);

	/// output part
	QString sep{""};
	QString sep1{"\t"};
	if(AUT_SETS.getOutputStyle() == outputStyle::Column) { sep1 = "\r\n"; }
	if(AUT_SETS.getOutputStyle() == outputStyle::Table) { sep = "\r\n"; }
	switch(AUT_SETS.getOutputSequence())
	{
	case outputSeq::ByChans:
	{
		for(int numF = 0; numF < funcs.size(); ++numF)
		{
			for(int j = 0; j < resCols; ++j) /// filters
			{
				for(int i = 0; i < res[numF].rows(); ++i) /// chans or groups
				{
					outStr << res[numF][i][j] << sep1;
				}
				outStr << sep;
			}
		}
		break;
	}
	case outputSeq::ByFilters:
	{
		for(int numF = 0; numF < funcs.size(); ++numF)
		{
			for(int i = 0; i < res[numF].rows(); ++i) /// chans or groups
			{
				for(int j = 0; j < resCols; ++j) /// filters
				{
					outStr << res[numF][i][j] << sep1;
				}
				outStr << sep;
			}
		}
		break;
	}
//	default: { /* can't get here */ }
	}
}

void countWavelet(const matrix & inData,
				  double srate,
				  std::ostream & outStr)
{

#if WAVELET_MATLAB
	const int numOfFreqs = wvlt::cwt(inData[0], srate).rows(); /// pewpew

	using funcType = std::function<double(const std::valarray<double> &)>;
	std::vector< funcType > funcs;
//	funcs.push_back(static_cast<funcType>(smLib::mean<double>));
	funcs.push_back(static_cast<funcType>(smLib::sigma<std::valarray<double>>));
//	funcs.push_back(static_cast<funcType>(smLib::median<std::valarray<double>>));

	const int resRows = inData.rows();
	const int resCols = numOfFreqs;

	std::vector<matrix> res(funcs.size());
	for(auto & in : res)
	{
		in.resize(resRows, resCols);
	}

	for(int func = 0; func < funcs.size(); ++func)
	{
		for(int i = 0; i < resRows; ++i) /// chans
		{
			matrix m = wvlt::cwt(inData[i], srate); /// freqs
			for(int j = 0; j < resCols; ++j)
			{
				res[func][i][j] = (funcs[func])(m[i]);
			}
		}
	}
	/// output part
	QString sep{""};
	QString sep1{"\t"};
	if(AUT_SETS.getOutputStyle() == outputStyle::Column) { sep1 = "\r\n"; }
	if(AUT_SETS.getOutputStyle() == outputStyle::Table) { sep = "\r\n"; }
	switch(AUT_SETS.getOutputSequence())
	{
	case outputSeq::ByChans:
	{
		for(int func = 0; func < funcs.size(); ++func)
		{
			for(int j = 0; j < resCols; ++j) /// freqs
			{
				for(int i = 0; i < resRows; ++i) /// chans
				{
					outStr << res[func][i][j] << sep1;
				}
				outStr << sep;
			}
		}
		break;
	}
	case outputSeq::ByFilters:
	{
		for(int func = 0; func < funcs.size(); ++func)
		{
			for(int i = 0; i < resRows; ++i) /// chans
			{
				for(int j = 0; j < resCols; ++j) /// freqs
				{
					outStr << res[func][i][j] << sep1;
				}
				outStr << sep;
			}
		}
		break;
	}
	}
#else
	std::cout << "waveletOneFile doesn't work" << std::endl;
#endif
}

void countHjorth(const matrix & inData,
				 double srate,
				 std::ostream & outStr)
{
	static const std::vector<std::function<double(const std::valarray<double>&)>> funcs
	{myLib::hjorthMobility, myLib::hjorthComplexity};

	const int resCols = AUT_SETS.getFilter(feature::Hjorth).size();
	const int resRows = inData.rows();

	/// [numFunc][chan][filter]
	std::vector<matrix> res(funcs.size());
	for(auto & in : res)
	{
		in.resize(resRows, resCols);
	}

	for(int j = 0; j < resCols; ++j)
	{
		const auto & filterLims = AUT_SETS.getFilter(feature::Hjorth)[j];
		matrix currMat{};
		if(filterLims != std::pair<double,double>(0, 0))
		{
			currMat = myLib::refilterMat(inData,
										 filterLims.first,
										 filterLims.second,
										 false,
										 srate);
		}
		else
		{
			currMat = inData;
		}

		for(int i = 0; i < resRows; ++i)
		{
			for(int funcNum = 0; funcNum < funcs.size(); ++funcNum)
			{
				res[funcNum][i][j] = (funcs[funcNum])(currMat[i]);
			}
		}
	}


	/// output part
	QString sep{""};
	QString sep1{"\t"};
	if(AUT_SETS.getOutputStyle() == outputStyle::Column) { sep1 = "\r\n"; }
	if(AUT_SETS.getOutputStyle() == outputStyle::Table) { sep = "\r\n"; }
	switch(AUT_SETS.getOutputSequence())
	{
	case outputSeq::ByChans:
	{
		for(int numF = 0; numF < funcs.size(); ++numF)
		{
			for(int j = 0; j < resCols; ++j) /// filters
			{
				for(int i = 0; i < resRows; ++i) /// chans
				{
					outStr << res[numF][i][j] << sep1;
				}
				outStr << sep;
			}
		}
		break;
	}
	case outputSeq::ByFilters:
	{
		for(int numF = 0; numF < funcs.size(); ++numF)
		{
			for(int i = 0; i < resRows; ++i) /// chans
			{
				for(int j = 0; j < resCols; ++j) /// filters
				{
					outStr << res[numF][i][j] << sep1;
				}
				outStr << sep;
			}
		}
		break;
	}
//	default: { /* can't get here */ }
	}
}






double countRhythmAdoption(const std::valarray<double> & sigRest,
						   const std::valarray<double> & sigAdop,
						   double freq)
{
	int fftLen = std::max(smLib::fftL(sigRest.size()),
						  smLib::fftL(sigAdop.size()));
	auto specRest = myLib::spectreRtoR(sigRest, fftLen);
	auto specAdop = myLib::spectreRtoR(sigAdop, fftLen);

	int lowFr = smLib::fftLimit(freq - 0.5, DEFS.getFreq(), fftLen);
	int higFr = smLib::fftLimit(freq + 0.5, DEFS.getFreq(), fftLen) + 1;

	return
			std::accumulate(std::begin(specAdop) + lowFr,
							std::begin(specAdop) + higFr,
							0.)
			/
			std::accumulate(std::begin(specRest) + lowFr,
							std::begin(specRest) + higFr,
							0.)
			;

}

std::valarray<double> countRhythmAdoption(const matrix & specRest,
										  const matrix & specAdop,
										  int fftLen,
										  double freq)
{
	const int lowFr = smLib::fftLimit(freq - 0.5, DEFS.getFreq(), fftLen);
	const int higFr = smLib::fftLimit(freq + 0.5, DEFS.getFreq(), fftLen) + 1;
	std::valarray<double> res(specRest.rows());

	for(int i = 0; i < res.size(); ++i)
	{
		res[i] = std::accumulate(std::begin(specAdop[i]) + lowFr,
								 std::begin(specAdop[i]) + higFr,
								 0.);
		auto a = std::accumulate(std::begin(specRest[i]) + lowFr,
								 std::begin(specRest[i]) + higFr,
								 0.);
		if(a == 0.)	{ res[i] = -1; }
		else		{ res[i] /= a; }
	}
	return res;
}




void rhythmAdoption(const QString & guyPath,
					const QString & restMark,
					const QString & stimType,
					const std::vector<int> & freqs)
{
	const int numChans = 19;


	const QString outDir = guyPath.left(guyPath.lastIndexOf('/'));
	const QString restFileName = QDir(guyPath).entryList({"*" + restMark + "*"})[0];
	const QString ExpName = restFileName.left(restFileName.indexOf(restMark));

	matrix res(freqs.size(), numChans, -1);
	edfFile restEdf(guyPath + "/" + restFileName);
	const matrix restData = restEdf.getData().subRows(19);
	const int fftLen = smLib::fftL(restData.cols());

	const std::unordered_map<int, std::pair<double, double>> mp
	{
		{2,		{1., 13.}},
		{4,		{1., 13.}},
		{8,		{1., 13.}},
		{16,	{6., 20.}},
		{32,	{28., 36.}},
	};
	const matrix specRest = myLib::countSpectre(restData, fftLen, 0); // spectre 0-35 Hz

	for(int j = 0; j < freqs.size(); ++j)
	{
		const QString currPath = guyPath + "/"
								 + ExpName + "_" + nm(freqs[j])
								 + stimType + ".edf";
		if(!QFile::exists(currPath))
		{
			continue;
		}

		const QString a = ExpName + "_" + nm(freqs[j]) + stimType + ".edf";
		std::cout << a << std::endl;

		edfFile currFile(currPath);
		const matrix specAdop
				= myLib::countSpectre(currFile.getData().subRows(19), fftLen, 0);

		res[j] = countRhythmAdoption(specRest,
									 specAdop,
									 fftLen,
									 freqs[j]);

		/// draw part
		{
			const QString picPath = outDir + "/out/"
									+ ExpName + "_" + nm(freqs[j])
									+ stimType + ".jpg";
			const auto specDiff = specAdop - specRest;

			const int lefLim = smLib::fftLimit(mp.at(freqs[j]).first, DEFS.getFreq(), fftLen);
			const int rigLim = smLib::fftLimit(mp.at(freqs[j]).second, DEFS.getFreq(), fftLen) + 1;

			matrix drawData(3);
			drawData[0] = specAdop.subCols(lefLim, rigLim).toValarByRows();
			drawData[1] = specRest.subCols(lefLim, rigLim).toValarByRows();
			drawData[2] = specDiff.subCols(lefLim, rigLim).toValarByRows();

			myLib::drw::drawArrays(
						myLib::drw::drawTemplate(true,
												 mp.at(freqs[j]).first,
												 mp.at(freqs[j]).second),
								   drawData).save(picPath, nullptr, 100);
		}

		/// draw full
		{
			const QString picPath = outDir + "/out/"
									+ ExpName + "_" + nm(freqs[j])
									+ stimType + "_all.jpg";
			const auto specDiff = specAdop - specRest;

			const int lefLim = smLib::fftLimit(1., DEFS.getFreq(), fftLen);
			const int rigLim = smLib::fftLimit(36., DEFS.getFreq(), fftLen) + 1;

			matrix drawData(3);
			drawData[0] = specAdop.subCols(lefLim, rigLim).toValarByRows();
			drawData[1] = specRest.subCols(lefLim, rigLim).toValarByRows();
			drawData[2] = specDiff.subCols(lefLim, rigLim).toValarByRows();

			myLib::drw::drawArrays(myLib::drw::drawTemplate(true, 1., 36.),
								   drawData).save(picPath, nullptr, 100);
		}

//		for(int i = 0; i < numChans; ++i)
//		{
//			res[j][i] = countRhythmAdoption(restData[i],
//											currFile.getData(i),
//											freqs[j],
//											picPath);
//		}
	}
	std::ofstream outFile((outDir + "/"
						   + ExpName + "_" + stimType + ".txt").toStdString());
	outFile.precision(3);
	for(int i = 0; i < res.rows(); ++i)
	{
		outFile << res[i] << "\r\n"; ///
	}
	outFile.close();
}

void rhythmAdoptionGroup(const QString & groupPath,
						 const QString & restMark,
						 const QString & stimType)
{
	auto guysList = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QDir(groupPath).mkdir("out");
	const std::vector<int> freqs{2, 4, 8, 16, 32};

	for(const QString & guy : guysList)
	{
		if(guy == "out") { continue; }
		rhythmAdoption(groupPath + "/" + guy,
					   restMark,
					   stimType,
					   freqs);
	}

	QString subdir = groupPath.mid(groupPath.lastIndexOf("/") + 1);

	autos::ArrangeFilesToTable(groupPath,
							   groupPath + "/out/all_" + subdir + "_" + stimType + ".txt",
							   true,
							   stimType);
}


/// further generalizations
void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag)
{
	DEFS.setNtFlag(false);

	for(const QString& guy : guyList)
	{
		if(cutOnlyFlag)
		{
			autos::cutFilesInFolder(def::mriFolder + "/" + guy, 2);
			continue;
		}

#if 0
		/////// PEWPEWPWEPWEPPEWWPEWPEEPPWPEPWEPWEPWPEPWEPWPEWPEWPEPWEPWEPWEPWPEWP
		autos::GalyaFull(def::mriFolder +
						 "/" + guy +
						 "/" + guy + "_winds_cleaned");
#endif

		QString outPath = def::mriFolder + "/OUT/" + guy;
		QString dropPath = "/media/Files/Dropbox/DifferentData/EEG-MRI/Results";
		QStringList files = QDir(outPath).entryList({"*.txt"});

		/// make archive
		QString cmd = "cd " + outPath + " && " +
					  "rar a " + guy + ".rar ";
		for(const QString & a : files)
		{
			cmd += a + " ";
		}
		system(cmd.toStdString().c_str());

		/// copy to Dropbox folder
		cmd = "cp " + outPath + "/" + guy + ".rar " +
			  dropPath + "/" + guy + ".rar";
		system(cmd.toStdString().c_str());

		/// copy link
//		std::this_thread::sleep_for(std::chrono::seconds(15)); /// wait for copy to end?
		cmd = "./dropbox.py sharelink " +  dropPath + "/" + guy + ".rar" +
			  " | xclip -selection clipboard";
		system(cmd.toStdString().c_str());
	}
}

/// recount FD
void EEG_MRI_FD()
{
	DEFS.setNtFlag(false);

	const QString workDir = "/media/Files/Data/MRI_winds";
	const QString OUT = "/media/Files/Data/MRI_winds_out";
	if(!QDir(OUT).exists())
	{
		QDir().mkpath(OUT);
	}

	QStringList guyList = QDir(workDir).entryList(QDir::Dirs|QDir::NoDotAndDotDot);

	for(const QString & guy : guyList)
	{
		break;
		const QString outPath = workDir + "/" + guy + "/out";

		myLib::cleanDir(outPath);
		autos::calculateFeatures(workDir + "/" + guy,
							   coords::lbl32,
							   outPath);
		/// list in order
		auto lst = QDir(outPath).entryList(QDir::Files, QDir::Name);
		/// arrange to tables
		autos::ArrangeFilesToTable(outPath,
								 OUT + "/" + guy + ".txt");
	}
	autos::ArrangeFilesToTable(OUT,
							 OUT + "/all.txt",
							 true);
}


void ProcessByGroups(const QString & inPath,
					 const QString & outPath,
					 const std::vector<QString> & channs)
{
	if(!outPath.isEmpty() && !QDir(outPath).exists()) { QDir().mkdir(outPath); }

	for(const QString & group : QDir(inPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		const QString groupPath = inPath + "/" + group;

		/// list of guys
		const QStringList guys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(const QString & guy : guys)
		{
			const QString guyPath = groupPath + "/" + guy;
			const QStringList edfs = QDir(guyPath).entryList(def::edfFilters);
			if(edfs.isEmpty())
			{
				std::cout << "Xenia_TBI_finalest: guyPath is empty " << guyPath << std::endl;
				continue;
			}

			QString guyOutPath{};
			if(outPath.isEmpty())
			{
				guyOutPath = guyPath + "/out";
				if(!QDir(guyOutPath).exists()) { QDir().mkpath(guyOutPath); }
				myLib::cleanDir(guyOutPath);
			}
			else
			{
				guyOutPath = outPath;
			}

			autos::calculateFeatures(guyPath, channs, guyOutPath);
		}
	}
}

/// each file is one-line
void ArrangeFilesToTable(const QString & inPath,
						 QString outTablePath,
						 bool writePeople,
						 const QString & auxFilter)
{
	QDir deer(inPath);

	if(outTablePath.isEmpty())
	{
		deer.cdUp();
		outTablePath = deer.absolutePath() + "/table.txt";
		deer.cd(inPath);
	}
	const QString tableName = myLib::getFileName(outTablePath);


	QFile outStr(outTablePath);
	outStr.open(QIODevice::WriteOnly);

	std::ofstream fileNames;
	if(writePeople)
	{
		QString peoplePath = outTablePath;
		peoplePath.replace(".txt", "_people.txt");
		fileNames.open(peoplePath.toStdString());
	}

	for(const QString & fileName : deer.entryList({"*" + auxFilter + ".txt"}, //// pewpepwpewpwe
												  QDir::Files,
												  QDir::Time
												  | QDir::Reversed
												  ))
	{
		if((fileName == tableName)
		   || fileName.contains("_people")
		   || fileName.contains("labels.txt")
		   ) { continue; }

		if(writePeople)
		{
			fileNames << fileName.left(fileName.lastIndexOf(".")) << "\n";
		}

		QFile fil(deer.absolutePath() + "/" + fileName);
		fil.open(QIODevice::ReadOnly);
		auto contents = fil.readAll();
		fil.close();
		outStr.write(contents);
		outStr.write("\r\n");
	}
	if(writePeople)
	{
		fileNames.close();
	}
	outStr.close();
}




void cutOneFile(const QString & filePath,
				const int wndLen,
				const QString & outPath)
{
	edfFile initEdf(filePath);

#define ADD_DIR 0
#if ADD_DIR
	QString addDir = initEdf.getExpName();
	addDir.resize(addDir.indexOf("_"));
	QDir(outPath).mkdir(addDir);
#endif

	/// generality experimental
	if(initEdf.getEdfPlusFlag()) /// if annotations
	{
		initEdf.removeChannels(std::vector<int>{initEdf.getMarkChan()}); /// remove Annotations
	}

	int fr = initEdf.getFreq();
	const int numOfWinds = std::ceil(initEdf.getDataLen() / fr / wndLen);

	for(int i = 0; i < numOfWinds; ++i)
	{
		initEdf.saveSubsection(
					i * fr * wndLen,
					std::min((i + 1) * fr * wndLen, initEdf.getDataLen()),
					outPath
			#if ADD_DIR
					+ "/" + addDir
			#endif
					+ "/" + initEdf.getExpName()
					+ "_wnd_" + rn(i + 1, floor(std::log10(numOfWinds)) + 1)
					+ ".edf");
	}
}

void cutFilesInFolder(const QString & path,
					  const int wndLen,
					  QString outPath)
{

	const QString outDir = myLib::getFileName(path) + "_winds";
	const QString smallsDir = myLib::getFileName(path) + "_smalls";
	const QString smallsPath = path + "/" + smallsDir;

	if(outPath.isEmpty()) { outPath = path + "/" + outDir; }
	QDir().mkpath(outPath);
	QDir().mkpath(smallsPath);


	QDir tmpDir(path);
	const QStringList leest1 = tmpDir.entryList(def::edfFilters);
	const auto filesVec = leest1.toVector();

	const QString logPath = def::GalyaFolder + "/log.txt";
	std::ofstream logStream(logPath.toStdString(), std::ios_base::app);

#if 0
#ifdef _OPENMP
	omp_set_dynamic(0);
	omp_set_num_threads(3);
#pragma omp parallel
#pragma omp for nowait
#endif
#endif
	for(const auto & fileName : filesVec)
	{
		std::cout << fileName << "\t"; /// may be followed by "smallFile"
		QString helpString = path + "/" + fileName;
		edfFile initEdf(helpString, true);

		/// some check for small
		if(initEdf.getNdr() * initEdf.getDdr() <= wndLen )
		{
			QFile::remove(smallsPath + "/" + initEdf.getFileName(false));
			QFile::copy(initEdf.getFilePath(),
						smallsPath + "/" + initEdf.getFileName(false));

			std::cout << "smallFile !!!";
			logStream << initEdf.getFilePath() << "\t" << "too small" << "\n";
			continue;
		}
		std::cout << std::endl;

		cutOneFile(helpString,
				   wndLen,
				   outPath);
	}
	logStream.close();

}



void rereferenceFolder(const QString & procDirPath,
					   reference newRef)
{
	const QStringList filesList = QDir(procDirPath).entryList(def::edfFilters,
															  QDir::NoFilter,
															  QDir::Size | QDir::Reversed);

	for(const QString & fileName : filesList)
	{
		QString helpString = procDirPath + "/" + fileName;
		edfFile fil(helpString);
		fil.rereferenceData(newRef, false, false).writeEdfFile(helpString);
	}
}

void refilterFolder(const QString & procDirPath,
					  double lowFreq,
					  double highFreq,
					  bool isNotch)
{
	for(const QString & fileName : QDir(procDirPath).entryList(def::edfFilters))
	{
		QString helpString = procDirPath + "/" + fileName;
		edfFile fil(helpString);
		fil.refilter(lowFreq, highFreq, isNotch).writeEdfFile(helpString);
	}
}

void EdfsToFolders(const QString & inPath)
{
	auto lst = QDir(inPath).entryList(def::edfFilters);
	for(const QString & in : lst)
	{
		QString ExpName = in.left(in.lastIndexOf("_"));
		if(!QDir(inPath + "/" + ExpName).exists())
		{
			QDir().mkpath(inPath + "/" + ExpName);
		}
		/// copy if not delete initial
		QFile::rename(inPath + "/" + in,
					  inPath + "/" + ExpName + "/" + in);
	}
}

void ProcessAllInOneFolder(const QString & inPath,
						   const QString & outPath,
						   const std::vector<QString> & channs)
{
	if(!QDir(outPath).exists()) { QDir().mkpath(outPath); }
	myLib::cleanDir(outPath, "txt", true);

	QStringList edfs = QDir(inPath).entryList(def::edfFilters);
	if(edfs.isEmpty())
	{
		std::cout << "ProcessAllInOneFolder: inPath is empty " << inPath << std::endl;
	}

	const QString guysOutPath = inPath + "/out";

	/// process?
	if(01)
	{
		/// clear outFolder
		myLib::cleanDir(guysOutPath, "txt", true);
		autos::calculateFeatures(inPath, channs, guysOutPath);
	}

	/// make one line file for each file
	if(1)
	{
		for(QString ExpName : edfs)
		{
			ExpName = ExpName.left(ExpName.indexOf('.'));

			std::vector<QString> fileNamesToArrange{};
			for(feature feat : AUT_SETS.getAutosMaskArray())
			{
				fileNamesToArrange.push_back(ExpName + "_" + autos::featToStr(feat) + ".txt");
			}
			myLib::concatFilesHorz(guysOutPath,
								   fileNamesToArrange,
								   guysOutPath + "/" + ExpName + ".txt");

			/// copy files into outPath
			if(1)
			{
				QFile::remove(outPath + "/" + ExpName + ".txt");
				QFile::copy(guysOutPath + "/" + ExpName + ".txt",
							outPath + "/" + ExpName + ".txt");
			}
		}
	}
	/// make tables whole and people list
	autos::ArrangeFilesToTable(outPath,
							   outPath + "/all.txt",
							   true);

}

void ProcessByFolders(const QString & inPath,
					  const QString & outPath,
					  const std::vector<QString> & channs,
					  const std::vector<QString> & markers)
{
	if(!QDir(outPath).exists()) { QDir().mkpath(outPath); }
	myLib::cleanDir(outPath, "txt", true);

	auto guyList = QDir(inPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(const QString & guy : guyList)
	{
		const QString guyPath = inPath + "/" + guy;
		QStringList edfs = QDir(guyPath).entryList(def::edfFilters);
		if(edfs.isEmpty())
		{
			std::cout << "ProcessByFolders: guyPath is empty " << guy << std::endl;
			continue;
		}

		QString ExpName = edfs[0];
		ExpName = ExpName.left(ExpName.lastIndexOf('_'));

		const QString guyOutPath = guyPath + "/out";
		if(!QDir(guyOutPath).exists()) { QDir().mkpath(guyOutPath); }

		/// process?
		if(01)
		{
			/// clear guyOutPath
			myLib::cleanDir(guyOutPath, "txt", true);
			autos::calculateFeatures(guyPath, channs, guyOutPath);
		}

		/// make one line file for each stimulus
		if(01)
		{
			for(const QString & mark : markers)
			{
				std::vector<QString> fileNamesToArrange{};
				for(feature feat : AUT_SETS.getAutosMaskArray())
				{
					const QString fileName = ExpName + mark
											 + "_" + autos::featToStr(feat) + ".txt";
					fileNamesToArrange.push_back(fileName);

					/// create files if they are absent
					/// this should not usually happen
					if(01)
					{
						if(!QFile::exists(guyOutPath + "/" + fileName))
						{
							std::ofstream outStr((guyOutPath + "/" + fileName).toStdString());
							for(int i = 0; i < autos::getFileLength(feat); ++i)
							{
								outStr << " " << '\t';
//								outStr << 0 << '\t';
							}
							outStr.close();
						}
					}
				}
				myLib::concatFilesHorz(guyOutPath,
									   fileNamesToArrange,
									   guyOutPath + "/" + ExpName + mark + ".txt");
			}
		}

		/// make whole line from all stimuli for current guy
		if(1)
		{
			std::vector<QString> fileNamesToArrange{};
			for(const QString & mark : markers)
			{
				fileNamesToArrange.push_back(ExpName + mark + ".txt");
			}
			myLib::concatFilesHorz(guyOutPath,
									  fileNamesToArrange,
									  guyOutPath + "/" + ExpName + ".txt");
		}

		/// copy files into outPath
		if(1)
		{
			QFile::remove(outPath + "/" + ExpName + ".txt");
			QFile::copy(guyOutPath + "/" + ExpName + ".txt",
						outPath + "/" + ExpName + ".txt");
		}
	}

	/// make whole table and people list
	autos::ArrangeFilesToTable(outPath,
							   outPath + "/all.txt",
							   true);
}

void makeLabelsFile(const std::vector<QString> & chans,
					const QString & outFilePath,
					const std::vector<QString> & usedMarkers,
					int Mask,
					const QString & sep)
{
	std::ofstream lab(outFilePath.toStdString());

	/// make lambda
	auto out = [&](QString mrk, feature feat, const std::vector<QString> & funcs) -> void
	{
		for(const auto & f : funcs)
		{
			switch(AUT_SETS.getOutputSequence())
			{
			case outputSeq::ByChans:
			{
				for(const auto & filt : AUT_SETS.getFilter(feat))
				{
					for(const QString & lbl : chans)
					{
						lab << mrk
							<< "_" << f
							<< "_" << nm(filt.first)
							<< "_" << nm(filt.second)
							<< "_"  << lbl << sep;
					}
				}
				break;
			}
			case outputSeq::ByFilters:
			{
				for(const QString & lbl : chans)
				{
					for(const auto & filt : AUT_SETS.getFilter(feat))
					{
						lab << mrk
							<< "_" << f
							<< "_" << nm(filt.first)
							<< "_" << nm(filt.second)
							<< "_"  << lbl << sep;
					}
				}
				break;
			}
//			default: { /* can't get here */ }
			}
		}
	};

	for(QString mark : usedMarkers)
	{
		mark.remove('_');

		if(Mask & feature::fft)
		{
			out(mark, feature::fft, {"fft"});
		}

		if(Mask & feature::alphaPeak)
		{
			for(const QString & lbl : chans)
			{
				lab << mark << "_" << "alpha" << "_"  << lbl << sep;
			}
		}

		if(Mask & feature::fracDim)
		{
			out(mark, feature::fracDim, {"fd"});
		}

		if(Mask & feature::Hilbert)
		{
			out(mark, feature::Hilbert, {"hilbcarr", "hilbsd"});
		}

		if(Mask & feature::Hjorth)
		{
			out(mark, feature::Hjorth, {"hjorthmob", "hjorthcom"});
		}

#if WAVELET_MATLAB
		if(Mask & feature::wavelet)
		{
			for(int i = 0; i < 19; ++i) /// wavelet freqs magic const
			{
				for(const QString & lbl : chans)
				{
					lab << mark
						<< "_" << "wavSD"
						<< "_" << nm(i + 2)
						<< "_" << lbl << sep;
				}
			}
		}
#endif

		if(Mask & feature::logFFT)
		{
			out(mark, feature::logFFT, {"logfft"});
		}
	}
	lab.close();
}


void makeLabelsXeniaWithAverage(const std::vector<QString> & chans,
								int guyNum,
								const QString & outFilePath,
								const std::vector<QString> & usedMarkers,
								feature feat,
								const QString & sep)
{
	std::ofstream lab(outFilePath.toStdString());

	/// make lambda
	auto out = [&](QString mrk, int num, feature feat, const std::vector<QString> & funcs) -> void
	{
		for(const auto & f : funcs)
		{
			switch(AUT_SETS.getOutputSequence())
			{
			case outputSeq::ByChans:
			{
				for(const auto & filt : AUT_SETS.getFilter(feat))
				{
					for(const QString & lbl : chans)
					{
						lab
								<< mrk
								<< "_" << nm(num + 1)
								<< "_" << f;
						if(filt != autos::filtFreqs.at(autos::filter::none))
						{
							lab
									<< "_" << nm(filt.first)
									<< "_" << nm(filt.second);
						}
						lab << "_"  << lbl << sep;
					}
				}
				break;
			}
			case outputSeq::ByFilters:
			{
				for(const QString & lbl : chans)
				{
					for(const auto & filt : AUT_SETS.getFilter(feat))
					{
						lab
								<< mrk
								<< "_" << nm(num + 1)
								<< "_" << f;
						if(filt != autos::filtFreqs.at(autos::filter::none))
						{
							lab
									<< "_" << nm(filt.first)
									<< "_" << nm(filt.second);
						}
						lab << "_"  << lbl << sep;
					}
				}
				break;
			}
//			default: { /* can't get here */ }
			}
		}
	};

	const std::vector<QString> funcNames = ([feat]() -> std::vector<QString>
	{
		switch(feat)
		{
		case feature::fft:			{ return {"fft"}; }
		case feature::fracDim:		{ return {"fd"}; }
		case feature::Hilbert:		{ return {"hilbcarr", "hilbsd"}; }
		case feature::alphaPeak:	{ return {"alpha"}; }
		case feature::wavelet:		{ return {"wavsd"}; }
		case feature::Hjorth:		{ return {"hjorthmob", "hjorthcom"}; }
		case feature::logFFT:		{ return {"logfft"}; }
		}
		return {};
	})();

	for(QString mark : usedMarkers)
	{
		mark.remove('_');
		for(int num = 0; num < guyNum; ++num)
		{
			out(mark, num, feat, funcNames);
		}
	}
	lab.close();
}


} /// end of namespace autos
