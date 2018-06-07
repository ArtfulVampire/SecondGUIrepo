#ifndef EDFFILE_FB_H
#define EDFFILE_FB_H
#include <other/edffile.h>
#include <classifier/classifier.h>

namespace fb
{

enum class taskType : int {spat = 0, verb = 1, rest = 2};
enum class fileNum  : int {first = 0, third = 1, second = 2};

/// bit structure is used in:
/// FBedf::getNum
/// FBedf::getTimes
/// FBedf::backgroundCompare
enum class ansType : int {skip		= 0b0001,					/// 1
						  correct	= 0b0010,					/// 2
						  wrong		= 0b0100,					/// 4
						  answrd	= correct | wrong,			/// 6
						  all		= correct | wrong | skip	/// 7
						 };

const int numOfClasses = 3;

class FBedf : public edfFile
{
private:

	/// [type][numReal] = realMatrix
	std::vector<std::vector<matrix>> realsSignals; /// w/o markers

	/// [type][numReal] = length in seconds
	std::vector<std::valarray<double>> solvTime;

	/// [type][numReal]
	std::vector<std::vector<ansType>> ans;

	/// [numReal]
	std::vector<ansType> ansInRow;

	/// [type][numOfReal] = spectre[channel][freq]
	std::vector<std::vector<matrix>> realsSpectra; /// with empty spectra for short reals

	/// [numWind] = spectre by rows
	matrix windSpectra;

	/// [numWind]
	std::vector<taskType> windTypes;

	/// [numWind]
	std::vector<ansType> windAns;

public:
	static const std::vector<int> chansToProcess;

	static const int numTasks = 40;
	static constexpr double solveThres = 40.;			/// 40 sec for a task

	static const int windLen = 1024;
	static const int windFftLen = smLib::fftL(windLen);
	static constexpr double fftLen = 4096.;
	static constexpr double spStep = 250. / 4096.;
	static constexpr double leftFreq = 5.;
	static constexpr double rightFreq = 20.;

public:
	/// solvTime? ans spectre inside
	FBedf() {}
	FBedf(const QString & edfPath, const QString & ansPath,
		  double overlapPart = 0.0,
		  int numSkipStartWinds = 0);
	FBedf(const FBedf & other)=default; /// used in Net::innerClassHistogram
	FBedf & operator=(FBedf && other)=default;	/// used in FeedbackClass constructor
	FBedf & operator=(const FBedf & other)=default;

	void remakeWindows(double overlapPart = 0.0, int numSkipStartWinds = 0);

	ClassifierData prepareClDataWinds(bool reduce);
	double partOfCleanedWinds();

	double spectreDispersion(taskType typ);
	double distSpec(taskType type1, taskType type2);

	double insightPartOfAll(double thres) const;
	double insightPartOfSolved(double thres) const;

	QPixmap kdeForSolvTime(taskType typ) const;
	QPixmap verbShortLong(double thres) const;		/// spectra of short and long anagramms
	QPixmap rightWrongSpec(taskType typ) const;
	Classifier::avType classifyReals() const;
	Classifier::avType classifyWinds() const;
	matrix backgroundCompare(taskType typ, ansType howSolved) const;

	/// get interface
	operator bool() const { return isGood; }
	std::valarray<double> getTimes(taskType typ, ansType howSolved) const;
	int getNum(taskType typ, ansType howSolved) const;
	ansType getAns(int i) const { return ansInRow[i]; }
	int getNumInsights(double thres) const;

	const matrix & getWindSpectra() const						{ return windSpectra; }
	const std::vector<taskType> & getWindTypes() const			{ return windTypes; }
	const std::valarray<double> & getWindSpectra(int i) const	{ return windSpectra[i]; }
	taskType getWindTypes(int i) const							{ return windTypes[i]; }
	ansType getWindAns(int i) const								{ return windAns[i]; }

	int getLeftLim() const { return fftLimit(fb::FBedf::leftFreq, srate, fb::FBedf::windFftLen); }
	int getRightLim() const { return fftLimit(fb::FBedf::rightFreq, srate, fb::FBedf::windFftLen); }

private:
	bool isGood{false};
	std::vector<double> freqs;
	std::vector<ansType> readAns(const QString & ansPath);
	std::valarray<double> spectralRow(taskType type, int chan, double freq); /// remake and deprecate
};




/// feedback
/// includes both 1st and 3rd files
class FeedbackClass
{
public:

	FeedbackClass() { isGood = false; }

	FeedbackClass(const QString & guyPath_,
				  const QString & guyName_,
				  const QString & postfix_);

	~FeedbackClass() {}


	void writeStat();
	void writeFile();

	/// to make
	void writeDists();				/// 1st: 0-1, 0-2, 1-2, 2nd: 0-1, 0-2, 1-2
	void writeDispersions();		/// 1st: 0, 1, 2, 2nd: 0, 1, 2
	void writeKDEs(const QString & prePath);
	void writeShortLongs(const QString & prePath);
	void writeRightWrong(const QString & prePath);
	void writeClass();
	void writeSuccessive();
	void writeBackgroundCompare(taskType typ, ansType howSolved);


	void remakeWindows(fileNum num, double overlapPart);
	ClassifierData prepareClDataWinds(fileNum num, bool reduce);

	void writeSuccessive3();
	void writePartOfCleaned();
	void writeLearnedPatterns();

	operator bool() { return isGood; }

private:
	void checkStatTimes(taskType in, ansType howSolved);
	void checkStatSolving(taskType typ, ansType howSolved);
	void checkStatInsight(double thres); /// onle verb but of solved and of all


private:
	FBedf files[3];

private:
	QString guyPath;
	QString guyName;
	QString postfix;
	bool isGood{false};
};




void coutAllFeatures(const QString & dear,
					 const std::vector<std::pair<QString, QString>> & guysList,
					 const QString & postfix);

void createAnsFiles(const QString & guyPath, QString guyName);
void checkMarkFBfinal(const QString & filePath);
void repairMarkersInNewFB(QString edfPath, int numSession);
void successiveNetPrecleanWinds(const QString & windsPath);



} // end namespace fb

#endif // EDFFILE_FB_H
