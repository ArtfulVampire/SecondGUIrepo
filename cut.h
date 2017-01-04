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
    void setFileType(const QString & dataFileName);

public slots:
    void createImage(const QString & dataFileName);
    void mousePressSlot(char btn__, int coord__);
    void next();
    void prev();
    void cut();
    void zero();
	void undoSlot();
    void paint();
    void save();
    void rewrite();
    void cutEyesAll();
    void browse();
    void splitCut();
    void forwardStepSlot();
    void backwardStepSlot();
    void forwardFrameSlot();
    void backwardFrameSlot();
    void resizeWidget(double);
	void iitpAutoCorrSlot();
	void iitpAutoJumpSlot();
	void iitpManualSlot();
	void subtractMeansSlot();
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

	void drawSamples();


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
	int NumOfSlices{0};

	std::vector<matrix> undoData;
	std::function<void(void)> undoAction;
	std::vector<std::function<void(void)>> undos;


};

#endif // CUT_H
