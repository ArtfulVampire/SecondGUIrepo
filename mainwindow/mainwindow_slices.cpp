#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace myLib;

void MainWindow::sliceWindFromReal()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int eyesCounter;


    QStringList lst;
    makeFullFileList(def::dir->absolutePath()
					 + slash + "Reals",
                     lst
                     , {def::ExpName}
                     );

    const int timeShift = ui->timeShiftSpinBox->value() * def::freq;
    const int wndLength = ui->windowLengthSpinBox->value() * def::freq;

    matrix dataReal;

    int eyes;
    int offset;
    int NumOfSlices;

    int localNs = def::ns;
    for(int i = 0; i < lst.length(); ++i)
    {
        localNs = def::ns;
        helpString = (def::dir->absolutePath()
											  + slash + "Reals"
                                              + slash + lst[i]);
        readPlainData(helpString, dataReal, NumOfSlices);

        offset = 0;
        for(int h = 0; h < ceil((NumOfSlices - wndLength) / double(timeShift)); ++h)
        {
            //correct eyes number
            eyes = 0;
            for(int l = offset; l < min(offset + wndLength, NumOfSlices); ++l)
            {
                eyesCounter = 0.;
                for(int m = 0; m < localNs; ++m)
                {
                    if(dataReal[m][l] == 0.)  //generality different nr
                    eyesCounter += 1;
                }
                if(eyesCounter >= (localNs - 1)) eyes += 1;
            }

            if(eyes / double(wndLength) > 0.3)  //generality coefficient
            {
                continue;
            }

            //else
            helpString = def::dir->absolutePath()
						 + slash + "winds"
                         + slash + "fromreal"
                         + slash + lst[i]
                         + "." + rightNumber(h, 2);
            helpString = (helpString);

            /// wowo wowoww wowowowo owowwowo
            writePlainData(helpString, dataReal, wndLength, offset);

            offset += timeShift;
        }
        qApp->processEvents();
        if(stopFlag)
        {
            stopFlag = false;
            break;
        }

        ui->progressBar->setValue(i * 100. / lst.length());
    }

    ui->progressBar->setValue(0);
	helpString = "winds from Reals sliced ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to " + QString::number(def::ns);
    ui->textEdit->append(helpString);

    cout << "sliceWindFromReal: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

void MainWindow::sliceIlya(const QString &fileName, QString marker) //beginning - from mark1 to mark 2, end - from 251 to 255. Marker - included in filename
{
#if 0
    QString helpString;
    FILE * file;
    FILE * in;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    int tmpMarker;
    int NumOfSlices;

    helpString = def::dir->absolutePath()
            + slash + fileName;

    matrix dataI;
    readPlainData(helpString, dataI, NumOfSlices);



    for(int i = 0; i < NumOfSlices; ++i)
    {
        if(dataI[ns-1][i] != 0 && dataI[ns-1][i]!=1 && (dataI[ns-1][i] < 50 || dataI[ns-1][i]>53) && h== 0)
        {
            j=i;
            h=1;
            tmpMarker=dataI[ns-1][i];
            continue;
        }
        if(dataI[ns-1][i] != 0 && dataI[ns-1][i]!=1 && (dataI[ns-1][i] < 50 || dataI[ns-1][i]>53) && h==1)
        {

            if(dataI[ns-1][i]==27)
            {
                k=i+250;
                if(tmpMarker==21) marker="DODO";
                if(tmpMarker==22) marker="DONT";
            }
            else
            {
                k=i;
                if(dataI[ns-1][i]==2) marker="Begin";
                if(dataI[ns-1][i]>=11 && dataI[ns-1][i] <= 15) marker="Beep";
                if(dataI[ns-1][i]==21 || dataI[ns-1][i]==22) marker="St";
            }
            ++h;

        }
        if(h==2)
        {
            ++number;
            helpString=(def::dir->absolutePath()).append(slash).append(marker).append(slash).append(fileName).append("_").append(marker);
            file=fopen(helpString.toStdString().c_str(), "w");

            fprintf(file, "NumOfSlices %d \r\n", k-j);
            for(int l=j; l < k; ++l)
            {
                for(int m = 0; m < ns-1; ++m)   //w/o markers
                {
                    fprintf(file, "%lf\r\n", dataI[m][l]);
                }
            }
            fclose(file);
            i -= 2;
            h = 0;
        }
    }
#endif
}

void MainWindow::sliceIlya(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - from 251 to 255. Marker - included in filename
{
#if 0
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr * def::freq; ++i)
    {
        if((markChanArr[i]>=marker1) && (markChanArr[i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            marker.setNum(int(markChanArr[i]));
            continue;
        }
//        if(markChanArr[i]>=251 && markChanArr[i] <= 254)
        if(markChanArr[i]==21 || markChanArr[i]==22)
        {
            k=i+500;
            if(h==1) ++h;

        }
        if(h==2)
        {
            ++number;
			helpString=(def::dir->absolutePath()).append(slash).append("Reals").append(slash).append(def::ExpName).append("_").append(marker);
            file=fopen(helpString.toStdString().c_str(), "w");

            fprintf(file, "NumOfSlices %d \r\n", k-j);
            for(int l=j; l < k; ++l)
            {
                for(int m = 0; m < ns; ++m)  //with markers
                {
                    fprintf(file, "%lf\r\n", data[m][l*nr[m]/def::freq]);
                }
            }
            fclose(file);
            i -= 2;
            h = 0;
        }
    }
#endif
}

void MainWindow::slice(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - 254. Marker - included in filename
{
    // for Boris neurotravel
#if 0
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    double solveTime = 0.;
//    double solve[4];
//    for(int i = 0; i < 4; ++i)
//    {
//        solve[i] = 0.;
//    }
    for(int i = 0; i < ndr*def::freq; ++i)
    {
        if((markChanArr[i]>=marker1) && (markChanArr[i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            continue;
        }
        if(markChanArr[i]==254)
        {
            k=i;
            if(h==1)
            {
                h = 0;
                if(k-j < 500) continue;
                ++number;
				helpString=(def::dir->absolutePath()).append(slash).append("Reals").append(slash).append(def::ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices %d \n", k-j);
                solveTime += (k-j);


//                switch(number/12)
//                {
//                case 0: {solve[0] += (k-j); break;}
//                case 1: {solve[1] += (k-j); break;}
//                case 2: {solve[2] += (k-j); break;}
//                case 3: {solve[3] += (k-j); break;}
//                default: break;
//                }

                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/def::freq]);
                    }
                }
                fclose(file);
                i += 17;
            }
        }
    }
//    cout << number << endl;
//    for(int i = 0; i < 4; ++i)
//    {
//        cout << "solve[" << i << "]=" << solve[i]/(12 *def::freq) << endl;
//    }

    solveTime/=(def::freq*number);
    cout << "solveTime " << marker.toStdString() << " =" << solveTime << endl << endl;

    FILE * res = fopen((def::dir->absolutePath().append(slash).append("results.txt")).toStdString().c_str(), "a+");
    if(ui->eyesCleanCheckBox->isChecked()) fprintf(res, "solve time %s \t %lf \n", marker.toStdString().c_str(), solveTime);
    fclose(res);
#endif
}

void MainWindow::sliceFromTo(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - 254. Marker - included in filename
{
    // for biofeedback starts
#if 0
    QString helpString;
    FILE * file;
    FILE * out;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    double solveTime = 0.;

    helpString=(def::dir->absolutePath().append(slash).append("Help").append(slash).append(marker));
    out = fopen(helpString.toStdString().c_str(), "w");
    for(int i = 0; i < ndr*def::freq; ++i)
    {
        if(h== 0)
        {
            if(markChanArr[i]==marker1)
            {
                j=i;
                h=1;
                continue;
            }
        }
        else
        {
            if(markChanArr[i]==254)
            {
                h = 0;
                continue;
            }
            if(markChanArr[i]==marker2)
            {
                k=i;
                if(h==1)
                {
                    h = 0;
                    if(k-j < 1000) continue;
                    ++number;
					helpString=(def::dir->absolutePath().append(slash).append("Reals").append(slash).append(def::ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4)));
                    file=fopen(helpString.toStdString().c_str(), "w");
                    fprintf(file, "NumOfSlices %d \n", k-j);

                    fprintf(out, "%d \n", k-j);
                    solveTime += (k-j);

                    for(int l=j; l < k; ++l)
                    {
                        for(int m = 0; m < ns-1; ++m)
                        {
                            fprintf(file, "%lf\n", data[m][l*nr[m]/def::freq]);
                        }
                    }
                    fclose(file);
                    i += 17;
                }
            }
        }
    }
    fclose(out);
    helpString=(def::dir->absolutePath().append(slash).append("Help").append(slash).append(marker));
    QFile *file2 = new QFile(helpString);
    file2->open(QIODevice::ReadOnly);
    QByteArray contents = file2->readAll();
    file2->close();
    file2->open(QIODevice::WriteOnly);
    file2->write("NumOfFiles ");
    file2->write(QString::number(number).toStdString().c_str());
    file2->write("\r\n");
    file2->write(contents);
    file2->close();
    delete file2;

//    kernelest(helpString);

    solveTime/=(def::freq*number);
//    cout << "average time before feedback " << marker.toStdString() << " =" << solveTime << endl;

    FILE * res = fopen((def::dir->absolutePath().append(slash).append("results.txt")).toStdString().c_str(), "a+");
    if(def::ExpName.contains("FB")) fprintf(res, "time before feedback %s\t%lf\n", marker.toStdString().c_str(), solveTime);
    fclose(res);
#endif
}

void MainWindow::sliceByNumberAfter(int marker1, int marker2, QString marker)
{
    //deprecated
#if 0
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr*def::freq; ++i)
    {
        if((markChanArr[i]>=marker1) && (markChanArr[i] <= marker2) && h== 0)
        {
            h=1;
            continue;
        }
        if(h==1 && markChanArr[i] != 0 && markChanArr[i]!=254)
        {
            h=2;
            j=i;
            continue;
        }
        if(h==1 && markChanArr[i]==254)
        {
            h = 0;
            continue;
        }
        if(markChanArr[i]==254)
        {
            k=i;
            if(h==2)
            {
                ++number;
                h = 0;
				helpString=(def::dir->absolutePath()).append(slash).append("Reals").append(slash).append(def::ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices %d \n", k-j);

                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/def::freq]);
                    }
                }
                fclose(file);
                i += 17;
            }
        }
    }

#endif


}

void MainWindow::sliceBak(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end 250. Marker - included in filename
{
    // for Baklushev
    QString helpString;

    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    const edfFile & fil = globalEdf;

    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        if((fil.getData()[fil.getMarkChan()][i] >= marker1)
                && (fil.getData()[fil.getMarkChan()][i] <= marker2) && h== 0)
        {
            j = i;
            h = 1;
            continue;
        }
        if(fil.getData()[fil.getMarkChan()][i] == 250.)
        {
            k = i;
            if(h == 1) ++h;
        }
        if(h == 2)
        {
            ++number;
            helpString = def::dir->absolutePath()
					+ slash + "Reals"
                    + slash + def::ExpName
                    + "_" + marker
                    + "." + rightNumber(number, 4);
            // to test?
            fil.saveSubsection(j, k, helpString, true);

            i += 17;
            h = 0;
        }
    }
}

void MainWindow::sliceWindow(int startSlice, int endSlice, int number, int marker)
{
    QString helpString;

//    const int wndLength = ui->windowLengthSpinBox->value() * def::freq;

//    double helpDouble = 0.;
//    int helpInt = 0;

//    //check real signal contained
//    for(int l = startSlice; l < endSlice; ++l)
//    {
//        helpInt = 0;
//        for(int m = 0; m < def::ns - 1; ++m) // no marker channel
//        {
//            if(globalEdf.getData()[m][l*nr[m]/nr[def::ns - 1]] == 0) ++helpInt;
//        }
//        if(helpInt == def::ns - 1) helpDouble +=1.;
//    }
//    if(helpDouble > 0.1 * wndLength) return;


    helpString = (def::dir->absolutePath()
										  + slash + "winds"
                                          + slash + def::ExpName
                                          + "-" + rightNumber(number, 4)
                                          + "_" + QString::number(marker)); //number.marker

    globalEdf.saveSubsection(startSlice, endSlice, helpString, true);
}

void MainWindow::sliceGaps()
{

//deprecated

#if 0
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag
    for(int i = 0; i < ndr*def::freq; ++i)
    {
        if(markChanArr[i]==254 && h== 0)
        {
            j=i;
            h=1;
            continue;
        }

        if(markChanArr[i]==201 && h==1) //don't consider with pauses
        {
            h = 0;
            continue;
        }

        if(markChanArr[i]!=254 && markChanArr[i]!=255 && markChanArr[i] != 0) //generality
        {
            k=i;
            if(h==1)
            {
                h = 0;
                if(k-j < 500 || (k-j)>10000) continue;
                ++number;
				helpString=(def::dir->absolutePath()).append(slash).append("Reals").append(slash).append(def::ExpName).append("_254.").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");
                fprintf(file, "NumOfSlices %d \n", k-j);
                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/def::freq]);
                    }

                }
                fclose(file);
            }
        }
    }
#endif
}

void MainWindow::sliceOneByOne()
{
    QString helpString;
    int number = 0;
    QString marker = "000";
    int start = 0;

    const edfFile & fil = globalEdf;
    const lineType & markChanArr = fil.getData()[fil.getMarkChan()];

    // 200, (241||247, (1), 254, 255)
    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        if(markChanArr[i] == 0 ||
           !(markChanArr[i] == 241 ||
             markChanArr[i] == 247 ||
             markChanArr[i] == 254))
        {
            continue;
        }
        else
        {
            const int & finish = i;

            helpString = def::dir->absolutePath()
						 + slash + "Reals"
                         + slash + def::ExpName
                         + "." + rightNumber(number++, 4);


            if(finish > start)
            {
                if(finish - start <= def::freq * 42) /// const generality limit
                {
                    helpString += "_" + marker;
                    fil.saveSubsection(start,
                                       finish,
                                       helpString, true);
                }
                else /// pause rest
                {
                    if(def::wirteStartEndLong)
                    {
                        helpString += "_000";
                        fil.saveSubsection(start,
                                           finish,
                                           helpString, true);
                    }
                    else
                    {
                        helpString += "_" + marker;
                        matrix tempData(fil.getNs(), 100, 0.);
                        tempData[fil.getMarkChan()][0] = markChanArr[start];
                        writePlainData(helpString, tempData);
                    }
                }
            }
            ui->progressBar->setValue(i * 100. / fil.getDataLen());

            qApp->processEvents();
            if(stopFlag)
            {
                stopFlag = 0;
                return;
            }

            marker = QString::number(markChanArr[finish]);
            start = finish;
        }
    }
    /// write final
    {
        helpString = def::dir->absolutePath()
					 + slash + "Reals"
                     + slash + def::ExpName
                     + "." + rightNumber(number++, 4);
        if(fil.getDataLen() - start < 40 * def::freq) /// if last realisation or interstimulus
        {
            helpString += "_" + marker;
            fil.saveSubsection(start,
                               fil.getDataLen(),
                               helpString, true);
        }
        else /// just last big rest with eyes closed/open
        {
            if(def::wirteStartEndLong)
            {
                helpString += "_000";
                fil.saveSubsection(start,
                                   fil.getDataLen(),
                                   helpString, true);
            }
            else /// not to loose the last marker
            {
                helpString += "_" + marker;
                matrix tempData(fil.getNs(), 100, 0.);
                tempData[fil.getMarkChan()][0] = markChanArr[start];
                writePlainData(helpString, tempData);
            }
        }
    }
}

void MainWindow::sliceOneByOneNew() // deprecated numChanWrite - always with markers
{
    QString helpString;
    int number = 0;
    int j = 0;
    int h = 0; //h == 0 - 241, h == 1 - 247
    QString marker = "000";

    const edfFile & fil = globalEdf;

    const lineType & markChanArr = fil.getData()[fil.getMarkChan()];

    // 200, 255, (241||247, num, 254, 255)
    // with feedback 200 (241||247, num, 231||237, (234), 254, 255)
    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        if(markChanArr[i] == 0.)
        {
            continue;
        }
        else if((markChanArr[i] > 200 && markChanArr[i] < 241) ||
                markChanArr[i] == 255 ||
                markChanArr[i] == 250 ||
                markChanArr[i] == 251) //all not interesting markers
        {
            continue;
        }
        else if(markChanArr[i] == 241 || markChanArr[i] == 247)
        {
            marker = "254";
            if(markChanArr[i] == 241) h = 0;
            else if (markChanArr[i] == 247) h = 1;
            continue; // wait for num marker
        }
        else if(true) //marker can be num <= 200, ==254, smth else
        {
            if(marker.isEmpty())
            {
                marker = "sht";
            }

            helpString = def::dir->absolutePath()
						 + slash + "Reals"
                         + slash + def::ExpName
                         + "." + rightNumber(number++, 4);
//            cout << helpString << endl;
            if(i > j)
            {
                if(i - j <= def::freq * 60) /// const generality limit
                {
                    helpString += "_" + marker;
                    fil.saveSubsection(j, i, helpString, true);
                }
                else /// pause rest
                {
                    if(def::wirteStartEndLong)
                    {
                        helpString += "_000";
                        fil.saveSubsection(j, i, helpString, true);
                    }
                    else
                    {
                        helpString += "_" + marker;
                        matrix tempData(fil.getNs(), 100, 0.);
                        tempData[fil.getMarkChan()][0] = markChanArr[j];
                        writePlainData(helpString, tempData);
                    }
                }
            }
            ui->progressBar->setValue(i * 100. / fil.getDataLen());

            qApp->processEvents();
            if(stopFlag)
            {
                stopFlag = 0;
                break;
            }

            marker.clear();
            if(h == 0) marker = "241";
            else if(h == 1) marker = "247";
            h = -1;
            j = i;
            continue;
        }
    }
    /// write final
    {
        helpString = def::dir->absolutePath()
					 + slash + "Reals"
                     + slash + def::ExpName
                     + "." + rightNumber(number++, 4);
        if(fil.getDataLen() - j < 40 * def::freq) /// if last realisation or interstimulus
        {
            helpString += "_" + marker;
            fil.saveSubsection(j, fil.getDataLen(), helpString, true);
        }
        else /// just last big rest with eyes closed/open
        {
            if(def::wirteStartEndLong)
            {
                helpString += "_000";
                fil.saveSubsection(j, fil.getDataLen(), helpString, true);
            }
            else /// not to loose the last marker
            {
                helpString += "_" + marker;
                matrix tempData(fil.getNs(), 100, 0.);
                tempData[fil.getMarkChan()][0] = markChanArr[j];
                writePlainData(helpString, tempData);
            }
        }
    }

}

void MainWindow::sliceMatiSimple()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int start = 0;
    int end = -1;
    std::vector<bool> markers;
    bool state[3];
    QString fileMark;
    int session[4]; //generality
    int type = 3;

    for(int i = 0; i < 4; ++i)
    {
        session[i] = 0;
    }

    const edfFile & fil = globalEdf;
    double currMarker;
    int number;
    double piece = ui->matiPieceLengthSpinBox->value() * def::freq;

    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        currMarker = fil.getData()[fil.getMarkChan()][i];
        if(currMarker == 0)
        {
            continue;
        }
        else
        {
            markers = matiCountByte(currMarker);
            //decide whether the marker is interesting: 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0
            for(int i = 0; i < 3; ++i)
            {
                state[i] = markers[i + 8]; //always elder byte is for count adn type of the session
            }

            if(!(state[0] || state[1] || state[2])) continue; // if all are zeros

            if(state[2] == 1) //the end of a session
            {
                if(state[1] == 0 && state[0] == 1) //end of a counting session
                {
                    type = 0;
                    fileMark = "241"; //count
                }
                if(state[1] == 1 && state[0] == 0) //end of a tracking session
                {
                    type = 1;
                    fileMark = "247"; //follow
                }
                if(state[1] == 1 && state[0] == 1) //end of a composed session
                {
                    type = 2;
                    fileMark = "244"; //composed
                }
            }
            else //if the start of a session
            {
                type = 3;
                fileMark = "254"; //rest. start of the session is sliced too
            }
            end = i + 1; // end marker should be included
        }

        //save session edf
        if(end > start)
        {
            if(state[2]) // if not rest
            {
                number = int(ceil((end-start)/double(piece)));

                for(int j = 0; j < number; ++j) // num of pieces
                {
                    helpString = (def::dir->absolutePath()
														  + slash + "Reals"
                                                          + slash + def::ExpName
                                                          + "_" + QString::number(type)
                                                          + "_" + QString::number(session[type])
                                                          + "_" + rightNumber(j, 2)
                                                          + '.' + fileMark);

                    int NumOfSlices = min(end - start - j * piece, piece);
                    writePlainData(helpString, fil.getData(), NumOfSlices, start + j * piece);
                }
                fileMark.clear();
                ++session[type];
            }

            start = end - 1; // = i // start marker should be included
            end = -1;
        }
        ui->progressBar->setValue(end * 100. / fil.getDataLen());

        qApp->processEvents();
        if (stopFlag == 1)
        {
            break;
        }
    }
    ui->progressBar->setValue(0);

    cout << "sliceMatiSimple: time = " << myTime.elapsed()/1000. << " sec" << endl;
    stopFlag = 0;
}

void MainWindow::sliceMati()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int start = 0;
    int end = -1;
    std::vector<bool> markers;
    bool state[3];
    QString fileMark;
    int session[4]; //generality
    int type = 3;

    for(int i = 0; i < 4; ++i)
    {
        session[i] = 0;
    }

    const edfFile & fil = globalEdf;
    double currMarker;


    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        currMarker = fil.getData()[fil.getMarkChan()][i];
        if(currMarker == 0)
        {
            continue;
        }
        else
        {
            markers = matiCountByte(currMarker);
            //decide whether the marker is interesting: 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0
            for(int i = 0; i < 3; ++i)
            {
                state[i] = markers[i + 8]; //always elder byte is for count adn type of the session
            }

            if(!(state[0] || state[1] || state[2])) continue; // if all are zeros

            if(state[2] == 1) //the end of a session
            {
                if(state[1] == 0 && state[0] == 1) //end of a counting session
                {
                    type = 0;
                    fileMark = "241"; //count
                }
                if(state[1] == 1 && state[0] == 0) //end of a tracking session
                {
                    type = 1;
                    fileMark = "247"; //follow
                }
                if(state[1] == 1 && state[0] == 1) //end of a composed session
                {
                    type = 2;
                    fileMark = "244"; //composed
                }
            }
            else //if the start of a session
            {
                type = 3;
                fileMark = "254"; //rest. start of the session is sliced too
            }
            end = i + 1; // end marker should be included
        }

        //save session edf
        if(end > start)
        {
            if(type != 3) // dont write rests
            {
                helpString = (def::dir->absolutePath()

                                                      + slash + "auxEdfs"

                                                      + slash + def::ExpName
                                                      + "_" + QString::number(type)
                                                      + "_" + QString::number(session[type])
                                                      + ".edf");

                fil.saveSubsection(start,
                                   end,
                                   helpString);
            }

            start = end - 1; //start marker should be included
            end = -1;
            ++session[type];
        }
        ui->progressBar->setValue(end * 100. / fil.getDataLen());

        qApp->processEvents();
        if (stopFlag == 1)
        {
            break;
        }
    }
    ui->progressBar->setValue(0);
    cout << "sliceMati: time = " << myTime.elapsed()/1000. << " sec" << endl;
    stopFlag = 0;
}

void MainWindow::sliceMatiPieces(bool plainFlag)
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int dataLen;
    QString fileMark;
    QString folder;
    int pieceNum;
    int currStart;
    int currEnd;
    const double pieceLength = ui->matiPieceLengthSpinBox->value();
    const bool adjustPieces = ui->adjustPiecesCheckBox->isChecked();

    def::dir->cd(globalEdf.getDirPath());
    edfFile fil;

    if(pieceLength <= 4.)
    {
		folder = "winds";
    }
    else
    {
		folder = "Reals";
    }

    for(int type = 0; type < 3; ++type)
    {
        for(int session = 0; session < 15; ++session)
        {
            // edf session path
            helpString = (def::dir->absolutePath()

                                                  + slash + "auxEdfs"

                                                  + slash + globalEdf.getExpName()
                                                  + "_" + QString::number(type)
                                                  + "_" + QString::number(session)
                                                  + ".edf");

            if(QFile::exists(helpString))
            {
                fil.readEdfFile(helpString);
                dataLen = fil.getDataLen();
                pieceNum = 0;
                currStart = 0;
                currEnd = -1; // [currStart, currEnd)

                if(type == 0)       fileMark = "241";
                else if(type == 1)  fileMark = "247";
                else if(type == 2)  fileMark = "244";
                else                fileMark = "254";

                if(adjustPieces)
                {
                    do
                    {
                        currEnd = min(int(currStart + pieceLength * def::freq), dataLen);

                        if(type == 0 || type == 2)
                        {
                            // std::search
                            while ( ! (matiCountBit(fil.getData()[fil.getMarkChan()][currEnd-1], 14) ||
                                       matiCountBit(fil.getData()[fil.getMarkChan()][currEnd-1], 10)) ) // while not (given answer OR session End)
                            {
                                --currEnd;
                            }
                        }
                        else if(currEnd == dataLen) // should do nothing due to edfFile::cutZerosAtEnd
                        {
                            while ( ! (matiCountBit(fil.getData()[fil.getMarkChan()][currEnd - 1], 10)) ) // while not session end
                            {
                                --currEnd;
                            }
                        }

                        if(currEnd <= currStart) // no count answers during pieceLength seconds
                        {
                            currEnd = min(int(currStart + pieceLength * def::freq), dataLen);
                        }

                        // type and session already in the fil.ExpName

                        helpString = (def::dir->absolutePath()
                                                              + slash + folder
                                                              + slash + fil.getExpName()
                                                              + "_" + rightNumber(pieceNum, 2)
                                                              + '_' + fileMark);


                        fil.saveSubsection(currStart, currEnd, helpString, plainFlag);
                        ++pieceNum;
                        currStart = currEnd;

                    } while (!matiCountBit(fil.getData()[fil.getMarkChan()][currEnd - 1], 10) );
                }
                else
                {
                    while(currStart < dataLen)
                    {
                        currEnd = min(int(currStart + pieceLength * def::freq), dataLen);
                        helpString = (def::dir->absolutePath()
                                                              + slash + folder
                                                              + slash + fil.getExpName()
                                                              + "_" + rightNumber(pieceNum, 2)
                                                              + '_' + fileMark);
                        fil.saveSubsection(currStart, currEnd, helpString, plainFlag);
                        ++pieceNum;
                        currStart = currEnd;
                    }
                }
            }
        }
    }
    cout << "sliceMatiPieces: time = " << myTime.elapsed() / 1000. << " sec" << endl;
}
