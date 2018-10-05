#include <widgets/spectre.h>
#include "ui_spectre.h"

#include <myLib/signalProcessing.h>
#include <myLib/wavelet.h>
#include <myLib/statistics.h>
#include <myLib/output.h>
#include <myLib/dataHandlers.h>
#include <myLib/draws.h>
#include <myLib/drw.h>

#include <QButtonGroup>
#include <QFileDialog>

using namespace myOut;

Spectre::Spectre() :
    ui(new Ui::Spectre)
{
    ui->setupUi(this);

    this->setWindowTitle("Spectra Counter");

	backupDirPath = DEFS.dirPath();

    QButtonGroup * group1 = new QButtonGroup;
    group1->addButton(ui->jpgButton);
    group1->addButton(ui->svgButton);
    ui->jpgButton->setChecked(true);

    QButtonGroup * group3 = new QButtonGroup;
    group3->addButton(ui->amplitudeWaveletButton);
    group3->addButton(ui->phaseWaveletButton);
    ui->amplitudeWaveletButton->setChecked(true);

    QButtonGroup * group4 = new QButtonGroup;
    group4->addButton(ui->grayRadioButton);
    group4->addButton(ui->colourRadioButton);
    ui->colourRadioButton->setChecked(true);


    ui->amplitudeWaveletButton->setChecked(true);

    ui->smoothBox->setValue(5);
    ui->smoothBox->setMaximum(500);

    ui->fftComboBox->addItem("1024");
    ui->fftComboBox->addItem("2048");
    ui->fftComboBox->addItem("4096");
    ui->fftComboBox->addItem("8192");

    ui->progressBar->setValue(0);

    ui->integrateLineEdit->setText("5-8; 8-9.5; 9.5-13; 13-20");

	ui->scalingDoubleSpinBox->setMinimum(-1.0);
	ui->scalingDoubleSpinBox->setMaximum(200.0);
	ui->scalingDoubleSpinBox->setSingleStep(0.1);
	ui->scalingDoubleSpinBox->setValue(-1.0);

    ui->powDoubleSpinBox->setDecimals(2);
    ui->powDoubleSpinBox->setSingleStep(0.05);
    ui->powDoubleSpinBox->setValue(1.0);
    ui->powDoubleSpinBox->setMaximum(2.0);

    ui->leftSpinBox->setMinimum(0);
    ui->leftSpinBox->setMaximum(1000);

    ui->rightSpinBox->setMinimum(0);
    ui->rightSpinBox->setMaximum(1000);

    ui->MWcheckBox->setChecked(false);

	rangeLimits.resize(DEFS.nsWOM());

    QObject::connect(ui->inputBroswe, SIGNAL(clicked()), this, SLOT(inputDirSlot()));
    QObject::connect(ui->outputBroswe, SIGNAL(clicked()), this, SLOT(outputDirSlot()));

    QObject::connect(ui->countButton, SIGNAL(clicked()), this, SLOT(countSpectraSlot()));

    QObject::connect(ui->fftComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(highlighted(int)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(activated(int)), this, SLOT(setFftLengthSlot()));


    QObject::connect(ui->avButton, SIGNAL(clicked()), this, SLOT(compare()));

    QObject::connect(ui->centerButton, SIGNAL(clicked()), this, SLOT(center()));

    QObject::connect(ui->psaButton, SIGNAL(clicked()), this, SLOT(psaSlot()));

    QObject::connect(ui->integrateButton, SIGNAL(clicked()), this, SLOT(integrate()));

    QObject::connect(ui->waveletsPushButton, SIGNAL(clicked()), this, SLOT(drawWavelets()));

	QObject::connect(ui->leftSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLeft()));
	QObject::connect(ui->rightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setRight()));

#if 0
	/// already inside this->setLeft() and this->setRight()
	QObject::connect(ui->leftSpinBox, &QSpinBox::editingFinished,
					 [this](){ DEFS.setLeftFreq(ui->leftSpinBox->value() * DEFS.spStep()); });
	QObject::connect(ui->RightSpinBox, &QSpinBox::editingFinished,
					 [this](){ DEFS.setRightFreq(ui->leftSpinBox->value() * DEFS.spStep()); });
#endif

    ui->specLabel->installEventFilter(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    {
		int tmp = log2(DEFS.getFftLen()) - 10;
        ui->fftComboBox->setCurrentIndex(0);
        ui->fftComboBox->setCurrentIndex(1);
        ui->fftComboBox->setCurrentIndex(tmp);
    }
}

void Spectre::defaultState()
{
	ui->inputDirLineEdit->setText(defaultInPath);
	ui->outputDirLineEdit->setText(defaultOutPath);

	if(ui->fftComboBox->currentIndex() == 0) /// 1024
    {
        /// shit with external spectra counting
		ui->inputDirLineEdit->setText(defaultInPathW);
		ui->outputDirLineEdit->setText(defaultOutPathW);
        ui->smoothBox->setValue(5);
    }
	else if(ui->fftComboBox->currentIndex() == 1) /// 2048
    {
        ui->smoothBox->setValue(10);
    }
	else if(ui->fftComboBox->currentIndex() == 2) /// 4096
    {
        ui->smoothBox->setValue(15);
    }
	else if(ui->fftComboBox->currentIndex() == 3) /// 8192
    {
        ui->smoothBox->setValue(20);
    }
	ui->leftSpinBox->setValue(DEFS.left());
	ui->rightSpinBox->setValue(DEFS.right());
}

void Spectre::setFftLength(int i)
{
	ui->fftComboBox->setCurrentText(nm(i));
}

void Spectre::setPow(double a)
{
    ui->powDoubleSpinBox->setValue(a);
}

int findChannel(int x, int y, QSize siz)
{
	/// magic consts
	const int a = std::floor(16. / 3. * x / siz.width());	/// look coords::x
	const int b = std::floor(16. / 3. * y / siz.height());	/// look coords::y

    switch(b)
    {
	/// Fp1, Fp2
	case 0:
	{
		if(a == 1) { return 0; }
		if(a == 3) { return 1; }
		break;
	}
	/// O1, O2
	case 4:
	{
		if(a == 1) { return 17; }
		if(a == 3) { return 18; }
		break;
	}
#if 0
		///  inside default
	case 1: { num += 2; break; }
	case 2: { num += 7; break; }
	case 3: { num += 12; break; }
#endif
	/// (Fp1, Fp2) + some lines of 5 + number in a line
	default:
	{
		return 2 + 5 * (b - 1) + a;
	}
	}
	/// never should get here
	return -1;
}

bool Spectre::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == ui->specLabel) /// this is magic
	{
		if(event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
			/// fmod is float resting

			/// between channels graphs by y-axis
			if(std::fmod(16. * mouseEvent->y() / ui->specLabel->height(), 3.) < 0.5)
			{
				return false;
			}

			/// between channels graphs by x-axis
			if(std::fmod(16. * mouseEvent->x() / ui->specLabel->width(), 3.) < 0.5)
			{
				if(mouseEvent->button() == Qt::LeftButton)
				{
					int chanNum = findChannel(mouseEvent->x(),
											  mouseEvent->y(),
											  ui->specLabel->size());
					rangeLimits[chanNum].first = 0;
				}
				else if(mouseEvent->button() == Qt::RightButton)
				{

					int chanNum = findChannel(mouseEvent->x(),
											  mouseEvent->y(),
											  ui->specLabel->size()) - 1;
					rangeLimits[chanNum].second = DEFS.spLength();
				}
				return true;
			}
			/// else
			{
				QString helpString = DEFS.dirPath()
									 + "/Help"
									 + "/" + DEFS.getExpName() + "_all.jpg";
				QPixmap pic;
				pic.load(helpString);
				QPainter paint;
				paint.begin(&pic);

				int chanNum = findChannel(mouseEvent->x(),
										  mouseEvent->y(),
										  ui->specLabel->size());

				double val = (mouseEvent->x()
							  / static_cast<double>(ui->specLabel->width())	/// part of click x
							  - coords::x[chanNum])						/// part of origin x
							 / coords::scale							/// part of graph width
							 * DEFS.spLength();
				if(mouseEvent->button() == Qt::LeftButton)
				{
					rangeLimits[chanNum].first = std::floor(val);

				}
				else if(mouseEvent->button() == Qt::RightButton)
				{
					rangeLimits[chanNum].second = std::ceil(val);
				}

				/// draw limits
				for(int i = 0; i < rangeLimits.size(); ++i)
				{
					paint.setPen(QPen(QBrush("blue"), 2));

					double blueXpart = coords::x[i]
									   + rangeLimits[i].first * coords::scale / DEFS.spLength();

					double redXpart = coords::x[i]
									  + rangeLimits[i].second * coords::scale / DEFS.spLength();

					paint.drawLine(
								QPointF(blueXpart * paint.device()->width(),
										coords::y[i] * paint.device()->height()),
								QPointF(blueXpart * paint.device()->width(),
										(coords::y[i] - coords::scale ) * paint.device()->height())
								);

					paint.setPen(QPen(QBrush("red"), 2));
					paint.drawLine(QPointF(redXpart * paint.device()->width(),
										   coords::y[i] * paint.device()->height()),
								   QPointF(redXpart * paint.device()->width(),
										   (coords::y[i] - coords::scale) * paint.device()->height())
								   );
				}
				ui->specLabel->setPixmap(pic.scaled(ui->specLabel->size()));

				return true;
			}
		}
		/// else
		{
			return false;
		}
    }
    return QWidget::eventFilter(obj, event);
}

void Spectre::inputDirSlot()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose input dir"),
                                                           backupDirPath);
    if(!helpString.isEmpty())
    {
		ui->inputDirLineEdit->setText(helpString);
    }
}

void Spectre::outputDirSlot()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose input dir"),
                                                           backupDirPath);
    if(!helpString.isEmpty())
    {
		ui->outputDirLineEdit->setText(helpString);
    }
}

void Spectre::integrate() /// to deprecate
{
	QStringList ranges = ui->integrateLineEdit->text().split(QRegExp("[,; ]"),
															 QString::SkipEmptyParts);
	const int numOfInt = ranges.size();
    std::vector<int> begins(numOfInt);
	std::vector<int> ends(numOfInt);

	for(const QString & item : ranges)
	{
		QStringList limits = item.split('-', QString::SkipEmptyParts);

		begins.push_back(std::max(fftLimit(limits[0].toDouble(),
								  DEFS.getFreq(),
								  DEFS.getFftLen()) - DEFS.left(),
						 0));
		ends.push_back(std::min(fftLimit(limits[1].toDouble(),
								DEFS.getFreq(),
								DEFS.getFftLen()) - DEFS.left(),
					   DEFS.spLength()));
	}

	matrix dataOut(DEFS.nsWOM(), numOfInt, 0.);

	QStringList lst = myLib::makeFullFileList(ui->inputDirLineEdit->text());

    for(const QString & fileName : lst)
    {

		QString inString = ui->inputDirLineEdit->text()
							 + "/" + fileName;

		matrix dataInt = myLib::readMatrixFile(inString);

        for(uint h = 0; h < dataOut.rows(); ++h)
        {
            for(uint j = 0; j < dataOut.cols(); ++j)
            {
				dataOut[h][j] += std::accumulate(std::begin(dataInt[h]) + begins[j],
												 std::begin(dataInt[h]) + ends[j],
												 0.);
            }
        }

		QString outString = ui->outputDirLineEdit->text()
							+ "/" + fileName;
		myLib::writeMatrixFile(outString, dataOut);
    }
}

void Spectre::psaSlot()
{
    QTime myTime;
    myTime.start();

	const QString psaPath = DEFS.dirPath()
							+ "/Help"
							+ "/psa";

	const QString avPicPath = DEFS.dirPath()
							  + "/Help"
							  + "/" + DEFS.getExpName() + "_all.jpg";



	trivector<int> MW{};

	if(DEFS.isUser(username::ElenaC))
    {
		/// ints
		MW = myLib::countMannWhitney(ui->inputDirLineEdit->text(),
									 nullptr,
									 nullptr);
		QString helpString;
		helpString = DEFS.dirPath() + "/" + DEFS.getExpName() + "_MannWhitney.txt";
		myLib::writeMannWhitney(MW, helpString, " ");

		/// pValues
		trivector<double> MWD = myLib::countMannWhitneyD(ui->inputDirLineEdit->text());
		helpString = DEFS.dirPath() + "/" + DEFS.getExpName() + "_MannWhitneyD.txt";
		myLib::writeMannWhitney(MWD, helpString, "\t");
    }


	/// read data to draw
	matrix drawData;
	for(int i = 0; i < DEFS.numOfClasses(); ++i)
	{
		QString str = psaPath
					  + "/" + DEFS.getExpName()
					  + "_class_" + nm(i + 1)
					  + ".psa";
		std::valarray<double> tempVec = myLib::readFileInLine(str);
		drawData.push_back(tempVec);
	}


	/// add conditions, particularly 128 channels
	if(!DEFS.isUser(username::Ossadtchi))
	{
		/// set colors
		std::vector<QColor> colors;
		if(ui->colourRadioButton->isChecked()) { colors = def::colours; }
		else if(ui->grayRadioButton->isChecked())
		{
			for(int i = 0; i < DEFS.numOfClasses(); ++i)
			{
				colors.push_back(QColor(255. * (DEFS.numOfClasses() - i) / DEFS.numOfClasses(),
										255. * (DEFS.numOfClasses() - i) / DEFS.numOfClasses(),
										255. * (DEFS.numOfClasses() - i) / DEFS.numOfClasses()));
			}
		}
		/// draw
		/// remake to myLib::drw::...
		myLib::drawTemplate(avPicPath);
		myLib::drawArrays(avPicPath,
						  drawData,
						  false,
						  DEFS.getSpecNorm(),
						  ui->scalingDoubleSpinBox->value(),
						  colors);
		if(ui->MWcheckBox->isChecked())
        {
			if(MW.empty())
			{
				MW = myLib::countMannWhitney(ui->inputDirLineEdit->text(),
											 nullptr,
											 nullptr);
			}
			myLib::drawMannWitney(avPicPath,
								  MW,
								  colors);
        }
    }
	else //if(DEFS.isUser(username::Ossadtchi))
    {
		myLib::drawArraysInLine(avPicPath,
								drawData);
        if(ui->MWcheckBox->isChecked())
        {
			myLib::drawMannWitneyInLine(avPicPath,
										MW);
        }
    }
    defaultState();
	ui->specLabel->setPixmap(QPixmap(avPicPath).scaled(ui->specLabel->size()));
	std::cout << "psaSlot: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void Spectre::compare()
{
    QTime myTime;
    myTime.start();

	const QString filesPath = ui->inputDirLineEdit->text();
	const QString savePath = DEFS.dirPath()
							 + "/Help"
							 + "/psa";

	auto leest = myLib:: makeFileLists(filesPath);

	for(int i = 0; i < DEFS.numOfClasses(); ++i)
    {
		const QStringList & lst = leest[i];

		/// set meanVec size
		std::valarray<double> meanVec = myLib::readFileInLine(filesPath + "/" + lst[0]);

		std::valarray<double> tempVec{};
		for(const QString & fileName : lst)
		{
			tempVec = myLib::readFileInLine(filesPath + "/" + fileName);
            meanVec += tempVec;
        }
		meanVec /= lst.size();

		/// maybe make as spectraFile?
		myLib::writeFileInLine(savePath
							   + "/" + DEFS.getExpName()
							   + "_class_" + nm(i + 1)
							   + ".psa",
							   meanVec);
#if 0
		/// draw average for one type
		helpString = DEFS.dirPath()
					 + "/Help"
					 + "/" + DEFS.getExpName()
					 + "_class_" + nm(i + 1)
					 + ".jpg";
        drawTemplate(helpString);
        drawArray(helpString, meanVec);
#endif
    }
	std::cout << "compare: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void Spectre::setFftLengthSlot()
{
	DEFS.setFftLen(ui->fftComboBox->currentText().toInt());

    defaultState();

	for(auto & in : rangeLimits)
    {
		in.first = 0;
		in.second = DEFS.spLength();
    }
}

void Spectre::center() /// to deprecate
{
	const QString pathToCenter = QFileDialog::getExistingDirectory(this,
																   tr("Choose dir"),
																   DEFS.dirPath());
	if(pathToCenter.isEmpty())
    {
        return;
	}

	QStringList lst = QDir(pathToCenter).entryList(QDir::Files | QDir::NoDotAndDotDot);
	matrix averages(DEFS.nsWOM(), DEFS.spLength(), 0.);
	matrix tempData(DEFS.nsWOM(), DEFS.spLength());

    for(const QString & fileName : lst)
    {
		QString helpString = pathToCenter + "/" + fileName;
		tempData = myLib::readMatrixFile(helpString);
        averages += tempData;
    }
    averages /= lst.length();


    /// divide by sigma in each sample. valarray
    for(const QString & fileName : lst)
    {
		QString helpString = pathToCenter + "/" + fileName;
		tempData = myLib::readMatrixFile(helpString);
        tempData -= averages;
        tempData *= 10.;
		myLib::writeMatrixFile(helpString, tempData);
    }
}

Spectre::~Spectre()
{
    delete ui;
}

void Spectre::setSmooth(int a)
{
    this->ui->smoothBox->setValue(a);
}

void Spectre::setRight()
{
	ui->rightHzEdit->setText(nm(ui->rightSpinBox->value() * DEFS.spStep()));
	for(auto & in : rangeLimits)
	{
		in.second = DEFS.spLength();
	}
}

void Spectre::setLeft()
{
	ui->leftHzEdit->setText(nm(ui->leftSpinBox->value() * DEFS.spStep()));
	for(auto & in : rangeLimits)
	{
		in.first = 0;
	}
}


void Spectre::writeSpectra(const std::vector<int> & chanList,
						   const double leftFreq,
						   const double rightFreq)
{

    QTime myTime;
    myTime.start();

	const QString outDirPath = ui->outputDirLineEdit->text();

    if(!QDir(outDirPath).exists())
    {
		std::cout << "writeSpectra: outDir doesn't exist" << std::endl;
        return;
    }

	const int left = fftLimit(leftFreq, globalEdf.getFreq(), DEFS.getFftLen());
	const int right = fftLimit(rightFreq, globalEdf.getFreq(), DEFS.getFftLen()) + 1;

    QStringList lst = ui->dropChannelsLineEdit->text().split(
                          QRegExp("[,;\\s]"), QString::SkipEmptyParts);
	std::cout << "writeSpectra: num of dropped channels = " << lst.size() << std::endl;

	for(const QString & str : lst)
    {
		rangeLimits[str.toInt() - 1] = {0, 0}; /// to fill with zeros
    }

	int dataFFTcounter = 0;
	for(const QString & inFileName : fileNames)
    {
		QString outString = outDirPath + "/" + inFileName;
		outString.replace(".edf", "." + def::spectraDataExtension); /// was def::plainDataExtension

		std::ofstream outStream;
		outStream.open(outString.toStdString());
        if(!outStream.good())
        {
			std::cout << "bad outStream" << std::endl;
            continue;
        }
		outStream << "NumOfChannels " << chanList.size() << '\t';
		outStream << "spLength " << right - left << "\r\n";

		outStream << std::fixed;
		outStream.precision(4);

		/// which channels to write ???
		for(int j = 0; j < chanList.size(); ++j) //
		{
			for(int k = left; k < left + rangeLimits[j].first; ++k)
			{
				outStream << "0.000" << '\t';
			}
			for(int k = left + rangeLimits[j].first;
				k < left + rangeLimits[j].second;
				++k)
			{
				outStream << dataFFT[dataFFTcounter][j][k] << '\t';
			}
			for(int k = std::max(left + rangeLimits[j].first,
								 left + rangeLimits[j].second);
				k < right;
				++k)
			{
				outStream << "0.000" << '\t';
			}
			outStream << "\r\n";
		}
		++dataFFTcounter;
        outStream.close();
    }
	std::cout << "writeSpectra: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
}
void Spectre::countSpectraSlot()
{
//	defaultState(); /// why was it here? 4-Mar-18

	const auto chanList{smLib::range<std::vector<int>>(0, DEFS.getNs())};

	if(!ui->bypassCountCheckBox->isChecked())
	{
		chanList = countSpectra(chanList);
	}
	if(!ui->bypassWriteCheckBox->isChecked())
	{
		writeSpectra(chanList);
	}

#if 0
    /// if clean
	cleanSpectra(); /// using mann-whitney
#endif

	ui->inputDirLineEdit->setText(ui->outputDirLineEdit->text());
	ui->outputDirLineEdit->setText(DEFS.dirPath() + "/Help");
}

void Spectre::cleanSpectra()
{
    QTime myTime;
	myTime.start();

    trivector<int> MW;
	MW = myLib::countMannWhitney(ui->outputDirLineEdit->text(),  /// SpectraSmooth
								 nullptr,
								 nullptr);
    int cnt = 0;
	for(int k = 0; k < MW[0][1].size(); ++k)
	{
		int num = 0;
		for(int i = 0; i < DEFS.numOfClasses(); ++i)
        {
			for(int j = i + 1; j < DEFS.numOfClasses(); ++j)
            {
                if(MW[i][j - i][k] != 0)
                {
                    ++num;
                }
            }
        }
        if(num < 2)
        {
            for(matrix & sp : dataFFT)
            {
				sp[k / DEFS.spLength()][k % DEFS.spLength()] = 0.;
            }
            ++cnt;
		}
    }
	std::cout << "cleanSpectra: num of zeroed points = " << cnt << std::endl;
	ui->outputDirLineEdit->setText(ui->outputDirLineEdit->text() + "/Clean");
    writeSpectra();
	std::cout << "cleanSpectra: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void Spectre::setInPath(const QString & in)
{
    defaultInPath = in;
}
void Spectre::setOutPath(const QString & out)
{
    defaultOutPath = out;
}

std::vector<int> Spectre::countSpectra(std::vector<int> chanList)
{
	QTime myTime;
	myTime.start();

	if(chanList.empty())
	{
		chanList = coords::leest19;
		chanList = globalEdf.findChannels("EEG ");
	}

	const QString inDirPath = ui->inputDirLineEdit->text();

	QStringList lst = myLib::makeFullFileList(inDirPath);
	const int numFiles = lst.length();

	dataFFT.resize(numFiles);
	fileNames.resize(numFiles);
	std::copy(std::begin(lst), std::end(lst), std::begin(fileNames));

	int cnt = 0;
	std::vector<uint> exIndices;
	uint fileNumber = 0;
	std::vector<uint> exFileNumbers;

	for(const QString & fileName : fileNames)
	{
		if(fileName.contains("_num") ||
		   fileName.contains("_sht")) { continue; }

		/// read data file
		matrix dataIn = edfFile(inDirPath + "/" + fileName).getData().subRows(chanList);

		dataFFT[cnt] = myLib::countSpectre(dataIn, DEFS.getFftLen(), ui->smoothBox->value());
		if(!dataFFT[cnt].isEmpty())
		{
			++cnt;
		}
		else
		{
			/// if can't calculate spectre, e.g. too short real/wind
			QRegExp reg{};
			if(DEFS.isUser(username::ElenaC))
			{
				reg = QRegExp(R"(_[0-9]{1,}_)");
			}
			else
			{
				reg = QRegExp(R"([._][0-9]{4}[._])");
			}

			reg.indexIn(fileName);
			exFileNumbers.push_back(reg.cap().remove("_").remove(".").toInt());
			exIndices.push_back(fileNumber);
		}
		ui->progressBar->setValue(++fileNumber * 100. / numFiles);
		qApp->processEvents();
	}

	if(DEFS.isUser(username::ElenaC))
	{
		if(!exFileNumbers.empty())
		{
			std::cout << "countSpectra: haven't calculated files - " << std::endl;
			std::cout << exFileNumbers << std::endl;
		}
	}

	smLib::eraseItems(fileNames, exIndices); /// effect on this->writeSpectra()
	dataFFT.resize(cnt);

	ui->progressBar->setValue(0);

	/// generality
	std::cout << "countSpectra: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
	return chanList;
}

void Spectre::drawWavelets() /// unused
{
    QString helpString;
    QString filePath;
	QDir localDir(DEFS.dirPath());
	helpString = DEFS.dirPath() + "/visualisation/wavelets";
    localDir.cd(helpString);

	/// make dirs
	for(int i = 0; i < DEFS.nsWOM(); ++i)
    {
		localDir.mkdir(nm(i));

        if(ui->phaseWaveletButton->isChecked())
        {
			/// for phase
			localDir.cd(nm(i));
			for(int j = 0; j < DEFS.getNs(); ++j)
            {
				localDir.mkdir(nm(j));
            }
            localDir.cdUp();
        }
    }

	QStringList lst = myLib::makeFullFileList(ui->inputDirLineEdit->text());

    matrix signal;
    matrix coefs;
	std::ofstream outStr;
	outStr.open(helpString.toStdString(), std::ios_base::app);

	std::set<double, std::greater<>> tempVec;

	/// count maxValue
    for(const QString & fileName : lst)
    {
		filePath = ui->inputDirLineEdit->text()
				   + "/" + fileName;
		signal = edfFile(filePath).getData();

		for(int chanNum = 0; chanNum < DEFS.nsWOM(); ++chanNum)
        {
            coefs = wvlt::countWavelet(signal[chanNum]);
            tempVec.emplace(coefs.maxVal());
            continue;

			helpString = DEFS.dirPath()
						 + "/visualisation"
						 + "/wavelets"
						 + "/" + nm(chanNum) + ".txt";

			outStr << coefs.maxVal() << std::endl;
		}
	}
	outStr.close();

#if 0
	/// cout min and max by each channel
	for(int chanNum = 0; chanNum < DEFS.nsWOM(); ++chanNum)
	{
		helpString = DEFS.dirPath()
					 + "/visualisation"
					 + "/wavelets"
					 + "/" + nm(chanNum) + ".txt";
		tempVec = readFileInLine(helpString);
		std::sort(std::begin(tempVec), std::end(tempVec));
		std::cout << tempVec.front() << "\t" << tempVec.back() << std::endl;
	}
#endif
	return;


    for(auto it = lst.begin(); it != lst.end(); ++it)
    {
        const QString & fileName = (*it);
		filePath = DEFS.dirPath()
				   + "/" + fileName;
        if(ui->amplitudeWaveletButton->isChecked())
		{
			for(int channel = 0; channel < DEFS.nsWOM(); ++channel)
			{
				helpString = fileName;
				helpString.replace('.', '_');
				helpString = backupDirPath
							 + "/visualisation"
							 + "/wavelets"
							 + "/" + nm(channel)
							 + "/" + helpString
							 + "_wavelet_" + nm(channel)
							 + ".jpg";
				std::cout << helpString.toStdString() << std::endl;
				wvlt::wavelet(filePath, helpString, channel, DEFS.getNs());
			}
		}
		if(ui->phaseWaveletButton->isChecked())
		{
			for(int channel1 = 0; channel1 < DEFS.nsWOM(); ++channel1)
			{
				for(int channel2 = channel1+1; channel2 < DEFS.nsWOM(); ++channel2)
				{
					helpString = fileName;
					helpString.replace('.', '_');
					helpString = backupDirPath
								 + "/visualisation"
								 + "/wavelets"
								 + "/" + nm(channel1)
								 + "/" + nm(channel2)
								 + "/" + helpString
								 + "_wavelet_" + nm(channel1)
								 + "_" + nm(channel2)
								 + ".jpg";
					std::cout << helpString.toStdString() << std::endl;
#if 0
					waveletPhase(helpString, file1, ns, channel1, channel2, 20., 5., 0.95, 32);
#endif
				}
			}
        }
        ui->progressBar->setValue((std::distance(lst.begin(), it) + 1)
                                  * 100. / lst.length());
        qApp->processEvents();
    }
    ui->progressBar->setValue(0);
}

