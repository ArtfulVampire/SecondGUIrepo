#include <other/edffile.h>

#include <myLib/mati.h>
#include <myLib/signalProcessing.h>
#include <myLib/drw.h>
#include <myLib/dataHandlers.h>

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

void edfFile::setDataFromList(const std::list<std::valarray<double>> & inList)
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

edfFile::edfFile(const QString & txtFilePath, inst which)
{
	if(which == inst::mati)
	{

		// quant, time
		// amplX, amplY, freqX, freqY
		// targX, targY, mousX, mousY
		// traceSuccessXY, secondBit
		// right, wrong, skipped answers

		FILE* inStr;
		inStr = fopen(txtFilePath, "r");

		if(inStr == NULL)
		{
			std::cout << "edfFile(matiLogFile): input file is NULL" << std::endl;
			return;
		}

		int numOfParams = 15 - 2; // -currTime & quantLength generality
		int currTimeIndex;

		//    this->headerInitialInfo = myLib::fitString("Edf for AMOD Data", 184);
		this->headerInitialInfo = myLib::fitString("Edf for AMOD Data", 184).toStdString();
		this->headerReservedField = myLib::fitString("headerReservedField", 44);
		this->headerRest = QString();


		this->filePath = txtFilePath;
		this->ExpName = myLib::getExpNameLib(txtFilePath);
		this->dirPath = txtFilePath.left(txtFilePath.lastIndexOf( "/" ) );

		this->ns = numOfParams;
		this->ddr = 1.;

		/// 250 freq generality DEFS
		this->nr = std::valarray<double> (250., this->ns);
		// ndr definedlater
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

		this->physMin = {0, 0, 0, 0, // ampls, freqs
						 -1, -1, -1, -1, // coordinates
						 0, 0, // status values
						 0,   0,   0}; // answers

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
						 255-1, 255-1, 255-1}; // -1 for universal formula except markers

		this->prefiltering = std::vector<QString> (this->ns, QString(myLib::fitString("AMOD no prefiltering", 80)));
		this->reserved = std::vector<QString> (this->ns, QString(myLib::fitString("AMOD reserved", 32)));

		this->edfData.resize(this->ns);
		for(int i = 0; i < this->ns; ++i)
		{
			/// 250 freq generality DEFS
			this->edfData[i].resize(6 * 60 * 25); // for 6 minutes generality
		}

		currTimeIndex = 0;
		while(!feof(inStr))
		{
			fscanf(inStr, "%*d %*f"); // quantLength & currTime
			for(int i = 0; i < numOfParams; ++i)
			{
				fscanf(inStr, "%lf", &(this->edfData[i][currTimeIndex]));
			}
			++currTimeIndex;
		}
		--currTimeIndex; // to loose the last read string;

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
		matrix iitpData{};
		std::vector<QString> iitpLabels{};
		myLib::readIITPfile(txtFilePath, iitpData, iitpLabels);

		this->srate = 1000;

		int numOfParams = iitpData.rows();

		this->headerInitialInfo = myLib::fitString("Edf for IITP EMG Data", 184).toStdString();
		this->headerReservedField = myLib::fitString("headerReservedField", 44);
		this->headerRest = QString();

		this->filePath = txtFilePath;
		this->ExpName = myLib::getExpNameLib(txtFilePath);
		this->dirPath = myLib::getDirPathLib(txtFilePath);

		this->ns = numOfParams;
		this->ddr = 1.;
		////////////////

		this->nr = std::valarray<double> (this->srate, this->ns);
		// ndr defined later
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

		this->physMin.resize(this->ns, -2048);
		this->physMax.resize(this->ns, 2048);

		this->digMin.resize(this->ns, -32768);
		this->digMax.resize(this->ns, 32768);

		this->prefiltering = std::vector<QString> (this->ns,
											   myLib::fitString("IITP no prefiltering", 80));
		this->reserved = std::vector<QString> (this->ns, myLib::fitString("IITP reserved", 32));

//		this->edfData = matrix();
		this->edfData = std::move(iitpData);
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
		for(QString lab : {
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
			for(QString post : {"_l", "_r"})
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

edfFile & edfFile::readEdfFile(QString EDFpath, bool headerOnly)
{
    QTime myTime;
    myTime.start();
//    this->fftData.clear(); /// crucial
//	this->markers.clear();
	*this = edfFile{}; /// pewpewpewpewpepw

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

void edfFile::writeEdfFile(QString EDFpath, bool asPlain)
{
	if(*this == edfFile{}) { return; }

    QTime myTime;
    myTime.start();
    if(!asPlain)
    {
        if(QFile::exists(EDFpath))
        {
//			std::cout << "writeEdfFile: destination file already exists, RETURN\n" << EDFpath << std::endl; return;
//			std::cout << "writeEdfFile: destination file already exists, REWRITE = \n" << EDFpath << " ";

//			std::cout << "writeEdfFile: destination file already exists, NAME += _rw" << std::endl;
//			EDFpath.replace(".", "_rw.");
        }
        this->handleEdfFile(EDFpath, false);
    }
    else // if(asPLain)
	{
		myLib::writePlainData(EDFpath, this->edfData);
    }
}

// readFlag: 1 - read, 0 - write
void edfFile::handleEdfFile(QString EDFpath, bool readFlag, bool headerOnly)
{
    //    a = a0 + (a1-a0) * (d-d0) / (d1-d0).

    //    8 ascii : version of this data format (0)
    //    80 ascii : local patient identification (mind item 3 of the additional EDF+ specs)
    //    80 ascii : local recording identification (mind item 4 of the additional EDF+ specs)
    //    8 ascii : startdate of recording (dd.mm.yy) (mind item 2 of the additional EDF+ specs)
    //    8 ascii : starttime of recording (hh.mm.ss)
    //    8 ascii : number of bytes in header record
    //    44 ascii : reserved
    //    236

    //    8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs)
    //    8 ascii : duration of a data record, in seconds
    //    4 ascii : number of signals (ns) in data record
    //    ns * 16 ascii : ns * labels (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)
    //    ns * 80 ascii : ns * transducer type (e.g. AgAgCl electrode)
    //    ns * 8 ascii : ns * physical dimension (e.g. uV or degreeC)
    //    ns * 8 ascii : ns * physical minimum (e.g. -500 or 34)
    //    ns * 8 ascii : ns * physical maximum (e.g. 500 or 40)
    //    ns * 8 ascii : ns * digital minimum (e.g. -2048)
    //    ns * 8 ascii : ns * digital maximum (e.g. 2047)
    //    ns * 80 ascii : ns * prefiltering (e.g. HP:0.1Hz LP:75Hz)
    //    ns * 8 ascii : ns * nr of samples in each data record
    //    ns * 32 ascii : ns * reserved

        //    Physical minimum: -4096   a0
        //    Physical maximum: 4096    a1
        //    Digital minimum: -32768  d0
        //    Digital maximum: 32767   d1



    QString helpString;
    if(readFlag && !QFile::exists(EDFpath))
    {
		std::cout << "handleFile: file to read doesn't exist\n" << EDFpath << std::endl;
        return;
    }

#if EDFSTREAM
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

#else
    FILE * edfDescriptor;
	edfDescriptor = fopen(EDFpath, (readFlag ? "r" : "w")); // generality
	if(edfDescriptor == NULL)
	{
		std::cout << "handleFile: cannot open edf file " << EDFpath << std::endl;
		return;
	}
#endif

	if(readFlag)
	{
		filePath = EDFpath;
		fileName = myLib::getFileName(EDFpath);
		dirPath = EDFpath.left(EDFpath.lastIndexOf('/'));
		ExpName = myLib::getExpNameLib(filePath);

		QFile::remove(dirPath + "/markers.txt"); /// delete markers file
	}

#if EDFSTREAM
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
#else

	FILE * header = NULL;
	if(readFlag && (writeHeaderFlag || headerOnly))
    {
		helpString = dirPath + "/header.txt";
        header = fopen(helpString, "w");
        if(header == NULL)
        {
			std::cout << "edfFile::handleEdfFile: cannot open header.txt file" << std::endl;
            return;
        }
    }
#endif


    if(!readFlag)
    {
		this->fitData(edfData.cols());
    }

#if EDFSTREAM
	handleParam(headerInitialInfo, 184, readFlag, edfStream, headerStream);
	handleParam(bytes, 8, readFlag, edfStream, headerStream);
	handleParam(headerReservedField, 44, readFlag, edfStream, headerStream);
	handleParam(ndr, 8, readFlag, edfStream, headerStream);
	handleParam(ddr, 8, readFlag, edfStream, headerStream);
	handleParam(ns, 4, readFlag, edfStream, headerStream);
#else
    handleParam(headerInitialInfo, 184, readFlag, edfDescriptor, header);
    handleParam(bytes, 8, readFlag, edfDescriptor, header);
    handleParam(headerReservedField, 44, readFlag, edfDescriptor, header);
    handleParam(ndr, 8, readFlag, edfDescriptor, header);
    handleParam(ddr, 8, readFlag, edfDescriptor, header);
    handleParam(ns, 4, readFlag, edfDescriptor, header);
#endif

	/// start channels read
#if EDFSTREAM
	handleParamArray(labels, ns, 16, readFlag, edfStream, headerStream);
#else
	handleParamArray(labels, ns, 16, readFlag, edfDescriptor, header);
#endif

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
//			else if(labels[i].contains("A1-A2"))
//			{
//				labels[i] = myLib::fitString("EEG A2-A1", 16);
//			}
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
				for(auto lbl : coords::lbl_all) /// most wide list of channels
				{
					if(labels[i].contains("EOG") &&
					   !labels[i].startsWith("EOG "))
					{
						labels[i].prepend("EOG ");
						continue;
					}
					else if(labels[i].contains(lbl) &&
							!labels[i].contains(QRegExp("E[OC]G")) &&
							!labels[i].startsWith("EEG "))
					{
						labels[i].prepend("EEG ");
						continue;
					}
				}
			}
		}

		if(std::find_if(std::begin(labels),
						std::end(labels),
						[](const QString & in)
		{
						if(
							in.contains("Markers")
							|| in.contains("Annotations")
							|| in.contains("Status")) { return true; }
						else { return false; }
	}) == std::end(labels))
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

#if EDFSTREAM
	handleParamArray(transducerType, ns, 80, readFlag, edfStream, headerStream);
	handleParamArray(physDim, ns, 8, readFlag, edfStream, headerStream);
#else
	handleParamArray(transducerType, ns, 80, readFlag, edfDescriptor, header);
	handleParamArray(physDim, ns, 8, readFlag, edfDescriptor, header);
#endif

    if(!readFlag)
    {
        for(int i = 0; i < ns; ++i)
        {
			/// ECG for IITP
			if(labels[i].contains(QRegExp("E[EOC]G")))
            {
                /// encephalan only !!!!!1111
//				physMax[i] = 4096;
//				physMin[i] = -4096;
//				digMax[i] = 32767;
//				digMin[i] = -32768;
            }
            /// repair for equal phys min/max
            if(physMin[i] == physMax[i])
            {
                physMax[i] = physMin[i] + 1.;
            }
        }
	}
#if EDFSTREAM
	handleParamArray(physMin, ns, 8, readFlag, edfStream, headerStream);
	handleParamArray(physMax, ns, 8, readFlag, edfStream, headerStream);
	handleParamArray(digMin, ns, 8, readFlag, edfStream, headerStream);
	handleParamArray(digMax, ns, 8, readFlag, edfStream, headerStream);
#else
    handleParamArray(physMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(physMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMax, ns, 8, readFlag, edfDescriptor, header);
#endif

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
#if EDFSTREAM
	handleParamArray(prefiltering, ns, 80, readFlag, edfStream, headerStream);
	handleParamArray(nr, ns, 8, readFlag, edfStream, headerStream);
#else
    handleParamArray(prefiltering, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(nr, ns, 8, readFlag, edfDescriptor, header);
#endif

    // real length handler
    if(readFlag)
    {
        /// olololololololololololololololo
        this->srate = std::round(nr[0] / ddr);
        const long long fileSize = QFile(EDFpath).size();
        const int sumNr = std::accumulate(std::begin(nr),
                                          std::end(nr),
                                          0.);
        const double realNdr = (fileSize - bytes)
                               / (sumNr * 2.); // 2 bytes for a point
        if(int(realNdr) != realNdr)
        {
			std::cout << ExpName << ", ";
			std::cout << "handleEdfFile(read): realNdr is not integral = "
				 << realNdr << std::endl;
			std::cout << "ndr from file = " << ndr << std::endl;
			std::cout << "dataSize = " << fileSize - bytes << std::endl;
			std::cout << "Excessive data size (according to written ndr) = "
				 << (fileSize - bytes) - ndr * sumNr * 2.<< std::endl;
        }
//        ndr = min(int(realNdr), ndr); /// sometimes the tails are shit
		ndr = int(realNdr); /// sometimes ndr from file is a lie
    }

#if EDFSTREAM
	handleParamArray(reserved, ns, 32, readFlag, edfStream, headerStream);
#else
	handleParamArray(reserved, ns, 32, readFlag, edfDescriptor, header);
#endif
	/// end channels read

#if EDFSTREAM
	handleParam(headerRest, int(bytes - (ns + 1) * 256), readFlag, edfStream, headerStream);
#else
	handleParam(headerRest, int(bytes - (ns + 1) * 256), readFlag, edfDescriptor, header);
#endif

	/// files are already opened as binary, no need to reopen
	if(0)
	{
#if !EDFSTREAM
		if(readFlag && (header != NULL))
		{
			fclose(header);
		}
		if(headerOnly) { fclose(edfDescriptor); return; }
		fclose(edfDescriptor);


		edfDescriptor = fopen(EDFpath, (readFlag ? "rb" : "ab"));
		fseek(edfDescriptor, bytes, SEEK_SET);
#else
		/// headerStream will close by itself
		///
		auto pos = (readFlag ? edfStream.tellg() : edfStream.tellp());
		edfStream.close();
		edfStream.open(EDFpath.toStdString(),
					   std::ios_base::binary | std::ios_base::app |
					   (readFlag ? std::ios_base::in : std::ios_base::out));
		if(readFlag)	{ edfStream.seekg(pos); }
		else			{ edfStream.seekp(pos); }
#endif
	}

	if(readFlag) { annotations.clear(); }

	/// read write data

#if EDFSTREAM
	handleData(readFlag, edfStream);
	edfStream.close();
#else
    handleData(readFlag, edfDescriptor);
	fclose(edfDescriptor);
#endif

	if(readFlag) { this->cutZerosAtEnd(); }

	/// experimental annotations - just delete
    if(this->edfPlusFlag)
    {
		this->removeChannels({this->markerChannel});
        this->edfPlusFlag = false;
        this->markerChannel = -1;
    }

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
	int currTimeIndex;
	QString helpString;
	if(readFlag)
	{
		/// fails when different frequencies. nr.max() could heal it
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
	for(int i = 0; i < ndr; ++i)
	{
		for(int currNs = 0; currNs < ns; ++currNs)
		{
			helpString.clear();
			for(int k = 0; k < nr[currNs]; ++k)
			{
				currTimeIndex = i * nr[currNs] + k;

				handleDatum(currNs, currTimeIndex, readFlag, helpString, edfForData);
				/// use annotations
			}
			if(currNs == markerChannel && this->edfPlusFlag)
			{
				annotations.push_back(helpString);
			}
		}
	}
}
void edfFile::handleDatum(int currNs,
						  int currTimeIndex,
						  bool readFlag,
						  QString & ntAnnot,
						  std::fstream & edfForDatum)
{
	char helpChar = '0';
	qint16 a = 0;
	quint16 markA = 0;

	double & currDatum = this->edfData[currNs][currTimeIndex];

	if(readFlag)
	{
		if(currNs != markerChannel) // usual data read
		{
			edfForDatum.read((char*)&a, sizeof(qint16));
//            fread(&a, sizeof(qint16), 1, edfForDatum);

			currDatum = physMin[currNs]
						+ (double(a) - digMin[currNs])
						* (physMax[currNs] - physMin[currNs])
						/ (digMax[currNs] - digMin[currNs]
						   + (this->ntFlag ? 0 : 1));
			/// neurotravel + 0
			/// encephalan + 1

		}
		else // if markers channel
		{
			if(this->edfPlusFlag)
			{
				// edf+
				edfForDatum >> helpChar;
//                fread(&helpChar, sizeof(char), 1, edfForDatum);
				ntAnnot += helpChar;

				edfForDatum >> helpChar;
//                fread(&helpChar, sizeof(char), 1, edfForDatum);
				ntAnnot += helpChar;
			}
			else if(this->matiFlag)
			{
				edfForDatum.read((char*)&markA, sizeof(qint16));
//                fread(&markA, sizeof(quint16), 1, edfForDatum);
				currDatum = physMin[currNs]
							+ (double(markA) - digMin[currNs])
							* (physMax[currNs] - physMin[currNs])
							/ (digMax[currNs] - digMin[currNs]);
//                currDatum = markA;

				if(currDatum != 0 )
				{
					myLib::matiFixMarker(currDatum);
				}
			}
			else // simple edf
			{
				edfForDatum.read((char*)&a, sizeof(qint16));
//                fread(&a, sizeof(qint16), 1, edfForDatum);
				currDatum = physMin[currNs]
						+ (physMax[currNs] - physMin[currNs])
						* (double(a) - digMin[currNs])
						/ (digMax[currNs] - digMin[currNs]);

//                currDatum = a; // generality encephalan
			}

			/// read marker into special array
			if(currDatum != 0.)
			{
				markers.push_back(std::make_pair(currTimeIndex, currDatum));

				if(writeMarkersFlag && !edfPlusFlag)
				{
					writeMarker(currDatum, currTimeIndex);
				}
			}
		}
	}
	else // if write
	{
		if(currNs != markerChannel) // usual data write
		{
			// round better to N * 1/8.
			if(currNs < 21) // generality bicycle
			{
				currDatum = smLib::doubleRoundFraq(currDatum,
													  int( (digMax[currNs] - digMin[currNs] + 1)
														   / (physMax[currNs] - physMin[currNs]) )
													  ); // need for eyes cleaned EEG only
			}

			a = (qint16)((currDatum - physMin[currNs])
						* (digMax[currNs] - digMin[currNs]
						   + (this->ntFlag ? 0 : 1))
						/ (physMax[currNs] - physMin[currNs])
						+ digMin[currNs]);
			edfForDatum.write((char*)&a, sizeof(quint16));
//            fwrite(&a, sizeof(qint16), 1, edfForDatum);
		}
		else // if markers channel
		{
			if(this->edfPlusFlag) ////////////////////////// to do???
			{
				// edf+
//                fwrite(&helpChar, sizeof(char), 1, edfDescriptor);
			}
			else if(this->matiFlag)
			{
//                markA = (quint16) (currDatum);
				markA = (quint16)( (currDatum - physMin[currNs])
										  * (digMax[currNs] - digMin[currNs])
										  / (physMax[currNs] - physMin[currNs])
										  + digMin[currNs]);
				edfForDatum.write((char*)&markA, sizeof(quint16));
//                fwrite(&markA, sizeof(quint16), 1, edfForDatum);
			}
			else // simple edf
			{
//                a = (qint16) (currDatum);
				a = (qint16)( (currDatum - physMin[currNs])
							* (digMax[currNs] - digMin[currNs])
							/ (physMax[currNs] - physMin[currNs])
							+ digMin[currNs]);
				edfForDatum.write((char*)&a, sizeof(quint16));
//                fwrite(&a, sizeof(qint16), 1, edfForDatum);
			}
		}
	}
}

uint edfFile::countMarker(int mrk) const
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

void edfFile::handleData(bool readFlag,
                         FILE * edfForData)
{
    int currTimeIndex;
    QString helpString;
    if(readFlag)
	{
		/// fails when different frequencies. nr.max() could heal it
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
    for(int i = 0; i < ndr; ++i)
    {
        for(int currNs = 0; currNs < ns; ++currNs)
        {
            helpString.clear();
            for(int k = 0; k < nr[currNs]; ++k)
            {
                currTimeIndex = i * nr[currNs] + k;
                handleDatum(currNs, currTimeIndex, readFlag, helpString, edfForData);
                /// use annotations
            }
            if(currNs == markerChannel && this->edfPlusFlag)
            {
                annotations.push_back(helpString);
            }
        }
    }
}

void edfFile::handleDatum(int currNs,
                          int currTimeIndex,
                          bool readFlag,
                          QString & ntAnnot,
                          FILE * edfForDatum)
{
    char helpChar = '0';
	qint16 a = 0;
	quint16 markA = 0;

	double & currDatum = this->edfData[currNs][currTimeIndex];

    if(readFlag)
    {
        if(currNs != markerChannel) // usual data read
        {
            fread(&a, sizeof(qint16), 1, edfForDatum);
            currDatum = physMin[currNs]
                    + (physMax[currNs] - physMin[currNs])
                    * (double(a) - digMin[currNs])
                    / (digMax[currNs] - digMin[currNs]
					   + (this->ntFlag ? 0 : 1));
            /// neurotravel + 0
            /// encephalan + 1

//            currDatum = a * 1./8.; // generality encephalan

        }
		else // if markers channel
        {
            if(this->edfPlusFlag)
            {
				// edf+
                fread(&helpChar, sizeof(char), 1, edfForDatum);
                ntAnnot += helpChar;
                fread(&helpChar, sizeof(char), 1, edfForDatum);
                ntAnnot += helpChar;
            }
            else if(this->matiFlag)
            {
                fread(&markA, sizeof(quint16), 1, edfForDatum);
                currDatum = physMin[currNs]
                        + (physMax[currNs] - physMin[currNs])
                        * (double(markA) - digMin[currNs])
                        / (digMax[currNs] - digMin[currNs]);
//                currDatum = markA;

                if(currDatum != 0 )
                {
					myLib::matiFixMarker(currDatum);
                }
            }
            else // simple edf
            {
                fread(&a, sizeof(qint16), 1, edfForDatum);
                currDatum = physMin[currNs]
                        + (physMax[currNs] - physMin[currNs])
                        * (double(a) - digMin[currNs])
                        / (digMax[currNs] - digMin[currNs]);

//                currDatum = a; // generality encephalan
            }
			if(writeMarkersFlag &&
			   !edfPlusFlag &&
               currDatum != 0) // make markers file when read only
            {
                writeMarker(currDatum, currTimeIndex);
            }
        }
    }
	else // if write
    {
        if(currNs != markerChannel) // usual data write
        {
            // round better to N * 1/8.
            if(currNs < 21) // generality bicycle
            {
				currDatum = smLib::doubleRoundFraq(currDatum,
													  int( (digMax[currNs] - digMin[currNs] + 1)
														   / (physMax[currNs] - physMin[currNs]) )
													  ); // need for eyes cleaned EEG only
            }

            a = (qint16)((currDatum - physMin[currNs])
                        * (digMax[currNs] - digMin[currNs]
						   + (this->ntFlag ? 0 : 1))
                        / (physMax[currNs] - physMin[currNs])
						+ digMin[currNs]);

            fwrite(&a, sizeof(qint16), 1, edfForDatum);
        }
		else // if markers channel
        {
            if(this->edfPlusFlag) ////////////////////////// to do???
            {
				// edf+
//                fwrite(&helpChar, sizeof(char), 1, edfDescriptor);
            }
            else if(this->matiFlag)
            {
//                markA = (quint16) (currDatum);
                markA = (quint16)( (currDatum - physMin[currNs])
                                          * (digMax[currNs] - digMin[currNs])
                                          / (physMax[currNs] - physMin[currNs])
                                          + digMin[currNs]);
                fwrite(&markA, sizeof(quint16), 1, edfForDatum);
            }
            else // simple edf
            {
//                a = (qint16) (currDatum);
                a = (qint16)( (currDatum - physMin[currNs])
                            * (digMax[currNs] - digMin[currNs])
                            / (physMax[currNs] - physMin[currNs])
                            + digMin[currNs]);
                fwrite(&a, sizeof(qint16), 1, edfForDatum);
            }
        }
    }
}


void edfFile::writeMarker(double currDatum,
						  int currTimeIndex) const
{
#define MARKERS_STREAM 01
	std::vector<bool> byteMarker;
	QString helpString;

//	if(currDatum == 1 || currDatum == 255) { return; } /// for FeedbackFinal

	/// marker file name choose
	helpString = dirPath + "/"
//				 + this->ExpName + "_" /// for FeedbackFinal
				 + "markers.txt";

#if MARKERS_STREAM
	std::ofstream markersStream(helpString.toStdString(), std::ios_base::app);
	markersStream << currTimeIndex << "\t"
				  << currTimeIndex / double(this->srate) << "\t"
				  << currDatum;
#else
	FILE * markers;
    markers = fopen(helpString, "a+");
    fprintf(markers, "%d %d", currTimeIndex, int(currDatum));
#endif


    if(this->matiFlag)
    {
		byteMarker = myLib::matiCountByte(currDatum);

#if MARKERS_STREAM
		markersStream << "\t";
#else
		fprintf(markers, "\t");
#endif
		for(int s = 15; s >= 0; --s)
		{
#if MARKERS_STREAM
			markersStream << byteMarker[s];
#else
			fprintf(markers, "%d", int(byteMarker[s]));
#endif
			if(s == 8)
			{
#if MARKERS_STREAM
				markersStream << " ";
#else
				fprintf(markers, " ");  // byte delimiter
#endif
			}
		}
		if(byteMarker[10])
		{
#if MARKERS_STREAM
			markersStream << " - session end";
#else
			fprintf(markers, " - session end");
#endif
		}
		else if(byteMarker[9] || byteMarker[8])
		{
#if MARKERS_STREAM
			markersStream << " - session start";
#else
			fprintf(markers, " - session start");
#endif
		}
		if(byteMarker[12])
		{
#if MARKERS_STREAM
			markersStream << " - 12 bit error";
#else
			fprintf(markers, " - 12 bit error");
#endif
		}
		if(byteMarker[11])
		{
#if MARKERS_STREAM
			markersStream << " - 11 bit error";
#else
			fprintf(markers, " - 11 bit error");
#endif
		}
    }
#if MARKERS_STREAM
	markersStream << "\n";
	markersStream.close();
#else
	fprintf(markers, "\n");
	fclose(markers);
#endif

}

void edfFile::handleAnnotations(int currNs,
                                int currentTimeIndex,
                                QString helpString,
								std::vector<QString> annotations)
{

//    return; // I dont care
    /*
    currStart = 0;
    for(int l = 0; l < len(helpString); ++l)
    {
        if(int(helpString.toStdString()[l])== 0 || (int(helpString.toStdString()[l])==20 && (int(helpString.toStdString()[l+1])== 0 || int(helpString.toStdString()[l+1])==20)))
        {
            for(int p=currStart; p < l; ++p)
            {
                annotations[numOfAnn].append(helpString[p]);
            }
            ++numOfAnn;
            while((int(helpString.toStdString()[l])== 0 || int(helpString.toStdString()[l])==20) && l < len(helpString)) ++l;
            currStart = l;
        }
    }
    // I dont care
    if(this->ntFlag)
    {
        double markTime;
        char * markNum = new char[60];
        QString markValue;
        for(int j = 0; j < numOfAnn; ++j)
        {
            markNum[0]='\0';
            markValue="";
            sscanf(annotations[j].toStdString().c_str(), "+%lf\24", &markTime);
			// set time into helpString with 3 float numbers
            helpString.setNum(markTime);
			if(helpString[helpString.length()-3]=='.') helpString.append("0"); // float part - 2 or 3 signs
            else
            {
                if(helpString[helpString.length()-2]=='.') helpString.append("00");
                else helpString.append(".000");
            }
			for(int i = helpString.length()+2; i < annotations[j].length(); ++i) // +2 because of '+' and '\24'
            {
                markValue.append(annotations[j][i]);
            }
            sscanf(annotations[j].toStdString().c_str(), "+%lf\24%s", &markTime, markNum);
            data[ns-1][int(markTime*nr[ns-1]/ddr)] = atoi(markNum);
        }
//        nr[ns-1] = this->srate; // generality.getFreq() change
    }
    */

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
            this->markerChannel = i; // set markersChannel
            this->edfPlusFlag = false;
//            break; // Markers channel - the last
        }
        else if(this->channels[i].label.contains("Annotations"))
        {
            this->markerChannel = i; // set markersChannel
            this->edfPlusFlag = true;
            //            break; // Markers channel - the last
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
	if(this->markerChannel == -1 || this->markerChannel == this->ns - 1) return;

    if(!(this->channels.back().label.contains("Marker") ||
         this->channels.back().label.contains("Status")))
	{
		// backup marker channel
		edfChannel tempMarkChan = this->channels[this->markerChannel];
		std::valarray<double> tempMarkData = this->edfData[this->markerChannel];

		// remove markerChannel
		this->channels.erase(this->channels.begin() + this->markerChannel);
		this->edfData.myData.erase(this->edfData.begin() + this->markerChannel);

		// return markerChannel to the end of a list
		this->channels.push_back(tempMarkChan);
		this->edfData.push_back(tempMarkData);
    }
    this->adjustArraysByChannels();
}


edfFile edfFile::vertcatFile(QString addEdfPath, QString outPath) const
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

	if(!outPath.isEmpty())
	{
		temp.writeEdfFile(outPath);
	}
	return temp;
}

edfFile & edfFile::zeroChannels(const std::vector<uint> & chanNums)
{
	for(uint chan : chanNums)
	{
		this->edfData[chan] = 0;
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

uint edfFile::findChannel(const QString & str) const
{
	for(int i = 0; i < this->ns; ++i)
	{
		if(labels[i].contains(str, Qt::CaseInsensitive)) return i;
	}
	return -1;
}

std::vector<uint> edfFile::findChannels(const QString & filter) const
{
	std::vector<uint> res{};
	for(int i = 0; i < this->ns; ++i)
	{
		if(labels[i].contains(filter, Qt::CaseInsensitive)) { res.push_back(i); }
	}
	return res;
}

std::vector<uint> edfFile::findChannels(const std::vector<QString> & strs) const
{
	std::vector<uint> res{};
	for(int i = 0; i < this->ns; ++i)
	{
		for(QString str : strs)
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

edfFile & edfFile::concatFile(QString addEdfPath, QString outPath) // assume only data concat
{
    edfFile addEdf;
    addEdf.readEdfFile(addEdfPath);

	const int oldLen = this->getDataLen();
	const int addLen = addEdf.getDataLen();

	edfData.resizeCols( oldLen + addLen);


	for(int i = 0; i < std::min(this->ns, addEdf.getNs()); ++i)
	{
		std::copy(std::begin(addEdf.getData()[i]),
				  std::end(addEdf.getData()[i]),
				  std::begin(this->edfData[i]) + oldLen);
	}
    /// remake
    if(!outPath.isEmpty())
    {
        this->writeEdfFile(outPath);
    }
	return (*this);
}


void edfFile::downsample(double newFreq,
						 QString outPath,
						 std::vector<int> chanList) const
{
	edfFile temp(*this);
	if(newFreq > temp.getFreq()) // or not integer ratio
	{
		std::cout << "edfFile::downsample: wrong newFreq" << std::endl;
		return;
	}
	if(chanList.empty())
	{
		chanList.resize(temp.ns);
		std::iota(std::begin(chanList), std::end(chanList), 0);

		/// not downsample markers channel - really needed?
//		auto it = std::find(std::begin(chanList), std::end(chanList), temp.markerChannel);
//		if(it != std::end(chanList))
//		{
//			chanList.erase(it);
//		}

	}
	for(int numChan : chanList)
	{
		if(nr[numChan] == newFreq) continue;

		temp.edfData[numChan] = myLib::downsample(temp.edfData[numChan],
												  temp.nr[numChan],
												  newFreq);
		/// no need to adjustArraysByChannels
		temp.nr[numChan] = newFreq;
		temp.channels[numChan].nr = newFreq;
		temp.srate = newFreq;
	}

	if(outPath.isEmpty())
	{
		outPath = temp.getFilePath();
		outPath.insert(outPath.lastIndexOf('.'), "_downsampled");
	}
	temp.writeEdfFile(outPath);
}

void edfFile::upsample(double newFreq,
					   QString outPath,
					   std::vector<int> chanList) const
{
	edfFile temp(*this);
	if(newFreq < temp.getFreq()) // or not integer ratio
	{
		std::cout << "edfFile::upsample: wrong newFreq" << std::endl;
		return;
	}
	if(chanList.empty())
	{
		chanList.resize(temp.ns);
		std::iota(std::begin(chanList), std::end(chanList), 0);

		/// really needed?
		auto it = std::find(std::begin(chanList), std::end(chanList), temp.markerChannel);
		if(it != std::end(chanList))
		{
			chanList.erase(it);
		}
	}
	for(int numChan : chanList)
	{
		if(nr[numChan] == newFreq) continue;

		temp.edfData[numChan] = myLib::upsample(temp.edfData[numChan],
												temp.nr[numChan],
												newFreq);
		/// no need to adjustArraysByChannels
		temp.nr[numChan] = newFreq;
		temp.channels[numChan].nr = newFreq;
		temp.srate = newFreq;
	}
	if(outPath.isEmpty())
	{
		outPath = temp.getFilePath();
		outPath.insert(outPath.lastIndexOf('.'), "_upsampled");
	}
	temp.writeEdfFile(outPath);
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

	if(byEeg) numECG = 0;

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

	if(byEeg) numECG = 0;

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
		if(this->labels[i].contains(QRegExp("E[OEC]G"))) // filter only EEG, EOG and ECG signals
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
		continue;
	}
}

edfFile & edfFile::refilter(double lowFreq,
							double highFreq,
							bool isNotch,
							std::vector<uint> chanList)
{
	if(chanList.empty())
	{
		for(int i = 0; i < this->ns; ++i)
		{
			/// filter only EEG, EOG signals - look labels!!!!
			/// pewpew IITP - no filter ECG
			if(this->labels[i].contains(QRegExp("E[OE]G"))
			   || (this->filterIITPflag && this->labels[i].startsWith("IT "))
//			   || this->labels[i].startsWith("XX ") // Artefac IITP channel
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

/// need check
edfFile edfFile::rereferenceData(const QString & newRef) const
{
	if(newRef.contains("CAR", Qt::CaseInsensitive))
	{
		return this->rereferenceDataCAR();
	}

	// A1, A2, Ar, N
	// A1-A2, A1-N
	// Ar means 0.5*(A1+A2)

	edfFile temp(*this);

	int groundChan = -1;	// A1-N
	int earsChan1 = -1;		// A1-A2
	int earsChan2 = -1;		// A2-A1
	int eog1 = -1;			// EOG1
	int eog2 = -1;			// EOG2

	bool eogAsIs = false;
	bool bipolarEog12 = false;

	for(int i = 0; i < temp.ns; ++i)
	{
		if(temp.labels[i].contains("A1-N"))		{ groundChan = i; }
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

	QString helpString;
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
			// define current ref
			QRegExp forRef(R"([\-].{1,4}[ ])");
			forRef.indexIn(temp.labels[i]);
			QString refName = forRef.cap();
			refName.remove(QRegExp(R"([\-\s])"));

			/// if no reference found - leave as is
			if(refName.isEmpty()) { helpString += currNumStr + " "; }

			QString chanName = myLib::getLabelName(temp.labels[i]);

			QString targetRef = newRef;

			if(newRef == "Base")
			{
				if(std::find(std::begin(coords::lbl_A1),
							 std::end(coords::lbl_A1),
							 chanName) != std::end(coords::lbl_A1))
				{
					targetRef = "A1";
				}
				else
				{
					targetRef = "A2";
				}
			}
			helpString += myLib::rerefChannel(refName,
											  targetRef,
											  currNumStr,
											  earsChanStr,
											  groundChanStr,
											  sign) + " ";
			temp.labels[i].replace(refName, targetRef);
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
	temp = this->rereferenceData("N");

	const auto & usedLabels = coords::lbl19;	/// to build reref array
	const auto & rerefLabels = coords::lbl21;	/// list to reref (with EOG)

	/// refArr = (Fp1 + Fp2 + ... + O1 + O2)/19 - N
	std::valarray<double> refArr(temp.edfData.cols());
	for(QString chanName : usedLabels)
	{
		auto ref = temp.findChannel(chanName);
		refArr += temp[ref];
	}
	refArr /= usedLabels.size();

	for(int i = 0; i < temp.ns; ++i)
	{
		auto it = std::find_if(std::begin(rerefLabels), std::end(rerefLabels),
							   [temp, i](const QString & in)
		{ return temp.labels[i].contains(in); });

		if(it != std::end(rerefLabels))
		{
			if(!(*it).contains("EOG"))
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
			QString newLabel = temp.labels[i];
			newLabel = myLib::fitString(newLabel.left(newLabel.indexOf('-') + 1) + "CAR", 16);
			temp.labels[i] = newLabel;
		}
	}
	return temp;
}

void edfFile::saveSubsection(int startBin,
                             int finishBin,
                             const QString & outPath,
                             bool plainFlag) const // [start, finish)
{
    if(plainFlag)
	{
		myLib::writePlainData(outPath,
							  this->edfData,
							  startBin,
							  finishBin);
    }
    else
    {
        edfFile temp(*this, true);

		temp.edfData.resize(this->ns, finishBin - startBin);
        for(int i = 0; i < this->ns; ++i)
        {
			std::copy(std::begin(this->edfData[i]) + startBin,
					  std::begin(this->edfData[i]) + finishBin,
					  std::begin(temp.edfData[i]));
		}
		temp.writeEdfFile(outPath, plainFlag);
    }
}

void edfFile::drawSubsection(int startBin, int finishBin, QString outPath) const
{
	myLib::drw::drawEeg(this->edfData.subCols(startBin, finishBin),
						this->srate).save(outPath, 0, 100);
}

void edfFile::cleanFromEyes(QString eyesPath,
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
            if(eegNums.size() == coefs.rows()) break; /// bicycle generality - only first 19
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
    for(uint i = 0; i < coefs.rows(); ++i)
    {
        for(uint k = 0; k < coefs.cols(); ++k)
        {

			this->edfData[ eegNums[i] ] -= this->edfData [ eogNums[k] ] * coefs[i][k];
        }
    }
    if(removeEogChannels)
    {
        this->removeChannels(eogNums);
	}

	std::cout << "cleanFromEyes: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

edfFile edfFile::reduceChannels(const std::vector<int> & chanList) const // much memory
{
	// more general, much memory
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

edfFile & edfFile::removeChannels(const std::vector<uint> & chanList)
{
    std::set<int, std::greater<int>> excludeSet;
    for(int val : chanList)
    {
		excludeSet.emplace(val);
    }

    for(int k : excludeSet)
	{
		if(k < 0) continue; /// good for -1 in findChannel

		this->edfData.eraseRow(k);
        this->channels.erase(std::begin(this->channels) + k);
    }
    this->adjustArraysByChannels();
	return *this;
}

edfFile & edfFile::removeChannels(const QStringList & chanList)
{
	std::set<int, std::greater<int>> excludeSet;
	for(const QString & ch : chanList)
	{
		auto val = this->findChannel(ch);
		if(val != -1) { excludeSet.emplace(val); }
		else { std::cout << "edfFile::removeChannels(vector<QString>): bad channel "
						 << ch << std::endl; }
	}

	for(int k : excludeSet)
	{
		if(k < 0) continue; /// for -1 in findChannel

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
		if(smLib::isInt(item)) // just copy
		{
			temp.edfData[itemCounter] = this->edfData[accordNum];
			temp.channels[itemCounter] = this->channels[accordNum];
        }
		else if(item.contains(QRegExp(R"([\+\-\*\/])")))
        {
			int lengthCounter = 0; // length of the expression in chars
			auto lst = item.split(QRegExp(R"([\+\-\*\/])"), QString::SkipEmptyParts);
//			std::cout << lst << std::endl;

			/// check that nums between operators
			for(auto in : lst)
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
				if(item[lengthCounter] == '+') sign = 1.;
				else if(item[lengthCounter] == '-') sign = -1.;
				else // this should never happen!
                {
					std::cout << "edfFile::reduceChannels: first sign is not + or -, return * this" << std::endl;
					return *this;
                }
				lengthCounter += 1; // sign length
				lengthCounter += (*lstIter).length();

				// check '/' and '*'
				if(item[lengthCounter] == '/')
                {
					sign /= (*(lstIter+1)).toDouble(); // already checked for being int
                }
				else if(item[lengthCounter] == '*')
                {
					sign *= (*(lstIter+1)).toDouble(); // already checked for being int
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
					lengthCounter += 1 + (*lstIter+1).length(); // sign and argument
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
//	std::cout << std::endl;
//	std::cout << "setLabels:" << std::endl;
//	std::cout << this->labels << std::endl;
}

edfFile & edfFile::insertChannel(int num, const std::valarray<double> & dat, edfChannel ch)
{
	this->channels.insert(std::begin(this->channels) + num, ch);
	this->edfData.myData.insert(std::begin(this->edfData.myData) + num, dat);
	this->adjustArraysByChannels();
	return *this;
}

/// exceptions
void edfFile::setChannels(const std::vector<edfChannel> & inChannels)
{
    for(uint i = 0; i < inChannels.size(); ++i)
    {
        this->channels[i] = inChannels[i];
    }
}


// template

void edfFile::writeOtherData(const matrix & newData,
                             const QString & outPath,
							 std::vector<int> chanList) const
{
    edfFile temp(*this, true); // copy-construct everything but data

    if(chanList.empty())
    {
        chanList.resize(newData.size());
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
        /// pewpwepwwpepwpewpepwepwpep
//        temp.edfData.push_back( newData[item] );
		temp.edfData.push_back( newData[num++] );
    }
    temp.adjustArraysByChannels(); // set in particular ns = chanList.length();
    temp.writeEdfFile(outPath);
}

void edfFile::fitData(int initSize) // append zeros to whole ndr's
{
    this->ndr = ceil(double(initSize) / (this->srate * this->ddr)); // generality
	this->edfData.resizeCols(ndr * ddr * srate);
}

void edfFile::cutZerosAtEnd() // cut zeros when readEdf, before edfChannels are allocated
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
				break; // do clean
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

	this->ndr = ceil(this->edfData.cols() / (this->srate * this->ddr)); // should be unchanged
}

double edfFile::checkDdr(const QString & inPath)
{
    if(!QFile::exists(inPath))
    {
		std::cout << "edfFile::checkDdr: file doesn't exist" << std::endl;
        return -1;
    }
    FILE * tmp = fopen(inPath, "r");
    QString tempStr;
    double localDdr = 0.;

    handleParam(tempStr, 244, true, tmp, (FILE*)NULL);
    handleParam(localDdr, 8, true, tmp, (FILE*)NULL);
    fclose(tmp);

    return localDdr;
}

bool edfFile::isRerefChannel(const QString & inLabel)
{
	for(QString str: {"A1-A2", "A2-A1", "A1-N", "A2-N"})
	{
		if(inLabel.contains(str)) return true;
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
	else
	{
		edfFile temp(*this);
		temp.setDataFromList(dataList);
		return temp;
	}
}


edfFile edfFile::repairChannelsOrder(const std::vector<QString> & standard) const
{

	std::vector<int> reorderChanList{};
	for(uint i = 0; i < standard.size(); ++i)
	{
		for(int j = 0; j < this->ns; ++j)
		{
			if(this->labels[j].contains(standard[i])) /// unique standards
			{
				reorderChanList.push_back(j);
				break;
			}
		}
	}

	// fill the rest of channels
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
	else
	{
		edfFile temp(*this);
		return temp.reduceChannels(reorderChanList);
	}
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
	else
	{
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
}

void edfFile::transformEdfMatrix(const QString & inEdfPath,
                                 const matrix & matrixW,
                                 const QString & newEdfPath)
{
    QTime myTime;
    myTime.start();

    edfFile fil;
    fil.readEdfFile(inEdfPath);
    matrix newData;

    matrixProduct(matrixW,
                  fil.getData(),
                  newData,
				  uint(DEFS.nsWOM())); // w/o markers from globalEdf data

	newData.push_back(fil.getData()[fil.getMarkChan()]); // copy markers
    fil.writeOtherData(newData, newEdfPath);
	std::cout << "transformEdfMaps: time elapsed = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}








/// non-members for static operation
void myTransform(int & output, char * input)			{ output = atoi(input); }
void myTransform(double & output, char * input)			{ output = atof(input); }
void myTransform(QString & output, char * input)		{ output = QString(input); }
void myTransform(std::string & output, char * input)	{ output = std::string(input); }

char * QStrToCharArr(const QString & input, int len = -1)
{
	// fixes problem with labels length

	int leng = input.length();
	if(len != -1)
	{
		leng = len;
	}
	char * array = new char [leng + 1];
	memcpy(array, input.toStdString().c_str(), input.length());

	if(len != -1)
	{
		for(int i = input.length(); i < leng; ++i)
		{
			array[i] = ' ';
		}
	}
	array[leng] = '\0';
	return array;
}

void myTransform(int input, int len, char ** output)
{
	(*output) = QStrToCharArr(myLib::fitNumber(input, len));
}
void myTransform(double input, int len, char ** output)
{
	(*output) = QStrToCharArr(myLib::fitNumber(input, len));
}
void myTransform(const QString & input, int len, char ** output)
{
    (*output) = QStrToCharArr(input, len);
}
void myTransform(const std::string & input, int len, char ** output)
{
    (*output) = new char [len + 1];
    std::string tmp = input;
    for(int i = input.length(); i < len; ++i)
    {
        tmp.push_back(' ');
    }

    std::copy(std::begin(tmp),
              std::end(tmp),
              (*output));
    (*output)[len] = '\0';
}








template <typename Typ>
void handleParam(Typ & qStr,
                 int length,
                 bool readFlag,
                 FILE * ioFile,
                 FILE * headerFile)
{
    char * array;
    if(readFlag)
    {
        array = new char [length + 1];
        fread (array, sizeof(char), length, ioFile); array[length] = '\0';
        myTransform(qStr, array);
        if(headerFile != NULL)
        {
            fwrite(array, sizeof(char), length, headerFile);
        }
        delete []array;
    }
    else
    {
        myTransform(qStr, length, &array);
//        fwrite(array, sizeof(char), strlen(array), ioFile); /// not strlen but size???
        fprintf(ioFile, "%s", array);
        delete []array;
    }
}

template <typename Typ>
void handleParamArray(std::valarray<Typ> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile)
{
    if(readFlag) qStr = std::valarray<Typ>(Typ(), number); // clean param vector

    for(int i = 0; i < number; ++i)
    {
        handleParam <Typ> (qStr[i], length, readFlag, ioFile, headerFile);
    }
}

template <typename Typ>
void handleParamArray(std::vector<Typ> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile)
{
    if(readFlag) qStr = std::vector<Typ>(number, Typ()); // clean param vector

    for(int i = 0; i < number; ++i)
    {
        handleParam <Typ> (qStr[i], length, readFlag, ioFile, headerFile);
    }
}

template
void handleParamArray(std::vector<int> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);
template
void handleParamArray(std::vector<double> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);
template
void handleParamArray(std::vector<QString> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);

template
void handleParamArray(std::valarray<int> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);
template
void handleParamArray(std::valarray<double> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);
template
void handleParamArray(std::valarray<QString> & qStr,
                      int number,
                      int length,
                      bool readFlag,
                      FILE * ioFile,
                      FILE * headerFile);

template
void handleParam(int & qStr,
                 int length,
                 bool readFlag,
                 FILE * ioFile,
                 FILE * headerFile);
template
void handleParam(double & qStr,
                 int length,
                 bool readFlag,
                 FILE * ioFile,
                 FILE * headerFile);
template
void handleParam(QString & qStr,
                 int length,
                 bool readFlag,
                 FILE * ioFile,
                 FILE * headerFile);

template
void handleParam(std::string & qStr,
                 int length,
                 bool readFlag,
                 FILE * ioFile,
                 FILE * headerFile);



/// with streams

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
//        fread (array, sizeof(char), length, ioFile); array[length] = '\0';
		myTransform(qStr, array);
		if(headerFile.good())
		{
			headerFile.write(array, length);
//            fwrite(array, sizeof(char), length, headerFile);
		}
		delete []array;
	}
	else
	{
		myTransform(qStr, length, &array);
//        fwrite(array, sizeof(char), strlen(array), ioFile); /// not strlen but size???
		ioFile.write(array, length);
		delete []array;
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
		qStr = std::vector<Typ>(number);
		for(auto & in : qStr)
		{
			in = Typ();
		}
	}

	for(int i = 0; i < number; ++i)
	{
		handleParam <Typ> (qStr[i], length, readFlag, ioFile, headerFile);
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
		for(auto & in : qStr)
		{
			in = Typ();
		}
	}

	for(int i = 0; i < number; ++i)
	{
		handleParam <Typ> (qStr[i], length, readFlag, ioFile, headerFile);
	}
}


template
void handleParamArray(std::vector<int> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile);
template
void handleParamArray(std::vector<double> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile);
template
void handleParamArray(std::vector<QString> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile);

template
void handleParamArray(std::valarray<int> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile);
template
void handleParamArray(std::valarray<double> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile);
template
void handleParamArray(std::valarray<QString> & qStr,
					  int number,
					  int length,
					  bool readFlag,
					  std::fstream & ioFile,
					  std::fstream & headerFile);

template
void handleParam(int & qStr,
				 int length,
				 bool readFlag,
				 std::fstream & ioFile,
				 std::fstream & headerFile);
template
void handleParam(double & qStr,
				 int length,
				 bool readFlag,
				 std::fstream & ioFile,
				 std::fstream & headerFile);
template
void handleParam(QString & qStr,
				 int length,
				 bool readFlag,
				 std::fstream & ioFile,
				 std::fstream & headerFile);

template
void handleParam(std::string & qStr,
				 int length,
				 bool readFlag,
				 std::fstream & ioFile,
				 std::fstream & headerFile);
