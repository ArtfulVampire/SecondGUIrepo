#include "library.h"

// make valarray
template <typename signalType>
void readFileInLine(const QString & filePath,
                    signalType & result)
{
    ifstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "readFileInLine: bad file " << filePath << endl;
        return;
    }
    vec outData;
    double tmp;
    int num = 0;
    while(!file.eof())
    {
        file >> tmp;
        outData.push_back(tmp);

        // test
        if(num > def::spLength() * def::nsWOM())
        {
            cout << "readFileInLine: too long file" << endl;
        }

        ++num;
    }
    outData.pop_back(); ///// prevent doubling last item (eof) bicycle
    file.close();

    result.resize(outData.size());
    std::copy(outData.begin(),
              outData.end(),
              begin(result));
}

template <typename signalType>
void writeFileInLine(const QString & filePath,
                     const signalType & outData)
{
    ofstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "bad file" << endl;
        return;
    }
    for(auto out : outData)
    {
        file << doubleRound(out, 4) << '\n'; // \t or \n
    }
    file.close();
}


void writePlainData(const QString outPath,
                    const matrix & data,
                    const int & ns,
                    int numOfSlices,
                    const int & start)
{
    numOfSlices = min(numOfSlices,
                      data.cols() - start);

//    if(numOfSlices < 250) return; /// used for sliceWindFromReal() but Cut::cut() ...

    ofstream outStr;
    outStr.open(outPath.toStdString());
    outStr << "NumOfSlices " << numOfSlices;
    outStr << "\tNumOfChannels " << ns;
    outStr << endl;
    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            outStr << doubleRound(data[j][i + start], 3) << '\t';
        }
        outStr << '\n';
    }
    outStr.flush();
    outStr.close();
}


void readPlainData(const QString & inPath,
                   matrix & data,
                   const int & ns,
                   int & numOfSlices,
                   const int & start)
{
    ifstream inStr;
    inStr.open(inPath.toStdString());
    if(!inStr.good())
    {
        cout << "readPlainData: cannot open file" << endl;
        return;
    }
    inStr.ignore(64, ' '); // "NumOfSlices "
    inStr >> numOfSlices;
    inStr.ignore(64, '\n'); // "NumOfChannels N\n"

    data.resize(ns, numOfSlices + start);

    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            inStr >> data[j][i + start];
            /// Ossadtchi
//            if(j == ns - 1 && def::OssadtchiFlag)
//            {
//                if(i == 0) data[j][i + start] = inPath.right(3).toDouble();
//                else if(i == numOfSlices-1) data[j][i + start] = 254;
//                else data[j][i + start] = 0.;
//            }

        }
    }
    inStr.close();
}

void readSpectraFile(const QString & filePath,
                     matrix & outData,
                     int inNs,
                     int spL)
{
    ifstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "readSpectreFile: bad input file " << filePath << endl;
        return;
    }
    outData.resize(inNs, spL);

    for(int i = 0; i < inNs; ++i)
    {
        for(int j = 0; j < spL; ++j)
        {
            file >> outData[i][j];
        }
    }
    file.close();
}


void writeSpectraFile(const QString & filePath,
                      const matrix & outData,
                      int inNs,
                      int spL)

{
    if(inNs > outData.rows() ||
       spL > outData.cols())
    {
        cout << "bad inputs while writing matrix" << endl;
        return;
    }

    ofstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "bad output file:\n" << filePath.toStdString() << endl;
        return;
    }

    for(int i = 0; i < inNs; ++i)
    {
        for(int j = 0; j < spL; ++j)
        {
            file << doubleRound(outData[i][j], 4) << '\t';
        }
        file << endl;
    }
    file.close();
}


bool readICAMatrix(const QString & path, matrix & matrixA)
{
    readSpectraFile(path, matrixA, def::nsWOM(), def::nsWOM());
    return 1;
}

void writeICAMatrix(const QString & path, const matrix & matrixA)
{
    writeSpectraFile(path,
                     matrixA,
                     matrixA.rows(),
                     matrixA.cols());
}


template void writeFileInLine(const QString & filePath, const lineType & outData);
template void writeFileInLine(const QString & filePath, const vectType & outData);

template void readFileInLine(const QString & filePath, lineType & outData);
template void readFileInLine(const QString & filePath, vectType & outData);

