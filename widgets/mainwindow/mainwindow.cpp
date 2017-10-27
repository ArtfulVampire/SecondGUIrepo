#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/drw.h>
#include <myLib/signalProcessing.h>
#include <myLib/mati.h>
#include <myLib/general.h>
#include <myLib/dataHandlers.h>
#include <myLib/qtlib.h>

using namespace myOut;

MainWindow::MainWindow() :
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Main");
    setlocale(LC_NUMERIC, "C");

    autoProcessingFlag = false;
	stdOutBuf = std::cout.rdbuf();
    stopFlag = 0;


//	QButtonGroup * group1;
//	group1 = new QButtonGroup(this);

	QButtonGroup * group2;
	group2 = new QButtonGroup(this);
    group2->addButton(ui->windsButton);
	group2->addButton(ui->realsButton);

	ui->drawCoeffSpinBox->setValue(1.0);
	ui->drawCoeffSpinBox->setSingleStep(0.1);
	ui->sliceCheckBox->setChecked(true);
	ui->progressBar->setValue(0);

	ui->drawDirBox->addItem("Reals");
    ui->drawDirBox->addItem("cut");
	ui->drawDirBox->addItem("winds");
    ui->drawDirBox->addItem("SpectraSmooth");
	ui->drawDirBox->addItem("SpectraSmooth/winds");
	ui->drawDirBox->addItem("winds/SpectraSmooth"); /// tcpClient test
	ui->drawDirBox->addItem("winds/fromreal");

	/// set fileMarks lineEdit
	QString helpString;
    for(const QString & fileMark : def::fileMarkers)
    {
        helpString += fileMark + "; ";
    }
    helpString.resize(helpString.size() - 2); // remove the last "; "
    ui->fileMarkersLineEdit->setText(helpString);

	ui->reduceChannelsComboBox->addItem("EEG,reref,EOG,mark");
	ui->reduceChannelsComboBox->addItem("EEG,reref,mark");
	ui->reduceChannelsComboBox->addItem("EEG,EOG,other,mark");
	ui->reduceChannelsComboBox->addItem("EEG,other,mark");
	ui->reduceChannelsComboBox->addItem("EEG,EOG,mark");
	ui->reduceChannelsComboBox->addItem("EEG,mark");

	ui->reduceChannelsComboBox->setCurrentText("EEG,mark");

	/// in seconds !!!!!
    ui->timeShiftSpinBox->setMinimum(0.1);
    ui->timeShiftSpinBox->setMaximum(32);
    ui->timeShiftSpinBox->setValue(0.5);
    ui->timeShiftSpinBox->setSingleStep(0.1);

    ui->windowLengthSpinBox->setMaximum(32);
    ui->windowLengthSpinBox->setMinimum(1);
    ui->windowLengthSpinBox->setSingleStep(0.1);
    ui->windowLengthSpinBox->setValue(4);
    ui->realsButton->setChecked(true);

    if(def::OssadtchiFlag)
    {
        ui->timeShiftSpinBox->setValue(10);
        ui->windowLengthSpinBox->setValue(10);
	}

	ui->highFreqFilterDoubleSpinBox->setMaximum(500);
	ui->lowFreqFilterDoubleSpinBox->setMaximum(500);

	ui->numOfIcSpinBox->setMaximum(19); // generality
    ui->numOfIcSpinBox->setMinimum(2);
    ui->numOfIcSpinBox->setValue(19);

    ui->numComponentsSpinBox->setMaximum(19);
    ui->numComponentsSpinBox->setValue(10);

    ui->svdDoubleSpinBox->setDecimals(1);
    ui->svdDoubleSpinBox->setMaximum(15);
    ui->svdDoubleSpinBox->setMinimum(2);
    ui->svdDoubleSpinBox->setValue(4.0);
    ui->svdDoubleSpinBox->setValue(9.0);
    ui->svdDoubleSpinBox->setSingleStep(0.5);

    ui->vectwDoubleSpinBox->setDecimals(1);
    ui->vectwDoubleSpinBox->setMaximum(15.0);
    ui->vectwDoubleSpinBox->setMinimum(1.5);
    ui->vectwDoubleSpinBox->setValue(12.0);
    ui->vectwDoubleSpinBox->setSingleStep(0.5);

	ui->cleanRealsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
	ui->cleanWindsCheckBox->setChecked(false);
	ui->cleanWindsSpectraCheckBox->setChecked(true);
	ui->cleanRealsSignalsCheckBox->setChecked(true);
	ui->cleanWindsSignalsCheckBox->setChecked(true);
	ui->cleanFromRealsCheckBox->setChecked(true);
    ui->cleanMarkersCheckBox->setChecked(true);
    ui->cleanSpectraImgCheckBox->setChecked(true);

    ui->highFreqFilterDoubleSpinBox->setValue(40.);
    ui->highFreqFilterDoubleSpinBox->setSingleStep(1.0);
    ui->lowFreqFilterDoubleSpinBox->setValue(3.5);
    ui->lowFreqFilterDoubleSpinBox->setSingleStep(0.1);

    ui->rereferenceDataComboBox->addItem("A1");
    ui->rereferenceDataComboBox->addItem("A2");
    ui->rereferenceDataComboBox->addItem("Ar");
    ui->rereferenceDataComboBox->addItem("N");
	ui->rereferenceDataComboBox->addItem("Base");
    ui->rereferenceDataComboBox->setCurrentText("Ar");

    ui->matiPieceLengthSpinBox->setMaximum(64);
    ui->matiPieceLengthSpinBox->setMinimum(4);
    ui->matiPieceLengthSpinBox->setValue(16);
    ui->matiCheckBox->setChecked(def::matiFlag);
	ui->markerBinTimeSpinBox->setMaximum(250 * 60 * 60 * 2);   // 2 hours
	ui->markerSecTimeDoubleSpinBox->setMaximum(60 * 60 * 2); // 2 hours

#if SHOW_MATI_WIDGETS
	/// mati
	QObject::connect(ui->matiPreprocessingPushButton, SIGNAL(clicked()), this, SLOT(matiPreprocessingSlot()));
	QObject::connect(ui->markerGetPushButton, SIGNAL(clicked()), this, SLOT(markerGetSlot()));
	QObject::connect(ui->markerSetPushButton, SIGNAL(clicked()), this, SLOT(markerSetSlot()));
	QObject::connect(ui->markerBinTimeSpinBox, SIGNAL(valueChanged(int))this, SLOT(markerSetSecTime(int)));

//	QObject::connect(ui->markerDecimalLineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetBinValueSlot()));
	QObject::connect(ui->markerDecimalLineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetBinValueSlot()));
//	QObject::connect(ui->markerBin0LineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetDecValueSlot()));
	QObject::connect(ui->markerBin0LineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetDecValueSlot()));
//	QObject::connect(ui->markerBin1LineEdit, SIGNAL(returnPressed()), this, SLOT(markerSetDecValueSlot()));
	QObject::connect(ui->markerBin1LineEdit, SIGNAL(textChanged(QString)), this, SLOT(markerSetDecValueSlot()));

	globalEdf.setMatiFlag(ui->matiCheckBox->isChecked());
	QObject::connect(ui->markerSaveEdfPushButton, SIGNAL(clicked()), this, SLOT(markerSaveEdf()));
	QObject::connect(ui->matiCheckBox, &QCheckBox::stateChanged,
					 [this](int a) { this->globalEdf.setMatiFlag(a); });
#else
	qtLib::hideLayout(ui->matiGridLayout);
	ui->matiPieceLengthSpinBox->hide();
	ui->matiCheckBox->hide();
	ui->matiPreprocessingPushButton->hide();
	ui->matiPieceLenlabel->hide();
	ui->matiAdjustPiecesCheckBox->hide();
#endif
	qtLib::hideLayout(ui->testDataGridLayout);


	/// open other widgets
	QObject::connect(ui->browseButton, SIGNAL(clicked()),
					 this, SLOT(setEdfFileSlot()));
	QObject::connect(ui->cut_e, SIGNAL(clicked()), this, SLOT(showCut()));
	QObject::connect(ui->netButton, SIGNAL(clicked()), this, SLOT(showNet()));
	QObject::connect(ui->countSpectra, SIGNAL(clicked()), this, SLOT(showCountSpectra()));

	/// small things, ~constant edf
	QObject::connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));
	QObject::connect(ui->fileMarkersLineEdit, SIGNAL(editingFinished()), this, SLOT(setFileMarkers()));

	QObject::connect(ui->cleanDirsButton, SIGNAL(clicked()), this, SLOT(cleanDirs()));
	QObject::connect(ui->cleanDirsCheckAllButton, &QPushButton::clicked,
					 [this](){ this->cleanDirsCheckAllBoxes(true); });
	QObject::connect(ui->cleanDirsUncheckAllButton, &QPushButton::clicked,
					 [this](){ this->cleanDirsCheckAllBoxes(false); });
	QObject::connect(ui->drawButton, SIGNAL(clicked()), this, SLOT(drawDirSlot()));
	QObject::connect(ui->drawMapsPushButton, SIGNAL(clicked()), this, SLOT(drawMapsSlot()));
	QObject::connect(ui->eyesButton, SIGNAL(clicked()), this, SLOT(processEyes()));
	QObject::connect(ui->succPrecleanPushButton, &QPushButton::clicked,
					 [this](){ autos::successiveNetPrecleanWinds(
					this->globalEdf.getDirPath() + "/SpectraSmooth/winds"); });

	/// slice
	QObject::connect(ui->cutEDF, SIGNAL(clicked()), this, SLOT(sliceAll()));

	/// process edf
	QObject::connect(ui->icaPushButton, SIGNAL(clicked()), this, SLOT(ICA()));

	/// edit edf
	QObject::connect(ui->reduceChannelsComboBox, SIGNAL(highlighted(int)),
					 this, SLOT(changeRedNsLine(int)));
	QObject::connect(ui->reduceChannelsComboBox, SIGNAL(currentIndexChanged(int)),
					 this, SLOT(changeRedNsLine(int)));
	QObject::connect(ui->refilterDataPushButton, SIGNAL(clicked()), this, SLOT(refilterDataSlot()));
	QObject::connect(ui->reduceChannelsNewEDFPushButton, SIGNAL(clicked()), this, SLOT(reduceChannelsEDFSlot()));
	QObject::connect(ui->rereferenceDataPushButton, SIGNAL(clicked()), this, SLOT(rereferenceDataSlot()));
	QObject::connect(ui->rereferenceCARPushButton, SIGNAL(clicked()), this, SLOT(rereferenceCARSlot()));

	QObject::connect(ui->cleanEdfFromEyesButton, SIGNAL(clicked()),
					 this, SLOT(cleanEdfFromEyesSlot()));

	QObject::connect(ui->reduceChannesPushButton, SIGNAL(clicked()), this, SLOT(reduceChannelsSlot()));
#if 1
	ui->reduceChannesPushButton->hide();
#endif

    customFunc();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void QWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

void MainWindow::changeRedNsLine(int a)
{
	if(!QFile::exists(ui->filePathLineEdit->text()))
	{
		std::cout << "changeRedNsLine: file doesn't exist" << std::endl;
		return;
	}
	if(globalEdf.isEmpty())
	{
		this->readData();
	}

#if 0
	ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->itemData(a).toString());
#else
	QString str = ui->reduceChannelsComboBox->itemText(a);
	QString outStr;
	bool eeg = str.contains("EEG");
	bool reref = str.contains("reref");
	bool emg = str.contains("EMG");
	bool eog = str.contains("EOG");
	bool mark = str.contains("mark");
	bool oth = str.contains("other");
	for(int i = 0; i < globalEdf.getNs(); ++i)
	{
		const QString & lab = globalEdf.getLabels(i);

		if(lab.contains("EEG"))
		{
			if(
			   !(
				   (eeg && !edfFile::isRerefChannel(lab))
				   || (reref && edfFile::isRerefChannel(lab))
				)
			   )
			{
				continue;
			}
		}
		else if(lab.contains("EMG"))	{ if(!emg)	continue; }
		else if(lab.contains("EOG"))	{ if(!eog)	continue; }
		else if(lab.contains("Marker"))	{ if(!mark)	continue; }
		else /*any other channel*/		{ if(!oth)	continue; }

		outStr += nm(i + 1) + " ";
	}
	ui->reduceChannelsLineEdit->setText(outStr);
#endif
}

void MainWindow::showCountSpectra()
{
    Spectre *sp = new Spectre();
    sp->show();
}

void MainWindow::processEyes()
{
	readData();
	const edfFile & fil = globalEdf;

    std::vector<int> eegs;
    std::vector<int> eogs;
    for(int i = 0; i < fil.getNs(); ++i)
    {
        const auto & labl = fil.getLabels()[i];
        if(labl.contains("EOG"))
        {
            eogs.push_back(i);
        }
        /// what with A1-A2 and A1-N ???
        else if(labl.contains("EEG"))
        {
            if(!labl.contains("A1-A2") &&
               !labl.contains("A2-A1") &&
               !labl.contains("A1-N" ) &&
               !labl.contains("A2-N"))
            {
                eegs.push_back(i);
            }
        }
    }
    /// or (eogs, eegs)
	myLib::eyesProcessingStatic(eogs); // for first 19 eeg channels
}

void MainWindow::showNet()
{
    Net * ANN = new Net();
    ANN->show();
}

void MainWindow::showCut()
{
    Cut *cut_e = new Cut();
    cut_e->show();
}

void MainWindow::setEdfFileSlot()
{
    QString helpString;

	if(def::dir->isRoot())
    {
        helpString = QFileDialog::getOpenFileName((QWidget*)this,
                                                  tr("EDF to open"),
                                                  def::dataFolder,
                                                  tr("EDF files (*.EDF *.edf)"));
    }
    else
    {
        helpString = QFileDialog::getOpenFileName((QWidget*)this,
                                                  tr("EDF to open"),
												  def::dirPath(),
                                                  tr("EDF files (*.EDF *.edf)"));
    }

	if(!helpString.isEmpty()) { setEdfFile(helpString); }
}

void MainWindow::setEdfFile(const QString & filePath)
{
    QString helpString;
    helpString = filePath;

	ui->filePathLineEdit->setText(helpString);

	def::ExpName = myLib::getExpNameLib(filePath);

	helpString.resize(helpString.lastIndexOf("/"));
    def::dir->cd(helpString);

	if(def::redirectStdOutFlag)
    {
		// redirect std::cout to logfile
        if(generalLogStream.is_open())
        {
            generalLogStream.close();
        }
		helpString +=  "/generalLog.txt";

		generalLogStream.open(helpString.toStdString(), std::ios_base::app);
		generalLogStream << std::endl << std::endl << std::endl;

		std::cout.rdbuf(generalLogStream.rdbuf());
    }
    else
    {
		std::cout.rdbuf(stdOutBuf);
    }


	def::dir->mkdir("Reals");
	def::dir->mkdir("Reals/BC");
	def::dir->mkdir("winds");
	def::dir->mkdir("winds/fromreal");
	def::dir->mkdir("SpectraSmooth");
	def::dir->mkdir("SpectraSmooth/winds");

	def::dir->mkdir("Signals");
	def::dir->mkdir("Signals/winds");
	def::dir->mkdir("SpectraImg");

	def::dir->mkdir("Help");
	def::dir->mkdir("Help/maps");
	def::dir->mkdir("Help/psa");
	def::dir->mkdir("Help/ica");
	def::dir->mkdir("Help/wm");
	def::dir->mkdir("Help/PA");
	def::dir->mkdir("Help/wts");
//	def::dir->mkdir("amod");
//	def::dir->mkdir("auxEdfs");

	readData();
	ui->textEdit->append("EDF file read successfull\nns equals to " + nm(globalEdf.getNs()));
}


void MainWindow::readData()
{

    QTime myTime;
    myTime.start();

    QString helpString;
	helpString = ui->filePathLineEdit->text();
    if(!QFile::exists(helpString))
    {
		std::cout << "readData: edf file doent exist\n" << helpString << std::endl;
        return;
    }
	globalEdf.readEdfFile(helpString);

	helpString = "data have been read\nns equals to " + nm(globalEdf.getNs());
    ui->textEdit->append(helpString);

	ui->reduceChannelsComboBox->currentIndexChanged(ui->reduceChannelsComboBox->currentIndex());

    ui->markerSecTimeDoubleSpinBox->setMaximum(globalEdf.getDataLen() / def::freq);
    ui->markerBinTimeSpinBox->setMaximum(globalEdf.getDataLen());
}

void MainWindow::drawDirSlot()
{
    const QString deer = ui->drawDirBox->currentText();
    if(deer.contains("spectr", Qt::CaseInsensitive))
    {
		drawSpectraSlot();
    }
    else
    {
		drawReals();
    }
}


void MainWindow::drawSpectraSlot()
{
	const QString prePath = def::dirPath() + "/" + ui->drawDirBox->currentText();
	const QString outPath = def::dirPath() + "/SpectraImg";
    drawSpectra(prePath, outPath);
}


void MainWindow::drawSpectra(const QString & prePath,
                             const QString & outPath)
{
    QTime myTime;
    myTime.start();

    QStringList lst;
	myLib::makeFullFileList(prePath, lst);
    std::valarray<double> dataS;
    int i = 0;
    QString helpString;
    for(const QString & str : lst)
    {
		helpString = prePath + "/" + str;
		myLib::readFileInLine(helpString,
                       dataS);

		helpString = outPath + "/" + str + ".jpg";
		myLib::drawTemplate(helpString);
		myLib::drawArray(helpString, dataS);

        ui->progressBar->setValue(100 * (++i) / lst.length());
        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = false;
            break;
        }
    }
    ui->progressBar->setValue(0);
	std::cout << "drawSpectra: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void MainWindow::drawReals()
{
    QString helpString;

    QTime myTime;
    myTime.start();
    ui->progressBar->setValue(0);
    matrix dataD;

	QString prePath = def::dirPath() + "/" + ui->drawDirBox->currentText();
//    makeFullFileList(prePath, lst);

	auto a = def::edfFilters + QStringList("*." + def::plainDataExtension);
	QStringList lst = QDir(prePath).entryList(a);

    int redCh = -1;
    int blueCh = -1;
    const edfFile & fil = globalEdf;

	/// magic const, switch
	if(globalEdf.getNs() == 41)
    {
        redCh = 21;
        blueCh = 22;
    }
	else if(globalEdf.getNs() == 22 || globalEdf.getNs() == 21)
    {
        redCh = 19;
        blueCh = 20;
    }
	else if(globalEdf.getNs() == 24)
    {
        redCh = 21;
        blueCh = 22;
    }
	else /// not the best solution
    {
        for(int i = 0; i < fil.getNs(); ++i)
        {
			if(fil.getLabels(i).contains("EOG1")) redCh = i;
			else if(fil.getLabels(i).contains("EOG2")) blueCh = i;
        }
    }

    for(int i = 0; i < lst.length(); ++i)
    {
        if(stopFlag)
        {
            break;
        }
		helpString = prePath + "/" + lst[i];
		myLib::readPlainData(helpString,
					  dataD);

		if(dataD.cols() > 15000)
        {
            continue;
        }

		helpString = myLib::getPicPath(helpString);


		myLib::drw::drawEeg(dataD * ui->drawCoeffSpinBox->value(),
							def::freq,
		{ std::make_pair(blueCh, "blue"), std::make_pair(redCh, "red") }).save(
					helpString, 0, 100);


        ui->progressBar->setValue(100 * (i + 1) / lst.length());
        qApp->processEvents();
    }
    ui->progressBar->setValue(0);

	helpString = "signals are drawn\nns equals to " + nm(globalEdf.getNs());
    ui->textEdit->append(helpString);

    stopFlag = 0;
	std::cout << "drawReals: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
}

void MainWindow::cleanDirsCheckAllBoxes(bool fl)
{
	for(auto ch : qtLib::widgetsOfLayout<QCheckBox>(ui->cleanDirsGrid))
	{
		ch->setChecked(fl);
	}
}

void MainWindow::cleanDirs()
{
    QString helpString;

    if(def::dir->isRoot())
	{
		std::cout << "cleanDirs: def::dir is root, return" << std::endl;
        return;
    }

	for(auto item : qtLib::widgetsOfLayout<QCheckBox>(ui->cleanDirsGrid))
	{
		if(item->isChecked())
		{
			if(item->text() == "markers")
			{ myLib::cleanDir(def::dirPath(), "markers", 0); }
			else
			{ myLib::cleanDir(def::dirPath() + "/" + item->text()); }
		}
	}
	helpString = "dirs cleaned\nns equals to " + nm(globalEdf.getNs());
    ui->textEdit->append(helpString);
}




void MainWindow::drawMapsSlot()
{
    QString helpString = QFileDialog::getOpenFileName(this,
                                                      tr("Choose maps file"),
													  def::dirPath(),
                                                      tr("*.txt"));
    if(helpString.isEmpty())
    {
		helpString = def::dirPath()
					 + "/Help"
					 + "/" + def::ExpName
                     + "_maps.txt";
        helpString.remove("_ica");
    }
    if(!QFile::exists(helpString))
    {
        return;
    }
	myLib::drawMapsICA(helpString,
					   def::dirPath()
					   + "/Help"
					   + "/Maps",
					   def::ExpName);
}


void MainWindow::stop()
{
    stopFlag = 1;
}

void MainWindow::setFileMarkers()
{
    def::fileMarkers = ui->fileMarkersLineEdit->text().split(QRegExp(R"([,;])"),
                                                             QString::SkipEmptyParts);
    ui->textEdit->append(R"(fileMarkers renewed)");
}
