#include <other/feedback.h>
#include <widgets/net.h>

namespace fb
{

/// other functions and autos
void coutAllFeatures(const QString & dear,
					 const std::vector<std::pair<QString, QString> > & guysList,
					 const QString & postfix)
{
	const QString guysPath = DEFS.dirPath() + "/" + dear;

//	Net * net = new Net();

	for(const auto & in : guysList)
	{
		const QString guyPath = guysPath + "/" + in.first;
		if(!QDir(guyPath).exists()) { continue; }


		fb::FeedbackClass fbItem(guyPath, in.second, postfix); if(!fbItem) { continue; }

		/// ExpName
		std::cout << in.second << "\t";

//		FBedf fil(guyPath + "/" + in.second + "_1.edf",
//				  guyPath + "/" + in.second + "_ans1.txt");
//		for(double time = 3.; time <= 6.; time += 0.5)
//		{
//			std::cout << fil.getNumInsights(time) << "\t";
//		}

		/// stats of solving and times
//		fbItem.writeStat();											std::cout.flush(); /// 23
//		fbItem.writeDists();										std::cout.flush(); /// 6
//		fbItem.writeKDEs(guyPath + "/" + in.second + "_");			std::cout.flush();
//		fbItem.writeShortLongs(guyPath + "/" + in.second + "_");	std::cout.flush();
//		fbItem.writeRightWrong(guyPath + "/" + in.second + "_");	std::cout.flush();
//		fbItem.writeClass();										std::cout.flush(); /// 6
//		fbItem.writeSuccessive();									std::cout.flush();
//		fbItem.writeLearnedPatterns();								std::cout.flush();
//		fbItem.writeSuccessive3();									std::cout.flush();
//		fbItem.writePartOfCleaned();								std::cout.flush();

		/// new things
		/// compare with background
		fbItem.writeBackgroundCompare(fb::taskType::spat, fb::ansType::correct);		/// 3 pics
		fbItem.writeBackgroundCompare(fb::taskType::verb, fb::ansType::correct);		/// 3 pics

		/// normalized dispersions
//		fbItem.writeDispersions(ansType::correct);		std::cout.flush();	/// 9 vals

		/// classify winds/reals by alpha
//		fbItem.writeClass(false);						std::cout.flush();	/// 6 vals
//		fbItem.writeClass(true);						std::cout.flush();	/// 6 vals

		/// distances only for correctly solved
//		fbItem.writeDists(ansType::correct);			std::cout.flush();	/// 9 vals

		for(fb::fileNum fileN : {fb::fileNum::first, fb::fileNum::second, fb::fileNum::third} )
		{
			for(fb::taskType taskT : {fb::taskType::spat, fb::taskType::verb} )
			{
				for(fb::ansType ansT : {fb::ansType::correct, fb::ansType::all} )
				{
//					net->innerClassHistogram(fbItem.getFile(static_cast<int>(fileN)),
//											 taskT,
//											 ansT);
				}
			}
		}

		exit(0);

		///
		std::cout << std::endl;


		/// write solving stats into txt file
//		fbItem.writeFile();
	}
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