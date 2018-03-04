#ifndef GENERAL_H
#define GENERAL_H

#include <chrono>
#include <iostream>

#include <other/matrix.h>

#include <QString>
#include <QTime>

namespace myLib
{

std::string funcName(std::string in);
#define TIME(arg)\
	do{\
		auto t0 = std::chrono::high_resolution_clock::now();\
		arg;\
		auto t1 = std::chrono::high_resolution_clock::now();\
		std::cout << myLib::funcName(#arg) \
		<< ": time elapsed = "\
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count() / 1000.\
		<< " nsec" << std::endl;\
	}while(false)



// small shit
void writeWavFile(const std::vector<double> & inData, const QString & outPath);
int len(const QString & s); // string length for EDF+ annotations

QString kyrToLatin(const QString & in);
QString getFileName(const QString & filePath, bool withExtension = true);
QString getExpNameLib(const QString & filePath, bool shortened = false);
QString getDirPathLib(const QString & filePath);
QString getExtension(const QString & filePath);
QString getLabelName(const QString & label);
QString getPicPath(const QString & dataPath,
				   const QString & ExpNameDir = DEFS.dirPath());
QString getFileMarker(const QString & fileName);
int getTypeOfFileName(const QString & fileName);

void cleanDir(QString dirPath, QString nameFilter = QString(), bool ext = true);

QString setFileName(const QString & initNameOrPath); // -> initName_i.ext



bool areEqualFiles(QString path1, QString path2);
double areSimilarFiles(const QString & path1,
					   const QString & path2);
int countSymbolsInFile(const QString & filePath, char inChar);
void replaceSymbolsInFile(const QString & filePath,
						  QString inChar,
						  QString outChar,
						  QString outPath);

std::vector<int> splitStringIntoVec(const QString & in);

template <typename Container>
uint indexOfMax(const Container & cont);

template <typename Container>
uint indexOfMin(const Container & cont);

template <typename Container, typename Typ>
int indexOfVal(const Container & cont, Typ val);

template <typename Container, typename Typ>
bool contains(const Container & cont, Typ val);

template <typename Container, typename Typ>
std::pair<bool, Typ> contains(const Container & cont, const std::vector<Typ> & val);

template <class InputIterator, class Typ>
InputIterator find_back(InputIterator last, InputIterator first, const Typ & val);

template <class InputIterator, class Pred>
InputIterator find_back_if(InputIterator last, InputIterator first, Pred pred);

QString rerefChannel(const QString & initialName,
					 const QString & targetRef = "Ar",
					 const QString & currentNum = "1",
					 const QString & earsChan = "20",
					 const QString & groundChan = "21",
					 const std::vector<QString> & sign = {"-", "+"});
void dealWithEyes(matrix & inData,
				  const int dimension);


} // myLib namespace

#endif
