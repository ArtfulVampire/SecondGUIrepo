#ifndef DATAHANDLERS_H
#define DATAHANDLERS_H

#include <valarray>

#include <other/matrix.h>
#include <other/defs.h>

#include <QString>

namespace myLib
{
QStringList makeFullFileList(const QString & path,
							 const QStringList & auxFilters = QStringList());

std::vector<QStringList> makeFileLists(const QString & path,
									   const QStringList & auxFilters = QStringList());
std::vector<matrix> readSpectraDir(const QString & spectraPath);


matrix readPlainData(const QString & inPath,
					 int start = 0);

void writePlainData(const QString outPath,
					const matrix & data,
					int sta = 0,
					int fin = -1);

matrix readMatrixFile(const QString & filePath);

matrix readMatrixFileRaw(const QString & filePath);


void writeMatrixFile(const QString & filePath,
					 const matrix & outData,
					 const QString & rowsString = "NumOfRows",
					 const QString & colsString = "NumOfCols");

void writeSubmatrixFile(const QString & filePath,
						const matrix & outData,
						int sta = 0,
						int fin = -1,
						const QString & rowsString = "NumOfRows",
						const QString & colsString = "NumOfCols");

/// transpose?
void invertMatrixFile(const QString & inPath,
					  const QString & outPath);


matrix readIITPfile(const QString & filePath);

void readIITPfile(const QString & filePath,
				  matrix & outData,
				  std::vector<QString> & outLabels);

void readUCIdataSet(const QString & setName,
					matrix & outData,
					std::vector<uint> & outTypes);


std::valarray<double> readFileInLine(const QString & filePath);

std::valarray<double> readFileInLineRaw(const QString & filePath);

template <typename ArrayType>
void writeFileInLine(const QString & filePath,
					 const ArrayType & outData);


}

#endif // DATAHANDLERS_H
