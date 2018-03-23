#ifndef SPECTRE_H
#define SPECTRE_H

#include <other/defs.h>
#include <other/matrix.h>

#include <QWidget>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QtSvg>


#define SPECTRA_EXP_NAME_SPECIFICITY 0


namespace Ui {
    class Spectre;
}

class Spectre : public QWidget
{
    Q_OBJECT

public:
    explicit Spectre();
    ~Spectre();
    void setSmooth(int);
    bool eventFilter(QObject *obj, QEvent *event);
    void defaultState();
    void setPow(double);
    void setFftLength(int);

	void writeSpectra(const std::vector<int> & chanList = std::vector<int>{},
					  const double leftFreq = DEFS.getLeftFreq(),
					  const double rightFreq = DEFS.getRightFreq());

    void setInPath(const QString &);
    void setOutPath(const QString &);
	std::vector<int> countSpectra(std::vector<int> chanList = std::vector<int>{});

	/// deprecated
	bool countOneSpectre(const matrix & data2,
						 matrix & dataFFT);
    void cleanSpectra();


public slots:
    void countSpectraSlot();
    void inputDirSlot();
    void outputDirSlot();
    void setFftLengthSlot();

    void setLeft();
    void setRight();
    void drawWavelets();
    void compare();
    void psaSlot();
    void integrate();
    void center();

private:
    Ui::Spectre * ui;
    QString backupDirPath;

	/// replace in code
	QString defaultInPath = DEFS.dirPath()
							+ "/" + "Reals";
	QString defaultOutPath = DEFS.dirPath()
							 + "/" + "SpectraSmooth";

	QString defaultInPathW = DEFS.dirPath()
                             + "/winds"
                             + "/fromreal";
	QString defaultOutPathW = DEFS.dirPath()
                             + "/SpectraSmooth"
                             + "/winds";

    std::vector<std::pair<int, int>> rangeLimits;

    std::vector<matrix> dataFFT;
    std::vector<QString> fileNames;
};

#endif // SPECTRE_H
