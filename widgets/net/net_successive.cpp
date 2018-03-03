#include <widgets/net.h>
#include <other/edffile.h>

#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>

using namespace myOut;


Classifier::avType Net::successiveByEDF(const QString & edfPath1, const QString & ansPath1,
										const QString & edfPath2, const QString & ansPath2)
{
	fb::FBedf file1(edfPath1, ansPath1);
	fb::FBedf file2(edfPath2, ansPath2);

	const auto & markers1 = file1.getMarkers();
	const auto & markArr1 = file1.getMarkArr();
	const double freq1 = file1.getFreq();

	auto staIt1 = std::find_if(std::begin(markers1),
							   std::end(markers1),
							   [](const std::pair<int, int> & in)
	{ return (in.second == 241) || (in.second == 247); });

	int sta1 = (*staIt1).first;

	fb::taskType typ = fb::taskType::rest;
	if((*staIt1).second == 241.)		{ typ = fb::taskType::spat; }
	else if((*staIt1).second == 247.)	{ typ = fb::taskType::verb; }

	myClassifierData = ClassifierData();

	matrix dt1 = file1.getData().subRows(smLib::range<std::vector<uint>>(0, 18 + 1));

	for(uint i = sta1; i < dt1.cols() - suc::windLength * freq1; i += freq1 * suc::shiftLearn)
	{
		auto mark = smLib::contSubsec(markArr1, i, i + suc::windLength * freq1);
		std::pair<bool, double> a = myLib::contains(mark, std::vector<double>{241., 247., 254.});
		if(a.first)
		{
			if(a.second == 241.)		{ typ = fb::taskType::spat; }
			else if(a.second == 247.)	{ typ = fb::taskType::verb; }
			else if(a.second == 254.)	{ typ = fb::taskType::rest; }
			i = i + myLib::indexOfVal(mark, a.second) + 1
				+ 1. * freq1 /// to loose some first windows
				;
			continue;
		}
		matrix wind = dt1.subCols(i, i + suc::windLength * freq1);
		matrix spec = myLib::countSpectre(wind, 1024, suc::numSmooth);

		if(spec.isEmpty()) continue;
		spec = spec.subCols(fftLimit(5., freq1, 1024),
							fftLimit(20., freq1, 1024) + 1);
		myClassifierData.push_back(spec.toVectorByRows(), int(typ), "L " + nm(i));
	}
	myClassifierData.reduceSize(suc::learnSetStay);
	myClassifierData.setApriori(myClassifierData.getClassCount());
	myClassifierData.z_transform();

	this->setClassifier(ModelType::NBC);
	this->setClassifier(ModelType::ANN);
	ANN * myANN = dynamic_cast<ANN *>(myModel);
	if(!myANN)
	{
		std::cout << "Net::successiveByEDF: ANN bad cast" << std::endl;
		return {};
	}

	myANN->setCritError(0.05);
	myANN->setLrate(0.002);
	myANN->learnAll(); /// get initial weights on train set
	myANN->writeWeight(defs::helpPath + "/" + file1.getExpName() + "_init.wts");
	myANN->drawWeight(defs::helpPath + "/" + file1.getExpName() + "_init.wts",
					  defs::helpPath + "/" + file1.getExpName() + "_init.jpg");

	/// consts - set postlearn
	myANN->setCritError(0.01);
	myANN->setLrate(0.005);

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;



	const auto & markers2 = file2.getMarkers();
	const std::valarray<double> & markArr2 = file2.getMarkArr();
	const double freq2 = file2.getFreq();

	auto staIt2 = std::find_if(std::begin(markers2),
							   std::end(markers2),
							   [](const auto & in)
	{ return (in.second == 241) || (in.second == 247); });

	int sta2 = (*staIt2).first;

	typ = fb::taskType::rest;
	if((*staIt2).second == 241.)		{ typ = fb::taskType::spat; }
	else if((*staIt2).second == 247.)	{ typ = fb::taskType::verb; }

	matrix dt2 = file2.getData().subRows(smLib::range<std::vector<uint>>(0, 18 + 1));

	for(int i = sta2; i < dt2.cols() - suc::windLength * freq2; i += freq2 * suc::shiftTest)
	{
		auto mark = smLib::contSubsec(markArr2, i, i + suc::windLength * freq2);

		std::pair<bool, double> a = myLib::contains(mark, std::vector<double>{241., 247., 254.});
		if(a.first)
		{
			if(a.second == 241.)		{ typ = fb::taskType::spat; }
			else if(a.second == 247.)	{ typ = fb::taskType::verb; }
			else if(a.second == 254.)	{ typ = fb::taskType::rest; }
			i = i + myLib::indexOfVal(mark, a.second) + 1;
			continue;
		}
		matrix wind = dt2.subCols(i, i + suc::windLength * freq2);
		matrix spec = myLib::countSpectre(wind, 1024, suc::numSmooth);
		if(spec.isEmpty()) continue;
		spec = spec.subCols(fftLimit(5., freq2, 1024),
							fftLimit(20., freq2, 1024) + 1);

		successiveLearning(spec.toVectorByRows(), int(typ), "T " + nm(i));
	}

	return myModel->averageClassification();
}

/// with correctness, etc
Classifier::avType Net::successiveByEDFfinal(const QString & edfPath1, const QString & ansPath1,
											 const QString & edfPath2, const QString & ansPath2)
{
	DEFS.setFftLen(1024);
	fb::FBedf file1(edfPath1, ansPath1);
	fb::FBedf file2(edfPath2, ansPath2);

	const auto & markers1 = file1.getMarkers();
	const auto & markArr1 = file1.getMarkArr();
	const double freq1 = file1.getFreq();

	auto staIt1 = std::find_if(std::begin(markers1),
							   std::end(markers1),
							   [](const auto & in)
	{ return (in.second == 241) || (in.second == 247); });

	int sta1 = (*staIt1).first;

	fb::taskType typ = fb::taskType::rest;
	if((*staIt1).second == 241.)		{ typ = fb::taskType::spat; }
	else if((*staIt1).second == 247.)	{ typ = fb::taskType::verb; }

	myClassifierData = ClassifierData();

	matrix dt1 = file1.getData().subRows(smLib::range<std::vector<uint>>(0, 18 + 1));

	for(uint i = sta1; i < dt1.cols() - suc::windLength * freq1; i += freq1 * suc::shiftLearn)
	{
		auto mark = smLib::contSubsec(markArr1, i, i + suc::windLength * freq1);
		std::pair<bool, double> a = myLib::contains(mark, std::vector<double>{241., 247., 254.});
		if(a.first)
		{
			if(a.second == 241.)		{ typ = fb::taskType::spat; }
			else if(a.second == 247.)	{ typ = fb::taskType::verb; }
			else if(a.second == 254.)	{ typ = fb::taskType::rest; }
			i = i + myLib::indexOfVal(mark, a.second) + 1
				+ 1. * freq1 /// to loose some first windows
				;
			continue;
		}
		matrix wind = dt1.subCols(i, i + suc::windLength * freq1);
		matrix spec = myLib::countSpectre(wind, 1024, suc::numSmooth);
		if(spec.isEmpty()) continue;
		spec = spec.subCols(fftLimit(5., file1.getFreq(), 1024),
							fftLimit(20., file1.getFreq(), 1024) + 1);
		myClassifierData.push_back(spec.toVectorByRows(), int(typ), "L " + nm(i));
	}


	myClassifierData.reduceSize(suc::learnSetStay);
	myClassifierData.setApriori(myClassifierData.getClassCount());
	myClassifierData.z_transform();

//	std::cout << "learn data size = " << myClassifierData.getData().size() << std::endl;

	this->setClassifier(ModelType::NBC);
	this->setClassifier(ModelType::ANN);
	ANN * myANN = dynamic_cast<ANN *>(myModel);
	if(!myANN)
	{
		std::cout << "Net::successiveByEDF: ANN bad cast" << std::endl;
		return {};
	}

	myANN->setCritError(0.05);
	myANN->setLrate(0.002);
	myANN->learnAll(); /// get initial weights on train set
	myANN->writeWeight(defs::helpPath + "/" + file1.getExpName() + "_init.wts");
	myANN->drawWeight(defs::helpPath + "/" + file1.getExpName() + "_init.wts",
					  defs::helpPath + "/" + file1.getExpName() + "_init.jpg");

	/// consts - set postlearn
	myANN->setCritError(0.01);
	myANN->setLrate(0.005);

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

	QDir(defs::helpPath).mkdir(myLib::getExpNameLib(edfPath1, true));



	const auto & markers2 = file2.getMarkers();
	const std::valarray<double> & markArr2 = file2.getMarkArr();
	const double freq2 = file2.getFreq();

	auto staIt2 = std::find_if(std::begin(markers2),
							   std::end(markers2),
							   [](const auto & in)
	{ return (in.second == 241) || (in.second == 247); });

	int sta2 = (*staIt2).first;

	typ = fb::taskType::rest;
	if((*staIt2).second == 241)		{ typ = fb::taskType::spat; }
	else if((*staIt2).second == 247){ typ = fb::taskType::verb; }
	int numTask = 0;

	matrix dt2 = file2.getData().subRows(smLib::range<std::vector<uint>>(0, 18 + 1));

	matrix relearn{};
	for(int i = sta2; i < dt2.cols() - suc::windLength * freq2; i += freq2 * suc::shiftTest)
	{
		auto mark = smLib::contSubsec(markArr2, i, i + suc::windLength * freq2);

		std::pair<bool, double> a = myLib::contains(mark, std::vector<double>{241., 247., 254.});
		if(a.first)
		{
			if(!relearn.isEmpty()) { successiveLearningFinal(relearn, int(typ)); relearn.clear(); }

			if(a.second == 241.)		{ typ = fb::taskType::spat; ++numTask; }
			else if(a.second == 247.)	{ typ = fb::taskType::verb; ++numTask; }
			else if(a.second == 254.)	{ typ = fb::taskType::rest; }
			i = i + myLib::indexOfVal(mark, a.second) + 1;
			continue;
		}
		matrix wind = dt2.subCols(i, i + suc::windLength * freq2);
		matrix spec = myLib::countSpectre(wind, 1024, suc::numSmooth);

		if(spec.isEmpty()) continue;
		spec = spec.subCols(fftLimit(5., file2.getFreq(), 1024),
							fftLimit(20., file2.getFreq(), 1024) + 1);

		if(typ == fb::taskType::rest || file2.getAns(numTask) == 1)
		{
			relearn.push_back(spec.toVectorByRows());
		}
	}
	myANN->writeWeight(defs::helpPath + "/" + file1.getExpName() + "_last.wts");
	myANN->drawWeight(defs::helpPath + "/" + file1.getExpName() + "_last.wts",
					  defs::helpPath + "/" + file1.getExpName() + "_last.jpg");
	return myModel->averageClassification();
}


void Net::successiveProcessing()
{
    const QString trainMarker = "_train";
	const QString testMarker = "_test";

	const QString helpString = DEFS.windsSpectraDir();

	this->loadData(helpString, {DEFS.getExpName().left(3) + "*" + trainMarker + "*"});

	/// leave 800, z_transform, then reduceSize
	myClassifierData.reduceSize(suc::learnSetStay);

    /// consts - set prelearn
    setErrCrit(0.05);
    setLrate(0.002);

    myModel->learnAll(); /// get initial weights on train set

	ANN * myANN = dynamic_cast<ANN *>(myModel);
	if(!myANN) { std::cout << "Net::successiveProcessing: ANN bad cast" << std::endl; } // return?

	myANN->writeWeight("/media/Files/Data/Feedback/SuccessClass/1.txt");
	myANN->drawWeight("/media/Files/Data/Feedback/SuccessClass/1.txt",
					  "/media/Files/Data/Feedback/SuccessClass/1.jpg");
    /// consts - set postlearn
    setErrCrit(0.01);
    setLrate(0.005);


	QStringList leest = QDir(helpString).entryList(
	{DEFS.getExpName().left(3) + "*" + testMarker + "*"}); /// special generality

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

    std::valarray<double> tempArr;
	int type = -1;
    for(const QString & fileNam : leest)
    {
		tempArr = myLib::readFileInLine(helpString + "/" + fileNam);
		type = myLib::getTypeOfFileName(fileNam);
		successiveLearning(tempArr, type, fileNam);
	}
    myModel->averageClassification();
}

void Net::successivePreclean(const QString & spectraPath, const QStringList & filters)
{
//	QStringList leest = myLib::makeFullFileList(spectraPath, filters);

//	std::cout << "clean first 2 winds" << std::endl;
//	for(const QString & str : leest)
//	{
//		if(str.contains(QRegExp("\\.0[0-1]\\."))) /// change to 0-x for x first winds to delete
//		{
//			QFile::remove(spectraPath + "/" + str);
//		}
//	}

//	std::cout << "clean by learnSetStay" << std::endl;
//	auto leest2 = myLib::makeFileLists(spectraPath);

//	for(int j = 0; j < leest2.size(); ++j)
//	{
//		auto it = std::begin(leest2[j]);
//		for(int i = 0;
//			i < leest2[j].size() - suc::learnSetStay * 2; /// magic const generality
//			++i, ++it)
//		{
//			QFile::remove(spectraPath + "/" + (*it));
//		}
//	}

    Source = source::winds;
    Mode = myMode::N_fold;

    setErrCrit(0.05);

	std::cout << "N-fold cleaning" << std::endl;

    myModel->setTestCleanFlag(true);
	for(int i = 0; i < 5; ++i)
    {
		if(autoClassification(spectraPath).first == 100.) break;
    }
    myModel->setTestCleanFlag(false);
}

void Net::successiveLearning(const std::valarray<double> & newSpectre,
							 const uint newType,
                             const QString & newFileName)
{
	static int numGoodNew = 0;
	myClassifierData.addItem(newSpectre, newType, newFileName);

	// take the last and increment confusion matrix
	const std::pair<int, double> outType = myModel->classifyDatumLast();

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
		myModel->successiveRelearn();
		ANN * myANN = dynamic_cast<ANN *>(myModel);

		myANN->writeWeight();

		numGoodNew = 0;
	}
}

void Net::successiveLearningFinal(const matrix & newSpectra,
								  const int newType)
{
	for(const auto & in : newSpectra)
	{
		/// I don't care about fileNames
		myClassifierData.addItem(in, newType, "");

		/// take the last and increment confusion matrix
		const std::pair<int, double> outType = myModel->classifyDatumLast();

		if((outType.first == newType && outType.second < suc::errorThreshold)
		   || passed[newType] < suc::learnSetStay /// add first learnSetStay windows unconditionally
		   )
		{
			/// delete older row of the same type
			myClassifierData.removeFirstItemOfType(newType);
		}
		else
		{
			myClassifierData.pop_back();
		}
		++passed[newType];
	}
	myModel->successiveRelearn();

	static int wtsCounter = 0;

	ANN * myANN = dynamic_cast<ANN *>(myModel);
	myANN->writeWeight(defs::helpPath
					   + "/" + DEFS.getExpName()
					   + "/" + DEFS.getExpName() + "_" + nm(wtsCounter++) + ".wts");
}
