#ifndef DATAHANDLERS_H
#define DATAHANDLERS_H

#include <valarray>

#include <other/matrix.h>

#include <QString>

namespace myLib
{
QStringList makeFullFileList(const QString & path,
							 const QStringList & auxFilters = QStringList());

std::vector<QStringList> makeFileLists(const QString & path,
									   const QStringList & auxFilters = QStringList());

std::vector<matrix> readSpectraDir(const QString & spectraPath);


//matrix readPlainData(const QString & inPath); /// deprecated

//void writePlainData(const QString & outPath, const matrix & data); /// deprecated

int numLines(const QString & filePath);

matrix readMatrixFile(const QString & filePath);

matrix readMatrixFileRaw(const QString & filePath); /// without head line "rows, cols"


void writeMatrixFile(const QString & filePath,
					 const matrix & outData,
					 const QString & rowsString = "NumOfRows",
					 const QString & colsString = "NumOfCols");


matrix readIITPfile(const QString & filePath); /// dat file emg - unused

void readIITPfile(const QString & filePath,
				  matrix & outData,
				  std::vector<QString> & outLabels);/// dat file emg - remake return pair

void readUCIdataSet(const QString & setName,
					matrix & outData,
					std::vector<uint> & outTypes);


std::valarray<double> readFileInLine(const QString & filePath);
std::pair<std::valarray<double>, int> readFileInLinePair(const QString & filePath); /// with cols

std::valarray<double> readFileInLineRaw(const QString & filePath); /// w/o headers

template <typename ArrayType>
void writeFileInLine(const QString & filePath,
					 const ArrayType & outData);

void concatFilesHorz(const QString & inDirPath,
					 const std::vector<QString> & fileNames,
					 const QString & outFilePath);

void concatFilesVert(const QString & inDirPath,
					 const std::vector<QString> fileNames,
					 const QString & outFilePath);

}

#endif /// DATAHANDLERS_H
