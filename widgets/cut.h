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
#include <QSpinBox>
#include <QLineEdit>

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
	bool checkBadRange(int start, int end, QString func); // return true if too long

	void paintData(matrix & drawDataLoc);
	void repaintData(matrix & drawDataLoc, int sta, int fin);
	matrix makeDrawData();

	/// manual signal draw
	int getDrawedChannel(QMouseEvent * clickEvent);
	void manualDraw(QMouseEvent * mouseMoveEvent);
	void manualDrawAddUndo();

	void colorSpinSlot(QSpinBox * spin, QLineEdit * lin);
	double normCoeff();
	void paintLimits();
	void paintMarkers(const matrix & drawDataLoc);
	void setValuesByEdf();
	void resetLimits();
	void showDerivatives();
	void smartFindCountParams();
	void findNextMark(int mark);
	void findPrevMark(double mark);
	void smartFindFind(bool forward);
	void smartFindShowValues();
//	bool smartFindCheck(); // return true if bad window

//	void drawSamples();
	std::vector<std::pair<int, QColor>> makeColouredChans();
	template<class...params> void logAction(const params &... par);
	void applyLog(const QString & logPath);
	void iitpLog(const QString & typ, int num = 2, const QString & add = QString()); // to deprecate

public slots:
	void browseSlot();
	void openFile(const QString & dataFileName);
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
	void linearApproxSlot();
	void subtractMeansSlot();
	void subtractMeanFrameSlot();
	void setMarkerSlot(bool);

	void mousePressSlot(Qt::MouseButton btn, int coord);
	void timesAndDiffSlot();

	void undoSlot();
	void copySlot();
	void cutSlot();
	void pasteSlot();
	void smartFindLearnSlot();
	void smartFindNextSlot();
	void smartFindPrevSlot();

	void paint();
	void forwardStepSlot();
    void backwardStepSlot();
    void forwardFrameSlot();
	void backwardFrameSlot();

	void refilterFrameSlot();
	void refilterAllSlot();
	void refilterResetSlot();

	/// iitp
	void iitpAutoCorrSlot();
	void iitpAutoJumpSlot();
	void iitpManualSlot();
	void saveNewNumSlot();
	void rectifyEmgSlot();

protected:
    void resizeEvent(QResizeEvent *);

private:
	struct thrParam
	{
//		QString name{};
		double mean;
		double sigma;
		int numChan;
		int numParam;
	};
	void smartFindSetFuncs();

private:
    Ui::Cut *ui;

	/// draw
	QPixmap currentPic{};
	int leftDrawLimit; /// in slices	
	std::vector<int> zeroedChannels{};
	std::vector<std::tuple<QSpinBox*, QLineEdit*, QLineEdit*>> colouredWidgets;
	static const int scrollAreaGapX = 20;
	static const int scrollAreaGapY = 15;
	static const int picScaleGapY = 20;
	bool drawFlag{false}; /// to prevent many redraws in e.g. setValuesByEdf()

	bool manualDrawFlag{false};						/// manual signal draw
	QPoint manualDrawStart{};						/// manual signal draw
	matrix manualDrawDataBackup;					/// manual signal draw

	/// edf and related widget-globals
	fileType myFileType{fileType::edf}; /// to deprecate, leave edf only
	edfFile edfFil{};
	bool fileOpened{false};
	matrix dataCutLocal{};
	matrix dataCutLocalBackup{}; /// for reset filtering
	matrix drawData{};

	/// next/prev
	QStringList filesList;
	QStringList::iterator fileListIter{};
	QString currentFile; /// deprecate to edfFile filePath (as fileType::real will be deprecated)
	int addNum = 0; // for cut() winds

	/// copy, split, paste, undo
	matrix copyData{};
	std::vector<matrix> undoData;
	std::vector<std::function<void(void)>> undoActions;


	/// for auto lookup for bad points
	const int smartFindNumCh = 19;
	const int smartFindWindLen = std::pow(2, 7); // = 128 = 0.5 sec
	std::vector<matrix> smartFindLearnData{}; /// to find bad points

	std::vector<std::vector<thrParam>> smartFindThresholds{}; /// [channel][param]
	std::vector<std::vector<std::vector<double>>> smartFindWindParams{}; /// [windNum][channel][param]

	/// to struct?
	std::vector<std::function<double(const std::valarray<double> &)>> smartFindFuncs{};
	std::vector<QString> paramNames{};
	std::vector<double> paramSigmaThreshold{};
	std::vector<double> paramAbsThreshold{};
};

#endif // CUT_H
