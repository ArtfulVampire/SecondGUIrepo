#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <widgets/spectre.h>
#include <widgets/net.h>
#include <widgets/cut.h>

#include <other/autos.h>
#include <other/defs.h>

#include <myLib/drw.h>
#include <myLib/mati.h>
#include <myLib/dataHandlers.h>
#include <myLib/general.h>

#include <other/subjects.h>

using namespace myOut;

matrix countEvoked(const std::vector<matrix> & inData, int length)
{
	matrix res(inData[0].rows(), length);

	for(const matrix & in : inData)
	{
		res += in.subCols(0, length);
	}
	res /= inData.size();
	return res;
}

void MainWindow::drawWeights(const QString & wtsPath,
							 const QString & spectraPath,
							 const QString & outPath,
							 int fftLen)
{
	/// draw Wts from a folder
	ANN * net = new ANN();

	ClassifierData cl = ClassifierData(spectraPath);
	net->setClassifierData(cl);

	DEFS.setFftLen(fftLen);

	for(const QString & fileName : QDir(wtsPath).entryList({"*.wts"}))
	{
		QString drawName = fileName;
		drawName.replace(".wts", ".jpg");
		net->drawWeight(wtsPath + "/" + fileName,
						outPath + "/" + drawName);
	}
	delete net;
}

void MainWindow::makeEvoked(const QString & edfPath,
							const std::valarray<double> & startMarkers,
							const QString & outPath)
{
	const int length = 600;
	edfFile fil;
	fil.readEdfFile(edfPath);
	std::vector<matrix> dat;
	for(auto mark : fil.getMarkers())
	{
		if(std::find(std::begin(startMarkers),
					 std::end(startMarkers),
					 mark.second) != std::end(startMarkers))
		{
			dat.push_back(fil.getData().subCols(mark.first, mark.first + length));
		}
	}
	auto res = countEvoked(dat, length);
	myLib::writeMatrixFile(outPath, res);
}

void MainWindow::makeEvoked(const QString & realsPath,
							const QStringList & fileFilters,
							const QString & outPath)
{
	const int length = 600;
	QStringList reals = QDir(realsPath).entryList(fileFilters);

	std::vector<matrix> dat(reals.size());
	int i = 0;
	for(const QString & fileName : reals)
	{
		dat[i++] = myLib::readMatrixFile(realsPath + "/" + fileName);
	}
	auto res = countEvoked(dat, length);
	myLib::writeMatrixFile(outPath, res);
}


/// successiveByEDF ansPathes
void MainWindow::testSuccessive2()
{
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};
//	const QStringList names {"GAS"};


	std::vector<double> res;
	for(const QString & name : names)
	{
		/// current best set
		suc::numGoodNewLimit = 3;
		suc::learnSetStay = 40;
		suc::decayRate = 0.00;


		/// should not change averageDatum and sigmaVector ?
		Net * net = new Net();

		net->setSource("w");
		net->setMode("t"); /// train-test

		std::cout << name << std::endl;
		res.push_back(std::get<0>(net->successiveByEDFnew(path + name + "_train" + ".edf", "",
														  path + name + "_test" + ".edf", "")));
		delete net;
	}
	outStream << "average by people = "
			  << std::accumulate(std::begin(res), std::end(res), 0.) / res.size() << std::endl;
}

void MainWindow::testSuccessive(const std::vector<double> & vals)
{
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

//	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};
	const QStringList names {"GAS"};

	bool sliceAndCount = true;
//	bool sliceAndCount = false;



	const double windLength	= suc::windLength;
	const double shiftTest	= suc::shiftTest;
	const double shiftLearn	= suc::shiftLearn;
	const int numSmooth		= suc::numSmooth;


	ui->windowLengthSpinBox->setValue(windLength);
	ui->windsButton->setChecked(true); /// sliceWindFromReal

	for(const QString & name : names)
	{
		/// successive
		setEdfFile(path + name + "_train.edf");

		myLib::cleanDir(path + "Reals");
		myLib::cleanDir(path + "winds/fromreal");
		myLib::cleanDir(path + "SpectraSmooth/winds");

		ui->timeShiftSpinBox->setValue(shiftLearn);
		if(sliceAndCount)
		{
			sliceAll();
			myLib::cleanDir(path + "Reals");
//			countSpectraSimple(1024, numSmooth);


			/// delete first three winds from each realisation
			QStringList windsList = QDir(path + "winds/fromreal").entryList({"*_train*.00",
																			 "*_train*.01",
																			 "*_train*.02"},
																			QDir::Files);
			/// delete first some winds from reals
			for(const QString & name : windsList)
			{
				QFile::remove(path + "winds/fromreal/" + name);
			}

			/// magic constant
			/// leave last 600 winds (some will fall out further due to zeros)
			/// REMAKE - leave 120 each type
			windsList = myLib::makeFullFileList(path + "winds/fromreal", {"_train"});
			for(int i = 0; i < windsList.length() - 800; ++i) /// constant
			{
				QFile::remove(path + "winds/fromreal/" + windsList[i]);
			}
		}
		setEdfFile(path + name + "_test" + ".edf");
		ui->timeShiftSpinBox->setValue(shiftTest);

		/// DON'T CLEAR, TRAIN winds TAKEN BY SUCCESSIVE
		if(sliceAndCount)
		{
			sliceAll();
			myLib::cleanDir(path + "Reals");
//			countSpectraSimple(1024, numSmooth);
		}
		else
		{
			readData();
		}

		/// current best set
		suc::numGoodNewLimit = 3;
		suc::learnSetStay = 40;
		suc::decayRate = 0.00;


		countSpectraSimple(1024, numSmooth);
		/// should not change averageDatum and sigmaVector ?
		Net * net = new Net();
		net->loadData(DEFS.windsSpectraDir(), {name + "_train"});

		net->setClassifier(ModelType::ANN);
		net->setSource("w");
		net->setMode("t"); /// train-test

		std::cout << name << std::endl;
		net->successiveProcessing();
		delete net;


	}
}

void MainWindow::testNewClassifiers()
{
	QString paath = "/media/Files/Data/Feedback/SuccessClass/";

//	for(const QString & guy : {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"})
	for(const QString & guy : {"PMI", "SMM", "SMS", "SUA"})
//	for(const QString & guy : {"GAS"})
	{
		for(const QString & suff :{"_train", "_test"})
		{
			setEdfFile(paath + guy + suff + ".edf");
//			readData();

			Net * net = new Net();
			net->loadData(paath + "/SpectraSmooth/winds", {guy + suff});

			net->setClassifier(ModelType::ANN);
			net->setMode("k");
			net->setSource("w");
			net->setNumOfPairs(30);
			net->setFold(4);

//			net->customF(); /// clean to 3*N train windows


//			std::cout << guy + suff << std::endl;
			net->autoClassification();



			std::vector<std::vector<double>> allPew =  {{3., 30.},
														{3., 40.},
														{4., 40.},
														{4., 50.},
														{5., 50.},
														{6., 60.},
														{8., 80.},
														{8., 100.},
														{10., 100.}
													   };
//			for(auto pewww : allPew)
//			{
			for(int i1 = 3; i1 <= 8; ++i1)
			{
				for(int i2 = 70; i2 <= 110; i2 += 10)
				{
					for(double i3 : {-0.005, 0.00, 0.005})
					{
//						suc::numGoodNewLimit = pewww[0];
//						suc::learnSetStay = pewww[1];
						suc::numGoodNewLimit = i1;
						suc::learnSetStay = i2;
						suc::decayRate = i3;
						std::cout << guy << std::endl;
						std::cout << suc::numGoodNewLimit << '\t';
						std::cout << suc::learnSetStay << '\t';
						std::cout << suc::decayRate << std::endl;
						net->successiveProcessing();
					}
				}

			}

			delete net;
			break; /// only suff = "_train"
		}

//		continue;

		/// loading UCI dataset - add enum
//		net->loadDataUCI("cmi");
//		net->setClassifier(ClassifierType::RDA);
//		net->setRdaLambdaSlot(0.8);
//		net->setRdaShrinkSlot(0.8);
//		net->autoClassification();

	}
}

void MainWindow::BaklushevDraw(const QString & workPath, const QString & edfName)
{
	QString guy = myLib::getFileName(edfName, false);

	QString filePath = workPath + "/" + edfName;
	if(!QFile::exists(filePath))
	{
		std::cout << "file doesn't exist: " << filePath << std::endl;
		std::cout << "nothing will be drawn" << std::endl;
		return;
	}

	setEdfFile(filePath);
	readData();
	Spectre * sp = new Spectre();
	sp->setFftLength(4096);
	delete sp;

	QString spectraPath = workPath + "/SpectraSmooth";

	const std::vector<QString> marker{"*_241*" , "*_247*"};
	std::array<matrix, 2> drawMat{};

	for(int i = 0; i < 2; ++i)
	{
		QStringList lst = QDir(spectraPath).entryList({marker[i]});
		drawMat[i].reserve(lst.size());

		for(const QString & fileName : lst)
		{
			drawMat[i].push_back(myLib::readFileInLine(spectraPath + "/" + fileName));
		}
	}
	double norm = 0;

	std::vector<std::valarray<double>> avArr = {drawMat[0].averageRow(),
												drawMat[1].averageRow()};
	std::vector<std::valarray<double>> sigmArr = {drawMat[0].sigmaOfCols(),
												  drawMat[1].sigmaOfCols()};


	QColor currColor[2] = {QColor("blue"), QColor("red")};

	const QString picture[2] = {workPath + "/" + guy + "_picSpat.jpg",
								workPath + "/" + guy + "_picVerb.jpg"};
	const QString pictures[2] = {workPath + "/"  + guy + "_picSpat_2.jpg",
								 workPath + "/"  + guy + "_picVerb_2.jpg"};


	/// max val of avArr + sigmArr
	norm = std::max((avArr[0] + sigmArr[0]).max(), (avArr[1] + sigmArr[1]).max());
	for(int i = 0; i < 2; ++i)
	{
		myLib::drw::drawArrayWithSigma(myLib::drw::drawTemplate(true),
									   avArr[i],
									   sigmArr[i],
									   norm,
									   currColor[i]).save(pictures[i], nullptr, 100);
	}


	/// max val in any of realisations
	norm = std::max(drawMat[0].maxVal(), drawMat[1].maxVal());
	/// draw each realisation spectre
	for(int i = 0; i < 2; ++i)
	{
		myLib::drw::drawArrays(myLib::drw::drawTemplate(),
							   drawMat[i],
							   norm,
							   false,
							   std::vector<QColor>(drawMat[i].rows(), currColor[i]),
							   std::vector<int>(drawMat[i].rows(), 1)).save(
					picture[i], nullptr, 100);

	}
}

void MainWindow::countSpectraSimple(int fftLen, int inSmooth)
{
    Spectre * sp = new Spectre();
    sp->setFftLength(fftLen);
    if(inSmooth >= 0)
    {
        sp->setSmooth(inSmooth);
    }
    sp->countSpectraSlot();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
}

void MainWindow::iitpNamesFix()
{
	/// IITP filenames prep
	const QString wrk = "/media/Files/Data/iitp/Aliev2";
	QStringList lst = QDir(wrk).entryList({"*.*"}, QDir::Files|QDir::NoDotAndDotDot);
	for(QString str : lst)
	{
		QString newName = str;
//		newName.remove("virt");
//		newName.remove("VR");
//		newName.replace("Aliev", "Aliev2");
		newName.replace(".EDF", "_eeg.edf");
		if(0)
		{
			std::cout << str << std::endl;
			std::cout << newName << std::endl;
			std::cout << std::endl;
		}
		else
		{
			QFile::rename(wrk + "/" + str,
						  wrk + "/" + newName);

		}
	}


#if 0
	/// IITP invert channels
	const QString toInvert{"Knee_l"};
	const QStringList guyList{"Aliev2", "Dima2", "Victor2"};
	for(QString guy : guyList)
	{
		const QString workDir = def::iitpSyncFolder + "/" + guy;
		for(const QString & fn : QDir(workDir).entryList(def::edfFilters))
		{
			edfFile fil;
			fil.readEdfFile(workDir + "/" + fn);

			const auto newRow = -fil.getData(toInvert);
			fil.setData(fil.findChannel(toInvert), newRow);
			fil.writeEdfFile(workDir + "/" + fn);
		}
	}
	exit(0);
#endif


#if 0
	/// IITP substitute channels
	{
		const QString oldChan{"Elbow_r"};
		const QString newChan{"Elbow_l"};
		const QStringList guyList{"Michael"};
		const std::vector<int> nums{17, 32, 34, 35};
		for(QString guy : guyList)
		{
			const QString workDir = def::iitpSyncFolder + "/" + guy;
			for(const QString & fn : QDir(workDir).entryList(def::edfFilters))
			{
				bool skip{true};
				for(int each : nums) { if(fn.contains("_" + nm(each))) { skip = false; } }
				if(skip) { continue; }

				edfFile fil;
				fil.readEdfFile(workDir + "/" + fn);

				fil.setData(fil.findChannel(oldChan),
							-fil.getData(newChan));
				fil.writeEdfFile(workDir + "/" + fn);
			}
		}
	}
	exit(0);
#endif


#if 0
	/// insert absent channels for all files of a guy
	{
		const QString guy = "Isakov2";
//		const QString guyPath = def::iitpSyncFolder + "/" + guy + "/bc_noRectify";
		const QString guyPath = def::iitpFolder + "/" + guy;
		const QString postfix = "_eeg";
		auto badFiles = autos::IITPtestEegChannels(guyPath, postfix);
//		autos::IITPinsertChannels(guyPath, badFiles);
		exit(0);
	}
#endif


#if 0
	/// replace Knee_r with Elbow_r		when legs only
	/// and Knee_r with -Knee_l			when legs + arms
	for(const QString & guy : {"Elena", "Ivan"})
	{
		const QString pth = def::iitpSyncFolder + "/" + guy;
		for(const QString & mrk : {"02", "30", "31"})
		{
			const QString filePath = pth + "/" + guy + "_" + mrk + "_sum_new.edf";
			if(!QFile::exists(filePath)) { continue; }

			edfFile dt;
			dt.readEdfFile(filePath);
			dt.setData(dt.findChannel("Knee_r"), dt.getData("Elbow_r"));
			dt.setData(dt.findChannel("Elbow_r"), std::valarray<double>(0., dt.getDataLen()));
			dt.rewriteEdfFile();
		}
		for(const QString & mrk : {"17", "34", "35"})
		{
			const QString filePath = pth + "/" + guy + "_" + mrk + "_sum_new.edf";
			if(!QFile::exists(filePath)) { continue; }

			edfFile dt;
			dt.readEdfFile(filePath);
			dt.setData(dt.findChannel("Knee_r"), -dt.getData("Knee_l"));
			dt.rewriteEdfFile();
		}
	}
	exit(0);
#endif

#if 0
	/// IITP file into two files

	iitp::iitpData fil;
	fil.readEdfFile("/media/Files/Data/iitp/SYNCED/Test/Test_01_sum_new.edf");
	std::valarray<double> iitp1 = fil.getData("P4");
	iitp1 -= smLib::mean( iitp1 );
//	std::valarray<double> iitp2 = fil.getData("Fcr");
	std::valarray<double> iitp2 = fil.getData("Ta");
	iitp2 -= smLib::mean( iitp2 );

	if(0)
	{
		myLib::writeFileInLine("/media/Files/Data/iitp1.txt",
							   iitp1);
		myLib::writeFileInLine("/media/Files/Data/iitp2.txt",
							   iitp2);
//		exit(0);
	}


	/// IITP test coh two files
	const int fftLen = 256;
	const double srate = 250.;
	const double overlap = 0.0;

	auto usual = iitp::coherenciesUsual(iitp1,
										iitp2,
										overlap,
										srate,
										fftLen);
	usual[0] = {0.};

	const int windNum = (iitp1.size() - fftLen) / (fftLen * (1. - overlap));
	const double confidence = 1. - std::pow(0.05, 1. / (windNum - 1));
	std::cout << "windNum = " << windNum << std::endl;
	std::cout << "confidence = " << confidence << std::endl;


//	std::valarray<double> fromMatlab;
//	fromMatlab = myLib::readFileInLineRaw("/media/Files/Data/mCoh.txt");
//	smLib::resizeValar(fromMatlab, siz);
//	double m = std::max(fromMatlab.max(), usualDraw.max());

//	std::valarray<double> rat = usualDraw / fromMatlab;
//	std::cout << rat << std::endl;
//	myLib::histogram(rat, 20
//					 , std::make_pair(5,20)
//					 ).save("/media/Files/Data/rat.jpg", nullptr, 100);
//	autos::IITPdrawCoh(fromMatlab, m, confidence).
//			save("/media/Files/Data/mCoh.jpg", nullptr, 100);

	autos::IITPdrawCoh(usual,
					   8.,
					   45.,
					   srate,
					   fftLen,
					   0.,
					   confidence).
			save("/media/Files/Data/cCohUsual.jpg", nullptr, 100);



	exit(0);
#endif


#if 0
	/// IITP test model data
	const int fftLen = 1024;
	const int wndN = 64;
	const double srate = 250.;
	const double spStep = srate / fftLen;
	const double intFr = 17.2;
	const double interestFreq = std::round(intFr / spStep) * spStep;
//	const double interestFreq = 17.2;
	std::cout << "interestFreq = " << interestFreq << std::endl;

	std::valarray<double> sig1 = myLib::makeSine(fftLen * wndN, interestFreq, srate, 0);
	sig1 += myLib::makeNoise(sig1.size()) * 0.01;
	std::valarray<double> sig2 = myLib::makeSine(fftLen * wndN, interestFreq, srate, 0.5);
//	sig2 += myLib::makeNoise(sig2.size()) * 0.05;

	for(int i = 0; i < sig1.size() - fftLen; i += fftLen)
	{
		auto t = myLib::makeSine(fftLen * wndN, interestFreq, srate,
								 (rand() % 2 - 0.5) * 2 * (rand() % 100) / 100. * 0.5);
		for(int j = 0; j < fftLen; ++j)
		{
			sig2[j + i] = t[j + i];
		}
	}
	std::this_thread::sleep_for(std::chrono::seconds{2});
	sig2 += myLib::makeNoise(sig2.size()) * 0.01;


	myLib::writeFileInLine("/media/Files/Data/sig1.txt",
						   sig1);
	myLib::writeFileInLine("/media/Files/Data/sig2.txt",
						   sig2);


	auto mine = iitp::coherenciesMine(sig1,
									  sig2,
									  srate,
									  fftLen);


	auto usual = iitp::coherenciesUsual(sig1,
										sig2,
										srate,
										fftLen);
	const double confidence = 1. - std::pow(0.05, 1. / (sig1.size() / fftLen - 1));
	std::cout << "confidence = " << confidence << std::endl;

	const double freqC = fftLen / srate;
	std::cout << "freqC = " << freqC << std::endl;
	const int numFreq = 45;
	const int siz = freqC * numFreq;
	std::cout << "siz = " << siz << std::endl;


	std::valarray<double> mineDraw(siz);
	std::valarray<double> usualDraw(siz);
	for(int i = 0; i < siz; ++i)
	{
		mineDraw[i] = std::abs(mine[i]);
		usualDraw[i] = std::abs(usual[i]);
	}

#if 0
	const auto & arr = mineDraw;
	const QString nam = "Mine";
#else
	const auto & arr = usualDraw;
	const QString nam = "Usual";
#endif

//	std::cout << arr.max() << std::endl;

	QPixmap pic(800, 600);
	pic.fill();
	QPainter pnt;
	pnt.begin(&pic);
	for(int i = 0; i < siz-1; ++i)
	{
//		std::cout << i << "\t" << arr[i] << std::endl;
		pnt.drawLine( pic.width() * i / static_cast<double>(siz),
					  pic.height() * (1. - arr[i]),
					  pic.width() * (i + 1) / static_cast<double>(siz),
					  pic.height() * (1. - arr[i + 1]));
	}
	/// draw Hz
	const QFont font = QFont("Helvetica", 8);
	pnt.setFont(font);
	for(int i = 0; i < numFreq; ++i)
	{
		pnt.drawLine( pic.width() * i / numFreq,
					  pic.height() * 1,
					  pic.width() * i / numFreq,
					  pic.height() * (1. - 0.03));
		pnt.drawText(pic.width() * i

					 auto usual = iitp::coherenciesUsual(sig1,
														 sig2,
														 srate,
														 fftLen);
					 const double confidence = 1. - std::pow(0.05, 1. / (sig1.size() / fftLen - 1));
					 std::cout << "confidence = " << confidence << std::endl;

					 const double freqC = fftLen / srate;
					 std::cout << "freqC = " << freqC << std::endl;
					 const int numFreq = 45;
					 const int siz = freqC * numFreq;
					 std::cout << "siz = " << siz << std::endl;


					 std::valarray<double> mineDraw(siz);
					 std::valarray<double> usualDraw(siz);
					 for(int i = 0; i < siz; ++i)
					 {
						 mineDraw[i] = std::abs(mine[i]);
						 usualDraw[i] = std::abs(usual[i]);
					 }

				 #if 0
					 const auto & arr = mineDraw;
					 const QString nam = "Mine";
				 #else
					 const auto & arr = usualDraw;
					 const QString nam = "Usual";
				 #endif

				 //	std::cout << arr.max() << std::endl;

					 QPixmap pic(800, 600);
					 pic.fill();
					 QPainter pnt;
					 pnt.begin(&pic);
					 for(int i = 0; i < siz-1; ++i)
					 {
				 //		std::cout << i << "\t" << arr[i] << std::endl;
						 pnt.drawLine( pic.width() * i / static_cast<double>(siz),
									   pic.height() * (1. - arr[i]),
									   pic.width() * (i + 1) / static_cast<double>(siz),
									   pic.height() * (1. - arr[i + 1]));
					 }
					 /// draw Hz
					 const QFont font = QFont("Helvetica", 8);
					 pnt.setFont(font);
					 for(int i = 0; i < numFreq; ++i)
					 {
						 pnt.drawLine( pic.width() * i / numFreq,
									   pic.height() * 1,
									   pic.width() * i / numFreq,
									   pic.height() * (1. - 0.03));
						 pnt.drawText(pic.width() * i / numFreq,
									  pic.height() * (1. - 0.03) + QFontMetrics(font).xHeight(),
									  QString::number(i));
					 }
					 /// confidence line
					 pnt.drawLine(0,
								  pic.height() * (1. - confidence),
								  pic.width(),
								  pic.height() * (1. - confidence));
					 pnt.end();
					 pic.save("/media/Files/Data/cCoh" + nam + ".jpg", nullptr, 100);

					 exit(0); / numFreq,
					 pic.height() * (1. - 0.03) + QFontMetrics(font).xHeight(),
					 QString::number(i));
	}
	/// confidence line
	pnt.drawLine(0,
				 pic.height() * (1. - confidence),
				 pic.width(),
				 pic.height() * (1. - confidence));
	pnt.end();
	pic.save("/media/Files/Data/cCoh" + nam + ".jpg", nullptr, 100);

	exit(0);
#endif

}

void MainWindow::iitpPreproc()
{
	/// prepare FeedbackFinalMark for eyes clean
	const QString path = DEFS.dirPath() + "/FeedbackNewMark";

	Cut * cut = new Cut();
	for(auto in : subj::guysFBnew)
	{
		const QString dr = std::get<0>(in);
		const QString ExpName = std::get<1>(in);


		for(int i : {1, 2, 3})
		{
//			if(!(ExpName == "TAA" && i == 2)) continue;
			QString fn = path + "/" + dr + "/" + ExpName + "_" + nm(i) + "_good";

			std::cout << std::endl << ExpName << " " << i << std::endl;

			if(0)
			{
				/// rename all EDF to edf
				if(QFile::exists(fn + ".EDF"))
				{
					if(QFile::exists(fn + ".edf"))
					{
						std::cout << "both files exist " << ExpName << " " << i << std::endl;
						continue;
					}
					QFile::rename(fn + ".EDF", fn + ".edf");
				}
				continue;

			}
			cut->openFile(fn + ".edf");
			cut->cutPausesSlot();
			cut->saveSlot();

			this->setEdfFile(fn + "_new.edf");
			this->rereferenceDataSlot();
			this->setEdfFile(fn + "_new_rr.edf");
			this->refilterDataSlot();
		}
//		break;
	}
	cut->close();
	delete cut;
	exit(0);
}

void MainWindow::iitpMainProc()
{
	QString guy = "Isakov2";
	{
		if(0)
		{
			/// check Da/Dp channels - should appear in 15th file and further
			const QString ph = def::iitpSyncFolder + "/" + guy;
			for(QString fn : QDir(ph).entryList({"*_sum_new.edf"}))
			{
				edfFile fl;
				fl.readEdfFile(ph + "/" + fn, true);
				if(fl.findChannel("Da") != -1)
				{
					std::cout << "Da: " << fn << std::endl;
				}
				if(fl.findChannel("Dp") != -1)
				{
					std::cout << "Dp: " << fn << std::endl;

				}
				continue;

			}
		}

		if(0)
		{
			autos::IITPremoveZchans(guy, def::iitpFolder);			/// rewrites _eeg.edf
			autos::IITPdatToEdf(guy);								/// all dat files in dir
			autos::IITPfilter(guy, "_emg", true, true, false);		/// rewrites all _emg.edf
			if(!autos::IITPtestInitialFiles(guy))
			{
				exit(0);
			}
			return; /// clean init eeg - zero in the beginning for better filering
		}
//		return;
		if(0)
		{
			if(01)
			{
				/// filter eeg 0.5-70, notch 45-55
//				autos::IITPfilter(guy, "_eeg_new", false, false, true);	/// rewrites _eeg_new.edf
				autos::IITPconcat(guy, "_eeg_new", "_emg");				/// if cleaned init eeg
			}
			else
			{
//				autos::IITPconcat(guy, "_eeg", "_emg");				/// if NOT cleaned init eeg
			}
			return; /// manual sync
		}
		/// copy files to SYNCED
		if(0)
		{
			/// rectifyEMG in SYNCED
			autos::IITPrectifyEmg(guy);
			exit(0);
		}
//		return;
		if(01)
		{
//			std::cout << "staging start" << std::endl;
//			autos::IITPstaging(guy);							/// flex/extend markers
//			std::cout << "staging end, copy start" << std::endl;
			autos::IITPcopyToCar(guy);							/// copy ALL *_stag.edf to guy_car
//			std::cout << "copy end, reref start" << std::endl;
			autos::IITPrerefCAR(guy + "_car");					/// rewrite ALL edfs in SYNCED/guy_car
//			return;
		}
		/// CHECK car files if needed
		if(01)
		{
//			std::cout << "copy end, process start" << std::endl;
			autos::IITPprocessStaged(guy);						/// both -Ref and -car
//			std::cout << "process end, draw start" << std::endl;
			autos::IITPdrawSpectralMaps(guy);					/// both -Ref and -car
//			std::cout << "all end" << std::endl;
//			continue;
//			exit(0);
		}

		exit(0);
//		continue;


#if 0
		/// deleted lists 14.10.2017
		myLib::drw::trueChans = iitp::interestEeg;

		using Typ = std::vector<int>;
		Typ nums = smLib::unite<Typ>({smLib::range<Typ>(0, 11 + 1),
									  smLib::range<Typ>(24, 29 + 1)});
//		nums.erase(std::find(std::begin(nums), std::end(nums), 8));
		autos::IITPdrawSameScale(guy, smLib::range<Typ>(0, 11 + 1));
		exit(0);
#endif
	}
}


void MainWindow::matiMainProc()
{
	if(1)
	{
		/// deprecated, use edfFile.concat
//        concatenateEDFs("/media/Files/IHNA/Data/MATI/archive/NOS_1.EDF",
//                        "/media/Files/IHNA/Data/MATI/archive/NOS_2.EDF",
//                        "/media/Files/IHNA/Data/MATI/archive/NOS.EDF");
//		exit(0);

//        QString helpString1 = "/media/Files/Data/Mati/PYV/PY_3.edf";
//        std::cout << fileNameOf(helpString1) << std::endl;


//        ui->matiCheckBox->setChecked(true);
//        ui->sliceWithMarkersCheckBox->setChecked(true);
//        ui->reduceChannelsCheckBox->setChecked(true);
//        ui->reduceChannelsComboBox->setCurrentText("Mati");
//        setEdfFile("/media/Files/Data/Mati/SDA/SDA_rr_f.edf");

		readData();
		exit(0);
//        ns = 22;
//        cutShow();
//        drawReals();
//        sliceAll();
//        ns = 19;
//        ui->reduceChannelsLineEdit->setText("1 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 28");
		return;

		QDir locDir(def::matiFolder);
		QStringList nameFilters;
		nameFilters << "*.edf" << "*.EDF";
		QString fileName;
		QString helpString;

		QStringList lst = locDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		for(int i = 0; i < lst.length(); ++i)
		{
			locDir.cd(lst[i]);

			helpString = locDir.absolutePath() + "/" + lst[i] + ".EDF";
			setEdfFile(helpString);
			rereferenceDataSlot();

			helpString = locDir.absolutePath() + "/" + lst[i] + "_rr.edf";
			setEdfFile(helpString);
			refilterDataSlot();

			helpString = locDir.absolutePath() + "/" + lst[i] + "_rr_f.edf";
			setEdfFile(helpString);
			sliceAll();
			drawReals();
			locDir.cdUp();

		}
		exit(0);
	}

	// test matiMarkers functions
	if(0)
	{
		double doub1 = 0b0000010110000000;
		double doub2 = 0b0000000010000000;

		double & firstMarker = doub1;
		double & lastMarker  = doub2;

		myLib::matiFixMarker(firstMarker); // fix the start marker for this small edf file
		myLib::matiFixMarker(lastMarker);  // fix the last  marker for this small edf file
		if(myLib::matiCountBit(firstMarker, 10) != myLib::matiCountBit(lastMarker, 10)) // if not one of them is the end of some session
		{
			lastMarker = firstMarker
				+ std::pow(2, 10) * (myLib::matiCountBit(firstMarker, 10) ? -1 : 1); // adjust the last marker
		}
		myLib::matiPrintMarker(lastMarker, "last");
		myLib::matiPrintMarker(doub2, "newData");
		exit(0);
	}

}
