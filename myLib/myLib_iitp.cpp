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








/// iitpData class
std::complex<double> iitpData::coherency(int chan1, int chan2, double freq)
{
	if(chan1 == -1 || chan2 == -1)
	{
		std::cout << "coherency: no such channel" << std::endl;
		return {};
	}
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
			this->countCrossSpectrumW(a.first, a.second, 0.5);
		}
	}
	if(this->coherencies[chan1][chan2].size() == 0)
	{
		coherencies[chan1][chan2] = this->crossSpectra[chan1][chan2] /
									std::sqrt(this->crossSpectra[chan1][chan1] *
											  this->crossSpectra[chan2][chan2]);
		mscoherencies[chan1][chan2] = std::pow(smLib::abs(coherencies[chan1][chan2]), 2.);
	}
	int index = freq / getSpStepW();
	return coherencies[chan1][chan2][index];
}

void iitpData::countCrossSpectrum(int chan1, int chan2)
{
	if(chan1 > chan2) { std::swap(chan1, chan2); }

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

void iitpData::countCrossSpectrumW(int chan1, int chan2, double overlap)
{
	if(chan1 > chan2) { std::swap(chan1, chan2); }


	const auto fileType = iitp::trialTypes[iitp::getFileNum(myLib::getFileName(this->filePath))];

	/// just to be sure it's empty
	this->crossSpectra[chan1][chan2].resize(fftLenW);
	this->crossSpectra[chan1][chan2] = 0.;


	const std::valarray<double> wnd = myLib::fftWindow(fftLenW, iitp::iitpWindow);

	/// periodic task
	if(fileType == iitp::trialType::real
	   ||fileType == iitp::trialType::passive )
	{

		for(int i = 0; i < this->piecesData.size(); ++i)
		{
			this->crossSpectra[chan1][chan2] += myLib::spectreCross(this->piecesData[i][chan1],
																	this->piecesData[i][chan2],
																	srate,
																	wnd,
																	fftLenW);
		}
		this->crossSpectra[chan1][chan2] /= std::complex<double>(this->piecesData.size(), 0);
	}
	else /// continious task
	{
		this->crossSpectra[chan1][chan2] = myLib::spectreWelchCross(this->edfData[chan1],
																	this->edfData[chan2],
																	overlap,
																	srate,
																	iitp::iitpWindow,
																	fftLenW);
	}
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

	this->coherencies.clear();
	this->coherencies.resize(this->ns);
	for(int i = 0; i < this->coherencies.size(); ++i)
	{
		this->coherencies[i].resize(this->ns, {});
	}

	this->mscoherencies.clear();
	this->mscoherencies.resize(this->ns);
	for(int i = 0; i < this->mscoherencies.size(); ++i)
	{
		this->mscoherencies[i].resize(this->ns, {});
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

void iitpData::cutPiecesW(double overlap)
{
	this->piecesData.clear();

	const int windStep = fftLenW * (1. - overlap);
	for(int windStart = 0; windStart < this->getDataLen() - fftLenW; windStart += windStep)
	{
		this->piecesData.push_back(this->edfData.subCols(windStart,
														 windStart + fftLenW));
	}
	this->clearCrossSpectra();
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

void iitpData::setPiecesW(int startMark, int finishMark, double overlap)
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
			if(i - beg < fftLenW)
			{
				int sta = (beg + i - fftLenW) / 2;
				int fin = sta + fftLenW;
				if(sta >= 0 && fin < edfData.cols())
				{
					piecesData.push_back(this->edfData.subCols(sta, fin));
				}
			}
			else
			{
				const double windStep = fftLenW * (1. - overlap);
				for(int windStart = beg; windStart < i - fftLenW; windStart += windStep)
				{
					piecesData.push_back(this->edfData.subCols(windStart, windStart + fftLenW));
				}
			}
			beg = badBeg;
		}
	}
	this->clearCrossSpectra();
}

void iitpData::countContiniousTaskSpectra()
{
	const int localFftLen = 1024;
	this->setFftLen(localFftLen);
	const int leftInd = iitp::leftFr / this->spStep;
	const int rightInd = iitp::rightFr / this->spStep + 1;
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


void iitpData::countContiniousTaskSpectraW(double overlap)
{
	const int leftInd = iitp::leftFr / getSpStepW();
	const int rightInd = iitp::rightFr / getSpStepW() + 1;
	const int spLen = rightInd - leftInd;
	const int numCh = 19;

	std::valarray<double> spectre(spLen * numCh); spectre = 0.;
	std::valarray<double> spec;

	for(int i = 0; i < numCh; ++i)
	{
		/// could use piecesData
		spec = myLib::spectreWelchRtoR(edfData[i],
									   overlap,
									   this->srate,
									   iitp::iitpWindow,
									   fftLenW);

		if(myLib::contains(iitp::interestEeg, i))
		{
			std::copy(std::begin(spec) + leftInd,
					  std::begin(spec) + rightInd,
					  std::begin(spectre) + i * spLen);
		}
	}
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
	const int rightInd = iitp::rightFr / this->spStep + 1;
	const int spLen = rightInd - leftInd;
	const int numCh = 19;

	const QString joint = "_" + iitp::gonioName(mark1);

	std::valarray<double> spectre(spLen * numCh);
	matrix spectra(2, 1);
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
	spectra[0] = spectre;
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
	spectra[1] = spectre;
	myLib::writeFileInLine(def::iitpResFolder
						   + "/" + this->getGuy()
						   + "/sp"
						   + "/" + this->getInit() + joint + "_extension_sp.txt",
						   spectre);


	QPixmap templ = myLib::drw::drawTemplate(true, leftFr, rightFr, numCh);
	templ = myLib::drw::drawArrays(templ, spectra);
	templ.save(def::iitpResFolder + "/" + this->getGuy()
			   + "/pic"
			   + "/" + this->getInit() + joint + ".jpg", 0, 100);
}


void iitpData::countFlexExtSpectraW(int mark1, int mark2, double overlap)
{
	const int leftInd = iitp::leftFr / getSpStepW();
	const int rightInd = iitp::rightFr / getSpStepW() + 1;
	const int spLen = rightInd - leftInd;

	const int numCh = 19;

	const QString joint = "_" + iitp::gonioName(mark1);

	std::valarray<double> spectre(spLen * numCh);
	std::valarray<double> spec(fftLenW);
	matrix spectra(2, 1); /// to draw

	const std::valarray<double> wnd = myLib::fftWindow(fftLenW, iitp::iitpWindow);

	/// flexion
	this->setPiecesW(mark1, mark2, overlap);
//	std::cout << this->getFileNam() << "\t" << this->piecesData.size() << std::endl;
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->getPieces().size(); ++j)
		{
			spec += std::pow(smLib::abs(myLib::spectreRtoC2(this->piecesData[j][i] * wnd,
															this->fftLenW,
															this->srate)), 2.);
		}
		spec /= this->getPieces().size();

		if(myLib::contains(iitp::interestEeg, i))
		{
			std::copy(std::begin(spec) + leftInd,
					  std::begin(spec) + rightInd,
					  std::begin(spectre) + i * spLen);
		}
	}
	spectra[0] = spectre;
	myLib::writeFileInLine(def::iitpResFolder
						   + "/" + this->getGuy()
						   + "/sp"
						   + "/" + this->getInit() + joint + "_flexion_sp.txt",
						   spectre);

	/// extension
	this->setPiecesW(mark2, mark1, overlap);
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(int j = 0; j < this->getPieces().size(); ++j)
		{
			spec += std::pow(smLib::abs(myLib::spectreRtoC2(this->piecesData[j][i] * wnd,
															this->fftLenW,
															this->srate)), 2.);
		}
		spec /= this->getPieces().size();

		if(myLib::contains(iitp::interestEeg, i))
		{
			std::copy(std::begin(spec) + leftInd,
					  std::begin(spec) + rightInd,
					  std::begin(spectre) + i * spLen);
		}
	}
	spectra[1] = spectre;
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
	if(chanNum == -1)
	{
		std::cout << "iitpData::staging: no such channel: " << chanName << "\n"
				  << "fileName: " << this->getFileNam() << ", return *this;" << std::endl;
		return *this;
	}
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
			int end = i - 1;


			std::valarray<double> val = smLib::valarSubsec(chan, start, end);
			val = val.apply(std::abs);

			/// marker on 80% of peak
			double maxVal = *std::max_element(std::begin(val), std::end(val));
			double threshold = (1. - alpha) * maxVal;

			srand(time(NULL));
			for(int j = 0; j < val.size() - 1; ++j)
			{
				/// val[j] <= threshold <= val[j + 1]
				/// val[j] >= threshold >= val[j + 1]
				if((val[j] - threshold) * (val[j + 1] - threshold) <= 0.)
				{
					if(currSign == 1)
					{
						marks[start + j] = markerMax;
		//				marks[start + j + 10 - rand()%20] = markerMax;
					}
					else
					{
						marks[start + j] = markerMin;
		//				marks[start + j + 10 - rand()%20] = markerMin;
					}
				}

			}
			start = i;
			currSign *= -1; /// currSign = sign(chan[start]);
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
