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
#include <other/consts.h>
#include <other/defs.h>

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
	void sliceWinds();
	void sliceJustWinds();

    void drawSpectraSlot();
    void drawDirSlot();
    void setEdfFileSlot();
	void cleanDirs();
	void drawMapsSlot();
	void reduceChannelsSlot();
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
	void rereferenceCARSlot();

	// other
    void stop();


public:
	// mati
	void sliceMati();
    void sliceMatiPieces(bool plainFlag = true);
    void sliceMatiSimple();

	void sliceBak(int marker1, int marker2, QString marker);
    void sliceOneByOne();
	void sliceOneByOneNew();
	void sliceElena();


	void setEdfFile(const QString & filePath);
	void readData();


	std::vector<int> makeChanList();
	void cleanDirsCheckAllBoxes(bool);


	void drawReals();
	void drawSpectra(const QString & inPath,
					 const QString & outPath);

	void rereferenceData(const QString & newRef);


	void customFunc();

	void countSpectraSimple(int fftLen, int inSmooth = -1);
	void BaklushevDraw(const QString & workPath, const QString & edfName);
	void makeEvoked(const QString & edfPath,
					const std::valarray<double> & startMarkers,
					const QString outPath);
	void makeEvoked(const QString & realsPath,
					const QStringList & fileFilters,
					const QString outPath);
	void testNewClassifiers();
	void testSuccessive(const std::vector<double> & vals = std::vector<double>{});
	void testSuccessive2();

private:
    Ui::MainWindow *ui;

    std::ofstream generalLogStream;
	std::streambuf * stdOutBuf;

//	edfFile globalEdf;

    matrix spocMixMatrix;
	std::valarray<double> spocWVector;

    bool autoProcessingFlag;
    bool stopFlag;
};

#endif // MAINWINDOW_H
