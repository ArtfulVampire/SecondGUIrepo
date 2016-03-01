#include "library.h"

QString setFileName(const QString & initNameOrPath) // append _num before the dot
{
    QString beforeDot = initNameOrPath;
    beforeDot.resize(beforeDot.lastIndexOf('.'));

    QString afterDot = initNameOrPath; //with the dot
    afterDot = afterDot.right(afterDot.length() - afterDot.lastIndexOf('.'));

    QString helpString;
    helpString = beforeDot + afterDot;
    int counter = 0;
    while (QFile::exists(helpString))
    {
        helpString = beforeDot + "_" + QString::number(counter++) + afterDot;
    }
    return helpString;
}

QString getExpNameLib(const QString & filePath) // getFileName
{
    QString hlp;
    hlp = QDir::toNativeSeparators(filePath);
    hlp = hlp.right(hlp.length() - hlp.lastIndexOf(slash()) - 1); // ExpName.edf
    hlp = hlp.left(hlp.lastIndexOf('.')); // ExpName
    return hlp;
}

QString getDirPathLib(const QString & filePath)
{
    QString str = filePath;
    return str.left(str.lastIndexOf(slash()));
}

QString getExt(QString filePath)
{
    QString helpString = QDir::toNativeSeparators(filePath);
    if(helpString.contains('.'))
    {
        helpString = helpString.right(helpString.length() - helpString.lastIndexOf('.') - 1);
        return helpString;
    }
    else
    {
        return QString();
    }
}

QString getFileName(const QString & filePath, bool withExtension)
{
    QString helpString = QDir::toNativeSeparators(filePath);
    helpString = helpString.right(helpString.length() - helpString.lastIndexOf(slash()) - 1);
    if(!withExtension)
    {
        helpString = helpString.left(helpString.lastIndexOf("."));
    }
    return helpString;
}

QString getPicPath(const QString & dataPath,
                   const QDir * ExpNameDir,
                   const int & ns)
{
    QString fileName = QDir::toNativeSeparators(dataPath);
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf(slash()) - 1);
    fileName.replace('.', '_');

    QString helpString = QDir::toNativeSeparators(ExpNameDir->absolutePath() + slash());

    if(dataPath.contains("Realisations"))
    {
        helpString += "Signals";
    }
    else if(dataPath.contains("windows"))
    {
        if(!dataPath.contains("fromreal"))
        {
            helpString += "Signals" + QString(slash()) + "windows";
        }
        else /// fromreals need other path?
        {
            helpString += "Signals" + QString(slash()) + "windows";
        }
    }
    else if(dataPath.contains("cut"))
    {
        helpString += "SignalsCut";
    }
    helpString += slash();
    if(ns == 19)
    {
        helpString += "after";
    }
    else if(ns == 21)
    {
        helpString += "before";
    }
    else
    {
        helpString += "other";
    }
    helpString += slash() + fileName + ".jpg";
    return helpString;
}



int typeOfFileName(const QString & fileName)
{
    QStringList leest;
    int res = 0;
    for(const QString & marker : def::fileMarkers)
    {
        leest.clear();
        leest = marker.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(const QString & filter : leest)
        {
            if(fileName.contains(filter))
            {
                return res;
            }
        }
        ++res;
    }
    return -1;
}




void eyesProcessingStatic(const vector<int> eogChannels,
                          const vector<int> eegChannels,
                          const QString & windowsDir,
                          const QString & outFilePath)
{
    QTime myTime;
    myTime.start();


    QStringList leest;
    makeFullFileList(windowsDir, leest);
    for(QString & item : leest)
    {
        item.prepend(windowsDir + slash());
    }

    const int Size = eogChannels.size() + 1; // usually 3

    matrix dataE;

    //make dataE array to count covariation matrix
    int NumOfSlices = 0;
    int help;
    for(auto filePath : leest)
    {
        readPlainData(filePath,
                      dataE,
                      def::ns,
                      help,
                      NumOfSlices);
        NumOfSlices += help;
    }

    vector<int> signalNums;
    for(int eogNum : eogChannels)
    {
        signalNums.push_back(eogNum);
    }

    matrix matrixInit(Size, Size);
    matrix coefficients(eegChannels.size(), eogChannels.size());

    for(int k = 0; k < eegChannels.size(); ++k)
    {
        signalNums.push_back(eegChannels[k]);
        for(int j = 0; j < Size; ++j)
        {
            for(int z = j; z < Size; ++z)
            {
                matrixInit[j][z] = covariance(  dataE[ signalNums[j] ],
                                                dataE[ signalNums[z] ],
                                                NumOfSlices,
                                                0,
                                                1) / NumOfSlices;
                if(j != z)
                {
                    matrixInit[z][j] = matrixInit[j][z];
                }
                // maybe (NumOfSlices-1), but it's not important here
            }
        }
        matrixInit.invert();
        for(int i = 0; i < eogChannels.size(); ++i)
        {
            coefficients[k][i] = - matrixInit[i][eogChannels.size()]
                                 / matrixInit[eogChannels.size()][eogChannels.size()];
        }
        signalNums.pop_back();
    }

    ofstream outStr;
    outStr.open(outFilePath.toStdString());
    outStr << "NumOfEyesChannels " << eogChannels.size() << endl;
    outStr << "NumOfEegChannels " << eegChannels.size() << endl;
    for(int k = 0; k < eegChannels.size(); ++k)
    {
        for(int i = 0; i < eogChannels.size(); ++i)
        {
            outStr << doubleRound(coefficients[k][i], 3) << "\t";
        }
        outStr << endl;
    }
    outStr.close();

    cout << "eyesProcessing: time elapsed = " << myTime.elapsed() / 1000. << " sec" << endl;
}



void makePaFile(const QString & spectraDir,
                const QStringList & fileNames,
                const double & coeff,
                const QString & outFile,
                const bool svmFlag) /// make enum
{
    //    QTime myTime;
    //    myTime.start();

    ofstream outStream(outFile.toStdString());
    if(!outStream.good())
    {
        cout << "bad out pa-file" << endl;
        return;
    }
    matrix data4;

    int type;
    QString helpString;

    for(const QString & fileName: fileNames)
    {
        type = typeOfFileName(fileName);
        helpString = spectraDir
                     + slash() + fileName;
        readMatrixFile(helpString, data4);



        if(!svmFlag)
        {
            outStream << fileName << endl;
            for(int l = 0; l < def::nsWOM(); ++l) // write PA files without markers
            {
                for(int k = 0; k < def::spLength(); ++k)
                {
                    outStream << doubleRound(data4[l][k] / coeff, 5) << '\t';
                    if(k%10 == 9)
                    {
                        outStream << '\n';
                    }
                }
                outStream << '\n';
            }
            for(int k = 0; k < def::numOfClasses(); ++k)
            {

                outStream << (k==type) << ' ';
            }
            outStream << "\n\n";
            outStream.flush();
        }
        else // svm
        {
            outStream << type << ' ';
            for(int l = 0; l < def::nsWOM(); ++l) // write PA files without markers
            {
                for(int k = 0; k < def::spLength(); ++k)
                {
                    outStream << l * def::spLength() + k << ':' << doubleRound(data4[l][k] / coeff, 5) << ' ';
                }
            }
            outStream << endl;
        }





    }
    outStream.close();
    //    cout << "makePaFile: time elapsed = " << myTime.elapsed()/1000. << " sec" <<endl;
}

void makeFileLists(const QString & path,
                   vector<QStringList> & lst,
                   const QStringList & auxFilters)
{
    QDir localDir(path);
    QStringList nameFilters, leest;
    QString helpString;
    for(const QString & fileMark : def::fileMarkers)
    {
        nameFilters.clear();
        leest.clear();
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(const QString & filter : leest)
        {
            helpString = "*" + filter + "*";
            if(!auxFilters.isEmpty())
            {
                for(const QString & aux : auxFilters)
                {
//                    nameFilters << QString(def::ExpName.left(3) + "*" + aux + helpString);
                    nameFilters << QString("*" + aux + helpString);
                }
            }
            else
            {
//                nameFilters << QString(def::ExpName.left(3) + helpString);
                nameFilters << helpString;

            }
        }
        lst.push_back(localDir.entryList(nameFilters,
                                         QDir::Files,
                                         QDir::Name)); /// Name ~ order
    }
}

void makeFullFileList(const QString & path,
                      QStringList & lst,
                      const QStringList & auxFilters)
{
    QDir localDir(path);
    QStringList nameFilters, leest;
    QString helpString;
    for(const QString & fileMark : def::fileMarkers)
    {
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(const QString & filter : leest)
        {
            helpString = "*" + filter + "*";
            if(!auxFilters.isEmpty())
            {
                for(const QString & aux : auxFilters)
                {
//                    nameFilters << QString(def::ExpName.left(3) + "*" + aux + helpString);
                    nameFilters << QString("*" + aux + helpString);
                }
            }
            else
            {
//                nameFilters << QString(def::ExpName.left(3) + helpString);
                nameFilters << helpString;
            }

        }
    }
//    for(QString str : nameFilters)
//    {
//        cout << str << endl;
//    }
    lst = localDir.entryList(nameFilters,
                             QDir::Files,
                             QDir::Name); /// Name ~ order
}

void makePaStatic(const QString & spectraDir,
                  const int & fold,
                  const double & coeff,
                  const bool svmFlag) /// make enum
{

    QString helpString;
    const QString paPath = def::dir->absolutePath()
                          + slash() + "PA";
    vector<QStringList> lst;
    makeFileLists(spectraDir, lst);

    int len[def::numOfClasses()];
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        len[i] = lst[i].length();
        if(len[i] == 0)
        {
            cout << "no spectra files found of type " << i << " found" << endl;
            return;
        }
    }

    int Length;
    Length = len[0];
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        Length = min(Length, len[i]);
    }

    vector<vector<int>> arr;
    arr.resize(def::numOfClasses());
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        arr[i].resize(len[i]);

        for(int j = 0; j < len[i]; ++j)
        {
            arr[i][j] = j;
        }
    }

    //generality
    if(def::nsWOM() == -1) return;
    if(def::spLength() == -1) return;
    //mix list

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        std::shuffle(arr[i].begin(),
                     arr[i].end(),
                     std::default_random_engine(seed));
    }



    QStringList listToWrite;
    listToWrite.clear();
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        for(int i = 0; i < (len[j] / fold) * (fold - 1); ++i)
        {
            listToWrite << lst[j][arr[j][i]];
        }
    }
    helpString = QDir::toNativeSeparators(paPath + slash());
    if(!svmFlag)
    {
        helpString += "1.pa";
        makePaFile(spectraDir, listToWrite, coeff, helpString);
    }
    else
    {
        helpString += "svm1";
        makePaFile(spectraDir, listToWrite, coeff, helpString, true);
    }




    listToWrite.clear();
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        for(int i = (len[j] / fold) * (fold - 1); i < (len[j] / fold) * fold; ++i)
        {
            listToWrite << lst[j][arr[j][i]];
        }
    }
    helpString = QDir::toNativeSeparators(paPath + slash());
    if(!svmFlag)
    {
        helpString += "2.pa";
        makePaFile(spectraDir, listToWrite, coeff, helpString);
    }
    else
    {
        helpString += "svm2";
        makePaFile(spectraDir, listToWrite, coeff, helpString, true);
    }


    if(!svmFlag)
    {
        /// sorting by classes !!!!!!!!!!!!!!!!!!!!!!!!!!
        /// very important for auto cross-validation
        listToWrite.clear();
        for(int j = 0; j < def::numOfClasses(); ++j)
        {
            for(int i = 0; i < (len[j] / fold) * fold; ++i)
            {
                listToWrite << lst[j][arr[j][i]];
            }
        }
        helpString = QDir::toNativeSeparators(paPath + slash() + "all.pa");
        makePaFile(spectraDir, listToWrite, coeff, helpString);
    }
}



template <typename T>
double distance(const vector<T> &vec1, const vector<T> &vec2, const int &dim)
{
    double dist = 0.;
    //Euclid
    for(int i = 0; i < dim; ++i)
    {
        dist += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return dist;
}









lineType signalFromFile(QString filePath,
                   int channelNumber,
                   int ns)
{
    matrix tempMat;
    int tempInt;
    readPlainData(filePath,
                  tempMat,
                  ns,
                  tempInt);
    return tempMat[channelNumber];
}



void readPaFile(const QString & paFile,
                matrix & dataMatrix,
                vector<int> types,
                vector<QString> & FileName,
                vector<double> & classCount)
{
    const int NetLength = def::nsWOM() * def::spLength();

    ifstream paSrc;
    paSrc.open(paFile.toStdString());
    if(!paSrc.good())
    {
        cout << "readPaFile: bad file" << endl;
        cout << "is_open:\t" << paSrc.is_open() << endl;
        cout << "eof:\t" << paSrc.eof() << endl;
        cout << "good:\t" << paSrc.good() << endl;
        cout << "bad:\t" << paSrc.bad() << endl;
        cout << "fail:\t" << paSrc.fail() << endl;
        cout << "errno = " << errno << endl;

        return;
    }

    vector<double> tempClass(def::numOfClasses());
    double tempVal;

    classCount = vector<double>(def::numOfClasses(), 0);
    FileName.clear();
    types.clear();
    dataMatrix = matrix(); /// not dataMatrix.clear

    std::string tempStr;
    vector<double> tempVec(NetLength + 2); // bias and class

    while(!paSrc.eof())
    {
        paSrc >> tempStr;

        /// no need
        QString tempQStr = QString(tempStr.c_str());
        if(!tempQStr.contains(def::ExpName.left(3))) /// generality
        {
            break;
        }
        FileName.push_back(tempQStr);

        for(int i = 0; i < NetLength; ++i)
        {
            paSrc >> tempVec[i];
        }

        tempVal = 0.;
        for(int i = 0; i < def::numOfClasses(); ++i)
        {
            paSrc >> tempClass[i];
            tempVal += tempClass[i] * i;
        }

        while(paSrc.peek() == int('\n') || paSrc.peek() == int(' '))
        {
            paSrc.get();
        }

//        tempVec[NetLength] = 1.; //bias
//        tempVec[NetLength + 1] = tempVal;

        if(int(tempVal) != tempVal)
        {
            cout << "type is wrong " << tempVal << endl;
            return;
        }

        dataMatrix.push_back(tempVec);
        types.push_back(tempVal);
        classCount[int(tempVal)] += 1.; // set number of vectors of each class
    }
    paSrc.close();
}


void countRCP(QString filePath, QString picPath, double * outMean, double * outSigma)
{
    lineType arr;
    readFileInLine(filePath, arr);

    (*outMean) = mean(arr);
    (*outSigma) = sigma(arr);

    if(!picPath.isEmpty())
    {
        kernelEst(arr, picPath);
    }
}




