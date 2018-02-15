#ifndef EDFFILE_FB_H
#define EDFFILE_FB_H
#include <other/edffile.h>

namespace fb
{

enum class taskType : int {spat = 0, verb = 1};
enum class fileNum  : int {first = 0, third = 1};
enum class ansType  : int {skip = 0, right = 1, wrong = 2, answrd = 3, all = 4};

class FBedf : public edfFile
{
private:

	/// [type][numReal] = realMatrix
	std::vector<std::vector<matrix>> realsSignals;

	/// [type][numReal] = length
	std::vector<std::valarray<double>> solvTime;

	/// [type][numReal]
	std::vector<std::vector<int>> ans;

	/// [numReal]
	std::vector<int> ansRow;

	/// [type][numOfReal] = spectre
	std::vector<std::vector<matrix>> realsSpectra;

	static const std::vector<int> chansToProcess;
	static const int numTasks = 40;
	static constexpr double spStep = 250. / 4096.;
	static constexpr double leftFreq = 5.;
	static constexpr double rightFreq = 20.;
	static constexpr double solveThres = 40.;			/// 40 sec for a task

public:
	/// solvTime? ans spectre inside
	FBedf() {}
	FBedf(const QString & edfPath, const QString & ansPath);

	double spectreDispersion(taskType typ);
	double distSpec(taskType type1, taskType type2);
	double insightPartOfAll(double thres);
	double insightPartOfSolved(double thres);
	QPixmap kdeForSolvTime(taskType typ);
	QPixmap verbShortLong(double thres);		/// spectra of short and long anagramms

	///for FeedbackClass [taskType][taskNum][ansType, time]
	std::vector<std::vector<std::pair<int, double>>> solvTimesTransform();

	int getAns(int i) { return ansRow[i]; }

	/// FeedbackClass interface
	std::valarray<double> getTimes(taskType typ, ansType howSolved);
	int getNum(taskType typ, ansType howSolved);

private:
	std::vector<double> freqs;
	std::vector<int> readAns(const QString & ansPath);
	std::valarray<double> spectralRow(taskType type, int chan, double freq);
};




/// feedback
/// includes both 1st and 3rd files
class FeedbackClass
{
public:

	FeedbackClass() {}

	FeedbackClass(const QString & guyPath_,
				  const QString & guyName_,
				  const QString & postfix_);

	~FeedbackClass() {}


	void checkStat();
	void writeFile();

	void checkStatTimes(taskType in, ansType howSolved);
	void checkStatSolving(taskType typ, ansType howSolved);

	static const int numTasks = 40;

private:
	FBedf files[2];

private:
	QString guyPath;
	QString guyName;
	QString postfix;
};


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
void checkStatResults(const QString & guyPath, const QString & guyName);

void successiveNetPrecleanWinds(const QString & windsPath);


} // end namespace fb

#endif // EDFFILE_FB_H
