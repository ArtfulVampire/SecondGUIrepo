#ifndef MYLIB_IITP_H
#define MYLIB_IITP_H

#include <complex>
#include <valarray>
#include <vector>
#include <cmath>

#include <other/matrix.h>
#include <other/edffile.h>

namespace iitp
{
/// IITP
std::complex<double> gFunc(const std::complex<double> & in);
/// PHI  = PHI(w) = crossSpectrum(w).arg();


std::complex<double> coherency(const std::vector<std::valarray<double>> & sig1,
							   const std::vector<std::valarray<double>> & sig2,
							   double srate,
							   double freq);







///// max - bend (flexed)
///// min - unbend (extended)
int gonioMinMarker(int numGonioChan); /// max = min + 1

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
	{Ankle_l, Ankle_r, Knee_l, Knee_r, Wrist_l, Wrist_r},
	// 11 legs + wrists imaginary
	{},
	// 12 static stress
	{},
	// 13 middle eyes closed
	{},
	// 14 middle eyes open
	{},
	// 15 arms
	{Elbow_l, Elbow_r, Wrist_l, Wrist_r},
	// 16 arms imaginary
	{},
	// 17 arms + legs
	{Elbow_l, Elbow_r, Wrist_l, Wrist_r, Ankle_l, Ankle_r, Knee_l, Knee_r},
	// 18 arms + legs imaginagy
	{},
	// 19 legs passive
	{Ankle_l, Ankle_r, Knee_l, Knee_r},
	// 20 arms passive
	{Elbow_l, Elbow_r, Wrist_l, Wrist_r},
	// 21 arms+legs passive
	{Elbow_l, Elbow_r, Wrist_l, Wrist_r, Ankle_l, Ankle_r, Knee_l, Knee_r},
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
const std::vector<std::valarray<int>> interestEmg{
	// 0 eyes closed
	{},
	// 1 eyes open
	{},
	// 2 legs
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 3 legs imaginary
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 4 feet
	{Ta_l, Ta_r},
	// 5 feet imaginary
	{Ta_l, Ta_r},
	// 6 wrists
	{Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 7 wrists imaginary
	{Fcr_l, Fcr_r, Ecr_l, Ecr_r},
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
	{},
	// 14 middle eyes open
	{},
	// 15 arms
	{Da_l, Da_r, Dp_l, Dp_r},
	// 16 arms imaginary
	{Da_l, Da_r, Dp_l, Dp_r},
	// 17 arms + legs
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 18 arms + legs imaginagy
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 19 legs passive
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 20 arms passive
	{Da_l, Da_r, Dp_l, Dp_r},
	// 21 arms + legs passive
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 22 final eyes closed
	{},
	// 23 final eyes open
	{},
	// 24 weak Ta_l
	{Ta_l},
	// 25 weak Ta_r
	{Ta_r},
	// 26 weak Fcr_l
	{Fcr_l},
	// 27 weak Fcr_r
	{Fcr_r},
	// 28 weak Ecr_l
	{Ecr_l},
	// 29 weak Ecr_r
	{Ecr_r}
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

const std::valarray<int> interestEeg{
//	0,	// Fp1
//	1,	// Fp2,
//	2,	// F7
	3,	// F3
	4,	// Fz
//	5,	// F4 - usually bad
//	6,	// F8
	7,	// T3
	8,	// C3
	9,	// Cz
	10,	// C4
	11,	// T4
//	12,	// T5
	13,	// P3
	14,	// Pz
//	15,	// P4 - usually bad
//	16,	// T5
//	17,	// O1
//	18	// O2
};

/// for spectra inspection
const double leftFr = 4;
const double rightFr = 40;

const std::vector<double> interestFrequencies = smLib::range<std::vector<double>>(8, 30);
//const std::valarray<double> interestFrequencies = smLib::range(8, 45);

const std::valarray<double> fileNums = smLib::range<std::valarray<double>>(0, 29);
//const std::valarray<double> fileNums = smLib::range(0, 5);
//const std::valarray<double> fileNums{4};


class iitpData : public edfFile
{
private:
	std::vector<matrix> piecesData;

	std::vector<std::vector<std::valarray<std::complex<double>>>> piecesFFT;
	std::vector<std::vector<std::valarray<std::complex<double>>>> coherenciesR;


	std::vector<std::vector<std::valarray<std::complex<double>>>> crossSpectra;
	std::vector<std::vector<std::valarray<std::complex<double>>>> coherencies;

	int fftLen = -1;
	double spStep = 0.;

public:
	std::complex<double> coherency(int chan1, int chan2, double freq);
	std::complex<double> coherencyR(int chan1, int chan2, double freq);
	void crossSpectrum(int chan1, int chan2);

	iitpData & staging(int numGonioChan);
	iitpData & staging(const QString & chanName,
					   int markerMin,
					   int markerMax);

	QString getGuy() {return iitp::getGuyName(this->ExpName);}
	QString getPost() {return iitp::getPostfix(this->ExpName);}
	QString getInit() {return iitp::getInitName(this->ExpName);}
	int getNum() {return iitp::getFileNum(this->ExpName);}


	void cutPieces(double length);
	void setPieces(int startMark = 10, int finishMark = 20);
	void countFlexExtSpectra(int mark1, int mark2);
	void countImagPassSpectra();

	void countPiecesFFT();
	void resizePieces(int in);
	void getPiecesParams();
	const std::vector<matrix> & getPieces() {return piecesData;}

	void clearCrossSpectra();
	int setFftLen(); /// determine by piecesData lengths
	void setFftLen(int in);

};

} // namespace iitp

#endif // MYLIB_IITP_H
