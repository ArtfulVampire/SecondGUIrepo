#include <widgets/spectre.h>
#include "ui_spectre.h"

#include <myLib/signalProcessing.h>
#include <myLib/wavelet.h>
#include <myLib/statistics.h>
#include <myLib/output.h>
#include <myLib/dataHandlers.h>
#include <myLib/draws.h>
#include <myLib/drw.h>

using namespace myOut;

Spectre::Spectre() :
    ui(new Ui::Spectre)
{
    ui->setupUi(this);

    this->setWindowTitle("Spectra Counter");

	backupDirPath = def::dirPath();

    QButtonGroup * group1 = new QButtonGroup;
    group1->addButton(ui->jpgButton);
    group1->addButton(ui->svgButton);
    ui->jpgButton->setChecked(true);

    QButtonGroup * group2 = new QButtonGroup;
    group2->addButton(ui->spectraRadioButton);
    group2->addButton(ui->brainRateRadioButton);
    group2->addButton(ui->bayesRadioButton);
    group2->addButton(ui->hilbertsVarRadioButton);
    group2->addButton(ui->d2RadioButton);
    ui->spectraRadioButton->setChecked(true);

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

    rangeLimits.resize(def::nsWOM());

    QObject::connect(ui->inputBroswe, SIGNAL(clicked()), this, SLOT(inputDirSlot()));
    QObject::connect(ui->outputBroswe, SIGNAL(clicked()), this, SLOT(outputDirSlot()));

    QObject::connect(ui->countButton, SIGNAL(clicked()), this, SLOT(countSpectraSlot()));

    QObject::connect(ui->fftComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(highlighted(int)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(activated(int)), this, SLOT(setFftLengthSlot()));

    QObject::connect(ui->leftSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLeft()));
    QObject::connect(ui->rightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setRight()));

    QObject::connect(ui->avButton, SIGNAL(clicked()), this, SLOT(compare()));

    QObject::connect(ui->centerButton, SIGNAL(clicked()), this, SLOT(center()));

    QObject::connect(ui->psaButton, SIGNAL(clicked()), this, SLOT(psaSlot()));

    QObject::connect(ui->integrateButton, SIGNAL(clicked()), this, SLOT(integrate()));

    QObject::connect(ui->waveletsPushButton, SIGNAL(clicked()), this, SLOT(drawWavelets()));

    ui->specLabel->installEventFilter(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    {
        int tmp = log2(def::fftLength) - 10;
        ui->fftComboBox->setCurrentIndex(0);
        ui->fftComboBox->setCurrentIndex(1);
        ui->fftComboBox->setCurrentIndex(tmp);
    }
}

void Spectre::defaultState()
{
    ui->lineEdit_1->setText(defaultInPath);
    ui->lineEdit_2->setText(defaultOutPath);

    if(ui->fftComboBox->currentIndex() == 0) // 1024
    {
        /// shit with external spectra counting
        ui->lineEdit_1->setText(defaultInPathW);
        ui->lineEdit_2->setText(defaultOutPathW);
        ui->smoothBox->setValue(5);
    }
    else if(ui->fftComboBox->currentIndex() == 1) // 2048
    {
        ui->smoothBox->setValue(10);
    }
    else if(ui->fftComboBox->currentIndex() == 2) // 4096
    {
        ui->smoothBox->setValue(15);
    }
    else if(ui->fftComboBox->currentIndex() == 3) // 8192
    {
        ui->smoothBox->setValue(20);
    }
    ui->leftSpinBox->setValue(def::left());
    ui->rightSpinBox->setValue(def::right());
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
    int a = floor( x * 16. / siz.width() / 3. );
    int b = floor( y * 16. / siz.height() / 3.);
    int num = 0;

    switch(b)
    {
    case 0:
    {
        if(a == 1) return 0;
		else if(a == 3) return 1;
		break;
    }
	case 1: { num += 2; break; }
	case 2: { num += 7; break; }
	case 3: { num += 12; break; }
    case 4:
    {
        if(a == 1) return 17;
        if(a == 3) return 18;
		break;
	}
    }
    num += a;
    return num;
}

bool Spectre::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->specLabel) // this is magic
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            int chanNum;
            QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
            if(fmod(16. * mouseEvent->y() / ui->specLabel->height(), 3.) < 0.5)
            {
                return false;
            }

            if(fmod(16. * mouseEvent->x() / ui->specLabel->width(), 3.) < 0.5)
            {
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    chanNum = findChannel(mouseEvent->x(),
                                          mouseEvent->y(),
                                          ui->specLabel->size());
                    rangeLimits[chanNum].first = 0;
                }
                else if(mouseEvent->button() == Qt::RightButton)
                {

                    chanNum = findChannel(mouseEvent->x(),
                                          mouseEvent->y(),
                                          ui->specLabel->size()) - 1;
                    rangeLimits[chanNum].second = def::spLength();
                }
                return true;
            }


			QString helpString = (def::dirPath()
														  + "/Help"
														  + "/" + def::ExpName + "_all.jpg");
            QPixmap pic;
            pic.load(helpString);
            QPainter paint;
            paint.begin(&pic);

            chanNum = findChannel(mouseEvent->x(),
                                  mouseEvent->y(),
                                  ui->specLabel->size());

            if(mouseEvent->button() == Qt::LeftButton)
            {

                rangeLimits[chanNum].first = floor((mouseEvent->x()
                                                    * paint.device()->width()
                                                    / ui->specLabel->width()
                                                    - coords::x[chanNum]
                                                    * paint.device()->width())
                                                   / (coords::scale * paint.device()->width() )
                                                   * def::spLength());

            }
            else if(mouseEvent->button() == Qt::RightButton)
            {
                rangeLimits[chanNum].second = ceil((mouseEvent->x()
                                                    * paint.device()->width()
                                                    / ui->specLabel->width()
                                                    - coords::x[chanNum]
                                                    * paint.device()->width())
                                                   / (coords::scale * paint.device()->width())
                                                   * def::spLength());
            }
            for(int i = 0; i < def::nsWOM(); ++i)
            {
                paint.setPen(QPen(QBrush("blue"), 2));
                paint.drawLine(QPointF(coords::x[i]
                                       * paint.device()->width()
                                       + rangeLimits[i].first
                                       * coords::scale
                                       * paint.device()->width()
                                       / def::spLength(),

                                       coords::y[i]
                                       * paint.device()->height()),

                               QPointF(coords::x[i]
                                       * paint.device()->width()
                                       + rangeLimits[i].first
                                       * coords::scale
                                       * paint.device()->width()
                                       / def::spLength(),

                                       coords::y[i]
                                       * paint.device()->height()
                                       - coords::scale
                                       * paint.device()->height()));

                paint.setPen(QPen(QBrush("red"), 2));
                paint.drawLine(QPointF(coords::x[i]
                                       * paint.device()->width()
                                       + rangeLimits[i].second
                                       * coords::scale
                                       * paint.device()->width()
                                       / def::spLength(),

                                       coords::y[i]
                                       * paint.device()->height()),

                               QPointF(coords::x[i]
                                       * paint.device()->width()
                                       + rangeLimits[i].second
                                       * coords::scale
                                       * paint.device()->width()
                                       / def::spLength(),

                                       coords::y[i]
                                       * paint.device()->height()
                                       - coords::scale
                                       * paint.device()->height()));
            }
            ui->specLabel->setPixmap(pic.scaled(ui->specLabel->size()));

            return true;
        }
        else
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
        ui->lineEdit_1->setText(helpString);
    }
}

void Spectre::outputDirSlot()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose input dir"),
                                                           backupDirPath);
    if(!helpString.isEmpty())
    {
        ui->lineEdit_2->setText(helpString);
    }
}

void Spectre::integrate()
{
    QStringList lst = ui->integrateLineEdit->text().split(QRegExp("[; ]"),
                                                          QString::SkipEmptyParts);
    const int numOfInt = lst.length();
    std::vector<int> begins(numOfInt);
    std::vector<int> ends(numOfInt);
    QString helpString;
    QStringList nameFilters;
    for(const QString & item : lst)
    {
        nameFilters = item.split('-', QString::SkipEmptyParts);

		begins.push_back(std::max(fftLimit(nameFilters[0].toDouble(),
								  def::freq,
								  ui->fftComboBox->currentText().toInt()) - def::left() + 1,
						 0));
		ends.push_back(std::min(fftLimit(nameFilters[1].toDouble(),
								def::freq,
								ui->fftComboBox->currentText().toInt()) - def::left() + 1,
					   def::spLength()));
    }

    matrix dataInt(def::nsWOM(), def::spLength());
    matrix dataOut(def::nsWOM(), numOfInt, 0.);

	myLib::makeFullFileList(ui->lineEdit_1->text(), lst);

    for(const QString & fileName : lst)
    {

        helpString = (ui->lineEdit_1->text()
											  + "/" + fileName);
		myLib::readMatrixFile(helpString,
								 dataInt);

        for(uint h = 0; h < dataOut.rows(); ++h)
        {
            for(uint j = 0; j < dataOut.cols(); ++j)
            {
                /// accumulate
                for(int k = begins[j]; k < ends[j]; ++k) // < or <= not really important
                {
                    dataOut[h][k] += dataInt[h][k];
                }
//                dataOut /= 10.; // just for fun?
            }
        }
        helpString = (ui->lineEdit_2->text()
											  + "/" + fileName);
		myLib::writeMatrixFile(helpString, dataOut);
    }
}

void Spectre::psaSlot()
{
    QTime myTime;
    myTime.start();

    matrix drawData;
    std::valarray<double> tempVec(def::spLength() * def::nsWOM());
    QString helpString;
	const QString psaPath = def::dirPath()
							+ "/Help"
							+ "/psa";

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        helpString = (psaPath
					  + "/" + def::ExpName
					  + "_class_" + nm(i + 1)
					  + ".psa");
		myLib::readFileInLine(helpString, tempVec);

        drawData.push_back(tempVec);
    }


    trivector<int> MW;

    if(ui->MWcheckBox->isChecked())
    {
		myLib::countMannWhitney(MW,
								ui->lineEdit_1->text());
		helpString = def::dirPath()
					 + "/" + def::ExpName + "_MannWhitney.txt";
		myLib::writeMannWhitney(MW, helpString);
    }

    if(drawData.cols() == 19 * def::spLength() ||
       drawData.cols() == 21 * def::spLength())
    {
		helpString = def::dirPath()
					 + "/Help"
					 + "/" + def::ExpName + "_all.jpg";
		myLib::drawTemplate(helpString);

        std::vector<QColor> colors;
        if(ui->colourRadioButton->isChecked())
        {
            colors = def::colours;
        }
        else if(ui->grayRadioButton->isChecked())
        {
            for(int i = 0; i < def::numOfClasses(); ++i)
            {
                colors.push_back(QColor(255. * (def::numOfClasses() - i) / def::numOfClasses(),
                                        255. * (def::numOfClasses() - i) / def::numOfClasses(),
                                        255. * (def::numOfClasses() - i) / def::numOfClasses()));
            }
        }

		myLib::drawArrays(helpString,
						  drawData,
						  false,
						  def::drawNormTyp,
						  ui->scalingDoubleSpinBox->value(),
						  colors);

        if(ui->MWcheckBox->isChecked())
        {
			myLib::drawMannWitney(helpString,
                           MW,
                           colors);
        }
    }
    else if(def::OssadtchiFlag)
    {
		myLib::drawArraysInLine(helpString,
                         drawData);
        if(ui->MWcheckBox->isChecked())
        {
			myLib::drawMannWitneyInLine(helpString,
                                 MW);
        }
    }
    defaultState();
    ui->specLabel->setPixmap(QPixmap(helpString).scaled(ui->specLabel->size()));
	std::cout << "psaSlot: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void Spectre::compare()
{
    QTime myTime;
    myTime.start();

    QString helpString;
	std::vector<QStringList> leest;

    std::valarray<double> tempVec(def::spLength() * def::nsWOM());
    std::valarray<double> meanVec(0., def::spLength() * def::nsWOM());

    const QString filesPath = ui->lineEdit_1->text();
	const QString savePath = def::dirPath()
							 + "/Help"
							 + "/psa";

   myLib:: makeFileLists(filesPath,
                  leest
			  #if SPECTRA_EXP_NAME_SPECIFICITY
				  ,{def::ExpName}
			  #endif
//                  ,{def::spectraDataExtension}
                  );


    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        const QStringList & lst = leest[i];
        const int NumOfPatterns = lst.length();
        meanVec = std::valarray<double>(0., def::spLength() * def::nsWOM());


        for(int j = 0; j < NumOfPatterns; ++j)
        {
			helpString = (filesPath
						  + "/"
						  + lst[j]);
			myLib::readFileInLine(helpString, tempVec);

            meanVec += tempVec;
        }
        meanVec /= NumOfPatterns;

        // psa name
		helpString = (savePath
					  + "/" + def::ExpName
					  + "_class_" + nm(i + 1)
					  + ".psa");
        /// maybe make as spectraFile?
		myLib::writeFileInLine(helpString, meanVec);
#if 0
		/// draw average for one type
		helpString = (def::dirPath()
					  + "/Help"
					  + "/" + def::ExpName
					  + "_class_" + nm(i + 1)
					  + ".jpg");
        drawTemplate(helpString);
        drawArray(helpString, meanVec);
#endif
    }

	std::cout << "compare: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void Spectre::setFftLengthSlot()
{
    def::fftLength = ui->fftComboBox->currentText().toInt();

    defaultState();

    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i].first = 0;
        rangeLimits[i].second = def::spLength();
    }
}

void Spectre::center()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose dir"),
														   def::dirPath());
    if(helpString.isEmpty())
    {
        return;
    }
    def::dir->setPath(helpString);

    QStringList lst = def::dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
    matrix averages(def::nsWOM(), def::spLength(), 0.);
    matrix tempData(def::nsWOM(), def::spLength());

    for(const QString & fileName : lst)
    {
		helpString = (def::dirPath()
											  + "/" + fileName);
		myLib::readMatrixFile(helpString,
                        tempData);
        averages += tempData;
    }
    averages /= lst.length();

    /// divide by sigma in each sample. valarray

    for(const QString & fileName : lst)
    {
		helpString = (def::dirPath()
											  + "/" + fileName);
		myLib::readMatrixFile(helpString,
                        tempData);
        tempData -= averages;
        tempData *= 10.;
		myLib::writeMatrixFile(helpString,
                         tempData);
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
    /// changed
    /// -1 to compensate +1 in def::right()
    def::rightFreq = (ui->rightSpinBox->value() - 1) * def::spStep();
	ui->rightHzEdit->setText(nm(def::rightFreq));
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i].second = def::spLength();
    }
}

void Spectre::setLeft()
{
    /// changed
    def::leftFreq = ui->leftSpinBox->value() * def::spStep();
	ui->leftHzEdit->setText(nm(def::leftFreq));
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i].second = def::spLength();
    }
}


void Spectre::writeSpectra(const double leftFreq,
                           const double rightFreq)
{

    QTime myTime;
    myTime.start();

	const QString outDirPath = ui->lineEdit_2->text();
//	const QString outDirPath = "/media/Files/Data/FeedbackTest/GA/SpectraSmooth";

    if(!QDir(outDirPath).exists())
    {
		std::cout << "writeSpectra: outDir doesn't exist" << std::endl;
        return;
    }

	const int left = def::left();
	const int right = def::right();

    QStringList lst = ui->dropChannelsLineEdit->text().split(
                          QRegExp("[,;\\s]"), QString::SkipEmptyParts);
	std::cout << "writeSpectra: num of dropped channels = " << lst.length() << std::endl;

    for(QString str : lst)
    {
        rangeLimits[str.toInt() - 1] = {0, 0}; // to fill with zeros
    }

	std::ofstream outStream;
	QString helpString;
	for(uint i = 0; i < fileNames.size(); ++i)
    {
		helpString = outDirPath + "/" + fileNames[i];
//		std::cout << helpString << std::endl;
        helpString.remove("." + def::plainDataExtension);
        helpString += "." + def::spectraDataExtension;
//		std::cout << helpString << std::endl;

        outStream.open(helpString.toStdString());
        if(!outStream.good())
        {
			std::cout << "bad outStream" << std::endl;
            continue;
        }
		outStream << "NumOfChannels " << def::nsWOM() << '\t';
		outStream << "spLength " << right - left << "\r\n";

		outStream << std::fixed;
		outStream.precision(4);

        if(ui->spectraRadioButton->isChecked())
        {
            /// which channels to write ???
			for(int j = 0; j < def::nsWOM(); ++j) //
			{
                for(int k = left; k < left + rangeLimits[j].first; ++k)
                {
                    outStream << "0.000" << '\t';
                }
                for(int k = left + rangeLimits[j].first;
                    k < left + rangeLimits[j].second; ++k)
                {
					outStream << dataFFT[i][j][k] << '\t';
                }
				for(int k = std::max(left + rangeLimits[j].first,
                                left + rangeLimits[j].second);
                    k < right; ++k)
                {
                    outStream << "0.000" << '\t';
                }
                outStream << "\r\n";
            }
        }
        outStream.close();
    }

	std::cout << "writeSpectra: time elapsed " << myTime.elapsed() / 1000. << " sec" << std::endl;
}
void Spectre::countSpectraSlot()
{
    defaultState();
	if(!ui->bypassCountCheckBox->isChecked())
	{
		countSpectra();
	}
	if(!ui->bypassWriteCheckBox->isChecked())
	{
		writeSpectra();
	}

#if 0
    /// if clean
    cleanSpectra(); // using mann-whitney
#endif

    ui->lineEdit_1->setText(ui->lineEdit_2->text());
	ui->lineEdit_2->setText(def::dirPath() + "/Help");
}

void Spectre::cleanSpectra()
{
    QTime myTime;
    myTime.start();


	std::cout << ui->lineEdit_2->text() << std::endl;
    trivector<int> MW;
	myLib::countMannWhitney(MW,
                     ui->lineEdit_2->text()); // SpectraSmooth
    int num;
    int cnt = 0;
    for(int k = 0; k < def::spLength() * def::nsWOM(); ++k)
    {
//        std::cout << "spPoint = " << k << std::endl;
        num = 0;
        for(int i = 0; i < def::numOfClasses(); ++i)
        {
            for(int j = i + 1; j < def::numOfClasses(); ++j)
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
                sp[k / def::spLength()][k % def::spLength()] = 0.;
            }
            ++cnt;
        }
        num = 0;
    }
	std::cout << "cleanSpectra: num of zeroed points = " << cnt << std::endl;
	ui->lineEdit_2->setText(ui->lineEdit_2->text() + "/Clean");
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

void Spectre::countSpectra()
{
	QTime myTime;
	myTime.start();

	const QString inDirPath = ui->lineEdit_1->text();
	QStringList lst;
	myLib::makeFullFileList(inDirPath,
							lst
						#if SPECTRA_EXP_NAME_SPECIFICITY
							,{def::ExpName}
						#endif
							//					 , {def::ExpName.left(3)}
							//                     ,{def::plainDataExtension}
							);
	const int numFiles = lst.length();

	matrix dataIn;
	QString helpString;

	dataFFT.resize(numFiles);
	for(matrix & datum : dataFFT)
	{
		datum.resize(def::ns, def::fftLength / 2, 0.);
	}

	fileNames.resize(lst.length());
	std::copy(std::begin(lst), std::end(lst), std::begin(fileNames));

	int cnt = 0;
	std::vector<uint> exIndices;
	uint fileNumber = 0;
	std::vector<uint> exFileNumbers;

	for(const QString & fileName : fileNames)
	{
		if(fileName.contains("_num") ||
//		   fileName.contains("_300") ||
		   fileName.contains("_sht")) continue;

//		std::cout << fileName << std::endl;
		// read data file
		helpString = inDirPath + "/" + fileName;
		myLib::readPlainData(helpString,  dataIn);

		if(ui->spectraRadioButton->isChecked())
		{
			if(countOneSpectre(dataIn, dataFFT[cnt]))
			{
				++cnt;
			}
			else
			{
				/// if can't calculate spectre, e.g. too short real/wind
#if ELENA_VARIANT
				QRegExp reg(R"(_[0-9]{1,}_)");
#else
				QRegExp reg(R"([._][0-9]{4}[._])"); /// winds
#endif
				reg.indexIn(fileName);
				exFileNumbers.push_back(reg.cap().remove("_").remove(".").toInt());
				exIndices.push_back(fileNumber);
			}
		}

		ui->progressBar->setValue(++fileNumber * 100. / numFiles);
		qApp->processEvents();
	}

#if ELENA_VARIANT
	if(!exFileNumbers.empty())
	{
		std::cout << "countSpectra: haven't calculated files - " << std::endl;
		std::cout << exFileNumbers << std::endl;
	}
#endif

	smLib::eraseItems(fileNames, exIndices);
	dataFFT.resize(cnt);

	ui->progressBar->setValue(0);

	// generality
	std::cout << "countSpectra: time elapsed " << myTime.elapsed()/1000. << " sec" << std::endl;
}

bool Spectre::countOneSpectre(matrix & data2, matrix & outData)
{	
	outData = myLib::countSpectre(data2, def::fftLength, ui->smoothBox->value());
	if(outData.isEmpty()) return false;
	return true;
}

void Spectre::drawWavelets()
{
    QString helpString;
    QString filePath;
	QDir localDir(def::dirPath());
	helpString = (def::dirPath()
										  + "/visualisation"
										  + "/wavelets");
    localDir.cd(helpString);

	// make dirs
    for(int i = 0; i < def::nsWOM(); ++i)
    {
		localDir.mkdir(nm(i));

        if(ui->phaseWaveletButton->isChecked())
        {
			// for phase
			localDir.cd(nm(i));
            for(int j = 0; j < def::ns; ++j)
            {
				localDir.mkdir(nm(j));
            }
            localDir.cdUp();
        }
    }

    QStringList lst;
	myLib::makeFullFileList(ui->lineEdit_1->text(), lst);

    matrix signal;
    matrix coefs;
	std::ofstream outStr;

	std::set<double, std::greater<double>> tempVec;

	// count maxValue
    for(const QString & fileName : lst)
    {
        filePath = (ui->lineEdit_1->text()
											+ "/" + fileName);
		myLib::readPlainData(filePath, signal);

        for(int chanNum = 0; chanNum < def::nsWOM(); ++chanNum)
        {
            coefs = wvlt::countWavelet(signal[chanNum]);
            tempVec.emplace(coefs.maxVal());
            continue;

			helpString = (def::dirPath()
												  + "/visualisation"
												  + "/wavelets"
												  + "/" + nm(chanNum) + ".txt");

			outStr.open(helpString.toStdString(), std::ios_base::app);
			outStr << coefs.maxVal() << std::endl;
            outStr.close();
        }
    }

//    for(int chanNum = 0; chanNum < def::nsWOM(); ++chanNum)
//    {
//        helpString = (def::dirPath()
//                                              + "/visualisation"
//                                              + "/wavelets"
//                                              + "/" + nm(chanNum) + ".txt");
//        readFileInLine(helpString, tempVec);
//        std::sort(tempVec.begin(), tempVec.end());
//        std::cout << tempVec.front() << "\t" << tempVec.back() << std::endl;
//    }
    return;


    for(auto it = lst.begin(); it != lst.end(); ++it)
    {
        const QString & fileName = (*it);
		filePath = def::dirPath()
				   + "/" + fileName;
        if(ui->amplitudeWaveletButton->isChecked())
        {
            for(int channel = 0; channel < def::nsWOM(); ++channel)
            {
                helpString = fileName;
                helpString.replace('.', '_');
                helpString = (backupDirPath
													  + "/visualisation"
													  + "/wavelets"
													  + "/" + nm(channel)
													  + "/" + helpString
													  + "_wavelet_" + nm(channel)
                                                      + ".jpg");
				std::cout << helpString.toStdString() << std::endl;
                wvlt::wavelet(filePath, helpString, channel, def::ns);
            }
        }
        if(ui->phaseWaveletButton->isChecked())
        {
            for(int channel1 = 0; channel1 < def::nsWOM(); ++channel1)
            {
                for(int channel2 = channel1+1; channel2 < def::nsWOM(); ++channel2)
                {
                    helpString = fileName;
                    helpString.replace('.', '_');
                    helpString = (backupDirPath
														  + "/visualisation"
														  + "/wavelets"
														  + "/" + nm(channel1)
														  + "/" + nm(channel2)
														  + "/" + helpString
														  + "_wavelet_" + nm(channel1)
														  + "_" + nm(channel2)
                                                          + ".jpg");
					std::cout << helpString.toStdString() << std::endl;
                    /// remake waveletPhase
//                    waveletPhase(helpString, file1, ns, channel1, channel2, 20., 5., 0.95, 32);
//                    if(channel2 == 2) return;
                }
            }
        }
        ui->progressBar->setValue((std::distance(lst.begin(), it) + 1)
                                  * 100. / lst.length());
        qApp->processEvents();
    }
    ui->progressBar->setValue(0);
}

