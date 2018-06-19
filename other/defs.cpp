#include <other/defs.h>
/// begin defs Singleton
/// define static consts
//const QString		def::plainDataExtension = QString("scg");
//const QString		def::spectraDataExtension = QString("psd");
//const QStringList	def::edfFilters = QStringList{"*.edf", "*.EDF"};
//const QStringList	def::plainFilters = QStringList{"*.scg"};
//const std::vector<QColor> def::colours { QColor("blue"),
//			   QColor("red"),
//			   QColor("green"),
//			   QColor("black"),
//			   QColor("gray")};
//const QString def::XeniaFolder			{"/media/Files/Data/Xenia"};
//const QString def::mriFolder			{"/media/Files/Data/MRI"};
//const QString def::GalyaFolder			{"/media/Files/Data/Galya"};
//const QString def::DashaFolder			{"/media/Files/Data/Dasha/AUDIO"};
//const QString def::uciFolder			{"/media/Files/Data/UCI"};
//const QString def::helpPath			{"/media/Files/Data/FeedbackFinalMark/Help"};

//const QString def::iitpFolder			{"/media/Files/Data/iitp"};
//const QString def::iitpSyncFolder		{"/media/Files/Data/iitp/SYNCED"};
//const QString def::iitpResFolder		{"/media/Files/Data/iitp/Results"};
//const QString def::matiFolder			{"/media/Files/Data/Mati"};

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
	case autosUser::XeniaFinalest:
	{
		autosMask = featuresMask::alpha |
					featuresMask::spectre |
					featuresMask::Hilbert |
//					featuresMask::wavelet |
					featuresMask::fracDim
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
		this->dir->cd("C:/Users/dell/Desktop/NEUROPHYSIOLOGY/02_EXPERIMENTS"); /// check
//		fileMarkers = QStringList{"_241", "_243", "_244", "_215"}; /// check
		fileMarkers = QStringList{"_241", "_242", "_244", "_210", "_212"}; /// new for tables
		break;
	}
	case username::GalyaP:
	{
		this->dir->cd(def::GalyaFolder);
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
		this->dir->cd(def::XeniaFolder);
		break;
	}
	case username::IITP:
	{
		this->leftFreq = 4.;
		this->rightFreq = 40.;
		this->dir->cd(def::iitpFolder);
		break;
	}
	case username::MichaelA:
	{
		this->dir->cd("/media/Files/Data");
		fileMarkers = QStringList{"_241", "_247", "_254"};
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
		this->dir->cd(def::matiFolder);
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

std::vector<featuresMask> defs::getAutosMaskArray() const
{
	std::vector<featuresMask> res{};
	for(int i = 0; i < 10; ++i)
	{
		int a = std::pow(2, i);
		if(a & this->autosMask) { res.push_back(featuresMask(a)); }
	}
	return res;
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
