#include "coord.h"
#include "library.h"

namespace def
{
// assigned in readData, reduce channels, etc
int ns = 20;
//int nsW = def::ns - 1 * def::withMarkersFlag; // ns without markers

// assigned in mainwindow.cpp
int left;
int right;
int spLength;

double freq = 250;
int fftLength = 4096;
QString cfgFileName = "tmp.net";

// for pewpew
double drawNorm = -1.;

double spStep = def::freq / def::fftLength;
QString ExpName;
QDir * dir = new QDir();

QStringList colours{"blue", "red", "green", "black", "gray"};
QStringList fileMarkers{"_241", "_247", "_254 _244"};
//QStringList fileMarkers{"_241", "_247"};

//QStringList fileMarkers{"_241", "_242", "_245", "_246"}; // Ossadtchi
//QStringList fileMarkers{"_241 _242", "_245 _246"}; // Ossadtchi
//QStringList fileMarkers{"_242", "_245"}; // Ossadtchi

int numOfClasses = fileMarkers.length();
}
