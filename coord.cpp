#include "coord.h"
#include "library.h"

namespace def
{
// assigned in readData, reduce channels, etc
int ns = 20;

// assigned in mainwindow.cpp
int left;
int right;
int spLength;

double leftFreq = 5.;
double rightFreq = 20.;

double freq = 250;
int fftLength = 4096;
//int fftLength = 1024;
QString cfgFileName = "tmp.net";

// for pewpew
double drawNorm = -1.;
spectraGraphsNormalization drawNormTyp = spectraGraphsNormalization::all;

double spStep = def::freq / def::fftLength;
QString ExpName;
QDir * dir = new QDir();

QStringList colours{"blue", "red", "green", "black", "gray"};
QStringList fileMarkers{"_241", "_247", "_254 _244"};
//QStringList fileMarkers{"train*_241", "train*_247", "train*_254 train*_244"};
//QStringList fileMarkers{"_241", "_247"};

//QStringList fileMarkers{"_241", "_242", "_245", "_246"}; // Ossadtchi
//QStringList fileMarkers{"_241 _242", "_245 _246"}; // Ossadtchi
//QStringList fileMarkers{"_242", "_245"}; // Ossadtchi

int numOfClasses = fileMarkers.length();
}
