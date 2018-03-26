#include <other/feedback.h>

#include <myLib/output.h>
#include <myLib/statistics.h>
#include <myLib/valar.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <myLib/highlevel.h>
#include <myLib/signalProcessing.h>
#include <myLib/drw.h>

/// for successive preclean
#include <widgets/net.h>

 /// for std::defaultfloat
#include <ios>
#include <iostream>
#include <bits/ios_base.h>

using namespace myOut;

namespace fb
{
/// FBedf


/// from 0 - for dispersion, distance, etc
const std::vector<int> FBedf::chansToProcess{
//	0, 1,		// Fp1,	Fp2
//	2, 6,		// F7,	F8
	3, 4, 5,	// F3,	Fz,	F4
//	7, 11,		// T3, T4
	8, 9, 10,	// C3,	Cz,	C4
//	12, 16,		// T5,	T6
	13, 14, 15,	// P3,	Pz,	P4
//	17, 18		// O1,	O2
};

FBedf::FBedf(const QString & edfPath, const QString & ansPath)
{
	isGood = false;
	if(!QFile::exists(edfPath) ||
	   !QFile::exists(ansPath))
	{
		return;
	}

	this->readEdfFile(edfPath);

	/// arrange ans
	this->ansInRow = this->readAns(ansPath);
	this->ans.resize(2);
	int c = 0;
	for(auto mrk : this->markers)
	{
		if(mrk.second == 241)		{ this->ans[0].push_back(this->ansInRow[c++]); }
		else if(mrk.second == 247)	{ this->ans[1].push_back(this->ansInRow[c++]); }
	}

	/// divide to reals w/o markers
	this->realsSignals = myLib::sliceData(this->getData().subRows(-1), /// drop markers
										  this->getMarkers());
	if(this->realsSignals[0].size() != numTasks)
	{ std::cout << "spatTasks num = " << this->realsSignals[0].size() << std::endl; }
	if(this->realsSignals[1].size() != numTasks)
	{ std::cout << "verbTasks num = " << this->realsSignals[1].size() << std::endl; }
	/// last rest till 255 cross included ( or make from 254 + 8 sec?)
	if(this->realsSignals[2].size() != numTasks * 2)
	{ std::cout << "rests num = " << this->realsSignals[2].size() << std::endl; }

	realsSpectra.resize(realsSignals.size());
	solvTime.resize(realsSignals.size());

	/// count spectra and solvTime
	for(int typ = 0; typ < realsSignals.size(); ++typ)
	{
		solvTime[typ].resize(realsSignals[typ].size());
		realsSpectra[typ].reserve(realsSignals[typ].size());

		for(int real = 0; real < realsSignals[typ].size(); ++real)
		{
			solvTime[typ][real] = realsSignals[typ][real].cols() / this->srate;


			matrix a = myLib::countSpectre(realsSignals[typ][real],
										   4096,
										   15);
			realsSpectra[typ].push_back(a); /// even if empty
//			if(!a.isEmpty()) { realsSpectra[typ].push_back(a); }
		}
	}

	/// make freqs vector
	freqs.clear();
	for(int i = std::floor(FBedf::leftFreq / FBedf::spStep);
		i < std::ceil(FBedf::rightFreq / FBedf::spStep);
		++i)
	{
		freqs.push_back(i * FBedf::spStep);
	}
	isGood = true;


	if(0)
	{
		std::cout << realsSpectra.size() << std::endl; /// how many classes
		for(const auto & in : realsSpectra)
		{
			std::cout << in.size() << "\t"; /// how many reals
		}
		std::cout << std::endl;

		for(const auto & in : realsSpectra)
		{
			for(const auto & in2 : in)
			{
				std::cout << in2.size() << "\t"; /// how many rows in reals;
			}
			std::cout << std::endl;
		}
	}
}

std::vector<int> FBedf::readAns(const QString & ansPath)
{
	/// read answers
	std::vector<int> res; res.reserve(80);

	std::ifstream answers;
	answers.open(ansPath.toStdString());

	char ans;
	while(answers >> ans)
	{
		while(ans != '0' && ans != '1' && ans != '2') { answers >> ans; }

		bool ok{};
		int tmp = QString(ans).toInt(&ok);

		if(ok) { res.push_back(tmp); }
		else
		{
			std::cout << "FeedbackClass::readAnsFile: wrong char in file = "
					  << ans << std::endl;
		}
	}
	answers.close();

	if(res.size() != 2 * numTasks)
	{
		std::cout << "FeedbackClass::readAnsFile: wrong num of anwers = "
				  << res.size()
				  << ", expected = " << 2 * numTasks << std::endl;
	}

	return res;
}

std::valarray<double> FBedf::spectralRow(taskType type, int chan, double freq)
{
//	std::valarray<double> res(realsSpectra[int(type)].size());
	std::vector<double> res;	res.reserve(realsSpectra[int(type)].size());

	for(int i = 0; i < realsSpectra[int(type)].size(); ++i)
	{
		if(!realsSpectra[int(type)][i].isEmpty()) /// may be empty spectre
		{
			res.push_back(realsSpectra[int(type)][i][chan][ std::floor(freq / FBedf::spStep) ]);
		}
	}
	return smLib::vecToValar(res);
//	return res;
}

double FBedf::distSpec(taskType type1, taskType type2)
{
	int all = 0;
	int diff = 0;

	for(int chan : FBedf::chansToProcess)
	{
		for(double freq : this->freqs)
		{
			auto a = myLib::MannWhitney(this->spectralRow(type1, chan, freq),
										this->spectralRow(type2, chan, freq),
										0.05);
			++all;
			if(a != 0) { ++diff; }
		}
	}

	return double(diff) / all;
}

double FBedf::insightPartOfAll(double thres) const
{
	return this->getInsight(thres) / this->getNum(taskType::verb, ansType::all);
}

double FBedf::insightPartOfSolved(double thres) const
{
	return this->getInsight(thres) / this->getNum(taskType::verb, ansType::answrd);
}

double FBedf::spectreDispersion(taskType typ)
{
	double res = 0.;
	for(int chan : FBedf::chansToProcess)
	{
		for(double freq : this->freqs)
		{
			auto row = this->spectralRow(typ, chan, freq);
			res += smLib::sigma(row);
		}
	}
	return res;
}

std::valarray<double> FBedf::getTimes(taskType typ, ansType howSolved) const
{
	std::vector<double> res;
	if(howSolved == ansType::all)
	{
		return solvTime[int(typ)];
	}
	else if(howSolved == ansType::answrd)
	{
		for(int i = 0; i < solvTime[int(typ)].size(); ++i)
		{
			if(ans[int(typ)][i] == int(ansType::right) ||
			   ans[int(typ)][i] == int(ansType::wrong) )
			{
				res.push_back(solvTime[int(typ)][i]);
			}
		}
	}
	else
	{
		for(int i = 0; i < solvTime[int(typ)].size(); ++i)
		{
			if(ans[int(typ)][i] == int(howSolved))
			{
				res.push_back(solvTime[int(typ)][i]);
			}
		}
	}
	return smLib::vecToValar(res);
}

int FBedf::getNum(taskType typ, ansType howSolved) const
{
	int num = 0;
	if(howSolved == ansType::answrd) /// not yet needed
	{
		for(auto in : ans[int(typ)])
		{
			if(in == int(ansType::right) ||
			   in == int(ansType::wrong) ) { ++num; }
		}
	}
	else if(howSolved == ansType::all)
	{
		return ans[int(typ)].size();
	}
	else
	{
		for(auto in : ans[int(typ)])
		{
			if(in == int(howSolved)) { ++num; }
		}
	}
	return num;
}


int FBedf::getInsight(double thres) const
{
	const auto & times = solvTime[int(taskType::verb)];
	int res = 0;
	std::for_each(std::begin(times), std::end(times),
				  [&res, thres](double in)
	{
		if(in <= thres) { ++res; }
	});
	return res;
}


QPixmap FBedf::kdeForSolvTime(taskType typ) const
{
	std::vector<double> res{};
	std::for_each(std::begin(solvTime[int(typ)]),
			std::end(solvTime[int(typ)]),
			[&res](double in)
	{
		if(in < FBedf::solveThres - 0.5) { res.push_back(in); }
	});
	return myLib::kernelEst(smLib::vecToValar(res));
}

QPixmap FBedf::verbShortLong(double thres) const
{
	const int taskTyp = int(taskType::verb);

	std::vector<matrix> shrts;
	std::vector<matrix> lngs;

	for(int i = 0; i < solvTime[taskTyp].size(); ++i)
	{
		if(!realsSpectra[taskTyp][i].isEmpty())
		{
			if(solvTime[1][i] > thres)	{ lngs.push_back(realsSpectra[taskTyp][i]);		}
			else						{ shrts.push_back(realsSpectra[taskTyp][i]);	}
		}
	}

	matrix shrt	= shrts[0];	shrt.fill(0.);
	matrix lng	= lngs[0];	lng.fill(0.);
	for(const auto & sp : lngs)
	{
		lng += sp;
	}
	lng /= lngs.size();

	for(const auto & sp : shrts)
	{
		shrt += sp;
	}
	shrt /= shrts.size();

	int leftLim = fftLimit(FBedf::leftFreq, this->getFreq(), FBedf::fftLen);
	int rightLim = fftLimit(FBedf::rightFreq, this->getFreq(), FBedf::fftLen);
	matrix both(2, 1);
	both[0] = shrt.subCols(leftLim, rightLim).toValarByRows();
	both[1] = lng.subCols(leftLim, rightLim).toValarByRows();

	auto tmplt = myLib::drw::drawTemplate(true, FBedf::leftFreq, FBedf::rightFreq, 19);
	return myLib::drw::drawArrays(tmplt, both);
}

Classifier::avType FBedf::classifyReals() const
{
	ANN * net = new ANN();

	std::vector<uint> types{};	types.reserve(160);
	matrix clData{};			clData.reserve(160);

	for(int i = 0; i < realsSpectra.size(); ++i)
	{
		for(int j = 0; j < realsSpectra[i].size(); ++j)
		{
			if(!realsSpectra[i][j].isEmpty())
			{
				/// fill types
				types.push_back(i);
				/// fill data matrix
				clData.push_back(realsSpectra[i][j].toValarByRows());
			}
		}
	}

	ClassifierData dt = ClassifierData(clData, types);
	net->setClassifierData(dt);

	net->crossClassification(2, 2);
	auto res = net->averageClassification();

	delete net;
	return res;
}

Classifier::avType FBedf::classifyWinds(int windLen) const
{
	const int fftLen = smLib::fftL(windLen);
	DEFS.setFftLen(fftLen);

	ANN * net = new ANN();

	/// slice winds
	double overlapPart = 0.0;
	const int windStep = (1. - overlapPart) * windLen;
	const int maxNumWinds = (this->getData().cols() - windLen) / windStep;
	std::list<uint> types{};		//types.reserve(maxNumWinds);
	std::vector<matrix> clData{};	clData.reserve(maxNumWinds);

	/// markers droppped already
	for(int i = 0; i < realsSignals.size(); ++i)
	{
		for(int j = 0; j < realsSignals[i].size(); ++j)
		{
			for(int windStart = 0;
				windStart < int(realsSignals[i][j].cols()) - windLen;
				windStart += windStep)
			{
				clData.push_back(realsSignals[i][j].subCols(windStart,
															windStart + windLen));
				types.push_back(i);
			}
		}
	}

	matrix clSpec{};	clSpec.reserve(clData.size());
	auto typIt = std::begin(types);
	for(int i = 0; i < clData.size(); ++i, ++typIt)
	{
		matrix pew = myLib::countSpectre(clData[i],
										 fftLen,
										 5);
		if(!pew.isEmpty())
		{
			clSpec.push_back(pew.toValarByRows());
			typIt = types.erase(typIt);
			--typIt;
		}
	}

	std::vector<uint> typs(types.size());
	std::copy(std::begin(types), std::end(types), std::begin(typs));


	ClassifierData dt = ClassifierData(clSpec, typs);
	/// arguments of wrong size
	net->setClassifierData(dt);

	net->crossClassification(10, 8);
	auto res = net->averageClassification();

	delete net;
	return res;
}







/// FeedbackClass
FeedbackClass::FeedbackClass(const QString & guyPath_,
							 const QString & guyName_,
							 const QString & postfix_)
	: guyPath(guyPath_), guyName(guyName_), postfix(postfix_)
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

	for(int i : {1, 3})
	{
		auto & fil = files[int(i != 1)];
		fil = FBedf(filePath(i), ansPath(i));
		if(!fil)
		{
			this->isGood = false;
			return;
		}
	}
	isGood = true;
}

void FeedbackClass::checkStatTimes(taskType typ, ansType howSolved)
{
	std::valarray<double> vals1 = files[int(fileNum::first)].getTimes(typ, howSolved);
	std::valarray<double> vals2 = files[int(fileNum::third)].getTimes(typ, howSolved);

	if(0)
	{
		if(typ == taskType::spat)		{ std::cout << "spatTime:" << "\n"; }
		else if(typ == taskType::verb)	{ std::cout << "verbTime:" << "\n"; }
	}

	switch(myLib::MannWhitney(vals1, vals2, 0.05))
	{
	case 0: { std::cout << "0"; break; }
	case 1: { std::cout << "1"; break; }
	case 2: { std::cout << "-1"; break; }
	default: { break; }
	}
	std::cout << "\t";


	std::cout
//			  << "av.acceleration = "
			  << (smLib::mean(vals1) - smLib::mean(vals2)) / smLib::mean(vals1) << "\t"
//			  << std::endl
				 ;
}

void FeedbackClass::checkStatSolving(taskType typ, ansType howSolved)
{
	int num1 = files[int(fileNum::first)].getNum(typ, howSolved); /// solved in the  first file
	int num2 = files[int(fileNum::third)].getNum(typ, howSolved); /// solved in the second file

	if(0)
	{
		if(typ == taskType::spat)		{ std::cout << "spatNum:" << "\n"; }
		else if(typ == taskType::verb)	{ std::cout << "verbNum:" << "\n"; }
	}

	auto a = myLib::binomialOneTailed(num1, num2, FBedf::numTasks);
	std::cout
			<< num1 << "\t"
			<< num2 << "\t"
//			<< "p-value = "
			<< a << "\t"
			<< (a <= 0.05) << "\t"
//			<< "improvement = "
			<< double(num2 - num1) / num1 << "\t"
//			<< std::endl
			   ;
}

void FeedbackClass::checkStatInsight(double thres)
{
	int num1 = files[int(fileNum::first)].getInsight(thres);
	int num2 = files[int(fileNum::third)].getInsight(thres);
	int numAll1 = files[int(fileNum::first)].getNum(taskType::verb,
													ansType::answrd);
	int numAll2 = files[int(fileNum::third)].getNum(taskType::verb,
													ansType::answrd);

	/// of all
	auto a = myLib::binomialOneTailed(num1, num2, FBedf::numTasks);
	std::cout
			<< num1 << "\t"
			<< num2 << "\t"
//			<< "p-value = "
			<< a << "\t"
			<< (a <= 0.05) << "\t"
//			<< "improvement = "
//			<< std::endl
			   ;

	/// of solved
	auto b = myLib::binomialOneTailed(num1, num2, numAll1, numAll2);
	std::cout
			<< num1 / double(numAll1) << "\t"
			<< num2 / double(numAll2) << "\t"
//			<< "p-value = "
			<< b << "\t"
			<< (b <= 0.05) << "\t"
//			<< "improvement = "
//			<< std::endl
			   ;



}

void FeedbackClass::writeStat()
{
	std::cout << std::fixed;
	std::cout.precision(2);
	for(auto typ : {taskType::spat, taskType::verb})
	{
		checkStatSolving(typ, ansType::right);	/// cout 5 values
		checkStatTimes(typ, ansType::right);	/// cout 2 values
	}
	checkStatInsight(4.);	/// cout 8 values
	checkStatInsight(6.);	/// cout 8 values

	std::cout << std::defaultfloat;
}

void FeedbackClass::writeDists() /// 1st: 0-1, 0-2, 1-2, 2nd: 0-1, 0-2, 1-2
{
	for(int i = 0; i < fb::numOfClasses; ++i)
	{
		for(int j = i + 1; j < fb::numOfClasses; ++j)
		{
			double a = files[0].distSpec(taskType(i), taskType(j));
			double b = files[1].distSpec(taskType(i), taskType(j));
			std::cout
					<< a << "\t"
					<< b << "\t"
					<< (b - a) / a << "\t";
		}
	}
}

void FeedbackClass::writeDispersions() /// 1st: 0, 1, 2, 2nd: 0, 1, 2
{
	for(int i = 0; i < fb::numOfClasses; ++i)
	{
		double a = files[0].spectreDispersion(taskType(i));
		double b = files[1].spectreDispersion(taskType(i));
		std::cout
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}
}

void FeedbackClass::writeKDEs(const QString & prePath)
{
	for(int num : {0, 1})
	{
		files[num].kdeForSolvTime(taskType::spat).save(prePath + "kde_spat_" + nm(num) + ".jpg");
		files[num].kdeForSolvTime(taskType::verb).save(prePath + "kde_verb_" + nm(num) + ".jpg");
	}
}

void FeedbackClass::writeShortLongs(const QString & prePath)
{
	for(int num : {0, 1})
	{
		files[num].verbShortLong(4).save(prePath + "shortLong_4s_" + nm(num) + ".jpg");
		files[num].verbShortLong(6).save(prePath + "shortLong_6s_" + nm(num) + ".jpg");
		files[num].verbShortLong(10).save(prePath + "shortLong_10s_" + nm(num) + ".jpg");
	}
}

void FeedbackClass::writeClass()
{
	if(0)
	{
		double a = files[0].classifyReals().first;
		double b = files[1].classifyReals().first;
		std::cout
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}

	{
		double a = files[0].classifyWinds(1024).first;
		double b = files[1].classifyWinds(1024).first;
		std::cout
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
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


	for(int i = 0; i < 2; ++i) // fileNum
	{
		for(int j = 0; j < 2; ++j) // taskType
		{

			outStr << std::fixed;
			outStr.precision(1);

			for(ansType ans : {
				ansType::right,
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

			for(ansType ans : {ansType::right, ansType::wrong, ansType::skip})
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







/// other functions and autos
void coutAllFeatures(const QString & dear,
					 const std::vector<std::pair<QString, QString> > & guysList,
					 const QString & postfix)
{
	const QString guysPath = DEFS.dirPath() + "/" + dear;

	for(const auto & in : guysList)
	{
		const QString guyPath = guysPath + "/" + in.first;
		if(!QDir(guyPath).exists()) { continue; }

		fb::FeedbackClass fb(guyPath, in.second, postfix);
		if(!fb) { continue; }
		std::cout << in.second << "\t";

		/// stats of solving and times
//		fb.writeStat();											std::cout.flush(); /// 23
//		fb.writeDists();										std::cout.flush(); /// 6
//		fb.writeDispersions();									std::cout.flush(); /// 9
//		fb.writeKDEs(guysPath + "/" + in.second + "_");			std::cout.flush();
//		fb.writeShortLongs(guysPath + "/" + in.second + "_");	std::cout.flush();
		fb.writeClass();										std::cout.flush();
		std::cout << std::endl; exit(0);

		///
		std::cout << std::endl;


		/// write solving stats into txt file
//		fb.writeFile();
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
