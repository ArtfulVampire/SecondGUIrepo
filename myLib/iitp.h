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
					   int markerMax,
					   int markerMin);
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
