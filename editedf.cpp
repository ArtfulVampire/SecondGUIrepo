#ifndef EDITEDF_CPP
#define EDITEDF_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"



void MainWindow::makeChanList(QList<int> & chanList)
{
    chanList.clear();
    QStringList lst;
    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(lst.last().toInt() - 1 != globalEdf.getMarkChan())
    {
        cout << "makeChanList: no markers channel" << endl;
        return;
    }
    for(int i = 0; i < lst.length(); ++i)
    {
        chanList << lst[i].toInt() - 1;
    }
}

void MainWindow::rereferenceDataSlot()
{
    QString helpString = dir->absolutePath() + slash() + ExpName + ".edf"; //ui->filePathLineEdit->text()
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

    QStringList lst;
    QString helpString;
    QString helpString2;

    readData();

    helpString.clear();
    for(int i = 0; i < ns; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);


    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
    {
        cout << "refilterData: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }

    int groundChan = -1; //A1-N
    int earsChan = -1; //A1-A2
    for(int i = 0; i < ns; ++i)
    {
        if(QString(label[i]).contains("A1-N"))
        {
            groundChan = i;
        }
        else if(QString(label[i]).contains("A1-A2"))
        {
            earsChan = i;
        }
    }
    if((groundChan+1) * (earsChan+1) == 0)
    {
        cout << "some of reref channels is absent" << endl;
        return;
    }


    helpString.clear();
    if(newRef == "A1")
    {
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-A1")) //generality
            {
                if(QString(label[i]).contains("-A2"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1);
                }
                else if(QString(label[i]).contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1);
                }
                else if(QString(label[i]).contains("-Ar"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "/2";
                }
                else
                {
                    helpString += QString::number(i+1);
                }
            }
            else //EOG and a shet
            {
                helpString += QString::number(i+1);
            }
            helpString += " "; // space after each channel
        }
    }
    else if(newRef == "A2")
    {
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-A2")) //generality
            {
                if(QString(label[i]).contains("-A1"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(earsChan+1);
                }
                else if(QString(label[i]).contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1) + "+" + QString::number(earsChan+1);
                }
                else if(QString(label[i]).contains("-Ar"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(earsChan+1) + "/2";
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
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-N")) //generality
            {
                if(QString(label[i]).contains("-A1"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(groundChan+1);
                }
                else if(QString(label[i]).contains("-A2"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "+" + QString::number(groundChan+1);
                }
                else if(QString(label[i]).contains("-Ar"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "/2" + "+" + QString::number(groundChan+1);
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
        for(int i = 0; i < ns; ++i) //ns -> 21
        {
            if(i == groundChan || i == earsChan)
            {
                helpString += QString::number(i+1);
            }
            else if(!QString(label[i]).contains("-Ar")) //generality
            {
                if(QString(label[i]).contains("-A1"))
                {
                    helpString += QString::number(i+1) + "+" + QString::number(earsChan+1) + "/2";
                }
                else if(QString(label[i]).contains("-A2"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(earsChan+1) + "/2";
                }
                else if(QString(label[i]).contains("-N"))
                {
                    helpString += QString::number(i+1) + "-" + QString::number(groundChan+1) + "+" + QString::number(earsChan+1) + "/2";
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
    for(int i = 0; i < ns; ++i)
    {
        helpString = QString(label[lst[i].toInt() - 1]);
        if(helpString.contains('-') && (i != groundChan && i != earsChan))
        {
            // helpString2 - oldRef
            helpString2 = helpString;
            helpString2.remove(0, helpString.indexOf('-') + 1);
            helpString2.remove(helpString2.indexOf(' '), helpString2.length());
            helpString.replace(helpString2, newRef);
        }
        strcpy(label[i], helpString.toStdString().c_str());
    }
    globalEdf.setLabels(label);
    reduceChannelsFast();

    //set all of channels to the lineedit
    helpString.clear();
    for(int i = 0; i < ns; ++i)
    {
        helpString += QString::number(i+1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);

    globalEdf.writeEdfFile(newPath);
    cout << "rereferenceData: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::refilterDataSlot()
{
    double lowFreq = ui->lowFreqFilterDoubleSpinBox->value();
    double highFreq = ui->highFreqFilterDoubleSpinBox->value();
    QString helpString = dir->absolutePath()
            + slash() + ExpName + ".edf"; //ui->filePathLineEdit->text()
    helpString.replace(".edf", "_f.edf");
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
    helpString = dir->absolutePath() + slash() + ExpName + "_rdcChan.edf";
    reduceChannelsEDF(helpString);
}

void MainWindow::reduceChannelsEDF(QString newFilePath)
{
    QTime myTime;
    myTime.start();
    edfFile temp;
    temp.readEdfFile(ui->filePathLineEdit->text());

    QStringList lst;
    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
//    if(!QString(temp.getLabels()[lst.last().toInt() - 1]).contains("Markers"))
    if(lst.last().toInt() - 1 != temp.getMarkChan())
    {
        cout << "reduceChannelsEDF: no markers channel" << endl;
        return;
    }

    QList <int> chanList;
    for(int i = 0; i < lst.length(); ++i)
    {
        chanList << lst[i].toInt() - 1;
    }
    temp.reduceChannels(chanList);
    temp.writeEdfFile(newFilePath);

    cout << "reduceChannelsEDF: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::reduceChannelsSlot()
{
    QStringList lst;
    QString helpString;

    int * num = new int [maxNs];
    FILE * file;

    mat dataR;
    /*
    double ** dataR = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataR[i] = new double [250 * 60];   // generality for 1 minute realisations
    }
    */

    helpString = ui->reduceChannelsLineEdit->text();
    QStringList list = helpString.split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(int i = 0; i < list.length(); ++i)
    {
        num[i] = list[i].toInt();
    }

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files, QDir::NoSort);


    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath()
                                              + slash() + lst[i]);
        readPlainData(helpString, dataR, ns, NumOfSlices);
        ////////////////////// kashghasgjdgakjsdgakjsgdkjasgkdagkhscvawdc
        file = fopen(helpString, "w");
        if(file == NULL)
        {
//            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open file to write"), QMessageBox::Ok);
            cout << "reduceChannelsSLot: cannot open file to write, return" << endl;
            return;
        }
        fprintf(file, "NumOfSlices %d\n", NumOfSlices);
        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < list.length(); ++k)
            {
                fprintf(file, "%lf\n", dataR[ num[k] - 1 ][j]);
            }
        }
        fclose(file);
    }

//    for(int i = 0; i < ns; ++i)
//    {
//        delete []dataR[i];
//    }
//    delete[]dataR;
    ns = list.length();
    delete []num;
    dir->cdUp();

    helpString = "channels reduced ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to ";
    helpString + QString::number(ns);
    ui->textEdit->append(helpString);
}

void MainWindow::reduceChannelsFast()
{
    QString helpString;
#if 1
    globalEdf.reduceChannels(ui->reduceChannelsLineEdit->text());
    ns = globalEdf.getNs();
#else

    // more general, but needs tons of additional memory

    QStringList lst;
    QStringList list;

    list = ui->reduceChannelsLineEdit->text().split(QRegExp("[,;\\s]"), QString::SkipEmptyParts);
    if(!QString(label[list[list.length() - 1].toInt() - 1]).contains("Markers"))
    {
        QMessageBox::critical(this, tr("Error"), tr("bad channels list"), QMessageBox::Ok);
        return;
    }


    double sign;
    int lengthCounter; //length of the expression in chars



    double ** temp = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        temp[i] = new double [ndr * nr[i]];
    }
    for(int k = 0; k < list.length(); ++k)
    {
        if(QString::number(list[k].toInt()) == list[k]) // just copy
        {
            memcpy(temp[k],
                   globalEdf.getData()[list[k].toInt() - 1].data(),
                    ndr * nr[list[k].toInt() - 1] * sizeof(double));
        }
        else if(list[k].contains(QRegExp("[\\+\\-\\*\\/]")))
        {
            lengthCounter = 0;
            lst = list[k].split(QRegExp("[\\+\\-\\*\\/]"), QString::SkipEmptyParts);
            for(int h = 0; h < lst.length(); ++h)
            {
                if(QString::number(lst[h].toInt()) != lst[h]) // if not a number between operations
                {
                    cout << "bad rdc chan string" << endl;
                    for(int i = 0; i < ns; ++i)
                    {
                        delete []temp[i];
                    }
                    delete []temp;
                    return;
                }
            }
            memcpy(temp[k],
                   globalEdf.getData()[lst[0].toInt() - 1].data(),
                    ndr*nr[k] * sizeof(double));

            lengthCounter += lst[0].length();
            for(int h = 1; h < lst.length(); ++h)
            {
                if(list[k][lengthCounter] == '+') sign = 1.;
                else if(list[k][lengthCounter] == '-') sign = -1.;
                else //this should never happen!
                {
                    cout << "bad rdc chan string" << endl;
                    for(int i = 0; i < ns; ++i)
                    {
                        delete []temp[i];
                    }
                    delete []temp;
                    return;
                }
                lengthCounter += 1; //sign length
                lengthCounter += lst[h].length();

                //check '/' and '*'
                if(list[k][lengthCounter] == '/')
                {
                    sign /= lst[h+1].toDouble();
                }
                else if(list[k][lengthCounter] == '*')
                {
                    sign *= lst[h+1].toDouble();
                }
                for(int j = 0; j < ndr*nr[k]; ++j) //generality k
                {
                    temp[k][j] += sign * globalEdf.getData()[lst[h].toInt() - 1][j];
                }

                if(list[k][lengthCounter] == '/' || list[k][lengthCounter] == '*')
                {
                    lengthCounter += 1; // / or *
                    lengthCounter += lst[h+1].length(); //what was divided onto
                    ++h;
                }
            }
        }
        else
        {
            cout << "bad rdc chan string" << endl;
            for(int i = 0; i < ns; ++i)
            {
                delete []temp[i];
            }
            delete []temp;
            return;
        }

        ui->progressBar->setValue(k * 100. / list.length());
    }
    for(int k = 0; k < list.length(); ++k)
    {
        for(int j = 0; j < globalEdf.getDataLen(); ++j)
        {
            globalEdf.setData(k, j, temp[k][j]);
        }
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []temp[i];
    }
    delete []temp;

    ns = list.length();
#endif

    helpString = "channels reduced fast ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to " + QString::number(ns);
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
        helpString = dir->absolutePath() + slash() + ExpName + "_new.edf";
        constructEDF(helpString);
    }
    else //if(ui->matiCheckBox->isChecked())
    {
        const QString initEDF = ui->filePathLineEdit->text();
        helpString = dir->absolutePath()
                + slash() + ExpName.left(3)
                + "_splitZerosLog.txt";
        ofstream outStream;
        outStream.open(helpString.toStdString().c_str());
        outStream << ExpName.left(3).toStdString() << "\t";
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
                helpString = ExpName
                        + "_" + QString::number(i)
                        + "_" + QString::number(j)
                        + "*";
                filters << helpString;

                // outPath for session edfs
                helpString = dir->absolutePath()

                        + slash() + "auxEdfs"

                        + slash() + ExpName
                        + "_c"
                        + "_" + QString::number(i)
                        + "_" + QString::number(j)
                        + ".edf";

                constructEDF(helpString, filters);
                ui->progressBar->setValue(100. *
                                          (1. / 3. * (i + 1. / 6. * j)));
                qApp->processEvents();
            }

            dir->cd("auxEdfs");
            // template for session edfs
            helpString = ExpName
                    + "_c"
                    + "_" + QString::number(i)
                    + "_*.edf";
            QStringList lst = dir->entryList(QStringList(helpString));  //filter for edfs

            for(int k = 0; k < lst.length(); ++k)
            {
                lst[k].prepend( dir->absolutePath() + slash() );
            }

//            dir->cdUp(); // if save concatenated into ExpName dir

            //outPath for concatenated
            helpString = dir->absolutePath()
                    + slash() + ExpName.left(3)
                    + "_" + QString::number(i)
                    + ".edf";

//            dir->cdUp(); // if save concatenated into auxEdfs

            concatenateEDFs(lst, helpString);

            setEdfFile(initEDF);
        }

    }
    ui->progressBar->setValue(0);
    cout << "constructEdf: FULL time = " << myTime.elapsed()/1000. << " sec" << endl;

    helpString = "constructEdf finished\n";
    helpString += "ns equals to ";
    helpString += QString::number(ns);
    ui->textEdit->append(helpString);
}

void MainWindow::constructEDF(QString newPath, QStringList nameFilters) // all the realisations, to newPath based on ui->filePathLineEdit
{
    QStringList lst;
    QString helpString;

    QTime myTime;
    myTime.start();
    readData(); // read globalEdf


    lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    ns = lst.length();
    if(!QString(label[lst[ns-1].toInt()-1]).contains("Markers"))
    {
        cout << "constructEDF: bad reduceChannelsLineEdit - no markers" << endl;
        return;
    }
    QList<int> chanList;
    for(int i = 0; i < lst.length(); ++i)
    {
        chanList << lst[i].toInt() - 1;
    }


    if(!ui->sliceWithMarkersCheckBox->isChecked())
    {
        cout << "constructEDF: withMarkersCheckBox is not checked" << endl;
        return;
    }

    dir->cd("Realisations");
    if(!nameFilters.isEmpty())
    {
        lst = dir->entryList(nameFilters, QDir::Files, QDir::Name); //generality
    }
    else
    {
        lst = dir->entryList(QDir::Files, QDir::Name); //generality
    }
    dir->cdUp();



    if(lst.isEmpty())
    {
//        cout << "constructEDF: list of realisations is empty. filter[0] = " << nameFilters[0].toStdString() << endl;
        return;
    }

#if 0
    double ** newData = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        newData[i] = new double [250 * 60 * 120]; // for 2 hours
    }
#else
    mat newData;
    newData.resize(ns);
    for(int i = 0; i < ns; ++i)
    {
        newData[i].resize(250 * 60 * 250);
    }
#endif


    int currSlice = 0;
    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath()
                                              + slash() + "Realisations"
                                              + slash() + lst[i]);
        readPlainData(helpString, newData, ns, NumOfSlices, currSlice);
        currSlice += NumOfSlices;
    }
    int helpInt = currSlice;

    int offset = 0;
    if(ui->matiCheckBox->isChecked()) // bicycle generality
    {
        QString fileName = getFileName(newPath, true);
        helpString = dir->absolutePath()
                + slash() + ExpName.left(3)
                + "_splitZerosLog.txt";

        splitZeros(newData, ns, helpInt, &currSlice, helpString, fileName); // helpString unchanged

        ofstream outStream;
        outStream.open(helpString.toStdString().c_str(), ios_base::app);

        if(ui->roundOffsetCheckBox->isChecked())
        {
            double saveMarker = newData[ns - 1][0];
            offset = currSlice%(16*250) + 16*250; // Mati offset ~20 seconds in the beginning

            for (int i = 0; i < ns; ++i) //shift start pointers
            {
//                newData[i] = newData[i] + offset; // for double **
                newData[i].erase(newData[i].begin(),
                                 newData[i].begin() + offset);
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

        if((matiCountBit(firstMarker, 10) == matiCountBit(lastMarker, 10)) || lastMarker == 0) //if not one of them is the end of some session
        {
            lastMarker = firstMarker
                    + pow(2, 10) * ((matiCountBit(firstMarker, 10))?-1:1); //adjust the last marker
        }
    }
    else if(ui->splitZerosCheckBox->isChecked())
    {
        splitZeros(newData, ns, helpInt, &currSlice);
    }

    /// remake with dataType
//    writeEdf(ui->filePathLineEdit->text(), newData, newPath, currSlice); // old
    globalEdf.writeOtherData(newData, newPath, chanList); // new to check

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
    helpString = QDir::toNativeSeparators(dir->absolutePath()
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
            if(QString(label[i]).contains("EOG1", Qt::CaseInsensitive)) //generality eog channels names
            {
                a[0] = i;
            }
            else if(QString(label[i]).contains("EOG2", Qt::CaseInsensitive)) //generality eog channels names
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
