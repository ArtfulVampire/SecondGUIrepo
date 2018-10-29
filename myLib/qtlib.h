#ifndef QTLIB_H
#define QTLIB_H

#include <QtWidgets>

#include <sstream>
#include <iostream>
#include <fstream>
#include <type_traits>

namespace qtLib
{
void hideLayout(QLayout * lay);
template <class C> std::vector<C*> widgetsOfLayout(QLayout *);


enum class outputType{cout, file, textEdit};
class MyTextStream // : public std::basic_ostream<char, std::char_traits<char>>
{
public:
	MyTextStream() {}
	MyTextStream(outputType in) : typ{in} {}
	~MyTextStream() { if(fil) { fil.close(); } }
	void setOutputType(qtLib::outputType in) { typ = in; }

	void setTextEdit(QTextEdit * in) { te = in; }
	void setFile(const QString path, std::ios_base::openmode mode)
	{
		fil.open(path.toStdString(), mode);
	}

	using stdCoutType = std::basic_ostream<char, std::char_traits<char>>;
	using stdEndlType = stdCoutType& (*)(stdCoutType &);


	/// std::endl
	MyTextStream & operator<< (stdEndlType)
	{
		switch(typ)
		{
		case outputType::cout: { std::cout << std::endl; break; }
		case outputType::file: { fil << std::endl; break; }
		case outputType::textEdit: { te->append(""); break; }
//		default: { /* should never get here */ }
		}
		return *this;
	}

	/// all the others
	template <typename Arg
			  ,typename = typename std::enable_if<
				  !std::is_same<Arg,std::__cxx11::basic_string<char>>::value
				  >::type
			  >
	MyTextStream & operator <<(Arg && arg)
	{
		std::ostringstream str;
		str << std::forward<Arg>(arg);
		switch(typ)
		{
		case outputType::cout: { std::cout << str.str(); break; }
		case outputType::file: { fil << str.str(); break; }
		case outputType::textEdit:
		{
			te->moveCursor(QTextCursor::End);
			te->insertPlainText(QString(str.str().c_str()));
			te->moveCursor(QTextCursor::End);
			break;
		}
//		default: { /* should never get here */ }
		}
		return *this;
	}

private:
	QTextEdit * te;
	outputType typ{outputType::cout};
	std::ofstream fil{};
};

}

#endif /// QTLIB_H
