#include "iitp.h"
#include <myLib/signalProcessing.h>
#include <myLib/output.h>

using namespace myOut;


namespace iitp
{
std::complex<double> gFunc(const std::complex<double> & in)
{

	return (1 - pow(std::abs(in), 2)) * pow(atanh(std::abs(in)), 2) / std::norm(in);
}



std::complex<double> coherency(const std::vector<std::valarray<double>> & sig1,
							   const std::vector<std::valarray<double>> & sig2,
							   double srate,
							   double freq)
{
	if(sig1.size() != sig2.size())
	{
		return {};
	}
	int siz = sig1.size();

	/// define fftLen
	int fftLen = 0;
	std::for_each(std::begin(sig1),
				  std::end(sig1),
				  [&fftLen](const std::valarray<double> & in)
	{
		fftLen = std::max(size_t(fftLen), in.size());
	});
	fftLen = smallLib::fftL(fftLen);

	const int index = freq * fftLen / srate;
	const double lowThreshold = 1e-5;

	/// set sizes
	std::valarray<std::complex<double>> spec1;
	std::valarray<std::complex<double>> spec2;

	spec1 = myLib::spectreRtoCcomplex(sig1[0], fftLen);
	spec2 = myLib::spectreRtoCcomplex(sig2[0], fftLen);

	std::valarray<std::complex<double>> res12 =
			spec1 * myLib::spectreConj(spec2);
	std::valarray<std::complex<double>> res11 =
			spec1 * myLib::spectreConj(spec1);
	std::valarray<std::complex<double>> res22 =
			spec2 * myLib::spectreConj(spec2);

	std::valarray<std::complex<double>> res1122 =
			sqrt(res11 * res22);
	std::valarray<std::complex<double>> R = res12 /
											sqrt(res11 * res22);
	std::valarray<std::complex<double>> a12, a11, a22;

	for(int i = 1; i < siz; ++i)
	{
		spec1 = myLib::spectreRtoCcomplex(sig1[i], fftLen);
		spec2 = myLib::spectreRtoCcomplex(sig2[i], fftLen);

		a12 = spec1 * myLib::spectreConj(spec2);
		a11 = spec1 * myLib::spectreConj(spec1);
		a22 = spec2 * myLib::spectreConj(spec2);

//		std::cout << i << '\t'
//				  << smallLib::doubleRound(std::abs(a12[index]), 6) << '\t'
//				  << smallLib::doubleRound(std::abs(a11[index]), 6) << '\t'
//				  << smallLib::doubleRound(std::abs(a22[index]), 6) << '\t'
//				  << std::abs((a12 / sqrt(a11 * a22))[index]) << std::endl;

		if(std::abs(a22[index]) > lowThreshold && std::abs(a11[index]) > lowThreshold)
		{
			R += a12 / sqrt(a11 * a22);
		}

		res12 += a12;
		res11 += a11;
		res22 += a22;
		res1122 += sqrt(a11 * a22);
	}
	res12 /= siz;
	res11 /= siz;
	res22 /= siz;
	res1122 /= siz;
	R /= siz;


//	return res12[index] / res1122[index];
//	return R[index];
	return res12[index] / sqrt(res11[index] * res22[index]);
}



/// iitpData class
std::complex<double> iitpData::coherency(int chan1, int chan2, double freq)
{
//	if(chan1 > chan2) std::swap(chan1, chan2);
//	std::vector<std::valarray<double>> sig1;
//	std::vector<std::valarray<double>> sig2;
//	for(const matrix & m : this->piecesData)
//	{
//		sig1.push_back(m[chan1]);
//		sig2.push_back(m[chan2]);
//	}
//	return iitp::coherency(sig1, sig2, this->srate, freq);


	for(std::pair<int, int> a : {
		std::pair<int, int>(chan1, chan1),
		std::pair<int, int>(chan2, chan2),
		std::pair<int, int>(chan1, chan2)
})
	{
		uint b = this->crossSpectra[a.first][a.second].size();
		if(b == 0)
		{
			this->crossSpectrum(a.first, a.second);
		}
	}
	if(this->coherencies[chan1][chan2].size() == 0)
	{
		this->coherencies[chan1][chan2] = this->crossSpectra[chan1][chan2] /
										  sqrt(this->crossSpectra[chan1][chan1] *
											   this->crossSpectra[chan2][chan2]);
	}
	int index = freq / this->spStep;

	return coherencies[chan1][chan2][index];

}

void iitpData::crossSpectrum(int chan1, int chan2)
{
	if(chan1 > chan2)
	{
		std::swap(chan1, chan2);
	}

	std::valarray<std::complex<double>> spec1;
	std::valarray<std::complex<double>> spec2;

	spec1 = myLib::spectreRtoCcomplex(this->piecesData[0][chan1], fftLen);
	spec2 = myLib::spectreRtoCcomplex(this->piecesData[0][chan2], fftLen);
	std::valarray<std::complex<double>> res = spec1 * myLib::spectreConj(spec2);

	for(int i = 1; i < this->piecesData.size(); ++i)
	{
		spec1 = myLib::spectreRtoCcomplex(this->piecesData[i][chan1], fftLen);
		spec2 = myLib::spectreRtoCcomplex(this->piecesData[i][chan2], fftLen);
		res += spec1 * myLib::spectreConj(spec2);
	}
	res /= this->piecesData.size();

	this->crossSpectra[chan1][chan2] = res;
}

void iitpData::cutPieces(double length)
{
	this->piecesData.clear();
	const int numPieces = this->getDataLen() / (this->srate * length);
	for(int i = 0; i < numPieces; ++i)
	{
		this->piecesData.push_back(this->edfData.subCols(length * this->srate * i,
														 length * this->srate * (i + 1)));
	}
	this->setFftLen();
	this->clearCrossSpectra();
}

void iitpData::clearCrossSpectra()
{
	this->crossSpectra.clear();
	this->crossSpectra.resize(this->ns);
	for(int i = 0; i < this->crossSpectra.size(); ++i)
	{
		this->crossSpectra[i].resize(this->ns, {});
	}

	this->coherencies.clear();
	this->coherencies.resize(this->ns);
	for(int i = 0; i < this->coherencies.size(); ++i)
	{
		this->coherencies[i].resize(this->ns, {});
	}
}

void iitpData::resizePieces(int in)
{
	for(matrix & m : this->piecesData)
	{
		m.resizeCols(in);
	}
	this->clearCrossSpectra();
}

void iitpData::getPiecesParams()
{
	std::cout << "size = " << this->piecesData.size() << std::endl;
	std::cout << "length = " << this->piecesData[0].cols() << std::endl;
}

void iitpData::setPieces(int mark1, int mark2)
{
	const int badBeg = -1;
	int beg = badBeg;
	for(int i = 0; i < this->edfData.cols(); ++i)
	{
		if(this->edfData[this->markerChannel][i] == mark1)
		{
			if(beg == badBeg)
			{
				std::cout << "iitpData::setPieces: something wrong with markers" << std::endl;
			}
			beg = i;
		}
		else if(this->edfData[this->markerChannel][i] == mark2)
		{
			if(beg == badBeg)
			{
				std::cout << "iitpData::setPieces: something wrong with markers" << std::endl;
			}
			else
			{
				piecesData.push_back(this->edfData.subCols(beg, i));
				beg = badBeg;
			}
		}
	}
	this->setFftLen();
	this->clearCrossSpectra();
}

int iitpData::setFftLen()
{
	int res = 0;
	for(const auto & in : this->piecesData)
	{
		res = std::max(smallLib::fftL(in.cols()), res);
	}
	this->fftLen = res;
	this->spStep = this->srate / double(this->fftLen);
	return res;
}

void iitpData::setFftLen(int in)
{
	this->fftLen = smallLib::fftL(in);
	this->spStep = this->srate / this->fftLen;
}


} // namespace iitp
