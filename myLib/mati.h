#ifndef MATI_H
#define MATI_H

#include <vector>
#include <iostream>
#include <cmath>

#include <QString>

namespace myLib
{
std::vector<bool> matiCountByte(double marker);
QString matiCountByteStr(double marker);
void matiPrintMarker(double const & marker, QString pre  = QString());
void matiFixMarker(double & marker);
int matiCountDecimal(std::vector<bool> byteMarker);
int matiCountDecimal(QString byteMarker);
inline bool matiCountBit(double const & marker, int num)
{
	return (int(marker) / int(std::pow(2, num))) % 2;
}

}

#endif // MATI_H
