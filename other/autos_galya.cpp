#include <other/autos.h>

#include <myLib/clustering.h>
#include <myLib/iitp.h>
#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>
#include <myLib/output.h>
#include <myLib/statistics.h>

 /// for std::defaultfloat
#include <ios>
#include <iostream>
#include <bits/ios_base.h>
#include <functional>

using namespace myOut;

namespace autos
{

QString getFeatureString(featuresMask in)
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


void calculateFeatures(const QString & pathWithEdfs,
					   const int numChan,
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
	const int Mask = DEFS.getAutosMask();

	if(Mask & featuresMask::wavelet)
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

		edfFile initEdf;
		initEdf.readEdfFile(filePath);

		/// different checks crutches
		if(initEdf.getNdr() == 0)
		{
			std::cout << "ndr = 0\t" << filesVec[i] << std::endl;
			continue;
		}

		if(initEdf.getNs() < numChan)
		{
			std::cout << "calculateFeatures: too few channels - " << filePath << std::endl;
			continue;
		}

		/// cout fileSize
		std::cout << edfs[i] << '\t'
				  << smLib::doubleRound(QFile(filePath).size() / std::pow(2, 10), 1)
				  << " kB" << std::endl;


		const QString preOutPath = outPath + "/" + initEdf.getExpName();

		matrix tmpData = initEdf.getData();
		tmpData.resizeRows(numChan); /// saves the data stored in first numChan rows

		switch(DEFS.getAutosUser())
		{
		case autosUser::Galya:
		{
			break;
		}
		case autosUser::Xenia:
		{
			tmpData = tmpData.subCols(0, 30 * initEdf.getFreq()); /// not resizeCols
			break;
		}
		case autosUser::XeniaFinalest:
		{
#if 01
			/// first 30 seconds
			tmpData = tmpData.subCols(0, 30 * initEdf.getFreq());	/// not resizeCols
#else
			/// second 30 secods
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
#endif
			tmpData.eraseRows({4, 9, 14});							/// skip Fz, Cz, Pz
			break;
		}
		default:
		{ /* do nothing */ }
		}
		countFeatures(tmpData, initEdf.getFreq(), Mask, preOutPath);
	}
}

void countFeatures(const matrix & inData,
				   const double srate,
				   const int Mask,
				   const QString & preOutPath)
{
	/// spectre will be count twice for alpha and FFT but I dont care
	for(const auto & num : FEATURES)
	{
		if(Mask & std::get<0>(num))
		{
			const QString outPath = preOutPath + "_" + std::get<1>(num) + ".txt";

			QFile::remove(outPath);
			std::ofstream outStr;
			outStr.open(outPath.toStdString());

			outStr << std::fixed;
			outStr.precision(4);

			const auto& f = std::get<2>(num);
			f(inData,
			  srate,
			  outStr);

			outStr.close();
		}
	}
}

void countFFT(const matrix & inData,
			  double srate,
			  std::ostream & outStr)
{

	matrix spectra(inData.rows(), 1); /// [chan][freq]
	for(int i = 0; i < inData.rows(); ++i)
	{
		/// norming is necessary
		auto helpSpectre = myLib::smoothSpectre(
							   myLib::spectreRtoR(inData[i]) *
							   myLib::spectreNorm(smLib::fftL(inData.cols()),
												  inData.cols(),
												  srate),
							   -1);
		spectra[i] = myLib::integrateSpectre(helpSpectre,
											 srate,
											 2,
											 19,
											 1);
	}


	switch(DEFS.getAutosUser())
	{
	case autosUser::XeniaFinalest:
	{
		for(uint i = 0; i < spectra.rows(); ++i)
		{
			for(uint j = 0; j < spectra[i].size(); ++j)
			{
				outStr << spectra[i][j] << "\t";
			}
			outStr << "\r\n";
		}
		break;
	}
	default: /// Xenia and Galya
	{
		for(uint j = 0; j < spectra[0].size(); ++j) /// 18 freqs
		{
			for(int i = 0; i < inData.rows(); ++i) /// 19 channels
			{
				outStr << spectra[i][j] << "\t";
			}
		}
		break;
	}
	}
}

void countLogFFT(const matrix & inData,
				 double srate,
				 std::ostream & outStr)
{
	matrix spectra(inData.rows(), 1); /// [chan][freq]
	for(int i = 0; i < inData.rows(); ++i)
	{
		/// norming is necessary
		auto helpSpectre = myLib::smoothSpectre(
							   myLib::spectreRtoR(inData[i]) *
							   myLib::spectreNorm(smLib::fftL(inData.cols()),
												  inData.cols(),
												  srate),
							   -1);
		spectra[i] = myLib::integrateSpectre(helpSpectre,
											 srate,
											 2,
											 19,
											 1);
	}

	switch(DEFS.getAutosUser())
	{
	case autosUser::XeniaFinalest:
	{
		for(uint i = 0; i < spectra.rows(); ++i)
		{
			for(uint j = 0; j < spectra[i].size(); ++j)
			{
				outStr << std::log10(spectra[i][j]) << "\t";
			}
			outStr << "\r\n";
		}
		break;
	}
	default:
	{
		for(uint j = 0; j < spectra[0].size(); ++j)
		{
			for(int i = 0; i < inData.rows(); ++i)
			{
				outStr << std::log(spectra[i][j]) << "\t";
			}
		}
	}
	}
}

void countAlphaPeak(const matrix & inData,
					double srate,
					std::ostream & outStr)
{
	std::vector<double> res{};
	for(int i = 0; i < inData.rows(); ++i)
	{
		/// norming is necessary, but why?
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
	switch(DEFS.getAutosUser())
	{
	case autosUser::XeniaFinalest:
	{
		for(auto val : res)
		{
			outStr << val << "\r\n";
		}
		break;
	}
	default:
	{
		for(auto val : res)
		{
			outStr << val << "\t";
		}
		break;
	}
	}
}

void countFracDim(const matrix & inData,
				  double srate,
				  std::ostream & outStr)
{
	switch(DEFS.getAutosUser())
	{
	case autosUser::XeniaFinalest:
	{
		for(int i = 0; i < inData.rows(); ++i)
		{
			outStr << myLib::fractalDimension(inData[i]) << "\r\n";
		}
		break;
	}
	default:
	{
		for(int i = 0; i < inData.rows(); ++i)
		{
			outStr << myLib::fractalDimension(inData[i]) << "\t";
		}
		break;
	}
	}

}

void countHilbert(const matrix & inData,
				  double srate,
				  std::ostream & outStr)
{

	const double hilbertFreqLimit = 40.;

	std::valarray<double> env;
	std::valarray<double> envSpec;

	std::vector<std::pair<double, double>> filters{
				std::make_pair(0, 0),		/// [0] no filter
				std::make_pair(4, 6),		/// [1] theta
				std::make_pair(8, 13),		/// [2] alpha
				std::make_pair(8, 10),		/// [3] low_alpha
				std::make_pair(10, 13),		/// [4] high_alpha
				std::make_pair(2, 20)		/// [5] band of interest
	};

	std::vector<std::vector<std::vector<double>>> hilb(filters.size()); /// [filter][chan][0-carr, 1-SD]

	for(uint numFilt = 0; numFilt < filters.size(); ++numFilt)
	{
		std::pair<double, double> filterLims = filters[numFilt];

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

		hilb[numFilt].resize(inData.rows());
		for(int i = 0; i < inData.rows(); ++i)
		{
			/// write envelope median spectre
			env = myLib::hilbertPieces(currMat[i]) ;
			envSpec = myLib::spectreRtoR(env);
			envSpec[0] = 0.;

			double helpDouble = 0.;
			double sumSpec = 0.;
			for(int j = 0;
				j < fftLimit(hilbertFreqLimit,
							 srate,
							 smLib::fftL( inData.cols() ));
				++j)
			{
				helpDouble += envSpec[j] * j;
				sumSpec += envSpec[j];
			}
			helpDouble /= sumSpec;
			helpDouble /= fftLimit(1.,
								   srate,
								   smLib::fftL( inData.cols() )); /// convert to Hz

			hilb[numFilt][i] = {helpDouble, smLib::sigma(env) / smLib::mean(env)};
		}
	}

	switch(DEFS.getAutosUser())
	{
	case autosUser::Galya:
	{
		for(int func : {0, 1}) /// carr or SD
		{
			for(int filt : {0, 2}) /// whole and alpha
			{
				for(int ch = 0; ch < inData.rows(); ++ch)
				{
					outStr << hilb[filt][ch][func] << "\t";
				}
			}
		}
		break;
	}
	case autosUser::Xenia:
	{
		for(int filt : {0, 1, 2}) /// whole, theta, alpha
		{
			for(int func : {0, 1})  /// carr or SD
			{
				for(int ch = 0; ch < inData.rows(); ++ch)
				{
					outStr << hilb[filt][ch][func] << "\t";
				}
			}
		}
		break;
	}
	case autosUser::XeniaFinalest:
	{
		for(int ch = 0; ch < inData.rows(); ++ch)
		{
			for(int filt : {0, 1, 2, 3, 4,  5}) /// whole, theta, alpha, low-alpha, high-alpha, 2-20
//			for(int filt : {3, 4}) /// 8-10, 10-13
//			int filt = 5; /// 2-20
			{
				for(int func : {0, 1})  /// carr or SD
				{
					outStr << hilb[filt][ch][func] << "\t";
				}
			}
			outStr << "\r\n";
		}
		break;
	}
	default:
	{ /* do nothing */ }
	}

}

void countWavelet(const matrix & inData,
				  double srate,
				  std::ostream & outStr)
{

#if WAVELET_MATLAB
	const int numOfFreqs = wvlt::cwt(inData[0], srate).rows(); /// pewpew


//	using funcType = double(*)(const std::valarray<double> &);
	using funcType = std::function<double(const std::valarray<double> &)>;
	std::vector< funcType > funcs;
	funcs.push_back(static_cast<funcType>(smLib::mean<double>));
	funcs.push_back(static_cast<funcType>(smLib::sigma<std::valarray<double>>));
	funcs.push_back(static_cast<funcType>(smLib::median<std::valarray<double>>));

	std::vector<matrix> outData{};
	for(const auto & func : funcs)
	{
		matrix dataToWrite(numOfFreqs, inData.rows()); /// [freq][chan]

		for(int j = 0; j < inData.rows(); ++j)
		{
			matrix m = wvlt::cwt(inData[j], srate);
			for(int i = 0; i < numOfFreqs; ++i) /// each frequency
			{
				dataToWrite[i][j] = func(m[i]);
			}
		}
		outData.push_back(dataToWrite);


	}

	switch(DEFS.getAutosUser())
	{
	case autosUser::XeniaFinalest:
	{
		for(int j = 0; j < inData.rows(); ++j) /// channels
		{
			for(int a = 0; a < outData.size(); ++a)
			{
				for(int i = 0; i < numOfFreqs; ++i)
				{
					outStr << outData[a][i][j] << "\t";
				}
			}
			outStr << "\r\n";
		}
		break;
	}
	default:
	{
		for(int a = 0; a < outData.size(); ++a) /// num of functions
		{
			for(int i = 0; i < numOfFreqs; ++i) /// 19 freqs
			{
				for(int j = 0; j < inData.rows(); ++j) /// 19 channels
				{
					outStr << outData[a][i][j] << "\t";
				}
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
	for(auto f : {myLib::hjorthMobility, myLib::hjorthComplexity})
	{
		for(auto row : inData)
		{
			outStr << f(row) << "\t";
		}
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

	int lowFr = fftLimit(freq - 0.5, DEFS.getFreq(), fftLen);
	int higFr = fftLimit(freq + 0.5, DEFS.getFreq(), fftLen);

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

void rhythmAdoption(const QString & filesPath,
					const QString & restMark,
					const QString & stimType)
{
	const int numChans = 19;
	const std::vector<int> freqs{2, 4, 8, 16};
	matrix res(freqs.size(), numChans);

	const QString restFileName = QDir(filesPath).entryList({"*" + restMark + "*"})[0];
	const QString ExpName = restFileName.left(restFileName.indexOf(restMark));

	edfFile restEdf;
	restEdf.readEdfFile(filesPath + "/" + restFileName);
	const matrix restData = restEdf.getData();

	for(uint j = 0; j < freqs.size(); ++j)
	{
		edfFile currFile;
		currFile.readEdfFile(filesPath + "/"
							 + ExpName + "_" + nm(freqs[j]) + stimType + ".edf");

		for(int i = 0; i < numChans; ++i)
		{
			res[j][i] = countRhythmAdoption(restData[i],
											currFile.getData(i),
											freqs[j]);
		}
	}

	QDir dr(filesPath);
	dr.cdUp();
	dr.mkdir("out");
	dr.cd("out");

	std::ofstream outFile((dr.absolutePath() + "/"
						   + ExpName + "_" + stimType + ".txt").toStdString());
	outFile.precision(3);
	for(int i = 0; i < res.rows(); ++i)
	{
		outFile << res[i] << "\t"; ///
	}
	outFile.close();
}

void rhythmAdoptionGroup(const QString & groupPath,
						 const QString & restMark,
						 const QString & stimType)
{
	auto lst = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for(const QString & guy : lst)
	{
		if(guy == "out") { continue; }

		rhythmAdoption(groupPath + "/" + guy,
					   restMark,
					   stimType);
	}

	QString subdir = groupPath.mid(groupPath.lastIndexOf("/") + 1);

	autos::ArrangeFilesToTable(groupPath + "/out",
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
		std::this_thread::sleep_for(std::chrono::seconds(15)); /// wait for copy to end?
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
							   32,
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

void Xenia_repairTable(const QString & initPath,
					   const QString & repairedPath,
					   const QString & groupsPath,
					   const QString & namesPath)
{
	QFile fil(initPath);
	fil.open(QIODevice::ReadWrite);
	auto arr = fil.readAll();
	fil.reset();
	arr.replace(",", ".");
	arr.replace(" ", "_");
	fil.write(arr);

	fil.reset();
	QTextStream str(&fil);

	QFile newFil(repairedPath); newFil.open(QIODevice::WriteOnly);
	QTextStream newStr(&newFil);

	QFile namesFil(namesPath); namesFil.open(QIODevice::WriteOnly);
	QTextStream namesStr(&namesFil);

	QFile groupsFil(groupsPath); groupsFil.open(QIODevice::WriteOnly);
	QTextStream groupsStr(&groupsFil);

	QString name;
	int group;
	QString rest;

	while(1)
	{
		str >> name >> group;
		if(str.atEnd())
		{
			break;
		}
		namesStr << name << "\r\n";
		groupsStr << group << "\r\n";

		str.skipWhiteSpace();
		rest = str.readLine();
		newStr << rest << "\r\n";
	}

	namesFil.close();
	groupsFil.close();
	newFil.close();
	fil.close();
}


/// old
void Xenia_TBI(const QString & tbi_path)
{
	/// TBI Xenia cut, process, tables
	DEFS.setNtFlag(false);

	QStringList markers{"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"};
//	QStringList markers{"_isopropanol", "_vanilla", "_needles", "_brush",
//						"_cry", "_fire", "_flower", "_wc"};



	QStringList subdirs = QDir(tbi_path).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	if(subdirs.isEmpty())
	{
		subdirs = QStringList{""};
	}
	else
	{
		repair::toLatinDir(tbi_path, {});
		repair::deleteSpacesFolders(tbi_path);
		subdirs = QDir(tbi_path).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	}


#if 01
	/// count
	for(const QString & subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir;

		repair::toLatinDir(workPath, {});
		repair::toLowerDir(workPath, {});
		repair::deleteSpacesDir(workPath, {});

		autos::cutFilesInFolder(workPath, 8);
		continue;

		/// list of guys
		QStringList guys = QDir(workPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		for(const QString & guy : guys)
		{

			repair::deleteSpacesFolders(workPath + "/" + guy);
//			repair::toLatinDir(workPath + "/" + guy, {});
//			repair::toLowerDir(workPath + "/" + guy, {});
//			continue;



			QStringList t = QDir(workPath + "/" + guy).entryList(def::edfFilters);
			if(t.isEmpty()) { continue; }

			QString ExpName = t[0];
			ExpName = ExpName.left(ExpName.lastIndexOf('_'));

			/// filter?
			if(1)
			{
				autos::refilterFolder(workPath + "/" + guy,
									  1.,
									  30.);
			}

			/// cut?
			if(0)
			{
				autos::cutFilesInFolder(workPath + "/" + guy,
								8,
								workPath + "_cut/" + guy);
			}

			/// process?
			if(1)
			{
				autos::calculateFeatures(workPath + "/" + guy,
									   19,
									   workPath + "_tmp");
			}

			QStringList fileNames;
			for(const QString & marker : markers)
			{
				fileNames.clear();
				for(featuresMask type : {
					featuresMask::alpha,
					featuresMask::fracDim,
					featuresMask::Hilbert,
					featuresMask::spectre,
					featuresMask::wavelet})
				{
					QString typ = "_" + autos::getFeatureString(type);
					fileNames << ExpName + marker + typ + ".txt";
				}
				autos::ArrangeFilesToLine(workPath + "_tmp",
										  fileNames,
										  workPath + "_tmp2" + "/"
										  + ExpName + marker + ".txt"); /// guy <-> ExpName
			}

			fileNames.clear();
			for(const QString & marker : markers)
			{
				fileNames <<  ExpName + marker + ".txt"; /// guy <-> ExpName
			}
			autos::ArrangeFilesToLine(workPath + "_tmp2",
									  fileNames,
									  workPath + "_OUT" + "/"
									  + ExpName + ".txt"); /// guy <-> ExpName
		}
	}
#endif

#if 0
	/// make tables by stimulus
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir + "_tmp2";
		for(QString marker : markers)
		{
			autos::makeTableFromRows(workPath,
									 tbi_path + "/" + subdir + "_table" + marker + ".txt",
									 marker);
		}
	}
#endif


#if 0
	/// make tables whole
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir + "_OUT";
		autos::makeTableFromRows(workPath,
								 tbi_path + "/" + subdir + "_all" + ".txt");
	}
#endif

#if 0
	/// people list
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir + "_OUT";
		QString outFile = tbi_path + "/" + subdir + "_people.txt";
		std::ofstream outStr;
		outStr.open(outFile.toStdString());

		for(QString fileName : QDir(workPath).entryList({"*.txt"},
														QDir::Files,
														QDir::Name))
		{
			outStr << fileName.remove(".txt") << std::endl;
		}
		outStr.close();
	}
#endif
}


void Xenia_TBI_final(const QString & finalPath,
					 QString outPath)
{
	QTime myTime;
	myTime.start();

	/// TBI Xenia cut, process, tables
	DEFS.setNtFlag(false);
	const std::vector<QString> tbiMarkers{"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"};

	QStringList subdirs = QDir(finalPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	if(subdirs.isEmpty())
	{
		subdirs = QStringList{""};
	}
	else
	{
		repair::toLatinDir(finalPath, {});
		repair::deleteSpacesFolders(finalPath);
		subdirs = QDir(finalPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	}

	if(outPath == QString())
	{
		outPath = finalPath + "_out";
	}

	if(!QDir(outPath).exists())
	{
		QDir().mkpath(outPath);
	}


	/// count
	for(const QString & subdir : subdirs)
	{
		const QString groupPath = finalPath + "/" + subdir;

		repair::toLatinDir(groupPath, {});
		repair::toLowerDir(groupPath, {});
		repair::deleteSpacesDir(groupPath, {});

		/// list of guys
		QStringList guys = QDir(groupPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		for(const QString & guy : guys)
		{
			const QString guyPath = groupPath + "/" + guy;

			if(0) /// repair fileNames
			{
				repair::deleteSpacesFolders(guyPath);
				repair::toLatinDir(guyPath);
				repair::toLowerDir(guyPath);
			}

			QStringList edfs = QDir(guyPath).entryList(def::edfFilters);
			if(edfs.isEmpty())
			{
				std::cout << "Xenia_TBI_final: guyPath is empty " << guyPath << std::endl;
				continue;
			}
			QString ExpName = edfs[0];
			ExpName = ExpName.left(ExpName.lastIndexOf('_'));


			/// physMinMax & holes
			if(0)
			{
				repair::physMinMaxDir(guyPath);
				repair::holesDir(guyPath,
								 19,
								 guyPath);	/// rewrite after repair
			}

			/// rereference
			if(0)
			{
//				autos::rereferenceFolder(guyPath, "Ar");
			}

			/// filter?
			if(0)
			{
				/// already done ?
				autos::refilterFolder(guyPath,
									  1.6,
									  30.);
			}

//			continue;

			/// cut?
			if(0)
			{
				autos::cutFilesInFolder(guyPath,
								8,
								groupPath + "_cut/" + guy);
			}


			outPath = guyPath + "/out";

			/// process?
			if(1)
			{
				autos::calculateFeatures(guyPath, 19, outPath);
			}

			/// make one line file for each stimulus
			if(1)
			{
				for(const QString & mark : tbiMarkers)
				{
					QStringList fileNamesToArrange;
					for(featuresMask func : {
						featuresMask::spectre,
						featuresMask::fracDim,
						featuresMask::Hilbert,
						featuresMask::wavelet,
						featuresMask::alpha})
					{
						fileNamesToArrange.push_back(ExpName + mark
													 + "_" + autos::getFeatureString(func) + ".txt");
					}
					std::cout << fileNamesToArrange << std::endl << std::endl;
					autos::ArrangeFilesToLine(outPath,
											  fileNamesToArrange,
											  outPath + "/" + ExpName + mark + ".txt");
				}
			}

			/// make whole line from all stimuli
			if(1)
			{
				QStringList fileNamesToArrange;
				for(const QString & mark : tbiMarkers)
				{
					fileNamesToArrange.push_back(ExpName + mark + ".txt");
				}
				autos::ArrangeFilesToLine(outPath,
										  fileNamesToArrange,
										  outPath + "/" + ExpName + ".txt");

				QFile::copy(outPath + "/" + ExpName + ".txt",
							finalPath + "_out/" + ExpName + ".txt");
			}
//			return; /// only first guy from first subdir
		}
	}
	/// make tables whole and people list
	autos::ArrangeFilesToTable(finalPath + "_out",
							 finalPath + "_out/all.txt",
							 true);

	std::cout << "Xenia_TBI_final: time elapsed = "
			  << myTime.elapsed() / 1000. << " sec" << std::endl;
}


/// 8-Mar-2018
void Xenia_TBI_finalest(const QString & finalPath,
						const QString & outPath,
						const std::vector<QString> markers)
{
	DEFS.setNtFlag(false);
	if(!QDir(outPath).exists()) { QDir().mkpath(outPath); }

	for(const QString & subdir : {"Healthy", "Moderate", "Severe"})
	{
		const QString groupPath = finalPath + "/" + subdir;

		/// list of guys
		QStringList guys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(const QString & guy : guys)
		{
			const QString guyPath = groupPath + "/" + guy;

			if(0) /// repair fileNames
			{
				repair::deleteSpacesFolders(guyPath);
				repair::toLatinDir(guyPath);
				repair::toLowerDir(guyPath);
			}

			QStringList edfs = QDir(guyPath).entryList(def::edfFilters);
			if(edfs.isEmpty())
			{
				std::cout << "Xenia_TBI_final: guyPath is empty " << guyPath << std::endl;
				continue;
			}
			QString ExpName = edfs[0];
			ExpName = ExpName.left(ExpName.lastIndexOf('_'));


			/// physMinMax & holes
			if(0)
			{
				repair::physMinMaxDir(guyPath);
				repair::holesDir(guyPath,
								 19,
								 guyPath);	/// rewrite after repair
			}

			/// rereference
			if(0)
			{
//				autos::rereferenceFolder(guyPath, "Ar");
			}

			/// filter?
			if(0)
			{
				/// already done ?
				autos::refilterFolder(guyPath,
									  1.6,
									  30.);
			}

//			continue;

			/// cut?
			if(0)
			{
				autos::cutFilesInFolder(guyPath,
								8,
								groupPath + "_cut/" + guy);
			}


//			myLib::cleanDir(outPath);

			/// process?
			if(01)
			{
				autos::calculateFeatures(guyPath, 19, outPath);
			}
		}
	}
//	exit(0);

	if(01)
	{
		/// make full matrices for each type of feature

		/// read guys_finalest.txt to guys
		std::vector<QString> guys{};
		QFile fil("/media/Files/Data/Xenia/guys_finalest.txt");
		fil.open(QIODevice::ReadOnly);
		while(1)
		{
			QString guy = fil.readLine();
			guy.chop(1); /// chop \n
			if(guy.isEmpty()) { break; }
			guys.push_back(guy);
		}
		fil.close();

		for(featuresMask feature : DEFS.getAutosMaskArray())
		{
			std::vector<QString> filesToVertCat{};
			for(const QString & mark : markers)
			{
				for(const QString & guy : guys)
				{
					filesToVertCat.push_back(
								outPath + "/" +				/// path or name
								guy
								+ mark
								+ "_" + autos::getFeatureString(feature)
								+ ".txt");
				}
			}
			autos::ArrangeFilesVertCat(filesToVertCat,
									   outPath + "/table_"
									   + autos::getFeatureString(feature) + ".txt");

		}
	}

}




void ArrangeFilesVertCat(const std::vector<QString> pathes,
						 const QString & outPath)
{
	QFile outStr(outPath);
	outStr.open(QIODevice::WriteOnly);
	for(const QString & filePath : pathes)
	{
		QFile fil(filePath);
		fil.open(QIODevice::ReadOnly);
		auto contents = fil.readAll();
		fil.close();

		outStr.write(contents);
		if(!contents.endsWith("\r\n")) { outStr.write("\r\n"); }
	}
	outStr.close();
}

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
												  QDir::Name
//												  | QDir::Reversed
												  ))
	{
		if((fileName == tableName)
		   || fileName.contains("people")) { continue; }

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



void ArrangeFilesToLine(const QString & dirPath,
						const QStringList & fileNames,
						const QString & outFilePath)
{
	QDir().mkpath(myLib::getDirPathLib(outFilePath));

#if 0
	/// old 2.12.2017
	std::ofstream outStr;
	outStr.open(outFilePath.toStdString());

	std::ifstream inStr;

	for(const QString & fileName : fileNames)
	{
		inStr.open((dirPath + "/" + fileName).toStdString());

		double val;
		while(inStr.good())
		{
			inStr >> val;
			if(!inStr.eof())
			{
				outStr << val << '\t';
			}
		}
		inStr.close();
	}
	outStr.close();
#else
	/// new 2.12.2017
	QFile out(outFilePath); out.open(QIODevice::WriteOnly);

	for(const QString & fileName : fileNames)
	{
		QFile in(dirPath + "/" + fileName); in.open(QIODevice::ReadOnly);
		out.write(in.readAll());
		in.close();
	}
	out.close();
#endif
}


void cutOneFile(const QString & filePath,
				const int wndLen,
				const QString & outPath)
{
	edfFile initEdf;
	initEdf.readEdfFile(filePath);


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
					fmin((i + 1) * fr * wndLen, initEdf.getDataLen()),
					QString(outPath
					#if ADD_DIR
							+ "/" + addDir
					#endif
							+ "/" + initEdf.getExpName()
							+ "_wnd_" + rn(
								i + 1,
								floor(std::log10(numOfWinds)) + 1)
							+ ".edf"));
	}
}

void cutFilesInFolder(const QString & path,
			  const int wndLen,
			  QString outPath)
{

	const QString outDir = myLib::getFileName(path) + "_winds";
	const QString smallsDir = myLib::getFileName(path) + "_smalls";

	QDir tmpDir(path);
	tmpDir.mkdir(smallsDir);
	const QString smallsPath = tmpDir.absolutePath() + "/" + smallsDir;

	if(outPath.isEmpty())
	{
		tmpDir.mkdir(outDir);
		outPath = tmpDir.absolutePath() + "/" + outDir;
	}
	else
	{
		tmpDir.mkpath(outPath);
	}

	/// to change
	const QString logPath = def::GalyaFolder + "/log.txt";
	std::ofstream logStream(logPath.toStdString(), std::ios_base::app);

	const QStringList leest1 = tmpDir.entryList(def::edfFilters);
	const auto filesVec = leest1.toVector();

	/// ??????????????????????
//#ifdef _OPENMP
//	omp_set_dynamic(0);
//	omp_set_num_threads(3);
//#pragma omp parallel
//#pragma omp for nowait
//#endif
	for(const auto & fileName : filesVec)
	{
		std::cout << fileName << std::endl;
		QString helpString = tmpDir.absolutePath() + "/" + fileName;
		edfFile initEdf;
		initEdf.readEdfFile(helpString, true);

		/// some check for small
		if(initEdf.getNdr() * initEdf.getDdr() <= wndLen )
		{
			QFile::remove(smallsPath + "/" + initEdf.getFileNam());
			QFile::copy(initEdf.getFilePath(),
						smallsPath + "/" + initEdf.getFileNam());

			std::cout << "smallFile \t" << initEdf.getFileNam() << std::endl;
			logStream << initEdf.getFilePath() << "\t" << "too small" << "\n";
			continue;
		}

		cutOneFile(helpString,
				   wndLen,
				   outPath);
	}
	logStream.close();

}



void rereferenceFolder(const QString & procDirPath,
					   const QString & newRef)
{
	const QStringList filesList = QDir(procDirPath).entryList(def::edfFilters,
															  QDir::NoFilter,
															  QDir::Size | QDir::Reversed);

	for(const QString & fileName : filesList)
	{
		QString helpString = procDirPath + "/" + fileName;
		edfFile fil;
		fil.readEdfFile(helpString);
		fil.rereferenceData(newRef).writeEdfFile(helpString);
	}
}

void refilterFolder(const QString & procDirPath,
					  double lowFreq,
					  double highFreq,
					  bool isNotch)
{

	const QStringList filesList = QDir(procDirPath).entryList(def::edfFilters,
															  QDir::NoFilter,
															  QDir::Size | QDir::Reversed);

	for(const QString & fileName : filesList)
	{
		QString helpString = procDirPath + "/" + fileName;
		edfFile fil;
		fil.readEdfFile(helpString);
		fil.refilter(lowFreq, highFreq, isNotch).writeEdfFile(helpString);
	}
}

void rewriteNew(const QString & inPath)
{
	auto lst = QDir(inPath).entryList({"*_new.edf"});
	for(const QString & in : lst)
	{
		QString oldName = in;
		oldName.replace("_new.edf", ".edf");

		QFile::remove(inPath + "/" + oldName);
		QFile::rename(inPath + "/" + in,
					  inPath + "/" + oldName);
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
						   QString outPath)
{
	QTime myTime;
	myTime.start();

	DEFS.setNtFlag(false);

	if(outPath == QString())
	{
		outPath = inPath + "_out";
	}

	if(!QDir(outPath).exists())
	{
		QDir().mkpath(outPath);
	}


	QStringList edfs = QDir(inPath).entryList(def::edfFilters);
	if(edfs.isEmpty())
	{
		std::cout << "Galya_tactile: inPath is empty " << inPath << std::endl;
	}

	if(0) /// repair fileNames
	{
		repair::deleteSpacesFolders(inPath);
		repair::toLatinDir(inPath);
		repair::toLowerDir(inPath);
	}

	if(0)
	{
		repair::physMinMaxDir(inPath);
		repair::holesDir(inPath,
						 19,		/// HOW MANY CHANNELS ???
						 inPath);	/// rewrite after repair
	}

	/// rereference
	if(0)
	{
		//		autos::rereferenceFolder(inPath, "Ar");
	}

	/// filter?
	if(0)
	{
		/// already done ?
		autos::refilterFolder(inPath,
							  1.6,
							  30.);
	}

	/// cut?
	if(0)
	{
		autos::cutFilesInFolder(inPath,
						10,
						inPath + "_cut/");
	}

	outPath = inPath + "/out";

	/// process?
	if(01)
	{
		/// clear outFolder
		myLib::cleanDir(inPath + "/out", "txt", true);
		autos::calculateFeatures(inPath, 19, outPath);
	}


	/// make one line file for each guy
	if(0)
	{
		for(QString ExpName : edfs)
		{
			ExpName = ExpName.left(ExpName.indexOf('.'));

			QStringList fileNamesToArrange;
			for(featuresMask func : {
				featuresMask::alpha,
				featuresMask::spectre,
				featuresMask::Hilbert,
				featuresMask::fracDim,
				featuresMask::Hjorth,
				featuresMask::wavelet
		})
			{
				const QString fileName = ExpName
										 + "_" + autos::getFeatureString(func) + ".txt";
				fileNamesToArrange.push_back(fileName);

				if(!QFile::exists(outPath + "/" + fileName))
				{
					std::cout << "File doesn't exist: " << fileName << std::endl;
					std::ofstream outStr;
					outStr.open((outPath + "/" + fileName).toStdString());
					for(int i = 0; i < autos::getFileLength(func); ++i)
					{
						outStr << 0 << '\t';
					}
					outStr.close();
				}
			}
			autos::ArrangeFilesToLine(outPath,
									  fileNamesToArrange,
									  outPath + "/" + ExpName + ".txt");

			/// copy files into _out
			if(1)
			{
				QFile::copy(outPath + "/" + ExpName + ".txt",
							inPath + "_out/" + ExpName + ".txt");
			}
		}
	}


	/// make table for each feature
	if(01)
	{
		QDir().mkpath(inPath + "/out2");
		for(featuresMask func : {
			featuresMask::alpha,
			featuresMask::spectre,
			featuresMask::Hilbert,
			featuresMask::fracDim
	})
		{
			const auto str = autos::getFeatureString(func);
			autos::ArrangeFilesToTable(outPath,
									   inPath + "/out2/" + str + ".txt",
									   true,
									   str);
		}
	}


	/// make tables whole and people list
	if(0)
	{
		autos::ArrangeFilesToTable(inPath + "_out",
								 inPath + "_out/all.txt",
								 true);
	}

	std::cout << "ProcessAllInOneFolder: time elapsed = "
			  << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void ProcessByFolders(const QString & inPath,
					  const std::vector<QString> & markers)
{
	QTime myTime;
	myTime.start();

	const int numChan = 16;
//	const int numChan = 31; /// MRI and other

	if(!QDir(inPath + "_out").exists())
	{
		QDir().mkdir(inPath + "_out");
	}

	/// calculation
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

		/// physMinMax & holes
		if(0)
		{
			repair::physMinMaxDir(guyPath);
			repair::holesDir(guyPath,
							 numChan,
							 guyPath);	/// rewrite after repair
//			continue;
		}

		/// rereference
		if(0)
		{
			autos::rereferenceFolder(guyPath, "Ar"); /// check Ar
		}

		/// filter?
		if(0)
		{
			/// already done ?
			autos::refilterFolder(guyPath,
								  1.6,
								  30.);
//			continue;
		}

		/// cut?
		if(0)
		{
			autos::cutFilesInFolder(guyPath,
									8,
									inPath + "_cut/");
		}


		const QString guyOutPath = guyPath + "/out";

		/// process?
		if(01)
		{
			/// clear outFolder
			myLib::cleanDir(guyPath + "/out", "txt", true);
			autos::calculateFeatures(guyPath, numChan, guyOutPath);
		}

		/// make one line file for each stimulus
		if(01)
		{
			for(const QString & mark : markers)
			{
				QStringList fileNamesToArrange;

				for(featuresMask func : DEFS.getAutosMaskArray())
				{
					const QString fileName = ExpName + mark
											 + "_" + autos::getFeatureString(func) + ".txt";
					fileNamesToArrange.push_back(fileName);

					if(01) /// create files if they are absent
					{
						if(!QFile::exists(guyOutPath + "/" + fileName))
						{
							std::ofstream outStr;
							outStr.open((guyOutPath + "/" + fileName).toStdString());
							for(int i = 0; i < autos::getFileLength(func); ++i)
							{
								outStr << " " << '\t';
//								outStr << 0 << '\t';
							}
							outStr.close();
						}
					}
				}
				autos::ArrangeFilesToLine(guyOutPath,
										  fileNamesToArrange,
										  guyOutPath + "/" + ExpName + mark + ".txt");
			}
		}

		/// make whole line from all stimuli
		if(1)
		{
			QStringList fileNamesToArrange;
			for(const QString & mark : markers)
			{
				fileNamesToArrange.push_back(ExpName + mark + ".txt");
			}
			autos::ArrangeFilesToLine(guyOutPath,
									  fileNamesToArrange,
									  guyOutPath + "/" + ExpName + ".txt");
		}

		/// copy files into _out
		if(1)
		{
			QFile::remove(inPath + "_out/" + ExpName + ".txt");
			QFile::copy(guyOutPath + "/" + ExpName + ".txt",
						inPath + "_out/" + ExpName + ".txt");
		}
	}

	/// make tables whole and people list
	autos::ArrangeFilesToTable(inPath + "_out",
							   inPath + "_out/all.txt",
							   true);

	std::cout << "ProcessByFolders: time elapsed = "
			  << myTime.elapsed() / 1000. << " sec" << std::endl;
}

} /// end of namespace autos
