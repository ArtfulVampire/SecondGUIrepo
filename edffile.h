#ifndef EDFFILE_H
#define EDFFILE_H

#include "library.h"

using namespace std;

typedef vector < vector <double> > dataType;

struct edfChannel
{
    QString label;
    QString transducerType;
    QString physDim;
    double physMax;
    double physMin;
    double digMax;
    double digMin;
    QString prefiltering;
    double nr;
    QString reserved;
    vector <double> data;

    edfChannel operator = (const edfChannel & other)
    {
        this->label = other.label;
        this->transducerType = other.transducerType;
        this->physDim = other.physDim;
        this->physMax = other.physMax;
        this->physMin = other.physMin;
        this->digMax = other.digMax;
        this->digMin = other.digMin;
        this->prefiltering = other.prefiltering;
        this->nr = other.nr;
        this->reserved = other.reserved;
        this->data = other.data;
        return *this;
    }

    //edfChannel(edfFile, int);
    edfChannel(QString in_label,
               QString in_transducerType,
               QString in_physDim,
               double in_physMax,
               double in_physMin,
               double in_digMax,
               double in_digMin,
               QString in_prefiltering,
               double in_nr,
               QString in_reserved,
               vector <double> in_data)
    {
        this->label = in_label;
        this->transducerType = in_transducerType;
        this->physDim = in_physDim;
        this->physMax = in_physMax;
        this->physMin = in_physMin;
        this->digMax = in_digMax;
        this->digMin = in_digMin;
        this->prefiltering = in_prefiltering;
        this->nr = in_nr;
        this->reserved = in_reserved;
        this->data = in_data;
    }

    edfChannel()
    {
        this->label = QString();
        this->transducerType = QString();
        this->physDim = QString();
        this->physMax = 0.;
        this->physMin = 0.;
        this->digMax = 0.;
        this->digMin = 0.;
        this->prefiltering = QString();
        this->nr = 0.;
        this->reserved = QString();
        this->data = vector <double> ();
    }
};


class edfFile
{
public:

    edfFile();

    /*
    //yet unused
    edfFile(int in_ndr,
            int in_ns,
            int in_ddr,
            vector <QString> in_labels,
            vector <double> in_physMin,
            vector <double> in_physMax,
            vector <double> in_digMin,
            vector <double> in_digMax,
            vector <double> in_nr,
            vector < vector <double> > in_data);
    */

    edfFile(const edfFile & other);
    edfFile(QString matiLogPath);

    void readEdfFile(QString EDFpath);
    void writeEdfFile(QString EDFpath, bool asPlain = false);
    void handleEdfFile(QString EDFpath,
                       bool readFlag);

    template <typename Typ>
    void handleParam(Typ & qStr,
                     int length,
                     bool readFlag,
                     FILE * ioFile,
                     FILE * headerFile);

    void myTransform(int & output, char * input) {output = atoi(input);}
    void myTransform(double & output, char * input) {output = atoi(input);}
    void myTransform(QString & output, char * input) {output = QString(input);}

    void myTransform(int & input, const int & len, char ** output){(*output) = QStrToCharArr(fitNumber(input, len));}
    void myTransform(double & input, const int & len, char ** output) {(*output) = QStrToCharArr(fitNumber(input, len));}
    void myTransform(QString & input, const int & len, char ** output) {(*output) = QStrToCharArr(input);}

    template <typename Typ>
    void handleParamArray(vector <Typ> & qStr,
                          int number,
                          int length,
                          bool readFlag,
                          FILE *ioFile,
                          FILE * headerFile);

    void handleData(bool readFlag,
                    FILE * edfForData);

    void handleDatum(const int & currNs,
                     const int & currTimeIndex,
                     bool readFlag,
                     QString & ntAnnot,
                     FILE * edfForDatum);

    void writeMarker(const int & currNs,
                     const int & currTimeIndex);

    void handleAnnotations(const int & currNs,
                           const int & currentTimeIndex,
                           QString helpString,
                           vector <QString> annotations);

    edfFile operator=(const edfFile & other);

    void adjustArraysByChannels();
    void appendFile(QString addEdfPath, QString outPath);
    void concatFile(QString addEdfPath, QString outPath);
    void appendChannel(edfChannel addChan, QString outPath); // check ndr
    void swapChannels(int num1, int num2);
    void saveSubsection(int startBin, int finishBin, QString outPath, bool plainFlag = false);
    void writeOtherData(vector < vector <double> > newData, QString outPath, QList<int> chanList);
    void writeOtherData(double ** newData, int newDataLength, QString outPath, QList<int> chanList);
    void fitData(int initSize);
    void cutZerosAtEnd();

private:
    QString headerInitialInfo = QString();
    int bytes = 256;
    QString headerReservedField = QString();

    int ndr = 0;
    double ddr = 1;
    int ns = 0;

    //channels arrays start
    vector <QString> labels;
    vector <QString> transducerType;
    vector <QString> physDim;
    vector <double> physMax;
    vector <double> physMin;
    vector <double> digMax;
    vector <double> digMin;
    vector <QString> prefiltering;
    vector <double> nr; // it is int really
    vector <QString> reserved;
    dataType data; // matrix.cpp
    //channels arrays end

    QString headerRest = QString();

    vector <edfChannel> channels;

    int staSlice = 0; // yet not useful
    int dataLength = 0;
    int markerChannel = -1;
    QString filePath = QString();
    QString ExpName = QString();
    QString dirPath = QString();

    bool matiFlag = 1;
    bool ntFlag = 0;


public:
    const QString & getHeaderInit() const {return headerInitialInfo;}
    const int & getBytes() const {return bytes;}
    const QString & getHeaderReserved() const {return headerReservedField;}

    const int & getNdr() const {return ndr;}
    const double & getDdr() const {return ddr;}
    const int & getNs() const {return ns;}
    const vector <QString> & getLabels() const {return labels;}
    const vector <QString> & getTransducer() const {return transducerType;}
    const vector <QString> & getPhysDim() const {return physDim;}
    const vector <double> & getPhysMax() const {return physMax;}
    const vector <double> & getPhysMin() const {return physMin;}
    const vector <double> & getDigMax() const {return digMax;}
    const vector <double> & getDigMin() const {return digMin;}
    const vector <QString> & getPrefiltering() const {return prefiltering;}
    const vector <double> & getNr() const {return nr;}
    const vector <QString> & getReserved() const {return reserved;}
    const dataType & getData() const {return data;}
    const QString & getHeaderRest() const {return headerRest;}

    void getDataCopy(dataType & destination) const {destination = data;}
    const vector <edfChannel> & getChannels() const {return channels;}

    const int &getDataLen() const {return dataLength;}
    const int &getMarkChan() const {return markerChannel;}

    const QString & getFilePath() const {return filePath;}
    const QString & getDirPath() const  {return dirPath;}
    const QString & getExpName() const {return ExpName;}

    const bool & getMatiFlag() const {return matiFlag;}
    const bool & getNtFlag() const {return ntFlag;}

    void setMatiFlag(bool newFlag) {matiFlag = newFlag;}
    void setNtFlag(bool newFlag) {ntFlag = newFlag;}

    void getLabelsCopy(char ** & dest)
    {
        for(int i = 0; i < this->ns; ++i)
        {
            memcpy(dest[i],
                   this->labels[i].toStdString().c_str(),
                   this->labels[i].length() * sizeof(char));
            dest[i][this->labels[i].length()] = '\0';
        }
    }

    void getDataCopy(double ** dest) const
    {
        for(int i = 0; i < this->ns; ++i)
        {
            memcpy(dest[i], this->data[i].data(), this->dataLength * sizeof(double));
        }
    }

};

#endif // EDFFILE_H
