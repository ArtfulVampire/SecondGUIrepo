#include "library.h"


using namespace std;
using namespace std::chrono;

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
    int rows;
    int cols;
    file.ignore(64, ' ');
    file >> rows;
    file.ignore(64, ' ');
    file >> cols;

    int len = rows * cols;
    result.resize(len);
    for(int i = 0; i < len; ++i)
    {
        file >> result[i];
    }
    file.close();
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
    file << "FileLen " << outData.size() << '\t';
    file << "Pewpew " << 1 << "\r\n";
    for(auto out : outData)
    {
        file << doubleRound(out, 3) << "\r\n";
    }
    file.close();
}

/// in file and in matrix - transposed
void writePlainData(const QString outPath,
                    const matrix & data,
                    int numOfSlices,
                    int start)
{
    if(numOfSlices <= 0)
    {
        numOfSlices = data.cols();
    }
    else
    {
        numOfSlices = min(numOfSlices,
                          data.cols() - start);
    }

//    if(numOfSlices < 250) return; /// used for sliceWindFromReal() but Cut::cut() ...

    ofstream outStr;
    outStr.open(outPath.toStdString());
    outStr << "NumOfSlices " << numOfSlices << '\t';
    outStr << "NumOfChannels " << data.rows() << "\r\n";
    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < data.rows(); ++j)
        {
            outStr << doubleRound(data[j][i + start], 3) << '\t';
        }
        outStr << "\r\n";
    }
    outStr.flush();
    outStr.close();
}




void readPlainData(const QString & inPath,
                   matrix & data,
                   int & numOfSlices,
                   const int & start)
{
    ifstream inStr;
    inStr.open(inPath.toStdString());
    if(!inStr.good())
    {
        cout << "readPlainData: cannot open file\n" << inPath << endl;
        return;
    }
    int localNs;
    inStr.ignore(64, ' '); // "NumOfSlices "
    inStr >> numOfSlices;
    inStr.ignore(64, ' '); // "NumOfChannels "
    inStr >> localNs;

    data.resize(localNs, numOfSlices + start);

    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < localNs; ++j)
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

void readMatrixFile(const QString & filePath,
                    matrix & outData)
{
    ifstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "readMatrixFile: bad input file " << filePath << endl;
        return;
    }
    int rows;
    int cols;
    file.ignore(64, ' ');
    file >> rows;
    file.ignore(64, ' ');
    file >> cols;

    outData.resize(rows, cols);

    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            file >> outData[i][j];
        }
    }
    file.close();
}


void writeMatrixFile(const QString & filePath,
                      const matrix & outData,
                     const QString & rowsString,
                     const QString & colsString)
{
    ofstream file(filePath.toStdString());
    if(!file.good())
    {
        cout << "writeMatrixFile: bad output file\n" << filePath.toStdString() << endl;
        return;
    }

    file << rowsString << " " << outData.rows() << '\t';
    file << colsString << " " << outData.cols() << "\r\n";

    for(int i = 0; i < outData.rows(); ++i)
    {
        for(int j = 0; j < outData.cols(); ++j)
        {
            file << doubleRound(outData[i][j], 4) << '\t';
        }
        file << "\r\n";
    }
    file.close();
}



template void writeFileInLine(const QString & filePath, const lineType & outData);
template void writeFileInLine(const QString & filePath, const vectType & outData);

template void readFileInLine(const QString & filePath, lineType & outData);
template void readFileInLine(const QString & filePath, vectType & outData);

