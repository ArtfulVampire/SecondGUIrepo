#include <other/autos.h>

#include <myLib/clustering.h>
#include <myLib/iitp.h>
#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>
#include <myLib/output.h>

#include <widgets/net.h>

using namespace myOut;

namespace autos
{

QString getFeatureString(int in)
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


	int Mask{};
	switch(def::currAutosUser)
	{
		case def::autosUser::Galya:
		{
			Mask = autos::featuresMask::alpha |
				   autos::featuresMask::spectre |
				   autos::featuresMask::Hilbert |
				   autos::featuresMask::fracDim |
				   autos::featuresMask::Hjorth;

//			Mask = autos::featuresMask::Hilbert;
//			Mask = autos::featuresMask::fracDim;

			break;
		}
		case def::autosUser::Xenia:
		{
			Mask = autos::featuresMask::alpha |
				   autos::featuresMask::spectre |
				   autos::featuresMask::Hilbert |
				   autos::featuresMask::fracDim |
				   autos::featuresMask::wavelet;
			break;
		}
		default:
		{ /* do nothing */ }
	}

	if(Mask & autos::featuresMask::wavelet)
	{
#if WAVELET_MATLAB
		if(!wvlt::isInit)
		{
			wvlt::initMtlb();
		}
#endif
	}

//	omp_set_dynamic(0);
//	omp_set_num_threads(3);
//#pragma omp parallel
//#pragma omp for nowait
	for(int i = 0; i < filesVec.size(); ++i)
	{
		QString helpString = dir.absolutePath() + "/" + filesVec[i];

		edfFile initEdf;
		initEdf.readEdfFile(helpString, true);

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
		switch(def::currAutosUser)
		{
			case def::autosUser::Galya:
			{
				break;
			}
			case def::autosUser::Xenia:
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

			std::get<2>(FEATURES[num])(inData,
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
		std::make_pair(8, 13)};
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

	switch(def::currAutosUser)
	{
		case def::autosUser::Galya:
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
		case def::autosUser::Xenia:
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
	//			smLib::max,
	//			smLib::min,
				smLib::mean,
				smLib::median,
				smLib::sigma
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

/// further generalizations
void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag)
{
	def::ntFlag = false;

	for(QString guy : guyList)
	{
		if(cutOnlyFlag)
		{
			autos::GalyaCut(def::mriFolder + "/" + guy, 2);
			continue;
		}

		autos::GalyaFull(def::mriFolder +
						 "/" + guy +
						 "/" + guy + "_winds_cleaned");

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
	def::ntFlag = false;

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
		autos::makeTableFromRows(outPath,
								 OUT + "/" + guy + ".txt");
	}
	autos::makeTableFromRows(OUT,
							 OUT + "/all.txt",
							 true);
}


/// old
void Xenia_TBI(const QString & tbi_path)
{
	/// TBI Xenia cut, process, tables
	def::ntFlag = false;

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

		autos::GalyaCut(workPath, 8);
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
				autos::GalyaCut(workPath + "/" + guy,
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
				for(int type : {
					autos::featuresMask::alpha,
					autos::featuresMask::fracDim,
					autos::featuresMask::Hilbert,
					autos::featuresMask::spectre,
					autos::featuresMask::wavelet})
				{
					QString typ = "_" + autos::getFeatureString(type);
					fileNames << ExpName + marker + typ + ".txt";
				}
				autos::XeniaArrangeToLine(workPath + "_tmp",
										  fileNames,
										  workPath + "_tmp2" + slash
										  + ExpName + marker + ".txt"); /// guy <-> ExpName
			}

			fileNames.clear();
			for(QString marker : markers)
			{
				fileNames <<  ExpName + marker + ".txt"; /// guy <-> ExpName
			}
			autos::XeniaArrangeToLine(workPath + "_tmp2",
									  fileNames,
									  workPath + "_OUT" + slash
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
	def::ntFlag = false;
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
				repair::holesDir(guyPath, guyPath);	/// rewrite after repair
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
				autos::GalyaCut(guyPath,
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
					for(int func : {
						autos::featuresMask::spectre,
						autos::featuresMask::fracDim,
						autos::featuresMask::Hilbert,
						autos::featuresMask::wavelet,
						autos::featuresMask::alpha})
					{
						fileNamesToArrange.push_back(ExpName + mark
													 + "_" + autos::getFeatureString(func) + ".txt");
					}
					std::cout << fileNamesToArrange << std::endl << std::endl;
					autos::XeniaArrangeToLine(outPath,
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
				autos::XeniaArrangeToLine(outPath,
										  fileNamesToArrange,
										  outPath + "/" + ExpName + ".txt");

				QFile::copy(outPath + "/" + ExpName + ".txt",
							finalPath + "_out/" + ExpName + ".txt");
			}
//			return; /// only first guy from first subdir
		}
	}
	/// make tables whole and people list
	autos::makeTableFromRows(finalPath + "_out",
							 finalPath + "_out/all.txt",
							 true);

	std::cout << "Xenia_TBI_final: time elapsed = "
			  << myTime.elapsed() / 1000. << " sec" << std::endl;
}






void repairMarkersInNewFB(QString edfPath, int numSession)
{
	/// repair markers in my files

	edfFile fil;
	fil.readEdfFile(edfPath);

	const std::valarray<double> & mrk = fil.getData()[fil.getMarkChan()];
	std::vector<int> marks;

	bool startFlag = true;
	for(int i = 0; i < mrk.size(); ++i)
	{
		if(mrk[i] == 239 && startFlag == true)
		{
			marks.push_back(i);
			startFlag = false;
		}
		else if(mrk[i] == 254)
		{
			startFlag = true;
		}

	}

	for(int i = 0; i < mrk.size(); ++i)
	{
		if(mrk[i] == 241. || mrk[i] == 247.)
		{
			fil.setData(fil.getMarkChan(), i, 0.);
		}
	}


	std::ifstream inStr;
	inStr.open(("/media/Files/Data/FeedbackNew/Tables/types"
				+ nm(numSession + 1) + ".txt").toStdString());
	std::vector<int> marksList;
	char c;
	while(!inStr.eof())
	{
		inStr >> c;
		if(!inStr.eof())
		{
			if(c =='s')
			{
				marksList.push_back(241);
			}
			else if(c == 'v')
			{
				marksList.push_back(247);
			}
		}
	}

	if(marksList.size() != marks.size())
	{
		std::cout << myLib::getExpNameLib(edfPath) << " ";
		std::cout << "inequal sizes: ";
		std::cout << marksList.size() << '\t';
		std::cout << marks.size() << std::endl;
		return;
	}

	for(int i = 0; i < 80; ++i)
	{
		fil.setData(fil.getMarkChan(), marks[i] - 1, marksList[i]);
	}
	edfPath.replace(".edf", "_good.edf");
	fil.writeEdfFile(edfPath);

}


int numMarkers(const QString & edfPath, const std::vector<int> & markers)
{
	edfFile fil;
	fil.readEdfFile(edfPath);
	int res = 0;
	for(double i : fil.getMarkArr())
	{
		if(std::find(std::begin(markers),
					 std::end(markers),
					 std::round(i)) != std::end(markers))
		{
			++res;
		}
	}
	return res;

}

/// not in use
void mixNumbersCF(const QString & dirPath)
{
	auto fileList = QDir(dirPath).entryList({"*.jpg"}, QDir::Files, QDir::Name);
	std::vector<int> newNums(fileList.size());
	std::iota(std::begin(newNums), std::end(newNums), 0);
	std::shuffle(std::begin(newNums), std::end(newNums),
				 std::default_random_engine(
					 std::chrono::system_clock::now().time_since_epoch().count()));

	int numCounter = 0;
	for(QString oldName : fileList)
	{
		QString newName = oldName;
		QStringList parts = newName.split(QRegExp(R"([_\.])"), QString::SkipEmptyParts);
		newName.clear();
		for(QString & part : parts)
		{
			bool ok = false;
			part.toInt(&ok);
			/// if part is a number
			if(ok)
			{
				part = rn(newNums[numCounter++], 3);
//				std::cout << a << "\t" << part << std::endl;
			}
			newName += part + "_";
		}
		QFile::rename(dirPath + "/" + oldName,
					  dirPath + "/" + newName);
	}


	for(QString oldName : QDir(dirPath).entryList({"*_jpg_*"}, QDir::Files, QDir::Name))
	{
		QString newName = oldName;
		newName.replace("_jpg_", ".jpg");

		QFile::rename(dirPath + "/" + oldName,
					  dirPath + "/" + newName);
	}
}

/// not in use
void makeRightNumbersCF(const QString & dirPath, int startNum)
{
	for(QString oldName : QDir(dirPath).entryList({"*jpg"}, QDir::Files, QDir::Name))
	{
		QString newName = oldName;
		QStringList parts = newName.split(QRegExp(R"([_\.])"), QString::SkipEmptyParts);
		newName.clear();
		for(QString & part : parts)
		{
			bool ok = false;
			int a = part.toInt(&ok);
			if(ok)
			{
				part = rn(startNum++, 3);
				std::cout << a << "\t" << part << std::endl;
			}
			newName += part + "_";
		}

//		std::cout << dirPath + "/" + oldName << std::endl
//				  << dirPath + "/" + newName << std::endl << std::endl;

		QFile::rename(dirPath + "/" + oldName,
					  dirPath + "/" + newName);
	}


	for(QString oldName : QDir(dirPath).entryList({"*_jpg_*"}, QDir::Files, QDir::Name))
	{
		std::cout << oldName << std::endl;

		QString newName = oldName;
		newName.replace("_jpg_", ".jpg");

//		std::cout << dirPath + "/" + oldName << std::endl
//				  << dirPath + "/" + newName << std::endl << std::endl;

		QFile::rename(dirPath + "/" + oldName,
					  dirPath + "/" + newName);
	}
}

void makeRightNumbers(const QString & dirPath,
					  int length)
{
	QDir deer(dirPath);
	QStringList leest = deer.entryList(QDir::Files);
	for(const QString & oldName : leest)
	{
		QString newName = oldName;
		QStringList parts = newName.split('_');
		newName.clear();
		for(QString & part : parts)
		{
			bool ok = false;
			int a = part.toInt(&ok);
			if(ok)
			{
				part = rn(a, length);
			}
			newName += part + "_";
		}
		newName.remove(newName.length() - 1, 1); // remove last

		QFile::rename(deer.absolutePath() + "/" + oldName,
					  deer.absolutePath() + "/" + newName);
	}
}


void makeTableFromRows(const QString & inPath,
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
		fileNames.open((inPath + "/people.txt").toStdString());
	}

	for(const QString & fileName : deer.entryList({"*" + auxFilter +".txt"},
												  QDir::Files,
												  QDir::Time
												  | QDir::Reversed
												  ))
	{
		if(fileName.contains(tableName)
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


matrix makeTestData(const QString & outPath)
{
	matrix testSignals(8, 250 * 60 * 3); /// 3 min

	// signals
	double helpDouble;
	double x, y;

	std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());

	std::uniform_real_distribution<double> distr(0, 1);

	for(uint i = 0; i < testSignals.cols(); ++i)
	{
		helpDouble = 2. * pi * i / def::freq * 12.5; // 12.5 Hz ~ 20 bins per period
		testSignals[0][i] = sin(helpDouble); // sine

		testSignals[1][i] = (i + 2) % 29;      // saw

		testSignals[2][i] = (i % 26 >= 13); // rectangle

		x = distr(gen);
		y = distr(gen);
		testSignals[3][i] = sqrt(-2. * log(x)) * cos(2. * pi * y); // gauss?

		testSignals[4][i] = std::abs(i % 22 - 11); // triangle

		testSignals[5][i] = rand() % 27; // noise

		x = distr(gen);
		y = distr(gen);
		testSignals[6][i] = sqrt(-2. * log(x)) * cos(2. * pi * y); // gauss?

		testSignals[7][i] = pow(rand() % 13, 3); // non-white noise
	}


	double sum1, sum2;
	// normalize by dispersion = coeff
	double coeff = 10.;
	for(uint i = 0; i < testSignals.rows(); ++i)
	{
		sum1 = smLib::mean(testSignals[i]);
		sum2 = smLib::variance(testSignals[i]);

		testSignals[i] -= sum1;
		testSignals[i] /= sqrt(sum2);
		testSignals[i] *= coeff;

	}
	matrix pewM(testSignals.rows(),
				testSignals.rows());
	pewM.random(-1., 1.);


	testSignals = pewM * testSignals;

	myLib::writePlainData(outPath, testSignals);
	myLib::writePlainData(outPath + "_", testSignals.resizeCols(2000)); // edit out time
	return pewM;
}


void XeniaArrangeToLine(const QString & dirPath,
						const QStringList & fileNames,
						const QString & outFilePath)
{
	QDir().mkpath(myLib::getDirPathLib(outFilePath));

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

void GalyaCut(const QString & path,
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
	omp_set_dynamic(0);
	omp_set_num_threads(3);
#pragma omp parallel
#pragma omp for nowait
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

void GalyaToFolders(const QString & inPath)
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

void Galya_tactile(const QString & inPath,
				   QString outPath)
{
	QTime myTime;
	myTime.start();

	def::ntFlag = false;
//	const std::vector<QString> markers{"_buk", "_kis", "_rol", "_sch", "_og", "_zg"};
	const std::vector<QString> markers{"_2sv", "_2zv", "_4sv", "_4zv",
									   "_8sv", "_8zv", "_16sv", "_16zv", "_og", "_zg"};

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
			std::cout << "Galya_tactile: inPath is empty " << inPath << std::endl;
		}
		QString ExpName = edfs[0];
		ExpName = ExpName.left(ExpName.lastIndexOf('_'));


		/// physMinMax & holes
		if(0)
		{
			repair::physMinMaxDir(guyPath);
			repair::holesDir(guyPath, guyPath);	/// rewrite after repair
		}

		/// rereference
		if(0)
		{
			//		autos::rereferenceFolder(guyPath, "Ar");
		}

		/// filter?
		if(0)
		{
			/// already done ?
			autos::refilterFolder(guyPath,
								  1.6,
								  30.);
		}

		/// cut?
		if(0)
		{
			autos::GalyaCut(guyPath,
							8,
							inPath + "_cut/");
		}


		outPath = guyPath + "/out";

		/// process?
		if(0)
		{
			/// clear outFolder
			myLib::cleanDir(guyPath + "/out", "txt", true);

			autos::GalyaProcessing(guyPath, 19, outPath);
		}

		/// make one line file for each stimulus
		if(1)
		{
			for(QString mark : markers)
			{
				QStringList fileNamesToArrange;
				for(int func : {
					autos::featuresMask::alpha,
					autos::featuresMask::spectre,
					autos::featuresMask::Hilbert,
					autos::featuresMask::fracDim,
					autos::featuresMask::Hjorth})
				{
					fileNamesToArrange.push_back(ExpName + mark
												 + "_" + autos::getFeatureString(func) + ".txt");

					if(!QFile::exists(outPath + "/" + ExpName + mark
									  + "_" + autos::getFeatureString(func) + ".txt"))
					{
						std::ofstream outStr;
						outStr.open((outPath + "/" + ExpName + mark
									 + "_" + autos::getFeatureString(func) + ".txt").toStdString());
						for(int i = 0; i < autos::getFileLength(func); ++i)
						{
							outStr << 0 << '\t';
						}
						outStr.close();
					}
				}
//				std::cout << fileNamesToArrange << std::endl << std::endl;
				autos::XeniaArrangeToLine(outPath,
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
			autos::XeniaArrangeToLine(outPath,
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
	autos::makeTableFromRows(inPath + "_out",
							 inPath + "_out/all.txt",
							 true);

	std::cout << "Xenia_TBI_final: time elapsed = "
			  << myTime.elapsed() / 1000. << " sec" << std::endl;
}


/// to deprecate
void GalyaFull(const QString & inDirPath,
			   QString outDirPath,
			   QString outFileNames,
			   int numChan,
			   int freq,
			   int rightNum)
{
	QDir tmp(inDirPath);
	if(!tmp.exists())
	{
		std::cout << "GalyaFull: path = " << inDirPath << " is wrong" << std::endl;
		return;
	}

	if(outFileNames.isEmpty())
	{
		const QString firstName = tmp.entryList(def::edfFilters)[0];
		outFileNames = firstName.left(firstName.indexOf('_'));
	}

	if(outDirPath.isEmpty())
	{
		tmp.cdUp();
		tmp.cdUp();
		tmp.mkdir("OUT");
		tmp.cd("OUT");
		tmp.mkdir(outFileNames);
		tmp.cd(outFileNames);
		outDirPath = tmp.absolutePath();
	}
	tmp.mkpath(outDirPath);


	const QString outPath = inDirPath + "_out";
//	const QString outPath = inDirPath + "/" + myLib::getFileName(inDirPath) + "_out";
	tmp.mkpath(outPath);

	const QString waveletPath = inDirPath + "_wavelet";
//	const QString waveletPath = inDirPath + "/" + myLib::getFileName(inDirPath) + "_wavelet";
	tmp.mkpath(waveletPath);

#if 01
	autos::GalyaProcessing(inDirPath,
						   numChan,
						   outPath);
	autos::makeRightNumbers(outPath,
							rightNum);

	/// Galya
	for(int typ : {
		featuresMask::alpha,
		featuresMask::spectre,
		featuresMask::Hilbert,
		featuresMask::fracDim})
	{
		QString type = autos::getFeatureString(typ);
		autos::makeTableFromRows(outPath,
								 outDirPath + "/" + outFileNames
								 + "_" + type + ".txt",
								 false,
								 type);
	}
#endif

	return;
//	exit(0);

	/// rename the folder in OUT to guy
	autos::makeRightNumbers(waveletPath, rightNum);
	autos::makeTableFromRows(waveletPath,
							 outDirPath + "/" + outFileNames + "_wavelet.txt");
}

void feedbackFinalTimes(const QString & edfsPath,
						const QString & expName)
{

	std::ofstream outStr;
	outStr.open((edfsPath + "/" + expName + "_" + "timesResults" + ".txt").toStdString());

	for(int numSess : {1, 2, 3})
	{
		auto filePath = [edfsPath, expName](int i) -> QString
		{
			return edfsPath + "/" + expName + "_" + nm(i) + ".edf";
		};

		edfFile fil;
		fil.readEdfFile(filePath(numSess));
		const std::vector<std::pair<int, int>> & markers = fil.getMarkers();


		std::vector<int> nums = {241, 247};
		int sta = 0;
//		int fin = 0;
		int num = -1;

		/// [time, corr, type]
		std::vector<std::tuple<double, int, int>> values; values.reserve(80);

		/// read answers file
		std::vector<int> corrs; corrs.reserve(80);
		std::ifstream answers;
		answers.open((edfsPath + "/" + expName + "_ans" + nm(numSess) + ".txt").toStdString());
		char ans;
		while(answers >> ans)
		{
			if(ans == '\n' || ans == '\r') answers >> ans;
			corrs.push_back(QString(ans).toInt());
		}
		answers.close();


//		std::cout << corrs.size() << std::endl;

		int count = 0;
		for(const std::pair<int, int> & mrk : markers)
		{
			int mark = mrk.second;

			if(myLib::contains(nums, mark))
			{
				sta = mrk.first;
				num = (mark == 241) ? 0 : 1;
			}
			else if(mark == 254)
			{
				values.push_back(std::make_tuple(mrk.first - sta,
												 corrs[count++],
												 num)
								 );
			}
		}

//		std::cout << values.size() << std::endl;
//		for(auto in : values)
//		{
//			std::cout << std::get<0>(in) << "\t" // time
//					  << std::get<1>(in) << "\t" // corr
//					  << std::get<2>(in) << "\t" // type
//					  << std::endl;
//		}
//		return;


		/// write in a row:
		/// (corr, incorr, answrd) x (mean, median, sgm) + - 0

		for(int typ : {0, 1}) /// 241, 247
		{

			outStr << std::fixed;
			outStr.precision(1);

			for(std::vector<int> anss :
				std::vector<std::vector<int>>{{1}, {2}, {1,2}}) /// corr, incorr, answrd
			{
				std::vector<double> vals{};

				std::for_each(std::begin(values),
							  std::end(values),
							  [&vals, typ, anss](const auto & in)
				{
					if(std::get<2>(in) == typ && myLib::contains(anss, std::get<1>(in)))
					{
						vals.push_back(std::get<0>(in));
					}
				});
				auto vl = smLib::vecToValar(vals);
				vl /= fil.getFreq();
				outStr << smLib::mean(vl) << "\t"
					   << smLib::median(vl) << "\t"
					   << smLib::sigma(vl) << "\t";
			}
			outStr << std::defaultfloat;

			for(int ans : {1, 2, 0})
			{
				outStr << std::count_if(std::begin(values),
										std::end(values),
										[typ, ans](const auto & in)
				{
					return (std::get<2>(in) == typ) && (std::get<1>(in) == ans);
				})
					   << "\t";
			}
		}
		outStr << "\r\n";
	}
	outStr.close();
}

void avTimesNew(const QString & edfPath,
				const QString & guy,
				int numSession)

{
	int ans;
	double tim;

	std::vector<int> nums = {241, 247};

	for(int i = 0; i < 2; ++i)
	{
		QString timesPath = myLib::getDirPathLib(edfPath) + "/" + guy + "_times_"
							+ nm(numSession) + "_"
							+ nm(nums[i]) + ".txt";

		std::ifstream inStr;
		inStr.open(timesPath.toStdString());

		std::vector<double> times[5];
		while(!inStr.eof())
		{
			inStr >> ans;
			inStr >> tim;
			if(!inStr.eof())
			{
				times[ans].push_back(tim);
				times[3].push_back(tim);
				if(ans != 0) times[4].push_back(tim);
			}
		}
		inStr.close();

		QString fileName = timesPath.replace("_times_", "_avTimes_");
		QFile::remove(fileName);

		std::ofstream outStr;
		outStr.open(fileName.toStdString());

		outStr << "\t" << "\t" << "mean" << "\t" << "sigma" << "\r\n";
		for(std::pair<int, QString> a : {std::pair<int, QString>(1, "corr"),
			std::pair<int, QString>(2, "incorr"),
			std::pair<int, QString>(4, "answrd"),
			std::pair<int, QString>(3, "allll")})
		{
			auto valar = smLib::vecToValar(times[a.first]);
			outStr << a.second << "\t"
				   << smLib::doubleRound(smLib::mean(valar), 2) << "\t"
				   << smLib::doubleRound(smLib::sigma(valar), 2) << "\r\n";
		}
		outStr.close();
	}
}

void createAnsFiles(const QString & dirPath, QString guy)
{
	for(int i : {1, 2, 3})
	{
		QFile fil(dirPath + "/" + guy + "_ans" + nm(i) + ".txt");
		fil.open(QIODevice::WriteOnly);
		fil.close();
	}
}

void timesNew(const QString & edfPath,
			  const QString & guy,
			  int numSession)
{
	if(!QFile::exists(edfPath)) return;

	edfFile fil;
	fil.readEdfFile(edfPath);
	const std::vector<std::pair<int, int>> & markers = fil.getMarkers();

	const QString outPath = myLib::getDirPathLib(edfPath) + "/times/";
	QDir().mkpath(outPath);
	std::ofstream out;

	std::ifstream answers;
	answers.open((myLib::getDirPathLib(edfPath)
				  + "/" + guy + "_ans"
				  + nm(numSession) + ".txt").toStdString());


	int sta = 0;
	int fin = 0;
	char ans;

	std::vector<std::vector<int>> corr(2, std::vector<int>(3)); // [241|247][0, +, -]
	std::vector<int> nums = {241, 247};
	int num = -1;

	for(int i : nums)
	{
		QFile::remove(outPath
					  + guy + "_correctness_"
					  + nm(numSession) + "_"
					  + nm(nums[i]) + ".txt");
		QFile::remove(outPath
					  + guy + "_times_"
					  + nm(numSession) + "_"
					  + nm(nums[i]) + ".txt");
	}

	for(const std::pair<int, int> & mrk : markers)
	{
		int mark = mrk.second;

		if(mark == nums[0] || mark == nums[1])
		{
			sta = mrk.first;

			if(mark == 241) num = 0;
			else num = 1;
		}
		else if(mark == 254)
		{
			fin = mrk.first;

			answers >> ans;
			if(ans == '\n' || ans == '\r') answers >> ans;

			++corr[num][QString(ans).toInt()];

			out.open((outPath
					  + guy + "_times_"
					  + nm(numSession) + "_"
					  + nm(nums[num]) + ".txt").toStdString(), std::ios_base::app);
			out
					<< ans << "\t"
					<< (fin - sta) / fil.getFreq() << "\r\n";
			out.close();
		}
	}
	for(int i = 0; i < nums.size(); ++i)
	{
		out.open((outPath + guy + "_correctness_"
				  + nm(numSession) + "_"
				  + nm(nums[i]) + ".txt").toStdString(), std::ios_base::app);
		out << "+" << "\t"
			<< "-" << "\t"
			<< "0" << std::endl;
		out << corr[i][1] << '\t' << corr[i][2] << '\t' << corr[i][0] << "\r\n";
		out.close();
	}
	answers.close();
}

void avTime(const QString & realsDir)
{
	int shortThreshold = 3 * def::freq;
	int longThreshold = 31.5 * def::freq;
	int shortReals = 0;
	int longReals = 0;

	std::valarray<double> means{};
	means.resize(4); /// 4 types of mean - with or w/o shorts and longs

	int num = 0;

	QString helpString;

	QDir localDir;
	localDir.cd(realsDir);

	for(const QString & tmp : {"241", "247"})
	{
		means = 0.;
		shortReals = 0;
		longReals = 0;

		QStringList lst = QDir(realsDir).entryList({"*_" + tmp + "*"}, QDir::Files);
		for(const QString & fileName : lst)
		{
			helpString = realsDir + "/" + fileName;

			// read numOfSlices
			std::ifstream inStr;
			inStr.open(helpString.toStdString());
			inStr.ignore(64, ' ');
			inStr >> num;
			inStr.close();

			means += num;

			if(num < shortThreshold)
			{
				means[1] -= num;
				means[3] -= num;
				++shortReals;
			}
			else if(num > longThreshold)
			{
				means[2] -= num;
				means[3] -= num;
				++longReals;
			}
		}
		means[0] /= lst.length();
		means[1] /= (lst.length() - shortReals);
		means[2] /= (lst.length() - longReals);
		means[3] /= (lst.length() - shortReals - longReals);
		means /= def::freq;

		helpString = realsDir + "/../" + "avTime.txt";

		std::ofstream res;
		res.open(helpString.toStdString(), std::ios_base::app);
		res << "Reals type\t"	<< tmp << ":\r\n";
		res << "shortReals\t"	<< shortReals << "\r\n";
		res << "longReals\t"	<< longReals << "\r\n";
		res << "All together\t"	<< means[0] << "\r\n";
		res << "w/o shorts\t"	<< means[1] << "\r\n";
		res << "w/o longs\t"	<< means[2] << "\r\n";
		res << "w/o both\t"		<< means[3] << "\r\n\r\n";
		res.close();
	}
	std::cout << "avTime: finished" << std::endl;
}

void successiveNetPrecleanWinds(const QString & windsPath)
{
	Net * ann = new Net();
	ann->loadData(windsPath, {"*.psd"});
	ann->setClassifier(ModelType::ANN);
	ann->successivePreclean(windsPath, {});
	delete ann;
}

void successivePrecleanWinds(const QString & windsPath)
{
	QStringList leest;
	myLib::makeFullFileList(windsPath, leest, { def::plainDataExtension });

	/// moved to sliceWinds()
//	std::cout << "clean first 2 winds" << std::endl;
//	for(const QString & str : leest)
//	{
//		if(str.contains(QRegExp("\\.0[0-1]\\."))) /// change to 0-x for x first winds to delete
//		{
//			QFile::remove(windsPath + "/" + str);
//		}
//	}

	/// moved to sliceWinds()
//	std::cout << "clean by learnSetStay * 2" << std::endl;
//	std::vector<QStringList> leest2;
//	myLib::makeFileLists(windsPath, leest2);

//	for(int j = 0; j < leest2.size(); ++j)
//	{
//		auto it = std::begin(leest2[j]);

//		for(int i = 0;
//			i < leest2[j].size() - suc::learnSetStay * 2.5; /// magic const generality
//			++i, ++it)
//		{
//			QFile::remove(windsPath + "/" + (*it));
//		}
//	}




}



void clustering()
{
	srand(time(NULL));

	int numRow = 300;
	int numCol = 18;
	matrix cData(numRow, numCol);

	std::ifstream inStr;
	QString helpString = "/media/Files/Data/Mati/clust.txt";
	inStr.open(helpString.toStdString());
	for(int i = 0; i < numRow; ++i)
	{
		for(int j = 0; j < numCol; ++j)
		{
			inStr >> cData[i][j];
		}
	}


	std::vector<std::vector<double>> distOld;
	distOld.resize(numRow);
	for(int i = 0; i < numRow; ++i)
	{
		distOld[i].resize(numRow);
	}

	std::vector<int> types;
	types.resize(numRow);

	std::vector<std::vector<double>> dists; // distance, i, j,

	std::vector<double> temp(4);

	std::vector<bool> boundDots;
	std::vector<bool> isolDots;

	boundDots.resize(numRow);
	std::fill(boundDots.begin(), boundDots.end(), false);
	isolDots.resize(numRow);
	std::fill(isolDots.begin(), isolDots.end(), true);

	for(int i = 0; i < numRow; ++i)
	{
		types[i] = i % 3;
		for(int j = i+1; j < numRow; ++j)
		{
			temp[0] = smLib::distance(cData[i], cData[j]);
			temp[1] = i;
			temp[2] = j;
			temp[3] = 0;

			dists.push_back(temp);

			distOld[i][j] = temp[0];
			distOld[j][i] = temp[0];
		}
	}

	clust::sammonProj(distOld, types,
					  "/media/Files/Data/Mati/sammon.jpg");
#if 0
	// test

	const int N = 15;
	const int dim = 2;

	distOld.clear();
	distOld.resize(N);
	for(int i = 0; i < N; ++i)
	{
		distOld[i].resize(N);
	}

	vector< std::vector<double> > dots;
	dots.resize(N);
	vector<double> ass;
	ass.resize(dim);

	types.clear();
	types.resize(N);

	srand (756);
	for(int i = 0; i < N; ++i)
	{
		types[i] = i % 3;
		for(int j = 0; j < dim; ++j)
		{
			ass[j] =  -5. + 10.*( (rand())%300 ) / 150.;
		}
		dots[i] = ass;
	}

	for(int i = 0; i < N; ++i)
	{
		distOld[i][i] = 0.;
		for(int j = i+1; j < N; ++j)
		{
			distOld[i][j] = distance(dots[i], dots[j], dim);
			distOld[j][i] = distOld[i][j];
		}
	}

//    std::cout << distOld << std::endl;

	sammonProj(distOld, types,
			   "/media/Files/Data/Mati/sammon.jpg");



exit(1);
return;

#endif


#if 0
	// smallest tree
	std::sort(dists.begin(), dists.end(), mySort);
	// make first bound

	boundDots[ dists[0][1] ] = true;
	isolDots[ dists[0][1] ] = false;

	boundDots[ dists[0][2] ] = true;
	isolDots[ dists[0][2] ] = false;

	dists[0][3] = 2;
	newDists.push_back(dists[0]);



	std::vector<std::vector<double> >::iterator itt;
	while (contains(isolDots.begin(), isolDots.end(), true))
	{
		// adjust dists[i][3]
		for(std::vector<std::vector<double> >::iterator iit = dists.begin();
			iit < dists.end();
			++iit)
		{
			if(boundDots[ (*iit)[1] ])
			{
				(*iit)[3] += 1;
			}
			if(boundDots[ (*iit)[2] ])
			{
				(*iit)[3] += 1;
			}
		}

		// set new bound ()
		for(itt = dists.begin();
			itt < dists.end();
			++itt)
		{
			if((*itt)[3] == 1) break;
		}

		boundDots[ (*itt)[1] ] = true;
		isolDots[ (*itt)[1] ] = false;

		boundDots[ (*itt)[2] ] = true;
		isolDots[ (*itt)[2] ] = false;

		(*itt)[3] = 2;
		newDists.push_back(*itt);

//        for(int j = 0; j < 3; ++j)
//        {
//            std::cout << (*itt)[j] << '\t';
//        }
//        std::cout << std::endl;
	}
	std::sort(newDists.begin(), newDists.end(), mySort);
	vector<double> newD;
	for(int i = 0; i < newDists.size(); ++i)
	{
		newD.push_back(newDists[i][0]);
	}
//    std::cout << newD << std::endl;


//    helpString = "/media/Files/Data/Mati/clust.jpg";
//    kernelEst(newD.data(), newD.size(), helpString);
//    helpString = "/media/Files/Data/Mati/clustH.jpg";
//    histogram(newD.data(), newD.size(), 40, helpString);
#endif
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



} // namespace autos
