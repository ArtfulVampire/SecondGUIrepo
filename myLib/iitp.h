#ifndef MYLIB_IITP_H
#define MYLIB_IITP_H

#include <complex>
#include <valarray>
#include <vector>
#include <cmath>

#include <other/matrix.h>
#include <other/edffile.h>
#include <other/defaults.h>
#include <myLib/signalProcessing.h>


namespace iitp
{
/// IITP
std::complex<double> gFunc(const std::complex<double> & in);
/// PHI  = PHI(w) = crossSpectrum(w).arg();


/// standalone for testing
QPixmap phaseDifferences(const std::valarray<double> & sig1,
						 const std::valarray<double> & sig2,
						 double srate,
						 double freq, int fftLen);

std::valarray<std::complex<double>> coherenciesUsual(const std::valarray<double> & sig1,
													 const std::valarray<double> & sig2, double overlapPercent,
													 double srate,
													 int fftLen);

std::complex<double> coherencyUsual(const std::valarray<double> & sig1,
									const std::valarray<double> & sig2,
									double srate,
									double freq, int fftLen);








///// max - bend (flexed)
///// min - unbend (extended)
int gonioMinMarker(int numGonioChan); /// max = min + 1
int gonioNum(int marker);
QString gonioName(int marker);

QString getGuyName(const QString & fileName);
QString getInitName(const QString & fileName);
QString getPostfix(const QString & fileName);
int getFileNum(const QString & fileName);


const std::vector<QString> trialTypesNames
{
	"rest",
	"real",
	"imag",
	"passive",
	"stat"
};

/// class because of simple names
enum trialType {rest	= 0,
				real	= 1,
				imag	= 2,
				passive	= 3,
				stat	= 4};

const std::valarray<int> trialTypes{
	// 0 eyes closed
	trialType::rest,
	// 1 eyes open
	trialType::rest,
	// 2 legs
	trialType::real,
	// 3 legs imaginary
	trialType::imag,
	// 4 feet
	trialType::real,
	// 5 feet imaginary
	trialType::imag,
	// 6 wrists
	trialType::real,
	// 7 wrists imaginary
	trialType::imag,
	// 8 feet + wrists
	trialType::real,
	// 9 feet + wrists imaginary
	trialType::imag,
	// 10 legs + wrists
	trialType::real,
	// 11 legs + wrists imaginary
	trialType::imag,
	// 12 static stress
	trialType::stat,
	// 13 middle eyes closed
	trialType::rest,
	// 14 middle eyes open
	trialType::rest,
	// 15 arms
	trialType::real,
	// 16 arms imaginary
	trialType::imag,
	// 17 arms + legs
	trialType::real,
	// 18 arms + legs imaginagy
	trialType::imag,
	// 19 legs passive
	trialType::passive,
	// 20 arms passive
	trialType::passive,
	// 21 arms+legs passive
	trialType::passive,
	// 22 final eyes closed
	trialType::rest,
	// 23 final eyes open
	trialType::rest,
	// 24 weak Ta_l
	trialType::stat,
	// 25 weak Ta_r
	trialType::stat,
	// 26 weak Fcr_l
	trialType::stat,
	// 27 weak Fcr_r
	trialType::stat,
	// 28 weak Ecr_l
	trialType::stat,
	// 29 weak Ecr_r
	trialType::stat
};


enum gonioChans {Ankle_l	= 0,
				 Ankle_r	= 1,
				 Knee_l		= 2,
				 Knee_r		= 3,
				 Elbow_l	= 4,
				 Elbow_r	= 5,
				 Wrist_l	= 6,
				 Wrist_r	= 7};

const std::vector<QString> gonioNames {
	"Ankle_l",
	"Ankle_r",
	"Knee_l",
	"Knee_r",
	"Elbow_l",
	"Elbow_r",
	"Wrist_l",
	"Wrist_r"};

/// [numFile] - interestingChannels
/// commented wrists and ankles - letter 7-Nov-17
const std::vector<std::valarray<int>> interestGonios{
	// 0 eyes closed
	{},
	// 1 eyes open
	{},
	// 2 legs
	{Ankle_l, Ankle_r, Knee_l, Knee_r},
	// 3 legs imaginary
	{},
	// 4 feet
	{Ankle_l, Ankle_r},
	// 5 feet imaginary
	{},
	// 6 wrists
	{Wrist_l, Wrist_r},
	// 7 wrists imaginary
	{},
	// 8 feet + wrists
	{Ankle_l, Ankle_r, Wrist_l, Wrist_r},
	// 9 feet + wrists imaginary
	{},
	// 10 legs + wrists
	{ /*Ankle_l, Ankle_r, */ Knee_l, Knee_r, Wrist_l, Wrist_r},
	// 11 legs + wrists imaginary
	{},
	// 12 static stress
	{},
	// 13 middle eyes closed
	{},
	// 14 middle eyes open
	{},
	// 15 arms
	{Elbow_l, Elbow_r /*, Wrist_l, Wrist_r*/ },
	// 16 arms imaginary
	{},
	// 17 arms + legs
	{Elbow_l, Elbow_r, Knee_l, Knee_r /*, Ankle_l, Ankle_r, Wrist_l, Wrist_r*/ },
	// 18 arms + legs imaginagy
	{},
	// 19 legs passive
	{Knee_l, Knee_r  /*,Ankle_l, Ankle_r*/ },
	// 20 arms passive
	{Elbow_l, Elbow_r /*, Wrist_l, Wrist_r*/ },
	// 21 arms+legs passive
	{Elbow_l, Elbow_r, Knee_l, Knee_r /*, Wrist_l, Wrist_r, Ankle_l, Ankle_r*/ },
	// 22 final eyes closed
	{},
	// 23 final eyes open
	{},
	// 24 weak Ta_l
	{},
	// 25 weak Ta_r
	{},
	// 26 weak Fcr_l
	{},
	// 27 weak Fcr_r
	{},
	// 28 weak Ecr_l
	{},
	// 29 weak Ecr_r
	{}
};



enum emgChans {Ta_l		= 0,
			   Ta_r		= 1,
			   Bf_l		= 2,
			   Bf_r		= 3,
			   Fcr_l	= 4,
			   Fcr_r	= 5,
			   Ecr_l	= 6,
			   Ecr_r	= 7,
			   Da_l		= 8,
			   Da_r		= 9,
			   Dp_l		= 10,
			   Dp_r		= 11
			  };

const std::vector<QString> emgNamesPrefixes {
	"Ta",
	"Bf",
	"Fcr",
	"Ecr",
	"Da",
	"Dp"
};


const std::vector<QString> emgNames {
	"Ta_l",
	"Ta_r",
	"Bf_l",
	"Bf_r",
	"Fcr_l",
	"Fcr_r",
	"Ecr_l",
	"Ecr_r",
	"Da_l",
	"Da_r",
	"Dp_l",
	"Dp_r"};



/// [numFile] - interestingChannels
/// maybe all EMGs for rest and/or stat
/// 1 May 17 - added Ta_l, Ta_r everywhere for maps drawing
const std::vector<std::valarray<int>> interestEmg{
	// 0 eyes closed
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 1 eyes open
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 2 legs
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 3 legs imaginary
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 4 feet
	{Ta_l, Ta_r},
	// 5 feet imaginary
	{Ta_l, Ta_r},
	// 6 wrists
	{Fcr_l, Fcr_r, Ecr_l, Ecr_r,    Ta_l, Ta_r},
	// 7 wrists imaginary
	{Fcr_l, Fcr_r, Ecr_l, Ecr_r,    Ta_l, Ta_r},
	// 8 feet + wrists
	{Ta_l, Ta_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 9 feet + wrists imaginary
	{Ta_l, Ta_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 10 legs + wrists
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 11 legs + wrists imaginary
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 12 static stress
	{},
	// 13 middle eyes closed
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 14 middle eyes open
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 15 arms
	{Da_l, Da_r, Dp_l, Dp_r,    Ta_l, Ta_r},
	// 16 arms imaginary
	{Da_l, Da_r, Dp_l, Dp_r,    Ta_l, Ta_r},
	// 17 arms + legs
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 18 arms + legs imaginagy
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 19 legs passive
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 20 arms passive
	{Da_l, Da_r, Dp_l, Dp_r,    Ta_l, Ta_r},
	// 21 arms + legs passive
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 22 final eyes closed
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 23 final eyes open
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 24 weak Ta_l
	{Ta_l,    Ta_r},
	// 25 weak Ta_r
	{Ta_r,    Ta_l},
	// 26 weak Fcr_l
	{Fcr_l,    Ta_l, Ta_r},
	// 27 weak Fcr_r
	{Fcr_r,    Ta_l, Ta_r},
	// 28 weak Ecr_l
	{Ecr_l,    Ta_l, Ta_r},
	// 29 weak Ecr_r
	{Ecr_r,    Ta_l, Ta_r}
};

const std::vector<QString> eegNames{
	"Fp1",
	"Fp2",
	"F7",
	"F3",
	"Fz",
	"F4",
	"F8",
	"T3",
	"C3",
	"Cz",
	"C4",
	"T4",
	"T5",
	"P3",
	"Pz",
	"P4",
	"T6",
	"O1",
	"O2"};

const std::vector<int> interestEeg{
//	0,	// Fp1
//	1,	// Fp2,
	2,	// F7
	3,	// F3
	4,	// Fz
//	5,	// F4 - usually bad
	6,	// F8
	7,	// T3
	8,	// C3
	9,	// Cz
	10,	// C4
	11,	// T4
	12,	// T5
	13,	// P3
	14,	// Pz
//	15,	// P4 - usually bad
	16,	// T5
	17,	// O1
	18	// O2
};

/// for spectra inspection
const double leftFr = 4.;
const double rightFr = 40.;
const myLib::windowName iitpWindow = myLib::windowName::Hamming;

const std::vector<double> interestFrequencies = smLib::range<std::vector<double>>(8, 45 + 1);

/// all without 12th
//const std::valarray<double> fileNums = smLib::unite({smLib::range(0,  11 + 1),
//													 smLib::range(13, 29 + 1)});
/// all with 12th
const std::valarray<double> fileNums = smLib::range(0, 29+1);

/// test
//const std::valarray<double> fileNums = smLib::range(0, 8);
//const std::valarray<double> fileNums = smLib::range(22, 22+1);
//const std::valarray<double> fileNums = smLib::range(0, 12);
//const std::valarray<double> fileNums{0, 1, 8, 22, 23, 26, 27};
//const std::valarray<double> fileNums{15, 16, 17, 18, 22, 23, 24, 25, 26, 27};

const std::map<QString, std::vector<int>> badFiles{
	{"Alex",	{0, 1, 2, 3, 4, 5}},
	{"Boris",	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},
	{"Dima",	{2}},
	{"Egor",	{5}},
	{"Ira",		{11, 13}},
	{"Isakov",	{9}},
	{"Victor",	{9}}
};

class iitpData : public edfFile
{
public:
	/// [chan1][chan2][freq]
	using cohsType = std::vector<std::vector<std::valarray<std::complex<double>>>>;
	/// [chan1][chan2][freq]
	using mscohsType = std::vector<std::vector<std::valarray<double>>>;
private:
	std::vector<matrix> piecesData;

	/// [pieceNumber][channel][array of complex FFT]
	std::vector<std::vector<std::valarray<std::complex<double>>>> piecesFFT;
	/// [chan1][chan2][array of complex] - average crossSpecrum over pieces
	std::vector<std::vector<std::valarray<std::complex<double>>>> crossSpectra;

	/// [chan1][chan2][freq]
	cohsType coherencies;
	mscohsType mscoherencies;

	int fftLen = -1;
	int fftLenW = 256; /// ~1 sec
	double spStep = 0.;

public:
	std::complex<double> coherency(int chan1, int chan2, double freq);

	double confidence(double level) const
	{ return 1. - std::pow(level, 1. / (piecesData.size() - 1)); }

	const cohsType & getCoherencies() const { return coherencies; }
	const mscohsType & getMSCoherencies() const { return mscoherencies; }

	void countCrossSpectrum(int chan1, int chan2);
	void countCrossSpectrumW(int chan1, int chan2, double overlap);

	iitpData & staging(int numGonioChan);
	iitpData & staging(const QString & chanName,
					   int markerMin,
					   int markerMax);

	QString getGuy() const { return iitp::getGuyName(this->ExpName); }
	QString getPost() const { return iitp::getPostfix(this->ExpName); }
	QString getInit() const { return iitp::getInitName(this->ExpName); }
	int getNum() const { return iitp::getFileNum(this->ExpName); }

	/// continious task
	void cutPieces(double length);
	void cutPiecesW(double overlap);
	void countContiniousTaskSpectra();
	void countContiniousTaskSpectraW(const QString & resPath, double overlap);

	/// periodic task
	void setPieces(int startMark = 10, int finishMark = 20);
	void setPiecesW(int startMark, int finishMark, double overlap);
	void countFlexExtSpectra(int mark1, int mark2);
	void countFlexExtSpectraW(const QString & resPath, int mark1, int mark2, double overlap);

	void countPiecesFFT();
	void resizePieces(int in);
	void getPiecesParams();
	const std::vector<matrix> & getPieces() const { return piecesData; }

	void clearCrossSpectra();
	int setFftLen(); /// determine by piecesData lengths
	void setFftLen(int in);

	int getFftLen() const { return fftLen; }
	double getSpStep() const { return spStep; }
	double getSpStepW() const { return srate / fftLenW; }

};

/// EXPERIMENTAL
class fmRange // ~fmValue
{
private:
	double leftLim{};
	double rightLim{};
	std::valarray<double> meanVal{};
	std::valarray<double> maxVal{};

public:
	fmRange(double left, double right) : leftLim(left), rightLim(right)
	{
		meanVal.resize(19); meanVal = 0.;
		maxVal.resize(19); maxVal = 0.;
	}
	~fmRange() {}

	const std::valarray<double> & getMeanVal() const{ return meanVal; }
	double getMeanVal(int i) const					{ return meanVal[i]; }
	void setMeanVal(int i, double val)				{ meanVal[i] = val; }
	void meanValZero()								{ meanVal = 0.; }
	void meanValDivide(double denom)				{ meanVal /= denom; }

	const std::valarray<double> & getMaxVal() const	{ return maxVal; }
	double getMaxVal(int i) const					{ return maxVal[i]; }
	void setMaxVal(int i, double val)				{ maxVal[i] = val; }
	void maxValZero()								{ maxVal = 0.; }

	double lef() const								{ return leftLim; }
	double rig() const								{ return rightLim; }

};

/// EXPERIMENTAL
class fmRanges
{
private:
	/// alpha beta gamma
	std::vector<fmRange> ranges{{8., 13.}, {13., 25.}, {25., 45.}};
	std::vector<fmRange>::iterator begin() { return std::begin(ranges); }
	std::vector<fmRange>::iterator end() { return std::end(ranges); }

public:
	fmRanges() {}
	~fmRanges() {}

	fmRange & operator[](int i) { return ranges[i]; } /// bad incapsulation
	const fmRange & operator[](int i) const { return ranges[i]; } /// bad incapsulation

	int size() const { return ranges.size(); }
};

const std::vector<QString> forMapEmgNames {
	"Ta_l",
	"Ta_r",
	"Fcr_l",
	"Fcr_r",
	"Ecr_l",
	"Ecr_r"
};

const std::vector<QString> forMapRangeNames {
	"alpha",
	"beta",
	"gamma"
};

struct forMap
{
	int fileNum{-1};
	QString fileType{"notype"};
	QString gonio;
	std::vector<fmRanges> forMapRanges{6};
	std::vector<int> fmChans;

	bool isBad(int numEmg) const
	{
		/// may not have such Emg channel
		/// OR it is not interesting EMG,
		/// i.e. the signal may be bad and coherencies weren't calced
		///
//		std::cout << "isGood(" << numEmg << "):" << std::endl;
//		std::cout << "num of chan (should != -1) " << fmChans[numEmg] << std::endl;
//		std::cout << "is emg in interest chans? " << myLib::contains(iitp::interestEmg[fileNum],
//																	 myLib::indexOfVal(iitp::emgNames,
//																					   iitp::forMapEmgNames[numEmg])) << std::endl;
		return (fmChans[numEmg] == -1) ||
				(!myLib::contains(iitp::interestEmg[fileNum],
								 myLib::indexOfVal(iitp::emgNames,
												   iitp::forMapEmgNames[numEmg])));
	}

	~forMap(){}
	forMap(){}
	forMap(const iitp::iitpData::mscohsType & in,
		   const iitp::iitpData & inFile,
		   int filNum,
		   QString typ,
		   QString gon)
	{
		fileNum = filNum;
		fileType = typ;
		gonio = gon;



		fmChans.clear();
		for(QString emgNam : iitp::forMapEmgNames)
		{
			fmChans.push_back(inFile.findChannel(emgNam));
		}

		/// new 28.11.17 - EXPERIMENTAL
//		std::cout << fmChans << std::endl;
		for(int i = 0; i < forMapRanges.size(); ++i) /// i ~ EMG channel
		{
			if(isBad(i)) { continue; }

			for(int j = 0; j < forMapRanges[i].size(); ++j) /// j ~ alpha/beta/gamma
			{

				fmRange & pewpew = forMapRanges[i][j];

				pewpew.maxValZero();
				pewpew.meanValZero();

				for(int eegNum : iitp::interestEeg)
				{
					for(int FR = pewpew.lef(); FR < pewpew.rig(); ++FR)
					{
						const auto & val = in[eegNum][fmChans[i]][FR / inFile.getSpStepW()];
						if(std::abs(val) >= 1.0)
						{
							std::cout << "abs(coh) > 1"
									  << " fileNum " << fileNum
									  << " type " << typ
										 << " eeg " << inFile.getLabels(eegNum)
										 << " EMG " << iitp::forMapEmgNames[i]
										 << " freq " << FR
										 << " rhythm " << iitp::forMapRangeNames[j]
											<< std::endl;
						}
//						pewpew.meanVal[eegNum] += val;
//						pewpew.maxVal[eegNum] = std::max(pewpew.maxVal[eegNum], val);

						pewpew.setMeanVal(eegNum, val);
						pewpew.setMaxVal(eegNum,
										 std::max(pewpew.getMaxVal(eegNum), val));
					}
				}
//				pewpew.meanVal /= pewpew.rightLim - pewpew.leftLim;
				pewpew.meanValDivide(pewpew.rig() - pewpew.lef());
			}
		}

		if(0)
		{
			std::cout << fileType << std::endl;
			std::cout << std::fixed;
			std::cout.precision(3);
			for(int i = 0; i < forMapRanges.size(); ++i) /// i ~ EMG channel
			{
				for(int j = 0; j < forMapRanges[i].size(); ++j) /// j ~ alpha/beta/gamma
				{
					const fmRange & pewpew = forMapRanges[i][j];

					std::cout << fileNum << " " << i << " " << j << " : " << std::endl;
//					std::cout << pewpew.meanVal << std::endl;
//					std::cout << pewpew.maxVal << std::endl;

					std::cout << pewpew.getMeanVal() << std::endl;
					std::cout << pewpew.getMaxVal() << std::endl;
				}
				std::cout << std::endl;
			}
			std::cout << std::defaultfloat;
		}

	}
};




} // namespace iitp

#endif // MYLIB_IITP_H
