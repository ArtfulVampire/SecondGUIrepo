#include "cut.h"
#include "ui_cut.h"

using namespace std;
using namespace myLib;
using namespace smallLib;

Cut::Cut() :
    ui(new Ui::Cut)
{
    ui->setupUi(this);
    this->setWindowTitle("Cut-e");

    autoFlag = false;

    redCh = -1;
    blueCh = -1;

    if(def::ns == 41)
    {
        redCh = 21;
        blueCh = 22;
    }
    else if(def::ns == 22 || def::ns == 21)
    {
        redCh = 19;
        blueCh = 20;
    }
    else if(def::ns == 24)
    {
        redCh = 21;
        blueCh = 22;
    }

	ui->subdirComboBox->addItem("");
	ui->subdirComboBox->addItem("Reals");
	ui->subdirComboBox->addItem("winds");
	ui->subdirComboBox->addItem("winds/fromreal"); //generality

	ui->subdirComboBox->setCurrentText(""); /// me


    ui->eogDoubleSpinBox->setValue(2.40);
    ui->eogDoubleSpinBox->setSingleStep(0.1);

	ui->yNormDoubleSpinBox->setDecimals(2);
	ui->yNormDoubleSpinBox->setMaximum(5.);
	ui->yNormDoubleSpinBox->setMinimum(0.01);
	ui->yNormDoubleSpinBox->setValue(1.);
	ui->yNormDoubleSpinBox->setSingleStep(0.05);


	ui->xNormDoubleSpinBox->setDecimals(2);
	ui->xNormDoubleSpinBox->setMaximum(4.);
	ui->xNormDoubleSpinBox->setMinimum(0.25);
	ui->xNormDoubleSpinBox->setValue(1.);
	ui->xNormDoubleSpinBox->setSingleStep(0.25);

//    ui->paintStartDoubleSpinBox->setValue(0);
    ui->paintStartDoubleSpinBox->setDecimals(1);
    ui->paintStartDoubleSpinBox->setSingleStep(0.1);

//    ui->paintLengthDoubleSpinBox->setValue(4);
    ui->paintLengthDoubleSpinBox->setDecimals(1);
	ui->paintLengthDoubleSpinBox->setSingleStep(0.2);
	/// can change
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - 20) / currFreq);

    ui->checkBox->setChecked(true);

	ui->dirBox->addItem("Reals");
    ui->dirBox->addItem("cut");
	ui->dirBox->addItem("winds");
    ui->dirBox->setCurrentIndex(0);

    ui->nextButton->setShortcut(tr("d"));
    ui->prevButton->setShortcut(tr("a"));
    ui->cutButton->setShortcut(tr("c"));
    ui->zeroButton->setShortcut(tr("z"));
    ui->saveButton->setShortcut(tr("s"));
    ui->splitButton->setShortcut(tr("x"));
    ui->rewriteButton->setShortcut(tr("r"));
	ui->forwardFrameButton->setShortcut(QKeySequence::Forward);
	ui->backwardFrameButton->setShortcut(QKeySequence::Back);
	ui->forwardFrameButton->setShortcut(tr("e"));
	ui->backwardFrameButton->setShortcut(tr("q"));
	QShortcut * undoZero = new QShortcut(QKeySequence(tr("Ctrl+z")), this);


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

	QObject::connect(undoZero, SIGNAL(activated()), this, SLOT(undoZeroSlot()));
	QObject::connect(ui->yNormDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintStartDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(resizeWidget(double)));

    QObject::connect(this, SIGNAL(buttonPressed(char,int)), this, SLOT(mousePressSlot(char,int)));
    QObject::connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));
    QObject::connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    QObject::connect(ui->cutButton, SIGNAL(clicked()), this, SLOT(cut()));
    QObject::connect(ui->zeroButton, SIGNAL(clicked()), this, SLOT(zero()));
    QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
    QObject::connect(ui->rewriteButton, SIGNAL(clicked()), this, SLOT(rewrite()));
	QObject::connect(this, SIGNAL(openFile(QString)), ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(this, SIGNAL(openFile(QString)), this, SLOT(createImage(const QString &)));
    QObject::connect(ui->cutEyesButton, SIGNAL(clicked()), this, SLOT(cutEyesAll()));
    QObject::connect(ui->splitButton, SIGNAL(clicked()), this, SLOT(splitCut()));

    QObject::connect(ui->forwardStepButton, SIGNAL(clicked()), this, SLOT(forwardStepSlot()));
    QObject::connect(ui->backwardStepButton, SIGNAL(clicked()), this, SLOT(backwardStepSlot()));
    QObject::connect(ui->forwardFrameButton, SIGNAL(clicked()), this, SLOT(forwardFrameSlot()));
	QObject::connect(ui->backwardFrameButton, SIGNAL(clicked()), this, SLOT(backwardFrameSlot()));

	QObject::connect(ui->iitpAutoCorrPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoCorrSlot()));
	QObject::connect(ui->iitpAutoJumpPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoJumpSlot()));
	QObject::connect(ui->iitpManualPushButton, SIGNAL(clicked()), this, SLOT(iitpManualSlot()));
	QObject::connect(ui->subtractMeansPushButton, SIGNAL(clicked()), this, SLOT(subtractMeansSlot()));

    this->setAttribute(Qt::WA_DeleteOnClose);

}

Cut::~Cut()
{
    delete ui;
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
		QPixmap pic = drawOneSignal(seen, 50);

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

	QString filter{};
	for(const QString & in : def::edfFilters)
	{
		filter += in + " ";
	}
	filter += "*." + def::plainDataExtension;
	QString helpString = QFileDialog::getOpenFileName((QWidget*)this,
													  tr("Open file"),
													  path,
													  filter);


    if(helpString.isEmpty())
    {
        QMessageBox::information((QWidget*)this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
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
	lst = QDir(getDirPathLib(helpString)).entryList({"*." + getExt(helpString)});

    currentNumber = lst.indexOf(getFileName(helpString));

    createImage(helpString);
}

void Cut::resizeEvent(QResizeEvent * event)
{
    // adjust scrollArea size
	double newLen = doubleRound((event->size().width() - 10 * 2) / currFreq,
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
//                cout << "real" << endl;
                ui->scrollArea->horizontalScrollBar()->setSliderPosition(
                            ui->scrollArea->horizontalScrollBar()->sliderPosition() +
                            offset);
                return true;
            }
            else if(myFileType == fileType::edf)
            {
//                cout << "edf" << endl;
                if((leftDrawLimit + ui->scrollArea->width() > NumOfSlices && offset > 0) ||
                   (leftDrawLimit == 0 && offset < 0))
                {
                    return false;
                }
                leftDrawLimit = std::min(leftDrawLimit + offset, NumOfSlices);
				ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
                return true;
            }
            else
            {
//                cout << "none" << endl;
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

void Cut::cutEyesAll()
{
#if 0
    QString helpString;
    // automatization
    if(autoFlag)
    {
        ui->checkBox->setChecked(false);
		def::dir->cd("Reals");
        helpString = def::dir->entryList(QDir::Files)[0];
        def::dir->cdUp();
		helpString.prepend(def::dir->absolutePath() + slash + "Reals" + slash);
        cout<<helpString.toStdString()<<endl;
        emit openFile(helpString);
    }


    def::dir->cd(ui->dirBox->currentText());  // generality

    QStringList nameFilters; /// generality
    nameFilters << "*_241*";
    nameFilters << "*_247*";
    nameFilters << "*_254*"; // no need?

    lst = def::dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
    def::dir->cdUp();

	/// generality magic constant
	int NumEog = 2;
    double * thresholdEog = new double [NumEog];
    for(int i = 0; i < NumEog; ++i)
    {
        thresholdEog[i]=0.;
    }
//    cout<<"2"<<endl;

    double ** eogArray = new double* [NumEog];
    for(int i = 0; i < NumEog; ++i)
    {
        eogArray[i] = new double [8192*50]; // 50, size generality
    }
    int currentSlice = 0;
//    cout<<"3"<<endl;

    for(int k = 0; k < int(lst.length()/4); ++k)
    {
        helpString = (def::dir->absolutePath()
                                              + slash + ui->dirBox->currentText()
                                              + slash + lst[rand()%lst.length()]);
        emit openFile(helpString);

        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int i = def::ns - NumEog; i < def::ns; ++i)
            {
                eogArray[i-(ns-NumEog)][currentSlice] = data3[i][j];
            }
            ++currentSlice;
        }

    }
//    cout<<"4"<<endl;

    for(int i = 0; i < NumEog; ++i)
    {
        for(int j = 0; j < currentSlice; ++j)
        {
            thresholdEog[i] += eogArray[i][j]*eogArray[i][j];
        }
        thresholdEog[i] /= currentSlice;
        thresholdEog[i] = sqrt(thresholdEog[i]); // thresholdEog[i] = sigma
    }

    for(int i = 0; i < NumEog; ++i)
    {
        delete[] eogArray[i];
    }
    delete[] eogArray;

//    cout<<"5"<<endl;
    FILE * eyes;
    helpString = (def::dir->absolutePath()
                                          + slash + "eyesSlices");
    eyes = fopen(helpString, "w");

    int flag;
    int NumOfEogSlices = 0;
    int num = 0;
    for(int i = 0; i<lst.length(); ++i)
    {
        helpString = (def::dir->absolutePath()
                                              + slash + ui->dirBox->currentText()
                                              + slash + lst[i]);

        emit openFile(helpString);
        num += NumOfSlices;
        for(int i = 0; i < NumOfSlices; ++i)
        {
            flag = 1;
            for(int j = 0; j < NumEog; ++j)
            {
                /// uncomment one of them
//                if(fabs(data3[def::ns - NumEog + j][i]) < ui->eogDoubleSpinBox->value()*thresholdEog[j] && !ui->markersCheckBox->isChecked()) flag = 0; // w/o markers
//                if(fabs(data3[def::ns - 1 - NumEog + j][i]) < ui->eogDoubleSpinBox->value()*thresholdEog[j] && ui->markersCheckBox->isChecked()) flag = 0; // with markers
            }
            if(flag == 1)
            {
                for(int k = 0; k < ns; ++k)
                {
                    fprintf(eyes, "%lf\n", data3[k][i]);
                }
                ++NumOfEogSlices;
            }
        }
    }

    fclose(eyes);
    helpString = (def::dir->absolutePath()
                                          + slash + "eyesSlices");
    QFile *file = new QFile(helpString);
    file->open(QIODevice::ReadOnly);
    QByteArray contents = file->readAll();
    file->close();
    file->open(QIODevice::WriteOnly);
    file->write("NumOfSlices ");
    file->write(QString::number(NumOfEogSlices).toStdString().c_str());
    file->write("\r\n");
    file->write(contents);
    file->close();
    delete file;

    ui->eyesSlicesSpinBox->setValue(NumOfEogSlices);

    delete []thresholdEog;

    helpString = "NumOfEyesSlices = " + QString::number(NumOfEogSlices);

    // automatization
    if(!autoFlag)
    {
        QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
    }
#endif
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
        readPlainData(dataFileName, data3, NumOfSlices);
        leftDrawLimit = 0;
        rightDrawLimit = NumOfSlices;
    }
	else if(this->myFileType == fileType::edf)
    {
        edfFil.readEdfFile(dataFileName);
		def::ns = edfFil.getNs();
		currFreq = edfFil.getFreq();
        data3 = edfFil.getData();
        NumOfSlices = data3.cols();
        leftDrawLimit = 0;

		ui->leftLimitSpinBox->setMaximum(edfFil.getDataLen());
		ui->rightLimitSpinBox->setMaximum(edfFil.getDataLen());

		ui->paintStartDoubleSpinBox->setMaximum(floor(NumOfSlices / currFreq));
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
		for(int i = 0; i < edfFil.getNs(); ++i)
		{
			if(edfFil.getLabels()[i].contains(redStr)) redCh = i;
			else if(edfFil.getLabels()[i].contains(blueStr)) blueCh = i;
		}

    }

    /// if too long?
    /// draw only needed part?
    if(ui->checkBox->isChecked())
    {
        paint();
    }
    else
    {
        leftLimit = 0;
        rightLimit = 0;
    }
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(0);
}


void Cut::iitpAutoCorrSlot()
{
	edfFil.iitpSyncAutoCorr(ui->rightLimitSpinBox->value(),
							ui->leftLimitSpinBox->value(),
							ui->iitpByEegCheckBox->isChecked());
	QString newName = edfFil.getFileNam();
	newName.replace(".edf", "_sync.edf");
	std::cout << "iitpAutoSlot: newFileName = " << newName << std::endl;
	edfFil.writeEdfFile(edfFil.getDirPath() + slash + newName);
}

void Cut::iitpAutoJumpSlot()
{
	edfFil.iitpSyncAutoJump(ui->rightLimitSpinBox->value(),
							ui->leftLimitSpinBox->value(),
							ui->iitpByEegCheckBox->isChecked());
	QString newName = edfFil.getFileNam();
	newName.replace(".edf", "_sync.edf");
	std::cout << "iitpAutoSlot: newFileName = " << newName << std::endl;
	edfFil.writeEdfFile(edfFil.getDirPath() + slash + newName);
}

void Cut::iitpManualSlot()
{
	edfFil.iitpSyncManual(ui->rightLimitSpinBox->value(),
						  ui->leftLimitSpinBox->value(), 200);
	QString newName = edfFil.getFileNam();
	newName.replace(".edf", "_sync.edf");
	std::cout << "iitpManualSlot: newFileName = " << newName << std::endl;
	edfFil.writeEdfFile(edfFil.getDirPath() + slash + newName);
}

void Cut::mousePressSlot(char btn, int coord)
{
	if(btn == 'l' && coord < rightLimit)
	{
		leftLimit = coord;
		ui->leftLimitSpinBox->setValue(leftLimit + leftDrawLimit);
	}
	if(btn == 'r' && coord > leftLimit && coord < NumOfSlices)
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

	ui->picLabel->setPixmap(tempPic.scaled(currentPic.width() / ui->xNormDoubleSpinBox->value() - 2,
											  ui->scrollArea->height() - 20));
    paint.end();
}

void Cut::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
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
        helpString = getDirPathLib(currentFile) + slash + lst[++currentNumber];
        emit openFile(helpString);
        return;
    }
    currentNumber = tmp;
    cout << "next: bad number, too big" << endl;

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
        helpString = getDirPathLib(currentFile) + slash + lst[--currentNumber];
        emit openFile(helpString);
        return;
    }
    currentNumber = tmp;
    cout << "prev: bad number, too little" << endl;

}


void Cut::forwardStepSlot()
{
    if(leftDrawLimit + ui->scrollArea->width() > NumOfSlices)
    {
        cout << "end of file" << endl;
        return;
    }

	leftDrawLimit = std::min(leftDrawLimit + currFreq, double(NumOfSlices));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::backwardStepSlot()
{
    if(leftDrawLimit == 0)
    {
        cout << "begin of file" << endl;
        return;
    }
	leftDrawLimit = std::max(leftDrawLimit - currFreq, 0.);
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::forwardFrameSlot()
{
    if(leftDrawLimit + ui->scrollArea->width() > NumOfSlices)
    {
        cout << "end of file" << endl;
        return;
    }
    leftDrawLimit = std::min(leftDrawLimit +
							 ui->paintLengthDoubleSpinBox->value() * currFreq,
                             double(NumOfSlices));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
}
void Cut::backwardFrameSlot()
{
    if(leftDrawLimit == 0)
    {
        cout << "begin of file" << endl;
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

//    cout << "typeNum = " << typeNum << endl;
//    cout << "sesNum = " << sesNum << endl;
//    cout << "pieceNum = " << pieceNum << endl;

    if(typeNum != 0 && typeNum != 2) return; // adjust only if count or composed activity

    int newLeftLimit = leftLimit;
    int newRightLimit = rightLimit;

    while (!matiCountBit(data3[def::ns - 1][newLeftLimit], 14) && newLeftLimit > 0)
    {
        --newLeftLimit;
    }
    while (!matiCountBit(data3[def::ns - 1][newRightLimit], 14) && newRightLimit < NumOfSlices)
    {
        ++newRightLimit; // maximum of NumOfSlices
    }


    cout << "newLeftLimit = " << newLeftLimit << endl;
    cout << "newRightLimit = " << newRightLimit << endl;

    // adjust limits if slice by whole count problems
    ++newLeftLimit;
    if(newRightLimit != NumOfSlices) ++newRightLimit;

    leftLimit = newLeftLimit;
    rightLimit = newRightLimit;
    return;



    // adjust limits if slice by N seconds
    if(newLeftLimit > 0 || matiCountBit(data3[def::ns - 1][0], 14))
    {
        ++newLeftLimit; // after the previous marker
    }
    else // if(newLeftLimit == 0)
    {
        // cut end in previous file, suspect that there ARE count answers
        if(pieceNum != 0) // if this piece is not the first in the session
        {
//            cout << "zero prev file" << endl;
            prev();
            leftLimit = rightLimit;
            while (!matiCountBit(data3[def::ns - 1][leftLimit], 14)) // there ARE count answers
            {
                --leftLimit;
            }
            ++leftLimit;
            cout << "prev file leftLimit = " << leftLimit << endl;
            // zero() from tempLimit to rightLimit
            zeroData(data3, leftLimit, rightLimit);
            rewrite();
            next();
        }
    }


    if(newRightLimit < NumOfSlices)
    {
        ++newRightLimit; // after the previous marker
    }
    else if (matiCountBit(data3[def::ns - 1][NumOfSlices - 1], 14))
    {
        //do nothing
    }
    else // if(newRightLimit == NumOfSlices && bit == 0)
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
            while (!matiCountBit(data3[def::ns - 1][rightLimit], 14)) // there ARE count answers
            {
                ++rightLimit;
            }
            ++rightLimit;
            cout << "next file rightLimit = " << rightLimit << endl;

            zeroData(data3, leftLimit, rightLimit);
            rewrite();
        }
        prev();
    }
    leftLimit = newLeftLimit;
    rightLimit = newRightLimit;

}

void Cut::zero()
{
//    int h = 0;

    // if MATI with counts - adjust limits to problem edges
    // move leftLimit after the nearest marker
    // move rightLimit after the nearest marker
    // delete [leftLimit, rightLimit)

	/// MATI counting problem only
	{
		// ExpName.left(3)_fileSuffix_TYPE_SESSION_PIECE.MARKER
		QString helpString = "_0_[0-9]_[0-9]{2,2}";
		if(currentFile.contains(QRegExp(helpString)))
		{
//			cout << "zero: adjust limits   " << currentFile << endl;
//			matiAdjustLimits();
		}
	}



    undoBegin = leftDrawLimit + leftLimit;
    undoData = data3.subCols(undoBegin,
                             undoBegin + rightLimit - leftLimit);

    zeroData(data3,
             leftDrawLimit + leftLimit,
             leftDrawLimit + rightLimit); ///////// should TEST !!!!!!!1111
    paint();
}

void Cut::undoZeroSlot()
{
    for(int k = 0; k < def::nsWOM(); ++k) /// don't affect markers
    {
        std::copy(std::begin(undoData[k]),
                  std::end(undoData[k]),
                  std::begin(data3[k]) + undoBegin);
    }
    paint();
}

void Cut::cut()
{
    QString helpString;
	helpString = def::dir->absolutePath() +
				 slash + "winds" +
				 slash + getFileName(currentFile) +
				 "." + rightNumber(addNum++, 3);
    writePlainData(helpString,
                   data3,
                   rightLimit - leftLimit,
                   leftDrawLimit + leftLimit);

    rightLimit = rightDrawLimit - leftDrawLimit;
    leftLimit = 0;
    paint();
}

/// DANGER markers
void Cut::splitCut()
{
	for(int i = leftDrawLimit + leftLimit; i < NumOfSlices - (rightLimit - leftLimit); ++i)
    {
        for(int k = 0; k < def::nsWOM(); ++k)
        {
            if(i == 0) // dont touch first marker value
            {
                continue;
            }
            data3[k][i] = data3[k][i + (rightLimit - leftLimit)];
        }
    }
    NumOfSlices -= (rightLimit - leftLimit);
	data3.resizeCols(NumOfSlices);
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
                             + slash + getFileName(currentFile);

        // new
        writePlainData(helpString, data3);
    }
    else if(myFileType == fileType::edf)
    {
        QString newPath = currentFile;
        newPath.insert(newPath.lastIndexOf('.'), "_new");
        edfFil.writeOtherData(data3, newPath);
		cout << "Cut::save: edfFile saved - " << newPath << endl;
    }

}


void Cut::rewrite()
{
    if(myFileType == fileType::real)
    {
        writePlainData(currentFile,
                       data3);
        currentPic.save(getPicPath(currentFile), 0, 100);
    }
    else if(myFileType == fileType::edf)
	{
		std::cout << "Cut::rewrite: deliberately forbidden for edfs, use Save instead" << std::endl;
    }
}

void Cut::paint() // save to tmp.jpg and display
{
    QString helpString;
    helpString = def::dir->absolutePath() + slash + "tmp.jpg";

    if(myFileType == fileType::edf)
    {
		leftDrawLimit = ui->paintStartDoubleSpinBox->value() * currFreq;
        rightDrawLimit = std::min(leftDrawLimit + ui->scrollArea->width(), NumOfSlices);

		/// experimental xNorm
		rightDrawLimit *= ui->xNormDoubleSpinBox->value();
    }
    else if(myFileType == fileType::real)
    {
        leftDrawLimit = 0;
        rightDrawLimit = NumOfSlices;
    }

//    cout << "paint: left = " << leftDrawLimit << "\tright = " << rightDrawLimit << endl;

	currentPic = drawEeg(data3.subCols(leftDrawLimit, rightDrawLimit),
                         def::ns,
                         rightDrawLimit - leftDrawLimit,
						 currFreq,
                         helpString,
						 ui->yNormDoubleSpinBox->value(),
                         blueCh,
                         redCh); // generality.getFreq()

    /// -20 for scroll bar generality
	/// experimental xNorm
	ui->picLabel->setPixmap(currentPic.scaled(currentPic.width() / ui->xNormDoubleSpinBox->value() - 2,
											  ui->scrollArea->height() - 20));

    rightLimit = rightDrawLimit - leftDrawLimit;
    leftLimit = 0;
}

