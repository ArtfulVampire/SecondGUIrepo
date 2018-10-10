#include <other/edffile.h>

#include <myLib/mati.h>
#include <myLib/signalProcessing.h>
#include <myLib/drw.h>
#include <myLib/dataHandlers.h>

#include <bitset>

using namespace myOut;


std::list<std::valarray<double>> edfFile::getDataAsList() const
{
	std::list<std::valarray<double>> res(this->edfData.cols());
    uint j = 0;
    for(auto & in : res)
    {
		in = this->edfData.getCol(j++);
    }
    return res;
}


const std::valarray<double> & edfFile::getData(const QString & ch) const
{
	static std::valarray<double> badRes{};
	badRes = decltype(badRes){};
	auto a = this->findChannel(ch);
	if(a == -1)
	{
		std::cout << "edfFile::getData(QString): no such label - ";
		std::cout << ch << std::endl;
		return badRes;
	}
	return edfData[a];
}

std::vector<int> edfFile::getAllEegChannels(const std::vector<QString> & standard) const
{
	std::vector<int> res{};
	res.reserve(this->ns);
	for(int i = 0; i < this->ns; ++i)
	{
		if(labels[i].contains("EEG ", Qt::CaseInsensitive))
		{
			res.push_back(i);
			continue;
		}
		auto it = std::find_if(std::begin(standard),
							   std::end(standard),
							   [this, i](const QString & in)
		{
			return labels[i].contains(in);
		});
		if(it != std::end(standard))
		{
			res.push_back(i);
		}
	}
}

edfFile & edfFile::setDataFromList(const std::list<std::valarray<double>> & inList)
{
	this->edfData.resize(inList.front().size(), inList.size());
    uint col = 0;
    for(auto it = std::begin(inList); it != std::end(inList); ++it, ++col)
    {
        for(uint i = 0; i < (*it).size(); ++i)
        {
			this->edfData[i][col] = (*it)[i];
        }
    }
	return *this;
}


edfFile::edfFile(const edfFile &other, bool noData)
{
    this->filePath = other.getFilePath();
    this->ExpName = other.getExpName();
    this->dirPath = other.getDirPath();

    this->headerInitialInfo = other.getHeaderInit();
    this->bytes = other.getBytes();
    this->headerReservedField = other.getHeaderReserved();
	this->headerRest = other.getHeaderRest();

    this->ndr = other.getNdr();
    this->ddr = other.getDdr();
    this->ns = other.getNs();
	this->srate = other.getFreq();

    this->labels = other.getLabels();
    this->transducerType = other.getTransducer();
    this->physDim = other.getPhysDim();
    this->physMax = other.getPhysMax();
    this->physMin = other.getPhysMin();
    this->digMax = other.getDigMax();
    this->digMin = other.getDigMin();
    this->prefiltering = other.getPrefiltering();
    this->nr = other.getNr();
    this->reserved = other.getReserved();

	this->annotations = other.annotations;

	this->markers = other.getMarkers();
	this->markerChannel = other.getMarkChan();

    this->channels = other.getChannels();

    if(!noData)
    {
		this->edfData = other.getData();
		this->fftData = other.fftData;
    }
    else
    {
		this->edfData = matrix();
		this->fftData.clear();
    }

	this->matiFlag = other.matiFlag;
	this->ntFlag = other.ntFlag;
	this->edfPlusFlag = other.edfPlusFlag;
	this->filterIITPflag = other.filterIITPflag;

	this->writeMarkersFlag = other.writeMarkersFlag;
	this->writeLabelsFlag = other.writeLabelsFlag;
	this->writeHeaderFlag = other.writeHeaderFlag;
}

edfFile::edfFile(const QString & edfPath, bool headerOnly)
{
	this->readEdfFile(edfPath, headerOnly);
}

edfFile::edfFile(const QString & txtFilePath, inst which)
{
	if(which == inst::mati)
	{

		/// quant, time
		/// amplX, amplY, freqX, freqY
		/// targX, targY, mousX, mousY
		/// traceSuccessXY, secondBit
		/// right, wrong, skipped answers

		if(!QFile::exists(txtFilePath))
		{
			std::cout << "edfFile(matiLogFile): input file is NULL" << std::endl;
			return;
		}

		int numOfParams = 15 - 2; /// -currTime & quantLength generality
		int currTimeIndex;

		/// this->headerInitialInfo = myLib::fitString("Edf for AMOD Data", 184);
		this->headerInitialInfo = myLib::fitString("Edf for AMOD Data", 184);
		this->headerReservedField = myLib::fitString("headerReservedField", 44);
		this->headerRest = QString();


		this->filePath = txtFilePath;
		this->ExpName = myLib::getExpNameLib(txtFilePath);
		this->dirPath = txtFilePath.left(txtFilePath.lastIndexOf( "/" ) );

		this->ns = numOfParams;
		this->ddr = 1.;

		/// 250 freq generality DEFS
		this->nr = std::valarray<double> (250., this->ns);
		/// ndr definedlater
		this->bytes = 256 * (this->ns + 1);

		this->labels = {myLib::fitString("AMOD amplX", 16),
						myLib::fitString("AMOD amplY", 16),
						myLib::fitString("AMOD freqX", 16),
						myLib::fitString("AMOD freqY", 16),
						myLib::fitString("AMOD targX", 16),
						myLib::fitString("AMOD targY", 16),
						myLib::fitString("AMOD mouseX", 16),
						myLib::fitString("AMOD mouseX", 16),
						myLib::fitString("AMOD tracSucces", 16),
						myLib::fitString("AMOD mouseMove", 16),
						myLib::fitString("AMOD rightAns", 16),
						myLib::fitString("AMOD wrongAns", 16),
						myLib::fitString("AMOD skipdAns", 16)
					   };
		this->transducerType = std::vector<QString> (this->ns, myLib::fitString("AMOD transducer", 80));
		this->physDim = std::vector<QString> (this->ns, myLib::fitString("AMODdim", 8));

		this->physMin = {0, 0, 0, 0, /// ampls, freqs
						 -1, -1, -1, -1, /// coordinates
						 0, 0, /// status values
						 0,   0,   0}; /// answers

		this->physMax = {7, 7, 7, 7,
						 1,  1,  1,  1,
						 3, 7,
						 255, 255, 255};

		this->digMin = {-32768, -32768, -32768, -32768,
						-32768, -32768, -32768, -32768,
						0, 0,
						0,   0,   0};

		this->digMax = { 32767,  32767,  32767,  32767,
						 32767,  32767,  32767,  32767,
						 3-1, 7-1,
						 255-1, 255-1, 255-1}; /// -1 for universal formula except markers

		this->prefiltering = std::vector<QString> (this->ns, QString(myLib::fitString("AMOD no prefiltering", 80)));
		this->reserved = std::vector<QString> (this->ns, QString(myLib::fitString("AMOD reserved", 32)));

		this->edfData.resize(this->ns);
		for(int i = 0; i < this->ns; ++i)
		{
			/// 250 freq generality DEFS
			this->edfData[i].resize(6 * 60 * 25); /// for 6 minutes generality
		}

		/// remake with QFile
		FILE * inStr;
		inStr = fopen(txtFilePath.toStdString().c_str(), "r");

		currTimeIndex = 0;
		while(!feof(inStr))
		{
			fscanf(inStr, "%*d %*f"); /// quantLength & currTime
			for(int i = 0; i < numOfParams; ++i)
			{
				fscanf(inStr, "%lf", &(this->edfData[i][currTimeIndex]));
			}
			++currTimeIndex;
		}
		--currTimeIndex; /// to loose the last read string;

		this->fitData(currTimeIndex);

		for(int i = 0; i < this->ns; ++i)
		{
			this->channels.push_back(edfChannel(labels[i],
												transducerType[i],
												physDim[i],
												physMax[i],
												physMin[i],
												digMax[i],
												digMin[i],
												prefiltering[i],
												nr[i],
												reserved[i]
												)
									 );
		}
	}
	else if(which == inst::iitp)
	{
		std::vector<QString> iitpLabels{};
		myLib::readIITPfile(txtFilePath, this->edfData, iitpLabels);

		this->srate = 1000;

		int numOfParams = edfData.rows();

		this->headerInitialInfo = myLib::fitString("Edf for IITP EMG Data", 184);
		this->headerReservedField = myLib::fitString("headerReservedField", 44);
		this->headerRest = QString();

		this->filePath = txtFilePath;
		this->ExpName = myLib::getExpNameLib(txtFilePath);
		this->dirPath = myLib::getDirPathLib(txtFilePath);

		this->ns = numOfParams;
		this->ddr = 1.;
		////////////////

		this->nr = std::valarray<double> (this->srate, this->ns);
		/// ndr defined later
		this->bytes = 256 * (this->ns + 1);

		this->labels.resize(this->ns);
		for(int i = 0; i < this->ns; ++i)
		{
			iitpLabels[i] = iitpLabels[i].toLower();
			iitpLabels[i][0] = iitpLabels[i][0].toUpper(); /// First capital
			int s = iitpLabels[i].size();

			/// add _ if not _l or _r
			if(iitpLabels[i].contains(QRegExp("[lr]$")) &&
			   iitpLabels[i][s-2] != '_')
			{
				iitpLabels[i].insert(s-1, '_');
			}

			auto f = [ iitpLabels, i ](const QString & s) -> bool
			{ return iitpLabels[i].startsWith(s); };

			if(f("Ta") || f("Bf") || f("Fcr") || f("Ecr") || f("Da") || f("Dp"))
			{
				this->labels[i] = myLib::fitString("IT EMG " + iitpLabels[i], 16);
			}
			else if(!iitpLabels[i].contains("Artefac"))
			{
				this->labels[i] = myLib::fitString("IT " + iitpLabels[i], 16);
			}
			else
			{
				this->labels[i] = myLib::fitString("XX " + iitpLabels[i], 16);
			}
		}

		this->transducerType = std::vector<QString> (this->ns, myLib::fitString("IITP transducer", 80));
		this->physDim = std::vector<QString> (this->ns, myLib::fitString("IITPdim", 8));

		/// 2048 not enough for gonios sometimes
		this->physMin.resize(this->ns, -4096);
		this->physMax.resize(this->ns, 4096);

		this->digMin.resize(this->ns, -32768);
		this->digMax.resize(this->ns, 32768);

		this->prefiltering = std::vector<QString> (this->ns,
											   myLib::fitString("IITP no prefiltering", 80));
		this->reserved = std::vector<QString> (this->ns, myLib::fitString("IITP reserved", 32));

		this->fitData(this->edfData.cols());

		for(int i = 0; i < this->ns; ++i)
		{
			this->channels.push_back(edfChannel(labels[i],
												transducerType[i],
												physDim[i],
												physMax[i],
												physMin[i],
												digMax[i],
												digMin[i],
												prefiltering[i],
												nr[i],
												reserved[i]
												)
									 );
		}

		/// add markers channel
		this->edfData.push_back(std::valarray<double>(this->edfData.cols(), 0.));
		this->channels.push_back(edfChannel(myLib::fitString("Markers", 16),
											myLib::fitString("", 80),
											myLib::fitString("", 8),
											255,
											0,
											255,
											0,
											myLib::fitString("", 80),
											this->srate,
											myLib::fitString("", 32)
											)
								 );
		this->adjustArraysByChannels();

		/// mix channels
		std::vector<int> chanList;
		for(const QString & lab : {
			"Ta",
			"Bf",
			"Fcr",
			"Ecr",
			"Da",
			"Dp",
			"Ankle",
			"Knee",
			"Elbow",
			"Wrist"
	})
		{
			for(const QString & post : {"_l", "_r"})
			{
				auto i = this->findChannel(lab + post);
				if(i != -1)
				{
					chanList.push_back(i);
				}
			}
		}
		for(auto in : this->findChannels({"Artefac", "Marker"}))
		{
			chanList.push_back(in);
		}


		*this = this->reduceChannels(chanList);
	}
}


edfFile & edfFile::reOpen()
{
	return this->readEdfFile(this->filePath, false);
}

edfFile & edfFile::readEdfFile(const QString & EDFpath, bool headerOnly)
{
	*this = edfFile{}; //// pewpewpewpewpepew

    handleEdfFile(EDFpath, true, headerOnly);

	/// experimental, to deprecate
	DEFS.setNs(this->ns);
	DEFS.setFreq(srate);
	DEFS.setExpName(myLib::getExpNameLib(this->filePath, false));
	return *this;
}

void edfFile::rewriteEdfFile()
{
	this->handleEdfFile(this->getFilePath(), false);
}

void edfFile::writeEdfFile(const QString & EDFpath)
{
	if(*this == edfFile{}) { return; }

    QTime myTime;
    myTime.start();
	if(QFile::exists(EDFpath))
	{
//			std::cout << "writeEdfFile: file already exists, RETURN\n" << EDFpath << std::endl; return;
//			std::cout << "writeEdfFile: file already exists, REWRITE = \n" << EDFpath << " ";
//			std::cout << "writeEdfFile: file already exists, NAME += _rw" << std::endl; EDFpath.replace(".", "_rw.");
	}
	this->handleEdfFile(EDFpath, false);
#if 0
		/// old plain
		else /// if(asPLain)
		{
			myLib::writePlainData(EDFpath, this->edfData);
		}
#endif
}

/// readFlag: 1 - read, 0 - write
void edfFile::handleEdfFile(const QString & EDFpath, bool readFlag, bool headerOnly)
{
	/// a = a0 + (a1-a0) * (d-d0) / (d1-d0).

	/// 8 ascii : version of this data format (0)
	/// 80 ascii : local patient identification (mind item 3 of the additional EDF+ specs)
	/// 80 ascii : local recording identification (mind item 4 of the additional EDF+ specs)
	/// 8 ascii : startdate of recording (dd.mm.yy) (mind item 2 of the additional EDF+ specs)
	/// 8 ascii : starttime of recording (hh.mm.ss)
	/// 8 ascii : number of bytes in header record
	/// 44 ascii : reserved
	/// 236

	/// 8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs)
	/// 8 ascii : duration of a data record, in seconds
	/// 4 ascii : number of signals (ns) in data record
	/// 256

	/// ns * 16 ascii : ns * labels (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)
	/// ns * 80 ascii : ns * transducer type (e.g. AgAgCl electrode)
	/// ns * 8 ascii : ns * physical dimension (e.g. uV or degreeC)
	/// ns * 8 ascii : ns * physical minimum (e.g. -500 or 34)
	/// ns * 8 ascii : ns * physical maximum (e.g. 500 or 40)
	/// ns * 8 ascii : ns * digital minimum (e.g. -2048)
	/// ns * 8 ascii : ns * digital maximum (e.g. 2047)
	/// ns * 80 ascii : ns * prefiltering (e.g. HP:0.1Hz LP:75Hz)
	/// ns * 8 ascii : ns * nr of samples in each data record
	/// ns * 32 ascii : ns * reserved
	/// ns * 256

	/// Encephalan:
	/// Physical minimum: -4096   a0
	/// Physical maximum: 4096    a1
	/// Digital minimum: -32768  d0
	/// Digital maximum: 32767   d1


    QString helpString;
    if(readFlag && !QFile::exists(EDFpath))
    {
		std::cout << "handleFile: file to read doesn't exist\n" << EDFpath << std::endl;
        return;
    }

	std::fstream edfStream;
	if(readFlag)
	{ edfStream.open(EDFpath.toStdString(), std::ios_base::in | std::ios_base::binary); }
	else
	{ edfStream.open(EDFpath.toStdString(), std::ios_base::out | std::ios_base::binary); }

	if(!edfStream.good())
	{
		std::cout << "handleFile: cannot open edf file " << EDFpath << std::endl;
		return;
	}

	if(readFlag)
	{
		filePath = EDFpath;
		fileName = myLib::getFileName(EDFpath);
		dirPath = EDFpath.left(EDFpath.lastIndexOf('/'));
		ExpName = myLib::getExpNameLib(filePath);

		QFile::remove(dirPath + "/markers.txt"); /// delete markers file
	}

	std::fstream headerStream;
	if(readFlag && (writeHeaderFlag || headerOnly))
	{
		helpString = dirPath + "/header.txt";
		headerStream.open(helpString.toStdString(), std::ios_base::out);
		if(!headerStream.good())
		{
			std::cout << "edfFile::handleEdfFile: cannot open header.txt file" << std::endl;
			return;
		}
	}


    if(!readFlag)
    {
		this->fitData(edfData.cols());
    }

	handleParam(headerInitialInfo, 184, readFlag, edfStream, headerStream);
	handleParam(bytes, 8, readFlag, edfStream, headerStream);
	handleParam(headerReservedField, 44, readFlag, edfStream, headerStream);
	handleParam(ndr, 8, readFlag, edfStream, headerStream);
	handleParam(ddr, 8, readFlag, edfStream, headerStream);
	handleParam(ns, 4, readFlag, edfStream, headerStream);

	/// start channels read
	handleParamArray(labels, ns, 16, readFlag, edfStream, headerStream);

    /// generality for encephalan
	if(readFlag)
	{
		for(int i = 0; i < ns; ++i)
		{
			/// edit EOG channels to encephalan
			/// it is A1-EOG1 and A2-EOG2
			/// inversion is made in edfFile::reduceChannels(QString)
			if(labels[i].contains("EOG 1"))
			{
				if(this->edfEogType == eogType::cross)
				{
					labels[i] = myLib::fitString("EOG EOG1-A2", 16);
				}
				else if(this->edfEogType == eogType::correspond)
				{
					labels[i] = myLib::fitString("EOG EOG1-A1", 16);
				}
			}
			else if(labels[i].contains("EOG 2"))
			{
				if(this->edfEogType == eogType::cross)
				{
					labels[i] = myLib::fitString("EOG EOG2-A1", 16);
				}
				else if(this->edfEogType == eogType::correspond)
				{
					labels[i] = myLib::fitString("EOG EOG2-A2", 16);
				}
			}
			else if(labels[i].contains("EOG 3"))
			{
				labels[i] = myLib::fitString("EOG EOG3", 16);
			}
			/// set marker channel
			else if(labels[i].contains("Marker") ||
					labels[i].contains("Status"))
			{
				markerChannel = i;
				edfPlusFlag = false;
			}
			else if(labels[i].contains("Annotations"))
			{
				markerChannel = i;
				edfPlusFlag = true;
				std::cout << "handleEdfFile: Annotations! " << EDFpath << std::endl;
			}
			/// Mitsar and other sheet
			/// Need for repair::testChannelOrderConsistency
			/// essentially need for edfFile::refilter
			else
			{
				for(const QString & lbl : coords::lbl_all) /// most wide list of channels
				{
					if(labels[i].contains("EOG") &&
					   !labels[i].startsWith("EOG "))
					{
						labels[i].prepend("EOG ");
						continue;
					}
					if(labels[i].contains(lbl) &&
							!labels[i].contains(QRegExp("E[OC]G")) &&
							!labels[i].startsWith("EEG "))
					{
						labels[i].prepend("EEG ");
						continue;
					}
					if(labels[i].startsWith("Chan "))
					{
//						labels[i].replace("Chan ", "Chan");
						labels[i].prepend("EEG ");
						continue;
					}
					if(labels[i].contains(QRegExp(R"((PPG|FPG|SGR|KGR|Resp|RD))"))
							&& !labels[i].startsWith("POLY "))
					{
						labels[i].prepend("POLY ");
						continue;
					}
				}
			}
		}

		if(std::find_if(std::begin(labels),
						std::end(labels),
						[](const QString & in)
		{ return in.contains("Markers") || in.contains("Annotations") || in.contains("Status"); })
		   == std::end(labels))
		{
			this->writeMarkersFlag = false;
		}
	}

	if(readFlag && writeLabelsFlag)
    {
		helpString = dirPath + "/labels.txt";
		std::ofstream labelsStream(helpString.toStdString());
		if(labelsStream.good())
		{
			for(int i = 0; i < ns; ++i)
			{
				labelsStream << labels[i] << "\n";
			}
			labelsStream.flush();
			labelsStream.close();
		}
    }

	handleParamArray(transducerType, ns, 80, readFlag, edfStream, headerStream);
	handleParamArray(physDim, ns, 8, readFlag, edfStream, headerStream);

    if(!readFlag)
    {
        for(int i = 0; i < ns; ++i)
        {
			/// ECG for IITP
			if(labels[i].contains(QRegExp("E[EOC]G")))
            {
#if 0
				/// encephalan only !!!!!1111
				physMax[i] = 4096;
				physMin[i] = -4096;
				digMax[i] = 32767;
				digMin[i] = -32768;
#endif
            }
            /// repair for equal phys min/max
            if(physMin[i] == physMax[i])
            {
                physMax[i] = physMin[i] + 1.;
            }
        }
	}
	handleParamArray(physMin, ns, 8, readFlag, edfStream, headerStream);
	handleParamArray(physMax, ns, 8, readFlag, edfStream, headerStream);
	handleParamArray(digMin, ns, 8, readFlag, edfStream, headerStream);
	handleParamArray(digMax, ns, 8, readFlag, edfStream, headerStream);

	if(0)
	{
		if(readFlag)
		{
			for(int i = 0; i < ns; ++i)
			{
				if(labels[i].contains("EEG", Qt::CaseInsensitive))
				{
					if(physMax[i] == physMin[i])
					{
						std::cout << ExpName << "\t"
								  << "edfFile::readEdfFile: phys Max/Min are equal, chan(from 1) = "
								  << i + 1 << " - " << labels[i] << std::endl;
					}
					if(digMax[i] == digMin[i])
					{
						std::cout << ExpName << "\t"
								  << "edfFile::readEdfFile:  dig Max/Min are equal, chan(from 1) = "
								  << i + 1 << " - " << labels[i] << std::endl;
					}
				}
			}
		}
	}
	handleParamArray(prefiltering, ns, 80, readFlag, edfStream, headerStream);
	handleParamArray(nr, ns, 8, readFlag, edfStream, headerStream);

    /// real length handler
    if(readFlag)
    {
        /// olololololololololololololololo
        this->srate = std::round(nr[0] / ddr);
		const int64_t fileSize = QFile(EDFpath).size();
        const int sumNr = std::accumulate(std::begin(nr),
                                          std::end(nr),
                                          0.);
        const double realNdr = (fileSize - bytes)
                               / (sumNr * 2.); /// 2 bytes for a point
		if(static_cast<double>(static_cast<int>(realNdr)) != realNdr) /// fracPart == 0
        {
			std::cout << ExpName << ", ";
			std::cout << "handleEdfFile(read): realNdr is not integral = "
				 << realNdr << std::endl;
			std::cout << "ndr from file = " << ndr << std::endl;
			std::cout << "dataSize = " << fileSize - bytes << std::endl;
			std::cout << "Excessive data size (according to written ndr) = "
				 << (fileSize - bytes) - ndr * sumNr * 2.<< std::endl;
        }
///     ndr = std::min(static_cast<int>(realNdr), ndr); /// sometimes the tails are shit
		ndr = static_cast<int>(realNdr); /// sometimes ndr from file is a lie
    }

	handleParamArray(reserved, ns, 32, readFlag, edfStream, headerStream);
	/// end channels read

	handleParam(headerRest, bytes - (ns + 1) * 256, readFlag, edfStream, headerStream);

	/// files are already opened as binary, no need to reopen
	if(0)
	{
		/// headerStream will close by itself
		///
		auto pos = (readFlag ? edfStream.tellg() : edfStream.tellp());
		edfStream.close();
		edfStream.open(EDFpath.toStdString(),
					   std::ios_base::binary | std::ios_base::app |
					   (readFlag ? std::ios_base::in : std::ios_base::out));
		if(readFlag)	{ edfStream.seekg(pos); }
		else			{ edfStream.seekp(pos); }
	}

	if(readFlag) { annotations.clear(); }

	/// read write data
	handleData(readFlag, edfStream);
	edfStream.close();

	if(readFlag) { this->cutZerosAtEnd(); }

#if 0
	/// experimental annotations - just delete
	if(this->edfPlusFlag)
	{
		this->removeChannel(this->markerChannel);
		this->edfPlusFlag = false;
		this->markerChannel = -1;
	}
#endif

	/// ddr to 1
    {
        const double oldDdr = this->getDdr();
        ddr = 1.;
        for(double & nri : nr)
        {
            nri = std::round(nri / oldDdr);
        }
        for(edfChannel & ch : this->channels)
        {
            ch.nr = std::round(ch.nr / oldDdr);
        }
        ndr = std::ceil(ndr * oldDdr); /// ceil or round?
    }
}

void edfFile::handleData(bool readFlag,
						 std::fstream & edfForData)
{
	if(readFlag)
	{
		/// will fail when different frequencies. nr.max() could heal it
		this->edfData = matrix(ns, ndr * ddr * srate, 0.);
		this->channels.clear();
		for(int i = 0; i < this->ns; ++i)
		{
			this->channels.push_back(edfChannel(labels[i],
												transducerType[i],
												physDim[i],
												physMax[i],
												physMin[i],
												digMax[i],
												digMin[i],
												prefiltering[i],
												nr[i],
												reserved[i]
												)
									 );
		}
	}


	int annotationsWriteCounter = 0;
	for(int i = 0; i < ndr; ++i)
	{
		for(int currNs = 0; currNs < ns; ++currNs)
		{
			/// EDF+ annotations workaround
			if(!readFlag && this->edfPlusFlag && currNs == markerChannel)
			{
				edfForData.write(annotations[annotationsWriteCounter].toLatin1(),
								 nr[markerChannel] * 2);
				++annotationsWriteCounter;
				continue;
			}


			QString helpString;
			for(int k = 0; k < nr[currNs]; ++k)
			{
				int currTimeIndex = i * nr[currNs] + k;
				handleDatum(currNs, currTimeIndex, readFlag, helpString, edfForData);
				/// use annotations
			}
			if(currNs == markerChannel && this->edfPlusFlag)
			{
				annotations.push_back(helpString);
			}
		}
	}
	if(writeMarkersFlag)
	{
		if(edfPlusFlag)
		{
//			handleAnnotations();
			writeAnnotations();
		}
		else
		{
			writeMarkers();
		}
	}
}
void edfFile::handleDatum(int currNs,
						  int currTimeIndex,
						  bool readFlag,
						  QString & annotation,
						  std::fstream & edfForDatum)
{
	qint16 a = 0;
	quint16 markA = 0;

	double & currDatum = this->edfData[currNs][currTimeIndex];

	if(readFlag)
	{
		if(currNs != markerChannel) /// usual data read
		{
			edfForDatum.read(reinterpret_cast<char*>(&a), sizeof(qint16));

			currDatum = physMin[currNs]
						+ (static_cast<double>(a) - digMin[currNs])
						* (physMax[currNs] - physMin[currNs])
						/ (digMax[currNs] - digMin[currNs]
						   + (this->ntFlag ? 0 : 1));
			/// neurotravel + 0
			/// encephalan + 1

		}
		else /// if markers channel
		{
			if(this->edfPlusFlag)
			{
				/// edf+
				char helpChar;
				edfForDatum.read(&helpChar, 1);
				annotation += helpChar;
				edfForDatum.read(&helpChar, 1);
				annotation += helpChar;
			}
			else if(this->matiFlag)
			{
				edfForDatum.read(reinterpret_cast<char*>(&markA), sizeof(qint16));
				currDatum = physMin[currNs]
							+ (static_cast<double>(markA) - digMin[currNs])
							* (physMax[currNs] - physMin[currNs])
							/ (digMax[currNs] - digMin[currNs]);
//				currDatum = markA;

				if(currDatum != 0. )
				{
					myLib::matiFixMarker(currDatum);
				}
			}
			else /// simple edf
			{
				edfForDatum.read(reinterpret_cast<char*>(&a), sizeof(qint16));
				currDatum = physMin[currNs]
						+ (physMax[currNs] - physMin[currNs])
						* (static_cast<double>(a) - digMin[currNs])
						/ (digMax[currNs] - digMin[currNs]);

//				currDatum = a; /// generality encephalan
			}

			/// read marker into special array
			if(currDatum != 0.)
			{
				markers.push_back(std::make_pair(currTimeIndex, currDatum));
			}
		}
	}
	else /// if write
	{
		if(currNs != markerChannel) /// usual data write
		{
			/// round better to N * 1/8.
			if(currNs < 21) /// generality bicycle
			{
				currDatum = smLib::doubleRoundFraq
							(currDatum,
							 static_cast<int>( (digMax[currNs] - digMin[currNs] + 1)
								  / (physMax[currNs] - physMin[currNs]) )
							 ); /// need for eyes cleaned EEG only
			}

			a = (qint16)((currDatum - physMin[currNs])
						* (digMax[currNs] - digMin[currNs]
						   + (this->ntFlag ? 0 : 1))
						/ (physMax[currNs] - physMin[currNs])
						+ digMin[currNs]);
			edfForDatum.write(reinterpret_cast<char*>(&a), sizeof(quint16));
		}
		else /// if markers channel
		{
			if(this->edfPlusFlag) ////////////////////////// to do???
			{
				/// edf+ TO DO
			}
			else if(this->matiFlag)
			{
//				markA = (quint16) (currDatum);
				markA = (quint16)( (currDatum - physMin[currNs])
										  * (digMax[currNs] - digMin[currNs])
										  / (physMax[currNs] - physMin[currNs])
										  + digMin[currNs]);
				edfForDatum.write(reinterpret_cast<char*>(&markA), sizeof(quint16));
			}
			else /// simple edf
			{
//				a = (qint16) (currDatum);
				a = (qint16)( (currDatum - physMin[currNs])
							* (digMax[currNs] - digMin[currNs])
							/ (physMax[currNs] - physMin[currNs])
							+ digMin[currNs]);
				edfForDatum.write(reinterpret_cast<char*>(&a), sizeof(quint16));
			}
		}
	}
}

int edfFile::countMarker(int mrk) const
{
	return std::count_if(std::begin(this->markers),
						 std::end(this->markers),
						 [mrk](const std::pair<int, int> & in){ return in.second == mrk; });
}

std::vector<uint> edfFile::countMarkers(const std::vector<int> & mrks) const
{
	std::vector<uint> res(mrks.size(), 0);
	for(auto in : this->markers)
	{
		int a = myLib::indexOfVal(mrks, in.second);
		if(a < mrks.size())
		{
			++res[a];
		}
	}
	return res;
}

void edfFile::writeAnnotations() const
{
	std::ofstream annotStream((dirPath + "/" + ExpName + "_annots.txt").toStdString());
	for(const QString & annot : annotations)
	{
		auto a = annot.left(myLib::edfPlusLen(annot));
		for(QChar & ch : a)
		{
			if(ch.unicode() == 0 ||
			   ch.unicode() == 20 ||
			   ch.unicode() == 21) { ch = ' '; }
		}
		annotStream << a.toLatin1().toStdString() << std::endl;
	}
	annotStream.close();
}

void edfFile::writeMarkers() const
{
	std::ofstream markersStream((dirPath + "/" + "markers.txt").toStdString());
	std::ofstream taskMarkersStream((dirPath + "/" + "taskMarkers.txt").toStdString());

	for(const auto & mrk : markers)
	{
		markersStream << mrk.first << "\t"
					  << mrk.first / this->srate << "\t"
					  << mrk.second;

		if(mrk.second == 241 || mrk.second == 247 || mrk.second == 254)
		{
			taskMarkersStream << mrk.first << "\t"
							  << mrk.first / this->srate << "\t"
							  << mrk.second << "\r\n";
			if(mrk.second == 254)
			{
				taskMarkersStream << "\r\n";
			}
		}

		if(this->matiFlag)
		{
			std::bitset<16> byteMarker(mrk.second);

			markersStream << "\t";
			for(int s = byteMarker.size() - 1; s >= 0; --s)
			{
				markersStream << static_cast<int>(byteMarker[s]);
				if(s == 8) { markersStream << " "; } /// divide two bytes
			}
			if(byteMarker[9] || byteMarker[8])	{ markersStream << " - session start"; }
			if(byteMarker[10])					{ markersStream << " - session end"; }
			if(byteMarker[11])					{ markersStream << " - 11 bit error"; }
			if(byteMarker[12])					{ markersStream << " - 12 bit error"; }
		}
		markersStream << "\r\n";
	}
	markersStream.close();
	taskMarkersStream.close();
}

void edfFile::writeMarker(double currDatum,
						  int currTimeIndex) const
{
	std::vector<bool> byteMarker;

#if 0
	/// for FeedbackFinal
	if(currDatum == 1 || currDatum == 255) { return; }
#endif

	/// marker file name choose
	QString helpString = dirPath + "/"
//						 + this->ExpName + "_" /// for FeedbackFinal
						 + "markers.txt";

	std::ofstream markersStream(helpString.toStdString(), std::ios_base::app);
	markersStream << currTimeIndex << "\t"
				  << currTimeIndex / this->srate << "\t"
				  << currDatum;

    if(this->matiFlag)
    {
		byteMarker = myLib::matiCountByte(currDatum);

		markersStream << "\t";
		for(int s = 15; s >= 0; --s)
		{
			markersStream << byteMarker[s];
			if(s == 8)
			{
				markersStream << " ";
			}
		}
		if(byteMarker[10])
		{
			markersStream << " - session end";
		}
		else if(byteMarker[9] || byteMarker[8])
		{
			markersStream << " - session start";
		}
		if(byteMarker[12])
		{
			markersStream << " - 12 bit error";
		}
		if(byteMarker[11])
		{
			markersStream << " - 11 bit error";
		}
	}
	markersStream << "\n";
	markersStream.close();
}

/// time and string (discard duration and epoch start)
std::vector<std::pair<double, QString>> edfFile::handleAnnotation(const QString & annot) const
{
	const QChar ch00 = QChar(0);
	const QChar ch20 = QChar(20);
//	const QChar ch21 = QChar(21);
	const QString sep1 = QString(ch20) + QString(ch00);
	const QString sep2 = QString(ch20) + QString(ch20);

	std::vector<std::pair<double, QString>> res{};

	/// chop epoch time start
	int start{0};
	for(int i = 0; i < annot.size() - 1; ++i)
	{
		if(annot[i].unicode() == 20 && annot[i + 1].unicode() != 20) { start = i; break; }
	}
	QString tmp = annot.mid(start + 2);

	auto marks = tmp.split(sep1, QString::SkipEmptyParts);
	for(auto & mark : marks)
	{
		auto par = mark.split(sep2, QString::SkipEmptyParts);
		if(par.size() < 2 || par[1] != "stimPresentation") { continue; } /////////////////

		res.push_back({par[0].remove('+').toDouble(), par[1]});
	}
	return res;
}

std::vector<std::pair<double, QString>> edfFile::handleAnnotations() const
{
	std::vector<std::pair<double, QString>> res{};

	for(const QString & annot : annotations)
	{
		QString tmp = annot.left(myLib::edfPlusLen(annot));
		for(auto && in : handleAnnotation(tmp))
		{
			res.push_back(in);
		}
	}
	return res;
#if 0
	/// WTF was here?
	for(QString & annot : annotations)
	{
		auto charNum = [annot](int i) -> int { return int(annot.toStdString()[i]); }; /// ?????

		int currStart = 0;
		for(int l = 0; l < myLib::edfPlusLen(annot); ++l)
		{
			if(charNum(l) == 0 ||
			   (charNum(l) == 20 && (charNum(l + 1) == 0 || charNum(l + 1) == 20))
			   )
			{
				for(int p = currStart; p < l; ++p)
				{
					annotations[numOfAnn].append(helpString[p]);
				}
				++numOfAnn;

				while((charNum(l) == 0 || charNum(l) == 20) && l < myLib::edfPlusLen(annot))
				{
					++l;
				}
				currStart = l;
			}
		}
	}
    if(this->ntFlag)
    {
        double markTime;
        QString markValue;
		for(int j = 0; j < annotations.size(); ++j)
        {
			char * markNum = new char[60];
			markValue.clear();
			std::sscanf(annotations[j].toStdString().c_str(), "+%lf\24", &markTime);

			/// set time into helpString with 3 float numbers
			QString helpString{nm(markTime)};

//			if(helpString[helpString.length() - 3] == '.') { helpString.append("0"); }
//			else if(helpString[helpString.length() - 2] == '.') { helpString.append("00"); }
//			else helpString.append(".000");

			markValue += annotations[j].mid(helpString.size() + 2);/// +2 because of '+' and '\24'

			std::sscanf(annotations[j].toStdString().c_str(), "+%lf\24%s", &markTime, markNum);

			this->edfData[markerChannel][int(markTime * nr[markerChannel] / ddr)] = markNum;
			delete[] markNum;
        }
	}
	nr[ns - 1] = this->srate;
#endif
}

/// useful for reduceChannels
void edfFile::adjustArraysByChannels()
{
    this->ns = this->channels.size();
    this->bytes = 256 * (this->ns + 1);

    this->labels.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->labels.push_back(this->channels[i].label);
        if(this->channels[i].label.contains("Marker") ||
           this->channels[i].label.contains("Status"))
        {
            this->markerChannel = i; /// set markersChannel
			this->edfPlusFlag = false;
        }
        else if(this->channels[i].label.contains("Annotations"))
        {
            this->markerChannel = i; /// set markersChannel
			this->edfPlusFlag = true;
        }
    }

    this->transducerType.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->transducerType.push_back(this->channels[i].transducerType);
    }

    this->physDim.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->physDim.push_back(this->channels[i].physDim);
    }

    this->physMax.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->physMax[i] = this->channels[i].physMax;
    }

    this->physMin.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->physMin[i] = this->channels[i].physMin;
    }

    this->digMax.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->digMax[i] = this->channels[i].digMax;
    }

    this->digMin.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->digMin[i] = this->channels[i].digMin;
    }

    this->prefiltering.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->prefiltering.push_back(this->channels[i].prefiltering);
    }

    this->nr.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->nr[i] = this->channels[i].nr;
    }

    this->reserved.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->reserved.push_back(this->channels[i].reserved);
    }
	this->headerRest = QString();
}

void edfFile::adjustMarkerChannel()
{
	if(this->markerChannel == -1 || this->markerChannel == this->ns - 1) { return; }

    if(!(this->channels.back().label.contains("Marker") ||
         this->channels.back().label.contains("Status")))
	{
		/// backup marker channel
		edfChannel tempMarkChan = this->channels[this->markerChannel];
		std::valarray<double> tempMarkData = this->edfData[this->markerChannel];

		/// remove markerChannel
		this->channels.erase(this->channels.begin() + this->markerChannel);
		this->edfData.myData.erase(this->edfData.begin() + this->markerChannel);

		/// return markerChannel to the end of a list
		this->channels.push_back(tempMarkChan);
		this->edfData.push_back(tempMarkData);
    }
    this->adjustArraysByChannels();
}


edfFile edfFile::vertcatFile(const QString & addEdfPath) const
{
    edfFile temp(*this);
    edfFile addEdf;
    addEdf.readEdfFile(addEdfPath);

	edfChannel addChan;
	std::valarray<double> addData;

	double time1 = temp.getDataLen() / temp.getFreq();
	double time2 = addEdf.getDataLen() / addEdf.getFreq();

	double newTime = std::max(time1, time2);
	temp.edfData.resizeCols(newTime * temp.getFreq());
	addEdf.edfData.resizeCols(newTime * addEdf.getFreq());

    for(int i = 0; i < addEdf.getNs(); ++i)
    {
		addChan = addEdf.getChannels()[i];
		addData = addEdf.getData()[i];
		temp.channels.push_back(addChan);
		temp.edfData.push_back(addData);
	}
	temp.adjustMarkerChannel();
	temp.adjustArraysByChannels();
	return temp;
}

edfFile & edfFile::zeroChannels(const std::vector<int> & chanNums)
{
	for(int chan : chanNums)
	{
		edfData[chan] = 0.;
	}
	return *this;
}

edfFile & edfFile::divideChannel(uint chanNum, double denom)
{
	this->edfData[chanNum] /= denom;
	return *this;
}

edfFile & edfFile::divideChannels(std::vector<uint> chanNums, double denom)
{
	if(chanNums.empty())
	{
		std::cout << "edfFile::divideChannels: chanList is empty, divide all" << std::endl;
		for(int i = 0; i < this->getNs(); ++i)
		{
			if(i != this->markerChannel)
			{
				chanNums.push_back(i);
			}
		}
	}

	for(uint ind : chanNums)
	{
		this->edfData[ind] /= denom;
	}
	return *this;
}

edfFile & edfFile::multiplyChannel(uint chanNum, double mult)
{
	this->edfData[chanNum] *= mult;
	return *this;
}

edfFile & edfFile::multiplyChannels(std::vector<uint> chanNums, double mult)
{
	if(chanNums.empty())
	{
		std::cout << "edfFile::multiplyChannels: chanList is empty, multiply all" << std::endl;
		for(uint i = 0; i < this->getNs(); ++i)
		{
			if(i != this->markerChannel)
			{
				chanNums.push_back(i);
			}
		}
	}

	for(uint ind : chanNums)
	{
		this->edfData[ind] *= mult;
	}
	return *this;
}

int edfFile::findChannel(const QString & str) const
{
	for(int i = 0; i < this->ns; ++i)
	{
		if(labels[i].contains(str, Qt::CaseInsensitive)) { return i; }
	}
	return -1;
}

//std::vector<uint> edfFile::findChannels(const QString & filter) const
//{
//	std::vector<uint> res{};
//	for(int i = 0; i < this->ns; ++i)
//	{
//		if(labels[i].contains(filter, Qt::CaseInsensitive)) { res.push_back(i); }
//	}
//	return res;
//}

std::vector<int> edfFile::findChannels(const QString & filter) const
{
	std::vector<int> res{};
	for(int i = 0; i < this->ns; ++i)
	{
		if(labels[i].contains(filter, Qt::CaseInsensitive)) { res.push_back(i); }
	}
	return res;
}

std::vector<int> edfFile::findChannels(const std::vector<QString> & strs) const
{
	std::vector<int> res{};
	for(int i = 0; i < this->ns; ++i)
	{
		for(const QString & str : strs)
		{
			if(labels[i].contains(str, Qt::CaseInsensitive))
			{
				res.push_back(i);
				break;
			}
		}
	}
	return res;
}

edfFile & edfFile::subtractMeans(const QString & outPath)
{
	for(int i = 0; i < this->ns; ++i)
	{
		this->edfData[i] -= smLib::mean(this->edfData[i]);
	}
	if(!outPath.isEmpty())
	{
		this->writeEdfFile(outPath);
	}
	return *this;
}

edfFile & edfFile::concatFile(const QString & addEdfPath)
{
    edfFile addEdf;
    addEdf.readEdfFile(addEdfPath);

	const int oldLen = this->getDataLen();
	const int addLen = addEdf.getDataLen();

	edfData.resizeCols( oldLen + addLen);


	for(int i = 0; i < std::min(this->ns, addEdf.getNs()); ++i)
	{
		std::copy(std::begin(addEdf.getData(i)),
				  std::end(addEdf.getData(i)),
				  std::begin(this->edfData[i]) + oldLen);
	}
	return (*this);
}


edfFile & edfFile::downsample(double newFreq,
							  std::vector<int> chanList)
{
	if(newFreq > srate) /// or not integer ratio
	{
		std::cout << "edfFile::downsample: wrong newFreq" << std::endl;
		return *this;
	}
	if(chanList.empty())
	{
		chanList.resize(this->ns);
		std::iota(std::begin(chanList), std::end(chanList), 0);

		/// not downsample markers channel - really needed?
		auto it = std::find(std::begin(chanList), std::end(chanList), this->markerChannel);
		if(it != std::end(chanList))
		{
			chanList.erase(it);
		}

	}
	for(int numChan : chanList)
	{
		if(getFreq(numChan) == newFreq) { continue; }

		edfData[numChan] = myLib::downsample(edfData[numChan],
											 getFreq(numChan),
											 newFreq);
		/// no need to adjustArraysByChannels
		nr[numChan] = newFreq;
		channels[numChan].nr = newFreq;
		srate = newFreq;
	}
	return *this;
}

edfFile & edfFile::upsample(double newFreq,
							std::vector<int> chanList)
{
	if(newFreq < srate) /// or not integer ratio
	{
		std::cout << "edfFile::downsample: wrong newFreq" << std::endl;
		return *this;
	}
	if(chanList.empty())
	{
		chanList.resize(this->ns);
		std::iota(std::begin(chanList), std::end(chanList), 0);

		/// not downsample markers channel - really needed?
		auto it = std::find(std::begin(chanList), std::end(chanList), this->markerChannel);
		if(it != std::end(chanList))
		{
			chanList.erase(it);
		}

	}
	for(int numChan : chanList)
	{
		if(nr[numChan] == newFreq) { continue; }

		edfData[numChan] = myLib::upsample(edfData[numChan],
										   getFreq(numChan),
										   newFreq);
		/// no need to adjustArraysByChannels
		nr[numChan] = newFreq;
		channels[numChan].nr = newFreq;
	}
	srate = newFreq;
	return *this;
}

int edfFile::findJump(int channel,
					  int startPoint,
					  double numSigmas) const
{
	return myLib::findJump(edfData[channel], startPoint, numSigmas);
}

edfFile & edfFile::iitpSyncAutoCorr(int startSearchEeg,
									int startEmg,
									bool byEeg)
{
	const int searchLength = 300;
	const int corrLength = 1600;

	auto numECG = this->findChannel("ECG");
	auto numArtefac = this->findChannel("Artefac");

	if(byEeg) { numECG = 0; }

	const std::valarray<double> & eegMarkChan = this->getData()[numECG];
	const std::valarray<double> & emgMarkChan = this->getData()[numArtefac];

	std::valarray<double> eegMarkSignal(corrLength);
	std::valarray<double> emgMarkSignal(corrLength);

	std::copy(std::begin(emgMarkChan) + startEmg,
			  std::begin(emgMarkChan) + startEmg + corrLength,
			  std::begin(emgMarkSignal));

	int offset = 0;
	double maxCorr = 0.;
	for(int searchOffset = 0; searchOffset < searchLength; ++searchOffset)
	{
		std::copy(std::begin(eegMarkChan) + startSearchEeg + searchOffset,
				  std::begin(eegMarkChan) + startSearchEeg + searchOffset + corrLength,
				  std::begin(eegMarkSignal));

		double a = smLib::correlation(eegMarkSignal, emgMarkSignal);



		if(std::abs(a) >= maxCorr)
		{
//			std::cout << ">" << std::endl;
			maxCorr = std::abs(a);
			offset = searchOffset + startSearchEeg;
		}		
		if(std::abs(a) > 0.99)
		{
			break;
		}
	}
	std::cout << "maxCorr = " << maxCorr << std::endl;
	std::cout << "emgPoint = " << startEmg << '\t' << "eegPoint = " << offset << std::endl;
	this->iitpSyncManual(offset, startEmg, 200);
	return *this;
}

std::pair<int, int> edfFile::iitpSyncAutoJump(int startSearchEeg,
											  int startSearchEmg,
											  bool byEeg)
{
	auto numECG = this->findChannel("ECG");
	auto numArtefac = this->findChannel("Artefac");

	if(byEeg) { numECG = 0; }

	const std::valarray<double> & eegMarkChan = this->getData()[numECG];
	const std::valarray<double> & emgMarkChan = this->getData()[numArtefac];

	int eegPoint = myLib::findJump(eegMarkChan, startSearchEeg);
	int emgPoint = myLib::findJump(emgMarkChan, startSearchEmg);

	return std::make_pair(emgPoint, eegPoint);
}

edfFile & edfFile::iitpSyncManual(int offsetEeg,
								  int offsetEmg,
								  int addLeft)
{
	for(int i = 0; i < this->getNs(); ++i)
	{
		if(!(labels[i].startsWith("IT ") || labels[i].startsWith("XX "))) /// XX Artefac
		{
			this->edfData[i] = smLib::contPopFront(this->edfData[i], offsetEeg - addLeft);
		}
		else
		{
			this->edfData[i] = smLib::contPopFront(this->edfData[i], offsetEmg - addLeft);
		}
	}

	return *this;
}


void edfFile::countFft()
{
    std::vector<int> chanList;
    for(int i = 0; i < this->ns; ++i)
    {
		/// ECG for IITP
		if(this->labels[i].contains(QRegExp("E[OEC]G"))) /// filter only EEG, EOG and ECG signals
        {
            chanList.push_back(i);
        }
    }
    this->fftData.resize(chanList.size());
	int i = 0;
    for(auto j : chanList)
	{
		/// new rewritten
		fftData[i] = myLib::spectreRtoC(this->edfData[j]); /// shouldn't norm?
		++i;
	}
}

edfFile & edfFile::refilter(double lowFreq,
							double highFreq,
							bool isNotch,
							std::vector<int> chanList)
{
	if(chanList.empty())
	{
		for(int i = 0; i < this->ns; ++i)
		{
			/// filter only EEG, EOG signals - look labels!!!!
			/// pewpew IITP - no filter ECG
			if(this->labels[i].contains(QRegExp("E[OE]G "))
			   || (this->filterIITPflag && this->labels[i].startsWith("IT "))
			   || this->labels[i].startsWith("POLY ")
//			   || this->labels[i].startsWith("XX ") /// Artefac IITP channel
			   )
			{
				chanList.push_back(i);
			}
		}
	}

	for(int j : chanList)
	{
		this->edfData[j] = myLib::refilter(this->edfData[j],
										   lowFreq,
										   highFreq,
										   isNotch,
										   this->getNr()[j] / this->getDdr());
	}
	return *this;
}

edfFile edfFile::rereferenceData(reference newRef,
								 bool eogAsIs,
								 bool bipolarEog12) const
{
	if(newRef == reference::CAR)
	{
		return this->rereferenceDataCAR();
	}
	else if(newRef == reference::Cz)
	{
		return this->rereferenceDataCz();
	}

	/// A1, A2, Ar, N
	/// A1-A2, A1-N
	/// Ar means 0.5*(A1+A2)

	edfFile temp(*this);

	int groundChan = -1;	/// A1-N
	int earsChan1 = -1;		/// A1-A2
	int earsChan2 = -1;		/// A2-A1
	int eog1 = -1;			/// EOG1
	int eog2 = -1;			/// EOG2

	for(int i = 0; i < temp.ns; ++i)
	{
		if(temp.labels[i].contains("A1-N"))			{ groundChan = i; }
		else if(temp.labels[i].contains("A1-A2"))	{ earsChan1 = i; }
		else if(temp.labels[i].contains("A2-A1"))	{ earsChan2 = i; }
		else if(temp.labels[i].contains("EOG1"))	{ eog1 = i; }
		else if(temp.labels[i].contains("EOG2"))	{ eog2 = i; }
	}
	if(groundChan == -1 || (earsChan1 == -1 && earsChan2 == -1))
	{
		std::cout << "edfFile::rereferenceData: some of ref channels are absent" << std::endl;
		return {};
	}

	int earsChan;
	std::vector<QString> sign;
	if(earsChan1 != -1)
	{
		earsChan = earsChan1;
		sign = {"-", "+"};
	}
	else
	{
		earsChan = earsChan2;
		sign = {"+", "-"};
	}

	const QString earsChanStr = nm(earsChan + 1);
	const QString groundChanStr = nm(groundChan + 1);

	QString helpString{};
	for(int i = 0; i < temp.ns; ++i)
	{
		const QString currNumStr = nm(i + 1);

		if(i == groundChan || i == earsChan1 || i == earsChan2) /// reref chans
		{
			helpString += currNumStr + " ";
		}
		else if(!temp.labels[i].contains(QRegExp("E[EO]G"))) /// not EOG, not EEG
		{
			helpString += currNumStr + " ";
		}
		else if(temp.labels[i].contains("EOG") && eogAsIs)
		{
			helpString += currNumStr + " ";
		}
		else if(temp.labels[i].contains("EOG") && bipolarEog12)
		{
			if(temp.labels[i].contains("EOG1")) { /* do nothing */ }
			else if(temp.labels[i].contains("EOG2")) /// make bipolar EOG1-EOG2
			{
				/// EOG inversion is made in edfFile::reduceChannels
				/// here deal with them like EOG*-A*

				if(temp.edfEogType== eogType::cross)
				{
					/// (EOG1-A2) - (EOG2-A1) - (A1-A2)
					helpString += nm(eog1 + 1) + "-" + nm(eog2 + 1) + sign[0] + nm(earsChan + 1) + " ";
				}
				else if(temp.edfEogType== eogType::correspond)
				{
					/// (EOG1-A1) - (EOG2-A2) + (A1-A2)
					helpString += nm(eog1 + 1) + "-" + nm(eog2 + 1) + sign[1] + nm(earsChan + 1) + " ";
				}
			}
			else { helpString += currNumStr + " "; }
		}
		else /// EEG and usual EOG
		{
			/// define current ref
			QRegExp forRef(R"([\-].{1,4}[ ])");
			forRef.indexIn(temp.labels[i]);
			QString refName = forRef.cap();
			refName.remove(QRegExp(R"([\-\s])"));

			/// if no reference found - leave as is
			if(strToRef.count(refName) == 0) { helpString += currNumStr + " "; continue; }

			QString chanName = myLib::getLabelName(temp.labels[i]);

			reference targetRef = newRef;
			if(newRef == reference::Base)
			{
				if(std::find(std::begin(coords::lbl_A1),
							 std::end(coords::lbl_A1),
							 chanName) != std::end(coords::lbl_A1))
				{
					targetRef = reference::A1;
				}
				else
				{
					targetRef = reference::A2;
				}
			}
			helpString += edfFile::rerefChannel(strToRef.at(refName),
												targetRef,
												currNumStr,
												earsChanStr,
												groundChanStr,
												sign) + " ";
			temp.labels[i].replace(refName, refToStr.at(targetRef));
		}
	}

	/// the very processing
	temp = this->reduceChannels(helpString);

	/// fix EOG1-EOG2 label when bipolar
	/// generality
	if(bipolarEog12)
	{
		/// erase EOG1-A1
		temp.labels.erase(std::find_if(std::begin(temp.labels),
									   std::end(temp.labels),
									   [](const QString & in)
		{ return in.contains("EOG1-"); }));

		/// insert EOG1-EOG2
		temp.labels.insert(std::find_if(std::begin(temp.labels),
										std::end(temp.labels),
										[](const QString & in)
		{ return in.contains("EOG2-"); }),
						   myLib::fitString("EOG EOG1-EOG2", 16));

		/// erase EOG2-A2
		temp.labels.erase(std::find_if(std::begin(temp.labels),
									   std::end(temp.labels),
									   [](const QString & in)
		{ return in.contains("EOG2-"); }));
	}

	/// inverse EOG2-EOG1 back (look edfFile::reduceChannels near the end)
	if(int a = temp.findChannel("EOG1-EOG2") != -1)
	{
		temp.multiplyChannel(a, -1.);
	}
	else
	{
//		std::cout << "edfFile::rereferenceData: no bipolar EOG" << std::endl;
	}
	return temp;
}

/// need check
edfFile edfFile::rereferenceDataCAR() const
{
	edfFile temp(*this, false);

	/// check the same reference
	/// if not - reref to N
	temp = this->rereferenceData(reference::N, false, false);

	const std::vector<int> eegs
	{(ns < coords::egi::manyChannels) ? this->getAllEegChannels(coords::lbl19) : smLib::range<std::vector<int>>(0, 128)};
	const std::vector<QString> rerefLabels =
	{(ns < coords::egi::manyChannels) ? coords::lbl21 : smLib::range<std::vector<QString>>(1, 129)};	/// list to reref (with EOG)

	/// refArr = (Fp1 + Fp2 + ... + O1 + O2)/19 - N
	std::valarray<double> refArr(temp.edfData.cols());
	for(int chanNum : eegs)
	{
		refArr += temp[chanNum];
	}
	refArr /= eegs.size();

	for(int i = 0; i < temp.ns; ++i)
	{
		auto it = std::find_if(std::begin(rerefLabels),
							   std::end(rerefLabels),
							   [&temp, i](const QString & in)
		{ return temp.labels[i].contains(in); });

		if(it == std::end(rerefLabels)) { continue; }

		if(!(*it).contains("EOG")) /// non-EOGs
		{
			temp.edfData[i] -= refArr;
		}
		else
		{
			/// N-EOG1, N-EOG2
			/// crutch because inversed EOG
			temp.edfData[i] += refArr;
		}

		/// set new label *-CAR
		temp.labels[i] = myLib::fitString(temp.labels[i]
										  .left(temp.labels[i].indexOf('-'))
										  + "-CAR",
										  16);
	}
	return temp;
}

edfFile edfFile::rereferenceDataCz() const
{
	/// don't work for Geodesics
	if(this->ns > coords::egi::manyChannels)
	{
		std::cout << "edfFile::rereferenceDataCz: intentionally doesn't work" << std::endl;
		return *this;
	}

	edfFile temp(*this, false);

	/// check the same reference
	/// if not - reref to N
	temp = this->rereferenceData(reference::N, false, false);

	const std::vector<int> eegs = this->getAllEegChannels(coords::lbl19);
	const std::vector<QString> & rerefLabels{coords::lbl21};	/// list to reref (with EOG)

	/// Encephalan
	const std::valarray<double> refArr = this->getData("Cz");

	for(int i = 0; i < temp.ns; ++i)
	{
		auto it = std::find_if(std::begin(rerefLabels),
							   std::end(rerefLabels),
							   [&temp, i](const QString & in)
		{ return temp.labels[i].contains(in); });

		if(it == std::end(rerefLabels)) { continue; }

		if(!(*it).contains("EOG")) /// non-EOGs
		{
			temp.edfData[i] -= refArr;
		}
		else
		{
			/// N-EOG1, N-EOG2
			/// crutch because inversed EOG
			temp.edfData[i] += refArr;
		}

		/// set new label *-CAR
		temp.labels[i] = myLib::fitString(temp.labels[i]
										  .left(temp.labels[i].indexOf('-'))
										  + "-Cz",
										  16);
	}
	return temp;
}

void edfFile::saveSubsection(int startBin,
							 int finishBin,
							 const QString & outPath) const /// [start, finish)
{
#if 0
	/// old plain 26-Sep-2018
	myLib::writePlainData(outPath,
						  this->edfData.subCols(startBin,
												finishBin)
						  );
#endif

	edfFile temp(*this, true);

	temp.edfData.resize(this->ns, finishBin - startBin);
	for(int i = 0; i < this->ns; ++i)
	{
		std::copy(std::begin(this->edfData[i]) + startBin,
				  std::begin(this->edfData[i]) + finishBin,
				  std::begin(temp.edfData[i]));
	}
	temp.writeEdfFile(outPath.endsWith(".edf", Qt::CaseInsensitive) ? outPath : (outPath + ".edf"));

}

void edfFile::drawSubsection(int startBin, int finishBin, const QString & outPath) const
{
	myLib::drw::drawEeg(this->edfData.subCols(startBin, finishBin),
						this->srate,
						myLib::drw::eegPicHeight).save(outPath, nullptr, 100);
}

edfFile & edfFile::cleanFromEyes(QString eyesPath,
								 bool removeEogChannels,
								 std::vector<uint> eegNums,
								 std::vector<uint> eogNums)
{
    QTime myTime;
    myTime.start();

    if(eyesPath.isEmpty())
    {
		eyesPath = this->dirPath + "/eyes.txt";
    }

	matrix coefs = myLib::readMatrixFile(eyesPath);

    if(eegNums.empty())
    {
        for(int i = 0; i < this->ns; ++i)
        {
			/// exclude A1-N and such,
			/// doubling with eyesProcessingStatic
			if(this->channels[i].label.contains("EEG"))
            {
                eegNums.push_back(i);
            }
			if(eegNums.size() == coefs.rows()) { break; } /// bicycle generality - only first 19
        }
    }
    if(eogNums.empty())
    {
        for(int i = 0; i < this->ns; ++i)
        {
            if(this->channels[i].label.contains("EOG"))
            {
                eogNums.push_back(i);
            }
        }
    }
	for(int i = 0; i < coefs.rows(); ++i)
    {
		for(int k = 0; k < coefs.cols(); ++k)
        {

			this->edfData[ eegNums[i] ] -= this->edfData [ eogNums[k] ] * coefs[i][k];
        }
    }
    if(removeEogChannels)
    {
        this->removeChannels(eogNums);
	}
	std::cout << "cleanFromEyes: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
	return *this;
}

edfFile edfFile::reduceChannels(const std::vector<int> & chanList) const /// much memory
{
	/// more general, much memory
	edfFile temp(*this, true);
	temp.channels.clear();
	temp.edfData.clear();

    for(int item : chanList)
    {
		if(item >= this->getNs())
		{
			std::cout << "edfFile::reduceChannels: inappropriate number in chanList, return *this" << std::endl;
			return *this;
		}
		temp.channels.push_back(this->channels[item]);
		temp.edfData.push_back(this->edfData[item]);
	}
	/// if the cycle worked well
	temp.adjustArraysByChannels();

	return temp;
}

edfFile & edfFile::removeChannels(const std::vector<int> & chanList)
{
	std::set<int, std::greater<>> excludeSet;
    for(int val : chanList)
	{
		if(val != -1) { excludeSet.emplace(val); }
    }

    for(int k : excludeSet)
	{
//		if(k == -1) continue; /// good for -1 in findChannel

		this->edfData.eraseRow(k);
        this->channels.erase(std::begin(this->channels) + k);
    }
    this->adjustArraysByChannels();
	return *this;
}

edfFile & edfFile::removeChannels(const std::vector<uint> & chanList)
{
	std::set<uint, std::greater<>> excludeSet;
	for(uint val : chanList)
	{
		if(val != -1) { excludeSet.emplace(val); }
	}

	for(uint k : excludeSet)
	{
//		if(k == -1) continue; /// good for -1 in findChannel

		this->edfData.eraseRow(k);
		this->channels.erase(std::begin(this->channels) + k);
	}
	this->adjustArraysByChannels();
	return *this;
}

edfFile & edfFile::removeChannels(const QStringList & chanList)
{
	std::set<int, std::greater<>> excludeSet;
	for(const QString & ch : chanList)
	{
		auto val = this->findChannel(ch);
		if(val != -1) { excludeSet.emplace(val); }
		else { std::cout << "edfFile::removeChannels(vector<QString>): bad channel "
						 << ch << std::endl; }
	}

	for(int k : excludeSet)
	{
		if(k < 0) { continue; } /// for -1 in findChannel

		this->edfData.eraseRow(k);
		this->channels.erase(std::begin(this->channels) + k);
	}
	this->adjustArraysByChannels();
	return *this;
}

edfFile & edfFile::removeChannel(int num)
{
	if(num < 0 || num > this->getNs() - 1)
	{
		std::cout << "edfFile::removeChannel(int): bad channel num " << num << std::endl;
	}
	else
	{
		this->edfData.eraseRow(num);
		this->channels.erase(std::begin(this->channels) + num);
		this->adjustArraysByChannels();
	}
	return *this;
}
edfFile & edfFile::removeChannel(const QString & nam)
{
//	return this->removeChannel(this->findChannel(nam)); /// for short

	auto num = this->findChannel(nam);
	if(num == -1)
	{
		std::cout << "edfFile::removeChannel(str): no such channel " << nam << std::endl;
	}
	else
	{
		this->edfData.eraseRow(num);
		this->channels.erase(std::begin(this->channels) + num);
		this->adjustArraysByChannels();
	}
	return *this;
}

edfFile edfFile::reduceChannels(const QString & chanString) const
{
    QTime myTime;
	myTime.start();

	QStringList leest = chanString.split(QRegExp("[,;\\s]"), QString::SkipEmptyParts);
    if(leest.last().toInt() - 1 != this->markerChannel)
    {
		std::cout << "edfFile::reduceChannels: warning - last is not marker" << std::endl;
		std::cout << leest.last().toInt() - 1 << "\t" << this->markerChannel << std::endl;
    }

	edfFile temp(*this, true);
	temp.edfData = matrix();
	temp.edfData.resize(leest.length());
	temp.channels.resize(leest.length());

	const QRegExp eogNotBipolar{R"(EOG[12]-(?!EOG))"};

    /// need write a check of channel sequence

	int itemCounter = 0;
	for(auto item : leest)
	{
		int accordNum = item.toInt() - 1;
		if(smLib::isInt(item)) /// just copy
		{
			temp.edfData[itemCounter] = this->edfData[accordNum];
			temp.channels[itemCounter] = this->channels[accordNum];
		}
		else if(item.contains(QRegExp(R"([\+\-\*\/])")))
		{
			int lengthCounter = 0; /// length of the expression in chars
			auto lst = item.split(QRegExp(R"([\+\-\*\/])"), QString::SkipEmptyParts);

			/// check that nums between operators
			for(const auto & in : lst)
			{
				if(!smLib::isInt(in))
				{
					std::cout << "edfFile::reduceChannels: NAN between operators, return *this" << std::endl;
					return *this;
				}
			}

			temp.channels[itemCounter] = this->channels[lst.front().toInt() - 1];
			temp.edfData[itemCounter] = this->edfData[lst.front().toInt() - 1];
			lengthCounter += lst.front().length();

			/// invert EOG channes, then invert back EOG inverse
			const bool eogNotBipolarFlag =
					this->labels[lst.front().toInt() - 1].contains(eogNotBipolar);
			if(eogNotBipolarFlag)
			{
				temp.edfData[itemCounter] *= -1.;
			}

			for(auto lstIter = std::begin(lst) + 1; lstIter != std::end(lst); ++lstIter)
			{
				double sign = 0.;
				if(item[lengthCounter] == '+')		{ sign = 1.; }
				else if(item[lengthCounter] == '-')	{ sign = -1.; }
				else /// this should never happen!
				{
					std::cout << "edfFile::reduceChannels: first sign is not + or -, return * this" << std::endl;
					return *this;
				}
				lengthCounter += 1; /// sign length
				lengthCounter += (*lstIter).length();

				/// check '/' and '*'
				if(item[lengthCounter] == '/')
				{
					sign /= (*(lstIter+1)).toDouble(); /// already checked for being int
				}
				else if(item[lengthCounter] == '*')
				{
					sign *= (*(lstIter+1)).toDouble(); /// already checked for being int
				}

				/// for bipolar EOG because of EOG inverse
				double eogSign = 1.;
				if(this->labels[(*lstIter).toInt() - 1].contains(eogNotBipolar))
				{
					eogSign = -1.;
				}

				temp.edfData[itemCounter] += sign * eogSign * this->edfData[(*lstIter).toInt() - 1];

				if(item[lengthCounter] == '/' || item[lengthCounter] == '*')
				{
					lengthCounter += 1 + (*lstIter+1).length(); /// sign and argument
					++lstIter;
				}
			}
			/// invert EOG back EOG inverse
			/// will inverse EOG1-EOG2
			if(eogNotBipolarFlag)
			{
				temp.edfData[itemCounter] *= -1.;
			}
		}
		else
		{
			std::cout << "edfFile::reduceChannels: unknown format of the string, return *this" << std::endl;
			return *this;
		}
		++itemCounter;
	}
	std::cout << "edfFile::reduceChannels: ns = " << leest.length() << " "
			  << "time = " << myTime.elapsed() / 1000. << " sec" << std::endl;

	temp.ns = leest.length();
	temp.adjustArraysByChannels();
	return temp;
}

void edfFile::setLabel(int i, const QString & inLabel)
{
	this->channels[i].label = inLabel;
	this->labels[i] = inLabel;
}

void edfFile::setLabels(const std::vector<QString> & inLabels)
{
    if(this->ns != inLabels.size())
    {
		std::cout << "edfFile::setLabels: inappropriate vector size "
			 << this->ns << " != " << inLabels.size() << std::endl;
        return;
    }
    for(int i = 0; i < this->ns; ++i)
    {
        this->channels[i].label = inLabels[i];
        this->labels[i] = inLabels[i];
	}
}

edfFile & edfFile::insertChannel(int num,
								 const std::valarray<double> & dat,
								 const edfChannel & ch)
{
	this->channels.insert(std::begin(this->channels) + num, ch);
	this->edfData.myData.insert(std::begin(this->edfData.myData) + num, dat);
	this->adjustArraysByChannels();
	return *this;
}

edfFile & edfFile::addChannel(const std::valarray<double> & dat, const edfChannel & ch)
{
	return this->insertChannel(this->getNs(), dat, ch);
}

/// exceptions
void edfFile::setChannels(const std::vector<edfChannel> & inChannels)
{
    for(uint i = 0; i < inChannels.size(); ++i)
    {
        this->channels[i] = inChannels[i];
    }
}


/// template

void edfFile::writeOtherData(const matrix & newData,
                             const QString & outPath,
							 std::vector<int> chanList) const
{
    edfFile temp(*this, true); /// copy-construct everything but data

    if(chanList.empty())
    {
		chanList.resize(newData.rows());
        std::iota(std::begin(chanList),
                  std::end(chanList),
                  0);
    }
	temp.edfData = matrix();
    temp.channels.clear();
    int num = 0;
    for(int item : chanList)
    {
        temp.channels.push_back( this->channels[item] );
		////// pewpwepwwpepwpewpepwepwpep
		temp.edfData.push_back( newData[num++] );
    }
    temp.adjustArraysByChannels(); /// set in particular ns = chanList.length();
    temp.writeEdfFile(outPath);
}

void edfFile::fitData(int initSize) /// append zeros to whole ndr's
{
	this->ndr = std::ceil(initSize / (this->srate * this->ddr)); /// generality
	this->edfData.resizeCols(ndr * ddr * srate);
}

void edfFile::cutZerosAtEnd() /// cut zeros when readEdf, before edfChannels are allocated
{
	int currEnd = this->edfData.cols() - 1;
    bool doFlag = true;

    while(1)
    {
        for(int j = 0; j < this->ns; ++j)
		{
            /// for neurotravel cleaning - generality with digmaxmin
			if(std::abs(this->edfData[j][currEnd - 1]) >= 30000)
			{
				break; /// do clean
			}

			if(this->edfData[j][currEnd - 1] != this->edfData[j][currEnd])
            {
                doFlag = false;
                break;
			}
        }
        if(doFlag)
        {
            --currEnd;
        }
        else
        {
            break;
        }
        doFlag = true;
	}
	this->edfData.resizeCols(currEnd + 1);

	this->ndr = std::ceil(this->edfData.cols() / (this->srate * this->ddr)); /// should be unchanged
}

double edfFile::checkDdr(const QString & inPath)
{
    if(!QFile::exists(inPath))
    {
		std::cout << "edfFile::checkDdr: file doesn't exist" << std::endl;
        return -1;
    }
	QFile inStr(inPath);
	inStr.open(QIODevice::ReadOnly|QIODevice::Text);
	inStr.skip(244); /// all before ddr
	return QString(inStr.read(8)).toInt();
}

bool edfFile::isRerefChannel(const QString & inLabel)
{
	for(const QString & str: {"A1-A2", "A2-A1", "A1-N", "A2-N"})
	{
		if(inLabel.contains(str)) { return true; }
	}
	return false;
}

edfFile edfFile::repairDataScaling(int denominator) const
{
	edfFile temp(*this);
	temp.edfData /= denominator;
	temp.physMax /= denominator;
	temp.physMin /= denominator;
	return temp;
}


edfFile edfFile::repairHoles() const
{
	static const double thr = 0.005;

	auto dataList = this->getDataAsList();

	for(auto it = std::begin(dataList); it != std::end(dataList); ++it)
	{
		std::valarray<double> & col = *it;
		int count = 0;

		for(uint i = 0; i < col.size(); ++i)
		{
			if(col[i] > (1. - thr) * this->getDigMax()[i] + thr * this->getDigMin()[i] ||
			   col[i] < (1. - thr) * this->getDigMin()[i] + thr * this->getDigMax()[i])
			{
				++count;
			}
		}
		if(count > 19)
		{
			it = dataList.erase(it);
			--it;
		}
	}
	if(dataList.size() == this->edfData.cols())
	{
		return *this;
	}
	/// else
	edfFile temp(*this);
	temp.setDataFromList(dataList);
	return temp;
}


edfFile edfFile::repairChannelsOrder(const std::vector<QString> & standard) const
{

	std::vector<int> reorderChanList{};
	for(const auto & stand : standard)
	{
		for(int j = 0; j < this->ns; ++j)
		{
			if(this->labels[j].contains(stand)) /// unique standards
			{
				reorderChanList.push_back(j);
				break;
			}
		}
	}

	/// fill the rest of channels
	for(int j = 0; j < this->ns; ++j)
	{
		if(std::find(std::begin(reorderChanList),
					 std::end(reorderChanList),
					 j)
		   == std::end(reorderChanList))
		{
			reorderChanList.push_back(j);
		}
	}

	std::vector<int> ident(this->ns);
	std::iota(std::begin(ident), std::end(ident), 0);

	if(reorderChanList == ident)
	{
		return *this;
	}
	/// else
	edfFile temp(*this);
	return temp.reduceChannels(reorderChanList);

}

edfFile edfFile::repairPhysEqual() const
{
	bool found = false;
	for(int i = 0; i < this->ns; ++i)
	{
		if(this->physMin[i] == this->physMax[i])
		{
			found = true;
			break;
		}
	}

	if(!found)
	{
		return *this;
	}
	/// else
	edfFile temp(*this);
	for(int i = 0; i < temp.ns; ++i)
	{
		if(temp.physMin[i] == temp.physMax[i])
		{
			temp.physMax[i] = temp.physMin[i] + 1;
		}
	}
	return temp;
}

void edfFile::transformEdfMatrix(const QString & inEdfPath,
                                 const matrix & matrixW,
                                 const QString & newEdfPath)
{
    QTime myTime;
    myTime.start();

    edfFile fil;
    fil.readEdfFile(inEdfPath);
	matrix newData = fil.getData();
	newData.eraseRow(fil.getMarkChan());	/// remove markers
	newData = matrixW * newData;
	newData.push_back(fil.getMarkArr());	/// return markers

    fil.writeOtherData(newData, newEdfPath);
	std::cout << "transformEdfMaps: time elapsed = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

QString edfFile::rerefChannel(reference initialRef,
							  reference targetRef,
							  const QString & currentNum,
							  const QString & earsChan,
							  const QString & groundChan,
							  const std::vector<QString> & sign)
{
	/// assume there is only one '-' char in channel label
	/// assume the name to be: * Name-Ref *
	/// length of channel and ref are less than 4
	switch(targetRef)
	{
	case reference::A1:
	{
		switch(initialRef)
		{
		case reference::A1:		{ break; /* can't get here */ }
		case reference::A2:		{ return currentNum + sign[0] + earsChan; }
		case reference::Ar:		{ return currentNum + sign[0] + earsChan + "/2"; }
		case reference::N:		{ return currentNum + "-" + groundChan; }
		default:				{ break; /* can't get here */ }
		}
		break;
	}
	case reference::A2:
	{
		switch(initialRef)
		{
		case reference::A1:		{ return currentNum + sign[1] + earsChan; }
		case reference::A2:		{ break; /* can't get here */ }
		case reference::Ar:		{ return currentNum + sign[1] + earsChan + "/2"; }
		case reference::N:		{ return currentNum + "-" + groundChan + sign[1] + earsChan; }
		default:				{ break; /* can't get here */ }
		}
		break;
	}
	case reference::N:
	{
		switch(initialRef)
		{
		case reference::A1:		{ return currentNum + "+" + groundChan; }
		case reference::A2:		{ return currentNum + sign[0] + earsChan + "+" + groundChan; }
		case reference::Ar:		{ return currentNum + sign[0] + earsChan + "/2"  + "+" + groundChan; }
		case reference::N:		{ break; /* can't get here */ }
		default:				{ break; /* can't get here */ }
		}
		break;
	}
	case reference::Ar:
	{
		switch(initialRef)
		{
		case reference::A1:		{ return currentNum + sign[1] + earsChan + "/2"; }
		case reference::A2:		{ return currentNum + sign[0] + earsChan + "/2"; }
		case reference::Ar:		{ break; /* can't get here */ }
		case reference::N:		{ return currentNum + "-" + groundChan + sign[1] + earsChan + "/2"; }
		default:				{ break; /* can't get here */ }
		}
		break;
	}
	default:				{ break; /* can't get here */ }
	}
	return currentNum;
}
