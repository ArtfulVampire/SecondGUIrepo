#include <other/feedback.h>

#include <myLib/output.h>
#include <myLib/statistics.h>
#include <myLib/valar.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <myLib/highlevel.h>
#include <myLib/signalProcessing.h>
#include <myLib/drw.h>
#include <myLib/ica.h>
#include <myLib/valar.h>

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

bool isGoodAns(ansType real, ansType expected)
{
	return (static_cast<int>(real) & static_cast<int>(expected));
}


/// FBedf

/// from 0 - for dispersion, distance, etc
const std::vector<int> FBedf::chansToProcess{
//	0, 1,		/// Fp1,	Fp2
//	2, 6,		/// F7,	F8
	3, 4, 5,	/// F3,	Fz,	F4
//	7, 11,		/// T3, T4
	8, 9, 10,	/// C3,	Cz,	C4
//	12, 16,		/// T5,	T6
	13, 14, 15,	/// P3,	Pz,	P4
//	17, 18		/// O1,	O2
};

const std::vector<int> FBedf::chansToZero{
	0, 1,		/// Fp1,	Fp2
	2, 6,		/// F7,	F8
//	3, 4, 5,	/// F3,	Fz,	F4
	7, 11,		/// T3, T4
//	8, 9, 10,	/// C3,	Cz,	C4
	12, 16,		/// T5,	T6
//	13, 14, 15,	/// P3,	Pz,	P4
	17, 18		/// O1,	O2
};

const std::vector<int> chansToClassify{
	0, 1,		/// Fp1,	Fp2
	2, 6,		/// F7,	F8
	3, 4, 5,	/// F3,	Fz,	F4
	7, 11,		/// T3, T4
	8, 9, 10,	/// C3,	Cz,	C4
	12, 16,		/// T5,	T6
	13, 14, 15,	/// P3,	Pz,	P4
	17, 18		/// O1,	O2
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

	/// doesn't work
//	zeroChannels(FBedf::chansToZero); ///////////////////// zero uninteresting channels

//	auto a = chansToClassify;
//	a.push_back(this->markerChannel);
//	*this = this->reduceChannels(a); /// except chansToClassify

	this->removeChannels(std::vector<int>{0, 1, 17, 18});

	/// arrange ans
	this->ansInRow = this->readAns(ansPath);
	this->ans.resize(numOfClasses);
	int c = 0;
	for(auto mrk : this->markers)
	{
		if(mrk.second == 241)		{ this->ans[0].push_back(this->ansInRow[c++]); }
		else if(mrk.second == 247)	{ this->ans[1].push_back(this->ansInRow[c++]); }
	}
	this->ans[2] = std::vector<ansType>(80, ansType::correct);

	/// divide to reals w/o markers
	auto realsD = myLib::sliceData(this->getData().subRows(-1), /// drop markers chansToClassify.size()
										  this->getMarkers());
	this->realsSignals = realsD.first;
	this->realsStarts = realsD.second;
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
	this->remakeWindows((1. - overlapPart) * windLen, numSkipStartWinds);
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
				std::cout << in2.rows() << "\t"; /// how many rows in reals;
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
	clData.z_transform();
	clData.adjust();
	return clData;
}

double FBedf::partOfCleanedWinds()
{
	this->remakeWindows(windFftLen, 0);
	auto clData = this->prepareClDataWinds(false);
	const double init = clData.size();

	ANN * ann = new ANN();
	ann->setClassifierData(clData);
	do
	{
		clData = *(ann->getClassifierData());
		ann->setClassifierData(clData);			/// to zero confusion matrix
//		ann->cleaningNfold(2);
//		ann->cleaningNfold(-1);
		ann->cleaningKfold(3, 2);
	}
	while(ann->averageClassification(DEVNULL).first != 100.);

	const double last = ann->getClassifierData()->size();

	return (init - last) / init;
}

void FBedf::remakeWindows(int windStep, int numSkipStartWinds)
{
	/// make windSignals
	const int maxNumWinds = (edfData.cols() - windLen) / windStep; /// a little bit excessive

	std::vector<matrix> windSigData{};		windSigData.reserve(maxNumWinds);
	std::vector<taskType> windSigTypes{};	windSigTypes.reserve(maxNumWinds);
	std::vector<ansType> windSigAns{};		windSigAns.reserve(maxNumWinds);
	std::vector<int> windSta{};				windSta.reserve(maxNumWinds);

	for(int i = 0; i < realsSignals.size(); ++i) /// i - type (0 - spat, 1 - verb, 2 - rest)
	{
		/// is rest?
		int skipStart = (i == static_cast<int>(taskType::rest))
						? 0 : numSkipStartWinds * windStep;

		for(int j = 0; j < realsSignals[i].size(); ++j)
		{
			for(int windStart = 1 + skipStart;
				windStart < realsSignals[i][j].cols() - windLen;
				windStart += windStep)
			{
				windSigData.push_back(realsSignals[i][j].subCols(windStart,
																 windStart + windLen));
				windSigTypes.push_back(static_cast<taskType>(i));
				windSta.push_back(realsStarts[i][j] + windStart);

				if(i != static_cast<int>(taskType::rest)) /// if not rest
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
	this->windStarts.clear();		this->windStarts.reserve(windSigAns.size());

	for(int i = 0; i < windSigData.size(); ++i)
	{
		matrix pew = myLib::countSpectre(windSigData[i],
										 windFftLen,
										 5); /// magic const
		if(!pew.isEmpty())
		{
			this->windSpectra.push_back(pew.subCols(getLeftLimWind(),
													getRightLimWind()).toValarByRows());
			this->windTypes.push_back(windSigTypes[i]);
			this->windAns.push_back(windSigAns[i]);
			this->windStarts.push_back(windSta[i]);
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


bool FBedf::isGoodAns(taskType typ, int numReal, ansType expected) const
{
	return fb::isGoodAns(ans[static_cast<int>(typ)][numReal], expected);
}

std::valarray<double> FBedf::spectralRow(taskType typ, ansType howSolved, int chan, double freq) const
{
	const int taskNum = static_cast<int>(typ);
	std::vector<double> res;	res.reserve(realsSpectra[taskNum].size());

	for(int i = 0; i < realsSpectra[taskNum].size(); ++i)
	{
		if(realsSpectra[taskNum][i].isEmpty()	/// may be empty spectre
		   || !isGoodAns(typ, i, howSolved)	/// may be uninteresting answer
		   )
		{
			continue;
		}
		/// else
		res.push_back(realsSpectra[taskNum][i][chan][ std::floor(freq / FBedf::spStep) ]);
	}
	return smLib::vecToValar(res);
}

double FBedf::distSpec(taskType type1, taskType type2, ansType howSolved)
{
	int all = 0;
	int diff = 0;

	for(int chan : FBedf::chansToProcess)
	{
		for(double freq : this->freqs)
		{
			auto a = myLib::MannWhitney(this->spectralRow(type1, howSolved, chan, freq),
										this->spectralRow(type2, howSolved, chan, freq),
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


void FBedf::calculateICA() const
{
	////////////////////////////////// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa
	myLib::ICAclass icaItem(this->getData().subRows(19));		/// magic const
	icaItem.setNumIC(19);										/// magic const
	icaItem.setEigValThreshold(1e-10);
	icaItem.setVectWThreshold(1e-12);

	/// set output paths
	icaItem.setExpName(this->getExpName());
	icaItem.setOutPaths(this->getDirPath() + "/Help");

	/// calculate result
	icaItem.calculateICA();
	QString outICA = this->getFilePath();	outICA.replace(".edf", "_ica.edf");
	matrix comps = icaItem.getComponents();	comps.push_back(this->getMarkArr());
	this->writeOtherData(comps, outICA);

	/// write something to files
	icaItem.printTxts();

	/// draw
	icaItem.drawMaps();
	icaItem.drawSpectraWithMaps();
}

int FBedf::individualAlphaPeakIndexWind() const
{
	/// take only rest?
	const int pz = findChannel("Pz");
	const int lef = fftLimit(leftAlpha, srate, windFftLen);
	const int rig = fftLimit(rightAlpha, srate, windFftLen);

	/// get only Pz alpha range
	matrix sub = windSpectra.subCols(pz * getSpLenWind() + lef, pz * getSpLenWind() + rig);

	/// get only rest windows
	std::vector<uint> rowsToErase{};
	for(int i = 0; i < windTypes.size(); ++i)
	{
		if(windTypes[i] != taskType::rest) { rowsToErase.push_back(i); }
	}
	sub.eraseRows(rowsToErase);

	return lef + myLib::indexOfMax(sub.averageRow());
}

int FBedf::individualAlphaPeakIndexReal() const
{
	/// take only rest?
	const int pz = findChannel("Pz");
	const int lef = fftLimit(leftAlpha, srate, realFftLen);
	const int rig = fftLimit(rightAlpha, srate, realFftLen);

	/// get only rest reals, prepare average spec
	const std::vector<matrix> & spectraRef = realsSpectra[static_cast<int>(taskType::rest)];
	std::valarray<double> tmp(rig - lef);

	/// average spectra
	{
		int counter = 0;
		for(const auto & in : spectraRef)
		{
			if(!in.isEmpty())
			{
				tmp += smLib::contSubsec(in[pz], lef, rig); /// only pz alpha
				++counter;
			}
		}
		tmp /= counter;
	}

	return lef + myLib::indexOfMax(tmp);
}

double FBedf::spectreDispersion(taskType typ, ansType howSolved) const
{
	/// remake via subRows, subcols, transpose
	double res = 0.;
	for(int chan : FBedf::chansToProcess)
	{
		for(double freq : this->freqs)
		{
			auto row = this->spectralRow(typ, howSolved, chan, freq);
			res += smLib::sigma(row) / smLib::mean(row);
		}
	}
	return res;
}

double FBedf::spectreDispersionWinds(taskType typ, ansType howSolved) const
{
	/// remake via subRows, subcols, transpose
	int counter = 0;
	std::vector<int> goodRows{};
	for(auto in : this->windTypes)
	{
		if(in == typ && fb::isGoodAns(howSolved, windAns[counter]))
		{
			goodRows.push_back(counter);
		}

		++counter;
	}
	matrix windSpectraGood = windSpectra.subRows(goodRows);
	std::vector<std::pair<int, int>> goodCols{};

	for(int chan : FBedf::chansToProcess)
	{
		goodCols.push_back({chan * this->getSpLenWind(), (chan + 1) * this->getSpLenWind()});
	}
	windSpectraGood = windSpectraGood.subCols(goodCols);

	return (windSpectraGood.sigmaOfCols() / windSpectraGood.averageRow()).sum();
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
		if( isGoodAns(typ, i, howSolved) )
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
		if(fb::isGoodAns(in, howSolved)) { ++num; }
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

	/// magic consts
	matrix both(2, 1);
	both[0] = shrt.subCols(getLeftLim(), getRightLim()).toValarByRows();
	both[1] = lng.subCols(getLeftLim(), getRightLim()).toValarByRows();

	auto tmplt = myLib::drw::drawTemplate(true, FBedf::leftFreq, FBedf::rightFreq, 19);
	return myLib::drw::drawArrays(tmplt, both);
}

QPixmap FBedf::rightWrongSpec(taskType typ) const
{
	const int taskNum = static_cast<int>(typ);
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

	for(int i = 0; i < solvTime[taskNum].size(); ++i)
	{
		if(!realsSpectra[taskNum][i].isEmpty())
		{
			if(ans[taskNum][i] == ansType::correct)	{ rights.push_back(realsSpectra[taskNum][i]);	}
			else if(ans[taskNum][i] == wrongsType)	{ wrongs.push_back(realsSpectra[taskNum][i]);	}
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

	/// magic consts
	matrix both(2, 1);
	both[0] = rght.subCols(getLeftLim(), getRightLim()).toValarByRows();
	both[1] = wrng.subCols(getLeftLim(), getRightLim()).toValarByRows();

	auto tmplt = myLib::drw::drawTemplate(true, FBedf::leftFreq, FBedf::rightFreq, 19);
	return myLib::drw::drawArrays(tmplt, both);
}

matrix FBedf::backgroundCompare(taskType typ, ansType howSolved) const
{
	/// compose average spectre for the task
	matrix avTask{};
	{
		int counter = 1;
		const int taskNum = static_cast<int>(typ);
		for(int i = 0; i < realsSpectra[taskNum].size(); ++i)
		{
			if(!realsSpectra[taskNum][i].isEmpty()
			   && isGoodAns(typ, i, howSolved) /// check correctness
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
		const int restNum = static_cast<int>(taskType::rest);
		for(int i = 0; i < realsSpectra[restNum].size(); ++i)
		{
			if( !realsSpectra[restNum][i].isEmpty() )
			{
				if( avRest.isEmpty() )
				{
					avRest = realsSpectra[restNum][i];
				}
				else
				{
					avRest += realsSpectra[restNum][i];
					++counter;
				}
			}
		}
		avRest /= counter;
	}
	return (avTask - avRest);
}

Classifier::avType FBedf::classifyReals(bool alphaFlag) const
{
	DEFS.setFftLen(FBedf::realFftLen);
	ANN * net = new ANN();

	std::vector<uint> types{};	types.reserve(160);
	matrix clData{};			clData.reserve(160);


	int leftLim{};
	int rightLim{};

	if(alphaFlag)
	{
		const int alphaIndex = this->individualAlphaPeakIndexReal();
		const int twoHzRange = fftLimit(2., srate, realFftLen);
		leftLim = alphaIndex - twoHzRange;
		rightLim = alphaIndex + twoHzRange;
	}
	else
	{
		leftLim = getLeftLim();
		rightLim = getRightLim();
	}

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
	net->crossClassification(10, 5, DEVNULL);
	auto res = net->averageClassification(DEVNULL);

	delete net;
	return res;
}

Classifier::avType FBedf::classifyWinds(bool alphaFlag) const
{
	DEFS.setFftLen(windFftLen);

	ANN * net = new ANN();

	std::vector<uint> windTypesUint(this->windTypes.size());
	std::transform(std::begin(this->windTypes),
				   std::end(this->windTypes),
				   std::begin(windTypesUint),
				   [](taskType in) { return uint(static_cast<int>(in)); });

	/// class by alpha ?
	const int alphaIndex = this->individualAlphaPeakIndexWind();
	const int twoHzRange = fftLimit(2., srate, windFftLen);

	std::vector<std::pair<int, int>> alphaRange{};
	for(int i = 0; i < this->getNs() - 1; ++i) /// magic const num of channels chansToClassify.size()
	{
		alphaRange.push_back({i * getSpLenWind() + alphaIndex - twoHzRange,
							  i * getSpLenWind() + alphaIndex + twoHzRange});
	}

	ClassifierData dt{};
	if(alphaFlag)
	{
		dt = ClassifierData(windSpectra.subCols(alphaRange), windTypesUint);
	}
	else
	{
		dt = ClassifierData(this->windSpectra, windTypesUint);
	}

	/// arguments of wrong size
	net->setClassifierData(dt);
	net->adjustLearnRate(DEVNULL);
	net->crossClassification(10, 5, DEVNULL);
//	net->leaveOneOutClassification(DEVNULL);
	auto res = net->averageClassification(DEVNULL);

	delete net;
	return res;
}

} /// end namespace fb
