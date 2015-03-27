#ifndef EDFFILE_H
#define EDFFILE_H

#include "library.h"

using namespace std;


class edfFile
{
public:
    edfFile();
    typedef vector< vector <double> > dataType;


    void readEdfFile(QString EDFpath, bool matiFlag = true, bool ntFlag = false);
    void writeEdfFile(QString EDFpath, bool matiFlag = true, bool ntFlag = false);
    void handleEdfFile(QString EDFpath,
                       bool readFlag = true,
                       bool matiFlag = true,
                       bool ntFlag = false);

    template <typename Typ>
    void handleParam(Typ & qStr,
                     int length,
                     bool readFlag,
                     FILE * ioFile,
                     FILE * headerFile);

    void myTransform(int & output, char * input);
    void myTransform(int & input, const int & len, char ** output);
    void myTransform(double & output, char * input);
    void myTransform(double & input, const int & len, char ** output);
    void myTransform(QString & output, char * input);
    void myTransform(QString & input, const int & len, char ** output);

    template <typename Typ>
    void handleParamArray(vector <Typ> & qStr,
                          int number,
                          int length,
                          bool readFlag,
                          FILE *ioFile,
                          FILE * headerFile);

    void handleData(bool readFlag,
                    bool matiFlag,
                    bool ntFlag,
                    FILE * edfForData);

    void handleDatum(const int & currNs,
                     const int & currTimeIndex,
                     bool readFlag,
                     bool matiFlag,
                     bool ntFlag,
                     QString & ntAnnot,
                     FILE * edfForDatum);

    void writeMarker(const int & currNs,
                     const int & currTimeIndex,
                     bool matiFlag);

    void handleAnnotations(const int & currNs,
                           const int & currentTimeIndex,
                           QString helpString,
                           vector<QString> annotations);

private:
    int bytes;
    int ns;
    int ndr;
    int dataLength;
    int markerChannel;
    double ddr;
    vector <double> nr; // it is int really
    vector <QString> label;
    vector <QString> transducerType;
    vector <QString> physDim;
    vector <QString> prefiltering;
    vector <QString> reserved;

    vector <double> physMax;
    vector <double> physMin;
    vector <double> digMax;
    vector <double> digMin;
    QString headerInitialInfo;
    QString headerReservedField;
    QString restOfHeader;
    dataType data; // matrix.cpp
    int staSlice;

    QString filePath;
    QString ExpName;
    QString dirPath;
public:
    const dataType & getData() const;
    void getDataCopy(dataType & destination) const;
    const int &getNs() const;
    const int &getNdr() const;
    const int &getDdr() const;
    const int &getDataLen() const;
    const int &getMarkChan() const;
    const vector<double> & getNr() const;
    const QString & getFilePath() const;
    const QString & getDirPath() const ;
    const QString & getExpName() const;
};

#endif // EDFFILE_H
