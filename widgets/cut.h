#ifndef CUT_H
#define CUT_H

#include <iostream>
#include <cmath>
#include <functional>

#include <other/edffile.h>

#include <QDir>
#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include <QRegExp>
#include <QPainter>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QShortcut>

namespace Ui {
    class Cut;
}

enum class fileType {edf, real};

class Cut : public QWidget
{
    Q_OBJECT

public:
    explicit Cut();
    ~Cut();
	bool eventFilter(QObject *obj, QEvent *event);
    void matiAdjustLimits();

private:
    void setFileType(const QString & dataFileName);

	/// read or modify data3
	void zero(int start, int end);
	void splitSemiSlot(int start, int end, bool addUndo = true);
	void split(int start, int end);
	void linearApprox(int lef, int rig, std::vector<int> chanList = std::vector<int>{});
	void copy(int lef, int rig);
	void paste(int start, const matrix & inData, bool addUndo = true);
	void undo();
	void saveAs(const QString & addToName);
	void setMarker(int offset, int newVal);

	void paintLimits();
	void setValuesByEdf();
	void resetLimits();
	void showDerivatives();
	void countThrParams();
	void findNextMark(int mark);
	void findPrevMark(double mark);

	void drawSamples();
	std::vector<std::pair<int, QColor>> makeColouredChans();
	template<class...params>
	void logAction(const params &... par);
	void applyLog(const QString & logPath);
	void iitpLog(const QString & typ, int num = 2, const QString & add = QString());

public slots:
	void browseSlot();
    void createImage(const QString & dataFileName);
	void next();
	void prev();

	void saveSlot();
	void saveSubsecSlot();
	void rewrite();

	void zeroSlot();
	void splitSlot();
	void zeroFromZeroSlot();
	void splitFromZeroSlot();
	void zeroTillEndSlot();
	void splitTillEndSlot();
	void cutPausesSlot();
	void mousePressSlot(char btn__, int coord__);
	void timesAndDiffSlot();

	void setMarkerSlot(bool);

	void undoSlot();
	void copySlot();
	void cutSlot();
	void pasteSlot();
	void linearApproxSlot();
	void toLearnSetSlot();
	void nextBadPointSlot();

	void paint();
	void resizeWidget(double);

	void forwardStepSlot();
    void backwardStepSlot();
    void forwardFrameSlot();
	void backwardFrameSlot();

	void iitpAutoCorrSlot();
	void iitpAutoJumpSlot();
	void iitpManualSlot();
	void saveNewNumSlot();

	void color1SpinSlot();
	void color2SpinSlot();
	void color3SpinSlot();

protected:
    void resizeEvent(QResizeEvent *);

signals:
    void openFile(QString);
    void buttonPressed(char btn, int coord);

private:
	struct thrParam
	{
//		QString name{};
		double mean;
		double sigma;
		int numChan;
		int numParam;
	};
	void setThrParamsFuncs();

private:
    Ui::Cut *ui;

	/// draw
	QPixmap currentPic{};
	int leftDrawLimit; /// in slices

	/// edf and related globals
	edfFile edfFil{};
	fileType myFileType{fileType::edf};
	bool fileOpened{false};
	matrix dataCutLocal{};
	double currFreq{250}; /// to deprecate

	/// next/prev
	QStringList filesList;
	QStringList::iterator fileListIter{};
	QString currentFile; /// deprecate to edfFile filePath
	int addNum = 0; // for cut() winds

	/// copy, split, paste, undo
	matrix copyData{};
	std::vector<matrix> undoData;
//	std::function<void(void)> undoAction;
	std::vector<std::function<void(void)>> undos;


	/// for auto lookup for bad points
	const int paramsChanNum = 19;
	const int paramsWindLen = std::pow(2, 8);
	std::vector<matrix> learnSet{}; /// to find bad points

	std::vector<std::vector<thrParam>> thrParams{}; /// [channel][param]
	std::vector<std::vector<std::vector<double>>> windParams{}; /// [windNum][channel][param]

	/// to struct?
	std::vector<std::function<double(const std::valarray<double> &)>> paramFuncs{};
	std::vector<QString> paramNames{};
	std::vector<double> paramSigmaThreshold{};

};

#endif // CUT_H
