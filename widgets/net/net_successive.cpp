#include <widgets/net.h>
#include <other/edffile.h>

#include <other/defs.h>
#include <other/consts.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/highlevel.h>
#include <myLib/statistics.h>
#include <myLib/valar.h>

using namespace myOut;


/// without correctness
Classifier::avType Net::successiveByEDFnew(const QString & edfPath1, const QString & ansPath1,
										   const QString & edfPath2, const QString & ansPath2)
{
	/// not edfFile because of getLeftLimWind
	fb::FBedf file1(edfPath1, ansPath1, 0, 0);
	fb::FBedf file2(edfPath2, ansPath2, 0, 0);

	DEFS.setDir(file1.getDirPath());		/// for writeWeights
	const QString localExpName = file1.getExpNameShort();

	auto staIt1 = std::find_if(std::begin(file1.getMarkers()),
							   std::end(file1.getMarkers()),
							   [](const std::pair<int, int> & in)
	{ return (in.second == 241) || (in.second == 247); });

	int sta1 = (*staIt1).first + 1;		/// time-bin of the first task

	fb::taskType typ = fb::taskType::rest;
	if((*staIt1).second == 241.)		{ typ = fb::taskType::spat; }
	else if((*staIt1).second == 247.)	{ typ = fb::taskType::verb; }

	myClassifierData = ClassifierData();

	int numTask1 = 1;
	int lastRestStart1{file1.getDataLen()};

	const int step1 = file1.getFreq() * suc::shiftLearn;
	for(uint windStart = sta1;
		windStart < file1.getDataLen() - fb::FBedf::windLen;
		windStart += step1) /// shift 125
	{
		/// markers channel of the current wind
		auto mark = smLib::contSubsec(file1.getMarkArr(),
									  windStart,
									  windStart + fb::FBedf::windLen);

		std::pair<bool, double> a = myLib::contains(mark, std::vector<double>{241., 247., 254.});
		if(a.first) /// is there any markers of task start/end
		{
			if(a.second == 241.)		{ typ = fb::taskType::spat; ++numTask1; }
			else if(a.second == 247.)	{ typ = fb::taskType::verb; ++numTask1; }
			else if(a.second == 254.)
			{
				typ = fb::taskType::rest;
				if(numTask1 == fb::FBedf::numTasks * 2) /// rest after last task
				{
					lastRestStart1 = windStart + myLib::indexOfVal(mark, a.second) + 1;
//					std::cout << lastRestStart << std::endl; exit(0);
				}
			}

			/// move windStart to that marker
			windStart = windStart + myLib::indexOfVal(mark, a.second) + 1 - step1
//				+ 1. * file1.getFreq() /// to skip first windows
				;
			continue; /// windStart will be increased by "step"
		}

		/// take only 7 winds from last rest ~like in myLib::sliceData
		if(windStart > lastRestStart1 + 7 * step1) { break; }

		matrix wind = file1.getData()
					  .subCols(windStart, windStart + fb::FBedf::windLen)
					  .subRows(19); /// magic const
		matrix spec = myLib::countSpectre(wind,
										  fb::FBedf::windFftLen,
										  suc::numSmooth);
		if(!spec.isEmpty())
		{
			myClassifierData.push_back(spec.subCols(file1.getLeftLimWind(),
													file1.getRightLimWind()).toValarByRows(),
									   static_cast<uint>(static_cast<int>(typ)),
									   nm(windStart));
		}
	}
	myClassifierData.reduceSize(suc::learnSetStay);
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

	myANN->writeWeight(def::helpPath + "/" + localExpName + "_init_0.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_init_0.wts",
					  def::helpPath + "/" + localExpName + "_init_0.jpg");
//	return {};

	/// consts - set postlearn
	myANN->setCritError(0.01);
	myANN->setLrate(0.005);

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

	auto staIt2 = std::find_if(std::begin(file2.getMarkers()),
							   std::end(file2.getMarkers()),
							   [](const auto & in)
	{ return (in.second == 241) || (in.second == 247); });

	int sta2 = (*staIt2).first  + 1;

	typ = fb::taskType::rest;
	if((*staIt2).second == 241.)		{ typ = fb::taskType::spat; }
	else if((*staIt2).second == 247.)	{ typ = fb::taskType::verb; }


	int numTask2 = 1;
	int lastRestStart2{file2.getDataLen()};

	int counter2 = 0;

	const int step2 = file2.getFreq() * suc::shiftTest;
	for(int windStart = sta2;
		windStart < file2.getDataLen() - fb::FBedf::windLen;
		windStart += step2)
	{
		auto mark = smLib::contSubsec(file2.getMarkArr(),
									  windStart,
									  windStart + fb::FBedf::windLen);

		std::pair<bool, double> a = myLib::contains(mark, std::vector<double>{241., 247., 254.});
		if(a.first)
		{
			if(a.second == 241.)		{ typ = fb::taskType::spat; ++numTask2; }
			else if(a.second == 247.)	{ typ = fb::taskType::verb; ++numTask2; }
			else if(a.second == 254.)
			{
				typ = fb::taskType::rest;
				if(numTask2 == fb::FBedf::numTasks * 2) /// rest after last task
				{
					lastRestStart2 = windStart + myLib::indexOfVal(mark, a.second) + 1;
				}
			}
			windStart = windStart + myLib::indexOfVal(mark, a.second) + 1 - step2;
			continue;
		}

		/// take only 7 winds from last rest ~like in myLib::sliceData
		if(windStart > lastRestStart2 + 7 * step2) { break; }

		matrix wind = file2.getData()
					  .subCols(windStart, windStart + fb::FBedf::windLen)
					  .subRows(19); /// magic const

		matrix spec = myLib::countSpectre(wind, fb::FBedf::windFftLen, suc::numSmooth);
		if(!spec.isEmpty())
		{
			++counter2;
			successiveLearning(spec.subCols(file2.getLeftLimWind(),
											file2.getRightLimWind()).toValarByRows(),
								static_cast<uint>(static_cast<int>(typ)),
							   "T " + nm(windStart));
		}
	}
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_last_pre_0.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_last_pre_0.wts",
					  def::helpPath + "/" + localExpName + "_last_pre_0.jpg");
	std::cout << counter2 << std::endl;
	return myModel->averageClassification(DEVNULL);
}

Classifier::avType Net::notSuccessive(const fb::FBedf & file1,
									  const fb::FBedf & file2)
{
	DEFS.setFftLen(fb::FBedf::windFftLen);
	DEFS.setDir(file1.getDirPath());		/// for writeWeights
	const QString localExpName = file1.getExpNameShort();

	myClassifierData = ClassifierData();
	/// not via constructor because of special norming
	for(int i = 0; i < file1.getWindTypes().size(); ++i)
	{
		myClassifierData.push_back(file1.getWindSpectra(i),
								   static_cast<uint>(static_cast<int>(file1.getWindTypes(i))),
								   nm(file1.getWindStarts(i)));
	}
	myClassifierData.reduceSize(suc::learnSetStay);
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
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_init_1.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_init_1.wts",
					  def::helpPath + "/" + localExpName + "_init_1.jpg");

	/// consts - set postlearn
	myANN->setCritError(0.01);
	myANN->setLrate(0.005);

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

	QDir(def::helpPath).mkdir(localExpName);

	const int prevSize = myClassifierData.size();
	for(int i = 0; i < file2.getWindTypes().size(); ++i)
	{
		myClassifierData.addItem(file2.getWindSpectra(i),
								 static_cast<uint>(file2.getWindTypes(i)),
								 nm(file2.getWindStarts(i)));
	}
	myANN->test(smLib::range<std::vector<uint>>(prevSize, myClassifierData.size()));

//	myClassifierData = ClassifierData(file2.getWindSpectra(),
//									  static_cast<std::vector<uint>>(file2.getWindTypes()));
//	myANN->testAll();

	myANN->writeWeight(def::helpPath + "/" + localExpName + "_last_pre_1.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_last_pre_1.wts",
					  def::helpPath + "/" + localExpName + "_last_pre_1.jpg");

	return myANN->averageClassification(DEVNULL);
}

/// without correctness
Classifier::avType Net::successiveByEDFnew(const fb::FBedf & file1,
										   const fb::FBedf & file2)
{
	DEFS.setFftLen(fb::FBedf::windFftLen);
	DEFS.setDir(file1.getDirPath());		/// for writeWeights
	const QString localExpName = file1.getExpNameShort();

	myClassifierData = ClassifierData();
	/// not via constructor because of special norming
	for(int i = 0; i < file1.getWindTypes().size(); ++i)
	{
		myClassifierData.push_back(file1.getWindSpectra(i),
								   static_cast<uint>(static_cast<int>(file1.getWindTypes(i))),
								   nm(file1.getWindStarts(i)));
	}
	myClassifierData.reduceSize(suc::learnSetStay);
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
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_init_1.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_init_1.wts",
					  def::helpPath + "/" + localExpName + "_init_1.jpg");

	/// consts - set postlearn
	myANN->setCritError(0.01);
	myANN->setLrate(0.005);

	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

	QDir(def::helpPath).mkdir(localExpName);
#if 01
	/// don't use slicing by FBedf constructor
	/// it violates the sequence of winds (and the classification becomes too well)
	auto secondData = myLib::sliceWindows(file2.getData().subRows(19), /// magic const
										  file2.getMarkers(),
										  fb::FBedf::windLen,
										  suc::overlap125,
										  0, 0);
	matrix secondWindSpec{};
	std::vector<fb::taskType> secondWindTypes{};

	for(const auto & datum : secondData)
	{
		auto spec = myLib::countSpectre(std::get<0>(datum),
										fb::FBedf::windFftLen,
										suc::numSmooth);
		if(!spec.isEmpty())
		{
			secondWindSpec.push_back(spec.subCols(file2.getLeftLimWind(),
												  file2.getRightLimWind()).toValarByRows());
			secondWindTypes.push_back(static_cast<fb::taskType>(std::get<1>(datum)));
		}
	}

	/// successive itself
	for(int i = 0; i < secondWindTypes.size(); ++i)
	{
		successiveLearning(secondWindSpec[i],
						   static_cast<uint>(static_cast<int>(secondWindTypes[i])),
						   localExpName + " " + nm(i));
	}
#else
	//////// WRONG!!!!!!! at first - all spat. then all verb, then all rest
	//////// could be corrected in fb::FBedf constructor
	for(int i = 0; i < file2.getWindTypes().size(); ++i)
	{
		successiveLearning(file2.getWindSpectra(i),
						   static_cast<uint>(static_cast<int>(file2.getWindTypes(i))),
						   nm(file2.getWindStarts(i)));
	}
	std::cout << file2.getWindTypes().size() << std::endl;
#endif
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_last_pre_1.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_last_pre_1.wts",
					  def::helpPath + "/" + localExpName + "_last_pre_1.jpg");

	return myANN->averageClassification(DEVNULL);
}


/// with correctness, etc
Classifier::avType Net::successiveByEDFfinal(const fb::FBedf & file1,
											 const fb::FBedf & file2)
{

	DEFS.setFftLen(fb::FBedf::windFftLen);
	DEFS.setDir(file1.getDirPath());		/// for writeWeights
	const QString localExpName = file1.getExpNameShort();

	myClassifierData = ClassifierData();

	/// not via constructor because of norming
	for(int i = 0; i < file1.getWindTypes().size(); ++i)
	{
		myClassifierData.push_back(file1.getWindSpectra(i),
								   static_cast<uint>(static_cast<int>(file1.getWindTypes(i))),
								   "L " + nm(i));
	}
	myClassifierData.reduceSize(suc::learnSetStay);
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
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_init_f.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_init_f.wts",
					  def::helpPath + "/" + localExpName + "_init_f.jpg");

	/// consts - set postlearn
	myANN->setCritError(0.01);
	myANN->setLrate(0.005);


	this->passed.resize(this->myClassifierData.getNumOfCl());
	this->passed = 0.;

	QDir(def::helpPath).mkdir(localExpName);
	/// TILL NOW IS LIKE successiveByEdfNew(FBedf, FDedf)

	/// slice winds with overlap !!!!!1
	auto secondData = myLib::sliceWindows(file2.getData().subRows(19),  /// magic const
										  file2.getMarkers(),
										  fb::FBedf::windLen,
										  suc::overlap125,
										  0, 0);
	matrix secondWindSpec{};
	std::vector<fb::taskType> secondWindTypes{};


	for(const auto & datum : secondData)
	{
		auto spec = myLib::countSpectre(std::get<0>(datum),
										fb::FBedf::windFftLen,
										suc::numSmooth); ///
		if(!spec.isEmpty())
		{
			secondWindSpec.push_back(spec.subCols(file2.getLeftLimWind(),
												  file2.getRightLimWind()).toValarByRows());
			secondWindTypes.push_back(static_cast<fb::taskType>(std::get<1>(datum)));
		}
	}

	/// successive with correctness
	fb::taskType prevType = secondWindTypes[0];
	int numTask = 0; /// increments when the task ends

	matrix relearn{};
	for(int i = 1; i < secondWindTypes.size(); ++i)
	{
		if(secondWindTypes[i] != prevType) /// new task/rest
		{
			if(!relearn.isEmpty())
			{
				successiveLearningFinal(relearn,
										static_cast<uint>(static_cast<int>(prevType)),
										localExpName);
				relearn.clear();
			}
			if(prevType != fb::taskType::rest) { ++numTask; }

			prevType = secondWindTypes[i];
			continue;
		}
//		else /// if still the same task
		if(prevType == fb::taskType::rest || file2.getAns(numTask) == fb::ansType::correct)
		{
			/// classification correctness is checked inside successiveLearningFinal
			relearn.push_back(secondWindSpec[i]);
		}
	}
	myANN->writeWeight(def::helpPath + "/" + localExpName + "_last.wts");
	myANN->drawWeight(def::helpPath + "/" + localExpName + "_last.wts",
					  def::helpPath + "/" + localExpName + "_last.jpg");

	return myANN->averageClassification(DEVNULL);
}

Net::sucAllType Net::successiveByEDFall(const fb::FBedf & file1,
										const fb::FBedf & file2)
{
	DEFS.setFftLen(fb::FBedf::windFftLen);
	DEFS.setDir(file1.getDirPath());
	const QString localExpName = file1.getExpNameShort();

	myClassifierData = ClassifierData();
	/// not via constructor because of norming
	for(int i = 0; i < file1.getWindTypes().size(); ++i)
	{
		myClassifierData.push_back(file1.getWindSpectra(i),
								   static_cast<uint>(static_cast<int>(file1.getWindTypes(i))),
								   "L " + nm(i));
	}
	myClassifierData.reduceSize(suc::learnSetStay);
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
	auto secondData = myLib::sliceWindows(file2.getData().subRows(-1), /// drop markers
										  file2.getMarkers(),
										  fb::FBedf::windLen,
										  suc::overlap125,
										  0, 0);
	matrix secondWindSpec{};
	std::vector<fb::taskType> secondWindTypes{};

	for(const auto & datum : secondData)
	{
		auto spec = myLib::countSpectre(std::get<0>(datum),
										fb::FBedf::windFftLen,
										suc::numSmooth); ///
		if(!spec.isEmpty())
		{
			secondWindSpec.push_back(spec.subCols(file2.getLeftLimWind(),
												  file2.getRightLimWind()).toValarByRows());
			secondWindTypes.push_back(static_cast<fb::taskType>(std::get<1>(datum)));
		}
	}
	/// backup ClassifierData
	auto clDataBC = myClassifierData;


	if(01)
	{
		/// NOT SUCCESSIVE, BASE
		const int prevSize = myClassifierData.size();
		for(int i = 0; i < file2.getWindTypes().size(); ++i)
		{
			myClassifierData.addItem(file2.getWindSpectra(i),
									 static_cast<uint>(file2.getWindTypes(i)),
									 nm(file2.getWindStarts(i)));
		}
		myANN->test(smLib::range<std::vector<uint>>(prevSize, myClassifierData.size()));
	}
	auto res3 = myANN->averageClassification(DEVNULL);
//	std::cout << res3.first << "\t"; std::cout.flush();


	/// "clean" and restore ClassifierData
	this->passed = 0.;
	myClassifierData = clDataBC;
	myANN->setClassifierData(myClassifierData);

	if(01)
	{
		/// NEW
		for(int i = 0; i < secondWindTypes.size(); ++i)
		{
			successiveLearning(secondWindSpec[i],
							   static_cast<uint>(static_cast<int>(secondWindTypes[i])),
							   localExpName);
		}
		myANN->writeWeight(def::helpPath + "/" + localExpName + "_last_pre.wts");
		myANN->drawWeight(def::helpPath + "/" + localExpName + "_last_pre.wts",
						  def::helpPath + "/" + localExpName + "_last_pre.jpg");
	}
	auto res1 = myANN->averageClassification(DEVNULL);
//	std::cout << res1.first << "\t"; std::cout.flush();


	/// "clean" and restore ClassifierData
	/// NumGoodNew plays no role anymore
	this->passed = 0.;
	myClassifierData = clDataBC;
	myANN->setClassifierData(myClassifierData);

	if(01)
	{
		/// FINAL
		fb::taskType prevType = secondWindTypes[0];
		int numTask = 0; /// increments when the task ends

		matrix relearn{};
		for(int i = 1; i < secondWindTypes.size(); ++i)
		{
			if(secondWindTypes[i] != prevType) /// new task/rest
			{
				if(!relearn.isEmpty())
				{
					successiveLearningFinal(relearn,
											static_cast<uint>(static_cast<int>(prevType)),
											localExpName);
					relearn.clear();
				}
				if(prevType != fb::taskType::rest) { ++numTask; }

				prevType = secondWindTypes[i];
				continue;
			}

			if(prevType == fb::taskType::rest || file2.getAns(numTask) == fb::ansType::correct)
			{
				/// classification correctness is checked inside successiveLearningFinal
				relearn.push_back(secondWindSpec[i]);
			}
		}
		myANN->writeWeight(def::helpPath + "/" + localExpName + "_last.wts");
		myANN->drawWeight(def::helpPath + "/" + localExpName + "_last.wts",
						  def::helpPath + "/" + localExpName + "_last.jpg");
	}
	auto res2 =  myANN->averageClassification(DEVNULL);
//	std::cout << res2.first << std::endl;

	return Net::sucAllType{res1, res2, res3};
}

void Net::innerClassHistogram(const fb::FBedf & file1, fb::taskType typ, fb::ansType howSolved)
{
	std::cout << file1.getFileName(false)
			  << " " << fb::toStr(typ)
			  << " " << fb::toStr(howSolved)
			  << std::endl;

	DEFS.setFftLen(fb::FBedf::windFftLen);

	/// load windows spectra (2 or 3 classes), (from all or correct only)
	/// which class to discard in case of 2 classes? rest?
//	fb::taskType dropType = (typ == fb::taskType::spat) ? fb::taskType::verb : fb::taskType::spat;

	/// not via ClassifierData constructor because of special norming and stuff
	myClassifierData = ClassifierData();
	for(int i = 0; i < file1.getWindTypes().size(); ++i)
	{
		if(fb::isGoodAns(file1.getWindAns(i), howSolved)
//		   && file1.getWindTypes(i) != dropType					/// uncomment for 2 classes
//		   && file1.getWindTypes(i) != fb::taskType::rest		/// uncomment to drop rest
		   )
		{
			myClassifierData.push_back(file1.getWindSpectra(i),
									   static_cast<uint>(static_cast<int>(file1.getWindTypes(i))),
									   "L " + nm(i));
		}
	}
	if(myClassifierData.isEmpty())
	{
		std::cout << " empty data, return" << std::endl << std::endl;
		return;
	}
	myClassifierData.z_transform();
	myClassifierData.adjust(); /// sets numOfClasses and stuff

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
	myANN->adjustLearnRate(/*DEVNULL*/);


	std::cout << "start clean: size = " << myClassifierData.size() << std::endl;
	/// clean the set - VERY TIME CONSUMING
	(myANN->cleaningKfold(20, 5));
	myANN->learnAll();

//	std::cout << "end clean" << std::endl;

//	myANN->cleaningNfold(-1);			/// until all are true
//	myClassifierData.z_transform(); /// repeat z-transform?

	/// CAN AVOID COPYING
	/// fill myClassifierData with z-transformed winds
	fb::FBedf file2(file1);
	file2.remakeWindows(0.5 * file2.getFreq(), 0); /// magic consts

	std::cout << "winds remade" << std::endl;

	std::vector<double> res{};
	const int taskNum = static_cast<int>(typ);
	for(int i = 0; i < file2.getWindTypes().size(); ++i)
	{
		if(file2.getWindTypes(i) == typ
		   && fb::isGoodAns(file2.getWindAns(i), howSolved))
		{
			myClassifierData.addItem(file2.getWindSpectra(i),
									 static_cast<uint>(taskNum),
									 "L " + nm(i));

			/// classify (correctly solved and all)
			myANN->classifyDatumLast();

			/// record the ioutput of the classifier
			res.push_back( myANN->getOutputLayer(taskNum) );
		}
	}
	if(res.empty())
	{
		std::cout << " empty res, return" << std::endl << std::endl;
		return;
	}

	std::cout << "res ready: size = " << res.size() << std::endl;

	/// draw histogram and/or KDE
	QString savePath = file1.getFilePath();
	savePath.chop(4); /// chop .edf
	savePath += "_" + fb::toStr(typ)
				+ "_" + fb::toStr(howSolved);

	myLib::writeFileInLine(savePath + "_data.txt", smLib::vecToValar(res));

	myLib::kernelEst(smLib::vecToValar(res)).save(savePath + "_kde.jpg", nullptr, 100);

	std::cout << "kde ready" << std::endl;
	/// magic const
	myLib::histogram(smLib::vecToValar(res), 50).save(savePath + "_hist.jpg", nullptr, 100);
	std::cout << "hist ready" << std::endl << std::endl;
}


void Net::successiveProcessing()
{
    const QString trainMarker = "_train";
	const QString testMarker = "_test";

	const QString helpString = DEFS.windsSpectraDir();

	this->loadData(helpString, {DEFS.getExpNameShort() + "*" + trainMarker + "*"});

	/// leave 800, z_transform, then reduceSize
	myClassifierData.reduceSize(suc::learnSetStay);

    /// consts - set prelearn
    setErrCrit(0.05);
    setLrate(0.002);

    myModel->learnAll(); /// get initial weights on train set

	ANN * myANN = dynamic_cast<ANN *>(myModel);
	if(!myANN) { std::cout << "Net::successiveProcessing: ANN bad cast" << std::endl; } /// return?

	myANN->writeWeight("/media/Files/Data/Feedback/SuccessClass/1.txt");
	myANN->drawWeight("/media/Files/Data/Feedback/SuccessClass/1.txt",
					  "/media/Files/Data/Feedback/SuccessClass/1.jpg");
    /// consts - set postlearn
    setErrCrit(0.01);
    setLrate(0.005);


	QStringList leest = QDir(helpString).entryList(
	{DEFS.getExpNameShort() + "*" + testMarker + "*"}); /// special generality

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


////////////// To Do, chech
void Net::successivePreclean(const QString & spectraPath, const QStringList & filters)
{
//	QStringList leest = myLib::makeFullFileList(spectraPath, filters);

	/// clean first 2 winds
//	for(const QString & str : leest)
//	{
//		if(str.contains(QRegExp("\\.0[0-1]\\."))) /// change to 0-x for x first winds to delete
//		{
//			QFile::remove(spectraPath + "/" + str);
//		}
//	}

	/// clean by learnSetStay
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

    myModel->setTestCleanFlag(true);
	for(int i = 0; i < 5; ++i)
    {
		if(std::get<0>( autoClassification(spectraPath) ) == 100.) { break; }
    }
    myModel->setTestCleanFlag(false);
}

void Net::successiveLearning(const std::valarray<double> & newSpectre,
							 const uint newType,
                             const QString & newFileName)
{
	static int numGoodNew = 0;
	myClassifierData.addItem(newSpectre, newType, newFileName);

	/// take the last and increment confusion matrix
	const Classifier::classOneType outType = myModel->classifyDatumLast();

	if((std::get<0>(outType) && std::get<2>(outType) < suc::errorThreshold) /// correct class
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
//		ANN * myANN = dynamic_cast<ANN*>(myModel);
//		myANN->writeWeight();
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

		if((std::get<0>(outType) && std::get<2>(outType) < suc::errorThreshold) /// correct class
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
