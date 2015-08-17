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

int fftLength = 4096;
QString cfgFileName = "16sec19ch.net";

double spStep = def::freq / def::fftLength;
QString ExpName;
QDir * dir = new QDir();

QStringList colours = QStringList{"blue", "red", "green", "black", "gray"};
}
