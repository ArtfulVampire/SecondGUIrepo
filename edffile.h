#ifndef EDFFILE_H
#define EDFFILE_H

#include "library.h"

using namespace std;


class edfFile
{
public:
    typedef vector< vector <double> > dataType;

    edfFile();
    edfFile(int in_ns, int in_dataLength);
    edfFile(int in_ns,
            int in_ndr, int in_ddr, vector<double> in_nr,
            vector < vector<double> > in_data,
            vector<QString> in_labels,
            vector<double> in_physMin,
            vector<double> in_physMax,
            vector<double> in_digMin,
            vector<double> in_digMax);

    edfFile(const edfFile &other);


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

    edfFile operator=(edfFile & other);

//    void appendFile(QString initPath, QString addPath, QString outPath);
//    void appendData(QString initPath, int addNs, double ** addData, vector<QString> addLabels);

private:
    QString filePath;
    QString ExpName;
    QString dirPath;

    int bytes;
    int dataLength;
    int markerChannel;

    int ns;
    int ndr;
    double ddr;
    vector <double> nr; // it is int really
    vector <QString> labels;
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
    QString headerRest;
    dataType data; // matrix.cpp

    int staSlice;

public:
    const dataType & getData() const {return data;}
    void getDataCopy(dataType & destination) const;
    const int &getNs() const;
    const int &getNdr() const;
    const int &getDdr() const;
    const int &getBytes() const;
    const int &getDataLen() const;
    const int &getMarkChan() const;
    const vector<double> & getNr() const;
    const QString & getFilePath() const;
    const QString & getDirPath() const ;
    const QString & getExpName() const;
    const QString & getHeaderInit() const;
    const QString & getHeaderReserved() const ;
    const QString & getHeaderRest() const;

    const vector<QString> & getLabels() const;
    const vector<QString> & getTransducer() const;
    const vector<QString> & getPrefiltering() const;
    const vector<QString> & getPhysDim() const;
    const vector<QString> & getReserved() const;
    const vector<double> & getPhysMax() const;
    const vector<double> & getPhysMin() const;
    const vector<double> & getDigMax() const;
    const vector<double> & getDigMin() const;



};

#endif // EDFFILE_H
