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
#include "matrix.h"
#include "edffile.h"

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
    void showNet();
    void showCountSpectra();
    void showMakePa();
    void showMakeCfg();
    void showEyes();
    void showCut();

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
    void markerSaveEdf();
    void markerGetSlot();
    void markerSetSlot();
    void markerSetSecTime(int);
    void markerSetDecValueSlot();
    void markerSetBinValueSlot();
    void matiPreprocessingSlot();

    //signals
    void ICA();
    void waveletCount();
    void Bayes();
    void spoc();
    void hilbertCount();
    void refilterDataSlot();
    void rereferenceDataSlot();
    void transformReals(); //reads ICA map file and transforms all the realisations in the chosen folder (windows e.g.)

    //different automatizations
    void makeTestData();
    void diffSmooth();
    void diffPow();
    void icaClassTest();
    void throwIC();
    void randomDecomposition();
    void clustering();

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
    void sliceIlya(const QString &fileName, QString marker);
    void sliceIlya(int marker1, int marker2, QString marker);
    void sliceFromTo(int marker1, int marker2, QString marker);
    void sliceMati();
    void sliceMatiPieces(bool plainFlag = true);
    void sliceMatiSimple();

    void rereferenceData(QString newRef, QString newPath);

    void refilterData(double lowFreq, double highFreq, QString newPath);
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
    void constructEDF(QString newPath, QStringList nameFilters = QStringList());
    void concatenateEDFs(QString inPath1, QString inPath2, QString outPath);
    void concatenateEDFs(QStringList inPath, QString outPath);
    void writeCorrelationMatrix(QString edfPath, QString outPath);
    void ICsSequence(QString EDFica1, QString EDFica2, QString maps1Path, QString maps2Path, int mode = 0);
    double fileInnerClassification(QString workPath, QString fileName, QString cfgFileName = "16sec19ch", int NumOfPairs = 50, bool windows = false, int wndLen = 1000, int tShift = 125);
    double filesCrossClassification(QString workPath, QString fileName1, QString fileName2, QString cfgFileName = "16sec19ch", int NumOfRepeats = 50, double startCoeff = 2., bool windows = false, int wndLen = 1000, int tShift = 125);
    double filesDropComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats = 30, bool windows = false, int wndLen = 1000, int tShift = 125);
    double filesAddComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats = 30, bool windows = false, int wndLen = 1000, int tShift = 125);

    void countICAs(QString workPath, QString fileName, bool icaFlag = true, bool transformFlag = true, bool sequenceFlag = false, bool sumFlag = false); //to delete (listFileCrossClassification)
    void customFunc();

private:
    Ui::MainWindow *ui;
    edfFile globalEdf;
    ofstream generalLogStream;
    streambuf *coutBuf;
    QDir *dir;
    QString ExpName;
    int ndr, ddr, ns, nsBackup;
    int * nr;
    int NumOfSlices;
    double ** spocMixMatrix;
    double * spocWVector;
    char ** label;
    QPixmap pic;
    QPainter *paint;
    int spLength, left, right;
    double spStep;
    int staSlice;
    int NumOfEdf;
    bool autoProcessingFlag;
    bool stopFlag;
    bool withMarkersFlag;
    bool redirectCoutFlag;
    double reduceCoefficient;
};

#endif // MAINWINDOW_H
