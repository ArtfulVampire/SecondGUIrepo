#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/drw.h>
#include <myLib/iitp.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/temp.h>
#include <myLib/statistics.h>
#include <myLib/wavelet.h>
#include <other/subjects.h>
#include <thread>

using namespace myOut;

void MainWindow::customFunc()
{
	ui->matiCheckBox->setChecked(false);
	ui->realsButton->setChecked(true);
	if(DEFS.isUser(username::ElenaC))
	{
		ui->elenaSliceCheckBox->setChecked(true); // Elena
		ui->eogBipolarCheckBox->setChecked(false); // Elena
	}

	/// feedback successive
//	const QString wrk = DEFS.dirPath() + "/FeedbackFinalMark/Avdeev";
//	Net * net = new Net();
//	net->successiveByEDFfinal(wrk + "/AKV_1_fin.edf",
//							  wrk + "/AKV_ans1.txt",
//							  wrk + "/AKV_2_fin.edf",
//							  wrk + "/AKV_ans2.txt");
//	exit(0);

//	edfFile fil;
//	fil.readEdfFile("/media/Files/Data/Geodesics/Polina/Mal.edf");
//	double maxDif = 0;
//	for(int i = 0; i < fil.getNs() - 1; ++i)
//	{
//		maxDif = std::max(maxDif, fil.getPhysMax()[i] - fil.getPhysMin()[i]);
//	}
//	exit(0);

	return;

#if 0
	/// Xenia finalest
	const QString workPath = "/media/Files/Data/Xenia/FINAL";
	const std::vector<QString> subdirs{"Healthy", "Moderate", "Severe"};

	if(0) /// initial rename guyDirs as edfs inside
	{
		for(QString subdir : subdirs)
		{
			const QString groupPath = workPath + "/" + subdir;
			const QStringList groupGuys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			for(QString guy : groupGuys)
			{
				const QString guyPath = groupPath + "/" + guy;
				repair::deleteSpacesDir(guyPath);
				QString firstFile = QDir(guyPath).entryList(def::edfFilters)[0];
				firstFile.resize(firstFile.lastIndexOf('_'));
				QDir().rename(guyPath,
							  groupPath + "/" + firstFile);
				continue;
			}
		}
		exit(0);
	}
	if(0) /// check each guy has all stimuli files - OK
	{
		const std::vector<QString> tbiMarkers{"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"};
		for(QString subdir : subdirs)
		{
			const QString groupPath = workPath + "/" + subdir;
			const QStringList groupGuys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			for(QString guy : groupGuys)
			{
				const QString guyPath = groupPath + "/" + guy;
				const QStringList guyFiles = QDir(guyPath).entryList(def::edfFilters);
				for(const QString & mark : tbiMarkers)
				{
					bool ok = false;
					for(const QString guyFile : guyFiles)
					{
						if(guyFile.contains(mark)) { ok = true; break; }
					}
					if(!ok) { std::cout << guy << mark << " not found!!!" << std::endl; }
				}

			}
		}
		exit(0);
	}
	if(0) /// find absent guyNames in guys_finalest.txt and rename properly (manually)
	{
		/// read guys_finalest.txt
		QFile fil("/media/Files/Data/Xenia/guys_finalest.txt");
		fil.open(QIODevice::ReadOnly);
		std::vector<QString> guys{};
		while(1)
		{
			QString guy = fil.readLine();
			guy.chop(1); /// chop \n
			if(guy.isEmpty()) { break; }
			guys.push_back(guy);
		}
		fil.close();


		std::vector<QString> guysReal{};
		for(QString subdir : subdirs)
		{
			const QString groupPath = workPath + "/" + subdir;
			const QStringList groupGuys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			for(QString guy : groupGuys)
			{
				guysReal.push_back(guy);
			}
		}
		for(const auto & guy : guys)
		{
			if(!myLib::contains(guysReal, guy))
			{
				std::cout << guy << std::endl;
			}
		}
		exit(0);
	}

	if(01) /// processing itself
	{
		DEFS.setAutosUser(autosUser::XeniaFinalest);
		const std::vector<QString> tbiMarkers{"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"};
		DEFS.setAutosMask(featuresMask::wavelet);
		autos::Xenia_TBI_finalest(workPath, workPath + "_res", tbiMarkers);
	}

	exit(0);
#endif

#if 0
	/// check marks
	const QString path = QString("/media/Files/Data/FeedbackNewMark/");
	for(const QString & subdir : QDir(path).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
		for(QString fl : QDir(path + "/" + subdir).entryList({"*_good.edf", "*_good.EDF"}))
		{
			fb::checkMarkFBfinal(path + "/" + subdir + "/" + fl);
		}
	}
	exit(0);
#endif


#if 0
	/// count correctness and average times
#if 0 /// new (~10 people)
	const QString dear = "FeedbackNewMark";
	const auto & guysList = subj::guysFBnew;
	const QString postfix = "_good";
#else /// final (~16 people)
	const QString dear = "FeedbackFinalMark";
	const auto & guysList = subj::guysFBfinal;
	const QString postfix = "_fin";
	/// make all data_fin
#endif
	fb::countStats(dear, guysList, postfix);
	exit(0);
#endif

#if 0
	/// cut central 9 channels
	QPixmap pic;
	QDir tmp("/media/michael/Files/IHNA/Pew");
	QStringList lii = tmp.entryList(QDir::Files);
	for(QString picPath : lii)
	{
		pic = QPixmap(tmp.absolutePath() + "/" + picPath);
		QPixmap cut = pic.copy(QRect(330, 350, 900, 900));
		QString out = tmp.absolutePath() + "/" + picPath;
		out.replace(".jpg", "_cut.jpg");
		cut.save(out, 0, 100);
	}
	exit(0);
#endif

#if 0
	/// draw Wts from a folder
	const QString inDir = "/media/Files/Data/FeedbackTest/GA_FB/GA_FB_weights";
	ANN * net = new ANN();
	ClassifierData cl = ClassifierData("/media/Files/Data/FeedbackTest/GA_FB/winds/fromreal");
	net->setClassifierData(cl);

//	std::cout << net->getClassifierData()->getData().cols() << std::endl;
	DEFS.setFftLen(1024);

	for(QString fileName : QDir(inDir).entryList({"*.wts"}))
	{
		QString drawName = fileName;
		drawName.replace(".wts", ".jpg");
		net->drawWeight(inDir + "/" + fileName,
					   inDir + "/" + drawName);
//		break;
	}
	exit(0);
#endif

#if 0
	/// prepare FeedbackFinalMark for eyes clean
	const QString path = DEFS.dirPath() + "/FeedbackFinalMark";

	Cut * cut = new Cut();
	for(auto in : subj::guysFBfinal)
	{
		const QString dr = std::get<0>(in);
		const QString ExpName = std::get<1>(in);


		for(int i : {1, 2, 3})
		{
			QString fn = path + "/" + dr + "/" + ExpName + "_" + nm(i);

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
#endif

#if 0
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
	exit(0);
#endif


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
	{
		const QString guy = "Michael";
		const QString guyPath = def::iitpFolder + "/" + guy;
		const QString postfix = "_eeg";
		auto badFiles = autos::IITPtestEegChannels(guyPath, postfix);
		autos::IITPinsertChannels(guyPath, badFiles);
		exit(0);
	}
#endif

#if 0
	/// IITP
	QStringList guyList{
		"Alex",		//
		"Aliev",	//
		"Aliev2",	//
		"Boris",	//
		"Dima",		//
		"Egor",		//
		"Galya",	//
		"Ira",		//
		"Isakov",	//
		"Levik",	//
		"Oleg",		//
		"Victor"	//
	};
	QStringList vrList{
		"Aliev2"
//		"Dima2",
//		"Ira2",
//		"Victor2"
	};

	return;

	QString guy = "Michael";
//	for(QString guy : guyList)
//	for(QString guy : vrList)
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
			}
		}
		if(0)
		{
//			autos::IITPremoveZchans(guy, def::iitpFolder);			/// rewrites _eeg.edf
//			autos::IITPdatToEdf(guy);								/// all dat files in dir
//			autos::IITPfilter(guy, "_emg", true, true, false);		/// rewrites all _emg.edf
			if(!autos::IITPtestInitialFiles(guy))
			{
				exit(0);
			}
			return; /// clean init eeg - zero in the beginning for better filering
		}

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
//		return;
		if(01)
		{
			std::cout << "staging start" << std::endl;
			autos::IITPstaging(guy);							/// flex/extend markers
			std::cout << "staging end, copy start" << std::endl;
			autos::IITPcopyToCar(guy);							/// copy ALL *_stag.edf to guy_car
			std::cout << "copy end, reref start" << std::endl;
			autos::IITPrerefCAR(guy);							/// rewrite ALL edfs in SYNCED/guy_car
			std::cout << "copy end, process start" << std::endl;
			autos::IITPprocessStaged(guy);						/// both -Ref and -car
			std::cout << "process end, draw start" << std::endl;
			autos::IITPdrawSpectralMaps(guy);					/// both -Ref and -car
			std::cout << "all end" << std::endl;
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
	exit(0);
#endif

#if 0
	/// defs Singleton test
	{
		auto & S = def::inst();

		std::cout << S.getDirPath() << std::endl;
		S.setDir("/media/Files");
		std::cout << S.getDirPath() << std::endl;

		exit(0);
	}
#endif

#if 0
	/// check ratio crossSpectrum my and matlab
	using TT = std::valarray<double>;

	TT c1 = myLib::readFileInLine("/media/Files/Data/cSpec12.txt");

	TT m1 = myLib::readFileInLineRaw("/media/Files/Data/mSpec12.txt");

//	smLib::resizeValar(c1, m1.size());

	std::cout << std::valarray<double>(c1 / m1) << std::endl;

//	myLib::histogram(c1 / m1, 50, "/media/Files/Data/hist.jpg"
////					 , std::make_pair(0, 35)
//					 );
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
//	myLib::histogram(rat, 20, "/media/Files/Data/rat.jpg"
//					 , std::make_pair(5,20)
//					 );
//	autos::IITPdrawCoh(fromMatlab, m, confidence).
//			save("/media/Files/Data/mCoh.jpg", 0, 100);

	autos::IITPdrawCoh(usual,
					   8.,
					   45.,
					   srate,
					   fftLen,
					   0.,
					   confidence).
			save("/media/Files/Data/cCohUsual.jpg", 0, 100);



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
		pnt.drawLine( pic.width() * i / double(siz),
					  pic.height() * (1. - arr[i]),
					  pic.width() * (i + 1) / double(siz),
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
						 pnt.drawLine( pic.width() * i / double(siz),
									   pic.height() * (1. - arr[i]),
									   pic.width() * (i + 1) / double(siz),
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
					 pic.save("/media/Files/Data/cCoh" + nam + ".jpg", 0, 100);

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
	pic.save("/media/Files/Data/cCoh" + nam + ".jpg", 0, 100);

	exit(0);
#endif

#if 0
	/// Dasha ToTable processing for Galya's paper on tactile spaces
//	myLib::areEqualFiles("/media/Files/Data/Dasha/Totable/Berlin_brush.edf",
//						 "/media/Files/Data/Dasha/Totable/Berlin/Berlin_brush.edf");
//	exit(0);

	DEFS.setNtFlag(true); /// for Dasha's and EEGMRI
//	return;

//	const QString pth = DEFS.dirPath() + "/Dasha/Totable_best";
//	const QString cut = DEFS.dirPath() + "/Dasha/Totable_best_cut";

	DEFS.setNtFlag(false);
	const QString pth{};
	const QString cut = "/media/Files/Data/Galya/SomeData9Nov17";

	if(01)
	{
		/// by letter 30-31 Oct 17


		/// Cut 10 sec
		if(0)
		{
			for(auto str : QDir(pth).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
			{
				autos::GalyaCut(pth + "/" + str,  10, cut);
			}
			exit(0);
		}
		/// process cut
		if(01)
		{
			DEFS.setAutosUser(autosUser::Galya);
			autos::ProcessAllInOneFolder(cut);
			exit(0);
		}
		/// labels
		if(0) /// labels
		{
			std::ofstream labStr;

			labStr.open((cut + "/out/alpha_labels.txt").toStdString());
			for(QString lab : coords::lbl19)
			{
				labStr << "alpha_" << lab << "\t";
			}
			labStr.close();

			labStr.open((cut + "/out/fracDim_labels.txt").toStdString());
			for(QString lab : coords::lbl19)
			{
				labStr << "FD_" << lab << "\t";
			}
			labStr.close();

			labStr.open((cut + "/out/spectre_labels.txt").toStdString());
			for(int lf = 2; lf <= 19; ++lf)
			{
				for(QString lab : coords::lbl19)
				{
					labStr << "FFT_" << lf << "_" << lf+1 << "_" << lab << "\t";
				}
			}
			labStr.close();

			labStr.open((cut + "/out/Hilbert_labels.txt").toStdString());
			for(QString func : {"_carr_", "_sd_"})
			{
				for(QString lab : coords::lbl19)
				{
					labStr << "Hilb" << func << lab << "\t";
				}
			}
			labStr.close();

			exit(0);
		}
	}



	for(auto str : QDir(pth).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
		if(0) /// delete old, remain new
		{
			for(const QString fil : QDir(pth + "/" + str).entryList(def::edfFilters))
			{
				if(fil.contains("_new.edf")) { continue; }

				const QString fileName = pth + "/" + str + "/" + fil;

				QString filC = fil;
				filC.replace(".edf", "_new.edf");
				const QString fileNameNew = pth + "/" + str + "/" + filC;

				if(QFile::exists(fileNameNew))
				{
					QFile::remove(fileName);
					QFile::rename(fileNameNew, fileName);
				}
			}
		}
		if(0) /// delete all but edfs
		{
			/// dirs
			for(const QString dr : QDir(pth + "/" + str).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
			{
				QDir(pth + "/" + str + "/" + dr).removeRecursively();
			}
			/// files
			for(const QString fl : QDir(pth + "/" + str).entryList(QDir::Files))
			{
				if(!fl.contains(".edf", Qt::CaseInsensitive))
				{
					QFile::remove(pth + "/" + str + "/" + fl);
				}
			}
		}

//		repair::channelsOrderDir(pth + "/" + str,
//								 pth + "/" + str,
//								 coords::lbl31_good);
//		repair::testChannelsOrderConsistency(pth + "/" + str);
//		deleteSpaces(def::DashaFolder + "/" + str);
	}
//	DEFS.setAutosUser(autosUser::Galya);
//	autos::ProcessByFolders(pth);
//	return;
	if(01) /// labels
	{
		std::ofstream labStr;
		labStr.open((pth + "_out/labels.txt").toStdString());
		const std::vector<QString> markers{"brush", "cry", "fire", "flower",
										   "isopropanol", "needles", "vanilla", "wc"};

		for(QString mrk : markers)
		{
			for(QString lab : coords::lbl19)
			{
				labStr << mrk << "_FD_" << lab << "\t";
			}
		}
		labStr.close();
	}

	exit(0);
#endif

#if 0
	/// check new butter:: filters
//	const QString prePath = "/media/Files/Data/FeedbackFinal/Burtcev/BAV_1_car";
//	double lF = 3.5;
//	double rF = 12.;
//	for(QString postPath : {"", "_but", "_dsp"})
//	{
//		edfFile fil;
//		fil.readEdfFile(prePath + postPath + ".edf");
//		if(0)
//		{
//			myLib::refilter = &butter::refilter;
//			fil.refilter(lF, rF, true).writeEdfFile(prePath + "_but" + ".edf");
//			myLib::refilter = &myDsp::refilter;
//			fil.refilter(lF, rF, true).writeEdfFile(prePath + "_dsp" + ".edf");
//			break;
//		}
//		myLib::drw::drawOneSpectrum(smLib::valarSubsec(fil.getData(9), 10000, 10000 + 8192),
//									lF-0.5, rF + 0.5, 250., 5).save(prePath + postPath + ".jpg");
//	}
//	exit(0);
#endif

#if 0
	/// many Higuchi tests
	edfFile fil;
//	fil.readEdfFile("/media/Files/Data/AAX/AAX_final.edf");
//	fil.writeEdfFile("/media/Files/Data/AAX/AAX_final_str.edf");
	exit(0);

	auto sig1 = smLib::valarSubsec(fil.getData(9),
								   250 * 60 * 15,
								   250 * 60 * 15 + 250 * 40);
	auto sig2 = myLib::refilter(
					sig1,
					1, 30,
					false, 250.);



	if(0)
	{
		const int K = 250 * 60 * 5;
		std::valarray<double> sig3(K);

		auto getAmpl = std::bind(std::uniform_real_distribution<>(1., 10.),
								 std::default_random_engine{});
		auto getFreq = std::bind(std::uniform_real_distribution<>(0.5, 80.),
								 std::default_random_engine{});
		auto getPhase = std::bind(std::uniform_real_distribution<>(0., M_PI),
								  std::default_random_engine{});
		for(int i = 0; i < 100; ++i)
		{
			sig3 += getAmpl() * myLib::makeSine(K, getFreq(), 250., getPhase());
		}
	}



	/// from Higichi 1998 FD = 1.5
	int N = pow(2, 17);
	std::valarray<double> Y(N);
	/// count
	if(0)
	{
		std::ofstream ostr("/media/Files/Data/Y2.txt");
		auto getNum = std::bind(std::normal_distribution<double>(0., 1.),
								std::default_random_engine{});

		for(int i = 0; i < 0; ++i)
		{
			getNum();
		}

		for(int i = 1; i < N; ++i)
		{
			if( (i - 1) % int(pow(2, 12)) == 0)
			{
				std::cout << (i - 1) / pow(2, 12) << std::endl;
			}
			Y[i] = Y[i-1] + getNum();
			ostr << Y[i] << "\n";
		}
		ostr.flush();
		ostr.close();
	}
	/// read
	if(0)
	{
		std::ifstream istr("/media/Files/Data/Y2.txt");
		for(int i = 0; i < N; ++i)
		{
			istr >> Y[i];
		}
		istr.close();
	}

	if(0)
	{
		myLib::drw::drawOneSignal(
					smLib::valarSubsec(Y, 0, 1000)).save("/media/Files/Data/Y.jpg");
	}

	if(0)
	{
		std::cout << myLib::fractalDimension(sig2,
											 6,
											 "/media/Files/Data/AAX_FD.jpg")
				  << std::endl;
	}
	if(1)
	{
		/// from poli... 2010, Weierstrass cosines
		const double lambda = 5.;
		const int M = 26;
//		const int LEN = 1600;
		for(int LEN : {100, 300, 500, 1000, 1500, 1800, 2500})
		{
			std::cout << LEN << std::endl;

			std::valarray<double> powLam1(M+1);
			std::valarray<double> powLam2(M+1);
			for(int i = 0; i <= M; ++i)
			{
				powLam1[i] = pow(lambda, i);
			}

			std::valarray<double> FDarr(9);
			std::iota(std::begin(FDarr), std::end(FDarr), 11);
			FDarr /= 10.;

			for(int rightLim = 3; rightLim < 20; ++rightLim)
			{
				double MSE = 0.;
				for(double FD : FDarr)
				{
					const double H = 2. - FD;

					for(int i = 0; i <= M; ++i)
					{
						powLam2[i] = std::pow(lambda, -i * H);
					}

					std::valarray<double> res(LEN);
					std::valarray<double> tmp(LEN);
					for(int i = 0; i <= M; ++i)
					{
						const double a = 2. * M_PI * powLam1[i];
						for(int j = 0; j < LEN; ++j)
						{
							tmp[j] = cos(a * j / LEN);
						}
						res += tmp * powLam2[i];
					}
					myLib::drw::drawOneSignal(res)
							;
//					.save("/media/Files/Data/FD/Weier_" + nm(FD) + ".jpg");
					QString picPath = QString("/media/Files/Data/FD/Weier")
									  + "_" + nm(rightLim)
									  + "_" + nm(FD)
									  + ".jpg";

					double FDest = myLib::fractalDimension(res, rightLim
//														   , picPath
														   );
					MSE += std::pow(FDest - FD, 2.);
				}
				MSE /= FDarr.size();
				std::cout << rightLim - 1 << "\t" << MSE << std::endl;
			}
		}
	}
#endif

#if 0
	/// Xenia final count
	DEFS.setAutosUser(autosUser::Xenia)
	autos::Xenia_TBI_final(def::XeniaFolder + "/FINAL");
	exit(0);

	/// Xenia FINAL labels
	std::cout << myLib::countSymbolsInFile("/media/Files/Data/Xenia/FINAL_out/labels1.txt",
										   '\t') << std::endl;
	exit(0);

	const QString sep{"\t"};
//	const QString sep{"\r\n"};

	std::ofstream lab;
	lab.open((def::XeniaFolder + "/FINAL_out/labels1_6.txt").toStdString()
//			 , std::ios_base::app
			 );

	std::vector<QString> labels1 = coords::lbl19;
	for(QString & in : labels1)
	{
		in = in.toLower();
	}

	const QString initFreq = "_1.6-30";

//	for(QString mark : {"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"})
	for(QString mark : {"no", "kh", "sm", "cr", "bw", "bd", "fon"})
	{
		/// FFT
		for(int i = 2; i < 20; ++i)
		{
			for(QString lbl : labels1)
			{
				lab << mark
					<< "_" << "fft"
					<< "_" << nm(i)
					<< "-" << nm(i+1)
					<< "_"  << lbl << sep;
			}
		}

		/// CHAOS
		for(QString fir : {
			QString("fd")		+ initFreq,
			QString("hilbcarr")	+ initFreq,
			QString("hilbsd")	+ initFreq,
			QString("hilbcarr")	+ "_4-6",
			QString("hilbsd")	+ "_4-6",
			QString("hilbcarr")	+ "_8-13",
			QString("hilbsd")	+ "_8-13"
	}
			)
		{
			for(QString lbl : labels1)
			{
				lab << mark
					<< "_" << fir
					<< "_"
					<< lbl << sep;
			}
		}

		/// WAVELET
		for(QString fir : {"wavmean", "wavmed", "wavsgm"})
		{
			for(int i = 2; i <= 20; ++i)
			{
				for(QString lbl : labels1)
				{
					lab << mark
						<< "_" << fir
						<< "_" << nm(i)
						<< "-" << nm(i+1)
						<< "_" << lbl << sep;
				}
			}
		}

		/// ALPHA
		for(QString lbl : labels1)
		{
			lab << mark
				<< "_" << "alpha"
				<< initFreq
				<< "_" << lbl << sep;
		}
	}
	lab.close();
	exit(0);

	/// count
	autos::Xenia_TBI_final(def::XeniaFolder + "/FINAL");
	myLib::invertMatrixFile("/media/Files/Data/Xenia/FINAL_out/labels2.txt",
							"/media/Files/Data/Xenia/FINAL_out/labels2_inv.txt");
	exit(0);

	/// check files sizes
	QString ss = "/media/Files/Data/Xenia/FINAL/";
	std::vector<QString> drs{"Healthy", "Moderate", "Severe"};
	qint64 s = 10000000;
	for( auto dr : drs)
	{
		QStringList in = QDir(ss + dr).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(auto g : in)
		{
			auto in = QDir(ss + dr + "/" + g).entryInfoList(def::edfFilters);
			for(auto i : in)
			{
				s = std::min(s, i.size());
				if(i.size() == 427936) std::cout << dr << " " << g << std::endl;
			}
		}
	}
	std::cout << s << std::endl;
	exit(0);
#endif

#if 0
	/// test draw windows
	int counter = 0;
	for(myLib::windowName wind : {
		myLib::windowName::Blackman,
		myLib::windowName::BlackmanHarris,
		myLib::windowName::BlackmanNuttal,
		myLib::windowName::FlatTop,
		myLib::windowName::Gaussian,
		myLib::windowName::Lanczos,
		myLib::windowName::Nuttal,
		myLib::windowName::Parzen,
		myLib::windowName::RifeVincent,
		myLib::windowName::sine,
		myLib::windowName::Welch
})
	{
		myLib::drw::drawOneGraph(myLib::fftWindow(1024, wind),
								 1024).save("/media/Files/Data/wind_" + nm(counter++) + ".jpg");
	}
	exit(0);
#endif

#if 0
	/// Galya processing things
	/// 15Mar18

	const QString workPath = def::GalyaFolder + "/15Mar18";
	const std::vector<QString> usedMarkers{"_bd", "_bw", "_cr", "_kh", "_na", "_no",
										   "_og", "_rv", "_sm", "_zg"};

	if(0)
	{
		/// delete old versions, rename _new files
		QStringList lst = QDir(workPath).entryList({"*_new.edf"}, QDir::Files);
		for(const QString & newName : lst)
		{
			QString oldName = newName;
			oldName.replace("_new.edf", ".edf");
			QFile::remove(workPath + "/" + oldName);
			QFile::rename(workPath + "/" + newName,
						  workPath + "/" + oldName);
		}
		exit(0);
	}

	if(0)
	{
		/// initial repair and to folders
//		for(QString add : {"young", "adults"})
		{
//			repair::toLatinDir(workPath);
//			repair::deleteSpacesDir(workPath);
//			repair::toLowerDir(workPath);
			/// some special names cleaning
			for(QString fileName : QDir(workPath).entryList(def::edfFilters))
			{
//				QString newName = fileName;
//				newName.replace(".edf", "_rest.edf", Qt::CaseInsensitive);
//				newName.replace("_DL_", "_", Qt::CaseInsensitive);
//				newName.replace("_dl.", ".", Qt::CaseInsensitive);
//				newName.replace("_PR.", ".", Qt::CaseInsensitive);
//				newName.replace("_rest_", "_", Qt::CaseInsensitive);
//				newName.replace("_rest.", ".", Qt::CaseInsensitive);
//				newName.replace("FON_", "", Qt::CaseInsensitive);

//				QFile::rename(workPath + "/" + fileName,
//							  workPath + "/" + newName);
			}

			autos::EdfsToFolders(workPath);

		}
		exit(0);
	}

	if(0)
	{
		/// checks and corrects channels order consistency
		QString str19;
		for(int i = 0; i < 19; ++i)
		{
			str19 += nm(i + 1) + " ";
		}
//		for(QString add : {"young", "adults"})
//		{
			const QString tact = workPath;
			auto dirList = QDir(tact).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			for(QString dr : dirList) /// each guy
			{
				const QString pt = tact + "/" + dr;
				auto filList = QDir(pt).entryList(def::edfFilters); /// edfs of one guy
				for(QString fl : filList)
				{
					edfFile file;
					file.readEdfFile(pt + "/" + fl, true);
					QString helpString{};
					for(int i = 0; i < 19; ++i)
					{
						helpString += nm(file.findChannel(coords::lbl19[i]) + 1) + " ";
					}
					if(helpString != str19)
					{
						std::cout << fl << std::endl;
						/// rewrite file with correct chan order - read with data
//						file.reduceChannels(helpString).writeEdfFile(tact + "/" + dr + "/" + fl);
					}
				}
			}
//		}
		exit(0);
	}

	if(0)
	{
		/// compose a list of markers
		std::set<QString> markers{};
		for(const QString & fn : QDir(workPath).entryList(def::edfFilters))
		{
			int a = fn.lastIndexOf('_');
			int b = fn.lastIndexOf('.');
			QString mrk = fn.mid(a + 1, b - a - 1);
			markers.emplace(mrk);
		}
		for(const auto & mrk : markers)
		{
//			std::cout << R"(")" << mrk << R"(", )";
			std::cout << mrk << " ";
		}
		std::cout << std::endl;
		exit(0);
	}
	if(0)
	{
		/// calculation itself
		DEFS.setAutosUser(autosUser::Galya);
//		autos::ProcessAllInOneFolder(workPath);

		autos::ProcessByFolders(workPath, usedMarkers);
		exit(0);
	}
	if(0)
	{
		/// test table rows lengths
		QFile feel(workPath + "_out/all.txt");
		feel.open(QIODevice::ReadOnly);
		while(!feel.atEnd())
		{
			std::cout << QString(feel.readLine()).count('\t') << std::endl;
		}
		feel.close();
		exit(0);
	}

	/// labels part

	if(01)
	{
		if(01)
		{
			/// cout lengths of all txts
			for(QString fn : QDir(workPath + "_out").entryList({"*.txt"}))
			{
				std::cout << fn << "\t"
						  << myLib::countSymbolsInFile(workPath + "_out" + "/" + fn, '\t') << std::endl;
			}
		}

		std::cout << "labels.txt" << "\t"
				  << myLib::countSymbolsInFile(workPath + "_out" + "/" + "labels.txt", '\t') << std::endl;
		exit(0);
	}

	if(0)
	{
		///make labels file

		const QString sep{"\t"};
//		const QString sep{"\r\n"};

		std::ofstream lab;
		lab.open((workPath + "_out" + "/labels.txt").toStdString());

		std::vector<QString> labels1 = coords::lbl19;
		for(QString & in : labels1) { in = in.toLower(); }

		const QString initFreq = "_1.6_30";

		for(QString mark : usedMarkers)
		{
			mark.remove('_');

			/// FFT
			/// 18 ranges 1-Hz-wide, 19 channels = 342 values
			for(int i = 2; i < 20; ++i)
			{
				for(QString lbl : labels1)
				{
					lab << mark
						<< "_" << "fft"
						<< "_" << nm(i)
						<< "_" << nm(i+1)
						<< "_"  << lbl << sep;
				}
			}

			/// ALPHA
			for(QString lbl : labels1) /// 19 values
			{
				lab << mark
					<< "_" << "alpha"
					<< initFreq
					<< "_" << lbl << sep;
			}

			/// Hilbert and Fractal Dimension 5*19 = 95 values
			for(QString fir : {
				QString("fd")		+ initFreq,
				QString("hilbcarr")	+ initFreq,
				QString("hilbcarr")	+ "_8_13",
				QString("hilbsd")	+ initFreq,
				QString("hilbsd")	+ "_8_13"
		}
				)
			{
				for(QString lbl : labels1)
				{
					lab << mark
						<< "_" << fir
						<< "_"
						<< lbl << sep;
				}
			}

			/// HJORTH
			/// 38 values
			for(QString fir : {"hjmob", "hjcom"})
			{
				for(QString lbl : labels1)
				{
					lab << mark
						<< "_" << fir
						<< initFreq
						<< "_" << lbl << sep;
				}

			}

#if 0
			/// WAVELET
			/// 19*19 = 361 values
			for(int i = 0; i < 19; ++i)
			{
				for(QString lbl : labels1)
				{
					lab << mark
						<< "_" << nm(i + 2)
						<< "_" << lbl << sep;
				}
			}
#endif


			/// logFFT
			/// 18*19 = 342 values
			for(int i = 2; i < 20; ++i)
			{
				for(QString lbl : labels1)
				{
					lab << mark
						<< "_" << "logfft"
						<< "_" << nm(i)
						<< "_" << nm(i+1)
						<< "_"  << lbl << sep;
				}
			}
		}
		lab.close();
		exit(0);
	}
	exit(0);
#endif

#if 0
	/// Xenia test 30 sec, 15+15 sec
	const QString pth = "/media/Files/Data/Xenia/1408/";
	const int Kmax = 8;
	QStringList lest = QDir(pth).entryList(def::edfFilters);
	std::ofstream outStr;
	outStr.open((pth + "res.txt").toStdString());

	for(QString fileNam : lest)
	{
		edfFile filee;
		filee.readEdfFile(pth + fileNam);
		filee.refilter(1.6, 30.);

		matrix subData = filee.getData().subCols(0, 250 * 30);
		matrix subData1 = filee.getData().subCols(0, 250 * 15);
		matrix subData2 = filee.getData().subCols(250 * 15, 250 * 30);
		outStr << fileNam << std::endl;
		outStr << std::fixed;
		outStr.precision(3);

		double av0 = 0.;
		double av = 0.;
		double av1 = 0.;
		double av2 = 0.;
		for(int i = 0; i < 19; ++i)
		{
			double z = myLib::fractalDimension(filee.getData(i), Kmax
//											   , pth + "pics/" + filee.getExpName() + "_" + nm(i) + "_0.jpg"
											   );
			double a = myLib::fractalDimension(subData[i], Kmax
//											   , pth + "pics/" + filee.getExpName() + "_" + nm(i) + ".jpg"
											   );
			double b = myLib::fractalDimension(subData1[i], Kmax
//											   , pth + "pics/" + filee.getExpName() + "_" + nm(i) + "_1.jpg"
											   );
			double c = myLib::fractalDimension(subData2[i], Kmax
//											   , pth + "pics/" + filee.getExpName() + "_" + nm(i) + "_2.jpg"
											   );
			outStr << z << "\t"
				   << a << "\t"
				   << b << "\t"
				   << c << "\t"
				   << std::abs(b-c) / a * 100. << "\t"
				   << std::abs(z-a) / (a + z) * 200.
				   << std::endl;
			av += a;
			av1 += b;
			av2 +=c;
			av0 += z;
		}
		outStr << av0 / 19. << "\t"
			   << av / 19. << "\t"
			   << av1 / 19. << "\t"
			   << av2 / 19. << "\t"
			   << std::endl;
	}
	exit(0);

#endif

#if 0
	/// Xenia FD tables classification
	const QString workDir = DEFS.dirPath() + "/Xenia_tables/";

	/// repair
	if(0)
	{
//		for(QString subdir : {"FD", "FFT", "SD_carr", "ampl_freq"})
		QString subdir = "ThirdVersion";
		{
			QString pewDir = workDir + subdir + "/";
			for(const QString & fileName : QDir(pewDir).entryList({"*FFT*var.txt"}))
			{
				int num = QString(fileName[ fileName.indexOf("var") - 1]).toInt();
//				std::cout << num << std::endl; continue;

				autos::Xenia_repairTable(pewDir + fileName,
										 pewDir + QString(fileName).replace(".txt", "_new.txt"),
										 pewDir + "Groups_" + nm(num) + ".txt",
										 pewDir + "Names_" + nm(num) + ".txt");
			}
		}
	}

	/// classify
	if(1)
	{

		std::ofstream res;
		res.open((workDir + "res.txt").toStdString());

		QString pewDir = workDir;
		pewDir.resize(pewDir.size() - 1);
		for(QString fileName : QDir(pewDir).entryList({"*FFT*_new*"}))
		{
			Net * ann = new Net();
			fileName.remove(".txt");
			std::cout << fileName << std::endl;
			ann->loadDataXenia(pewDir, fileName);
			ann->setClassifier(ClassifierType::ANN);
			ann->setKnnNumSlot(4);
			ann->setMode("N");
			res << fileName << "\t"
				<< ann->autoClassification().first << "\r\n";
			res.flush();
			ann->close();
		}
		res.close();
		exit(0);
	}

	exit(0);
#endif

#if 0
	/// Baklushev histograms

	int steps = 20;
	int norm = 8;
	std::valarray<double> res1;
	std::valarray<double> res2;
	res1 = readFileInLineRaw(DEFS.dirPath() + "/Baklushev/healthy.txt");
	res2 = readFileInLineRaw(DEFS.dirPath() + "/Baklushev/ill.txt");
	for(int steps = 12; steps <= 30; steps += 2)
	{
		myLib::histogram(res1, steps,
						 DEFS.dirPath() + "/Baklushev/healthy_" + nm(steps) + ".jpg",
		{50., 94.}, "blue", norm);
		myLib::histogram(res2, steps,
						 DEFS.dirPath() + "/Baklushev/ill_" + nm(steps) + ".jpg",
		{50., 94.}, "red", norm);
	}
	exit(0);
#endif

#if 0
	/// compose names for Xenia_TBI tables

	QString dirPath = def::XeniaFolder + "/15Nov";
	QStringList markers{"no", "kh", "sm", "cr", "bw", "bd", "fon"};
	std::vector<QString> waveletFuncs {"max", "min", "mean", "median", "sigma"};
	const std::vector<QString> & chans = coords::lbl19;
	std::vector<QString> filts;// = {"2-4", "4-6", "6-8", "8-10", "10-12", "12-14", "14-16", "16-18", "18-20", "all"};
	for(int i = 2; i <= 18; i += 2)
	{
		filts.push_back("_" + nm(i) + "-" + nm(i + 2));
	}
	filts.push_back("_all");

	std::vector<QString> freqs;
	for(int i = 2; i <= 20; i += 1)
	{
		freqs.push_back("_" + nm(i));
	}


	std::ofstream allFil;
	allFil.open((dirPath + "/str_all.txt").toStdString());
//	std::vector<int> count(5, 0);
	for(QString marker : markers)
	{
		std::ofstream outFil;
		outFil.open((dirPath + "/str_" + marker + ".txt").toStdString());
		/// alpha
		for(QString ch : chans)
		{
			outFil << marker + "_ampl_" + ch << '\t';
			outFil << marker + "_freq_" + ch << '\t';

			allFil << marker + "_ampl_" + ch << '\t';
			allFil << marker + "_freq_" + ch << '\t';
//			count[0] += 2;
		}
		/// d2
		for(QString filt : filts)
		{
			for(QString ch : chans)
			{
				outFil << marker + "_dd" + filt + "_" + ch << '\t';

				allFil << marker + "_dd" + filt + "_" + ch << '\t';
//				count[1] += 1;
			}
		}
		/// med_freq
		for(QString filt : filts)
		{
			for(QString ch : chans)
			{
				outFil << marker + "_carr" + filt + "_" + ch << '\t';
				outFil << marker + "_SD" + filt + "_" + ch << '\t';

				allFil << marker + "_carr" + filt + "_" + ch << '\t';
				allFil << marker + "_SD" + filt + "_" + ch << '\t';
//				count[2] += 2;
			}
		}
		/// spectre
		for(QString ch : chans)
		{
			for(QString fr : freqs)
			{
				outFil << marker + "_FFT" + fr + "_" + ch << '\t';

				allFil << marker + "_FFT" + fr + "_" + ch << '\t';
//				count[3] += 1;
			}
		}
		/// wavelet
		for(QString ch : chans)
		{
			for(QString foo : waveletFuncs)
			{
				for(QString fr : freqs)
				{
					outFil << marker + "_wav_" + foo + fr + "_" + ch << '\t';
//					count[4] += 1;

					allFil << marker + "_wav_" + foo + fr + "_" + ch << '\t';
				}
			}
		}
//		std::cout << count << std::endl;
		outFil.close();
	}
	allFil.close();

	exit(0);
#endif

#if 0
	/// successive auxiliary
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};


	ui->realsButton->setChecked(true); // sliceWindFromReal

	for(QString name : names)
	{
		DEFS.setDrawNorm(-1);
		setEdfFile(path + name + "_train.edf");
		sliceAll();
		countSpectraSimple(4096, 15);

		setEdfFile(path + name + "_test.edf");
		sliceAll();
		countSpectraSimple(4096, 15);
//		exit(0);
	}
	exit(0);

#endif

#if 0
	/// Xenia - check all files markers
	QString p = def::XeniaFolder + "/15Nov";
	QDir dr(p);
	for(QString str : {"healthy", "moderate_TBI", "severe_TBI"})
	{
		dr.cd(str);
		for(QString str2 : dr.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
		{
			dr.cd(str2);
			auto lst = dr.entryList(QDir::Files);
			for(QString marker : {"_bd", "_bw", "_cr", "_fon", "_kh", "_no", "_sm"})
			{
				bool peeew = false;
				for(auto str3 : lst)
				{
					if(str3.contains(marker + ".edf")) peeew = true;
				}
				if(!peeew)
				{
					std::cout << dr.absolutePath() << ": no " << marker << std::endl;
				}
			}


			dr.cdUp();
		}
		dr.cdUp();
	}
	exit(0);
#endif

#if 0
	/// backuping
	///
	/// find -type f -regextype sed -iregex ".*/[^/]\{3,5\}\.edf" | sed 's/\.\// \/media\/Files\/Data\/Feedback\// g'
	/// find -type f -regextype sed -iregex ".*/[^/]\{3\}\(_train\|_test\|_FB\|E\)\.edf"
	/// find -type f -regextype sed -iregex ".*/[^/]\{3\}\(_train\|_test\|_FB\|E\|_1\|_3\)\.edf"


	QDir tmp("/media/Files/Data/FeedbackNew");
//	QDir dest("/media/Files/Data/FeedbackNew/UDFs");
	QDir dest("/media/michael/Seagate Expansion Drive/Michael/Data/BACKUPS/FeedbackNew");

	for(auto deer : tmp.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
//		if(deer.startsWith("Succ")) continue;

		tmp.cd(deer);
		dest.mkdir(deer);
		dest.cd(deer);

		QStringList edfF = tmp.entryList(def::edfFilters, QDir::Files, QDir::Time | QDir::Reversed);
//		QStringList udfF = tmp.entryList({"*.UDF", "*.Hdr"}, QDir::Files);
//		std::cout << edfF << std::endl;

//		for(QString udfFin : udfF)
//		{
//			QFile::rename(tmp.absolutePath() + "/" + udfFin, dest.absolutePath() + "/" + udfFin);
//		}
		for(QString edfIN : edfF)
		{
			QFile::copy(tmp.absolutePath() + "/" + edfIN, dest.absolutePath() + "/" + edfIN);
		}

		tmp.cdUp();
		dest.cdUp();
	}
	exit(0);


//	const QString pe = "/media/Files/Data/FeedbackNew";
//	QStringList dl = QDir(pe).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
//	for(QString dr : dl)
//	{
//		for(QString fn : {"ans1.txt", "ans2.txt", "ans3.txt"})
//		{
//			QFile::copy(pe + "/" + dr + "/" + fn,
//						pe + "/Tables/" + dr + "_" + fn);
//		}
//	}
//	exit(0);
#endif

#if 0
	/// Dasha rename files totable
	QDir dr("/media/Files/Data/Dasha/Totable");
	QStringList lst = dr.entryList(def::edfFilters);
	for(QString str : lst)
	{
		if(!str.contains(".edf") && !str.contains(".EDF"))
		{
			QFile::rename(dr.absolutePath() + "/" + str,
						  dr.absolutePath() + "/" + str + ".edf");
		}

		if(str.contains("_48.edf"))
		{
			QString newName = str;
			newName = newName.left(newName.indexOf('_'));
			newName += "_wc.edf";
			QFile::rename(dr.absolutePath() + "/" + str,
						  dr.absolutePath() + "/" + newName);
		}

#if 01
		/// copy to folders
		QString newName = str;
		newName = newName.left(newName.indexOf('_'));
		if(!dr.exists(newName))
		{
			dr.mkdir(newName);
		}
		QFile::copy(dr.absolutePath() + "/" + str,
					dr.absolutePath() + "/" + newName + "/" + str);
#endif

	}
exit(0);
#endif

#if 0
	/// averaging data in results files
	const QString pth = "/media/Files/Data/Feedback/SuccessClass/Help/Succ";
//	for(QString suf :{"_z", "_noz"})
	QString suf = "_z_more";

	{
		QDir dr(pth + suf);
		QString outFilePath = "/media/Files/Data/Feedback/SuccessClass/avres.txt";

		auto fls = dr.entryList(QDir::Files);
		for(QString nam : fls)
		{
			std::vector<std::vector<double>> values(5);
			double tmp;
			std::string tmpStr;
			QString filePath = pth + suf + "/" + nam;
			ifstream inStr;
			inStr.open(filePath.toStdString());
			while(inStr.good())
			{
				for(int i = 0; i < 5; ++i)
				{
					inStr >> tmp;
					values[i].push_back(tmp);
				}
				inStr >> tmpStr;
			}
			inStr.close();

			ofstream outStr;
			outStr.open(outFilePath.toStdString(), std::ios_base::app);
			auto splitList = nam.split("_", QString::SkipEmptyParts);
			outStr << splitList[1] << '\t'
								   << splitList[2] << '\t'
								   << splitList[3].remove(".txt") << '\t';
			for(int i = 0; i < 5; ++i)
			{
				outStr << doubleRound( std::accumulate(std::begin(values[i]),
										  std::end(values[i]) - 1, 0.) / (values[i].size() - 1), 2) << '\t';
			}
			outStr << "\r\n";
			outStr.close();
		}

	}
	exit(0);
#endif

#if 0
	/// thesholding pictures
	QImage pic;
	pic.load("/media/Files/Rep/home2.png");
	const int thr = 170;
	for(int i = 0; i < pic.width(); ++i)
	{
		for(int j = 0; j < pic.height(); ++j)
		{
			QColor color = QColor(pic.pixel(i, j));

			if(color.red() > thr &&
			   color.green() > thr &&
			   color.blue() > thr)
			{
				pic.setPixelColor(i, j, QColor("white"));
			}
			else
			{
				pic.setPixelColor(i, j, QColor("black"));
			}
		}
	}
	pic.save("/media/Files/Rep/home2_.jpg", nullptr, 100);
	exit(0);
#endif

#if 0
    /// clean "new" Dasha files
	QStringList leest_audio = subj::leest_more + subj::leest_less;
	leest_audio.sort(Qt::CaseInsensitive); /// alphabet order
	const QString work = def::DashaFolder + "/CHANS/Audio_to_less_out_ALL";
    QDir deer(work);

    QStringList subdirs = deer.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    for(auto subdir : subdirs)
    {
        deer.cd(subdir);
		QString tablePath = deer.absolutePath() + "/Audio_" + subdir + ".txt";

        QFile outStr(tablePath);
        outStr.open(QIODevice::WriteOnly);

//        QStringList news = deer.entryList({"*_new_*"});
//        for(const QString & oneNew : news)
//        {
//            QString oldName = oneNew; oldName.replace("_new_", "_");

//            QFile::remove(deer.absolutePath() + "/" + oldName);
//            QFile::copy(deer.absolutePath() + "/" + oneNew,
//                        deer.absolutePath() + "/" + oldName);
//            QFile::remove(deer.absolutePath() + "/" + oneNew);
//        }
        for(const QString & guy : leest_audio)
        {
			/// should be "alpha", "d2_dim", "med_freq", "spectre"
            QStringList tryam = deer.entryList({guy + "*"});
			if(tryam[1].contains("med_freq"))
            {
                std::swap(tryam[1], tryam[2]);
            }
            for(const QString & fileName : tryam)
            {
				QFile fil(deer.absolutePath() + "/" + fileName);
                fil.open(QIODevice::ReadOnly);
                auto contents = fil.readAll();

                outStr.write(contents);
//                ifstream inStr;
//                inStr.open(deer.absolutePath() + "/" + fileName);


            }
            outStr.write("\r\n");


        }
        outStr.close();
        deer.cdUp();
//        exit(0);
    }
    exit(3);
#endif

#if 0
    /// Ossadtchi
//    setEdfFile("/media/Files/Data/Ossadtchi/lisa2/lisa2.edf");
    setEdfFile("/media/Files/Data/Ossadtchi/alex1/alex1.edf");
	DEFS.getFreq() = 100; /// epwpewpwepepwwpwpwepwewepwep
	DEFS.setNs(32);
#if 0
	// reduce channels in Reals
	DEFS.setNs(32);
    const set<int, std::less<int>> exclude{3, 4, 14};
    QString helpString;
	for(int i = 0; i < DEFS.getNs(); ++i)
    {
        if(std::find(exclude.begin(), exclude.end(), i) != exclude.end()) continue;
		helpString += nm(i + 1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);
    reduceChannelsSlot();
#endif
//    return;



	DEFS.setNs(29);

    std::vector<pair<int, double>> pew;
    for(int i : {17, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20})
    {
        countSpectraSimple(1024, i);

        Net * net = new Net();

		net->autoClassification(DEFS.dirPath()
								+ "/SpectraSmooth"
								+ "/winds");

        pew.push_back(make_pair(i, net->getAverageAccuracy()));
        delete ann;

        break;
    }
    for(auto out : pew)
    {
		std::cout << out.first << "\t" << out.second << std::endl;
    }
    exit(0);
    return;
#endif



#if 0
    /// uncode matlab color scale
    QImage img;
    img.load("/media/Files/Data/matlab.png");
    QRgb color1 = img.pixel(img.width()/2, img.height() - 1);
    QRgb color2;

    QPixmap pic(img.height(), img.height() * 0.7);
    pic.fill();
    QPainter pnt;
    pnt.begin(&pic);


    for(int i = 2; i < img.height(); ++i)
    {
        color2 = img.pixel(img.width()/2, img.height() - i);

        pnt.setPen("red");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qRed(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qRed(color2) / 255.));

        pnt.setPen("green");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qGreen(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qGreen(color2) / 255.));

        pnt.setPen("blue");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qBlue(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qBlue(color2) / 255.));

        pnt.setPen(QColor(color1));
        pnt.setBrush(QBrush(QColor(color1)));
        pnt.drawRect(i-1, 0,
                     1, 0.07 * pic.height());
        color1 = color2;
    }
    pnt.end();
    pic.save("/media/Files/Data/matlabOut.jpg", 0, 100);

    exit(0);
#endif

#if 0
    /// read ica data from matlab
    const QString init = "/media/Files/Data/Mati/ICAstudy/";
    const QString pth = "/media/Files/Data/Mati/ICAstudy/Help/";
    QString helpString;
	QDir locDir(pth);
	QStringList dataFiles = locDir.entryList(QStringList("*_ica.txt"));

    mat dataMat;
    dataMat.resize(20);



    for(QString &oneFile : dataFiles)
    {

        for(int i = 0; i < 20; ++i)
        {
            dataMat[i].resize(60 * 60 * 250); // 1 hour
        }

        helpString = pth + oneFile;
		dataMat = myLib::readPlainData(helpString);
		std::cout << dataMat.cols() << std::endl;

        for(int i = 0; i < 20; ++i)
        {
			dataMat[i].resize(dataMat.cols()); // 1 hour
        }

        // add markers
        helpString = oneFile;
        helpString.replace("_ica.txt", ".edf");
        helpString = init + helpString;
		std::cout << "initFile = " << helpString << std::endl;

        globalEdf.readEdfFile(helpString);
        dataMat[19] = globalEdf.getData()[globalEdf.getMarkChan()];

        helpString = oneFile;
        helpString.replace(".txt", ".edf");
        helpString = init + helpString;
		std::cout << "outFile = " << helpString << std::endl;
        globalEdf.writeOtherData(dataMat, helpString);
    }

    exit(0);
#endif

#if 0
	/// concat all mati sessions
	QDir locDir(def::matiFolder);
	QStringList dirLst = locDir.entryList(QStringList("???"), QDir::Dirs|QDir::NoDotAndDotDot);
    for(QString & guy : dirLst)
    {
		locDir.cd(guy);
		locDir.cd("auxEdfs");

		QString helpString = locDir.absolutePath() + "/" + guy + "_0.edf";
        if(!QFile::exists(helpString))
        {
			locDir.cdUp();
			locDir.cdUp();
            continue;
        }
        edfFile initFile;
        initFile.readEdfFile(helpString);
        helpString.replace("_0.edf", "_1.edf");
        initFile.concatFile(helpString);
        helpString.replace("_1.edf", "_2.edf");
        initFile.concatFile(helpString);

		locDir.cdUp();
		QString helpString2 = locDir.absolutePath() + "/" + guy + "_full.edf";
        initFile.writeEdfFile(helpString2);
		locDir.cdUp();
    }
    exit(0);
#endif

#if 0
	/// MATI
    if(1)
    {
		/// deprecated, use edfFile.concat
//        concatenateEDFs("/media/Files/IHNA/Data/MATI/archive/NOS_1.EDF",
//                        "/media/Files/IHNA/Data/MATI/archive/NOS_2.EDF",
//                        "/media/Files/IHNA/Data/MATI/archive/NOS.EDF");
        exit(0);

//        QString helpString1 = "/media/Files/Data/Mati/PYV/PY_3.edf";
//        std::cout << fileNameOf(helpString1) << std::endl;


//        ui->matiCheckBox->setChecked(true);
//        ui->sliceCheckBox->setChecked(true);
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

		matiFixMarker(firstMarker); // fix the start marker for this small edf file
		matiFixMarker(lastMarker);  // fix the last  marker for this small edf file
		if(matiCountBit(firstMarker, 10) != matiCountBit(lastMarker, 10)) // if not one of them is the end of some session
        {
            lastMarker = firstMarker
				+ pow(2, 10) * (matiCountBit(firstMarker, 10) ? -1 : 1); // adjust the last marker
        }
        matiPrintMarker(lastMarker, "last");
        matiPrintMarker(doub2, "newData");
        exit(0);
    }

#endif
}
