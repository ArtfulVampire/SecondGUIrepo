#include "cut.h"
#include "ui_cut.h"

#include <myLib/drw.h>


using namespace myOut;

Cut::Cut() :
    ui(new Ui::Cut)
{
    ui->setupUi(this);
    this->setWindowTitle("Cut-e");

    autoFlag = false;

    redCh = -1;
    blueCh = -1;

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


    ui->eogDoubleSpinBox->setValue(2.40);
    ui->eogDoubleSpinBox->setSingleStep(0.1);

	ui->yNormDoubleSpinBox->setDecimals(2);
	ui->yNormDoubleSpinBox->setMaximum(5.);
	ui->yNormDoubleSpinBox->setMinimum(0.01);
	ui->yNormDoubleSpinBox->setValue(1.);
	ui->yNormDoubleSpinBox->setSingleStep(0.05);

	ui->greenChanSpinBox->setMinimum(-1);
	ui->greenChanSpinBox->setValue(-1);

//    ui->paintStartDoubleSpinBox->setValue(0);
    ui->paintStartDoubleSpinBox->setDecimals(1);
    ui->paintStartDoubleSpinBox->setSingleStep(0.1);

//    ui->paintLengthDoubleSpinBox->setValue(4);
    ui->paintLengthDoubleSpinBox->setDecimals(1);
	ui->paintLengthDoubleSpinBox->setSingleStep(0.2);
	/// can change
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - 20) / currFreq);


	ui->dirBox->addItem("Reals");
    ui->dirBox->addItem("cut");
	ui->dirBox->addItem("winds");
    ui->dirBox->setCurrentIndex(0);

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


    ///////////////
    currentNumber =- 1;

#if 0
    QFileDialog * browser = new QFileDialog();
    browser->setDirectory(def::dir->absolutePath());
    browser->setViewMode(QFileDialog::Detail);
    QObject::connect(ui->browseButton, SIGNAL(clicked()), browser, SLOT(show()));
    QObject::connect(ui->subdirComboBox, SIGNAL(currentTextChanged(QString)),
                     this, SLOT(setBrowserDir()));
    QObject::connect(browser, SIGNAL(fileSelected(QString)),
                     ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(browser, SIGNAL(fileSelected(QString)),
                     this, SLOT(createImage(QString)));
    //    QObject::connect(browser, SIGNAL(fileSelected(QString)), browser, SLOT(hide()));
#else
    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));
#endif

	QObject::connect(undoShortcut, SIGNAL(activated()), this, SLOT(undoSlot()));
	QObject::connect(copyShortcut, SIGNAL(activated()), this, SLOT(copySlot()));
	QObject::connect(pasteShortcut, SIGNAL(activated()), this, SLOT(pasteSlot()));
	QObject::connect(cutShortcut, SIGNAL(activated()), this, SLOT(cutSlot()));

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
	QObject::connect(ui->subtractMeansPushButton, SIGNAL(clicked()), this, SLOT(subtractMeansSlot()));

	QObject::connect(ui->zeroButton, SIGNAL(clicked()), this, SLOT(zeroSlot()));
	QObject::connect(ui->splitButton, SIGNAL(clicked()), this, SLOT(splitSlot()));
	QObject::connect(ui->zeroFromZeroPushButton, SIGNAL(clicked()), this, SLOT(zeroFromZeroSlot()));
	QObject::connect(ui->splitFromZeroPushButton, SIGNAL(clicked()), this, SLOT(splitFromZeroSlot()));
	QObject::connect(ui->zeroTillEndPushButton, SIGNAL(clicked()), this, SLOT(zeroTillEndSlot()));
	QObject::connect(ui->splitTillEndPushButton, SIGNAL(clicked()), this, SLOT(splitTillEndSlot()));

	QObject::connect(this, SIGNAL(openFile(QString)), ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(this, SIGNAL(openFile(QString)), this, SLOT(createImage(const QString &)));

    QObject::connect(ui->forwardStepButton, SIGNAL(clicked()), this, SLOT(forwardStepSlot()));
    QObject::connect(ui->backwardStepButton, SIGNAL(clicked()), this, SLOT(backwardStepSlot()));
    QObject::connect(ui->forwardFrameButton, SIGNAL(clicked()), this, SLOT(forwardFrameSlot()));
	QObject::connect(ui->backwardFrameButton, SIGNAL(clicked()), this, SLOT(backwardFrameSlot()));

	QObject::connect(ui->iitpAutoCorrPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoCorrSlot()));
	QObject::connect(ui->iitpAutoJumpPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoJumpSlot()));
	QObject::connect(ui->iitpManualPushButton, SIGNAL(clicked()), this, SLOT(iitpManualSlot()));
	QObject::connect(ui->setMark1PushButton, SIGNAL(clicked()), this, SLOT(set1MarkerSlot()));
	QObject::connect(ui->setMark2PushButton, SIGNAL(clicked()), this, SLOT(set2MarkerSlot()));
	QObject::connect(ui->greenChanSpinBox, SIGNAL(valueChanged(int)), this, SLOT(greenChanSlot()));

    this->setAttribute(Qt::WA_DeleteOnClose);

}

Cut::~Cut()
{
    delete ui;
}

void zeroData(matrix & inData, int leftLimit, int rightLimit)
{
	for(int k = 0; k < def::nsWOM(); ++k) /// don't affect markers
	{
		std::fill(std::begin(inData[k]) + leftLimit,
				  std::begin(inData[k]) + rightLimit,
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
				   slash + ui->subdirComboBox->currentText();
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
	lst = QDir(myLib::getDirPathLib(helpString)).entryList({"*." + myLib::getExt(helpString)});

	currentNumber = lst.indexOf(myLib::getFileName(helpString));

    createImage(helpString);
}

void Cut::resizeEvent(QResizeEvent * event)
{
    // adjust scrollArea size
	double newLen = smallLib::doubleRound((event->size().width() - 10 * 2) / currFreq,
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

void Cut::createImage(const QString & dataFileName)
{
    addNum = 1;
    currentFile = dataFileName;

    if(this->myFileType == fileType::real)
    {
		int tmp = 0;
		myLib::readPlainData(dataFileName, data3, tmp);
        leftDrawLimit = 0;
		rightDrawLimit = data3.cols();
    }
	else if(this->myFileType == fileType::edf)
	{
        edfFil.readEdfFile(dataFileName);
		def::ns = edfFil.getNs();
		currFreq = edfFil.getFreq();
		data3 = edfFil.getData();
		leftDrawLimit = 0;

		ui->leftLimitSpinBox->setMaximum(edfFil.getDataLen());
		ui->rightLimitSpinBox->setMaximum(edfFil.getDataLen());

		ui->paintStartDoubleSpinBox->setMaximum(floor(data3.cols() / currFreq));
		ui->paintStartDoubleSpinBox->setValue(0); /// or not needed?
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
		redCh = edfFil.findChannel(redStr);
		blueCh = edfFil.findChannel(blueStr);
		ui->greenChanSpinBox->setMaximum(edfFil.getNs() - 1);
		ui->greenChanSpinBox->setValue(-1);
    }

	paint();
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(0);
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
	tmpFile.writeEdfFile(tmpFile.getDirPath() + slash + newName);
}

void Cut::iitpAutoJumpSlot()
{
	edfFile tmpFile = edfFil;
	tmpFile.iitpSyncAutoJump(ui->rightLimitSpinBox->value(),
							 ui->leftLimitSpinBox->value(),
							 ui->iitpByEegCheckBox->isChecked());
	QString newName = tmpFile.getFileNam();
	newName.replace(".edf", "_sync.edf");
	std::cout << "iitpAutoSlot: newFileName = " << newName << std::endl;
	tmpFile.writeEdfFile(tmpFile.getDirPath() + slash + newName);
}

void Cut::iitpManualSlot()
{
	edfFile tmpFile = edfFil;
	tmpFile.iitpSyncManual(ui->rightLimitSpinBox->value(),
						   ui->leftLimitSpinBox->value(),
						   200);
	QString newName = tmpFile.getFileNam();
	newName.replace(".edf", "_sync.edf");
	std::cout << "iitpManualSlot: newFileName = " << newName << std::endl;
	tmpFile.writeEdfFile(tmpFile.getDirPath() + slash + newName);
}

void Cut::mousePressSlot(char btn, int coord)
{
	if(btn == 'l' && coord < rightLimit)
	{
		leftLimit = coord;
		ui->leftLimitSpinBox->setValue(leftLimit + leftDrawLimit);
	}
	else if(btn == 'r' && coord > leftLimit && coord < data3.cols())
	{
		rightLimit = coord;
		ui->rightLimitSpinBox->setValue(rightLimit + leftDrawLimit);
	}

	QPixmap tempPic = currentPic;
    QPainter paint;
	paint.begin(&tempPic);

    paint.setPen(QPen(QBrush("blue"), 2));
    paint.drawLine(leftLimit, 0, leftLimit, currentPic.height());
    paint.setPen(QPen(QBrush("red"), 2));
    paint.drawLine(rightLimit, 0, rightLimit, currentPic.height());

	ui->picLabel->setPixmap(tempPic.scaled(currentPic.width() - 2,
											  ui->scrollArea->height() - 20));
    paint.end();
}

void Cut::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
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
		helpString = myLib::getDirPathLib(currentFile) + slash + lst[++currentNumber];
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
		helpString = myLib::getDirPathLib(currentFile) + slash + lst[--currentNumber];
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


void Cut::resizeWidget(double a)
{
	if(this->size().width() == a * currFreq + 20) return;
	this->resize(a * currFreq + 20, this->height());
}


/// make for opened edf
void Cut::matiAdjustLimits() /////// should TEST !!!!!
{
    QStringList lst = currentFile.split(QRegExp("[_.]"),
                                        QString::SkipEmptyParts); // to detect session, type, piece
    int tempNum = 0;
    for(int i = 0; i < lst.length(); ++i)
    {
        if(QString::number(lst[i].toInt()) == lst[i])
        {
            tempNum = i;
            break;
        }
    }
    int typeNum = lst[tempNum].toInt();
    int sesNum = lst[tempNum + 1].toInt();
    int pieceNum = lst[tempNum + 2].toInt();

//    std::cout << "typeNum = " << typeNum << std::endl;
//    std::cout << "sesNum = " << sesNum << std::endl;
//    std::cout << "pieceNum = " << pieceNum << std::endl;

    if(typeNum != 0 && typeNum != 2) return; // adjust only if count or composed activity

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
		undoAction = [num, offset, val, this](){this->data3[num][offset] = val;};
		undos.push_back(undoAction);

		data3[num][offset] = inVal;
	}
	else if(myFileType == fileType::real)
	{
		int num = data3.rows() - 1; /// last channel
		int offset = ui->leftLimitSpinBox->value();
		int val = data3[num][offset];
		undoAction = [num, offset, val, this](){data3[num][offset] = val;};
		undos.push_back(undoAction);

		data3[num][offset] = inVal;
	}
	paint();
}

void Cut::set1MarkerSlot()
{
	this->setMarker(ui->mark1LineEdit->text().toInt());
}

void Cut::set2MarkerSlot()
{
	this->setMarker(ui->mark2LineEdit->text().toInt());
}


void Cut::greenChanSlot()
{
	const int n = ui->greenChanSpinBox->value();
	if(n >=0)
	{
		if(myFileType == fileType::edf)
		{
			QString ch = QString(edfFil.getLabels()[n]);
			ui->greenChanLineEdit->setText(ch.remove("EEG ").remove("IT "));
		}
	}
	else
	{
		ui->greenChanLineEdit->clear();
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
//    int h = 0;

	// if MATI with counts - adjust limits to problem edges
	// move leftLimit after the nearest marker
	// move rightLimit after the nearest marker
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
		for(int k = 0; k < def::nsWOM(); ++k) /// don't affect markers
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
	paint();
}

void Cut::splitSlot()
{
	this->split(ui->leftLimitSpinBox->value(),
				ui->rightLimitSpinBox->value());
}

void Cut::splitFromZeroSlot()
{
	this->split(0, ui->rightLimitSpinBox->value());
	ui->paintStartDoubleSpinBox->setValue(0.);
}

void Cut::splitTillEndSlot()
{
	data3.resizeCols(ui->leftLimitSpinBox->value());
	paint();
}

void Cut::subtractMeansSlot()
{
	for(auto & row : data3)
	{
		row -= smallLib::mean(row);
	}
	paint();
}

void Cut::save()
{
    if(myFileType == fileType::real)
    {
        QString helpString = def::dir->absolutePath()
                             + slash + "cut"
							 + slash + myLib::getFileName(currentFile);

        // new
		myLib::writePlainData(helpString, data3);
    }
    else if(myFileType == fileType::edf)
    {
        QString newPath = currentFile;
        newPath.insert(newPath.lastIndexOf('.'), "_new");
        edfFil.writeOtherData(data3, newPath);
		std::cout << "Cut::save: edfFile saved - " << newPath << std::endl;
    }

}

void Cut::saveSubsecSlot()
{
	if(myFileType == fileType::real)
	{
		QString helpString;
		helpString = def::dir->absolutePath() +
					 slash + "winds" +
					 slash + myLib::getFileName(currentFile) +
					 "." + myLib::rightNumber(addNum++, 3);
		myLib::writePlainData(helpString,
							  data3.subCols(ui->leftLimitSpinBox->value(),
											ui->rightLimitSpinBox->value()));
	}
	else if(myFileType == fileType::edf)
	{
		if(ui->leftLimitSpinBox->value() > ui->rightLimitSpinBox->value())
		{
			std::cout << "Cut::saveSubsecSlot: leftEdge > rightEdge" << std::endl;
			return;
		}
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
			newPath.insert(newPath.lastIndexOf('.'), "_" + QString::number(counter));
			++counter;
		}

		edfFil.writeOtherData(data3.subCols(ui->leftLimitSpinBox->value(),
											ui->rightLimitSpinBox->value()), newPath);
		std::cout << "Cut::saveSubsecSlot: edfFile saved - " << newPath << std::endl;
	}
	paint();
}


void Cut::rewrite()
{
    if(myFileType == fileType::real)
    {
		myLib::writePlainData(currentFile,
							  data3);
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
	for(std::pair<int, QColor> pairs : {
		std::pair<int, QColor>{blueCh, "blue"},
		std::pair<int, QColor>{redCh, "red"},
		std::pair<int, QColor>{ui->greenChanSpinBox->value(), "green"}
})
	{
		if(pairs.first >= 0)
		{
			res.push_back(pairs);
		}
	}
	return res;
}

void Cut::paint() // save to tmp.jpg and display
{
    QString helpString;
    helpString = def::dir->absolutePath() + slash + "tmp.jpg";

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
//	subData[edfFil.findChannel("ECG")] = 0; /// for iitp ecg

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
			pnt.setFont(QFont("", 14));

			for(int i = 0; i < subData.cols(); ++i)
			{
				if(subData[mrk][i] != 0.)
				{
					pnt.drawText(i,
								 pnt.device()->height() * (mrk + 1) / (subData.rows() + 2) - 3,
								 QString::number(int(subData[mrk][i])));
				}
			}
			pnt.end();
		}
	}

    /// -20 for scroll bar generality
	/// experimental xNorm
	ui->picLabel->setPixmap(currentPic.scaled(currentPic.width() - 2,
											  ui->scrollArea->height() - 20));

    rightLimit = rightDrawLimit - leftDrawLimit;
    leftLimit = 0;
}

