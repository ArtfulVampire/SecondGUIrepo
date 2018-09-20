#ifndef OUTPUT_H
#define OUTPUT_H

#include <other/consts.h>

#include <ios>
#include <iostream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>
#include <valarray>
#include <list>
#include <type_traits>

#include <QString>
#include <QStringList>
#include <QList>

namespace myLib
{

QString rightNumber(int input, int N);					/// prepend zeros
QString fitNumber(double input, int N);					/// append spaces
QString fitString(const QString & input, int N);		/// append spaces

}

namespace myOut
{

template <typename Typ>
inline QString nm(Typ in) { return QString::number(in); }
template QString nm(int in);
template QString nm(double in);

inline QString rn(int in, int num) { return myLib::rightNumber(in, num); }

FILE * fopen(QString filePath, const char *__modes);

std::istream & operator>> (std::istream & is, QString & in);
std::ostream & operator<< (std::ostream & os, QChar toOut);
std::ostream & operator<< (std::ostream & os, const QString & toOut);
std::ostream & operator<< (std::ostream & os, const std::complex<double> & toOut);
std::ostream & operator<< (std::ostream & os, const QStringList & toOut);


#if CPP17
template <typename T
		  , typename = typename std::enable_if<!std::is_same<T,char>::value &&
											   !std::is_same<T,std::string>::value>::type
		  >
inline std::ostream & operator+= (std::ostream & os, const T & in)
{
	os << in << "\t";
	return os;
}
template<class... inputs>
inline std::ostream & myWrite (std::ostream & os, const inputs &... ins)
{
	(os += ... += ins);
	os << std::endl;
	return os;
}
#else
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
#endif


/// containers w/o allocators
template <typename Typ,
		  template <typename> class Cont
		  ,	typename = typename std::enable_if<!std::is_same<Cont<Typ>, std::string>::value>::type
		  >
inline std::ostream & operator<< (std::ostream & os, const Cont<Typ> & toOut)
{
#if 01
	std::string separ = "\t";
#else
	std::string separ = "\n";
#endif
	for(const auto & in : toOut)
	{
		os << in << separ;
	}
	os.flush();
	return os;
}

/// containers with allocators
template <typename Typ,
		  template <typename, typename = std::allocator<Typ>> class Cont
		  >
inline std::ostream & operator<< (std::ostream & os, const Cont<Typ> & toOut)
{
#if 01
	std::string separ = "\t";
#else
	std::string separ = "\n";
#endif
	for(const auto & in : toOut)
	{
		os << in << separ;
	}
	os.flush();
	return os;
}

}


#endif /// OUTPUT_H
