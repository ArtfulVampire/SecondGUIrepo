#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <myLib/qtlib.h>

#include <QMainWindow>
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
    void showCut();

	/// parameters processing
	void setFileMarkers();
	void changeRedNsLine(int a);
	void changeEogChannelsLine(int);

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
	void drawHeadSlot();
	void reduceChannelsSlot();
	void reduceChannelsEDFSlot() const;
	void cleanEdfFromEyesSlot();
	void eegVegetSlot();
	void reoEyeSlot();

	/// mati
    void markerSaveEdf();
    void markerGetSlot();
    void markerSetSlot();
    void markerSetSecTime(int);
    void markerSetDecValueSlot();
    void markerSetBinValueSlot();
	void matiPreprocessingSlot();

	/// signals ???
	std::pair<std::vector<int>, std::vector<int>> processEyes();
    void ICA();
	void refilterDataSlot();
	void rereferenceDataSlot();
	void rereferenceFolderSlot();
	void addRefSlot();
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
	void sliceBak2017();
    void sliceOneByOne();
	void sliceOneByOneNew();
	void sliceElena();
	void sliceElenaReo();
	void PausePiecesII();
	void PausePieces(bool a);


	void setEdfFile(const QString & filePath);
	void readData();


	std::vector<int> makeChanList(QString text = {}) const;
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
	void testSuccessive(const std::vector<double> & vals);
	void testSuccessive2();
	void iitpPreproc();
	void iitpMainProc();
	void iitpFixes();
	void matiMainProc();

private:
    Ui::MainWindow *ui;
	bool stopFlag{false};

	/// for different outputs
	qtLib::MyTextStream outStream{qtLib::outputType::cout};
	void setOutputStream(qtLib::outputType in);

};

#endif /// MAINWINDOW_H
