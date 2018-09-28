#include <myLib/mati.h>

#include <myLib/output.h>

using namespace myOut;

namespace myLib
{


QString matiCountByteStr(double marker)
{
    QString result;
    std::vector<bool> byteMarker;
    byteMarker = matiCountByte(marker);

    for(int h = 15; h >= 0; --h)
    {
		result += nm(static_cast<int>(byteMarker[h]));
		if(h == 8) { result += " "; }
    }
    return result;

}
void matiPrintMarker(double marker, const QString & pre)
{
    std::vector<bool> byteMarker;
    byteMarker = matiCountByte(marker);

    if(!pre.isEmpty())
    {
		std::cout << pre << " = ";
    }
	std::cout << marker << "\t" << matiCountByteStr(marker) << std::endl;
}

std::vector<bool> matiCountByte(double marker)
{
	std::vector<bool> byteMarker(16);
    for(int h = 0; h < 16; ++h)
    {
		byteMarker[h] = matiCountBit(marker, h);
    }
    return byteMarker;
}

void matiFixMarker(double & marker)
{
    /// throw 10000000 00000000 and 00000000 10000000 and 00000000 00000000
	if(marker == std::pow(2, 15) || marker == std::pow(2, 7) || marker == 0.)
    {
        marker = 0;
        return;
    }

    std::vector<bool> byteMarker = matiCountByte(marker);
    bool boolBuf;

    if(!byteMarker[7]) /// elder byte should start with 0 and younger - with 1
    {
        /// swap bytes if wrong order
        for(int h = 0; h < 8; ++h)
        {
            boolBuf = byteMarker[h];
            byteMarker[h] = byteMarker[h + 8];
            byteMarker[h + 8] = boolBuf;
        }
        byteMarker[7] = 1;
    }
	marker = static_cast<double>(matiCountDecimal(byteMarker));
}

int matiCountDecimal(std::vector<bool> byteMarker)
{
    int res = 0;
    for(int h = 0; h < 16; ++h)
    {
        res += byteMarker[h] * std::pow(2, h);
    }
    return res;
}

int matiCountDecimal(QString byteMarker)
{
    byteMarker.remove(' ');
	if(byteMarker.length() != 16) { return 0; }

    int res = 0;
    for(int h = 0; h < 16; ++h)
    {
		res += ((byteMarker[h] == QChar('1')) ? 1 : 0) * std::pow(2, 15-h);
    }
    return res;
}

} /// end of namespace myLib
