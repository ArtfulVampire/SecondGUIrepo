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
#include <myLib/qtlib.h>

#include <QMainWindow>
#include <QWidget>
#include <QString>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();
	void keyPressEvent(QKeyEvent *event) override;

public slots:
	/// other widgets
    void showNet();
    void showCountSpectra();
	void showReduce();
    void showCut();

	/// parameters processing
	void setFileMarkers();
	void changeRedNsLine(int a);

	/// in-window calculations, substantional actions
	/// files
	void sliceAll();
	void sliceWinds();
	void sliceJustWinds();

    void drawSpectraSlot();
    void drawDirSlot();
    void setEdfFileSlot();
	void cleanDirs();
	void drawMapsSlot();
	void reduceChannelsSlot();
	void reduceChannelsEDFSlot() const;
	void cleanEdfFromEyesSlot();
	void eegVegetSlot();

	/// mati
    void markerSaveEdf();
    void markerGetSlot();
    void markerSetSlot();
    void markerSetSecTime(int);
    void markerSetDecValueSlot();
    void markerSetBinValueSlot();
	void matiPreprocessingSlot();

	/// signals
    void processEyes();
    void ICA();
    void refilterDataSlot();
    void rereferenceDataSlot();
#if 0
	void rereferenceCARSlot();
#endif

	/// other
    void stop();


public:
	/// mati
	void sliceMati();
	void sliceMatiPieces();
    void sliceMatiSimple();

	void sliceBak(int marker1, int marker2, const QString & marker);
    void sliceOneByOne();
	void sliceOneByOneNew();
	void sliceElena();


	void setEdfFile(const QString & filePath);
	void readData();


	std::vector<int> makeChanList() const;
	void cleanDirsCheckAllBoxes(bool);


	void drawReals();
	void drawSpectra(const QString & inPath,
					 const QString & outPath);
	void drawWeights(const QString & wtsPath,
					 const QString & spectraPath,
					 const QString & outPath,
					 int fftLen);

#if 0
	void rereferenceData(reference newRef);
#endif


	void customFunc();

	/// autos
	void countSpectraSimple(int fftLen, int inSmooth = -1);
	void BaklushevDraw(const QString & workPath, const QString & edfName);
	void makeEvoked(const QString & edfPath,
					const std::valarray<double> & startMarkers,
					const QString & outPath);
	void makeEvoked(const QString & realsPath,
					const QStringList & fileFilters,
					const QString & outPath);
	void testNewClassifiers();
	void testSuccessive(const std::vector<double> & vals = std::vector<double>{});
	void testSuccessive2();
	void iitpPreproc();
	void iitpMainProc();
	void iitpNamesFix();
	void matiMainProc();

private:
    Ui::MainWindow *ui;
	bool stopFlag{false};

	/// for different outputs
	qtLib::MyTextStream outStream{qtLib::outputType::cout};

	void setOutputStream(qtLib::outputType in);

};

#endif /// MAINWINDOW_H
