#include "edffile.h"
using namespace std;

namespace repair
{
using namespace myLib;

void repairPhysBadChan(const QString & dirPath, const QStringList & filters)
{
    for(QString str : QDir(dirPath).entryList(filters))
    {
        edfFile feel;
        feel.readEdfFile(dirPath + slash + str);
        feel.repairPhysMax();
    }
}

void checkPhysBadChan(const QString & dirPath, const QStringList & filters)
{
    QDir(dirPath).mkdir("bad");
    for(QString str : QDir(dirPath).entryList(filters))
    {
        edfFile feel;
        feel.readEdfFile(dirPath + slash + str);
        for(int i = 0; i < feel.getNs(); ++i)
        {
            if(feel.getPhysMin()[i] == feel.getPhysMax()[i])
            {
                QFile::rename(dirPath + slash + str,
                              dirPath + slash + "bad" + slash + str);
            }
        }
    }
}

void renameFileToLatin(const QString & filePath)
{
    QString dirName = getDirPathLib(filePath);
    QString fileName = getFileName(filePath);
    QString newFileName;
    for(const QChar & ch : fileName)
    {
        int num = ch.unicode();
        if(coords::kyrToLatin.find(num) != coords::kyrToLatin.end())
        {
            newFileName += coords::kyrToLatin.at(num);
        }
        else
        {
            newFileName += ch;
        }
    }
//    cout << newFileName << endl;
    QFile::rename(filePath, dirName + slash + newFileName);
}

void dirToLatin(const QString & dirPath, const QStringList & filters)
{
    QStringList leest;
    if(!filters.empty())
    {
        leest = QDir(dirPath).entryList(filters);
    }
    else
    {
        leest = QDir(dirPath).entryList();
    }

    for(const QString & str : leest)
    {
        renameFileToLatin(dirPath + slash + str);
    }
}

void deleteSpaces(const QString & dirPath, const QStringList & filters)
{
    QDir tmp(dirPath);
    QStringList lst;
    if(filters.isEmpty())
    {
        lst = tmp.entryList(QDir::Files);
    }
    else
    {
        lst = tmp.entryList(filters, QDir::Files);
    }
    QString newName;
    for(const QString & fileName : lst)
    {
        newName = fileName;
        newName.replace(' ', '_');
        newName.remove(R"(')");
        newName.replace("__", "_");
        QFile::rename(tmp.absolutePath() + slash + fileName,
                      tmp.absolutePath() + slash + newName);
    }
}


bool testChannelsOrderConsistency(const QString & dirPath)
{
    std::vector<QString> labelsBC;
    std::vector<QString> labels;
    edfFile fil;
    QStringList leest = QDir(dirPath).entryList({"*.edf", "*.EDF"});
//    cout << "main: " << leest[0] << endl;

    fil.readEdfFile(dirPath + slash + leest[0], true);
    labelsBC = fil.getLabels();
    for(int i = 0; i < 5; ++i)
    {
        auto bad = std::find_if(std::begin(labelsBC),
                                std::end(labelsBC),
                                [&labelsBC](const QString & in)
        {
            return !in.contains("EEG", Qt::CaseInsensitive);
        });
        if(bad != std::end(labelsBC))
        {
            labelsBC.erase(bad);
        }
    }

    bool res = true;


    for(const QString & guy : leest)
    {
        fil.readEdfFile(dirPath + slash + guy, true);
        labels = fil.getLabels();

        for(int i = 0; i < 5; ++i)
        {
            auto bad = std::find_if(std::begin(labels),
                                    std::end(labels),
                                    [&labels](const QString & in)
            {
                return !in.contains("EEG", Qt::CaseInsensitive);
            });
            if(bad != std::end(labels))
            {
                labels.erase(bad);
            }
        }

        if(labels != labelsBC)
        {
            cout << guy << endl;
            res = false;
        }
    }
    cout << "testChannelsOrderConsistency:\n" << dirPath << "\t" << res << endl;
    return res;
}


void repairChannelsOrder(const QString & inFilePath,
                         QString outFilePath,
                         const std::vector<QString> & standard)
{
    if(outFilePath.isEmpty())
    {
        outFilePath = inFilePath;
        outFilePath.replace(".edf", "_goodChan.edf");
    }

    std::vector<int> reorderChanList{};
    edfFile initFile;
    initFile.readEdfFile(inFilePath, true);
    for(uint i = 0; i < standard.size(); ++i) /// only for 31 channels
    {
        for(int j = 0; j < initFile.getNs(); ++j)
        {
            if(initFile.getLabels()[j].contains(standard[i]))
            {
                reorderChanList.push_back(j);
                break;
            }
        }
    }
    // fill the rest of channels
    for(int j = 0; j < initFile.getNs(); ++j)
    {
        if(std::find(std::begin(reorderChanList),
                     std::end(reorderChanList),
                     j)
           == std::end(reorderChanList))
        {
            reorderChanList.push_back(j);
        }
    }

    std::vector<int> ident(initFile.getNs());
    std::iota(std::begin(ident), std::end(ident), 0);
    if(reorderChanList != ident)
    {
        initFile.readEdfFile(inFilePath);
        initFile.reduceChannels(reorderChanList);
        initFile.writeEdfFile(outFilePath);
    }
    else
    {
        QFile::copy(inFilePath, outFilePath);
    }
}

void repairChannels(const QString & inDirPath,
                    const QString & outDirPath,
                    const std::vector<QString> & standard)
{
    const auto leest = QDir(inDirPath).entryList(def::edfFilters, QDir::Files);
    const auto vec = leest.toVector();

//#pragma omp parallel
//#pragma omp for nowait schedule(dynamic,3)
    for(int i = 0; i < vec.size(); ++i)
    {
        QString outName = vec[i];
//        outName.replace(".edf", "_goodChan.edf", Qt::CaseInsensitive);
//        cout << outName << endl;
        repairChannelsOrder(inDirPath + slash + vec[i],
                            outDirPath + slash + outName,
                            standard);
    }
}

} /// end namespace repair
