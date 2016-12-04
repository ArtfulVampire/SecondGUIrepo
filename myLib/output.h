#ifndef OUTPUT_H
#define OUTPUT_H

#include <ios>
#include <QString>

namespace myLib
{

const QString slash = "/"; // QString(QDir::separator());

std::istream & operator>> (std::istream &is, QString & in);
std::ostream & operator<< (std::ostream &os, const QString & toOut);
std::ostream & operator<< (std::ostream &os, QChar toOut);

// containers with no allocators
template <typename Typ, template <typename> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);

template <typename Typ, template <typename, typename = std::allocator<Typ>> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut);
}

#endif // OUTPUT_H
