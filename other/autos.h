#ifndef AUTOS_H
#define AUTOS_H

#include <fstream>
#include <functional>

#include <other/edffile.h>
#include <other/defs.h>
#include <myLib/iitp.h>

namespace autos
{

/// features
void countFFT(const matrix & inData,
			  double srate,
			  std::ostream & outStr);
void countAlphaPeak(const matrix & inData,
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


using featureFuncType = std::function<void(const matrix &, double, std::ostream &)>;

const std::vector<std::tuple<int, QString, featureFuncType, int>> FEATURES {
	std::make_tuple(featuresMask::fft,	"spectre",	autos::countFFT,		18 * 19),
			std::make_tuple(featuresMask::alphaPeak,	"alpha",	autos::countAlphaPeak,	1 * 19),
			std::make_tuple(featuresMask::fracDim,	"fracDim",	autos::countFracDim,	1 * 19),
			std::make_tuple(featuresMask::Hilbert,	"Hilbert",	autos::countHilbert,	2 * 2 * 19),	/// 2 * 3 * 19 Xenia
			std::make_tuple(featuresMask::wavelet,	"wavelet",	autos::countWavelet,	1 * 19 * 19),	/// 3 is a number of wavelet funcs
			std::make_tuple(featuresMask::Hjorth,	"Hjorth",	autos::countHjorth,		2 * 19),		/// mobility and complexity
			std::make_tuple(featuresMask::logFFT,	"logFFT",	autos::countLogFFT,		18 * 19)

};
QString getFeatureString(featuresMask in);
int getFileLength(int in);

double countRhythmAdoption(const std::valarray<double> & sigRest,
						   const std::valarray<double> & sigAdop,
						   double freq);


void rhythmAdoptionGroup(const QString & groupPath,
						 const QString & restMark,
						 const QString & stimType);
void rhythmAdoption(const QString & filesPath,
					const QString & restMark,			/// "_og", "_zg" or "_fon"
					const QString & stimType);		/// "sv" or "zv"

/// what to do with files
void countFeatures(const matrix & inData,
				   const double srate,
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

/// labels file
void makeLabelsFile(int numChan,
					const QString & workPath,
					const QString & initFreq,
					const std::vector<QString> & usedMarkers,
					const QString & sep);


/// main function
void calculateFeatures(const QString & pathWithEdfs,
					 const int numChan,
					 const QString & outPath);

void elenaCalculation(const QString & realsPath, const QString & outTableDir);

/// quite useful general functions
void rewriteNew(const QString & inPath);
void EdfsToFolders(const QString & inPath);
void ProcessByFolders(const QString & inPath, int numChan,
					  const std::vector<QString> & markers);
void ProcessAllInOneFolder(const QString & inPath,
						   QString outPath = QString());

void ArrangeFilesVertCat(const std::vector<QString> pathes,
						 const QString & outPath);

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
void Xenia_TBI_finalest(const QString & finalPath,
						const QString & outPath,
						const std::vector<QString> markers);
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

bool IITPtestInitialFiles(const QString & guyName);

/// vector[fileName, [absent channels]]
std::vector<std::pair<QString, std::vector<QString>>> IITPtestEegChannels(const QString & guyDir,
																		  const QString & postfix);

void IITPinsertChannels(const QString & guyDir,
						const std::vector<std::pair<QString, std::vector<QString>>> & forInsert);


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

void IITPrectifyEmg(const QString & guyName,
				  const QString & postfix = "_sum_new",
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


/// OTHER
matrix makeTestData(const QString & outPath);
void clustering();


namespace marks
{
	/// Galya markers
	const std::vector<QString> tactile{"_buk", "_kis", "_rol", "_sch", "_og", "_zg"};

	const std::vector<QString> Dasha{"_brush", "_cry", "_fire", "_flower",
									 "_isopropanol", "_needles", "_vanilla", "_wc"};

	const std::vector<QString> usedMarkers{"_bd", "_bw", "_cr", "_kh", "_na", "_no",
										   "_ph", "_rv", "_sc", "_sm", "_og", "_zg"};
	const std::vector<QString> TBI{"_bd", "_bw", "_cr", "_kh", "_na", "_no",
										   "_og", "_rv", "_sm", "_zg"};

	const std::vector<QString> rhythAdoption{"_2sv", "_2zv",
											 "_4sv", "_4zv",
											 "_8sv", "_8zv",
											 "_16sv", "_16zv",
											 "_fon",
//											 "_og",
//											 "_zg",
											};
	const std::vector<QString> buben{"_bub",		"_bubAfter",
									 "_bubAud",	"_bubAudAfter",
									 "_time",
									 "_og",		"_ogAfter",
									 "_zg",
									};
	const std::vector<QString> burdenkoFaces{"_all", "_svoi", "_other"};
}

}

#endif /// AUTOS_H
