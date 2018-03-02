#include <other/coord.h>
/*
namespace def
{
/// to def class

//const username currUser = username::MichaelA;
autosUser currAutosUser = autosUser::Xenia;

// assigned in readData, reduce channels, etc
int ns = 20;
QString ExpName;
QDir * dir = new QDir(QDir::root());

bool ntFlag = false;

double drawNorm = -1.;
def::spectraNormTyp drawNormTyp = def::spectraNormTyp::all;

double leftFreq = 5.;
double rightFreq = 20.;

double freq = 250.;
int fftLength = 4096;

QStringList fileMarkers{"_241", "_247", "_254 _244"}; // Me
//QStringList fileMarkers{"_241", "_242", "_243", "_244"}; // Elena
//QStringList fileMarkers{"_241", "_242", "_245", "_246"}; // Ossadtchi
//QStringList fileMarkers{"_241 _242", "_245 _246"}; // Ossadtchi
//QStringList fileMarkers{"_242", "_245"}; // Ossadtchi
//QStringList fileMarkers{"_261", "_262", "_263"}; // Polina
}
*/


/// begin defs Singleton
/// define static consts
const QString		defs::plainDataExtension = QString("scg");
const QString		defs::spectraDataExtension = QString("psd");
const QStringList	defs::edfFilters = QStringList{"*.edf", "*.EDF"};
const QStringList	defs::plainFilters = QStringList{"*.scg"};
const std::vector<QColor> defs::colours { QColor("blue"),
			   QColor("red"),
			   QColor("green"),
			   QColor("black"),
			   QColor("gray")};
const QString defs::XeniaFolder			{"/media/Files/Data/Xenia"};
const QString defs::mriFolder			{"/media/Files/Data/MRI"};
const QString defs::GalyaFolder			{"/media/Files/Data/Galya"};
const QString defs::DashaFolder			{"/media/Files/Data/Dasha/AUDIO"};
const QString defs::uciFolder			{"/media/Files/Data/UCI"};
const QString defs::helpPath			{"/media/Files/Data/FeedbackFinalMark/Help"};

const QString defs::iitpFolder			{"/media/Files/Data/iitp"};
const QString defs::iitpSyncFolder		{"/media/Files/Data/iitp/SYNCED"};
const QString defs::iitpResFolder		{"/media/Files/Data/iitp/Results"};
const QString defs::matiFolder			{"/media/Files/Data/Mati"};

void defs::setAutosUser(autosUser in)
{
	this->currAutosUser = in;
	switch(in)
	{
	case autosUser::Galya:
	{
		autosMask = featuresMask::alpha |
					featuresMask::spectre |
					featuresMask::Hilbert |
					featuresMask::fracDim |
					featuresMask::Hjorth |
//					featuresMask::wavelet |
					featuresMask::logFFT
			   ;
		break;
	}
	case autosUser::Xenia:
	{
		autosMask = featuresMask::alpha |
					featuresMask::spectre |
					featuresMask::Hilbert |
					featuresMask::fracDim |
					featuresMask::wavelet
					;
		break;
	}
	default: { break; }
	}
}

void defs::setUser(username in)
{
	/// write all the defaults, which may be changed
	this->leftFreq = 5.;
	this->rightFreq = 20.;
	this->ntFlag = false;

	this->currUser = in;

	switch(in)
	{
	case username::ElenaC:
	{
		this->leftFreq = 4.;
		this->rightFreq = 24.;
		this->dir->cd("C:/Michael/Data"); /// check
		fileMarkers = QStringList{"_241" "_243", "_244", "_215"}; /// check
		break;
	}
	case username::GalyaP:
	{
		this->dir->cd(defs::GalyaFolder);
		break;
	}
	case username::PolinaM:
	{
		fileMarkers = QStringList{"_261" "_262", "_263", "_264"}; /// check
		this->dir->cd("C:/PolinaData"); /// check
		break;
	}
	case username::XeniaG:
	{
		this->dir->cd(defs::XeniaFolder);
		break;
	}
	case username::IITP:
	{
		this->leftFreq = 4.;
		this->rightFreq = 40.;
		this->dir->cd(defs::iitpFolder);
		break;
	}
	case username::MichaelA:
	{
		this->dir->cd("/media/Files/Data");
		break;
	}
	case username::MichaelB:
	{
		this->dir->cd("c:/EEG"); /// check
		break;
	}
	case username::OlgaK:
	{
		this->dir->cd("D:/MichaelAtanov/Data");
		break;
	}
	case username::Mati:
	{
		this->dir->cd(defs::matiFolder);
		break;
	}
	case username::Ossadtchi:
	{
		fileMarkers = QStringList{"_241", "_242", "_245", "_246"}; // Ossadtchi
//		fileMarkers = QStringList{"_241 _242", "_245 _246"}; // Ossadtchi
//		fileMarkers = QStringList{"_242", "_245"}; // Ossadtchi
		this->dir->cd("/media/Files/Data/Ossadtchi");
		break;
	}
	default: { break; } /// never get here
	}
}
/// end defs Singleton



namespace suc
{
int numGoodNewLimit = 5;
int learnSetStay = 60;
double decayRate = 0.01;
double errorThreshold = 1.0;

double inertiaCoef = std::exp(-5. / 7);

double windLength = 4.;
double shiftLearn = 0.5;
double shiftTest = 0.5;
double numSmooth = 8;
}
