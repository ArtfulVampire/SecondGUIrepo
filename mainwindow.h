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

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <cmath>
#include <cstddef>

#include "library.h"
#include <unistd.h>
#include <QMessageBox>
#include <libxml2/libxml/parser.h>

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
    void setNs2(int);
    void sliceWindFromReal();
    void diffSmooth();
    void avTime();
    void ICA();
    void constructEDF();
    void drawMaps();
    void stop();
    void Bayes();
    void makeTestData();
    void spoc();
    void icaClassTest();


public:
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
    void sliceMati(int numChanWrite);
    void visualisation();
    void kernelest(const QString &str);
    void writeEdf(FILE * edf, double ** components, QString fileName, int numSlices);
    void sliceOneByOne();
    void sliceOneByOneNew(int numChanWrite);
    void drawMap(double ** matrixA, int num);


private:
    Ui::MainWindow *ui;
    QVariant var;
    FILE *edf;
    QDir *dir;
    QString helpString;
    int helpInt;
    double helpDouble;
    QString ExpName;
    QStringList lst, nameFilters;
    int ndr, ddr, ns, nsBackup;
    int *nr;
    int NumOfSlices, eyes;
    double ** data;
    double ** spocMixMatrix;
    double * spocWVector;
    char **label;
    QPixmap pic;
    QPainter *paint;
    int spLength, left, right, offset;
    int timeShift;
    int wndLength;
    int staSlice;
    int NumOfEdf;
    bool autoProcessingFlag;
    bool stopFlag;
    double spStep;
};

#endif // MAINWINDOW_H
