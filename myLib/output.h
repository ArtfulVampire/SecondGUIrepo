#ifndef OUTPUT_H
#define OUTPUT_H

#include <ios>
#include <QString>
#include <complex>

const QString slash = "/"; // QString(QDir::separator());

namespace myLib
{

QString rightNumber(const unsigned int input, int N); // prepend zeros
QString fitNumber(const double & input, int N); // append spaces
QString fitString(const QString & input, int N); // append spaces

}


namespace myOut
{

template <typename Typ>
inline QString nm(Typ in) {return QString::number(in);}
template QString nm(int in);
template QString nm(double in);


FILE * fopen(QString filePath, const char *__modes);

std::istream & operator>> (std::istream &is, QString & in);
std::ostream & operator<< (std::ostream &os, const QString & toOut);
std::ostream & operator<< (std::ostream &os, QChar toOut);
std::ostream & operator<< (std::ostream &os, const std::complex<double> & toOut);

// containers w/o allocators
template <typename Typ, template <typename> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);

// containers with allocators
template <typename Typ, template <typename, typename = std::allocator<Typ>> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);
}


#endif // OUTPUT_H