#ifndef EDFFILE_H
#define EDFFILE_H

#include <QString>
#include <iostream>
#include <fstream>
#include <cmath>
#include <QList>

using namespace std;


class edfFile
{
public:
    edfFile();

    void readEdf(QString inPath);
    void countICA(QString outPath);
    double countSpoc(QString outWeightFile);
    void sliceOneByOne(QList<int> markers);
    void constructEdf(edfFile initFile, QString realsPath, QString outFile);


private:
    int ns;
    int ndr;
    double ddr;
    int *nr;
    char ** label;
    double ** data;
    int staSlice;
    QString filePath;
    QString ExpName;


};

#endif // EDFFILE_H
