#ifndef CUT_H
#define CUT_H

#include <functional>
#include <fstream>

#include <other/edffile.h>

#include <QWidget>
#include <QPixmap>
#include <QString>
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
//    void setFileType(const QString & dataFileName); /// deprecated

	/// read or modify data3
	void zero(int start, int end);
	void zeroChannel(int chanNum, int start, int end);
	void splitSemiSlot(int start, int end, bool addUndo = true);
	void split(int start, int end);
	void linearApprox(int lef, int rig, std::vector<int> chanList = std::vector<int>{});
	void copy(int lef, int rig);
	void paste(int start, const matrix & inData, bool addUndo = true);
	void undo();
	void saveAs(const QString & addToName);
	void setMarker(int offset, int newVal);
	bool checkBadRange(int start, int end, QString func); /// return true if too long

	void paintData(matrix & drawDataLoc);
	void repaintData(matrix & drawDataLoc, int sta, int fin);
	matrix makeDrawData();

	/// manual signal draw
	int getDrawnChannel(const QPoint & clickPos);
	void manualDraw(QPoint finP); /// copy parameter - can be changed
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
//	bool smartFindCheck(); /// return true if bad window

	std::vector<std::pair<int, QColor>> makeColouredChans();
	template<class...params> void logAction(const params &... par)
	{
		std::ofstream outStr;
		outStr.open((edfFil.getDirPath() + "/" +
					 edfFil.getExpNameShort() + "_cutLog.txt").toStdString(), std::ios_base::app);
		myWrite(outStr, par...);
		outStr.close();
	}
	void applyLog(const QString & logPath);
	void iitpLog(const QString & typ, int num = 2, const QString & add = QString()); /// to deprecate

public slots:
	void browseSlot();
	void openFile(const QString & dataFileName);
	void next();
	void prev();
	void saveSlot();
	void saveSubsecSlot();
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
	void xNormSlot();

	void mousePressSlot(Qt::MouseButton btn, int coord);
	void timesAndDiffSlot();

	void undoSlot();
	void copySlot();
	void cutSlot();
	void pasteSlot();
	void smartFindLearnSlot();
	void smartFindNextSlot();
	void smartFindPrevSlot();

	void marksToDrawSet();
	void drawSpectre();
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
		double mean{};
		double sigma{};
		int numChan{};
		int numParam{};
	};
	void smartFindSetFuncs();

private:
    Ui::Cut *ui;

	/// draw
	QPixmap currentPic{};
	int leftDrawLimit{}; /// in slices
	std::vector<int> zeroedChannels{};
	std::vector<std::tuple<QSpinBox*, QLineEdit*, QLineEdit*>> colouredWidgets;
	static const int scrollAreaGapX = 20;
	static const int scrollAreaGapY = 15;
	static const int picScaleGapY = 20;
	bool drawFlag{false}; /// to prevent many redraws in e.g. setValuesByEdf()
	std::vector<int> marksToDraw{};
	double prevXnorm{1.};

	/// manual signal draw
	bool manualDrawFlag{false};						/// manual signal draw
	QPoint manualDrawStart{};						/// manual signal draw
	matrix manualDrawDataBackup{};					/// manual signal draw

	/// edf and related widget-globals
	fileType myFileType{fileType::edf};
	edfFile edfFil{};
	bool fileOpened{false};
	matrix dataCutLocal{};				/// eegData of the whole file
	matrix dataCutLocalBackup{};		/// edfData backup for reset filtering
	matrix drawData{};					/// a piece of data to be drawn

	/// next/prev
	QStringList filesList{};
	QStringList::iterator fileListIter{};
	int addNum = 0; /// for cut() winds

	/// markers
	static constexpr double findNextGap = 0.5; /// sec

	/// copy, split, paste, undo
	matrix copyData{};
	std::vector<matrix> undoData{};
	std::vector<std::function<void(void)>> undoActions{};

	/// for auto lookup for bad points
	const int smartFindNumCh = 19;
	const int smartFindWindLen{128}; /// ~0.5 sec
	std::vector<matrix> smartFindLearnData{}; /// to find bad points

	std::vector<std::vector<thrParam>> smartFindThresholds{}; /// [channel][param]
	std::vector<std::vector<std::vector<double>>> smartFindWindParams{}; /// [windNum][channel][param]

	/// to struct?
	std::vector<std::function<double(const std::valarray<double> &)>> smartFindFuncs{};
	std::vector<QString> paramNames{};
	std::vector<double> paramSigmaThreshold{};
	std::vector<double> paramAbsThreshold{};
};

#endif /// CUT_H
