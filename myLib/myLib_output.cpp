#include <myLib/output.h>

#include <myLib/small.h>

namespace myLib
{

QString fitNumber(double input, int N) /// append spaces
{
	auto a = QString::number(input);
	return a + QString(N - a.size(), ' ');
}
QString fitString(const QString & input, int N) /// append spaces
{
	return input + QString(N - input.size(), ' ');
}

QString rightNumber(int input, int N) /// prepend zeros
{
	auto a = QString::number(input);
	return QString(N - a.size(), '0') + a;
}

QString getDialogFilter(const QString & suffix)
{
	QString filter{};
	for(const QString & in : def::edfFilters)
	{
		auto a = (suffix.isEmpty() ? "" :  ("*" + suffix)) + in;
		filter += a + " ";
	}
	filter.prepend("EDF files (");
	filter += ')';
	return filter;
}

QStringList getFilters(const QString & suffix)
{
	QStringList filters{};
	for(const QString & in : def::edfFilters)
	{
		auto a = (suffix.isEmpty() ? "" :  ("*" + suffix)) + in;
		filters.push_back(a);
	}
	return filters;
}

} /// end of namespace myLib



namespace myOut
{

std::istream & operator>> (std::istream & is, QString & in)
{
	std::string tmp;
	is >> tmp;
	in = QString(tmp.c_str());
	return is;
}

std::ostream & operator<< (std::ostream & os, const QString & toOut)
{
	os << toOut.toStdString();
	return os;
}

std::ostream & operator << (std::ostream & os, QChar toOut)
{
	os << QString(toOut);
	return os;
}

std::ostream & operator<< (std::ostream & os, const std::complex<double> & toOut)
{
	os << toOut.real() << ((toOut.imag() >= 0.) ? " + " : " - ") << std::abs(toOut.imag()) << "i";
	return os;
}

std::ostream & operator<< (std::ostream & os, const QStringList & toOut)
{
#if 0
	std::string separ = "\t";
#else
	std::string separ = "\r\n";
#endif
	for(const QString & in : toOut)
	{
		os << in << separ;
	}
	return os;
}


//// with allocators
//template std::ostream & operator << (std::ostream & os, const std::vector<std::vector<double>> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::vector<int> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::vector<uint> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::vector<double> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::vector<QString> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::list<int> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::list<double> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::list<QString> & toOut);
////template std::ostream & operator << (std::ostream & os, const QString & toOut);

//// w/o allocators
//template std::ostream & operator << (std::ostream & os, const std::valarray<double> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::valarray<std::complex<double>> & toOut);
//template std::ostream & operator << (std::ostream & os, const std::valarray<int> & toOut);
} /// end of namespace myOut
