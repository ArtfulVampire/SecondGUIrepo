#include "net.h"
using namespace std;
using namespace myLib;


void Net::successiveProcessing()
{
    const QString trainMarker = "_train";
	const QString testMarker = "_test";
    numGoodNew = 0;
	const QString helpString = def::windsSpectraDir();

	this->loadData(helpString, {def::ExpName.left(3) + "*" + trainMarker + "*"});

//	cout << 1 << endl;
	/// reduce learning set to (NumOfClasses * suc::learnSetStay)
	myClassifierData.reduceSize(suc::learnSetStay);

    /// consts - set prelearn
    setErrCrit(0.05);
    setLrate(0.002);

//	cout << 1 << endl;
    myClassifier->learnAll(); /// get initial weights on train set

//	cout << 1 << endl;
    /// consts - set postlearn
    setErrCrit(0.01);
    setLrate(0.005);


	QStringList leest = QDir(helpString).entryList(
    {def::ExpName.left(3) + "*" + testMarker + "*"}); /// special generality

	this->passed.resize(this->myClassifierData.getNumOfCl());

//	cout << 1 << endl;
    lineType tempArr;
    int type = -1;
//	int count = 0;
    for(const QString & fileNam : leest)
    {
        readFileInLine(helpString + slash + fileNam,
                       tempArr);
		type = getTypeOfFileName(fileNam);
        successiveLearning(tempArr, type, fileNam);
//		++count; if(count == 500) break;
    }
    myClassifier->averageClassification();
}

void Net::successivePreclean(const QString & spectraPath)
{
    QStringList leest;
    makeFullFileList(spectraPath, leest, {"*train*"});
    // clean from first 2 winds from each realisation
    cout << "clean first 2 winds" << endl;

    for(const QString & str : leest)
    {
		if(str.contains(QRegExp("\\.0[0-1]$"))) /// change to 0-x for x first winds to delete
        {
            QFile::remove(spectraPath + slash + str);
        }
    }

    // clean by learnSetStay
    cout << "clean by learnSetStay" << endl;
    std::vector<QStringList> leest2;
    makeFileLists(spectraPath, leest2);

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
    cout << "N-fold cleaning" << endl;

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
