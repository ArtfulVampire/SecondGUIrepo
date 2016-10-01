#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "library.h"
#include "cut.h"
#include "net.h"
#include "spectre.h"
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
    void showCut();

    //parameters processing
    void setExpName();
    void setFileMarkers();
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
    void drawSpectraSlot();
    void drawDirSlot();
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
    void cleanEdfFromEyesSlot();

    //signals
    void processEyes();
    void ICA();
    void waveletCount();
    void Bayes();
    void spoc();
    void hilbertCount();
    void refilterDataSlot();
    void rereferenceDataSlot();
    void transformReals(); //reads ICA map file and transforms all the realisations in the chosen folder (windows e.g.)

    //different automatizations
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

    matrix makeTestData(const QString & outPath);

    void countSpectraFeatures(const QString & filePath,
                              const int numChan,
                              const QString & outPath);
    void countChaosFeatures(const QString & filePath,
                              const int numChan,
                              const QString & outPath);
    void GalyaProcessing(const QString &procDirPath,
                         const int numChan = 31,
                         QString outPath = QString());


    void makeRightNumbers(const QString & dirPath,
                          int length = 3);
    void makeTableFromRows(const QString & work,
                           QString tablePath = QString(),
                           const QString & auxFilter = QString());


    void cutOneFile(const QString & filePath,
                    const int wndLen,
                    const QString & outPath);
    void GalyaCut(const QString & path,
                  const int wndLen = 16,
                  QString outPath = QString());
    void countEdfSpectra(const QString & inPath,
                         const QString & outPath,
                         int numChan,
                         int fftLength);
    void drawSpectra(const QString & inPath,
                     const QString & outPath);

    void rereferenceData(const QString newRef, const QString newPath);
    void makeChanList(std::vector<int> & chanList);

    void refilterData(const double & lowFreq,
                      const double & highFreq,
                      const QString & newPath,
                      bool notch);
    void countSpectraSimple(int fftLen, int inSmooth = -1);
    void visualisation();
    void kernelest(const QString &str);
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
    void ICsSequence(const QString & EDFica1,
                     const QString & EDFica2,
                     const int mode = 1,
                     QString maps1Path = QString(),
                     QString maps2Path = QString());
    void reorderIcaFile(const QString & icaPath,
                        std::vector<int> chanList,
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

    std::ofstream generalLogStream;
    std::streambuf * coutBuf;

    edfFile globalEdf;
    int staSlice;

    matrix spocMixMatrix;
    lineType spocWVector;

    bool autoProcessingFlag;
    bool stopFlag;
};

#endif // MAINWINDOW_H
