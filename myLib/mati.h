#ifndef MATI_H
#define MATI_H

#include <vector>
#include <cmath>

#include <QString>

namespace myLib
{
std::vector<bool> matiCountByte(double marker);
QString matiCountByteStr(double marker);
void matiPrintMarker(double marker, const QString & pre  = QString());
void matiFixMarker(double & marker);
int matiCountDecimal(std::vector<bool> byteMarker);
int matiCountDecimal(QString byteMarker);
inline bool matiCountBit(double marker, int num)
{
	return static_cast<int>(marker / std::pow(2, num)) % 2;
}

}

#endif /// MATI_H
