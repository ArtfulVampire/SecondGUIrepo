#include <widgets/mainwindow.h>
#include <myLib/adhoc.h>

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <thread>

#include <other/matrix.h>
#include <other/edffile.h>
#include <other/autos.h>
#include <other/consts.h>
#include <other/defs.h>

#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>
#include <myLib/output.h>
#include <myLib/small.h>
#include <myLib/statistics.h>
#include <myLib/mati.h>
#include <myLib/drw.h>
#include <myLib/valar.h>

using namespace myOut;

namespace myLib
{

std::vector<QString> readBurdenkoLog(const QString & logPath)
{
	QFile inStr(logPath);
	inStr.open(QIODevice::ReadOnly);

	std::vector<QString> res(240); /// magic const

	/// discard first 5 lines
	for(int i = 0; i < 5; ++i)
	{
		inStr.readLine();
	}
	for(auto & inRes : res)
	{
		QString line = QString(inStr.readLine().toStdString().c_str());
		inRes = line.split('\t', QString::SkipEmptyParts)[2].mid(1);
	}
//	std::cout << res << std::endl;
	inStr.close();
	return res;
}

/// {timeBin, marker}
std::vector<std::pair<int, int>> handleAnnots(const std::vector<QString> & annotations,
											  double srate)
{
	const std::vector<std::pair<QString, int>> markers
	{
		{"Presentation stimulation code: 0 (stimPresentation)", 10},
		{"Presentation stimulation code: 1 (stimPresentation)", 11},
		{"stimPresentation", 15},
		{"eventButton", 20},
	};

	const QString ch00 = QString(QChar(0));
	const QString ch20 = QString(QChar(20));

	const QString sep1 = ch20 + ch00;
	const QString sep2 = ch20 + ch20;

	std::vector<std::pair<int, int>> res{};

	for(const QString & annot : annotations)
	{
		/// chop epoch time start
		int start{0};
		for(int i = 0; i < annot.size() - 1; ++i)
		{
			if(annot[i].unicode() == 20 && annot[i + 1].unicode() != 20) { start = i; break; }
		}
		QString tmp = annot.mid(start + 2);
//		std::cout << tmp << std::endl; continue;

		auto marks = tmp.split(sep1, QString::SkipEmptyParts);
		for(auto & mark : marks)
		{
			/// workaround "empty" parts
			mark.replace(QRegExp(" {2,}"), " ");
			mark.replace(QRegExp("\\0{2,}"), " ");
			if(mark.size() < 10) { continue; } /// magic const
			if(mark.contains(QChar(21))) { continue; } /// edf+ "duration"

//			std::cout << mark << std::endl; continue;
			auto par = mark.split(sep2, QString::SkipEmptyParts);
			int outMark{0};
			for(const auto & mrk : markers)
			{
				if(par[1].contains(mrk.first)) { outMark = mrk.second; break; }
			}
			if(outMark)
			{
				std::pair<int, int> newPair{std::round(par[0].remove('+').toDouble() * srate),
							outMark};

				/// same time (10 or 11) and 15
				if(outMark == 10 || outMark == 11) { --newPair.first; }

				if(res.size() < 3
				   ||
				   (res[res.size() - 1] != newPair
				   && res[res.size() - 2] != newPair))
				{
					res.push_back(newPair);
				}
			}
		}
	}
	return res;
}

void XeniaPretable()
{
	std::vector<std::vector<int>> groupIds
	{
		smLib::range<std::vector<int>>(1, 13 + 1),
				smLib::range<std::vector<int>>(14, 26 + 1),
				smLib::range<std::vector<int>>(27, 41 + 1),
	};

	struct chann
	{
		int spssNum;
		int loc;
		int hemisph;
		chann(int num_, int loc_, int hemi_) : spssNum{num_}, loc{loc_}, hemisph{hemi_} {}
	};

	std::vector<chann> chanVec
	{
		{101, 1, 1},	/// Fp1
		{102, 1, 2},	/// Fp2
		{103, 2, 1},	/// F7
		{104, 3, 1},	/// F3
//		{105, 1, 3},	/// Fz
		{106, 3, 2},	/// F4
		{107, 2, 2},	/// F8
		{108, 4, 1},	/// T3
		{109, 5, 1},	/// C3
//		{110, 1, 3},	/// Cz
		{111, 5, 2},	/// C4
		{112, 4, 2},	/// T4
		{113, 6, 1},	/// T5
		{114, 7, 1},	/// P3
//		{115, 1, 3},	/// Pz
		{116, 7, 2},	/// P4
		{117, 6, 2},	/// T6
		{118, 8, 1},	/// O1
		{119, 8, 2},	/// O2

	};

	std::ofstream fil;
	fil.open("/media/Files/Data/Xenia/FINAL_res/table_pre.txt");

	for(int group : {1, 2, 3})
	{
		for(int stim : {1, 2, 3, 4, 5, 6, 7})
		{
			for(int id : groupIds[group - 1])
			{
				for(chann chan :  chanVec)
				{
					fil
							<< group << "\t"
							<< stim << "\t"
							<< id << "\t"
							<< chan.spssNum << "\t"
							<< chan.loc << "\t"
							<< chan.hemisph << "\t"
							<< std::endl;
				}
			}
		}
	}
	fil.close();
}

void XeniaFinalest()
{
	DEFS.setAutosUser(autosUser::XeniaFinalest);
	const QString workPath = "/media/Files/Data/Xenia/FINAL";
	const std::vector<QString> subdirs{"Healthy", "Moderate", "Severe"};
	const std::vector<QString> tbiMarkers{"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"};
	DEFS.setAutosMask(0
//					  | featuresMask::alphaPeak
					  | featuresMask::fracDim
					  | featuresMask::Hilbert
//					  | featuresMask::fft
//					  | featuresMask::logFFT
					  );

#if 0
	/// check zeros
	const QString dr = "/media/Files/Data/Xenia/FINAL_res";
	auto lst = QDir(dr).entryList({"Nemirov*_spectre.txt"});
	int cc = 0;
	for(auto fn : lst)
	{
		std::ifstream in;
		in.open((dr + "/" + fn).toStdString());
		double tmp;
		while(in >> tmp)
		{
			if(tmp <= 0.0001) { ++cc; }
			{ std::cout << fn << std::endl; break; }
		}
		in.close();
	}
	std::cout << cc << std::endl;
	return;
#endif

#if 0
	/// initial rename guyDirs as edfs inside
	for(const QString & subdir : subdirs)
	{
		const QString groupPath = workPath + "/" + subdir;
		const QStringList groupGuys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(const QString & guy : groupGuys)
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
	return;
#endif

#if 0
	/// check each guy has all stimuli files - OK
	for(const QString & subdir : subdirs)
	{
		const QString groupPath = workPath + "/" + subdir;
		const QStringList groupGuys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(const QString & guy : groupGuys)
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
	return;
#endif


#if 0
	/// find absent guyNames in guys_finalest.txt and rename properly (manually)
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
	for(const QString & subdir : subdirs)
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
	return;
#endif

#if 01
	/// processing itself
	autos::Xenia_TBI_finalest(workPath, workPath + "_res", tbiMarkers);
#endif



#if 0
	/// Xenia FINAL make labels
	const QString sep{"\t"};

	DEFS.setAutosMask(featuresMask::Hilbert
					  | featuresMask::fracDim);

	std::ofstream lab;
	lab.open((def::XeniaFolder + "/labels.txt").toStdString());

	std::vector<QString> labels1 = coords::lbl19;
	for(QString & in : labels1)
	{
		in = in.toLower();
	}
	smLib::eraseItems(labels1, std::vector<int>{4, 9, 14});	/// skip Fz, Cz, Pz

	const QString initFreq = "_1.6-30";
	for(const QString & mark_ : tbiMarkers)
	{
		QString mark = mark_;
		mark.remove("_");

		if(DEFS.getAutosMask() & featuresMask::fft)
		{
			/// FFT
			for(int i = 2; i < 20; ++i) /// freqs
			{
				for(const QString & lbl : labels1)
				{
					lab << mark
						<< "_" << "fft"
						<< "_" << nm(i)
						<< "-" << nm(i + 1)
						<< "_"  << lbl << sep;
				}
			}
		}
		if(DEFS.getAutosMask() & featuresMask::fracDim)
		{
			const QString fir = "fd" + initFreq;
			{
				for(const QString & lbl : labels1)
				{
					lab << mark
						<< "_" << fir
						<< "_"
						<< lbl << sep;
				}
			}
		}

		if(DEFS.getAutosMask() & featuresMask::Hilbert)
		{
			/// CHAOS - check hilbertFilters in countHilbert (
			for(const QString & fir : {
				QString("hilbcarr")	+ initFreq,
				QString("hilbsd")	+ initFreq,
				QString("hilbcarr")	+ "_2-20",
				QString("hilbsd")	+ "_2-20",
				QString("hilbcarr")	+ "_8-13",
				QString("hilbsd")	+ "_8-13",
				QString("hilbcarr")	+ "_2-7",
				QString("hilbsd")	+ "_2-7",
				QString("hilbcarr")	+ "_13-18",
				QString("hilbsd")	+ "_13-18",
		}
				)
			{
				for(const QString & lbl : labels1)
				{
					lab << mark
						<< "_" << fir
						<< "_"
						<< lbl << sep;
				}
			}
		}

		if(DEFS.getAutosMask() & featuresMask::wavelet)
		{
			/// WAVELET
			for(const QString & fir : {"wavmean", "wavmed", "wavsgm"})
			{
				for(int i = 2; i <= 20; ++i)
				{
					for(const QString & lbl : labels1)
					{
						lab << mark
							<< "_" << fir
							<< "_" << nm(i)
							<< "-" << nm(i+1)
							<< "_" << lbl << sep;
					}
				}
			}
		}

		if(DEFS.getAutosMask() & featuresMask::alphaPeak)
		{
			/// ALPHA
			for(const QString & lbl : labels1)
			{
				lab << mark
					<< "_" << "alpha"
					<< initFreq
					<< "_" << lbl << sep;
			}
		}
	}
	lab.close();
	return;
#endif /// labels

#if 0
	/// labels check
	std::cout << myLib::countSymbolsInFile(
					 def::XeniaFolder + "/table_Hilbert_first30.txt", '\t') / 41 << std::endl;

	std::cout << myLib::countSymbolsInFile(
					 def::XeniaFolder + "/labels.txt", '\t') << std::endl;

	return;
#endif

}

void GalyaProcessing()
{
	//	const QStringList subdirs = QDir(workPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	//	const QStringList subdirs{"young", "adult"};
		const QStringList subdirs{""};
		const QString workPath = def::GalyaFolder + "/Faces";
		const int numChan = 19;
		const std::vector<QString> usedMarkers = autos::marks::buben;

#if 0
		/// initial repair filenames
		for(const QString & subdir : subdirs)
		{
			repair::toLatinDir(workPath + "/" + subdir);
			repair::deleteSpacesDir(workPath + "/" + subdir);
			repair::toLowerDir(workPath + "/" + subdir);
			/// some special names cleaning
			for(const QString & fileName : QDir(workPath + "/" + subdir).entryList(def::edfFilters))
			{
				QString newName = fileName;
				newName.replace("_new.edf", ".edf", Qt::CaseInsensitive);
				newName.replace("_bub_after", "_bubAfter", Qt::CaseInsensitive);
				newName.replace("_bub_aud", "_bubAud", Qt::CaseInsensitive);
				newName.replace("_bubAud_after", "_bubAudAfter", Qt::CaseInsensitive);
				newName.replace("_bub_aud_after", "_bubAudAfter", Qt::CaseInsensitive);
				newName.replace("_og_after", "_ogAfter", Qt::CaseInsensitive);
//				newName.replace("_4_", "_4", Qt::CaseInsensitive);
//				newName.replace("_8_", "_8", Qt::CaseInsensitive);
//				newName.replace("_16_", "_16", Qt::CaseInsensitive);

				bool p = false;
				for(auto mrk : usedMarkers)
				{
					if(newName.contains(mrk)) { p = true; break;}
				}
				if(!p)
				{
					std::cout << subdir + "/" + newName << std::endl;
				}

				if(newName != fileName)
				{
					QFile::remove(workPath + "/" + subdir + "/" + newName);
					QFile::rename(workPath + "/" + subdir + "/" + fileName,
								  workPath + "/" + subdir + "/" + newName);
				}

			}
		}
		exit(0);
#endif


#if 0
		/// checks and corrects channels order consistency

		edfFile labels;
		labels.readEdfFile(workPath + "/labels.edf");

		QString str19;
		for(int i = 0; i < 19; ++i)
		{
			str19 += nm(labels.findChannel(coords::lbl19[i])) + " ";
		}
//		std::cout << str19 << std::endl;

		for(const QString & subdir : subdirs)
		{
			const QString groupDir = workPath + "/" + subdir;
			const auto guyDirs = QDir(groupDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

			for(const QString & guyDir : guyDirs) /// each guy
			{
				const QString guyPath = groupDir + "/" + guyDir;
				auto filList = QDir(guyPath).entryList(def::edfFilters); /// edfs of one guy
				for(const QString & fl : filList)
				{
					edfFile file;
					file.readEdfFile(guyPath + "/" + fl, true);
					QString helpString{};
					for(int i = 0; i < 19; ++i)
					{
						helpString += nm(file.findChannel(coords::lbl19[i])) + " ";
					}

					if(helpString != str19)
					{
						std::cout << fl << std::endl;
						/// rewrite file with correct chan order - read with data
//						file.reduceChannels(helpString).writeEdfFile(tact + "/" + dr + "/" + fl);
					}
				}
			}
//			autos::EdfsToFolders(workPath + "/" + subdir);
		}
		exit(0);
#endif


#if 0
		/// each subject into his/her own folder
		for(const QString & subdir : subdirs)
		{
//			autos::rewriteNew(workPath + "/" + subdir);
			autos::EdfsToFolders(workPath + "/" + subdir);
		}
		exit(0);
#endif




#if 0
		/// calculation itself
		DEFS.setAutosUser(autosUser::Galya);

//		DEFS.setAutosMask(featuresMask::wavelet);

		for(const QString & subdir : subdirs)
		{
			/// usual processing
			autos::ProcessByFolders(workPath + "/" + subdir,
									usedMarkers);
#if 0
			/// rhythm adoption
			for(const QString & stimType : {"sv", "zv"})
			{
				autos::rhythmAdoptionGroup(workPath + "/" + subdir,
										   "_fon",
										   stimType);
			}
#endif
		}
		exit(0);
#endif


#if 0
		/// test table rows lengths
		for(const QString & subdir : subdirs)
		{
			QFile feel(workPath + "/" + subdir + "_out/all.txt");
			feel.open(QIODevice::ReadOnly);
			while(!feel.atEnd())
			{
				std::cout << QString(feel.readLine()).count('\t') << std::endl;
			}
			feel.close();
		}
		exit(0);
#endif


/// labels part
#if 0
		/// labels rhythm adoption

		const QString sep{"\t"};
//		const QString sep{"\r\n"};

		std::vector<QString> labels1 = coords::lbl19;
		for(QString & in : labels1) { in = in.toLower(); }

		std::ofstream lab;
		for(const QString & T : {"sv", "zv"})
		{
			lab.open((workPath + "/labels_" + T + ".txt").toStdString());

			for(int fr : {2, 4, 8, 16})
			{
				for(QString lbl : labels1)
				{
					lab << nm(fr)
						<< "_" << T
						<< "_" << lbl << sep;
				}
			}
			lab.close();
		}
		exit(0);
#endif

		/// labels part
		autos::makeLabelsFile(numChan, workPath, "_1.6_30", usedMarkers, "\t");

#if 0
		if(01)
		{
			/// cout lengths of all txts
			for(const QString & subdir : subdirs)
			{
				for(QString fn : QDir(workPath + "/" + subdir + "_out").entryList({"*.txt"}))
				{
					std::cout << fn << "\t"
							  << myLib::countSymbolsInFile(workPath + "/" + subdir + "_out" + "/" + fn, '\t') << std::endl;
				}
			}
		}
		std::cout << "labels.txt" << "\t"
				  << myLib::countSymbolsInFile(workPath + "/labels.txt", '\t') << std::endl;
		exit(0);
#endif

#if 0
		/// test table rows lengths
		for(const QString & subdir : subdirs)
		{
			QFile feel(workPath + "/" + subdir + "_out/all.txt");
			feel.open(QIODevice::ReadOnly);
			while(!feel.atEnd())
			{
				std::cout << QString(feel.readLine()).count('\t') << std::endl;
			}
			feel.close();
		}
		exit(0);
#endif
}

std::vector<QString> composeMarkersList(const QString & workPath)
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
		std::cout << R"(")" << mrk << R"(", )";
		std::cout << mrk << " ";
	}
	std::cout << std::endl;
	std::vector<QString> res(markers.size());
	for(const auto & in : markers)
	{
		res.push_back(in);
	}
	return res;
}

void testEgiChns128()
{
	/// test coords::egi::chans128 for duplicates and lost channels
	std::vector<int> vec(130, 0);
	for(const auto & in : coords::egi::chans128)
	{
		for(const auto & in2 : in.second)
		{
			QString t = in2;
			t.remove(" ");

			if(vec[t.toInt()] == 1)
			{
				std::cout << t.toInt() << " collision" <<  std::endl;
			}
			vec[t.toInt()] = 1;
		}
	}

	for(int i = 1; i < vec.size(); ++i)
	{
		if(vec[i] == 0)
		{
			std::cout << i << " unused" << std::endl;
		}
	}
}

void burdenkoStuff()
{

#if 0
	/// downsample for Burdenko
	const QString fold = "/media/Files/Data/Galya/Burd/Data";
	for(int i = 19; i <= 34; ++i) /// 19-34
	{
		QDir dr = QDir(fold + "/" + nm(i));
		edfFile fil;
		const QString fn = dr.entryList({"*" + nm(i) + "_*.edf"})[0];
		fil.readEdfFile(dr.absolutePath() + "/" + fn);
		QString outPath = fil.getFilePath();
		outPath.replace(".edf", "_ds.edf");
		fil.downsample(500).refilter(0.5, 35).writeEdfFile(outPath);
	}
	exit(0);
#endif

#if 0
	/// copy files Burdenko
	const QString fold = "/media/Files/Data/Galya/Burd/Data";
	for(int i = 24; i <= 34; ++i) /// 19-34
	{
		QDir dr = QDir(fold + "/" + nm(i));

	}
	exit(0);
#endif

#if 0
	/// Burdenko Edf+ and log to simple EDF
	const QString fold = "/media/Files/Data/Galya/Burd/Data";
	for(int i = 4; i <= 34; ++i) /// 19-34
	{
		const QString wrk = fold + "/" + rn(i, 2);
		QDir dr(wrk);
		const auto log = dr.entryList({"*.log"})[0];
		auto picNums = myLib::readBurdenkoLog(wrk + "/" + log);

		const auto edfPlus = dr.entryList({"*.edf"})[0];
		edfFile fil(wrk + "/" + edfPlus);

		auto annots = myLib::handleAnnots(fil.getAnnotations(), 500);


		auto it1 = std::begin(annots);
		for(auto picNum : picNums)
		{
			auto itNext = std::find_if(it1, std::end(annots),
									   [](const auto & ann)
			{ return ann.second == 15; });
			(*itNext).second = picNum.toInt();
			it1 = itNext + 1;
		}

		fil.removeChannel(fil.getMarkChan());
		edfChannel newMarkChan("Markers",
							   "AgAgCl",
							   "uV",
							   4096,
							   0,
							   4096,
							   0,
							   "",
							   500,
							   "");
		std::valarray<double> newMarkData(fil.getDataLen());
		for(const auto & mrk : annots)
		{
			newMarkData[mrk.first] = mrk.second;
		}

		fil.addChannel(newMarkData, newMarkChan);

		QString edfName = edfPlus;
		edfName.replace(".edf", "_new.edf");
		fil.writeEdfFile(wrk + "/" + edfName);

//		break;

	}
	exit(0);
#endif

#if 0
	/// Burdenko reaction Time
	const QString fold = "/media/Files/Data/Galya/Burd/Data";
	for(int i = 4; i <= 34; ++i) /// 19-34
	{
		const QString wrk = fold + "/" + rn(i, 2);
		QDir dr(wrk);

		const auto edf = dr.entryList({"*_new.edf"})[0];
		QString log = edf;
		log.replace(".edf", "_reac.txt");

		edfFile fil(wrk + "/" + edf);
		std::ofstream outStr((wrk + "/" + log).toStdString());

		auto mrks = fil.getMarkers();
		for(int i = 0; i < mrks.size(); ++i)
		{
			if(mrks[i].second == 11 && mrks[i + 2].second != 20)
			{
				outStr
						<< mrks[i + 1].second << "\t" /// picNum
						<< (mrks[i + 2].first - mrks[i + 1].first) / fil.getFreq() << "\t" /// time
						<< "false-negative" << "\t"
						<< std::endl;
			}
			if(mrks[i].second == 20)
			{
				outStr
						<< mrks[i - 1].second << "\t" /// picNum
						<< (mrks[i].first - mrks[i-1].first) / fil.getFreq() << "\t" /// time
						<< ((mrks[i - 2].second == 11) ? "correct" : "wrong") << "\t"
						<< std::endl;
			}

		}
		outStr.close();
	}
#endif
}

void drawEnvelope(const QString & gh, const QString & fff)
{
	edfFile fl;
	fl.readEdfFile(gh);
	/// magic consts
	std::valarray<double> arr = smLib::contSubsec(fl.getData("Pz"),
												  5.6 * 250,
												  (5.6 + 7) * 250);
	arr = myLib::refilter(arr, 8, 13, false, 250);
	myLib::hilbertPieces(arr, fff);
}

void checkFeedbackFiles()
{
#if 0
	/// check markers feedback
	const QString path = QString("/media/Files/Data/FeedbackNewMark/");
	for(const QString & subdir : QDir(path).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
		for(QString fl : QDir(path + "/" + subdir).entryList({"*_good.edf", "*_good.EDF"}))
		{
			fb::checkMarkFBfinal(path + "/" + subdir + "/" + fl);
		}
	}

	/// check all fin files presence
	for(const auto & sub : subj::guysFBnew)
	{
		const QString path = "/media/Files/Data/FeedbackFinalMark/" + sub.first;
		for(int i = 1; i <= 3; ++i)
		{
			if(!QFile::exists(path + "/" + sub.second + "_" + nm(i) + "_fin.edf"))
			{
				std::cout << sub.second << "\t" << i << std::endl;
			}
		}
	}
#endif
}

void HiguchiTests()
{
	edfFile fil;
	auto sig1 = smLib::contSubsec(fil.getData(9),
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
	int N = std::pow(2, 17);
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
			if( (i - 1) % static_cast<int>(std::pow(2, 12)) == 0)
			{
				std::cout << (i - 1) / std::pow(2, 12) << std::endl;
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
		myLib::drw::drawOneSignal(smLib::contSubsec(Y, 0,  1000), 1000)
				.save("/media/Files/Data/Y.jpg", nullptr, 100);
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
				powLam1[i] = std::pow(lambda, i);
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
}

void binomialToFile()
{
	std::ofstream ff;
	ff.open("/media/Files/Data/aklsjajkdf.txt");
	ff << 0 << "\t";
	for(int j = 10; j <= 40; ++j)
	{
		ff << j << "\t";
	}
	ff << std::endl;

	for(int i = 10; i <= 40; ++i)
	{
		ff << i << "\t";
		for(int j = 10; j <= 40; ++j)
		{
			ff << myLib::binomialOneTailed(i, j, 40) << "\t";
		}
		ff << std::endl;
	}
	ff.close();
}

void binomialThresholds()
{
	std::cout.precision(3);
	std::cout << " \t";

	for(int N = 80; N >= 30; N-= 5)
	{
		std::cout << N << "\t";
	}
	std::cout << std::endl;

	std::cout << std::fixed;
	std::cout.precision(1);

	for(int numClass = 2; numClass <= 6; ++numClass)
	{
		std::cout << numClass << "\t";
		for(int N = 80; N >= 30; N-= 5)
		{
			std::cout << myLib::binomialLimitOfSignificance(N, 1. / numClass, 0.05) * 100. / N << "\t";
		}
		std::cout << std::endl;
	}
}

void drawFftWindows()
{
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
}


void BaklushevHistograms()
{
	const int norm = 8;

	std::valarray<double> res1 = readFileInLineRaw(DEFS.dirPath() + "/Baklushev/healthy.txt");
	std::valarray<double> res2 = readFileInLineRaw(DEFS.dirPath() + "/Baklushev/ill.txt");
	for(int steps = 12; steps <= 30; steps += 2)
	{
		myLib::histogram(res1, steps, {50., 94.}, "blue", norm)
				.save(DEFS.dirPath() + "/Baklushev/healthy_" + nm(steps) + ".jpg", nullptr, 100);
		myLib::histogram(res2, steps, {50., 94.}, "red", norm)
				.save(DEFS.dirPath() + "/Baklushev/ill_" + nm(steps) + ".jpg", nullptr, 100);
	}
}

void matiConcat()
{
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
}


} /// end namespace myLib

