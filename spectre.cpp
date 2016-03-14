#include "spectre.h"
#include "ui_spectre.h"

Spectre::Spectre() :
    ui(new Ui::Spectre)
{
    ui->setupUi(this);

    this->setWindowTitle("Spectra Counter");

    backupDirPath = def::dir->absolutePath();

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

    ui->scalingDoubleSpinBox->setValue(1.0);
    ui->scalingDoubleSpinBox->setSingleStep(0.05);

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
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i].resize(2);
    }

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


    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i][0] = 0;
        rangeLimits[i][1] = def::spLength();
    }

}

void Spectre::defaultState()
{
    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "Realisations");
    ui->lineEdit_1->setText(helpString);
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "SpectraSmooth");   //smooth right after spectra count
    ui->lineEdit_2->setText(helpString);
    ui->fftComboBox->setCurrentIndex(2); //4096
    ui->spectraRadioButton->setChecked(true); // count FFT
}

void Spectre::setFftLength(int i)
{
    ui->fftComboBox->setCurrentText(QString::number(i));
}

void Spectre::setPow(double a)
{
    ui->powDoubleSpinBox->setValue(a);
}

int findChannel(int x, int y, QSize siz)
{
    int a, b, num;
    a = floor( x * 16./double(siz.width())/3. );
    b = floor( y * 16./double(siz.height())/3. );
    num = 0;
    switch(b)
    {
    case 0:
        {
            if(a==1) return 0;
            if(a==3) return 1;
        }
    case 1:{num+=2; break;}
    case 2:{num+=7; break;}
    case 3:{num+=12; break;}
    case 4:
        {
            if(a==1) return 17;
            if(a==3) return 18;
        }
    }
    switch(a)
    {
    case 0:{num+=0; break;}
    case 1:{num+=1; break;}
    case 2:{num+=2; break;}
    case 3:{num+=3; break;}
    case 4:{num+=4; break;}
    }
    return num;
}

bool Spectre::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->specLabel) //this is magic
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            int chanNum;
            QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
            if(fmod(16 * double(mouseEvent->y()) / ui->specLabel->height(), 3.) < 0.5)
            {
                return false;
            }

            if(fmod(16 * double(mouseEvent->x()) / ui->specLabel->width(), 3.) < 0.5)
            {
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size());
                    rangeLimits[chanNum][0] = 0;
                }
                if(mouseEvent->button() == Qt::RightButton)
                {

                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size()) - 1;
                    rangeLimits[chanNum][1] = def::spLength();
                }
                return true;
            }

            QPixmap pic;
            QPainter paint;
            pic.load(rangePicPath, 0, Qt::ColorOnly);
            paint.begin(&pic);


            chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size());

            if(mouseEvent->button()==Qt::LeftButton)
            {

                rangeLimits[chanNum][0] = floor((mouseEvent->x()
                                                 * paint.device()->width()
                                                 / ui->specLabel->width()
                                                 - coords::x[chanNum]
                                                 * paint.device()->width())
                                                / (coords::scale * paint.device()->width() )
                                                * def::spLength());

            }
            if(mouseEvent->button()==Qt::RightButton)
            {
                rangeLimits[chanNum][1] = ceil((mouseEvent->x()
                                                * paint.device()->width()
                                                / ui->specLabel->width()
                                                - coords::x[chanNum]
                                                * paint.device()->width())
                                               / (coords::scale * paint.device()->width())
                                               * def::spLength());
            }
            for(int i = 0; i < def::ns; ++i)
            {
                paint.setPen(QPen(QBrush("blue"), 2));
                paint.drawLine(QPointF(coords::x[i]
                                        * paint.device()->width()
                                        + rangeLimits[i][0]
                                * coords::scale
                                * paint.device()->width()
                                / def::spLength(),

                                coords::y[i]
                                * paint.device()->height()),

                        QPointF(coords::x[i]
                                * paint.device()->width()
                                + rangeLimits[i][0]
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
                                        + rangeLimits[i][1]
                                * coords::scale
                                * paint.device()->width()
                                / def::spLength(),

                                coords::y[i]
                                * paint.device()->height()),

                        QPointF(coords::x[i]
                                * paint.device()->width()
                                + rangeLimits[i][1]
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
    vector<int> begins(numOfInt);
    vector<int> ends(numOfInt);
    QString helpString;
    QStringList nameFilters;
    for(const QString & item : lst)
    {
        nameFilters = item.split('-', QString::SkipEmptyParts);

        begins.push_back(max(fftLimit(nameFilters[0].toDouble()) - def::left() + 1,
                         0));
        ends.push_back(min(fftLimit(nameFilters[1].toDouble()) - def::left() + 1,
                         def::spLength()));
    }

    matrix dataInt(def::nsWOM(), def::spLength());
    matrix dataOut(def::nsWOM(), numOfInt, 0.);

    makeFullFileList(ui->lineEdit_1->text(), lst);

    for(const QString & fileName : lst)
    {

        helpString = QDir::toNativeSeparators(ui->lineEdit_1->text()
                                              + slash() + fileName);
        readMatrixFile(helpString,
                        dataInt);

        for(int h = 0; h < dataOut.rows(); ++h)
        {
            for(int j = 0; j < dataOut.cols(); ++j)
            {
                /// accumulate
                for(int k = begins[j]; k <= ends[j]; ++k) // < or <= not really important
                {
                    dataOut[h][k] += dataInt[h][k];
                }
                dataOut /= 10.; // just for fun?
            }
        }
        helpString = QDir::toNativeSeparators(ui->lineEdit_2->text()
                                              + slash() + fileName);
        writeMatrixFile(helpString,
                        dataOut);
    }

}

void Spectre::psaSlot()
{
    QTime myTime;
    myTime.start();

    matrix drawData;
    lineType tempVec(def::spLength() * def::nsWOM());
    QString helpString;
    const QString psaPath = def::dir->absolutePath()
                            + slash() + "Help"
                            + slash() + "psa";

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        helpString = QDir::toNativeSeparators(psaPath
                                              + slash() + def::ExpName
                                              + "_class_" + QString::number(i + 1)
                                              + ".psa");
        readFileInLine(helpString, tempVec);

        drawData.push_back(tempVec);
    }

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName + "_all.jpg");

    trivector<int> MW;



    if(ui->MWcheckBox->isChecked())
    {
        countMannWhitney(MW,
                         ui->lineEdit_1->text());
    }

    if(drawData.cols() == 19 * def::spLength() ||
       drawData.cols() == 21 * def::spLength())
    {
        drawTemplate(helpString);

        vector<QColor> colors;
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

        def::drawNorm = -1;
        def::drawNorm = drawArrays(helpString,
                                   drawData,
                                   false,
                                   def::drawNormTyp,
                                   def::drawNorm,
                                   colors);
        if(ui->MWcheckBox->isChecked())
        {
            drawMannWitney(helpString,
                           MW,
                           colors);
        }
    }
    else if(def::OssadtchiFlag)
    {
        drawArraysInLine(helpString,
                         drawData);
        if(ui->MWcheckBox->isChecked())
        {
            drawMannWitneyInLine(helpString,
                                 MW);
        }
    }
    setFftLengthSlot();
    ui->specLabel->setPixmap(QPixmap(helpString).scaled(ui->specLabel->size()));
    cout << "psaSlot: time elapsed " << myTime.elapsed() / 1000. << " sec" << endl;
}

void Spectre::compare()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    vector<QStringList> leest;

    lineType tempVec(def::spLength() * def::nsWOM());
    lineType meanVec(0., def::spLength() * def::nsWOM());

    const QString filesPath = ui->lineEdit_1->text();
    const QString savePath = def::dir->absolutePath()
                             + slash() + "Help"
                             + slash() + "psa";

    makeFileLists(filesPath,
                  leest);


    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        const QStringList & lst = leest[i];
        const int NumOfPatterns = lst.length();
        meanVec = lineType(0., def::spLength() * def::nsWOM());


        for(int j = 0; j < NumOfPatterns; ++j)
        {
            helpString = QDir::toNativeSeparators(filesPath
                                                  + slash()
                                                  + lst[j]);
            readFileInLine(helpString, tempVec);

            /// make valarray
            meanVec += tempVec;
        }
        meanVec /= NumOfPatterns;

        // psa name
        helpString = QDir::toNativeSeparators(savePath
                                              + slash() + def::ExpName
                                              + "_class_" + QString::number(i + 1)
                                              + ".psa");
        /// maybe make as spectraFile?
        writeFileInLine(helpString, meanVec);
#if 0
        // draw average for one type
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Help"
                                              + slash() + def::ExpName
                                              + "_class_" + QString::number(i + 1)
                                              + ".jpg");

        //        cout << helpString << endl;
        drawTemplate(helpString);
        drawArray(helpString, meanVec);
#endif
    }

    cout << "compare: time elapsed " << myTime.elapsed() / 1000. << " sec" << endl;
}

void Spectre::setFftLengthSlot()
{
    def::fftLength = ui->fftComboBox->currentText().toInt();

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() +"Realisations");
    ui->lineEdit_1->setText(helpString);

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "SpectraSmooth");
    ui->lineEdit_2->setText(helpString);

    if(ui->fftComboBox->currentIndex() == 0) //1024
    {
        ui->smoothBox->setValue(4);
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "windows");
        helpString += slash() + "fromreal";


        ui->lineEdit_1->setText(helpString);
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "SpectraSmooth"
                                              + slash() + "windows");
        ui->lineEdit_2->setText(helpString);
    }
    else if(ui->fftComboBox->currentIndex() == 1) //2048
    {
        ui->smoothBox->setValue(10);
    }
    else if(ui->fftComboBox->currentIndex() == 2) //4096
    {
//        ui->smoothBox->setValue(10);
        ui->smoothBox->setValue(15);
    }
    else if(ui->fftComboBox->currentIndex() == 3) //8192
    {
        ui->smoothBox->setValue(15);
    }

    ui->leftSpinBox->setValue(def::left());
    ui->rightSpinBox->setValue(def::right());

//    cout << endl << def::fftLength << endl;
//    cout << def::left() << endl;
//    cout << def::right() << endl;
//    cout << def::spLength() << endl;
//    cout << ui->lineEdit_1->text() << endl;
//    cout << ui->lineEdit_2->text() << endl << endl;

    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i][0] = 0;
        rangeLimits[i][1] = def::spLength();
    }
//    cout << "setFftLengthSlot: spLength = " << def::spLength() << endl;

}

void Spectre::center()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose dir"),
                                                           def::dir->absolutePath());
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
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + fileName);
        readMatrixFile(helpString,
                        tempData);
        averages += tempData;
    }
    averages /= lst.length();

    /// divide by sigma in each sample. valarray

    for(const QString & fileName : lst)
    {
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + fileName);
        readMatrixFile(helpString,
                        tempData);
        tempData -= averages;
        tempData *= 10.;
        writeMatrixFile(helpString,
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
    ui->rightHzEdit->setText(QString::number(def::rightFreq));
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i][1] = def::spLength();
    }
}

void Spectre::setLeft()
{
    /// changed
    def::leftFreq = ui->leftSpinBox->value() * def::spStep();
    ui->leftHzEdit->setText(QString::number(def::leftFreq));
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        rangeLimits[i][1] = def::spLength();
    }
}


void Spectre::writeSpectra(const double leftFreq,
                           const double rightFreq,
                           const bool rangeLimitCheck)
{

    QTime myTime;
    myTime.start();

    ofstream outStream;
    QString helpString;
    const QString outDirPath = ui->lineEdit_2->text();

    const int left = fftLimit(leftFreq);
    const int right = fftLimit(rightFreq) + 1;

//    cout << leftFreq << '\t' << rightFreq << endl;
//    cout << left << '\t' << right << endl;
//    for(int i = 0; i < def::nsWOM(); ++i)
//    {
//        cout << rangeLimits[i][0] << '\t' << rangeLimits[i][1] << endl;
//    }

    for(int i = 0; i < fileNames.size(); ++i)
    {
        helpString = outDirPath + slash() + fileNames[i];

        outStream.open(helpString.toStdString());
        if(!outStream.good())
        {
            cout << "bad outStream" << endl;
            continue;
        }
        outStream << "NumOfChannels " << def::nsWOM() << '\t';
        outStream << "spLength " << right - left << endl;

        if(ui->spectraRadioButton->isChecked())
        {
            if(rangeLimitCheck)
            {
                for(int j = 0; j < def::nsWOM(); ++j) //
                {
                    for(int k = left; k < right; ++k) // [left, right)
                    {
                        if((k - def::left()) >= rangeLimits[j][0] &&
                           (k - def::left()) <= rangeLimits[j][1])
                            outStream << doubleRound(dataFFT[i][j][k], 4) << '\t';
                        else
                            outStream << "0.000" << '\t';
                    }
                    outStream << '\n';
                }
            }
            else
            {
                for(int j = 0; j < def::nsWOM(); ++j) //
                {
                    for(int k = left; k < right; ++k) // [left, right)
                    {
                        outStream << doubleRound(dataFFT[i][j][k], 4) << '\t';
                    }
                    outStream << '\n';
                }
            }
        }
        outStream.close();
    }

    cout << "writeSpectra: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;
}
void Spectre::countSpectraSlot()
{
    setFftLengthSlot();
    countSpectra();
    writeSpectra();


#if 0
    /// if clean
    cleanSpectra(); // using mann-whitney
#endif

    ui->lineEdit_1->setText(ui->lineEdit_2->text());
    ui->lineEdit_2->setText(def::dir->absolutePath() + slash() + "Help");
}

void Spectre::cleanSpectra()
{
    QTime myTime;
    myTime.start();


    cout << ui->lineEdit_2->text() << endl;
    trivector<int> MW;
    countMannWhitney(MW,
                     ui->lineEdit_2->text()); // SpectraSmooth
    int num;
    int cnt = 0;
    for(int k = 0; k < def::spLength() * def::nsWOM(); ++k)
    {
//        cout << "spPoint = " << k << endl;
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
    cout << "cleanSpectra: num of zeroed points = " << cnt << endl;
    ui->lineEdit_2->setText(ui->lineEdit_2->text() + slash() + "Clean");
    writeSpectra();
    cout << "cleanSpectra: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;

}

void Spectre::countSpectra()
{
    QTime myTime;
    myTime.start();


    const QString inDirPath = ui->lineEdit_1->text();
    QStringList lst;
//    makeFullFileList(inDirPath, lst, {"_test"});
    makeFullFileList(inDirPath, lst);
    const int numFiles = lst.length();

//    cout << inDirPath << endl;
//    cout << numFiles << endl;

    matrix dataIn;
    QString helpString;
    int NumOfSlices;

    dataFFT.resize(numFiles);
    for(matrix & datum : dataFFT)
    {
        datum.resize(def::ns, def::fftLength / 2, 0.);
    }

    fileNames.resize(numFiles);
    std::copy(begin(lst), end(lst), fileNames.begin());

    int cnt = 0;
    vector<int> exIndices;
    for(int i = 0; i < numFiles; ++i)
    {
        const QString & fileName = fileNames[i];
        if(fileName.contains("_num") || fileName.contains("_300") || fileName.contains("_sht")) continue;

        //read data file
        helpString = QDir::toNativeSeparators(inDirPath
                                              + slash() + fileName);
        readPlainData(helpString,
                      dataIn,
                      NumOfSlices);



        if(ui->brainRateRadioButton->isChecked() || ui->spectraRadioButton->isChecked())
        {
            if(countOneSpectre(dataIn, dataFFT[cnt]))
            {
                ++cnt;
            }
            else
            {
                exIndices.push_back(i);
            }


#if 0
            else if(ui->brainRateRadioButton->isChecked())
            {
                // write brainRate
                for(int j = 0; j < def::nsWOM(); ++j)
                {
                    double sum1 = 0.;
                    double sum2 = 0.;
                    for(int k = def::left(); k < def::right(); ++k)
                    {
                        sum1 += dataFFT[i][j][k];
                        sum2 += dataFFT[i][j][k] * (k * def::spStep());
                    }
                    sum2 /= sum1; /// sum2 is the result
                }
            }
#endif
        }
        else if(ui->hilbertsVarRadioButton->isChecked())
        {
            QMessageBox::critical(this,
                                  tr("error"),
                                  tr("look at code, fix split zeros"),
                                  QMessageBox::Ok);
            return;
        }
        else if(ui->bayesRadioButton->isChecked())
        {
            //clean from zeros
            QMessageBox::critical(this,
                                  tr("error"),
                                  tr("look at code, fix split zeros"),
                                  QMessageBox::Ok);
            return;
        }
        else if(ui->d2RadioButton->isChecked())
        {
//            splitZerosEdges(dataIn, def::ns, def::fftLength, &NumOfSlices);
//            for(int i = 0; i < def::ns; ++i)
//            {
//                outStream << fractalDimension(dataIn[i]) << '\n';
//            }
        }
        ui->progressBar->setValue(i * 100. / numFiles);
        qApp->processEvents();
    }

    eraseItems(fileNames, exIndices);
    dataFFT.resize(cnt);

    ui->progressBar->setValue(0);


    //generality
    cout << "countSpectra: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;

}

bool Spectre::countOneSpectre(matrix & data2, matrix & outData)
{
    //correct Eyes number and dataLen
    int Eyes = 0;
    if(data2.cols() < def::fftLength)
    {
        data2.resizeCols(def::fftLength); /// clever resizing in matrix.cpp
    }
    else
    {
        // take the last def::fftLength samples
        const int a = def::fftLength;

        std::for_each(data2.begin(),
                      data2.end(),
                      [a](lineType & in)
        {
            lineType newArr = in[std::slice(in.size() - a,
                                            a,
                                            1)];
            in = newArr;
        });
    }

    int h = 0;
    for(int i = 0; i < def::fftLength; ++i)
    {
        h = 0;
        for(int j = 0; j < def::nsWOM(); ++j)
        {
            if(fabs(data2[j][i]) <= 0.125) ++h; // generality 1/8.
        }
        if(h == def::nsWOM()) Eyes += 1;
    }

    //generality
    if(def::fftLength - Eyes < def::freq * 2.8) // real signal less than 2.5 seconds
    {
        return false;
    }

    for(int i = 0; i < data2.rows(); ++i)
    {
        calcSpectre(data2[i],
                    outData[i],
                    def::fftLength,
                    ui->smoothBox->value(),
                    Eyes);
    }
    return true;
}


int Spectre::readFilePhase(double ** data2, double ***dataPhase)
{
#if 0
        int h = 0;

        def::dir->cd(ui->lineEdit_1->text());

        //correct Eyes number
        Eyes = 0;
        int NumOfSlices = def::fftLength;
        for(int i = 0; i < NumOfSlices; ++i)
        {
            h = 0;
            for(int j = 0; j < def::ns; ++j)
            {
                if(fabs(data2[j][i]) <= 0.07) ++h;
            }
            if(h == def::ns) Eyes+=1;
        }

        if((NumOfSlices-Eyes) < 500) // 0.2*4096/250 = 3.1 sec
        {
            cout << "Too short real signal " << endl;// << NumOfSlices << "  " << Eyes << endl << endl;


            return 0;
        }
    //    cout << "!!" << endl;


        double norm1 = def::fftLength / double(def::fftLength - Eyes);              //norm with eyes


        double ** spectre = new double *[2];
        spectre[0] = new double [def::fftLength * 2];
        spectre[1] = new double [def::fftLength * 2];

        double * help = new double [2];
//        int leftSmoothLimit, rightSmoothLimit;

        for(int c1 = 0; c1 < def::ns; ++c1)
        {
            for(int i = 0; i < def::fftLength; ++i)            //make appropriate array
            {
                spectre[0][ i * 2 ] = (double)(data2[c1][ i ]*sqrt(norm1));
                spectre[0][ i * 2 + 1 ] = 0.;
            }
            four1(spectre[0] - 1, def::fftLength, 1);        //Fourier transform

            for(int c2 = c1+1; c2 < def::ns; ++c2)
            {


                for(int i = 0; i < def::fftLength; ++i)            //make appropriate array
                {
                    spectre[1][ i * 2 ] = (double)(data2[c2][ i ]*sqrt(norm1));
                    spectre[1][ i * 2 + 1 ] = 0.;
                }
                four1(spectre[1] - 1, def::fftLength, 1);        //Fourier transform

                for(int i = def::left(); i < def::right() + 2; ++i )      //get the absolute value of FFT
                {
                    dataPhase[c1][c2][ i ] = atan(spectre[0][ i * 2] / spectre[0][ i * 2 + 1])
                            - atan(spectre[1][ i * 2] / spectre[1][ i * 2 +1]); //!!!!!!!!!!atan(Im/Re)
                    help[0] = ( pow(spectre[0][ i * 2 + 1 ], 2)
                            + pow(spectre[0][ i * 2 ], 2)) * 2 / def::freq / def::fftLength;
                    help[1] = ( pow(spectre[1][ i * 2 + 1 ], 2)
                            + pow(spectre[1][ i * 2 ], 2)) * 2 / def::freq / def::fftLength;

                    dataPhase[c1][c2][ i ] *= sqrt(help[0] * help[1]) / (help[0] + help[1]); //normalisation
                    if(QString::number(dataPhase[c1][c2][i]).contains('n')) //why nan and inf???
                    {
                        cout << "dataPhase[" << c1 << "][" << c2 << "][" << i << "] = " <<  dataPhase[c1][c2][i]  << endl;
                        return 0;
                    }
                }
            }

        }





        for(int i = 0; i < def::ns; ++i)
        {
            delete []data2[i];
        }
        delete []spectre[0];
        delete []spectre[1];
        delete []spectre;
        delete []data2;
        delete []help;
        def::dir->cd(backupDirPath);
        return 1;
#endif
}

void Spectre::drawWavelets()
{
    QString helpString;
    QString filePath;
    QDir localDir(def::dir->absolutePath());
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "visualisation"
                                          + slash() + "wavelets");
    localDir.cd(helpString);

    //make dirs
    for(int i = 0; i < def::nsWOM(); ++i)
    {
        localDir.mkdir(QString::number(i));

        if(ui->phaseWaveletButton->isChecked())
        {
            //for phase
            localDir.cd(QString::number(i));
            for(int j = 0; j < def::ns; ++j)
            {
                localDir.mkdir(QString::number(j));
            }
            localDir.cdUp();
        }
    }

    QStringList lst;
    makeFullFileList(ui->lineEdit_1->text(), lst);

    matrix signal;
    matrix coefs;
    ofstream outStr;

    set<double, std::greater<double>> tempVec;

    // count maxValue
    int NumOfSlices = 0;
    for(const QString & fileName : lst)
    {
        filePath = QDir::toNativeSeparators(ui->lineEdit_1->text()
                                            + slash() + fileName);
        readPlainData(filePath,
                      signal,
                      NumOfSlices);

        for(int chanNum = 0; chanNum < def::nsWOM(); ++chanNum)
        {
            coefs = countWavelet(signal[chanNum]);
            tempVec.emplace(coefs.maxVal());
            continue;

            helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "visualisation"
                                                  + slash() + "wavelets"
                                                  + slash() + QString::number(chanNum) + ".txt");

            outStr.open(helpString.toStdString(), ios_base::app);
            outStr << coefs.maxVal() << endl;
            outStr.close();
        }
    }

//    for(int chanNum = 0; chanNum < def::nsWOM(); ++chanNum)
//    {
//        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
//                                              + slash() + "visualisation"
//                                              + slash() + "wavelets"
//                                              + slash() + QString::number(chanNum) + ".txt");
//        readFileInLine(helpString, tempVec);
//        std::sort(tempVec.begin(), tempVec.end());
//        cout << tempVec.front() << "\t" << tempVec.back() << endl;
//    }
    return;


    for(auto it = lst.begin(); it != lst.end(); ++it)
    {
        const QString & fileName = (*it);
        filePath = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + fileName);
        if(ui->amplitudeWaveletButton->isChecked())
        {
            for(int channel = 0; channel < def::nsWOM(); ++channel)
            {
                helpString = fileName;
                helpString.replace('.', '_');
                helpString = QDir::toNativeSeparators(backupDirPath
                                                      + slash() + "visualisation"
                                                      + slash() + "wavelets"
                                                      + slash() + QString::number(channel)
                                                      + slash() + helpString
                                                      + "_wavelet_" + QString::number(channel)
                                                      + ".jpg");
                cout << helpString.toStdString() << endl;
                wavelet(filePath, helpString, channel, def::ns);
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
                    helpString = QDir::toNativeSeparators(backupDirPath
                                                          + slash() + "visualisation"
                                                          + slash() + "wavelets"
                                                          + slash() + QString::number(channel1)
                                                          + slash() + QString::number(channel2)
                                                          + slash() + helpString
                                                          + "_wavelet_" + QString::number(channel1)
                                                          + "_" + QString::number(channel2)
                                                          + ".jpg");
                    cout << helpString.toStdString() << endl;
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

