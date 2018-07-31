#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/drw.h>
#include <myLib/mati.h>
#include <myLib/dataHandlers.h>
#include <myLib/general.h>
#include <myLib/highlevel.h>

using namespace myOut;

matrix countEvoked(const std::vector<matrix> & inData, int length)
{
	matrix res(inData[0].rows(), length);

	for(const matrix & in : inData)
	{
		res += in.subCols(0, length);
	}
	res /= inData.size();
	return res;
}

void MainWindow::drawWeights(const QString & wtsPath,
							 const QString & spectraPath,
							 const QString & outPath,
							 int fftLen)
{
	/// draw Wts from a folder
	ANN * net = new ANN();

	ClassifierData cl = ClassifierData(spectraPath);
	net->setClassifierData(cl);

	DEFS.setFftLen(fftLen);

	for(const QString & fileName : QDir(wtsPath).entryList({"*.wts"}))
	{
		QString drawName = fileName;
		drawName.replace(".wts", ".jpg");
		net->drawWeight(wtsPath + "/" + fileName,
						outPath + "/" + drawName);
	}
	delete net;
}

void MainWindow::makeEvoked(const QString & edfPath,
							const std::valarray<double> & startMarkers,
							const QString outPath)
{
	const int length = 600;
	edfFile fil;
	fil.readEdfFile(edfPath);
	std::vector<matrix> dat;
	for(auto mark : fil.getMarkers())
	{
		if(std::find(std::begin(startMarkers),
					 std::end(startMarkers),
					 mark.second) != std::end(startMarkers))
		{
			dat.push_back(fil.getData().subCols(mark.first, mark.first + length));
		}
	}
	auto res = countEvoked(dat, length);
	myLib::writeMatrixFile(outPath, res);
}

void MainWindow::makeEvoked(const QString & realsPath,
							const QStringList & fileFilters,
							const QString outPath)
{
	const int length = 600;
	QStringList reals = QDir(realsPath).entryList(fileFilters);

	std::vector<matrix> dat(reals.size());
	int i = 0;
	for(QString fileName : reals)
	{
		dat[i++] = myLib::readMatrixFile(realsPath + "/" + fileName);
	}
	auto res = countEvoked(dat, length);
	myLib::writeMatrixFile(outPath, res);
}


/// successiveByEDF ansPathes
void MainWindow::testSuccessive2()
{
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};
//	const QStringList names {"GAS"};


	std::vector<double> res;
	for(QString name : names)
	{
		/// current best set
		suc::numGoodNewLimit = 3;
		suc::learnSetStay = 40;
		suc::decayRate = 0.00;


		/// should not change averageDatum and sigmaVector ?
		Net * net = new Net();

		net->setSource("w");
		net->setMode("t"); /// train-test

		std::cout << name << std::endl;
		res.push_back(
					net->successiveByEDFnew(path + name + "_train" + ".edf", "",
										 path + name + "_test" + ".edf", "")
					.first);
		delete net;
	}
	outStream << "average by people = "
			  << std::accumulate(std::begin(res), std::end(res), 0.) / res.size() << std::endl;
}

void MainWindow::testSuccessive(const std::vector<double> & vals)
{
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

//	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};
	const QStringList names {"GAS"};

	bool sliceAndCount = true;
//	bool sliceAndCount = false;



	const double windLength	= suc::windLength;
	const double shiftTest	= suc::shiftTest;
	const double shiftLearn	= suc::shiftLearn;
	const int numSmooth		= suc::numSmooth;


	ui->windowLengthSpinBox->setValue(windLength);
	ui->windsButton->setChecked(true); /// sliceWindFromReal

	for(QString name : names)
	{
		/// successive
		setEdfFile(path + name + "_train.edf");

		myLib::cleanDir(path + "Reals");
		myLib::cleanDir(path + "winds/fromreal");
		myLib::cleanDir(path + "SpectraSmooth/winds");

		ui->timeShiftSpinBox->setValue(shiftLearn);
		if(sliceAndCount)
		{
			sliceAll();
			myLib::cleanDir(path + "Reals");
//			countSpectraSimple(1024, numSmooth);


			/// delete first three winds from each realisation
			QStringList windsList = QDir(path + "winds/fromreal").entryList({"*_train*.00",
																			 "*_train*.01",
																			 "*_train*.02"},
																			QDir::Files);
			/// delete first some winds from reals
			for(const QString & name : windsList)
			{
				QFile::remove(path + "winds/fromreal/" + name);
			}

			/// magic constant
			/// leave last 600 winds (some will fall out further due to zeros)
			/// REMAKE - leave 120 each type
			windsList = myLib::makeFullFileList(path + "winds/fromreal", {"_train"});
			for(int i = 0; i < windsList.length() - 800; ++i) /// constant
			{
				QFile::remove(path + "winds/fromreal/" + windsList[i]);
			}
		}
		setEdfFile(path + name + "_test" + ".edf");
		ui->timeShiftSpinBox->setValue(shiftTest);

		/// DON'T CLEAR, TRAIN winds TAKEN BY SUCCESSIVE
		if(sliceAndCount)
		{
			sliceAll();
			myLib::cleanDir(path + "Reals");
//			countSpectraSimple(1024, numSmooth);
		}
		else
		{
			readData();
		}

		/// current best set
		suc::numGoodNewLimit = 3;
		suc::learnSetStay = 40;
		suc::decayRate = 0.00;


		countSpectraSimple(1024, numSmooth);
		/// should not change averageDatum and sigmaVector ?
		Net * net = new Net();
		net->loadData(DEFS.windsSpectraDir(), {name + "_train"});

		net->setClassifier(ModelType::ANN);
		net->setSource("w");
		net->setMode("t"); /// train-test

		std::cout << name << std::endl;
		net->successiveProcessing();
		delete net;


	}
}

void MainWindow::testNewClassifiers()
{
	QString paath = "/media/Files/Data/Feedback/SuccessClass/";

//	for(QString guy : {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"})
	for(QString guy : {"PMI", "SMM", "SMS", "SUA"})
//	for(QString guy : {"GAS"})
	{
		for(QString suff :{"_train", "_test"})
		{
			setEdfFile(paath + guy + suff + ".edf");
//			readData();

			Net * net = new Net();
			net->loadData(paath + "/SpectraSmooth/winds", {guy + suff});

			net->setClassifier(ModelType::ANN);
			net->setMode("k");
			net->setSource("w");
			net->setNumOfPairs(30);
			net->setFold(4);

//			net->customF(); /// clean to 3*N train windows


//			std::cout << guy + suff << std::endl;
			net->autoClassification();



			std::vector<std::vector<double>> allPew =  {{3., 30.},
														{3., 40.},
														{4., 40.},
														{4., 50.},
														{5., 50.},
														{6., 60.},
														{8., 80.},
														{8., 100.},
														{10., 100.}
													   };
//			for(auto pewww : allPew)
//			{
			for(int i1 = 3; i1 <= 8; ++i1)
			{
				for(int i2 = 70; i2 <= 110; i2 += 10)
				{
					for(double i3 : {-0.005, 0.00, 0.005})
					{
//						suc::numGoodNewLimit = pewww[0];
//						suc::learnSetStay = pewww[1];
						suc::numGoodNewLimit = i1;
						suc::learnSetStay = i2;
						suc::decayRate = i3;
						std::cout << guy << std::endl;
						std::cout << suc::numGoodNewLimit << '\t';
						std::cout << suc::learnSetStay << '\t';
						std::cout << suc::decayRate << std::endl;
						net->successiveProcessing();
					}
				}

			}

			delete net;
			break; /// only suff = "_train"
		}

//		continue;

		/// loading UCI dataset - add enum
//		net->loadDataUCI("cmi");
//		net->setClassifier(ClassifierType::RDA);
//		net->setRdaLambdaSlot(0.8);
//		net->setRdaShrinkSlot(0.8);
//		net->autoClassification();

	}
}

void MainWindow::BaklushevDraw(const QString & workPath, const QString & edfName)
{
	QString guy = myLib::getFileName(edfName, false);

	QString filePath = workPath + "/" + edfName;
	if(!QFile::exists(filePath))
	{
		std::cout << "file doesn't exist: " << filePath << std::endl;
		std::cout << "nothing will be drawn" << std::endl;
		return;
	}

	setEdfFile(filePath);
	readData();
	Spectre * sp = new Spectre();
	sp->setFftLength(4096);
	delete sp;

	QString spectraPath = workPath + "/SpectraSmooth";

	int numOfReals[2];
	QStringList lst[2];
	QString marker[2] = {"*_241*" , "*_247*"};
	matrix drawMat[2];

	for(int i = 0; i < 2; ++i)
	{
		lst[i] = QDir(spectraPath).entryList({marker[i]});
		numOfReals[i] = lst[i].size();

		drawMat[i] = matrix(numOfReals[i], 1);

		for(int j = 0; j < numOfReals[i]; ++j)
		{
			drawMat[i][j] = myLib::readFileInLine(spectraPath + "/" + lst[i][j]);
		}
	}
	double norm = 0;

	std::vector<std::valarray<double>> avArr = {drawMat[0].averageRow(),
												drawMat[1].averageRow()};
	std::vector<std::valarray<double>> sigmArr = {drawMat[0].sigmaOfCols(),
												  drawMat[1].sigmaOfCols()};


	QColor currColor[2] = {QColor("blue"), QColor("red")};

	const QString picture[2] = {workPath + "/" + guy + "_picSpat.jpg",
								workPath + "/" + guy + "_picVerb.jpg"};
	const QString pictures[2] = {workPath + "/"  + guy + "_picSpat_2.jpg",
								 workPath + "/"  + guy + "_picVerb_2.jpg"};


	/// max val of avArr + sigmArr
	norm = std::max((avArr[0] + sigmArr[0]).max(), (avArr[1] + sigmArr[1]).max());
	for(int i = 0; i < 2; ++i)
	{
		myLib::drw::drawArrayWithSigma(myLib::drw::drawTemplate(true),
									   avArr[i],
									   sigmArr[i],
									   norm,
									   currColor[i]).save(pictures[i], nullptr, 100);
	}


	/// max val in any of realisations
	norm = std::max(drawMat[0].maxVal(), drawMat[1].maxVal());
	/// draw each realisation spectre
	for(int i = 0; i < 2; ++i)
	{
		myLib::drw::drawArrays(myLib::drw::drawTemplate(),
							   drawMat[i],
							   norm,
							   false,
							   std::vector<QColor>(drawMat[i].rows(), currColor[i]),
							   std::vector<int>(drawMat[i].rows(), 1)).save(
					picture[i], 0, 100);

	}
}

void MainWindow::countSpectraSimple(int fftLen, int inSmooth)
{
    Spectre * sp = new Spectre();
    sp->setFftLength(fftLen);
    if(inSmooth >= 0)
    {
        sp->setSmooth(inSmooth);
    }
    sp->countSpectraSlot();
    sp->compare();
    sp->psaSlot();
    sp->close();
    delete sp;
}

