#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/drw.h>
#include <myLib/iitp.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/temp.h>

using namespace myOut;

void MainWindow::customFunc()
{
    ui->matiCheckBox->setChecked(false);
	ui->realsButton->setChecked(true);
#if ELENA_VARIANT
	ui->elenaSliceCheckBox->setChecked(true); // Elena
	ui->eogBipolarCheckBox->setChecked(true); // Elena
#endif

	/// count correctness and average times
//	QString guy = "IAE";
//	auto name = [guy](int i)
//	{
//		return def::dataFolder + "/FeedbackFinal/"
//				+ guy + "/"
//				+ guy + "_" + nm(i) + "_fin.edf";
//	};
//	for(int i : {1, 2, 3})
//	{
//		autos::timesNew(name(i), i);
//		autos::avTimesNew(name(i), i);
//	}
//	exit(0);

//	edfFile file1;
//	file1.readEdfFile("/media/Files/Data/FeedbackFinal/Ilyin/IAE_3_1.edf");
//	file1.concatFile("/media/Files/Data/FeedbackFinal/Ilyin/IAE_3_2.edf",
//					 "/media/Files/Data/FeedbackFinal/Ilyin/IAE_3.edf");
//	exit(0);





//	def::currAutosUser = def::autosUser::Xenia;
//	autos::Xenia_TBI_final(def::XeniaFolder + "/FINAL");

//	QString str19;
//	for(int i = 0; i < 19; ++i)
//	{
//		str19 += nm(i + 1) + " ";
//	}

//	const QString tact = "/media/Files/Data/Galya/AllTactile_backup";
//	auto dirList = QDir(tact).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
//	for(QString dr : dirList)
//	{
//		auto filList = QDir(tact + "/" + dr).entryList(def::edfFilters);
//		for(QString fl : filList)
//		{


//			edfFile file;
//			file.readEdfFile(tact + "/" + dr + "/" + fl, true);
//			QString helpString{};
//			for(int i = 0; i < 19; ++i)
//			{
//				helpString += nm(file.findChannel(coords::lbl19[i]) + 1) + " ";
//			}
//			if(helpString != str19)
//			{
//				file.reduceChannels(helpString).writeEdfFile(tact + "/" + dr + "/" + fl);
//			}
//		}
//	}
//	exit(0);

//	auto txtLst = QDir("/media/Files/Data/Galya/AllTactile_out/").entryList({"*.txt"});
//	for(QString txt : txtLst)
//	{
//		std::cout << myLib::countSymbolsInFile("/media/Files/Data/Galya/AllTactile_out/" + txt,
//								  '\t') << std::endl;
//	}
//	exit(0);

//	def::currAutosUser = def::autosUser::Galya;
//	autos::Galya_tactile("/media/Files/Data/Galya/AllTactile");
//	exit(0);



//	return;

#if 0
	/// IITP
	QStringList guyList{
//		"Alex",
//		"Boris",
		"Ira",
		"Levik",
		"Oleg",
		"Egor",
		"Victor",
		"Dima",
		"Isakov",
		"Galya"
	};

//	for(QString guy : guyList)
	QString guy = "Test";
//	QString guy = "Galya_test_new";
	{
//		autos::IITPdatToEdf(guy);
//		autos::IITPremoveZchans(guy, def::iitpFolder);
//		autos::IITPrerefCAR(guy);
//		return; /// clean init eeg - zero in the beginning
//		autos::IITPconcat(guy);
//		return; /// manual sync

//		/// copy files to SYNCED

		autos::IITPemgToAbs(guy);
//		autos::IITPstaging(guy);	/// flex/extend
//		autos::IITPfilter(guy);		/// optional
//		autos::IITPtestCoh(guy);
		autos::IITPtestCoh2(guy);
//		autos::IITPprocessStaged(guy);
//		autos::IITPdrawSpectralMaps(guy);

//		continue;
		exit(0);

		if(guy == "Alex")
		{
			myLib::drw::zeroChans = {};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14, 15};
		}
		else if(guy == "Boris")
		{
			myLib::drw::zeroChans = {0, 1, 5, 7, 15};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14};
		}
		else if(guy == "Ira")
		{
			myLib::drw::zeroChans = {0, 1};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14, 15};
		}
		else if(guy == "Levik")
		{
			myLib::drw::zeroChans = {0, 1};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14, 15};
		}
		else if(guy == "Oleg")
		{
			myLib::drw::zeroChans = {5, 15, 16, 18};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14};
		}
		else if(guy == "Victor")
		{
			myLib::drw::zeroChans = {};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14, 15};
		}
		else if(guy == "Dima")
		{
			myLib::drw::zeroChans = {};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14};
		}
		else if(guy == "Isakov")
		{
			myLib::drw::zeroChans = {};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14};
		}
		else if(guy == "Galya")
		{
			myLib::drw::zeroChans = {};
			myLib::drw::trueChans = {7, 8, 9, 10, 11, 13, 14};
		}

		using Typ = std::vector<int>;
		Typ nums = smLib::unite<Typ>({smLib::range<Typ>(0, 11 + 1),
									  smLib::range<Typ>(24, 29 + 1)});
//		nums.erase(std::find(std::begin(nums), std::end(nums), 8));
		autos::IITPdrawSameScale(guy, nums);
		exit(0);
	}



//	autos::IITPprocessStaged("Victor",
//							 "_sum_f_new_gon_stag");
//	autos::IITPfilterGonios("Oleg",
//							"_emg_f");
	exit(0);
//	return;
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
	/// Galya tactile labels
	std::cout << myLib::countSymbolsInFile("/media/Files/Data/Galya/AllTactile_out/Alexey_Coma.txt",
										   '\t') << std::endl;
	std::cout << myLib::countSymbolsInFile("/media/Files/Data/Galya/AllTactile_out/labels.txt",
										   '\t') << std::endl;
	exit(0);

	const QString sep{"\t"};
//	const QString sep{"\r\n"};

	std::ofstream lab;
	lab.open((def::GalyaFolder + "/AllTactile_out/labels.txt").toStdString());

	std::vector<QString> labels1 = coords::lbl19;
	for(QString & in : labels1)
	{
		in = in.toLower();
	}

	const QString initFreq = "_1_30";

	for(QString mark : {"buk", "kis", "rol", "sch", "og", "zg"})
	{
		/// ALPHA
		for(QString lbl : labels1)
		{
			lab << mark
				<< "_" << "alpha"
				<< initFreq
				<< "_" << lbl << sep;
		}

		/// FFT
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

		/// CHAOS
		for(QString fir : {
			QString("hilbcarr")	+ initFreq,
			QString("hilbcarr")	+ "_8_13",
			QString("hilbsd")	+ initFreq,
			QString("hilbsd")	+ "_8_13",
			QString("fd")		+ initFreq
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
		for(QString fir : {"hjorthcom", "hjorthmob"})
		{
			for(QString lbl : labels1)
			{
				lab << mark
					<< "_" << fir
					<< initFreq
					<< "_" << lbl << sep;
			}

		}
	}
	lab.close();
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
	const QString workDir = def::dataFolder + "/Xenia_tables/";

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
	/// draw Wts from a folder
	const QString inDir = "/media/Files/Data/FeedbackTest/GA_FB/GA_FB_weights";
	ANN * net = new ANN();
	ClassifierData cl = ClassifierData("/media/Files/Data/FeedbackTest/GA_FB/winds/fromreal");
	net->setClassifierData(cl);

//	std::cout << net->getClassifierData()->getData().cols() << std::endl;
	def::fftLength = 1024;

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

//	testNewClassifiers();
//	testSuccessive()
	exit(0);


#if 0
	/// Baklushev histograms

	int steps = 20;
	int norm = 8;
	std::valarray<double> res1;
	std::valarray<double> res2;
	readFileInLineRaw(def::dataFolder + "/Baklushev/healthy.txt", res1);
	readFileInLineRaw(def::dataFolder + "/Baklushev/ill.txt", res2);
	for(int steps = 12; steps <= 30; steps += 2)
	{
		myLib::histogram(res1, steps,
						 def::dataFolder + "/Baklushev/healthy_" + nm(steps) + ".jpg",
		{50., 94.}, "blue", norm);
		myLib::histogram(res2, steps,
						 def::dataFolder + "/Baklushev/ill_" + nm(steps) + ".jpg",
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
		def::drawNorm = -1;
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
    /// Dasha processing

    def::ntFlag = true; /// for Dasha's and EEGMRI
//    def::ntFlag = false; /// encephalan (+1)



    /// 32 channels for EEGMRI
    for(auto str : leest_more)
    {
		testChannelsOrderConsistency(def::DashaFolder + "/" + str);
		deleteSpaces(def::DashaFolder + "/" + str);
//        channelsOrderDir(def::DashaFolder + "/" + str,
//                       def::DashaFolder + "/Nonaudio_to_less",
//                       coords::lbl31_less);

//        GalyaProcessing(def::DashaFolder + "/AUDIO/" + str,
//                        31,
//                        def::DashaFolder + "/AUDIO/CHANS/Audio_to_less_out_ALL");
//            GalyaCut(def::DashaFolder + "/NONAUDIO/" + str,
//                     16,
//                     def::DashaFolder + "/AUDIO/CHANS/Audio_to_less_13");

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
	QStringList leest_audio = subjects::leest_more + subjects::leest_less;
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
	def::freq = 100;
    def::ns = 32;
#if 0
	// reduce channels in Reals
    def::ns = 32;
    const set<int, std::less<int>> exclude{3, 4, 14};
    QString helpString;
    for(int i = 0; i < def::ns; ++i)
    {
        if(std::find(exclude.begin(), exclude.end(), i) != exclude.end()) continue;
		helpString += nm(i + 1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);
    reduceChannelsSlot();
#endif
//    return;



    def::ns = 29;

    std::vector<pair<int, double>> pew;
    for(int i : {17, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20})
    {
        countSpectraSimple(1024, i);

        Net * net = new Net();

        net->autoClassification(def::dir->absolutePath()
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
    def::dir->cd(pth);
    QStringList dataFiles = def::dir->entryList(QStringList("*_ica.txt"));

    mat dataMat;
    dataMat.resize(20);



    for(QString &oneFile : dataFiles)
    {

        for(int i = 0; i < 20; ++i)
        {
            dataMat[i].resize(60 * 60 * 250); // 1 hour
        }

        helpString = pth + oneFile;
        readPlainData(helpString,
					  dataMat);
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
    def::dir->cd("/media/Files/Data/Mati");
    QStringList dirLst = def::dir->entryList(QStringList("???"), QDir::Dirs|QDir::NoDotAndDotDot);
    for(QString & guy : dirLst)
    {
        def::dir->cd(guy);
        def::dir->cd("auxEdfs");

		QString helpString = def::dir->absolutePath() + "/" + guy + "_0.edf";
        if(!QFile::exists(helpString))
        {
            def::dir->cdUp();
            def::dir->cdUp();
            continue;
        }
        edfFile initFile;
        initFile.readEdfFile(helpString);
        helpString.replace("_0.edf", "_1.edf");
        initFile.concatFile(helpString);
        helpString.replace("_1.edf", "_2.edf");
        initFile.concatFile(helpString);

        def::dir->cdUp();
		QString helpString2 = def::dir->absolutePath() + "/" + guy + "_full.edf";
        initFile.writeEdfFile(helpString2);
        def::dir->cdUp();
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

        QDir * tDir = new QDir();
        tdef::dir->cd("/media/Files/Data/Mati");
        QStringList nameFilters;
        nameFilters << "*.edf" << "*.EDF";
        QString fileName;
        QString helpString;

        QStringList lst = tdef::dir->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            tdef::dir->cd(lst[i]);

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + ".EDF";
            setEdfFile(helpString);
            rereferenceDataSlot();

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + "_rr.edf";
            setEdfFile(helpString);
            refilterDataSlot();

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + "_rr_f.edf";
            setEdfFile(helpString);
            sliceAll();
			drawReals();
            tdef::dir->cdUp();

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
