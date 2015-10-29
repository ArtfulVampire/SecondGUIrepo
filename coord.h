#ifndef COORD_H
#define COORD_H
#include <QString>
#include <QStringList>
#include <QDir>
#define MAXNS 50

namespace coords
{
//relative coords
    const double scale = 250./1600.;
    const double x[] = {0.21875, 0.59375,
                        0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                        0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                        0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                        0.21875, 0.59375}; //-0.5/16
    const double y[] = {0.1875, 0.1875,
                        0.375, 0.375, 0.375, 0.375, 0.375,
                        0.5625, 0.5625, 0.5625, 0.5625, 0.5625,
                        0.75, 0.75, 0.75, 0.75, 0.75,
                        0.9375, 0.9375};
    const char lbl[][19] = {"Fp1", "Fp2",
                            "F7", "F3", "Fz", "F4", "F8",
                            "T3", "C3", "Cz", "C4", "T4",
                            "T5", "P3", "Pz", "P4", "T6",
                            "O1", "O2"};
}

namespace def
{
    const bool matiFlag = true;
    const bool ntFlag = false;
    const bool withMarkersFlag = true; /// should check everywhere if changed to false

    const double freq = 250.;
    const double leftFreq = 5.;
    const double rightFreq = 20.;

    const bool wirteStartEndLong = false; //for slice

    extern int left;
    extern int right;
    extern double spStep;
    extern int spLength;
    extern QString ExpName;
    extern QDir * dir;
    extern int ns;
    extern int fftLength;
    extern QString cfgFileName;
    extern QStringList colours;
    extern QStringList fileMarkers;
    extern int numOfClasses;

//    extern int nsW;
    inline int nsWOM() {return def::ns - 1 * def::withMarkersFlag;}


//    Atanov

    const QString dataFolder = "/media/Files/Data/Mati";
    const QString GalyaFolder = "/media/Files/Data/Galya";
    const bool opencl = true;
    const bool openmp = true;


//    MATI
//    const int genNs = 20;
//    const QString dataFolder = "/media/Files/Data/Mati";
//    const int fftLength = 4096;
//    const int numOfClasses = 3;
//    const bool opencl = true;
//    const bool openmp = true;
//    const QString cfgFileName = "16sec19ch";

//    //Baklushev
//    const QString dataFolder = "c:/EEG";
//    const int fftLength = 2048;
//    const int left = 41;
//    const int right = 164;
//    const int numOfClasses = 2;
//    const bool opencl = 0;
//    const bool openmp = 0;
//    const QString cfgFileName = "8sec19ch";

//    //Ivanitsky
//    const QString dataFolder = "d:/AtanovMichael/Data";
//    const int fftLength = 4096;
//    const int left = 82;
//    const int right = 328;
//    const int numOfClasses = 3;
//    const bool opencl = 0;
//    const bool openmp = 0;
//    const QString cfgFileName = "16sec19ch";

}



#endif

