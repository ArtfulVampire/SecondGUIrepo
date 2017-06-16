#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>

using namespace myOut;

Cut::Cut() :
    ui(new Ui::Cut)
{
    ui->setupUi(this);
    this->setWindowTitle("Cut-e");


	ui->subdirComboBox->addItem("");
	ui->subdirComboBox->addItem("Reals");
	ui->subdirComboBox->addItem("winds");
	ui->subdirComboBox->addItem("winds/fromreal"); //generality
	ui->subdirComboBox->setCurrentText(""); /// me

	ui->suffixComboBox->addItem("");
	ui->suffixComboBox->addItem("eeg");
	ui->suffixComboBox->addItem("emg");
	ui->suffixComboBox->addItem("sum");
	ui->suffixComboBox->addItem("new");
	ui->suffixComboBox->setCurrentText("");
//	ui->suffixComboBox->setCurrentText("sum"); /// iitp


	ui->yNormDoubleSpinBox->setDecimals(2);
	ui->yNormDoubleSpinBox->setMaximum(5.);
	ui->yNormDoubleSpinBox->setMinimum(0.01);
	ui->yNormDoubleSpinBox->setValue(1.);
	ui->yNormDoubleSpinBox->setSingleStep(0.05);


    ui->paintStartDoubleSpinBox->setDecimals(1);
    ui->paintStartDoubleSpinBox->setSingleStep(0.1);
	ui->paintStartDoubleSpinBox->setValue(0);

    ui->paintLengthDoubleSpinBox->setDecimals(1);
	ui->paintLengthDoubleSpinBox->setSingleStep(0.2);


    ui->nextButton->setShortcut(tr("d"));
    ui->prevButton->setShortcut(tr("a"));
	ui->saveSubsecPushButton->setShortcut(tr("c"));
    ui->zeroButton->setShortcut(tr("z"));
	ui->saveButton->setShortcut(tr("s"));
    ui->splitButton->setShortcut(tr("x"));
    ui->rewriteButton->setShortcut(tr("r"));
	ui->forwardFrameButton->setShortcut(QKeySequence::Forward);
	ui->backwardFrameButton->setShortcut(QKeySequence::Back);
	ui->forwardFrameButton->setShortcut(tr("e"));
	ui->backwardFrameButton->setShortcut(tr("q"));
	QShortcut * undoShortcut = new QShortcut(QKeySequence(tr("Ctrl+z")), this);
	QShortcut * copyShortcut = new QShortcut(QKeySequence(tr("Ctrl+c")), this);
	QShortcut * pasteShortcut = new QShortcut(QKeySequence(tr("Ctrl+v")), this);
	QShortcut * cutShortcut = new QShortcut(QKeySequence(tr("Ctrl+x")), this);
	ui->setMark1PushButton->setShortcut(tr("1"));
	ui->setMark2PushButton->setShortcut(tr("2"));

	ui->mark1LineEdit->setText("10");
	ui->mark2LineEdit->setText("20");


    ui->picLabel->installEventFilter(this);
	drawSamples();

    ui->scrollArea->setWidget(ui->picLabel);
    ui->scrollArea->installEventFilter(this);

	ui->color1LineEdit->setText("blue");
	ui->color2LineEdit->setText("red");
	ui->color3LineEdit->setText("orange");
	ui->color1SpinBox->setMinimum(-1); ui->color1SpinBox->setValue(-1);
	ui->color2SpinBox->setMinimum(-1); ui->color2SpinBox->setValue(-1);
	ui->color3SpinBox->setMinimum(-1); ui->color3SpinBox->setValue(-1);

	ui->iitpSaveNewNumSpinBox->setMaximum(50);
	ui->iitpSaveNewNumSpinBox->setValue(24);


	ui->valDoubleSpinBox->setMaximum(1000);
	ui->firstDoubleSpinBox->setMaximum(500);
	ui->secondDoubleSpinBox->setMaximum(500);
	ui->valDoubleSpinBox->setMinimum(-1000);
	ui->firstDoubleSpinBox->setMinimum(-500);
	ui->secondDoubleSpinBox->setMinimum(-500);

	ui->findMarkSpinBox->setMaximum(255);
	ui->findMarkSpinBox->setValue(241);


	QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));

	QObject::connect(undoShortcut, SIGNAL(activated()), this, SLOT(undoSlot()));
	QObject::connect(copyShortcut, SIGNAL(activated()), this, SLOT(copySlot()));
	QObject::connect(pasteShortcut, SIGNAL(activated()), this, SLOT(pasteSlot()));
	QObject::connect(cutShortcut, SIGNAL(activated()), this, SLOT(cutSlot()));

	QObject::connect(ui->iitpDisableEcgCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormInvertCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintStartDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(resizeWidget(double)));
	QObject::connect(this, SIGNAL(buttonPressed(char,int)), this, SLOT(mousePressSlot(char,int)));

    QObject::connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));
	QObject::connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
	QObject::connect(ui->saveSubsecPushButton, SIGNAL(clicked()), this, SLOT(saveSubsecSlot()));
	QObject::connect(ui->rewriteButton, SIGNAL(clicked()), this, SLOT(rewrite()));
	QObject::connect(ui->subtractMeansPushButton, &QPushButton::clicked,
					 [this](){ 	for(auto & row : data3) { row -= smLib::mean(row); } paint(); });
	QObject::connect(ui->linearApproxPushButton, SIGNAL(clicked()), this, SLOT(linearApproxSlot()));

	QObject::connect(ui->zeroButton, SIGNAL(clicked()), this, SLOT(zeroSlot()));
	QObject::connect(ui->splitButton, SIGNAL(clicked()), this, SLOT(splitSlot()));
	QObject::connect(ui->zeroFromZeroPushButton, SIGNAL(clicked()), this, SLOT(zeroFromZeroSlot()));
	QObject::connect(ui->splitFromZeroPushButton, SIGNAL(clicked()), this, SLOT(splitFromZeroSlot()));
	QObject::connect(ui->zeroTillEndPushButton, SIGNAL(clicked()), this, SLOT(zeroTillEndSlot()));
	QObject::connect(ui->splitTillEndPushButton, SIGNAL(clicked()), this, SLOT(splitTillEndSlot()));
	QObject::connect(ui->cutPausesPushButton, SIGNAL(clicked()), this, SLOT(cutPausesSlot()));

	QObject::connect(this, SIGNAL(openFile(QString)), ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(this, SIGNAL(openFile(QString)), this, SLOT(createImage(const QString &)));

    QObject::connect(ui->forwardStepButton, SIGNAL(clicked()), this, SLOT(forwardStepSlot()));
    QObject::connect(ui->backwardStepButton, SIGNAL(clicked()), this, SLOT(backwardStepSlot()));
    QObject::connect(ui->forwardFrameButton, SIGNAL(clicked()), this, SLOT(forwardFrameSlot()));
	QObject::connect(ui->backwardFrameButton, SIGNAL(clicked()), this, SLOT(backwardFrameSlot()));
	QObject::connect(ui->findMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(ui->findMarkSpinBox->value()); });
	QObject::connect(ui->findNonzeroMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(-1); });

	QObject::connect(ui->iitpAutoCorrPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoCorrSlot()));
	QObject::connect(ui->iitpAutoJumpPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoJumpSlot()));
	QObject::connect(ui->iitpManualPushButton, SIGNAL(clicked()), this, SLOT(iitpManualSlot()));
	QObject::connect(ui->iitpSaveNewNumPushButton, SIGNAL(clicked()), this, SLOT(saveNewNumSlot()));
	QObject::connect(ui->setMark1PushButton, &QPushButton::clicked,
					 [this]()
	{
		this->setMarker(ui->mark1LineEdit->text().toInt());
	});
	QObject::connect(ui->setMark2PushButton, &QPushButton::clicked,
					 [this]()
	{
		this->setMarker(ui->mark2LineEdit->text().toInt());
	});
	QObject::connect(ui->toLearnThresholdPushButton, SIGNAL(clicked()), this, SLOT(toLearnSetSlot()));
	QObject::connect(ui->nextBadPointPushButton, SIGNAL(clicked()), this, SLOT(nextBadPointSlot()));
	QObject::connect(ui->clearThresholdSetPushButton, &QPushButton::clicked,
					 [this]()
	{
		learnSet.clear();
		windParams.clear();
		thrParams.clear();
	});

	QObject::connect(ui->color1SpinBox, SIGNAL(valueChanged(int)), this, SLOT(color1SpinSlot()));
	QObject::connect(ui->color2SpinBox, SIGNAL(valueChanged(int)), this, SLOT(color2SpinSlot()));
	QObject::connect(ui->color3SpinBox, SIGNAL(valueChanged(int)), this, SLOT(color3SpinSlot()));
	QObject::connect(ui->color1LineEdit, SIGNAL(returnPressed()), this, SLOT(paint()));
	QObject::connect(ui->color2LineEdit, SIGNAL(returnPressed()), this, SLOT(paint()));
	QObject::connect(ui->color3LineEdit, SIGNAL(returnPressed()), this, SLOT(paint()));

	setThrParamsFuncs();


    this->setAttribute(Qt::WA_DeleteOnClose);

}

Cut::~Cut()
{
    delete ui;
}

void zeroData(matrix & inData, int leftLim, int rightLim)
{
	for(int k = 0; k < inData.rows() - 1; ++k) /// don't affect markers
	{
		std::fill(std::begin(inData[k]) + leftLim,
				  std::begin(inData[k]) + rightLim,
				  0.);
	}
}

void Cut::drawSamples()
{
	std::vector<QLabel *> picLabels{
		ui->hz5Label,
				ui->hz10Label,
				ui->hz15Label,
				ui->hz20Label,
				ui->hz25Label,
				ui->hz30Label
	};

	std::valarray<double> seen(150);
	for(int num = 0; num < 6; ++num)
	{
		int freq = 5 * (num + 1);
		myLib::makeSine(seen, freq, 0, -1, 250.);
		QPixmap pic = myLib::drawOneSignal(seen, 50);

		picLabels[num]->setPixmap(pic);
//		picLabels[num]->setPixmap(pic.scaledToHeight(picLabels[num]->height()));
	}
//	ui->hzLayout->setGeometry(QRect(ui->hzLayout->geometry().x(),
//									ui->hzLayout->geometry().y(),
//									ui->hzLayout->sizeHint().width(),
//									ui->hzLayout->sizeHint().height()));
}

void Cut::browse()
{
	QString path;
	if(def::dir->isRoot())
	{
		path = def::dataFolder;
	}
	else
	{
		if(this->myFileType == fileType::edf)
		{
			path = edfFil.getDirPath();
		}
		else if(this->myFileType == fileType::real)
		{
			path = def::dir->absolutePath() +
				   "/" + ui->subdirComboBox->currentText();
		}

	}

	const QString suffix = ui->suffixComboBox->currentText();
	QString filter{};
	for(const QString & in : def::edfFilters)
	{
		filter += (suffix.isEmpty() ? "" :  ("*" + suffix)) + in + " ";
	}
	filter += (suffix.isEmpty() ? "" :  ("*" + suffix)) + "*." + def::plainDataExtension;
	QString helpString = QFileDialog::getOpenFileName((QWidget*)this,
													  tr("Open file"),
													  path,
													  filter);


    if(helpString.isEmpty())
    {
//        QMessageBox::information((QWidget*)this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }
    ui->lineEdit->setText(helpString);

	setFileType(helpString);

	/// set def::dir
	if(def::dir->isRoot())
	{
		def::dir->cd(myLib::getDirPathLib(helpString));
		if(this->myFileType == fileType::edf)
		{
			// do nothing
		}
		else if(this->myFileType == fileType::real)
		{
			def::dir->cdUp();
		}
	}
	lst = QDir(myLib::getDirPathLib(helpString)).entryList(
	{"*" + ui->suffixComboBox->currentText() +  "*." + myLib::getExt(helpString)});

	currentNumber = lst.indexOf(myLib::getFileName(helpString));

    createImage(helpString);
}

void Cut::resizeEvent(QResizeEvent * event)
{
    // adjust scrollArea size
	double newLen = smLib::doubleRound((event->size().width() - 10 * 2) / currFreq,
										  ui->paintLengthDoubleSpinBox->decimals());
    ui->scrollArea->setGeometry(ui->scrollArea->geometry().x(),
                                ui->scrollArea->geometry().y(),
								newLen * currFreq,
                                ui->scrollArea->geometry().height());
	ui->paintLengthDoubleSpinBox->setValue(newLen);
	if(!data3.isEmpty())
	{
		paint();
	}
}

bool Cut::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->picLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) emit buttonPressed('l', mouseEvent->x());
            if(mouseEvent->button() == Qt::RightButton) emit buttonPressed('r', mouseEvent->x());
            return true;
        }
        else
        {
            return false;
        }
    }

    if(obj == ui->scrollArea)
    {
        if(event->type() == QEvent::Wheel)
        {
            QWheelEvent * scrollEvent = static_cast<QWheelEvent*>(event);
            int offset = -0.8 * scrollEvent->delta();

            if(myFileType == fileType::real)
            {
//                std::cout << "real" << std::endl;
                ui->scrollArea->horizontalScrollBar()->setSliderPosition(
                            ui->scrollArea->horizontalScrollBar()->sliderPosition() +
                            offset);
                return true;
            }
            else if(myFileType == fileType::edf)
            {
//                std::cout << "edf" << std::endl;
				if((leftDrawLimit + ui->scrollArea->width() > data3.cols() && offset > 0) ||
                   (leftDrawLimit == 0 && offset < 0))
                {
                    return false;
                }
				leftDrawLimit = std::min(leftDrawLimit + offset, int(data3.cols()));
				ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
                return true;
            }
            else
            {
//                std::cout << "none" << std::endl;
                return false;
            }
        }
		else if(event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent * clickEvent = static_cast<QMouseEvent*>(event);
			if(clickEvent->button() == Qt::BackButton)
			{
				this->backwardFrameSlot();
			}
			else if(clickEvent->button() == Qt::ForwardButton)
			{
				this->forwardFrameSlot();
			}

			return true;
		}
		else if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->key() == Qt::Key_Left)
			{
				ui->leftLimitSpinBox->stepDown();
			}
			else if(keyEvent->key() == Qt::Key_Right)
			{
				ui->leftLimitSpinBox->stepUp();
			}
			showDerivatives();
			paintLimits();
		}

        else
		{
            return false;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void Cut::setFileType(const QString & dataFileName)
{
    if(dataFileName.endsWith(".edf", Qt::CaseInsensitive))
    {
        this->myFileType = fileType::edf;
    }
	else if(dataFileName.endsWith(def::plainDataExtension, Qt::CaseInsensitive))
    {
        this->myFileType = fileType::real;
    }
}


void Cut::resetLimits()
{
	ui->leftLimitSpinBox->setValue(0);
	ui->rightLimitSpinBox->setValue(data3.cols());
}

void Cut::setValuesByEdf()
{
	currFreq = edfFil.getFreq();
	data3 = edfFil.getData(); /// expensive


	ui->leftLimitSpinBox->setMaximum(edfFil.getDataLen());
	ui->rightLimitSpinBox->setMaximum(edfFil.getDataLen());
	resetLimits();

	ui->paintStartDoubleSpinBox->setMaximum(floor(data3.cols() / currFreq));
	ui->paintStartDoubleSpinBox->setValue(0); /// or not needed?
	ui->paintStartLabel->setText("start (max " + nm(floor(data3.cols() / currFreq)) + ")");
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - 20) / currFreq);
	ui->paintLengthDoubleSpinBox->setValue((this->width() - 20) / currFreq);

	/// set coloured channels
	QString redStr = "EOG1";
	QString blueStr = "EOG2";
	if(edfFil.getNs() > 35)
	{
		redStr = "ECG";
		blueStr = "Artefac";
	}

	ui->color1SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color2SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color3SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color1SpinBox->setValue(edfFil.findChannel(blueStr));
	ui->color2SpinBox->setValue(edfFil.findChannel(redStr));
	ui->color3SpinBox->setValue(-1);

	ui->linearApproxSpinBox->setMaximum(edfFil.getNs());
	ui->linearApproxSpinBox->setValue(edfFil.getNs() - 1); // markers
}

void Cut::createImage(const QString & dataFileName)
{
    addNum = 1;
    currentFile = dataFileName;

	leftDrawLimit = 0;
    if(this->myFileType == fileType::real)
	{
		myLib::readPlainData(dataFileName, data3);
    }
	else if(this->myFileType == fileType::edf)
	{
        edfFil.readEdfFile(dataFileName);
		def::ns = edfFil.getNs();
		setValuesByEdf();
    }
	ui->iitpDisableEcgCheckBox->setChecked(false);
	paint();
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(0);
}

void Cut::iitpLog(const QString & typ, int num, const QString & add)
{
	std::ofstream outStr;
	QString name = edfFil.getExpName();
	name = name.left(name.indexOf('_'));
	outStr.open((edfFil.getDirPath() + "/" +
				 name + "_syncLog.txt").toStdString(), std::ios_base::app);
	outStr << edfFil.getExpName() << "\t"
		   << typ << "\t"
		   << ui->leftLimitSpinBox->value() << "\t";
	if(num == 2)
	{
		outStr << ui->rightLimitSpinBox->value() << "\t";
	}
	if(!add.isEmpty())
	{
		outStr << add << "\t";
	}
	outStr << std::endl;
	outStr.close();
}

void Cut::iitpAutoCorrSlot()
{
	edfFile tmpFile = edfFil;
	tmpFile.iitpSyncAutoCorr(ui->rightLimitSpinBox->value(),
							 ui->leftLimitSpinBox->value(),
							 ui->iitpByEegCheckBox->isChecked());
	QString newName = tmpFile.getFileNam();
	newName.replace(".edf", "_sync.edf");
	std::cout << "iitpAutoSlot: newFileName = " << newName << std::endl;
	tmpFile.writeEdfFile(tmpFile.getDirPath() + "/" + newName);
}

void Cut::iitpAutoJumpSlot()
{
	auto lims = edfFil.iitpSyncAutoJump(ui->rightLimitSpinBox->value(),
										ui->leftLimitSpinBox->value(),
										ui->iitpByEegCheckBox->isChecked());

	ui->rightLimitSpinBox->setValue(lims.second);
	ui->leftLimitSpinBox->setValue(lims.first);
	paint();

}

void Cut::iitpManualSlot()
{

	edfFil.iitpSyncManual(ui->rightLimitSpinBox->value(),
						  ui->leftLimitSpinBox->value(),
						  std::min(200, ui->leftLimitSpinBox->value()));
	iitpLog("sync");

	setValuesByEdf();
	paint();
}

void Cut::showDerivatives()
{
	const std::valarray<double> & sig = data3[ui->linearApproxSpinBox->value()];
	const int ind = ui->leftLimitSpinBox->value();
	const int st = 5;
	ui->valDoubleSpinBox->setValue(sig[ind]);
	ui->firstDoubleSpinBox->setValue(sig[ind + st] -  sig[ind - st]);
	ui->secondDoubleSpinBox->setValue(sig[ind + 2 * st] + sig[ind - 2 * st] - 2 * sig[ind]);
}

void Cut::mousePressSlot(char btn, int coord)
{
	if(btn == 'l' &&
	   coord + leftDrawLimit < ui->rightLimitSpinBox->value())
	{
		ui->leftLimitSpinBox->setValue(coord + leftDrawLimit);
		showDerivatives();
	}
	else if(btn == 'r' &&
			coord + leftDrawLimit > ui->leftLimitSpinBox->value() &&
			coord < data3.cols())
	{
		ui->rightLimitSpinBox->setValue(coord + leftDrawLimit);
	}
	paintLimits();
}

void Cut::copySlot()
{
	this->copyData = data3.subCols(ui->leftLimitSpinBox->value(),
								   ui->rightLimitSpinBox->value());
	paint();
}

void Cut::cutSlot()
{
	this->copySlot();
	this->splitSlot();
}

void Cut::paste(int start, const matrix & inData, bool addUndo)
{
	int cls = inData.cols();

	if(addUndo)
	{
		undoAction = [start, cls, this]()
		{
			this->split(start, start + cls, false);
		};
		undos.push_back(undoAction);
	}

	matrix data2 = this->data3.subCols(ui->leftLimitSpinBox->value(), data3.cols());
	data3.resizeCols(ui->leftLimitSpinBox->value());
	data3.horzCat(inData).horzCat(data2);
	paint();
}

void Cut::pasteSlot()
{
	this->paste(ui->leftLimitSpinBox->value(), copyData);
}

void Cut::next()
{

    QString helpString;
    int tmp = currentNumber;
    for(; currentNumber < lst.length() - 1; ++currentNumber)  // generality
    {
        /// remake regexps or not?
        if(lst[currentNumber + 1].contains("_num") ||
           lst[currentNumber + 1].contains("_000") || /// number starts with .000
           lst[currentNumber + 1].contains("_sht"))
        {
            continue;
        }
		helpString = myLib::getDirPathLib(currentFile) + "/" + lst[++currentNumber];
        emit openFile(helpString);
        return;
    }
    currentNumber = tmp;
	std::cout << "next: bad number, too big" << std::endl;
}

void Cut::prev()
{

    QString helpString;
    int tmp = currentNumber;
    for(; currentNumber > 0 + 1; --currentNumber)  // generality
    {
        /// remake regexps or not?
        if(lst[currentNumber - 1].contains("_num") ||
           lst[currentNumber - 1].contains("_000") || /// number starts with .000
           lst[currentNumber - 1].contains("_sht"))
        {
            continue;
        }
		helpString = myLib::getDirPathLib(currentFile) + "/" + lst[--currentNumber];
        emit openFile(helpString);
        return;
    }
    currentNumber = tmp;
	std::cout << "prev: bad number, too little" << std::endl;

}


void Cut::forwardStepSlot()
{
	if(leftDrawLimit + ui->scrollArea->width() > data3.cols())
    {
		std::cout << "end of file" << std::endl;
        return;
    }

	leftDrawLimit = std::min(leftDrawLimit + currFreq, double(data3.cols()));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::backwardStepSlot()
{
    if(leftDrawLimit == 0)
    {
		std::cout << "begin of file" << std::endl;
        return;
    }
	leftDrawLimit = std::max(leftDrawLimit - currFreq, 0.);
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::forwardFrameSlot()
{
	if(leftDrawLimit + ui->scrollArea->width() > data3.cols())
    {
		std::cout << "end of file" << std::endl;
        return;
    }
    leftDrawLimit = std::min(leftDrawLimit +
							 ui->paintLengthDoubleSpinBox->value() * currFreq,
							 double(data3.cols()));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::backwardFrameSlot()
{
    if(leftDrawLimit == 0)
    {
		std::cout << "begin of file" << std::endl;
        return;
    }
    leftDrawLimit = std::max(leftDrawLimit -
							 ui->paintLengthDoubleSpinBox->value() * currFreq, 0.);
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}

void Cut::findNextMark(int mark)
{
	if(myFileType == fileType::edf)
	{
		auto it = std::begin(edfFil.getMarkArr());
		if(mark > 0)
		{
			it = std::find(std::begin(edfFil.getMarkArr()) + leftDrawLimit + 150,
						   std::end(edfFil.getMarkArr()),
						   mark);
		}
		else
		{
			it = std::find_if(std::begin(edfFil.getMarkArr()) + leftDrawLimit + 150,
							  std::end(edfFil.getMarkArr()),
							  [](double in){ return in != 0.; });
		}

		if(it == std::end(edfFil.getMarkArr()))
		{
			std::cout << "findNextMark: marker not found" << std::endl;
			return;
		}

		int index = std::distance(std::begin(edfFil.getMarkArr()), it);
		ui->paintStartDoubleSpinBox->setValue(std::max(0., double(index) / edfFil.getFreq() - 0.5));
		ui->leftLimitSpinBox->setValue(index);
		showDerivatives();
		paint();
	}
}

void Cut::cutPausesSlot()
{
	while(1)
	{
		const auto beg = std::begin(data3[edfFil.getMarkChan()]);
		const auto en = std::end(data3[edfFil.getMarkChan()]);

		auto sta = std::find(beg,
							 en,
							 201);
		auto fin = std::find(sta,
							 en,
							 202);
		if( (sta != en) && (fin != en) )
		{
			this->split(std::distance(beg, sta) - 1, std::distance(beg, fin) + 1, false);
		}
		else
		{
			break;
		}
	}
//	this->saveAs("_noPaus");
	std::cout << "CutPausesSlot: pauses cut" << std::endl;
}


void Cut::resizeWidget(double a)
{
	if(this->size().width() == a * currFreq + 20) return;
	this->resize(a * currFreq + 20, this->height());
}

/// FULL REMAKE with fileType::edf
void Cut::matiAdjustLimits() /////// should TEST !!!!!
{
#if 0
    QStringList lst = currentFile.split(QRegExp("[_.]"),
                                        QString::SkipEmptyParts); // to detect session, type, piece
    int tempNum = 0;
    for(int i = 0; i < lst.length(); ++i)
    {
		if(nm(lst[i].toInt()) == lst[i])
        {
            tempNum = i;
            break;
        }
    }
    int typeNum = lst[tempNum].toInt();
    int sesNum = lst[tempNum + 1].toInt();
	int pieceNum = lst[tempNum + 2].toInt();

	if(typeNum != 0 && typeNum != 2) return; // adjust only for counting or composed activity

    int newLeftLimit = leftLimit;
    int newRightLimit = rightLimit;

	while (!myLib::matiCountBit(data3[def::ns - 1][newLeftLimit], 14)
		   && newLeftLimit > 0)
    {
        --newLeftLimit;
    }
	while (!myLib::matiCountBit(data3[def::ns - 1][newRightLimit], 14)
		   && newRightLimit < data3.cols())
    {
		++newRightLimit; // maximum of Num Of Slices
    }


	std::cout << "newLeftLimit = " << newLeftLimit << std::endl;
	std::cout << "newRightLimit = " << newRightLimit << std::endl;

    // adjust limits if slice by whole count problems
    ++newLeftLimit;
	if(newRightLimit != data3.cols()) ++newRightLimit;

    leftLimit = newLeftLimit;
    rightLimit = newRightLimit;
    return;



    // adjust limits if slice by N seconds
	if(newLeftLimit > 0 || myLib::matiCountBit(data3[def::ns - 1][0], 14))
    {
        ++newLeftLimit; // after the previous marker
    }
    else // if(newLeftLimit == 0)
    {
        // cut end in previous file, suspect that there ARE count answers
        if(pieceNum != 0) // if this piece is not the first in the session
        {
//            std::cout << "zero prev file" << std::endl;
            prev();
            leftLimit = rightLimit;
			while (!myLib::matiCountBit(data3[def::ns - 1][leftLimit], 14)) // there ARE count answers
            {
                --leftLimit;
            }
            ++leftLimit;
			std::cout << "prev file leftLimit = " << leftLimit << std::endl;
            // zero() from tempLimit to rightLimit
            zeroData(data3, leftLimit, rightLimit);
            rewrite();
            next();
        }
    }


	if(newRightLimit < data3.cols())
    {
        ++newRightLimit; // after the previous marker
    }
	else if (myLib::matiCountBit(data3[def::ns - 1][data3.cols() - 1], 14))
    {
        //do nothing
    }
	else // if(newRightLimit == data3.cols() && bit == 0)
    {
        // cut start in next file, suspect that there ARE count answers
        next();
        lst = currentFile.split(QRegExp("[_.]"),
                                QString::SkipEmptyParts);
		if (lst[tempNum + 1].toInt() == sesNum
			&& lst[tempNum].toInt() == typeNum
			&& lst[tempNum + 2].toInt() == pieceNum + 1) // if next piece is ok
        {
            rightLimit = leftLimit;
			while (!myLib::matiCountBit(data3[def::ns - 1][rightLimit], 14)) // there ARE count answers
            {
                ++rightLimit;
            }
            ++rightLimit;
			std::cout << "next file rightLimit = " << rightLimit << std::endl;

			zeroData(data3, leftLimit, rightLimit);
            rewrite();
        }
        prev();
    }
    leftLimit = newLeftLimit;
    rightLimit = newRightLimit;
#endif

}


void Cut::undoSlot()
{
	if(undos.empty())
	{
		std::cout << "Cut::undoSlot: undos empty" << std::endl;
		return;
	}
	undos.back()();
	undos.pop_back();
    paint();
}



void Cut::setMarker(int inVal)
{
	if(myFileType == fileType::edf)
	{
		int num = edfFil.getMarkChan();
		if(num <= 0)
		{
			std::cout << "Cut::set1MarkSlot: haven't found markers channel" << std::endl;
			return;
		}
		int offset = ui->leftLimitSpinBox->value();
		int val = data3[num][offset];
		undoAction = [num, offset, val, this](){this->data3[num][offset] = val; };
		undos.push_back(undoAction);

		data3[num][offset] = inVal;
	}
	else if(myFileType == fileType::real)
	{
		int num = data3.rows() - 1; /// last channel
		int offset = ui->leftLimitSpinBox->value();
		int val = data3[num][offset];
		undoAction = [num, offset, val, this](){data3[num][offset] = val; };
		undos.push_back(undoAction);

		data3[num][offset] = inVal;
	}
	paint();
}

void Cut::toLearnSetSlot()
{

	matrix sub = data3.subCols(ui->leftLimitSpinBox->value(),
							   ui->rightLimitSpinBox->value());

	for(int i = 0; i < sub.cols() / paramsWindLen; ++i)
	{
		learnSet.push_back(sub.subCols(paramsWindLen * i, paramsWindLen * (i + 1)));
	}

	/// count params
	if(learnSet.size() >= 50)
	{
		thrParams.resize(paramsChanNum);
		for(int ch = 0; ch < paramsChanNum; ++ch)
		{
			thrParams[ch].resize(paramFuncs.size());

			for(int numFunc = 0; numFunc < paramFuncs.size(); ++numFunc)
			{
				thrParam param;


				std::valarray<double> vals(learnSet.size());
				for(int windNum = 0; windNum < learnSet.size(); ++windNum)
				{
					vals[windNum] = paramFuncs[numFunc] (learnSet[windNum][ch]);
				}

//				param.name = paramNames[numFunc];
				param.numParam = numFunc;
				param.mean = smLib::mean(vals);
				param.sigma = (smLib::sigma(vals) != 0) ? smLib::sigma(vals) : 1.;

				param.numChan = ch;
				thrParams[ch][numFunc] = param;
			}
		}

		std::cout << "toLearnSetSlot: learning done" << std::endl;
		countThrParams();
	}
	else
	{
		std::cout << "toLearnSetSlot: learning set size = " << learnSet.size() << std::endl;
	}
	resetLimits();
	paint();
}

void Cut::countThrParams()
{
	/// should have placed windNum as last index ?
	windParams.resize(data3.cols() / paramsWindLen);
	for(int windNum = 0; windNum < windParams.size(); ++windNum)
	{
		matrix localData = data3.subCols(paramsWindLen * windNum,
										 paramsWindLen * (windNum + 1));
		windParams[windNum].resize(paramsChanNum);
		for(int ch = 0; ch < paramsChanNum; ++ch)
		{
			windParams[windNum][ch].resize(paramFuncs.size());
			for(int numFunc = 0; numFunc < paramFuncs.size(); ++numFunc)
			{
				windParams[windNum][ch][numFunc] = paramFuncs[numFunc] (localData[ch]);
			}
		}
	}

	/// cout max values
	for(int numFunc = 0; numFunc < paramFuncs.size(); ++numFunc)
	{
		double maxVal = 0.;
		int maxCh = -1;
//		int maxWind = -1;
		for(int ch = 0; ch < paramsChanNum; ++ch)
		{
			for(int windNum = 0; windNum < windParams.size(); ++windNum)
			{
				if(windParams[windNum][ch][numFunc] > maxVal)
				{
					maxVal = windParams[windNum][ch][numFunc];
					maxCh = ch;
//					maxWind = windNum;
				}
			}
		}
		std::cout << "max " << paramNames[numFunc] << " = " << maxVal << "\t"
				  << "ch = " << maxCh << std::endl;
	}

	std::cout << "countThrParams: done" << std::endl;
}

void Cut::setThrParamsFuncs()
{
	static const double norm = myLib::spectreNorm(smLib::fftL(paramsWindLen),
												  paramsWindLen,
												  edfFil.getFreq());
	/// alpha
	if(1)
	{
		paramFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, paramsWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(8, edfFil.getFreq(), paramsWindLen),
								   std::begin(spec) + fftLimit(13, edfFil.getFreq(), paramsWindLen),
								   0.);
		});
		paramNames.push_back("alpha");
		paramSigmaThreshold.push_back(10); /// check and adjust
	}

	/// theta
	if(1)
	{
		paramFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, paramsWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(4, edfFil.getFreq(), paramsWindLen),
								   std::begin(spec) + fftLimit(8, edfFil.getFreq(), paramsWindLen),
								   0.);
		});
		paramNames.push_back("theta");
		paramSigmaThreshold.push_back(15); /// check and adjust
	}

	/// beta
	if(1)
	{
		paramFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, paramsWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(13, edfFil.getFreq(), paramsWindLen),
								   std::begin(spec) + fftLimit(25, edfFil.getFreq(), paramsWindLen),
								   0.);
		});
		paramNames.push_back("beta");
		paramSigmaThreshold.push_back(10); /// check and adjust
	}

	/// gamma
	if(0)
	{
		paramFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, paramsWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(25, edfFil.getFreq(), paramsWindLen),
								   std::begin(spec) + fftLimit(40, edfFil.getFreq(), paramsWindLen),
								   0.);
		});
		paramNames.push_back("gamma");
		paramSigmaThreshold.push_back(15); /// check and adjust
	}

	/// max ampl
	if(1)
	{
		paramFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			return std::max(std::abs(in.max()), std::abs(in.min()));
		});
		paramNames.push_back("maxAmpl");
		paramSigmaThreshold.push_back(10); /// check and adjust
	}

	/// integral
	if(1)
	{
		paramFuncs.push_back([this](const std::valarray<double> & in) -> double
		{
			std::valarray<double> spec = myLib::spectreRtoR(in, paramsWindLen) * norm;
			return std::accumulate(std::begin(spec) + fftLimit(4, edfFil.getFreq(), paramsWindLen),
								   std::begin(spec) + fftLimit(40, edfFil.getFreq(), paramsWindLen),
								   0.);
		});
		paramNames.push_back("integral");
		paramSigmaThreshold.push_back(5); /// check and adjust
	}

}

void Cut::nextBadPointSlot()
{
	if(learnSet.empty() || thrParams.empty() || windParams.empty())
	{
		std::cout << "nextBadPointSlot: data not ready" << std::endl;
		return;
	}
	int windNum = leftDrawLimit / paramsWindLen + 1; /// check
	bool proceed = true;
	while(proceed)
	{
//		std::cout << "windNum = " << windNum << std::endl;
		for(int ch = 0; ch < paramsChanNum; ++ch)
		{
//			std::cout << "ch = " << ch << std::endl;
			for(int numFunc = 0; numFunc < paramFuncs.size(); ++numFunc)
			{
//				std::cout << "numFunc = " << numFunc << std::endl;
//				std::cout << "mean = " << thrParams[ch][numFunc].mean << std::endl;
//				std::cout << "sigma = " << thrParams[ch][numFunc].sigma << std::endl;
//				std::cout << "val = " << windParams[windNum][ch][numFunc] << std::endl;
				double numSigmas = std::abs(windParams[windNum][ch][numFunc]
											- thrParams[ch][numFunc].mean)
								   /  thrParams[ch][numFunc].sigma;

				if(numSigmas > paramSigmaThreshold[numFunc]) /// add spin box
				{
					std::cout << "nextBad: param = " << paramNames[numFunc] << " "
							  << "chan = " << ch << " "
//							  << "windNum = " << windNum << " "
							  << "numSigmas = " << numSigmas << " "
							  << std::endl;
					proceed = false;
					break; /// from while(proceed)
				}
			}
		}
		if(proceed) ++windNum;

		if(windNum == data3.cols() / paramsWindLen - 1)
		{
			std::cout << "nextBadPointSlot: end of file" << std::endl;
			return;
		}
	}

	if(!proceed) std::cout << std::endl;

	resetLimits();
	ui->paintStartDoubleSpinBox->setValue(windNum * paramsWindLen / double(edfFil.getFreq()) - 0.5);
	ui->leftLimitSpinBox->setValue(windNum * paramsWindLen);
	paint();
}

void Cut::color1SpinSlot()
{
	const int n = ui->color1SpinBox->value();
	if(n >=0)
	{
		if(myFileType == fileType::edf)
		{
			QString ch = QString(edfFil.getLabels()[n]);
			ui->colorChan1LineEdit->setText(ch.remove("EEG ").remove("IT ").remove("EOG "));
		}
	}
	else
	{
		ui->colorChan1LineEdit->clear();
	}
	paint();
}

void Cut::color2SpinSlot()
{
	const int n = ui->color2SpinBox->value();
	if(n >=0)
	{
		if(myFileType == fileType::edf)
		{
			QString ch = QString(edfFil.getLabels()[n]);
			ui->colorChan2LineEdit->setText(ch.remove("EEG ").remove("IT ").remove("EOG "));
		}
	}
	else
	{
		ui->colorChan2LineEdit->clear();
	}
	paint();
}

void Cut::color3SpinSlot()
{
	const int n = ui->color3SpinBox->value();
	if(n >=0)
	{
		if(myFileType == fileType::edf)
		{
			QString ch = QString(edfFil.getLabels()[n]);
			ui->colorChan3LineEdit->setText(ch.remove("EEG ").remove("IT ").remove("EOG "));
		}
	}
	else
	{
		ui->colorChan3LineEdit->clear();
	}
	paint();
}

void Cut::zero(int start, int end)
{
	if(start > end)
	{
		std::cout << "Cut::split: leftEdge > rightEdge" << std::endl;
		return;
	}

	// if MATI with counting - adjust limits to small task edges
	// move leftLimit on the nearest prev marker
	// move rightLimit on the nearest next marker
	// delete [leftLimit, rightLimit)

//	this->zero(ui->leftLimitSpinBox->value(), ui->rightLimitSpinBox->value());
	/// MATI counting problem only
	{
		// ExpName.left(3)_fileSuffix_TYPE_SESSION_PIECE.MARKER
		QString helpString = "_0_[0-9]_[0-9]{2,2}";
		if(currentFile.contains(QRegExp(helpString)))
		{
//			std::cout << "zero: adjust limits   " << currentFile << std::endl;
//			matiAdjustLimits();
		}
	}

	undoData.push_back(data3.subCols(start, end));
	undoAction = [start, this]()
	{
		for(int k = 0; k < data3.rows() - 1; ++k) /// don't affect markers
		{
			std::copy(std::begin(undoData.back()[k]),
					  std::end(undoData.back()[k]),
					  std::begin(data3[k]) + start);
		}
		undoData.pop_back();
	};
	undos.push_back(undoAction);


	zeroData(data3,
			 start,
			 end);
	resetLimits();
	paint();

}


void Cut::zeroSlot()
{
	this->zero(ui->leftLimitSpinBox->value(), ui->rightLimitSpinBox->value());
}

void Cut::zeroFromZeroSlot()
{
	this->zero(0, ui->rightLimitSpinBox->value());
}

void Cut::zeroTillEndSlot()
{
	this->zero(ui->leftLimitSpinBox->value(), data3.cols());
}


/// DANGER markers
void Cut::split(int start, int end, bool addUndo)
{
	if(start > end)
	{
		std::cout << "Cut::split: leftEdge > rightEdge" << std::endl;
		return;
	}
	if(std::find_if(std::begin(data3.back()),
					std::end(data3.back()),
					[](double in){ return in != 0.; }) != std::end(data3.back()))
	{
		std::cout << "Cut::split: there are non-zero markers" << std::endl;
	}
	if(addUndo)
	{
		undoData.push_back(data3.subCols(start, end));
		undoAction = [start, this]()
		{
			this->paste(start, undoData.back(), false);
			undoData.pop_back();
		};
		undos.push_back(undoAction);
	}

	matrix data2 = data3.subCols(end, data3.cols());
	data3.resizeCols(start).horzCat(data2); /// +1 to save first marker in reals
	ui->paintStartLabel->setText("start (max " + nm(floor(data3.cols() / currFreq)) + ")");

	resetLimits();
	ui->paintStartDoubleSpinBox->setValue(start / edfFil.getFreq() - 1.5);
	ui->leftLimitSpinBox->setValue(start);
	paint();
}

void Cut::splitSlot()
{
	this->split(ui->leftLimitSpinBox->value(),
				ui->rightLimitSpinBox->value());
}

void Cut::splitFromZeroSlot()
{
	iitpLog("split0");

	this->split(0, ui->rightLimitSpinBox->value());
	ui->paintStartDoubleSpinBox->setValue(0.);
}

void Cut::splitTillEndSlot()
{
	iitpLog("splitE");
	data3.resizeCols(ui->leftLimitSpinBox->value());
	ui->paintStartLabel->setText("start (max " + nm(floor(data3.cols() / currFreq)) + ")");
	paint();
}

void Cut::linearApproxSlot()
{
	const int lef = ui->leftLimitSpinBox->value();
	const int rig = ui->rightLimitSpinBox->value();

	std::vector<int> chanList;
	for(int i = 0; i < data3.rows(); ++i)
	{
		if(ui->linearApproxAllEegCheckBox->isChecked()
		   && edfFil.getLabels()[i].startsWith("EEG "))
		{
			chanList.push_back(i);
		}
		else if(ui->linearApproxAllGoniosCheckBox->isChecked()
		   && (edfFil.getLabels()[i].contains("_l") || edfFil.getLabels()[i].contains("_r")))
		{
			chanList.push_back(i);
		}
	}
	if(chanList.empty() && ui->color3SpinBox->value() > 0)
	{
		chanList.push_back(ui->color3SpinBox->value());
	}
	for(int ch : chanList)
	{
		const double coeff = (data3[ch][rig] - data3[ch][lef]) / (rig - lef);
		for(int i = lef + 1; i < rig; ++i)
		{
			data3[ch][i] = data3[ch][lef] + coeff * (i - lef);
		}
	}
	paint();
}

void Cut::saveAs(const QString & addToName)
{
	QString newPath = currentFile;
	newPath.insert(newPath.lastIndexOf('.'), addToName);
	edfFil.writeOtherData(data3, newPath);
}

void Cut::save()
{
    if(myFileType == fileType::real)
    {
        QString helpString = def::dir->absolutePath()
							 + "/cut"
							 + "/" + myLib::getFileName(currentFile);

        // new
		myLib::writePlainData(helpString, data3);
    }
    else if(myFileType == fileType::edf)
    {
		this->saveAs("_new");
		std::cout << "Cut::save: edfFile saved" << std::endl;
    }

}

void Cut::saveNewNumSlot()
{
	QString newName = edfFil.getFileNam();
	newName.replace(QRegExp("[0-9]{2}"), rn(ui->iitpSaveNewNumSpinBox->value(), 2));

	edfFil.writeOtherData(data3.subCols(ui->leftLimitSpinBox->value(),
										ui->rightLimitSpinBox->value()),
						  edfFil.getDirPath() + "/" + newName);

	std::cout << "Cut::saveNewNumSlot: edfFile saved - " << newName << std::endl;
	iitpLog("savNewN", 2, newName);

}

void Cut::saveSubsecSlot()
{
	if(myFileType == fileType::real || 1) /// write plain windows (eyes)
	{

		QString helpString;
		helpString = def::dir->absolutePath() +
					 "/winds" +
					 "/" + myLib::getFileName(currentFile) +
					 "." + rn(addNum++, 3);
		myLib::writePlainData(helpString,
							  data3.subCols(ui->leftLimitSpinBox->value(),
											ui->rightLimitSpinBox->value()));
	}
	else if(myFileType == fileType::edf)
	{
		QString newPath = currentFile;
		QString ad = ui->saveSubsecAddNameLineEdit->text();
		if( !ad.isEmpty())
		{
			newPath.insert(newPath.lastIndexOf('.'), "_" + ad);
		}

		int counter = 0;
		while(QFile::exists(newPath))
		{
			newPath = currentFile;
			newPath.insert(newPath.lastIndexOf('.'), "_" + nm(counter));
			++counter;
		}

		edfFil.writeOtherData(data3.subCols(ui->leftLimitSpinBox->value(),
											ui->rightLimitSpinBox->value()), newPath);

		iitpLog("saveSub", 2, newPath);

		std::cout << "Cut::saveSubsecSlot: edfFile saved - " << newPath << std::endl;
	}
	resetLimits();
	paint();
}


void Cut::rewrite()
{
    if(myFileType == fileType::real)
    {
		myLib::writePlainData(currentFile, data3);
		currentPic.save(myLib::getPicPath(currentFile), 0, 100);
    }
    else if(myFileType == fileType::edf)
	{
		std::cout << "Cut::rewrite: deliberately forbidden for edfs, use Save instead" << std::endl;
    }
}


std::vector<std::pair<int, QColor>> Cut::makeColouredChans()
{
	std::vector<std::pair<int, QColor>> res;
	if(ui->color1SpinBox->value() >= 0)
	{
		res.push_back(std::make_pair(ui->color1SpinBox->value(),
									 QColor(ui->color1LineEdit->text())));
	}
	if(ui->color2SpinBox->value() >= 0)
	{
		res.push_back(std::make_pair(ui->color2SpinBox->value(),
									 QColor(ui->color2LineEdit->text())));
	}
	if(ui->color3SpinBox->value() >= 0)
	{
		res.push_back(std::make_pair(ui->color3SpinBox->value(),
									 QColor(ui->color3LineEdit->text())));
	}
	return res;
}

void Cut::paint() // save to tmp.jpg and display
{
    QString helpString;
	helpString = def::dir->absolutePath() + "/tmp.jpg";

	int rightDrawLimit = 0.;
    if(myFileType == fileType::edf)
    {
		leftDrawLimit = ui->paintStartDoubleSpinBox->value() * currFreq;
		rightDrawLimit = std::min(leftDrawLimit + ui->scrollArea->width(), int(data3.cols()));
    }
    else if(myFileType == fileType::real)
    {
        leftDrawLimit = 0;
		rightDrawLimit = data3.cols();
    }
	matrix subData = data3.subCols(leftDrawLimit, rightDrawLimit);


	int ecg = edfFil.findChannel("ECG");
	if(ui->iitpDisableEcgCheckBox->isChecked() && ecg >= 0)
	{
		subData[ecg] = 0; /// for iitp ecg
	}

	double coeff = ui->yNormDoubleSpinBox->value()
				   * ((ui->yNormInvertCheckBox->isChecked())? -1 : 1);
	auto colouredChans = this->makeColouredChans();

	currentPic = myLib::drw::drawEeg(subData * coeff,
									 currFreq,
									 colouredChans);


	/// draw markers numbers
	if(1)
	{
		const int mrk = edfFil.getMarkChan();
//		std::cout << mrk << std::endl;
		if(mrk > 0)
		{
			QPainter pnt;
			pnt.begin(&currentPic);
			pnt.setFont(QFont("", 16));

			for(int i = 0; i < subData.cols(); ++i)
			{
				if(subData[mrk][i] != 0.)
				{
					pnt.drawText(i,
								 pnt.device()->height() * (mrk + 1) / (subData.rows() + 2) - 3,
								 nm(int(subData[mrk][i])));
				}
			}
			pnt.end();
		}
	}

	paintLimits();

    /// -20 for scroll bar generality
	/// experimental xNorm
//	ui->picLabel->setPixmap(currentPic.scaled(currentPic.width() - 2,
//											  ui->scrollArea->height() - 20));

//    rightLimit = rightDrawLimit - leftDrawLimit;
//    leftLimit = 0;
}

void Cut::paintLimits()
{
	QPixmap tempPic = currentPic;
	QPainter paint;
	paint.begin(&tempPic);

	int leftX = ui->leftLimitSpinBox->value() - leftDrawLimit;
	if(leftX >= 0 && leftX < ui->paintLengthDoubleSpinBox->value() * this->currFreq)
	{
		paint.setPen(QPen(QBrush("blue"), 2));
		paint.drawLine(leftX, 0, leftX, currentPic.height());
	}

	int rightX = ui->rightLimitSpinBox->value() - leftDrawLimit;
	if(rightX >= 0 && rightX < ui->paintLengthDoubleSpinBox->value() * this->currFreq)
	{
		paint.setPen(QPen(QBrush("red"), 2));
		paint.drawLine(rightX, 0, rightX, currentPic.height());
	}

	ui->picLabel->setPixmap(tempPic.scaled(currentPic.width() - 2,
											  ui->scrollArea->height() - 20));
	paint.end();
}

