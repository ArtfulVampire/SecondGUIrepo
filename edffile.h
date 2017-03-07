#ifndef EDFFILE_H
#define EDFFILE_H

#include "library.h"
#include <myLib/small.h>



template <typename Typ>
void handleParam(Typ & qStr,
                 int length,
                 bool readFlag,
                 FILE * ioFile,
                 FILE * headerFile);

template <typename Typ>
void handleParamArray(std::vector<Typ> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);
template <typename Typ>
void handleParamArray(std::valarray<Typ> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);


void myTransform(int & output, char * input);
void myTransform(double & output, char * input);
void myTransform(QString & output, char * input);

void myTransform(int input, int len, char ** output);
void myTransform(double input, int len, char ** output);
void myTransform(const QString & input, int len, char ** output);



//typedef matrix edfDataType;

struct edfChannel
{
public:
	QString label{};
	QString transducerType{};
	QString physDim{};
	double physMax{};
	double physMin{};
	double digMax{};
	double digMin{};
	QString prefiltering{};
	double nr{};
	QString reserved{};

public:
	edfChannel()=default;
	~edfChannel() {}

    edfChannel(QString in_label,
               QString in_transducerType,
               QString in_physDim,
               double in_physMax,
               double in_physMin,
               double in_digMax,
               double in_digMin,
               QString in_prefiltering,
               double in_nr,
			   QString in_reserved)
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
    }

	edfChannel & operator=(const edfChannel & other)=default;
//	edfChannel & operator=(edfChannel && other)=default;


};


enum class inst {mati, iitp};

class edfFile
{
public:

	edfFile()=default;
	~edfFile() {}


	edfFile(const edfFile & other, bool noData = false);
	edfFile(const QString & txtFilePath, inst which = inst::mati);

	edfFile & operator=(const edfFile & other)=default;
	edfFile & operator=(edfFile && other)=default;

	/// read/write
    void readEdfFile(QString EDFpath, bool headerOnly = false);
    void writeEdfFile(QString EDFpath, bool asPlain = false);
	void rewriteEdfFile();

    void handleEdfFile(QString EDFpath,
                       bool readFlag,
                       bool headerOnly = false);

    void handleData(bool readFlag,
                    FILE * edfForData);
    void handleDatum(int currNs,
                     int currTimeIndex,
                     bool readFlag,
                     QString & ntAnnot,
                     FILE * edfForDatum);
    void writeMarker(double currDatum,
                     int currTimeIndex) const;
    void handleAnnotations(int currNs,
                           int currentTimeIndex,
                           QString helpString,
						   std::vector<QString> annotations);

	/// make edfFile & func(...);
	/// and  edfFile   func(...) const;
	/// modify
    void adjustArraysByChannels();
	edfFile vertcatFile(QString addEdfPath, QString outPath = QString()) const;
	edfFile & concatFile(QString addEdfPath, QString outPath = QString());
	edfFile & subtractMeans(const QString & outPath = QString());
	edfFile & divideChannel(uint chanNum, double denom);
	edfFile & divideChannels(std::vector<uint> chanNums = {}, double denom = 2.);
	uint findChannel(const QString & str);
    void countFft();
	edfFile & refilter(double lowFreq,
					   double highFreq,
					   const QString & newPath = QString(),
					   bool isNotch = false,
					   std::vector<uint> chanList = {});
    void saveSubsection(int startBin, int finishBin, const QString & outPath, bool plainFlag = false) const;
    void drawSubsection(int startBin, int finishBin, QString outPath) const;
	edfFile reduceChannels(const std::vector<int> & chanList) const;
	edfFile reduceChannels(const QString & chanStr) const;
	void removeChannels(const std::vector<int> & chanList);

    void setLabels(const std::vector<QString> & inLabels);
    void setChannels(const std::vector<edfChannel> & inChannels);
    void setLabels(char ** inLabels);
    void cleanFromEyes(QString eyesPath = QString(),
                       bool removeEogChannels = false,
                       std::vector<int> eegNums = {},
                       std::vector<int> eogNums = {});
    void writeOtherData(const matrix & newData,
                        const QString & outPath,
                        std::vector<int> chanList = {});
    void fitData(int initSize);
    void cutZerosAtEnd();
	void adjustMarkerChannel();


	/// for iitp
	void downsample(double newFreq,
					QString outPath = QString(),
					std::vector<int> chanList = std::vector<int>{}) const;

	void upsample(double newFreq,
				  QString outPath = QString(),
				  std::vector<int> chanList = std::vector<int>{}) const;

	int findJump(int channel,
				 int startPoint,
				 double numSigmas = 5);

	/// modifying
	edfFile & iitpSyncManual(int offsetEeg,
							 int offsetEmg,
							 int addLeft = 0);

	edfFile & iitpSyncAutoCorr(int startSearchEeg,
							   int startEmg,
							   bool byEeg = false);


	std::pair<int, int> iitpSyncAutoJump(int startSearchEeg,
							   int startSearchEmg,
							   bool byEeg = false);

protected:
//    QString headerInitialInfo = QString();
    std::string headerInitialInfo = std::string();
    int bytes = 256;
    QString headerReservedField = QString();

    int ndr = 0;
    double ddr = 1.;
	int ns = 0;


    //channels arrays start
	std::vector<QString> labels;
	std::vector<QString> transducerType;
	std::vector<QString> physDim;
	std::valarray<double> physMax;
	std::valarray<double> physMin;
	std::valarray<double> digMax;
	std::valarray<double> digMin;
	std::vector<QString> prefiltering;
	std::valarray<double> nr; // it is int really
	std::vector<QString> reserved;
	std::vector<QString> annotations;

    //channels arrays end

    QString headerRest = QString();

	/// my fields

	// fast access for slicing (time-bin, marker)
	std::vector<std::pair<int, double>> sessionEdges = std::vector<std::pair<int, double>>();
	int srate = 250;

    std::vector<edfChannel> channels;
	matrix edfData; // matrix.cpp
	std::vector<std::valarray<double>> fftData{}; // mutable?

    int markerChannel = -1;
    QString filePath = QString();
    QString ExpName = QString();
    QString dirPath = QString();

    bool matiFlag = def::matiFlag;
    bool ntFlag = def::ntFlag;
    bool edfPlusFlag = false; // to detect
	bool filterIITPflag = true;

	bool writeMarkersFlag = true;
	bool writeLabelsFlag = false;
	bool writeHeaderFlag = false;

public:
//    const QString & getHeaderInit() const {return headerInitialInfo;}
    const std::string & getHeaderInit() const {return headerInitialInfo;}

    int getBytes() const {return bytes;}
    const QString & getHeaderReserved() const {return headerReservedField;}

    int getNdr() const {return ndr;}
    double getDdr() const {return ddr;}
    int getNs() const {return ns;}
	int getFreq() const {return srate;}

	const std::vector<QString> & getLabels() const {return labels;}
	const std::vector<QString> & getTransducer() const {return transducerType;}
	const std::vector<QString> & getPrefiltering() const {return prefiltering;}
	const std::vector<QString> & getPhysDim() const {return physDim;}
	const std::vector<QString> & getReserved() const {return reserved;}
	const std::valarray<double> & getPhysMax() const {return physMax;}
	const std::valarray<double> & getPhysMin() const {return physMin;}
	const std::valarray<double> & getDigMax() const {return digMax;}
	const std::valarray<double> & getDigMin() const {return digMin;}
	const std::valarray<double> & getNr() const {return nr;}
    const QString & getHeaderRest() const {return headerRest;}

    const std::vector<edfChannel> & getChannels() const {return channels;}

	int getDataLen() const {return edfData.cols();}
	int getMarkChan() const {return markerChannel;}

    const QString & getFilePath() const {return filePath;}
    const QString & getDirPath() const  {return dirPath;}
    const QString & getExpName() const {return ExpName;}
    QString getFileNam() const {return myLib::getFileName(filePath);}

    const bool & getMatiFlag() const {return matiFlag;}
    const bool & getNtFlag() const {return ntFlag;}
    const bool & getEdfPlusFlag() const {return edfPlusFlag;}

    void setMatiFlag(bool newFlag) {matiFlag = newFlag;}
    void setNtFlag(bool newFlag) {ntFlag = newFlag;}
    void setEdfPlusFlag(bool newFlag) {edfPlusFlag = newFlag;}
	void setFilterIITPFlag(bool newFlag) {filterIITPflag = newFlag;}

    // operations with data


	const matrix & getData() const {return edfData;}
	const std::valarray<double> & getMarkArr() const {return edfData[markerChannel];}
	void setData(int chanNum, int timeBin, double val) {edfData[chanNum][timeBin] = val;}

    const std::valarray<double> & operator [](int i) const
    {
		return edfData[i];
    }


public:
    static void transformEdfMatrix(const QString & inEdfPath,
                                   const matrix & matrixW,
                                   const QString & newEdfPath);
    static double checkDdr(const QString & inPath);

    std::list<std::valarray<double>> getDataAsList() const;
    void setDataFromList(const std::list<std::valarray<double>> & inList);

	/// edf file repairs
	edfFile repairDataScaling(int denominator = 1000.) const;
	edfFile repairChannelsOrder(const std::vector<QString> & standard) const;
	edfFile repairHoles() const;
	edfFile repairPhysEqual() const;

};


/// public "static" funcs to repair edfs or filenames
namespace repair
{

bool testChannelsOrderConsistency(const QString & dirPath);
void channelsOrderFile(const QString & inFilePath,
                       QString outFilePath = QString(),
                       const std::vector<QString> & standard = coords::lbl31_more);
void channelsOrderDir(const QString & inDirPath,
                      const QString & outDirPath,
                      const std::vector<QString> & standard = coords::lbl31_more);

void holesFile(const QString & inFilePath,
               QString outFilePath = QString());
void holesDir(const QString & inDirPath,
              const QString & outDirPath);

void physMinMaxCheck(const QString & dirPath);
void physMinMaxDir(const QString & dirPath, const QStringList & filters = def::edfFilters);


/// only renames files/dirs
void deleteSpacesFileOrFolder(const QString & fileOrFolderPath);
void deleteSpacesDir(const QString & dirPath, const QStringList & filters = def::edfFilters);

void toLatinFileOrFolder(const QString & fileOrFolderPath);
void toLatinDir(const QString & dirPath, const QStringList & filters = def::edfFilters);

void toLowerFileOrFolder(const QString & filePath);
void toLowerDir(const QString & dirPath, const QStringList & filters = def::edfFilters);

void fullRepairDir(const QString & dirPath, const QStringList & filters = def::edfFilters);
}

#endif // EDFFILE_H
