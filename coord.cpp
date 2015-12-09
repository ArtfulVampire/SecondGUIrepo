#include "coord.h"
#include "library.h"

namespace def
{
// assigned in readData, reduce channels, etc
int ns = 20;

// assigned in mainwindow.cpp
int right() {return fftLimit(def::rightFreq, def::freq, def::fftLength) + 1;}
int left()  {return fftLimit(def::leftFreq, def::freq, def::fftLength);}

double leftFreq = 5.;
double rightFreq = 20.;

double freq = 250;
//int fftLength = 4096;
int fftLength = 1024;
QString cfgFileName = "tmp.net";

// for pewpew
double drawNorm = -1.;
spectraGraphsNormalization drawNormTyp = spectraGraphsNormalization::all;


QString ExpName;
QDir * dir = new QDir();

QStringList colours{"blue", "red", "green", "black", "gray"};
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
int learnSetStay = 100;
double decayRate = 0.00;
}
