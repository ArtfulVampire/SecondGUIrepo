#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <QTextStream>

using namespace myOut;

namespace myLib
{

std::vector<QStringList> makeFileLists(const QString & path,
									   const QStringList & auxFilters)
{
	std::vector<QStringList> res;

	QDir localDir(path);
	for(const QString & fileMark : DEFS.getFileMarks()) /// numOfClasses
	{
		QStringList nameFilters;
		QStringList leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
		for(const QString & filter : leest)
		{
			QString helpString = "*" + filter + "*";
			if(!auxFilters.isEmpty())
			{
				for(const QString & aux : auxFilters)
				{
					nameFilters << QString("*" + aux + helpString);
				}
			}
			else
			{
				nameFilters << helpString;
			}
		}
		res.push_back(localDir.entryList(nameFilters,
										 QDir::Files,
										 QDir::Name)); /// Name ~ order
	}
	return res;
}

QStringList makeFullFileList(const QString & path,
							 const QStringList & auxFilters)
{
	QStringList res{};
	if(DEFS.getFileMarks().isEmpty())
	{
		res = QDir(path).entryList({"*.edf", "*.EDF", QString("*." + def::plainDataExtension)},
								   QDir::Files,
								   QDir::Name); /// Name ~ order
	}
	else
	{
		QDir localDir(path);
		QStringList nameFilters, leest;
		QString helpString;
		for(const QString & fileMark : DEFS.getFileMarks())
		{
			leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
			for(const QString & filter : leest)
			{
				helpString = "*" + filter + "*";
				if(!auxFilters.isEmpty())
				{
					for(const QString & aux : auxFilters)
					{
						nameFilters << QString("*" + aux + helpString);
						nameFilters << QString(helpString + aux + "*");

					}
				}
				else
				{
					nameFilters << helpString;
				}

			}
		}
		res = localDir.entryList(nameFilters,
								 QDir::Files,
								 QDir::Name); /// Name ~ order
	}
	return res;
}


std::vector<matrix> readSpectraDir(const QString & spectraPath)
{
	std::vector<QStringList> lst = makeFileLists(spectraPath);
	std::vector<matrix> spectra;

	for(const QStringList & sublist : lst) /// for each class
	{
		spectra.push_back(matrix()); /// allocate matrix for the class
		for(const QString & item : sublist) /// for each file
		{
			spectra.back().push_back(myLib::readFileInLine(spectraPath + "/" + item));
		}
	}
	return spectra;
}

std::valarray<double> readFileInLineRaw(const QString & filePath)
{
	std::valarray<double> res{};
	std::ifstream file(filePath.toStdString());
	if(!file.good())
	{
		std::cout << "readFileInLine: bad file " << filePath << std::endl;
		return {};
	}
	std::vector<double> vec{};
	double tmp;
	while(file >> tmp)
	{
		if(!file.eof())
		{
			vec.push_back(tmp);
		}
		else { break; }
	}
	file.close();
	res.resize(vec.size());
	std::copy(std::begin(vec), std::end(vec), std::begin(res));
	return res;
}

std::valarray<double> readFileInLine(const QString & filePath)
{
	std::valarray<double> res;
	std::ifstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "readFileInLine: bad file " << filePath << std::endl;
		return {};
    }
    int rows;
    int cols;
    file.ignore(64, ' ');
    file >> rows;
    file.ignore(64, ' ');
    file >> cols;

    int len = rows * cols;
	res.resize(len);
    for(int i = 0; i < len; ++i)
    {
		file >> res[i];
    }
    file.close();
	return res;
}

template <typename ArrayType>
void writeFileInLine(const QString & filePath,
					 const ArrayType & outData)
{
	std::ofstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "writeFileInLine: bad file" << std::endl;
        return;
    }

//	bool asCol = false;
	bool asCol = true;

	file << std::fixed;
	file.precision(6);

	if(asCol)
	{
		file << "FileLen " << outData.size() << "\t";
		file << "Pewpew " << 1 << "\r\n";
		for(auto out : outData)
		{
			file << out << "\r\n";
		}
	}
	else
	{
		file << "Pewpew " << 1 << "\t";
		file << "FileLen " << outData.size() << "\r\n";
		for(auto out : outData)
		{
			file << out << "\t";
		}
	}


    file.close();
}

/// in file and in matrix - transposed
void writePlainData(const QString outPath,
					const matrix & data,
					int sta,
					int fin)
{
	if(fin == -1) { fin = data.cols(); }

	std::ofstream outStr;
	if(outPath.endsWith(def::plainDataExtension))
    {
        outStr.open(outPath.toStdString());
    }
    else
    {
        QString outPathNew = outPath;
		outPathNew.remove("." + def::plainDataExtension); /// what is this for?
		outStr.open((outPathNew + '.' + def::plainDataExtension).toStdString());
    }
	outStr << "NumOfSlices " << fin - sta << '\t';
    outStr << "NumOfChannels " << data.rows() << "\r\n";

	outStr << std::fixed;
	outStr.precision(4);



	for (int i = sta; i < fin; ++i)
    {
        for(uint j = 0; j < data.rows(); ++j)
        {
			outStr << data[j][i] << '\t';
        }
        outStr << "\r\n";
    }
    outStr.flush();
    outStr.close();
}

matrix readPlainData(const QString & inPath)
{
	matrix res{};
	std::ifstream inStr;
    inStr.open(inPath.toStdString());
    if(!inStr.good())
    {
		std::cout << "readPlainData: cannot open file\n" << inPath << std::endl;
		return {};
    }
    int localNs;
	int numOfSlices;
    inStr.ignore(64, ' '); // "NumOfSlices "
    inStr >> numOfSlices;
    inStr.ignore(64, ' '); // "NumOfChannels "
    inStr >> localNs;

	res.resize(localNs, numOfSlices);

    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < localNs; ++j)
        {
			inStr >> res[j][i];
            /// Ossadtchi
//            if(j == ns - 1 && DEFS.isUser(username::Ossadtchi))
//            {
//                if(i == 0) data[j][i + start] = inPath.right(3).toDouble();
//                else if(i == numOfSlices-1) data[j][i + start] = 254;
//                else data[j][i + start] = 0.;
//            }

        }
    }
    inStr.close();
	return res;
}

matrix readMatrixFileRaw(const QString & filePath)
{
	matrix outData{};

	QFile fil(filePath);
	fil.open(QIODevice::ReadOnly);
	if(!fil.isOpen())
	{
		std::cout << "readMatrixFile: can't open file " << filePath << std::endl;
		return {};
	}
	while(1)
	{
		/// QFile
		QString str = fil.readLine();
		{
			if(str.isEmpty()) { break; }
		}
		QStringList lst = str.split('\t',
									QString::SkipEmptyParts);

		std::vector<double> tmp{};
		for(const QString & item : lst)
		{
			tmp.push_back(item.toDouble());
		}
		outData.push_back(tmp);
	}
	fil.close();
	return outData;
}

matrix readMatrixFile(const QString & filePath)
{
	matrix res;
	std::ifstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "readMatrixFile: bad input file " << filePath << std::endl;
		return {};
    }
    int rows;
    int cols;
    file.ignore(64, ' ');
    file >> rows;
    file.ignore(64, ' ');
    file >> cols;

	res.resize(rows, cols);

    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
			file >> res[i][j];
        }
    }
    file.close();
	return res;
}


matrix readIITPfile(const QString & filePath)
{
	std::ifstream inStr;
	inStr.open(filePath.toStdString());
	if(!inStr.good())
	{
		std::cout << "readIITPfile: file not good - " << filePath << std::endl;
		return {};
	}


	/// hat
	inStr.ignore(64, '\n'); /// data
	inStr.ignore(64, '\n'); /// name
	inStr.ignore(64, '\n'); /// Hellow
	inStr.ignore(64, '\n'); /// Dolly
	inStr.ignore(64, '\n'); /// Frames Values Tstart Interval (ms)
	uint numOfRows;
	uint numOfCols;
	inStr >> numOfRows >> numOfCols; inStr.ignore(128, '\n'); /// no need Tstart and inerval

	inStr.ignore(512, '\n'); /// names of values

	matrix res(numOfCols, numOfRows, 0); /// transposed
	for(int i = 0; i < numOfRows; ++i)
	{
		inStr >> res[0][i]; /// ignore first value(time)
		for(int j = 0; j < numOfCols; ++j)
		{
			inStr >> res[j][i];
		}
	}
	inStr.close();
	return res;
}


void readIITPfile(const QString & filePath,
				  matrix & outData,
				  std::vector<QString> & outLabels)
{
	std::ifstream inStr;
	inStr.open(filePath.toStdString());
	if(!inStr.good())
	{
		std::cout << "readIITPfile: file not good - " << filePath << std::endl;
		return;
	}

	/// hat
	inStr.ignore(256, '\n'); /// data
	inStr.ignore(256, '\n'); /// name
	inStr.ignore(256, '\n'); /// Hellow
	inStr.ignore(256, '\n'); /// Dolly
	inStr.ignore(256, '\n'); /// Frames Values Tstart Interval (ms)
	int numOfRows;
	int numOfCols;
	inStr >> numOfRows >> numOfCols; inStr.ignore(256, '\n'); /// no need Tstart and inerval
//	std::cout << numOfRows << '\t' << numOfCols << std::endl;

	std::string tmp;
	outLabels.resize(numOfCols);
	inStr >> tmp; /// Time
	for(int i = 0; i < outLabels.size(); ++i)
	{
		inStr >> tmp;
		outLabels[i] = QString(tmp.c_str());
	}
//	inStr.ignore(64, '\n'); /// need?

	outData = matrix(numOfCols, numOfRows, 0.); /// transposed
	for(int i = 0; i < numOfRows; ++i)
	{
		inStr >> outData[0][i]; /// ignore first value(time)
		for(int j = 0; j < numOfCols; ++j)
		{
			inStr >> outData[j][i];
		}
	}
	inStr.close();

	for(int i = 0; i < outData.rows(); ++i)
	{
		outData[i] -= smLib::mean(outData[i]);
		outData[i] *= 1000; /// magic const
	}
//	return outData;
}

void readUCIdataSet(const QString & setName,
                    matrix & outData,
                    std::vector<uint> & outTypes)
{
    QString newName = setName.toUpper();
	outData = myLib::readMatrixFile(def::uciFolder + "/" + newName + "_data.txt");


	std::ifstream inStr;
	inStr.open((def::uciFolder + "/" + newName + "_types.txt").toStdString());

    int num = 0;
    inStr >> num;
    std::valarray<int> cls(num);
    for(int i = 0; i < num; ++i)
    {
        inStr >> cls[i];
    }
    inStr.close();

    outTypes.resize(cls.sum());
    auto beg = std::begin(outTypes);
    for(int i = 0; i < num; ++i)
    {
        std::fill(beg,
                  beg + cls[i],
                  i);
        beg += cls[i];
    }
}

/// transpose?
void invertMatrixFile(const QString & inPath,
					  const QString & outPath)
{
	std::vector<std::vector<QString>> mtrx;
	QFile inFil(inPath);
	inFil.open(QIODevice::ReadOnly);
//	auto line1 = inFil.readLine();
//	int cols = line1.count('\t') + 1;
//	inFil.reset(); /// set to the beginning

	while(1)
	{
		auto bytArr = inFil.readLine();
		int cols = bytArr.count('\t') + 1;
		if(!bytArr.isEmpty())
		{
			mtrx.push_back(std::vector<QString>(cols));
			QTextStream inStr(bytArr);
			for(int i = 0; i < cols; ++i)
			{
				inStr >> mtrx.back()[i];
			}
		}
		else { break; }

	}
	inFil.close();

	// make equal length
//	std::vector<QString>::size_type maxLen = 0;
//	for(auto in : mtrx)
//	{
//		maxLen = std::max(maxLen, in.size());
//	}
//	for(std::vector<QString> & in : mtrx)
//	{
//		in.resize(maxLen, "");
//	}



	std::ofstream outStr;
	outStr.open(outPath.toStdString());

	for(int j = 0; j < mtrx[0].size(); ++j)
	{
		for(int i = 0; i < mtrx.size(); ++i)
		{
			outStr << mtrx[i][j] << "\t";
		}
		outStr << "\n";
	}
	outStr.close();
}


void writeMatrixFile(const QString & filePath,
					 const matrix & outData,
					 const QString & rowsString,
					 const QString & colsString)
{
	std::ofstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "writeMatrixFile: bad output file\n" << filePath << std::endl;
        return;
    }

    file << rowsString << " " << outData.rows() << '\t';
	file << colsString << " " << outData.cols() << "\r\n";

	file << std::fixed;
	file.precision(4);

	for(const auto row : outData)
	{
		file << row << "\r\n";
    }
	file.flush();
    file.close();
}

void writeSubmatrixFile(const QString & filePath,
						const matrix & outData,
						int sta,
						int fin,
						const QString & rowsString,
						const QString & colsString)
{
	std::ofstream file(filePath.toStdString());
	if(!file.good())
	{
		std::cout << "writeMatrixFile: bad output file\n" << filePath << std::endl;
		return;
	}

	if(fin == -1) { fin = outData.cols(); }

	file << rowsString << " " << outData.rows() << '\t';
	file << colsString << " " << fin - sta << "\r\n";

	file << std::fixed;
	file.precision(4);

	for(uint i = 0; i < outData.rows(); ++i)
	{
		for(uint j = sta; j < fin; ++j)
		{
			file << outData[i][j] << '\t';
		}
		file << "\r\n";
	}
	file.flush();
	file.close();
}

template void writeFileInLine(const QString & filePath, const std::vector<double> & outData);
template void writeFileInLine(const QString & filePath, const std::valarray<double> & outData);
template void writeFileInLine(const QString & filePath, const std::valarray<std::complex<double>> & outData);

} /// end of namespace myLib
