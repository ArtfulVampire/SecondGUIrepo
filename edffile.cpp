#include "edffile.h"

edfFile::edfFile()
{
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
    this->channels = other.getChannels();
    this->markerChannel = other.getMarkChan();
    this->filePath = other.getFilePath();
    this->ExpName = other.getExpName();
    this->dirPath = other.getDirPath();
}



edfFile edfFile::operator=(const edfFile & other)
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
    this->channels = other.getChannels();
    this->markerChannel = other.getMarkChan();
    this->filePath = other.getFilePath();
    this->ExpName = other.getExpName();
    this->dirPath = other.getDirPath();
    return (*this);
}

edfFile::edfFile(QString matiLogPath)
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
    int numOfDoubleParams = 8;
    int numOfParams = 15 - 2; // -currTime & quantLength
    int currTimeIndex;

    this->headerInitialInfo = fitString("Edf for AMOD Data", 184);
    this->headerReservedField = fitString("headerReservedField", 44);
    this->headerRest = QString();


    this->filePath = matiLogPath;
    this->ExpName = getExpNameLib(matiLogPath);
    this->dirPath = matiLogPath.left(matiLogPath.lastIndexOf( slash() ) );

    this->ns = numOfParams;
    this->ddr = 1.;
    this->nr = vector<double> (this->ns, def::freq);
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
    this->transducerType = vector<QString> (this->ns, fitString("AMOD transducer", 80));
    this->physDim = vector<QString> (this->ns, fitString("AMODdim", 8));

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
                     255-1, 255-1, 255-1};

    this->prefiltering = vector<QString>(this->ns, QString(fitString("AMOD no prefiltering", 80)));
    this->reserved = vector<QString>(this->ns, QString(fitString("AMOD reserved", 32)));

    this->data.resize(this->ns);
    for(int i = 0; i < this->ns; ++i)
    {
        this->data[i].resize(6 * 60 * def::freq); // for 6 minutes generality
    }

    currTimeIndex = 0;
    while(!feof(inStr))
    {
        fscanf(inStr, "%*d %*f"); // quantLength & currTime
        for(int i = 0; i < numOfParams; ++i)
        {
            fscanf(inStr, "%lf", &data[i][currTimeIndex]);

        }
        ++currTimeIndex;
    }
    --currTimeIndex; // to loose the last read string;
    for(int i = 0; i < currTimeIndex; ++i)
    {
//        cout << data[numOfDoubleParams - 2][i] << endl;
    }
    this->ndr = ceil(currTimeIndex/250.);
    this->dataLength = this->ndr * def::freq;

    for(int j = 0; j < numOfParams; ++j)
    {
        for(int i = currTimeIndex; i < this->dataLength; ++i)
        {
            data[j][i] = 0.; //.push_back(0.);
        }
        data[j].resize(dataLength);
    }

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
                                            reserved[i],
                                            data[i])
                                 );
    }
}
/*
edfFile::edfFile(int in_ndr, int in_ns,
                 int in_ddr,
                 vector<QString> in_labels,
                 vector<double> in_physMin,
                 vector<double> in_physMax,
                 vector<double> in_digMin,
                 vector<double> in_digMax,
                 vector<double> in_nr,
                 vector < vector<double> > in_data)
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
                                               reserved[i],
                                               data[i])
                                    );
    }
}
*/

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
        if(headerFile != NULL)
        {
            fwrite(array, sizeof(char), length, headerFile);
        }
        delete []array;
    }
    else
    {
        myTransform(qStr, length, &array);
        fprintf(ioFile, "%s", array);
    }
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

void edfFile::readEdfFile(QString EDFpath)
{
    QTime myTime;
    myTime.start();
    handleEdfFile(EDFpath, true);
//    cout << "readEdfFile: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void edfFile::writeEdfFile(QString EDFpath, bool asPlain)
{
    QTime myTime;
    myTime.start();
    if(!asPlain)
    {
        if(!QFile::exists(EDFpath))
        {
            this->fitData(this->data[0].size());
            this->handleEdfFile(EDFpath, false);
        }
        else
        {
            cout << "writeEdfFile: destination file already exists = \n" << EDFpath << endl;
        }
    }
    else // if(asPLain)
    {
        ofstream outStr;
        outStr.open(EDFpath.toStdString().c_str());
        outStr << "NumOfSlices " << this->dataLength << endl;
        for (int i = 0; i < this->dataLength; ++i)
        {
            for(int j = 0; j < this->ns; ++j)
            {
                outStr << fitNumber(doubleRound(this->data[j][i], 3), 7) << '\t';
            }
            outStr << '\n';
        }
        outStr.flush();
        outStr.close();
    }

//    cout << "writeEdfFile: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

// readFlag: 1 - read, 0 - write
void edfFile::handleEdfFile(QString EDFpath, bool readFlag)
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
    if(readFlag)
    {
        header = fopen(helpString, "w");
    if(header == NULL)
    {
        cout << "handleFile: cannot open header.txt file" << endl;
        return;
    }
    }


    handleParam(headerInitialInfo, 184, readFlag, edfDescriptor, header);
    handleParam(bytes, 8, readFlag, edfDescriptor, header);
    handleParam(headerReservedField, 44, readFlag, edfDescriptor, header);
    handleParam(ndr, 8, readFlag, edfDescriptor, header);
    handleParam(ddr, 8, readFlag, edfDescriptor, header);
    handleParam(ns, 4, readFlag, edfDescriptor, header);





    //start channels read
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

    if(readFlag)
    {
        helpString = QDir::toNativeSeparators( dirPath + slash() + "labels.txt" );
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
    handleParamArray(physMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(physMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(prefiltering, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(nr, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(reserved, ns, 32, readFlag, edfDescriptor, header);
    //end channels read




    handleParam(headerRest, int(bytes-(ns+1)*256), readFlag, edfDescriptor, header);
    if(readFlag)
    {
        fclose(header);
    }

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


    handleData(readFlag, edfDescriptor);
    fclose(edfDescriptor);

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
                                            reserved[i],
                                            data[i])
                                 );
    }
}

void edfFile::handleData(bool readFlag,
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
    for(int i = 0; i < ndr; ++i)
    {
        for(int currNs = 0; currNs < ns; ++currNs)
        {
            helpString.clear();
            for(int k = 0; k < nr[currNs]; ++k)
            {
                currTimeIndex = i * nr[currNs] + k;
                handleDatum(currNs, currTimeIndex, readFlag, helpString, edfForData);
            }

            if(currNs == markerChannel && this->ntFlag)
            {
                handleAnnotations(currNs, currTimeIndex, helpString, annotations);
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
    short a = 0;
    unsigned short markA = 0;

    double & currDatum = data[currNs][currTimeIndex];

    if(readFlag)
    {
        if(currNs != markerChannel) // usual data read
        {
            fread(&a, sizeof(short), 1, edfForDatum);
            currDatum = physMin[currNs]
                    + (physMax[currNs] - physMin[currNs])
                    * (double(a) - digMin[currNs])
                    / (digMax[currNs] - digMin[currNs]  + 1);

//            currDatum = a * 1./8.; // generality encephalan

        }
        else //if markers channel
        {
            if(this->ntFlag)
            {
                //edf+
                //            fscanf(edf, "%c", &helpChar);
                fread(&helpChar, sizeof(char), 1, edfForDatum);
                ntAnnot += helpChar;
                //            fscanf(edf, "%c", &helpChar);
                fread(&helpChar, sizeof(char), 1, edfForDatum);
                ntAnnot += helpChar;
            }
            else if(this->matiFlag)
            {
                fread(&markA, sizeof(unsigned short), 1, edfForDatum);
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
            else // simple encephalan
            {
                fread(&a, sizeof(short), 1, edfForDatum);
                currDatum = physMin[currNs]
                        + (physMax[currNs] - physMin[currNs])
                        * (double(a) - digMin[currNs])
                        / (digMax[currNs] - digMin[currNs]);

//                currDatum = a; //generality encephalan
            }

            if(!this->ntFlag && currDatum != 0 && readFlag) // make markers file when read only
            {
                writeMarker(currNs, currTimeIndex);
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
            a = (short)( (currDatum - physMin[currNs])
                        * (digMax[currNs] - digMin[currNs] + 1)
                        / (physMax[currNs] - physMin[currNs])
                        + digMin[currNs]);

//            a  = (short)(currDatum * 8.); // generality encephalan

            fwrite(&a, sizeof(short), 1, edfForDatum);
        }
        else //if markers channel
        {
            if(this->ntFlag) ////////////////////////// to do???
            {
                //edf+
//                fwrite(&helpChar, sizeof(char), 1, edfDescriptor);
            }
            else if(this->matiFlag)
            {
//                markA = (unsigned short) (currDatum);
                markA = (unsigned short)( (currDatum - physMin[currNs])
                                          * (digMax[currNs] - digMin[currNs])
                                          / (physMax[currNs] - physMin[currNs])
                                          + digMin[currNs]);
                fwrite(&markA, sizeof(unsigned short), 1, edfForDatum);
            }
            else // simple encephalan
            {
//                a = (short) (currDatum);
                a = (short)( (currDatum - physMin[currNs])
                            * (digMax[currNs] - digMin[currNs])
                            / (physMax[currNs] - physMin[currNs])
                            + digMin[currNs]);
                fwrite(&a, sizeof(short), 1, edfForDatum);
            }
        }
    }
}


void edfFile::writeMarker(const int & currNs,
                           const int & currTimeIndex)
{
    vector<bool> byteMarker;
    QString helpString;
    double & currDatum = data[currNs][currTimeIndex];

    FILE * markers;


    helpString = dirPath + slash() + ExpName + "_markers.txt";
    markers = fopen(helpString, "a+");
    fprintf(markers, "%d %d", currTimeIndex, int(currDatum)); //////////////////// danger int()
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
//        nr[ns-1] = def::freq; // generality freq change
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
        if(labels[i].contains("Markers"))
        {
            markerChannel = i;
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

    this->physMax.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->physMax.push_back(this->channels[i].physMax);
    }

    this->physMin.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->physMin.push_back(this->channels[i].physMin);
    }

    this->digMax.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->digMax.push_back(this->channels[i].digMax);
    }

    this->digMin.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->digMin.push_back(this->channels[i].digMin);
    }

    this->prefiltering.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->prefiltering.push_back(this->channels[i].prefiltering);
    }

    this->nr.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->nr.push_back(this->channels[i].nr);
    }

    this->reserved.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->reserved.push_back(this->channels[i].reserved);
    }

    this->data.clear();
    for(int i = 0; i < this->ns; ++i)
    {
        this->data.push_back(this->channels[i].data);
    }
    this->headerRest = QString();

    this->dataLength = this->data[0].size(); // dunno
    this->ndr = ceil(this->data[0].size() / def::freq); // generality


}

void edfFile::appendChannel(edfChannel addChan, QString outPath)
{
    this->channels.push_back(addChan);
    this->adjustArraysByChannels();
    this->writeEdfFile(outPath);
}

void edfFile::appendFile(QString addEdfPath, QString outPath)
{
    edfFile addEdf;
    addEdf.readEdfFile(addEdfPath);
    edfChannel tempMarkChan = this->channels[markerChannel]; // save markerChannel
    this->channels.erase(this->channels.begin() + markerChannel); // remove markerChannel
    for(int i = 0; i < addEdf.getNs(); ++i)
    {
        this->channels.push_back(addEdf.getChannels()[i]);
    }
    this->channels.push_back(tempMarkChan); // return markerChannel to the end of a list
    this->adjustArraysByChannels();
    this->writeEdfFile(outPath);
}

void edfFile::swapChannels(int num1, int num2)
{
    edfChannel tmp = this->channels[num1];
    this->channels[num1] = this->channels[num2];
    this->channels[num2] = tmp;
}


void edfFile::saveSubsection(int startBin, int finishBin, QString outPath, bool plainFlag) // [start, finish)
{
    const edfFile temp = (*this);

    for(int i = 0; i < this->ns; ++i)
    {
        this->data[i].erase(this->data[i].begin() + finishBin, this->data[i].end());
        this->data[i].erase(this->data[i].begin(), this->data[i].begin() + startBin);
    }

    this->ndr = ceil((finishBin - startBin) / def::freq);
    this->dataLength = this->ndr * def::freq;
    this->writeEdfFile(outPath, plainFlag);
    (*this) = temp;

//    this->writeEdfFile(setFileName(this->filePath) ); // for test
}




void edfFile::saveOtherData(vector < vector <double> > newData, QString outPath, QList<int> chanList)
{
    const edfFile temp(*this);

    //adjust channels
    this->channels.clear();
    for(int i = 0; i < chanList.length(); ++i)
    {
        this->channels.push_back( temp.getChannels()[chanList[i]] ); // count from 1 generality
        this->channels[i].data = newData[i];
    }
    this->adjustArraysByChannels();

    //adjust data
    this->ndr = ceil(newData[0].size() / def::freq); // generality
    this->dataLength = this->ndr * def::freq;
    this->writeEdfFile(outPath);
    *this = temp;

}

void edfFile::saveOtherData(double ** newData, int newDataLength, QString outPath, QList<int> chanList)
{
    const edfFile temp(*this);

    //adjust channels
    this->channels.clear();
    for(int i = 0; i < chanList.length(); ++i)
    {
        this->channels.push_back( temp.getChannels()[chanList[i]] ); // count from 1 generality
        this->channels[i].data.resize(newDataLength);
        memcpy(this->channels[i].data.data(), newData[i], sizeof(double) * newDataLength);
    }
    this->adjustArraysByChannels();

    this->writeEdfFile(outPath);
    *this = temp;
}

void edfFile::fitData(int initSize)
{
    this->ndr = ceil(initSize / def::freq); // generality
    this->dataLength = this->ndr * def::freq;
    for(int i = 0; i < this->ns; ++i)
    {
        for(int j = initSize; j < this->dataLength; ++j)
        {
            this->channels[i].data.push_back(0.);
            this->data[i].push_back(0.);
        }
    }

}
