#include <other/edffile.h>

#include <other/consts.h>
#include <myLib/valar.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>

using namespace myOut;

namespace repair
{

void physMinMaxDir(const QString & dirPath, const QStringList & filters)
{
	for(const QString & str : QDir(dirPath).entryList(filters))
    {
        edfFile feel;
		feel.readEdfFile(dirPath + "/" + str);
		feel.repairPhysEqual().writeEdfFile(dirPath + "/" + str);
    }
}

void physMinMaxCheck(const QString & dirPath)
{
    QDir(dirPath).mkdir("bad");
	for(const QString & str : QDir(dirPath).entryList(def::edfFilters))
    {
        edfFile feel;
		feel.readEdfFile(dirPath + "/" + str);
        for(int i = 0; i < feel.getNs(); ++i)
        {
            if(feel.getPhysMin()[i] == feel.getPhysMax()[i])
            {
				QFile::rename(dirPath + "/" + str,
							  dirPath + "/bad/" + str);
            }
        }
    }
}

void deleteNewContents(const QString & inPath)
{
	auto lst = QDir(inPath).entryList(def::edfFilters);
	for(const QString & in : lst)
	{
		QString newName = in;
		newName.remove("_new");

		QFile::remove(inPath + "/" + newName);
		QFile::rename(inPath + "/" + in,
					  inPath + "/" + newName);
	}
}

void renameContents(const QString & dirPath, const std::vector<std::pair<QString, QString>> & temp)
{
	QStringList leest = QDir(dirPath).entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	for(const QString & str : leest)
	{
		QString strNew = str;
		for(const auto & in : temp)
		{
			strNew.replace(in.first, in.second, Qt::CaseInsensitive);
		}
		QDir::root().rename(dirPath + "/" + str,
							dirPath + "/" + strNew);
	}
}

void toLatinItem(const QString & inPath)
{
	QDir().rename(inPath,
				  myLib::getDirPathLib(inPath)
				  + "/" + myLib::kyrToLatin( myLib::getFileName(inPath) ));
}

void toLatinContents(const QString & dirPath, const QStringList & filters)
{
	QStringList leest;
	if(!filters.isEmpty())
	{
		leest = QDir(dirPath).entryList(filters);
	}
	else
	{
		leest = QDir(dirPath).entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	}

    for(const QString & str : leest)
    {
		toLatinItem(dirPath + "/" + str);
    }
}

void deleteSpacesItem(const QString & inPath)
{
	QString path = myLib::getDirPathLib(inPath);
	QString newName = myLib::getFileName(inPath);
	newName.replace(' ', '_');
	newName.remove(R"(')");
	newName.replace(QRegExp("[_]{2,}"), "_");
	QDir().rename(inPath, path + "/" + newName);
}

void deleteSpacesContents(const QString & dirPath, const QStringList & filters)
{
	QDir tmp(dirPath);
    QStringList lst;
    if(filters.isEmpty())
    {
		lst = tmp.entryList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    }
    else
    {
		lst = tmp.entryList(filters, QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
	}
    for(const QString & fileName : lst)
	{
		deleteSpacesItem(dirPath + "/" + fileName);
    }
}

void deleteSpacesFoldersOnly(const QString & dirPath)
{
    QDir tmp(dirPath);
	QStringList lst = tmp.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QString newName;
    for(const QString & dirName : lst)
    {
		newName = dirName;
		newName.replace(' ', '_');
		newName.remove(R"(')");
		newName.replace(QRegExp(R"(_{1,})"), "_");
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

	fil.readEdfFile(dirPath + "/" + leest[0], true);
	for(QString lbl : fil.getLabels())
	{
		if( lbl.startsWith("EEG ") )
		{
			lbl.remove("EEG ");
			lbl.truncate(lbl.indexOf('-'));
			if(lbl.contains("Pg") || lbl.contains("Cb")) { continue; }

			labelsBC.push_back(lbl);
        }
    }
	std::cout << "testChannelsOrderConsistency: main seq = " << leest[0] << std::endl;

    bool res = true;

    for(const QString & guy : leest)
    {
		fil.readEdfFile(dirPath + "/" + guy, true);
		labels.clear();
		for(QString lbl : fil.getLabels())
		{
			if( lbl.startsWith("EEG ") )
			{
				lbl.remove("EEG ");
				lbl.truncate(lbl.indexOf('-'));
				if(lbl.contains("Pg") || lbl.contains("Cb")) { continue; }

				labels.push_back(lbl);
			}
		}
        if(labels != labelsBC)
        {
			std::cout << "testChannelsOrderConsistency: other seq = " << guy << std::endl;
            res = false;
        }
    }
	std::cout << "testChannelsOrderConsistency:\n" << dirPath << "\t" << res << std::endl;
    return res;
}


edfFile channelsOrderFile(const QString & inFilePath,
						 const std::vector<QString> & standard)
{
	edfFile initFile(inFilePath, true);

	std::vector<int> reorderChanList{};
	reorderChanList.reserve(initFile.getNs());

	std::set<int> leftChannels;
	for(int i = 0; i < initFile.getNs(); ++i) { leftChannels.emplace(i); }

    for(const auto & i : standard) /// only for 31 channels
    {
        for(int j = 0; j < initFile.getNs(); ++j)
        {
			if(initFile.getLabels(j).contains(i))
            {
                reorderChanList.push_back(j);
				leftChannels.erase(j);
                break;
            }
        }
	}
	for(int j : leftChannels) { reorderChanList.push_back(j); }

	std::vector<int> ident = smLib::range<std::vector<int>>(0, initFile.getNs());

    if(reorderChanList != ident)
    {
        initFile.readEdfFile(inFilePath);
		initFile = initFile.reduceChannels(reorderChanList);
		return initFile;
	}
	return {};
}

void channelsOrderDir(const QString & inDirPath,
                    const QString & outDirPath,
                    const std::vector<QString> & standard)
{
    const auto leest = QDir(inDirPath).entryList(def::edfFilters, QDir::Files);
    const auto vec = leest.toVector();

/// #pragma omp parallel
/// #pragma omp for nowait schedule(dynamic,3)
	for(const QString & outName : vec)
	{
		channelsOrderFile(inDirPath + "/" + outName,
						  standard)
				.writeEdfFile(outDirPath + "/" + outName);
    }
}

void holesFile(const QString & inFilePath,
			   int numChan,
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
		for(int i = 0; i < numChan; ++i)
		{
			auto highLim = (1. - thr) * fil.getPhysMax()[i] + thr * fil.getPhysMin()[i];
			if(std::abs(col[i]) > highLim)
            {
                ++count;
            }
        }
		if(count == numChan)
		{
            it = dataList.erase(it);
			--it;
        }
    }

	int a = fil.getDataLen() - dataList.size();
	if(a != 0)
	{
		std::cout << myLib::getFileName(inFilePath, false)
				  << "\t" << a << std::endl;

	}
    fil.setDataFromList(dataList);
    fil.writeOtherData(fil.getData(), outFilePath);
}

void holesDir(const QString & inDirPath,
			  int numChan,
			  const QString & outDirPath)
{
	const auto leest = QDir(inDirPath).entryList(def::edfFilters, QDir::Files);

	for(const auto & fileName : leest)
	{
		holesFile(inDirPath + "/" + fileName,
				  numChan,
				  outDirPath + "/" + fileName);
    }
}

void toLowerItem(const QString & inPath)
{
	QString dirName = myLib::getDirPathLib(inPath);
	QString fileName = myLib::getFileName(inPath);
	QString newFileName = fileName.toLower();
	newFileName[0] = newFileName[0].toUpper();

	QDir().rename(inPath, dirName + "/" + newFileName);
}

void toLowerContents(const QString & dirPath, const QStringList & filters)
{
	QStringList leest;
	if(!filters.isEmpty())
	{
		leest = QDir(dirPath).entryList(filters);
	}
	else
	{
		leest = QDir(dirPath).entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	}

	for(const QString & str : leest)
	{
		toLowerItem(dirPath + "/" + str);
	}
}

void testArtifacts(const QString & dirPath, const QStringList & filters)
{
	QStringList leest;
	if(!filters.isEmpty())
	{
		leest = QDir(dirPath).entryList(filters);
	}
	else
	{
		leest = QDir(dirPath).entryList();
	}


	const double amplThreshold = 150.;
	const int fftLen = 512;
	const myLib::windowName window = myLib::windowName::Hamming;

	for(const QString & str : leest)
	{

		edfFile fil;
		fil.readEdfFile(dirPath + "/" + str);
		const double srate = fil.getFreq();

		if(fil.getData().maxAbsVal() > amplThreshold)
		{
			std::cout << fil.getFileNam() << ": amplitude " << std::endl;
			continue;
		}

		using specType = std::valarray<std::complex<double>>;
		const int windStep = fftLen * 0.5;
		const std::valarray<double> wnd = myLib::fftWindow(fftLen, window);

		for(int i = 0; i < 19; ++i) /// numChans magic const
		{
			const int siz = (fil.getData(i).size() - fftLen) / windStep;
			int num = 0;
			std::valarray<double> thetaPart(siz);
			std::valarray<double> thetaAbs(siz);
			std::valarray<double> betaPart(siz);
			std::valarray<double> betaAbs(siz);

			for(uint windStart = 0;
				windStart < fil.getData(i).size() - fftLen;
				windStart += windStep, ++num)
			{
				std::valarray<double> part1 = smLib::contSubsec(fil.getData(i),
																windStart,
																windStart + fftLen) * wnd;
				const specType spec1 = myLib::spectreRtoC2(part1, fftLen, srate);
				specType tmp = spec1 * spec1.apply(std::conj);

				/// magic consts
				double theta = std::abs(std::accumulate(
											std::begin(tmp) + smLib::fftLimit(4., srate, fftLen),
											std::begin(tmp) + smLib::fftLimit(7.5, srate, fftLen),
											std::complex<double>{}));
				double beta = std::abs(std::accumulate(
											std::begin(tmp) + smLib::fftLimit(15., srate, fftLen),
											std::begin(tmp) + smLib::fftLimit(20., srate, fftLen),
											std::complex<double>{}));
				double whole = std::abs(std::accumulate(
											std::begin(tmp) + smLib::fftLimit(5., srate, fftLen),
											std::begin(tmp) + smLib::fftLimit(20., srate, fftLen),
											std::complex<double>{}));
				thetaPart[num] = theta / whole;
				thetaAbs[num] = theta;
				betaPart[num] = beta / whole;
				betaAbs[num] = beta;
			}

			/// magic consts
			if(thetaPart.max() > 0.5)
			{
				std::cout << fil.getFileNam() << ": thetaPart " << std::endl;
				continue;
			}
			if(thetaAbs.max() > 300)
			{
				std::cout << fil.getFileNam() << ": thetaAbs " << std::endl;
				continue;
			}
			if(betaPart.max() > 0.5)
			{
				std::cout << fil.getFileNam() << ": betaPart " << std::endl;
				continue;
			}
			if(betaAbs.max() > 300)
			{
				std::cout << fil.getFileNam() << ": betaAbs " << std::endl;
				continue;
			}

		}
	}

}

void fullRepairDir(const QString & dirPath, const QStringList & filters)
{
	repair::deleteNewContents(dirPath);
	repair::deleteSpacesContents(dirPath, filters);
	repair::toLatinContents(dirPath, filters);
	repair::toLowerContents(dirPath, filters);
}

} /// end of namespace repair
