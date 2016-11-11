#include "edffile.h"
using namespace std;
using namespace myLib;

namespace repair
{

void physMinMaxDir(const QString & dirPath)
{
    for(QString str : QDir(dirPath).entryList(def::edfFilters))
    {
        edfFile feel;
        feel.readEdfFile(dirPath + slash + str);
        feel.repairPhysMax();
    }
}

void physMinMaxCheck(const QString & dirPath)
{
    QDir(dirPath).mkdir("bad");
    for(QString str : QDir(dirPath).entryList(def::edfFilters))
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


void scalingFactorFile(const QString & inFilePath,
                       QString outFilePath)
{
    if(outFilePath.isEmpty())
    {
        outFilePath = inFilePath;
        outFilePath.replace(".edf", "_scal.edf", Qt::CaseInsensitive);
    }
    edfFile fil;
    fil.readEdfFile(inFilePath);
    fil.writeEdfFile(outFilePath);
}

void scalingFactorDir(const QString & inDirPath,
					  QString outDirPath)
{
    QStringList leest = QDir(inDirPath).entryList(def::edfFilters);
	if(outDirPath.isEmpty())
	{
		QDir tmp(inDirPath);
		tmp.cdUp();
		tmp.mkdir(myLib::getFileName(inDirPath, false) + "_scal");
		tmp.cd(myLib::getFileName(inDirPath, false) + "_scal");
		outDirPath = tmp.absolutePath();
	}
    for(const QString & str : leest)
    {
        scalingFactorFile(inDirPath + slash + str,
                          outDirPath + slash + str);
    }
}

void toLatinFileOrFolder(const QString & fileOrFolderPath)
{
	QString dirName = getDirPathLib(fileOrFolderPath);
	QString fileName = getFileName(fileOrFolderPath);
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
	QDir tmp(fileOrFolderPath);
	tmp.rename(fileOrFolderPath,  dirName + slash + newFileName);
//	QFile::rename(fileOrFolderPath, dirName + slash + newFileName);
}

void toLatinDir(const QString & dirPath, const QStringList & filters)
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
		toLatinFileOrFolder(dirPath + slash + str);
    }
}

void deleteSpacesDir(const QString & dirPath, const QStringList & filters)
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

void deleteSpacesFolders(const QString & dirPath)
{
    QDir tmp(dirPath);
    QStringList lst = tmp.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QString newName;
    for(const QString & dirName : lst)
    {
        newName = dirName;
        newName.replace(' ', '_');
        newName.remove(R"(')");
        newName.replace("__", "_");
        tmp.rename(dirName,
                   newName);
    }
}

bool testChannelsOrderConsistency(const QString & dirPath)
{
    std::vector<QString> labelsBC;
    std::vector<QString> labels;
    edfFile fil;
	QStringList leest = QDir(dirPath).entryList(def::edfFilters);

	fil.readEdfFile(dirPath + slash + leest[0], true);
	for(const QString & lbl : fil.getLabels())
	{
		if( lbl.startsWith("EEG ") )
		{
			labelsBC.push_back(lbl);
        }
    }
	std::cout << "testChannelsOrderConsistency: main seq = " << leest[0] << std::endl;

    bool res = true;

    for(const QString & guy : leest)
    {
		fil.readEdfFile(dirPath + slash + guy, true);
		labels.clear();
		for(const QString & lbl : fil.getLabels())
		{
			if( lbl.startsWith("EEG ") )
			{
				labels.push_back(lbl);
			}
		}
        if(labels != labelsBC)
        {
			std::cout << "testChannelsOrderConsistency: other seq = " << guy << std::endl;
            res = false;
        }
    }
    cout << "testChannelsOrderConsistency:\n" << dirPath << "\t" << res << endl;
    return res;
}


void channelsOrderFile(const QString & inFilePath,
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

//	cout << inFilePath << endl;
//	cout << reorderChanList << endl;

    std::vector<int> ident(initFile.getNs());
    std::iota(std::begin(ident), std::end(ident), 0);

    if(reorderChanList != ident)
    {
        initFile.readEdfFile(inFilePath);
		initFile = initFile.reduceChannels(reorderChanList);
        initFile.writeEdfFile(outFilePath);
    }
    else
    {
        QFile::copy(inFilePath, outFilePath);
    }
}

void channelsOrderDir(const QString & inDirPath,
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
        channelsOrderFile(inDirPath + slash + vec[i],
                            outDirPath + slash + outName,
                            standard);
//		break;
    }
}

void holesFile(const QString & inFilePath,
			   QString outFilePath)
{
    if(outFilePath.isEmpty())
    {	
        outFilePath = inFilePath;
        outFilePath.replace(".edf", "_repInf.edf", Qt::CaseInsensitive);
    }

    static const double thr = 0.005;

    edfFile fil;
    fil.readEdfFile(inFilePath);
    auto dataList = fil.getDataAsList();
    for(auto it = std::begin(dataList); it != std::end(dataList); ++it)
    {
        std::valarray<double> & col = *it;
        int count = 0;
        for(uint i = 0; i < col.size(); ++i)
        {
            if(col[i] > (1. - thr) * fil.getDigMax()[i] + thr * fil.getDigMin()[i] ||
               col[i] < (1. - thr) * fil.getDigMin()[i] + thr * fil.getDigMax()[i])
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
    fil.setDataFromList(dataList);
    fil.writeOtherData(fil.getData(), outFilePath);
}

void holesDir(const QString & inDirPath,
			  const QString & outDirPath)
{
    const auto leest = QDir(inDirPath).entryList(def::edfFilters, QDir::Files);
    const auto vec = leest.toVector();

    for(int i = 0; i < vec.size(); ++i)
	{
		holesFile(inDirPath + slash + vec[i],
				  outDirPath + slash + vec[i]);
    }
}

void toLowerFileOrFolder(const QString & fileOrFolderPath)
{
	QString dirName = myLib::getDirPathLib(fileOrFolderPath);
	QString fileName = myLib::getFileName(fileOrFolderPath);
	QString newFileName = fileName.toLower();

	newFileName[0] = newFileName[0].toUpper();
	for(int i = 1; i < newFileName.length(); ++i)
	{
		/// care about fileMarkers - not replace the last "_**" in edf-files
		if(newFileName[i - 1] == '_' &&
		   !(newFileName.contains(".edf", Qt::CaseInsensitive) &&
			 i - 1 == newFileName.lastIndexOf("_"))
		   )
		{
			newFileName[i] = newFileName[i].toUpper();
		}
	}
	QDir tmp(fileOrFolderPath);
	tmp.rename(fileOrFolderPath, dirName + slash + newFileName);
}

void toLowerDir(const QString & dirPath, const QStringList & filters)
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
		toLowerFileOrFolder(dirPath + slash + str);
	}
}

void fullRepairDir(const QString & dirPath, const QStringList & filters)
{
	repair::deleteSpacesDir(dirPath, filters);
	repair::toLatinDir(dirPath, filters);
	repair::toLowerDir(dirPath, filters);
//	repair::physMinMaxDir(dirPath);
	repair::scalingFactorDir(dirPath, dirPath);
//	repair::holesDir(dirPath, dirPath);
}

} /// end namespace repair
