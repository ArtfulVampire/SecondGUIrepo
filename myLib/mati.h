#ifndef MATI_H
#define MATI_H

#include <vector>
#include <QString>

namespace myLib
{
std::vector<bool> matiCountByte(const double & marker);
QString matiCountByteStr(const double & marker);
void matiPrintMarker(double const & marker, QString pre  = QString());
void matiFixMarker(double & marker);
int matiCountDecimal(std::vector<bool> byteMarker);
int matiCountDecimal(QString byteMarker);
inline bool matiCountBit(double const & marker, int num)
{
	return (int(marker) / int(pow(2, num))) % 2;
}

}

#endif // MATI_H
