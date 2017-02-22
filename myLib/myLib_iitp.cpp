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

void iitpData::setPieces(int startMark, int finishMark)
{
	piecesData.clear();

	const int badBeg = -1;
	int beg = badBeg;
	for(int i = 0; i < this->edfData.cols(); ++i)
	{
		if(this->edfData[this->markerChannel][i] == startMark)
		{
			beg = i;
		}
		else if(this->edfData[this->markerChannel][i] == finishMark && beg != badBeg)
		{
			piecesData.push_back(this->edfData.subCols(beg, i));
			beg = badBeg;
		}
	}
	this->clearCrossSpectra();
	this->setFftLen();
}

void iitpData::countImagPassSpectra()
{
	const int localFftLen = 1024;
	this->setFftLen(localFftLen);
	const int leftInd = iitp::leftFr / this->spStep;
	const int rightInd = iitp::rightFr / this->spStep;
	const int spLen = rightInd - leftInd;
	const int numCh = 19;

	std::valarray<double> spectre(spLen * numCh);
	std::valarray<double> spec(localFftLen);

	this->cutPieces(localFftLen / double(this->srate));
//	this->setFftLen(localFftLen);
	this->countPiecesFFT();
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->piecesData.size(); ++j)
		{
			spec += smallLib::abs(this->piecesFFT[j][i]);
		}
		spec *= (4. / localFftLen);
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
	int num = this->ExpName.split("_", QString::SkipEmptyParts)[1].toInt();
	QString add = iitp::trialTypesNames[trialTypes[num]];

	myLib::writeFileInLine(this->getDirPath()
						   + "/sp"
						   + "/" + this->getInit() + "_sp.txt",
						   spectre);

	QPixmap templ = myLib::drw::drawTemplate(true, leftFr, rightFr, numCh);
	templ = myLib::drw::drawArray(templ, spectre);
	templ.save(this->getDirPath()
			   + "/pic"
			   + "/" + this->getInit() + ".jpg", 0, 100);
}

void iitpData::countFlexExtSpectra(int mark1, int mark2)
{
	const int localFftLen = 1024;
	this->setFftLen(localFftLen);
	const int leftInd = iitp::leftFr / this->spStep;
	const int rightInd = iitp::rightFr / this->spStep;
	const int spLen = rightInd - leftInd;
	const int numCh = 19;
	QString joint = "_" + iitp::gonioNames[(mark1 - 100) / 10 - 1 + (mark1 % 10) / 2];

	QPixmap templ = myLib::drw::drawTemplate(true, leftFr, rightFr, numCh);

	std::valarray<double> spectre(spLen * numCh);
	matrix spectra{2, 1};
	std::valarray<double> spec(localFftLen);

	this->setPieces(mark1, mark2);

	this->setFftLen(localFftLen);
	this->countPiecesFFT();
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->piecesData.size(); ++j)
		{
			spec += smallLib::abs(this->piecesFFT[j][i]);
		}
		spec *= (4. / localFftLen);
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
	spectra[0] = spectre;
	myLib::writeFileInLine(this->getDirPath()
						   + "/sp"
						   + "/" + this->getInit() + joint + "_flexion_sp.txt",
						   spectre);

	this->setPieces(mark2, mark1);
	this->setFftLen(localFftLen);
	this->countPiecesFFT();
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->piecesData.size(); ++j)
		{
			spec += smallLib::abs(this->piecesFFT[j][i]);
		}
		spec *= (4. / localFftLen);
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
	spectra[1] = spectre;
	myLib::writeFileInLine(this->getDirPath() + "/" +
						   + "/sp"
						   + "/" + this->getInit() + joint + "_extension_sp.txt",
						   spectre);
	templ = myLib::drw::drawArrays(templ, spectra);
	templ.save(this->getDirPath()
			   + "/pic"
			   + "/" + this->getInit() + joint + ".jpg", 0, 100);
}

int gonioMinMarker(int numGonioChan)
{
	return 100 +
			(numGonioChan / 2 + 1) * 10 +
			(numGonioChan % 2) * 2 + 0;
}

QString getInitName(const QString & fileName)
{
	int en = fileName.indexOf('_', fileName.indexOf('_') + 1);
	return fileName.left(en);
}

QString getPostfix(const QString & fileName)
{
	int lef = fileName.indexOf('_', fileName.indexOf('_') + 1);
	int rig = fileName.indexOf('.');
	return fileName.mid(lef, rig-lef);
}


iitpData & iitpData::staging(int numGonioChan)
{
	int minMarker = iitp::gonioMinMarker(numGonioChan);
	return this->staging(iitp::gonioNames[numGonioChan], minMarker, minMarker + 1);

}

iitpData & iitpData::staging(const QString & chanName,
							 int markerMin,
							 int markerMax)
{
	int chanNum = this->findChannel(chanName);
	const std::valarray<double> & chan = this->edfData[chanNum];
	std::valarray<double> & marks = this->edfData[this->markerChannel];
	auto sign = [](double in) -> int
	{
		return (in > 0) ? 1 : -1;
	};


#if 0
	/// test first and second derivatives

	const int st = 5;
	std::valarray<double> firstDeriv = chan.cshift(-st) - chan.cshift(st);

	std::valarray<double> secondDeriv = chan.cshift(2 * st) - chan * 2. + chan.cshift(-2 * st);

	for(int k = 0; k < st + 1; ++k)
	{
		firstDeriv[k] = 0.;
		firstDeriv[firstDeriv.size() - 1 - k] = 0.;
	}
	for(int k = 0; k < 2 * st; ++k)
	{
		secondDeriv[k] = 0.;
		secondDeriv[secondDeriv.size() - 1 - k] = 0.;
	}

	myLib::histogram(firstDeriv,
					 50,
					 "/media/Files/Data/deriv.jpg",
	{-25, 25});

	firstDeriv *= 10;
	secondDeriv *= 30;

	std::copy(std::begin(firstDeriv),
			  std::end(firstDeriv),
			  std::begin(this->edfData[this->findChannel("Ankle_r")]) + start);
	std::copy(std::begin(secondDeriv),
			  std::end(secondDeriv),
			  std::begin(this->edfData[this->findChannel("Knee_l")]) + start);

	return *this;
#endif




	int currSign = sign(chan[0]);
	int start = 0;
	const double alpha = 0.20;

	for(int i = 0; i < chan.size(); ++i)
	{
		if(sign(chan[i]) != currSign)
		{
			int end = i;


			std::valarray<double> val = smallLib::valarSubsec(chan, start, end);
			val = val.apply(std::abs);

			/// marker on 80% of peak
			double maxVal = *std::max_element(std::begin(val), std::end(val));
			double threshold = (1. - alpha) * maxVal;

			srand(time(NULL));
			for(int j = 0; j < val.size(); ++j)
			{
				if((val[j] - threshold) * (val[j-1] - threshold) <= 0.)
				{
					if(currSign == 1)
					{
						marks[start + j] = markerMax;
		//				marks[start + nm + 10 - rand()%20] = markerMax;
					}
					else
					{
						marks[start + j] = markerMin;
		//				marks[start + nm + 10 - rand()%20] = markerMin;
					}
				}

			}

			start = i;
			currSign *= -1;
		}
	}
	return *this;
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
