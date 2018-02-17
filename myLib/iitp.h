#ifndef MYLIB_IITP_H
#define MYLIB_IITP_H

#include <complex>
#include <valarray>
#include <vector>
#include <cmath>

#include <other/matrix.h>
#include <other/edffile.h>
#include <other/defaults.h>
#include <myLib/signalProcessing.h>



namespace iitp
{
/// IITP
std::complex<double> gFunc(const std::complex<double> & in);
/// PHI  = PHI(w) = crossSpectrum(w).arg();


/// standalone for testing
QPixmap phaseDifferences(const std::valarray<double> & sig1,
						 const std::valarray<double> & sig2,
						 double srate,
						 double freq, int fftLen);

std::valarray<std::complex<double>> coherenciesUsual(const std::valarray<double> & sig1,
													 const std::valarray<double> & sig2, double overlapPercent,
													 double srate,
													 int fftLen);

std::complex<double> coherencyUsual(const std::valarray<double> & sig1,
									const std::valarray<double> & sig2,
									double srate,
									double freq, int fftLen);








///// max - bend (flexed)
///// min - unbend (extended)
int gonioMinMarker(int numGonioChan); /// max = min + 1
int gonioNum(int marker);
QString gonioName(int marker);

QString getGuyName(const QString & fileName);
QString getInitName(const QString & fileName);
QString getPostfix(const QString & fileName);
int getFileNum(const QString & fileName);


class iitpData : public edfFile
{
public:
	/// [chan1][chan2][freq]
	using cohsType = std::vector<std::vector<std::valarray<std::complex<double>>>>;
	/// [chan1][chan2][freq]
	using mscohsType = std::vector<std::vector<std::valarray<double>>>;
private:
	std::vector<matrix> piecesData;

	/// [pieceNumber][channel][array of complex FFT]
	std::vector<std::vector<std::valarray<std::complex<double>>>> piecesFFT;
	/// [chan1][chan2][array of complex] - average crossSpecrum over pieces
	std::vector<std::vector<std::valarray<std::complex<double>>>> crossSpectra;

	/// [chan1][chan2][freq]
	cohsType coherencies;
	mscohsType mscoherencies;

	int fftLen = -1;
	int fftLenW = 256; /// ~1 sec
	double spStep = 0.;

public:
	std::complex<double> coherency(int chan1, int chan2, double freq);

	double confidence(double level) const
	{ return 1. - std::pow(level, 1. / (piecesData.size() - 1)); }

	const cohsType & getCoherencies() const { return coherencies; }
	const mscohsType & getMSCoherencies() const { return mscoherencies; }

	void countCrossSpectrum(int chan1, int chan2);
	void countCrossSpectrumW(int chan1, int chan2, double overlap);

	iitpData & staging(int numGonioChan);
	iitpData & staging(const QString & chanName,
					   int markerMin,
					   int markerMax);

	QString getGuy() const { return iitp::getGuyName(this->ExpName); }
	QString getPost() const { return iitp::getPostfix(this->ExpName); }
	QString getInit() const { return iitp::getInitName(this->ExpName); }
	int getNum() const { return iitp::getFileNum(this->ExpName); }

	/// continious task
	void cutPieces(double length);
	void cutPiecesW(double overlap);
	void countContiniousTaskSpectra();
	void countContiniousTaskSpectraW(const QString & resPath, double overlap);

	/// periodic task
	void setPieces(int startMark = 10, int finishMark = 20);
	void setPiecesW(int startMark, int finishMark, double overlap);
	void countFlexExtSpectra(int mark1, int mark2);
	void countFlexExtSpectraW(const QString & resPath, int mark1, int mark2, double overlap);

	void countPiecesFFT();
	void resizePieces(int in);
	void getPiecesParams();
	const std::vector<matrix> & getPieces() const { return piecesData; }

	void clearCrossSpectra();
	int setFftLen(); /// determine by piecesData lengths
	void setFftLen(int in);

	int getFftLen() const { return fftLen; }
	double getSpStep() const { return spStep; }
	double getSpStepW() const { return srate / fftLenW; }

};




/// EXPERIMENTAL
class fmRange // ~fmValue
{
private:
	double leftLim{};
	double rightLim{};
	std::valarray<double> meanVal{};
	std::valarray<double> maxVal{};

public:
	fmRange(double left, double right) : leftLim(left), rightLim(right)
	{
		meanVal.resize(19); meanVal = 0.;
		maxVal.resize(19); maxVal = 0.;
	}
	~fmRange() {}

	const std::valarray<double> & getMeanVal() const{ return meanVal; }
	double getMeanVal(int i) const					{ return meanVal[i]; }
	void setMeanVal(int i, double val)				{ meanVal[i] = val; }
	void meanValZero()								{ meanVal = 0.; }
	void meanValDivide(double denom)				{ meanVal /= denom; }

	const std::valarray<double> & getMaxVal() const	{ return maxVal; }
	double getMaxVal(int i) const					{ return maxVal[i]; }
	void setMaxVal(int i, double val)				{ maxVal[i] = val; }
	void maxValZero()								{ maxVal = 0.; }

	double lef() const								{ return leftLim; }
	double rig() const								{ return rightLim; }

};



/// EXPERIMENTAL
class fmRanges
{
private:
	/// alpha beta gamma
	std::vector<fmRange> ranges{{8., 13.}, {13., 25.}, {25., 45.}};
	std::vector<fmRange>::iterator begin() { return std::begin(ranges); }
	std::vector<fmRange>::iterator end() { return std::end(ranges); }

public:
	fmRanges() {}
	~fmRanges() {}

	fmRange & operator[](int i) { return ranges[i]; } /// bad incapsulation
	const fmRange & operator[](int i) const { return ranges[i]; } /// bad incapsulation

	int size() const { return ranges.size(); }
};


struct forMap
{
	int fileNum{-1};
	QString fileType{"notype"};
	QString gonio;
	std::vector<fmRanges> forMapRanges;
	std::vector<int> fmChans;

	bool isBad(int numEmg) const;

	~forMap(){}
	forMap(){}
	forMap(const iitp::iitpData::mscohsType & in,
		   const iitp::iitpData & inFile,
		   int filNum,
		   QString typ,
		   QString gon);
};




} // namespace iitp

#endif // MYLIB_IITP_H
