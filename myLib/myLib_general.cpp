#include "library.h"


using namespace std;
using namespace std::chrono;
using namespace smallLib;

namespace myLib
{

QString getLabelName(const QString & label)
{
    QRegExp forChan(R"([ ].{1,4}[\-])");
    forChan.indexIn(label);
    QString chanName = forChan.cap();
    chanName.remove(QRegExp(R"([\-\s])"));
    return chanName;
}

QString rerefChannel(const QString & initialRef,
                     const QString & targetRef,
                     const QString & currentNum,
                     const QString & earsChan,
                     const QString & groundChan,
                     const std::vector<QString> & sign)
{
    /// assume there is only one '-' char in channel name
    /// assume the name to be: * Name-Ref *
    /// length of channel and ref are less than 4

    if(targetRef == "A1")
    {
        if(initialRef == "A2")
        {
            return currentNum + sign[0] + earsChan;
        }
        else if(initialRef == "N")
        {
            return currentNum + "-" + groundChan;
        }
        else if(initialRef == "Ar")
        {
            return currentNum + sign[0] + earsChan + "/2";
        }
    }
    else if(targetRef == "A2")
    {
        if(initialRef == "A1")
        {
            return currentNum + sign[1] + earsChan;
        }
        else if(initialRef == "N")
        {
            return currentNum + "-" + groundChan + sign[1] + earsChan;
        }
        else if(initialRef == "Ar")
        {
            return currentNum + sign[1] + earsChan + "/2";
        }
    }
    else if(targetRef == "N")
    {
        if(initialRef == "A1")
        {
            return currentNum + "+" + groundChan;
        }
        else if(initialRef == "A2")
        {
            return currentNum + sign[0] + earsChan + "+" + groundChan;
        }
        else if(initialRef == "Ar")
        {
            return currentNum + sign[0] + earsChan + "/2"  + "+" + groundChan;
        }
    }
    else if(targetRef == "Ar")
    {
        if(initialRef == "A1")
        {
            return currentNum + sign[1] + earsChan + "/2";
        }
        else if(initialRef == "A2")
        {
            return currentNum + sign[0] + earsChan + "/2";
        }
        else if(initialRef == "N")
        {
            return currentNum + "-" + groundChan + sign[1] + earsChan + "/2";
        }
    }
    return currentNum;

}

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

QString getExpNameLib(const QString & filePath, bool shortened) // getFileName
{
    QString hlp;
    hlp = (filePath);
    hlp = hlp.right(hlp.length() - hlp.lastIndexOf(slash) - 1); // ExpName.edf
    hlp = hlp.left(hlp.lastIndexOf('.')); // ExpName
	if(shortened)
	{
		hlp = hlp.left(hlp.indexOf('_')); //
	}
    return hlp;
}

QString getDirPathLib(const QString & filePath)
{
	return filePath.left(filePath.lastIndexOf(slash));
}


QString getFileMarker(const QString & fileName)
{
    for(const QString & fileMark : def::fileMarkers)
    {
        QStringList lst = fileMark.split(' ', QString::SkipEmptyParts);
        for(const QString & filter : lst)
        {
            if(fileName.contains(filter))
            {
                return filter.right(3); // generality markers appearance
            }
        }
    }
    return QString();
}

QString getExt(const QString & filePath)
{
    QString helpString = (filePath);
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
    QString helpString = (filePath);
    helpString = helpString.right(helpString.length() - helpString.lastIndexOf(slash) - 1);
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
    QString fileName = (dataPath);
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf(slash) - 1);
    fileName.replace('.', '_');

    QString helpString = (ExpNameDir->absolutePath() + slash);

    if(dataPath.contains("Reals"))
    {
        helpString += "Signals";
    }
    else if(dataPath.contains("winds"))
    {
        if(!dataPath.contains("fromreal"))
        {
            helpString += "Signals" + slash + "winds";
        }
        else /// fromreals need other path?
        {
            helpString += "Signals" + slash + "winds";
        }
    }
    else if(dataPath.contains("cut"))
    {
        helpString += "SignalsCut";
    }
    helpString += slash;
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
    helpString += slash + fileName + ".jpg";
    return helpString;
}



int getTypeOfFileName(const QString & fileName)
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




void eyesProcessingStatic(const std::vector<int> eogChannels,
                          const std::vector<int> eegChannels,
                          const QString & windsDir,
                          const QString & outFilePath)
{
    QTime myTime;
    myTime.start();


    QStringList leest;
    /// if cut from reals
//    makeFullFileList(windsDir, leest);
    /// if cut from edf
    leest = QDir(windsDir).entryList({"*" + def::plainDataExtension});

    for(QString & item : leest)
    {
        item.prepend(windsDir + slash);
    }

    const uint Size = eogChannels.size() + 1; // usually 3

    matrix dataE{};

    //make dataE array to count covariation matrix
    int NumOfSlices = 0;
    int help;
    for(auto filePath : leest)
    {
        readPlainData(filePath,
                      dataE,
                      help,
                      NumOfSlices);
        NumOfSlices += help;
    }

    std::vector<int> signalNums;
    for(int eogNum : eogChannels)
    {
        signalNums.push_back(eogNum);
    }

    matrix matrixInit(Size, Size);
    matrix coefficients(eegChannels.size(), eogChannels.size());

    for(uint k = 0; k < eegChannels.size(); ++k)
    {
        signalNums.push_back(eegChannels[k]);
        for(uint j = 0; j < Size; ++j)
        {
            for(uint z = j; z < Size; ++z)
            {
                matrixInit[j][z] = prod(dataE[signalNums[j]],
                        dataE[ signalNums[z]])
                        / NumOfSlices;
                // maybe (NumOfSlices-1), but it's not important here
                if(j != z)
                {
                    matrixInit[z][j] = matrixInit[j][z];
                }
            }
        }
        matrixInit.invert();
        for(uint i = 0; i < eogChannels.size(); ++i)
        {
            coefficients[k][i] = - matrixInit[i][eogChannels.size()]
                                 / matrixInit[eogChannels.size()][eogChannels.size()];
        }
        signalNums.pop_back();
    }

    writeMatrixFile(outFilePath,
                    coefficients,
                    "NumOfEegChannels",
                    "NumOfEogChannels");

    cout << "eyesProcessing: time elapsed = " << myTime.elapsed() / 1000. << " sec" << endl;
}




void makeFileLists(const QString & path,
				   std::vector<QStringList> & lst,
                   const QStringList & auxFilters)
{
    QDir localDir(path);
    QStringList nameFilters, leest;
    QString helpString;
    for(const QString & fileMark : def::fileMarkers)
    {
//		cout << fileMark << endl;
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
//		cout << nameFilters.toStdList() << endl;
        lst.push_back(localDir.entryList(nameFilters,
                                         QDir::Files,
                                         QDir::Name)); /// Name ~ order
    }
}

void makeFullFileList(const QString & path,
                      QStringList & lst,
                      const QStringList & auxFilters)
{
    if(def::fileMarkers.isEmpty())
    {
        lst = QDir(path).entryList({"*.edf", "*.EDF", QString("*." + def::plainDataExtension)},
                                   QDir::Files,
								   QDir::Name); /// Name ~ order
    }
    else
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
//                        nameFilters << QString(def::ExpName.left(3) + "*" + aux + helpString);
                        nameFilters << QString("*" + aux + helpString);
                        nameFilters << QString(helpString + aux + "*");

                    }
                }
                else
                {
//                    nameFilters << QString(def::ExpName.left(3) + helpString);
                    nameFilters << helpString;
                }

            }
        }
        lst = localDir.entryList(nameFilters,
                                 QDir::Files,
								 QDir::Name); /// Name ~ order
    }
}


template <typename T>
double distance(const std::vector<T> &vec1, const std::vector<T> &vec2, const int &dim)
{
    double dist = 0.;
    //Euclid
    for(int i = 0; i < dim; ++i)
    {
        dist += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
    }
    return dist;
}

lineType signalFromFile(const QString & filePath,
                   int channelNumber)
{
    matrix tempMat;
    int tempInt;
    readPlainData(filePath,
                  tempMat,
                  tempInt);
    return tempMat[channelNumber];
}

void countRCP(QString filePath, QString picPath, double * outMean, double * outSigma)
{
    lineType arr;
    readFileInLine(filePath, arr);

    (*outMean) = smallLib::mean(arr);
    (*outSigma) = smallLib::sigma(arr);

    if(!picPath.isEmpty())
    {
        kernelEst(arr, picPath);
    }
}

} // namespace myLib



