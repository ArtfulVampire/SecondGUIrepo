#include <widgets/net.h>
#include <other/edffile.h>

#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

Classifier::avType Net::successiveByEDF(const QString & edfPath1,
						  const QString & edfPath2)
{
	QTime myTime;
	myTime.start();

	edfFile fil1;
	fil1.readEdfFile(edfPath1);

	const std::valarray<double> & markers1 = fil1.getMarkArr();
	const double freq1 = fil1.getFreq();

	uint sta1 = std::min(
					myLib::indexOfVal(markers1, 241.),
					myLib::indexOfVal(markers1, 247.)) + 1;

	int typ = -1;
	if(markers1[sta1 - 1] == 241.) typ = 0;
	else if(markers1[sta1 - 1] == 247.) typ = 1;

	myClassifierData = ClassifierData();

	matrix dt1 = fil1.getData().subRows(smLib::range<std::vector<uint>>(0, 18 + 1));
	for(uint i = sta1; i < dt1.cols() - suc::windLength * freq1; i += freq1 * suc::shiftLearn)
	{
		auto mark = smLib::valarSubsec(markers1, i, i + suc::windLength * freq1);

		std::pair<bool, double> a = myLib::contains(mark, {241., 247., 254.});
		if(a.first)
		{
			if(a.second == 241.) typ = 0;
			else if(a.second == 247.) typ = 1;
			else if(a.second == 254.) typ = 2;
			i = i + myLib::indexOfVal(mark, a.second) + 1
//				+ 1. * freq1 /// to loose some first windows
				;
			continue;
		}

		matrix wind = dt1.subCols(i, i + suc::windLength * freq1);


		matrix spec = myLib::countSpectre(wind, 1024, suc::numSmooth);
		if(spec.isEmpty()) continue;
		spec = spec.subCols(fftLimit(5., fil1.getFreq(), 1024),
							fftLimit(20., fil1.getFreq(), 1024) + 1);


//		std::cout << count1++ << " " << typ << " " << "L " + nm(i) << std::endl;
		myClassifierData.push_back(spec.toVectorByRows(), typ, "L " + nm(i));
	}

	std::cout << "file 1 time = "
			  << myTime.elapsed() / 1000. << " sec" << std::endl;
	myTime.restart();

	myClassifierData.setApriori(myClassifierData.getClassCount());

//	myClassifierData.erase(
//				smLib::range<std::vector<uint>>
//				(0,
//				 myClassifierData.getData().rows() - 600));

	myClassifierData.z_transform();

	this->setClassifier(ClassifierType::NBC);
	this->setClassifier(ClassifierType::ANN);

	/// consts - set prelearn
	setErrCrit(0.05);
	setLrate(0.002);

	myClassifier->learnAll(); /// get initial weights on train set
	myClassifierData.reduceSize(suc::learnSetStay);

	ANN * myANN = dynamic_cast<ANN *>(myClassifier);
	myANN->writeWeight("/media/Files/Data/Feedback/SuccessClass/2.txt");
	myANN->drawWeight("/media/Files/Data/Feedback/SuccessClass/2.txt",
					  "/media/Files/Data/Feedback/SuccessClass/2.jpg");

	/// consts - set postlearn
	setErrCrit(0.01);
	setLrate(0.005);

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;


	edfFile fil2;
	fil2.readEdfFile(edfPath2);

	const std::valarray<double> & markers2 = fil2.getMarkArr();
	const double freq2 = fil2.getFreq();

	uint sta2 = std::min(
				   myLib::indexOfVal(markers2, 241.),
			myLib::indexOfVal(markers2, 247.)) + 1;

	typ = -1;
	if(markers2[sta2 - 1] == 241.) typ = 0;
	else if(markers2[sta2 - 1] == 247.) typ = 1;

	matrix dt2 = fil2.getData().subRows(smLib::range<std::vector<uint>>(0, 18 + 1)); /// EEG only
	for(uint i = sta2; i < dt2.cols() - suc::windLength * freq2; i += freq2 * suc::shiftTest)
	{
		auto mark = smLib::valarSubsec(markers2, i, i + suc::windLength * freq2);

		std::pair<bool, double> a = myLib::contains(mark, {241., 247., 254.});
		if(a.first)
		{
			if(a.second == 241.) typ = 0;
			else if(a.second == 247.) typ = 1;
			else if(a.second == 254.) typ = 2;
			i = i + myLib::indexOfVal(mark, a.second) + 1;
			continue;
		}
		matrix wind = dt2.subCols(i, i + suc::windLength * freq2);

		matrix spec = myLib::countSpectre(wind, 1024, suc::numSmooth);
		if(spec.isEmpty()) continue;
		spec = spec.subCols(fftLimit(5., fil2.getFreq(), 1024),
							fftLimit(20., fil2.getFreq(), 1024) + 1);


//		std::cout << count2++ << " " << typ << " " << "T " + nm(i) << std::endl;
		successiveLearning(spec.toVectorByRows(), typ, "T " + nm(i));

	}
	std::cout << "file 2 time = "
			  << myTime.elapsed() / 1000. << " sec" << std::endl;

	return myClassifier->averageClassification();
}

void Net::successiveProcessing()
{
    const QString trainMarker = "_train";
	const QString testMarker = "_test";

	const QString helpString = def::windsSpectraDir();

	this->loadData(helpString, {def::ExpName.left(3) + "*" + trainMarker + "*"});

	/// leave 800, z_transform, then reduceSize
	myClassifierData.reduceSize(suc::learnSetStay);

    /// consts - set prelearn
    setErrCrit(0.05);
    setLrate(0.002);

    myClassifier->learnAll(); /// get initial weights on train set

	ANN * myANN = dynamic_cast<ANN *>(myClassifier);
	myANN->writeWeight("/media/Files/Data/Feedback/SuccessClass/1.txt");
	myANN->drawWeight("/media/Files/Data/Feedback/SuccessClass/1.txt",
					  "/media/Files/Data/Feedback/SuccessClass/1.jpg");
    /// consts - set postlearn
    setErrCrit(0.01);
    setLrate(0.005);


	QStringList leest = QDir(helpString).entryList(
    {def::ExpName.left(3) + "*" + testMarker + "*"}); /// special generality

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

    std::valarray<double> tempArr;
	int type = -1;
    for(const QString & fileNam : leest)
    {
		myLib::readFileInLine(helpString + "/" + fileNam,
							  tempArr);
		type = myLib::getTypeOfFileName(fileNam);
		successiveLearning(tempArr, type, fileNam);
	}
    myClassifier->averageClassification();
}

void Net::successivePreclean(const QString & spectraPath)
{
    QStringList leest;
	myLib::makeFullFileList(spectraPath, leest, {"*train*"});
	std::cout << "clean first 2 winds" << std::endl;

    for(const QString & str : leest)
    {
		if(str.contains(QRegExp("\\.0[0-1]$"))) /// change to 0-x for x first winds to delete
        {
			QFile::remove(spectraPath + "/" + str);
        }
    }

	std::cout << "clean by learnSetStay" << std::endl;
    std::vector<QStringList> leest2;
	myLib::makeFileLists(spectraPath, leest2);

	for(int j = 0; j < leest2.size(); ++j)
    {
		auto it = std::begin(leest2[j]);
        for(int i = 0;
			i < leest2[j].size() - suc::learnSetStay * 1.3; /// magic const generality
            ++i, ++it)
        {
			QFile::remove(spectraPath + "/" + (*it));
        }
    }
    Source = source::winds;
    Mode = myMode::N_fold;

    setErrCrit(0.05);

	std::cout << "N-fold cleaning" << std::endl;

    myClassifier->setTestCleanFlag(true);
    for(int i = 0; i < 0; ++i)
    {
		autoClassification(spectraPath);
        if(myClassifier->averageClassification().first == 100.) break;
    }
    myClassifier->setTestCleanFlag(false);
}

void Net::successiveLearning(const std::valarray<double> & newSpectre,
							 const uint newType,
                             const QString & newFileName)
{
	static int numGoodNew = 0;
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

	if(numGoodNew >= suc::numGoodNewLimit)
	{
		myClassifier->successiveRelearn();
		numGoodNew = 0;
	}
}
