#ifndef WAVELET_H
#define WAVELET_H
#include <stdio.h>
#include <QString>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cmath>
#include <stdio.h>
#include <QFile>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QPainter>
#include <four1.h>

using namespace std;

void wavelet(QString out, FILE * file, int ns, int channelNumber, double freqMax, double freqMin, double freqStep, double pot);
void drawColorScale(QString filename, int range = 128);
QColor hue(int range, int j, double V, double S);
void waveletPhase(QString out, FILE * file, int ns, int channelNumber1, int channelNumber2, double freqMax, double freqMin, double freqStep, double pot);

#endif // WAVELET_H
