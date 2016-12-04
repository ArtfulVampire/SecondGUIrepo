#ifndef OUTPUT_H
#define OUTPUT_H

#include <ios>
#include <QString>

namespace myLib
{
const QString slash = "/"; // QString(QDir::separator());

QString rightNumber(const unsigned int input, int N); // prepend zeros
QString fitNumber(const double & input, int N); // append spaces
QString fitString(const QString & input, int N); // append spaces

std::istream & operator>> (std::istream &is, QString & in);
std::ostream & operator<< (std::ostream &os, const QString & toOut);
std::ostream & operator<< (std::ostream &os, QChar toOut);

// containers w/o allocators
template <typename Typ, template <typename> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);

// containers with allocators
template <typename Typ, template <typename, typename = std::allocator<Typ>> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);


template <typename Typ>
inline QString nm(Typ in) {return QString::number(in);}
template QString nm(int in);
template QString nm(double in);
}

#endif // OUTPUT_H
