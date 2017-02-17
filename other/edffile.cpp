#include "edffile.h"

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
    this->headerRest = other.getHeaderRest();

    this->channels = other.getChannels();
	this->markerChannel = other.getMarkChan();

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
}

edfFile::edfFile(const QString & txtFilePath, inst which)
{
	if(which == inst::mati)
	{

		//quant, time
		//amplX, amplY, freqX, freqY
		//targX, targY, mousX, mousY
		//traceSuccessXY, secondBit
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
		this->dirPath = txtFilePath.left(txtFilePath.lastIndexOf( slash ) );

		this->ns = numOfParams;
		this->ddr = 1.;

		this->nr = std::valarray<double> (def::freq, this->ns);
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
			this->edfData[i].resize(6 * 60 * def::freq); // for 6 minutes generality
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
			iitpLabels[i][0] = iitpLabels[i][0].toUpper();
			int s = iitpLabels[i].size();
			if(iitpLabels[i].contains(QRegExp("[lLrR]$")) &&
			   iitpLabels[i][s-2] != '_')
			{
				iitpLabels[i].insert(s-1, '_');
			}


			if(!iitpLabels[i].contains("Artefac"))
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
				int i = this->findChannel(lab + post);
				if(i >= 0)
				{
					chanList.push_back(i);
				}
			}
		}
		chanList.push_back(this->findChannel("Artefac"));
		chanList.push_back(this->findChannel("Marker"));

		*this = this->reduceChannels(chanList);
	}
}




void edfFile::readEdfFile(QString EDFpath, bool headerOnly)
{
    QTime myTime;
    myTime.start();
    this->fftData.clear(); /// crucial

    handleEdfFile(EDFpath, true, headerOnly);

	/// experimental
	def::ns = this->ns;
	def::freq = srate;
}

void edfFile::rewriteEdfFile()
{
	this->handleEdfFile(this->getFilePath(), false);
}

void edfFile::writeEdfFile(QString EDFpath, bool asPlain)
{
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

    FILE * edfDescriptor;
	edfDescriptor = fopen(EDFpath, (readFlag ? "r" : "w")); //generality
    if(edfDescriptor == NULL)
    {
		std::cout << "handleFile: cannot open edf file " << EDFpath << std::endl;
        return;
    }

	if(readFlag)
	{
		filePath = EDFpath;
		dirPath = EDFpath.left(EDFpath.lastIndexOf(slash));
		ExpName = myLib::getExpNameLib(filePath);
	}

	FILE * header = NULL;
	if(readFlag && (writeHeaderFlag || headerOnly))
    {
		helpString = dirPath + slash + "header.txt";
        header = fopen(helpString, "w");
        if(header == NULL)
        {
			std::cout << "edfFile::handleEdfFile: cannot open header.txt file" << std::endl;
            return;
        }
    }


    if(!readFlag)
    {
		this->fitData(edfData.cols());
    }


    handleParam(headerInitialInfo, 184, readFlag, edfDescriptor, header);
    handleParam(bytes, 8, readFlag, edfDescriptor, header);
    handleParam(headerReservedField, 44, readFlag, edfDescriptor, header);
    handleParam(ndr, 8, readFlag, edfDescriptor, header);
    handleParam(ddr, 8, readFlag, edfDescriptor, header);
    handleParam(ns, 4, readFlag, edfDescriptor, header);

	/// start channels read
    handleParamArray(labels, ns, 16, readFlag, edfDescriptor, header);

    /// generality for encephalan
	if(readFlag)
	{
		for(int i = 0; i < ns; ++i)
		{
			/// edit EOG channels to encephalan
			if(labels[i].contains("EOG 1"))
			{
				labels[i] = myLib::fitString("EOG EOG1-A2", 16);
			}
			else if(labels[i].contains("EOG 2"))
			{
				labels[i] = myLib::fitString("EOG EOG2-A1", 16);
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
	}

	if(readFlag && writeLabelsFlag)
    {
		helpString = dirPath + slash + "labels.txt";
		FILE * labelsFile = NULL;
        labelsFile = fopen(helpString, "w");
		if(labelsFile != NULL)
		{
			for(int i = 0; i < ns; ++i)                         //labels write in file
			{
				fprintf(labelsFile, "%s\n", labels[i].toStdString().c_str());
			}
			fclose(labelsFile);
		}
    }

    handleParamArray(transducerType, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(physDim, ns, 8, readFlag, edfDescriptor, header);
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
    handleParamArray(physMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(physMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMax, ns, 8, readFlag, edfDescriptor, header);
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

    handleParamArray(prefiltering, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(nr, ns, 8, readFlag, edfDescriptor, header);

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
			std::cout << "dataSize = " << fileSize - bytes << std::endl;
			std::cout << "ns = " << ns << std::endl;
			std::cout << "freq = " << this->srate << std::endl;
			std::cout << "ddr = " << ddr << std::endl;
			std::cout << "rest size = "
				 << (fileSize - bytes) - ndr * sumNr * 2.<< std::endl;
        }
//        ndr = min(int(realNdr), ndr); /// sometimes the tails are shit
		ndr = int(realNdr); /// sometimes ndr from file is a lie
    }
    handleParamArray(reserved, ns, 32, readFlag, edfDescriptor, header);
    //end channels read


    handleParam(headerRest, int(bytes - (ns + 1) * 256), readFlag, edfDescriptor, header);

	if(readFlag && (header != NULL))
    {
        fclose(header);
    }


    if(headerOnly)
    {
        fclose(edfDescriptor);
        return;
    }

    fpos_t *position = new fpos_t;
    fgetpos(edfDescriptor, position);
    fclose(edfDescriptor);
	edfDescriptor = fopen(EDFpath, (readFlag ? "rb" : "ab"));
    fsetpos(edfDescriptor, position);
    delete position;

    if(readFlag)
    {
        annotations.clear();
	}

    handleData(readFlag, edfDescriptor);
	fclose(edfDescriptor);
	if(readFlag)
	{
		this->cutZerosAtEnd();
	}

    /// experimental annotations
    if(this->edfPlusFlag)
    {
        this->removeChannels({this->markerChannel}); /// it should be zero
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
                       + (def::ntFlag ? 0 : 1));
            /// neurotravel + 0
            /// encephalan + 1

//            currDatum = a * 1./8.; // generality encephalan

        }
        else //if markers channel
        {
            if(this->edfPlusFlag)
            {
                //edf+
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

//                currDatum = a; //generality encephalan
            }
			if(writeMarkersFlag &&
			   !edfPlusFlag &&
               currDatum != 0) // make markers file when read only
            {
                writeMarker(currDatum, currTimeIndex);
            }
        }
    }
    else //if write
    {
        if(currNs != markerChannel) // usual data write
        {
            // round better to N * 1/8.
            if(currNs < 21) // generality bicycle
            {
				currDatum = smallLib::doubleRoundFraq(currDatum,
													  int( (digMax[currNs] - digMin[currNs] + 1)
														   / (physMax[currNs] - physMin[currNs]) )
													  ); // need for eyes cleaned EEG only
            }

            a = (qint16)((currDatum - physMin[currNs])
                        * (digMax[currNs] - digMin[currNs]
                           + (def::ntFlag ? 0 : 1))
                        / (physMax[currNs] - physMin[currNs])
						+ digMin[currNs]);

            fwrite(&a, sizeof(qint16), 1, edfForDatum);
        }
        else //if markers channel
        {
            if(this->edfPlusFlag) ////////////////////////// to do???
            {
                //edf+
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
	std::vector<bool> byteMarker;
	QString helpString;

    FILE * markers;
	/// marker file name choose
//    helpString = dirPath + slash + this->ExpName + "_markers.txt";
	helpString = dirPath + slash + "markers.txt";

    markers = fopen(helpString, "a+");
    fprintf(markers, "%d %d", currTimeIndex, int(currDatum));
    if(this->matiFlag)
    {
		byteMarker = myLib::matiCountByte(currDatum);

        fprintf(markers, "\t");
        for(int s = 15; s >= 0; --s)
        {
            fprintf(markers, "%d", int(byteMarker[s]));
            if(s == 8)
            {
                fprintf(markers, " ");  //byte delimiter
            }
        }
        if(byteMarker[10])
        {
            fprintf(markers, " - session end");
        }
        else if(byteMarker[9] || byteMarker[8])
        {
            fprintf(markers, " - session start");
        }
        if(byteMarker[12])
        {
            fprintf(markers, " - 12 bit error");
        }
        if(byteMarker[11])
        {
            fprintf(markers, " - 11 bit error");
        }
    }
    fprintf(markers, "\n");
    fclose(markers);
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
            //set time into helpString with 3 float numbers
            helpString.setNum(markTime);
            if(helpString[helpString.length()-3]=='.') helpString.append("0"); //float part - 2 or 3 signs
            else
            {
                if(helpString[helpString.length()-2]=='.') helpString.append("00");
                else helpString.append(".000");
            }
            for(int i = helpString.length()+2; i < annotations[j].length(); ++i) //+2 because of '+' and '\24'
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
	if(this->markerChannel < 0 || this->markerChannel == this->ns - 1) return;

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

edfFile & edfFile::divideChannel(uint chanNum, double denom)
{
	this->edfData[chanNum] /= denom;
	return *this;
}

edfFile & edfFile::divideChannels(std::vector<uint> chanNums, double denom)
{
	if(chanNums.empty())
	{
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

uint edfFile::findChannel(const QString & str)
{
	for(int i = 0; i < this->ns; ++i)
	{
		if(labels[i].contains(str, Qt::CaseInsensitive)) return i;
	}
	return -1;
}

edfFile & edfFile::subtractMeans(const QString & outPath)
{
	for(int i = 0; i < this->ns; ++i)
	{
		this->edfData[i] -= smallLib::mean(this->edfData[i]);
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
					  double numSigmas)
{
	const std::valarray<double> & chan = edfData[channel];
	return myLib::findJump(chan, startPoint, numSigmas);
}

edfFile & edfFile::iitpSyncAutoCorr(int startSearchEeg,
									int startEmg,
									bool byEeg)
{
	const int searchLength = 300;
	const int corrLength = 1600;

	int numECG = this->findChannel("ECG");
	int numArtefac = this->findChannel("Artefac");

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

		double a = smallLib::correlation(eegMarkSignal, emgMarkSignal);



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
	int numECG = this->findChannel("ECG");
	int numArtefac = this->findChannel("Artefac");

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
			this->edfData[i] = smallLib::pop_front_valar(this->edfData[i], offsetEeg - addLeft);
		}
		else
		{
			this->edfData[i] = smallLib::pop_front_valar(this->edfData[i], offsetEmg - addLeft);
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
							const QString & newPath,
							bool isNotch,
							std::vector<uint> chanList)
{
	if(chanList.empty())
	{
		for(int i = 0; i < this->ns; ++i)
		{
			/// filter only EEG, EOG signals - look labels!!!!
			/// pewpew IITP - no filter ECG
			if(this->labels[i].contains(QRegExp("E[OE]G")) ||
			   (this->filterIITPflag && this->labels[i].startsWith("IT ")))
			{
				chanList.push_back(i);
			}
		}
	}

#if 01
	/// new butterworth filtering
	/// faster and better on lower frequencies
	for(int j : chanList)
	{
		this->edfData[j] = myDsp::refilter(this->edfData[j],
										   lowFreq,
										   highFreq,
										   isNotch,
										   this->getNr()[j]);
	}
#else


	/// old FFT filtering
	int fftLength = fftL(this->dataLength);
	double spStep = this->srate / double(fftLength);
	std::valarray<double> spectre(2 * fftLength);

    if(this->fftData.empty())
	{
        this->countFft();
    }

	const int lowLim = ceil(2. * lowFreq / spStep);
	const int highLim = floor(2. * highFreq / spStep);

    int i = 0;
    for(int j : chanList)
	{
        std::copy(std::begin(fftData[i]),
                  std::begin(fftData[i]) + 2 * fftLength,
				  std::begin(spectre));
		++i;

		myLib::refilterSpectre(spectre, lowLim, highLim, isNotch);

		this->edfData[j] = spectreCtoRrev(spectre);
    }
#endif

    if(!newPath.isEmpty())
    {
        this->writeEdfFile(newPath);
	}
	return *this;
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
                       finishBin - startBin,
					   startBin);
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
	myLib::drawEeg(this->edfData,
				   this->ns,
				   startBin,
				   finishBin,
				   this->srate,
				   outPath);
}

void edfFile::cleanFromEyes(QString eyesPath,
                            bool removeEogChannels,
                            std::vector<int> eegNums,
                            std::vector<int> eogNums)
{
    QTime myTime;
    myTime.start();

    if(eyesPath.isEmpty())
    {
		eyesPath = this->dirPath + slash + "eyes.txt";
    }

    matrix coefs;
	myLib::readMatrixFile(eyesPath, coefs);

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

	std::cout << "cleanFromEyes: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
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

void edfFile::removeChannels(const std::vector<int> & chanList)
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
}

edfFile edfFile::reduceChannels(const QString & chanStr) const
{
    /// need fix, doesn't work properly
    QTime myTime;
	myTime.start();

    QStringList lst;
    QStringList leest = chanStr.split(QRegExp("[,;\\s]"), QString::SkipEmptyParts);
    if(leest.last().toInt() - 1 != this->markerChannel)
    {
		std::cout << "edfFile::reduceChannels: warning - last is not marker" << std::endl;
		std::cout << leest.last().toInt() - 1 << "\t" << this->markerChannel << std::endl;
    }

	edfFile temp(*this, true);
	temp.edfData = matrix();
	temp.edfData.resize(leest.length());
	temp.channels.resize(leest.length());

    /// need write a check of channel sequence

    double sign = 0.;
    int lengthCounter = 0; //length of the expression in chars

    for(int k = 0; k < leest.length(); ++k)
    {
        if(QString::number(leest[k].toInt()) == leest[k]) // just copy
		{
//			this->edfData[k] = this->edfData[leest[k].toInt() - 1];
			temp.edfData[k] = this->edfData[leest[k].toInt() - 1];
			temp.channels[k] = this->channels[leest[k].toInt() - 1];
        }
        else if(leest[k].contains(QRegExp(R"([\+\-\*\/])")))
        {
            lengthCounter = 0;
            lst = leest[k].split(QRegExp(R"([\+\-\*\/])"), QString::SkipEmptyParts);
            for(int h = 0; h < lst.length(); ++h)
            {
				if(!smallLib::isInt(lst[h]))
                {
					std::cout << "edfFile::reduceChannels: NAN between operators, return *this" << std::endl;
					return *this;
                }
            }
//            this->channels[k] = this->channels[lst[0].toInt() - 1];
			temp.channels[k] = this->channels[lst[0].toInt() - 1];

            lengthCounter += lst[0].length();
            for(int h = 1; h < lst.length(); ++h)
            {
                if(leest[k][lengthCounter] == '+') sign = 1.;
                else if(leest[k][lengthCounter] == '-') sign = -1.;
                else //this should never happen!
                {
					std::cout << "edfFile::reduceChannels: first sign is not + or -, return * this" << std::endl;
					return * this;;
                }
                lengthCounter += 1; //sign length
                lengthCounter += lst[h].length();

                //check '/' and '*'
                if(leest[k][lengthCounter] == '/')
                {
                    sign /= lst[h+1].toDouble();
                }
                else if(leest[k][lengthCounter] == '*')
                {
                    sign *= lst[h+1].toDouble();
                }

//                this->edfData[k] = this->edfData[lst[0].toInt() - 1]
//                        + sign * this->edfData[lst[h].toInt() - 1];

				temp.edfData[k] = this->edfData[lst[0].toInt() - 1]
						+ sign * this->edfData[lst[h].toInt() - 1];

                if(leest[k][lengthCounter] == '/' || leest[k][lengthCounter] == '*')
                {
                    lengthCounter += 1; // / or *
                    lengthCounter += lst[h+1].length(); //what was divided onto
                    ++h;
                }
                /// here should stop, no following h-iteration
            }
        }
        else
        {
			std::cout << "edfFile::reduceChannels: unknown format of the string, return *this" << std::endl;
			return *this;
        }
	}
	std::cout << "reduceChannelsFast: ns = " << ns;
	std::cout << ", time = " << myTime.elapsed() / 1000. << " sec";
	std::cout << std::endl;

	temp.ns = leest.length();
	temp.adjustArraysByChannels();
	return temp;

}

void edfFile::setLabels(char ** inLabels)
{
    for(int i = 0; i < this->ns; ++i)
    {
        this->channels[i].label = inLabels[i];
        this->labels[i] = inLabels[i];
    }
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

/// exceptions
void edfFile::setChannels(const std::vector<edfChannel> & inChannels)
{
    for(uint i = 0; i < inChannels.size(); ++i)
    {
        this->channels[i] = inChannels[i];
    }
}


//template

void edfFile::writeOtherData(const matrix & newData,
                             const QString & outPath,
                             std::vector<int> chanList)
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
			if(std::abs(this->edfData[j][currEnd - 1]) >= 30000) break; // do clean

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
                  uint(def::nsWOM())); // w/o markers from globalEdf data

    newData.push_back(fil.getData()[fil.getMarkChan()]); //copy markers
    fil.writeOtherData(newData, newEdfPath);
	std::cout << "transformEdfMaps: time elapsed = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}








/// non-members for static operation
void myTransform(int & output, char * input) {output = atoi(input);}
void myTransform(double & output, char * input) {output = atof(input);}
void myTransform(QString & output, char * input) {output = QString(input);}
void myTransform(std::string & output, char * input) {output = std::string(input);}

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
