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
	int howSolved = 1; /// 0 - skip, 1 - correct, 2 - incorrect
	std::valarray<double> vals1 = smLib::vecToValar(
									  autos::timesFromFile(
										  autos::timesPath(guyPath, guyName, 1, typ),
										  howSolved)
									  );
	std::valarray<double> vals2 = smLib::vecToValar(
									  autos::timesFromFile(
										  autos::timesPath(guyPath, guyName, 3, typ),
										  howSolved)
									  );
	if(typ == 241)		{ std::cout << "spatTime:" << "\n"; }
	else if(typ == 247)	{ std::cout << "verbTime:" << "\n"; }
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

void checkStatResults(const QString & guyPath, const QString & guyName)
{
	/// couts statistics of solving - number of solved tasks and av.time

	const QString timesPath = guyPath + "/" + guyName + "_timesResults.txt";

	QFile inFil(timesPath);
	if(!inFil.exists())
	{
		std::cout << "checkStatTimes: timesResults file not found " << guyName << std::endl;
		return;
	}
	inFil.open(QIODevice::ReadOnly);

	std::vector<std::vector<double>> vals(3);
	for(int i = 0; i < 3; ++i)
	{
		if(inFil.atEnd())
		{
			std::cout << "checkStatResults: timesResults file atEnd "
					  << guyName << " " << i << std::endl;
			return;
		}
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
	autos::checkStatTimesSolving(guyPath, guyName, 241); std::cout << std::endl;



	/// VERB
	std::cout << "verbNum:" << "\n"
			  << "p-value = " <<  myLib::binomialOneTailed(vals[0][21], vals[2][21], 40)
			<< "\t" << "improvement = " << (vals[2][21] - vals[0][21]) / vals[0][21]
			<< std::endl << std::endl;
	autos::checkStatTimesSolving(guyPath, guyName, 247);


	std::cout << std::endl;
	std::cout << std::defaultfloat;


}

void feedbackFinalTimes(const QString & guyPath,
						const QString & guyName,
						const QString & postfix)
{
	/// creates timesResults.txt file
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
	outStr.open((guyPath + "/" + guyName + "_timesResults.txt").toStdString());

	for(int numSess : {1, 2, 3})
	{
		auto filePath = [guyPath, guyName, postfix](int i) -> QString
		{
			return guyPath + "/" + guyName + "_" + nm(i) + postfix + ".edf";
		};

		if(!QFile::exists(filePath(numSess)))
		{
			std::cout << "feedbackFinalTimes: file not found - " << filePath(numSess) << std::endl;
			continue;
			return;
		}

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
		const QString ansPath = guyPath + "/" + guyName + "_ans" + nm(numSess) + ".txt";
		if(!QFile::exists(ansPath))
		{
			std::cout << "feedbackFinalTimes: file not found - " << ansPath << std::endl;
			continue;
			return;
		}
		answers.open(ansPath.toStdString());
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

void createAnsFiles(const QString & guyPath, QString guyName)
{
	for(int i : {1, 2, 3})
	{
		QFile fil(guyPath + "/" + guyName + "_ans" + nm(i) + ".txt");
		fil.open(QIODevice::WriteOnly);
		fil.close();
	}
}

QString timesPath(const QString & guyPath,
				  const QString & guyName,
				  int numSes,
				  int typ)
{
	return guyPath + "/"
			+ guyName + "_times_"
			+ nm(numSes) + "_"
			+ nm(typ) + ".txt";
}

void timesSolving(const QString & guyPath,
				  const QString & guyName,
				  const QString & postfix)
{
	/// creates times files with correctness and solving time

	for(int numSession : {1, 3})
	{
		const QString filePath = guyPath + "/"
							  + guyName + "_" + nm(numSession) + postfix + ".edf";
		if(!QFile::exists(filePath))
		{
			std::cout << "timesSolving: edf file not found - " << filePath << std::endl;
			continue;
			return;
		}

		edfFile fil;
		fil.readEdfFile(filePath);
		const std::vector<std::pair<int, int>> & markers = fil.getMarkers();




		int sta = 0;
		int fin = 0;

		const std::vector<int> nums = {241, 247};
		int typ = -1;

		/// remove old times files
		for(int i : nums)
		{
			QFile::remove(autos::timesPath(guyPath, guyName, numSession, i));
		}

		std::ifstream answers;
		const QString ansPath = guyPath
								+ "/" + guyName + "_ans"
								+ nm(numSession) + ".txt";
		if(!QFile::exists(ansPath))
		{
			std::cout << "timesSolving: ans file not found - " << ansPath << std::endl;
			continue;
			return;
		}

		answers.open(ansPath.toStdString());

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

				out.open((autos::timesPath(guyPath, guyName, numSession, nums[typ])).toStdString(), std::ios_base::app);
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
	/// data from times file into vector

	QFile fil(timesPath);
	if(!fil.exists())
	{
		std::cout << "timesFromFile: file not found " << timesPath << std::endl;
		return {};
	}
	fil.open(QIODevice::ReadOnly);

	std::vector<double> res;

	int numRead = 0;
	while(!fil.atEnd())
	{
		QString A = fil.readLine();
		if(A.isEmpty()) { break; }
		++numRead;
		QStringList lst = A.split("\t", QString::SkipEmptyParts);

		if(lst[0].toInt() == howSolved)
		{
			res.push_back(lst[1].toDouble());
		}
	}
//	std::cout << "timesFromFile: num lines read = " << numRead << std::endl;
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

void FeedbackClass::countTimes()
{
	auto filePath = [this](int numSes) -> QString
	{
		return this->guyPath + "/" + this->guyName + "_" + nm(numSes) + this->postfix + ".edf";
	};
	std::vector<int> numSes = {1, 3};

	for(int j : numSes)
	{
		if(!QFile::exists(filePath(j)))
		{
			std::cout << "FeedbackClass::countTimes: file not found - "
					  << filePath(j) << std::endl;
			return;
		}
	}
	for(int j : numSes)
	{
		if(!QFile::exists(ansPath(j)))
		{
			std::cout << "FeedbackClass::countTimes: file not found - "
					  << ansPath(j) << std::endl;
			return;
		}
	}

	for(int i : {0, 1})
	{
		/// read markers
		edfFile fil;
		fil.readEdfFile(filePath(numSes[i]));
		const std::vector<std::pair<int, int>> & markers = fil.getMarkers();

		std::vector<int> corrs = readAnsFile(numSes[i]);

		/// calc times
		for(auto tim : times[i]) { tim.clear(); } /// clear previous
		int count = 0;
		int sta = 0;
		int num = 0;
		std::vector<int> nums = {241, 247};
		for(const std::pair<int, int> & mrk : markers)
		{
			int mark = mrk.second;

			if(myLib::contains(nums, mark))
			{
				sta = mrk.first;
				num = ((mark == 241) ? 0 : 1);
			}
			else if(mark == 254)
			{
				times[i][num].push_back(std::make_pair(corrs[count++],
										(mrk.first - sta) / fil.getFreq())
						);
			}
		}
		for(auto tim : times[i])
		{
			if(tim.size() != numTasks)
			{
				std::cout << "FeedbackClass::countTimes: unexpected num of tasks = "
						  << tim.size() << ", expected = " << numTasks
						  << ", numSes = " << numSes[i] << std::endl;
			}
		}
	}
}

std::valarray<double> FeedbackClass::timesToArray(taskType typ, fileNum filNum, ansType howSolved)
{
	std::vector<double> res;
	for(auto in : times[int(filNum)][int(typ)])
	{
		if(in.first == int(howSolved)) { res.push_back(in.second); }
	}
	return smLib::vecToValar(res);
}

std::vector<int> FeedbackClass::readAnsFile(int numSes)
{
	/// read answers
	std::vector<int> corrs; corrs.reserve(80);
	std::ifstream answers;

	answers.open(ansPath(numSes).toStdString());
	char ans;
	while(answers >> ans)
	{
		if(ans == '\n' || ans == '\r') { answers >> ans; }
		bool ok{};
		int tmp = QString(ans).toInt(&ok);

		if(ok) { corrs.push_back(tmp); }
		else
		{
			std::cout << "FeedbackClass::readAnsFile: wrong char in file = "
					  << ans << std::endl;
		}
	}
	answers.close();

	if(corrs.size() != 2 * numTasks)
	{
		std::cout << "FeedbackClass::readAnsFile: wrong num of anwers = " << corrs.size()
				  << ", expected = " << 2 * numTasks << std::endl;
	}

	return corrs;
}

void FeedbackClass::checkStatTimes(taskType typ, ansType howSolved)
{
	std::valarray<double> vals1 = timesToArray(typ, fileNum::first, howSolved);
	std::valarray<double> vals2 = timesToArray(typ, fileNum::third, howSolved);


	if(typ == taskType::spat)		{ std::cout << "spatTime:" << "\n"; }
	else if(typ == taskType::verb)	{ std::cout << "verbTime:" << "\n"; }

	switch(myLib::MannWhitney(vals1, vals2, 0.05))
	{
	case 0: { std::cout << "not different"; break; }
	case 1: { std::cout << "time1 > time3"; break; }
	case 2: { std::cout << "time1 < time3"; break; }
	default: { break; }
	}

	std::cout << "\t"
			  << "av.acceleration = "
			  << (smLib::mean(vals1) - smLib::mean(vals2)) / smLib::mean(vals1)
			  << std::endl;
}

void FeedbackClass::checkStatSolving(taskType typ, ansType howSolved)
{
	int num1 = 0;
	for(auto in : times[int(fileNum::first)][int(typ)])
	{
		if(in.first == int(howSolved)) { ++num1; }

	}
//	std::cout << std::endl;
	int num2 = 0;
	for(auto in : times[int(fileNum::third)][int(typ)])
	{
		if(in.first == int(howSolved)) { ++num2; }
	}

	if(typ == taskType::spat)		{ std::cout << "spatTime:" << "\n"; }
	else if(typ == taskType::verb)	{ std::cout << "verbTime:" << "\n"; }

	std::cout << "p-value = " <<  myLib::binomialOneTailed(num1, num2, numTasks)
			  << "\t" << "improvement = " << double(num2 - num1) / num1
			  << std::endl;
}

void FeedbackClass::checkStat()
{
	std::cout << std::fixed;
	std::cout.precision(2);
	for(auto typ : {taskType::spat, taskType::verb})
	{
		checkStatSolving(typ, ansType::right);
		std::cout << std::endl;
		checkStatTimes(typ, ansType::right);
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::defaultfloat;
}

void FeedbackClass::writeFile()
{
	std::ofstream outStr;
	outStr.open((this->guyPath + "/" + this->guyName + "_timesResults_new.txt").toStdString());

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


	for(int i = 0; i < 2; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			const auto & tsk = times[i][j];

			outStr << std::fixed;
			outStr.precision(1);

			for(std::vector<int> ans : {
			std::vector<int>{int(ansType::right)},
			std::vector<int>{int(ansType::wrong)},
			std::vector<int>{int(ansType::right), int(ansType::wrong)}
		})
			{
				std::vector<double> vals{};

				std::for_each(std::begin(tsk),
							  std::end(tsk),
							  [&vals, ans](const auto & in)
				{
					if(myLib::contains(ans, std::get<0>(in)))
					{
						vals.push_back(std::get<1>(in));
					}
				});
				auto vl = smLib::vecToValar(vals);
				outStr << smLib::mean(vl) << "\t"
					   << smLib::median(vl) << "\t"
					   << smLib::sigma(vl) << "\t";
			}
			outStr << std::defaultfloat;

			for(ansType ans : {ansType::right, ansType::wrong, ansType::skip})
			{
				outStr << std::count_if(std::begin(tsk),
										std::end(tsk),
										[ans](const auto & in)
				{
					return std::get<0>(in) == int(ans);
				});
				outStr << "\t";
			}
		}
		outStr << "\r\n";
	}
	outStr.close();
}

QString FeedbackClass::ansPath(int numSes)
{
	return this->guyPath + "/" + this->guyName + "_ans" + nm(numSes) + ".txt";
}


} // end namespace autos
