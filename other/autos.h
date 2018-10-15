#ifndef AUTOS_H
#define AUTOS_H

#include <fstream>
#include <functional>
#include <map>
#include <unordered_map>

#include <other/edffile.h>
#include <other/defs.h>
#include <myLib/iitp.h>

namespace autos
{
/// magic const 7
enum feature {
	fft			= 0x01,	/// std::pow(2, 0);
	alphaPeak	= 0x02,	/// std::pow(2, 1);
	fracDim		= 0x04,	/// std::pow(2, 2);
	Hilbert		= 0x08,	/// std::pow(2, 3);
	wavelet		= 0x10,	/// std::pow(2, 4);
	Hjorth		= 0x20,	/// std::pow(2, 5);
	logFFT		= 0x40	/// std::pow(2, 6);
};
const int allFeaturesInt = std::pow(2, 7) - 1;
inline std::vector<feature> getFeatures(int in)
{
	std::vector<feature> res{};
	for(int i = 0; i < 7; ++i)
	{
		if((1 << i) & in)
		{
			res.push_back(static_cast<feature>( 1 << i ));
		}
	}
	return res;
}
const std::vector<feature> allFeatures{getFeatures(allFeaturesInt)};

enum class outputStyle {Line, Table};
enum class outputSeq {ByChans, ByFilters};
enum class initialCut {first30, second30, first60, none};
using filterFreqs = std::pair<double, double>;
enum class filter
{
	none,		/// = std::pair<double, double>{0, 0},
	interest,	/// = std::pair<double, double>{2, 20},
//	delta,		/// = std::pair<double, double>{0, 2},
//	delta_low,	/// = std::pair<double, double>{0, 1},
//	delta_high,	/// = std::pair<double, double>{1, 2},
	theta,		/// = std::pair<double, double>{4, 8},
	theta_2,	/// = std::pair<double, double>{4, 7},
	theta_low,	/// = std::pair<double, double>{4, 6},
	theta_high,	/// = std::pair<double, double>{6, 8},
	alpha,		/// = std::pair<double, double>{8, 13},
	alpha_low,	/// = std::pair<double, double>{8, 10},
	alpha_high,	/// = std::pair<double, double>{10, 13},
//	beta,		/// = std::pair<double, double>{13, 30},
	beta_low,	/// = std::pair<double, double>{13, 18},
//	beta_high,	/// = std::pair<double, double>{18, 30},
};
const std::unordered_map<filter, filterFreqs> filtFreqs
{
	{filter::none,			{0, 0}},
	{filter::interest,		{2, 20}},
//	{filter::delta,			{0, 2}},
//	{filter::delta_low,		{0, 1}},
//	{filter::delta_high,	{1, 2}},
	{filter::theta,			{4, 8}},
	{filter::theta_2,		{4, 7}},
	{filter::theta_low,		{4, 6}},
	{filter::theta_high,	{6, 8}},
	{filter::alpha,			{8, 13}},
	{filter::alpha_low,		{8, 10}},
	{filter::alpha_high,	{10, 13}},
//	{filter::beta,			{13, 30}},
	{filter::beta_low,		{13, 18}},
//	{filter::beta_high,		{0, 0}},
};
std::vector<filterFreqs> makeFilterVector(double start, double finish, double width);

/// Singleton
class AutosSettings
{
public:
	static AutosSettings & inst()
	{
		static AutosSettings pew{};
		return pew;
	}

	void setOutputStyle(outputStyle in)			{ this->outStyle = in; }
	auto getOutputStyle() const					{ return this->outStyle; }

	void setOutputSequence(outputSeq in)		{ this->outSeq = in; }
	auto getOutputSequence() const				{ return this->outSeq; }

	void setInitialCut(initialCut in)			{ this->initCut = in; }
	auto getInitialCut() const					{ return this->initCut; }

	void setAutosMask(int in)					{ this->autosMask = in; }
	auto getAutosMask() const					{ return this->autosMask; }
	std::vector<feature> getAutosMaskArray() const;

	void setChansProc(const std::vector<QString> & in){ this->channelsToProcess = in; }
	auto getChansProc() const					{ return this->channelsToProcess; }

	void setFeatureFilter(feature in, const std::vector<filterFreqs> & f)
	{
		filtersSets[in] = f;
	}

	const std::vector<filterFreqs> & getFilter(feature in) const
	{
		return filtersSets.at(in);
	}
private:
	AutosSettings()
	{
		/// default filters
		filtersSets[feature::fft]			= makeFilterVector(2, 20, 1);
		filtersSets[feature::alphaPeak]		= {filtFreqs.at(filter::none)};
		filtersSets[feature::fracDim]		= {filtFreqs.at(filter::none)};
		filtersSets[feature::Hilbert]		= {filtFreqs.at(filter::none),
											   filtFreqs.at(filter::alpha)};
		filtersSets[feature::wavelet]		= {filtFreqs.at(filter::none)};
		filtersSets[feature::Hjorth]		= {filtFreqs.at(filter::none)};
		filtersSets[feature::logFFT]		= makeFilterVector(2, 20, 1);

		autosMask = allFeaturesInt
			#if !WAVELET_MATLAB
					& ~feature::wavelet
			#endif
					;
	}
//	AutosSettings(AutosSettings && other)=delete;
	AutosSettings(const AutosSettings & other)=delete;
	AutosSettings & operator=(const AutosSettings &)=delete;

private:
	std::unordered_map<feature, std::vector<filterFreqs>> filtersSets;
	std::vector<QString> channelsToProcess{};

	initialCut initCut{initialCut::none};
	outputStyle outStyle{outputStyle::Line};
	outputSeq outSeq{outputSeq::ByChans};
	int autosMask{};
};

#define AUT_SETS autos::AutosSettings::inst()


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
	std::make_tuple(feature::fft,	"spectre",	autos::countFFT,		18 * 19),
			std::make_tuple(feature::alphaPeak,"alpha",	autos::countAlphaPeak,	1 * 19),
			std::make_tuple(feature::fracDim,	"fracDim",	autos::countFracDim,	1 * 19),
			std::make_tuple(feature::Hilbert,	"Hilbert",	autos::countHilbert,	2 * 2 * 19),	/// 2 * 3 * 19 Xenia
			std::make_tuple(feature::wavelet,	"wavelet",	autos::countWavelet,	1 * 19 * 19),	/// 3 is a number of wavelet funcs
			std::make_tuple(feature::Hjorth,	"Hjorth",	autos::countHjorth,		2 * 19),		/// mobility and complexity
			std::make_tuple(feature::logFFT,	"logFFT",	autos::countLogFFT,		18 * 19)

};
QString featToStr(feature in);
int getFileLength(int in);




/// adhoc
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
void countAllFeatures(const matrix & inData,
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
void makeLabelsFile(const std::vector<QString> & chans,
					const QString & outFilePath,
					const std::vector<QString> & usedMarkers,
					const QString & sep);


/// main function
void calculateFeatures(const QString & pathWithEdfs,
					 const std::vector<QString> & channs,
					 const QString & outPath);

void elenaCalculation(const QString & realsPath, const QString & outTableDir);

/// quite useful general functions
void EdfsToFolders(const QString & inPath);
void ProcessByFolders(const QString & inPath,
					  const QString & outPath,
					  const std::vector<QString> & channs,
					  const std::vector<QString> & markers);
void ProcessByGroups(const QString & inPath,
						const QString & outPath,
						const std::vector<QString> & channs,
						const std::vector<QString> markers);
void ProcessAllInOneFolder(const QString & inPath,
						   const QString & outPath,
						   const std::vector<QString> & channs);


void ArrangeFilesToTable(const QString & inPath,
					   QString outTablePath = QString(),
					   bool writePeople = false,
					   const QString & auxFilter = QString());


/// some special variants
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

}

#endif /// AUTOS_H
