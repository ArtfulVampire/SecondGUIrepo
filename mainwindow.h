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
#include "library.h"
#include "coord.cpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <cmath>
#include <cstddef>

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
    void countSpectraShow();
    void cutShow();
    void makeCfgShow();
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
    void sliceWithMarkersSlot(int);

    //in-window calculations, substantional actions
    //files
    void sliceAll();
    void reduceChannelsSlot();
    void drawRealisations();
    void setEdfFileSlot();
    void cleanDirs();
    void sliceWindFromReal();
    void avTime();
    void drawMapsSlot();
    void makeDatFile();
    void constructEDFSlot();
    void reduceChannelsEDFSlot();
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


    void countSpectraSimple(int fftLen);
    void visualisation();
    void kernelest(const QString &str);
    void writeEdf(QString inFilePath, double ** components, QString outFilePath, int numSlices, QList<int> chanList = QList<int>());
    void sliceOneByOne();
    void sliceOneByOneNew(int numChanWrite);
    void drawMap(double ** matrixA, int num);
    void setEdfFile(const QString filePath);
    void transformEDF(QString inEdfPath, QString mapsPath, QString newEdfPath);
    void reduceChannelsEDF(QString newFilePath);
    void constructEDF(QString newPath);
    void writeCorrelationMatrix(QString edfPath, QString outPath);
    void ICsSequence(QString EDFica1, QString EDFica2, QString maps1Path, QString maps2Path, int mode = 0);
    double fileInnerClassification(QString workPath, QString fileName, QString cfgFileName = "16sec19ch", int NumOfPairs = 50, bool windows = false, int wndLen = 1000, int tShift = 125);
    double filesCrossClassification(QString workPath, QString fileName1, QString fileName2, QString cfgFileName = "16sec19ch", int NumOfRepeats = 50, double startCoeff = 2., bool windows = false, int wndLen = 1000, int tShift = 125);
    double filesDropComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats = 30, bool windows = false, int wndLen = 1000, int tShift = 125);
    double filesAddComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats = 30, bool windows = false, int wndLen = 1000, int tShift = 125);

    void countICAs(QString workPath, QString fileName, bool icaFlag = true, bool transformFlag = true, bool sequenceFlag = false, bool sumFlag = false); //to delete (listFileCrossClassification)

    void autoIcaAnalysis3();
    void autoIcaAnalysis4();
    void autoIcaAnalysis5();


private:
    Ui::MainWindow *ui;
    ofstream generalLogStream;
    streambuf *coutBuf;
    QVariant var;
    QDir *dir;
    ifstream inStream;
    ofstream outStream;
    int helpInt;
    double helpDouble;
    QString ExpName;
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
    bool redirectCoutFlag;
    double reduceCoefficient;
};

#endif // MAINWINDOW_H
