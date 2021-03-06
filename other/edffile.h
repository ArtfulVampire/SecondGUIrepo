#ifndef EDFFILE_H
#define EDFFILE_H

//#include <optional> /// to do (for findChannels)

#include <fstream>
#include <bitset>

#include <other/matrix.h>
#include <myLib/small.h>
#include <myLib/output.h>
#include <myLib/general.h>

#include <QVector>
#include <QTime>


using namespace myOut;

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
	~edfChannel()=default;

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
		this->label				= in_label;
		this->transducerType	= in_transducerType;
		this->physDim			= in_physDim;
		this->physMax			= in_physMax;
		this->physMin			= in_physMin;
		this->digMax			= in_digMax;
		this->digMin			= in_digMin;
		this->prefiltering		= in_prefiltering;
		this->nr				= in_nr;
		this->reserved			= in_reserved;
    }

	edfChannel & operator=(const edfChannel & other)=default;
};



enum class inst {mati, iitp, veget};
enum class eogType {cross, correspond};
enum class reference {A1, A2, Ar, N, Cz, CAR, Base};

/// bidirectional map - use boost?
const std::map<QString, reference> strToRef
{
	{"A1",		reference::A1},
	{"A2",		reference::A2},
	{"Ar",		reference::Ar},
	{"N",		reference::N},
	{"Cz",		reference::Cz},
	{"CAR",		reference::CAR},
	{"Base",	reference::Base},
};
const std::map<reference, QString> refToStr
{
	{reference::A1,		"A1"},
	{reference::A2,		"A2"},
	{reference::Ar,		"Ar"},
	{reference::N,		"N"},
	{reference::Cz,		"Cz"},
	{reference::CAR,	"CAR"},
	{reference::Base,	"Base"},
};


class edfFile
{
public:
	edfFile()=default;
	~edfFile()=default;
	edfFile(edfFile && other)=default;
//	edfFile & operator=(const edfFile & other)=default;
	edfFile & operator=(edfFile && other)=default;

//	edfFile(const edfFile & other)=default;
	edfFile(const edfFile & other, bool noData = false);
	edfFile(const QString & txtFilePath, inst which);
	edfFile(const QString & edfPath, bool headerOnly = false);

	operator bool() { return edfData.isEmpty(); }
	bool isEmpty() { return edfData.isEmpty(); }

	/// read/write
	edfFile & readEdfFile(const QString & EDFpath, bool headerOnly = false);
	edfFile & reOpen();
	void writeEdfFile(const QString & EDFpath);
	void rewriteEdfFile();
	void writeOtherData(const matrix & newData,
						const QString & outPath,
						std::vector<int> chanList = {}) const;
	void saveSubsection(int startBin, int finishBin, const QString & outPath) const;
	void drawSubsection(int startBin, int finishBin, const QString & outPath) const; /// remake to QPixmap


	void handleEdfFile(const QString & EDFpath,
					   bool readFlag,
					   bool headerOnly = false);
	void handleData(bool readFlag,
					std::fstream & edfForData);
	void handleDatum(int currNs,
					 int currTimeIndex,
					 bool readFlag,
					 QString & ntAnnot,
					 std::fstream & edfForDatum);

    void writeMarker(double currDatum,
					 int currTimeIndex) const; /// deprecated

	void writeAnnotations() const;
	void writeMarkers() const;
	std::vector<std::pair<double, QString> > handleAnnotations() const;
	std::vector<std::pair<double, QString> > handleAnnotation(const QString & annot) const;

	/// make edfFile & func(...);
	/// and  edfFile   func(...) const;
	/// modify
	edfFile vertcatFile(const QString & addEdfPath) const;
	edfFile & concatFile(const QString & addEdfPath);
	edfFile & concatFiles(const std::vector<QString> & filePaths);
	static edfFile concatFilesStatic(const std::vector<QString> & filePaths);
	edfFile & subtractMeans(const QString & outPath = QString());

    void countFft();
	edfFile & refilter(double lowFreq,
					   double highFreq,
					   bool isNotch = false,
					   std::vector<int> chanList = {});

	/// need check
	edfFile rereferenceData(reference newRef,
							bool eogAsIs,
							bool bipolarEog12) const;
	/// need check
	edfFile rereferenceDataCAR() const;
	edfFile rereferenceDataCz() const;

	/// channels modify
	/// retain
	edfFile reduceChannels(const std::vector<int> & chanList) const;
	edfFile reduceChannels(const QString & chanString) const;
	/// To Do
//	edfFile & retainChannels(const std::vector<int> & chanList);
//	edfFile & retainChannels(const std::vector<uint> & chanList);
//	edfFile & retainChannels(const QStringList & chanList);
//	edfFile & retainChannel(int num);
//	edfFile & retainChannel(const QString & nam);

	edfFile & removeChannels(const std::vector<int> & chanList);
	edfFile & removeChannels(const std::vector<uint> & chanList);
	edfFile & removeChannels(const QStringList & chanList);
	edfFile & removeChannel(int num);
	edfFile & removeChannel(const QString & nam);

	edfFile & insertChannel(int num, const std::valarray<double> & dat, const edfChannel & ch);
	edfFile & addChannel(const std::valarray<double> & dat, const edfChannel & ch);
	edfFile & addMarkerChannel();

	edfFile & multiplyChannel(uint chanNum, double mult);
	edfFile & multiplyChannels(std::vector<uint> chanNums, double mult);

	edfFile & divideChannel(uint chanNum, double denom);
	edfFile & divideChannels(std::vector<uint> chanNums, double denom);

	edfFile & zeroChannel(int chanNum) { edfData[chanNum] = 0.; return *this; }
	edfFile & zeroChannels(const std::vector<int> & chanNums);

	int findChannel(const QString & str) const;
	int findChannel(int num) const { return num; }
	std::vector<int> findChannels(const QString & filter) const;
	std::vector<int> findChannels(const std::vector<QString> & strs) const;
	std::vector<bool> hasChannels(const std::vector<QString> & strs) const;
	template<int N>
	std::bitset<N> hasChannels(const std::vector<QString> & strs);

	int countMarker(int mrk) const;
	std::vector<uint> countMarkers(const std::vector<int> & mrks) const;
	std::pair<int, int> findMarker(const std::vector<int> & mrks) const;
	std::vector<std::pair<int, int>> findMarkers(const std::vector<int> & mrks) const;

    void setLabels(const std::vector<QString> & inLabels);
	void setLabel(int i, const QString & inLabel);

	void setChannels(const std::vector<edfChannel> & inChannels);
	edfFile & cleanFromEyes(QString eyesPath = QString(),
							bool removeEogChannels = false,
							std::vector<int> eegNums = {},
							std::vector<int> eogNums = {});

	edfFile & downsample(double newFreq,
					std::vector<int> chanList = std::vector<int>{});

	edfFile & upsample(double newFreq,
					   std::vector<int> chanList = std::vector<int>{});

	int findJump(int channel,
				 int startPoint,
				 double numSigmas = 5) const;

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

	const QString & getHeaderInit() const						{ return headerInitialInfo; }
	const QString & getHeaderReserved() const					{ return headerReservedField; }
	int getBytes() const										{ return bytes; }
	int getNdr() const											{ return ndr; }
	double getDdr() const										{ return ddr; }
	int getNs() const											{ return ns; }
	double getFreq() const										{ return srate; } /// wow wow
	double getFreq(int numChan) const							{ return nr[numChan] / ddr; }

	const std::vector<QString> & getLabels() const					{ return labels; }
	const QString & getLabels(int i) const							{ return labels[i]; }
	const std::vector<QString> & getTransducer() const				{ return transducerType; }
	const std::vector<QString> & getPrefiltering() const			{ return prefiltering; }
	const std::vector<QString> & getPhysDim() const					{ return physDim; }
	const std::vector<QString> & getReserved() const				{ return reserved; }
	const std::valarray<double> & getPhysMax() const				{ return physMax; }
	const std::valarray<double> & getPhysMin() const				{ return physMin; }
	const std::valarray<double> & getDigMax() const					{ return digMax; }
	const std::valarray<double> & getDigMin() const					{ return digMin; }
	const std::valarray<double> & getNr() const						{ return nr; }
	double getNr(int i) const										{ return nr[i]; }
	const QString & getHeaderRest() const							{ return headerRest; }

	const std::vector<std::pair<int, int>> & getMarkers() const		{ return markers; }
	const std::vector<QString> & getAnnotations() const				{ return annotations; }
	const std::vector<edfChannel> & getChannels() const				{ return channels; }
	const edfChannel & getChannels(int i) const						{ return channels[i]; }

	const QString & getFilePath() const								{ return filePath; }
	const QString & getDirPath() const								{ return dirPath; }
	const QString & getExpName() const								{ return ExpName; }
	QString getExpNameShort() const					{ return ExpName.left(ExpName.indexOf('_')); }
	QString getFileName(bool ext) const				{ return myLib::getFileName(filePath, ext); }

	/// flags
	/// gets
	bool getMatiFlag() const							{ return matiFlag; }
	bool getNtFlag() const								{ return ntFlag; }
	bool getEdfPlusFlag() const							{ return edfPlusFlag; }
	eogType getEogType() const							{ return this->edfEogType; }
	/// sets
	void setMatiFlag(bool newFlag)						{ matiFlag = newFlag; }
	void setNtFlag(bool newFlag)						{ ntFlag = newFlag; }
	void setEdfPlusFlag(bool newFlag)					{ edfPlusFlag = newFlag; }
	void setFilterIITPFlag(bool newFlag)				{ filterIITPflag = newFlag; }
	void setEogType(eogType in)							{ this->edfEogType = in; }

	/// operations with data
	/// gets
	const matrix & getData() const						{ return edfData; }
	const std::valarray<double> & getData(int i) const	{ return edfData[i]; }
	const std::valarray<double> & getMarkArr() const	{ return edfData[markerChannel]; }
	double getMarkArr(int i) const						{ return edfData[markerChannel][i]; }
	int getDataLen() const								{ return edfData.cols(); }
	int getMarkChan() const								{ return markerChannel; }
	const std::valarray<double> & getData(const QString & ch) const;
	std::vector<int> getAllEegChannels(const std::vector<QString> & standard) const;

	/// make edfFile &
	/// sets
	void setData(int chanNum, int timeBin, double val)	{ edfData[chanNum][timeBin] = val; }
	void setMarker(int timeBin, double val)				{ edfData[markerChannel][timeBin] = val; }
	void setData(int chanNum,
				 const std::valarray<double> & newChan) { edfData[chanNum] = newChan; }
	const std::valarray<double> & operator [](int i) const { return edfData[i]; }
	static void transformEdfMatrix(const QString & inEdfPath,
								   const matrix & matrixW,
								   const QString & newEdfPath);
	static double checkDdr(const QString & inPath);
	static bool isRerefChannel(const QString & inLabel);
	static QString rerefChannel(reference initialRef,
								reference targetRef,
								const QString & currentNum,
								const QString & earsChan,
								const QString & groundChan,
								const std::vector<QString> & sign);
	std::list<std::valarray<double>> getDataAsList() const;
	edfFile & setDataFromList(const std::list<std::valarray<double>> & inList);

	/// edf file repairs
	edfFile repairDataScaling(int denominator = 1000.) const;
	edfFile repairChannelsOrder(const std::vector<QString> & standard) const;
	edfFile repairHoles() const;
	edfFile repairPhysEqual() const;

private:
	void adjustArraysByChannels();
	void fitData(int initSize);
	void cutZerosAtEnd();
	void adjustMarkerChannel();
	void adjustByData() { ndr = this->getDataLen() / (this->getFreq() * ddr); }

protected:
	/// location
	QString filePath{""};
	QString fileName{""};
	QString ExpName{""};
	QString dirPath{""};

	/// header stuff
	int bytes = 256;
	QString headerInitialInfo{};	/// why not QString or QByteArray ???
	QString headerReservedField{};
	QString headerRest{};

	std::vector<edfChannel> channels{};
	/// channels arrays start
	std::vector<QString> labels;
	std::vector<QString> transducerType;
	std::vector<QString> physDim;
	std::valarray<double> physMax;
	std::valarray<double> physMin;
	std::valarray<double> digMax;
	std::valarray<double> digMin;
	std::vector<QString> prefiltering;
	std::valarray<double> nr; /// it is int really
	std::vector<QString> reserved;
	std::vector<QString> annotations;
	/// channels arrays end

	/// cross EOG1-A2, EOG2-A1
	/// correspond EOG1-A1, EOG2-A2
	/// but really it is A*-EOG*
	/// EOG1 is on the right side (horizontal) and EOG2 on the left (vertical)
	eogType edfEogType = eogType::correspond; /// true story, 21.07.17

	/// data and my stuff
	int ns;
	int ndr;
	double ddr{1.}; /// good default
	double srate{250.};
	int markerChannel{-1};
	matrix edfData; /// matrix.cpp
	std::vector<std::valarray<double>> fftData{}; /// mutable?
	std::vector<std::pair<int, int>> markers{}; /// (time-bin, marker)

	/// some flags
	bool matiFlag{false};
	bool ntFlag{false};
	bool edfPlusFlag{false}; /// to detect
	bool filterIITPflag{true};
	bool writeMarkersFlag{true};
	bool writeLabelsFlag{false};
	bool writeHeaderFlag{false};
};

inline void myTransformRead(int & output, char * input)			{ output = atoi(input); }
inline void myTransformRead(double & output, char * input)		{ output = atof(input); }
inline void myTransformRead(QString & output, char * input)		{ output = QString(input); }

inline QString myTransformWrite(int input, int len)
{
	return myLib::fitNumber(input, len);
}
inline QString myTransformWrite(double input, int len)
{
	return myLib::fitNumber(input, len);
}
inline QString myTransformWrite(const QString & input, int len)
{
	return input + QString(' ', len - input.size());
}

template <typename Typ>
void handleParam(Typ & qStr,
				 int length,
				 bool readFlag,
				 std::fstream & ioFile,
				 std::fstream & headerFile)
{
	char * array;
	if(readFlag)
	{
		array = new char [length + 1];
		ioFile.read(array, length); array[length] = '\0';
		myTransformRead(qStr, array);
		if(headerFile.good())
		{
			headerFile.write(array, length);
		}
		delete []array;
	}
	else
	{
		QString outString = myTransformWrite(qStr, length);
		ioFile.write(outString.toStdString().c_str(), length);
	}
}

template <typename Typ>
void handleParamArray(std::vector<Typ> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile)
{
	if(readFlag)
	{
		qStr = std::vector<Typ>(number, Typ());
	}

	for(auto & in : qStr )
	{
		handleParam(in, length, readFlag, ioFile, headerFile);
	}
}

template <typename Typ>
void handleParamArray(std::valarray<Typ> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile)
{
	if(readFlag)
	{
		qStr = std::valarray<Typ>(number);
	}

	for(auto & in : qStr )
	{
		handleParam(in, length, readFlag, ioFile, headerFile);
	}
}





/// public "static" funcs to repair edfs or filenames
namespace repair
{
void addReference(const QString & inDirPath,
				  const QString & outDirPath,
				  const QString & ref);
bool testChannelsOrderConsistency(const QString & dirPath);
edfFile channelsOrderFile(const QString & inFilePath,
					   const std::vector<QString> & standard);
void channelsOrderDir(const QString & inDirPath,
                      const QString & outDirPath,
					  const std::vector<QString> & standard);

/// void invertEogs(const QString & inFilePath);
void holesFile(const QString & inFilePath, int numChan,
			   QString outFilePath = QString());
void holesDir(const QString & inDirPath,
			  int numChan,
			  const QString & outDirPath);

void physMinMaxCheck(const QString & dirPath);
void physMinMaxDir(const QString & dirPath, const QStringList & filters);


/// only renames files/dirs
void deleteSpacesItem(const QString & inPath);
void deleteSpacesFoldersOnly(const QString & dirPath);
void deleteSpacesContents(const QString & dirPath, const QStringList & filters);

void deleteNewContents(const QString & inPath);

void renameContents(const QString & dirPath, const std::vector<std::pair<QString, QString>> & temp);

void toLatinItem(const QString & inPath);
void toLatinContents(const QString & dirPath, const QStringList & filters);

void toLowerItem(const QString & inPath);
void toLowerContents(const QString & dirPath, const QStringList & filters);

void fullRepairDir(const QString & dirPath, const QStringList & filters);

edfFile addMarkerChannel(const QString & filePath);

void testArtifacts(const QString & dirPath, const QStringList & filters);
}

#endif /// EDFFILE_H
