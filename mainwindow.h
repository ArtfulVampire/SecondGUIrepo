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

#include "library.h"
#include "cut.h"
#include "cfg.h"
#include "net.h"
#include "spectre.h"
#include "makepa.h"
#include "eyes.h"
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
    void setFileMarkers();

    //parameters processing
    void setExpName();
    void setNs();
    void changeNsLine(int);
    void setBoxMax(double);
    void setNsSlot(int);
    void matiCheckBoxSlot(int a);

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
    void sliceGalya();

    void GalyaProcessing(const QString &procDirPath);
    void GalyaCut(const QString & path, QString outPath = QString());

    void rereferenceData(QString newRef, QString newPath);
    void makeChanList(vector<int> & chanList);

    void refilterData(double lowFreq, double highFreq, QString newPath);
    void countSpectraSimple(int fftLen, int inSmooth = -1);
    void visualisation();
    void kernelest(const QString &str);
//    void writeEdf(QString inFilePath, double ** components, QString outFilePath, int numSlices, QList<int> chanList = QList<int>());
    void sliceOneByOne();
    void sliceOneByOneNew();
    void drawMap(double ** matrixA, int num);
    void setEdfFile(const QString &filePath);
    void transformEdfMaps(const QString & inEdfPath,
                          const QString & mapsPath,
                          const QString & newEdfPath);
    void reduceChannelsEDF(const QString & newFilePath);
    void constructEDF(const QString & newPath,
                      const QStringList & nameFilters = QStringList());
    void concatenateEDFs(QString inPath1, QString inPath2, QString outPath);
    void concatenateEDFs(QStringList inPath, QString outPath);
    void writeCorrelationMatrix(QString edfPath, QString outPath);
    void ICsSequence(const QString & EDFica1,
                     const QString & EDFica2,
                     const int mode = 1,
                     QString maps1Path = QString(),
                     QString maps2Path = QString());
    void reorderIcaFile(const QString & icaPath,
                        vector<int> chanList,
                        QString icaOutPath = QString(),
                        QString mapsPath = QString(),
                        QString mapsOutPath = QString());

    // autos
    double innerClass(const QString & workPath,
                      const QString & fileName,
                      const int & fftLen = pow(2, 12),
                      const QString & mode = "N",
                      const int & NumOfPairs = 50,
                      const bool & windows = false,
                      const int & wndLen = 4,
                      const int & tShift = 0.5);
    double filesCrossClassification(QString workPath,
                                    QString fileName1,
                                    QString fileName2,
                                    QString cfgFileName = "16sec19ch",
                                    int NumOfRepeats = 35,
                                    double startCoeff = 2.,
                                    bool windows = false,
                                    int wndLen = 4,
                                    int tShift = 0.5);
    double filesDropComponents(QString workPath,
                               QString fileName1,
                               QString fileName2,
                               int NumOfRepeats = 30,
                               bool windows = false,
                               int wndLen = 4,
                               int tShift = 0.5);
    double filesAddComponentsCross(QString workPath,
                                   QString fileName1,
                                   QString fileName2,
                                   int NumOfRepeats,
                                   bool windows = false,
                                   int wndLen = 4,
                                   int tShift = 0.5);
    double filesAddComponentsInner(const QString &workPath,
                                   const QString &fileName,
                                   int NumOfRepeats,
                                   bool windows = false,
                                   int wndLen = 4,
                                   int tShift = 0.5);

    void countICAs(QString workPath,
                   QString fileName,
                   bool icaFlag = true,
                   bool transformFlag = true,
                   bool sequenceFlag = false,
                   bool sumFlag = false); //to delete (listFileCrossClassification)
    void customFunc();

private:
    Ui::MainWindow *ui;

    ofstream generalLogStream;
    streambuf *coutBuf;
    bool redirectCoutFlag;

    edfFile globalEdf;
    vector<QString> label;
    int staSlice;
    int NumOfEdf;

    double ** spocMixMatrix;
    double * spocWVector;

    bool autoProcessingFlag;
    bool stopFlag;
};

#endif // MAINWINDOW_H
