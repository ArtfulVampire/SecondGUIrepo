#ifndef AUTOS_H
#define AUTOS_H

#include <thread>
#include <fstream>

#include <other/edffile.h>

#include <QTime>
#include <QTextStream>

namespace autos
{

void filtering_test();

matrix makeTestData(const QString & outPath);

void refilterFolder(const QString & procDirPath,
					double lowFreq,
					double highFreq,
					bool isNotch = false);
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



void IITPpre(const QString & guyName = "Ira");
void IITPpre2(const QString & guyName = "Ira");
void IITPtestCoh(const QString & guyName = "Ira");
void IITPtestCoh2(const QString & guyName = "Ira");

void IITPfilterGonios(const QString & guyName,
					  const std::vector<QString> & joints = {"elbow",
															"wrist",
															"knee",
															"ankle"});

void IITPremoveZchans(const QString & hauptDir = def::iitpSyncFolder);
void IITPremoveZchans(const QString & guyName, const QString & dirPath = def::iitpSyncFolder);


void IITPstaging(const QString & guyName,
				 const QString & dirPath = def::iitpSyncFolder);
void IITPprocessStaged(const QString & guyName,
					   const QString & dirPath = def::iitpSyncFolder);

void IITPdrawSameScale(const QString & guyName, const std::vector<int> & nums);

void IITPdat(const QString & guyName = "Ira");
void IITPrename(const QString & guyName = "Ira");




void repairMarkersInNewFB(QString edfPath, int numSession);
int numMarkers(const QString & edfPath, const std::vector<int> & markers);

void makeRightNumbersCF(const QString & dirPath, int startNum);
void makeRightNumbers(const QString & dirPath,
					  int length = 3);
void makeTableFromRows(const QString & work,
					   QString tablePath = QString(),
					   const QString & auxFilter = QString());

void avTime(const QString & realsDir = def::dir->absolutePath() + "/Reals");
void timesNew(const QString & edfPath, int numSession);
void avTimesNew(const QString & edfPath, int numSession);

void clustering();
}

#endif // AUTOS_H
