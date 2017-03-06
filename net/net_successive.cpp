#include "net.h"
using namespace myOut;

void Net::successiveProcessing()
{
    const QString trainMarker = "_train";
	const QString testMarker = "_test";

    numGoodNew = 0;
	const QString helpString = def::windsSpectraDir();

	this->loadData(helpString, {def::ExpName.left(3) + "*" + trainMarker + "*"});

//	std::cout << 1 << std::endl;
	/// reduce learning set to (NumOfClasses * suc::learnSetStay)
	myClassifierData.reduceSize(suc::learnSetStay);

    /// consts - set prelearn
    setErrCrit(0.05);
    setLrate(0.002);

//	std::cout << 1 << std::endl;
    myClassifier->learnAll(); /// get initial weights on train set

//	std::cout << 1 << std::endl;
    /// consts - set postlearn
    setErrCrit(0.01);
    setLrate(0.005);


	QStringList leest = QDir(helpString).entryList(
    {def::ExpName.left(3) + "*" + testMarker + "*"}); /// special generality


	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

    std::valarray<double> tempArr;
	int type = -1;
//	int count = 0;
    for(const QString & fileNam : leest)
    {
		myLib::readFileInLine(helpString + "/" + fileNam,
							  tempArr);
		type = myLib::getTypeOfFileName(fileNam);
        successiveLearning(tempArr, type, fileNam);
//		++count; if(count == 500) break;
	}
    myClassifier->averageClassification();
}

void Net::successivePreclean(const QString & spectraPath)
{
    QStringList leest;
	myLib::makeFullFileList(spectraPath, leest, {"*train*"});
    // clean from first 2 winds from each realisation
	std::cout << "clean first 2 winds" << std::endl;

    for(const QString & str : leest)
    {
		if(str.contains(QRegExp("\\.0[0-1]$"))) /// change to 0-x for x first winds to delete
        {
            QFile::remove(spectraPath + slash + str);
        }
    }

    // clean by learnSetStay
	std::cout << "clean by learnSetStay" << std::endl;
    std::vector<QStringList> leest2;
	myLib::makeFileLists(spectraPath, leest2);

    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        auto it = leest2[j].begin();
        for(int i = 0;
			i < leest2[j].size() - suc::learnSetStay * 1.3; /// magic const generality
            ++i, ++it)
        {
            QFile::remove(spectraPath + slash + (*it));
        }
    }
    Source = source::winds;
    Mode = myMode::N_fold;

    setErrCrit(0.05);

    // N-fold cleaning
	std::cout << "N-fold cleaning" << std::endl;

    myClassifier->setTestCleanFlag(true);
    for(int i = 0; i < 0; ++i)
    {
        autoClassification(spectraPath);
        if(myClassifier->averageClassification().first == 100.) break;
    }
    myClassifier->setTestCleanFlag(false);
}

/// on one incoming vector
void Net::successiveLearning(const std::valarray<double> & newSpectre,
							 const uint newType,
                             const QString & newFileName)
{
	myClassifierData.addItem(newSpectre, newType, newFileName);

	// take the last and increment confusion matrix
	const std::pair<int, double> outType = myClassifier->classifyDatumLast();

	if((outType.first == newType && outType.second < suc::errorThreshold)
	   || passed[newType] < suc::learnSetStay /// add first learnSetStay windows unconditionally
	   )
	{
		/// delete older row of the same type
		myClassifierData.removeFirstItemOfType(newType);
		++numGoodNew;
	}
	else
	{
		myClassifierData.pop_back();
	}
	++passed[newType];

	if(numGoodNew == suc::numGoodNewLimit)
	{
		myClassifier->successiveRelearn();
		numGoodNew = 0;
	}
}
