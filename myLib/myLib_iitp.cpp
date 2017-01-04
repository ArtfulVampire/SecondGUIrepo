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

std::valarray<std::complex<double>> crossSpectrum(
		const std::vector<std::valarray<double>> & in1,
		const std::vector<std::valarray<double>> & in2)
{
	if(in1.size() != in2.size())
	{
		std::cout << "crossSpectrum: data sizes inequal" << std::endl;
		return {};
	}
	const int siz = in1.size();

	/// determine fftLength
	int fftLen = 0;
	for(int i = 0; i < siz; ++i)
	{
		fftLen = std::max(smallLib::fftL(in1[i].size()),
						  fftLen);
		fftLen = std::max(smallLib::fftL(in2[i].size()),
						  fftLen);
	}

	/// count spectra
	std::valarray<std::complex<double>> spec1 = myLib::spectreRtoCcomplex(in1[0], fftLen);
	std::valarray<std::complex<double>> spec2 = myLib::spectreConj(
													myLib::spectreRtoCcomplex(in2[0], fftLen));
	std::valarray<std::complex<double>> res = spec1 * spec2; /// to determine size
	for(int i = 1; i < in1.size(); ++i)
	{
		spec1 = myLib::spectreRtoCcomplex(in1[i], fftLen);
		spec2 = myLib::spectreConj(myLib::spectreRtoCcomplex(in2[i], fftLen));
		res += spec1 * spec2;
	}
	res /= siz;
	return res;
}


/// iitpData class

std::complex<double> iitpData::coherency(int chan1, int chan2, double freq)
{
	if(chan1 > chan2) std::swap(chan1, chan2);
	int index = freq / this->spStep;
	index = 4;

	for(std::pair<int, int> a : {
	std::pair<int, int>(chan1, chan2),
	std::pair<int, int>(chan2, chan2),
	std::pair<int, int>(chan1, chan1)
})
	{
//		std::cout << 1 << std::endl;
		uint b = this->crossSpectra[a.first][a.second].size();
		if(b == 0)
		{
//			std::cout << 2 << std::endl;
			this->crossSpectrum(a.first, a.second);
//			std::cout << 3 << std::endl;
		}
//		std::cout << this->crossSpectra[a.first][a.second][index] << std::endl;
	}
//	exit(0);

	return this->crossSpectra[chan1][chan2][index] /
			sqrt( this->crossSpectra[chan1][chan1][index] *
				  this->crossSpectra[chan2][chan2][index]);

}

void iitpData::crossSpectrum(int chan1, int chan2)
{
	if(chan1 > chan2) std::swap(chan1, chan2);
	std::valarray<std::complex<double>> spec1;
	std::valarray<std::complex<double>> spec2;

	spec1 = myLib::spectreRtoCcomplex(this->piecesData[0][chan1], fftLen);
	spec2 = myLib::spectreRtoCcomplex(this->piecesData[0][chan2], fftLen);
	std::valarray<std::complex<double>> res =
			spec1 * myLib::spectreConj(spec2);


	for(int i = 1; i < this->piecesData.size(); ++i)
	{
		spec1 = myLib::spectreRtoCcomplex(this->piecesData[i][chan1], fftLen);
		spec2 = myLib::spectreRtoCcomplex(this->piecesData[i][chan2], fftLen);
		res += spec1 * myLib::spectreConj(spec2);
	}
	res /= this->piecesData.size();

	this->crossSpectra[chan1][chan2] = res;
}

void iitpData::cutPieces(int length)
{
	this->piecesData.clear();
	const int numPieces = this->getDataLen() / (this->srate * length);
	for(int i = 0; i < numPieces; ++i)
	{
		this->piecesData.push_back(this->edfData.subCols(length * this->srate * i,
														 length * this->srate * (i + 1)));
	}
	this->setFftLen();
	this->crossSpectra.resize(this->ns);
	for(int i = 0; i < this->crossSpectra.size(); ++i)
	{
		this->crossSpectra[i].resize(this->ns, {});
	}
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
	this->crossSpectra.resize(this->ns);
	for(int i = 0; i < this->crossSpectra.size(); ++i)
	{
		this->crossSpectra[i].resize(this->ns);
	}
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
