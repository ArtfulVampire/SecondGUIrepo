#ifndef AUTOS_CPP
#define AUTOS_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::Bayes()
{
    QStringList lst;
    QString helpString;

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files);
    dir->cdUp();

    FILE * file;
    double ** dataBayes = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataBayes[i] = new double [250*60*5];
    }
    double maxAmpl = 10.; //generality from readData

    maxAmpl += 0.001; //bicycle

    int numOfIntervals = ui->BayesSpinBox->value();
    numOfIntervals *= 2; //odd or even?

    spLength = numOfIntervals;
    left = 1;
    right = spLength;

    int * count = new int [numOfIntervals];

    double helpDouble = 0.;
    int helpInt = 0;

    for(int i = 0; i < lst.length(); ++i)
    {
        if(lst[i].contains("num")) continue;
        helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "Realisations" + slash() + lst[i]);
        file = fopen(helpString.toStdString().c_str(), "r");
        fscanf(file, "NumOfSlices %d\n", &NumOfSlices);
        if(NumOfSlices < 250)
        {
            fclose(file);
            continue;
        }

        for(int j = 0; j < NumOfSlices; ++j)
        {
            for(int k = 0; k < ns; ++k)
            {
                fscanf(file, "%lf\n", &dataBayes[k][j]);
            }
        }
        fclose(file);

        helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "SpectraSmooth" + slash() + "Bayes" + slash() + lst[i]);
        file = fopen(helpString.toStdString().c_str(), "w");
        for(int l = 0; l < ns; ++l)
        {
            if(ui->BayesRadioButton->isChecked())
            {
                //bayes itself;
                for(int k = 0; k < numOfIntervals; ++k)
                {
                    count[k] = 0;
                }
                for(int j = 0; j < NumOfSlices; ++j)
                {
                    helpInt = int(floor((dataBayes[l][j] + maxAmpl) / (2.*maxAmpl/double(numOfIntervals))));

                    if(helpInt != min(max(0, helpInt), numOfIntervals-1)) continue; // out of range

                    count[helpInt] += 1;
                }
                for(int k = 0; k < numOfIntervals; ++k)
                {
                    helpDouble = count[k]/double(NumOfSlices); // norm coeff
                    fprintf(file, "%lf\n", helpDouble);
                }
            }

            if(ui->HiguchiRadioButton->isChecked())
            {
                //fractal dimension
                helpString = QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("Help").append(slash()).append("Fractals").append(slash()).append(lst[i]).append("_").append(QString::number(l)).append(".png"));
                helpDouble = fractalDimension(dataBayes[l], NumOfSlices, helpString);
                fprintf(file, "%.3lf\n", helpDouble);
            }
        }
        fclose(file);
    }
    delete [] count;
    for(int i = 0; i < ns; ++i)
    {
        delete [] dataBayes[i];
    }
    delete [] dataBayes;
}


void MainWindow::hilbertCount()
{
    QString helpString;
    readData();
    if(ui->reduceChannelsCheckBox->isChecked()) reduceChannelsFast();

    double fr = def::freq; //generality

    double ** hilbertData = new double * [ns];
    hilbertPieces(data[0], 45536, fr, 9., 11., &hilbertData[0], "");

    cout << variance(hilbertData[0], 45536) << endl;
    hilbert(data[0], 65536, fr, 9., 11., &hilbertData[0], "");
    cout << variance(hilbertData[0], 65536) << endl;
    return;




    for(int i = 0; i < ns-1; ++i) //no markers
    {
//        hilbert(data[i], ndr*fr, fr, 5., 20., &hilbertData[i]);
    }
    memcpy(hilbertData[ns-1], data[ns-1], ndr*fr*sizeof(double)); //markers channel
    helpString = dir->absolutePath() + slash() + ExpName + "_hilbert.edf";
    writeEdf(ui->filePathLineEdit->text(), hilbertData, helpString, ndr*fr); ////////////////////////////need fix

    matrixDelete(&hilbertData, ns);
}

void MainWindow::diffSmooth()
{
    Spectre *sp;
    Net * ANN;
    QTime myTime;
    myTime.start();
    for(int i = 10; i <= 100; i += 10)
    {
        sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
        sp->setSmooth(i);
        sp->countSpectra();
        sp->close();
        delete sp;

        ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(true);
        ANN->loadCfg();
        ANN->autoClassification("SpectraSmooth");
        ANN->close();
        cout << "smooth = " << i << " done" << endl;
        delete ANN;
        sleep(60);

    }
    cout << "diffSmooth: time = " << myTime.elapsed()/1000. << " sec" << endl;

}


void MainWindow::diffPow()
{
    QString helpString;
    Spectre *sp;
    Net * ANN;
    MakePa * mkPa;
    QTime myTime;
    FILE * outFile;
    myTime.start();
    helpString = dir->absolutePath() + slash() + "diffPow.txt";
//    for(double i = 0.45; i >= 0.45; i -= 0.0)
        while(1)
    {
        double i = 0.45;
        sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
        sp->setPow(i);
        sp->countSpectra();
        sp->close();
        delete sp;

        ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(1);


        //set appropriate coeff

        helpString = dir->absolutePath() + slash() + "SpectraSmooth";
        mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
        mkPa->setRdcCoeff(4);
        while(1)
        {
            cout << "reduce coeff = " << mkPa->getRdcCoeff() << endl;
            mkPa->makePaSlot();
            ANN->PaIntoMatrixByName("1");
            ANN->LearnNet();
            if(ANN->getEpoch() > 140 || ANN->getEpoch() < 60)
            {
                mkPa->setRdcCoeff(mkPa->getRdcCoeff() / sqrt(ANN->getEpoch() / 100.) );
            }
            else
            {
                reduceCoefficient = mkPa->getRdcCoeff();
                cout << "file = " << ExpName.toStdString() << "\t" << "reduceCoeff = " << reduceCoefficient << endl;
                ANN->setReduceCoeff(reduceCoefficient);
                break;
            }
        }
        mkPa->close();
        delete mkPa;

        helpString = dir->absolutePath() + slash() + "SpectraSmooth";
        ANN->autoClassification(helpString);
        ANN->close();

        helpString = dir->absolutePath() + slash() + "diffPow.txt";
        outFile = fopen(helpString.toStdString().c_str(), "a");
        fprintf(outFile, "pow = %.2lf\t%.2lf\n", i, ANN->getAverageAccuracy());
        fclose(outFile);
        delete ANN;

    }
    cout << "diffPow: time = " << myTime.elapsed()/1000. << " sec" << endl;

}


double MainWindow::fileInnerClassification(QString workPath, QString fileName, QString cfgFileName, int NumOfPairs, bool windows, int wndLen, int tShift)
{
    QString helpString;

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);
    Net * ANN;

    if(!windows)
    {
        ui->realButton->setChecked(true);
    }
    else
    {
        ui->windButton->setChecked(true);
        ui->windowLengthSpinBox->setValue(wndLen);
        ui->timeShiftSpinBox->setValue(tShift);
    }

    helpString = tmpDir->absolutePath() + slash() + fileName;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    ANN = new Net(tmpDir, ns, left, right, spStep, ExpName);
    ANN->loadCfgByName(cfgFileName);
    ANN->setAutoProcessingFlag(true);
    ANN->setNumOfPairs(NumOfPairs);
    ANN->autoClassificationSimple();
    double res = ANN->getAverageAccuracy();
    ANN->close();
    delete ANN;
    delete tmpDir;
    return res;

}


double MainWindow::filesCrossClassification(QString workPath, QString fileName1, QString fileName2, QString cfgFileName, int NumOfRepeats, double startCoeff, bool windows, int wndLen, int tShift)
{
    QString helpString;

    //learn by 1 file
    //recognize 2 file
    //yet for 19 channels only

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    Net * ANN;
    MakePa * mkPa;

    if(windows)
    {
        ui->windButton->setChecked(true);
        ui->windowLengthSpinBox->setValue(wndLen);
        ui->timeShiftSpinBox->setValue(tShift);
    }
    else
    {
        ui->realButton->setChecked(true);
    }

    //open 1 file
    helpString = tmpDir->absolutePath() + slash() + fileName1;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    ANN = new Net(tmpDir, ns, left, right, spStep, ExpName);
    ANN->setReduceCoeff(startCoeff);
    ANN->setAutoProcessingFlag(true);
    ANN->loadCfgByName(cfgFileName);
    helpString = tmpDir->absolutePath() + slash() + "SpectraSmooth";
    if(windows)
    {
        helpString += QString(slash()) + "windows";
    }
    mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
    ANN->adjustReduceCoeff(helpString, 90, 150, mkPa, "all");
    cleanDir(tmpDir->absolutePath(), "wts");

    cout << "crossClass: learn (max " << NumOfRepeats << ") ";
    for(int k = 0; k < NumOfRepeats; ++k)
    {
        cout << k+1 << " ";
        flush(cout);
        mkPa->makePaSlot();
        ANN->PaIntoMatrixByName("all");
        ANN->LearnNet();
        helpString = tmpDir->absolutePath() + slash() + "crossClass_" + QString::number(k) + ".wts";
        ANN->saveWts(helpString);
    }
    cout << endl;


    //open 2nd file
    helpString = tmpDir->absolutePath() + slash() + fileName2;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    mkPa->makePaSlot();
    ANN->PaIntoMatrixByName("all");

    QStringList wtsFiles = tmpDir->entryList(QStringList("*.wts"));
//    cout << "crossClass: tall (max " << NumOfRepeats << ") ";
    for(int k = 0; k < NumOfRepeats; ++k)
    {
//        cout << k+1 << " "; cout.flush();
        helpString = tmpDir->absolutePath() + slash() + wtsFiles[k];
        ANN->loadWtsByName(helpString);
        ANN->tall();
    }
//    cout << endl;
    cleanDir(tmpDir->absolutePath(), "wts");
    ANN->averageClassification();
    double res = ANN->getAverageAccuracy();
    mkPa->close();
    delete mkPa;
    ANN->close();
    delete ANN;
    return res;
}


void MainWindow::countICAs(QString workPath, QString fileName, bool icaFlag, bool transformFlag, bool sequenceFlag, bool sumFlag) //to delete (listFileCrossClassification)
{
    QString helpString2;
    QString helpString;

    //for young scientists conference
    QTime myTime;
    myTime.start();

    ui->svdDoubleSpinBox->setValue(9.5);
    ui->vectwDoubleSpinBox->setValue(9.5);

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsCheckBox->setChecked(false);
    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);



    QString addName = fileName.left(3);
    QString ExpName2;
    QString mapsPath;
    QString mapsPath2;


    ui->realButton->setChecked(true);
    if(icaFlag) //count ICA on corresponding 3 files
    {

        helpString = tmpDir->absolutePath() + slash() + fileName;
        setEdfFile(helpString); // open ExpName_1.edf
        ICA();

        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_2.edf");
        setEdfFile(helpString); // open ExpName_2.edf
        cleanDirs();
        ICA();

        if(sumFlag)
        {
            helpString = tmpDir->absolutePath() + slash() + fileName;
            helpString.replace("_1.edf", "_sum.edf");
            setEdfFile(helpString); // open ExpName_sum.edf
            cleanDirs();
            ICA();
        }
    }

    if(transformFlag) //transform 2nd with 1st maps, and both with comman maps
    {

        //transform 2nd file with 1st maps
        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
        mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_1_maps.txt";

        helpString2 = tmpDir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_2_ica_by1.edf"); //write to ExpName_2_ica_by1.edf
        transformEDF(helpString, mapsPath, helpString2);

        if(sumFlag)
        {
            //transform both files with general maps
            helpString = tmpDir->absolutePath() + slash() + fileName; //
            mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_sum_maps.txt";
            helpString2 = tmpDir->absolutePath() + slash() + fileName;
            helpString2.replace("_1.edf", "_1_ica_sum.edf"); //write to ExpName_1_ica_sum.edf
            transformEDF(helpString, mapsPath, helpString2);

            helpString = dir->absolutePath() + slash() + fileName;
            helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
            mapsPath = dir->absolutePath() + slash() + "Help" + slash() + addName + "_sum_maps.txt";
            helpString2 = dir->absolutePath() + slash() + fileName;
            helpString2.replace("_1.edf", "_2_ica_sum.edf"); //write to ExpName_2_ica_sum.edf
            transformEDF(helpString, mapsPath, helpString2);
        }

        //_1_ica.edf -> _1_ica_by1.edf
        helpString = dir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_1_ica.edf");
        helpString2 = dir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_1_ica_by1.edf");
        QFile::copy(helpString, helpString2);
    }

    if(sequenceFlag) //sequencing 1 and 2
    {
        //transform 1st and 2nd correspondingly
        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_sum_ica.edf");
        helpString2 = tmpDir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_1_ica.edf");
        mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_sum_maps.txt";
        mapsPath2 = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_1_maps.txt";
        ICsSequence(helpString, helpString2, mapsPath, mapsPath2);

        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_1_ica.edf");
        helpString2 = tmpDir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_2_ica.edf");
        mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_1_maps.txt";
        mapsPath2 = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_2_maps.txt";
        ICsSequence(helpString, helpString2, mapsPath, mapsPath2);
    }
    delete tmpDir;
}


double MainWindow::filesDropComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats, bool windows, int wndLen, int tShift)
{
    QString helpString2;
    QString helpString;

    //for young scientists conference
    //learn on 1 -> classify 2
    //Icas or rawData

    QTime myTime;
    myTime.start();

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    QList <int> neededChannels;
    QList <int> channelsSet;
    QList <int> channelsSetExclude;

    int tempItem;
    int tempIndex;
    double tempAccuracy;
    double lastAccuracy;
    bool foundFlag;
    ofstream logF;
    QString logPath = workPath + slash() + "dropCompsLog .txt";


    ns = 19;

    channelsSet.clear();
    for(int j = 0; j < 19; ++j)
    {
        channelsSet << j;
    }
    channelsSetExclude.clear();
    neededChannels.clear();

    //////////////////////////////////////////////////////////initial accuracy count
    //clean wts
    cleanDir(workPath, "wts");
    makeCfgStatic(workPath, 19*247, "Reduced");

    helpString = fileName1;
    helpString2 = fileName2;
    double initAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 2., windows, wndLen, tShift);

    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << ExpName.left(3).toStdString() << " initialAccuracy = " << tempAccuracy << endl << endl;
        logF.close();
    }
    ////////////////////////////////////////////////////////////////////////////////////////initial accuracy count end

    while(1)
    {
        ns = channelsSet.length() - 1;
        makeCfgStatic(tmpDir->absolutePath(), ns*247, "Reduced");
        foundFlag = false;

        for(int l = 0; l < channelsSet.length(); ++l)
        {
            //clean wts
            cleanDir(dir->absolutePath(), "wts");
            cleanDir(dir->absolutePath(), "markers", 0);

            tempItem = channelsSet[l];
            if(neededChannels.contains(tempItem)) continue;

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << endl << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " start" << endl;
                logF.close();
            }

            channelsSet.removeAt(l);
            channelsSetExclude.push_back(tempItem);

            //drop some channels
            helpString.clear();
            for(int k = 0; k < 19; ++k)
            {
                if(channelsSet.contains(k))
                {
                    helpString += QString::number(k+1) + " ";
                }
            }
            helpString += "20";
            ui->reduceChannelsLineEdit->setText(helpString);

            //exclude channels from 1st file
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //exclude channels from 2nd file
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);



            //cross class
            helpString = fileName1;
            helpString.replace(".edf", "_rdc.edf");
            helpString2 = fileName2;
            helpString2.replace(".edf", "_rdc.edf");
            lastAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift);


            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " done" << endl;
                logF << "channelsSet:" << endl;
                for(int q = 0; q < channelsSet.length(); ++q)
                {
                    logF << channelsSet[q] + 1 << "  ";
                }
                logF << endl << "Accuracy = " << lastAccuracy << endl;
                logF.close();
            }


            //set back
            channelsSet.insert(l, tempItem);
            channelsSetExclude.removeLast();



            if(lastAccuracy > tempAccuracy + 1.5)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
                break;
            }
            else if(lastAccuracy > tempAccuracy + 0.7)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
            }
            else if(lastAccuracy < tempAccuracy - 2.0)
            {
                logF.open(logPath.toStdString().c_str(), ios_base::app);
                if(!logF.is_open())
                {
                    cout << "cannot open logFile: " << logPath.toStdString() << endl;
                }
                else
                {
                    logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to needed channels" << endl;
                    logF.close();
                }
                neededChannels << tempItem;
            }

        } //end of cycle l

        if(foundFlag)
        {
            channelsSetExclude.push_back(channelsSet[tempIndex]);
            channelsSet.removeAt(tempIndex);

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << channelsSet[tempIndex] + 1 << " to drop\t";
                logF << "currentAccuracy" << tempAccuracy << endl;
                logF.close();
            }

        }
        else
        {

            break;
        }

    } //end of while(1)
    cout << ExpName.left(3).toStdString() << " ended in " << myTime.elapsed()/1000. << " sec" << endl;



    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << "FINAL SET" << endl;
        logF << ExpName.left(3).toStdString() << endl;
        logF << "channelsSet:" << endl;
        for(int q = 0; q < channelsSet.length(); ++q)
        {
            logF << channelsSet[q] + 1 << "  ";
        }
        logF << endl << "neededChannels:" <<endl;
        for(int q = 0; q < neededChannels.length(); ++q)
        {
            logF << neededChannels[q] + 1 << "  ";
        }
        logF << endl << "Accuracy = " << tempAccuracy << endl;
        logF.close();
    }
    cleanDir(tmpDir->absolutePath(), "wts");
    cleanDir(tmpDir->absolutePath(), "markers", 0);
    delete tmpDir;
    cout << initAccuracy << "->" << tempAccuracy << endl;
    return tempAccuracy;
}

double MainWindow::filesAddComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats, bool windows, int wndLen, int tShift)
{

    QString helpString2;
    QString helpString;

    //check the percentage on all of 3 components
    //add the best one giving the maximum of percentage

    QTime myTime;
    myTime.start();

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    QList <int> unneededChannels;
    QList <int> channelsSet;
    QList <int> channelsSetExclude;

    int tempItem;
    int tempIndex;
    double tempAccuracy = 0.;
    double lastAccuracy;
    bool foundFlag;
    QString logPath = tmpDir->absolutePath() + slash() + "addCompsLog.txt";



    channelsSet.clear();
    unneededChannels.clear();

    //////////////////////////////////////////////////////////initial accuracy count
    //checkpercentage on the set of components (i, j, k)
    double initAccuracy = 0;
    int iS, jS, kS;
    makeCfgStatic(workPath, 1*247, "Reduced");

    ofstream logF;
    logF.open(logPath.toStdString().c_str(), ios_base::app);
    logF << fileName1.toStdString() << endl;

    for(int i = 0; i < 19; ++i) //19 is not ns
    {
        break; //already done
        for(int j = i+1; j < 19; ++j) //19 is not ns
        {
            for(int k = j+1; k < 19; ++k) //19 is not ns
            {
                myTime.restart();

                //clean wts
                cleanDir(workPath, "wts");
                channelsSet.clear();
                channelsSet << i;
                channelsSet << j;
                channelsSet << k;

                makeCfgStatic(workPath, channelsSet.length()*247, "Reduced");
                //drop some channels
                helpString.clear();
                for(int l = 0; l < 19; ++l)
                {
                    if(channelsSet.contains(l))
                    {
                        helpString += QString::number(l+1) + " ";
                    }
                }
                helpString += "20";
                ui->reduceChannelsLineEdit->setText(helpString);

                //exclude channels from 1st file
                helpString = tmpDir->absolutePath() + slash() + fileName1;
                setEdfFile(helpString);
                helpString = tmpDir->absolutePath() + slash() + fileName1;
                helpString.replace(".edf", "_rdc.edf");
                reduceChannelsEDF(helpString);

                //exclude channels from 2nd file
                helpString = tmpDir->absolutePath() + slash() + fileName2;
                setEdfFile(helpString);
                helpString = tmpDir->absolutePath() + slash() + fileName2;
                helpString.replace(".edf", "_rdc.edf");
                reduceChannelsEDF(helpString);


                helpString = fileName1;
                helpString.replace(".edf", "_rdc.edf");
                helpString2 = fileName2;
                helpString2.replace(".edf", "_rdc.edf");

                tempAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift); //0.5 generality
                cout << "check percentage: ";
                for(int p = 0; p < channelsSet.length(); ++p)
                {
                    cout << channelsSet[p]+1 << " ";
                    logF << channelsSet[p]+1 << " ";
                }
                cout << " = " << tempAccuracy << "\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
                logF << " = " << tempAccuracy << endl;

                if(tempAccuracy > initAccuracy + 0.5)
                {
                    initAccuracy = tempAccuracy;
                    iS = i;
                    jS = j;
                    kS = k;
                }
            }
        }
    }
//    logF << "final set: " << iS+1 <<" " << jS+1 << " " << kS+1 << " " << initAccuracy << endl << endl;
//    logF.close();
//    return initAccuracy;


    //results generality
    if(fileName1.contains("AAX"))
    {
        iS = 3;
        jS = 7;
        kS = 16;
        initAccuracy = 97.01;
    }
    else if(fileName1.contains("BED"))
    {
        iS = 2;
        jS = 7;
        kS = 13;
        initAccuracy = 89.80;
    }
    else if(fileName1.contains("FIX"))
    {
        iS = 3;
        jS = 11;
        kS = 18;
        initAccuracy = 93.75;
    }
    else if(fileName1.contains("GAS"))
    {
        iS = 7;
        jS = 13;
        kS = 17;
        initAccuracy = 89.73;
    }
    else if(fileName1.contains("KUS"))
    {
        iS = 4;
        jS = 5;
        kS = 7;
        initAccuracy = 97.59;
    }
    else if(fileName1.contains("PAS"))
    {
        iS = 8;
        jS = 15;
        kS = 16;
        initAccuracy = 95.53;
    }
    else if(fileName1.contains("RMS"))
    {
        iS = 3;
        jS = 11;
        kS = 13;
        initAccuracy = 93.05;
    }
    else if(fileName1.contains("SMM"))
    {
        iS = 1;
        jS = 2;
        kS = 4;
        initAccuracy = 96.51;
    }
    else if(fileName1.contains("SUA"))
    {
        iS = 2;
        jS = 7;
        kS = 9;
        initAccuracy = 97.79;
    }
    else if(fileName1.contains("VDA"))
    {
        iS = 3;
        jS = 9;
        kS = 16;
        initAccuracy = 95.93;
    }
    else return 0;
    --iS; --jS; --kS; //because of start from 0 or 1
    channelsSet << iS << jS << kS;


    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << ExpName.left(3).toStdString() << " initialAccuracy = " << initAccuracy << endl;
        logF << ExpName.left(3).toStdString() << " initialSet: " << iS+1 << " " << jS+1 << " " << kS+1 << " " << endl << endl;
        logF.close();
    }
    ////////////////////////////////////////////////////////////////////////////////////////initial accuracy count end

    channelsSetExclude.clear();
    for(int j = 0; j < 19; ++j)
    {
        channelsSetExclude << j;
    }
    channelsSetExclude.removeOne(iS);
    channelsSetExclude.removeOne(jS);
    channelsSetExclude.removeOne(kS);


    tempAccuracy = 0.;
    while(1)
    {
        ns = channelsSet.length() + 1;
        makeCfgStatic(tmpDir->absolutePath(), ns*247, "Reduced");
        foundFlag = false;

        for(int l = 0; l < channelsSetExclude.length(); ++l)
        {
            //clean wts
            cleanDir(dir->absolutePath(), "wts");
            cleanDir(dir->absolutePath(), "markers", 0);
            tempItem = channelsSetExclude[l];

            if(unneededChannels.contains(tempItem)) continue; //if the channel is veru bad

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << endl << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " start" << endl;
                logF.close();
            }

            channelsSet.push_back(tempItem); //add to considered channels
            channelsSetExclude.removeOne(tempItem); //temporarily remove from non-considered list

            //drop some channels
            helpString.clear();
            for(int k = 0; k < 19; ++k)
            {
                if(channelsSet.contains(k))
                {
                    helpString += QString::number(k+1) + " ";
                }
            }
            helpString += "20";
            ui->reduceChannelsLineEdit->setText(helpString);

            //exclude channels from 1st file
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //exclude channels from 2nd file
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //cross class
            helpString = fileName1;
            helpString.replace(".edf", "_rdc.edf");
            helpString2 = fileName2;
            helpString2.replace(".edf", "_rdc.edf");
            lastAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift);


            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " done" << endl;
                logF << "channelsSet:" << endl;
                for(int q = 0; q < channelsSet.length(); ++q)
                {
                    logF << channelsSet[q] + 1 << "  ";
                }
                logF << endl << "Accuracy = " << lastAccuracy << endl;
                logF.close();
            }


            //set back
            channelsSet.removeOne(tempItem); //pop tempItem back
            channelsSetExclude.insert(l, tempItem); //return tempItem onto its place in channelsSetExclude



            if(lastAccuracy > tempAccuracy + 0.5)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
            }
            else if(lastAccuracy < tempAccuracy - 2.0)
            {
                logF.open(logPath.toStdString().c_str(), ios_base::app);
                if(!logF.is_open())
                {
                    cout << "cannot open logFile: " << logPath.toStdString() << endl;
                }
                else
                {
                    logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to unneeded channels" << endl;
                    logF.close();
                }
                unneededChannels << tempItem;
            }

        } //end of cycle l

        if(foundFlag)
        {

            tempItem = channelsSetExclude[tempIndex];
            cout << endl << "channel " << tempItem << " at index " << tempIndex << " to add" << endl << endl;
            channelsSetExclude.removeOne(tempItem);
            channelsSet.push_back(tempItem);

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to add" << endl;
                logF << "currentAccuracy = " << tempAccuracy << endl;
                logF.close();
            }

        }
        else
        {

            break;
        }

    } //end of while(1)
    cout << ExpName.left(3).toStdString() << " ended in " << myTime.elapsed()/1000. << " sec" << endl;


    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << "FINAL SET" << endl;
        logF << ExpName.left(3).toStdString() << endl;
        logF << "channelsSet:" << endl;
        for(int q = 0; q < channelsSet.length(); ++q)
        {
            logF << channelsSet[q]+1 << "  ";
        }
        logF << endl << "unneededChannels:" <<endl;
        for(int q = 0; q < unneededChannels.length(); ++q)
        {
            logF << unneededChannels[q]+1 << "  ";
        }
        logF << endl << "Accuracy = " << tempAccuracy << endl;
        logF.close();
    }
    cleanDir(tmpDir->absolutePath(), "wts");
    cleanDir(tmpDir->absolutePath(), "markers", 0);
    delete tmpDir;
    cout << initAccuracy << "->" << tempAccuracy << endl;
    return tempAccuracy;
}

#endif // AUTOS_CPP
