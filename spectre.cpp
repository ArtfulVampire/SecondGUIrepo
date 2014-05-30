#include "spectre.h"
#include "ui_spectre.h"

Spectre::Spectre(QDir *dir_, int ns_, QString ExpName_) :
    ui(new Ui::Spectre)
{
    ui->setupUi(this);

    this->setWindowTitle("Spectra Counter");

    norm = 20.;
    Eyes=0;

    ExpName = ExpName_;
    dir = new QDir();
    dirBC = new QDir();

    dir->cd(QDir::toNativeSeparators(dir_->absolutePath()));
    dirBC->cd(QDir::toNativeSeparators(dir_->absolutePath()));
    ns=ns_;

    QButtonGroup * group1, *group2, *group3, *group4;

    group1 = new QButtonGroup;
    group1->addButton(ui->jpgButton);
    group1->addButton(ui->svgButton);
    ui->jpgButton->setChecked(true);

    group2 = new QButtonGroup;
    group2->addButton(ui->spectraRadioButton);
    group2->addButton(ui->brainRateRadioButton);
    group2->addButton(ui->phaseDifferenceRadioButton);
    ui->spectraRadioButton->setChecked(true);

    group3 = new QButtonGroup;
    group3->addButton(ui->amplitudeWaveletButton);
    group3->addButton(ui->phaseWaveletButton);
    ui->amplitudeWaveletButton->setChecked(true);

    group4 = new QButtonGroup;
    group4->addButton(ui->grayRadioButton);
    group4->addButton(ui->colourRadioButton);
    ui->colourRadioButton->setChecked(true);


    ui->amplitudeWaveletButton->setChecked(true);


    paint = new QPainter();

    ui->smoothBox->setValue(5);
    ui->smoothBox->setMaximum(500);

    ui->fftComboBox->addItem("1024");
    ui->fftComboBox->addItem("2048");
    ui->fftComboBox->addItem("4096");
    ui->fftComboBox->addItem("8192");

    ui->fftComboBox->setCurrentIndex(3);

    ui->progressBar->setValue(0);

    ui->integrateLineEdit->setText("5-8; 8-9.5; 9.5-13; 13-20");

    ui->scalingDoubleSpinBox->setValue(1.0);
    ui->scalingDoubleSpinBox->setSingleStep(0.05);



    rangeLimits = new int * [ns];
    for(int i=0; i<ns; ++i)
    {
        rangeLimits[i] = new int [2];
        rangeLimits[i][0] = 0;
        rangeLimits[i][1] = 247;
    }


//    browser1 = new QFileDialog(this, tr("Browser1"), dir->absolutePath(), "");
//    browser2 = new QFileDialog(this, tr("Browser2"), dir->absolutePath(), "");
//    browser1->setFilter(QDir::NoDotAndDotDot);
//    browser2->setFilter(QDir::NoDotAndDotDot);
//    QObject::connect(ui->inputBroswe, SIGNAL(clicked()), browser1, SLOT(show()));
//    QObject::connect(browser1, SIGNAL(directoryEntered(QString)), ui->lineEdit_1, SLOT(setText(QString)));
//    QObject::connect(browser1, SIGNAL(fileSelected(QString)), ui->lineEdit_1, SLOT(setText(QString)));

//    QObject::connect(ui->outputBroswe, SIGNAL(clicked()), browser2, SLOT(show()));
//    QObject::connect(browser2, SIGNAL(directoryEntered(QString)), ui->lineEdit_2, SLOT(setText(QString)));
//    QObject::connect(browser2, SIGNAL(fileSelected(QString)), ui->lineEdit_2, SLOT(setText(QString)));


        QObject::connect(ui->inputBroswe, SIGNAL(clicked()), this, SLOT(inputDirSlot()));
        QObject::connect(ui->outputBroswe, SIGNAL(clicked()), this, SLOT(outputDirSlot()));

    QObject::connect(ui->countButton, SIGNAL(clicked()), this, SLOT(countSpectra()));

    QObject::connect(ui->fftComboBox, SIGNAL(activated(int)), this, SLOT(setFftLength()));
    QObject::connect(ui->fftComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFftLength()));
    QObject::connect(ui->fftComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFftLength()));

    QObject::connect(ui->leftSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLeft()));
    QObject::connect(ui->rightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setRight()));

    QObject::connect(ui->avButton, SIGNAL(clicked()), this, SLOT(compare()));

    QObject::connect(ui->centerButton, SIGNAL(clicked()), this, SLOT(center()));

    QObject::connect(ui->psaButton, SIGNAL(clicked()), this, SLOT(psaSlot()));

    QObject::connect(ui->integrateButton, SIGNAL(clicked()), this, SLOT(integrate()));

    QObject::connect(ui->waveletsPushButton, SIGNAL(clicked()), this, SLOT(drawWavelets()));

    QObject::connect(ui->rangeButton, SIGNAL(clicked()), this, SLOT(specifyRange()));

    ui->specLabel->installEventFilter(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->fftComboBox->setCurrentIndex(2); //0 for 1024   2 for 4096

    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations"));
    ui->lineEdit_1->setText(helpString);
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));   //smooth right after spectra count
    ui->lineEdit_2->setText(helpString);
}

int findChannel(int x, int y, QSize siz)
{
    int a, b, num;
    a = floor( x*16./double(siz.width())/3. );
    b = floor( y*16./double(siz.height())/3. );
    num=0;
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
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(fmod(16*double(mouseEvent->y())/ui->specLabel->height(), 3.) < 0.5)
            {
                return false;
            }

            if(fmod(16*double(mouseEvent->x())/ui->specLabel->width(), 3.) < 0.5)
            {
                if(mouseEvent->button()==Qt::LeftButton)
                {
                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size());
                    rangeLimits[chanNum][0] = 0;
                }
                if(mouseEvent->button()==Qt::RightButton)
                {

                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size()) - 1;
                    rangeLimits[chanNum][1] = spLength-1;
                }
                return true;
            }

            paint->end();
            pic.load(rangePicPath, 0, Qt::ColorOnly);
            paint->begin(&pic);


            chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size());
//            cout<<chanNum<<endl;

            if(mouseEvent->button()==Qt::LeftButton)
            {

                rangeLimits[chanNum][0] = floor((mouseEvent->x()*paint->device()->width()/ui->specLabel->width() - coords::x[chanNum]*paint->device()->width())/(coords::scale*paint->device()->width())*spLength);

            }
            if(mouseEvent->button()==Qt::RightButton)
            {
                rangeLimits[chanNum][1] = ceil((mouseEvent->x()*paint->device()->width()/ui->specLabel->width() - coords::x[chanNum]*paint->device()->width())/(coords::scale*paint->device()->width())*spLength);
            }
//            cout<<paint->device()->width()<<endl;
//            cout<<pic.width()<<endl;
            for(int i=0; i<ns; ++i)
            {
                paint->setPen(QPen(QBrush("blue"), 2));
                paint->drawLine(QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][0]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height()), QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][0]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height() - coords::scale*paint->device()->height()));

                paint->setPen(QPen(QBrush("red"), 2));
                paint->drawLine(QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][1]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height()), QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][1]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height() - coords::scale*paint->device()->height()));
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
    helpString = QFileDialog::getExistingDirectory(this, tr("Choose input dir"), dirBC->absolutePath());
    if(!helpString.isEmpty())
    {
        ui->lineEdit_1->setText(helpString);
    }
}

void Spectre::outputDirSlot()
{
    helpString = QFileDialog::getExistingDirectory(this, tr("Choose input dir"), dirBC->absolutePath());
    if(!helpString.isEmpty())
    {
        ui->lineEdit_2->setText(helpString);
    }
}

void Spectre::integrate()
{
    lst = ui->integrateLineEdit->text().split(QRegExp("[; ]"), QString::SkipEmptyParts);
    int numOfInt = lst.length();
    double tmp;
    int *begins = new int[lst.length()];
    int *ends = new int[lst.length()];
    for(int i=0; i<lst.length(); ++i)
    {
        helpString = lst.at(i);
        nameFilters.clear();
        nameFilters = helpString.split('-', QString::SkipEmptyParts);
        begins[i] = max(floor(nameFilters.at(0).toDouble() * fftLength / 250.) - left + 1, 0.);  //generality 250
        ends[i] = min(ceil(nameFilters.at(1).toDouble() * fftLength / 250.) - left + 1, right - left + 1.);  //generality 250
    }

    FILE * file;
    double **dataInt = new double * [ns];
    for(int i=0; i<ns; ++i)
    {
        dataInt[i] = new double [ns*spLength];
    }
    dir->cd(ui->lineEdit_1->text());
    lst.clear();
    lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
    for(int i=0; i<lst.length(); ++i)
    {

        dir->cd(ui->lineEdit_1->text());
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i));
        file = fopen(helpString.toStdString().c_str(), "r");
        for(int j=0; j<ns; ++j)
        {
            for(int k=0; k<spLength; ++k)
            {
                fscanf(file, "%lf", &dataInt[j][k]);
            }
        }
        fclose(file);


        dir->cd(ui->lineEdit_2->text());
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(i));
        cout<<helpString.toStdString()<<endl;
        file = fopen(helpString.toStdString().c_str(), "w");
        for(int h=0; h<ns; ++h)
        {
            for(int j=0; j<numOfInt; ++j)
            {
                tmp=0.;
                for(int k=begins[j]; k<ends[j]; ++k)
                {
                    tmp+=dataInt[h][k];
                }
                fprintf(file, "%lf\n", tmp);     // tmp/double(ends[j]-begins[j]+1)
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
    for(int i=0; i<ns; ++i)
    {
        delete []dataInt[i];
    }
    delete []dataInt;

}

void Spectre::specifyRange()
{
//    RangeWidget * specWidget = new RangeWidget(NULL, ns, left, right, spStep, spLength, dirBC);
//    specWidget->show();
}
void Spectre::psaSlot()
{
    dir->cd(dirBC->absolutePath());
    //read 2 psa and draw in red and blue
    int * spL;
    int count = 2;
    double ***sp;

    FILE **f = new FILE * [3];
    f[0]=fopen(ui->lineEdit_1->text().toStdString().c_str(), "r");
    f[1]=fopen(ui->lineEdit_2->text().toStdString().c_str(), "r");
    if(f[0]==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Critical"), tr("cannot open the first file"), QMessageBox::Ok);
        return;
    }
    if(f[1]==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Critical"), tr("cannot open the second file"), QMessageBox::Ok);
        return;
    }

    if(ui->fftComboBox->currentIndex()!=0)
    {
        helpString = dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append("_254.psa");
    }
    else
    {
        helpString = dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append("_254_wnd.psa");
    }
    f[2] = fopen(helpString.toStdString().c_str(), "r");

    if(f[2] != NULL)
    {
        count = 3;
    }
    cout << "count = " << count << endl;
    spL = new int [count];
    sp = new double ** [count];

    for(int i=0; i<count; ++i)
    {
        fscanf(f[i], "spLength %d", &spL[i]);
    }


    for(int k = 0; k < count; ++k)
    {
        sp[k] = new double* [ns];
    }

//    cout<<"1"<<endl;
    for(int k = 0; k < count; ++k)
    {
        for(int i=0; i<ns; ++i)
        {
            sp[k][i] = new double [spL[k]];
        }
    }
//    cout<<"2"<<endl;

    for(int k = 0; k < count; ++k)
    {
        for(int i = 0; i < ns; ++i)
        {
            for(int j = 0; j < spL[k]; ++j)
            {
                fscanf(f[k], "%lf", &sp[k][i][j]);
            }
        }
    }
//    cout<<"3"<<endl;



    QSvgGenerator svgGen;
    if(ui->svgButton->isChecked())
    {
        svgGen.setSize(QSize(800, 800));
        svgGen.setViewBox(QRect(QPoint(0,0), svgGen.size()));
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ui->lineEdit_m2->text()).append(".svg");
        svgGen.setFileName(helpString);
        paint->begin(&svgGen);
        paint->setBrush(QBrush("white"));
        paint->drawRect(QRect(QPoint(0,0), svgGen.size()));
    }
    else if(ui->jpgButton->isChecked())
    {

        pic=QPixmap(1600,1600);
        pic.fill();
        paint->begin(&pic);
    }
    //finding maximum magnitude
    norm = 0.;
    for(int j=0; j<count; ++j)
    {
        for(int c2=0; c2<ns; ++c2)
        {
            for(int k=0; k<int(coords::scale * paint->device()->width()); ++k)
            {
                norm = max(norm, sp[j][c2][int(k*spL[j]/(coords::scale * paint->device()->width()))]);  //doesn't work
            }
        }
    }
    cout<<"max magnitude = "<<norm<<endl;

//    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
//    fprintf(res, "\n%.3lf\n", norm);
//    fclose(res);

    norm = (coords::scale * paint->device()->height())/norm ; //250 - pixels per graph, generality
//    cout<<"norm = "<<norm<<endl;
    norm *= ui->scalingDoubleSpinBox->value();


//    cout<<"prep spectra draw"<<endl;
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        //draw spectra
        for(int k=0; k<int(coords::scale * paint->device()->width())-1; ++k)
        {
            for(int j=0; j<count; ++j)
            {
                if(ui->colourRadioButton->isChecked())
                {
                    if(j==0) paint->setPen(QPen(QBrush("blue"), 2));
                    if(j==1) paint->setPen(QPen(QBrush("red"), 2));
                    if(j==2) paint->setPen(QPen(QBrush("green"), 2));
                }
                if(ui->grayRadioButton->isChecked())
                {
                    if(j==0) paint->setPen(QPen(QBrush(QColor(0,0,0,255)), 2));
                    if(j==1) paint->setPen(QPen(QBrush(QColor(80,80,80,255)), 2));
                    if(j==2) paint->setPen(QPen(QBrush(QColor(160,160,160,255)), 2));
                }
                paint->drawLine(QPointF(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - sp[j][c2][int(k*spL[j]/(coords::scale * paint->device()->width()))]*norm), QPointF(paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - sp[j][c2][int((k+1)*spL[j]/(coords::scale * paint->device()->width()))]*norm));
            }
        }

        //draw axes
        paint->setPen("black");
        paint->drawLine(QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2] - coords::scale * paint->device()->height())); //250 - length of axes generality
        paint->drawLine(QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2] + coords::scale * paint->device()->width(), paint->device()->height() * coords::y[c2])); //250 - length of axes generality

        //draw Herzes
        paint->setFont(QFont("Helvitica", int(8*(paint->device()->height()/1600.))));
        for(int k=0; k<int(coords::scale * paint->device()->width()); ++k) //for every Hz generality
        {
            if( (left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep - floor((left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep) < spLength/(coords::scale * paint->device()->width())*spStep/2. || ceil((left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep) - (left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep < spLength/(coords::scale * paint->device()->width())*spStep/2.)  //why spLength - generality 250 - length of axes
            {
                paint->drawLine(QPointF(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2] + 5 * (paint->device()->height()/1600.)));

                helpInt = int((left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep + 0.5);
                helpString.setNum(helpInt);
                if(helpInt<10)
                {
                    paint->drawText(QPointF(paint->device()->width() * coords::x[c2] + k - 3 * (paint->device()->width()/1600.), paint->device()->height() * coords::y[c2] + 15 * (paint->device()->height()/1600.)), helpString);  //-3 getFont->size
                }
                else
                {
                    paint->drawText(QPointF(paint->device()->width() * coords::x[c2] + k - 5 * (paint->device()->width()/1600.), paint->device()->height() * coords::y[c2] + 15 * (paint->device()->height()/1600.)), helpString);  //-5 getFont->size
                }
            }
        }

    }
//    cout<<"spectra drawn"<<endl;

    //write channels labels
    paint->setFont(QFont("Helvetica", int(24*paint->device()->height()/1600.), -1, false));
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText(QPointF((paint->device()->width() * coords::x[c2] - 20 * (paint->device()->width()/1600.)), (paint->device()->height() * coords::y[c2] - (coords::scale * paint->device()->height()) - 2)), QString(coords::lbl[c2]));
    }

    //draw coords::scale
    paint->drawLine(QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1]), QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())));  //250 - graph height generality

    //returning norm = max magnitude
    norm /= ui->scalingDoubleSpinBox->value();
    norm = (coords::scale * paint->device()->height()) / norm;
    norm /= ui->scalingDoubleSpinBox->value();  //scaling generality
    norm = int(norm*10.)/10.;

    helpString.setNum(norm);
    helpString.append(tr(" mcV^2/Hz"));
    paint->drawText(QPointF(paint->device()->width() * coords::x[6]+5., paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())/2.), helpString);




    if(ui->jpgButton->isChecked())
    {
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ui->lineEdit_m2->text()).append(".jpg");
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ui->lineEdit_m2->text()).append(".png");
        pic.save(helpString, 0, 100);
        rangePicPath = helpString;
        ui->specLabel->setPixmap(pic.scaled(ui->specLabel->size()));
    }
    paint->end();




    for(int k = 0; k < count; ++k)
    {
        for(int i=0; i<ns; ++i)
        {
            delete []sp[k][i];
        }
        delete []sp[k];
    }
    delete []sp;
    for(int k = 0; k < count; ++k)
    {
        fclose(f[k]);
    }

    ui->fftComboBox->setCurrentIndex(ui->fftComboBox->currentIndex()+1);
    ui->fftComboBox->setCurrentIndex(ui->fftComboBox->currentIndex()-1);
    cout<<"average spectra drawn"<<endl<<endl;

}

void Spectre::compare()
{
    QStringList list; //0 - Spatial, 1 - Verbal, 2 - Gaps


    dir->cd(ui->lineEdit_1->text());   //input dir = /SpectraSmooth or
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_m1->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst.clear();
    lst=dir->entryList(nameFilters, QDir::Files, QDir::Size);

    FILE * file;
    int NumOfPatterns=0;

    double ** dataFFT = new double * [ns];
    for(int i=0; i<ns; ++i)
    {
        dataFFT[i] = new double [spLength];
    }

    double ** spectre = new double * [ns];
    for(int i=0; i<ns; ++i)
    {
        spectre[i] = new double [spLength];
    }

    for(int i=0; i<ns; ++i)                                //put zeros into dataFFT
    {
        for(int k=0; k<spLength; ++k)
        {
            dataFFT[i][k]=0.;
        }
    }

    for(int j=0; j<lst.length(); ++j)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst.at(j));
//        cout<<helpString.toStdString()<<endl;
        file=fopen(helpString.toStdString().c_str(), "r");

        if(file!=NULL)
        {
            for(int i=0; i<ns; ++i)                                ////////save BY CHANNELS!!!!!!!!!!!
            {
                for(int k=0; k<spLength; ++k)
                {
                    fscanf(file, "%lf\n", &spectre[i][k]);
                    dataFFT[i][k]+=spectre[i][k];             //sum all spectra
                }
                fscanf(file, "\n");
            }
            ++NumOfPatterns;
            fclose(file);
        }
    }

//    cout<<NumOfPatterns<<endl;
    dir->cd(ui->lineEdit_2->text());  //output dir /Help
    helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(ui->lineEdit_m2->text()).append(".psa"); ////////////////////////////////////LAWL?????
//    cout<<helpString.toStdString()<<endl;


    ////write AVspectra into .psa file
    file=fopen(helpString.toStdString().c_str(), "w");
    if(file==NULL)
    {
        cout<<"cannot open file"<<endl;
        return;
    }
    fprintf(file, "spLength %d\n", spLength);
    for(int i=0; i<ns; ++i)
    {
        for(int k=0; k<spLength; ++k)
        {
            fprintf(file, "%lf\n", dataFFT[i][k]/double(NumOfPatterns));
        }
        fprintf(file, "\n");
    }
    fclose(file);
    pic = QPixmap(1600, 1600);
    pic.fill();
    paint->begin(&pic);
    double ext = spLength/250.;
    for(int c2=0; c2<ns; ++c2)
    {
        for(int k=0; k<250-1; ++k)
        {
            //spectre itslef
            paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - dataFFT[c2][int(k*ext)]/double(NumOfPatterns)*norm, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - dataFFT[c2][int((k+1)*ext)]/double(NumOfPatterns)*norm);
        }

        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);


        paint->setFont(QFont("Helvitica", 8));
        for(int k=0; k<250-1; ++k) //for every Hz generality
        {
//            paint->drawLine(paint->device()->width() * coords::x[c2]+250*k/15, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250*k/15, paint->device()->height() * coords::y[c2]+10);
            if( (left + k*(spLength)/250.)*spStep - floor((left + k*(spLength)/250.)*spStep) < spLength/250.*spStep/2. || ceil((left + k*(spLength)/250.)*spStep) - (left + k*(spLength)/250.)*spStep < spLength/250.*spStep/2.)
            {
                paint->drawLine(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]+5);

                helpInt = int((left + k*(spLength)/250.)*spStep + 0.5);
                helpString.setNum(helpInt);
                if(helpInt<10)
                {
                    paint->drawText(paint->device()->width() * coords::x[c2] + k-3, paint->device()->height() * coords::y[c2]+15, helpString);
                }
                else
                {
                    paint->drawText(paint->device()->width() * coords::x[c2] + k-5, paint->device()->height() * coords::y[c2]+15, helpString);
                }
            }
        }

    }

    paint->setFont(QFont("Helvetica", 24, -1, false));
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText((paint->device()->width() * coords::x[c2]-20), (paint->device()->height() * coords::y[c2]-252), QString(coords::lbl[c2]));
    }

    helpString = QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(ui->lineEdit_m2->text()).append(".jpg");

    pic.save(helpString, 0, 100);

    paint->end();

    for(int i=0; i<ns; ++i)
    {
        delete []dataFFT[i];
        delete []spectre[i];
    }
    delete []dataFFT;
    delete []spectre;

    if(ui->fftComboBox->currentIndex()!=0)
    {
        if(ui->lineEdit_m1->text().contains("241"))
        {
            ui->lineEdit_m1->setText("_247");
            helpString = ExpName;
            helpString.append("_247");
            ui->lineEdit_m2->setText(helpString);
        }

        else if(ui->lineEdit_m1->text().contains("247"))
        {
            ui->lineEdit_m1->setText("_254");
            helpString = ExpName;
            helpString.append("_254");
            ui->lineEdit_m2->setText(helpString);
        }

        else if(ui->lineEdit_m1->text().contains("254"))
        {
//            helpString = ui->lineEdit_1->text();
//            helpString.resize(helpString.lastIndexOf(QDir::separator())+1);
//            cout<<helpString.toStdString()<<endl;
//            helpString.append("Help").append(QDir::separator()).append(ExpName).append("_241.psa");
            helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append("_241.psa");
            ui->lineEdit_1->setText(QDir::toNativeSeparators(helpString));


//            helpString = ui->lineEdit_2->text();
//            helpString.resize(helpString.lastIndexOf(QDir::separator())+1);
//            cout<<helpString.toStdString()<<endl;
//            helpString.append("Help").append(QDir::separator()).append(ExpName).append("_247.psa");
            helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append("_247.psa");
            ui->lineEdit_2->setText(QDir::toNativeSeparators(helpString));


            ui->lineEdit_m1->clear();
            helpString = ExpName;
            helpString.append("_all");
            ui->lineEdit_m2->setText(helpString);
        }
    }
    else
    {
        if(ui->lineEdit_m1->text().contains("241"))
        {
            ui->lineEdit_m1->setText("_247");
            helpString = ExpName;
            helpString.append("_247_wnd");
            ui->lineEdit_m2->setText(helpString);
        }

        else if(ui->lineEdit_m1->text().contains("247"))
        {
            ui->lineEdit_m1->setText("_254");
            helpString = ExpName;
            helpString.append("_254_wnd");
            ui->lineEdit_m2->setText(helpString);
        }

        else if(ui->lineEdit_m1->text().contains("254"))
        {
//            helpString = ui->lineEdit_1->text();
//            helpString.resize(helpString.lastIndexOf(QDir::separator()));
//            helpString.resize(helpString.lastIndexOf(QDir::separator())+1);
//            cout<<helpString.toStdString()<<endl;
//            helpString.append("Help").append(QDir::separator()).append(ExpName).append("_241_wnd.psa");
            helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append("_241_wnd.psa");
            ui->lineEdit_1->setText(QDir::toNativeSeparators(helpString));


//            helpString = ui->lineEdit_2->text();
//            helpString.resize(helpString.lastIndexOf(QDir::separator())+1);
//            cout<<helpString.toStdString()<<endl;
//            helpString.append("Help").append(QDir::separator()).append(ExpName).append("_247_wnd.psa");
            helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append("_247_wnd.psa");
            ui->lineEdit_2->setText(QDir::toNativeSeparators(helpString));


            ui->lineEdit_m1->clear();
            helpString = ExpName;
            helpString.append("_all_wnd");
            ui->lineEdit_m2->setText(helpString);
        }
    }

}

void Spectre::setFftLength()
{
    fftLength = ui->fftComboBox->currentText().toInt();

    ui->leftSpinBox->setMinimum(0);
    ui->leftSpinBox->setMaximum(1000);

    ui->rightSpinBox->setMinimum(0);
    ui->rightSpinBox->setMaximum(1000);

    if(ui->fftComboBox->currentIndex()==0)
    {
        left = 20; right=82; ui->smoothBox->setValue(3);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("windows").append(QDir::separator()).append("fromreal"));
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("windows")); //for windows
        ui->lineEdit_1->setText(helpString);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows"));
        ui->lineEdit_2->setText(helpString);
    }
    if(ui->fftComboBox->currentIndex()==1)
    {
        left = 41; right=164;

        ui->smoothBox->setValue(4);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations"));
        ui->lineEdit_1->setText(helpString);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
        ui->lineEdit_2->setText(helpString);
    }
    if(ui->fftComboBox->currentIndex()==2)
    {
        left = 82; right=328;

        ui->smoothBox->setValue(15);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations"));
        ui->lineEdit_1->setText(helpString);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
        ui->lineEdit_2->setText(helpString);
    }
    if(ui->fftComboBox->currentIndex()==3)
    {
        left = 164; right=655;

        ui->smoothBox->setValue(6);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Realisations"));
        ui->lineEdit_1->setText(helpString);
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SpectraSmooth"));
        ui->lineEdit_2->setText(helpString);
    }
    ui->leftSpinBox->setValue(left);
    ui->rightSpinBox->setValue(right);

    spLength=right-left+1;
    for(int i=0; i<ns; ++i)
    {
        rangeLimits[i][1] = spLength;
    }


    spStep = 250./double(fftLength); //generality 250 = frequency
    emit spValues(left, right, spStep);
}

void Spectre::center()
{
    helpString = QFileDialog::getExistingDirectory(this, tr("Choose dir"), dir->absolutePath());
    if(helpString=="") return;
    dir->setPath(helpString);
    cout<<dir->absolutePath().toStdString()<<endl;
    cout<<ns<<" "<<spLength<<endl;

    lst = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
    FILE * fil;
    double helpDouble;
    double ** averages = new double * [ns];
    for(int j = 0; j < ns; ++j)
    {
        averages[j] = new double [spLength];
        for(int k = 0; k < spLength; ++k)
        {
            averages[j][k] = 0.;
        }
    }

    double ** tempData = new double * [ns];
    for(int j = 0; j < ns; ++j)
    {
        tempData[j] = new double [spLength];
    }

    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(lst[i]));
        fil = fopen(helpString.toStdString().c_str(), "r");

        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < spLength; ++k)
            {
                fscanf(fil, "%lf\n", &helpDouble);
                averages[j][k] += helpDouble/lst.length();
            }
        }
        fclose(fil);
    }
    for(int j = 0; j < ns; ++j)
    {
        for(int k = 0; k < spLength; ++k)
        {
            cout<<averages[j][k]<<"\t";
        }
        cout<<endl;
    }

    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(lst[i]));
        fil = fopen(helpString.toStdString().c_str(), "r");

        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < spLength; ++k)
            {
                fscanf(fil, "%lf\n", &tempData[j][k]);
            }
        }
        fclose(fil);

        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(lst[i]));
        fil = fopen(helpString.toStdString().c_str(), "w");

        for(int j = 0; j < ns; ++j)
        {
            for(int k = 0; k < spLength; ++k)
            {
                helpDouble = tempData[j][k] - averages[j][k];
                helpDouble *= 10.;
                fprintf(fil, "%lf\n", helpDouble);
            }
            fprintf(fil, "\n");
        }
        fclose(fil);
    }


    for(int j = 0; j < ns; ++j)
    {
        delete []averages[j];
        delete []tempData[j];
    }
    delete []averages;
    delete []tempData;

}

Spectre::~Spectre()
{
    delete ui;
    delete dir;
    delete dirBC;
//    delete browser1;
//    delete browser2;
}

void Spectre::setSmooth(int a)
{
    this->ui->smoothBox->setValue(a);
}

void Spectre::setRight()
{    
//    ui->leftSpinBox->setMaximum(ui->rightSpinBox->value());
//    ui->rightSpinBox->setMinimum(ui->leftSpinBox->value());

    right = ui->rightSpinBox->value();
    helpString.setNum(right*250./fftLength); //generality
    ui->rightHzEdit->setText(helpString);
    spLength=right-left+1;
    emit spValues(left, right, spStep);
}

void Spectre::setLeft()
{
//    ui->leftSpinBox->setMaximum(ui->rightSpinBox->value());
//    ui->rightSpinBox->setMinimum(ui->leftSpinBox->value());

    left = ui->leftSpinBox->value();
    helpString.setNum(left*250./fftLength);//generality
    ui->leftHzEdit->setText(helpString);
    spLength=right-left+1;
    emit spValues(left, right, spStep);
}

void Spectre::countSpectra()
{
    QTime myTime;
    myTime.start();
    emit spValues(left, right, spStep);

    dir->cd(ui->lineEdit_1->text());
    nameFilters.clear();
    lst=dir->entryList(QDir::Files, QDir::Name);
    dir->cd(dirBC->absolutePath());

    double ** dataFFT = new double * [ns];
    for(int i=0; i<ns; ++i)
    {
        dataFFT[i] = new double [fftLength/2];
    }

    double *** dataPhase = new double ** [ns];
    for(int i = 0; i < ns; ++i)
    {
        dataPhase[i] = new double * [ns];
        for(int j = 0; j < ns; ++j)
        {
            dataPhase[i][j] = new double [fftLength/2];
        }
    }

    double sum1 = 0.;
    double sum2 = 0.;
    FILE * file;

    for(int a = 0; a < lst.length(); ++a)
    {
        if(lst[a].contains("_num") || lst[a].contains("_300")) continue;


        if(!ui->phaseDifferenceRadioButton->isChecked())
        {
            if(!readFile(a, dataFFT)) continue;

            dir->cd(ui->lineEdit_2->text());  //cd to output dir
            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst[a]);  /////separator
            file = fopen(helpString.toStdString().c_str(), "w");
            if(file == NULL)
            {
                cout<<"file to write spectra == NULL"<<endl;
                return;
            }


            if(ui->spectraRadioButton->isChecked())
            {
                // write spectra
                for(int i=0; i<ns; ++i)                               ///save BY CHANNELS!!!  except markers
                {
                    for(int k=left; k<right+1; ++k)
                    {
                        if((k-left)>=rangeLimits[i][0] && (k-left)<=rangeLimits[i][1])
                            fprintf(file, "%lf\n", dataFFT[i][k]);
                        else
                            fprintf(file, "0.000\n");
                    }
                    fprintf(file, "\n");
                }
            }
            if(ui->brainRateRadioButton->isChecked())
            {
                // write brainRate
                for(int i = 0; i < ns; ++i)
                {
                    sum1 = 0.;
                    sum2 = 0.;
                    for(int j = left; j < right+1; ++j)
                    {
                        sum1 += dataFFT[i][j];
                        sum2 += dataFFT[i][j] * (j * 250. / fftLength);
                    }
                    sum2 /= sum1;
                    //                cout<<sum2<<endl;
                    fprintf(file, "%lf\n", sum2);
                }
            }
        }
        else
        {
            if(!readFilePhase(a, dataPhase))
            {
                cout<<"bad file "<<lst[a].toStdString()<<endl;
                continue;
            }

            dir->cd(ui->lineEdit_2->text());  //cd to output dir
            helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(lst[a]);  /////separator
            file = fopen(helpString.toStdString().c_str(), "w");
            if(file == NULL)
            {
                cout<<"file to write spectra == NULL"<<endl;
                return;
            }

            // write spectra
            for(int i = 0; i < ns; ++i)                               ///save BY CHANNELS!!!  except markers
            {
                for(int j = i+1; j < ns; ++j)
                {
                    for(int k=left; k<right+1; ++k)
                    {
                        if((k-left)>=rangeLimits[i][0] && (k-left)<=rangeLimits[i][1])
                        {
                            fprintf(file, "%lf\n", dataPhase[i][j][k]);
                        }
                        else
                            fprintf(file, "0.000\n");
                    }
                    fprintf(file, "\n");
                }
            }
        }
        fclose(file);



        if(100*(a+1)/lst.length() > ui->progressBar->value())
        {
            ui->progressBar->setValue(100*(a+1)/lst.length());
        }
    }
    ui->progressBar->setValue(0);

    dir->cd(dirBC->absolutePath());


    for(int i=0; i<ns; ++i)
    {
        delete []dataFFT[i];
    }
    delete []dataFFT;

    for(int i = 0; i < ns; ++i)
    {
        for(int j = 0; j < ns; ++j)
        {
            delete []dataPhase[i][j];
        }

        delete []dataPhase[i];
    }
    delete []dataPhase;


    //generality
    //part from smooth()
    if(ui->fftComboBox->currentIndex()!=0) //if not windows
    {
//        helpString = ui->lineEdit_1->text();  // /media/Files/Data/ExpName/Realisations
//        helpString.resize(helpString.lastIndexOf(QDir::separator())+1); // /media/Files/Data/ExpName/
//        helpString.append("SpectraSmooth"); // /media/Files/Data/ExpName/SpectraSmooth
        helpString = dirBC->absolutePath().append(QDir::separator()).append("SpectraSmooth");
        ui->lineEdit_1->setText(QDir::toNativeSeparators(helpString));


//        helpString = ui->lineEdit_2->text(); // /media/Files/Data/ExpName/SpectraSmooth
//        helpString.resize(helpString.lastIndexOf(QDir::separator())+1); // /media/Files/Data/ExpName/
//        helpString.append("Help"); // /media/Files/Data/ExpName/SpectraSmooth/Help
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help");
        ui->lineEdit_2->setText(QDir::toNativeSeparators(helpString));

        ui->lineEdit_m1->setText("_241");
        helpString = ExpName;
        helpString.append("_241");
        ui->lineEdit_m2->setText(helpString);
    }
    else
    {
//        helpString = ui->lineEdit_1->text(); // /media/Files/Data/ExpName/windows/fromreal
//        helpString.resize(helpString.lastIndexOf(QDir::separator())); // /media/Files/Data/ExpName/windows
//        helpString.resize(helpString.lastIndexOf(QDir::separator())+1); // /media/Files/Data/ExpName/
//        helpString.append("SpectraSmooth"); // /media/Files/Data/ExpName/SpectraSmooth
//        helpString.append(QDir::separator()).append("windows"); // /media/Files/Data/ExpName/SpectraSmooth/windows
        helpString = dirBC->absolutePath().append(QDir::separator()).append("SpectraSmooth").append(QDir::separator()).append("windows");
        ui->lineEdit_1->setText(QDir::toNativeSeparators(helpString));

//        helpString = ui->lineEdit_2->text(); // /media/Files/Data/ExpName/SpectraSmooth/windows
//        helpString.resize(helpString.lastIndexOf(QDir::separator())); // /media/Files/Data/ExpName/SpectraSmooth
//        helpString.resize(helpString.lastIndexOf(QDir::separator())+1); // /media/Files/Data/ExpName/
//        helpString.append("Help"); // /media/Files/Data/ExpName/SpectraSmooth/Help
        helpString = dirBC->absolutePath().append(QDir::separator()).append("Help");
        ui->lineEdit_2->setText(QDir::toNativeSeparators(helpString));


        ui->lineEdit_m1->setText("_241");
        helpString = ExpName;
        helpString.append("_241_wnd");
        ui->lineEdit_m2->setText(helpString);
    }

    cout << "time elapsed " << myTime.elapsed()/1000. << " sec" << endl;

}

int Spectre::readFile(int &num, double **dataFFT)  /////////EDIT
{

//    cout<<QDir::toNativeSeparators(dir->absolutePath()).toStdString()<<endl;



    dir->cd(ui->lineEdit_1->text());
    FILE * file;

    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(lst[num]));

    double ** data2;
    readDataFile(helpString, &data2, ns, &NumOfSlices, fftLength);

    //correct Eyes number
    Eyes = 0;
    NumOfSlices = fftLength;
    int h = 0;
    for(int i = 0; i < fftLength; ++i)
    {
        h = 0;
        for(int j = 0; j < ns; ++j)
        {
            if(data2[j][i] == 0.) ++h;
        }
        if(h == ns) Eyes += 1;
    }

    if((NumOfSlices-Eyes) < 500) // 0.2*4096/250 = 3.1 sec
    {
        for(int i=0; i<ns; ++i)
        {
            delete []data2[i];
        }
        delete []data2;
        return 0;
    }

    calcSpectre(data2, dataFFT, ns, fftLength, Eyes, ui->smoothBox->value());


    for(int i=0; i<ns; ++i)
    {
        delete []data2[i];
    }
    delete []data2;

    dir->cd(dirBC->absolutePath());
    return 1;
}


int Spectre::readFilePhase(int &num, double ***dataPhase)
{
        int h = 0;

        dir->cd(ui->lineEdit_1->text());
//        FILE * file;

        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(lst[num]));
        file1=fopen(helpString.toStdString().c_str(), "r");
        if(file1==NULL)
        {
            cout<<"file==NULL"<< endl;
            return 0;
        }



        fscanf(file1, "%*s %d\n", &NumOfSlices);
        fscanf(file1, "%*s %d \n", &Eyes);



        double ** data2 = new double* [ns];
        for(int i=0; i<ns; ++i)
        {
            data2[i] = new double [fftLength];
        }

        double uh;
        if(NumOfSlices>fftLength)   //too long - take the end of realisation
        {
            for(int i=fftLength; i<NumOfSlices; ++i)
            {

                for(int k=0; k<ns; ++k)
                {
                    fscanf(file1, "%lf", &uh);
                }
            }
            for(int i=0; i<fftLength; ++i)         //saved BY SLICES!!
            {
                for(int k=0; k<ns; ++k)
                {
                    fscanf(file1, "%lf\n", &data2[k][i]);
                }
            }
        }
        else
        {
            for(int i=0; i<NumOfSlices; ++i)         //saved BY SLICES!!
            {
                for(int k=0; k<ns; ++k)
                {
                    fscanf(file1, "%lf\n", &data2[k][i]);
                }
            }
            //fill the rest with zeros
            for(int i=NumOfSlices; i<fftLength; ++i)
            {
                for(int k=0; k<ns; ++k)
                {
                    data2[k][i]=0.;
                }
            }


        }
        fclose(file1);

        //correct Eyes number
        Eyes=0;
        NumOfSlices = fftLength;
        for(int i=0; i<NumOfSlices; ++i)
        {
            h=0;
            for(int j=0; j<ns; ++j)
            {
                if(data2[j][i]==0.) ++h;
            }
            if(h==ns) Eyes+=1;
        }

//        if((NumOfSlices-Eyes)/double(NumOfSlices)<0.1) // 0.2*4096/250 = 3.1 sec
        if((NumOfSlices-Eyes) < 500) // 0.2*4096/250 = 3.1 sec
        {
            cout<<"Too short real signal "<<helpString.toStdString()<<endl;//<<NumOfSlices<<"  "<<Eyes<<endl<<endl;

            for(int i=0; i<ns; ++i)
            {
                delete []data2[i];
            }
            delete []data2;

            return 0;
        }
    //    cout<<"!!"<<endl;


        double norm1=fftLength/double(fftLength-Eyes);              //norm with eyes


        double ** spectre = new double *[2];
        spectre[0] = new double [fftLength*2];
        spectre[1] = new double [fftLength*2];

        double * help = new double [2];
//        int leftSmoothLimit, rightSmoothLimit;

        for(int c1 = 0; c1 < ns; ++c1)
        {
            for(int i = 0; i < fftLength; ++i)            //make appropriate array
            {
                spectre[0][ i * 2 ] = (double)(data2[c1][ i ]*sqrt(norm1));
                spectre[0][ i * 2 + 1 ] = 0.;
            }
            four1(spectre[0] - 1, fftLength, 1);        //Fourier transform

            for(int c2 = c1+1; c2 < ns; ++c2)
            {


                for(int i = 0; i < fftLength; ++i)            //make appropriate array
                {
                    spectre[1][ i * 2 ] = (double)(data2[c2][ i ]*sqrt(norm1));
                    spectre[1][ i * 2 + 1 ] = 0.;
                }
                four1(spectre[1] - 1, fftLength, 1);        //Fourier transform

                for(int i = 0; i < right + 2; ++i )      //get the absolute value of FFT
                {
                    dataPhase[c1][c2][ i ] = atan(spectre[0][ i * 2] / spectre[0][ i * 2 +1]) - atan(spectre[1][ i * 2] / spectre[1][ i * 2 +1]); //!!!!!!!!!!atan(Im/Re)
                    help[0] = ( spectre[0][ i * 2 +1 ] * spectre[0][ i * 2 +1 ] + spectre[0][ i * 2 ] * spectre[0][ i * 2 ] ) * 2 * 0.004/fftLength;
                    help[1] = ( spectre[1][ i * 2 +1 ] * spectre[1][ i * 2 +1 ] + spectre[1][ i * 2 ] * spectre[1][ i * 2 ] ) * 2 * 0.004/fftLength;
                    dataPhase[c1][c2][ i ] *= sqrt(help[0] * help[1]) / (help[0] + help[1]); //normalisation
                    if(QString::number(dataPhase[c1][c2][i]).contains('n')) //why nan and inf???
                    {
                        cout<<"dataPhase[" << c1 << "][" << c2 << "][" << i << "] = "<< dataPhase[c1][c2][i] <<endl;
                        return 0;
                    }
                }
            }

        }





        for(int i=0; i<ns; ++i)
        {
            delete []data2[i];
        }
        delete []spectre[0];
        delete []spectre[1];
        delete []spectre;
        delete []data2;
        delete []help;
        dir->cd(dirBC->absolutePath());
        return 1;
}

void Spectre::drawWavelets()
{
    dir->cd(dirBC->absolutePath() + QDir::separator() + "visualisation" + QDir::separator() + "wavelets");
    //make dirs
    for(int i = 0; i < ns; ++i)
    {
        dir->mkdir(QString::number(i));
        dir->cd(QString::number(i));
        for(int j = 0; j < ns; ++j)
        {
            dir->mkdir(QString::number(j));
        }
        dir->cdUp();
    }



    dir->cd(ui->lineEdit_1->text());
    nameFilters.clear(); //generality
    nameFilters.append("*_241*");
    nameFilters.append("*_247*");
    nameFilters.append("*_254*");
    lst = dir->entryList(nameFilters, QDir::Files);

    FILE * file1;

    for(int a = 0; a < lst.length(); ++a) // wtf a = 54?
    {


        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append(lst[a]));
        cout<<helpString.toStdString()<<endl;
        file1 = fopen(helpString.toStdString().c_str(),"r");
        if(file1==NULL)
        {
            QMessageBox::warning((QWidget*)this, tr("Warning"), tr("wrong filename"), QMessageBox::Ok);
            continue;
        }

        if(ui->amplitudeWaveletButton->isChecked())
        {
            for(int channel = 0; channel < ns; ++channel)
            {
                helpString = lst[a];
                helpString.replace('.', '_');
                helpString=QDir::toNativeSeparators(dirBC->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append("wavelets").append(QDir::separator()).append(QString::number(channel)).append(QDir::separator()).append(helpString).append("_wavelet_").append(QString::number(channel)).append(".jpg"));
                cout<<helpString.toStdString()<<endl;
                wavelet(helpString, file1, ns, channel, 20., 5., 0.95, 32);
            }
        }
        if(ui->phaseWaveletButton->isChecked())
        {
            for(int channel1 = 0; channel1 < ns; ++channel1)
            {
                for(int channel2 = channel1+1; channel2 < ns; ++channel2)
                {
                    helpString = lst[a];
                    helpString.replace('.', '_');
                    helpString = QDir::toNativeSeparators(dirBC->absolutePath().append(QDir::separator()).append("visualisation").append(QDir::separator()).append("wavelets").append(QDir::separator()).append(QString::number(channel1)).append(QDir::separator()).append(QString::number(channel2)).append(QDir::separator()).append(helpString).append("_wavelet_").append(QString::number(channel1)).append("_").append(QString::number(channel2)).append(".jpg"));
                    cout<<helpString.toStdString()<<endl;
                    waveletPhase(helpString, file1, ns, channel1, channel2, 20., 5., 0.95, 32);
//                    if(channel2 == 2) return;
                }
            }
        }
        fclose(file1);
        if(a==2) return;

        if(100*(a+1)/lst.length() > ui->progressBar->value())
        {
            ui->progressBar->setValue(100*(a+1)/lst.length());
        }
    }
    ui->progressBar->setValue(0);


    dir->cd(dirBC->absolutePath());
}

