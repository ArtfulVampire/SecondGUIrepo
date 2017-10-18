#ifndef OUTPUT_H
#define OUTPUT_H

#include <ios>
#include <iostream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>
#include <valarray>
#include <list>

#include <QString>
#include <QStringList>
#include <QList>

namespace myLib
{

QString rightNumber(const unsigned int input, int N); // prepend zeros
QString fitNumber(double input, int N); // append spaces
QString fitString(const QString & input, int N); // append spaces

}

namespace myOut
{

template <typename Typ>
inline QString nm(Typ in) { return QString::number(in); }
template QString nm(int in);
template QString nm(double in);

inline QString rn(double in, int num) { return myLib::rightNumber(in, num); }

FILE * fopen(QString filePath, const char *__modes);

std::istream & operator>> (std::istream &is, QString & in);
std::ostream & operator<< (std::ostream &os, QChar toOut);
std::ostream & operator<< (std::ostream &os, const QString & toOut);
std::ostream & operator<< (std::ostream &os, const std::complex<double> & toOut);
std::ostream & operator<< (std::ostream &os, const QStringList & toOut);

inline std::ostream & myWrite (std::ostream & os)
{
	os << std::endl;
	return os;
}

template<class input, class... inputs>
inline std::ostream & myWrite (std::ostream & os, const input & in, const inputs &... ins)
{
	os << in << "\t";
	myWrite(os, ins...);
	return os;
}


// containers w/o allocators
template <typename Typ, template <typename> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);

// containers with allocators
template <typename Typ, template <typename, typename = std::allocator<Typ>> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);


}


#endif // OUTPUT_H
