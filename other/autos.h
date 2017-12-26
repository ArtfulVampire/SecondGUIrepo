#ifndef AUTOS_H
#define AUTOS_H

#include <chrono>
#include <thread>
#include <fstream>
#include <functional>

#include <other/edffile.h>
#include <myLib/iitp.h>

#include <QTime>
#include <QTextStream>

namespace autos
{

/// features
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
void countLogFFT(const matrix & inData,
				 double srate,
				 std::ostream & outStr);

enum featuresMask {
	spectre	= 0x01,
	alpha	= 0x02,
	fracDim	= 0x04,
	Hilbert	= 0x08,
	wavelet	= 0x10,
	Hjorth	= 0x20,
	logFFT	= 0x40
};

using featureFuncType = std::function<void(const matrix &, double, std::ostream &)>;

const std::vector<std::tuple<int, QString, featureFuncType, int>> FEATURES {
	std::make_tuple(featuresMask::spectre,	"spectre",	autos::countFFT,		18 * 19),
			std::make_tuple(featuresMask::alpha,	"alpha",	autos::countAlpha,		1 * 19),
			std::make_tuple(featuresMask::fracDim,	"fracDim",	autos::countFracDim,	1 * 19),
			std::make_tuple(featuresMask::Hilbert,	"Hilbert",	autos::countHilbert,	2 * 2 * 19),	/// 2 * 3 * 19 Xenia
			std::make_tuple(featuresMask::wavelet,	"wavelet",	autos::countWavelet,	3 * 19 * 19),
			std::make_tuple(featuresMask::Hjorth,	"Hjorth",	autos::countHjorth,		2 * 19),
			std::make_tuple(featuresMask::logFFT,	"logFFT",	autos::countLogFFT,		18 * 19)

};
QString getFeatureString(featuresMask in);
int getFileLength(int in);


/// what to do with files
void countFeatures(const matrix & inData,
				   double srate,
				   const int Mask,
				   const QString & preOutPath);
void cutOneFile(const QString & filePath,
				const int wndLen,
				const QString & outPath);
void cutFilesInFolder(const QString & path,
			  const int wndLen = 16,
			  QString outPath = QString());
void refilterFolder(const QString & procDirPath,
					double lowFreq,
					double highFreq,
					bool isNotch = false);
void rereferenceFolder(const QString & procDirPath,
					   const QString & newRef);



/// main function
void GalyaProcessing(const QString & procDirPath,
					 const int numChan = 31,
					 QString outPath = QString());

/// quite useful general functions
void EdfsToFolders(const QString & inPath);
void ProcessByFolders(const QString & inPath,
				   QString outPath = QString());
void ProcessAllInOneFolder(const QString & inPath,
						   QString outPath = QString());
void ArrangeFilesToTable(const QString & inPath,
					   QString outTablePath = QString(),
					   bool writePeople = false,
					   const QString & auxFilter = QString());
void ArrangeFilesToLine(const QString & dirPath,
						const QStringList & fileNames,
						const QString & outFilePath);


/// some special variants
void Xenia_TBI(const QString & tbi_path);
void Xenia_TBI_final(const QString & finalPath,
					 QString outPath = QString());
void EEG_MRI_FD();
void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag);

void Xenia_repairTable(const QString & initPath,
					   const QString & repairedPath,
					   const QString & groupsPath = QString(),
					   const QString & namesPath = QString());








/// IITP
void IITPconcat(const QString & guyName,
				const QString & eegPostfix,
				const QString & emgPostfix);
void IITPdatToEdf(const QString & guyName);


void IITPcopyChannel(const QString & guy,
					 const QString & whatChan,
					 const QString & whereChan);

//void IITPdownsampleEmgs(const QString & guyName);
void IITPfilter(const QString & guyName,
				const QString & postfix,
				bool emg = true,
				bool gonios = true,
				bool eeg = true);

void IITPpre2(const QString & guyName);
void IITPtestCoh(const QString & guyName);
void IITPtestCoh2(const QString & guyName);

void IITPfilterGonios(const QString & guyName,
					  const std::vector<QString> & joints = {"elbow",
															"wrist",
															"knee",
															"ankle"});

void IITPremoveZchans(const QString & hauptDir = def::iitpSyncFolder);
void IITPremoveZchans(const QString & guyName, const QString & dirPath = def::iitpSyncFolder);


void IITPcopyToCar(const QString & guyName);

void IITPrerefCAR(const QString & guyName,
				  const QString & dirPath = def::iitpSyncFolder,
				  const QString & addFilter = QString());

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

void IITPwriteCohsToFile(std::ofstream & outStr,
						 iitp::iitpData & dt,
						 int fileNum,
						 double confidenceLevel);
/// func 28.11.17
void IITPdrawCohMaps(const std::vector<iitp::forMap> & forMapsVector,
					 const QString & guyName,
					 const QString & resultsPathPrefix);


void IITPdrawSpectralMaps(const QString & guyName,
						  const QString & dirPath = def::iitpResFolder);

void IITPdrawSameScale(const QString & guyName, const std::vector<int> & nums);

void IITPdat(const QString & guyName = "Ira");
void IITPrename(const QString & guyName = "Ira");

QPixmap IITPdrawCoh(const std::valarray<std::complex<double> > & inData,
					double minFreq,
					double maxFreq,
					double srate,
					double fftLen,
					double maxVal,
					double confidence);




class FeedbackClass
{
public:
	FeedbackClass() {}
	FeedbackClass(const QString & guyPath_,
				  const QString & guyName_,
				  const QString & postfix_)
		: guyPath(guyPath_), guyName(guyName_), postfix(postfix_) {}
	~FeedbackClass() {}

	enum class taskType : int {spat = 0, verb = 1};
	enum class fileNum  : int {first = 0, third = 1};
	enum class ansType  : int {skip = 0, right = 1, wrong = 2};

	void checkStat();
	void writeFile();

	void checkStatTimes(taskType in, ansType howSolved);
	void checkStatSolving(taskType typ, ansType howSolved);
	void countTimes();

	static const int numTasks = 40;

private:
	QString ansPath(int numSes);
	std::vector<int> readAnsFile(int numSes);
	std::valarray<double> timesToArray(taskType in, fileNum filNum, ansType howSolved);

private:
	QString guyPath;
	QString guyName;
	QString postfix;

	std::vector<std::pair<int, double>> times[2][2]; /// [numFile][taskType][taskNum][ansType, time]

};

/// FB
void createAnsFiles(const QString & guyPath, QString guyName);
void checkMarkFBfinal(const QString & filePath);
void timesSolving(const QString & guyPath,
				  const QString & guyName,
				  const QString & postfix = QString());
QString timesPath(const QString & guyPath,
				  const QString & guyName,
				  int numSes,
				  int typ);
std::vector<double> timesFromFile(const QString & timesPath, int howSolved);
void feedbackFinalTimes(const QString & guyPath,
						const QString & guyName,
						const QString & postfix = QString());
void  checkStatResults(const QString & guyPath, const QString & guyName);

void successiveNetPrecleanWinds(const QString & windsPath);




/// OTHER
matrix makeTestData(const QString & outPath);
void clustering();

}

#endif // AUTOS_H
