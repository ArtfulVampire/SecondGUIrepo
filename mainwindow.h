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
#include "autos.h"

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
	// other widgets
    void showNet();
    void showCountSpectra();
    void showCut();

	// parameters processing
    void setExpName();
    void setFileMarkers();
    void setNs();
    void changeNsLine(int);
    void setNsSlot(int);
    void matiCheckBoxSlot(int a);

	// in-window calculations, substantional actions
	// files
    void sliceAll();
	void sliceWindFromReal();
    void reduceChannelsSlot();
	void drawReals();
    void drawSpectraSlot();
    void drawDirSlot();
    void setEdfFileSlot();
	void cleanDirs();
	void cleanDirsCheckAll();
	void cleanDirsUncheckAll();
    void drawMapsSlot();
    void constructEDFSlot();
    void reduceChannelsEDFSlot();
	void cleanEdfFromEyesSlot();

	// mati
    void markerSaveEdf();
    void markerGetSlot();
    void markerSetSlot();
    void markerSetSecTime(int);
    void markerSetDecValueSlot();
    void markerSetBinValueSlot();
	void matiPreprocessingSlot();

	// signals
    void processEyes();
    void ICA();
    void refilterDataSlot();
    void rereferenceDataSlot();

	// other
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
    void sliceIlya(const QString &fileName, QString marker);
    void sliceIlya(int marker1, int marker2, QString marker);
    void sliceFromTo(int marker1, int marker2, QString marker);
    void sliceMati();
    void sliceMatiPieces(bool plainFlag = true);
    void sliceMatiSimple();


    void countSpectraSimple(int fftLen, int inSmooth = -1);
    void sliceOneByOne();
	void sliceOneByOneNew();
    void setEdfFile(const QString &filePath);
	void makeChanList(std::vector<int> & chanList);
	void cleanDirsAll(bool);


	void drawSpectra(const QString & inPath,
					 const QString & outPath);

	void refilterData(const double & lowFreq,
					  const double & highFreq,
					  const QString & newPath,
					  bool notch);
	void rereferenceData(const QString newRef,
						 const QString newPath);
	void reduceChannelsEDF(const QString & newFilePath);
	void constructEDF(const QString & newPath,
					  const QStringList & nameFilters = QStringList());


	// local autos
    void concatenateEDFs(QString inPath1, QString inPath2, QString outPath);
    void concatenateEDFs(QStringList inPath, QString outPath);
	void customFunc();

	void BaklushevDraw();
	void testNewClassifiers();
	void testSuccessive();


private:
    Ui::MainWindow *ui;

    std::ofstream generalLogStream;
    std::streambuf * coutBuf;

    edfFile globalEdf;
    int staSlice;

    matrix spocMixMatrix;
	std::valarray<double> spocWVector;

    bool autoProcessingFlag;
    bool stopFlag;
};

#endif // MAINWINDOW_H
