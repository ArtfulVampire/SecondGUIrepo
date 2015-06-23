#ifndef EDFFILE_H
#define EDFFILE_H

#define DATA_IN_CHANS 0
#define DATA_POINTER_IN_CHANS 0
#define DATA_POINTER 1

#include "library.h"

using namespace std;

typedef vector < vector <double> > mat;

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
#if DATA_IN_CHANS
    vector <double> * dataP;
#endif

#if DATA_IN_CHANS
    vector <double> data;
#endif

    edfChannel operator = (const edfChannel & other)
    {
        if(this == &other)
        {
//            cout << "operator =: channels are the same" << endl;
            return *this;
        }
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

#if DATA_POINTER_IN_CHANS
        this->dataP = other.dataP;
#endif

#if DATA_IN_CHANS
        this->data = other.data;
#endif
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
               QString in_reserved

           #if DATA_POINTER_IN_CHANS
               , vector<double> * in_dataP
           #endif
           #if DATA_IN_CHANS
               ,vector <double> in_data
           #endif
               )
//
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
#if DATA_POINTER_IN_CHANS
        this->dataP = in_dataP;
#endif
#if DATA_IN_CHANS
        this->data = in_data;
#endif
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
#if DATA_POINTER_IN_CHANS
    this->dataP = nullptr;
#endif
#if DATA_IN_CHANS
        this->data = vector <double> ();
#endif
    }
    ~edfChannel()
    {
    }
};


class edfFile
{
public:

    edfFile();
    ~edfFile();

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

    edfFile(const edfFile & other, bool noData = false);
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

    void myTransform(int & input, const int & len, char ** output)
    {
        (*output) = QStrToCharArr(fitNumber(input, len));
    }
    void myTransform(double & input, const int & len, char ** output)
    {
        (*output) = QStrToCharArr(fitNumber(input, len));
    }
    void myTransform(QString & input, const int & len, char ** output)
    {
        (*output) = QStrToCharArr(input, len);
    }

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

    void writeMarker(const double & currDatum,
                     const int & currTimeIndex) const;

    void handleAnnotations(const int & currNs,
                           const int & currentTimeIndex,
                           QString helpString,
                           vector <QString> annotations);

    edfFile operator=(const edfFile & other);

    void adjustArraysByChannels();
    void appendFile(QString addEdfPath, QString outPath) const;
    void concatFile(QString addEdfPath, QString outPath = QString());
    void refilter(const double &lowFreq, const double &highFreq, QString newPath = QString());
    void saveSubsection(int startBin, int finishBin, const QString &outPath, bool plainFlag = false) const;
    void drawSubsection(int startBin, int finishBin, QString outPath) const;
    void reduceChannels(QList<int> chanList);
    void reduceChannels(QString chanStr);
    void setLabels(char ** inLabels);
    void cleanFromEyes(QString eyesPath = QString(),
                       bool removeEogChannels = false,
                       QList <int> eegNums = QList <int>(),
                       QList <int> eogNums = QList <int>());
    void writeOtherData(mat &newData, QString outPath, QList<int> chanList = QList<int>());
    void writeOtherData(matrix &newData, QString outPath, QList<int> chanList = QList<int>());
    void writeOtherData(double ** newData, int newDataLength, QString outPath, QList<int> chanList = QList<int>()) const;
    void fitData(int initSize);
    void cutZerosAtEnd();
    void adjustMarkerChannel();


private:
    QString headerInitialInfo = QString();
    int bytes = 256;
    QString headerReservedField = QString();

    int ndr = 0;
    double ddr = 1;
    int ns = 0;

    vector < pair <int, double> > sessionEdges = vector < pair <int, double> >(0); // fast access

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

    //channels arrays end

    QString headerRest = QString();

    vector <edfChannel> channels;
    mat data; // matrix.cpp

#if DATA_POINTER
    mat (*dataPointer) = &data;
#endif


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
    const QString & getHeaderRest() const {return headerRest;}
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

    void getLabelsCopy(char ** & dest) const
    {
        for(int i = 0; i < this->ns; ++i)
        {
            memcpy(dest[i],
                   this->labels[i].toStdString().c_str(),
                   this->labels[i].length() * sizeof(char));
            dest[i][this->labels[i].length()] = '\0';
        }
    }

    // operations with data
#if DATA_POINTER
    const mat & getData() const {return (*dataPointer);}
    void setData(int chanNum, int timeBin, double val) {(*dataPointer)[chanNum][timeBin] = val;}
    void getDataCopy(mat & destination) const {destination = (*dataPointer);}

    void getDataCopy(double ** & dest) const
    {
        for(int i = 0; i < this->ns; ++i)
        {
            memcpy(dest[i],
                   (*this->dataPointer)[i].data(),
                   this->dataLength * sizeof(double));
        }
    }
#else

    const mat & getData() const {return data;}
    void setData(int chanNum, int timeBin, double val) {data[chanNum][timeBin] = val;}
    void getDataCopy(mat & destination) const {destination = data;}

    void getDataCopy(double ** & dest) const
    {
        for(int i = 0; i < this->ns; ++i)
        {
            memcpy(dest[i], this->data[i].data(), this->dataLength * sizeof(double));
        }
    }
#endif

};

#endif // EDFFILE_H
