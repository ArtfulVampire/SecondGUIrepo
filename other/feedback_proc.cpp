#include <other/feedback.h>
#include <widgets/net.h>
#include <sstream>

namespace fb
{

taskType & operator ++(taskType & in) /// prefix
{
	return in = (in == taskType::rest)
				? taskType::spat
				: static_cast<taskType>(static_cast<int>(in) + 1);
}

taskType operator ++(taskType & in, int) /// postfix
{
	auto res = in;
	++in;
	return res;
}

QString toStr(taskType in)
{
	static const std::map<taskType, QString> res
	{
		{taskType::spat, "spat"},
		{taskType::verb, "verb"},
		{taskType::rest, "rest"},
	};
	return res.at(in);
}
QString toStr(fileNum in)
{
	static const std::map<fileNum, QString> res
	{
		{fileNum::first, "1"},
		{fileNum::second, "2"},
		{fileNum::third, "3"},
	};
	return res.at(in);
}
QString toStr(ansType in)
{
	{
		static const std::map<ansType, QString> res
		{
			{ansType::skip,		"skip"},
			{ansType::correct,	"correct"},
			{ansType::notwrong,	"notwrong"},
			{ansType::wrong,	"wrong"},
			{ansType::bad,		"bad"},
			{ansType::answrd,	"answrd"},
			{ansType::all,		"all"},
		};
		return res.at(in);
	}
}

/// make return
std::map<QString, QString>
coutAllFeatures(const QString & dear,
				const std::vector<std::pair<QString, QString>> & guysList,
				const QString & postfix)
{
	const QString guysPath = DEFS.dirPath() + "/" + dear;

	std::map<QString, QString> results{};

	Net * net = new Net();

	bool pass = true;
	for(const auto & in : guysList)
	{
		const QString guyPath = guysPath + "/" + in.first;
		if(!QDir(guyPath).exists()) { continue; }

		if(in.second == "BDA") { pass = false; }
		if(pass) { continue; }

//		if(in.second = "AKV") { continue; }


		fb::FeedbackClass fbItem(guyPath, in.second, postfix); if(!fbItem) { continue; }

		std::ostringstream oss{};
		oss.precision(4);
		fbItem.setOstream(oss);

		/// ExpName
		std::cout << in.second << "\t"; std::cout.flush();

		/// insights with different thresholds
//		FBedf fil(guyPath + "/" + in.second + "_1.edf",
//				  guyPath + "/" + in.second + "_ans1.txt");
//		for(double time = 3.; time <= 6.; time += 0.5)
//		{
//			std::cout << fil.getNumInsights(time) << "\t";
//		}

		/// stats of solving and times
//		fbItem.writeStat();											std::cout.flush(); /// 23 vals
//		fbItem.writeDists();										std::cout.flush(); /// 6 vals
//		fbItem.writeClass();										std::cout.flush(); /// 6 vals
//		fbItem.writeSuccessive();									std::cout.flush(); /// 1 val
//		fbItem.writeLearnedPatterns();								std::cout.flush(); /// 2 vals
//		fbItem.writeSuccessive3();									std::cout.flush(); /// 1 val
//		fbItem.writePartOfCleaned();								std::cout.flush(); /// 1 val
//		fbItem.writeKDEs(guyPath + "/" + in.second + "_");			std::cout.flush(); /// 4 pics
//		fbItem.writeShortLongs(guyPath + "/" + in.second + "_");	std::cout.flush(); /// 6 pics
//		fbItem.writeRightWrong(guyPath + "/" + in.second + "_");	std::cout.flush(); /// 6 pics

		/// new things
		/// compare with background
//		fbItem.writeBackgroundCompare(fb::taskType::spat, fb::ansType::correct);		/// 3 pics
//		fbItem.writeBackgroundCompare(fb::taskType::verb, fb::ansType::correct);		/// 3 pics

		/// normalized dispersions
		fbItem.writeDispersions(ansType::correct);		std::cout.flush();	/// 18 vals

		/// classify winds/reals by alpha
//		fbItem.writeClass(false);						std::cout.flush();	/// 6 vals
//		fbItem.writeClass(true);						std::cout.flush();	/// 6 vals

		/// distances only for correctly solved
//		fbItem.writeDists(ansType::correct);			std::cout.flush();	/// 9 vals


//		net->innerClassHistogram(fbItem.getFile(fb::fileNum::second), taskType::verb, ansType::correct);


		for(fb::fileNum fileN : {fb::fileNum::first, fb::fileNum::second, fb::fileNum::third} )
		{
			for(fb::taskType taskT : {fb::taskType::spat, fb::taskType::verb} )
			{
				for(fb::ansType ansT : {fb::ansType::correct, fb::ansType::all} )
				{
//					continue; /// dont do histograms

					if(fbItem.getFile(fileN).getNum(taskT, ansT) == 0) { continue; }

					net->innerClassHistogram(fbItem.getFile(fileN), taskT, ansT);
				}
			}
		}
		results[in.second] = QString(oss.str().c_str());
//		break;
	}
	std::cout << std::endl;
	delete net;
	return results;
}

void calculateICA(const QString & dear,
				  const std::vector<std::pair<QString, QString>> & guysList,
				  const QString & postfix)
{
	const QString guysPath = DEFS.dirPath() + "/" + dear;
	for(const auto & in : guysList)
	{
		const QString guyPath = guysPath + "/" + in.first;
		if(!QDir(guyPath).exists()) { continue; }

		fb::FeedbackClass fbItem(guyPath, in.second, postfix); if(!fbItem) { continue; }

		/// ExpName
		std::cout << in.second << "\t";

		fbItem.calculateICAs();
	}
}

void checkMarkFBfinal(const QString & filePath)
{
	/// checks number of 241, 247, 254 markers
	/// and couts possible missings

	if(!QFile::exists(filePath))
	{
		return;
	}

	/// markers check
	edfFile fil;
	fil.readEdfFile(filePath);
	std::cout << fil.getExpName() << std::endl;

	std::vector<uint> a = fil.countMarkers({241, 247, 254});
	if(a != std::vector<uint>{40, 40, 80})
	{
		std::cout << a << std::endl;
	}
	else
	{
		std::cout << "markers count is OK" << std::endl;
		return; /// ?
	}

	std::pair<int, int> prev{0, 254};
	int taskNum = 1;
	for(const std::pair<int, int> & in : fil.getMarkers())
	{
		/// only interesting marks
		if(in.second != 241 && in.second != 247 && in.second != 254) { continue; }


		/// rest min = 7.6, mean = 8.4, max = 9.2
		/// after cross min = 0.54, mean = 0.63, max = 0.75
		if(in.second == 254 && prev.second == 254)
		{
			std::cout << "missed 241/247" << "\t"
					  << "approx time = " << prev.first / fil.getFreq() + 8.4
					  << "\ttask " << taskNum << " start"
					  << std::endl;
		}
		if(in.second == 254) { ++taskNum; }

		if((in.second == 241 || in.second == 247) &&
		   (prev.second == 241 || prev.second == 247))
		{
			std::cout << "missed 254" << "\t"
					  << "approx time = " << in.first / fil.getFreq() - 8.4
					  << "\ttask " << taskNum << " finish"
					  << std::endl;
			++taskNum;
		}
		prev = in;
	}
}


void createAnsFiles(const QString & guyPath, QString guyName)
{
	for(int i : {1, 2, 3})
	{
		QFile fil(guyPath + "/" + guyName + "_ans" + nm(i) + ".txt");
		fil.open(QIODevice::WriteOnly);
		fil.close();
	}
}


/// CHECK, what it does
void repairMarkersInNewFB(QString edfPath, int numSession)
{
	/// repair markers in my files

	edfFile fil;
	fil.readEdfFile(edfPath);

	const std::valarray<double> & mrk = fil.getMarkArr();
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
	/// magic const string
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

void successiveNetPrecleanWinds(const QString & windsPath)
{
	Net * ann = new Net();
	ann->loadData(windsPath, {"*.psd"});
	ann->setClassifier(ModelType::ANN);
	ann->successivePreclean(windsPath, {});
	delete ann;
}
} // end namespace fb
