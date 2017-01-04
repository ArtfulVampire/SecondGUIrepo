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

std::valarray<std::complex<double>> crossSpectrum(
		const std::vector<std::valarray<double>> & in1,
		const std::vector<std::valarray<double>> & in2);

class iitpData : public edfFile
{
private:
	std::vector<matrix> piecesData;

	int fftLen;
	double spStep;

public:
	std::complex<double> coherency(int chan1, int chan2, double freq);
	std::valarray<std::complex<double>> crossSpectrum(int chan1, int chan2);

	void setPieces(int mark1 = 10, int mark2 = 20);

	int setFftLen(); /// determine by piecesData lengths
	void setFftLen(int in);

};

} // namespace iitp

#endif // MYLIB_IITP_H
