#include <myLib/general.h>

#include <fstream>
#include <map>

#include <myLib/output.h>

#include <QDir>

using namespace myOut;

const std::map<int, QString> kyrToLatinData{
	{1072, "a"},
	{1073, "b"},
	{1074, "v"},
	{1075, "g"},
	{1076, "d"},
	{1077, "e"},
	{1078, "zh"},
	{1079, "z"},
	{1080, "i"},
	{1081, "j"},
	{1082, "k"},
	{1083, "l"},
	{1084, "m"},
	{1085, "n"},
	{1086, "o"},
	{1087, "p"},
	{1088, "r"},
	{1089, "s"},
	{1090, "t"},
	{1091, "u"},
	{1092, "f"},
	{1093, "kh"},
	{1094, "tz"},
	{1095, "ch"},
	{1096, "sh"},
	{1097, "sch"},
	{1098, ""},
	{1099, "yi"},
	{1100, ""},
	{1101, "e"},
	{1102, "yu"},
	{1103, "ya"},
	{1040, "A"},
	{1041, "B"},
	{1042, "V"},
	{1043, "G"},
	{1044, "D"},
	{1045, "E"},
	{1046, "ZH"},
	{1047, "Z"},
	{1048, "I"},
	{1049, "J"},
	{1050, "K"},
	{1051, "L"},
	{1052, "M"},
	{1053, "N"},
	{1054, "O"},
	{1055, "P"},
	{1056, "R"},
	{1057, "S"},
	{1058, "T"},
	{1059, "U"},
	{1060, "F"},
	{1061, "KH"},
	{1062, "TZ"},
	{1063, "CH"},
	{1064, "SH"},
	{1065, "SCH"},
	{1066, ""},
	{1067, "YI"},
	{1068, ""},
	{1069, "E"},
	{1070, "YU"},
	{1071, "YA"},
	{1105, "yo"},
	{1025, "YO"}
};

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

QString setFileName(const QString & initNameOrPath) /// append _num before the dot
{
    QString beforeDot = initNameOrPath;
    beforeDot.resize(beforeDot.lastIndexOf('.'));

    QString afterDot = initNameOrPath; /// with the dot
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

QString getDirPathLib(const QString & filePath)
{
	return filePath.left(filePath.lastIndexOf('/'));
}

QString getExtension(const QString & filePath)
{
    QString helpString = (filePath);
    if(helpString.contains('.'))
    {
        helpString = helpString.right(helpString.length() - helpString.lastIndexOf('.') - 1);
        return helpString;
	}
	return {};
}

QString kyrToLatin(const QString & in)
{
	QString res{};
	res.reserve(in.size());
	for(const QChar & ch : in)
	{
		int num = ch.unicode();
		if(kyrToLatinData.count(num) > 0)
		{
			res += kyrToLatinData.at(num);
		}
		else
		{
			res += ch;
		}
	}
	return res;
}

QString getFileName(const QString & filePath, bool withExtension)
{
	QString hlp{filePath};
	hlp = hlp.right(hlp.length() - hlp.lastIndexOf('/') - 1);
    if(!withExtension)
    {
		hlp = hlp.left(hlp.lastIndexOf("."));
    }
	return hlp;
}

QString getExpNameLib(const QString & filePath, bool shortened) /// getFileName
{
	QString hlp{myLib::getFileName(filePath, false)};
	if(shortened)
	{
		hlp = hlp.left(hlp.indexOf('_')); ///
	}
	return hlp;
}

QString getPicPath(const QString & dataPath,
				   const QString & ExpNameDir)
{
	QString fileName = myLib::getFileName(dataPath);
	fileName.replace(".edf", "_", Qt::CaseInsensitive);

	QString addName{};
	if(dataPath.contains("Reals"))		{ addName = "Signals"; }
	else if(dataPath.contains("winds")) { addName = "Signals/winds"; }
	else if(dataPath.contains("cut"))	{ addName = "SignalsCut"; }

	return ExpNameDir + "/" + addName + "/" + fileName + ".jpg";
}

/// add std::optional
int getTypeOfFileName(const QString & fileName, const QStringList & markers)
{
    QStringList leest;
    int res = 0;
	for(const QString & marker : markers)
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
int indexOfVal(const Container & cont, Typ val)
{
	int i = 0;
	for(auto it = std::begin(cont); it != std::end(cont); ++it, ++i)
	{
		if(*it == val) { return i; }
	}
	return -1;
}
template int indexOfVal(const std::valarray<double> & cont, double val);
template int indexOfVal(const std::vector<double> & cont, double val);
template int indexOfVal(const std::vector<int> & cont, int val);
template int indexOfVal(const std::vector<QString> & cont, QString val);

template <typename Container, typename Typ>
bool contains(const Container & cont, Typ val)
{
	return std::find(std::begin(cont), std::end(cont), val) != std::end(cont);
}
template bool contains(const std::vector<int> & cont, int val);
template bool contains(const std::valarray<int> & cont, int val);
template bool contains(const std::valarray<int> & cont, uint val);
template bool contains(const std::vector<QString> & cont, QString val);

template <typename Container, typename Typ>
std::pair<bool, Typ> contains(const Container & cont, const std::vector<Typ> & val)
{
	for(Typ in : cont)
	{
		for(Typ v : val)
		{
			if(in == v) { return std::make_pair(true, v); }
		}
	}
	return std::make_pair(false, 0);
}
template std::pair<bool, double> contains(const std::valarray<double> & cont,
										  const std::vector<double> & val);
template std::pair<bool, int> contains(const std::valarray<double> & cont,
										  const std::vector<int> & val);


template <class InputIterator, class Typ>
InputIterator find_back(InputIterator last, InputIterator first, const Typ & val)
{
	while (last != first)
	{
		if (*last == val) { return last; }
		--last;
	}
	return first;
}

template <class InputIterator, class Pred>
InputIterator find_back_if(InputIterator last, InputIterator first, Pred pred)
{
	while (last != first)
	{
		if (pred(*last)) { return last; }
		--last;
	}
	return first;
}

std::vector<int> splitStringIntoVec(const QString & in)
{
	std::vector<int> res{};
	QStringList lst = in.split(QRegExp("[,;\\s]"), QString::SkipEmptyParts);
	for(const QString & p : lst)
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
		tempInt = (value / static_cast<int>(std::pow(256, i))) % 256;
		writeByte(fil, tempInt);
	}
}

/// lentgh till \0\0
int edfPlusLen(const QString & s)
{
	int res = 0;
	for(int i = 0; i < s.size(); ++i)
	{
		if(s[i] != '\0')
		{
			++res;
		}
		else
		{
			if(s[i + 1] != '\0')
			{
				++res;
			}
			else
			{
				++res;
				return res;
			}
		}
	}
	return -1;
}


bool areEqualFiles(const QString & path1, const QString & path2)
{
	QTime myTime;
	myTime.start();

	using byte = qint8;
	FILE * fil1 = fopen(path1.toStdString().c_str(), "rb");
	FILE * fil2 = fopen(path2.toStdString().c_str(), "rb");
	if(fil1 == nullptr || fil2 == nullptr)
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
		sum += std::pow(dat1 - dat2, 2);
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
	QFile inFile(filePath);
	inFile.open(QIODevice::ReadOnly);
	char tmp;
	int res{0};
	while(inFile.getChar(&tmp))
	{
		if(tmp == inChar) { ++res; }
	}
	inFile.close();
	return res;

#if 0
	FILE * inStr;
	inStr = fopen(filePath.toStdString().c_str(), "rb");
	char tmpChar;
	int res = 0;
	while(!feof(inStr))
	{
		fscanf(inStr, "%c", &tmpChar);
		if(!feof(inStr) && tmpChar == inChar) { ++res; }
	}
	return res;
#endif
}

void replaceSymbolsInFile(const QString & filePath,
						  const QString & inChar,
						  const QString & outChar,
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


void cleanDir(const QString & dirPath, const QString & nameFilter, bool ext)
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
	/// http:/// soundfile.sapp.org/doc/WaveFormat/


	FILE * outFile;
	outFile = fopen(outPath.toStdString().c_str(), "wb");
	if(outFile == nullptr)
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

	/// RIFF
	writeByte(outFile, 0x52);
	writeByte(outFile, 0x49);
	writeByte(outFile, 0x46);
	writeByte(outFile, 0x46);

	/// chunksize = 44 + ns * numSamples * bytesPerSample
	writeBytes(outFile, chunkSize, 4);

	/// WAVE
	writeByte(outFile, 0x57);
	writeByte(outFile, 0x41);
	writeByte(outFile, 0x56);
	writeByte(outFile, 0x45);

	/// fmt
	writeByte(outFile, 0x66);
	writeByte(outFile, 0x6d);
	writeByte(outFile, 0x74);
	writeByte(outFile, 0x20);

	/// Subchunk1Size = 16 for pcm
	writeBytes(outFile, 16, 4);

	/// audioFormat = 1 PCM
	writeBytes(outFile, 1, 2);

	/// numChannels
	writeBytes(outFile, numChannels, 2);

	/// sampleRate
	writeBytes(outFile, sampleFreq, 4);

	/// BYTErate
	writeBytes(outFile, byteRate, 4);

	/// block align
	writeBytes(outFile, blockAlign, 2);

	/// bitsPerSample
	writeBytes(outFile, bitsPerSample, 2);

	/// data
	writeByte(outFile, 0x64);
	writeByte(outFile, 0x61);
	writeByte(outFile, 0x74);
	writeByte(outFile, 0x61);

	/// subchunk2size
	writeBytes(outFile, subchunk2size, 4);

	/// the data itself
	int currVal;
	for(int i = 0; i < numSamples; ++i)
	{
		for(int j = 0; j < numChannels; ++j)
		{
			currVal = inData[i] * std::pow(256, bitsPerSample / 8 / numChannels) / maxAmpl;
			writeBytes(outFile, currVal, bitsPerSample / 8 / numChannels);
		}
	}
	fclose(outFile);
}


template uint indexOfMax(const std::vector<int> & cont);
template uint indexOfMax(const std::vector<double> & cont);
template uint indexOfMax(const std::valarray<double> & cont);
template uint indexOfMax(const std::list<double> & cont);

template uint indexOfMin(const std::vector<int> & cont);
template uint indexOfMin(const std::vector<double> & cont);
template uint indexOfMin(const std::valarray<double> & cont);
template uint indexOfMin(const std::list<double> & cont);

} /// end of namespace myLib
