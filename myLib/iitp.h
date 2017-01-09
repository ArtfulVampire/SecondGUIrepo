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
	std::vector<std::vector<std::valarray<std::complex<double>>>> crossSpectra;

	int fftLen = -1;
	double spStep = 0.;

public:
	std::complex<double> coherency(int chan1, int chan2, double freq);
	void crossSpectrum(int chan1, int chan2);

	void setPieces(int mark1 = 10, int mark2 = 20);
	void cutPieces(double length);
	void resizePieces(int in);
	void getPiecesParams();
	const std::vector<matrix> & getPieces() {return piecesData;}

	void clearCrossSpectra();
	int setFftLen(); /// determine by piecesData lengths
	void setFftLen(int in);

};

} // namespace iitp

#endif // MYLIB_IITP_H