#ifndef SLICES_CPP
#define SLICES_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"



void MainWindow::sliceWindFromReal()
{

    QStringList lst;
    QStringList nameFilters;
    QString helpString;
    int eyesCounter;
    QTime myTime;
    myTime.start();
    FILE *real;
    FILE *out;
    dir->cd("Realisations");
    lst.clear();
    nameFilters.clear();
    nameFilters << "*_241*";
    nameFilters << "*_247*";
    nameFilters << "*_244*";
    nameFilters << "*_254*";
    lst = dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot);
    dir->cdUp();


    int timeShift=ui->timeShiftSpinBox->value();
    int wndLength = ui->windowLengthSpinBox->value();

//    cout << "timeShift = " << timeShift << endl;
//    cout << "wndLength = " << wndLength << endl;

    double ** dataReal = new double *[ns];
    for(int i = 0; i < ns; ++i)
    {
        dataReal[i] = new double [250*60*1]; //generality 1 min
    }

    int eyes;
    int offset;
    for(int i = 0; i < lst.length(); ++i)
    {
        offset = 0;
        helpString = QDir::toNativeSeparators(dir->absolutePath()
                                              + slash() + "Realisations"
                                              + slash() + lst[i]);
        readPlainData(helpString, dataReal, ns, NumOfSlices);

        for(int h = 0; h < ceil((NumOfSlices - wndLength)/double(timeShift)); ++h)
        {

            //correct eyes number
            eyes = 0;
            for(int l = offset; l < (offset + wndLength); ++l)
            {
                eyesCounter = 0.;
                for(int m = 0; m < ns; ++m)
                {
                    if(dataReal[m][l] == 0.)  //generality different nr
                    eyesCounter += 1;
                }
                if(eyesCounter>=(ns-1)) eyes += 1;
            }

            if(eyes / double(wndLength) > 0.3)  //generality coefficient
            {
                fclose(out);

                if(remove(helpString.toStdString().c_str()) != 0)
                {
                    perror("cannot delete file");
                }

                continue;
            }
            //else


            helpString = dir->absolutePath()
                    + slash() + "windows"
                    + slash() + "fromreal"
                    + slash() + ExpName
                    + "_";
            if(helpString.contains("241"))
            {
                helpString += "241";
            }
            if(helpString.contains("247"))
            {
                helpString += "247";
            }
            if(helpString.contains("254"))
            {
                helpString += "254";
            }
            helpString += QString::number(i)+ "." + rightNumber(h, 2);
            helpString = QDir::toNativeSeparators(helpString);

            writePlainData(helpString, dataReal, ns, wndLength, offset);
            offset += timeShift;
        }
        ui->progressBar->setValue(i * 100. / lst.length());
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []dataReal[i];
    }
    delete []dataReal;

    ui->progressBar->setValue(0);

    helpString="windows from realisations sliced ";
    ui->textEdit->append(helpString);

    helpString="ns equals to ";
    helpString.append(QString::number(ns));
    ui->textEdit->append(helpString);

    cout << "WindFromReals: time = " << myTime.elapsed()/1000. << " sec" << endl;

    helpString.setNum(myTime.elapsed()/1000.);
    helpString.prepend("Data sliced \nTime = ");
    helpString.append(" sec");
    //automatization
    if(autoProcessingFlag == false)
    {
        QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
    }

}

void MainWindow::sliceIlya(const QString &fileName, QString marker) //beginning - from mark1 to mark 2, end - from 251 to 255. Marker - included in filename
{
    QString helpString;
    FILE * file;
    FILE * in;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    int tmpMarker;

    helpString=dir->absolutePath().append(slash()).append(fileName);
    in = fopen (QDir::toNativeSeparators(helpString).toStdString().c_str(), "r");
    if(in==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open file"), QMessageBox::Ok);
        return;
    }
    fscanf(in, "NumOfSlices %d\r\n", &NumOfSlices);
    double ** dataI = new double *[ns];
    for(int i = 0; i < ns; ++i)
    {
        dataI[i] = new double [NumOfSlices];
    }

    for(int i = 0; i < NumOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fscanf(in, "%lf", &dataI[j][i]);
        }
    }


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
            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append(marker).append(slash()).append(fileName).append("_").append(marker);
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
    for(int i = 0; i < ns; ++i)
    {
        delete []dataI[i];

    }
    delete []dataI;
}

void MainWindow::sliceIlya(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - from 251 to 255. Marker - included in filename
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            marker.setNum(int(data[ns-1][i]));
            continue;
        }
//        if(data[ns-1][i]>=251 && data[ns-1][i] <= 254)
        if(data[ns-1][i]==21 || data[ns-1][i]==22)
        {
            k=i+500;
            if(h==1) ++h;

        }
        if(h==2)
        {
            ++number;
            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append("_").append(marker);
            file=fopen(helpString.toStdString().c_str(), "w");

            fprintf(file, "NumOfSlices %d \r\n", k-j);
            for(int l=j; l < k; ++l)
            {
                for(int m = 0; m < ns; ++m)  //with markers
                {
                    fprintf(file, "%lf\r\n", data[m][l*nr[m]/nr[ns-1]]);
                }
            }
            fclose(file);
            i -= 2;
            h = 0;
        }
    }
}

void MainWindow::slice(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - 254. Marker - included in filename
{
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
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            continue;
        }
        if(data[ns-1][i]==254)
        {
            k=i;
            if(h==1)
            {
                h = 0;
                if(k-j < 500) continue;
                ++number;
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
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
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
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

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("results.txt")).toStdString().c_str(), "a+");
    if(ui->eyesCleanCheckBox->isChecked()) fprintf(res, "solve time %s \t %lf \n", marker.toStdString().c_str(), solveTime);
    fclose(res);
}

void MainWindow::sliceFromTo(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end - 254. Marker - included in filename
{
    QString helpString;
    FILE * file;
    FILE * out;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    double solveTime = 0.;

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("Help").append(slash()).append(marker));
    out = fopen(helpString.toStdString().c_str(), "w");
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(h== 0)
        {
            if(data[ns-1][i]==marker1)
            {
                j=i;
                h=1;
                continue;
            }
        }
        else
        {
            if(data[ns-1][i]==254)
            {
                h = 0;
                continue;
            }
            if(data[ns-1][i]==marker2)
            {
                k=i;
                if(h==1)
                {
                    h = 0;
                    if(k-j < 1000) continue;
                    ++number;
                    helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("Realisations").append(slash()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4)));
                    file=fopen(helpString.toStdString().c_str(), "w");
                    fprintf(file, "NumOfSlices %d \n", k-j);

                    fprintf(out, "%d \n", k-j);
                    solveTime += (k-j);

                    for(int l=j; l < k; ++l)
                    {
                        for(int m = 0; m < ns-1; ++m)
                        {
                            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                        }
                    }
                    fclose(file);
                    i += 17;
                }
            }
        }
    }
    fclose(out);
    helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("Help").append(slash()).append(marker));
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

    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("results.txt")).toStdString().c_str(), "a+");
    if(ExpName.contains("FB")) fprintf(res, "time before feedback %s\t%lf\n", marker.toStdString().c_str(), solveTime);
    fclose(res);
}

void MainWindow::sliceByNumberAfter(int marker1, int marker2, QString marker)
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            h=1;
            continue;
        }
        if(h==1 && data[ns-1][i] != 0 && data[ns-1][i]!=254)
        {
            h=2;
            j=i;
            continue;
        }
        if(h==1 && data[ns-1][i]==254)
        {
            h = 0;
            continue;
        }
        if(data[ns-1][i]==254)
        {
            k=i;
            if(h==2)
            {
                ++number;
                h = 0;
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices %d \n", k-j);

                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
                fclose(file);
                i += 17;
            }
        }
    }




}

void MainWindow::sliceBak(int marker1, int marker2, QString marker) //beginning - from mark1 to mark 2, end 250. Marker - included in filename
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag of marker2 read
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if((data[ns-1][i]>=marker1) && (data[ns-1][i] <= marker2) && h== 0)
        {
            j=i;
            h=1;
            continue;
        }
        if(data[ns-1][i]==250)
        {
            k=i;
            if(h==1) ++h;

        }
        if(h==2)
        {
            ++number;
            helpString=dir->absolutePath().append(slash()).append("Realisations").append(slash()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
            file=fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(), "w");

            fprintf(file, "NumOfSlices %d \n", k-j);
//            fprintf(file, "NumOfSlicesEyesCut 0 \n");
            for(int l=j; l < k; ++l)
            {
                for(int m = 0; m < ns-1; ++m)
                {
                    fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                }
            }
            fclose(file);
            i += 17;
            h = 0;
        }
    }
}

void MainWindow::sliceWindow(int startSlice, int endSlice, int number, int marker)
{
    QString helpString;
    if(endSlice - startSlice > 2500) return;

    int wndLength = ui->windowLengthSpinBox->value();


    double helpDouble = 0.;
    int helpInt = 0;
    //check real signal contained
    for(int l = startSlice; l < endSlice; ++l)
    {
        helpInt = 0;
        for(int m = 0; m < ns-1; ++m) // no marker channel
        {
            if(data[m][l*nr[m]/nr[ns-1]] == 0) ++helpInt;
        }
        if(helpInt == ns-1) helpDouble +=1.;
    }
    if(helpDouble > 0.1 * wndLength) return;

    FILE * file;
    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("windows").append(slash()).append(ExpName).append("-").append(rightNumber(number, 4)).append("_").append(QString::number(marker)); //number.marker
    file=fopen(helpString.toStdString().c_str(), "w");
    if(file==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("cannot open file"), QMessageBox::Ok);
        return;
    }

    fprintf(file, "NumOfSlices %d \n", endSlice-startSlice);
    for(int l = startSlice; l < endSlice; ++l)
    {
        for(int m = 0; m < ns-1; ++m) // no marker channel
        {
            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
        }
    }
    fclose(file);
}

void MainWindow::sliceGaps()
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;                                     //flag of marker1 read
    int h = 0;                                     //flag
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(data[ns-1][i]==254 && h== 0)
        {
            j=i;
            h=1;
            continue;
        }

        if(data[ns-1][i]==201 && h==1) //don't consider with pauses
        {
            h = 0;
            continue;
        }

        if(data[ns-1][i]!=254 && data[ns-1][i]!=255 && data[ns-1][i] != 0) //generality
        {
            k=i;
            if(h==1)
            {
                h = 0;
                if(k-j < 500 || (k-j)>10000) continue;
                ++number;
                helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append("_254.").append(rightNumber(number, 4));
                file=fopen(helpString.toStdString().c_str(), "w");
                fprintf(file, "NumOfSlices %d \n", k-j);
                for(int l=j; l < k; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }

                }
                fclose(file);
            }
        }
    }
}

void MainWindow::sliceOneByOne() //generality, just for my current
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;
    int h = 1;
    QString marker = "000";
    //200, 255, (241||247, num, 254, 255)
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(data[ns-1][i] == 0)
        {
            continue;
        }
        if((data[ns-1][i] > 200 && data[ns-1][i] < 241) || data[ns-1][i] == 255 || data[ns-1][i] == 250 || data[ns-1][i] == 251) //order inportant! - not interesting markers
        {
            continue;
        }
        if((data[ns-1][i] == 254 || data[ns-1][i] == 200) && h == 0) //start gap
        {
            j = i;
            h = 1;
            marker.setNum(data[ns-1][i]);
            continue;
        }
//        if((data[ns-1][i] == 241 || data[ns-1][i] == 247) && h != 1) //start task
        if((data[ns-1][i] == 241 || data[ns-1][i] == 247) && h == 0) //start task
        {
            j = i;
            h = 2;
            marker.setNum(data[ns-1][i]);
            continue;
        }
        if(data[ns-1][i] != 0 && h == 2) //write between unknown marker and 241 || 247
        {
            ++number;
            helpString = QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append("num");
            file = fopen(helpString.toStdString().c_str(), "w");
            fprintf(file, "NumOfSlices %d \n", i-j);
            if(i-j > 15000 && (marker == "255" || marker == "254"))
            {
                for(int l = j; l < i; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "0.000\n");
                    }
                }
            }
            else
            {
                for(int l = j; l < i; ++l)
                {
                    for(int m = 0; m < ns-1; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
            }
            fclose(file);


            h = 1;
            j = i;
            continue;
        }

        if(data[ns-1][i] != 0 && h == 1) //generality first nonzero marker after start - write
        {
            k = i;
            h = 0;
            ++number;
            helpString = QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
//            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
            file = fopen(helpString.toStdString().c_str(), "w");
            fprintf(file, "NumOfSlices %d \n", k-j);
            for(int l = j; l < k; ++l)
            {
                for(int m = 0; m < ns-1; ++m)
                {
                    fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                }

            }
            fclose(file);
            --i; //for checking the last marker once more
        }
//        if(number == 125) cout<<i<<endl;
    }

    //write last rest state
    marker = "end";
    k = ndr*nr[ns-1];
    ++number;
    helpString = QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
//    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append("_").append(marker).append(".").append(rightNumber(number, 4));
    file=fopen(helpString.toStdString().c_str(), "w");
    fprintf(file, "NumOfSlices %d \n", k-j);
    for(int l = j; l < k; ++l)
    {
        for(int m = 0; m < ns-1; ++m)
        {
            fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
        }

    }
    fclose(file);
}

void MainWindow::sliceOneByOneNew(int numChanWrite)
{
    QString helpString;
    FILE * file;
    int number = 0;
    int k;
    int j = 0;
    int h = 0; //h == 0 - 241, h == 1 - 247
    QString marker = "000";
    //200, 255, (241||247, num, 254, 255)
    for(int i = 0; i < ndr*nr[ns-1]; ++i)
    {
        if(data[ns-1][i] == 0)
        {
            continue;
        }
        else if((data[ns-1][i] > 200 && data[ns-1][i] < 241) || data[ns-1][i] == 255 || data[ns-1][i] == 250 || data[ns-1][i] == 251) //all not interesting markers
        {
            continue;
        }
        else if(data[ns-1][i] == 241 || data[ns-1][i] == 247)
        {
            marker = "254";
            if(data[ns-1][i] == 241) h = 0;
            else if (data[ns-1][i] == 247) h = 1;
            continue; // wait for num marker
        }
        else if(1) //marker can be <=200, ==254, smth else
        {
            if(marker.isEmpty())
            {
                marker = "sht";
            }

            ++number;

            helpString=QDir::toNativeSeparators(dir->absolutePath() + slash() + "Realisations" + slash() + ExpName + "." + rightNumber(number, 4) + "_" + marker);

            if(((marker == "000") && def::wirteStartEndLong) || (marker != "000")) // write the beginning?
            {
                file = fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices %d \n", i-j);
                for(int l = j; l < i; ++l)
                {
                    for(int m = 0; m < numChanWrite; ++m)
                    {
                        fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
                    }
                }
                fclose(file);
            }
            else if (i-j > 2500 && (marker == "254"))
            {
                file = fopen(helpString.toStdString().c_str(), "w");

                fprintf(file, "NumOfSlices 1000 \n");
                for(int l = 0; l < 1000; ++l)
                {
                    for(int m = 0; m < numChanWrite; ++m)
                    {
                        fprintf(file, "0.000\n");
                    }
                }
                fclose(file);
            }

            ui->progressBar->setValue(double(i)*100./ndr*nr[ns-1]);
            qApp->processEvents();

            marker.clear();
            if(h == 0) marker = "241";
            else if(h == 1) marker = "247";
            h = -1;
            j = i;
            continue;
        }

    }

    //write last file
    if(0)
    {
        marker = "254";
        ++number;
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
        file = fopen(helpString.toStdString().c_str(), "w");
        fprintf(file, "NumOfSlices 10 \n");
        for(int l = 0; l < 10; ++l)
        {
            for(int m = 0; m < numChanWrite; ++m)
            {
                if(m != numChanWrite-1 && l != 0)
                {
                    fprintf(file, "0.000\n");
                }
                else
                {
                    fprintf(file, "254.000\n");
                }
            }
        }
        fclose(file);
    }
    else if(0)
    {
        //write last rest state
        k = ndr*nr[ns-1];
        ++number;
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(slash()).append("Realisations").append(slash()).append(ExpName).append(".").append(rightNumber(number, 4)).append("_").append(marker);
        file=fopen(helpString.toStdString().c_str(), "w");
        fprintf(file, "NumOfSlices %d \n", k-j);
        for(int l = j; l < k; ++l)
        {
            for(int m = 0; m < numChanWrite; ++m)
            {
                fprintf(file, "%lf\n", data[m][l*nr[m]/nr[ns-1]]);
            }
        }
        fclose(file);
    }

}

void MainWindow::sliceMatiSimple()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int start = 0;
    int end = -1;
    vector<bool> markers;
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
                    helpString = QDir::toNativeSeparators(dir->absolutePath()
                                                          + QDir::separator() + "Realisations"
                                                          + QDir::separator() + ExpName
                                                          + "_" + QString::number(type)
                                                          + "_" + QString::number(session[type])
                                                          + "_" + rightNumber(j, 2)
                                                          + '.' + fileMark);

                    //                outStream.open(helpString.toStdString().c_str());
                    NumOfSlices = min(end - start - j*piece, piece);
#if 1
                    writePlainData(helpString, fil.getData(), ns, NumOfSlices, start + j*piece);
#else

                    file = fopen(helpString.toStdString().c_str(), "w");
                    fprintf(file, "NumOfSlices %d \n", NumOfSlices);
                    //                outStream << "NumOfSlices " << NumOfSlices << endl;
                    {
                        for(int l = start  + j*piece; l < min(start + (j+1)*piece, end); ++l) //end slice included with session end marker
                        {
                            for(int m = 0; m < numChanWrite; ++m)
                            {
                                fprintf(file, "%.4lf\n", data[m][l]); // generality l * nr[m] / nr[ns-1]
                                //                            outStream << data[m][l] << '\n';
                            }
                        }
                    }
                    fclose(file);
                    //                outStream.close();
#endif
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
    vector<bool> markers;
    bool state[3];
    QString fileMark;
    int session[4]; //generality
    int type = 3;

    for(int i = 0; i < 4; ++i)
    {
        session[i] = 0;
    }

    edfFile & fil = globalEdf;
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
                helpString = QDir::toNativeSeparators(dir->absolutePath()

                                                      + slash() + "auxEdfs"

                                                      + slash() + ExpName
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
    int pieceNum;
    int currStart;
    int currEnd;
    double pieceLength = ui->matiPieceLengthSpinBox->value();

    dir->cd(globalEdf.getDirPath());
    edfFile fil;

    for(int type = 0; type < 3; ++type)
    {
        for(int session = 0; session < 15; ++session)
        {
            // edf session path
            helpString = QDir::toNativeSeparators(dir->absolutePath()

                                                  + slash() + "auxEdfs"

                                                  + slash() + globalEdf.getExpName()
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

                do
                {
                    currEnd = min(int(currStart + pieceLength * def::freq), dataLen);

                    if(type == 0 || type == 2)
                    {
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

                    // type and session already in the ExpName
                    helpString = QDir::toNativeSeparators(dir->absolutePath()
                                                          + slash() + "Realisations"
                                                          + slash() + fil.getExpName()
                                                          + "_" + rightNumber(pieceNum, 2)
                                                          + '.' + fileMark);
                    fil.saveSubsection(currStart, currEnd, helpString, plainFlag);
                    ++pieceNum;
                    currStart = currEnd;

                } while (!matiCountBit(fil.getData()[fil.getMarkChan()][currEnd - 1], 10) );
            }
        }
    }
    cout << "sliceMatiPieces: time = " << myTime.elapsed()/1000. << " sec" << endl;
}

#endif
