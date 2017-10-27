#include <other/coord.h>

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

//QStringList fileMarkers;
QStringList fileMarkers{"_241", "_247", "_254 _244"}; // Me
//QStringList fileMarkers{"_241", "_242", "_243", "_244"}; // Elena
//QStringList fileMarkers{"_241", "_242", "_245", "_246"}; // Ossadtchi
//QStringList fileMarkers{"_241 _242", "_245 _246"}; // Ossadtchi
//QStringList fileMarkers{"_242", "_245"}; // Ossadtchi
}

/// begin defs Singleton
/// define static consts
const QString		defs::plainDataExtension = QString("scg");
const QString		defs::spectraDataExtension = QString("scg");
const QStringList	defs::edfFilters = QStringList{"*.edf", "*.EDF"};
const QStringList	defs::plainFilters = QStringList{"*.scg"};
const std::vector<QColor> defs::colours { QColor("blue"),
			   QColor("red"),
			   QColor("green"),
			   QColor("black"),
			   QColor("gray")};
const QString defs::XeniaFolder{"/media/Files/Data/Xenia"};
const QString defs::mriFolder{"/media/Files/Data/MRI"};
const QString defs::GalyaFolder{"/media/Files/Data/Galya"};
const QString defs::DashaFolder{"/media/Files/Data/Dasha/AUDIO"};
const QString defs::uciFolder{"/media/Files/Data/UCI"};

const QString defs::iitpFolder{"/media/Files/Data/iitp"};
const QString defs::iitpSyncFolder{"/media/Files/Data/iitp/SYNCED"};
const QString defs::iitpResFolder{"/media/Files/Data/iitp/Results"};
const QString defs::matiFolder{"/media/Files/Data/Mati"};
/// end defs Singleton

namespace suc
{
int numGoodNewLimit = 5;
int learnSetStay = 60;
double decayRate = 0.01;
double errorThreshold = 1.0;

double inertiaCoef = std::exp(-5. / 7);

double windLength = 4.;
double shiftLearn = 0.2;
double shiftTest = 0.2;
double numSmooth = 8;
}
