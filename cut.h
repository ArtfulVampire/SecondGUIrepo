#ifndef CUT_H
#define CUT_H

//#include <QtWidgets>

#include <QWidget>
#include <QPixmap>
#include <iostream>
#include <QMouseEvent>
#include <QRegExp>
#include <QPainter>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <QDir>
#include <QString>
#include <cstdlib>
#include <QFileDialog>
#include <cmath>
#include <QMessageBox>
#include <QScrollBar>
#include "mainwindow.h"
#include "edffile.h"
#include <functional>

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
    void setAutoProcessingFlag(bool);
    void matiAdjustLimits();

private:
    void setFileType(const QString & dataFileName);
	void zero(int start, int end);
	void split(int start, int end, bool addUndo = true);
	void paste(int start, const matrix & inData, bool addUndo = true);
	void setMarker(int inVal);
	void drawSamples();


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
	void set1MarkerSlot();
	void set2MarkerSlot();



protected:
    void resizeEvent(QResizeEvent *);

signals:
    void openFile(QString);
    void buttonPressed(char btn, int coord);

private:
    Ui::Cut *ui;

	bool autoFlag;


    fileType myFileType{fileType::real};
    int leftDrawLimit; /// in slices
	int rightDrawLimit{0}; /// in slices
	edfFile edfFil;
	double currFreq{250};

    QStringList lst;
    QString currentFile;
	int currentNumber; /// in lst
	int addNum = 0; // for cut() winds

    QPixmap currentPic;
    int redCh, blueCh; // for drawing
    int rightLimit; /// in slices
    int leftLimit; /// in slices

	matrix data3{};

	matrix copyData{};
	std::vector<matrix> undoData;
	std::function<void(void)> undoAction;
	std::vector<std::function<void(void)>> undos;


};

#endif // CUT_H
