#include "iitp.h"
#include <myLib/signalProcessing.h>
#include <myLib/output.h>
#include <myLib/drw.h>

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
	/// plain
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
		if(this->crossSpectra[a.first][a.second].size() == 0)
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

std::complex<double> iitpData::coherencyR(int chan1, int chan2, double freq)
{
	if(chan1 > chan2)
	{
		std::swap(chan1, chan2);
	}
	if(coherenciesR[chan1][chan2].size() == 0)
	{
		coherenciesR[chan1][chan2].resize(this->piecesFFT[0][0].size());

		for(int i = 0; i < this->piecesData.size(); ++i)
		{
#if 01
			coherenciesR[chan1][chan2] += this->piecesFFT[i][chan1] *
										  this->piecesFFT[i][chan2].apply(std::conj) /
										  (this->piecesFFT[i][chan1].apply(smallLib::abs) *
										   this->piecesFFT[i][chan2].apply(smallLib::abs));
#else
			/// OR
			coherenciesR[chan1][chan2] += this->piecesFFT[i][chan1] *
										  this->piecesFFT[i][chan2].apply(std::conj) /
										  (smallLib::abs(this->piecesFFT[i][chan1]) *
										   smallLib::abs(this->piecesFFT[i][chan2]));
#endif
		}
		coherenciesR[chan1][chan2] /= this->piecesData.size();
	}
	int index = freq / this->spStep;

	return coherenciesR[chan1][chan2][index];
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

void iitpData::countPiecesFFT()
{
	for(int i = 0; i < this->piecesData.size(); ++i)
	{
		for(int j = 0; j < this->ns; ++j)
		{
			this->piecesFFT[i][j] = myLib::spectreRtoCcomplex(this->piecesData[i][j],
															  this->fftLen);
		}
	}
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

	this->coherenciesR.clear();
	this->coherenciesR.resize(this->ns);
	for(int i = 0; i < this->coherenciesR.size(); ++i)
	{
		this->coherenciesR[i].resize(this->ns, {});
	}

	this->piecesFFT.clear();
	this->piecesFFT.resize(this->piecesData.size());
	for(int i = 0; i < this->piecesFFT.size(); ++i)
	{
		this->piecesFFT[i].resize(this->ns, {});
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

	/// experimental for coherencyR
	this->countPiecesFFT();
}

void iitpData::setPieces(int start, int finish)
{
	piecesData.clear();

	const int badBeg = -1;
	int beg = badBeg;
	for(int i = 0; i < this->edfData.cols(); ++i)
	{
		if(this->edfData[this->markerChannel][i] == start)
		{
			beg = i;
		}
		else if(this->edfData[this->markerChannel][i] == finish)
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
	this->clearCrossSpectra();
	this->setFftLen();
}

QPixmap iitpData::drawSpectra(int mark1, int mark2)
{
	const double leftFr = 2;
	const double rightFr = 40;
	const int leftInd = leftFr / this->spStep;
	const int rightInd = rightFr / this->spStep;
	const int spLen = rightInd - leftInd;
	const int localFftLen = 1024;
	const int numCh = 19;

	QPixmap templ = myLib::drw::drawTemplate(true, leftFr, rightFr, numCh);

	std::valarray<double> spectre(spLen * numCh);
	matrix spectra(2);
	std::valarray<double> spec(localFftLen);

	this->setPieces(mark1, mark2);
	this->setFftLen(localFftLen);
	this->countPiecesFFT();
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->piecesData.size(); ++j)
		{
			spec += smallLib::abs(this->piecesFFT[j][i]) *
					myLib::spectreNorm(localFftLen,
									   this->piecesData[j].cols(),
									   this->srate);
		}
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
	spectra[0] = spectre;
//	templ = myLib::drw::drawArray(templ, spectre, QColor("blue"));
//	return templ;

	this->setPieces(mark2, mark1);
	this->setFftLen(localFftLen);
	this->countPiecesFFT();
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->piecesData.size(); ++j)
		{
			spec += smallLib::abs(this->piecesFFT[j][i]) *
					myLib::spectreNorm(localFftLen,
									   this->piecesData[j].cols(),
									   this->srate);
		}
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
	spectra[1] = spectre;
//	templ = myLib::drw::drawArray(templ, spectre, QColor("red"));
	templ = myLib::drw::drawArrays(templ, spectra);
	return templ;
}


iitpData & iitpData::staging(const QString & chanName,
							 int markerMax,
							 int markerMin)
{
	int chanNum = this->findChannel(chanName);
	const std::valarray<double> & chan = this->edfData[chanNum];
	std::valarray<double> & marks = this->edfData[this->markerChannel];
	auto sign = [](double in) -> int
	{
		return (in > 0) ? 1 : -1;
	};
	int currSign = sign(chan[0]);
	int start = 0;

	for(int i = 0; i < chan.size(); ++i)
	{
		if(sign(chan[i]) != currSign)
		{
			int end = i - 1;

//			std::cout << start << '\t' << end << std::endl;

			auto val = std::valarray<double>(chan[std::slice(start, end - start, 1)]);
			val = val.apply(std::abs);
			uint nm = myLib::indexOfMax(val);
			if(currSign == 1)
			{
				marks[start + nm] = markerMax;
			}
			else
			{
				marks[start + nm] = markerMin;
			}

			start = i;
			currSign *= -1;
		}
	}
	return *this;


#if 0
	/// via derivative
	std::valarray<double> deriv = std::valarray<double>(chan).cshift(1) - chan;

	int winLen = 20;
	std::valarray<double> mean(deriv.size() - winLen);
	for(int i = winLen/2; i < deriv.size() - winLen/2; ++i)
	{
		std::valarray<double> win = deriv[std::slice(i - winLen/2, winLen, 1)];
		mean[i - winLen/2] = smallLib::mean(win);
	}

	for(int i = 0; i < mean.size() - 1; ++i)
	{
		if(mean[i] >= 0. && mean[i + 1] < 0.)
		{
			this->edfData[this->markerChannel][i + winLen/2] = markerMax;
		}
		else if(mean[i] <= 0. && mean[i + 1] > 0.)
		{
			this->edfData[this->markerChannel][i + winLen/2] = markerMin;
		}
	}
	return *this;
#endif
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
	this->spStep = this->srate / double(this->fftLen);
}


} // namespace iitp
