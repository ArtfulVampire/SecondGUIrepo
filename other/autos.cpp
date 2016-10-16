#include "autos.h"

using std::cout;
using std::endl;
using namespace myLib;

namespace autos
{

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
				part = myLib::rightNumber(a, length);
			}
			newName += part + "_";
		}
		newName.remove(newName.length() - 1, 1); // remove last

		QFile::rename(deer.absolutePath() + slash + oldName,
					  deer.absolutePath() + slash + newName);
	}
}


void makeTableFromRows(const QString & work,
					   QString tablePath,
					   const QString & auxFilter)
{
	QDir deer(work);

	if(tablePath.isEmpty())
	{
		deer.cdUp();
		tablePath = deer.absolutePath()
					+ slash + "table.txt";
		deer.cd(work);
	}
	const QString tableName = getFileName(tablePath);

	QFile outStr(tablePath);
	outStr.open(QIODevice::WriteOnly);

	for(const QString & fileName : deer.entryList({"*" + auxFilter +".txt"},
												  QDir::Files,
												  QDir::Name))
	{
		if(fileName.contains(tableName)) continue;

		QFile fil(deer.absolutePath() + slash + fileName);
		fil.open(QIODevice::ReadOnly);
		auto contents = fil.readAll();
		fil.close();
		outStr.write(contents);
		outStr.write("\r\n");
	}
	outStr.close();
}


matrix makeTestData(const QString & outPath)
{
	matrix testSignals(8, 250 * 60 * 3); /// 3 min

	//signals
	double helpDouble;
	double x, y;

	std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());

	std::uniform_real_distribution<double> distr(0, 1);

	for(uint i = 0; i < testSignals.cols(); ++i)
	{
		helpDouble = 2. * pi * i / def::freq * 12.5; // 12.5 Hz = 20 bins
		testSignals[0][i] = sin(helpDouble); // sine

		testSignals[1][i] = (i + 2) % 29;      // saw

		testSignals[2][i] = (i % 26 >= 13); // rectangle

		x = distr(gen);
		y = distr(gen);
		testSignals[3][i] = sqrt(-2. * log(x)) * cos(2. * pi * y); // gauss?

		testSignals[4][i] = fabs(i % 22 - 11); //triangle

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
		sum1 = smallLib::mean(testSignals[i]);
		sum2 = smallLib::variance(testSignals[i]);

		testSignals[i] -= sum1;
		testSignals[i] /= sqrt(sum2);
		testSignals[i] *= coeff;

	}
	matrix pewM(testSignals.rows(),
				testSignals.rows());
	pewM.random(-1., 1.);


	testSignals = pewM * testSignals;

	writePlainData(outPath,
				   testSignals);
	writePlainData(outPath + "_",
				   testSignals, 2000); // edit out time
	return pewM;
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

	const int & fr = initEdf.getFreq();
	const int numOfWindows = ceil(initEdf.getDataLen() / fr / wndLen);


	for(int i = 0; i < numOfWindows; ++i)
	{
		initEdf.saveSubsection(
					i * fr * wndLen,
					fmin((i + 1) * fr * wndLen, initEdf.getDataLen()),
					QString(outPath
					#if ADD_DIR
							+ slash + addDir
					#endif
							+ slash + initEdf.getExpName()
							+ "_wnd_" + myLib::rightNumber(
								i + 1,
								floor(log10(numOfWindows)) + 1)
							+ ".edf"));
	}
}

void GalyaCut(const QString & path,
			  const int wndLen,
			  QString outPath)
{

	const QString outDir = getFileName(path) + "_windows";
	const QString smallsDir = getFileName(path) + "_smalls";

	QDir tmpDir(path);
	tmpDir.mkdir(smallsDir);
	const QString smallsPath = tmpDir.absolutePath() + slash + smallsDir;

	if(outPath.isEmpty())
	{
		tmpDir.mkdir(outDir);
		outPath = tmpDir.absolutePath() + slash + outDir;
	}
	else
	{
		tmpDir.mkpath(outPath);
	}

	/// to change
	const QString logPath = def::GalyaFolder + slash + "log.txt";
	std::ofstream logStream(logPath.toStdString(), std::ios_base::app);

	const QStringList leest1 = tmpDir.entryList(def::edfFilters);
	const auto filesVec = leest1.toVector();

	/// ??????????????????????
	omp_set_dynamic(0);
	omp_set_num_threads(3);
#pragma omp parallel
#pragma omp for nowait schedule(dynamic,2)
	for(int i = 0; i < filesVec.size(); ++i)
	{
		cout << filesVec[i] << endl;
		QString helpString = tmpDir.absolutePath() + slash + filesVec[i];
		edfFile initEdf;
		initEdf.readEdfFile(helpString, true);

		/// some check for small
		if(initEdf.getNdr() * initEdf.getDdr() <= wndLen )
		{
			QFile::remove(smallsPath + slash + initEdf.getFileNam());
			QFile::copy(initEdf.getFilePath(),
						smallsPath + slash + initEdf.getFileNam());

			cout << "smallFile \t" << initEdf.getFileNam() << endl;
			logStream << initEdf.getFilePath() << "\t" << "too small" << "\n";
			continue;
		}

		cutOneFile(helpString,
				   wndLen,
				   outPath);
	}
	logStream.close();

}

/// local
void matToFile(const matrix & mat, std::ofstream & fil, double (*func)(const lineType&))
{
	for(int i = 0; i < mat.rows(); ++i)
	{
		fil << smallLib::doubleRound(func(mat[i]), 4) << "\t";
	}
}

void waveletOneFile(const matrix & inData,
					int numChan,
					double freq,
					const QString & outFile)
{

	std::ofstream outStr;
	outStr.open(outFile.toStdString());

	/// can put OMP here, but wait when writing to file
	for(int j = 0; j < numChan; ++j)
	{
		matrix m = wvlt::cwt(inData[j], freq);
		for(auto foo : {smallLib::max,
			smallLib::min,
			smallLib::mean,
			smallLib::median,
			smallLib::sigma})
		{
			matToFile(m, outStr, foo);
		}
	}
	outStr.close();
}

void GalyaWavelets(const QString & inPath,
				   int numChan,
				   double freq,
				   QString outPath)
{
	QDir tmpDir(inPath);

	const QStringList lst = tmpDir.entryList(def::edfFilters);
//	const QString exp = myLib::getExpNameLib(lst.first(), true);
//	std::cout << lst.length() << std::endl;

	if(outPath.isEmpty())
	{
		tmpDir.mkdir("wavelet");
		outPath = tmpDir.absolutePath() + myLib::slash + "wavelet";
	}
	else
	{
//		tmpDir.mkpath(outPath + exp);
		tmpDir.mkpath(outPath);
	}


	const auto filesVec = lst.toVector();

//#pragma omp parallel
//#pragma omp for nowait
	for(int i = 0; i < filesVec.size(); ++i)
	{
		cout << filesVec[i] << endl;
		QString helpString = tmpDir.absolutePath() + slash + filesVec[i];

		edfFile initEdf;
		initEdf.readEdfFile(helpString);



		helpString = outPath
//					 + myLib::slash + exp
					 + myLib::slash
					 + myLib::getFileName(filesVec[i], false)
					 + "_wavelet.txt";

		waveletOneFile(initEdf.getData(), numChan, freq, helpString);
	}
//	if(wvlt::isInit) wvlt::termMtlb();
}




void countSpectraFeatures(const QString & filePath,
						  const int numChan,
						  const QString & outPath)
{
	const double leftFreqLim = 2.;
	const double rightFreqLim = 20.;
	const double spectreStepFreq = 1.;

	const double alphaMaxLimLeft = 8.;
	const double alphaMaxLimRight = 13.;

	const QString ExpName = getFileName(filePath, false);
	const QString spectraFilePath = outPath + slash + ExpName + "_spectre.txt";
	const QString alphaFilePath = outPath + slash + ExpName + "_alpha.txt";

	// remove previous
	QFile::remove(spectraFilePath);
	QFile::remove(alphaFilePath);

	std::ofstream outSpectraStr;
	std::ofstream outAlphaStr;
	outSpectraStr.open(spectraFilePath.toStdString());
	outAlphaStr.open(alphaFilePath.toStdString());

	int helpInt;
	double helpDouble;
	vectType fullSpectre;
	lineType helpSpectre;

	edfFile initEdf;
	initEdf.setMatiFlag(false);
	initEdf.readEdfFile(filePath);


	const double fr = initEdf.getFreq();

	for(int i = 0; i < numChan; ++i)
	{
		/// norming is not necessary
		helpSpectre = spectreRtoR(initEdf.getData()[i]);
		helpSpectre = smoothSpectre(helpSpectre,
									ceil(10 * sqrt(initEdf.getDataLen() / 4096.)));

		// count individual alpha peak
		helpDouble = 0.;
		helpInt = 0;
		for(int k = fftLimit(alphaMaxLimLeft,
							 fr,
							 smallLib::fftL(initEdf.getDataLen()));
			k < fftLimit(alphaMaxLimRight,
						 fr,
						 smallLib::fftL(initEdf.getDataLen()));
			++k)
		{
			if(helpSpectre[k] > helpDouble)
			{
				helpDouble = helpSpectre[k];
				helpInt = k;
			}
		}
		// max alpha magnitude
		outAlphaStr << helpDouble << "\t";
		// max alpha.getFreq()
		outAlphaStr << helpInt * fr / smallLib::fftL(initEdf.getDataLen()) << "\t";

		// integrate spectre near the needed.getFreq()s
		fullSpectre.clear();
		for(double j = leftFreqLim;
			j <= rightFreqLim;
			j += spectreStepFreq)
		{
			helpDouble = 0.;
			helpInt = 0;
			for(int k = fftLimit(j - spectreStepFreq / 2.,
								 fr,
								 smallLib::fftL(initEdf.getDataLen()));
				k < fftLimit(j + spectreStepFreq / 2.,
							 fr,
							 smallLib::fftL(initEdf.getDataLen()));
				++k)
			{
				helpDouble += helpSpectre[k];
				++helpInt;
			}
			fullSpectre.push_back(helpDouble/helpInt);
		}

#if 0
		// normalize spectre for 1 integral
		// rewrite lib::normalize()
		helpDouble = 0.;
		for(auto it = fullSpectre.begin();
			it != fullSpectre.end();
			++it)
		{
			helpDouble += (*it);
		}
		helpDouble = 1. / helpDouble;
		for(auto it = fullSpectre.begin();
			it < fullSpectre.end();
			++it)
		{
			(*it) *= helpDouble * 20.;
		}
#endif

		for(auto it = std::begin(fullSpectre);
			it < std::end(fullSpectre);
			++it)
		{

			outSpectraStr << smallLib::doubleRound((*it), 4) << "\t";  // write
		}
	}
	outAlphaStr.close();
	outSpectraStr.close();
}

void countChaosFeatures(const QString & filePath,
						const int numChan,
						const QString & outPath)
{
	const double leftFreqLim = 2.;
	const double rightFreqLim = 20.;
	const double stepFreq = 2.;
	const double hilbertFreqLimit = 40.;

	const QString ExpName = getFileName(filePath, false);
	const QString d2dimFilePath = outPath + slash + ExpName + "_d2_dim.txt";
	const QString hilbertFilePath = outPath + slash + ExpName + "_med.getFreq().txt";

	// remove previous
	QFile::remove(d2dimFilePath);
	QFile::remove(hilbertFilePath);

	std::ofstream outDimStr;
	std::ofstream outHilbertStr;
	outDimStr.open(d2dimFilePath.toStdString());
	outHilbertStr.open(hilbertFilePath.toStdString());

	double helpDouble;
	double sumSpec = 0.;
	std::valarray<double> env;
	std::valarray<double> envSpec;

	edfFile initEdf;
	initEdf.setMatiFlag(false);
	initEdf.readEdfFile(filePath);

	const double fr = initEdf.getFreq();

	for(double freqCounter = leftFreqLim;
	freqCounter <= rightFreqLim;
	freqCounter += stepFreq)
	{
		/// remake further, non-filtered first
		if(freqCounter != rightFreqLim)
		{
			initEdf.refilter(freqCounter, freqCounter + stepFreq);
		}
		else
		{
			/// reread file
			initEdf.readEdfFile(initEdf.getFilePath());
			/// or refilter 0. 70.
		}

		// write d2 dimension

		for(int i = 0; i < numChan; ++i)
		{
			helpDouble = fractalDimension(initEdf.getData()[i]);
			outDimStr << smallLib::doubleRound(helpDouble, 4) << "\t";
		}

#if 0
		// write enthropy
		helpString = outPath
					 + slash + ExpName;
#if 0
		iffreqCounter != rightFreqLim)
		{
			helpString += "_" + QString::numberfreqCounter)
						  + "-" + QString::numberfreqCounter + stepFreq);
		}
#endif
		helpString += "_" + enthropyFileName;
		outStr.open(helpString.toStdString().c_str(), ios_base::app);
		for(int i = 0; i < numChan; ++i)
		{
			helpDouble = enthropy(initEdf.getData()[i].data(),
								  initEdf.getDataLen());
			outStr << doubleRound(helpDouble, 4) << "\t";
		}
		outStr.close();
#endif



		// write envelope median spectre
		for(int i = 0; i < numChan; ++i)
		{
			//                helpString.clear(); // no pictures
			env = myLib::hilbertPieces(initEdf.getData()[i],
//								initEdf.getDataLen(),
								fr,
								1., /// no difference - why?
								40. /// no difference - why?
								//                                    ,helpString
								);

			/// norming is not necessary here
			envSpec = spectreRtoR(env);
			envSpec[0] = 0.;
#if 0
			iffreqCounter <= rightFreqLim + stepFreq)
			{
				helpString = outPath
							 + slash + ExpName
							 + "_" + QString::numberfreqCounter)
							 + "_" + QString::number(numChan)
							 + "_fSpec.jpg";
			}
			else
			{
				helpString.clear();
			}
			helpString.clear(); // no picture of spectre

			drawOneArray(envSpec,
						 helpString);
#endif

			helpDouble = 0.;
			sumSpec = 0.;

			for(int j = 0;
				j < fftLimit(hilbertFreqLimit, fr, smallLib::fftL(initEdf.getDataLen()));
				++j)
			{
				helpDouble += envSpec[j] * j;
				sumSpec += envSpec[j];
			}
			helpDouble /= sumSpec;
			helpDouble /= fftLimit(1., fr, smallLib::fftL(initEdf.getDataLen())); // convert to Hz

			outHilbertStr << smallLib::doubleRound(helpDouble, 4) << "\t";

			/// experimental add
			outHilbertStr << smallLib::doubleRound(smallLib::sigma(env) / smallLib::mean(env), 4) << "\t";
		}
	}
	outDimStr.close();
	outHilbertStr.close();
}

void GalyaProcessing(const QString & procDirPath,
					 const int numChan,
					 QString outPath)
{
	const QString outDir = getFileName(procDirPath) + "_out";

	QDir dir;
	dir.cd(procDirPath);
	if(outPath.isEmpty())
	{
		dir.mkdir(outDir);
		outPath = dir.absolutePath() + slash + outDir;
	}
	else
	{
		dir.mkpath(outPath);
	}

	const QStringList filesList = dir.entryList(def::edfFilters,
												QDir::NoFilter,
												QDir::Size|QDir::Reversed);
	const auto filesVec = filesList.toVector();

#pragma omp parallel
#pragma omp for nowait
	for(int i = 0; i < filesVec.size(); ++i)
	{
		QString helpString = dir.absolutePath() + slash + filesVec[i];
		edfFile initEdf;

		initEdf.readEdfFile(helpString, true);

		/// different checks
		if(initEdf.getNdr() == 0)
		{
			cout << "ndr = 0\t" << filesVec[i] << endl;
			continue;
		}


		if(initEdf.getNs() < numChan)
		{
			cout << "GalyaProcessing: too few channels - " << procDirPath << endl;
			continue;
		}

		cout << filesList[i] << '\t'
			 << smallLib::doubleRound(QFile(helpString).size() / pow(2, 10), 1) << " kB" << endl;


		countChaosFeatures(helpString, numChan, outPath);
		countSpectraFeatures(helpString, numChan, outPath);
	}
}

void GalyaFull(const QString & inDirPath,
			   QString outDirPath,
			   QString outFileNames,
			   int numChan,
			   int freq,
			   int rightNum)
{
	if(!QDir().exists(inDirPath))
	{
		std::cout << "GalyaFull: path = " << inDirPath << " is wrong" << std::endl;
		return;
	}

	QDir tmp(inDirPath);
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
//	const QString outPath = inDirPath + myLib::slash + myLib::getFileName(inDirPath) + "_out";
	tmp.mkpath(outPath);

	const QString waveletPath = inDirPath + "_wavelet";
//	const QString waveletPath = inDirPath + myLib::slash + myLib::getFileName(inDirPath) + "_wavelet";
	tmp.mkpath(waveletPath);

	autos::GalyaProcessing(inDirPath,
						   numChan,
						   outPath);
	autos::makeRightNumbers(outPath,
							rightNum);

	for(QString type : {"_spectre", "_alpha", "_d2_dim", "_med.getFreq()"})
	{
		autos::makeTableFromRows(outPath,
								 outDirPath + slash + outFileNames + type + ".txt",
								 type);
	}

//	return;

	if(!wvlt::isInit) wvlt::initMtlb();
	autos::GalyaWavelets(inDirPath,
						 numChan, freq,
						 waveletPath);

	/// rename the folder in OUT to guy
	autos::makeRightNumbers(waveletPath, rightNum);
	autos::makeTableFromRows(waveletPath, outDirPath + slash + outFileNames + "_wavelet.txt");
}


void avTime(const QString & realsDir)
{
	int shortThreshold = 2 * def::freq;
	int longThreshold = 32 * def::freq - 125;
	int shortReals = 0;
	int longReals = 0;

	std::valarray<double> means{};
	means.resize(4); /// 4 types of mean - with or w/o shorts and longs

	int num = 0;
	std::ifstream inStr;

	QStringList lst;
	QString helpString;

	QDir localDir;
	localDir.cd(realsDir);

	for(const QString & tmp : {"241", "247"})
	{
		means = 0.;
		shortReals = 0;
		longReals = 0;

		lst = localDir.entryList({"*_" + tmp + "*"}, QDir::Files);
		for(const QString & fileName : lst)
		{
			helpString = localDir.absolutePath() + myLib::slash + fileName;

			// read numOfSlices
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

		localDir.cdUp();
		helpString = localDir.absolutePath() + myLib::slash + "avTime.txt";
		std::ofstream res;
		res.open(helpString.toStdString(), std::ios_base::app);
		res << "Reals type\t" << tmp << ":\r\n";
		res << "shortReals\t" << shortReals << "\r\n";
		res << "longReals\t" << longReals << "\r\n";
		res << "All together\t" << means[0] << "\r\n";
		res << "w/o shorts\t" << means[1] << "\r\n";
		res << "w/o longs\t" << means[2] << "\r\n";
		res << "w/o both\t" << means[3] << "\r\n\r\n";
		res.close();
	}
	cout << "avTime: finished" << endl;
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
			temp[0] = smallLib::distance(cData[i], cData[j]);
			temp[1] = i;
			temp[2] = j;
			temp[3] = 0;

			dists.push_back(temp);

			distOld[i][j] = temp[0];
			distOld[j][i] = temp[0];
		}
	}

	myLib::sammonProj(distOld, types,
					  "/media/Files/Data/Mati/sammon.jpg");
#if 0
	//test

	const int N = 15;
	const int dim = 2;

	distOld.clear();
	distOld.resize(N);
	for(int i = 0; i < N; ++i)
	{
		distOld[i].resize(N);
	}

	vector < vector<double> > dots;
	dots.resize(N);
	vector <double> ass;
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

//    cout << distOld << endl;

	sammonProj(distOld, types,
			   "/media/Files/Data/Mati/sammon.jpg");



exit(1);
return;

#endif


#if 0
	//smallest tree
	std::sort(dists.begin(), dists.end(), mySort);
	// make first bound

	boundDots[ dists[0][1] ] = true;
	isolDots[ dists[0][1] ] = false;

	boundDots[ dists[0][2] ] = true;
	isolDots[ dists[0][2] ] = false;

	dists[0][3] = 2;
	newDists.push_back(dists[0]);



	vector<vector<double> >::iterator itt;
	while (contains(isolDots.begin(), isolDots.end(), true))
	{
		//adjust dists[i][3]
		for(vector<vector<double> >::iterator iit = dists.begin();
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
//            cout << (*itt)[j] << '\t';
//        }
//        cout << endl;
	}
	std::sort(newDists.begin(), newDists.end(), mySort);
	vector <double> newD;
	for(int i = 0; i < newDists.size(); ++i)
	{
		newD.push_back(newDists[i][0]);
	}
//    cout << newD << endl;


//    helpString = "/media/Files/Data/Mati/clust.jpg";
//    kernelEst(newD.data(), newD.size(), helpString);
//    helpString = "/media/Files/Data/Mati/clustH.jpg";
//    histogram(newD.data(), newD.size(), 40, helpString);
#endif
}




} // namespace autos
