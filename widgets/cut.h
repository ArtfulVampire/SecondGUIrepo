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
	void zero(int start, int end);
	void split(int start, int end, bool addUndo = true);
	void paste(int start, const matrix & inData, bool addUndo = true);
	void saveAs(const QString & addToName);
	void setMarker(int inVal);
	void drawSamples();
	std::vector<std::pair<int, QColor>> makeColouredChans();
	void paintLimits();
	void setValuesByEdf();
	void resetLimits();

	void iitpLog(const QString & typ, int num = 2, const QString & add = QString());
	void showDerivatives();

	void countThrParams();



public slots:
    void createImage(const QString & dataFileName);
    void mousePressSlot(char btn__, int coord__);

    void next();
    void prev();	
	void zeroSlot();
	void splitSlot();
	void zeroFromZeroSlot();
	void splitFromZeroSlot();
	void zeroTillEndSlot();
	void splitTillEndSlot();
	void undoSlot();
	void copySlot();
	void cutSlot();
	void pasteSlot();
    void save();
	void saveSubsecSlot();
    void rewrite();
	void subtractMeansSlot();
	void linearApproxSlot();
	void toLearnSetSlot();
	void nextBadPointSlot();

	void paint();
	void browse();
	void resizeWidget(double);

	void forwardStepSlot();
    void backwardStepSlot();
    void forwardFrameSlot();
    void backwardFrameSlot();
	void findNextMark();
	void cutPausesSlot();

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

	QPixmap currentPic;
	int leftDrawLimit; /// in slices

	fileType myFileType{fileType::real};

	edfFile edfFil;
	matrix data3{};
	double currFreq{250}; /// to deprecate

	const int paramsChanNum = 19;
	const int paramsWindLen = std::pow(2, 8);
	std::vector<matrix> learnSet{}; /// to find bad points
	std::vector<std::function<double(const std::valarray<double> &)>> paramFuncs{};
	std::vector<QString> paramNames{};
	std::vector<std::vector<thrParam>> thrParams{}; /// [channel][param]
	std::vector<std::vector<std::vector<double>>> windParams{}; /// [windNum][channel][param]


    QStringList lst;
	QString currentFile; /// deprecate to edfFile filePath
	int currentNumber{-1}; /// in lst
	int addNum = 0; // for cut() winds

	matrix copyData{};
	std::vector<matrix> undoData;
	std::function<void(void)> undoAction;
	std::vector<std::function<void(void)>> undos;

};

#endif // CUT_H
