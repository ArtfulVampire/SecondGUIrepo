#ifndef EYES_H
#define EYES_H

//#include <QtWidgets>
#include <QWidget>
#include <QDir>
#include <QFileDialog>
#include <QAbstractButton>

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include "library.h"

#include <cmath>
#include <QMessageBox>

using namespace std;

namespace Ui {
    class Eyes;
}

class Eyes : public QWidget
{
    Q_OBJECT

public:
    explicit Eyes(QDir *dir_ = new QDir("/"), int ns_=19);
    void setAutoProcessingFlag(bool);
    ~Eyes();

public slots:
    void eyesProcessing();
    void eyesClean();
    void setDir(QAbstractButton*);

signals:
    void setNsMain(int);

private:
    Ui::Eyes *ui;
    int ns;
    bool autoFlag;
    QDir *dir, *dirBC;
    QStringList lst;
    double **dataE;
};

#endif // EYES_H
