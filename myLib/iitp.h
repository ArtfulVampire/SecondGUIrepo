#ifndef MYLIB_IITP_H
#define MYLIB_IITP_H

#include <complex>
#include <valarray>
#include <vector>
#include <cmath>
#include "matrix.h"
#include "edffile.h"

namespace iitp
{
/// IITP
std::complex<double> gFunc(const std::complex<double> & in);
/// PHI  = PHI(w) = crossSpectrum(w).arg();


std::complex<double> coherency(const std::vector<std::valarray<double>> & sig1,
							   const std::vector<std::valarray<double>> & sig2,
							   double srate,
							   double freq);
//namespace marks
//{
///// max - bend
///// min - unbend
/// marker = 100 + (gonioNamesIndex / 2 + 1) * 10 + ("_l")?0:1 * 2 + ("_max")?1:0
//const int Ankle_l_min	= 110;
//const int Ankle_l_max	= 111;
//const int Ankle_r_min	= 112;
//const int Ankle_r_max	= 113;

//const int Knee_l_min	= 120;
//const int Knee_l_max	= 121;
//const int Knee_r_min	= 122;
//const int Knee_r_max	= 123;

//const int Elbow_l_min	= 130;
//const int Elbow_l_max	= 131;
//const int Elbow_r_min	= 132;
//const int Elbow_r_max	= 133;

//const int Wrist_l_min	= 140;
//const int Wrist_l_max	= 141;
//const int Wrist_r_min	= 142;
//const int Wrist_r_max	= 143;
//}

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

enum emgChans {Ta_l		= 0,
			   Ta_r		= 1,
			   Bf_l		= 2,
			   Bf_r		= 3,
			   Fcr_l	= 4,
			   Fcr_r	= 5,
			   Ecr_l	= 6,
			   Ecr_r	= 7};

const std::vector<QString> emgNames {
	"Ta_l",
	"Ta_r",
	"Bf_l",
	"Bf_r",
	"Fcr_l",
	"Fcr_r",
	"Ecr_l",
	"Ecr_r"};

/// [numFile] - interestingChannels
const std::vector<std::vector<int>> interestEmg{
	// 0 eyes closed
	{},
	// 1 eyes open
	{},
	// 2 legs
	{Knee_l, Knee_r},
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
	{Knee_l, Knee_r, Wrist_l, Wrist_r},
	// 11 legs + wrists imaginary
	{},
	// 12 static stress
	{},
	// 13 middle eyes closed
	{},
	// 14 middle eyes open
	{},
	// 15 arms
	{Elbow_l, Elbow_r},
	// 16 arms imaginary
	{},
	// 17 arms + legs
	{Elbow_l, Elbow_r, Knee_l, Knee_r},
	// 18 arms + legs imaginagy
	{},
	// 19 legs passive
	{Knee_l, Knee_r},
	// 20 arms passive
	{Elbow_l, Elbow_r},
	// 21 arms+legs passive
	{Elbow_l, Elbow_r, Knee_l, Knee_r},
	// 22 final eyes closed
	{},
	// 23 final eyes open
	{}
};


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

	iitpData & staging(const QString & chanName,
					   int markerMin,
					   int markerMax);

	iitpData & staging(int numGonioChan);
	void cutPieces(double length);
	void setPieces(int start = 10, int finish = 20);
	QPixmap drawSpectra(int mark1, int mark2);

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
