#include <myLib/iitp.h>
#include <myLib/iitp_consts.h>
#include <myLib/iitp_variables.h>
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
		auto part1 = smLib::contSubsec(sig1, start, start + fftLen);
		auto part2 = smLib::contSubsec(sig2, start, start + fftLen);

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

	for(uint windStart = 0;
		windStart < sig1.size() - fftLen;
		windStart += windStep)
	{
		std::valarray<double> part1 = smLib::contSubsec(sig1,
														 windStart,
														 windStart + fftLen);
		part1 *= wnd;
		std::valarray<double> part2 = smLib::contSubsec(sig2,
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
		std::cout << "iitpDa::coherency: no such channel " << std::endl;
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

		/// crutch
//		if(mscoherencies[chan1][chan2].max() > 1.0)
//		{
//			mscoherencies[chan1][chan2] = 0.;
//		}
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

	for(uint i = 1; i < this->piecesData.size(); ++i)
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


	const auto fileType = iitp::trialTypes[iitp::iitpData::getFileNum(this->fileName)];

	/// just to be sure it's empty
	this->crossSpectra[chan1][chan2].resize(fftLenW);
	this->crossSpectra[chan1][chan2] = 0.;


	const std::valarray<double> wnd = myLib::fftWindow(fftLenW, iitp::iitpWindow);

	/// periodic task
	if(fileType == iitp::trialType::real
	   ||fileType == iitp::trialType::passive )
	{

		for(uint i = 0; i < this->piecesData.size(); ++i)
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
	for(uint i = 0; i < this->piecesData.size(); ++i)
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
	for(uint i = 0; i < this->crossSpectra.size(); ++i)
	{
		this->crossSpectra[i].resize(this->ns, {});
	}

	this->coherencies.clear();
	this->coherencies.resize(this->ns);
	for(uint i = 0; i < this->coherencies.size(); ++i)
	{
		this->coherencies[i].resize(this->ns, {});
	}

	this->mscoherencies.clear();
	this->mscoherencies.resize(this->ns);
	for(uint i = 0; i < this->mscoherencies.size(); ++i)
	{
		this->mscoherencies[i].resize(this->ns, {});
	}


	this->piecesFFT.clear();
	this->piecesFFT.resize(this->piecesData.size());
	for(uint i = 0; i < this->piecesFFT.size(); ++i)
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

int iitpData::getFileNum(const QString & in)
{
	int num{-1};
	bool ok{false};
	auto lst = in.split(QRegExp(R"([\._])"), QString::SkipEmptyParts);
	for(const auto & str : lst)
	{
		num = str.toInt(&ok);
		if(ok) { break; }
	}
	if(!ok)
	{
		std::cout << "can't find state number in file " << in << std::endl;
		num = -1;
	}
	return num;
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

	/// myLib::sliceWinds
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
		for(uint j = 0; j < this->piecesData.size(); ++j)
		{
			spec += std::pow(smLib::abs(this->piecesFFT[j][i]), 2.);
		}
		spec /= this->piecesData.size();
		std::copy(std::begin(spec) + leftInd,
				  std::begin(spec) + rightInd,
				  std::begin(spectre) + i * spLen);
	}
	int num = this->getFileNum();
	QString add = iitp::trialTypesNames[trialTypes[num]];

	myLib::writeFileInLine(def::iitpResFolder
						   + "/" + this->getGuy()
						   + "/sp"
						   + "/" + this->getInit()
						   + "_" + iitp::trialTypesNames[iitp::trialTypes[this->getFileNum()]]
						   + "_sp.txt",
						   spectre);

//	QPixmap templ = myLib::drw::drawTemplate(true, leftFr, rightFr, numCh);
//	templ = myLib::drw::drawArray(templ, spectre);
//	templ.save(def::iitpResFolder + "/" + this->getGuy()
//			   + "/pic"
//			   + "/" + this->getInit() + ".jpg", 0, 100);
}


void iitpData::countContiniousTaskSpectraW(const QString & resPath, double overlap)
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
	myLib::writeFileInLine(resPath
						   + "/sp"
						   + "/" + this->getInit()
						   + "_" + iitp::trialTypesNames[iitp::trialTypes[this->getFileNum()]]
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
		for(uint j = 0; j < this->piecesData.size(); ++j)
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
		for(uint j = 0; j < this->piecesData.size(); ++j)
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


void iitpData::countFlexExtSpectraW(const QString & resPath, int mark1, int mark2, double overlap)
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
		for(uint j = 0; j < this->getPieces().size(); ++j)
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
	myLib::writeFileInLine(resPath
						   + "/sp"
						   + "/" + this->getInit() + joint + "_flexion_sp.txt",
						   spectre);

	/// extension
	this->setPiecesW(mark2, mark1, overlap);
	for(int i = 0; i < numCh; ++i)
	{
		spec = 0.;
		for(uint j = 0; j < this->getPieces().size(); ++j)
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
	myLib::writeFileInLine(resPath
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


iitpData & iitpData::staging(int numGonioChan)
{
	int minMarker = iitp::gonioMinMarker(numGonioChan);
	return this->staging(iitp::gonioNames[numGonioChan], minMarker, minMarker + 1);

}


std::vector<int> suspectGoodThreshold(const std::valarray<double> & piece, double alpha)
{
	std::vector<int> res{};

	double maxVal = *std::max_element(std::begin(piece), std::end(piece));
	double threshold = (1. - alpha) * maxVal;

	for(uint j = 0; j < piece.size() - 1; ++j)
	{
		if((piece[j] - threshold) * (piece[j + 1] - threshold) <= 0.)
		{
			res.push_back(j);
		}
	}
	return res;
}

std::vector<int> suspectGoodSecDeriv(const std::valarray<double> & piece)
{
	std::vector<std::pair<int, double>> preRes{};
	std::vector<int> res{};
	const int st = 5;
	std::valarray<double> secondDeriv = piece.cshift(2 * st) - piece * 2. + piece.cshift(-2 * st);
	secondDeriv = secondDeriv.apply(std::abs);

#if 1
	/// zero secondDeriv somewhere
	std::valarray<double> pieceAbs = piece.apply(std::abs);
	for(uint i = 0; i < pieceAbs.size(); ++i)
	{
		if(pieceAbs[i] < 0.7 * pieceAbs.max()) /// magic const
		{
			secondDeriv[i] = 0.;
		}
	}
#endif

	for(int k = 0; k < 2 * st; ++k)
	{
		secondDeriv[k] = 0.;
		secondDeriv[secondDeriv.size() - 1 - k] = 0.;
	}

	for (uint k = 2 * st; k < secondDeriv.size() - 2 * st; ++k)
	{
		bool tmp = true;
		if(secondDeriv[k] == 0.) { continue; } /// magic const
//		if(secondDeriv[k] < 5.) { continue; } /// magic const
		for(int z = 0; z < 15; ++z) /// magic const
		{
			if(secondDeriv[k] < secondDeriv[k + z] ||
			   secondDeriv[k] < secondDeriv[k - z])
			{
				tmp = false;
				break;
			}
		}
		if(tmp)
		{
			preRes.push_back({k, secondDeriv[k]});
			res.push_back(k);
		}
	}

#if 0
	/// take two with the highest values
	res.clear();
	std::sort(std::begin(preRes),
			  std::end(preRes),
			  [](const auto & in1, const auto & in2)
	{ return in1.second  > in2. second; });

	if(preRes.size() > 0) { res.push_back(preRes[0].first); }
	if(preRes.size() > 1) { res.push_back(preRes[1].first); }
#endif

	return res;
}


iitpData & iitpData::staging(const QString & chanName,
							 int markerMin,
							 int markerMax)
{
	auto chanNum = this->findChannel(chanName);
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

	int currSign = sign(chan[0]);
	int start = 0;

	std::ofstream of("/media/Files/Data/iitp/out.txt", std::ios_base::app);
	for(uint i = 0; i < chan.size() - 1; ++i) // -1 for stability
	{

		if(sign(chan[i]) != currSign)
		{
			int end = i - 1;

			if(end - start < 10)
			{
				break;
				start = i;
				currSign *= -1;
			}

			std::valarray<double> val = smLib::contSubsec(chan, start, end + 1); /// i
			val = val.apply(std::abs);

			std::vector<int> candidates{};

			/// Isakov2
			if(chanName.contains("Knee") && this->ExpName.contains("Isakov2"))
			{
				/// marker by absolute value of second derivative
				if(currSign > 0)
				{
					const double alpha = 0.05;
					candidates = suspectGoodThreshold(val, alpha);
				}
				else
				{
					candidates = suspectGoodSecDeriv(val);
				}
			}
			else
			{
				/// marker on (1. - alpha) of peak
				const double alpha = 0.20;
				candidates = suspectGoodThreshold(val, alpha);
			}

			/// adjust for zero marker places (hole they dont overlap)
			const int lefrig = 8;
			for(auto & in : candidates)
			{
				in += start;
				/// look for zero marker in the neighbourhood
				for(int k = 0; k < lefrig; ++k)
				{
					if(in + k < marks.size() && marks[in + k] == 0.)
					{
						in = in + k;
						break;
					}
					else if(in - k > 0 && marks[in - k] == 0.)
					{
						in = in - k;
						break;
					}
				}
			}

			/// mark only first and last threshold crossing. Also okay if size == 1
			if(!candidates.empty())
			{
				marks[candidates.front()] = ((currSign == 1) ? markerMax : markerMin);
				marks[candidates.back()]  = ((currSign == 1) ? markerMax : markerMin);
			}
			else
			{
				of << "iitpData::staging: candidates empty, "
				   << "file = " << this->ExpName << "   "
				   << "chan = " << chanName << "   "
				   << "start = " << start << "   "
				   << std::endl;
			}

			start = i;
			currSign *= -1;
		}
	}
	of.close();
	return *this;


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
	{-25, 25}).save("/media/Files/Data/deriv.jpg", 0, 100);

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




/// forMap
///
forMap::forMap(const iitp::iitpData::mscohsType & in,
			   const iitp::iitpData & inFile,
			   int filNum,
			   QString typ,
			   QString gon)
{
	fileNum = filNum;
	fileType = typ;
	gonio = gon;



	fmChans.clear();
	for(QString emgNam : iitp::forMapEmgNames)
	{
//			auto a = inFile.findChannel(emgNam);
//			if(a != -1)
//			{
//				fmChans.push_back(a);
//			}
		fmChans.push_back(inFile.findChannel(emgNam));
	}

	/// new 28.11.17 - EXPERIMENTAL
//		std::cout << fmChans << std::endl;
	forMapRanges.resize(iitp::forMapEmgNames.size());
	for(uint i = 0; i < forMapRanges.size(); ++i) /// i ~ EMG channel
	{
		if(isBad(i)) { continue; }

		for(int j = 0; j < forMapRanges[i].size(); ++j) /// j ~ alpha/beta/gamma
		{

			fmRange & pewpew = forMapRanges[i][j];

			pewpew.maxValZero();
			pewpew.meanValZero();

			for(int eegNum : iitp::interestEeg)
			{
				for(int FR = pewpew.lef(); FR < pewpew.rig(); ++FR)
				{
					const auto & val = in[eegNum][fmChans[i]][FR / inFile.getSpStepW()];
					if(std::abs(val) >= 1.0)
					{
						std::cout << "abs(coh) > 1"
								  << " fileNum " << fileNum
								  << " type " << typ
									 << " eeg " << inFile.getLabels(eegNum)
									 << " EMG " << iitp::forMapEmgNames[i]
									 << " freq " << FR
									 << " rhythm " << iitp::forMapRangeNames[j]
										<< std::endl;
					}
//						pewpew.meanVal[eegNum] += val;
//						pewpew.maxVal[eegNum] = std::max(pewpew.maxVal[eegNum], val);

					pewpew.setMeanVal(eegNum, val);
					pewpew.setMaxVal(eegNum,
									 std::max(pewpew.getMaxVal(eegNum), val));
				}
			}
//				pewpew.meanVal /= pewpew.rightLim - pewpew.leftLim;
			pewpew.meanValDivide(pewpew.rig() - pewpew.lef());
		}
	}

	if(0)
	{
		std::cout << fileType << std::endl;
		std::cout << std::fixed;
		std::cout.precision(3);
		for(uint i = 0; i < forMapRanges.size(); ++i) /// i ~ EMG channel
		{
			for(int j = 0; j < forMapRanges[i].size(); ++j) /// j ~ alpha/beta/gamma
			{
				const fmRange & pewpew = forMapRanges[i][j];

				std::cout << fileNum << " " << i << " " << j << " : " << std::endl;
//					std::cout << pewpew.meanVal << std::endl;
//					std::cout << pewpew.maxVal << std::endl;

				std::cout << pewpew.getMeanVal() << std::endl;
				std::cout << pewpew.getMaxVal() << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::defaultfloat;
	}

}


bool forMap::isBad(int numEmg) const
{
	/// may not have such Emg channel
	/// OR it is not interesting EMG,
	/// i.e. the signal may be bad and coherencies weren't calced
	///
//		std::cout << "isGood(" << numEmg << "):" << std::endl;
//		std::cout << "num of chan (should != -1) " << fmChans[numEmg] << std::endl;
//		std::cout << "is emg in interest chans? " << myLib::contains(iitp::interestEmg[fileNum],
//																	 myLib::indexOfVal(iitp::emgNames,
//																					   iitp::forMapEmgNames[numEmg])) << std::endl;
	return (fmChans[numEmg] == -1) ||
			(!myLib::contains(iitp::interestEmg[fileNum],
							 myLib::indexOfVal(iitp::emgNames,
											   iitp::forMapEmgNames[numEmg])));
}


} // namespace iitp
