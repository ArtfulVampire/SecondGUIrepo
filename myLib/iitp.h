#ifndef MYLIB_IITP_H
#define MYLIB_IITP_H

#include <complex>
#include <valarray>
#include <vector>
#include <cmath>
#include "matrix.h"

namespace iitp
{
/// IITP
std::complex<double> gFunc(const std::complex<double> & in);
/// PHI  = PHI(w) = crossSpectrum(w).arg();

std::valarray<std::complex<double>> crossSpectrum(
		const std::vector<std::valarray<double>> & in1,
		const std::vector<std::valarray<double>> & in2);

class iitpData
{
private:
	std::vector<matrix> piecesData;

	int fftLen;
	double spStep;

public:
	std::complex<double> coherency(int chan1, int chan2, double freq);
	void readPiecesFromEdf(const QString & edfPath,
						   int mark1,
						   int mark2);

	int setFftLen(); /// determine by piecesData lengths
	void setFftLen(int in);
	std::valarray<std::complex<double>> crossSpectrum(int chan1, int chan2);

};

} // namespace iitp

#endif // MYLIB_IITP_H
