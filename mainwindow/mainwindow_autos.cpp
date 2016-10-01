#ifndef AUTOS_CPP
#define AUTOS_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace myLib;
using namespace smallLib;

bool mySort(std::vector<double> i,
            std::vector<double> j)
{
    return (i[0] < j[0]);
}

template <class InputIterator, class T>
bool contains (InputIterator first, InputIterator last, const T & val)
{
  while (first != last) {
    if (*first == val) return true;
    ++first;
  }
  return false;
}


void MainWindow::countSpectraSimple(int fftLen, int inSmooth)
{
    Spectre * sp = new Spectre();
    sp->setFftLength(fftLen);
    if(inSmooth >= 0)
    {
        sp->setSmooth(inSmooth);
    }
    sp->countSpectraSlot();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
}


void MainWindow::matiPreprocessingSlot()
{
    // for eeg+amod composition


    def::dir->cd(def::dataFolder); // "/media/Files/Data/Mati/"
    QStringList dirList = def::dir->entryList(QDir::Dirs|QDir::NoDotAndDotDot); // list of folders
//    dirList = {"ADA", "BSA", "FEV", "KMX", "NOS", "NVV", "PYV", "SDA", "SDV", "SIV"};
    bool flagCommaDot = false;
    bool flagAmodLogToEdf = false;
    bool flagSliceEdfBySessions = false;
    bool flagAppendAmodToEeg = false;
    bool flagMakeDiffMark = false;
    bool flagSliceSessionsToPieces = false;

//    flagCommaDot = true;
//    flagAmodLogToEdf = true;
//    flagSliceEdfBySessions = true;
//    flagAppendAmodToEeg = true;
    flagMakeDiffMark = true;
//    flagSliceSessionsToPieces = true;


    QString fileSuffix = "_w";
//    fileSuffix = "_rr_f";


    for(int dirNum = 0; dirNum < dirList.length(); ++dirNum)
    {
        if(!dirList[dirNum].contains("SDA")) continue;

        if(flagCommaDot) // change , to . in amod logFiles
        {
            ui->progressBar->setValue(0.);
            QTime myTime;
            myTime.start();
            cout << "comma->dot replace start" << endl;

            char ch;
            QString helpString;

            helpString = def::dataFolder + slash + dirList[dirNum] + slash + "amod";
            def::dir->cd(helpString);
            QStringList lst = def::dir->entryList(QStringList("*.txt"));
            FILE * fil;
            FILE * fil1;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
                        + slash + dirList[dirNum]
                        + slash + "amod"
                        + slash + lst[i];
                fil = fopen(helpString, "r");
                helpString = def::dataFolder
                        + slash + dirList[dirNum]
                        + slash + "amod"
                        + slash + lst[i] + "_";
                fil1 = fopen(helpString, "w");
                while(!feof(fil))
                {
                    fread(&ch, sizeof(char), 1, fil);
                    if(ch == ',') ch = '.';
                    fwrite(&ch, sizeof(char), 1, fil1);
                }
                fclose(fil);
                fclose(fil1);
                ui->progressBar->setValue(i * 100. / lst.length());
            }

            cout << "comma->dot replace finish: time = " << myTime.elapsed()/1000. << " sec" << endl;
            ui->progressBar->setValue(0.);
        }

        if(flagAmodLogToEdf) // corrected logFiles to amod edfs
        {
            ui->progressBar->setValue(0.);
            QTime myTime;
            myTime.start();
            cout << "amod.txt -> amod.edf start" << endl;
            QString helpString = def::dataFolder + slash + dirList[dirNum] + slash + "amod";
            def::dir->cd(helpString);
            QStringList lst = def::dir->entryList(QStringList("*.txt_")); // _ for corrected logs

            edfFile tempEdf;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
                        + slash + dirList[dirNum]
                        + slash + "amod"
                        + slash + lst[i];
                tempEdf = edfFile(helpString); // edfFile(QString) for MATI only

                helpString = def::dataFolder
                        + slash + "auxEdfs"
                        + slash + dirList[dirNum]
                        + slash + getExpNameLib(lst[i])
                        + "_amod.edf";
                tempEdf.writeEdfFile(helpString);

                ui->progressBar->setValue(i * 100. / lst.length());
            }
            cout << "amod.txt -> amod.edf: time = " << myTime.elapsed()/1000. << " sec" << endl;
            ui->progressBar->setValue(0);
        }

        if(flagSliceEdfBySessions) // slice initial edf to edf sessions
        {
            QTime myTime;
            myTime.start();
            cout << "slice edf by sessions start" << endl;

            QString helpString;
            helpString = def::dataFolder
                    + slash + dirList[dirNum]
                    + slash + dirList[dirNum] + fileSuffix
                    + ".edf";
            if(!QFile(helpString).exists())
            {
                cout << "cant open ExpName_fileSuffix.edf (flagSliceEdfBySessions)" << endl;
                break;
            }
            setEdfFile(helpString);
            sliceMati();

            cout << "slice edf by sessions: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }


        if(flagAppendAmodToEeg) // append amod data to EEG data
        {
            QTime myTime;
            myTime.start();
            cout << "append amod.edf to eeg.edf start" << endl;


            QString outPath;
            QString addPath;
            QString helpString;
            edfFile tempEdf;
            for(int type = 0; type < 3; ++type)
            {
                for(int session = 0; session < 6; ++session)
                {
                    // session path
                    helpString = def::dataFolder
                            + slash + dirList[dirNum]
                            + slash + "auxEdfs"
                            + slash + dirList[dirNum] + fileSuffix
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf";

                    if(!QFile(helpString).exists())
                    {
                        cout << "cant open session.edf (flagAppendAmodToEeg)" << endl;
                        continue;
                    }

                    tempEdf.readEdfFile(helpString);

                    // amod file
                    addPath = def::dataFolder
                            + slash + dirList[dirNum]
                            + slash + "auxEdfs"
                            + slash + dirList[dirNum]
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + "_amod.edf"; // generality

                    // eeg+amod path
                    outPath = def::dataFolder
                            + slash + dirList[dirNum]
                            + slash + "auxEdfs"
                            + slash + dirList[dirNum] + fileSuffix
                            + "_a"
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf"; // generality

                    tempEdf.appendFile(addPath, outPath);

#if 0
                    // copy the same files into amod dir

                    helpString = def::dataFolder
                                 + slash + dirList[dirNum]
                                 + slash + "amod"
                                 + slash + dirList[dirNum] + fileSuffix
                                 + "_a"
                                 + "_" + QString::number(type)
                                 + "_" + QString::number(session)
                                 + ".edf"; // generality
                    if(QFile::exists(helpString)) QFile::remove(helpString);
                    QFile::copy(outPath, helpString);
#endif


                }
            }
            cout << "append amod.edf to eeg.edf: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }

        if(flagMakeDiffMark) // make files of markers differences
        {
            QTime myTime;
            myTime.start();
            cout << "make diffMark files start" << endl;

            edfFile fil;
            QString diffMark;
            QString helpString;
            for(int type = 0; type < 3; ++type)
            {
                for(int session = 0; session < 6; ++session)
                {
                    // eeg+amod path
                    helpString = def::dataFolder
                            + slash + dirList[dirNum]

                            + slash + "auxEdfs"

                            + slash + dirList[dirNum] + fileSuffix
                            + "_a"
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf";
                    if(!QFile::exists(helpString)) continue;

                    diffMark = helpString;
                    diffMark.replace(".edf", "_diffMark.txt");

                    fil.readEdfFile(helpString);
                    ofstream outStr(diffMark.toStdString().c_str());
                    outStr << "board" << '\t' << "amod" << '\t' << endl;

                    for(int i = 0; i < fil.getDataLen(); ++i)
                    {
                        if(fil.getData()[fil.getMarkChan()][i] == 0) continue;
                        if(matiCountBit(fil.getData()[fil.getMarkChan()][i], 14))
                        {
                            for(int j = i - 40; j < i + 40; ++j)
                            {
                                if(fil.getData()[37][j+1] != fil.getData()[37][j]) // 37 for EEG + AMOD
                                {
                                    outStr << i << "\t" << j+1 << "\t" << (i-j-1)*4 << '\n';
                                    break;
                                }
                            }
                        }
                    }
                    outStr.flush();
                    outStr.close();
                }
            }

            cout << "make diffMark files: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }

        if(flagSliceSessionsToPieces) // slice constructed eeg+amod files for small pieces
        {
            // not finished
            QTime myTime;
            myTime.start();
            cout << "slice sessions to pieces start" << endl;

            QString outPath;
            QString helpString;
            helpString = def::dataFolder
                    + slash + dirList[dirNum]

                    + slash + "auxEdfs"

                    + slash + dirList[dirNum] + fileSuffix
                    + "_a"
                    + "_0"
                    + "_1"
                    + ".edf"; // generality
            outPath = def::dataFolder
                    + slash + dirList[dirNum]
                    + slash + dirList[dirNum] + fileSuffix
                    + "_a"
                    + ".edf"; // generality
            if(QFile::exists(outPath)) QFile::remove(outPath);
            QFile::copy(helpString, outPath);

            setEdfFile(outPath);

            sliceMatiPieces(true);

            cout << "slice sessions to pieces: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }
    }
}


#endif // AUTOS_CPP
