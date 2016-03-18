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

using namespace std;


namespace Ui {
    class Cut;
}

class Cut : public QWidget
{
    Q_OBJECT

public:
    explicit Cut();
    ~Cut();
    bool eventFilter(QObject *obj, QEvent *event);
    void setAutoProcessingFlag(bool);
    void matiAdjustLimits();

public slots:
    void createImage(QString dataFileName);
    void mousePressSlot(char btn__, int coord__);
    void next();
    void prev();
    void cut();
    void zero();
    void paint();
    void save();
    void rewrite();
    void cutEyesAll();
    void browse();
    void splitCut();


protected:
    void resizeEvent(QResizeEvent *);

signals:
    void openFile(QString);
    void buttonPressed(char btn, int coord);

private:
    Ui::Cut *ui;

    int rightLimit;
    int leftLimit;

    QString currentPicPath;
    QString currentFile;
    QString fileNumber;
    QString fileName;
    QPixmap currentPic;

    int addNum = 0; // for cut()

    QStringList lst;
    int currentNumber;

    matrix data3;
    int NumOfSlices, Eyes;
    int pointNumber;

    bool autoFlag;

    double coeff;      //for fft
    int timeShift;   //in time-bins
    double wndLength;  //in seconds
    int redCh, blueCh; // for drawing

    double zoomPrev, zoomCurr;
};

#endif // CUT_H
