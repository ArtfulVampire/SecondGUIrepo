#include <QtGui>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
//    int x[19] = {400, 1000, 100, 400,700, 1000, 1300, 100, 400, 700, 1000, 1300, 100, 400, 700, 1000, 1300, 400, 1000};
//    int y[19] = {300, 300, 600, 600, 600, 600, 600, 900, 900, 900, 900, 900, 1200, 1200, 1200, 1200, 1200, 1500, 1500};
//    char *lbl[19] = {"Fp1", "Fp2", "F7", "F3", "Fz", "F4", "F7", "T3", "C3", "Cz", "C4", "T4", "T5", "P3", "Pz", "P4", "T6", "O1", "O2"};
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
