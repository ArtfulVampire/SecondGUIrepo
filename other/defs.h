#ifndef COORD_H
#define COORD_H


#ifdef _OPENMP
#include <omp.h>
#endif

#include <other/consts.h> // for uint and dataFolder

#include <cmath>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QColor>

#include <other/edffile.h>



namespace suc
{
extern int numGoodNewLimit;
extern int learnSetStay;
extern double decayRate;
extern double errorThreshold;

extern double inertiaCoeff;

/// new successive by edf files
extern double windLength;
extern double shiftLearn;
extern double shiftTest;
extern double numSmooth;
}


inline int fftLimit(double inFreq,
					double sampleFreq,
					int fftL)
{
	return std::ceil(inFreq / sampleFreq * fftL - 0.5);
}


enum class username {MichaelA,
					 MichaelB,
					 ElenaC,
					 XeniaG,
					 GalyaP,
					 OlgaK,
					 Ossadtchi,
					 Mati,
					 IITP,
					 PolinaM};
enum class autosUser {Xenia, Galya, XeniaFinalest};
enum featuresMask {
	spectre	= 0x01,	/// std::pow(2, 0);
	alpha	= 0x02,	/// std::pow(2, 1);
	fracDim	= 0x04,	/// std::pow(2, 2);
	Hilbert	= 0x08,	/// std::pow(2, 3);
	wavelet	= 0x10,	/// std::pow(2, 4);
	Hjorth	= 0x20,	/// std::pow(2, 5);
	logFFT	= 0x40	/// std::pow(2, 6);
};
enum class spectraNorming {all, each};

class defs
{
	/// Singleton begin
public:
	static defs & inst()
	{
		static defs d{};
		return d;
	}
	static std::ofstream & nullStream()
	{
		static std::ofstream str("/dev/null");
		return str;
	}
private:
	defs()
	{

	}
	defs(const defs &)=delete;
	defs & operator=(const defs &)=delete;
	/// Singleton end

//public:
//	/// consts
//	static const bool withMarkersFlag{true};	/// remove everywhere as true

//	static const bool writeLongStartEnd{true};	/// don't know, do I need it?
//	static const bool redirectStdOutFlag{false};/// check how to operator<< to NotificationArea

//	static const QString plainDataExtension;
//	static const QString spectraDataExtension;
//	static const QStringList edfFilters;
//	static const QStringList plainFilters;
//	static const std::vector<QColor> colours;

//	static const QString XeniaFolder;
//	static const QString mriFolder;
//	static const QString matiFolder;
//	static const QString GalyaFolder;
//	static const QString DashaFolder;
//	static const QString uciFolder;
//	static const QString helpPath;


//	static const QString iitpFolder;
//	static const QString iitpSyncFolder;
//	static const QString iitpResFolder;


public:
	QString dirPath() const				{ return this->dir->absolutePath(); }
	QString realsDir() const			{ return this->dirPath() + "/Reals"; }
	QString windsDir() const			{ return this->dirPath() + "/winds"; }
	QString windsFromRealsDir() const	{ return this->dirPath() + "/winds/fromreal"; }
	QString realsSpectraDir() const		{ return this->dirPath() + "/SpectraSmooth"; }
	QString windsSpectraDir() const		{ return this->dirPath() + "/SpectraSmooth/winds"; }
	QString pcaSpectraDir() const		{ return this->dirPath() + "/SpectraSmooth/PCA"; }
	QString paDir() const				{ return this->dirPath() + "/Help/PA"; }


	/// legacy
	int left() const		{ return fftLimit(this->leftFreq, this->freq, this->fftLength); }
	int right() const		{ return fftLimit(this->rightFreq, this->freq, this->fftLength) + 1; }
	int spLength() const	{ return this->right() - this->left(); }
	double spStep() const	{ return this->freq / this->fftLength; }
	/// to deprecate
	int numOfClasses() const{ return this->fileMarkers.length(); }
	int nsWOM() const		{ return this->ns - 1; }

private:
	QDir * dir{new QDir(QDir::root())};
	edfFile globalEDF;


	autosUser currAutosUser{autosUser::Xenia};
	username currUser{username::MichaelA};
	int autosMask{0};

	spectraNorming specNormTyp{spectraNorming::all};
	double drawNorm{-1.};

	bool ntFlag{false};

	bool opencl{false};		/// unused now
	bool openmp{false};		/// unused now

	QString ExpName;		/// to deprecate, edfFile.getExpName()
	int ns{20};				/// to deprecate, edfFile.getNs();
	double freq{250.};		/// to deprecate, edfFile.getFreq()
	int fftLength{4096};

	double leftFreq{5.};
	double rightFreq{20.};

	QStringList fileMarkers{"_241", "_247", "_254 _244"};
public:
	/// dir operations
	bool dirCdUp()								{ return this->dir->cdUp(); }
	bool dirIsRoot() const						{ return this->dir->isRoot(); }
	bool dirMkdir(const QString & in) const		{ return this->dir->mkdir(in); }
	QStringList dirEntryList(const QStringList & nameFilters,
							 QDir::Filters filter = QDir::NoFilter,
							 QDir::SortFlags sort = QDir::NoSort) const
	{ return this->dir->entryList(nameFilters, filter, sort); }
	QStringList dirEntryList(QDir::Filters filter = QDir::NoFilter,
							 QDir::SortFlags sort = QDir::NoSort) const
	{ return this->dir->entryList(filter, sort); }


public:
	/// HOLY INCAPSULATION
	edfFile & getEdfRef()						{ return this->globalEDF; }

	/// gets/sets
	void setDir(const QString & in)				{ this->dir->setPath(in); }
	auto getDir() const							{ return this->dir; }
	auto getDirPath() const						{ return this->dir->absolutePath(); }

	void setAutosUser(autosUser in);			/// set some parameters
	auto getAutosUser() const					{ return this->currAutosUser; }

	bool isUser(username in) const				{ return this->currUser == in; }
	void setUser(username in);					/// sets many parameters
	auto getUser() const						{ return this->currUser; }

	void setSpecNorm(spectraNorming in)			{ this->specNormTyp = in; }
	auto getSpecNorm() const					{ return this->specNormTyp; }

	void setDrawNorm(double in)					{ this->drawNorm = in; }
	auto getDrawNorm() const					{ return this->drawNorm; }

	/// to deprecate, globalEdf.getExpName()
	void setExpName(const QString & in)			{ this->ExpName = in; }
	auto getExpName() const						{ return this->ExpName; }

	/// to deprecate, globalEdf.getExpName()
	void setNs(int in)							{ this->ns = in; }
	auto getNs() const							{ return this->ns; }

	/// to deprecate, globalEdf.getExpName()
	void setFreq(double in)						{ this->freq = in; }
	auto getFreq() const						{ return this->freq; }

	void setFftLen(int in)						{ this->fftLength = in; }
	auto getFftLen() const						{ return this->fftLength; }

	void setLeftFreq(double in)					{ this->leftFreq = in; }
	auto getLeftFreq() const					{ return this->leftFreq; }

	void setAutosMask(int in)					{ this->autosMask = in; }
	auto getAutosMask() const					{ return this->autosMask; }
	std::vector<featuresMask> getAutosMaskArray() const;

	void setRightFreq(double in)				{ this->rightFreq = in; }
	auto getRightFreq() const					{ return this->rightFreq; }

	void setFreqs(double lef, double rig)		{ this->leftFreq = lef; this->rightFreq = rig; }
	auto getFreqs() const						{ return std::make_pair(this->leftFreq,
																		this->rightFreq); }

	void setFileMarks(const QStringList & in)	{ this->fileMarkers = in; }
	auto getFileMarks() const					{ return this->fileMarkers; }

	void setNtFlag(bool in)						{ this->ntFlag = in; }
	auto getNtFlag() const						{ return this->ntFlag; }

};

#define DEFS defs::inst()
#define globalEdf defs::inst().getEdfRef()
#define DEVNULL defs::nullStream()


#endif /// COORD_H

