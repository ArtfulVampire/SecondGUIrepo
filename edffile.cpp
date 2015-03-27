#include "edffile.h"

edfFile::edfFile()
{
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
    //    ns * 16 ascii : ns * label (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)
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

    cout << "1" << endl;

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
    handleParamArray(label, ns, 16, readFlag, edfDescriptor, header);
    //edit EOG channels generality for encephalan
    for(int i = 0; i < ns; ++i)
    {
        if(label[i].contains("EOG 1"))
        {
            label[i] = "EOG EOG1-A2     ";
        }
        else if(label[i].contains("EOG 2"))
        {
            label[i] = "EOG EOG2-A1     ";
        }
        else if(label[i].contains("Marker"))
        {
            markerChannel = i;
        }
    }

    helpString = QDir::toNativeSeparators( dirPath + slash() + "labels.txt" );
    FILE * labels;
    labels = fopen(helpString, "w");
    for(int i = 0; i < ns; ++i)                         //label write in file
    {
        fprintf(labels, "%s\n", label[i].toStdString().c_str());
    }
    fclose(labels);

    handleParamArray(transducerType, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(physDim, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(physMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(physMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMin, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(digMax, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(prefiltering, ns, 80, readFlag, edfDescriptor, header);
    handleParamArray(nr, ns, 8, readFlag, edfDescriptor, header);
    handleParamArray(reserved, ns, 32, readFlag, edfDescriptor, header);
    handleParam(restOfHeader, int(bytes-(ns+1)*256), readFlag, edfDescriptor, header);
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
            data[i].reserve(dataLength);
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

const edfFile::dataType & edfFile::getData() const
{
//    const dataType & result = data;
//    return result;
    return data;
}
void edfFile::getDataCopy(dataType & destination) const
{
    destination = data;
}
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
