#ifndef EDITEDF_CPP
#define EDITEDF_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"



void MainWindow::makeChanList(vector<int> & chanList)
{
    chanList.clear();
    QStringList lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(lst.last().toInt() - 1 != globalEdf.getMarkChan())
    {
        cout << "makeChanList: no markers channel" << endl;
        ///
//        return;
    }
    for(auto str : lst)
    {
        chanList.push_back(str.toInt() - 1);
    }
}


void MainWindow::cleanEdfFromEyesSlot()
{
    readData();
    globalEdf.cleanFromEyes();
    QString helpString = def::dir->absolutePath()
                         + slash() + def::ExpName + "_eyesClean.edf";
    globalEdf.writeEdfFile(helpString);
}

void MainWindow::rereferenceDataSlot()
{
    QString helpString = def::dir->absolutePath() + slash() + def::ExpName + ".edf"; //ui->filePathLineEdit->text()
    helpString.replace(".edf", "_rr.edf");
    rereferenceData(ui->rereferenceDataComboBox->currentText(), helpString);
}

void MainWindow::rereferenceData(QString newRef, QString newPath)
{
    //A1, A2, Ar, Cz, N
    //A1-A2, A1-N
    // Ar means 0.5*(A1+A2)

    QTime myTime;
    myTime.start();

    QString helpString;
    QString helpString2;

    readData();

    helpString.clear();
    for(int i = 0; i < def::ns; ++i)
    {
        helpString += QString::number(i + 1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);

    std::vector<int> chanList(def::ns);
    std::iota(chanList.begin(),
              chanList.end(),
              0);

    int groundChan = -1; //A1-N
    int earsChan1 = -1; //A1-A2
    int earsChan2 = -1; //A2-A1
    for(int i = 0; i < def::ns; ++i)
    {
        if(label[i].contains("A1-N"))
        {
            groundChan = i;
        }
        else if(label[i].contains("A1-A2"))
        {
            earsChan1 = i;
        }
        else if(label[i].contains("A2-A1"))
        {
            earsChan2 = i;
        }
    }
    if(groundChan == -1 || (earsChan1 == -1 && earsChan2 == -1))
    {
        cout << "some of reref channels are absent" << endl;
        return;
    }

    int earsChan = (earsChan1 != -1) ? earsChan1 : earsChan2;

    vector<QString> sign;
    if(earsChan1 != -1)
    {
        sign = {"-", "+"};
    }
    else
    {
        sign = {"+", "-"};
    }
    helpString.clear();
    if(newRef == "A1")
    {
        for(int i = 0; i < def::ns; ++i) //ns -> 21
        {

            if(ui->eogAsIsCheckBox->isChecked() && label[i].contains("EOG"))
            {
                helpString += QString::number(i+1) + " "; continue;
            }

            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!label[i].contains("-A1")) //generality
            {
                if(label[i].contains("-A2"))
                {
                    helpString += QString::number(i+1) + sign[0] + QString::number(earsChan + 1);
                }
                else if(label[i].contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan + 1);
                }
                else if(label[i].contains("-Ar"))
                {
                    helpString += QString::number(i+1) + sign[0] + QString::number(earsChan + 1) + "/2";
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else // EOG and a shet
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    else if(newRef == "A2")
    {
        for(int i = 0; i < def::ns; ++i) //ns -> 21
        {
            if(ui->eogAsIsCheckBox->isChecked() && label[i].contains("EOG"))
            {
                helpString += QString::number(i+1) + " "; continue;
            }
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!label[i].contains("-A2")) //generality
            {
                if(label[i].contains("-A1"))
                {
                    helpString += QString::number(i+1) + sign[1] + QString::number(earsChan+1);
                }
                else if(label[i].contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1) + "+" + QString::number(earsChan1+1);
                }
                else if(label[i].contains("-Ar"))
                {
                    helpString += QString::number(i+1) + sign[1] + QString::number(earsChan+1) + "/2";
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    else if(newRef == "N")
    {
        for(int i = 0; i < def::ns; ++i) //ns -> 21
        {
            if(ui->eogAsIsCheckBox->isChecked() && label[i].contains("EOG"))
            {
                helpString += QString::number(i+1) + " "; continue;
            }
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!label[i].contains("-N")) //generality
            {
                if(label[i].contains("-A1"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(groundChan+1);
                }
                else if(label[i].contains("-A2"))
                {
                    helpString += QString::number(i+1) +
                                  sign[0] + QString::number(earsChan+1) +
                            "+" + QString::number(groundChan+1);
                }
                else if(label[i].contains("-Ar"))
                {
                    helpString += QString::number(i+1) +
                                  sign[0] + QString::number(earsChan+1) + "/2" +
                            "+" + QString::number(groundChan+1);
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    else if(newRef == "Ar")
    {
        for(int i = 0; i < def::ns; ++i) //ns -> 21
        {
            if(ui->eogAsIsCheckBox->isChecked() && label[i].contains("EOG"))
            {
                helpString += QString::number(i+1) + " "; continue;
            }
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!label[i].contains("-Ar")) //generality
            {
                if(label[i].contains("-A1"))
                {
                    helpString += QString::number(i+1) +
                                  sign[1] + QString::number(earsChan + 1) + "/2";
                }
                else if(label[i].contains("-A2"))
                {
                    helpString += QString::number(i+1) +
                                  sign[0] + QString::number(earsChan + 1) + "/2";
                }
                else if(label[i].contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1) + "+" + QString::number(earsChan1+1) + "/2";
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    cout << "rereferenceData:\n" << helpString.toStdString() << endl;
    ui->reduceChannelsLineEdit->setText(helpString);


    //change labels
    for(int i = 0; i < def::ns; ++i)
    {
        helpString = label[i];
        if(helpString.contains('-') && (i != groundChan && i != earsChan))
        {
            // helpString2 - oldRef
            helpString2 = helpString;
            helpString2.remove(0, helpString.indexOf('-') + 1);
            helpString2.remove(helpString2.indexOf(' '), helpString2.length());
            helpString.replace(helpString2, newRef);
        }
        label[i] = helpString;
    }
    globalEdf.setLabels(label);
    reduceChannelsFast();

    //set all of channels to the lineedit
    helpString.clear();
    for(int i = 0; i < def::ns; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);

    globalEdf.writeEdfFile(newPath);
    cout << "rereferenceData: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::refilterDataSlot()
{

    const double lowFreq = ui->lowFreqFilterDoubleSpinBox->value();
    const double highFreq = ui->highFreqFilterDoubleSpinBox->value();
    QString helpString = def::dir->absolutePath()
            + slash() + def::ExpName + ".edf"; //ui->filePathLineEdit->text()
    readData();
    helpString.replace(".edf",
                       "_f"
                       + QString::number(lowFreq) + '-' + QString::number(highFreq)
                       + ".edf");

    refilterData(lowFreq, highFreq, helpString);
    int tmp = ui->reduceChannelsComboBox->currentIndex();
    ui->reduceChannelsComboBox->setCurrentIndex(0);
    ui->reduceChannelsComboBox->setCurrentIndex(tmp);
}

void MainWindow::refilterData(double lowFreq, double highFreq, QString newPath)
{
#if 1
    globalEdf.refilter(lowFreq, highFreq, newPath);
#else
    QTime myTime;
    myTime.start();

    readData();
    edfFile & fil = globalEdf;

    int fftLength = fftL(fil.getDataLen());

    double spStep = def::freq / double(fftLength);

    /////////////////////////////////////////////////////////how many channels to filter????

    QList <int> chanList;
    for(int i = 0; i < fil.getNs(); ++i)
    {
        if(fil.getLabels()[i].contains(QRegExp("E[OE]G"))) // filter only EEG, EOG signals
        {
            chanList << i;
        }
    }
//    cout << chanList << endl;
    int numOfChan = chanList.length(); //NOT MARKERS

    double norm1 = fftLength / double(fil.getDataLen());
    double * spectre = new double [fftLength*2];

    for(int j = 0; j < numOfChan; ++j)
    {
        for(int i = 0; i < fil.getDataLen(); ++i)            //make appropriate array
        {
            spectre[ i * 2 + 0 ] = (double)(fil.getData()[ chanList[j] ][ i ] * sqrt(norm1));
            spectre[ i * 2 + 1 ] = 0.;
        }
        for(int i = fil.getDataLen(); i < fftLength; ++i)            //make appropriate array
        {
            spectre[ i * 2 + 0 ] = 0.;
            spectre[ i * 2 + 1 ] = 0.;
        }
        four1(spectre - 1, fftLength, 1);       //Fourier transform

        //filtering
        for(int i = 0; i < fftLength; ++i)
        {
            if(i < 2. * lowFreq/spStep || i > 2. * highFreq/spStep)
                spectre[i] = 0.;
        }
        for(int i = fftLength; i < 2*fftLength; ++i)
        {
            if(((2*fftLength - i) < 2. * lowFreq/spStep) || (2 * fftLength - i > 2. * highFreq/spStep))
                spectre[i] = 0.;
        }
        //end filtering

        four1(spectre-1, fftLength, -1);
        for(int i = 0; i < fil.getDataLen(); ++i)
        {
            fil.setData(chanList[j],
                        i,
                        spectre[2*i] / (fftLength * sqrt(norm1)) );
        }
        ui->progressBar->setValue(j * 100. / numOfChan);
    }
    ui->progressBar->setValue(0);

    fil.writeEdfFile(newPath);

    cout << "refilterData: time = " << myTime.elapsed()/1000. << " sec" << endl;
#endif
}

void MainWindow::reduceChannelsEDFSlot()
{
    QString helpString;
    helpString = def::dir->absolutePath()
                 + slash() + def::ExpName + "_rdc.edf";
    reduceChannelsEDF(helpString);
}

void MainWindow::reduceChannelsEDF(const QString & newFilePath)
{
    QTime myTime;
    myTime.start();
    edfFile temp;
    temp.readEdfFile(ui->filePathLineEdit->text());

    vector<int> chanList;
    makeChanList(chanList);

    temp.reduceChannels(chanList);

//    for(int i = 0; i < chanList.size(); ++i)
//    {
//        for(int j = 0; j < 5; ++j)
//        {
//            cout << temp.getData()[i][j] << "\t";
//        }
//        cout << endl;
//    }
//    cout << endl;

    temp.writeEdfFile(newFilePath);

    cout << "reduceChannelsEDF: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

/// Ossadtchi only ?
void MainWindow::reduceChannelsSlot()
{
#if 1
    // reduce channels in Realisations
    /// not the same as edfFile::reduceChannels
    QStringList lst;
    matrix dataR;

    std::set<int, std::greater<int>> excludeList;
    for(int i = 0; i < def::ns; ++i)
    {
        excludeList.emplace(i);
    }


    QString helpString = ui->reduceChannelsLineEdit->text();
    QStringList leest = helpString.split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(const QString & chanStr : leest)
    {
        excludeList.erase(std::find(excludeList.begin(),
                                    excludeList.end(),
                                    chanStr.toInt() - 1));
    }
    cout << "reduceChannelsSlot: excludeList = ";
    for(const int & in : excludeList)
    {
        cout << in << "  ";
    }
    cout << endl;

    QDir localDir(def::dir->absolutePath());
    localDir.cd("Realisations");
    lst = localDir.entryList(QDir::Files, QDir::NoSort);


    int NumOfSlices;
    int localNs;
    for(const QString & fileName : lst)
    {
        localNs = def::ns;
        helpString = QDir::toNativeSeparators(localDir.absolutePath()
                                              + slash() + fileName);
        readPlainData(helpString, dataR, NumOfSlices);
        for(const int & exclChan : excludeList)
        {
            dataR.eraseRow(exclChan);
            --localNs;
        }
        writePlainData(helpString, dataR, localNs, NumOfSlices);
    }

    def::ns -= excludeList.size();

    helpString = "channels reduced ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);

    cout << "reduceChannelsSlot: finished";
#endif
}

void MainWindow::reduceChannelsFast()
{
    QString helpString;
    globalEdf.reduceChannels(ui->reduceChannelsLineEdit->text());
    def::ns = globalEdf.getNs();

    helpString = "channels reduced fast ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to " + QString::number(def::ns);
    ui->textEdit->append(helpString);

    ui->progressBar->setValue(0);
}

void MainWindow::concatenateEDFs(QStringList inPath, QString outPath)
{
    if(inPath.isEmpty())
    {
        cout << "concatenateEDFs: input list is empty" << endl;
        return;
    }
    QTime myTime;
    myTime.start();

    //assume the files are concatenable
    edfFile * resultEdf = new edfFile;
    resultEdf->readEdfFile(inPath[0]);
    for(int k = 1; k < inPath.length(); ++k)
    {
        resultEdf->concatFile(inPath[k]);
    }
    resultEdf->writeEdfFile(outPath);
    delete resultEdf;
    cout << "concatenateEDF: " << getFileName(outPath) << "\ttime = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::concatenateEDFs(QString inPath1, QString inPath2, QString outPath)
{
    QTime myTime;
    myTime.start();
    QStringList lst;
    lst << inPath1 << inPath2;
    concatenateEDFs(lst, outPath);
    cout << "concatenateEDFs: " << getFileName(outPath) << "\ttime = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::constructEDFSlot()
{
    QTime myTime;
    myTime.start();
    QString helpString;
    QStringList filters;
    if(!ui->matiCheckBox->isChecked())
    {
        helpString = def::dir->absolutePath() + slash() + def::ExpName + "_new.edf";
        constructEDF(helpString);
    }
    else //if(ui->matiCheckBox->isChecked())
    {
        const QString initEDF = ui->filePathLineEdit->text();
        helpString = def::dir->absolutePath()
                + slash() + def::ExpName.left(3)
                + "_splitZerosLog.txt";

        ofstream outStream;
        outStream.open(helpString.toStdString());
        outStream << def::ExpName.left(3).toStdString() << "\t";
        outStream << "type" << "\t";
        outStream << "sessn" << "\t";
        outStream << "offset" << "\t";
        outStream << "offsetS" << endl;
        outStream.close();

        for(int i = 0; i < 3; ++i) //every type 0-count, 1-track, 2-composed, 3-rest
        {
            setEdfFile(initEDF);
            for(int j = 0; j < 6; ++j) //every session
            {

                filters.clear();
                // filter for realisations
                helpString = def::ExpName
                        + "_" + QString::number(i)
                        + "_" + QString::number(j)
                        + "*";
                filters << helpString;

                // outPath for session edfs
                helpString = def::dir->absolutePath()

                        + slash() + "auxEdfs"

                        + slash() + def::ExpName
                        + "_c"
                        + "_" + QString::number(i)
                        + "_" + QString::number(j)
                        + ".edf";

                constructEDF(helpString, filters); // construct 1 session from realisations
                ui->progressBar->setValue(100. *
                                          (1. / 3. * (i + 1. / 6. * j)));
                qApp->processEvents();
            }

            def::dir->cd("auxEdfs");
            // template for session edfs
            helpString = def::ExpName
                    + "_c"
                    + "_" + QString::number(i)
                    + "_*.edf";
            QStringList lst = def::dir->entryList(QStringList(helpString));  //filter for edfs

            for(int k = 0; k < lst.length(); ++k)
            {
                lst[k].prepend( def::dir->absolutePath() + slash() );
            }

//            def::dir->cdUp(); // if save concatenated into def::ExpName dir

            //outPath for concatenated
            helpString = def::dir->absolutePath()
                    + slash() + def::ExpName.left(3)
                    + "_" + QString::number(i)
                    + ".edf";

//            def::dir->cdUp(); // if save concatenated into auxEdfs

            concatenateEDFs(lst, helpString);

            setEdfFile(initEDF);
        }

        // concatenate all session files
        helpString = def::dir->absolutePath()
                     + slash() + def::ExpName.left(3)
                     + "_cl";
        if(ui->splitZerosCheckBox->isChecked()) helpString += "_nz";
        helpString += ".edf";

        def::dir->cd("auxEdfs");
        QStringList lst;
        for(auto i : {"_0.edf", "_1.edf", "_2.edf"})
        {
            lst << def::dir->absolutePath() + slash() + def::ExpName.left(3) + i;
        }
        concatenateEDFs(lst, helpString);
        setEdfFile(initEDF);
    }
    ui->progressBar->setValue(0);
    cout << "constructEdf: FULL time = " << myTime.elapsed()/1000. << " sec" << endl;

    helpString = "constructEdf finished\n";
    helpString += "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);
}

void MainWindow::constructEDF(const QString & newPath,
                              const QStringList & nameFilters)
{
    // all the realisations, to newPath based on ui->filePathLineEdit
    QString helpString;

    QTime myTime;
    myTime.start();

    readData(); // read globalEdf based on ui->filePathEdit to renew the initial channels list

    vector<int> chanList;
    makeChanList(chanList);

    ////////////////////// aaaaaaaaaaaaaaaaaaaaaaaaa
    def::ns = chanList.size();
    const int ns = chanList.size();


    QStringList lst;
    if(!nameFilters.isEmpty())
    {
        lst = QDir(def::dir->absolutePath() + slash() + "Realisations").entryList(
                  nameFilters, QDir::Files, QDir::Name); /// Name ~ order
    }
    else
    {
        lst = QDir(def::dir->absolutePath() + slash() + "Realisations").entryList(
                  QDir::Files, QDir::Name); /// Name ~ order
//        lst = def::dir->entryList(QDir::Files, QDir::Name); /// Name ~ order
    }

    if(lst.isEmpty())
    {
        cout << "constructEDF: list of realisations is empty. filter[0] = " << nameFilters[0].toStdString() << endl;
        return;
    }

    matrix newData;

    int NumOfSlices;
    int currSlice = 0;

    for(const QString & fileName : lst)
    {
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Realisations"
                                              + slash() + fileName);
        readPlainData(helpString, newData, NumOfSlices, currSlice);
        currSlice += NumOfSlices;
    }


    int helpInt = currSlice;

    if(currSlice < 16 * def::freq)
    {
        cout << "constructEDF: too little data 1 =  " << currSlice << endl;
        return;
    }


    int offset = 0;
//    if(globalEdf.getMatiFlag())
    if(ui->matiCheckBox->isChecked()) // bicycle generality
    {
        QString fileName = getFileName(newPath);
        helpString = def::dir->absolutePath()
                     + slash() + def::ExpName.left(3)
                     + "_splitZerosLog.txt";

        splitZeros(newData, helpInt, &currSlice, helpString, fileName); // helpString unchanged

        ofstream outStream;
        outStream.open(helpString.toStdString(), ios_base::app);

        if(ui->roundOffsetCheckBox->isChecked())
        {
            if(currSlice < 16 * def::freq)
            {
                cout << "too little data to construct edf" << endl;
                return;
            }

            const double saveMarker = newData[ns - 1][0];
            // Mati offset ~20 seconds in the beginning
            offset = currSlice % int(16 * def::freq) + 16 * def::freq;

            if(currSlice <= offset)
            {
                cout << "constructEDF: too little data " << currSlice << endl;
                return;
            }


            for (int i = 0; i < ns; ++i) //shift start pointers
            {
                // vector
//                newData[i].erase(newData[i].begin(),
//                                 newData[i].begin() + offset);
                // valarray
                lineType newArr = newData[i][std::slice(offset,
                                                        newData[i].size() - offset,
                                                        1)];
                newData[i] = newArr;
//                std::remove_if(begin(newData[i]),
//                               begin(newData[i]) + offset,
//                               [](double){return true;});
            }

            newData[ns - 1][0] = saveMarker;
            currSlice -= offset;

            //write the exclusion of the first part [start, finish)
            outStream << fileName.toStdString() << "\t";
            outStream << 0 << "\t"; // first bin to exclude
            outStream << offset << "\t"; // first bin NOT TO exclude
            outStream << offset << "\t"; // length

            outStream << "0.000" << "\t"; // start time to exclude
            outStream << offset / def::freq << "\t"; // first time NOT TO exclude
            outStream << offset / def::freq << endl << endl; // length
            outStream.close();
        }



        //set the last marker if it's not
        //fix the first resting file
        if(newData[ns - 1][0] == 0)
        {
            newData[ns - 1][0] = matiCountDecimal("0000 0101 1000 0000"); //as a count session end
        }
        else
        {
//            matiPrintMarker(newData[ns - 1][0], "after if == 0");
        }

        double & firstMarker = newData[ns - 1][0];
        double & lastMarker = newData[ns - 1][currSlice -  1];
        matiFixMarker(firstMarker); //fix the start marker for this small edf file
        matiFixMarker(lastMarker);  //fix the last  marker for this small edf file

        //if not one of them is the end of some session
        if((matiCountBit(firstMarker, 10) == matiCountBit(lastMarker, 10)) || lastMarker == 0)
        {
            lastMarker = firstMarker
                    + pow(2, 10) * ((matiCountBit(firstMarker, 10))?-1:1); //adjust the last marker
        }
    }
    else if(ui->splitZerosCheckBox->isChecked())
    {
        splitZeros(newData, helpInt, &currSlice);
    }

    /// remake with dataType
    globalEdf.writeOtherData(newData, newPath, chanList); // new to check

//    def::ns = globalEdf.getNs(); /// should test

    cout << "constructEDF: " << getFileName(newPath) << "\ttime = " << myTime.elapsed() / 1000. << " sec" << endl;
}

void MainWindow::eyesFast()  //generality
{
#if 1
    globalEdf.cleanFromEyes();
#else
    QTime myTime;
    myTime.start();

    QString helpString;
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "eyes.txt");

    FILE * coef = fopen(helpString, "r");
    if(coef == NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No eyes coefficients found"), QMessageBox::Ok);
        return;
    }

    int NumEog, NumEeg;

    fscanf(coef, "NumOfEyesChannels %d\n", &NumEog);
    fscanf(coef, "NumOfEegChannels %d\n", &NumEeg);

    double ** coefficients = new double * [NumEeg];
    for(int i = 0; i < NumEeg; ++i)
    {
        coefficients[i] = new double [NumEog];
    }

    for(int k = 0; k < NumEeg; ++k)
    {
        for(int i = 0; i < NumEog; ++i)
        {
            fscanf(coef, "%lf", &coefficients[k][i]);
        }
    }
    fclose(coef);

    int a[2]; //generality 2
    a[0] = -1;
    a[1] = -1;

    if(ui->enRadio->isChecked())
    {
        for(int i = 0; i < ns; ++i)
        {
            if(label[i].contains("EOG1", Qt::CaseInsensitive)) //generality eog channels names
            {
                a[0] = i;
            }
            else if(label[i].contains("EOG2", Qt::CaseInsensitive)) //generality eog channels names
            {
                a[1] = i;
            }
        }
    }
    if(a[0] == -1 && a[1] == -1)
    {
        cout << "eyesFast: EOG channels not found" << endl;
        for(int i = 0; i < NumEeg; ++i)
        {
            delete []coefficients[i];
        }
        delete []coefficients;
        return;
    }

    double helpDouble;
    for(int k = 0; k < NumEeg; ++k)
    {
        for(int j = 0; j < ndr * nr[k]; ++j) //generality nr
        {
            for(int z = 0; z < NumEog; ++z)
            {
                helpDouble = globalEdf.getData()[k][j]
                        - coefficients[k][z] * globalEdf.getData()[ a[z] ][j];
                globalEdf.setData(k, j, helpDouble);
            }
        }
    }

//    cout << "eyesFast: eyes cleaned, ns = " << ns << endl;

    for(int i = 0; i < NumEeg; ++i)
    {
        delete []coefficients[i];
    }
    delete []coefficients;

    helpString = "eyes cleaned fast ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    cout << "eyesFast: time = " << myTime.elapsed()/1000. << " sec" << endl;
#endif
}


#endif
