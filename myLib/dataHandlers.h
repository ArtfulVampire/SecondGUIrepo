#ifndef DATAHANDLERS_H
#define DATAHANDLERS_H

#include <matrix.h>
#include <QString>
#include <valarray>

namespace myLib
{
void makeFullFileList(const QString & path,
					  QStringList & lst,
					  const QStringList & auxFilters = QStringList());

void makeFileLists(const QString & path,
				   std::vector<QStringList> & lst,
				   const QStringList & auxFilters = QStringList());

void cleanDir(QString dirPath, QString nameFilter = QString(), bool ext = true);

void readPlainData(const QString & inPath,
				   matrix & data,
				   int & numOfSlices,
				   int start = 0);

void writePlainData(const QString outPath,
					const matrix & data,
					int numOfSlices = 0,
					int start = 0);

void readMatrixFile(const QString & filePath,
					matrix & outData);

void readMatrixFileRaw(const QString & filePath,
					   matrix & outData);

void writeMatrixFile(const QString & filePath,
					 const matrix & outData,
					 const QString & rowsString = "NumOfRows",
					 const QString & colsString = "NumOfCols");


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
