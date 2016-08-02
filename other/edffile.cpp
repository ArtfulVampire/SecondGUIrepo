#include "edffile.h"

using namespace std;
using namespace myLib;
using namespace smallLib;

edfFile::edfFile()
{
#if DATA_POINTER
    this->dataPointer = &(this->data);
#endif
}

std::list<std::valarray<double>> edfFile::getDataAsList() const
{
    std::list<std::valarray<double>> res(this->data.cols());
    uint j = 0;
    for(auto & in : res)
    {
        in = this->data.getCol(j++);
    }
    return res;
}

void edfFile::setDataFromList(const std::list<std::valarray<double>> & inList)
{
    this->data.resize(inList.front().size(), inList.size());
    uint col = 0;
    for(auto it = std::begin(inList); it != std::end(inList); ++it, ++col)
    {
        for(uint i = 0; i < (*it).size(); ++i)
        {
            this->data[i][col] = (*it)[i];
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
    this->srate = other.freq();

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
    this->dataLength = this->ndr * this->nr[0]; // other.getDataLen();

    if(!noData)
    {
        this->data = other.getData();
        this->fftData = other.fftData;
#if DATA_POINTER
        this->dataPointer = &(this->data);
#endif
    }
    else
    {
        this->data = edfDataType();
    }
}



edfFile edfFile::operator=(const edfFile & other)
{
    this->headerInitialInfo = other.getHeaderInit();
    this->bytes = other.getBytes();
    this->headerReservedField = other.getHeaderReserved();

    this->ndr = other.getNdr();
    this->ddr = other.getDdr();
    this->ns = other.getNs();
    this->srate = other.freq();

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

    this->data = other.getData();
    this->fftData = other.fftData;

#if DATA_POINTER
    this->dataPointer = &(this->data);
#endif

    this->dataLength = this->ndr * this->nr[0];  // other.getDataLen();
    this->channels = other.getChannels();
    this->markerChannel = other.getMarkChan();
    this->filePath = other.getFilePath();
    this->ExpName = other.getExpName();
    this->dirPath = other.getDirPath();
    return (*this);
}

edfFile::edfFile(const QString & matiLogPath)
{

    //quant, time
    //amplX, amplY, freqX, freqY
    //targX, targY, mousX, mousY
    //traceSuccessXY, secondBit
    // right, wrong, skipped answers

    FILE* inStr;
    inStr = fopen(matiLogPath, "r");

    if(inStr == NULL)
    {
        cout << "edfFile(matiLogFile): input file is NULL" << endl;
        return;
    }

    int numOfParams = 15 - 2; // -currTime & quantLength generality
    int currTimeIndex;

//    this->headerInitialInfo = fitString("Edf for AMOD Data", 184);
    this->headerInitialInfo = fitString("Edf for AMOD Data", 184).toStdString();
    this->headerReservedField = fitString("headerReservedField", 44);
    this->headerRest = QString();


    this->filePath = matiLogPath;
    this->ExpName = getExpNameLib(matiLogPath);
    this->dirPath = matiLogPath.left(matiLogPath.lastIndexOf( slash ) );

    this->ns = numOfParams;
    this->ddr = 1.;

    this->nr = std::valarray <double> (def::freq, this->ns);
    // ndr definedlater
    this->bytes = 256 * (this->ns + 1);

    this->labels = {fitString("AMOD amplX", 16),
                    fitString("AMOD amplY", 16),
                    fitString("AMOD freqX", 16),
                    fitString("AMOD freqY", 16),
                    fitString("AMOD targX", 16),
                    fitString("AMOD targY", 16),
                    fitString("AMOD mouseX", 16),
                    fitString("AMOD mouseX", 16),
                    fitString("AMOD tracSucces", 16),
                    fitString("AMOD mouseMove", 16),
                    fitString("AMOD rightAns", 16),
                    fitString("AMOD wrongAns", 16),
                    fitString("AMOD skipdAns", 16)
                   };
    this->transducerType = vector <QString> (this->ns, fitString("AMOD transducer", 80));
    this->physDim = vector <QString> (this->ns, fitString("AMODdim", 8));

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

    this->prefiltering = vector <QString> (this->ns, QString(fitString("AMOD no prefiltering", 80)));
    this->reserved = vector <QString> (this->ns, QString(fitString("AMOD reserved", 32)));

    this->data.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->data[i].resize(6 * 60 * def::freq); // for 6 minutes generality
    }
#if DATA_POINTER
    this->dataPointer = &(this->data);
#endif

    currTimeIndex = 0;
    while(!feof(inStr))
    {
        fscanf(inStr, "%*d %*f"); // quantLength & currTime
        for(int i = 0; i < numOfParams; ++i)
        {
            fscanf(inStr, "%lf", &(this->data[i][currTimeIndex]));
        }
        ++currTimeIndex;
    }
    --currTimeIndex; // to loose the last read string;
    //// CHECK
    /// old
//    this->ndr = ceil(currTimeIndex/250.);
//    this->dataLength = this->ndr * def::freq;

//    for(int j = 0; j < numOfParams; ++j)
//    {
//        std::remove_if(begin(this->data[j]) + dataLength,
//                       end(this->data[j]),
//                       [](double){return true;});
//    }
    /// new
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


                                    #if DATA_POINTER_IN_CHANS
                                        ,&data[i] // edfChannel::dataP
                                    #endif
                                    #if DATA_IN_CHANS
                                            ,data[i]
                                    #endif
                                            )
                                 );
    }
}




void edfFile::readEdfFile(QString EDFpath, bool headerOnly)
{
    QTime myTime;
    myTime.start();
    this->fftData.clear(); /// crucial

    handleEdfFile(EDFpath, true, headerOnly);
}

void edfFile::writeEdfFile(QString EDFpath, bool asPlain)
{
    QTime myTime;
    myTime.start();
    if(!asPlain)
    {
        if(QFile::exists(EDFpath))
        {
//            cout << "writeEdfFile: destination file already exists, RETURN\n" << EDFpath << endl; return;
//            cout << "writeEdfFile: destination file already exists, REWRITE = \n" << EDFpath << " ";
        }
        this->handleEdfFile(EDFpath, false);
    }
    else // if(asPLain)
    {
#if DATA_POINTER
        writePlainData(EDFpath,
                       *(this->dataPointer),
                       this->ns,
                       this->dataLength);
#else
        writePlainData(EDFpath, this->data);
#endif
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
        cout << "handleFile: file to read doesn't exist\n" << EDFpath << endl;
        return;
    }

    FILE * edfDescriptor;
    edfDescriptor = fopen(EDFpath, (readFlag)?"r":"w"); //generality
    if(edfDescriptor == NULL)
    {
        cout << "handleFile: cannot open edf file " << EDFpath << endl;
        return;
    }

    filePath = EDFpath;
    dirPath = EDFpath.left(EDFpath.lastIndexOf(slash));
    ExpName = getExpNameLib(filePath);

    FILE * header;
    helpString = dirPath + slash + "header.txt";
    if(readFlag)
    {
        header = fopen(helpString, "w");
        if(header == NULL)
        {
            cout << "handleFile: cannot open header.txt file" << endl;
            return;
        }
    }

    if(!readFlag)
    {
        this->fitData(this->dataLength);
    }



    handleParam(headerInitialInfo, 184, readFlag, edfDescriptor, header);
    handleParam(bytes, 8, readFlag, edfDescriptor, header);
    handleParam(headerReservedField, 44, readFlag, edfDescriptor, header);
    handleParam(ndr, 8, readFlag, edfDescriptor, header);
    handleParam(ddr, 8, readFlag, edfDescriptor, header);
    handleParam(ns, 4, readFlag, edfDescriptor, header);

    //start channels read
    handleParamArray(labels, ns, 16, readFlag, edfDescriptor, header);

    /// generality for encephalan
    for(int i = 0; i < ns; ++i)
    {
        /// edit EOG channels to encephalan
        if(labels[i].contains("EOG 1"))
        {
            labels[i] = "EOG EOG1-A2     ";
        }
        else if(labels[i].contains("EOG 2"))
        {
            labels[i] = "EOG EOG2-A1     ";
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
            cout << "handleEdfFile: Annotations! " << EDFpath << endl;
        }

        /// Mitsar and other sheet
        else
        {
            for(uint j = 0; j < coords::lbl19.size(); ++j)
            {
                if(labels[i].contains(coords::lbl19[j]) &&
                   !labels[i].contains("EEG"))
                {
//                    labels[i].prepend("EEG ");
                }
            }
        }

    }

    if(readFlag)
    {
        helpString = QDir::toNativeSeparators( dirPath + slash + "labels.txt" );
        FILE * labelsFile;
        labelsFile = fopen(helpString, "w");
        for(int i = 0; i < ns; ++i)                         //labels write in file
        {
            fprintf(labelsFile, "%s\n", labels[i].toStdString().c_str());
        }
        fclose(labelsFile);
    }

    handleParamArray(transducerType, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(physDim, ns, 8, readFlag, edfDescriptor, header);
    if(!readFlag)
    {
        for(int i = 0; i < ns; ++i)
        {
            if(labels[i].contains("EEG", Qt::CaseInsensitive))
            {
                /// encephalan only !!!!!1111
//                physMax[i] = 4096;
//                physMin[i] = -4096;
//                digMax[i] = 32767;
//                digMin[i] = -32768;
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
                    cout << "edfFile::readEdfFile: phys Max/Min are equal, chan(from 1) = "
                         << i + 1 << endl;
                }
                if(digMax[i] == digMin[i])
                {
                    cout << "edfFile::readEdfFile:  dig Max/Min are equal, chan(from 1) = "
                         << i + 1 << endl;
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
            cout << "handleEdfFile(read): realNdr is not integral = "
                 << realNdr << endl;
            cout << "dataSize = " << fileSize - bytes << endl;
            cout << "ns = " << ns << endl;
            cout << "freq = " << this->srate << endl;
            cout << "ddr = " << ddr << endl;
            cout << "rest size = "
                 << (fileSize - bytes) - ndr * sumNr * 2.<< endl;
        }
        ndr = min(int(realNdr), ndr);
    }
    handleParamArray(reserved, ns, 32, readFlag, edfDescriptor, header);
    //end channels read


    handleParam(headerRest, int(bytes - (ns + 1) * 256), readFlag, edfDescriptor, header);

    if(readFlag)
    {
        fclose(header);
    }



    /// experimental
    if(headerOnly)
    {
        fclose(edfDescriptor);

//        if(this->edfPlusFlag)
//        {

//            this->channels.erase(std::begin(this->channels) + this->markerChannel);
//            this->adjustArraysByChannels();

//            this->edfPlusFlag = false;
//            this->markerChannel = -1;
//        }

        return;
    }



    fpos_t *position = new fpos_t;
    fgetpos(edfDescriptor, position);
    fclose(edfDescriptor);
    edfDescriptor = fopen(EDFpath, readFlag?"rb":"ab");
    fsetpos(edfDescriptor, position);
    delete position;

    QFile::remove(helpString = dirPath + slash + this->ExpName + "_markers.txt");


    if(readFlag)
    {
        dataLength = ndr * nr[0]; // generality
    }
    if(readFlag)
    {
        annotations.clear();
    }
    handleData(readFlag, edfDescriptor);

//    cout << "ns = " << ns << endl;
    /// experimental annotations
    if(this->edfPlusFlag)
    {
        this->removeChannels({this->markerChannel}); /// it should be zero
        this->edfPlusFlag = false;
        this->markerChannel = -1;
    }

    /// experimental
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

    fclose(edfDescriptor);

    if(readFlag)
    {
        this->cutZerosAtEnd();
    }
}

void edfFile::handleData(bool readFlag,
                         FILE * edfForData)
{
    int currTimeIndex;
    QString helpString;
    if(readFlag)
    {
        // allocate memory for data array
#if DATA_POINTER
        (*(this->dataPointer)).resize(ns);
#else
        this->data = matrix(ns, dataLength, 0.);
//        cout << this->data.cols() << "\t" << this->data.rows() << endl;
#endif
        for(int i = 0; i < ns; ++i)
        {
#if DATA_POINTER
            (*(this->dataPointer))[i].resize(dataLength);
#else
//            data[i].resize(dataLength);
#endif
        }

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

                                        #if DATA_POINTER_IN_CHANS
                                            ,&data[i] // edfChannel::dataP
                                        #endif

                                        #if DATA_IN_CHANS
                                                ,data[i]
                                        #endif
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

void edfFile::handleDatum(const int & currNs,
                          const int & currTimeIndex,
                          bool readFlag,
                          QString & ntAnnot,
                          FILE * edfForDatum)
{
    char helpChar = '0';
    qint16 a = 0;
    quint16 markA = 0;


#if DATA_POINTER_IN_CHANS
    double & currDatum = (*(this->channels[currNs].dataP))[currTimeIndex];
#elif DATA_POINTER
    double & currDatum = (*(this->dataPointer))[currNs][currTimeIndex];
#else
    double & currDatum = this->data[currNs][currTimeIndex];
#endif

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
                    matiFixMarker(currDatum);
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
            if(this->writeMarkersFlag &&
               !this->edfPlusFlag &&
               currDatum != 0) // make markers file when read only
            {
                writeMarker(currDatum, currTimeIndex);
            }
            if(currDatum == 200 )
            {
                staSlice = currTimeIndex;
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
                currDatum = doubleRoundFraq(currDatum,
                                            int( (digMax[currNs] - digMin[currNs] + 1)
                                                 / (physMax[currNs] - physMin[currNs]) )
                                            ); // need for eyes cleaned EEG only
            }

            a = (qint16)((currDatum - physMin[currNs])
                        * (digMax[currNs] - digMin[currNs]
                           + (def::ntFlag ? 0 : 1))
                        / (physMax[currNs] - physMin[currNs])
                        + digMin[currNs]);

//            a  = (qint16)(currDatum * 8.); // generality encephalan

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


void edfFile::writeMarker(const double & currDatum,
                           const int & currTimeIndex) const
{
    vector <bool> byteMarker;
    QString helpString;
//    const double & currDatum = (*dataPointer)[currNs][currTimeIndex];

    FILE * markers;


    helpString = dirPath + slash + this->ExpName + "_markers.txt";
    markers = fopen(helpString, "a+");
    fprintf(markers, "%d %d", currTimeIndex, int(currDatum));
    if(this->matiFlag)
    {
        byteMarker = matiCountByte(currDatum);

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

void edfFile::handleAnnotations(const int & currNs,
                                const int & currentTimeIndex,
                                QString helpString,
                                std::vector <QString> annotations)
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
//        nr[ns-1] = this->srate; // generality freq change
    }
    */

}

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

#if DATA_IN_CHANS
    this->data.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->data.push_back(this->channels[i].data);
    }
#endif

#if DATA_POINTER_IN_CHANS
    for(int i = 0; i < this->ns; ++i)
    {
        this->data[i] = *(this->channels[i].dataP);
    }
#endif

    this->headerRest = QString();
#if DATA_POINTER
    this->dataLength = (*(this->dataPointer))[0].size(); // dunno
#else
    this->dataLength = this->data[0].size(); // dunno
#endif
}

void edfFile::adjustMarkerChannel()
{
    if(!(this->channels.back().label.contains("Marker") ||
         this->channels.back().label.contains("Status")))
    {
#if DATA_IN_CHANS
        edfChannel tempMarkChan = this->channels[this->markerChannel]; // save markerChannel
        this->channels.erase(this->channels.begin() + this->markerChannel); // remove markerChannel
        this->channels.push_back(tempMarkChan); // return markerChannel to the end of a list
#elif DATA_POINTER_IN_CHANS

#else
        edfChannel tempMarkChan = this->channels[this->markerChannel];
#if DATA_POINTER
        lineType tempMarkData = (*(this->dataPointer))[this->markerChannel];
#else
        lineType tempMarkData = this->data[this->markerChannel];
#endif

        this->channels.erase(this->channels.begin() + this->markerChannel); // remove markerChannel
#if DATA_POINTER
        // valarray
        (*(this->dataPointer)).erase((*(this->dataPointer)).begin()
                                   + this->markerChannel); // remove markerChannel

#else
        this->data.data.erase(this->data.begin() + this->markerChannel); // remove markerChannel
#endif


        this->channels.push_back(tempMarkChan); // return markerChannel to the end of a list
#if DATA_POINTER
        (*(this->dataPointer)).push_back(tempMarkData); // return markerChannel
#else
        this->data.push_back(tempMarkData); // return markerChannel
#endif

#endif
    }
    this->adjustArraysByChannels();
}

void edfFile::appendFile(QString addEdfPath, QString outPath) const
{
    edfFile temp(*this);
    edfFile addEdf;
    addEdf.readEdfFile(addEdfPath);

#if DATA_IN_CHANS
    edfChannel tempChan;
    for(int i = 0; i < addEdf.getNs(); ++i)
    {
        tempChan = addEdf.getChannels()[i];
        //old variant
        for(int i = tempChan.data.size(); i < this->dataLength; ++i)
        {

            tempChan.data.push_back(0.);
        }
        tempChan.data.resize(this->dataLength);

        this->channels.push_back(tempChan);
    }
#else
    edfChannel tempChan;
    lineType tempData;
    for(int i = 0; i < addEdf.getNs(); ++i)
    {

        tempChan = addEdf.getChannels()[i];
        /// valarray strange handling
        /// resizeValar(addEdf.data[i], this->dataLength);
        tempData.resize(this->dataLength);
        std::copy(begin(addEdf.getData()[i]),
                  begin(addEdf.getData()[i]) + min(addEdf.getDataLen(), this->getDataLen()),
                  begin(tempData));

        temp.channels.push_back(tempChan);
        temp.data.push_back(tempData);
    }
#endif

    temp.adjustMarkerChannel();
    temp.adjustArraysByChannels();

    temp.writeEdfFile(outPath);
}

void edfFile::concatFile(QString addEdfPath, QString outPath) // assume only data concat
{
    edfFile temp(*this);
    edfFile addEdf;
    addEdf.readEdfFile(addEdfPath);

    addEdf.cutZerosAtEnd();
    temp.cutZerosAtEnd();

    const int dataLen1 = temp.getDataLen();
    const int dataLen2 = addEdf.getDataLen();
    for(int i = 0; i < this->ns; ++i)
    {
        this->data[i].resize( dataLen1 + dataLen2 );
        // copy 1
        std::copy(begin(temp.data[i]),
                  end(temp.data[i]),
                  begin(this->data[i]));
        // copy 2
        std::copy(begin(addEdf.data[i]),
                  end(addEdf.data[i]),
                  begin(this->data[i]) + dataLen1);
    }
    temp.adjustArraysByChannels();

    /// remake

    if(!outPath.isEmpty())
    {
        this->writeEdfFile(outPath);
    }

}


void edfFile::countFft()
{
    int fftLength = fftL(this->dataLength);
    double norm1 = fftLength / double(this->dataLength);

    std::vector<int> chanList;
    for(int i = 0; i < this->ns; ++i)
    {
        if(this->labels[i].contains(QRegExp("E[OE]G"))) // filter only EEG, EOG signals
        {
            chanList.push_back(i);
        }
    }
    this->fftData.resize(chanList.size());
    int i = 0;
    double * spectre = new double [2 * fftLength];
    for(auto j : chanList)
    {

        std::fill(spectre, spectre + fftLength * 2, 0.); // fill all with zeros
#if DATA_POINTER
        for(auto it = begin((*(this->dataPointer))[chanList[j]]);
            it < end((*(this->dataPointer))[chanList[j]];
            ++it)
#else
        for(auto it = std::begin(this->data[j]);
            it < std::end(this->data[j]);
            ++it)
#endif
        {
#if DATA_POINTER
            // set even elements to signal values
            spectre[2 * (it - begin((*(this->dataPointer))[j])) ] = (*it) * sqrt(norm1);
#else
            spectre[2 * (it - begin(this->data[j])) ] = (*it) * sqrt(norm1);
#endif
        }
        four1(spectre - 1, fftLength, 1);       //Fourier transform

        fftData[i].resize(2 * fftLength);
        std::copy(spectre,
                  spectre + 2 * fftLength,
                  std::begin(fftData[i]));
        ++i;
    }
    delete[] spectre;
}

/// remake vector
void edfFile::refilter(const double & lowFreq,
                       const double & highFreq,
                       const QString & newPath,
                       bool isNotch)
{

    int fftLength = fftL(this->dataLength);
    double spStep = this->srate / double(fftLength);
    double norm1 = fftLength / double(this->dataLength);
    double * spectre = new double [fftLength * 2];

    std::vector<int> chanList;
    for(int i = 0; i < this->ns; ++i)
    {
        /// filter only EEG, EOG signals - look labels!!!!
        if(this->labels[i].contains(QRegExp("E[OE]G")))
        {
            chanList.push_back(i);
        }
    }

    if(this->fftData.empty())
    {
//        cout << "edfFile::refilter: countFFT()" << endl;
        this->countFft();
    }

    const int lowLim = ceil(2. * lowFreq / spStep);
    const int highLim = floor(2. * highFreq / spStep);

    int i = 0;
    for(int j : chanList)
    {
        std::copy(std::begin(fftData[i]),
                  std::begin(fftData[i]) + 2 * fftLength,
                  spectre);
        ++i;
        //filtering

#if 0
        /// old
        for(int i = 0; i < fftLength; ++i)
        {
            if(i < 2. * lowFreq / spStep ||
               i > 2. * highFreq / spStep)
                spectre[i] = 0.;
        }
        for(int i = fftLength; i < 2 * fftLength; ++i)
        {
            if(2 * fftLength - i < 2. * lowFreq / spStep ||
               2 * fftLength - i > 2. * highFreq / spStep)
                spectre[i] = 0.;
        }
#else
        if(!isNotch)
        {
            ///new
            std::fill(spectre,
                      spectre + lowLim,
                      0.);
            std::fill(spectre + highLim,
                      spectre + fftLength,
                      0.);
            std::fill(spectre + fftLength,
                      spectre + 2 * fftLength - highLim - 1,
                      0.);
            std::fill(spectre + 2 * fftLength - lowLim + 1,
                      spectre + 2 * fftLength,
                      0.);
        }
        else
        {
            std::fill(spectre + lowLim,
                      spectre + highLim,
                      0.);
            std::fill(spectre + 2 * fftLength - highLim,
                      spectre + 2 * fftLength - lowLim,
                      0.);
        }

#endif

        //end filtering
        four1(spectre - 1, fftLength, -1);       // inverse transform
#if DATA_POINTER
        for(auto it = (*(this->dataPointer))[j].begin();
            it < (*(this->dataPointer))[j].end();
            ++it)
        {
            (*it) = spectre[2 * (it - (*(this->dataPointer))[j].begin()) ]
                    / (fftLength * sqrt(norm1));

//            if (j == 0 && (it - (*(this->dataPointer))[chanList[j]].begin()) < 24000)
//            {
//                cout << *it << endl;
//            }
        }
#else
        for(auto it = std::begin(this->data[j]);
            it < std::end(this->data[j]);
            ++it)
        {
            (*it) = spectre[2 * (it - begin(this->data[j])) ]
                    / (fftLength * sqrt(norm1));
        }
#endif
    }
    if(!newPath.isEmpty())
    {
        this->writeEdfFile(newPath);
    }
    delete []spectre;
}

void edfFile::saveSubsection(int startBin,
                             int finishBin,
                             const QString & outPath,
                             bool plainFlag) const // [start, finish)
{
    if(plainFlag)
    {
#if DATA_POINTER

        writePlainData(outPath,
                       *(this->dataPointer),
                       finishBin-startBin,
                       startBin);
#else
        writePlainData(outPath,
                       this->data,
                       finishBin - startBin,
                       startBin);
#endif

    }
    else
    {
        edfFile temp(*this, true);
#if 0
        for(int i = 0; i < this->ns; ++i)
        {
            this->data[i].assign(temp.getData()[i].begin() + startBin, temp.getData()[i].begin() + finishBin);
        }
        this->dataLength = finishBin - startBin;
        this->writeEdfFile(outPath, plainFlag);
        (*this) = temp;
#else
        temp.data.resize(this->ns);
        for(int i = 0; i < this->ns; ++i)
        {
            temp.data[i].resize(finishBin - startBin);
            std::copy(std::begin(this->data[i]) + startBin,
                      std::begin(this->data[i]) + finishBin,
                      std::begin(temp.data[i]));
        }
        temp.dataLength = finishBin - startBin;
        temp.writeEdfFile(outPath, plainFlag);
#endif
    }
}

void edfFile::drawSubsection(int startBin, int finishBin, QString outPath) const
{
#if DATA_POINTER
    drawEeg((*(this->dataPointer)),
#else
    drawEeg(this->data,
#endif
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
    readMatrixFile(eyesPath, coefs);

    if(eegNums.empty())
    {
        for(int i = 0; i < this->ns; ++i)
        {
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

#if DATA_IN_CHANS
    for(int i = 0; i < numEeg; ++i)
    {
        for(int k = 0; k < numEog; ++k)
        {
#if 1
            std::transform(this->channels[ eegNums[i] ].data.begin(),
                    this->channels[ eegNums[i] ].data.end(),
                    this->channels[ eogNums[k] ].data.begin(),
                    this->channels[ eegNums[i] ].data.begin(),
                    [&](double a, double b) {return a - b * coefs[i][k];}
            );
#else

            for(int j = 0; j < this->dataLength; ++j)
            {
                this->data[ eegNums[i] ][j] -= coefs[ i ][ k ]   // dataPointer ???
                        * this->data[ eogNums[k] ][j];
            }
#endif
        }
    }

    if(removeEogChannels)
    {
        for(int k = 0; k < numEog; ++k)
        {
            this->channels.erase(this->channels.begin() + eogNums[k]);
        }
    }
#else
    for(uint i = 0; i < coefs.rows(); ++i)
    {
        for(uint k = 0; k < coefs.cols(); ++k)
        {
#if DATA_POINTER
            /// make valarray ?
            (*(this->dataPointer))[ eegNums[i] ] -= (*(this->dataPointer)) [eogNums[k] ]
                    * coefs[i][k];
#else
            this->data[ eegNums[i] ] -= this->data [ eogNums[k] ] * coefs[i][k];
#endif
        }
    }
    if(removeEogChannels)
    {
        this->removeChannels(eogNums);
    }
#endif

    cout << "cleanFromEyes: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void edfFile::reduceChannels(const std::vector<int> & chanList) // much memory
{
#if 1 // more general, much memory
    edfFile temp(*this);

    this->channels.clear();

#if !DATA_IN_CHANS
#if DATA_POINTER
    (*(this->dataPointer)).clear();
#else
    this->data = edfDataType();
#endif
#endif

    for(int item : chanList)
    {
        this->channels.push_back( temp.getChannels()[ item ] );

#if !DATA_IN_CHANS

#if DATA_POINTER
        (*(this->dataPointer)).push_back( temp.getData()[ item ] );
#else
        this->data.push_back( temp.getData()[ item ] );
#endif

#endif

    }
#else
    for(int i = 0; i < chanList.length(); ++i)
    {
        this->channels[i] = this->channels[ chanList[i] ];
        this->data[i] = this->data[ chanList[i] ];
    }
#endif

    this->adjustArraysByChannels();
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
#if DATA_POINTER

        /// was 10-Nov-2015
//        (*(this->dataPointer)).erase((*(this->dataPointer)).begin() + chanList[k]);
        /// new 10-Nov-2015
        (*(this->dataPointer)).erase((*(this->dataPointer)).begin() + k);
#else
        this->data.eraseRow(k);
#endif
        /// was 10-Nov-2015
//        this->channels.erase(this->channels.begin() + chanList[k]);
        /// new 10-Nov-2015
        this->channels.erase(std::begin(this->channels) + k);
    }
    this->adjustArraysByChannels();
}
//void removeChannels(const QString & chanStr);


void edfFile::reduceChannels(const QString & chanStr)
{
    /// need fix, doesn't work properly
    QTime myTime;
    myTime.start();

    QStringList lst;
    QStringList leest = chanStr.split(QRegExp("[,;\\s]"), QString::SkipEmptyParts);
    if(leest.last().toInt() - 1 != this->markerChannel)
    {
        cout << "Reduce channels: bad channels list - no markers" << endl;
//        return;
    }

    /// need write a check of channel sequence

    double sign = 0.;
    int lengthCounter = 0; //length of the expression in chars

    for(int k = 0; k < leest.length(); ++k)
    {
        if(QString::number(leest[k].toInt()) == leest[k]) // just copy
        {
#if DATA_POINTER
            (*(this->dataPointer))[k] = (*(this->dataPointer))[leest[k].toInt() - 1];
#else
            this->data[k] = this->data[leest[k].toInt() - 1];
#endif
        }
        else if(leest[k].contains(QRegExp(R"([\+\-\*\/])")))
        {
            lengthCounter = 0;
            lst = leest[k].split(QRegExp(R"([\+\-\*\/])"), QString::SkipEmptyParts);
            for(int h = 0; h < lst.length(); ++h)
            {
                if(QString::number(lst[h].toInt()) != lst[h]) // if not a number between operations
                {
                    cout << "NAN between operators" << endl;
                    return;
                }
            }
            this->channels[k] = this->channels[lst[0].toInt() - 1];

            lengthCounter += lst[0].length();
            for(int h = 1; h < lst.length(); ++h)
            {
                if(leest[k][lengthCounter] == '+') sign = 1.;
                else if(leest[k][lengthCounter] == '-') sign = -1.;
                else //this should never happen!
                {
                    cout << "first sign is not + or -" << endl;
                    return;
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
#if DATA_POINTER

                /// make valarray
                (*(this->dataPointer))[k] = (*(this->dataPointer))[lst[0].toInt() - 1]
                + sign * (*(this->dataPointer))[lst[h].toInt() - 1];

//                std::transform((*(this->dataPointer))[lst[0].toInt() - 1].begin(),
//                        (*(this->dataPointer))[lst[0].toInt() - 1].end(),
//                        (*(this->dataPointer))[lst[h].toInt() - 1].begin(),
//                        (*(this->dataPointer))[k].begin(),
//                        [&](double i, double j){return i + sign * j;}
//                );

#else
                this->data[k] = this->data[lst[0].toInt() - 1]
                        + sign * this->data[lst[h].toInt() - 1];

//                std::transform(this->data[lst[0].toInt() - 1].begin(),
//                               this->data[lst[0].toInt() - 1].end(),
//                               this->data[lst[h].toInt() - 1].begin(),
//                        this->data[k].begin(),
//                        [&](double i, double j){return i + sign * j;}
//                );

#endif

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
            cout << "unknown format of the string" << endl;
            return;
        }
    }
    this->ns = leest.length();
    this->channels.resize(this->ns);
    this->adjustArraysByChannels();

    cout << "reduceChannelsFast: ns = " << ns;
    cout << ", time = " << myTime.elapsed() / 1000. << " sec";
    cout << endl;
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
        cout << "edfFile::setLabels: inappropriate vector size "
             << this->ns << " != " << inLabels.size() << endl;
        return;
    }
    for(int i = 0; i < this->ns; ++i)
    {
        this->channels[i].label = inLabels[i];
        this->labels[i] = inLabels[i];
    }
}

/// exceptions
void edfFile::setChannels(const vector<edfChannel> & inChannels)
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
    temp.data = edfDataType();
    temp.channels.clear();
    int num = 0;
    for(int item : chanList)
    {
        temp.channels.push_back( this->channels[item] );
        /// pewpwepwwpepwpewpepwepwpep
//        temp.data.push_back( newData[item] );
        temp.data.push_back( newData[num++] );
    }
    temp.adjustArraysByChannels(); // set in particular ns = chanList.length();
    temp.writeEdfFile(outPath);
}

void edfFile::fitData(int initSize) // append zeros to whole ndr's
{
    this->ndr = ceil(double(initSize) / (this->srate * this->ddr)); // generality
    this->dataLength = this->ndr * (this->srate * this->ddr);

    for(int i = 0; i < this->ns; ++i)
    {
#if DATA_POINTER
        (*(this->dataPointer))[i].resize(this->dataLength);
        std::fill(begin((*(this->dataPointer))[i]) + initSize,
                  end((*(this->dataPointer))[i]),
                  0.)
#else
        /// valarray strange handling
        resizeValar(data[i], this->dataLength);
//        lineType tempData = data[i];
//        this->data[i].resize(this->dataLength);
//        std::copy(begin(tempData),
//                  end(tempData),
//                  begin(this->data[i]));
#endif

#if DATA_IN_CHANS
        this->channels[i].data.resize(this->dataLength);
        std::fill(this->channels[i].data.begin() + initSize,
                  this->channels[i].data.end(),
                  0.); // optional ?
#endif
    }
}

void edfFile::cutZerosAtEnd() // cut zeros when readEdf, before edfChannels are allocated
{
    int currEnd = this->dataLength;
    bool doFlag = true;

    while(1)
    {
        for(int j = 0; j < this->ns; ++j)
        {
#if DATA_POINTER
            if((*(this->dataPointer))[j][currEnd - 1] != 0.)
            {
                doFlag = false;
                break;
            }
#else
            /// for neurotravel cleaning - generality with digmaxmin
            if(abs(this->data[j][currEnd - 1]) >= 30000) break; // do clean

            if(this->data[j][currEnd - 1] != 0.)
            {
                doFlag = false;
                break;
            }
#endif
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
    this->dataLength = currEnd;
    for(int j = 0; j < this->ns; ++j)
    {
#if DATA_POINTER
        (*(this->dataPointer))[j].resize(this->dataLength);

#else
        /// good removing
        resizeValar(data[j], currEnd);

//        lineType newArr = this->data[j][slice(0, currEnd, 1)];
//        this->data[j] = newArr;

//        std::remove_if(begin(this->data[j]) + currEnd,
//                       end(this->data[j]),
//                       [](double){return true;});

#endif
#if DATA_IN_CHANS
        this->channels[j].data.resize(this->dataLength);
#endif
    }
//    cout << this->data.maxVal() << endl;
    this->ndr = ceil(this->dataLength / (this->srate * this->ddr)); // should be unchanged
}

double edfFile::checkDdr(const QString & inPath)
{
    if(!QFile::exists(inPath))
    {
        cout << "edfFile::checkDdr: file doesn't exist" << endl;
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
    cout << "transformEdfMaps: time elapsed = " << myTime.elapsed() / 1000. << " sec" << endl;
}

void edfFile::repairPhysMax()
{
    bool rewrite = false;
    for(int i = 0; i < this->ns; ++i)
    {
        if(physMin[i] == physMax[i])
        {
            rewrite = true;
            this->physMax[i] = this->physMin[i] + 1.;
        }
    }
    if(rewrite)
    {
        this->writeEdfFile(this->filePath);
    }
}




/// non-members for static operation
void myTransform(int & output, char * input) {output = atoi(input);}
void myTransform(double & output, char * input) {output = atof(input);}
void myTransform(QString & output, char * input) {output = QString(input);}
void myTransform(std::string & output, char * input) {output = std::string(input);}

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
void myTransform(const std::string & input, const int & len, char ** output)
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
