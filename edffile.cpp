#include "edffile.h"

edfFile::edfFile()
{
}

edfFile::edfFile(int in_ns, int in_dataLength)
{
    this->ns = in_ns;
    this->dataLength = in_dataLength;
}
edfFile::edfFile(const edfFile &other)
{
    this->headerInitialInfo = other.getHeaderInit();
    this->bytes = other.getBytes();
    this->headerReservedField = other.getHeaderReserved();

    this->ndr = other.getNdr();
    this->ddr = other.getDdr();
    this->ns = other.getNs();

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

    this->dataLength = this->ndr * this->nr[0];
    this->filePath = other.getFilePath();
    this->ExpName = other.getExpName();
    this->dirPath = other.getDirPath();
}



edfFile edfFile::operator=(edfFile & other)
{
    this->headerInitialInfo = other.getHeaderInit();
    this->bytes = other.getBytes();
    this->headerReservedField = other.getHeaderReserved();

    this->ndr = other.getNdr();
    this->ddr = other.getDdr();
    this->ns = other.getNs();

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

    this->dataLength = this->ndr * this->nr[0];
    this->filePath = other.getFilePath();
    this->ExpName = other.getExpName();
    this->dirPath = other.getDirPath();
    return (*this);
}

edfFile::edfFile(QString matiLogPath)
{
    FILE* inStr;
    inStr = fopen(matiLogPath, "r");
    int numOfDoubleParams = 8;
    int numOfParams = 15 - 2; // -currTime & quantLength
    int currTimeIndex = 0;

    this->headerInitialInfo = fitString("Edf for AMOD Data", 184);
    this->headerReservedField = fitString("headerReservedField", 44);
    this->headerRest = QString();


    this->filePath = matiLogPath;
    this->ExpName = getExpNameLib(matiLogPath);
    this->dirPath = matiLogPath.left(EDFpath.lastIndexOf( slash() ) );

    this->ns = numOfParams;
    this->bytes = 256 * (this->ns + 1);

    this->ddr = 1.;
    this->labels = {fitString("AMOD amplX", 16),
                    fitString("AMOD amplY", 16),
                    fitString("AMOD freqX", 16),
                    fitString("AMOD freqY", 16),
                    fitString("AMOD targX", 16),
                    fitString("AMOD targY", 16),
                    fitString("AMOD mouseX", 16),
                    fitString("AMOD mouseX", 16),
                    fitString("AMOD tracePerf", 16),
                    fitString("AMOD mouseMove", 16),
                    fitString("AMOD rightAns", 16),
                    fitString("AMOD wrongAns", 16),
                    fitString("AMOD skipdAns", 16)
                   };
    this->transducerType = vector<QString> (this->ns, fitString("AMOD transducer", 80));
    this->transducerType = vector<QString> (this->ns, fitString("AMODdim", 8));

    this->physMin = {0, 0, 0, 0, // ampls, freqs
                     -1, -1, -1, -1, // coordinates
                     0, 0, // status values
                     0,   0,   0}; // answers

    this->physMax = {7, 7, 7, 7,
                     1,  1,  1,  1,
                     3, 1,
                     255, 255, 16383};

    this->digMin = {-32768, -32768, -32768, -32768,
                    -32768, -32768, -32768, -32768,
                    0, 0,
                    0,   0,   0};

    this->digMax = { 32767,  32767,  32767,  32767,
                     32767,  32767,  32767,  32767,
                     3, 1,
                     255, 255, 16383};

    this->prefiltering = vector<QString>(this->ns, QString(fitString("AMOD prefiltering", 80)));
    this->nr = vector<double> (this->ns, def::freq);
    this->reserved = vector<QString>(this->ns, QString(fitString("AMOD reserved", 32)));

    this->data.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->data[i].resize(6 * 60 * def::freq); // for 6 minutes generality
    }

    while(!feof(inStr))
    {
        fscanf(inStr, "%*s%*f"); // currTime & quantLength
        for(int i = 0; i < numOfDoubleParams; ++i)
        {
            fscanf(inStr, "%lf", &data[i][currTimeIndex]);
        }
        for(int i = numOfDoubleParams; i < numOfParams; ++i)
        {
            fscanf(inStr, "%d", &data[i][currTimeIndex]);
        }
        ++currTimeIndex;
    }
    for(int i = currTimeIndex; i < 250 * ceil(currTimeIndex/250.); ++i)
    {
        for(int j = 0; j < numOfDoubleParams; ++j)
        {
            data[j][i] = 0.;
        }
    }

    this->ndr = ceil(currTimeIndex/250.);
    this->dataLength = this->ndr * this->nr[0];
}

edfFile::edfFile(int in_ns,
                 int in_ndr,
                 int in_ddr,
                 vector<double> in_nr,
                 vector < vector<double> > in_data,
                 vector<QString> in_labels,
                 vector<double> in_physMin,
                 vector<double> in_physMax,
                 vector<double> in_digMin,
                 vector<double> in_digMax)
{
    this->ns = in_ns;
    this->ndr = in_ndr;
    this->ddr = in_ddr;
    this->nr = in_nr;
    this->data = in_data;
    this->labels = in_labels;

    this->physMax = in_physMax;
    this->physMin = in_physMin;
    this->digMax = in_digMax;
    this->digMin = in_digMin;
    this->dataLength = this->ndr * this->nr[0];

    this->filePath = QString();
    this->ExpName = QString();
    this->dirPath = QString();

    this->headerInitialInfo = fitString("headerInitialInfo", 184);
    this->bytes = 256 * (this->ns + 1);
    this->headerReservedField = fitString("headerReservedField", 44);
    this->headerRest = QString();

    this->transducerType = vector<QString>(this->ns, QString(fitString("transducerType", 80)));
    this->physDim = vector<QString>(this->ns, QString(fitString("physDim", 8)));
    this->prefiltering = vector<QString>(this->ns, QString(fitString("prefiltering", 80)));
    this->reserved = vector<QString>(this->ns, QString(fitString("reserved", 32)));
}

template <typename Typ>
void edfFile::handleParam(Typ & qStr,
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
        fwrite(array, sizeof(char), length, headerFile);
        delete []array;
    }
    else
    {
        myTransform(qStr, length, &array);
        fprintf(ioFile, "%s", array);
    }
}


//void edfFile::myTransform(short & output, char * input)
//{
//    output = atoi(input);
//}
void edfFile::myTransform(int & output, char * input)
{
    output = atoi(input);
}
void edfFile::myTransform(double & output, char * input)
{
    output = atof(input);
}
void edfFile::myTransform(QString & output, char * input)
{
    output = QString(input);
}

//void edfFile::myTransform(short & input, const int & len, char * output)
//{
//    output = QStrToCharArr(fitNumber(input, len));
//}
void edfFile::myTransform(int & input, const int & len, char ** output)
{
    (*output) = QStrToCharArr(fitNumber(input, len));
}
void edfFile::myTransform(double & input, const int & len, char ** output)
{
    (*output) = QStrToCharArr(fitNumber(input, len));
}
void edfFile::myTransform(QString & input, const int & len, char ** output)
{
    (*output) = QStrToCharArr(input);
}

template <typename Typ>
void edfFile::handleParamArray(vector <Typ> & qStr,
                               int number,
                               int length,
                               bool readFlag,
                               FILE *ioFile,
                               FILE * headerFile)
{
    for(int i = 0; i < number; ++i)
    {
        if(readFlag) qStr.push_back(Typ());
        handleParam <Typ> (qStr[i], length, readFlag, ioFile, headerFile);
    }
}

void edfFile::readEdfFile(QString EDFpath, bool matiFlag, bool ntFlag)
{
    QTime myTime;
    myTime.start();
    handleEdfFile(EDFpath, true, matiFlag, ntFlag);
    cout << "readEdfFile: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void edfFile::writeEdfFile(QString EDFpath, bool matiFlag, bool ntFlag)
{
    QTime myTime;
    myTime.start();
    handleEdfFile(EDFpath, false, matiFlag, ntFlag);
    cout << "writeEdfFile: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

// readFlag: 1 - read, 0 - write
void edfFile::handleEdfFile(QString EDFpath, bool readFlag, bool matiFlag, bool ntFlag)
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
        cout << "handleFile: cannot open edf file" << endl;
        return;
    }



    filePath = EDFpath;
    dirPath = EDFpath.left(EDFpath.lastIndexOf(slash()));

    ExpName = getExpNameLib(filePath);

    FILE * header;
    helpString = dirPath + slash() + "header.txt";
    header = fopen(helpString, "w");
    if(header == NULL)
    {
        cout << "handleFile: cannot open header.txt file" << endl;
        return;
    }


    handleParam(headerInitialInfo, 184, readFlag, edfDescriptor, header);
    handleParam(bytes, 8, readFlag, edfDescriptor, header);
    handleParam(headerReservedField, 44, readFlag, edfDescriptor, header);
    handleParam(ndr, 8, readFlag, edfDescriptor, header);
    handleParam(ddr, 8, readFlag, edfDescriptor, header);
    handleParam(ns, 4, readFlag, edfDescriptor, header);
    handleParamArray(labels, ns, 16, readFlag, edfDescriptor, header);

    //edit EOG channels generality for encephalan
    for(int i = 0; i < ns; ++i)
    {
        if(labels[i].contains("EOG 1"))
        {
            labels[i] = "EOG EOG1-A2     ";
        }
        else if(labels[i].contains("EOG 2"))
        {
            labels[i] = "EOG EOG2-A1     ";
        }
        else if(labels[i].contains("Marker"))
        {
            markerChannel = i;
        }
    }

    helpString = QDir::toNativeSeparators( dirPath + slash() + "labels.txt" );
    FILE * labelsFile;
    labelsFile = fopen(helpString, "w");
    for(int i = 0; i < ns; ++i)                         //labels write in file
    {
        fprintf(labelsFile, "%s\n", labels[i].toStdString().c_str());
    }
    fclose(labelsFile);

    handleParamArray(transducerType, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(physDim, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(physMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(physMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(prefiltering, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(nr, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(reserved, ns, 32, readFlag, edfDescriptor, header);
    handleParam(headerRest, int(bytes-(ns+1)*256), readFlag, edfDescriptor, header);
    fclose(header);

    fpos_t *position = new fpos_t;
    fgetpos(edfDescriptor, position);
    fclose(edfDescriptor);
    edfDescriptor = fopen(EDFpath, readFlag?"rb":"ab");
    fsetpos(edfDescriptor, position);
    delete position;

    if(readFlag)
    {
        dataLength = ndr * nr[0]; // generality
    }


    handleData(readFlag, matiFlag, ntFlag, edfDescriptor);

    fclose(edfDescriptor);
}

void edfFile::handleData(bool readFlag,
                         bool matiFlag,
                         bool ntFlag,
                         FILE * edfForData)
{
    int currTimeIndex;
    QString helpString;
    vector < QString > annotations;
    if(readFlag)
    {
        data.resize(ns);
        for(int i = 0; i < ns; ++i)
        {
            data[i].resize(dataLength);
        }
    }

//    if(readFlag)
//    {
//        data = new double * [ns];
//        for(int i = 0; i < ns; ++i)
//        {
//            data[i] = new double [dataLength];
//        }
//    }

    for(int i = 0; i < ndr; ++i)
    {
        for(int currNs = 0; currNs < ns; ++currNs)
        {
            helpString.clear();
            for(int k = 0; k < nr[currNs]; ++k)
            {
                currTimeIndex = i * nr[currNs] + k;
                handleDatum(currNs, currTimeIndex, readFlag, matiFlag, ntFlag, helpString, edfForData);
            }

            if(currNs == markerChannel && ntFlag)
            {
                handleAnnotations(currNs, currTimeIndex, helpString, annotations);
            }
        }
    }

}

void edfFile::handleDatum(const int & currNs,
                          const int & currTimeIndex,
                          bool readFlag,
                          bool matiFlag,
                          bool ntFlag,
                          QString & ntAnnot,
                          FILE * edfForDatum)
{
    char helpChar = '0';
    short a = 0;
    unsigned short markA = 0;

    double & currDatum = data[currNs][currTimeIndex];

    if(readFlag)
    {
        if(currNs != markerChannel) // usual data read
        {
            fread(&a, sizeof(short), 1, edfForDatum);
//            currDatum = physMin[currNs] +
//                    (physMax[currNs] - physMin[currNs])
//                    * (double(a)  -digMin[currNs])
//                    / (digMax[currNs] - digMin[currNs]);   //neurotravel

            currDatum = a * 1./8.; // generality encephalan

        }
        else //if markers channel
        {
            if(ntFlag)
            {
                //edf+
                //            fscanf(edf, "%c", &helpChar);
                fread(&helpChar, sizeof(char), 1, edfForDatum);
                ntAnnot += helpChar;
                //            fscanf(edf, "%c", &helpChar);
                fread(&helpChar, sizeof(char), 1, edfForDatum);
                ntAnnot += helpChar;
            }
            else if(matiFlag)
            {
                fread(&markA, sizeof(unsigned short), 1, edfForDatum);
                currDatum = markA;


                if(currDatum != 0 )
                {
                    matiFixMarker(currDatum);
                }
            }
            else // simple encephalan
            {
                fread(&a, sizeof(short), 1, edfForDatum);
//                currDatum = physMin[currNs] +
//                        (physMax[currNs] - physMin[currNs])
//                        * (double(a) - digMin[currNs])
//                        / (digMax[currNs] - digMin[currNs]);

                currDatum = a; //generality encephalan
            }

            if(!ntFlag && currDatum != 0)
            {
                writeMarker(currNs, currTimeIndex, matiFlag);
            }

            if(currDatum == 200 )
            {
                staSlice = currTimeIndex;
            }
        }
    }
    else //if write
    {
        if(currNs != markerChannel) // usual data read
        {
//            a = (short)( (currDatum - physMin[currNs])
//                        * (digMax[currNs] - digMin[currNs])
//                        / (physMax[currNs] - physMin[currNs])
//                        + digMin[currNs]);

            a  = (short)(currDatum * 8.); // generality encephalan

            fwrite(&a, sizeof(short), 1, edfForDatum);
        }
        else //if markers channel
        {
            if(ntFlag) ////////////////////////// to do???
            {
                //edf+
//                fwrite(&helpChar, sizeof(char), 1, edfDescriptor);
            }
            else if(matiFlag)
            {
                markA = (unsigned short) (currDatum);
                fwrite(&markA, sizeof(unsigned short), 1, edfForDatum);
            }
            else // simple encephalan
            {
                a = (short) (currDatum);
                fwrite(&a, sizeof(short), 1, edfForDatum);
            }
        }
    }
}


void edfFile::writeMarker(const int & currNs,
                           const int & currTimeIndex,
                           bool matiFlag)
{
    vector<bool> byteMarker;
    QString helpString;
    double & currDatum = data[currNs][currTimeIndex];

    FILE * markers;


    helpString = dirPath + slash() + ExpName + "_markers.txt";
    markers = fopen(helpString, "a+");
    fprintf(markers, "%d %d", currTimeIndex, int(currDatum)); //////////////////// danger int()
    if(matiFlag)
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
                                vector<QString> annotations)
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
    if(ntFlag)
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
//        nr[ns-1] = def::freq; // generality freq change
    }
    */

}


void edfFile::getDataCopy(dataType & destination) const
{
    destination = data;
}

//const edfFile::dataType & edfFile::getData() const
//{
//    return data;
//}

const int & edfFile::getNs() const
{
    return ns;
}
const int & edfFile::getNdr() const
{
    return ndr;
}
const int & edfFile::getDdr() const
{
    return ddr;
}
const int & edfFile::getBytes() const
{
    return bytes;
}
const int & edfFile::getDataLen() const
{
    return dataLength;
}
const int & edfFile::getMarkChan() const
{
    return markerChannel;
}
const vector<double> & edfFile::getNr() const
{
    return nr;
}
const QString & edfFile::getFilePath() const
{
    return filePath;
}
const QString & edfFile::getDirPath() const
{
    return dirPath;
}
const QString & edfFile::getExpName() const
{
    return ExpName;
}
const QString & edfFile::getHeaderInit() const
{
    return headerInitialInfo;
}
const QString & edfFile::getHeaderReserved() const
{
    return headerReservedField;
}
const QString & edfFile::getHeaderRest() const
{
    return headerRest;
}


const vector<QString> & edfFile::getLabels() const
{
    return labels;
}
const vector<QString> & edfFile::getTransducer() const
{
    return transducerType;
}
const vector<QString> & edfFile::getPrefiltering() const
{
    return prefiltering;
}
const vector<QString> & edfFile::getPhysDim() const
{
    return physDim;
}
const vector<QString> & edfFile::getReserved() const
{
    return reserved;
}
const vector<double> & edfFile::getPhysMax() const
{
    return physMax;
}
const vector<double> & edfFile::getPhysMin() const
{
    return physMin;
}
const vector<double> & edfFile::getDigMax() const
{
    return digMax;
}
const vector<double> & edfFile::getDigMin() const
{
    return digMin;
}
