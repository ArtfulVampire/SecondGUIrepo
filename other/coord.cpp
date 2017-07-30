#include <other/coord.h>

namespace def
{
/// to def class

// assigned in readData, reduce channels, etc
int ns = 20;
QString ExpName;
QDir * dir = new QDir();

bool ntFlag = false;

double drawNorm = -1.;
def::spectraGraphsNormalization drawNormTyp = def::spectraGraphsNormalization::all;

int left()  { return fftLimit(def::leftFreq, def::freq, def::fftLength); }
int right() { return fftLimit(def::rightFreq, def::freq, def::fftLength) + 1; }

double leftFreq = 5.;
double rightFreq = 20.;
double inertiaCoef = exp(-5. / 7);

double freq = 250.;
int fftLength = 4096;
// int fftLength = 1024;


#if ELENA_VARIANT
QStringList fileMarkers{"_241", "_242", "_243", "_244"}; // Elena
#else
QStringList fileMarkers{"_241", "_247", "_254 _244"}; // Me
#endif

// QStringList fileMarkers{"_241", "_242", "_245", "_246"}; // Ossadtchi
// QStringList fileMarkers{"_241 _242", "_245 _246"}; // Ossadtchi
// QStringList fileMarkers{"_242", "_245"}; // Ossadtchi
}


namespace suc
{
int numGoodNewLimit = 5;
int learnSetStay = 60;
double decayRate = 0.01;
double errorThreshold = 1.0;

double windLength = 4.;
double shiftLearn = 0.2;
double shiftTest = 0.2;
double numSmooth = 8;
}
