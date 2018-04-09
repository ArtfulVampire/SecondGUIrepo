#include <other/feedback.h>

#include <myLib/output.h>
#include <myLib/statistics.h>
#include <myLib/valar.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <myLib/highlevel.h>
#include <myLib/signalProcessing.h>
#include <myLib/drw.h>

/// for successive preclean and imitation feedback
#include <widgets/net.h>

 /// for std::defaultfloat
#include <ios>
#include <iostream>
#include <iomanip>
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
		std::cout << "FBedf: some of files absent" << std::endl;
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
		}
	}

	/// make windSignals

	const double overlapPart = 0.0;
	const int windStep = (1. - overlapPart) * windLen;						/// = windLen
	const int maxNumWinds = (this->getData().cols() - windLen) / windStep;
	const int numSkipStartWinds = 0; /// skip 2 windows

	std::vector<matrix> windSigData{};		windSigData.reserve(maxNumWinds);
	std::vector<uint> windSigTypes{};		windSigTypes.reserve(maxNumWinds);

	for(int i = 0; i < realsSignals.size(); ++i)
	{
		/// is rest?
		int skipStart = (i == 2) ? 0 : numSkipStartWinds * windStep;
		for(int j = 0; j < realsSignals[i].size(); ++j)
		{
			for(int windStart = 0 + skipStart;
				windStart < int(realsSignals[i][j].cols()) - windLen;
				windStart += windStep)
			{
				windSigData.push_back(realsSignals[i][j].subCols(windStart,
																 windStart + windLen));
				windSigTypes.push_back(i);
			}
		}
	}

	/// make windSpectra

	this->windSpectra.clear();		this->windSpectra.reserve(windSigData.size());
	this->windTypes.clear();		this->windTypes.reserve(windSigTypes.size());

	int leftLim = fftLimit(FBedf::leftFreq, this->getFreq(), windFftLen);
	int rightLim = fftLimit(FBedf::rightFreq, this->getFreq(), windFftLen);

	for(int i = 0; i < windSigData.size(); ++i)
	{
		matrix pew = myLib::countSpectre(windSigData[i],
										 windFftLen,
										 5);
		if(!pew.isEmpty())
		{
			this->windSpectra.push_back(pew.subCols(leftLim, rightLim).toValarByRows());
			this->windTypes.push_back(windSigTypes[i]);
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


	/// test cout
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


ClassifierData FBedf::prepareClDataWinds(bool reduce)
{
	ClassifierData clData{};
	for(int i = 0; i < windTypes.size(); ++i)
	{
		clData.push_back(
					windSpectra[i],
					windTypes[i],
					"L " + nm(i));
	}
	if(reduce) { clData.reduceSize(suc::learnSetStay); }
	clData.setApriori(clData.getClassCount());
	clData.z_transform();
	return clData;
}

double FBedf::partOfCleanedWinds()
{
	this->remakeWindows(0.0);
	auto clData = this->prepareClDataWinds(false);
	const double init = clData.size();

	ANN * ann = new ANN();
	ann->setClassifierData(clData);
//	int counter = 0;
	do
	{
		clData = *(ann->getClassifierData());
		ann->setClassifierData(clData);			/// to zero confusion matrix
//		ann->cleaningNfold(2);
		ann->cleaningKfold(3, 2);
//		std::cout << counter++ << " "; std::cout.flush();
	}
	while(ann->averageClassification(DEVNULL).first != 100.);

	const double last = ann->getClassifierData()->size();

	return (init - last) / init;
}

void FBedf::remakeWindows(double overlapPart)
{
	/// make windSignals
	const int windStep = (1. - overlapPart) * windLen;						/// = windLen
	const int maxNumWinds = (this->getData().cols() - windLen) / windStep;
	const int numSkipStartWinds = 0; /// skip 2 windows

	std::vector<matrix> windSigData{};		windSigData.reserve(maxNumWinds);
	std::vector<uint> windSigTypes{};		windSigTypes.reserve(maxNumWinds);

	for(int i = 0; i < realsSignals.size(); ++i)
	{
		/// is rest?
		int skipStart = (i == 2) ? 0 : numSkipStartWinds * windStep;
		for(int j = 0; j < realsSignals[i].size(); ++j)
		{
			for(int windStart = 0 + skipStart;
				windStart < int(realsSignals[i][j].cols()) - windLen;
				windStart += windStep)
			{
				windSigData.push_back(realsSignals[i][j].subCols(windStart,
																 windStart + windLen));
				windSigTypes.push_back(i);
			}
		}
	}

	/// make windSpectra
	this->windSpectra.clear();		this->windSpectra.reserve(windSigData.size());
	this->windTypes.clear();		this->windTypes.reserve(windSigTypes.size());

	int leftLim = fftLimit(FBedf::leftFreq, this->getFreq(), windFftLen);
	int rightLim = fftLimit(FBedf::rightFreq, this->getFreq(), windFftLen);

	for(int i = 0; i < windSigData.size(); ++i)
	{
		matrix pew = myLib::countSpectre(windSigData[i],
										 windFftLen,
										 5);
		if(!pew.isEmpty())
		{
			this->windSpectra.push_back(pew.subCols(leftLim, rightLim).toValarByRows());
			this->windTypes.push_back(windSigTypes[i]);
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
	return this->getNumInsights(thres) / this->getNum(taskType::verb, ansType::all);
}

double FBedf::insightPartOfSolved(double thres) const
{
	return this->getNumInsights(thres) / this->getNum(taskType::verb, ansType::answrd);
}

double FBedf::spectreDispersion(taskType typ)
{
	/// remake via subRows, subcols, transpose
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


int FBedf::getNumInsights(double thres) const
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
	if(shrts.size() == 0) { return {}; }

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

QPixmap FBedf::rightWrongSpec(taskType typ) const
{
	std::vector<matrix> rights;
	std::vector<matrix> wrongs;

	int wrongsType = 0;

	if(typ == taskType::spat) /// right vs wrong
	{
		wrongsType = 2;
	}
	else if(typ == taskType::verb) /// ans vs noans
	{
		wrongsType = 0;
	}

	for(int i = 0; i < solvTime[int(typ)].size(); ++i)
	{
		if(ans[int(typ)][i] == 1)				{ rights.push_back(realsSpectra[int(typ)][i]);	}
		else if(ans[int(typ)][i] == wrongsType)	{ wrongs.push_back(realsSpectra[int(typ)][i]);	}
	}

	if(rights.size() == 0 || wrongs.size() == 0) { return {}; }

	matrix rght	= rights[0];	rght.fill(0.);
	matrix wrng	= wrongs[0];	wrng.fill(0.);
	for(const auto & sp : wrongs)
	{
		wrng += sp;
	}
	wrng /= wrongs.size();

	for(const auto & sp : rights)
	{
		rght += sp;
	}
	rght /= rights.size();

	int leftLim = fftLimit(FBedf::leftFreq, this->getFreq(), FBedf::fftLen);
	int rightLim = fftLimit(FBedf::rightFreq, this->getFreq(), FBedf::fftLen);
	matrix both(2, 1);
	both[0] = rght.subCols(leftLim, rightLim).toValarByRows();
	both[1] = wrng.subCols(leftLim, rightLim).toValarByRows();

	auto tmplt = myLib::drw::drawTemplate(true, FBedf::leftFreq, FBedf::rightFreq, 19);
	return myLib::drw::drawArrays(tmplt, both);
}

Classifier::avType FBedf::classifyReals() const
{
	DEFS.setFftLen(FBedf::fftLen);
	ANN * net = new ANN();

	std::vector<uint> types{};	types.reserve(160);
	matrix clData{};			clData.reserve(160);


	int leftLim = fftLimit(FBedf::leftFreq, this->getFreq(), FBedf::fftLen);
	int rightLim = fftLimit(FBedf::rightFreq, this->getFreq(), FBedf::fftLen);

	for(int i = 0; i < realsSpectra.size(); ++i)
	{
		for(int j = 0; j < realsSpectra[i].size(); ++j)
		{
			if(!realsSpectra[i][j].isEmpty())
			{
				types.push_back(i);
				clData.push_back(realsSpectra[i][j].subCols(leftLim, rightLim).toValarByRows());
			}
		}
	}

	ClassifierData dt = ClassifierData(clData, types);
	net->setClassifierData(dt);

	net->adjustLearnRate(DEVNULL);
//	net->adjustLearnRate();
	net->crossClassification(10, 5, DEVNULL);
//	net->leaveOneOutClassification(DEVNULL);
//	net->leaveOneOutClassification();
	auto res = net->averageClassification(DEVNULL);

	delete net;
	return res;
}

Classifier::avType FBedf::classifyWinds() const
{
	DEFS.setFftLen(windFftLen);

	ANN * net = new ANN();

	ClassifierData dt = ClassifierData(this->windSpectra, this->windTypes);
	/// arguments of wrong size
	net->setClassifierData(dt);

	net->adjustLearnRate(DEVNULL);
	net->crossClassification(10, 5, DEVNULL);
//	net->leaveOneOutClassification(DEVNULL);
	auto res = net->averageClassification(DEVNULL);

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

	for(int i : {1, 2, 3})
	{
		int fileNumber{};
		switch(i)
		{
		case 1: { fileNumber = int(fileNum::first);		break; }	/// 0
		case 2: { fileNumber = int(fileNum::second);	break; }	/// 2
		case 3: { fileNumber = int(fileNum::third);		break; }	/// 1
		}
		auto & fil = files[fileNumber];

		fil = FBedf(filePath(i), ansPath(i));

		if(!fil)
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
	int num1 = files[int(fileNum::first)].getNumInsights(thres);
	int num2 = files[int(fileNum::third)].getNumInsights(thres);
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

void FeedbackClass::writeDists()
{
	for(int i = 0; i < fb::numOfClasses; ++i)
	{
		for(int j = i + 1; j < fb::numOfClasses; ++j)
		{
			double a = files[int(fileNum::first)].distSpec(taskType(i), taskType(j));
			double b = files[int(fileNum::third)].distSpec(taskType(i), taskType(j));
			std::cout
//					<< std::setprecision(4)
					<< a << "\t"
					<< b << "\t"
					<< (b - a) / a << "\t";
		}
	}
}

void FeedbackClass::writeDispersions()
{
	for(int i = 0; i < fb::numOfClasses; ++i)
	{
		double a = files[int(fileNum::first)].spectreDispersion(taskType(i));
		double b = files[int(fileNum::third)].spectreDispersion(taskType(i));
		std::cout
//				<< std::setprecision(4)
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}
}

void FeedbackClass::writeKDEs(const QString & prePath)
{
	for(int num : {int(fileNum::first), int(fileNum::third)})
	{
		files[num].kdeForSolvTime(taskType::spat).save(prePath + "kde_spat_" + nm(num) + ".jpg");
		files[num].kdeForSolvTime(taskType::verb).save(prePath + "kde_verb_" + nm(num) + ".jpg");
	}
}

void FeedbackClass::writeShortLongs(const QString & prePath)
{
	for(int num : {int(fileNum::first), int(fileNum::third)})
	{
		files[num].verbShortLong(4).save(prePath + "shortLong_4s_" + nm(num) + ".jpg");
		files[num].verbShortLong(6).save(prePath + "shortLong_6s_" + nm(num) + ".jpg");
		files[num].verbShortLong(10).save(prePath + "shortLong_10s_" + nm(num) + ".jpg");
	}
}


void FeedbackClass::writeRightWrong(const QString & prePath)
{
	for(int num : {int(fileNum::first), int(fileNum::third), int(fileNum::second)})
	{
		files[num].rightWrongSpec(taskType::spat).save(prePath + "spat_" + nm(num) + ".jpg");
		files[num].rightWrongSpec(taskType::verb).save(prePath + "verb_" + nm(num) + ".jpg");
	}
}

void FeedbackClass::writeClass()
{
	if(01)
	{
		double a = files[int(fileNum::first)].classifyReals().first;
		double b = files[int(fileNum::third)].classifyReals().first;
		std::cout
//				<< std::setprecision(3)
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}


	if(01)
	{
		double a = files[int(fileNum::first)].classifyWinds().first;
		double b = files[int(fileNum::third)].classifyWinds().first;
		std::cout
//				<< std::setprecision(3)
				<< a << "\t"
				<< b << "\t"
				<< (b - a) / a << "\t";
	}
}


void FeedbackClass::remakeWindows(fileNum num, double overlapPart)
{
	this->files[int(num)].remakeWindows(overlapPart);
}

ClassifierData FeedbackClass::prepareClDataWinds(fileNum num, bool reduce)
{
	return this->files[int(num)].prepareClDataWinds(reduce);
}


void FeedbackClass::writeLearnedPatterns()
{
	ANN * ann = new ANN();

	this->files[int(fileNum::first)].remakeWindows(3.5 / 4.0);

	auto clData = prepareClDataWinds(fileNum::first, true);

	ann->setClassifierData(clData);
	ann->cleaningNfold(3);
//	ann->getClassifierData()->z_transform(); /// again?
	ann->learnAll();

	QString wtsPath = files[int(fileNum::first)].getFilePath();
	wtsPath.replace(".edf", ".wts");
	ann->writeWeight(wtsPath);

	this->files[int(fileNum::second)].remakeWindows(3.5 / 4.0);
	auto clData2 = prepareClDataWinds(fileNum::second, false);
	ann->setClassifierData(clData2);
	ann->readWeight(wtsPath);
	ann->testAll();
	auto res1 = ann->averageClassification(DEVNULL);

	this->files[int(fileNum::third)].remakeWindows(3.5 / 4.0);
	auto clData3 = prepareClDataWinds(fileNum::third, false);
	ann->setClassifierData(clData3);
	ann->readWeight(wtsPath);
	ann->testAll();
	auto res2 = ann->averageClassification(DEVNULL);

	std::cout
			<< res1.first << "\t"
			<< res2.first << "\t"
			   ;

	delete ann;
}

void FeedbackClass::writeSuccessive()
{
	Net * net = new Net();


	std::cout
			<< net->successiveByEDFfinal(
					files[int(fileNum::first)],
					files[int(fileNum::second)]).first
			<< "\t"; std::cout.flush();
	delete net;
}

void FeedbackClass::writeSuccessive3()
{
	ANN * ann = new ANN();
	this->files[int(fileNum::third)].remakeWindows(3.5 / 4.0);
	auto clData3 = prepareClDataWinds(fileNum::third, false);
	ann->setClassifierData(clData3);
	ann->readWeight(def::helpPath + "/" + this->files[int(fileNum::third)].getExpName().left(3)
			+ "_last.wts");
	ann->testAll();
	auto res2 = ann->averageClassification(DEVNULL);
	std::cout << res2.first << "\t";
}


void FeedbackClass::writePartOfCleaned()
{
	for(fileNum num : {fileNum::first, fileNum::second, fileNum::third})
	{
		std::cout << this->files[int(num)].partOfCleanedWinds() << "\t";
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


	for(int i : {int(fileNum::first), int(fileNum::third)})
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


		fb::FeedbackClass fb(guyPath, in.second, postfix); if(!fb) { continue; }

		/// ExpName
//		std::cout << in.second << "\t";

//		FBedf fil(guyPath + "/" + in.second + "_1.edf",
//				  guyPath + "/" + in.second + "_ans1.txt");
//		for(double time = 3.; time <= 6.; time += 0.5)
//		{
//			std::cout << fil.getNumInsights(time) << "\t";
//		}

		/// stats of solving and times
//		fb.writeStat();											std::cout.flush(); /// 23
//		fb.writeDists();										std::cout.flush(); /// 6
//		fb.writeDispersions();									std::cout.flush(); /// 9
//		fb.writeKDEs(guyPath + "/" + in.second + "_");			std::cout.flush();
		fb.writeShortLongs(guyPath + "/" + in.second + "_");	std::cout.flush();
//		fb.writeClass();										std::cout.flush(); /// 6
//		fb.writeSuccessive();									std::cout.flush();
//		fb.writeLearnedPatterns();								std::cout.flush();
//		fb.writeSuccessive3();									std::cout.flush();
//		fb.writePartOfCleaned();								std::cout.flush();

//		exit(0);

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
