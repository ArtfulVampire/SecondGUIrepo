#include "library.h"



namespace myLib
{

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


void writeWavFile(const std::vector<double> & inData, const QString & outPath)
{
    // http://soundfile.sapp.org/doc/WaveFormat/


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

    //RIFF
    writeByte(outFile, 0x52);
    writeByte(outFile, 0x49);
    writeByte(outFile, 0x46);
    writeByte(outFile, 0x46);

    //chunksize = 44 + ns * numSamples * bytesPerSample
    writeBytes(outFile, chunkSize, 4);

    //WAVE
    writeByte(outFile, 0x57);
    writeByte(outFile, 0x41);
    writeByte(outFile, 0x56);
    writeByte(outFile, 0x45);

    // fmt
    writeByte(outFile, 0x66);
    writeByte(outFile, 0x6d);
    writeByte(outFile, 0x74);
    writeByte(outFile, 0x20);

    //Subchunk1Size = 16 for pcm
    writeBytes(outFile, 16, 4);

    //audioFormat = 1 PCM
    writeBytes(outFile, 1, 2);

    //numChannels
    writeBytes(outFile, numChannels, 2);

    //sampleRate
    writeBytes(outFile, sampleFreq, 4);

    //BYTErate
    writeBytes(outFile, byteRate, 4);

    //block align
    writeBytes(outFile, blockAlign, 2);

    //bitsPerSample
    writeBytes(outFile, bitsPerSample, 2);

    //data
    writeByte(outFile, 0x64);
    writeByte(outFile, 0x61);
    writeByte(outFile, 0x74);
    writeByte(outFile, 0x61);

    //subchunk2size
    writeBytes(outFile, subchunk2size, 4);

    //the data itself
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


int len(const QString & s) //lentgh till double \0-byte for EDF+annotations
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

char * strToChar(const QString & input)
{
    char * array = new char [input.length() + 1];
    memcpy(array, input.toStdString().c_str(), input.length());
    array[input.length()] = '\0';
    return array;
}

char * QStrToCharArr(const QString & input, const int &len)
{
    // fixes problem with labels length

    int leng = input.length();
    if(len != -1)
    {
        leng = len;
    }
    char * array = new char [leng + 1];
    memcpy(array, input.toStdString().c_str(), input.length());

    if(len != -1)
    {
        for(int i = input.length(); i < leng; ++i)
        {
            array[i] = ' ';
        }
    }
    array[leng] = '\0';
    return array;
}


FILE * fopen(QString filePath, const char *__modes)
{
    return std::fopen(filePath.toStdString().c_str(), __modes);
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
        QFile::remove(tmpDir.absolutePath() + slash + lst[h]);
    }

}



QString fitNumber(const double &input, int N) // append spaces
{
    QString h;
    h.setNum(input);
    h += QString(N, ' ');
    return h.left(N);
}
QString fitString(const QString & input, int N) // append spaces
{
    QString h(input);
    h += QString(N, ' ');
    return h.left(N);
}

QString rightNumber(const unsigned int input, int N) // prepend zeros
{
    QString h;
    h.setNum(input);
    h.prepend(QString(N, '0'));
    return h.right(N);
}






QString matiCountByteStr(const double & marker)
{
    QString result;
    std::vector<bool> byteMarker;
    byteMarker = matiCountByte(marker);

    for(int h = 15; h >= 0; --h)
    {
        result += QString::number(int(byteMarker[h]));
        if(h == 8) result += " ";
    }
    return result;

}
void matiPrintMarker(const double &marker, QString pre)
{
    std::vector<bool> byteMarker;
    byteMarker = matiCountByte(marker);

    if(!pre.isEmpty())
    {
		std::cout << pre.toStdString() << " = ";
    }
	std::cout << marker << "\t" << matiCountByteStr(marker) << std::endl;
}

std::vector<bool> matiCountByte(double const &  marker)
{
    std::vector<bool> byteMarker;
    for(int h = 0; h < 16; ++h)
    {
        byteMarker.push_back(matiCountBit(marker, h));
    }
    return byteMarker;
}

void matiFixMarker(double & marker)
{
    //throw 10000000 00000000 and 00000000 10000000 and 00000000 00000000
    if(marker == pow(2, 15) || marker == pow(2, 7) || marker == 0)
    {
        marker = 0;
        return;
    }

    std::vector<bool> byteMarker = matiCountByte(marker);
    bool boolBuf;

    if(!byteMarker[7]) //elder byte should start with 0 and younger - with 1
    {
        //swap bytes if wrong order
        for(int h = 0; h < 8; ++h)
        {
            boolBuf = byteMarker[h];
            byteMarker[h] = byteMarker[h + 8];
            byteMarker[h + 8] = boolBuf;
        }
        byteMarker[7] = 1;
    }
    marker = double(matiCountDecimal(byteMarker));
}

int matiCountDecimal(std::vector<bool> byteMarker)
{
    int res = 0;
    for(int h = 0; h < 16; ++h)
    {
        res += byteMarker[h] * pow(2, h);
    }
    return res;
}

int matiCountDecimal(QString byteMarker)
{
    byteMarker.remove(' ');
    if(byteMarker.length() != 16) return 0;

    int res = 0;
    for(int h = 0; h < 16; ++h)
    {
		res += ((byteMarker[h] == QChar('1')) ? 1 : 0) * pow(2, 15-h);
    }
    return res;
}


template uint indexOfMax(const std::vector<int> & cont);
template uint indexOfMax(const std::vector<double> & cont);
template uint indexOfMax(const std::valarray<double> & cont);
template uint indexOfMax(const std::list<double> & cont);

}// namespace myLib
