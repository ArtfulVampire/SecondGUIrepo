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
    bool eventFilter(QObject *obj, QEvent *event);
    void setAutoProcessingFlag(bool);

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
    void setNs(int);


protected:
    void resizeEvent(QResizeEvent *);

signals:
    void openFile(QString);
    void buttonPressed(char btn, int coord);

private:
    Ui::Cut *ui;
    int rightLimit;
    int leftLimit;
    QString currentPicPath, currentFile;
    QString fileNumber;
    QString fileName;
    QPixmap currentPic;
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
    QDir *dir;
    QStringList nameFilters;
    double coeff;      //for fft
    int timeShift;   //in time-bins
    double wndLength;  //in seconds
    int flagWnd;
    bool withMarkersFlag;

    double zoomPrev, zoomCurr;


};

#endif // CUT_H
