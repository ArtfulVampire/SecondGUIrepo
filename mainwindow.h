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
//#include <libxml2/libxml/parser.h>

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
    //other widgets
    void countSpectra();
    void cutShow();
    void makeShow();
    void makePaShow();
    void eyesShow();
    void netShow();

    //parameters processing
    void setExpName();
    void takeSpValues(int, int, double);
    void setNs();
    void changeNsLine(int);
    void setBoxMax(double);
    void setNsSlot(int);

    //in-window calculations, substantional actions
    //files
    void sliceAll();
    void reduceChannels();
    void drawRealisations();
    void setEdfFile();
    void cleanDirs();
    void sliceWindFromReal();
    void avTime();
    void drawMaps();
    void makeDatFile();
    void constructEDF();
    //signals
    void ICA();
    void waveletCount();
    void Bayes();
    void spoc();
    void hilbertCount();
    void refilterData();
    void transformReals(); //reads ICA map file and transforms all the realisations in the chosen folder (windows e.g.)

    //different automatizations
    void makeTestData();
    void diffSmooth();
    void diffPow();
    void icaClassTest();
    void throwIC();
    void randomDecomposition();

    //other
    void stop();


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
    void writeEdf(FILE * edfIn, double ** components, QString fileName, int numSlices);
    void sliceOneByOne();
    void sliceOneByOneNew(int numChanWrite);
    void drawMap(double ** matrixA, int num);
    void setEdfFile(QString filePath);
    void autoIcaAnalysis();


private:
    Ui::MainWindow *ui;
    QVariant var;
    FILE *edf;
    QDir *dir;
    QString helpString;
    ifstream inStream;
    ofstream outStream;
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
    bool withMarkersFlag;
    double reduceCoefficient;
};

#endif // MAINWINDOW_H
