#include <widgets/mainwindow.h>
#include <myLib/adhoc.h>

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <thread>
#include <map>

#include <other/matrix.h>
#include <other/edffile.h>
#include <other/autos.h>
#include <other/coords.h>
#include <other/defs.h>
#include <other/subjects.h>
#include <other/feedback.h>
#include <other/feedback_autos.h>

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
std::vector<std::pair<int, int>> handleBurdenkoAnnots(const std::vector<QString> & annotations,
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

		auto marks = tmp.split(sep1, QString::SkipEmptyParts);
		for(auto & mark : marks)
		{
			/// mark is like "+138.481  code1"

			/// workaround "empty" parts
			mark.replace(QRegExp(" {2,}"), " ");
			mark.replace(QRegExp("\\0{2,}"), " ");
			if(mark.size() < 10) { continue; }				/// magic const
			if(mark.contains(QChar(21))) { continue; }		/// edf+ "duration"


			auto par = mark.split(sep2, QString::SkipEmptyParts);
			/// par[0] is time
			/// par[1] is annotation

			int outMark{0};
			for(const auto & mrk : markers)
			{
				if(par[1].contains(mrk.first)) { outMark = mrk.second; break; }
			}
			if(outMark) /// != 0
			{
				std::pair<int, int> newPair
				{
					std::round(par[0].remove('+').toDouble() * srate),	/// time in bins
							outMark										/// marker value
				};

				/// same time (two 10 or 11) and 15
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


/// special Xenia table making
void TablingXenia(const QString & outPath)
{
	/// make matrix for each feature separately
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

	for(autos::feature feat : AUT_SETS.getAutosMaskArray())
	{
		std::vector<QString> filesToVertCat{};
		for(const QString & mark : AUT_SETS.getMarkers())
		{
			for(const QString & guy : guys)
			{
				/// each file is supposed to consist of numOfChannels rows
				filesToVertCat.push_back(
							guy
							+ mark
							+ "_" + autos::featToStr(feat)
							+ ".txt");
			}
		}
		myLib::concatFilesVert(outPath,
							   filesToVertCat,
							   outPath + "/table_"
							   + autos::featToStr(feat) + ".txt");
	}
}

/// one more Xenia special tabling
void TablingXeniaWithAverage(const QString & inPath,
							 const QString & outFilePath,
							 const QString & featName)
{
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


	std::vector<QString> filesToVertCat{};
	for(const QString & mark : AUT_SETS.getMarkers())
	{
		for(const QString & guy : guys)
		{
			/// each file is supposed to consist of numOfChannels rows
			filesToVertCat.push_back(
						guy
						+ mark
						+ "_" + featName
						+ ".txt");
		}
	}
	myLib::concatFilesVert(inPath,
						   filesToVertCat,
						   outFilePath);
}

void TablingUsual(const QString & workPath, const QString & outPath)
{
	auto groups = QDir(workPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	if(groups.isEmpty()) { groups = QStringList{""}; }
	for(const QString & group : groups)
	{
		const QString groupPath = workPath + "/" + group;
		/// list of guys
		const QStringList guys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(const QString & guy : guys)
		{
			const QString guyOutPath = groupPath + "/" + guy + "/out";

			/// make one line file for each stimulus
			if(01)
			{
				for(const QString & mark : AUT_SETS.getMarkers())
				{
					std::vector<QString> fileNamesToArrange{};
					for(autos::feature func : AUT_SETS.getAutosMaskArray())
					{
						const QString fileName = guy + mark
												 + "_" + autos::featToStr(func) + ".txt";
						fileNamesToArrange.push_back(fileName);
					}
					myLib::concatFilesHorz(guyOutPath,
										   fileNamesToArrange,
										   guyOutPath + "/" + guy + mark + ".txt");
				}
			}

			/// make whole line from all stimuli
			if(1)
			{
				std::vector<QString> fileNamesToArrange{};
				for(const QString & mark : AUT_SETS.getMarkers())
				{
					fileNamesToArrange.push_back(guy + mark + ".txt");
				}
				myLib::concatFilesHorz(guyOutPath,
									   fileNamesToArrange,
									   guyOutPath + "/" + guy + ".txt");
			}

			/// copy files into _out
			if(1)
			{
				QFile::remove(outPath + "/" + guy + ".txt");
				QFile::copy(guyOutPath + "/" + guy + ".txt",
							outPath + "/" + guy + ".txt");
			}
		}

	}
	/// make tables whole and people list
	autos::ArrangeFilesToTable(outPath,
							   outPath + "/all.txt",
							   true);
}


void XeniaFinal()
{
	SettingsXenia();
	const QString inPath{"/media/Files/Data/Xenia/FINAL"};

	for(const QString & group : {"Healthy", "Moderate", "Severe"})
	{
		const QString groupPath = inPath + "/" + group;

		/// list of guys
		const QStringList guys = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(const QString & guy : guys)
		{
			const QString guyPath = groupPath + "/" + guy;
			const QString guyOutPath = guyPath + "/out";
			if(!QDir(guyOutPath).exists()) { QDir().mkpath(guyOutPath); }

			myLib::cleanDir(guyOutPath);
			autos::calculateFeatures(guyPath, AUT_SETS.getChansProc(), guyOutPath);

			/// make one line file for each stimulus
			if(01)
			{
				for(const QString & mark : AUT_SETS.getMarkers())
				{
					std::vector<QString> fileNamesToArrange{};
					for(autos::feature func : AUT_SETS.getAutosMaskArray())
					{
						const QString fileName = guy + mark
												 + "_" + autos::featToStr(func) + ".txt";
						fileNamesToArrange.push_back(fileName);
					}
					myLib::concatFilesHorz(guyOutPath,
										   fileNamesToArrange,
										   guyOutPath + "/" + guy + mark + ".txt");
				}
			}

			/// make whole line from all stimuli
			if(1)
			{
				std::vector<QString> fileNamesToArrange{};
				for(const QString & mark : AUT_SETS.getMarkers())
				{
					fileNamesToArrange.push_back(guy + mark + ".txt");
				}
				myLib::concatFilesHorz(guyOutPath,
									   fileNamesToArrange,
									   guyOutPath + "/" + guy + ".txt");
			}

			/// copy files into _out
			if(1)
			{
				QFile::remove(inPath + "_out/" + guy + ".txt");
				QFile::copy(guyOutPath + "/" + guy + ".txt",
							inPath + "_out/" + guy + ".txt");
			}
		}

	}
	/// make tables whole and people list
	autos::ArrangeFilesToTable(inPath + "_out",
							   inPath + "_out/all.txt",
							   true);
	autos::makeLabelsFile(AUT_SETS.getChansProc(),
						  inPath + "/labels.txt",
						  AUT_SETS.getMarkers(),
						  AUT_SETS.getAutosMask(),
						  "\t");
}


void checkMarkersFiles(const QString & workPath, const std::vector<QString> & inMarkers)
{
	/// check each guy has all stimuli files - OK
	for(const QString & group : QDir(workPath).entryList(QDir::Files | QDir::NoDotAndDotDot))
	{
		const QString groupPath = workPath + "/" + group;
		const QStringList guysList = QDir(groupPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for(const QString & guy : guysList)
		{
			const QString guyPath = groupPath + "/" + guy;
			const QStringList edfs = QDir(guyPath).entryList(def::edfFilters);
			for(const QString & mark : inMarkers)
			{
				bool ok = false;
				for(const QString & edfName : edfs)
				{
					if(edfName.contains(mark)) { ok = true; break; }
				}
				if(!ok) { std::cout << guy << mark << " not found!!!" << std::endl; }
			}
		}
	}
}

void checkGuysFinalest()
{
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
}

void SettingsXenia()
{
	AUT_SETS.setAutosMask(0
						  | autos::feature::fft
						  | autos::feature::fracDim
						  | autos::feature::Hilbert
						  );

	AUT_SETS.setOutputStyle(autos::outputStyle::Line);	/// Xenia Final
//	AUT_SETS.setOutputStyle(autos::outputStyle::Table);	/// Xenia Finalest
	AUT_SETS.setOutputSequence(autos::outputSeq::ByChans);
	AUT_SETS.setInitialCut(autos::initialCut::first60);

	AUT_SETS.setChansProc(coords::lbl16noz);
	AUT_SETS.setMarkers(subj::marksLists::tbiMarkers);
}

void XeniaWithAverage(const QString & workPath)
{

	AUT_SETS.setOutputStyle(autos::outputStyle::Column);
	AUT_SETS.setOutputSequence(autos::outputSeq::ByChans);
	AUT_SETS.setInitialCut(autos::initialCut::first60);
	AUT_SETS.setMarkers(subj::marksLists::tbiMarkers);
	AUT_SETS.setChansProc(coords::lbl19);

//	AUT_SETS.setAutosMask(0
//						  | autos::feature::fft
//						  | autos::feature::Hilbert
//						  | autos::feature::fracDim
//						  | autos::feature::alphaPeak
//						  );



//	AUT_SETS.setFeatureFilter(autos::feature::fft,		{{1, 4}, {3, 8}, {8, 13}, {14, 20}});
//	AUT_SETS.setFeatureFilter(autos::feature::Hilbert,	{{1, 4}, {3, 8}, {8, 13}, {14, 20}, {1, 20}});
//	AUT_SETS.setFeatureFilter(autos::feature::fracDim,	{{1, 5}, {3, 8}, {8, 13}, {14, 20}, {1, 20}});

	using oneGroupType = std::pair<std::vector<int>, QString>;
	using chanGroupType = std::vector<oneGroupType>;

	///			Fp1=0			Fp2=1
	/// F7=2	F3=3	Fz=4	F4=5	F8=6
	/// T3=7	C3=8	Cz=9	C4=10	T4=11
	/// T5=12	P3=13	Pz=14	P4=15	T6=16
	///			O1=17			O2=18
	const std::vector<chanGroupType> chanGroups
	{
#if 01
		{
			{{2, 3, 7, 8, 12, 13, 17}, "left"},
			{{5, 6, 10, 11, 15, 16, 18}, "right"},
		},
#endif

#if 01
		{
			{{4, 9}, "FCz"},
			{{9, 14}, "CPz"},
			{{3, 5}, "F34"},
			{{2, 6}, "F78"},
			{{8, 10}, "C34"},
			{{7, 11}, "T34"},
			{{12, 16}, "T56"},
			{{13, 15}, "P34"},
			{{17, 18}, "O12"},
		},
#endif

#if 01
		{
//			{{0}, "Fp1"},	{{1}, "Fp2"},
			{{3}, "F3"},	{{5}, "F4"},
			{{2}, "F7"},	{{6}, "F8"},
//			{{4}, "Fz"},
			{{8}, "C3"},	{{10}, "C4"},
			{{7}, "T3"},	{{11}, "T4"},
			{{12}, "T5"},	{{16}, "T6"},
//			{{9}, "Cz"},
			{{13}, "P3"},	{{15}, "P4"},
//			{{14}, "Pz"},
			{{17}, "O1"},	{{18}, "O2"},
		},
#endif
	};

	/// for each feature
	/// new 13-Nov
	const std::vector<autos::feature> features
	{
		autos::feature::fft,
//		autos::feature::Hilbert,
//		autos::feature::fracDim,
//		autos::feature::alphaPeak,
	};

	/// new 13-Nov
	const std::map<autos::featureEnumType, std::vector<autos::filterFreqs>> featuresFilters
	{
		{autos::feature::alphaPeak,	{autos::filtFreqs.at(autos::filter::none)}},
		{autos::feature::fft,
			{
//				{1, 4},
				{3, 8},
//				{8, 13},
//				{14, 20}
			}
		},
		{autos::feature::Hilbert,
			{
				{1, 4},
				{3, 8},
				{8, 13},
				{14, 20},
				{1, 20}
			}
		},
		{autos::feature::fracDim,
			{
				{1, 5},
				{3, 8},
				{8, 13},
				{14, 20},
				{1, 20}
			}
		},
	};

	const QString tmpPath = workPath + "_res2";

	/// each type of averaging
//	for(int i = 0 ; i < chanGroups.size(); ++i)
	for(int i : {1})
	{
		AUT_SETS.setChansGroups(chanGroups[i]);

		for(auto feat : features)
		{
			for(auto flt : featuresFilters.at(feat))
			{
				myLib::cleanDir(tmpPath);
				AUT_SETS.setAutosMask(feat);
				AUT_SETS.setFeatureFilter(feat, {flt});

				autos::ProcessByGroups(workPath,
									   tmpPath,
									   AUT_SETS.getChansProc());

				const QString filtString = ([](autos::filterFreqs in) -> QString
				{
					if(in != autos::filterFreqs{0, 0})
					{ return QString{"_" + nm(in.first) + "_" + nm(in.second)}; }
					return QString{};
				})(flt);

				/// magic const 41
				autos::makeLabelsXeniaWithAverage(AUT_SETS.getChansGroupsNames(),
												  41,

												  workPath + "/labels_"
												  + autos::featToStr(feat)
												  + filtString
												  + "_av" + nm(i + 1)
												  + ".txt",

												  AUT_SETS.getMarkers(),
												  feat,
												  "\r\n");

				myLib::TablingXeniaWithAverage(tmpPath,

											   workPath
											   + "/table_"
											   + autos::featToStr(feat)
											   + filtString
											   + "_av" + nm(i + 1)
											   + ".txt",

											   autos::featToStr(feat));
			}
		}
	}
}


void XeniaProcessing(const QString & workPath)
{
	SettingsXenia();
#if 01
	/// processing itself
	autos::ProcessByGroups(workPath,
						   QString{},
						   AUT_SETS.getChansProc());
	autos::makeLabelsFile(AUT_SETS.getChansProc(),
						  workPath + "/labels.txt",
						  AUT_SETS.getMarkers(),
						  AUT_SETS.getAutosMask(),
						  "\t");
#endif

	if(AUT_SETS.getOutputStyle() == autos::outputStyle::Table)
	{
		/// special Xenia tabling, aka finalest
		TablingXenia(workPath + "_res");
	}
	/// Column is way too special
	else /// if(AUT_SETS.getOutputStyle() == autos::outputStyle::Line)
	{
		/// usual, like for Galya, aka Final
		TablingUsual(workPath, workPath + "_res");
	}
}

void preprocessDir(const QString & inPath)
{
#if 0
	/// special renames
	const std::vector<std::pair<QString, QString>> rnm1
	{
		{"_new", ""},
		{"4EPHOPAEB_32",		"Chernoraev32"},
		{"4UJLO9lH_55",			"Chuloyan55"},
		{"4YP6AHOB_30",			"Churbanov30"},
		{"AJLELLlUH_31",		"Aleshin31"},
		{"BAHl~EJLUI'I_37",		"Vangely37"},
		{"baykov_58",			"Baykov58"},
		{"syputdinov_38",		"Suputdinov38"},
		{"JLblCOB_53",			"Lyisov53"},
		{"KBALLlHUHOB_51",		"Kvashninov51"},
		{"l`l9lTKUH",			"Pyatkin"},
		{"MAH9lXUH_34",			"Manyahin34"},
		{"MAMEDOB_16",			"Mamedov16"},
		{"qPJLOPOBCKUI'I_18",	"Frolovsky18"},
		{"qPOH",				"fon"},
		{"XATATAEB_26",			"Khatataev26"},

		{"_31",	"_coma_31"},
		{"_32",	"_coma_32"},
		{"_181",	"_coma_181"},
		{"_182",	"_coma_182"},
		{"_fon",	"_coma_fon"},
		{"_coma_coma",	"_coma"},
	};
	const std::vector<std::pair<QString, QString>> renames
	{
		{"was1", "new1"},
		{"was2", "new2"}
	};
	repair::renameContents(workPath + "/" + subdir, renames);
#endif

#if 01
	repair::fullRepairDir(inPath, {});
#endif

#if 0
	/// physMinMax & holes
	repair::physMinMaxDir(inPath);
	repair::holesDir(inPath,
					 numChan,
					 inPath);
#endif

#if 0
	/// reref
	autos::rereferenceFolder(guyPath, reference::Ar);
#endif

#if 0
	/// refilter
	autos::refilterFolder(guyPath,
						  1.6,
						  30.);
#endif
	exit(0);
}

std::vector<QString> checkChannels(const QString & inPath, const std::vector<QString> & refChans)
{
	const std::vector<bool> res(refChans.size(), true);
	std::vector<bool> maxset(refChans.size(), true);

	auto subdirs = QDir(inPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	if(!QDir(inPath).entryList(def::edfFilters).isEmpty() || subdirs.isEmpty())
	{
		subdirs = QStringList{""};
	}

	for(const QString & subdir : subdirs)
	{
		const QString groupDir = inPath + "/" + subdir;

		auto guyDirs = QDir(groupDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		if(!QDir(groupDir).entryList(def::edfFilters).isEmpty() || guyDirs.isEmpty())
		{
			guyDirs = QStringList{""};
		}

		for(const QString & guyDir : guyDirs) /// each guy
		{
			const QString guyPath = groupDir + "/" + guyDir;
			auto filList = QDir(guyPath).entryList(def::edfFilters); /// edfs of one guy
			for(const QString & fl : filList)
			{
				edfFile file(guyPath + "/" + fl, true);
				auto tmp = file.hasChannels(refChans);

//				std::cout << fl << std::endl;

				if(res != tmp)
				{
					std::cout
							<< subdir << "\t"
							<< guyDir << "\t"
							<< fl << " absent: "
//							<< std::endl
							   ;
					for(int i = 0; i < refChans.size(); ++i)
					{
						if(!tmp[i]) { std::cout << refChans[i] << " "; }
						maxset[i] = maxset[i] & tmp[i];
					}
					std::cout << std::endl;
				}
				else
				{
//					std::cout << "OK: " <<  fl << std::endl;
				}
			}
		}
	}
	std::vector<QString> out{};
	std::cout << "maxset = " << std::endl;
	for(int i = 0; i < refChans.size(); ++i)
	{
		if(maxset[i])
		{
			out.push_back(refChans[i]);
			std::cout << refChans[i] << " ";
		}
	}
	std::cout << std::endl;
	return out;
}

void SettingsGalya()
{
	AUT_SETS.setAutosMask(0
						  | autos::feature::fft
						  | autos::feature::alphaPeak
						  | autos::feature::fracDim
						  | autos::feature::Hilbert
						  | autos::feature::Hjorth
//						  | autos::feature::wavelet
						  | autos::feature::logFFT
						  );
	AUT_SETS.setFeatureFilter(autos::feature::fracDim,
	{autos::filtFreqs.at(autos::filter::none)});

	AUT_SETS.setFeatureFilter(autos::feature::Hilbert,
	{autos::filtFreqs.at(autos::filter::none),
	 autos::filtFreqs.at(autos::filter::alpha)});

	AUT_SETS.setOutputStyle(autos::outputStyle::Line);
	AUT_SETS.setOutputSequence(autos::outputSeq::ByChans);
	AUT_SETS.setInitialCut(autos::initialCut::none);

	AUT_SETS.setChansProc(coords::lbl17noFP);
	AUT_SETS.setMarkers(subj::marksLists::tactileComa);
}

void GalyaProcessing(const QString & workPath)
{
	SettingsGalya();

#if 0
	/// checks channels presence
	const std::vector<bool> refChans(AUT_SETS.getChansProc().size(), true);

	auto subdirs = QDir(workPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	if(subdirs.isEmpty()) { subdirs = QStringList{""}; }
	for(const QString & subdir : subdirs)
	{
		const QString groupDir = workPath + "/" + subdir;
		auto guyDirs = QDir(groupDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		if(guyDirs.isEmpty()) { guyDirs = QStringList{""}; }

		for(const QString & guyDir : guyDirs) /// each guy
		{
			const QString guyPath = groupDir + "/" + guyDir;
			auto filList = QDir(guyPath).entryList(def::edfFilters); /// edfs of one guy
			for(const QString & fl : filList)
			{
				edfFile file(guyPath + "/" + fl, true);
				auto tmp = file.hasChannels(AUT_SETS.getChansProc());
				if(refChans != tmp)
				{
					std::cout << "not enough channels: " <<  fl << std::endl;
				}
				else
				{
//					std::cout << "OK: " <<  fl << std::endl;
				}
			}
		}
	}
	exit(0);
#endif

#if 0
	{
		/// each subject into his/her own folder
		auto subdirs = QDir(workPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		if(subdirs.isEmpty()) { subdirs = QStringList{""}; }
		for(const QString & subdir : subdirs)
		{
			autos::EdfsToFolders(workPath + "/" + subdir);
		}
	}
	exit(0);
#endif

#if 01
	/// calculation itself
	///
#if 01
	/// usual processing - each guy in his folder
	autos::ProcessByFolders(workPath,
							workPath + "_out",
							AUT_SETS.getChansProc(),
							AUT_SETS.getMarkers());
	autos::makeLabelsFile(AUT_SETS.getChansProc(),
						  workPath + "_out/labels.txt",
						  AUT_SETS.getMarkers(),
						  AUT_SETS.getAutosMask(),
						  "\t");
#endif

#if 01
	/// rhythm adoption
	for(const QString & subdir : QDir(workPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		for(const QString & stimType : {"sv", "zv"})
		{
			autos::rhythmAdoptionGroup(workPath + "/" + subdir,
									   "_fon",
									   stimType);
		}
	}
#endif

#if 0
	/// all in one
	autos::ProcessAllInOneFolder(workPath,
								 workPath + "_out",
								 AUT_SETS.getChansProc());
	autos::makeLabelsFile(AUT_SETS.getChansProc(),
						  workPath + "_out/labels.txt",
						  subj::marksLists::none,
						  AUT_SETS.getAutosMask(),
						  "\t");
#endif
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

void feedbackFinal()
{
	/// count correctness, average times, ICA
#if 0 /// new (~10 people)
	const QString dear = "FeedbackNewMark";
	const auto & guysList = subj::guysFBnew;
	const QString postfix = "_fin";
#else /// final (~16 people)
	const QString dear = "FeedbackFinalMark";
	const QString postfix = "_fin";
//	const QString postfix = "";
	const auto & guysList = subj::guysFBfinal.at(subj::fbGroup::all);
#endif

#if 01
	/// calculate successive for "New" and "Final" schemes on "New" data
	std::cout
			<< "NEW" << "\t"
			<< "FINAL" << "\t"
			<< "BASE" << "\t"
			<< std::endl;
	auto res = fb::calculateSuccessiveBoth(DEFS.dirPath() + "/" + dear, guysList, postfix);

//	for(const auto & in : res)
//	{
//		std::cout
//				<< std::get<0>(in).first << "\t"
//				<< std::get<1>(in).first << "\t"
//				<< std::get<2>(in).first << "\t"
//				<< std::endl;
//	}
	exit(0);
#endif

//	fb::calculateICA(DEFS.dirPath() + "/" + dear, guysList, postfix);

#if 0
	auto results = fb::coutAllFeatures(DEFS.dirPath() + "/" + dear, guysList, postfix);
	std::ofstream outStream("/media/Files/Data/FeedbackFinal/wlkdn.txt");
	for(const subj::fbGroup & group :
	{
		subj::fbGroup::experiment,
		subj::fbGroup::control,
		subj::fbGroup::improved,
		subj::fbGroup::not_improved}
		)
	{
		for(const auto & in : subj::guysFBfinal.at(group))
		{
			outStream << in.second << "\t" << results[in.second] << std::endl;
		}
		outStream << std::endl << std::endl;
	}
#endif

	exit(0);
}

void burdenkoAnnotsSimpler()
{
	const QString fold = "/media/Files/Data/Galya/Burd/Data";
	for(int i = 4; i <= 34; ++i) /// 19-34
	{
		const QString wrk = fold + "/" + rn(i, 2);
		QDir dr(wrk);

		const auto annots = dr.entryList({"*_annots.txt"})[0];
		QString outP = annots;
		outP.replace(".txt", "_simple.txt");

		QFile in(wrk + "/" + annots);	in.open(QIODevice::ReadOnly);
		QFile out(wrk + "/" + outP);	out.open(QIODevice::WriteOnly);
		auto text = in.readAll(); in.close();
		text.replace("Presentation stimulation code: 0 (stimPresentation)", "code0");
		text.replace("Presentation stimulation code: 1 (stimPresentation)", "code1");
		text.replace("stimPresentation", "stimStart");
		text.replace("eventButton", "buttonPressed");
		out.write(text);
		out.close();
	}
}

void burdenkoReadEdfPlus()
{
	const QString fold = "/media/Files/Data/Galya/Burd/Data";

	/// read EDF+ and write annotations
	for(int i = 4; i <= 34; ++i) /// 19-34
	{
		const QString wrk = fold + "/" + rn(i, 2);
		QDir dr(wrk);
		const auto edfPlus = dr.entryList({"*.edf"});					/// logile name
		for(const auto & edf : edfPlus)
		{
			if(!edf.contains("_new"))
			{
				edfFile fil(wrk + "/" + edf);
			}
		}
	}
}

void burdenkoEdfPlusToEdf()
{

	const QString fold = "/media/Files/Data/Galya/Burd/Data";

	/// Burdenko Edf+ and log to simple EDF
	for(int i = 4; i <= 34; ++i) /// 19-34
	{
		const QString wrk = fold + "/" + rn(i, 2);
		QDir dr(wrk);
		const auto log = dr.entryList({"*.log"})[0];					/// logile name
		auto picNums = myLib::readBurdenkoLog(wrk + "/" + log);			/// p0513 -> 0513

		const auto edfPlus = dr.entryList({"*.edf"})[0];				/// edfPlus name
		edfFile fil(wrk + "/" + edfPlus);								/// read file

		auto annots = myLib::handleBurdenkoAnnots(fil.getAnnotations(), 500);	/// annots to {timeBin, marker}

		auto it1 = std::begin(annots);
		for(const auto & picNum : picNums)
		{
			auto itNext = std::find_if(it1, std::end(annots),
									   [](const auto & ann)
			{ return ann.second == 15; }); /// magic const means "stimPresentation" from myLib::handleAnnots

			(*itNext).second = picNum.toInt();
			it1 = itNext; ++it1;
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
	}
}

void burdenkoReadEdf()
{
	const QString fold = "/media/Files/Data/Galya/Burd/Data";

	/// read EDF+ and write annotations
	for(int i = 4; i <= 34; ++i) /// 19-34
	{
		const QString wrk = fold + "/" + rn(i, 2);
		QDir dr(wrk);
		const auto edf = dr.entryList({"*_new.edf"})[0];
		edfFile fil(wrk + "/" + edf);

		std::cout << edf << std::endl;
		for(int mrk : {10, 11, 20, 15})
		{
			std::cout << mrk << " num = " << fil.countMarker(mrk) << std::endl;
		}
		std::cout << std::endl;
	}
}

void burdenkoReactionTime()
{
	const QString fold = "/media/Files/Data/Galya/Burd/Data";
	for(int i = 4; i <= 34; ++i) /// 19-34
	{
		const QString wrk = fold + "/" + rn(i, 2);
		QDir dr(wrk);

		const auto edf = dr.entryList({"*_new.edf"})[0];			/// simple edf
		QString log = edf;
		log.replace(".edf", "_reac.txt");

		edfFile fil(wrk + "/" + edf);
		std::ofstream outStr((wrk + "/" + log).toStdString());

		auto mrks = fil.getMarkers();
		for(int i = 0; i < mrks.size(); ++i)
		{
			if(mrks[i].second == 11			/// 11 - magic const (code = 1, self face)
			   && mrks[i + 2].second != 20	/// 20 - magic const (eventButton) +2 - overnext marker
			   )
			{
				outStr
						<< mrks[i + 1].second << "\t"										/// picNum
						<< (mrks[i + 2].first - mrks[i + 1].first) / fil.getFreq() << "\t"	/// reac time
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

