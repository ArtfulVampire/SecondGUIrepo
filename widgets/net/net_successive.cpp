#include <widgets/net.h>
#include <other/edffile.h>

#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/highlevel.h>
#include <myLib/statistics.h>

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
		myClassifierData.push_back(spec.toValarByRows(), int(typ), "L " + nm(i));
	}
	myClassifierData.reduceSize(suc::learnSetStay);
	myClassifierData.setApriori(myClassifierData.getClassCount());
	myClassifierData.z_transform();
	myClassifierData.adjust(); /// sets numOfClasses and stuff

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
	myANN->writeWeight(def::helpPath + "/" + file1.getExpName() + "_init.wts");
	myANN->drawWeight(def::helpPath + "/" + file1.getExpName() + "_init.wts",
					  def::helpPath + "/" + file1.getExpName() + "_init.jpg");

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

		successiveLearning(spec.toValarByRows(), int(typ), "T " + nm(i));
	}

	return myModel->averageClassification();
}

/// with correctness, etc
Classifier::avType Net::successiveByEDFfinal(const fb::FBedf & file1,
											 const fb::FBedf & file2)
{
	DEFS.setFftLen(fb::FBedf::windFftLen);
	const QString localExpName = file1.getExpName().left(file1.getExpName().indexOf('_'));

	myClassifierData = ClassifierData();

	/// not via constructor because of norming
	for(int i = 0; i < file1.getWindTypes().size(); ++i)
	{
		myClassifierData.push_back(file1.getWindSpectra(i),
								   uint(static_cast<int>(file1.getWindTypes(i))),
								   "L " + nm(i));
	}
	myClassifierData.reduceSize(suc::learnSetStay);
	myClassifierData.setApriori(myClassifierData.getClassCount());
	myClassifierData.z_transform();
	myClassifierData.adjust(); /// sets numOfClasses and stuff

	this->setClassifier(ModelType::NBC);
	this->setClassifier(ModelType::ANN);
	ANN * myANN = dynamic_cast<ANN *>(myModel);
	if(!myANN)
	{
		std::cout << "Net::successiveByEDF: ANN bad cast" << std::endl;
		return {};
	}

	/// get initial weights on the train set
	myANN->setCritError(0.05);
	myANN->setLrate(0.002);
	myANN->learnAll();

	/// save these weights
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_init.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_init.wts",
					  def::helpPath + "/" + localExpName + "_init.jpg");

	/// consts - set postlearn
	myANN->setCritError(0.01);
	myANN->setLrate(0.005);


	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

	QDir(def::helpPath).mkdir(localExpName);

	/// slice winds with overlap !!!!!1
	auto secondData = myLib::sliceWindows(file2.getData().subRows(19),
										  file2.getMarkers(),
										  1000,
										  3.5 / 4.0,
										  0, 0);
	matrix secondWindSpec{};
	std::vector<fb::taskType> secondWindTypes{};

	int leftLim = fftLimit(fb::FBedf::leftFreq, file2.getFreq(), fb::FBedf::windFftLen);
	int rightLim = fftLimit(fb::FBedf::rightFreq, file2.getFreq(), fb::FBedf::windFftLen);

	for(const auto & datum : secondData)
	{
		auto spec = myLib::countSpectre(std::get<0>(datum),
										fb::FBedf::windFftLen,
										5); ///
		if(!spec.isEmpty())
		{
			secondWindSpec.push_back(spec.subCols(leftLim, rightLim).toValarByRows());
			secondWindTypes.push_back(static_cast<fb::taskType>(std::get<1>(datum)));
		}
	}

	fb::taskType prevType = secondWindTypes[0];
	int numTask = 0;
	matrix relearn{};
	for(int i = 1; i < secondWindTypes.size(); ++i)
	{
		if(secondWindTypes[i] != prevType)
		{
			if(!relearn.isEmpty())
			{
				successiveLearningFinal(relearn, uint(static_cast<int>(prevType)), localExpName);
				relearn.clear();
			}
			if(prevType != fb::taskType::rest) { ++numTask; }

			prevType = secondWindTypes[i];
			continue;
		}

		if(prevType == fb::taskType::rest || file2.getAns(numTask) == fb::ansType::correct)
		{
			relearn.push_back(secondWindSpec[i]);
		}
	}
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_last.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_last.wts",
					  def::helpPath + "/" + localExpName + "_last.jpg");

	std::ofstream nullStr("dev/null/");
	return myANN->averageClassification(nullStr);
}

void Net::innerClassHistogram(const fb::FBedf & file1, fb::taskType typ)
{
	DEFS.setFftLen(fb::FBedf::windFftLen);
//	const QString localExpName = file1.getExpName().left(file1.getExpName().indexOf('_'));

	/// load windows spectra (2 or 3 classes), (from all or correct only)
	/// which class to discard in case of 2 classes
//	fb::taskType dropType = (typ == fb::taskType::spat) ? fb::taskType::verb : fb::taskType::spat;

	/// not via ClassifierData constructor because of norming and stuff
	myClassifierData = ClassifierData();
	for(int i = 0; i < file1.getWindTypes().size(); ++i)
	{
		if((file1.getWindAns(i) == fb::ansType::correct
//		   || 1 /// uncomment to add all winds, comment for only correct
		   )
//		   && file1.getWindTypes(i) != dropType /// uncomment for 2 classes
		   )
		{
			myClassifierData.push_back(file1.getWindSpectra(i),
									   uint(static_cast<int>(file1.getWindTypes(i))),
									   "L " + nm(i));
		}
	}
	myClassifierData.setApriori(myClassifierData.getClassCount());
	myClassifierData.z_transform();
	myClassifierData.adjust(); /// sets numOfClasses and stuff

	/// clean the set, learn classifier
	this->setClassifier(ModelType::NBC);
	this->setClassifier(ModelType::ANN);
	ANN * myANN = dynamic_cast<ANN *>(myModel);
	if(!myANN)
	{
		std::cout << "Net::successiveByEDF: ANN bad cast" << std::endl;
		return;
	}
	myANN->setCritError(0.05);
	myANN->setLrate(0.001);
	myANN->adjustLearnRate(DEVNULL);
	myANN->cleaningNfold(-1);			/// until all are true
//	myClassifierData.z_transform(); /// repeat z-transform?

	/// fill myClassifierData with z-transformed winds
	fb::FBedf file2(file1);
	file2.remakeWindows(3.5 / 4.0, 0); /// magic consts

	std::vector<double> res{};
	const int taskNum = static_cast<int>(typ);
	for(int i = 0; i < file2.getWindTypes().size(); ++i)
	{
		if(file2.getWindTypes(i) == typ
		   && file2.getWindAns(i) == fb::ansType::correct /// comment to add all winds, uncomment for only correct
		   )
		{
			myClassifierData.addItem(file2.getWindSpectra(i),
									 uint(taskNum),
									 "L " + nm(i));

			/// classify (correctly solved and all)
			myANN->classifyDatumLast();

			/// record the ioutput of the classifier
			res.push_back( myANN->getOutputLayer(taskNum) );
		}
	}
	/// draw histogram and/or KDE
	QString savePath = file1.getFilePath();
	savePath.replace(".edf", "_kde.jpg", Qt::CaseInsensitive);
	myLib::kernelEst(smLib::vecToValar(res)).save(savePath);
	savePath.replace("_kde.edf", "_hist.jpg", Qt::CaseInsensitive);
	myLib::histogram(smLib::vecToValar(res),
					 res.size() / 30); /// magic const
}

Classifier::avType Net::successiveByEDFfinal(const QString & edfPath1, const QString & ansPath1,
											 const QString & edfPath2, const QString & ansPath2)
{
	return this->successiveByEDFfinal(fb::FBedf(edfPath1, ansPath1),
									  fb::FBedf(edfPath2, ansPath2));
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
		type = myLib::getTypeOfFileName(fileNam, DEFS.getFileMarks());
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
	const Classifier::classOneType outType = myModel->classifyDatumLast();

	if((std::get<0>(outType) && std::get<2>(outType) < suc::errorThreshold)
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
								  const int newType,
								  const QString & currExpName)
{
	for(const auto & in : newSpectra)
	{
		/// I don't care about fileNames
		myClassifierData.addItem(in, newType, "");

		/// take the last and increment confusion matrix
		const Classifier::classOneType outType = myModel->classifyDatumLast();

		if((std::get<0>(outType) && std::get<2>(outType) < suc::errorThreshold)
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
	myANN->writeWeight(def::helpPath
					   + "/" + currExpName
					   + "/" + currExpName + "_" + nm(wtsCounter++) + ".wts");
}
