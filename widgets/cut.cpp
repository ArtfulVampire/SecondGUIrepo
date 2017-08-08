#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/qtlib.h>

using namespace myOut;

Cut::Cut() :
    ui(new Ui::Cut)
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle("Cut-e");

	ui->subdirComboBox->addItem("");
	ui->subdirComboBox->addItem("Reals");
	ui->subdirComboBox->addItem("winds");
	ui->subdirComboBox->addItem("winds/fromreal"); // generality
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


	ui->setMarkLeftSpinBox->setMaximum(255);
	ui->setMarkRightSpinBox->setMaximum(255);
	ui->setMarkLeftSpinBox->setValue(241);
	ui->setMarkRightSpinBox->setValue(0);

	drawSamples();

    ui->scrollArea->setWidget(ui->picLabel);
    ui->scrollArea->installEventFilter(this);

	ui->color1LineEdit->setText("blue");
	ui->color2LineEdit->setText("red");
	ui->color3LineEdit->setText("orange");
	for(auto * colSpin : {ui->color1SpinBox, ui->color2SpinBox, ui->color3SpinBox})
	{
		colSpin->setMinimum(-1); colSpin->setMaximum(24); colSpin->setValue(-1);
	}

	ui->iitpSaveNewNumSpinBox->setMaximum(50);
	ui->iitpSaveNewNumSpinBox->setValue(24);


	// derivativesGridLayout
	ui->derivVal1SpinBox->setMaximum(1000);		ui->derivVal1SpinBox->setMinimum(-1000);
	ui->derivVal2SpinBox->setMaximum(1000);		ui->derivVal2SpinBox->setMinimum(-1000);
	ui->derivFirst1SpinBox->setMaximum(500);	ui->derivFirst1SpinBox->setMinimum(-500);
	ui->derivFirst2SpinBox->setMaximum(500);	ui->derivFirst2SpinBox->setMinimum(-500);
	ui->derivSecond1SpinBox->setMaximum(500);	ui->derivSecond1SpinBox->setMinimum(-500);
	ui->derivSecond2SpinBox->setMaximum(500);	ui->derivSecond2SpinBox->setMinimum(-500);

	ui->findNextMarkSpinBox->setMaximum(255);
	ui->findNextMarkSpinBox->setValue(241);
	ui->findPrevMarkSpinBox->setMaximum(255);
	ui->findPrevMarkSpinBox->setValue(241);

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
//	ui->setMarkLeftPushButton->setShortcut(tr("1"));
//	ui->setMarkRightPushButton->setShortcut(tr("2"));


	QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseSlot()));

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
					 [this](){ 	for(auto & row : dataCutLocal) { row -= smLib::mean(row); } paint(); });
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
	QObject::connect(ui->findNextMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(ui->findNextMarkSpinBox->value()); });
	QObject::connect(ui->findNextNonzeroMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(-1); });
	QObject::connect(ui->findPrevMarkPushButton, &QPushButton::clicked,
					 [this](){ findPrevMark(ui->findPrevMarkSpinBox->value()); });
	QObject::connect(ui->findPrevNonzeroMarkPushButton, &QPushButton::clicked,
					 [this](){ findPrevMark(-1); });

	QObject::connect(ui->iitpAutoCorrPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoCorrSlot()));
	QObject::connect(ui->iitpAutoJumpPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoJumpSlot()));
	QObject::connect(ui->iitpManualPushButton, SIGNAL(clicked()), this, SLOT(iitpManualSlot()));
	QObject::connect(ui->iitpSaveNewNumPushButton, SIGNAL(clicked()), this, SLOT(saveNewNumSlot()));
	QObject::connect(ui->setMarkLeftPushButton, &QPushButton::clicked,
					 [this]()
	{
		this->setMarker(ui->setMarkLeftSpinBox->value(), true);
	});
	QObject::connect(ui->setMarkRightPushButton, &QPushButton::clicked,
					 [this]()
	{
		this->setMarker(ui->setMarkRightSpinBox->value(), false);
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

	QObject::connect(ui->leftLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));

	QObject::connect(ui->rightLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));

	QObject::connect(ui->color1SpinBox, SIGNAL(valueChanged(int)), this, SLOT(color1SpinSlot()));
	QObject::connect(ui->color2SpinBox, SIGNAL(valueChanged(int)), this, SLOT(color2SpinSlot()));
	QObject::connect(ui->color3SpinBox, SIGNAL(valueChanged(int)), this, SLOT(color3SpinSlot()));
	QObject::connect(ui->color1LineEdit, SIGNAL(returnPressed()), this, SLOT(paint()));
	QObject::connect(ui->color2LineEdit, SIGNAL(returnPressed()), this, SLOT(paint()));
	QObject::connect(ui->color3LineEdit, SIGNAL(returnPressed()), this, SLOT(paint()));
	ui->picLabel->installEventFilter(this);

	setThrParamsFuncs();
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

void Cut::timesAndDiffSlot()
{
	ui->rightTimeSpinBox->setValue(ui->rightLimitSpinBox->value() / currFreq);
	ui->leftTimeSpinBox->setValue(ui->leftLimitSpinBox->value() / currFreq);
	ui->diffLimitSpinBox->setValue(ui->rightLimitSpinBox->value() -
								   ui->leftLimitSpinBox->value());
	ui->diffTimeSpinBox->setValue(ui->diffLimitSpinBox->value() / currFreq);
	showDerivatives();
	paintLimits();
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
		seen = myLib::makeSine(150, freq);
		QPixmap pic = myLib::drawOneSignal(seen, 50);

		picLabels[num]->setPixmap(pic);
//		picLabels[num]->setPixmap(pic.scaledToHeight(picLabels[num]->height()));
	}
//	ui->hzLayout->setGeometry(QRect(ui->hzLayout->geometry().x(),
//									ui->hzLayout->geometry().y(),
//									ui->hzLayout->sizeHint().width(),
//									ui->hzLayout->sizeHint().height()));
}

void Cut::browseSlot()
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

	const QString helpString = QFileDialog::getOpenFileName((QWidget*)this,
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
	filesList = QDir(myLib::getDirPathLib(helpString)).entryList(
	{"*" + ui->suffixComboBox->currentText() +  "*." + myLib::getExt(helpString)});

	fileListIter = std::find(std::begin(filesList),
							 std::end(filesList),
							 myLib::getFileName(helpString));


	undoData.clear();
	undos.clear();
	copyData.clear();

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
	if(!dataCutLocal.isEmpty())
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
				if((leftDrawLimit + ui->scrollArea->width() > dataCutLocal.cols() && offset > 0) ||
                   (leftDrawLimit == 0 && offset < 0))
                {
                    return false;
                }
				leftDrawLimit = std::min(leftDrawLimit + offset, int(dataCutLocal.cols()));
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

			if((keyEvent->key() == Qt::Key_Left) || (keyEvent->key() == Qt::Key_Right))
			{
				auto * targetSpin = ui->leftLimitSpinBox;
				if(keyEvent->modifiers().testFlag(Qt::ControlModifier))
				{
					targetSpin = ui->rightLimitSpinBox;
				}

				if(keyEvent->key() == Qt::Key_Left)
				{
					targetSpin->stepDown();
				}
				else if(keyEvent->key() == Qt::Key_Right)
				{
					targetSpin->stepUp();
				}
				showDerivatives();
				paintLimits();
			}
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
	if( !fileOpened ) { return; }

	ui->leftLimitSpinBox->setValue(0);
	ui->rightLimitSpinBox->setValue(dataCutLocal.cols());
}

void Cut::setValuesByEdf()
{
	if( !fileOpened ) { return; }

	currFreq = edfFil.getFreq();
	dataCutLocal = edfFil.getData(); /// expensive


	ui->leftLimitSpinBox->setMaximum(edfFil.getDataLen());
	ui->rightLimitSpinBox->setMaximum(edfFil.getDataLen());
	ui->diffLimitSpinBox->setMaximum(edfFil.getDataLen());
	ui->leftTimeSpinBox->setMaximum(edfFil.getDataLen() / edfFil.getFreq());
	ui->leftTimeSpinBox->setDecimals(1);
	ui->rightTimeSpinBox->setMaximum(edfFil.getDataLen() / edfFil.getFreq());
	ui->rightTimeSpinBox->setDecimals(1);
	ui->diffTimeSpinBox->setMaximum(edfFil.getDataLen() / edfFil.getFreq());
	ui->diffTimeSpinBox->setDecimals(1);

	ui->paintStartDoubleSpinBox->setMaximum(floor(dataCutLocal.cols() / currFreq));
	ui->paintStartDoubleSpinBox->setValue(0); /// or not needed?
	ui->paintStartLabel->setText("start (max " + nm(floor(dataCutLocal.cols() / currFreq)) + ")");
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - 20) / currFreq);
	ui->paintLengthDoubleSpinBox->setValue((this->width() - 20) / currFreq);

	resetLimits();

	/// set coloured channels
	QString redStr = "EOG1";	// ~horizontal
	QString blueStr = "EOG2";	// ~vertical

	/// iitp
	if(edfFil.getNs() > 35)
	{
		redStr = "ECG";
		blueStr = "Artefac";
	}

	int eog1 = edfFil.findChannel(blueStr);
	int eog2 = edfFil.findChannel(redStr);
	if(eog2 == eog1) { eog2 = edfFil.findChannel("EOG3"); }

	ui->color1SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color2SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color3SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color1SpinBox->setValue(eog1);
	ui->color2SpinBox->setValue(eog2);
	ui->color3SpinBox->setValue(-1);

	ui->derivChan1SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->derivChan2SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->derivChan2SpinBox->setValue(edfFil.getNs() - 1); // markers
	ui->derivChan1SpinBox->setValue(edfFil.getNs() - 1); // markers
}

void Cut::createImage(const QString & dataFileName)
{
    addNum = 1;
    currentFile = dataFileName;

	leftDrawLimit = 0;
    if(this->myFileType == fileType::real)
	{
		myLib::readPlainData(dataFileName, dataCutLocal);
		fileOpened = true;
    }
	else if(this->myFileType == fileType::edf)
	{
        edfFil.readEdfFile(dataFileName);
		fileOpened = true;
		setValuesByEdf(); /// needs fileOpened
    }
	ui->iitpDisableEcgCheckBox->setChecked(false);
	paint();
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(0);
}

template<class... params>
void Cut::logAction(const params &... par)
{
	std::ofstream outStr;
	QString name = edfFil.getExpName();
	name = name.left(name.indexOf('_')); /// hope no '_' in dirPath
	outStr.open((edfFil.getDirPath() + "/" +
				 name + "_cutLog.txt").toStdString(), std::ios_base::app);
	myWrite(outStr, par...);
	outStr.close();
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
	if( !fileOpened ) { return; }

	const int st = 5;

	const std::valarray<double> & sig1 = dataCutLocal[ui->derivChan1SpinBox->value()];
	const int ind1 = ui->leftLimitSpinBox->value();
	ui->derivVal1SpinBox->setValue(sig1[ind1]);
	if(ind1 + st < sig1.size() && ind1 - st >=0)
	{
		ui->derivFirst1SpinBox->setValue(sig1[ind1 + st] -  sig1[ind1 - st]);
	}
	if(ind1 + 2 * st < sig1.size() && ind1 - 2 * st >=0)
	{
		ui->derivSecond1SpinBox->setValue(sig1[ind1 + 2 * st] + sig1[ind1 - 2 * st] - 2 * sig1[ind1]);
	}

	const std::valarray<double> & sig2 = dataCutLocal[ui->derivChan2SpinBox->value()];
	const int ind2 = ui->rightLimitSpinBox->value();
	ui->derivVal2SpinBox->setValue(sig2[ind2]);
	if(ind2 + st < sig2.size() && ind2 - st >=0)
	{
		ui->derivFirst2SpinBox->setValue(sig2[ind2 + st] -  sig2[ind2 - st]);
	}
	if(ind2 + 2 * st < sig2.size() && ind2 - 2 * st >=0)
	{
		ui->derivSecond2SpinBox->setValue(sig2[ind2 + 2 * st] + sig2[ind2 - 2 * st] - 2 * sig2[ind2]);
	}


}

void Cut::mousePressSlot(char btn, int coord)
{
	if(btn == 'l' &&
	   coord + leftDrawLimit < ui->rightLimitSpinBox->value())
	{
		ui->leftLimitSpinBox->setValue(coord + leftDrawLimit);
	}
	else if(btn == 'r' &&
			coord + leftDrawLimit > ui->leftLimitSpinBox->value() &&
			coord < dataCutLocal.cols())
	{
		ui->rightLimitSpinBox->setValue(coord + leftDrawLimit);
	}
	showDerivatives();
	paintLimits();
}

void Cut::copySlot()
{
	if( !fileOpened ) { return; }

	this->copyData = dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
								   ui->rightLimitSpinBox->value());

	logAction("copy");
	paint();
}

void Cut::cutSlot()
{
	this->copySlot();
	this->splitSlot();
}

void Cut::paste(int start, const matrix & inData, bool addUndo)
{
	if( !fileOpened ) { return; }

	int cls = inData.cols();

	if(addUndo)
	{
		auto undoAction = [start, cls, this]()
		{
			this->split(start, start + cls, false);
		};
		undos.push_back(undoAction);
	}

	matrix data2 = this->dataCutLocal.subCols(ui->leftLimitSpinBox->value(), dataCutLocal.cols());
	dataCutLocal.resizeCols(ui->leftLimitSpinBox->value());
	dataCutLocal.horzCat(inData).horzCat(data2);
	logAction("paste", start, addUndo);
	paint();
}

void Cut::pasteSlot()
{
	if(copyData.isEmpty()) return;
	this->paste(ui->leftLimitSpinBox->value(), copyData);
}

void Cut::next()
{
    QString helpString;

	auto iterBackup = fileListIter;
	++fileListIter;
	for(; fileListIter != std::end(filesList); ++fileListIter)
	{
		if((*fileListIter).contains(QRegExp(R"({_num|_000|_sht})")))
		{
			std::cout << "next: bad name" << std::endl;
			continue;
		}
		helpString = myLib::getDirPathLib(currentFile) + "/" + (*fileListIter);
		emit openFile(helpString);
		return;
	}
	fileListIter = iterBackup;
	std::cout << "next: bad number, too big" << std::endl;
}

void Cut::prev()
{
    QString helpString;

	auto iterBackup = fileListIter;
	--fileListIter;
	for(; fileListIter != std::begin(filesList); --fileListIter)
	{
		if((*fileListIter).contains(QRegExp(R"({_num|_000|_sht})")))
		{
			std::cout << "prev: bad name" << std::endl;
			continue;
		}
		helpString = myLib::getDirPathLib(currentFile) + "/" + (*fileListIter);
		emit openFile(helpString);
		return;
	}
	fileListIter = iterBackup;
	std::cout << "prev: bad number, too little" << std::endl;
}

void Cut::forwardStepSlot()
{
	if( !fileOpened ) { return; }

	if(leftDrawLimit + ui->scrollArea->width() > dataCutLocal.cols())
    {
		std::cout << "end of file" << std::endl;
        return;
    }

	leftDrawLimit = std::min(leftDrawLimit + currFreq, double(dataCutLocal.cols()));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::backwardStepSlot()
{
	if( !fileOpened ) { return; }

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
	if( !fileOpened ) { return; }

	if(leftDrawLimit + ui->scrollArea->width() > dataCutLocal.cols())
    {
		std::cout << "end of file" << std::endl;
        return;
    }
    leftDrawLimit = std::min(leftDrawLimit +
							 ui->paintLengthDoubleSpinBox->value() * currFreq,
							 double(dataCutLocal.cols()));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::backwardFrameSlot()
{
	if( !fileOpened ) { return; }

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
	if( !fileOpened ) { return; }

	/// make on dataCutLocal
	if(myFileType == fileType::edf)
	{
		const std::valarray<double> & markArr = dataCutLocal[edfFil.getMarkChan()];
		auto it = std::begin(markArr);
		if(mark > 0)
		{
			it = std::find(std::begin(markArr) + leftDrawLimit + 150,
						   std::end(markArr),
						   mark);
		}
		else
		{
			it = std::find_if(std::begin(markArr) + leftDrawLimit + 150,
							  std::end(markArr),
							  [](double in){ return in != 0.; });
		}

		if(it == std::end(markArr))
		{
			std::cout << "findNextMark: marker not found" << std::endl;
			return;
		}

		int index = std::distance(std::begin(markArr), it);
		ui->paintStartDoubleSpinBox->setValue(std::max(0., double(index) / edfFil.getFreq() - 0.5));
//		ui->leftLimitSpinBox->setValue(index);
		showDerivatives();
		paint();
	}
}


void Cut::findPrevMark(double mark)
{
	if( !fileOpened ) { return; }

	/// make on dataCutLocal
	if(myFileType == fileType::edf)
	{
		const std::valarray<double> & markArr = dataCutLocal[edfFil.getMarkChan()];
		const auto beg = std::begin(markArr);
		auto it = std::begin(markArr) + leftDrawLimit;
		if(mark > 0)
		{
			while (it != beg)
			{
				if (*it == mark) { break; }
				else { --it; }
			}
		}
		else
		{
			while (it != beg)
			{
				if (*it != 0.) { break; }
				else { --it; }
			}
		}

		if(it == beg)
		{
			std::cout << "findNextMark: marker not found" << std::endl;
			return;
		}

		int index = std::distance(beg, it);
		ui->paintStartDoubleSpinBox->setValue(std::max(0., double(index) / edfFil.getFreq() - 0.5));
//		ui->leftLimitSpinBox->setValue(index);
		showDerivatives();
		paint();
	}
}

void Cut::cutPausesSlot()
{
	if( !fileOpened ) { return; }

	while(1)
	{
		const auto beg = std::begin(dataCutLocal[edfFil.getMarkChan()]);
		const auto en = std::end(dataCutLocal[edfFil.getMarkChan()]);

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

	while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][newLeftLimit], 14)
		   && newLeftLimit > 0)
    {
        --newLeftLimit;
    }
	while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][newRightLimit], 14)
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
	if(newLeftLimit > 0 || myLib::matiCountBit(data3[edfFil.getNs() - 1][0], 14))
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
			while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][leftLimit], 14)) // there ARE count answers
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
	else if (myLib::matiCountBit(data3[edfFil.getNs() - 1][data3.cols() - 1], 14))
    {
		// do nothing
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
			while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][rightLimit], 14)) // there ARE count answers
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

	logAction("undo");
    paint();
}



void Cut::setMarker(int inVal, bool left)
{
	if( !fileOpened ) { return; }

	if(myFileType == fileType::edf)
	{
		int num = edfFil.getMarkChan();
		if(num <= 0)
		{
			std::cout << "Cut::setMarker: haven't found markers channel" << std::endl;
			return;
		}

		int offset = 0;
		if(left)	{ offset = ui->leftLimitSpinBox->value(); }
		else		{ offset = ui->rightLimitSpinBox->value(); }

		int val = dataCutLocal[num][offset];
		auto undoAction = [num, offset, val, this](){ this->dataCutLocal[num][offset] = val; };
		undos.push_back(undoAction);

		dataCutLocal[num][offset] = inVal;

		logAction("setMarker", inVal, offset, left);
	}
	else if(myFileType == fileType::real)
	{
		int num = dataCutLocal.rows() - 1; /// last channel

		int offset = 0;
		if(left)	{ offset = ui->leftLimitSpinBox->value(); }
		else		{ offset = ui->rightLimitSpinBox->value(); }

		int val = dataCutLocal[num][offset];
		auto undoAction = [num, offset, val, this](){ dataCutLocal[num][offset] = val; };
		undos.push_back(undoAction);

		dataCutLocal[num][offset] = inVal;
	}
	paint();
}

void Cut::toLearnSetSlot()
{
	if( !fileOpened ) { return; }

	matrix sub = dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
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
	if( !fileOpened ) { return; }

	/// should have placed windNum as last index ?
	windParams.resize(dataCutLocal.cols() / paramsWindLen);
	for(int windNum = 0; windNum < windParams.size(); ++windNum)
	{
		matrix localData = dataCutLocal.subCols(paramsWindLen * windNum,
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
	if( !fileOpened ) { return; }

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

		if(windNum == dataCutLocal.cols() / paramsWindLen - 1)
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
		if(myFileType == fileType::edf && !edfFil.isEmpty())
		{
			QString ch = QString(edfFil.getLabels()[n]);
			ui->colorChan1LineEdit->setText(ch
											.remove("EEG ")
											.remove("IT ")
											.remove("EOG ")
											.remove("EMG ")
											);
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
		if(myFileType == fileType::edf && !edfFil.isEmpty())
		{
			QString ch = QString(edfFil.getLabels()[n]);
			ui->colorChan2LineEdit->setText(ch
											.remove("EEG ")
											.remove("IT ")
											.remove("EOG ")
											.remove("EMG ")
											);
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
		if(myFileType == fileType::edf && !edfFil.isEmpty())
		{
			QString ch = QString(edfFil.getLabels()[n]);
			ui->colorChan3LineEdit->setText(ch
											.remove("EEG ")
											.remove("IT ")
											.remove("EOG ")
											.remove("EMG ")
											);
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
	if( !fileOpened ) { return; }

	if(start > end)
	{
		std::cout << "Cut::zero: leftEdge > rightEdge" << std::endl;
		return;
	}

	if(end - start == this->dataCutLocal.cols())
	{
		std::cout << "Cut::zero: you want zero whole file" << std::endl;
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

	undoData.push_back(dataCutLocal.subCols(start, end));
	auto undoAction = [start, this]()
	{
		for(int k = 0; k < dataCutLocal.rows() - 1; ++k) /// don't affect markers
		{
			std::copy(std::begin(undoData.back()[k]),
					  std::end(undoData.back()[k]),
					  std::begin(dataCutLocal[k]) + start);
		}
		undoData.pop_back();
	};
	undos.push_back(undoAction);


	logAction("zeroData", start, end);
	zeroData(dataCutLocal,
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
	this->zero(ui->leftLimitSpinBox->value(), dataCutLocal.cols());
}


/// DANGER markers
void Cut::split(int start, int end, bool addUndo)
{
	if( !fileOpened ) { return; }

	if(start > end)
	{
		std::cout << "Cut::split: leftEdge > rightEdge" << std::endl;
		return;
	}

	if(end - start == this->dataCutLocal.cols()
	   /// magic const 5 min
	   || (start == this->dataCutLocal.cols() && end - start > this->edfFil.getFreq() * 60 * 5)
	   )
	{
		std::cout << "Cut::split: you want split whole file" << std::endl;
		return;
	}

	if(std::find_if(std::begin(dataCutLocal.back()),
					std::end(dataCutLocal.back()),
					[](double in){ return in != 0.; }) != std::end(dataCutLocal.back()))
	{
		std::cout << "Cut::split: there are non-zero markers" << std::endl;
	}

	if(addUndo)
	{
		undoData.push_back(dataCutLocal.subCols(start, end));
		auto undoAction = [start, this]()
		{
			this->paste(start, undoData.back(), false);
			undoData.pop_back();
		};
		undos.push_back(undoAction);
	}

	matrix data2 = dataCutLocal.subCols(end, dataCutLocal.cols());
	dataCutLocal.resizeCols(start).horzCat(data2); /// +1 to save first marker in reals
	ui->paintStartLabel->setText("start (max " + nm(floor(dataCutLocal.cols() / currFreq)) + ")");

	logAction("split", start, end);
	resetLimits();
	ui->paintStartDoubleSpinBox->setValue(start / edfFil.getFreq() - 1.5);
	ui->leftLimitSpinBox->setValue(start); /// really needed?
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
	logAction("splitTillEnd", ui->leftLimitSpinBox->value());
	dataCutLocal.resizeCols(ui->leftLimitSpinBox->value());
	ui->paintStartLabel->setText("start (max " + nm(floor(dataCutLocal.cols() / currFreq)) + ")");
	paint();
}

void Cut::linearApproxSlot()
{
	if( !fileOpened ) { return; }

	const int lef = ui->leftLimitSpinBox->value();
	const int rig = ui->rightLimitSpinBox->value();


	std::vector<int> chanList;
	for(int i = 0; i < dataCutLocal.rows(); ++i)
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
		const double coeff = (dataCutLocal[ch][rig] - dataCutLocal[ch][lef]) / (rig - lef);
		for(int i = lef + 1; i < rig; ++i)
		{
			dataCutLocal[ch][i] = dataCutLocal[ch][lef] + coeff * (i - lef);
		}
	}


	logAction("linearApprox", lef, rig, chanList);
	paint();
}

void Cut::saveAs(const QString & addToName)
{
	if( !fileOpened ) { return; }

	QString newPath = currentFile;
	newPath.insert(newPath.lastIndexOf('.'), addToName);
	edfFil.writeOtherData(dataCutLocal, newPath);
	logAction("saveAs", addToName);
}

void Cut::save()
{
	if( !fileOpened ) { return; }

    if(myFileType == fileType::real)
    {
        QString helpString = def::dir->absolutePath()
							 + "/cut"
							 + "/" + myLib::getFileName(currentFile);

        // new
		myLib::writePlainData(helpString, dataCutLocal);
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

	edfFil.writeOtherData(dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
										ui->rightLimitSpinBox->value()),
						  edfFil.getDirPath() + "/" + newName);

	std::cout << "Cut::saveNewNumSlot: edfFile saved - " << newName << std::endl;
	iitpLog("savNewN", 2, newName);

}

void Cut::saveSubsecSlot()
{
	if( !fileOpened ) { return; }

	if(myFileType == fileType::real || 1) /// write plain windows (eyes)
	{

		QString helpString;
		helpString = def::dir->absolutePath() +
					 "/winds" +
					 "/" + myLib::getFileName(currentFile) +
					 "." + rn(addNum++, 3);
		myLib::writePlainData(helpString,
							  dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
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

		edfFil.writeOtherData(dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
											ui->rightLimitSpinBox->value()), newPath);

		iitpLog("saveSub", 2, newPath);

		std::cout << "Cut::saveSubsecSlot: edfFile saved - " << newPath << std::endl;
	}
	resetLimits();
	paint();
}


void Cut::rewrite()
{
	if( !fileOpened ) { return; }

    if(myFileType == fileType::real)
    {
		myLib::writePlainData(currentFile, dataCutLocal);
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
	if(dataCutLocal.isEmpty()) return;

	int rightDrawLimit = 0.;
    if(myFileType == fileType::edf)
    {
		leftDrawLimit = ui->paintStartDoubleSpinBox->value() * currFreq;
		rightDrawLimit = std::min(leftDrawLimit + ui->scrollArea->width(), int(dataCutLocal.cols()));
    }
    else if(myFileType == fileType::real)
    {
        leftDrawLimit = 0;
		rightDrawLimit = dataCutLocal.cols();
    }
	matrix subData = dataCutLocal.subCols(leftDrawLimit, rightDrawLimit);

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
			pnt.setFont(QFont("", 18)); /// magic const

			for(int i = 0; i < subData.cols(); ++i)
			{
				if(subData[mrk][i] != 0.)
				{
					/// magic consts
					pnt.drawText(i,
								 pnt.device()->height() * (mrk + 1) / (subData.rows() + 2) - 3,
								 nm(int(subData[mrk][i])));
				}
			}
			pnt.end();
		}
	}

	paintLimits();
}

void Cut::paintLimits()
{
	if( !fileOpened ) { return; }
	if(currentPic.isNull()) { return; }

	QPixmap tempPic = currentPic;
	QPainter paint;
	paint.begin(&tempPic);

	int leftX = ui->leftLimitSpinBox->value() - leftDrawLimit;
	if(leftX >= 0 && leftX < ui->paintLengthDoubleSpinBox->value() * this->currFreq)
	{
		paint.setPen(QPen(QBrush("blue"), 2));
		paint.drawLine(leftX, 0, leftX, tempPic.height());
	}

	int rightX = ui->rightLimitSpinBox->value() - leftDrawLimit;
	if(rightX >= 0 && rightX < ui->paintLengthDoubleSpinBox->value() * this->currFreq)
	{
		paint.setPen(QPen(QBrush("red"), 2));
		paint.drawLine(rightX, 0, rightX, tempPic.height());
	}

	ui->picLabel->setPixmap(tempPic.scaled(tempPic.width() - 2,
											  ui->scrollArea->height() - 20));
	paint.end();
}

