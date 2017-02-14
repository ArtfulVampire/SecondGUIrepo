#ifndef AUTOS_H
#define AUTOS_H

#include "library.h"
#include "edffile.h"

namespace autos
{

void filtering_test();

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
void GalyaWavelets(const QString & inPath,
				   int numChan = 19,
				   double freq = 250,
				   QString outPath = QString());
void waveletOneFile(const matrix & inData,
					int numChan,
					double freq,
					const QString & outFile);

void GalyaFull(const QString & inDirPath,
			   QString outDirPath = QString(),
			   QString outFileNames = QString(),
			   int numChan = 32,
			   int freq = 256,
			   int rightNum = 3);

void XeniaArrangeToLine(const QString & dirPath,
						const QStringList & fileNames,
						const QString & outFilePath);


void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag);
void Xenia_TBI();

void IITPpre(const QString & dirName = "Ira");
void IITPtestCoh(const QString & dirName = "Ira",
				 const QString & postfix = "_sum_f_new");
void IITPfilterGonios(const QString & guyName,
					  const QString & postfix = "_sum_f",
					  const std::vector<QString> & joints = {"elbow",
															"wrist",
															"knee",
															"ankle"});



void IITPstaging(const QString & guyName,
				 const QString & postfix = "_sum_f_new",
				 const QString & dirPath = def::iitpSyncFolder);
void IITPprocessStaged(const QString & guyName,
					   const QString & postfix = "_sum_f_new_stag",
					   const QString & dirPath = def::iitpSyncFolder);

void IITPdat(const QString & dirName = "Ira");
void IITPrename(const QString & dirName = "Ira");

void repairMarkersInNewFB(QString edfPath, int numSession);
int numMarkers(const QString & edfPath, const std::vector<int> & markers);

void makeRightNumbersCF(const QString & dirPath, int startNum);
void makeRightNumbers(const QString & dirPath,
					  int length = 3);
void makeTableFromRows(const QString & work,
					   QString tablePath = QString(),
					   const QString & auxFilter = QString());

void avTime(const QString & realsDir = def::dir->absolutePath() + slash + "Reals");
void timesNew(const QString & edfPath, int numSession);
void avTimesNew(const QString & edfPath, int numSession);

void clustering();
}

#endif // AUTOS_H
