#ifndef COORD_H
#define COORD_H

#include <map>
#include <cmath>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <QString>
#include <QStringList>
#include <QDir>
#include <QColor>

#include <other/defaults.h> // for uint typedef and dataFolder

namespace coords
{

// relative coords
const double scale = 250./1600.;

/// + 2 EOG
const int numOfChan = 19; // for drawTemplate - move to draw namespace
// 1/32 + n * 6/32
const std::vector<double> x {0.21875, 0.59375,
                             0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                             0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                             0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                             0.21875, 0.59375,
                             0.03125, 0.78125};
// 6/32 * n
const std::vector<double> y {0.1875, 0.1875,
                             0.375, 0.375, 0.375, 0.375, 0.375,
                             0.5625, 0.5625, 0.5625, 0.5625, 0.5625,
                             0.75, 0.75, 0.75, 0.75, 0.75,
                             0.9375, 0.9375,
                             0.9375, 0.9375};
/// for BaseMonopolar rereference
const std::vector<QString> lbl_A1 {"Fp1",
                                   "F7", "F3", "Fz",
                                   "T3", "C3",
                                   "T5", "P3", "Pz",
                                   "O1",
                                   "EOG2"};
const std::vector<QString> lbl_A2 {"Fp2",
                                   "F4", "F8",
                                   "Cz", "C4", "T4",
                                   "P4", "T6",
                                   "O2",
                                   "EOG1"};
const std::vector<QString> lbl19_with_ears {"Fp1", "Fp2",
											"F7", "F3", "Fz", "F4", "F8",
											"T3", "C3", "Cz", "C4", "T4",
											"T5", "P3", "Pz", "P4", "T6",
											"O1", "O2",
											"A1", "A2"};
const std::vector<QString> lbl19 {"Fp1", "Fp2",
								  "F7", "F3", "Fz", "F4", "F8",
								  "T3", "C3", "Cz", "C4", "T4",
								  "T5", "P3", "Pz", "P4", "T6",
								  "O1", "O2"};

const std::vector<QString> lbl21 {"Fp1", "Fp2",
                                  "F7", "F3", "Fz", "F4", "F8",
                                  "T3", "C3", "Cz", "C4", "T4",
                                  "T5", "P3", "Pz", "P4", "T6",
                                  "O1", "O2",
                                  "EOG1", "EOG2"};




/// Dasha/Neurotravel
const std::vector<QString> lbl31_less {"Fp1", "27", "Fp2",
                                       "F3", "Fz", "F4", "F7", "F8",
                                       "36", "39", "35", "29", "28",
                                       "C3", "Cz", "C4", "38", "40",
                                       "37", "P3", "Pz", "P4", "34",
                                       "33", "T3", "T4", "T5", "T6",
                                       "O1", "Oz", "O2"};

const std::vector<QString> lbl31_more_withEOG {"Fp1", "Fp2",
                                       "F3", "F4", "C3", "C4", "P3", "P4",
                                       "O1", "O2", "Pg1", "Pg2",
                                       "F7", "F8", "T3", "T4", "T5", "T6",
                                       "Cb1", "Cb2",
                                       "Fz", "Cz", "Pz", "33", "Oz",
                                       "A2", "A1",
                                       "27", "28", "29",
                                       "34", "35", "36", "37", "38", "39", "40"};
const std::vector<QString> lbl31_more {"Fp1", "Fp2",
									   "F3", "F4", "C3", "C4", "P3", "P4",
									   "O1", "O2",
									   "F7", "F8", "T3", "T4", "T5", "T6",
									   "Fz", "Cz", "Pz", "33", "Oz",
									   "A2", "A1",
									   "27", "28", "29",
									   "34", "35", "36", "37", "38", "39", "40"};

const std::vector<QString> lbl31_good {"Fp1", "27", "Fp2",
									   "F7", "F3", "Fz", "F4", "F8",
									   "29", "36", "39", "35", "28",
									   "T3", "C3", "Cz", "C4", "T4",
									   "34", "38", "40", "37", "33",
									   "T5", "P3", "Pz", "P4", "T6",
									   "O1", "Oz", "O2"};
/// fMRI
const std::vector<QString> lbl32 {"Fp1", "Fp2",
                                  "F7", "F3", "Fz", "F4", "F8",
                                  "T3", "C3", "Cz", "C4",
								  "Fpz", "Ft7", "Fc3", "Fc4", "Ft8", "Tp7",
                                  "T4", "T5", "P3", "Pz", "P4", "T6",
                                  "O1", "O2",
                                  "Cp3", "Cpz", "Cp4", "Tp8", "Po7", "Po8", "Oz"
                                 };
/// most wide list of channles - should add Neurotravel?
const std::vector<QString> lbl_all {"Fp1", "Fp2",
								  "F7", "F3", "Fz", "F4", "F8",
								  "T3", "C3", "Cz", "C4",
								  "Fpz", "Ft7", "Fc3", "Fc4", "Ft8", "Tp7",
								  "T4", "T5", "P3", "Pz", "P4", "T6",
								  "O1", "O2",
								  "Cp3", "Cpz", "Cp4", "Tp8", "Po7", "Po8", "Oz",
								  "A1", "A2"
								 };
}


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

//inline int vvv = 8;

namespace def
{
	enum class filteringTyp{butterworth, fft, dsp};
	const filteringTyp filterType = filteringTyp::butterworth;

//	enum class username {MichaelA, MichaelB, Elena, Xenia, Galya, Olga, Ossadtchi, Mati, IITP};
//	extern const username currUser;

	enum class autosUser {Xenia, Galya};
	extern autosUser currAutosUser;

    const bool matiFlag = false;

	const bool withMarkersFlag = true; /// should check everywhere if changed to false
    const bool OssadtchiFlag = false;

	const bool writeLongStartEnd = true; // for slice
	const bool redirectStdOutFlag = false; // redirect std::cout to generalLog.txt

	const QString plainDataExtension = "scg";   // slice common gradient ???
    const QString spectraDataExtension = "psd"; // power spectral density
	const QStringList edfFilters = {"*.edf", "*.EDF"};
	const QStringList plainFilters = {"*.scg"};

	const std::vector<QColor> colours { QColor("blue"),
				QColor("red"),
				QColor("green"),
				QColor("black"),
				QColor("gray")
									  };
	/// all channels the same or each channel has self coefficient
	enum class spectraNormTyp {all, each};
	extern spectraNormTyp drawNormTyp;
	extern double drawNorm;

//    Atanov
#if ELENA_VARIANT
//	const QString dataFolder = "C:/Michael/Data";
#else
    const QString dataFolder = "/media/Files/Data";
//	const QString dataFolder = "D:/MichaelAtanov/workData"; // on O.D. comp
#endif
    const QString XeniaFolder = "/media/Files/Data/Xenia";
    const QString mriFolder = "/media/Files/Data/MRI";
	const QString matiFolder = "/media/Files/Data/Mati";
    const QString GalyaFolder = "/media/Files/Data/Galya";
    const QString DashaFolder = "/media/Files/Data/Dasha/AUDIO";
    const QString uciFolder = "/media/Files/Data/UCI";
	const QString iitpFolder = "/media/Files/Data/iitp";
	const QString iitpSyncFolder = "/media/Files/Data/iitp/SYNCED";
	const QString iitpResFolder = "/media/Files/Data/iitp/Results";

    const bool opencl = true;
    const bool openmp = true;




	/// to def class

	extern QDir * dir;
	inline QString dirPath()			{ return def::dir->absolutePath(); }
	inline QString realsDir()			{ return def::dirPath() + "/Reals"; }
	inline QString windsDir()			{ return def::dirPath() + "/winds"; }
	inline QString windsFromRealsDir()	{ return def::dirPath() + "/winds/fromreal"; }
	inline QString realsSpectraDir()	{ return def::dirPath() + "/SpectraSmooth"; }
	inline QString windsSpectraDir()	{ return def::dirPath() + "/SpectraSmooth/winds"; }
	inline QString pcaSpectraDir()		{ return def::dirPath() + "/SpectraSmooth/PCA"; }
	inline QString paDir()				{ return def::dirPath() + "/Help/PA"; }

	extern bool ntFlag;
	extern int ns;

	extern double freq;
	extern int fftLength;

	extern double leftFreq;
	extern double rightFreq;
	extern double inertiaCoef;

	extern QStringList fileMarkers;
	extern QString ExpName;

	inline int nsWOM() { return def::ns - 1 * def::withMarkersFlag; }
	inline int numOfClasses()
	{
//		return 6;
		return def::fileMarkers.length();
	}

	inline int left()		{ return fftLimit(def::leftFreq, def::freq, def::fftLength); }
	inline int right()		{ return fftLimit(def::rightFreq, def::freq, def::fftLength) + 1; }
	inline int spLength()	{ return def::right() - def::left(); }
	inline double spStep()	{ return def::freq / def::fftLength; }
}




class defs
{
	/// Singleton begin
public:
	static defs & inst()
	{
		static defs d{};
		return d;
	}
private:
	defs()
	{

	}
	defs(const defs &)=delete;
	defs & operator=(const defs &)=delete;
	/// Singleton end

public:
	/// enums
	enum class username {MichaelA, MichaelB, ElenaC, XeniaG, GalyaP, OlgaK, Ossadtchi, Mati, IITP};
	enum class autosUser {Xenia, Galya};
	enum class spectraNorming {all, each};

	/// consts
	static const bool matiFlag{false};			/// replace with username mati
	static const bool OssadtchiFlag{false};		/// replace with username Ossadtchi

	static const bool withMarkersFlag{true}; /// remove everywhere as true

	static const bool writeLongStartEnd{true};
	static const bool redirectStdOutFlag{false};

	static const QString plainDataExtension;
	static const QString spectraDataExtension;
	static const QStringList edfFilters;
	static const QStringList plainFilters;
	static const std::vector<QColor> colours;

	static const QString XeniaFolder;
	static const QString mriFolder;
	static const QString matiFolder;
	static const QString GalyaFolder;
	static const QString DashaFolder;
	static const QString uciFolder;


	static const QString iitpFolder;
	static const QString iitpSyncFolder;
	static const QString iitpResFolder;


public:
	QString realsDir()			{ return this->dir->absolutePath() + "/Reals"; }
	QString windsDir()			{ return this->dir->absolutePath() + "/winds"; }
	QString windsFromRealsDir()	{ return this->dir->absolutePath() + "/winds/fromreal"; }
	QString realsSpectraDir()	{ return this->dir->absolutePath() + "/SpectraSmooth"; }
	QString windsSpectraDir()	{ return this->dir->absolutePath() + "/SpectraSmooth/winds"; }
	QString pcaSpectraDir()		{ return this->dir->absolutePath() + "/SpectraSmooth/PCA"; }
	QString paDir()				{ return this->dir->absolutePath() + "/Help/PA"; }
	QString dataFolder();		/// depending on username

	/// to deprecate
	int numOfClasses()			{ return this->fileMarkers.length(); }

	int right()		{ return fftLimit(this->leftFreq, this->freq, this->fftLength); }
	int left()		{ return fftLimit(this->rightFreq, this->freq, this->fftLength) + 1; }
	int spLength()	{ return this->right() - this->left(); }
	double spStep()	{ return this->freq / this->fftLength; }

private:
	QDir * dir{new QDir(QDir::root())};

	autosUser currAutosUser{autosUser::Xenia};

	username currUser{username::MichaelA};

	spectraNorming specNormTyp{spectraNorming::all};
	double drawNorm{-1.};

	bool opencl{false};		/// unused now
	bool openmp{false};		/// unused now
	bool ntFlag{false};

	QString ExpName;		/// to deprecate
	int ns{20};				/// to deprecate
	double freq{250.};		/// to deprecate
	int fftLength{4096};	/// to deprecate

	double leftFreq{5.};
	double rightFreq{20.};

	QStringList fileMarkers{"_241", "_247", "_254 _244"};
public:
	/// gets/sets
	void setDir(const QString & in)				{ this->dir->setPath(in); }
	auto getDir()								{ return this->dir; }
	auto getDirPath()							{ return this->dir->absolutePath(); }

	void setAutosUser(autosUser in)				{ this->currAutosUser = in; }
	auto getAutosUser()							{ return this->currAutosUser; }

	void setUser(username in)					{ this->currUser = in; }
	auto getUser()								{ return this->currUser; }

	void setSpecNorm(spectraNorming in)			{ this->specNormTyp = in; }
	auto getSpecNorm()							{ return this->specNormTyp; }

	void setDrawNorm(double in)					{ this->drawNorm = in; }
	auto getDrawNorm()							{ return this->drawNorm; }

	void setExpName(const QString & in)			{ this->ExpName = in; }
	auto getExpName()							{ return this->ExpName; }

	void setNs(int in)							{ this->ns = in; }
	auto getNs()								{ return this->ns; }

	void setFreq(double in)						{ this->freq = in; }
	auto getFreq()								{ return this->freq; }

	void setFftLen(int in)						{ this->fftLength = in; }
	auto getFftLen()							{ return this->fftLength; }

	void setLeftFreq(double in)					{ this->leftFreq = in; }
	auto getLeftFreq()							{ return this->leftFreq; }

	void setRightFreq(double in)				{ this->rightFreq = in; }
	auto getRightFreq()							{ return this->rightFreq; }

	void setFileMarks(const QStringList & in)	{ this->fileMarkers = in; }
	auto getFileMarks()							{ return this->fileMarkers; }
};










namespace subjects
{
const QStringList leest_less = {
	"Berlin-Henis",
	"Bushenkov",
	"CHemerisova",
	"Didkovskaya",
	"Grishina",
	"Ivanova",
	"Krasnobaev",
	"Melnik",
	"Paramonova",
	"Ryibalko",
	"Sarvardinova",
	"SHkarina",
	"Vasina",
//    "Zelenkov"
};
const QStringList leest_more = {
	"Burmistrova",
	"Garmotko",
	"Hanenya",
	"Kalinichenko",
	"Tinyanova"
};

const QStringList leest_non = {
	"tactile",
	"Dasha_GO",
	"Dasha_GZ",
	"Dasha_smell",
	"Dasha_smell_2"
};

const QStringList leest_mri = {
	"Ali-Zade",
	"Atanov",
	"Azieva",
	"Balaev",
	"Gavrilov",
	"Gudovich",
	"Ivashkin",
	"Kabanov",
	"Khasanov",
	"Khokhlov",
	"Levando",
	"Mezhenova",
	"Moskovtsev",
	"Muchanova",
	"Nikolaenko",
	"Novoselova",
	"Shevchenko",
	"Sushinsky",
	"Sushinsky2",
	"Umanskaya",
	"Vorobiev",
	"Zavyalova"
};
}


namespace coords
{

const std::map<int, QString> kyrToLatin{
    {1072, "a"},
    {1073, "b"},
    {1074, "v"},
    {1075, "g"},
    {1076, "d"},
    {1077, "e"},
    {1078, "zh"},
    {1079, "z"},
    {1080, "i"},
    {1081, "j"},
    {1082, "k"},
    {1083, "l"},
    {1084, "m"},
    {1085, "n"},
    {1086, "o"},
    {1087, "p"},
    {1088, "r"},
    {1089, "s"},
    {1090, "t"},
    {1091, "u"},
    {1092, "f"},
    {1093, "kh"},
    {1094, "tz"},
    {1095, "ch"},
    {1096, "sh"},
    {1097, "sch"},
    {1098, ""},
    {1099, "yi"},
    {1100, ""},
    {1101, "e"},
    {1102, "yu"},
    {1103, "ya"},
    {1040, "A"},
    {1041, "B"},
    {1042, "V"},
    {1043, "G"},
    {1044, "D"},
    {1045, "E"},
    {1046, "ZH"},
    {1047, "Z"},
    {1048, "I"},
    {1049, "J"},
    {1050, "K"},
    {1051, "L"},
    {1052, "M"},
    {1053, "N"},
    {1054, "O"},
    {1055, "P"},
    {1056, "R"},
    {1057, "S"},
    {1058, "T"},
    {1059, "U"},
    {1060, "F"},
    {1061, "KH"},
    {1062, "TZ"},
    {1063, "CH"},
    {1064, "SH"},
    {1065, "SCH"},
    {1066, ""},
    {1067, "YI"},
    {1068, ""},
    {1069, "E"},
    {1070, "YU"},
    {1071, "YA"},
    {1105, "yo"},
    {1025, "YO"}
};
}



#endif

