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


void GalyaProcessing(const QString & procDirPath,
					 const int numChan,
					 QString outPath)
{

	QDir dir;
	dir.cd(procDirPath);
	if(outPath.isEmpty())
	{
		const QString outDir = myLib::getFileName(procDirPath) + "_out";
		dir.mkdir(outDir);
		outPath = dir.absolutePath() + "/" + outDir;
	}
	else
	{
		dir.mkpath(outPath);
	}

	const QStringList filesList = dir.entryList(def::edfFilters,
												QDir::NoFilter,
												QDir::Size
												| QDir::Reversed
												);
	const auto filesVec = filesList.toVector();


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
//#ifdef _OPENMP
//	omp_set_dynamic(0);
//	omp_set_num_threads(3);
//#pragma omp parallel
//#pragma omp for nowait
//#endif
	for(int i = 0; i < filesVec.size(); ++i)
	{
		QString helpString = dir.absolutePath() + "/" + filesVec[i];

		edfFile initEdf;
		initEdf.readEdfFile(helpString);

		/// different checks
		if(initEdf.getNdr() == 0)
		{
			std::cout << "ndr = 0\t" << filesVec[i] << std::endl;
			continue;
		}

		if(initEdf.getNs() < numChan)
		{
			std::cout << "GalyaProcessing: too few channels - " << procDirPath << std::endl;
			continue;
		}

		std::cout << filesList[i] << '\t'
				  << smLib::doubleRound(QFile(helpString).size() / pow(2, 10), 1) << " kB" << std::endl;

		const QString preOutPath = outPath + "/" + myLib::getFileName(helpString, false);

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
				tmpData = tmpData.subCols(0, 30 * 250); /// not resizeCols
				break;
			}
			default:
			{ /* do nothing */ }
		}
		const matrix countData = std::move(tmpData);

		countFeatures(countData, initEdf.getFreq(), Mask, preOutPath);
	}
}

void countFeatures(const matrix & inData,
				   double srate,
				   const int Mask,
				   const QString & preOutPath)
{
	/// spectre will be count twice for alpha and FFT but I dont care
	for(int num = 0; num < FEATURES.size(); ++num)
	{
		if(Mask & std::get<0>(FEATURES[num]))
		{
			QString outPath = preOutPath + "_" + std::get<1>(FEATURES[num]) + ".txt";

			QFile::remove(outPath);
			std::ofstream outStr;
			outStr.open(outPath.toStdString());

			outStr << std::fixed;
			outStr.precision(4);

			auto f = std::get<2>(FEATURES[num]);
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

	std::vector<std::vector<double>> spectra(inData.rows()); /// [chan][freq]
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
											 inData.cols(),
											 srate);
	}

	for(int j = 0; j < spectra[0].size(); ++j)
	{
		for(int i = 0; i < inData.rows(); ++i)
		{
			outStr << spectra[i][j] << "\t";
		}
	}
}
void countAlpha(const matrix & inData,
				double srate,
				std::ostream & outStr)
{
	for(int i = 0; i < inData.rows(); ++i)
	{
		/// norming is necessary
		auto helpSpectre = myLib::smoothSpectre(
							   myLib::spectreRtoR(inData[i]) *
							   myLib::spectreNorm(smLib::fftL(inData.cols()),
												  inData.cols(),
												  srate),
							   -1);
		outStr << myLib::alphaPeakFreq(helpSpectre,
									   inData.cols(),
									   srate) << "\t";
	}
}

void countFracDim(const matrix & inData,
				  double srate,
				  std::ostream & outStr)
{
	for(int i = 0; i < inData.rows(); ++i)
	{
		outStr << myLib::fractalDimension(inData[i]) << "\t";
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
		std::make_pair(0.5, 70), /// i.e. no filter
		std::make_pair(4, 6),
		std::make_pair(8, 13)
	};

	std::vector<std::vector<std::vector<double>>> hilb(filters.size()); // [filter][chan][0-carr, 1-SD]

	for(int numFilt = 0; numFilt < filters.size(); ++numFilt)
	{
		std::pair<double, double> filterLims = filters[numFilt];

		matrix currMat = myLib::refilterMat(inData,
											filterLims.first,
											filterLims.second,
											false,
											srate);
		hilb[numFilt].resize(inData.rows());
		for(int i = 0; i < inData.rows(); ++i)
		{
			// write envelope median spectre
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
								   smLib::fftL( inData.cols() )); // convert to Hz

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

	for(auto func : {
//		smLib::max,
//		smLib::min,
//		smLib::mean,
//		smLib::median,
//		smLib::sigma,
		smLib::sigmaToMean
})
	{
		matrix dataToWrite(numOfFreqs, inData.rows());

		for(int j = 0; j < inData.rows(); ++j)
		{
			matrix m = wvlt::cwt(inData[j], srate);
			for(int i = 0; i < numOfFreqs; ++i) /// each frequency
			{
				dataToWrite[i][j] = func(m[i]);
			}
		}

		for(int i = 0; i < numOfFreqs; ++i)
		{
			for(int j = 0; j < inData.rows(); ++j)
			{
				outStr << dataToWrite[i][j] << "\t";
			}
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

void countLogFFT(const matrix & inData,
				 double srate,
				 std::ostream & outStr)
{
	std::vector<std::vector<double>> spectra(inData.rows()); /// [chan][freq]
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
											 inData.cols(),
											 srate);
	}

	for(int j = 0; j < spectra[0].size(); ++j)
	{
		for(int i = 0; i < inData.rows(); ++i)
		{
			outStr << std::log(spectra[i][j]) << "\t";
		}
	}
}

/// further generalizations
void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag)
{
	DEFS.setNtFlag(false);

	for(QString guy : guyList)
	{
		if(cutOnlyFlag)
		{
			autos::cutFilesInFolder(def::mriFolder + "/" + guy, 2);
			continue;
		}

		/// PEWPEWPWEPWEPPEWWPEWPEEPPWPEPWEPWEPWPEPWEPWPEWPEWPEPWEPWEPWEPWPEWP
//		autos::GalyaFull(def::mriFolder +
//						 "/" + guy +
//						 "/" + guy + "_winds_cleaned");

		QString outPath = def::mriFolder + "/OUT/" + guy;
		QString dropPath = "/media/Files/Dropbox/DifferentData/EEG-MRI/Results";
		QStringList files = QDir(outPath).entryList({"*.txt"});

		/// make archive
		QString cmd = "cd " + outPath + " && " +
					  "rar a " + guy + ".rar ";
		for(QString a : files)
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

	for(QString guy : guyList)
	{
		break;
		const QString outPath = workDir + "/" + guy + "/out";

		myLib::cleanDir(outPath);
		autos::GalyaProcessing(workDir + "/" + guy,
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
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir;

		repair::toLatinDir(workPath, {});
		repair::toLowerDir(workPath, {});
		repair::deleteSpacesDir(workPath, {});

		autos::cutFilesInFolder(workPath, 8);
		continue;

		/// list of guys
		QStringList guys = QDir(workPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		for(QString guy : guys)
		{

			repair::deleteSpacesFolders(workPath + "/" + guy);
//			repair::toLatinDir(workPath + "/" + guy, {});
//			repair::toLowerDir(workPath + "/" + guy, {});

//			continue;



			QStringList t = QDir(workPath + "/" + guy).entryList(def::edfFilters);
			if(t.isEmpty()) continue;

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
				autos::GalyaProcessing(workPath + "/" + guy,
									   19,
									   workPath + "_tmp");
			}

			QStringList fileNames;
			for(QString marker : markers)
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
			for(QString marker : markers)
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
	for(QString subdir : subdirs)
	{
//		break;
		const QString groupPath = finalPath + "/" + subdir;

		repair::toLatinDir(groupPath, {});
		repair::toLowerDir(groupPath, {});
		repair::deleteSpacesDir(groupPath, {});

		/// list of guys
		QStringList guys = QDir(groupPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		for(QString guy : guys)
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
				autos::GalyaProcessing(guyPath, 19, outPath);
			}

			/// make one line file for each stimulus
			if(1)
			{
				for(QString mark : tbiMarkers)
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
				for(QString mark : tbiMarkers)
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

	for(const QString & fileName : deer.entryList({"*" + auxFilter + "*.txt"},
												  QDir::Files,
												  QDir::Name
//												  | QDir::Reversed
												  ))
	{
		if((fileName == tableName)
		   || fileName.contains("people")) continue;

		if(writePeople)
		{
			fileNames << fileName << "\n";
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
	if(initEdf.getEdfPlusFlag()) // if annotations
	{
		initEdf.removeChannels({initEdf.getMarkChan()}); // remove Annotations
	}

	int fr = initEdf.getFreq();
	const int numOfWinds = ceil(initEdf.getDataLen() / fr / wndLen);


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
								floor(log10(numOfWinds)) + 1)
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
	for(int i = 0; i < filesVec.size(); ++i)
	{
		std::cout << filesVec[i] << std::endl;
		QString helpString = tmpDir.absolutePath() + "/" + filesVec[i];
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

void EdfsToFolders(const QString & inPath)
{
	auto lst = QDir(inPath).entryList(def::edfFilters);
	for(QString in : lst)
	{
		QString ExpName = in.left(in.lastIndexOf("_"));
		if(!QDir(inPath + "/" + ExpName).exists())
		{
			QDir().mkpath(inPath + "/" + ExpName);
		}
		QFile::copy(inPath + "/" + in,
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
		autos::GalyaProcessing(inPath, 19, outPath);
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
					  QString outPath)
{
	QTime myTime;
	myTime.start();
	/// to arguments
//	const std::vector<QString> markers{"_buk", "_kis", "_rol", "_sch", "_fon"};

//	const std::vector<QString> markers{"_rest"};
//	const std::vector<QString> markers{"_buk", "_kis", "_rol", "_sch", "_og", "_zg"};
//	const std::vector<QString> markers{"_2sv", "_2zv", "_4sv", "_4zv",
//									   "_8sv", "_8zv", "_16sv", "_16zv", "_og", "_zg"};
//	const std::vector<QString> markers{"_brush", "_cry", "_fire", "_flower",
//									   "_isopropanol", "_needles", "_vanilla", "_wc"};

	// 26-feb-18, tbi
	const std::vector<QString> markers{"_bd", "_bw", "_cr", "_kh", "_na", "_no",
									   "_ph", "_rv", "_sc", "_sm", "_og", "_zg"};
	const int numChan = 19;
//	const int numChan = 31; /// MRI and other

	if(outPath == QString())
	{
		outPath = inPath + "_out";
	}

	if(!QDir(outPath).exists())
	{
		QDir().mkpath(outPath);
	}


	/// count
	auto guyList = QDir(inPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString guy : guyList)
	{
		const QString guyPath = inPath + "/" + guy;

		if(0) /// repair fileNames
		{
			repair::deleteSpacesFolders(guyPath);
			repair::toLatinDir(guyPath);
			repair::toLowerDir(guyPath);
		}

		QStringList edfs = QDir(guyPath).entryList(def::edfFilters);
		if(edfs.isEmpty())
		{
			std::cout << "ProcessByFolders: guyPath is empty " << guy << std::endl;
		}
		QString ExpName = edfs[0];
		ExpName = ExpName.left(ExpName.lastIndexOf('_'));


		/// physMinMax & holes
		if(01)
		{
			repair::physMinMaxDir(guyPath);
			repair::holesDir(guyPath,
							 numChan,
							 guyPath);	/// rewrite after repair
			continue;
		}

		/// rereference
		if(01)
		{
			autos::rereferenceFolder(guyPath, "Ar");
		}

		/// filter?
		if(01)
		{
			/// already done ?
			autos::refilterFolder(guyPath,
								  1.6,
								  30.);
			continue;
		}

		/// cut?
		if(0)
		{
			autos::cutFilesInFolder(guyPath,
									8,
									inPath + "_cut/");
		}


		outPath = guyPath + "/out";

		/// process?
		if(01)
		{
			/// clear outFolder
			myLib::cleanDir(guyPath + "/out", "txt", true);
			autos::GalyaProcessing(guyPath, numChan, outPath);
		}

		/// make one line file for each stimulus
		if(01)
		{
			for(QString mark : markers)
			{
				QStringList fileNamesToArrange;

				/// remake with DEFS.autosMask
					for(featuresMask func : {
						featuresMask::alpha,
						featuresMask::spectre,
						featuresMask::Hilbert,
						featuresMask::fracDim,
						featuresMask::Hjorth,
						featuresMask::logFFT
			})
					{
						const QString fileName = ExpName + mark
												 + "_" + autos::getFeatureString(func) + ".txt";
						fileNamesToArrange.push_back(fileName);

						if(01) /// create files if they are absent
						{
							if(!QFile::exists(outPath + "/" + fileName))
							{
								std::ofstream outStr;
								outStr.open((outPath + "/" + fileName).toStdString());
								for(int i = 0; i < autos::getFileLength(func); ++i)
								{
									outStr << " " << '\t';
//									outStr << 0 << '\t';
								}
								outStr.close();
							}
						}
					}
					autos::ArrangeFilesToLine(outPath,
											  fileNamesToArrange,
											  outPath + "/" + ExpName + mark + ".txt");
			}
		}

		/// make whole line from all stimuli
		if(1)
		{
			QStringList fileNamesToArrange;
			for(QString mark : markers)
			{
				fileNamesToArrange.push_back(ExpName + mark + ".txt");
			}
			autos::ArrangeFilesToLine(outPath,
									  fileNamesToArrange,
									  outPath + "/" + ExpName + ".txt");
		}

		/// copy files into _out
		if(1)
		{
			QFile::copy(outPath + "/" + ExpName + ".txt",
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


} // namespace autos
