#ifndef AUTOS_H
#define AUTOS_H

#include "library.h"
#include "edffile.h"

namespace autos
{

matrix makeTestData(const QString & outPath);

void GalyaProcessing(const QString &procDirPath,
					 const int numChan = 31,
					 QString outPath = QString());
void countSpectraFeatures(const QString & filePath,
						  const int numChan,
						  const QString & outPath);
void countChaosFeatures(const QString & filePath,
						  const int numChan,
						  const QString & outPath);
void GalyaCut(const QString & path,
			  const int wndLen = 16,
			  QString outPath = QString());
void cutOneFile(const QString & filePath,
				const int wndLen,
				const QString & outPath);
void GalyaWavelets(const QString & inDir,
				   int numChan = 19,
				   double freq = 250,
				   QString outDir = QString());
void waveletOneFile(const matrix & inData,
					int numChan,
					double freq,
					const QString & outFile);


void makeRightNumbers(const QString & dirPath,
					  int length = 3);
void makeTableFromRows(const QString & work,
					   QString tablePath = QString(),
					   const QString & auxFilter = QString());

void avTime(const QString & realsDir = def::dir->absolutePath() + myLib::slash + "Realisations");

void clustering();
}

#endif // AUTOS_H
