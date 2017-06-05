#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <cmath>
#include <string>

#include <widgets/cut.h>
#include <widgets/net.h>
#include <widgets/spectre.h>

#include <other/matrix.h>
#include <other/edffile.h>
#include <other/autos.h>

#include <QMainWindow>
#include <QWidget>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDialogButtonBox>
#include <QMessageBox>

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
	void setFileMarkers();
	void changeRedNsLine(int a);

	// in-window calculations, substantional actions
	// files
    void sliceAll();
	void sliceWindFromReal(); /// deprecated

	void sliceWinds();
    void reduceChannelsSlot();
	void drawReals();
    void drawSpectraSlot();
    void drawDirSlot();
    void setEdfFileSlot();
	void cleanDirs();
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
    void sliceBak(int marker1, int marker2, QString marker);
	void readData();
	void eyesFast();
	void sliceMati();
    void sliceMatiPieces(bool plainFlag = true);
    void sliceMatiSimple();


    void countSpectraSimple(int fftLen, int inSmooth = -1);
    void sliceOneByOne();
	void sliceOneByOneNew();
	void setEdfFile(const QString & filePath);
	void makeChanList(std::vector<int> & chanList);
	void cleanDirsAll(bool);


	void drawSpectra(const QString & inPath,
					 const QString & outPath);

	void refilterData(double lowFreq,
					  double highFreq,
					  const QString & newPath,
					  bool notch);
	void rereferenceData(const QString & newRef,
						 const QString & newPath);
	void reduceChannelsEDF(const QString & newFilePath);
	void constructEDF(const QString & newPath,
					  const QStringList & nameFilters = QStringList());


	// local autos
    void concatenateEDFs(QString inPath1, QString inPath2, QString outPath);
    void concatenateEDFs(QStringList inPath, QString outPath);
	void customFunc();

	void BaklushevDraw(const QString & workPath, const QString & edfName);
	void testNewClassifiers();
	void testSuccessive(const std::vector<double> & vals = std::vector<double>{});
	void testSuccessive2();


private:
    Ui::MainWindow *ui;

    std::ofstream generalLogStream;
	std::streambuf * stdOutBuf;

    edfFile globalEdf;
    int staSlice;

    matrix spocMixMatrix;
	std::valarray<double> spocWVector;

    bool autoProcessingFlag;
    bool stopFlag;
};

#endif // MAINWINDOW_H
