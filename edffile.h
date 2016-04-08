#ifndef EDFFILE_H
#define EDFFILE_H

#define DATA_IN_CHANS 0
#define DATA_POINTER_IN_CHANS 0
#define DATA_POINTER 0

#include "library.h"
#include "smallFuncs.h"

typedef matrix edfDataType;

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
    std::vector <double> * dataP;
#endif

#if DATA_IN_CHANS
    std::vector <double> data;
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
               , std::vector<double> * in_dataP
           #endif
           #if DATA_IN_CHANS
               ,std::vector <double> in_data
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
        this->data = std::vector <double> ();
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
            std::vector <QString> in_labels,
            std::vector <double> in_physMin,
            std::vector <double> in_physMax,
            std::vector <double> in_digMin,
            std::vector <double> in_digMax,
            std::vector <double> in_nr,
            std::vector < std::vector <double> > in_data);
    */

    edfFile(const edfFile & other, bool noData = false);
    edfFile(const QString & matiLogPath);

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
    void myTransform(double & output, char * input) {output = atof(input);}
    void myTransform(QString & output, char * input) {output = QString(input);}

    void myTransform(const int & input, const int & len, char ** output)
    {
        (*output) = QStrToCharArr(fitNumber(input, len));
    }
    void myTransform(const double & input, const int & len, char ** output)
    {
        (*output) = QStrToCharArr(fitNumber(input, len));
    }
    void myTransform(const QString & input, const int & len, char ** output)
    {
        (*output) = QStrToCharArr(input, len);
    }

    template <typename Typ>
    void handleParamArray(std::vector <Typ> & qStr,
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
                           std::vector <QString> annotations);

    edfFile operator=(const edfFile & other);

    void adjustArraysByChannels();
    void appendFile(QString addEdfPath, QString outPath) const;
    void concatFile(QString addEdfPath, QString outPath = QString());
    void refilter(const double &lowFreq, const double &highFreq, const QString & newPath = QString());
    void saveSubsection(int startBin, int finishBin, const QString &outPath, bool plainFlag = false) const;
    void drawSubsection(int startBin, int finishBin, QString outPath) const;
    void reduceChannels(const std::vector<int> & chanList);
    void reduceChannels(const QString & chanStr);
    void removeChannels(const std::vector<int> & chanList);
//    void removeChannels(const QString & chanStr);


    void setLabels(const std::vector<QString> & inLabels);
    void setChannels(const std::vector<edfChannel> & inChannels);
    void setLabels(char ** inLabels);
    void cleanFromEyes(QString eyesPath = QString(),
                       bool removeEogChannels = false,
                       std::vector<int> eegNums = {},
                       std::vector<int> eogNums = {});
//    void writeOtherData(mat & newData,
//                        const QString & outPath,
//                        std::vector<int> chanList = {});
    void writeOtherData(const matrix & newData,
                        const QString & outPath,
                        std::vector<int> chanList = {});
//    void writeOtherData(double ** newData,
//                        int newDataLength,
//                        QString outPath,
//                        std::vector<int> chanList = {}) const;
    void fitData(int initSize);
    void cutZerosAtEnd();
    void adjustMarkerChannel();


private:
    QString headerInitialInfo = QString();
    int bytes = 256;
    QString headerReservedField = QString();

    int ndr = 0;
    double ddr = 1.;
    int ns = 0;

    // fast access for slicing (time-bin, marker)
    std::vector<std::pair<int, double>> sessionEdges = std::vector<std::pair<int, double>>();

    //channels arrays start
    std::vector <QString> labels;
    std::vector <QString> transducerType;
    std::vector <QString> physDim;
    std::vector <double> physMax;
    std::vector <double> physMin;
    std::vector <double> digMax;
    std::vector <double> digMin;
    std::vector <QString> prefiltering;
    std::vector <double> nr; // it is int really
    std::vector <QString> reserved;
    std::vector <QString> annotations;

    //channels arrays end

    QString headerRest = QString();

    std::vector <edfChannel> channels;
    edfDataType data; // matrix.cpp

#if DATA_POINTER
    edfDataType (*dataPointer) = &data;
#endif


    int staSlice = 0; // yet not useful
    int dataLength = 0;
    int markerChannel = -1;
    QString filePath = QString();
    QString ExpName = QString();
    QString dirPath = QString();

    bool matiFlag = def::matiFlag;
    bool ntFlag = def::ntFlag;
    bool edfPlusFlag = def::edfPlusFlag;

public:
    const QString & getHeaderInit() const {return headerInitialInfo;}
    const int & getBytes() const {return bytes;}
    const QString & getHeaderReserved() const {return headerReservedField;}

    const int & getNdr() const {return ndr;}
    const double & getDdr() const {return ddr;}
    const int & getNs() const {return ns;}
    const std::vector <QString> & getLabels() const {return labels;}
    const std::vector <QString> & getTransducer() const {return transducerType;}
    const std::vector <QString> & getPhysDim() const {return physDim;}
    const std::vector <double> & getPhysMax() const {return physMax;}
    const std::vector <double> & getPhysMin() const {return physMin;}
    const std::vector <double> & getDigMax() const {return digMax;}
    const std::vector <double> & getDigMin() const {return digMin;}
    const std::vector <QString> & getPrefiltering() const {return prefiltering;}
    const std::vector <double> & getNr() const {return nr;}
    const std::vector <QString> & getReserved() const {return reserved;}
    const QString & getHeaderRest() const {return headerRest;}
    const std::vector <edfChannel> & getChannels() const {return channels;}

    const int & getDataLen() const {return dataLength;}
    const int & getMarkChan() const {return markerChannel;}

    const QString & getFilePath() const {return filePath;}
    const QString & getDirPath() const  {return dirPath;}
    const QString & getExpName() const {return ExpName;}

    const bool & getMatiFlag() const {return matiFlag;}
    const bool & getNtFlag() const {return ntFlag;}
    const bool & getEdfPlusFlag() const {return edfPlusFlag;}

    void setMatiFlag(bool newFlag) {matiFlag = newFlag;}
    void setNtFlag(bool newFlag) {ntFlag = newFlag;}
    void setEdfPlusFlag(bool newFlag) {edfPlusFlag = newFlag;}

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
    const edfDataType & getData() const {return (*dataPointer);}
    void setData(int chanNum, int timeBin, double val) {(*dataPointer)[chanNum][timeBin] = val;}
    void getDataCopy(edfDataType & destination) const {destination = (*dataPointer);}
#else

    const edfDataType & getData() const {return data;}
    void setData(int chanNum, int timeBin, double val) {data[chanNum][timeBin] = val;}
    void getDataCopy(edfDataType & destination) const {destination = data;}

#endif


public:
    static void transformEdfMatrix(const QString & inEdfPath,
                                   const matrix & matrixW,
                                   const QString & newEdfPath);

};

#endif // EDFFILE_H
