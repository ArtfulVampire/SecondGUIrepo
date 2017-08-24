#ifndef AUTOS_H
#define AUTOS_H

#include <thread>
#include <fstream>

#include <other/edffile.h>

#include <QTime>
#include <QTextStream>

namespace autos
{

void countFFT(const matrix & inData,
			  double srate,
			  std::ostream & outStr);
void countAlpha(const matrix & inData,
				double srate,
				std::ostream & outStr);
void countFracDim(const matrix & inData,
				  double srate,
				  std::ostream & outStr);
void countHilbert(const matrix & inData,
				  double srate,
				  std::ostream & outStr);
void countWavelet(const matrix & inData,
				  double srate,
				  std::ostream & outStr);
void countHjorth(const matrix & inData,
				 double srate,
				 std::ostream & outStr);

enum featuresMask {spectre	= 0x01,
				   alpha	= 0x02,
				   fracDim	= 0x04,
				   Hilbert	= 0x08,
				   wavelet	= 0x10,
				   Hjorth	= 0x20};

using featureFuncType = std::function<void(const matrix &, double, std::ostream &)>;

const std::vector<std::tuple<int, QString, featureFuncType, int>> FEATURES {
	std::make_tuple(featuresMask::spectre,	"spectre",	autos::countFFT,		18 * 19),
	std::make_tuple(featuresMask::alpha,	"alpha",	autos::countAlpha,		1 * 19),
	std::make_tuple(featuresMask::fracDim,	"fracDim",	autos::countFracDim,	1 * 19),
	std::make_tuple(featuresMask::Hilbert,	"Hilbert",	autos::countHilbert,	2 * 2 * 19),	/// 2 * 3 * 19 Xenia
	std::make_tuple(featuresMask::wavelet,	"wavelet",	autos::countWavelet,	3 * 19 * 19),
	std::make_tuple(featuresMask::Hjorth,	"Hjorth",	autos::countHjorth,		2 * 19)
};
QString getFeatureString(int in);
int getFileLength(int in);



void countFeatures(const matrix & inData,
				   double srate,
				   const int Mask,
				   const QString & preOutPath);


/// main function
void GalyaProcessing(const QString & procDirPath,
					 const int numChan = 31,
					 QString outPath = QString());
void GalyaToFolders(const QString & inPath);


void Galya_tactile(const QString & inPath,
				   QString outPath = QString());
void Xenia_TBI(const QString & tbi_path);
void Xenia_TBI_final(const QString & finalPath,
					 QString outPath = QString());

void GalyaCut(const QString & path,
			  const int wndLen = 16,
			  QString outPath = QString());
void cutOneFile(const QString & filePath,
				const int wndLen,
				const QString & outPath);

/// temporary
void GalyaFull(const QString & inDirPath,
			   QString outDirPath = QString(),
			   QString outFileNames = QString(),
			   int numChan = 32,
			   int freq = 256,
			   int rightNum = 3);




void XeniaArrangeToLine(const QString & dirPath,
						const QStringList & fileNames,
						const QString & outFilePath);
void Xenia_repairTable(const QString & initPath,
					   const QString & repairedPath,
					   const QString & groupsPath = QString(),
					   const QString & namesPath = QString());



void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag);


void refilterFolder(const QString & procDirPath,
					double lowFreq,
					double highFreq,
					bool isNotch = false);

void rereferenceFolder(const QString & procDirPath,
					   const QString & newRef);






void IITPconcat(const QString & guyName = "Ira");
void IITPdatToEdf(const QString & guyName = "Ira");
//void IITPdownsampleEmgs(const QString & guyName);
void IITPfilter(const QString & guyName = "Ira",
				bool emg = true,
				bool gonios = true,
				bool eeg = true);

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

void IITPrerefCAR(const QString & guyName);

void IITPemgToAbs(const QString & guyName,
				  QString postfix = "_sum_new",
				  const QString & dirPath = def::iitpSyncFolder);
void IITPstagedToEnveloped(const QString & guyName,
						   QString postfix = "_sum_new_f",
						   const QString & dirPath = def::iitpSyncFolder);
void IITPstaging(const QString & guyName,
				 const QString & dirPath = def::iitpSyncFolder);
void IITPprocessStaged(const QString & guyName,
					   const QString & dirPath = def::iitpSyncFolder);
void IITPdrawSpectralMaps(const QString & guyName,
						  const QString & dirPath = def::iitpResFolder);

void IITPdrawSameScale(const QString & guyName, const std::vector<int> & nums);

void IITPdat(const QString & guyName = "Ira");
void IITPrename(const QString & guyName = "Ira");





void mixNumbersCF(const QString & dirPath);
void makeRightNumbersCF(const QString & dirPath, int startNum);
void makeRightNumbers(const QString & dirPath,
					  int length = 3);
void makeTableFromRows(const QString & inPath,
					   QString outTablePath = QString(),
					   const QString & auxFilter = QString());

void repairMarkersInNewFB(QString edfPath, int numSession);
int numMarkers(const QString & edfPath, const std::vector<int> & markers);
void avTime(const QString & realsDir = def::dir->absolutePath() + "/Reals");
void timesNew(const QString & edfPath, int numSession);
void avTimesNew(const QString & edfPath, int numSession);


void successivePrecleanWinds(const QString & windsPath);
void successiveNetPrecleanWinds(const QString & windsPath);


matrix makeTestData(const QString & outPath);
void filtering_test();
void clustering();
}

#endif // AUTOS_H
