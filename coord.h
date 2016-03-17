#ifndef COORD_H
#define COORD_H
#include <QString>
#include <QStringList>
#include <QDir>
#include <QColor>
#define MAXNS 50



namespace coords
{
//relative coords
    const double scale = 250./1600.;

    /// + 2 EOG
    const int numOfChan = 21; // for drawTemplate
    const std::vector<double> x {0.21875, 0.59375,
                                 0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                                 0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                                 0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                                 0.21875, 0.59375,
                                 0.03125, 0.78125};
    const std::vector<double> y {0.1875, 0.1875,
                                 0.375, 0.375, 0.375, 0.375, 0.375,
                                 0.5625, 0.5625, 0.5625, 0.5625, 0.5625,
                                 0.75, 0.75, 0.75, 0.75, 0.75,
                                 0.9375, 0.9375,
                                 0.9375, 0.9375};
    const std::vector<const char *> lbl {"Fp1", "Fp2",
                                         "F7", "F3", "Fz", "F4", "F8",
                                         "T3", "C3", "Cz", "C4", "T4",
                                         "T5", "P3", "Pz", "P4", "T6",
                                         "O1", "O2",
                                         "EOG1", "EOG2"};
}

namespace suc
{
extern int numGoodNewLimit;
extern int learnSetStay;
extern double decayRate;
}



enum spectraGraphsNormalization {all = 0, each = 1};

namespace def
{
    const bool matiFlag = false;
    const bool ntFlag = true;
    const bool edfPlusFlag = false;

    const bool withMarkersFlag = true; /// should check everywhere if changed to false
    const bool OssadtchiFlag = false;

    const bool wirteStartEndLong = false; //for slice




    extern QString ExpName;
    extern QDir * dir;
    extern int ns;

    extern double freq;
    extern int fftLength;

    extern double leftFreq;
    extern double rightFreq;

    extern QString cfgFileName;
    extern QStringList fileMarkers;
    extern std::vector<QColor> colours;

    extern double drawNorm;
    extern spectraGraphsNormalization drawNormTyp;

    inline int nsWOM() {return def::ns - 1 * def::withMarkersFlag;}
    inline int numOfClasses() {return def::fileMarkers.length();}

    extern int right();
    extern int left();
    inline int spLength() {return def::right() - def::left();}
    inline double spStep() {return def::freq / def::fftLength;}

//    Atanov

//    const QString dataFolder = "/media/Files/Data/Feedback/SuccessClass";
//    const QString dataFolder = "D:/MichaelAtanov/Data";
    const QString dataFolder = "D:/";
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

inline int fftLimit(const double & inFreq,
                    const double & sampleFreq = def::freq,
                    const int & fftL = def::fftLength)
{
    return ceil(inFreq / sampleFreq * fftL - 0.5);
}



#endif

