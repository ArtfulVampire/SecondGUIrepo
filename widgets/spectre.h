#ifndef SPECTRE_H
#define SPECTRE_H

#include <QWidget>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QFileDialog>
//#include <QMessageBox>
#include <QtSvg>
#include "mainwindow.h"

#include <other/coord.h>

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

	void writeSpectra(const double leftFreq = def::leftFreq,
					  const double rightFreq = def::rightFreq);

    void setInPath(const QString &);
    void setOutPath(const QString &);
    void countSpectra();
    bool countOneSpectre(matrix & data2,
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
    QString defaultInPath = def::dir->absolutePath()
                            + "/" +"Reals";
    QString defaultOutPath = def::dir->absolutePath()
                             + "/" +"SpectraSmooth";

    QString defaultInPathW = def::dir->absolutePath()
                             + "/winds"
                             + "/fromreal";
    QString defaultOutPathW = def::dir->absolutePath()
                             + "/SpectraSmooth"
                             + "/winds";

    std::vector<std::pair<int, int>> rangeLimits;

    std::vector<matrix> dataFFT;
    std::vector<QString> fileNames;
};

#endif // SPECTRE_H
