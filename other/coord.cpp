#include "coord.h"

namespace def
{
// assigned in readData, reduce channels, etc
int ns = 20;


bool ntFlag = false;

int left()  {return fftLimit(def::leftFreq, def::freq, def::fftLength);}
int right() {return fftLimit(def::rightFreq, def::freq, def::fftLength) + 1;}

double leftFreq = 5.;
double rightFreq = 20.;
double inertiaCoef = exp(-5. / 7);

double freq = 250.;
int fftLength = 4096;
//int fftLength = 1024;
QString cfgFileName = "tmp.net";


double drawNorm = -1.;
spectraGraphsNormalization drawNormTyp = spectraGraphsNormalization::all;


QString ExpName;
QDir * dir = new QDir();

std::vector<QColor> colours {QColor("blue"),
			QColor("red"),
			QColor("green"),
			QColor("black"),
			QColor("gray")};
QStringList fileMarkers{"_241", "_247", "_254 _244"};
//QStringList fileMarkers{"train*_241", "train*_247", "train*_254 train*_244"};
//QStringList fileMarkers{"_241", "_247"};

//QStringList fileMarkers{"_241", "_242", "_245", "_246"}; // Ossadtchi
//QStringList fileMarkers{"_241 _242", "_245 _246"}; // Ossadtchi
//QStringList fileMarkers{"_242", "_245"}; // Ossadtchi

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
