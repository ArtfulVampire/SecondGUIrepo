#include <myLib/output.h>

#include <myLib/small.h>

namespace myLib
{

QString fitNumber(double input, int N) // append spaces
{
	QString h;
	h.setNum(input);
	h += QString(N, ' ');
	return h.left(N);
}
QString fitString(const QString & input, int N) // append spaces
{
	QString h(input);
	h += QString(N, ' ');
	return h.left(N);
}

QString rightNumber(const unsigned int input, int N) // prepend zeros
{
	QString h;
	h.setNum(input);
	h.prepend(QString(N, '0'));
	return h.right(N);
}

} // namespace myLib



namespace myOut
{


FILE * fopen(QString filePath, const char *__modes)
{
	return std::fopen(filePath.toStdString().c_str(), __modes);
}


std::istream & operator>> (std::istream &is, QString & in)
{
	std::string tmp;
	is >> tmp;
	in = QString(tmp.c_str());
	return is;
}

std::ostream & operator << (std::ostream &os, const QString & toOut)
{
	os << toOut.toStdString();
	return os;
}

std::ostream & operator << (std::ostream &os, QChar toOut)
{
	os << QString(toOut);
	return os;
}

std::ostream & operator<< (std::ostream &os, const std::complex<double> & toOut)
{
	os << toOut.real() << ((toOut.imag() >= 0.)?" + ":" - ") << std::abs(toOut.imag()) << "i";
	return os;
}

std::ostream & operator<< (std::ostream &os, const QStringList & toOut)
{
//	std::string separ = "\t";
	std::string separ = "\r\n";
	for(const QString & in : toOut)
	{
		os << in << separ;
	}
	return os;
}

template <typename Typ, template <typename> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ> & toOut)
{
	std::string separ = "\t";
//	std::string separ = "\n";
	// if(is_container<Typ>) separ = std::endl;
	for(auto in : toOut)
	{
		os << in << separ;
//		os << smLib::doubleRound(in, 1) << separ;
	}
	os.flush();
	return os;
}

template <typename Typ, template <typename, typename> class Cont>
std::ostream & operator<< (std::ostream &os, const Cont <Typ, std::allocator<Typ>> & toOut)
{
	std::string separ = "\t";
//	std::string separ = "\n";
	// if(is_container<Typ>) separ = std::endl;
	for(auto in : toOut)
	{
		os << in << separ;
//		os << smLib::doubleRound(in, 1) << separ;
	}
	os.flush();
	return os;
}

// with allocators
template std::ostream & operator << (std::ostream & os, const std::vector<std::vector<double>> & toOut);
template std::ostream & operator << (std::ostream & os, const std::vector<int> & toOut);
template std::ostream & operator << (std::ostream & os, const std::vector<uint> & toOut);
template std::ostream & operator << (std::ostream & os, const std::vector<double> & toOut);
template std::ostream & operator << (std::ostream & os, const std::vector<QString> & toOut);
template std::ostream & operator << (std::ostream & os, const std::list<int> & toOut);
template std::ostream & operator << (std::ostream & os, const std::list<double> & toOut);
template std::ostream & operator << (std::ostream & os, const std::list<QString> & toOut);

// w/o allocators
template std::ostream & operator << (std::ostream & os, const std::valarray<double> & toOut);
template std::ostream & operator << (std::ostream & os, const std::valarray<std::complex<double>> & toOut);
template std::ostream & operator << (std::ostream & os, const std::valarray<int> & toOut);
template std::ostream & operator << (std::ostream & os, const QList<int> & toOut);
template std::ostream & operator << (std::ostream & os, const QList<double> & toOut);
} // namespace myOut
