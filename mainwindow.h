#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QtWidgets>
#include <QWidget>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDialogButtonBox>

#include "cut.h"
#include "cfg.h"
#include "net.h"
#include "spectre.h"
#include "makepa.h"
#include "eyes.h"
#include "wavelet.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <cmath>

#include "four1.h"
#include <cstdlib>
#include <unistd.h>
#include <QMessageBox>

using namespace std;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

public slots:
    void countSpectra();
    void setExpName();
    void sliceAll();
    void setEdfFile();   
    void cutShow();
    void makeShow();
    void makePaSlot();
    void netShow();
    void takeSpValues(int, int, double);
    void reduceChannels();
    void setNs();
    void drawRealisations();
    void eyesShow();
    void cleanDirs();
    void changeNsLine(int);
    void waveletCount();
    void setBoxMax(double);
    void makeDatFile();
    void autoProcessingFB();
    void setNs2(int);
    void sliceWindFromReal();
    void diffSmooth();
    void avTime();
    void ICA();
    void constructEDF();
    void drawMaps();
    void stop();
    void Bayes();



public:
    void autoProcessing();
    void autoWindowsProcessing();
    void makeDirs();
    void sliceGaps();
    void slice(int marker1, int marker2, QString marker);
    void sliceByNumberAfter(int marker1, int marker2, QString marker);
    void sliceBak(int marker1, int marker2, QString marker);
    void readData();
    void sliceWindow(int startSlice, int endSlice, int number, int marker);    
    void eyesFast();
    void reduceChannelsFast();
    void drawClassification();    
    void drawWeights();
    void drawEeg(int NumOfSlices_, double **dataD_, QString helpString_, int freq);
    void drawEeg(double **dataD_, double startTime, double endTime, QString helpString_, int freq);
    void sliceIlya(const QString &fileName, QString marker);
    void sliceIlya(int marker1, int marker2, QString marker);
    void sliceFromTo(int marker1, int marker2, QString marker);
    void visualisation();
    void kernelest(const QString &str);
    void writeEdf(FILE * edf, double ** components, QString fileName, int indepNum);
    void sliceOneByOne();
    void drawMap(double ** matrixA, int num);
    double fractalDimension(double * arr, int N, QString picPath);


private:
    Ui::MainWindow *ui;
    time_t duration;
    QVariant var;
    FILE *edf;
    QDir *dir;
    QString helpString;
    QString ExpName;
    QStringList lst, nameFilters;
//    QSignalMapper *signalMapper;
    int ndr, ddr, ns, nsBackup;
    int *nr;
    int NumOfSlices, eyes;
    double ** data;
    QPixmap pic;
    QPainter *paint;
    int spLength, left, right, offset;
    int timeShift;
    int wndLength;
    int staSlice;
    int NumOfEdf;
    int helpInt;
    bool autoProcessingFlag;
    bool stopFlag;
    double spStep;
};

#endif // MAINWINDOW_H
