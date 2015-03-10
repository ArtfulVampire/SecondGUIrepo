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
    explicit Cut(QDir * dir_ = 0, int ns_ = 21, bool withMarkersFlag_ = false);
    ~Cut();
    void setLimits();
    void lnWnd16();
    bool eventFilter(QObject *obj, QEvent *event);
    void setAutoProcessingFlag(bool);

public slots:
    void createImage(QString str);
    void mousePressSlot(char btn__, int coord__);
    void next();
    void prev();
    void cut();
    void zero();
    void paint();
    void save();
    void rewrite();
    void lnWndAll();
    void paintDistr();
//    void drawLogistic();
    void cycleTimeShift();
    void cycleWndLength();
    void setNs(int);
    void cutEyesAll();
    void browse();
    void splitCut();
//    void adjustScrollArea();


protected:
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);

signals:
    void openFile(QString);
    void buttonPressed(char btn, int coord);


private:
    Ui::Cut *ui;
    int rightLimit;
    int leftLimit;
    QString currentPicPath, currentFile, helpString, tmp;
    QPixmap currentPic;
    QPainter *painter;
    QString fileNumber;
    QString fileName;
    int addNum;
    QStringList lst;
    int currentNumber;
    double ** data3;
    int ns;
    int NumOfSlices, Eyes;
    int pointNumber;
    int helpInt;
    bool autoFlag;
    FILE * file;
    FILE * weights;
    FILE * wnd1;
    FILE * wnd2;
    FILE * wnd3;
    FILE * f1;
    FILE * f2;
    FILE * f3;
    QDir *dir;
    QStringList nameFilters;
    double coeff;      //for fft
    int timeShift;   //in time-bins
    double wndLength;  //in seconds
    double **weight;
    int numOfWndSpat;
    int numOfWndVerb;
    int numOfWndIdle;
    int flagWnd;
    time_t duration;
    bool withMarkersFlag;

    double zoomPrev, zoomCurr;


};

#endif // CUT_H
