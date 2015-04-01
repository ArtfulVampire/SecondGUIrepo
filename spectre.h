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
    explicit Spectre(QDir *dir_ = new QDir("/"), int ns_ = 20, QString ExpName_ = "TSL");
    ~Spectre();
    int countOneSpectre(double **data2, double ** dataFFT);
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
    void specifyRange();
    void center();

signals:
    void spValues(int, int, double);

private:
    Ui::Spectre *ui;
    QString ExpName;
    int ns;
    QDir *dir, *dirBC;
    int fftLength;
    QString helpString;
    QStringList lst, nameFilters;
    int left;
    int right;
    double spStep;
    int NumOfSlices;
    int Eyes;
    int helpInt;
    QFileDialog * browser1;
    QFileDialog * browser2;
    int spLength;
    QPixmap pic;
    QPainter *paint;
    bool ** boolArrrPirate;
    int ** rangeLimits;
    double norm;
    struct complex
    {
      double r;
      double i;
    }* spect;
    time_t duration;
    int chanNum;
    QString rangePicPath;
    FILE * file1;
    ifstream inStream;


};

#endif // SPECTRE_H
