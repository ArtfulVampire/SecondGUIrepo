#include <other/feedback.h>

#include <myLib/statistics.h>
#include <widgets/net.h>

#include <utility>

namespace fb
{
/// FeedbackClass
FeedbackClass::FeedbackClass(QString guyPath_,
							 QString guyName_,
							 QString postfix_)
	: guyPath(std::move(guyPath_)), guyName(std::move(guyName_)), postfix(std::move(postfix_))
{
	isGood = false;
	auto filePath = [this](int numSes) -> QString
	{
		return guyPath + "/" + guyName + "_" + nm(numSes) + postfix + ".edf";
	};
	auto ansPath = [this](int numSes) -> QString
	{
		return guyPath + "/" + guyName + "_ans" + nm(numSes) + ".txt";
	};

	for(int i : {1, 2, 3})
	{
		int fileNumber{};
		switch(i)
		{
		case 1: { fileNumber = static_cast<int>(fileNum::first);		break; }	/// 0
		case 2: { fileNumber = static_cast<int>(fileNum::second);		break; }	/// 2
		case 3: { fileNumber = static_cast<int>(fileNum::third);		break; }	/// 1
		}

		files[fileNumber] = FBedf(filePath(i),
								  ansPath(i),
								  suc::overlap125,
								  0); //// 0. or suc::overlap125 ?????

		if(!files[fileNumber])
		{
			std::cout << "badFile: " << filePath(i) << std::endl;
			this->isGood = false;
			return;
		}
	}
	isGood = true;
}

void FeedbackClass::checkStatTimes(taskType typ, ansType howSolved)
{
	std::valarray<double> vals1 = files[static_cast<int>(fileNum::first)].getTimes(typ, howSolved);
	std::valarray<double> vals2 = files[static_cast<int>(fileNum::third)].getTimes(typ, howSolved);

	switch(myLib::MannWhitney(vals1, vals2, 0.05))
	{
	case 0: { (*ostr) << "0";	break; }
	case 1: { (*ostr) << "1";	break; }
	case 2: { (*ostr) << "-1";	break; }
	default: { break; }
	}
	(*ostr) << "\t";

	(*ostr)
			  << (smLib::mean(vals1) - smLib::mean(vals2)) / smLib::mean(vals1) << "\t"
				 ;
}

void FeedbackClass::checkStatSolving(taskType typ, ansType howSolved)
{
	int num1 = files[static_cast<int>(fileNum::first)].getNum(typ, howSolved);
	int num2 = files[static_cast<int>(fileNum::third)].getNum(typ, howSolved);

	auto a = myLib::binomialOneTailed(num1, num2, FBedf::numTasks);
	(*ostr)
			<< num1 << "\t"
			<< num2 << "\t"
			<< a << "\t"
			<< (a <= 0.05) << "\t"
			<< (num2 - num1) / static_cast<double>(num1) << "\t"
			   ;
}

void FeedbackClass::checkStatInsight(double thres)
{
	int num1 = files[static_cast<int>(fileNum::first)].getNumInsights(thres);
	int num2 = files[static_cast<int>(fileNum::third)].getNumInsights(thres);
	int numAll1 = files[static_cast<int>(fileNum::first)].getNum(taskType::verb,
													ansType::answrd);
	int numAll2 = files[static_cast<int>(fileNum::third)].getNum(taskType::verb,
													ansType::answrd);

	/// of all
	auto a = myLib::binomialOneTailed(num1, num2, FBedf::numTasks);
	(*ostr)
			<< num1 << "\t"
			<< num2 << "\t"
			<< a << "\t"
			<< (a <= 0.05) << "\t"
			   ;

	/// of solved
	auto b = myLib::binomialOneTailed(num1, num2, numAll1, numAll2);
	(*ostr)
			<< num1 / static_cast<double>(numAll1) << "\t"
			<< num2 / static_cast<double>(numAll2) << "\t"
			<< b << "\t"
			<< (b <= 0.05) << "\t"
			   ;
}

void FeedbackClass::writeStat()
{
	(*ostr) << std::fixed;
	(*ostr).precision(2);
	for(auto typ : {taskType::spat, taskType::verb})
	{
		checkStatSolving(typ, ansType::correct);	/// 5 values
		checkStatTimes(typ, ansType::correct);		/// 2 values
	}
	checkStatInsight(4.);	/// 8 values
	checkStatInsight(6.);	/// 8 values

	(*ostr) << std::defaultfloat;
}

void FeedbackClass::writeDists(ansType howSolved)
{
	/// remake with taskType operator++
	for(int i = 0; i < fb::numOfClasses; ++i)
	{
		for(int j = i + 1; j < fb::numOfClasses; ++j)
		{
			double a = files[static_cast<int>(fileNum::first)].distSpec(taskType(i), taskType(j), howSolved);
			double b = files[static_cast<int>(fileNum::third)].distSpec(taskType(i), taskType(j), howSolved);
			(*ostr)
					<< a << "\t"
					<< b << "\t"
					<< (b - a) / a << "\t";
		}
	}
}

void FeedbackClass::writeDispersions(ansType howSolved)
{
	/// reals
	for(taskType typ : {taskType::spat, taskType::verb, taskType::rest})
	{
		double a = files[static_cast<int>(fileNum::first)].spectreDispersion(typ, howSolved);
		double b = files[static_cast<int>(fileNum::third)].spectreDispersion(typ, howSolved);
		(*ostr)
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}

	/// winds
	for(taskType typ : {taskType::spat, taskType::verb, taskType::rest})
	{

		double a = files[static_cast<int>(fileNum::first)].spectreDispersionWinds(typ, howSolved);
		double b = files[static_cast<int>(fileNum::third)].spectreDispersionWinds(typ, howSolved);
		(*ostr)
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";

	}
}

void FeedbackClass::writeKDEs(const QString & prePath)
{
	for(int num : {static_cast<int>(fileNum::first), static_cast<int>(fileNum::third)})
	{
		files[num].kdeForSolvTime(taskType::spat).save(prePath + "kde_spat_" + nm(num) + ".jpg");
		files[num].kdeForSolvTime(taskType::verb).save(prePath + "kde_verb_" + nm(num) + ".jpg");
	}
}

void FeedbackClass::writeShortLongs(const QString & prePath)
{
	for(int num : {static_cast<int>(fileNum::first), static_cast<int>(fileNum::third)})
	{
		files[num].verbShortLong(4).save(prePath + "shortLong_4s_" + nm(num) + ".jpg");
		files[num].verbShortLong(6).save(prePath + "shortLong_6s_" + nm(num) + ".jpg");
		files[num].verbShortLong(10).save(prePath + "shortLong_10s_" + nm(num) + ".jpg");
	}
}


void FeedbackClass::writeRightWrong(const QString & prePath)
{
	for(int num : {
		static_cast<int>(fileNum::first),
		static_cast<int>(fileNum::third),
		static_cast<int>(fileNum::second)})
	{
		files[num].rightWrongSpec(taskType::spat).save(prePath + "spat_" + nm(num) + ".jpg");
		files[num].rightWrongSpec(taskType::verb).save(prePath + "verb_" + nm(num) + ".jpg");
	}
}

void FeedbackClass::writeBackgroundCompare(taskType typ, ansType howSolved)
{
	int counter = 1;
	for(auto nn : {fileNum::first, fileNum::second, fileNum::third})
	{
		const int fileN = static_cast<int>(nn);
		matrix diff = this->files[fileN].backgroundCompare(typ, howSolved)
					  .subCols(this->files[fileN].getLeftLim(),
							   this->files[fileN].getRightLim());

		myLib::drw::drawArray(myLib::drw::drawTemplate(true, FBedf::leftFreq, FBedf::rightFreq),
							  diff).save(def::helpPath + "/"
										 + this->files[fileN].getExpNameShort()
										 + "_" + nm(counter++)
										 + "_" + nm(static_cast<int>(typ))
										 + "_" + nm(static_cast<int>(howSolved))
										 + "_taskRest.jpg");
	}
}

void FeedbackClass::writeClass(bool aplhaOnly)
{
	if(01)
	{
		double a = std::get<0>( files[static_cast<int>(fileNum::first)].classifyReals(aplhaOnly) );
		double b = std::get<0>( files[static_cast<int>(fileNum::third)].classifyReals(aplhaOnly) );
		(*ostr)
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}


	if(01)
	{
		double a = std::get<0>( files[static_cast<int>(fileNum::first)].classifyWinds(aplhaOnly) );
		double b = std::get<0>( files[static_cast<int>(fileNum::third)].classifyWinds(aplhaOnly) );
		(*ostr)
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}
}


void FeedbackClass::remakeWindows(fileNum num, double overlapPart)
{
	this->files[static_cast<int>(num)].remakeWindows((1. - overlapPart) * fb::FBedf::windLen, 0);
}

ClassifierData FeedbackClass::prepareClDataWinds(fileNum num, bool reduce)
{
	return this->files[static_cast<int>(num)].prepareClDataWinds(reduce);
}

void FeedbackClass::calculateICAs()
{
	for(const FBedf & in : files)
	{
		in.calculateICA();
	}
}

/// ???
void FeedbackClass::writeLearnedPatterns()
{
	ANN * ann = new ANN();

	auto & fil1 = this->files[static_cast<int>(fileNum::first)];
	fil1.remakeWindows(0.5 * fil1.getFreq(), 0); /// magic const

	auto clData = prepareClDataWinds(fileNum::first, true);

	ann->setClassifierData(clData);
	ann->cleaningNfold(3);
//	ann->getClassifierData()->z_transform(); /// again?
	ann->learnAll();

	QString wtsPath = files[static_cast<int>(fileNum::first)].getFilePath();
	wtsPath.replace(".edf", ".wts");
	ann->writeWeight(wtsPath);


	auto & fil2 = this->files[static_cast<int>(fileNum::second)];
	fil2.remakeWindows(0.5 * fil2.getFreq(), 0); /// magic const
	auto clData2 = prepareClDataWinds(fileNum::second, false);
	ann->setClassifierData(clData2);
	ann->readWeight(wtsPath);
	ann->testAll();
	auto res1 = ann->averageClassification(DEVNULL);

	auto & fil3 = this->files[static_cast<int>(fileNum::third)];
	fil3.remakeWindows(0.5 * fil3.getFreq(), 0); /// magic const
	auto clData3 = prepareClDataWinds(fileNum::third, false);
	ann->setClassifierData(clData3);
	ann->readWeight(wtsPath);
	ann->testAll();
	auto res2 = ann->averageClassification(DEVNULL);

	(*ostr)
			<< std::get<0>( res1 ) << "\t"
			<< std::get<0>( res2 ) << "\t"
			   ;

	delete ann;
}

void FeedbackClass::writeSuccessive()
{
	Net * net = new Net();
	(*ostr)
			<< std::get<0>(
				   net->successiveByEDFfinal(
					files[static_cast<int>(fileNum::first)],
					files[static_cast<int>(fileNum::second)])
			)
			<< "\t"; (*ostr).flush();
	delete net;
}

void FeedbackClass::writeSuccessive3()
{
	ANN * ann = new ANN();
	auto fil3 = this->files[static_cast<int>(fileNum::third)];
	fil3.remakeWindows(0.5 * fil3.getFreq(), 0); /// magic const
	auto clData3 = prepareClDataWinds(fileNum::third, false);
	ann->setClassifierData(clData3);
	ann->readWeight(def::helpPath
					+ "/"
					+ this->files[static_cast<int>(fileNum::third)].getExpNameShort()
			+ "_last.wts");
	ann->testAll();
	auto res2 = ann->averageClassification(DEVNULL);
	(*ostr) << std::get<0>( res2 ) << "\t";
}


void FeedbackClass::writePartOfCleaned()
{
	for(fileNum num : {fileNum::first, fileNum::second, fileNum::third})
	{
		/// not getFile(...) because of non const
		(*ostr) << files[static_cast<int>(num)].partOfCleanedWinds() << "\t";
	}
}

void FeedbackClass::writeFile()
{
	std::ofstream outStr;
	outStr.open((this->guyPath + "/" + this->guyName + "_res.txt").toStdString());

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


	for(int i : {static_cast<int>(fileNum::first), static_cast<int>(fileNum::third)})
	{
		for(int j = 0; j < 2; ++j) /// taskType
		{

			outStr << std::fixed;
			outStr.precision(1);

			for(ansType ans : {
				ansType::correct,
				ansType::wrong,
				ansType::answrd })
			{
				auto vl = files[i].getTimes(taskType(j), ans);

				/// output times (right, wring, answered)
				if(0)
				{
					outStr << smLib::mean(vl) << "\t"
						   << smLib::median(vl) << "\t"
						   << smLib::sigma(vl) << "\t";
				}
			}
			outStr << std::defaultfloat;

			for(ansType ans : {ansType::correct, ansType::wrong, ansType::skip})
			{
				/// output num of right, wrong, unanswered
				outStr << files[i].getNum(taskType(j), ans);
				outStr << "\t";
			}
		}
		outStr << "\r\n";
	}
	outStr.close();
}

} /// end of namespace fb
