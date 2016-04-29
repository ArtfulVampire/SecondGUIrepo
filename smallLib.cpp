#include "library.h"


using namespace std;
using namespace std::chrono;

template <typename Container>
int indexOfMax(const Container & cont)
{
    int res = 0;
    int ans = 0;
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
        if(b >= 0)
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


void writeWavFile(const vectType & inData, const QString & outPath)
{
    // http://soundfile.sapp.org/doc/WaveFormat/


    FILE * outFile;
    outFile = fopen(outPath, "wb");
    if(outFile == NULL)
    {
        cout << "cant open file to write" << endl;
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


int len(QString s) //lentgh till double \0-byte for EDF+annotations
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
        cout << "areEqualFiles: some of the files == NULL" << endl;
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
            cout << "equalFiles(false): time = " << myTime.elapsed() / 1000. << " sec"
                 << "\t" << "pos(bytes) = " << pos << endl;
            return false;
        }
        ++pos;
    }
    fclose(fil1);
    fclose(fil2);
    cout << "equalFiles(true): time = " << myTime.elapsed() / 1000. << " sec" << endl;
    return true;
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
//        newName.remove(' ');
        newName.replace(' ', '_');
        newName.remove(R"(')");
        QFile::rename(tmp.absolutePath() + slash() + fileName,
                      tmp.absolutePath() + slash() + newName);
    }
}

QString slash()
{
    return QString(QDir::separator());
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
    return fopen(filePath.toStdString().c_str(), __modes);
}

ostream & operator << (ostream &os, QString toOut)
{
    os << toOut.toStdString();
    return os;
}


template <typename T>
ostream & operator << (ostream &os, vector<T> toOut) // template!
{
    for(int i = 0; i < toOut.size(); ++i)
    {
        os << toOut[i] << '\t';
    }
    return os;
}

ostream & operator << (ostream &os, vector < vector < double > > toOut)
{
    for(unsigned int i = 0; i < toOut.size(); ++i)
    {
        os << toOut[i] << endl;
    }
    return os;

}

ostream & operator << (ostream &os, QList<int> toOut)
{
    for(int i = 0; i < toOut.length(); ++i)
    {
        os << toOut[i] << " ";
    }
    return os;
}

ostream & operator << (ostream &os, matrix toOut)
{
    for(auto it = toOut.data.begin(); it < toOut.data.end(); ++it)
    {
        for(auto itt = begin(*it); itt < end(*it); ++itt)
        {
            os << doubleRound((*itt), 4) << "\t";
        }
        os << endl;
    }
    return os;
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
        QFile::remove(tmpDir.absolutePath() + slash() + lst[h]);
    }

}


double countAngle(double initX, double initY)
{
    if(initX == 0.)
    {
        return (initY > 0.)?(pi/2.):(-pi/2);
    }

    if(initX > 0.)
    {
        return atan(initY/initX);
    }
    else
    {
        return atan(initY/initX) + pi;
    }
}

/*
bool gaussApproval(double * arr, int length) //kobzar page 239
{
    double z;
    int m = int(length/2);
    double a[m+1];
    double disp = variance(arr, length) * length;
    double B = 0.;
    double W;

    a[0] = 0.899/pow(length-2.4, 0.4162) - 0.02;
    for(int j = 1; j <= m; ++j)
    {
        z = (length - 2*j + 1.) / (length - 0.5);
        a[j] = a[0] * (z + 1483 / pow(3.-z, 10.845) + pow(71.61, -10.) / pow(1.1-z, 8.26));
        B += a[j] * (arr[length-j+1] - arr[j-1]); //or without +1
    }
    B *= B;
    W = (1 - 0.6695 / pow(length, 0.6518)) * disp / B;

    if(W < 1.) return true;
    return false;
}


bool gaussApproval(QString filePath)
{
    vectType arr;
    readFileInLine(filePath, arr);
    return gaussApproval(arr.data(), arr.size());
}

bool gaussApproval2(double * arr, int length) //kobzar page 238
{
    double W = 0.;
    double disp = variance(arr, length) * length;
    double c[length+1];
    double sum = 0.;
    for(int i = 1; i <= length; ++i)
    {
        sum += pow(rankit(i, length), 2.);
    }
    sum = sqrt(sum);
    for(int j = 1; j < length; ++j)
    {
        c[j] = rankit(length - j + 1, length) / sum;
        W += c[j] * (arr[length - j] - arr[j - 1]);
    }
    W /= disp;

    /// NOT READY
    return false;


}
*/

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



double quantile(double arg)
{
    double a, b;
    //    a = exp(0.14*log(arg));
    //    b = exp(0.14*log(1-arg));
    a = pow(arg, 0.14);
    b = pow(1. - arg, 0.14);
    return (4.91*(a-b));
}

double rankit(int i, int length, double k)
{
    return quantile( (i-k) / (length + 1. - 2. * k) );
}



QString matiCountByteStr(const double & marker)
{
    QString result;
    vector<bool> byteMarker;
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
    vector<bool> byteMarker;
    byteMarker = matiCountByte(marker);

    if(!pre.isEmpty())
    {
        cout << pre.toStdString() << " = ";
    }
    cout << marker << "\t" << matiCountByteStr(marker) << endl;
}

vector<bool> matiCountByte(double const &  marker)
{
    vector<bool> byteMarker;
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

    vector<bool> byteMarker = matiCountByte(marker);
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

int matiCountDecimal(vector<bool> byteMarker)
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
        res += ((byteMarker[h]==QChar('1'))?1:0) * pow(2, 15-h);
    }
    return res;
}


template int indexOfMax(const std::vector<double> & cont);
template int indexOfMax(const std::valarray<double> & cont);
template int indexOfMax(const std::list<double> & cont);
