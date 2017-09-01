#ifndef DATAHANDLERS_H
#define DATAHANDLERS_H

#include <valarray>

#include <other/matrix.h>

#include <QString>

namespace myLib
{
void makeFullFileList(const QString & path,
					  QStringList & lst,
					  const QStringList & auxFilters = QStringList());

void makeFileLists(const QString & path,
				   std::vector<QStringList> & lst,
				   const QStringList & auxFilters = QStringList());

void readPlainData(const QString & inPath,
				   matrix & data,
				   int start = 0);

void writePlainData(const QString outPath,
					const matrix & data,
					int sta = 0,
					int fin = -1);

void readMatrixFile(const QString & filePath,
					matrix & outData);

void readMatrixFileRaw(const QString & filePath,
					   matrix & outData);

void writeMatrixFile(const QString & filePath,
					 const matrix & outData,
					 int sta = 0,
					 int fin = -1,
					 const QString & rowsString = "NumOfRows",
					 const QString & colsString = "NumOfCols");

void writeSubmatrixFile(const QString & filePath,
						const matrix & outData,
						const QString & rowsString = "NumOfRows",
						const QString & colsString = "NumOfCols");

void invertMatrixFile(const QString & inPath,
					  const QString & outPath);


matrix readIITPfile(const QString & filePath);

void readIITPfile(const QString & filePath,
				  matrix & outData,
				  std::vector<QString> & outLabels);

void readUCIdataSet(const QString & setName,
					matrix & outData,
					std::vector<uint> &outTypes);


void readFileInLine(const QString & filePath,
					std::valarray<double> & result);

void readFileInLineRaw(const QString & filePath,
					   std::valarray<double> & result);

template <typename ArrayType>
void writeFileInLine(const QString & filePath,
					 const ArrayType & outData);


}

#endif // DATAHANDLERS_H
