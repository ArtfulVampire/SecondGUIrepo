#ifndef CUT_H
#define CUT_H

//#include <QtWidgets>

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
	void setMarker(int inVal);
	void drawSamples();
	std::vector<std::pair<int, QColor>> makeColouredChans();
	void paintLimits();
	void setValuesByEdf();
	void resetLimits();

	void iitpLog(const QString & typ, int num = 2, const QString & add = QString());
	void showDerivatives();


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

	void paint();
	void browse();
	void resizeWidget(double);

	void forwardStepSlot();
    void backwardStepSlot();
    void forwardFrameSlot();
    void backwardFrameSlot();

	void iitpAutoCorrSlot();
	void iitpAutoJumpSlot();
	void iitpManualSlot();
	void saveNewNumSlot();
	void set1MarkerSlot();
	void set2MarkerSlot();

	void color1SpinSlot();
	void color2SpinSlot();
	void color3SpinSlot();

protected:
    void resizeEvent(QResizeEvent *);

signals:
    void openFile(QString);
    void buttonPressed(char btn, int coord);

private:
    Ui::Cut *ui;

	QPixmap currentPic;
	int leftDrawLimit; /// in slices

	fileType myFileType{fileType::real};

	edfFile edfFil;
	matrix data3{};
	double currFreq{250};

    QStringList lst;
    QString currentFile;
	int currentNumber{-1}; /// in lst
	int addNum = 0; // for cut() winds

	matrix copyData{};
	std::vector<matrix> undoData;
	std::function<void(void)> undoAction;
	std::vector<std::function<void(void)>> undos;

};

#endif // CUT_H
