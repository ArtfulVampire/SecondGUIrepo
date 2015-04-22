#ifndef VISUAL_CPP
#define VISUAL_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::visualisation()   //just video
{
#if 0
        QString helpString;
        ns = 20;

        spLength = 63;


        char *helpCharArr = new char[50];
        int staSlice = 0;
        int NumOfSlices;
        setlocale(LC_NUMERIC, "C");

        helpString = dir->absolutePath()
                + slash() + ExpName + ".dat";
        FILE * datFile = fopen(helpString,"r");
        if(!datFile)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open dat-file"), QMessageBox::Ok);
            return;
        }
        fscanf(datFile, "NumOfSlices %d", &NumOfSlices);

        double ** data = new double * [ns];
        for(int i = 0; i < ns; ++i)
        {
            data[i] = new double [NumOfSlices];
        }

        for(int i = 0; i < NumOfSlices; ++i)
        {
            for(int j = 0; j < ns; ++j)
            {
                fscanf(datFile, "%lf\n", &data[j][i]);
            }
        }
        fclose(datFile);


        delete []helpCharArr;


        int NumOfClasses=3;
        int NetLength=19*63;  //generality

        double ** weight = new double * [NumOfClasses];
        for(int i = 0; i < NumOfClasses; ++i)
        {
            weight[i] = new double [NetLength+1];
        }



        helpString = dir->absolutePath().append(slash()).append("weights.wts"); //wts Name generality
        FILE * wts=fopen(helpString.toStdString().c_str(),"r"); // path generality
        if(wts==NULL)
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open wts-file"), QMessageBox::Ok);
            return;
        }

        for(int i = 0; i < NumOfClasses*(NetLength); ++i)
        {
            fscanf(wts, "%lf", &weight[i/(NetLength)][i%(NetLength)]);
        }
        for(int i = 0; i < NumOfClasses; ++i)
        {
            fscanf(wts, "%lf", &weight[i][NetLength]);
        }
        fclose(wts);


        dir->cd("visualisation");
        //generality
        dir->rmdir("video");
        dir->mkdir("video");

        dir->cd("video");



//        QStringList lst = dir->entryList(QDir::Files);
//        for(int i = 0; i < lst.length(); ++i)
//        {
//            remove(lst.at(i).toStdString().c_str());
//        }
        helpString = dir->absolutePath().append(slash()).append("visual.mpg");
        if(QFile::exists(helpString))
        {
            remove(helpString.toStdString().c_str());
        }


        QPixmap pic;
        QPainter * painter = new QPainter();


        double * spectre;
        spectre = new double [2*1024];  //generality


        double **dataFFT = new double * [ns];
        for(int k = 0; k < ns; ++k)
        {
            dataFFT[k] = new double [85];
        }

        double * matrix = new double [NetLength+1];

        double temperature = 1000.;

        int offsetTime = 0;  //in seconds
        int timeShift = 25;    //in slices
        int wholeLength = NumOfSlices/250-4;  //in seconds
        int length = 7;        //displayed window time in seconds

        int outNorm=40;       //graph. parameter

        int helpInt=ceil(wholeLength*250/double(timeShift));   //num of windows
    //    helpInt=300;

        double help1, help2;   //for smooth
        double ** output = new double * [NumOfClasses];

        for(int i = 0; i < NumOfClasses; ++i)
        {
            output[i] = new double [helpInt];
        }

//        cout << "start" << endl;

        int l;                  //
        int percentage = 0;

        for(int i = 0; i < 250*wholeLength; i += timeShift)

        {
//            cout << "1 " << i << endl;
            if((i-(staSlice+250*offsetTime))*100/(wholeLength*250)>percentage)
            {
                percentage=(i-(staSlice+250*offsetTime))*100/(wholeLength*250);
                cout << percentage << "% completed" << endl;
            }
            l=i-(staSlice+250*offsetTime);

//            cout << "2 " << i << endl;

            for(int k = 0; k < ns; ++k)
            {
                //count spectra
                for(int j = 0; j < 1024; ++j)
                {
                    spectre[2 * j ] = 0.;
                    spectre[2 * j + 1] = data[k][i+250*length-1024 + j];   //last ~4sec of length visual length of signal
                }
                four1(spectre-1, 1024, 1);

                for(int i = 0; i < 85; ++i )      //get the absolute value of FFT
                {
                    dataFFT[k][ i ] = ( spectre[ i * 2 +1] * spectre[ i * 2 +1] + spectre[ i * 2 + 1 +1] * spectre[ i * 2 + 1 +1] )*2*0.004/1024.; //generality
//                    dataFFT[k][ i ] = sqrt(dataFFT[k][ i ]);
                }

                //smooth spectra
                for(int a = 0; a < 3; ++a)
                {
                    help1=dataFFT[k][0];
                    for(int t=1; t < 85-1; ++t)
                    {
                        help2=dataFFT[k][t];
                        dataFFT[k][t]=(help1+help2+dataFFT[k][t+1])/3.;
                        help1=help2;
                    }
                }
            }

//            cout << "3 " << i << endl;

            for(int k = 0; k < ns; ++k)
            {
                for(int j = 0; j < 63; ++j) //generality spLength
                {
                    matrix[k*63+j] = dataFFT [k] [j+20];   //make a vector of appropriate spectra values
                }
            }
            matrix[NetLength]=1.;

//            cout << "4 " << i << endl;


            //count output
            for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
            {
                output[j][l/timeShift] = 0.;
                for(int i = 0; i < NetLength; ++i)
                {
                    output[j][l/timeShift] += weight[j][i]*matrix[i];
                }
                output[j][l/timeShift] += weight[j][NetLength]*matrix[NetLength];
                output[j][l/timeShift]=logistic(output[j][l/timeShift], temperature); // unlinear conformation
            }

//            cout << "5 " << i << endl;


            pic = QPixmap(length*100, 300);
            pic.fill();
            painter->begin(&pic);

            for(int j=i; j < i+250*length; ++j)
            {
                if(j < i+250*length-1024) painter->setPen("lightgray");
                else painter->setPen("black");


                //draw signals
                for(int k = 0; k < ns; ++k)
                {
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height()/(ns+3)*(k+1) - data[k][j], (j-i+1)/(250.*length)*pic.width(), pic.height()/(ns+3)*(k+1) - data[k][j+1]);
                }

                //draw outputs
                if(j>staSlice+250*offsetTime+250*length)
                {
                    int k=j-(staSlice+250*offsetTime+250*length);
                    painter->setPen(QPen(QBrush("blue"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height() - output[0][k/timeShift]*outNorm, (j-i+1)/(250.*length)*pic.width(),  pic.height() - output[0][(k+1)/timeShift]*outNorm);
                    painter->setPen(QPen(QBrush("red"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height() - output[1][k/timeShift]*outNorm, (j-i+1)/(250.*length)*pic.width(),  pic.height() - output[1][(k+1)/timeShift]*outNorm);
                    painter->setPen(QPen(QBrush("green"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), pic.height() - output[2][k/timeShift]*outNorm, (j-i+1)/(250.*length)*pic.width(),  pic.height() - output[2][(k+1)/timeShift]*outNorm);
                }

                //draw markers
                if(data[19][j]==241.)
                {
                    painter->setPen(QPen(QBrush("blue"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), 0, (j-i)/(250.*length)*pic.width(), pic.height());
                }
                if(data[19][j]==247.)
                {
                    painter->setPen(QPen(QBrush("red"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), 0, (j-i)/(250.*length)*pic.width(), pic.height());
                }
                if(data[19][j]==254.)
                {
                    painter->setPen(QPen(QBrush("green"), 2));
                    painter->drawLine((j-i)/(250.*length)*pic.width(), 0, (j-i)/(250.*length)*pic.width(), pic.height());
                }

            }
//            cout << "6 " << i << endl;
            pic.save(dir->absolutePath().append(slash()).append(rightNumber(i, 7)).append(".jpg"), 0, 100);
            painter->end();
        }

        helpString = "cd ";
        helpString.append(dir->absolutePath()).append(" && mencoder \"mf://*.jpg\" -mf type=jpg -o ").append(dir->absolutePath().append(slash()).append("visual.mpg")).append(" -ovc lavc -lavcopts vcodec=msmpeg4v2"); //path generality
        system(helpString.toStdString().c_str());

        dir->cdUp();
        dir->cdUp();
        cout << dir->absolutePath().toStdString() << endl;


        delete []spectre;


        for(int k = 0; k < ns; ++k)
        {
            delete []dataFFT[k];
        }
        delete []dataFFT;


        for(int i = 0; i < NumOfClasses; ++i)
        {
            delete []output[i];
        }
        delete [] output ;


        delete []matrix;
#endif
}

void MainWindow::waveletCount()
{
#if 0
    QString helpString;

    NumOfSlices=-1;

    FILE * file1;
    helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append(ExpName).append(".dat"));

    file1 = fopen(helpString.toStdString().c_str(),"r");
    if(file1 == NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Error"), tr("ExpName.dat file not found"), QMessageBox::Ok);
        return;
    }
    fscanf(file1, "NumOfSlices %d\n", &NumOfSlices);
    fclose(file1);

    if(ui->classBox->isChecked()) drawClassification();  //needs *.dat & weights.wts
    if(ui->weightsBox->isChecked()) drawWeights();       //needs         weights.wts

    if(ui->waveletsBox->isChecked())
    {
        for(int channel = 0; channel < 19; ++channel)
        {
            helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append(ExpName).append(".dat"));
            file1 = fopen(helpString.toStdString().c_str(),"r");
            if(file1==NULL)
            {
                QMessageBox::critical((QWidget*)this, tr("Error"), tr("ExpName.dat file not found"), QMessageBox::Ok);
                break;
            }

            helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("visualisation").append(slash()).append(ExpName).append("_wavelet_").append(QString::number(channel)).append(".jpg"));
            //                cout << helpString.toStdString() << endl;

            wavelet(helpString, file1, ns, channel, 20., 5., 0.98, 32);
            cout << channel << " wavelet drawn" << endl;
        }
    }

    if(ui->fullBox->isChecked())
    {
        QPixmap full(150*NumOfSlices/250 + 600, 800*20 + 50*19);
        QPainter *painter = new QPainter;
        full.fill();
        painter->begin(&full);

        QRectF target;
        QRectF source;
        QPixmap pic;

        //class
        target = QRectF(600, 0, 150*NumOfSlices/250, 800);
        helpString =  QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("visualisation").append(slash()).append("class.jpg"));
        pic = QPixmap(helpString);
        source = QRectF(pic.rect());
        painter->drawPixmap(target, pic, source);

        for(int i = 0; i < 19; ++i)
        {
            //void QPainter::drawPixmap ( const QRectF & target, const QPixmap & pixmap, const QRectF & source )

            //weights
            target = QRectF(0, (i+1)*(800 + 50), 600, 800);
            helpString = QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("visualisation").append(slash()).append(ExpName).append("_weights_").append(QString::number(i)).append(".jpg"));
            pic = QPixmap(helpString);
            source = QRectF(pic.rect());
            painter->drawPixmap(target, pic, source);

            //wavelets
            target = QRectF(600, (i+1)*(800 + 50), 150*NumOfSlices/250, 800);
            helpString = QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("visualisation").append(slash()).append(ExpName).append("_wavelet_").append(QString::number(i)).append(".jpg"));
            pic = QPixmap(helpString);
            source = QRectF(pic.rect());
            painter->drawPixmap(target, pic, source);
        }
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("visualisation").append(slash()).append(ExpName).append("full.jpg"));
        full.save(helpString, 0, 100);

        cout << "full picture drawn" << endl;

        painter->end();
        delete painter;
    }
    if(ui->visualisationBox->isChecked()) visualisation();
#endif
}

void MainWindow::drawWeights()
{
#if 0
    QString helpString;

    //read wts
    spLength = 63;
    int NumOfClasses=3;
    int NetLength=19*spLength;

    double ** weight = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        weight[i] = new double [NetLength+1];
    }

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("weights.wts"));
    FILE * w = fopen(helpString.toStdString().c_str(),"r");
    if(w==NULL)
    {
        cout << "cannot open wts file" << endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Cannot open wts-file"), QMessageBox::Ok);
        return;
    }
    double maxWeight = 0.;
    for(int i = 0; i < NumOfClasses*(NetLength); ++i)
    {
        if(feof(w))
        {
            cout << "wts-file too small" << endl;

            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too small. Nothing happened"), QMessageBox::Ok);
            return;
        }
        fscanf(w, "%lf\n", &weight[i/(NetLength)][i%(NetLength)]);
        maxWeight = max(weight[i/(NetLength)][i%(NetLength)], maxWeight);
    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        if(feof(w))
        {
            cout << "wts-file too small" << endl;

            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too small. Nothing happened"), QMessageBox::Ok);
            return;
        }
        fscanf(w, "%lf\n", &weight[i][NetLength]);
    }
    if(!feof(w))
    {
        cout << "wts-file too big" << endl;
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("Wts-file too long. Nothing happened"), QMessageBox::Ok);
        return;
    }
    fclose(w);



    QPixmap pic(600,800);   //generality 600
    QPainter * painter = new QPainter;
    pic.fill();
    painter->begin(&pic);

    int lineWidth=2;
    double norm = maxWeight/300.;

    for(int i = 0; i < 19; ++i)
    {
        pic.fill();
        for(int k = 0; k < NumOfClasses; ++k)
        {
            if(k == 0) painter->setPen(QPen(QBrush("blue"), lineWidth ));
            if(k == 1) painter->setPen(QPen(QBrush("red"), lineWidth ));
            if(k == 2) painter->setPen(QPen(QBrush("green"), lineWidth ));
            for(int j = 0; j < 63-1; ++j)
            {
                painter->drawLine(pic.width()/2-weight[k][i*63+j]/norm, pic.height()*(1.-j/63.), pic.width()/2-weight[k][i*63+(j+1)]/norm, pic.height()*(1.-(j+1)/63.));  //generality 63
            }
        }
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("visualisation").append(slash()).append(ExpName).append("_weights_").append(QString::number(i)).append(".jpg"));
        pic.save(helpString, 0, 100);

    }
    painter->end();
    delete painter;

    cout << "weights drawn" << endl;

#endif
}

void MainWindow::drawClassification()  //needs *.dat & weights.wts
{
#if 0
    QString helpString;


    FILE * file1;

    spLength = 63;

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append(ExpName).append(".dat"));
    file1 = fopen(helpString.toStdString().c_str(),"r");
    fscanf(file1, "NumOfSlices %d\n", &NumOfSlices);
//    cout << "NumOfSlices=" << NumOfSlices << endl;
//    cout << "ns=" << ns << endl;

    data = new double *[ns];
    for(int i = 0; i < ns; ++i)
    {
        data[i] = new double [NumOfSlices];
    }

    for(int i = 0; i < NumOfSlices; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            fscanf(file1, "%lf", &data[j][i]);
//            cout << data[j][i] << endl;
        }
    }
//    cout << "read" << endl;

    //generality
    int NumOfClasses=3;
    int NetLength=19*spLength;
    int timeShift = 125;
    FILE * wts;

    helpString=QDir::toNativeSeparators(dir->absolutePath().append(slash()).append("weights.wts")); //generality
    wts = fopen(helpString.toStdString().c_str(),"r");
    if(wts==NULL)
    {
        cout << "wts==NULL" << endl;
    }

    double ** weight = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        weight[i] = new double [NetLength+1];
    }
    for(int i = 0; i < NumOfClasses*(NetLength); ++i)
    {
        fscanf(wts, "%lf", &weight[i/(NetLength)][i%(NetLength)]);
//        cout << weight[i/(NetLength)][i%(NetLength)] << endl;

    }
    for(int i = 0; i < NumOfClasses; ++i)
    {
        fscanf(wts, "%lf", &weight[i][NetLength]);
    }
    fclose(wts);

//    cout << "wts read" << endl;

    double * spectre;
    spectre = new double [2*1024]; //generality


    double **dataFFT = new double * [19];
    for(int k = 0; k < 19; ++k)
    {
        dataFFT[k] = new double [85];
    }
    double * matrix = new double [NetLength+1];
    double ** output = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        output[i] = new double [int((NumOfSlices-1024)/timeShift + 10)]; //generality
    }
    double temperature = ui->tempBox->value();
    int l = 0;
    double outNorm = 0.8; //graphical parameter
    double help1, help2; // for smooth
    int lineWidth = 2;


    //for every window shifted with timeShift
    for(int i = 0; i < int(NumOfSlices-1024); i += timeShift) //generality
    {
        //count spectra
        for(int k = 0; k < 19; ++k)
        {
            for(int j = 0; j < 1024; ++j)
            {
                spectre[2 * j ] = 0.;
                spectre[2 * j + 1] = data[k][i+j];
            }
            four1(spectre-1, 1024, 1);
            for(int i = 0; i < 85; ++i )      //get the absolute value of FFT
            {
                dataFFT[k][ i ] = ( spectre[ i * 2 +1] * spectre[ i * 2 +1] + spectre[ i * 2 + 1 +1] * spectre[ i * 2 + 1 +1] )*2*0.004/1024.;
            }
            //smooth spectra
            for(int a = 0; a < 3; ++a) //generality numberOfSmooth
            {
                help1=dataFFT[k][0];
                for(int t=1; t < 85-1; ++t)
                {
                    help2=dataFFT[k][t];
                    dataFFT[k][t]=(help1+help2+dataFFT[k][t+1])/3.;
                    help1=help2;
                }

            }
        }
        for(int k = 0; k < 19; ++k)
        {
            for(int j = 0; j < spLength; ++j)
            {
                matrix[k*spLength+j] = dataFFT [k] [j+20];   //add reduce channels
            }
        }
        matrix[NetLength]=1.;

        //count output
        for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
        {
            output[j][l] = 0.;
            for(int i = 0; i < NetLength; ++i)
            {
                output[j][l] += weight[j][i]*matrix[i];
            }
            output[j][l] += weight[j][NetLength]*matrix[NetLength];
            output[j][l]=logistic(output[j][l], temperature); // unlinear conformation
        }

        ++l; //l=number of windows processed
    }

    --l;             //???


        QPixmap pic(150*NumOfSlices/250, 800);  //150 pix/sec
        QPainter * painter = new QPainter;
        pic.fill();
        painter->begin(&pic);

//        int lineWidth = 2;  //graphical parameter

        //for every window processed
        for(int j = 0; j < l-1; ++j)
        {
            painter->setPen("black");

            //draw output lines
            painter->setPen(QPen(QBrush("blue"), lineWidth ));
//            painter->drawLine((j+0.5)*(pic.width()/(l+1)), pic.height()*(1 - output[0][j]*outNorm), (j+1.5)*(pic.width()/(l+1)),  pic.height()*(1 - output[0][j+1]*outNorm));
//            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(1 - output[0][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(1 - output[0][j+1]*outNorm));
            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(outNorm - output[0][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(outNorm - output[0][j+1]*outNorm));


            painter->setPen(QPen(QBrush("red"), lineWidth ));
//            painter->drawLine((j+0.5)*(pic.width()/(l+1)), pic.height()*(1 - output[1][j]*outNorm), (j+1.5)*(pic.width()/(l+1)),  pic.height()*(1 - output[1][j+1]*outNorm));
            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(outNorm - output[1][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(outNorm - output[1][j+1]*outNorm));


            painter->setPen(QPen(QBrush("green"), lineWidth ));
//            painter->drawLine((j+0.5)*(pic.width()/(l+1)), pic.height()*(1 - output[2][j]*outNorm), (j+1.5)*(pic.width()/(l+1)),  pic.height()*(1 - output[2][j+1]*outNorm));
            painter->drawLine((j)*(pic.width()/(l+1)), pic.height()*(outNorm - output[2][j]*outNorm), (j+1)*(pic.width()/(l+1)),  pic.height()*(outNorm - output[2][j+1]*outNorm));

        }

        painter->setPen("black");
        painter->drawLine(0, pic.height()*(outNorm), pic.width(),  pic.height()*(outNorm));
//        painter->drawLine(0, pic.height()*(outNorm), pic.width(),  pic.height()*(outNorm));

//
        //check markers
        for(int i = 0; i < NumOfSlices; ++i)
        {
            //draw markers
            if(data[ns-1][i]==241.)
            {
                painter->setPen(QPen(QBrush("blue"), lineWidth ));
                painter->drawLine(i*pic.width()/NumOfSlices, 0, i*pic.width()/NumOfSlices, pic.height()); //j+0.5 ???
            }
            if(data[ns-1][i]==247.)
            {
                painter->setPen(QPen(QBrush("red"), lineWidth ));
                painter->drawLine(i*pic.width()/NumOfSlices, 0, i*pic.width()/NumOfSlices, pic.height()); //j+0.5 ???
            }
            if(data[ns-1][i]==254.)
            {
                painter->setPen(QPen(QBrush("green"), lineWidth ));
                painter->drawLine(i*pic.width()/NumOfSlices, 0, i*pic.width()/NumOfSlices, pic.height()); //j+0.5 ???
            }
        }

        painter->setFont(QFont("Helvetica", 32, -1, -1));
        painter->setPen(QPen(QBrush("black"), lineWidth ));
        for(int j = 0; j < int(NumOfSlices/250); ++j)
        {
            painter->drawLine(150*j, pic.height(), 150*j, pic.height()*(1.0-0.10));
            helpString.setNum(j);
            painter->drawText(150*j-5, pic.height()*(1.0-0.12), helpString);
        }

        pic.save(dir->absolutePath().append(slash()).append("visualisation").append(slash()).append("class.jpg"), 0, 100);
        painter->end();
        delete painter;
        for(int i = 0; i < NumOfClasses; ++i)
        {
            delete []output[i];
        }
        delete []output;
        for(int k = 0; k < 19; ++k)
        {
            delete []dataFFT[k];
        }
        delete []dataFFT;
        delete []matrix;
        delete []spectre;
        cout << "classification drawn" << endl;
#endif
}

void MainWindow::makeDatFile()
{
#if 0
    QString helpString;
    readData();
    if(ui->eyesCleanCheckBox->isChecked()) eyesFast();
    if(ui->reduceChannelsCheckBox->isChecked()) reduceChannelsFast();

    double startTime=ui->startTimeBox->value();
    cout << "startTime = " << startTime << endl;
    double finishTime=ui->finishTimeBox->value();
    cout << "finishTime = " << finishTime << endl;

    helpString = QDir::toNativeSeparators(dir->absolutePath().append(slash()).append(ExpName).append(".dat"));

    cout << "ns in dat-file=" << ns << endl;
    FILE * datFile = fopen(QDir::toNativeSeparators(helpString).toStdString().c_str(),"w");
    if(datFile==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("critical"), tr("cannot open datFile"), QMessageBox::Ok);
        return;
    }

    fprintf(datFile, "NumOfSlices %d\n", int((finishTime - startTime)*def::freq));

    for(int i = int(startTime * def::freq); i < int(finishTime * def::freq); ++i) //generality 250
    {
        for(int j = 0; j < ns; ++j)
        {
            fprintf(datFile, "%lf\n", data[j][i]);
        }
    }
    fclose(datFile);
#endif
}



#endif
