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


    ui->subdirComboBox->addItem("Realisations");
    ui->subdirComboBox->addItem("windows");
    ui->subdirComboBox->addItem("windows/fromreal"); //generality

//    ui->subdirComboBox->setCurrentText("windows/fromreal"); /// eyes
    ui->subdirComboBox->setCurrentText("Realisations"); /// me


    ui->eogSpinBox->setValue(2);
    ui->eogDoubleSpinBox->setValue(2.40);
    ui->eogDoubleSpinBox->setSingleStep(0.1);

    ui->drawNormDoubleSpinBox->setValue(1.);
    ui->drawNormDoubleSpinBox->setMaximum(10.);
    ui->drawNormDoubleSpinBox->setMinimum(0.01);
    ui->drawNormDoubleSpinBox->setSingleStep(0.1);
    ui->drawNormDoubleSpinBox->setDecimals(2);

//    ui->paintStartDoubleSpinBox->setValue(0);
    ui->paintStartDoubleSpinBox->setDecimals(1);
    ui->paintStartDoubleSpinBox->setSingleStep(0.1);

//    ui->paintLengthDoubleSpinBox->setValue(4);
    ui->paintLengthDoubleSpinBox->setDecimals(1);
    ui->paintLengthDoubleSpinBox->setSingleStep(0.5);
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - 20) / def::freq);

    ui->checkBox->setChecked(true);

    ui->dirBox->addItem("Realisations");
    ui->dirBox->addItem("cut");
    ui->dirBox->addItem("windows");
    ui->dirBox->setCurrentIndex(0);

//    this->ui->lineEdit_1->setText("0.0");
//    this->ui->lineEdit_2->setText("125");
//    this->ui->lineEdit_3->setText("4.0");
//    this->ui->extYbox->setValue(7);
//    this->ui->tempSpinBox->setMaximum(1500);

    ui->nextButton->setShortcut(tr("d"));
    ui->prevButton->setShortcut(tr("a"));
    ui->cutButton->setShortcut(tr("c"));
    ui->zeroButton->setShortcut(tr("z"));
    ui->saveButton->setShortcut(tr("s"));
    ui->splitButton->setShortcut(tr("x"));
    ui->rewriteButton->setShortcut(tr("r"));
	QShortcut * undoZero = new QShortcut(QKeySequence(tr("Ctrl+z")), this);


    ui->picLabel->installEventFilter(this);

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
	QObject::connect(ui->drawNormDoubleSpinBox, SIGNAL(valueChanged(double)),
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

    this->setAttribute(Qt::WA_DeleteOnClose);

}

Cut::~Cut()
{
    delete ui;
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
		path = def::dir->absolutePath() + slash + ui->subdirComboBox->currentText();
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
//    def::fileMarkers = QStringList{"_BD", "_BW", "_CR", "_Fon", "_kh", "_sm", "_NO"};
//    makeFullFileList(getDirPathLib(helpString), lst, {"." + getExt(helpString)});
	lst = QDir(getDirPathLib(helpString)).entryList({"*." + getExt(helpString)});

//    for(auto str : lst)
//    {
//        cout << str << endl;
//    }
    currentNumber = lst.indexOf(getFileName(helpString));

    createImage(helpString);
}

void Cut::resizeEvent(QResizeEvent * event)
{
    // adjust scrollArea size
	double newLen = doubleRound((event->size().width() - 10 * 2) / def::freq,
                             ui->paintLengthDoubleSpinBox->decimals());
    ui->scrollArea->setGeometry(ui->scrollArea->geometry().x(),
                                ui->scrollArea->geometry().y(),
								newLen * def::freq,
                                ui->scrollArea->geometry().height());
    ui->paintLengthDoubleSpinBox->setValue(newLen);
    paint();
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
				ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / def::freq);
                return true;
            }
            else
            {
//                cout << "none" << endl;
                return false;
            }
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
        def::dir->cd("Realisations");
        helpString = def::dir->entryList(QDir::Files)[0];
        def::dir->cdUp();
        helpString.prepend(def::dir->absolutePath() + slash + "Realisations" + slash);
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

    int NumEog = ui->eogSpinBox->value();
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
    else
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
    else
    {
        edfFil.readEdfFile(dataFileName);
        data3 = edfFil.getData();
        NumOfSlices = data3.cols();
        leftDrawLimit = 0;
		ui->paintStartDoubleSpinBox->setMaximum(floor(NumOfSlices / def::freq));
		ui->paintStartDoubleSpinBox->setValue(0); /// or not needed?
//        cout << freq = " << def::freq << endl;
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

void Cut::mousePressSlot(char btn, int coord)
{
    if(btn == 'l' && coord < rightLimit) leftLimit = coord;
    if(btn == 'r' && coord > leftLimit && coord < NumOfSlices) rightLimit = coord;

    QPixmap pic = currentPic;
    QPainter paint;
    paint.begin(&pic);

    paint.setPen(QPen(QBrush("blue"), 2));
    paint.drawLine(leftLimit, 0, leftLimit, currentPic.height());
    paint.setPen(QPen(QBrush("red"), 2));
    paint.drawLine(rightLimit, 0, rightLimit, currentPic.height());

    ui->picLabel->setPixmap(pic.scaled(pic.width(), ui->scrollArea->height() - 20)); // -20 generality

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

	leftDrawLimit = std::min(leftDrawLimit + def::freq, double(NumOfSlices));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / def::freq);
}
void Cut::backwardStepSlot()
{
    if(leftDrawLimit == 0)
    {
        cout << "begin of file" << endl;
        return;
    }
	leftDrawLimit = std::max(leftDrawLimit - def::freq, 0.);
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / def::freq);
}
void Cut::forwardFrameSlot()
{
    if(leftDrawLimit + ui->scrollArea->width() > NumOfSlices)
    {
        cout << "end of file" << endl;
        return;
    }
    leftDrawLimit = std::min(leftDrawLimit +
							 ui->paintLengthDoubleSpinBox->value() * def::freq,
                             double(NumOfSlices));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / def::freq);
}
void Cut::backwardFrameSlot()
{
    if(leftDrawLimit == 0)
    {
        cout << "begin of file" << endl;
        return;
    }
    leftDrawLimit = std::max(leftDrawLimit -
							 ui->paintLengthDoubleSpinBox->value() * def::freq, 0.);
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / def::freq);
}


void Cut::resizeWidget(double a)
{
	if(this->size().width() == a * def::freq + 20) return;
	this->resize(a * def::freq + 20, this->height());
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
    helpString = (def::dir->absolutePath() +
                                          slash + "windows" +
                                          slash + getFileName(currentFile) +
                                          "." + rightNumber(addNum++, 3));
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
		leftDrawLimit = ui->paintStartDoubleSpinBox->value() * def::freq;
        rightDrawLimit = std::min(leftDrawLimit + ui->scrollArea->width(), NumOfSlices);
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
						 def::freq,
                         helpString,
                         ui->drawNormDoubleSpinBox->value(),
                         blueCh,
                         redCh); // generality.getFreq()

    /// -20 for scroll bar generality
    ui->picLabel->setPixmap(currentPic.scaled(currentPic.width(),
                                              ui->scrollArea->height() - 20));

    rightLimit = rightDrawLimit - leftDrawLimit;
    leftLimit = 0;
}

