#ifndef EDFFILE_FB_H
#define EDFFILE_FB_H
#include <other/edffile.h>
#include <classifier/classifier.h>

namespace fb
{

enum class taskType : int {spat = 0, verb = 1, rest = 2};
taskType & operator ++(taskType & in); /// prefix
taskType operator ++(taskType & in, int); /// postfix

enum class fileNum  : int {first = 0, third = 1, second = 2};


/// bit structure is used in fb::isGoodAns
enum class ansType : int {skip		= 0b0001,											/// 1
						  correct	= 0b0010,											/// 2
						  notwrong	= ansType::correct | ansType::skip,					/// 3
						  wrong		= 0b0100,											/// 4
						  bad		= ansType::wrong | ansType::skip,					/// 5
						  answrd	= ansType::correct | ansType::wrong,				/// 6
						  all		= ansType::correct | ansType::wrong | ansType::skip	/// 7
						 };
QString toStr(taskType in);
QString toStr(fileNum in);
QString toStr(ansType in);

bool isGoodAns(ansType real, ansType expected);

/// omg omg omg omg omg omg omg omg omg omg omg omg omg omg omg omg omg omg
const int numOfClasses = 3;

class FBedf : public edfFile
{
private:
	/// really should use structs (matrix data, matrix spectre(full), taskType typ, ansType ans)
	///

	/// [type][numReal] = realMatrix
	std::vector<std::vector<matrix>> realsSignals; /// w/o markers

	/// [type][numReal] = startBin
	std::vector<std::vector<int>> realsStarts;

	/// [type][numOfReal] = spectre[channel][freq] (whole frequency range)
	std::vector<std::vector<matrix>> realsSpectra; /// with empty spectra for short reals

	/// [type][numReal] = length in seconds
	std::vector<std::valarray<double>> solvTime; /// compose with spec function

	/// [type][numReal]
	std::vector<std::vector<ansType>> ans;		/// rest always correct

	/// [numReal]
	std::vector<ansType> ansInRow;

	/// [numWind] = spectre by rows (already in leftFreq-rightFreq range)
	/// no empty spectra (look remakeWindows)
	matrix windSpectra;

	/// [numWind]
	std::vector<taskType> windTypes;

	/// [numWind]
	std::vector<ansType> windAns;

	/// [numWind]
	std::vector<int> windStarts;

public:
	static const std::vector<int> chansToProcess;
	static const std::vector<int> chansToZero;

	static const int numTasks = 40;
	static constexpr double solveThres = 40.;			/// 40 sec for a task

	static const int windLen = 1024;					/// may be changed to 1000
	static const int windFftLen = 1024;	/// smLib::fftL(windLen)
	static constexpr double realFftLen = 4096.;
	static constexpr double spStep = 250. / 4096.;
	static constexpr double leftFreq = 5.;
	static constexpr double rightFreq = 20.;

	/// ~static but srate is not
	int getLeftLimWind() const { static int leftLim{-1}; return (leftLim == -1)
				? fftLimit(leftFreq, srate, windFftLen) : leftLim; }
	int getRightLimWind() const { static int rightLim{-1}; return (rightLim == -1)
				? fftLimit(rightFreq, srate, windFftLen) + 1 : rightLim; }
	int getSpLenWind() const { return getRightLimWind() - getLeftLimWind(); }

	int getLeftLim() const { static int leftLim{-1}; return (leftLim == -1)
				? fftLimit(leftFreq, srate, realFftLen) : leftLim; }
	int getRightLim() const { static int rightLim{-1}; return (rightLim == -1)
				? fftLimit(rightFreq, srate, realFftLen) + 1 : rightLim; }
	int getSpLen() const { return getRightLim() - getLeftLim(); }

private:
	static constexpr double leftAlpha = 7.5;
	static constexpr double rightAlpha = 14;
	int individualAlphaPeakIndexWind() const;
	int individualAlphaPeakIndexReal() const;

	bool isGoodAns(taskType typ, int numReal, ansType expected) const;

public:
	/// solvTime? ans spectre inside
	FBedf() {}
	FBedf(const QString & edfPath,
		  const QString & ansPath,
		  double overlapPart,
		  int numSkipStartWinds);

	FBedf(const FBedf & other)=default;			/// used in Net::innerClassHistogram
	FBedf & operator=(FBedf && other)=default;	/// used in FeedbackClass constructor
	FBedf & operator=(const FBedf & other)=default;
	operator bool() const	{ return isGood; }

	void remakeWindows(int windStep, int numSkipStartWinds);

	ClassifierData prepareClDataWinds(bool reduce);
	double partOfCleanedWinds();

	/// complex calculations
	double spectreDispersion(taskType typ, ansType howSolved) const;
	double spectreDispersionWinds(taskType typ, ansType howSolved) const;
	double distSpec(taskType type1, taskType type2, ansType howSolved);
	QPixmap kdeForSolvTime(taskType typ) const;
	QPixmap verbShortLong(double thres) const;		/// spectra of short and long anagramms
	QPixmap rightWrongSpec(taskType typ) const;
	Classifier::avType classifyReals(bool alphaFlag) const;
	Classifier::avType classifyWinds(bool alphaFlag) const;
	matrix backgroundCompare(taskType typ, ansType howSolved) const;
	double insightPartOfAll(double thres) const;
	double insightPartOfSolved(double thres) const;
	void calculateICA() const;


	/// get interface
	int getNum(taskType typ, ansType howSolved) const;
	int getNumInsights(double thres) const;
	ansType getAns(int i) const { return ansInRow[i]; }
	std::valarray<double> getTimes(taskType typ, ansType howSolved) const;

	const matrix & getWindSpectra() const						{ return windSpectra; }
	const std::vector<taskType> & getWindTypes() const			{ return windTypes; }
	const std::vector<int> & getWindStarts() const				{ return windStarts; }
	const std::valarray<double> & getWindSpectra(int i) const	{ return windSpectra[i]; }
	taskType getWindTypes(int i) const							{ return windTypes[i]; }
	ansType getWindAns(int i) const								{ return windAns[i]; }
	int getWindStarts(int i) const								{ return windStarts[i]; }


private:
	bool isGood{false};
	std::vector<double> freqs;
	std::vector<ansType> readAns(const QString & ansPath);
	/// remake and deprecate ??
	std::valarray<double> spectralRow(taskType typ, ansType howSolved, int chan, double freq) const;
};






/// feedback
/// includes all sessions FBedf files
class FeedbackClass
{
public:

	FeedbackClass() { isGood = false; }

	FeedbackClass(const QString & guyPath_,
				  const QString & guyName_,
				  const QString & postfix_);

	~FeedbackClass() {}
	void setOstream(std::ostream & in) { ostr = &in; } /// DANGER ostr can overlive in


	void writeStat();
	void writeFile();

	/// to make
	void writeDists(ansType howSolved = ansType::all);				/// 1st: 0-1, 0-2, 1-2, 2nd: 0-1, 0-2, 1-2
	void writeDispersions(ansType howSolved = ansType::all);		/// 1st: 0, 1, 2, 2nd: 0, 1, 2
	void writeKDEs(const QString & prePath);
	void writeShortLongs(const QString & prePath);
	void writeRightWrong(const QString & prePath);
	void writeClass(bool aplhaOnly = false);
	void writeSuccessive();
	void writeBackgroundCompare(taskType typ, ansType howSolved);


	void remakeWindows(fileNum num, double overlapPart);
	ClassifierData prepareClDataWinds(fileNum num, bool reduce);
	void calculateICAs();

	void writeSuccessive3();
	void writePartOfCleaned();
	void writeLearnedPatterns();

	operator bool() const	{ return isGood; }

	const FBedf & getFile(int num) const				{ return files[num]; }
	const FBedf & getFile(fb::fileNum fNum) const		{ return files[static_cast<int>(fNum)]; }

private:
	std::ostream * ostr = &std::cout;
	void checkStatTimes(taskType in, ansType howSolved);
	void checkStatSolving(taskType typ, ansType howSolved);
	void checkStatInsight(double thres); /// onle verb but of solved and of all


private:
	FBedf files[3];

	QString guyPath;
	QString guyName;
	QString postfix;
	bool isGood{false};
};






} /// end namespace fb

#endif /// EDFFILE_FB_H
