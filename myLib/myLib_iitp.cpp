#include "iitp.h"
#include <myLib/signalProcessing.h>


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

std::complex<double> iitpData::coherency(int chan1, int chan2, double freq)
{

}

std::valarray<std::complex<double>> iitpData::crossSpectrum(int chan1, int chan2)
{
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
	return res;
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
}

int iitpData::setFftLen()
{
	int res = 0;
	for(const auto & in : this->piecesData)
	{
		res = std::max(smallLib::fftL(in.cols()), res);
	}
	return res;
}

void iitpData::setFftLen(int in)
{
	this->fftLen = in;
}


} // namespace iitp
