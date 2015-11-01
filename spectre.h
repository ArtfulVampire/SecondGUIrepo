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



public slots:
    void countSpectra();
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

signals:
    void spValues(int, int, double);

private:
    Ui::Spectre *ui;
    QString backupDirPath;

    int Eyes;
    QFileDialog * browser1;
    QFileDialog * browser2;

    bool ** boolArrrPirate;
    int ** rangeLimits;

    double norm;
    QButtonGroup * group1;

    struct complex
    {
      double r;
      double i;
    }* spect;

    int chanNum;
    QPixmap pic;
    QPainter * paint;
    QString rangePicPath;
};

#endif // SPECTRE_H
