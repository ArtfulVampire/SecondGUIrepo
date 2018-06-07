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

FBedf::FBedf(const QString & edfPath,
			 const QString & ansPath,
			 double overlapPart,
			 int numSkipStartWinds)
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
	this->ans.resize(2);	/// magic const 2 task classes
	int c = 0;
	for(auto mrk : this->markers)
	{
		if(mrk.second == 241)		{ this->ans[0].push_back(this->ansInRow[c++]); }
		else if(mrk.second == 247)	{ this->ans[1].push_back(this->ansInRow[c++]); }
	}

	/// divide to reals w/o markers
	this->realsSignals = myLib::sliceData(this->getData().subRows(-1), /// drop markers
										  this->getMarkers());
	if(01)
	{
		/// cout if wrong number of reals
		if(this->realsSignals[0].size() != numTasks)
		{ std::cout << "spatTasks num = " << this->realsSignals[0].size() << std::endl; }

		if(this->realsSignals[1].size() != numTasks)
		{ std::cout << "verbTasks num = " << this->realsSignals[1].size() << std::endl; }

		/// last rest till 255 cross included ( or make from 254 + 8 sec?)
		if(this->realsSignals[2].size() != numTasks * 2)
		{ std::cout << "rests num = " << this->realsSignals[2].size() << std::endl; }
	}

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
										   15); /// magic consts
			realsSpectra[typ].push_back(a); /// even if empty
		}
	}
#if 01
	this->remakeWindows(overlapPart, numSkipStartWinds);
	/// make windSignals and spectra
#else
	const int windStep = (1. - overlapPart) * windLen;		/// = windLen
	const int maxNumWinds = (this->getData().cols() - windLen) / windStep;

	std::vector<matrix> windSigData{};		windSigData.reserve(maxNumWinds);
	std::vector<taskType> windSigTypes{};	windSigTypes.reserve(maxNumWinds);
	std::vector<ansType> windSigAns{};		windSigAns.reserve(maxNumWinds);

	for(int i = 0; i < realsSignals.size(); ++i) /// i = type;
	{
		/// is rest?
		int skipStart = (i == static_cast<int>(taskType::rest))
						? 0 : numSkipStartWinds * windStep;

		for(int j = 0; j < realsSignals[i].size(); ++j)
		{
			for(int windStart = 0 + skipStart;
				windStart < int(realsSignals[i][j].cols()) - windLen;
				windStart += windStep)
			{
				windSigData.push_back(realsSignals[i][j].subCols(windStart,
																 windStart + windLen));
				windSigTypes.push_back(static_cast<taskType>(i));

				if(i != static_cast<int>(taskType::rest))
				{
					windSigAns.push_back(this->ans[i][j]);
				}
				else
				{
					windSigAns.push_back(ansType::correct);	/// all rest winds are correct
				}
			}
		}
	}

	this->windSpectra.clear();		this->windSpectra.reserve(windSigData.size());
	this->windTypes.clear();		this->windTypes.reserve(windSigTypes.size());
	this->windAns.clear();			this->windAns.reserve(windSigAns.size());

	int leftLim = fftLimit(FBedf::leftFreq, this->getFreq(), windFftLen);
	int rightLim = fftLimit(FBedf::rightFreq, this->getFreq(), windFftLen);

	for(int i = 0; i < windSigData.size(); ++i)
	{
		matrix pew = myLib::countSpectre(windSigData[i],
										 windFftLen,
										 5); /// magic const numSmooth
		if(!pew.isEmpty())
		{
			this->windSpectra.push_back(pew.subCols(leftLim, rightLim).toValarByRows());
			this->windTypes.push_back(windSigTypes[i]);
			this->windAns.push_back(windSigAns[i]);
		}
	}
#endif


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
					static_cast<int>(windTypes[i]),
					"L " + nm(i));
	}
	if(reduce) { clData.reduceSize(suc::learnSetStay); }
	clData.setApriori(clData.getClassCount());
	clData.z_transform();
	clData.adjust();
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
//		ann->cleaningNfold(-1);
		ann->cleaningKfold(3, 2);
//		std::cout << counter++ << " "; std::cout.flush();
	}
	while(ann->averageClassification(DEVNULL).first != 100.);

	const double last = ann->getClassifierData()->size();

	return (init - last) / init;
}

void FBedf::remakeWindows(double overlapPart, int numSkipStartWinds)
{
	/// make windSignals
	const int windStep = (1. - overlapPart) * windLen;
	const int maxNumWinds = (this->getData().cols() - windLen) / windStep;

	std::vector<matrix> windSigData{};		windSigData.reserve(maxNumWinds);
	std::vector<taskType> windSigTypes{};	windSigTypes.reserve(maxNumWinds);
	std::vector<ansType> windSigAns{};		windSigAns.reserve(maxNumWinds);

	for(int i = 0; i < realsSignals.size(); ++i)
	{
		/// is rest?
		int skipStart = (i == static_cast<int>(taskType::rest))
						? 0 : numSkipStartWinds * windStep;

		for(int j = 0; j < realsSignals[i].size(); ++j)
		{
			for(int windStart = 0 + skipStart;
				windStart < int(realsSignals[i][j].cols()) - windLen;
				windStart += windStep)
			{
				windSigData.push_back(realsSignals[i][j].subCols(windStart,
																 windStart + windLen));
				windSigTypes.push_back(static_cast<taskType>(i));

				if(i != static_cast<int>(taskType::rest))
				{
					windSigAns.push_back(this->ans[i][j]);
				}
				else
				{
					windSigAns.push_back(ansType::correct);	/// all rest winds are correct
				}
			}
		}
	}

	/// make windSpectra
	this->windSpectra.clear();		this->windSpectra.reserve(windSigData.size());
	this->windTypes.clear();		this->windTypes.reserve(windSigTypes.size());
	this->windAns.clear();			this->windAns.reserve(windSigAns.size());

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
			this->windAns.push_back(windSigAns[i]);
		}
	}
}

std::vector<ansType> FBedf::readAns(const QString & ansPath)
{
	/// read answers
	std::vector<ansType> res; res.reserve(80);

	std::ifstream answers;
	answers.open(ansPath.toStdString());

	char ans;
	while(answers >> ans)
	{
		while(ans != '0' && ans != '1' && ans != '2') { answers >> ans; }

		ansType tmp;
		switch(ans)
		{
		case '0': { tmp = ansType::skip;	break; }
		case '1': { tmp = ansType::correct;	break; }
		case '2': { tmp = ansType::wrong;	break; }
		default: { std::cout << "FBedf::readAns: unknown answer" << std::endl; }
		}
		res.push_back(tmp);
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
//	std::valarray<double> res(realsSpectra[static_cast<int>(type)].size());
	std::vector<double> res;	res.reserve(realsSpectra[static_cast<int>(type)].size());

	for(int i = 0; i < realsSpectra[static_cast<int>(type)].size(); ++i)
	{
		if(!realsSpectra[static_cast<int>(type)][i].isEmpty()) /// may be empty spectre
		{
			res.push_back(realsSpectra[static_cast<int>(type)][i][chan][ std::floor(freq / FBedf::spStep) ]);
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
	std::vector<double> res{};
	const int taskNum = static_cast<int>(typ);
	if(howSolved == ansType::all)
	{
		return solvTime[taskNum];
	}
	/// else
	for(int i = 0; i < solvTime[taskNum].size(); ++i)
	{
		if(static_cast<int>( ans[taskNum][i] ) &
		   static_cast<int>( howSolved ))
		{
			res.push_back(solvTime[taskNum][i]);
		}
	}

	return smLib::vecToValar(res);
}

int FBedf::getNum(taskType typ, ansType howSolved) const
{

	const int taskNum = static_cast<int>(typ);
	if(howSolved == ansType::all)
	{
		return ans[taskNum].size();
	}
	///else
	int num = 0;
	for(auto in : ans[taskNum])
	{
		if(static_cast<int>(in) & static_cast<int>(howSolved)) { ++num; }
	}
	return num;
}


int FBedf::getNumInsights(double thres) const
{
	const auto & times = solvTime[static_cast<int>(taskType::verb)];
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
	std::for_each(std::begin(solvTime[static_cast<int>(typ)]),
			std::end(solvTime[static_cast<int>(typ)]),
			[&res](double in)
	{
		if(in < FBedf::solveThres - 0.5) { res.push_back(in); }
	});
	return myLib::kernelEst(smLib::vecToValar(res));
}

QPixmap FBedf::verbShortLong(double thres) const
{
	const int taskTyp = static_cast<int>(taskType::verb);

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
	if(shrts.empty()) { return {}; }

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

	/// magic consts
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

	ansType wrongsType{ansType::wrong};

	if(typ == taskType::spat) /// right vs wrong
	{
		wrongsType = ansType::wrong;
	}
	else if(typ == taskType::verb) /// ans vs noans
	{
		wrongsType = ansType::skip;
	}

	for(int i = 0; i < solvTime[static_cast<int>(typ)].size(); ++i)
	{
		if(!realsSpectra[static_cast<int>(typ)][i].isEmpty())
		{
			if(ans[static_cast<int>(typ)][i] == ansType::correct)	{ rights.push_back(realsSpectra[static_cast<int>(typ)][i]);	}
			else if(ans[static_cast<int>(typ)][i] == wrongsType)	{ wrongs.push_back(realsSpectra[static_cast<int>(typ)][i]);	}
		}
	}

	if(rights.empty() || wrongs.empty()) { return {}; }

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

	/// magic consts
	matrix both(2, 1);
	both[0] = rght.subCols(leftLim, rightLim).toValarByRows();
	both[1] = wrng.subCols(leftLim, rightLim).toValarByRows();

	auto tmplt = myLib::drw::drawTemplate(true, FBedf::leftFreq, FBedf::rightFreq, 19);
	return myLib::drw::drawArrays(tmplt, both);
}

matrix FBedf::backgroundCompare(taskType typ, ansType howSolved) const
{
	/// compose average spectre for the task
	matrix avTask{};
	{
		const int taskNum = static_cast<int>(typ);
		int counter = 1;
		for(int i = 0; i < realsSpectra[taskNum].size(); ++i)
		{
			if(!realsSpectra[taskNum][i].isEmpty()
			   && (static_cast<int>(ans[taskNum][i]) & static_cast<int>(howSolved)) /// check correctness
			   )
			{
				if(avTask.isEmpty())
				{
					avTask = realsSpectra[taskNum][i];
				}
				else
				{
					avTask += realsSpectra[taskNum][i];
					++counter;
				}
			}
		}
		avTask /= counter;
	}

	/// compose average spectre for the backgroung
	matrix avRest{};
	{
		int counter = 1;
		const int numRest = static_cast<int>(taskType::rest);
		for(int i = 0; i < realsSpectra[numRest].size(); ++i)
		{
			if( !realsSpectra[numRest][i].isEmpty() )
			{
				if( avRest.isEmpty() )
				{
					avRest = realsSpectra[numRest][i];
				}
				else
				{
					avRest += realsSpectra[numRest][i];
					++counter;
				}
			}
		}
		avRest /= counter;
	}
	return (avTask - avRest);
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

	std::vector<uint> windTypesUint(this->windTypes.size());
	std::transform(std::begin(this->windTypes),
				   std::end(this->windTypes),
				   std::begin(windTypesUint),
				   [](taskType in) { return uint(static_cast<int>(in)); });

	ClassifierData dt = ClassifierData(this->windSpectra, windTypesUint);
	/// arguments of wrong size
	net->setClassifierData(dt);

	net->adjustLearnRate(DEVNULL);
	net->crossClassification(10, 5, DEVNULL);
//	net->leaveOneOutClassification(DEVNULL);
	auto res = net->averageClassification(DEVNULL);

	delete net;
	return res;
}

} // end namespace fb
