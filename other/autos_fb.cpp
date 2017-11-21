#include <other/autos.h>

#include <myLib/output.h>
#include <myLib/statistics.h>
#include <myLib/valar.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>

/// for successive preclean
#include <widgets/net.h>

 /// for std::defaultfloat
#include <ios>
#include <iostream>
#include <bits/ios_base.h>

using namespace myOut;

namespace autos
{
void checkMarkFBfinal(const QString & edfPath)
{
	/// checks number of 241, 247, 254 markers
	/// and couts possible missings

	if(!QFile(edfPath).exists())
	{
		return;
	}

	/// markers check
	edfFile fil;
	fil.readEdfFile(edfPath);
	std::cout << fil.getExpName() << std::endl;

	bool ok = true;

	std::vector<uint> a = fil.countMarkers({241, 247, 254});
	if(a != std::vector<uint>{40, 40, 80})
	{
		std::cout << a << std::endl;
	}

	std::pair<int, int> prev{0, 254};
	int taskNum = 1;
	for(std::pair<int, int> in : fil.getMarkers())
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
			ok = false;
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
			ok = false;
		}
		prev = in;
	}
	if(ok) { std::cout << "markers are OK" << std::endl; }
}

void checkStatTimesSolving(const QString & guyPath, const QString & guyName, int typ)
{
	/// couts statistics of solving time

	std::valarray<double> vals1 = smLib::vecToValar(autos::timesFromFile(guyPath + "/"
											  + guyName + "_times_1_" + nm(typ) + ".txt", 1));
	std::valarray<double> vals2 = smLib::vecToValar(autos::timesFromFile(guyPath + "/"
											  + guyName + "_times_3_" + nm(typ) + ".txt", 1));
	if(typ == 241)		{ std::cout << "spatTime:" << "/n"; }
	else if(typ == 247)	{ std::cout << "verbTime:" << "/n"; }
	else				{ std::cout << "checkStatTimesSolving: bad typ = "
									<< typ << std::endl; return; }

	switch(myLib::MannWhitney(vals1, vals2, 0.05))
	{
	case 0: { std::cout << "not different"; break; }
	case 1: { std::cout << "time1 > time2"; break; }
	case 2: { std::cout << "time1 < time2"; break; }
	default: { break; }
	}
	std::cout << "\t"
			  << "av.acceleration = "
			  << (smLib::mean(vals1) - smLib::mean(vals2)) / smLib::mean(vals1)
			  << std::endl;
}

void checkStatTimes(const QString & guyPath, const QString & guyName)
{
	/// couts statistics of solving - number of solved tasks and av.time

	const QString timesPath = guyPath + "/" + guyName + "_timesResults.txt";

	QFile inFil(timesPath);
	if(!inFil.exists())
	{
		std::cout << "checkStatTimes: no " << guyName << "_timesResults.txt file" << std::endl;
		return;
	}
	inFil.open(QIODevice::ReadOnly);

	std::vector<std::vector<double>> vals(3);
	for(int i = 0; i < 3; ++i)
	{
		auto lst = QString(inFil.readLine()).split("\t", QString::SkipEmptyParts);
		bool ok{};
		for(const QString & str : lst)
		{
			double val = str.toDouble(&ok);
			if(ok) { vals[i].push_back(val); }
			else   { vals[i].push_back(-1); }		/// for nan
		}
	}
	inFil.close();

	/// spatial
	/// 0, 1, 2 - mean, median, sigma of correct
	/// 3, 4, 5 - mean, median, sigma of incorrect
	/// 6, 7, 8 - mean, median, sigma of answered
	/// 9, 10, 11 - correct, incorrect, not answered
	/// verbal
	/// 12, 13, 14 - mean, median, sigma of correct
	/// 15, 16, 17 - mean, median, sigma of incorrect
	/// 18, 19, 20 - mean, median, sigma of answered
	/// 21, 22, 23 - correct, incorrect, not answered

	std::cout << std::fixed;
	std::cout.precision(2);

	std::cout << guyName << std::endl;

	/// SPAT
	std::cout << "spatNum:" << "\n"
			  << "p-value = " <<  myLib::binomialOneTailed(vals[0][9], vals[2][9], 40)
			<< "\t" << "improvement = " << (vals[2][9] - vals[0][9]) / vals[0][9]
			<< std::endl << std::endl;

//	/// can check _times.txt files and apply Mann-Whitney for significance
//	std::cout << "spatTime:" << "\n"
//			  << "av.acceleration = " << (vals[0][0] - vals[2][0]) / vals[0][0]
//			<< std::endl << std::endl;
	autos::checkStatTimesSolving(guyPath, guyName, 241);



	/// VERB
	std::cout << "verbNum:" << "\n"
			  << "p-value = " <<  myLib::binomialOneTailed(vals[0][21], vals[2][21], 40)
			<< "\t" << "improvement = " << (vals[2][21] - vals[0][21]) / vals[0][21]
			<< std::endl << std::endl;

//	/// can check _times.txt files and apply Mann-Whitney for significance
//	std::cout << "verbTime:" << "\n"
//			  << "av.acceleration = " << (vals[0][12] - vals[2][12]) / vals[0][12]
//			<< std::endl << std::endl;
	autos::checkStatTimesSolving(guyPath, guyName, 247);

	std::cout << std::endl;
	std::cout << std::defaultfloat;
}

void feedbackFinalTimes(const QString & edfsPath,
						const QString & expName,
						const QString & postfix)
{
	/// creates *_timesResults.txt file
	/// spatial
	/// 0, 1, 2 - mean, median, sigma of correct
	/// 3, 4, 5 - mean, median, sigma of incorrect
	/// 6, 7, 8 - mean, median, sigma of answered
	/// 9, 10, 11 - correct, incorrect, not answered
	/// verbal
	/// 12, 13, 14 - mean, median, sigma of correct
	/// 15, 16, 17 - mean, median, sigma of incorrect
	/// 18, 19, 20 - mean, median, sigma of answered
	/// 21, 22, 23 - correct, incorrect, not answered

	std::ofstream outStr;
	outStr.open((edfsPath + "/" + expName + "_timesResults.txt").toStdString());

	for(int numSess : {1, 2, 3})
	{
		auto filePath = [edfsPath, expName, postfix](int i) -> QString
		{
			return edfsPath + "/" + expName + "_" + nm(i) + postfix + ".edf";
		};

		if(!QFile::exists(filePath(numSess))) { continue; }

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
		/// spatial
		/// 0, 1, 2 - mean, median, sigma of correct
		/// 3, 4, 5 - mean, median, sigma of incorrect
		/// 6, 7, 8 - mean, median, sigma of answered
		/// 9, 10, 11 - correct, incorrect, not answered
		/// verbal
		/// 12, 13, 14 - mean, median, sigma of correct
		/// 15, 16, 17 - mean, median, sigma of incorrect
		/// 18, 19, 20 - mean, median, sigma of answered
		/// 21, 22, 23 - correct, incorrect, not answered

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

			for(int ans : {1, 2, 0}) /// corr, incorr, skipped
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

void createAnsFiles(const QString & dirPath, QString guy)
{
	for(int i : {1, 2, 3})
	{
		QFile fil(dirPath + "/" + guy + "_ans" + nm(i) + ".txt");
		fil.open(QIODevice::WriteOnly);
		fil.close();
	}
}

void timesSolving(const QString & guyPath,
				  const QString & guyName,
				  const QString & postfix)
{
	/// creates _times_numSes_241/247.txt files with correctness and solving time

	for(int numSession : {1, 3})
	{
		const QString filePath = guyPath + "/"
							  + guyName + "_" + nm(numSession) + postfix + ".edf";

		edfFile fil;
		fil.readEdfFile(filePath);
		const std::vector<std::pair<int, int>> & markers = fil.getMarkers();




		int sta = 0;
		int fin = 0;

		const std::vector<int> nums = {241, 247};
		int typ = -1;

		/// remove old files
		for(int i : nums)
		{
			QFile::remove(guyPath
						  + guyName + "_times_"
						  + nm(numSession) + "_"
						  + nm(nums[i]) + ".txt");
		}

		std::ifstream answers;
		answers.open((guyPath
					  + "/" + guyName + "_ans"
					  + nm(numSession) + ".txt").toStdString());

		std::ofstream out;
		for(const std::pair<int, int> & mrk : markers)
		{
			int mark = mrk.second;

			if(mark == nums[0] || mark == nums[1])
			{
				sta = mrk.first;

				if(mark == 241) typ = 0;
				else typ = 1;
			}
			else if(mark == 254)
			{
				fin = mrk.first;

				char ans;
				answers >> ans;
				if(ans == '\n' || ans == '\r') answers >> ans; /// skip bad symbols

				out.open((guyPath
						  + guyName + "_times_"
						  + nm(numSession) + "_"
						  + nm(nums[typ]) + ".txt").toStdString(), std::ios_base::app);
				out << ans << "\t"
					<< (fin - sta) / fil.getFreq() << "\r\n";
				out.close();
			}
		}
		answers.close();
	}
}

std::vector<double> timesFromFile(const QString & timesPath, int howSolved)
{
	/// data from *_times_numSes_241/247.txt file into vector

	QFile fil(timesPath);
	if(!fil.exists())
	{
		std::cout << "timesFromFile: file not found " << timesPath << std::endl;
		return {};
	}
	fil.open(QIODevice::ReadOnly);

	std::vector<double> res;

	while(fil.canReadLine())
	{
		QStringList lst = QString(fil.readLine()).split("\t", QString::SkipEmptyParts);
		if(lst[0].toInt() == howSolved)
		{
			res.push_back(lst[1].toDouble());
		}
	}
	fil.close();
	return res;
}



/// CHECK, what it does
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


void successiveNetPrecleanWinds(const QString & windsPath)
{
	Net * ann = new Net();
	ann->loadData(windsPath, {"*.psd"});
	ann->setClassifier(ModelType::ANN);
	ann->successivePreclean(windsPath, {});
	delete ann;
}


} // end namespace autos