#include <other/autos.h>

const std::vector<std::pair<double, double>> integrLimits
{
	/// 4-Hz wide
	{4, 8},
	{8, 12},
	{12, 16},
	{16, 20},
	{20, 24},

	/// 2-Hz wide
	{4, 6}, {6, 8},
	{8, 10}, {10, 12},

	/// 1-Hz wide
	{4, 5}, {5, 6}, {6, 7}, {7, 8},
	{8, 9}, {9, 10}, {10, 11}, {11, 12},
	{12, 13}, {13, 14}, {14, 15}, {15, 16},
	{16, 17}, {17, 18}, {18, 19}, {19, 20},
	{20, 21}, {21, 22}, {22, 23}, {23, 24},
};

const int numOfTasks = 180;
const int numSmooth = 15;
const double hilbertFreqLimit = 40.;

const std::vector<std::pair<double, double>> hilbFilters
{
	{0.,	0.},		/// [0] no filter
	{4.,	7.},		/// [1] theta
	{8.,	13.},		/// [2] alpha
	{8.,	10.},		/// [3] low_alpha
	{10.,	13.},		/// [4] high_alpha
	{2.,	20.},		/// [5] band of interest
	{2.,	6.},		/// [6] delta
};



/// size = integrLimits.size()
std::valarray<double> elenaFft(const std::valarray<double> & inSignal,
							   double srate)
{
	auto spec = myLib::smoothSpectre(
					myLib::spectreRtoR(inSignal) *
					myLib::spectreNorm(smLib::fftL(inSignal.size()),
									   inSignal.size(),
									   srate),
					numSmooth);
	return myLib::integrateSpectre(spec, srate, integrLimits);
}

/// size = 1
std::valarray<double> elenaAlphaPeak(const std::valarray<double> & inSignal,
									 double srate)
{
	auto spec = myLib::smoothSpectre(
					myLib::spectreRtoR(inSignal) *
					myLib::spectreNorm(smLib::fftL(inSignal.size()),
									   inSignal.size(),
									   srate),
					numSmooth);
	return {myLib::alphaPeakFreq(spec, inSignal.size(), srate)};
}

/// size = 1
std::valarray<double> elenaFracDim(const std::valarray<double> & inSignal,
								   double srate)
{
	return {myLib::fractalDimension(inSignal)};
}


/// size = hilbFilters.size() * 2
std::valarray<double> elenaHilbert(const std::valarray<double> & inSignal,
								   double srate)
{

	/// [filter1_carr, filter1_SD, filter2_carr, ..]
	std::vector<double> hilb{};
	hilb.reserve(hilbFilters.size() * 2);

	for(const std::pair<double,double> & filterLims : hilbFilters)
	{
		std::valarray<double> currSignal{};
		if(filterLims != std::pair<double,double>{0., 0.})
		{
			currSignal = myLib::refilter(inSignal,
										 filterLims.first,
										 filterLims.second,
										 false,
										 srate);
		}
		else
		{
			currSignal = inSignal;
		}

		/// write envelope median spectre
		const std::valarray<double> env = myLib::hilbertPieces(currSignal);
		std::valarray<double> envSpec = myLib::spectreRtoR(env);
		envSpec[0] = 0.;

		double helpDouble = 0.;
		double sumSpec = 0.;
		for(int j = 0;
			j < fftLimit(hilbertFreqLimit,
						 srate,
						 smLib::fftL( inSignal.size() ));
			++j)
		{
			helpDouble += envSpec[j] * j;
			sumSpec += envSpec[j];
		}
		helpDouble /= sumSpec;
		helpDouble /= fftLimit(1.,
							   srate,
							   smLib::fftL( inSignal.size() )); /// convert to Hz

		hilb.push_back(helpDouble);
		hilb.push_back(smLib::sigma(env) / smLib::mean(env));

	}
	return smLib::vecToValar(hilb);
}

/// size = 2
std::valarray<double> elenaHjorth(const std::valarray<double> & inSignal,
								  double srate)
{
	return {myLib::hjorthMobility(inSignal), myLib::hjorthComplexity(inSignal)};
}

using elenaFuncType = std::function<std::valarray<double>(const std::valarray<double> &, double)>;
const std::vector<std::pair<const elenaFuncType &, int>> funcsWithSizes
{
	{elenaFft, integrLimits.size()},
	{elenaAlphaPeak, 1},
	{elenaFracDim, 1},
	{elenaHilbert, hilbFilters.size() * 2},
	{elenaHjorth, 2},
};
const int sumSize{std::accumulate(std::begin(funcsWithSizes),
								  std::end(funcsWithSizes),
								  0,
								  [](int in1, const auto & in2)
	{ return in1 + in2.second; })
				 };


const QString RDstring{"RD"};
const QString PPGstring{"FPG"};
const QString EDAstring{"KGR"};


std::vector<QString> makeTableCols(const std::vector<QString> & labels)
{
	std::vector<QString> tableCols{};

	/// fft
	for(const auto & cluster : labels)
	{
		for(const auto & lim : integrLimits)
		{
			tableCols.push_back("fft_" + cluster + "_" + nm(lim.first) + "-" + nm(lim.second));
		}
	}

	/// alpha peak
	for(const auto & cluster : labels)
	{
		tableCols.push_back("alpha_" + cluster);
	}

	/// fracDim
	for(const auto & cluster : labels)
	{
		tableCols.push_back("fracDim_" + cluster);
	}

	/// Hilbert
	for(const auto & cluster : labels)
	{
		for(const auto & fil : hilbFilters)
		{
			tableCols.push_back("hilbCarr_"	+ cluster + "_" + nm(fil.first) + "-" + nm(fil.second));
			tableCols.push_back("hilbSD_"	+ cluster + "_" + nm(fil.first) + "-" + nm(fil.second));
		}
	}

	/// Hjorth
	for(const auto & cluster : labels)
	{
		tableCols.push_back("mobility_" + cluster);
		tableCols.push_back("complexity_" + cluster);
	}

	/// veget
	tableCols.push_back("SGRval");
	tableCols.push_back("SGRlat");
	tableCols.push_back("RDfreq");
	tableCols.push_back("PPGampl");
	tableCols.push_back("PPGfreq");
	tableCols.push_back("reacTime");

	/// auxiliary
	tableCols.push_back("taskNumber");
	tableCols.push_back("taskMark");
	tableCols.push_back("operMark");
	return tableCols;
}


void elenaCalculation(const QString & realsPath,
					  const QString & outTableDir)
{
	///////////////
	const std::vector<QString> markers
	{
		"_m_241",
		"_m_243",
		"_m_244",
//		"_t_314", /// closed eyes (whole)
//		"_t_315", /// open eyes (whole)
		"_t_214", /// closed eyes (winds)
		"_t_215", /// open eyes (winds)
	};


	const int fftLen = 4096;
	const int numChansForSpectre = 128; /// -1

	QDir().mkpath(outTableDir);
	const QString tablePath = outTableDir + "/table.txt";

	/// RD - recursia dyhaniya (don't know how it's called in English)
	/// FPG - FotoPletizmoGramma (PPG - PhotoPlethismoGram)
	/// KGR - Kozhno Galvanicheskaya Reakciya (really SGR - Skin Galvanic Reaction
	/// or EDA - ElectroDermal Activity)

	matrix result{};

	const auto forSet = smLib::range<std::vector<int>>(0, numOfTasks);
	std::set<int> allNumbers(std::begin(forSet), std::end(forSet));

	const QStringList reals = QDir(realsPath).entryList(def::edfFilters);
	for(const QString & fileName : reals)
	{
		edfFile fil(realsPath + "/" + fileName);
		const matrix & inData = fil.getData();

		/// read from filename
		std::vector<QString> fromFileName{};
		for(const QString & prefix : {"_eda_", "_n_", "_m_", "_t_"})
		{
			const int from = fileName.indexOf(prefix) + prefix.size();
			const int to = fileName.indexOf("_", from);
			fromFileName.push_back(fileName.mid(from, to - from));
		}

		/// calculate features
		matrix features(128, sumSize);
		for(int chanNum = 0; chanNum < 128; ++chanNum) ////////////////////
		{
			int currIndex{0};
			for(const auto & func : funcsWithSizes)
			{
				features[chanNum][std::slice(currIndex, func.second, 1)]
						= (func.first)(inData[chanNum], fil.getFreq());
				currIndex += func.second;
			}
		}

		/// integrate over clusters
		matrix avFeatures{};
		for(const auto & in : coords::egi::chans128)
		{
			std::valarray<double> av(sumSize);
			for(QString str : in.second)
			{
				str.remove(" ");
				av += features[str.toInt() - 1];
			}
			av /= in.second.size();
			avFeatures.push_back(av);
		}

		/// make a long line
		std::valarray<double> res(sumSize + 9); /////////// magic = vegetative + auxiliary
		res[std::slice(0, sumSize, 1)] = avFeatures.toValarByRows();

		/// calculate vegetative
		int vegetIndex = sumSize;
		auto eda = myLib::EDAmax(fil.getData(EDAstring), fromFileName[0].toDouble());
		res[vegetIndex++] = eda.first;
		res[vegetIndex++] = eda.second / fil.getFreq();
		res[vegetIndex++] = myLib::RDfreq(fil.getData(RDstring), fftLen);
		res[vegetIndex++] = myLib::PPGrange(fil.getData(PPGstring));
		res[vegetIndex++] = myLib::RDfreq(fil.getData(PPGstring), fftLen);
		res[vegetIndex++] = inData.cols() / fil.getFreq();

		/// auxiliary
		res[vegetIndex++] = fromFileName[1].toInt();	/// taskNumber
		res[vegetIndex++] = fromFileName[2].toInt();	/// taskMark
		res[vegetIndex++] = fromFileName[3].toInt();	/// operMark
		allNumbers.erase(fromFileName[1].toInt());
		result.push_back(res);
	}
#if 01
	/// cout unprocessed tasks
	if(!allNumbers.empty())
	{
		std::cout << "elenaCalculation: unprocessed reals:" << std::endl;
		for(auto each : allNumbers)
		{
			std::cout << each << " ";
		}
		std::cout << std::endl;
	}
#endif


#if 01
	/// get averages
	auto getAverage = [&result](int taskMark) -> std::valarray<double>
	{
		std::valarray<double> res(result.cols());
		int num = 0;
		for(const auto & row : result)
		{
			if(row[sumSize + 7] == taskMark)
			{
				res += row;
				++num;
			}
		}
		return res / static_cast<double>(num);
	};
	std::ofstream avStr((outTableDir + "/averages.txt").toStdString());
	for(int taskMark : {241, 242, 244})
	{
		avStr << getAverage(taskMark) << std::endl;
	}
	avStr.close();
#endif



	/// write into table file (with fromFileName data and vegetative)
#if 0
	/// sort by what?
	std::sort(std::begin(result), std::end(result),
			  [](const auto & a1, const auto a2)
	{
		return a1[sumSize + 6] < a2[sumSize + 6]; /// taskNumber
		return a1[sumSize + 7] < a2[sumSize + 7]; /// taskMark
		return a1[sumSize + 8] < a2[sumSize + 8]; /// operMark
	});
#endif
	std::ofstream outStr(tablePath.toStdString());
	outStr << makeTableCols(coords::egi::chans128groups) << std::endl;
	outStr << result << std::endl;
	outStr.close();




#if 0
	/// remove empty rows
	std::vector<int> inds{};
	for(int i = 0; i < table.rows(); ++i)
	{
		if(table[i].size() == 0) { inds.push_back(i); }
	}
	table.eraseRows(inds);

	tableStream << table;
	tableStream.close();
#endif
}

