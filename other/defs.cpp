#include <other/defs.h>
#include <other/consts.h>
/// begin defs Singleton

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
//		fileMarkers = QStringList{"_241", "_242", "_244", "_210", "_212"}; /// new for tables
		fileMarkers = QStringList{"_m_241", "_m_242", "_m_244", "_m_210", "_m_212"}; /// newest
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
		fileMarkers = QStringList{"_241", "_242", "_245", "_246"}; /// Ossadtchi
//		fileMarkers = QStringList{"_241 _242", "_245 _246"}; /// Ossadtchi
//		fileMarkers = QStringList{"_242", "_245"}; /// Ossadtchi
		this->dir->cd("/media/Files/Data/Ossadtchi");
		break;
	}
//	default: { break; /* do nothing */ } /// never get here
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
double overlap125 = (1024 - 125) / 1024.;
double numSmooth = 5;
}


