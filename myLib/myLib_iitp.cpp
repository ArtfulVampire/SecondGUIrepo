#include <myLib/iitp.h>
#include <typeinfo>

#include <myLib/signalProcessing.h>
#include <myLib/output.h>
#include <myLib/drw.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

namespace iitp
{
std::complex<double> gFunc(const std::complex<double> & in)
{

	return (1 - pow(std::abs(in), 2)) * pow(atanh(std::abs(in)), 2) / std::norm(in);
}

QPixmap phaseDifferences(const std::valarray<double> & sig1,
						 const std::valarray<double> & sig2,
						 double srate,
						 double freq,
						 int fftLen)
{
	if(sig1.size() != sig2.size())
	{
		return {};
	}
	int siz = sig1.size();

	const int index = freq * fftLen / srate;

	const int picSize = 400;
	QPixmap pic(picSize, picSize);
	pic.fill();
	QPainter pnt;
	pnt.begin(&pic);

	pnt.drawArc(pic.rect(), 0, 360 * 16); /// a circle

	std::vector<double> phis;
	phis.reserve(300);

	for(int start = 0; start < siz - fftLen; start += fftLen)
	{
		auto part1 = smLib::valarSubsec(sig1, start, start + fftLen);
		auto part2 = smLib::valarSubsec(sig2, start, start + fftLen);

		auto spec1 = myLib::spectreRtoC2(part1, fftLen, srate);
		auto spec2 = myLib::spectreRtoC2(part2, fftLen, srate);
		auto res12 = spec1 * spec2.apply(std::conj);

		auto phi = std::arg(res12[index]);
		phis.push_back(phi);
		pnt.drawLine(picSize / 2, picSize / 2,
					 picSize / 2 * (1. + cos(phi)), picSize / 2 * (1. - sin(phi)));

	}

	return pic;
}

std::valarray<std::complex<double>> coherenciesUsual(const std::valarray<double> & sig1,
													 const std::valarray<double> & sig2,
													 double overlapRatio,
													 double srate,
													 int fftLen)
{
	if(sig1.size() != sig2.size())
	{
		return {};
	}


	using specType = std::valarray<std::complex<double>>;
//	specType res{};
	specType av11{};
	specType av12{};
	specType av22{};

	const int windStep = fftLen * (1. - overlapRatio);
	const std::valarray<double> wnd = myLib::fftWindow(fftLen, myLib::windowName::Hamming);

	for(int windStart = 0;
		windStart < sig1.size() - fftLen;
		windStart += windStep)
	{
		std::valarray<double> part1 = smLib::valarSubsec(sig1,
														 windStart,
														 windStart + fftLen);
		part1 *= wnd;
		std::valarray<double> part2 = smLib::valarSubsec(sig2,
														 windStart,
														 windStart + fftLen);
		part2 *= wnd;

		const specType spec1 = myLib::spectreRtoC2(part1, fftLen, srate);
		const specType spec2 = myLib::spectreRtoC2(part2, fftLen, srate);

		specType res12 = spec1 * spec2.apply(std::conj);

		specType res11 = spec1 * spec1.apply(std::conj);
		specType res22 = spec2 * spec2.apply(std::conj);

		if(av11.size() != res11.size())
		{
			av11.resize(res11.size());
			av12.resize(res11.size());
			av22.resize(res11.size());
		}

		av11 += res11;
		av12 += res12;
		av22 += res22;
	}

	return std::pow(std::abs(av12) / std::sqrt(av11 * av22), 2);
}

std::complex<double> coherencyUsual(const std::valarray<double> & sig1,
									const std::valarray<double> & sig2,
									double srate,
									double freq, int fftLen)
{
	if(sig1.size() != sig2.size())
	{
		return {};
	}

	const int index = freq * fftLen / srate;
	return coherenciesUsual(sig1, sig2, 0., srate, fftLen)[index];
}

std::valarray<std::complex<double>> coherenciesMine(const std::valarray<double> & sig1,
													 const std::valarray<double> & sig2,
													 double srate,
													 int fftLen)
{
	if(sig1.size() != sig2.size())
	{
		return {};
	}
	int siz = sig1.size();

	using specType = std::valarray<std::complex<double>>;

	specType nom{};
	specType den{};

	const auto wnd = myLib::fftWindow(fftLen, myLib::windowName::Hamming);

	const int windNum = std::floor(sig1.size() / fftLen);
	for(int windCounter = 0; windCounter < windNum; ++windCounter)
	{
		decltype(sig1) part1 = smLib::valarSubsec(sig1,
												  windCounter * fftLen,
												  (windCounter + 1) * fftLen)
							   * wnd
							   ;
		decltype(sig2) part2 = smLib::valarSubsec(sig2,
												  windCounter * fftLen,
												  (windCounter + 1) * fftLen)
							   * wnd
							   ;

		const specType spec1 = myLib::spectreRtoC2(part1, fftLen, srate);
		const specType spec2 = myLib::spectreRtoC2(part2, fftLen, srate);

		specType res11 = spec1 * spec1.apply(std::conj);
		specType res12 = spec1 * spec2.apply(std::conj);
		specType res22 = spec2 * spec2.apply(std::conj);


		if(nom.size() != res12.size())
		{
			nom.resize(res12.size());
			den.resize(res12.size());
		}

		nom += res12;
		den += sqrt(res11 * res22);
	}
	return nom / den;
}

std::complex<double> coherencyMine(const std::valarray<double> & sig1,
								   const std::valarray<double> & sig2,
								   double srate,
								   double freq, int fftLen)
{
	if(sig1.size() != sig2.size())
	{
		return {};
	}
	const int index = freq * fftLen / srate;
	return coherenciesMine(sig1, sig2, srate, fftLen)[index];
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
	fftLen = smLib::fftL(fftLen);

	const int index = freq * fftLen / srate;
	const double lowThreshold = 1e-5;

	/// set sizes
	std::valarray<std::complex<double>> spec1;
	std::valarray<std::complex<double>> spec2;

	spec1 = myLib::spectreRtoC2(sig1[0], fftLen, srate);
	spec2 = myLib::spectreRtoC2(sig2[0], fftLen, srate);

	std::valarray<std::complex<double>> res12 =
			spec1 * spec2.apply(std::conj);
	std::valarray<std::complex<double>> res11 =
			spec1 * spec1.apply(std::conj);
	std::valarray<std::complex<double>> res22 =
			spec2 * spec2.apply(std::conj);

	std::valarray<std::complex<double>> res1122 =
			sqrt(res11 * res22);
	std::valarray<std::complex<double>> R = res12 /
											res1122;
	std::valarray<std::complex<double>> a12, a11, a22;

	for(int i = 1; i < siz; ++i)
	{
		spec1 = myLib::spectreRtoC2(sig1[i], fftLen, srate);
		spec2 = myLib::spectreRtoC2(sig2[i], fftLen, srate);

		a12 = spec1 * spec2.apply(std::conj);
		a11 = spec1 * spec1.apply(std::conj);
		a22 = spec2 * spec2.apply(std::conj);

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
std::complex<double> iitpData::coherencyUsual(int chan1, int chan2, double freq)
{
	/// usual
	if(chan1 > chan2)
	{
		std::swap(chan1, chan2);
	}

	for(std::pair<int, int> a : {
		std::pair<int, int>(chan1, chan1),
		std::pair<int, int>(chan2, chan2),
		std::pair<int, int>(chan1, chan2)
})
	{
		if(this->crossSpectra[a.first][a.second].size() == 0)
		{
			this->countCrossSpectrum(a.first, a.second);
		}
	}	
	if(this->coherenciesUsual[chan1][chan2].size() == 0)
	{
		/// WHAT THE FUCK ???
//		this->coherenciesUsual[chan1][chan2] = this->crossSpectra[chan1][chan2] /
//										  sqrt(this->crossSpectra[chan1][chan1] *
//											   this->crossSpectra[chan2][chan2]);


		/// replace 19.09.2017 - MEAN OF COMPLEX UNITS
		coherenciesUsual[chan1][chan2].resize(this->piecesFFT[0][0].size());

		for(int i = 0; i < this->piecesData.size(); ++i)
		{
			coherenciesUsual[chan1][chan2] += this->piecesFFT[i][chan1] *
											  this->piecesFFT[i][chan2].apply(std::conj) /
											  (this->piecesFFT[i][chan1].apply(smLib::abs) *
											   this->piecesFFT[i][chan2].apply(smLib::abs));
		}
		coherenciesUsual[chan1][chan2] /= this->piecesData.size();

	}
	int index = freq / this->spStep;
	return coherenciesUsual[chan1][chan2][index];
}

std::complex<double> iitpData::coherencyMine(int chan1, int chan2, double freq)
{
	/// mine
	if(chan1 > chan2)
	{
		std::swap(chan1, chan2);
	}

	if(coherenciesMine[chan1][chan2].size() == 0)
	{
		coherenciesMine[chan1][chan2].resize(this->piecesFFT[0][0].size());


		/// WHAT THE FUCK ??? - MEAN OF COMPLEX UNITS
//		for(int i = 0; i < this->piecesData.size(); ++i)
//		{
//			coherenciesMine[chan1][chan2] += this->piecesFFT[i][chan1] *
//										  this->piecesFFT[i][chan2].apply(std::conj) /
//										  (this->piecesFFT[i][chan1].apply(smLib::abs) *
//										   this->piecesFFT[i][chan2].apply(smLib::abs));
//		}
//		coherenciesMine[chan1][chan2] /= this->piecesData.size();


		/// replace 19.09.2017 - SUM OF A_j*exp(i * phi_j) / SUM OF A_j
		std::valarray<std::complex<double>> nominator(this->piecesFFT[0][0].size());
		std::valarray<std::complex<double>> denominator(this->piecesFFT[0][0].size());

		for(int i = 0; i < this->piecesData.size(); ++i)
		{
			nominator += this->piecesFFT[i][chan1] *
						 this->piecesFFT[i][chan2].apply(std::conj);
			denominator += this->piecesFFT[i][chan1].apply(smLib::abs) *
						   this->piecesFFT[i][chan2].apply(smLib::abs);
		}
		coherenciesMine[chan1][chan2] = nominator / denominator;

	}
	int index = freq / this->spStep;

	return coherenciesMine[chan1][chan2][index];
}

const iitpData::cohsType & iitpData::getCoherencies() const
{
#if !COHERENCY_TYPE
	return coherenciesUsual;
#else
	return coherenciesMine;
#endif
}

std::complex<double> iitpData::coherency(int chan1, int chan2, double freq)
{
#if !COHERENCY_TYPE
	return coherencyUsual(chan1, chan2, freq);
#else
	return coherencyMine(chan1, chan2, freq);
#endif

}


void iitpData::countCrossSpectrum(int chan1, int chan2)
{
	if(chan1 > chan2)
	{
		std::swap(chan1, chan2);
	}

	std::valarray<std::complex<double>> spec1;
	std::valarray<std::complex<double>> spec2;

	spec1 = myLib::spectreRtoC2(this->piecesData[0][chan1], fftLen, this->srate);
	spec2 = myLib::spectreRtoC2(this->piecesData[0][chan2], fftLen, this->srate);
	std::valarray<std::complex<double>> res = spec1 * spec2.apply(std::conj);

	for(int i = 1; i < this->piecesData.size(); ++i)
	{
		spec1 = myLib::spectreRtoC2(this->piecesData[i][chan1], fftLen, this->srate);
		spec2 = myLib::spectreRtoC2(this->piecesData[i][chan2], fftLen, this->srate);
		res += spec1 * spec2.apply(std::conj);
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
			this->piecesFFT[i][j] = myLib::spectreRtoC2(this->piecesData[i][j],
														this->fftLen,
														this->srate);
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

	this->coherenciesUsual.clear();
	this->coherenciesUsual.resize(this->ns);
	for(int i = 0; i < this->coherenciesUsual.size(); ++i)
	{
		this->coherenciesUsual[i].resize(this->ns, {});
	}

	this->coherenciesMine.clear();
	this->coherenciesMine.resize(this->ns);
	for(int i = 0; i < this->coherenciesMine.size(); ++i)
	{
		this->coherenciesMine[i].resize(this->ns, {});
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

	double rat = 1.;
	const int numPieces = this->getDataLen() / (this->srate * length);
	for(int i = 0; i < numPieces * rat; ++i)
	{
		int start = length * this->srate * i / rat;
		this->piecesData.push_back(this->edfData.subCols(start,
														 start + length * this->srate));
	}
	this->setFftLen();
	this->clearCrossSpectra();

	/// experimental for coherencyMine
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
			spec += std::pow(smLib::abs(this->piecesFFT[j][i]), 2.);
		}
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
	int num = this->getNum();
	QString add = iitp::trialTypesNames[trialTypes[num]];

	myLib::writeFileInLine(def::iitpResFolder
						   + "/" + this->getGuy()
						   + "/sp"
						   + "/" + this->getInit()
						   + "_" + iitp::trialTypesNames[iitp::trialTypes[this->getNum()]]
						   + "_sp.txt",
						   spectre);

//	QPixmap templ = myLib::drw::drawTemplate(true, leftFr, rightFr, numCh);
//	templ = myLib::drw::drawArray(templ, spectre);
//	templ.save(def::iitpResFolder + "/" + this->getGuy()
//			   + "/pic"
//			   + "/" + this->getInit() + ".jpg", 0, 100);
}

void iitpData::countFlexExtSpectra(int mark1, int mark2)
{
	const int localFftLen = 1024;
	this->setFftLen(localFftLen);
	const int leftInd = iitp::leftFr / this->spStep;
	const int rightInd = iitp::rightFr / this->spStep;
	const int spLen = rightInd - leftInd;
	const int numCh = 19;

	const QString joint = "_" + iitp::gonioName(mark1);

	std::valarray<double> spectre(spLen * numCh);
//	matrix spectra(2, 1);
	std::valarray<double> spec(localFftLen);

	/// flexion
	this->setPieces(mark1, mark2);
	this->setFftLen(localFftLen);
	this->countPiecesFFT();
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->piecesData.size(); ++j)
		{
			spec += std::pow(smLib::abs(this->piecesFFT[j][i]), 2.);
		}
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
//	spectra[0] = spectre;
	myLib::writeFileInLine(def::iitpResFolder
						   + "/" + this->getGuy()
						   + "/sp"
						   + "/" + this->getInit() + joint + "_flexion_sp.txt",
						   spectre);

	/// extension
	this->setPieces(mark2, mark1);
	this->setFftLen(localFftLen);
	this->countPiecesFFT();
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->piecesData.size(); ++j)
		{
			spec += std::pow(smLib::abs(this->piecesFFT[j][i]), 2.);
		}
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
//	spectra[1] = spectre;
	myLib::writeFileInLine(def::iitpResFolder
						   + "/" + this->getGuy()
						   + "/sp"
						   + "/" + this->getInit() + joint + "_extension_sp.txt",
						   spectre);


//	QPixmap templ = myLib::drw::drawTemplate(true, leftFr, rightFr, numCh);
//	templ = myLib::drw::drawArrays(templ, spectra);
//	templ.save(def::iitpResFolder + "/" + this->getGuy()
//			   + "/pic"
//			   + "/" + this->getInit() + joint + ".jpg", 0, 100);
}

int gonioMinMarker(int numGonioChan)
{
	return 110 +
			(numGonioChan / 2) * 10 +
			(numGonioChan % 2) * 2;
}

int gonioNum(int marker)
{
	return (marker - 110) / 10 * 2 + (marker % 10) / 2;
}

QString gonioName(int marker)
{
	return iitp::gonioNames[iitp::gonioNum(marker)];
}


QString getGuyName(const QString & fileName)
{
	return fileName.left(fileName.indexOf('_'));
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

int getFileNum(const QString & fileName)
{
	return fileName.split("_", QString::SkipEmptyParts)[1].toInt();
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


			std::valarray<double> val = smLib::valarSubsec(chan, start, end);
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
//	std::cout << this->ExpName << std::endl;
	for(const auto & in : this->piecesData)
	{
		res = std::max(smLib::fftL(in.cols()), res);
//		std::cout << res << "\t";
	}
//	std::cout << std::endl;
	this->fftLen = res;
	this->spStep = this->srate / double(this->fftLen);
	return res;
}

void iitpData::setFftLen(int in)
{
	this->fftLen = smLib::fftL(in);
	this->spStep = this->srate / double(this->fftLen);
}


} // namespace iitp
