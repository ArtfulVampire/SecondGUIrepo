#include "cut.h"
#include "ui_cut.h"


/*after every function the QDir dir is set to the ExpName directory*/


Cut::Cut(QDir * dir_, int ns_) :
    ui(new Ui::Cut)
{
    ui->setupUi(this);
    this->setWindowTitle("Cut-e");

    ui->nsBox->setValue(ns_);
    ns = ns_;

    dir = new QDir();
    dir->cd(dir_->absolutePath());

    autoFlag = false;

    redCh = -1;
    blueCh = -1;

    if(ns == 41)
    {
        redCh = 21;
        blueCh = 22;
    }
    else if(ns == 22 || ns == 21)
    {
        redCh = 19;
        blueCh = 20;
    }

    flagWnd = 0;


    ui->nsBox->setValue(21);
    ui->eogSpinBox->setValue(2);
    ui->eogDoubleSpinBox->setValue(2.40);
    ui->eogDoubleSpinBox->setSingleStep(0.1);

    ui->drawNormDoubleSpinBox->setValue(1.);
    ui->drawNormDoubleSpinBox->setMaximum(10.);
    ui->drawNormDoubleSpinBox->setMinimum(0.01);
    ui->drawNormDoubleSpinBox->setSingleStep(0.1);
    ui->drawNormDoubleSpinBox->setDecimals(2);


    ui->nsBox->setMaximum(40);
    ui->checkBox->setChecked(true);
//    ui->timeShiftSpinBox->setMaximum(1000.);
//    ui->timeShiftSpinBox->setValue(125.);
//    ui->timeShiftSpinBox->setSingleStep(25.);

//    ui->wndLengthSpinBox->setValue(4.0);

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

    ui->picLabel->installEventFilter(this);

    ui->scrollArea->setWidget(ui->picLabel);
    ui->scrollArea->installEventFilter(this);

    ui->markersCheckBox->setChecked(true);


    QFileDialog *browser = new QFileDialog();
    browser->setDirectory(dir->absolutePath() + slash() + "Realisations");
    browser->setViewMode(QFileDialog::Detail);

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files);
    dir->cdUp();
    currentNumber=-1;

    QObject::connect(ui->browseButton, SIGNAL(clicked()), browser, SLOT(show()));
    QObject::connect(browser, SIGNAL(fileSelected(QString)), ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(browser, SIGNAL(fileSelected(QString)), this, SLOT(createImage(QString)));
//    QObject::connect(browser, SIGNAL(fileSelected(QString)), browser, SLOT(hide()));

    QObject::connect(this, SIGNAL(buttonPressed(char,int)), this, SLOT(mousePressSlot(char,int)));
    QObject::connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));
    QObject::connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    QObject::connect(ui->cutButton, SIGNAL(clicked()), this, SLOT(cut()));
    QObject::connect(ui->zeroButton, SIGNAL(clicked()), this, SLOT(zero()));
    QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
    QObject::connect(ui->rewriteButton, SIGNAL(clicked()), this, SLOT(rewrite()));
    QObject::connect(this, SIGNAL(openFile(QString)), ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(this, SIGNAL(openFile(QString)), this, SLOT(createImage(QString)));
    QObject::connect(ui->nsBox, SIGNAL(valueChanged(int)), this, SLOT(setNs(int)));
    QObject::connect(ui->cutEyesButton, SIGNAL(clicked()), this, SLOT(cutEyesAll()));
    QObject::connect(ui->splitButton, SIGNAL(clicked()), this, SLOT(splitCut()));

//    this->ui->tempSpinBox->setValue(10);
    this->setAttribute(Qt::WA_DeleteOnClose);


//    data3 = new double* [ns];         // array for all data[numOfChan][numOfTimePin]
//    for(int i = 0; i < ns; ++i)
//    {
//        data3[i] = new double [250*250];         // generality for 250 seconds
//    }
}

Cut::~Cut()
{
    delete ui;
}

void Cut::browse()
{
    QString helpString = QFileDialog::getOpenFileName((QWidget*)this, tr("Open realisation"), dir->absolutePath() + slash() + "Realisations");
    if(helpString.isEmpty())
    {
        QMessageBox::information((QWidget*)this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }
    ui->lineEdit->setText(helpString);
    createImage(helpString);
}

void Cut::resizeEvent(QResizeEvent * event)
{
    // adjust scrollArea size
    ui->scrollArea->setGeometry(ui->scrollArea->geometry().x(),
                                ui->scrollArea->geometry().y(),
                                event->size().width() - 10 * 2,
                                ui->scrollArea->geometry().height());
}

bool Cut::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->picLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
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
            double coeff = -1.2;
            ui->scrollArea->horizontalScrollBar()->setSliderPosition( ui->scrollArea->horizontalScrollBar()->sliderPosition() + coeff * scrollEvent->delta());
            return true;


            // zoom
            /*
            if(zoomPrev*(1.+scrollEvent->delta()/720.)>1.)
            {
                zoomCurr=zoomPrev*(1.+scrollEvent->delta()/720.);

                leftLimit=int(leftLimit*zoomCurr/zoomPrev);
                rightLimit=int(rightLimit*zoomCurr/zoomPrev);

                QPixmap *tmp = new QPixmap;
                QPainter *pnt = new QPainter;

                tmp->load(currentPicPath);
                pnt->begin(tmp);

                // picLabel varies
                // Pixmap & scrollArea are constant

                pnt->setPen(QPen(QBrush("blue"), 2));
                pnt->drawLine(leftLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), 0, leftLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), tmp->height());

                pnt->setPen(QPen(QBrush("red"), 2));
                pnt->drawLine(rightLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), 0, rightLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), tmp->height());
                pnt->end();


                ui->picLabel->setPixmap(tmp->scaled(ui->scrollArea->size().width()*zoomCurr, ui->scrollArea->size().height()-20));  // -20 generality
                zoomPrev=zoomCurr;

                delete tmp;
                delete pnt;
                return true;
            }


            else
            {
                return false;
            }
*/
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
    QString helpString;
    // automatization
    if(autoFlag)
    {
        ui->checkBox->setChecked(false);
        dir->cd("Realisations");
        helpString = dir->entryList(QDir::Files)[0];
        dir->cdUp();
        helpString.prepend(dir->absolutePath() + slash() + "Realisations" + slash());
        cout<<helpString.toStdString()<<endl;
        emit openFile(helpString);
    }


    dir->cd(ui->dirBox->currentText());  // generality
    nameFilters.clear();
    nameFilters << "*_241*";
    nameFilters << "*_247*";
    nameFilters << "*_254*"; // no need?

    lst = dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
    dir->cdUp();

    int NumEog = ui->eogSpinBox->value();
    double * thresholdEog = new double [NumEog];
    for(int i = 0; i < NumEog; ++i)
    {
        thresholdEog[i]=0.;
    }
//    cout<<"2"<<endl;

    double ** eogArray = new double* [NumEog];
    for(int i=0; i<NumEog; ++i)
    {
        eogArray[i] = new double [8192*50]; // 50, size generality
    }
    int currentSlice = 0;
//    cout<<"3"<<endl;

    for(int k = 0; k < int(lst.length()/4); ++k)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath()
                                              + slash() + ui->dirBox->currentText()
                                              + slash() + lst[rand()%lst.length()]);
        emit openFile(helpString);

        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int i = ns-NumEog; i < ns; ++i)
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
    helpString=QDir::toNativeSeparators(dir->absolutePath() + slash() + "eyesSlices");
    eyes = fopen(helpString.toStdString().c_str(), "w");

    int flag;
    int NumOfEogSlices = 0;
    int num=0;
    for(int i=0; i<lst.length(); ++i)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath() + slash() + ui->dirBox->currentText() + slash() + lst[i]);

        emit openFile(helpString);
        num += NumOfSlices;
        for(int i=0; i<NumOfSlices; ++i)
        {
            flag = 1;
            for(int j = 0; j < NumEog; ++j)
            {
                if(fabs(data3[ns - NumEog + j][i]) < ui->eogDoubleSpinBox->value()*thresholdEog[j] && !ui->markersCheckBox->isChecked()) flag = 0; // w/o markers
                if(fabs(data3[ns-1 - NumEog + j][i]) < ui->eogDoubleSpinBox->value()*thresholdEog[j] && ui->markersCheckBox->isChecked()) flag = 0; // with markers
            }
            if(flag==1)
            {
                for(int k=0; k<ns; ++k)
                {
                    fprintf(eyes, "%lf\n", data3[k][i]);
                }
                ++NumOfEogSlices;
            }
        }
    }

    fclose(eyes);

    QFile *file = new QFile(helpString=QDir::toNativeSeparators(dir->absolutePath() + slash() + "eyesSlices"));
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
    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
}

void Cut::setNs(int a)
{
    ns = a;
}

void Cut::createImage(QString dataFileName) //
{
    zoomPrev = 1.;
    zoomCurr = 1.;
    addNum = 1;
    dir->setPath(dataFileName);
    currentFile = dataFileName;
    fileName = dir->dirName(); // real file name after last separator

    leftLimit = 0;
    Eyes = 0;

    dir->cdUp(); // drop the file name
    lst = dir->entryList(QDir::Files);
    dir->cdUp(); // go into ExpName dir


    readPlainData(currentFile, data3, ns, NumOfSlices);

    currentNumber = lst.indexOf(fileName);
    currentPicPath = getPicPath(dataFileName, dir, ns);

    if(ui->checkBox->isChecked())
    {
        QPainter * painter = new QPainter();
        currentPic = drawEeg(data3,
                             ns,
                             NumOfSlices,
                             def::freq,
                             currentPicPath,
                             ui->drawNormDoubleSpinBox->value(),
                             blueCh,
                             redCh);
//        currentPic.load(currentPicPath, "JPG", Qt::ColorOnly);
        painter->begin(&currentPic);

        // initial zoom
        zoomPrev = 1.;
        zoomCurr = NumOfSlices/double(ui->scrollArea->width()); // generality
        ui->picLabel->setPixmap(currentPic.scaled(currentPic.width(), ui->scrollArea->height() - 20)); // -20 generality
        rightLimit = NumOfSlices;

        painter->end();
        delete painter;


        /*
        // picLabel varies
        // Pixmap & scrollArea are constant.
        if(zoomCurr > 1.)
        {
            ui->picLabel->setPixmap(currentPic.scaled(ui->scrollArea->size().width()*zoomCurr, ui->scrollArea->size().height()-20));  // -20 generality
            zoomPrev = zoomCurr;
            rightLimit = NumOfSlices;
        }
        else
        {
            zoomCurr = 1.;
//            ui->picLabel->setPixmap(currentPic.scaled(ui->scrollArea->size().width(), ui->scrollArea->size().height()-20));  // -20 generality
            ui->picLabel->setPixmap(currentPic.scaled(currentPic.size().width(), ui->scrollArea->size().height()-20));  // -20 generality
            rightLimit = ui->scrollArea->size().width();
        }

        */
        // endof initial zoom
    }
    else
    {
        rightLimit = 0;
    }
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(0);
}

void Cut::mousePressSlot(char btn, int coord)
{
    if(btn == 'l' && coord < rightLimit) leftLimit = coord;
    if(btn == 'r' && coord > leftLimit && coord < NumOfSlices) rightLimit = coord;


    QPixmap pic = currentPic;
    QPainter * painter = new QPainter();
    painter->begin(&pic);

    painter->setPen(QPen(QBrush("blue"), 2));
    painter->drawLine(leftLimit, 0, leftLimit, currentPic.height());
    painter->setPen(QPen(QBrush("red"), 2));
    painter->drawLine(rightLimit, 0, rightLimit, currentPic.height());

    ui->picLabel->setPixmap(pic.scaled(pic.width(), ui->scrollArea->height() - 20)); // -20 generality

    painter->end();
    delete painter;
}

void Cut::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

void Cut::next()
{
//    rewrite();
    QString helpString;
    if(currentNumber+1 < lst.length())  // generality
    {
        helpString = dir->absolutePath() + slash();
        if(currentFile.contains("Realisations"))
        {
             helpString += "Realisations";
        }
        else if(currentFile.contains("windows") && !currentFile.contains("fromreal"))
        {
            helpString += "windows";
        }
        else if(currentFile.contains("cut"))
        {
            helpString += "cut";
        }
        helpString += slash();

        if(lst[currentNumber+1].contains("_num")) ++currentNumber; // generality crutch bicycle

        helpString += lst[currentNumber+1];
        emit openFile(helpString);               // sets dir into ExpName directory
    }
    else
    {
        cout << "bad number, too big" << endl; // QMessageBox
    }
}

void Cut::prev()
{
//    rewrite();
    QString helpString;
    if(currentNumber-1 >= 0)
    {
        helpString = dir->absolutePath() + slash();
        if(currentFile.contains("Realisations"))
        {
             helpString += "Realisations";
        }
        else if(currentFile.contains("windows") && !currentFile.contains("fromreal"))
        {
            helpString += "windows";
        }
        else if(currentFile.contains("cut"))
        {
            helpString += "cut";
        }
        helpString += slash();


        if(lst[currentNumber-1].contains("_num")) --currentNumber; // generality crutch bicycle

        helpString += lst[currentNumber-1];
        emit openFile(helpString);          // sets dir into ExpName directory
    }
    else
    {
        cout << "bad number, too little" << endl; // QMessageBox
    }
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

    while (!matiCountBit(data3[ns - 1][newLeftLimit], 14) && newLeftLimit > 0)
    {
        --newLeftLimit;
    }
    while (!matiCountBit(data3[ns - 1][newRightLimit], 14) && newRightLimit < NumOfSlices)
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
    if(newLeftLimit > 0 || matiCountBit(data3[ns - 1][0], 14))
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
            while (!matiCountBit(data3[ns - 1][leftLimit], 14)) // there ARE count answers
            {
                --leftLimit;
            }
            ++leftLimit;
            cout << "prev file leftLimit = " << leftLimit << endl;
            // zero() from tempLimit to rightLimit
            zeroData(data3, ns, leftLimit, rightLimit);
            rewrite();
            next();
        }
    }


    if(newRightLimit < NumOfSlices)
    {
        ++newRightLimit; // after the previous marker
    }
    else if (matiCountBit(data3[ns - 1][NumOfSlices - 1], 14))
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
            while (!matiCountBit(data3[ns - 1][rightLimit], 14)) // there ARE count answers
            {
                ++rightLimit;
            }
            ++rightLimit;
            cout << "next file rightLimit = " << rightLimit << endl;

            zeroData(data3, ns, leftLimit, rightLimit);
            rewrite();
        }
        prev();
    }
    leftLimit = newLeftLimit;
    rightLimit = newRightLimit;

}

void Cut::zero()
{
    int h = 0;

    // if MATI with counts - adjust limits to problem edges
    // move leftLimit after the nearest marker
    // move rightLimit after the nearest marker
    // delete [leftLimit, rightLimit)

    // ExpName.left(3)_rr_f_TYPE_SESSION_PIECE.MARKER
    QString helpString = "_0_[0-9]_[0-9]{2,2}";
    if(currentFile.contains(QRegExp(helpString)))
    {
        cout << "zero: adjust limits   " << currentFile << endl;
//        matiAdjustLimits();
    }

    zeroData(data3, ns, leftLimit, rightLimit); ///////// should TEST !!!!!!!1111
    paint();
}

void Cut::cut()
{
    QString helpString;
    dir->cd("windows");
    helpString = QDir::toNativeSeparators(dir->absolutePath()
                                          + slash() + fileName
                                          + "." + QString::number(addNum));
    ++addNum;
    // new
    writePlainData(helpString, data3, int(rightLimit-leftLimit), leftLimit);
    // old
    /*
    file = fopen(helpString.toStdString().c_str(),"w");
    fprintf(file, "NumOfSlices %d\n", int(rightLimit-leftLimit));
    for(int i = leftLimit; i < rightLimit; ++i)         // zoom
    {
        for(int k = 0; k < ns; ++k)
        {
            fprintf(file, "%lf\n", data3[k][i]);
        }
    }
    fclose(file);
    */
    dir->cdUp();
    rightLimit = NumOfSlices;
    leftLimit = 0;
    paint();
}

void Cut::splitCut()
{
    int & leftEdge = leftLimit;
    int & rightEdge = rightLimit;

    for(int i = leftEdge; i < NumOfSlices - (rightEdge - leftEdge); ++i)         // zoom
    {
        for(int k = 0; k < ns; ++k)
        {
            if(k == ns-1 && def::withMarkersFlag && (i==0)) // first marker value
            {
                continue;
            }
            data3[k][i] = data3[k][i + (rightEdge - leftEdge)];
        }
    }
    NumOfSlices -= (rightEdge - leftEdge);

    paint();
}

void Cut::save()
{
    QString helpString;
    dir->setPath(currentFile);  // .../expName/Realisations/fileName;
    fileName = dir->dirName();    // real fileName
    dir->cdUp();
    dir->cdUp();

    helpString = QDir::toNativeSeparators(dir->absolutePath()
                                          + slash() + "cut"
                                          + slash() + fileName);

    // new
    writePlainData(helpString, data3, NumOfSlices, ns);
    // old
    /*
    file = fopen(helpString.toStdString().c_str(), "w");
    fprintf(file, "NumOfSlices %d \n", NumOfSlices);
    for(int i = 0; i < NumOfSlices; ++i)         // saved BY SLICES!!
    {
        for(int k = 0; k < ns; ++k)
        {
            fprintf(file, "%lf\n", data3[k][i]);
        }
    }
    fclose(file);
    */

    fileName.replace('.', '_');
    helpString=QDir::toNativeSeparators(dir->absolutePath()
                                        + slash() + "SignalsCut"
                                        + slash());
    if(ns==19)
    {
         helpString += "after";
    }
    else if(ns==21)
    {
        helpString += "before";
    }
    else
    {
        helpString += "other";
    }
    helpString += slash() + fileName + ".jpg";

    QPixmap pic;
    pic = *(ui->picLabel->pixmap());
    pic.save(helpString, 0, 100);
}


void Cut::rewrite()
{
    writePlainData(currentFile,
                   data3,
                   ns,
                   NumOfSlices);
    currentPicPath = getPicPath(currentFile, dir, ns);
    currentPic.save(currentPicPath, 0, 100);
}

void Cut::paint() // save to tmp.jpg and display
{
    QString helpString;
    helpString = dir->absolutePath() + slash() + "tmp.jpg";

    currentPic = drawEeg(data3,
                         ns,
                         NumOfSlices,
                         def::freq,
                         helpString,
                         ui->drawNormDoubleSpinBox->value(),
                         blueCh,
                         redCh); // generality freq

//    helpString = getPicPath(currentFile, dir, ns);
//    currentPic.load(helpString);

    ui->picLabel->setPixmap(currentPic.scaled(currentPic.width(), ui->scrollArea->height() - 20)); // -20 generality

    rightLimit = NumOfSlices;
    leftLimit = 0;
}

