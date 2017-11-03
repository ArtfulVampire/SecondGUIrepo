#include <myLib/general.h>

#include <myLib/output.h>

using namespace myOut;

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

    QString afterDot = initNameOrPath; // with the dot
    afterDot = afterDot.right(afterDot.length() - afterDot.lastIndexOf('.'));

    QString helpString;
    helpString = beforeDot + afterDot;
    int counter = 0;
    while (QFile::exists(helpString))
    {
        helpString = beforeDot + "_" + nm(counter++) + afterDot;
    }
    return helpString;
}

QString getExpNameLib(const QString & filePath, bool shortened) // getFileName
{
    QString hlp;
    hlp = (filePath);
	hlp = hlp.right(hlp.length() - hlp.lastIndexOf('/') - 1); // ExpName.edf
    hlp = hlp.left(hlp.lastIndexOf('.')); // ExpName
	if(shortened)
	{
		hlp = hlp.left(hlp.indexOf('_')); // 
	}
    return hlp;
}

QString getDirPathLib(const QString & filePath)
{
	return filePath.left(filePath.lastIndexOf('/'));
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
	helpString = helpString.right(helpString.length() - helpString.lastIndexOf('/') - 1);
    if(!withExtension)
    {
        helpString = helpString.left(helpString.lastIndexOf("."));
    }
    return helpString;
}

QString getPicPath(const QString & dataPath,
				   const QString & ExpNameDir)
{
	QString fileName = myLib::getFileName(dataPath);
    fileName.replace('.', '_');

	QString helpString = (ExpNameDir + "/'");

    if(dataPath.contains("Reals"))
    {
		helpString += "Signals";
	}
    else if(dataPath.contains("winds"))
	{
		helpString += "Signals/winds";
    }
    else if(dataPath.contains("cut"))
    {
        helpString += "SignalsCut";
    }
	helpString += "/" + fileName + ".jpg";
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

template <typename Container>
uint indexOfMax(const Container & cont)
{
	uint res = 0;
	uint ans = 0;
	auto val = *(std::begin(cont));

	for(auto it = std::begin(cont);
		it != std::end(cont);
		++it, ++res)
	{
		if(*it > val)
		{
			ans = res;
			val = *it;
		}
	}
	return ans;
}

template <typename Container>
uint indexOfMin(const Container & cont)
{
	uint res = 0;
	uint ans = 0;
	auto val = *(std::begin(cont));

	for(auto it = std::begin(cont);
		it != std::end(cont);
		++it, ++res)
	{
		if(*it < val)
		{
			ans = res;
			val = *it;
		}
	}
	return ans;
}

template <typename Container, typename Typ>
uint indexOfVal(const Container & cont, Typ val)
{
	int i = 0;
	for(auto it = std::begin(cont); it != std::end(cont); ++it, ++i)
	{
		if(*it == val) break;
	}
	return i;
}
template uint indexOfVal(const std::valarray<double> & cont, double val);
template uint indexOfVal(const std::vector<double> & cont, double val);
template uint indexOfVal(const std::vector<int> & cont, int val);

template <typename Container, typename Typ>
bool contains(const Container & cont, Typ val)
{
//	return indexOfVal(cont, val) != cont.size();
	/// stl
	return std::find(std::begin(cont), std::end(cont), val) != std::end(cont);
}
template bool contains(const std::vector<int> & cont, int val);

template <typename Container, typename Typ>
std::pair<bool, Typ> contains(const Container & cont, const std::initializer_list<Typ> & val)
{
	for(Typ in : cont)
	{
		for(Typ v : val)
		{
			if(in == v) return std::make_pair(true, v);
		}
	}
	return std::make_pair(false, 0);
}
template std::pair<bool, double> contains(const std::valarray<double> & cont,
										  const std::initializer_list<double> & val);



template <class InputIterator, class Typ>
InputIterator find_back(InputIterator last, InputIterator first, const Typ & val)
{
	while (last!=first) {
	   if (*last==val) return last;
	   --last;
	 }
	 return first;
}

template <class InputIterator, class Pred>
InputIterator find_back_if(InputIterator last, InputIterator first, Pred pred)
{
	while (last!=first) {
	   if (pred(*last)) return last;
	   --last;
	 }
	 return first;
}

std::vector<int> splitStringIntoVec(const QString & in)
{
	std::vector<int> res{};
	QStringList lst = in.split(QRegExp("[,;\\s]"), QString::SkipEmptyParts);
	for(QString p : lst)
	{
		res.push_back(p.toInt());
	}
	return res;
}

std::string funcName(std::string in)
{
	in.resize(in.find('('));
	for(char a : {' ', '='})
	{
		auto b = in.rfind(a);
		if(b != std::string::npos)
		{
			in = in.substr(b + 1);
		}
	}
	return in;
}

void writeByte(FILE * fil, int num)
{
	char tempChar = num;
	fwrite(&tempChar, sizeof(char), 1, fil);
}

void writeBytes(FILE * fil, int value, int numBytes)
{
	int tempInt;
	for(int i = 0; i < numBytes; ++i)
	{
		tempInt = (value / int(pow(256, i)))%256;
		writeByte(fil, tempInt);
	}
}


int len(const QString & s) // lentgh till double \0-byte for EDF+annotations
{
	int l = 0;
	for(int i = 0; i < 100500; ++i)
	{
		if(s[i]!='\0') ++l;
		else
		{
			if(s[i+1]!='\0') ++l;
			if(s[i+1]=='\0')
			{
				++l;
				return l;
			}
		}
	}
	return -1;
}


bool areEqualFiles(QString path1, QString path2)
{
	QTime myTime;
	myTime.start();

	typedef qint8 byte;
	FILE * fil1 = fopen(path1, "rb");
	FILE * fil2 = fopen(path2, "rb");
	if(fil1 == NULL || fil2 == NULL)
	{
		std::cout << "areEqualFiles: some of the files == NULL" << std::endl;
	}
	byte byt1, byt2;
	int pos = 0;
	while(!feof(fil1) && !feof(fil2))
	{
		fread(&byt1, sizeof(byte), 1, fil1);
		fread(&byt2, sizeof(byte), 1, fil2);
		if(byt1 != byt2)
		{
			fclose(fil1);
			fclose(fil2);
			std::cout << "equalFiles(false): time = " << myTime.elapsed() / 1000. << " sec"
				 << "\t" << "pos(bytes) = " << pos << std::endl;
			return false;
		}
		++pos;
	}
	fclose(fil1);
	fclose(fil2);
	std::cout << "equalFiles(true): time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
	return true;
}

double areSimilarFiles(const QString & path1,
					   const QString & path2)
{
	std::ifstream fil1(path1.toStdString());
	std::ifstream fil2(path2.toStdString());
	if(!fil1.good() || !fil2.good())
	{
		std::cout << "areSimilarFiles: some of the files is not good" << std::endl;
		return -1.;
	}
	double dat1, dat2;
	double sum = 0.;
	int count = 0;
	while(!fil1.eof() || !fil2.eof())
	{
		fil1 >> dat1;
		fil2 >> dat2;
		sum += pow(dat1 - dat2, 2);
		++count;
	}
	fil1.close();
	fil2.close();
	sum /= count;
	std::cout << "areSimilarFiles: items = " << count << " square error = " << sum << std::endl;
	return sum;
}

int countSymbolsInFile(const QString & filePath, char inChar)
{
	FILE * inStr;
	inStr = fopen(filePath, "rb");
	char tmpChar;
	int res = 0;
	while(!feof(inStr))
	{
		fscanf(inStr, "%c", &tmpChar);
		if(!feof(inStr) && tmpChar == inChar) ++res;
	}
	return res;
}

void replaceSymbolsInFile(const QString & filePath,
						  QString inChar,
						  QString outChar,
						  QString outPath)
{
	QFile fil(filePath);
	fil.open(QIODevice::ReadOnly);
	auto dat = fil.readAll();
	dat.replace(inChar, outChar.toStdString().c_str());
	fil.close();

	if(outPath.isEmpty()) { outPath = filePath; }
	QFile outFil(outPath);
	outFil.open(QIODevice::WriteOnly);
	outFil.write(dat);
	outFil.close();
}


void cleanDir(QString dirPath, QString nameFilter, bool ext)
{
	QDir tmpDir(dirPath);

	QStringList lst;

	if(nameFilter.isEmpty())
	{
		lst = tmpDir.entryList(QDir::Files);
	}
	else
	{
		QStringList filter;
		QString hlp;
		filter.clear();
		if(ext)
		{
			hlp = "*." + nameFilter;
		}
		else
		{
			hlp = "*" + nameFilter + "*";
		}
		filter << hlp;
		lst = tmpDir.entryList(filter, QDir::Files);
	}

	for(int h = 0; h < lst.length(); ++h)
	{
		QFile::remove(tmpDir.absolutePath() + "/" + lst[h]);
	}

}


void writeWavFile(const std::vector<double> & inData, const QString & outPath)
{
	// http:// soundfile.sapp.org/doc/WaveFormat/


	FILE * outFile;
	outFile = fopen(outPath, "wb");
	if(outFile == NULL)
	{
		std::cout << "cant open file to write" << std::endl;
		return;
	}

	const int numChannels = 1;
	const int bitsPerSample = 16;
	const int sampleFreq = 44100;
	const int numSamples = inData.size();
	const double maxAmpl = *(std::max_element(inData.begin(), inData.end())) + 1e-3;
	const int subchunk2size = numSamples * numChannels * bitsPerSample / 8;
	const int chunkSize = 4 + (8 + 16) + (8 + subchunk2size);
	const int byteRate = sampleFreq * numChannels * bitsPerSample / 8;
	const int blockAlign = numChannels * bitsPerSample / 8;

	// RIFF
	writeByte(outFile, 0x52);
	writeByte(outFile, 0x49);
	writeByte(outFile, 0x46);
	writeByte(outFile, 0x46);

	// chunksize = 44 + ns * numSamples * bytesPerSample
	writeBytes(outFile, chunkSize, 4);

	// WAVE
	writeByte(outFile, 0x57);
	writeByte(outFile, 0x41);
	writeByte(outFile, 0x56);
	writeByte(outFile, 0x45);

	// fmt
	writeByte(outFile, 0x66);
	writeByte(outFile, 0x6d);
	writeByte(outFile, 0x74);
	writeByte(outFile, 0x20);

	// Subchunk1Size = 16 for pcm
	writeBytes(outFile, 16, 4);

	// audioFormat = 1 PCM
	writeBytes(outFile, 1, 2);

	// numChannels
	writeBytes(outFile, numChannels, 2);

	// sampleRate
	writeBytes(outFile, sampleFreq, 4);

	// BYTErate
	writeBytes(outFile, byteRate, 4);

	// block align
	writeBytes(outFile, blockAlign, 2);

	// bitsPerSample
	writeBytes(outFile, bitsPerSample, 2);

	// data
	writeByte(outFile, 0x64);
	writeByte(outFile, 0x61);
	writeByte(outFile, 0x74);
	writeByte(outFile, 0x61);

	// subchunk2size
	writeBytes(outFile, subchunk2size, 4);

	// the data itself
	int currVal;
	for(int i = 0; i < numSamples; ++i)
	{
		for(int j = 0; j < numChannels; ++j)
		{
			currVal = int(inData[i] * pow(256, bitsPerSample/8/numChannels) / maxAmpl);
			writeBytes(outFile, currVal, int(bitsPerSample/8/numChannels));
		}
	}
	fclose(outFile);
}



template uint indexOfVal(const std::vector<QString> & cont, const QString & val);

template uint indexOfMax(const std::vector<int> & cont);
template uint indexOfMax(const std::vector<double> & cont);
template uint indexOfMax(const std::valarray<double> & cont);
template uint indexOfMax(const std::list<double> & cont);

template uint indexOfMin(const std::vector<int> & cont);
template uint indexOfMin(const std::vector<double> & cont);
template uint indexOfMin(const std::valarray<double> & cont);
template uint indexOfMin(const std::list<double> & cont);


} // namespace myLib



