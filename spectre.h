#ifndef SPECTRE_H
#define SPECTRE_H

#include <QWidget>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QtSvg>
#include "mainwindow.h"

#include "coord.h"


namespace Ui {
    class Spectre;
}

class Spectre : public QWidget
{
    Q_OBJECT

public:
    explicit Spectre();
    ~Spectre();
    bool countOneSpectre(matrix & data2,
                         matrix & dataFFT);
    void setSmooth(int);
    bool eventFilter(QObject *obj, QEvent *event);
    int readFilePhase(double **data2, double ***dataPhase);
    void defaultState();
    void setPow(double);
    void setFftLength(int);
    void writeSpectra(const double leftFreq = def::leftFreq,
                      const double rightFreq = def::rightFreq,
                      const bool rangeLimitCheck = true);


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
    Ui::Spectre *ui;
    QString backupDirPath;

    vector<vector<int>> rangeLimits;
    QString rangePicPath;

    vector<matrix> dataFFT;
    vector<QString> fileNames;
};

#endif // SPECTRE_H
