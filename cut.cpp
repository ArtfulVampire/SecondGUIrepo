#include "cut.h"
#include "ui_cut.h"


/*after every function the QDir dir is set to the ExpName directory*/

QString qstr(int &input)
{
    QString a;
    a.setNum(input);
    return a;
}

Cut::Cut(QDir * dir_, int ns_) :
    ui(new Ui::Cut)
{
    ui->setupUi(this);
    this->setWindowTitle("Cut-e");
    dir = new QDir();
    dir->cd(dir_->absolutePath());

    autoFlag=false;

    this->ui->spinBox->setMaximum(15000);
    this->ui->doubleSpinBox->setMaximum(60.);

    flagWnd=0;

    painter= new QPainter();

    ui->nsBox->setValue(21);
    ui->eogSpinBox->setValue(2);
    ui->eogDoubleSpinBox->setValue(2.40);
    ui->eogDoubleSpinBox->setSingleStep(0.1);

    ui->nsBox->setValue(ns_);
    ns = ns_;


    ui->nsBox->setMaximum(40);
    ui->checkBox->setChecked(false);
    ui->checkBox_2->setChecked(true);
    ui->timeShiftSpinBox->setMaximum(1000.);
    ui->timeShiftSpinBox->setValue(125.);
    ui->timeShiftSpinBox->setSingleStep(25.);

    ui->wndLengthSpinBox->setValue(4.0);

    ui->dirBox->addItem("Realisations");
    ui->dirBox->addItem("cut");
    ui->dirBox->addItem("windows");
    ui->dirBox->setCurrentIndex(0);

//    this->ui->lineEdit_1->setText("0.0");
//    this->ui->lineEdit_2->setText("125");
//    this->ui->lineEdit_3->setText("4.0");
    this->ui->extYbox->setValue(7);
    this->ui->tempSpinBox->setMaximum(1500);
    ui->nextButton->setShortcut(tr("d"));
    ui->prevButton->setShortcut(tr("a"));
    ui->cutButton->setShortcut(tr("c"));
    ui->zeroButton->setShortcut(tr("z"));
    ui->saveButton->setShortcut(tr("s"));
    ui->rewriteButton->setShortcut(tr("r"));

    ui->eyesSlicesSpinBox->setMaximum(1e5);

    ui->picLabel->installEventFilter(this);

    ui->scrollArea->setWidget(ui->picLabel);
    ui->scrollArea->installEventFilter(this);


    QFileDialog *browser = new QFileDialog();
    browser->setDirectory(dir->absolutePath().append(QDir::separator()).append("Realisations"));
    browser->setViewMode(QFileDialog::Detail);

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files);
    dir->cdUp();
    currentNumber=-1;

//    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));

    QObject::connect(ui->browseButton, SIGNAL(clicked()), browser, SLOT(show()));
    QObject::connect(browser, SIGNAL(fileSelected(QString)), ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(browser, SIGNAL(fileSelected(QString)), this, SLOT(createImage(QString)));
    QObject::connect(browser, SIGNAL(fileSelected(QString)), browser, SLOT(hide()));

    QObject::connect(this, SIGNAL(buttonPressed(char,int)), this, SLOT(mousePressSlot(char,int)));
    QObject::connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));
    QObject::connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    QObject::connect(ui->cutButton, SIGNAL(clicked()), this, SLOT(cut()));
    QObject::connect(ui->zeroButton, SIGNAL(clicked()), this, SLOT(zero()));
    QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
    QObject::connect(ui->rewriteButton, SIGNAL(clicked()), this, SLOT(rewrite()));
    QObject::connect(this, SIGNAL(openFile(QString)), ui->lineEdit, SLOT(setText(QString)));
    QObject::connect(this, SIGNAL(openFile(QString)), this, SLOT(createImage(QString)));
    QObject::connect(ui->windowsButton, SIGNAL(clicked()), this, SLOT(lnWndAll()));
    QObject::connect(ui->nsBox, SIGNAL(valueChanged(int)), this, SLOT(setNs(int)));
    QObject::connect(ui->cycleWndLengthButton, SIGNAL(clicked()), this, SLOT(cycleWndLength()));

    QObject::connect(ui->tempSpinBox, SIGNAL(valueChanged(int)), this, SLOT(drawLogistic()));
    QObject::connect(ui->cutEyesButton, SIGNAL(clicked()), this, SLOT(cutEyesAll()));
    QObject::connect(ui->paintDistrButton, SIGNAL(clicked()), this, SLOT(paintDistr()));

    this->ui->tempSpinBox->setValue(10);
    this->setAttribute(Qt::WA_DeleteOnClose);

//    if(lst.length()!=0)
//    {
//        currentNumber=0;
//        emit(createImage(dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator()).append(lst.at(0))));
//    }

    data3 = new double* [ns];         //array for all data[numOfChan][numOfTimePin]
    for(int i=0; i<ns; ++i)
    {
        data3[i]=new double [250*250];         /////////generality for 250 seconds
    }
}

Cut::~Cut()
{
    delete ui;
}

void Cut::browse()
{
    helpString = QFileDialog::getOpenFileName((QWidget*)this, tr("Open realisation"), dir->absolutePath().append(QDir::separator()).append("Realisations"));    
    if(helpString=="")
    {
        QMessageBox::information((QWidget*)this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
        return;
    }
    ui->lineEdit->setText(helpString);
    createImage(helpString);
}

void Cut::closeEvent ( QCloseEvent * event )
{
    if(painter->isActive()) painter->end();
    event->accept();
}

bool Cut::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->picLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button()==Qt::LeftButton) emit buttonPressed('l', mouseEvent->x());
            if(mouseEvent->button()==Qt::RightButton) emit buttonPressed('r', mouseEvent->x());

//                            cout<<"left="<<leftLimit<<" right="<<rightLimit<<endl;
            return true;
        }
        else
        {
            return false;
        }
    }

    if(obj==ui->scrollArea)
    {
        if(event->type() == QEvent::Wheel)
        {

            QWheelEvent * scrollEvent = static_cast<QWheelEvent*>(event);
            double coeff = -0.9;
            ui->scrollArea->horizontalScrollBar()->setSliderPosition( ui->scrollArea->horizontalScrollBar()->sliderPosition() + coeff*scrollEvent->delta());
            return true;


            //zoom
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

                //picLabel varies
                //Pixmap & scrollArea are constant


                pnt->setPen(QPen(QBrush("blue"), 2));
                pnt->drawLine(leftLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), 0, leftLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), tmp->height());

                pnt->setPen(QPen(QBrush("red"), 2));
                pnt->drawLine(rightLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), 0, rightLimit*tmp->width()/(double(ui->picLabel->size().width())*zoomCurr/zoomPrev), tmp->height());
                pnt->end();


                ui->picLabel->setPixmap(tmp->scaled(ui->scrollArea->size().width()*zoomCurr, ui->scrollArea->size().height()-20));  //-20 generality
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

double logistic_(double &x, double t)
{
    if( x >   37.*t )  return 1.;
    if( x < -115.*t )  return 0.;
    return 1. / ( 1. + exp(-x/t) );
}

void Cut::cutEyesAll()
{
//    duration = time(NULL);

    //automatization
    if(autoFlag)
    {
        ui->checkBox->setChecked(false);
        dir->cd("Realisations");
        helpString = dir->entryList(QDir::Files|QDir::NoDotAndDotDot).operator [](0);
        dir->cdUp();
        helpString.prepend(dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator()));
        cout<<helpString.toStdString()<<endl;
        emit createImage(helpString);
    }

//    cout<<"1"<<endl;
    dir->setSorting(QDir::Name);
    dir->cd(ui->dirBox->currentText());  //generality
    nameFilters.clear();
    nameFilters.append("*_241*");
    nameFilters.append("*_247*");
    nameFilters.append("*_254*"); // no need?

    lst=dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
    dir->cdUp();

    int NumEog = ui->eogSpinBox->value();
    double * thresholdEog = new double [NumEog];
    for(int i=0; i<NumEog; ++i)
    {
        thresholdEog[i]=0.;
    }
//    cout<<"2"<<endl;

    double ** eogArray = new double* [NumEog];
    for(int i=0; i<NumEog; ++i)
    {
        eogArray[i] = new double [8192*50]; //50, size generality
    }
    int currentSlice=0;
//    cout<<"3"<<endl;

    for(int k=0; k<int(lst.length()/4); ++k)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(ui->dirBox->currentText()).append(QDir::separator()).append(lst.at(rand()%lst.length()));
        emit openFile(helpString);

        for(int j=0; j<NumOfSlices; ++j)
        {
            for(int i=ns-NumEog; i<ns; ++i)
            {
                eogArray[i-(ns-NumEog)][currentSlice] = data3[i][j];
            }
            ++currentSlice;
        }

    }
//    cout<<"4"<<endl;

    for(int i=0; i<NumEog; ++i)
    {
        for(int j=0; j<currentSlice; ++j)
        {
            thresholdEog[i]+=eogArray[i][j]*eogArray[i][j];
        }
        thresholdEog[i]/=currentSlice;
        thresholdEog[i]=sqrt(thresholdEog[i]); //thresholdEog[i] = dispersion
    }

    for(int i=0; i<NumEog; ++i)
    {
        delete[] eogArray[i];
    }
    delete[] eogArray;


//    cout<<"5"<<endl;
    FILE * eyes;
    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("eyesSlices");
    eyes = fopen(helpString.toStdString().c_str(), "w");

    int flag;
    int NumOfEogSlices = 0;
    int num=0;
    for(int i=0; i<lst.length(); ++i)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(ui->dirBox->currentText()).append(QDir::separator()).append(lst[i]);

        emit openFile(helpString);
        num+=NumOfSlices;
        for(int i=0; i<NumOfSlices; ++i)
        {
            flag=1;
            for(int j=0; j<NumEog; ++j)
            {
                if(fabs(data3[ns-NumEog + j][i]) < ui->eogDoubleSpinBox->value()*thresholdEog[j]) flag=0;
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

//        for(int i=0; i<ns; ++i)
//        {
//            delete []data3[i];
//        }
//        delete []data3;
    }
//    cout<<"6"<<endl;

    fclose(eyes);
//    cout<<"num "<<num<<endl;
//    cout<<"NumOfSlices "<<NumOfEogSlices<<endl;

    QFile *file = new QFile(helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("eyesSlices"));
    file->open(QIODevice::ReadOnly);
    QByteArray contents = file->readAll();
    file->close();
    file->open(QIODevice::WriteOnly);
    file->write("NumOfSlices ");
    file->write(qstr(NumOfEogSlices).toStdString().c_str());
    file->write("\r\n");
    file->write(contents);
    file->close();
    delete file;

    ui->eyesSlicesSpinBox->setValue(NumOfEogSlices);
//cout<<"7"<<endl;

//    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(eyesSlices);
//    eyes = fopen(helpString.toStdString().c_str(), "r+");
//    rewind(eyes);
//    fprintf(eyes, "NumOfSlices %d\n", NumOfEogSlices);
//    fclose(eyes);

    delete []thresholdEog;

//    duration = time(NULL) - duration;

    tmp.setNum(NumOfEogSlices);
//    helpString.setNum(int(duration));
//    helpString.prepend("Eyes cut \nTime = ");
//    helpString.append(" sec\n");
    helpString = "NumOfEyesSlices = ";
    helpString.append(tmp);

    //automatization
    if(!autoFlag)QMessageBox::information((QWidget*)this, tr("Info"), helpString, QMessageBox::Ok);
}





void Cut::setNs(int a)
{
    ns=a;
}

void Cut::drawLogistic()
{
    QPixmap pic = QPixmap(800, 600);
    pic.fill();
    QPainter *paint = new QPainter();
    paint->begin(&pic);

    double one=15.;
    for(int i=int(pic.width()*0.02); i<int(pic.width()*0.98); ++i)
    {
        paint->drawLine(i,   pic.height()*0.8-pic.height()*0.65*(1./( 1. + exp(-double(i-pic.width()/2.)/one/double(this->ui->tempSpinBox->value()) ))),   i+1,   pic.height()*0.8-pic.height()*0.65*(1./( 1. + exp(-double(i+1-pic.width()/2.)/one/double(this->ui->tempSpinBox->value())) )) );

    }
    paint->drawLine(pic.width()*0.03,  pic.height()*0.8,  pic.width()*0.97,  pic.height()*0.8);//X line
    paint->drawLine(pic.width()/2.,  pic.height()*0.1,  pic.width()/2.,  pic.height()*0.85);//Y line

    paint->drawLine(pic.width()/2.,  pic.height()*0.1,  pic.width()/2.+6,  pic.height()*0.1+17);//Y arrow
    paint->drawLine(pic.width()/2.,  pic.height()*0.1,  pic.width()/2.-6,  pic.height()*0.1+17);//Y arrow

    paint->drawLine(pic.width()*0.97,  pic.height()*0.8,  pic.width()*0.97-17,  pic.height()*0.8-6);//X arrow
    paint->drawLine(pic.width()*0.97,  pic.height()*0.8,  pic.width()*0.97-17,  pic.height()*0.8+6);//X arrow

    paint->drawLine(pic.width()/2.-5,  pic.height()*0.15,  pic.width()/2.+5,  pic.height()*0.15);//1 on Y line
    paint->drawLine(pic.width()/2.-5,  pic.height()*0.475,  pic.width()/2.+5,  pic.height()*0.475);//0.5 Y line
    paint->drawLine(pic.width()/2.+one,  pic.height()*0.8,  pic.width()/2.+one,  pic.height()*0.85); //1 X line
    paint->drawLine(pic.width()/2.+one*5,  pic.height()*0.8,  pic.width()/2.+one*5,  pic.height()*0.85); //5 X line
    paint->drawLine(pic.width()/2.+one*10,  pic.height()*0.8,  pic.width()/2.+one*10,  pic.height()*0.85); //10 X line

    paint->drawText(pic.width()/2.-15,  pic.height()*0.1+6, "Y");//Y arrow
    paint->drawText(pic.width()*0.97+6,  pic.height()*0.8+14,  "X");//X arrow

    paint->drawText(pic.width()/2.+one-3,   pic.height()*0.85+15,   "1");//1 X line
    paint->drawText(pic.width()/2.+one*5-3,   pic.height()*0.85+15,   "5");//5 X line
    paint->drawText(pic.width()/2.+one*10-3,   pic.height()*0.85+15,   "10");//10 X line
    paint->drawText(pic.width()/2.-3,   pic.height()*0.85+15,   "0");//0 X line
    paint->drawText(pic.width()/2.-18,   pic.height()*0.15+6,   "1");//1 Y line
    paint->drawText(pic.width()/2.-30 ,   pic.height()*0.475+5,   "0.5");//0.5 Y line

    //border
    paint->drawLine(1,  pic.height()-1,  pic.width()-1,  pic.height()-1);//hor bot
    paint->drawLine(1,  1,  pic.width()-1,  1);//hor top
    paint->drawLine(1,  1,  1,  pic.height()-1);//vert left
    paint->drawLine(pic.width()-1,  1,  pic.width()-1,  pic.height()-1);// vert right



    pic.save(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Logistic.jpg"), 0, 100);
    paint->end();
    delete paint;
    ui->label_7->setPixmap(QPixmap(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Logistic.jpg")).scaled(ui->label_7->size()));
}

void Cut::cycleWndLength()
{
    flagWnd = 1;
    for(double s=0.2; s<=4.0; s+=0.2)
    {        
        ui->wndLengthSpinBox->setValue(s);;
        lnWndAll();
    }
    flagWnd = 0;
    QMessageBox::information((QWidget*)this, tr("Info"), tr("windows examined"), QMessageBox::Ok);
}

void Cut::cycleTimeShift()
{    
    flagWnd = 2;
    for(double s=25; s<=250; s+=25)
    {
        ui->timeShiftSpinBox->setValue(s);
        lnWndAll();
    }
    flagWnd = 0;
    QMessageBox::information((QWidget*)this, tr("Info"), tr("windows examined"), QMessageBox::Ok);
}

void Cut::lnWndAll()
{
    dir->setSorting(QDir::Name);
    dir->cd("Realisations");  //generality
    nameFilters.clear();
    nameFilters.append("*_241*");
    nameFilters.append("*_247*");
    nameFilters.append("*_254*");

//    nameFilters.append("*SG*");
//    nameFilters.append("*VW*");

    lst=dir->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
    dir->cdUp();

    //cycle
    wnd1=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Spatial.txt").toStdString().c_str(), "w");
    wnd2=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Verbal.txt").toStdString().c_str(), "w");
    wnd3=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Idle.txt").toStdString().c_str(), "w");
    f1=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SpatialLen.txt").toStdString().c_str(), "w");
    f2=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("VerbalLen.txt").toStdString().c_str(), "w");
    f3=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("IdleLen.txt").toStdString().c_str(), "w");

//    weights=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("weights4s_rdc.wts").toStdString().c_str(),"r");    ////////generality

    weights=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("weights.wts").toStdString().c_str(),"r");

    weight = new double*[3];
    for(int i=0; i<3; ++i)
    {
        weight[i] = new double [247*19+1];
    }

    //save weights into arrays
    for(int j=0; j<3; ++j)
    {
        for(int i=0; i<247*19; ++i)
        {
            fscanf(weights, "%lf", &weight[j][i]);
        }
    }
    for(int j=0; j<3; ++j)
    {
        fscanf(weights, "%lf", &weight[j][247*19]); //generality
    }
    fclose(weights);

    timeShift=ui->timeShiftSpinBox->value();   //in time-bins
    wndLength=ui->wndLengthSpinBox->value();   //in seconds

    cout<<"timeShift="<<timeShift<<" wndLength="<<wndLength<<endl;

    numOfWndSpat=0;
    numOfWndVerb=0;
    numOfWndIdle=0;

    for(int i=0; i<lst.length(); ++i)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Realisations").append(QDir::separator()).append(lst.at(i));
        emit openFile(helpString);
        lnWnd16();
    }
//    cout<<"total "<<numOfWnd<<" windows"<<endl;
    fclose(wnd1);
    fclose(wnd2);
    fclose(wnd3);
    fclose(f1);
    fclose(f2);
    fclose(f3);

    paintDistr();   ////////////NAMES!!!!

    for(int i=0; i<3; ++i)
    {
        delete []weight[i];
    }
    delete []weight;

    if(flagWnd==0) QMessageBox::information((QWidget*)this, tr("Info"), tr("windows examined"), QMessageBox::Ok);

}

void Cut::lnWnd16()
{
    double *output = new double [3]; //generality NumOfClasses
    int cls;

    double **dataW = new double*[ns];
    for(int i=0; i<ns; ++i)
    {
        dataW[i] = new double [4096];
    }
    double norm1=4096./(wndLength*250.);
    double * spectre = new double [4096*2];

    double ** dataFFT = new double * [ns];
    for(int i=0; i<ns; ++i)
    {
        dataFFT[i] = new double [4096/2];
    }

    int p=0; //offset from the realisation beginning
    int n=0; //number of windows in a realisation

    n=int((NumOfSlices-wndLength*250.)/timeShift);
    if(n<0) n=0;

    //print into *Len files number of windows in every realisation of the class
    if(fileName.contains("_241")) fprintf(f1, "%d\n", n);
    if(fileName.contains("_247")) fprintf(f2, "%d\n", n);
    if(fileName.contains("_254")) fprintf(f3, "%d\n", n);


    while(NumOfSlices>int(p+wndLength*250.)) //for every window in a realisation
    {
        for(int j=0; j<int(wndLength*250.); ++j) //for every time bin
        {
            for(int k=0; k<ns; ++k) //for every channel
            {
                dataW[k][j]=data3[k][j+p];   //dataW - array for window-signal
            }
        }
        for(int j=int(wndLength*250.); j<4096; ++j) // put zeros till the end of FFT-window
        {
            for(int k=0; k<ns; ++k)
            {
                dataW[k][j]=0.;
            }
        }
        //count spectra
        for(int c1=0; c1<ns; ++c1)
        {
            for(int i=0; i<4096; ++i)            //make appropriate array
            {
                spectre[ i * 2 ] = 0.;
                spectre[ i * 2 + 1 ] = (double)(dataW[c1][ i ]*sqrt(norm1));
            }
            four1(spectre-1, 4096, 1);        //Fourier transform

            for(int i = 0; i < 4096/2; i++ )      //get the absolute value of FFT
            {
                dataFFT[c1][ i ] = ( spectre[ i * 2 +1] * spectre[ i * 2 +1] + spectre[ i * 2 + 1 +1] * spectre[ i * 2 + 1 +1] )*2*0.004/4096.;
            }
        }

        //classificate

        for(int j=0; j<3; ++j) //generality 3 NumOfClasses
        {
            output[j]=0.;
            for(int i=0; i<247*19; ++i)
            {
                output[j]+=dataFFT[i/247][i%247+81]*weight[j][i];  //+20 coz needed spectra' range is 20-82
            }

            output[j]+=weight[j][247*19];
            output[j]=logistic_(output[j], double(ui->tempSpinBox->value()));
        }
        cls=0;
        for(int j=1; j<3; ++j) //generality 3 NumOfClasses
        {
            if(output[j]>output[cls]) cls=j;
        }

        leftLimit=int(p*1050./NumOfSlices);                     ////////////////ALARM!!! generality
        rightLimit=int((p+250.*wndLength)*1050./NumOfSlices);  /////////////?ALARM!!!   generality

        //cut or not cut window
        if(fileName.contains("_241") && cls==0)
        {
            if(ui->checkBox_2->isChecked()) cut();
            fprintf(wnd1, "%d\n", p);  //print an offset of the right window into file
            ++numOfWndSpat;            //number of right classificated Spatial windows
        }
        if(fileName.contains("_247") && cls==1)
        {
            if(ui->checkBox_2->isChecked()) cut();
            fprintf(wnd2, "%d\n", p);    //print an offset of the right window into file
            ++numOfWndVerb;              //number of right classificated Verbal windows
        }
        if(fileName.contains("_254") && cls==2)
        {
            if(ui->checkBox_2->isChecked()) cut();
            fprintf(wnd3, "%d\n", p);    //print an offset of the right window into file
            ++numOfWndIdle;
        }
        p+=timeShift;  //add offset
    }
//    cout<<fileName.toStdString()  <<endl;//<<" total "<<numOfWnd<<" windows of "<<int((NumOfSlices-wndLength*250.)/timeShift+1)<<endl;

    for(int i=0; i<ns; ++i)
    {
        delete []dataFFT[i];
        delete []dataW[i];
    }
    delete []spectre;
    delete []dataFFT;
    delete []dataW;
    delete []output;

//    for(int i=0; i<ns; ++i)
//    {
//        delete []data3[i];
//    }
//    delete []data3;
}

void Cut::paintDistr()  ///////////generality//////////////
{
    wnd1=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Spatial.txt").toStdString().c_str(), "r");
    wnd2=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Verbal.txt").toStdString().c_str(), "r");
    wnd3=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Idle.txt").toStdString().c_str(), "r");

    f1=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SpatialLen.txt").toStdString().c_str(), "r");
    f2=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("VerbalLen.txt").toStdString().c_str(), "r");
    f3=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("IdleLen.txt").toStdString().c_str(), "r");

    //offsets of right windows
    int * a1 = new int [numOfWndSpat];
    int * a2 = new int [numOfWndVerb];
    int * a3 = new int [numOfWndIdle];

    // ~lengths of realisations in window numbers
    int * b1 = new int [200];
    int * b2 = new int [200];
    int * b3 = new int [200];


    int num1=0;
    int num2=0;
    int num3=0;
    int numb1=0;
    int numb2=0;
    int numb3=0;

    while(!feof(wnd1))
    {
        fscanf(wnd1, "%d\n", &a1[num1]);    //number of right windows
        ++num1;
    }
    while(!feof(wnd2))
    {
        fscanf(wnd2, "%d\n", &a2[num2]);    //number of right windows
        ++num2;
    }
    while(!feof(wnd3))
    {
        fscanf(wnd3, "%d\n", &a3[num3]);    //number of right windows
        ++num3;
    }

    //ANOTHER COUNTER!!!
    while(!feof(f1))
    {
        fscanf(f1, "%d\n", &b1[numb1]);    //number of right windows
        ++numb1;
    }
    while(!feof(f2))
    {
        fscanf(f2, "%d\n", &b2[numb2]);    //number of right windows
        ++numb2;
    }
    while(!feof(f3))
    {
        fscanf(f3, "%d\n", &b3[numb3]);    //number of right windows
        ++numb3;
    }


    QPixmap pic1 = QPixmap(1400, 600);
    pic1.fill();
    QPainter *paint1 = new QPainter();
    paint1->begin(&pic1);

    QPixmap pic2 = QPixmap(1400, 600);
    pic2.fill();
    QPainter *paint2 = new QPainter();
    paint2->begin(&pic2);

    QPixmap pic3 = QPixmap(1400, 600);
    pic3.fill();
    QPainter *paint3 = new QPainter();
    paint3->begin(&pic3);

    paint1->drawLine(0, 500, 1400, 500);
    paint2->drawLine(0, 500, 1400, 500);
    paint3->drawLine(0, 500, 1400, 500);


    //how many windows with cocncrete offset classificated right
    int * count1 = new int[7500/timeShift];
    int * count2 = new int[7500/timeShift];
    int * count3 = new int[7500/timeShift];

    //how many realisations contain a window with every offset (long enough)
    int * c1 = new int[7500/timeShift];
    int * c2 = new int[7500/timeShift];
    int * c3 = new int[7500/timeShift];

    for(int j=0; j<7500/timeShift; ++j)
    {
        count1[j]=0;
        count2[j]=0;
        count3[j]=0;
        c1[j]=0;
        c2[j]=0;
        c3[j]=0;
    }

    for(int p=0; p<7500; p+=timeShift)
    {

        for(int i=0; i<num1; ++i)
        {
            if(a1[i]==p)
            {
                count1[p/timeShift]+=1;
            }
        }
        for(int i=0; i<num2; ++i)
        {
            if(a2[i]==p)
            {
                count2[p/timeShift]+=1;
            }
        }
        for(int i=0; i<num3; ++i)
        {
            if(a3[i]==p)
            {
                count3[p/timeShift]+=1;
            }
        }

        for(int i=0; i<numb1; ++i)
        {
            if(b1[i]>=(p/timeShift))
            {
                c1[p/timeShift]+=1;
            }
        }
        for(int i=0; i<numb2; ++i)
        {
            if(b2[i]>=(p/timeShift))
            {
                c2[p/timeShift]+=1;
            }
        }
        for(int i=0; i<numb3; ++i)
        {
            if(b3[i]>=(p/timeShift))
            {
                c3[p/timeShift]+=1;
            }
        }
    }

    int extY=ui->extYbox->value(); //extY pix per 1 wnd, 500 pix - available height
    //draw number of realisations as a line
    cout<<"Spat wnds="<<numOfWndSpat<<" Verb wnds="<<numOfWndVerb<<" Idle wnds="<<numOfWndIdle<<endl;
    paint1->drawLine(0, 500-extY*numb1, 1400, 500-extY*numb1);
    paint2->drawLine(0, 500-extY*numb2, 1400, 500-extY*numb2);
    paint3->drawLine(0, 500-extY*numb3, 1400, 500-extY*numb3);

    //draw a graph itself
    for(int j=0; j<7500/timeShift; ++j)
    {
        if(c1[j]!=0 && c1[j+1]!=0) paint1->drawLine(j*pic1.width()/(7500./timeShift), 500-extY*count1[j]*numb1/c1[j], (j+1)*pic1.width()/(7500./timeShift), 500-extY*count1[j+1]*numb1/c1[j+1]); //extY pix per 1 wnd
        if(c2[j]!=0 && c2[j+1]!=0) paint2->drawLine(j*pic2.width()/(7500./timeShift), 500-extY*count2[j]*numb2/c2[j], (j+1)*pic2.width()/(7500./timeShift), 500-extY*count2[j+1]*numb2/c2[j+1]); //extY pix per 1 wnd
        if(c3[j]!=0 && c3[j+1]!=0) paint3->drawLine(j*pic3.width()/(7500./timeShift), 500-extY*count3[j]*numb3/c3[j], (j+1)*pic3.width()/(7500./timeShift), 500-extY*count3[j+1]*numb3/c3[j+1]); //extY pix per 1 wnd
    }


    //draw the sticks
    for(int i=0; i<25; ++i)
    {
        //1 type - spatial
        paint1->drawLine(pic1.width()/(7500/250.)*i*2, 500, pic1.width()/(7500/250.)*i*2, 500+15); //vertical under x axis
        paint1->drawLine(0, 500-extY*5*i, 10, 500-extY*5*i);                                                     //horizontal near y axis

        helpString.clear();
        num1=2*i;
        helpString.append(qstr(num1)).append(" sec");
        paint1->drawText(pic1.width()/(7500/250.)*i*2-10, 500+20+5, helpString); // x-stick description

        helpString.clear();
        num1=5*i;
        helpString.append(qstr(num1)).append(" wnd");
        paint1->drawText(12, 500-extY*5*i, helpString);                                 // y-stick description


        //2 type - verbal
        paint2->drawLine(pic2.width()/(7500/250.)*i*2, 500, pic2.width()/(7500/250.)*i*2, 500+15);  //vertical under x axis
        paint2->drawLine(0, 500-extY*5*i, 10, 500-extY*5*i);                                                      //horizontal near y axis

        helpString.clear();
        num2=2*i;
        helpString.append(qstr(num2)).append(" sec");
        paint2->drawText(pic2.width()/(7500/250.)*i*2-10, 500+20+5, helpString);          // x-stick description

        helpString.clear();
        num2=5*i;
        helpString.append(qstr(num2)).append(" wnd");
        paint2->drawText(12, 500-extY*5*i, helpString);                                           // y-stick description

        //2 type - verbal
        paint3->drawLine(pic3.width()/(7500/250.)*i*2, 500, pic3.width()/(7500/250.)*i*2, 500+15);  //vertical under x axis
        paint3->drawLine(0, 500-extY*5*i, 10, 500-extY*5*i);                                                      //horizontal near y axis

        helpString.clear();
        num3=2*i;
        helpString.append(qstr(num3)).append(" sec");
        paint3->drawText(pic3.width()/(7500/250.)*i*2-10, 500+20+5, helpString);          // x-stick description

        helpString.clear();
        num3=5*i;
        helpString.append(qstr(num3)).append(" wnd");
        paint3->drawText(12, 500-extY*5*i, helpString);                                           // y-stick description
    }

    if(flagWnd == 0) helpString="";
    else if(flagWnd == 1)
    {
        helpString.setNum(ui->wndLengthSpinBox->value());
        for(int i=0; i<helpString.length(); ++i)
        {
            if(helpString[i]=='.') helpString[i]=',';
        }
        helpString.prepend("_wndL_");
    }
    else
    {
        helpString.setNum(int(ui->timeShiftSpinBox->value()));
        helpString.prepend("_timeSh_");
    }
    pic1.save(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Help").append(QDir::separator()).append("Distr-Spatial").append(helpString).append(".jpg"));
    pic2.save(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Help").append(QDir::separator()).append("Distr-Verbal").append(helpString).append(".jpg"));
    pic3.save(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Help").append(QDir::separator()).append("Distr-Idle").append(helpString).append(".jpg"));
    paint1->end();
    paint2->end();
    paint3->end();
    delete paint1;
    delete paint2;
    delete paint3;


    delete []count1;
    delete []count2;
    delete []count3;
    delete []a1;
    delete []a2;
    delete []a3;
    delete []b1;
    delete []b2;
    delete []b3;
    delete []c1;
    delete []c2;
    delete []c3;



    fclose(wnd1);
    fclose(wnd2);
    fclose(wnd3);
    fclose(f1);
    fclose(f2);
    fclose(f3);

    remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Spatial.txt").toStdString().c_str());
    remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Verbal.txt").toStdString().c_str());
    remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Idle.txt").toStdString().c_str());

    remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SpatialLen.txt").toStdString().c_str());
    remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("VerbalLen.txt").toStdString().c_str());
    remove(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("IdleLen.txt").toStdString().c_str());
}

void Cut::createImage(QString str) //
{
    zoomPrev = 1.;
    zoomCurr = 1.;
    addNum = 1;
    dir->setPath(str);
    currentFile = str;
    fileName = dir->dirName(); //real file name TSL_241.5 generality?

    leftLimit = 0;
    Eyes = 0;

    dir->cdUp();
    cout<<"dir = "<<dir->absolutePath().toStdString()<<endl;
    lst = dir->entryList(QDir::Files);
    dir->cdUp();
    //now we are in ExpName dir
//    cout<<"ExpName dir"<<dir->absolutePath().toStdString()<<endl;

    for(int i=0; i<lst.length(); ++i)
    {
        if(lst[i].contains(fileName))
        {
            currentNumber = i;
            break;
        }
    }

    //find a point  and replace with _
    fileName.replace('.', '_');
//    for(int i=fileName.length(); i>0; --i)
//    {
//        if(fileName[i]=='.')
//        {
//            fileName[i]='_';
//            break;
//        }
//    }

//    helpString = QDir::separator();
//    helpString.append("Realisations");
    if(str.contains("Realisations"))
    {
        if(ns==19)
        {
            currentPicPath=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("after").append(QDir::separator()).append(fileName).append(".jpg"); //absolute path of appropriate signal.jpg
        }
        if(ns==21)
        {
            currentPicPath=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("before").append(QDir::separator()).append(fileName).append(".jpg"); //absolute path of appropriate signal.jpg
        }
    }
    else if(str.contains("windows") && !str.contains("fromreal"))
    {
        if(ns==19)
        {
            currentPicPath=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("windows").append(QDir::separator()).append("after").append(QDir::separator()).append(fileName).append(".jpg"); //absolute path of appropriate signal.jpg
        }
        if(ns==21)
        {
            currentPicPath=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("Signals").append(QDir::separator()).append("windows").append(QDir::separator()).append("before").append(QDir::separator()).append(fileName).append(".jpg"); //absolute path of appropriate signal.jpg
        }
    }
    else if(str.contains("cut"))
        {
            if(ns==19)
            {
                currentPicPath=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("after").append(QDir::separator()).append(fileName).append(".jpg"); //absolute path of appropriate signal.jpg
            }
            if(ns==21)
            {
                currentPicPath=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("before").append(QDir::separator()).append(fileName).append(".jpg"); //absolute path of appropriate signal.jpg
            }
        }


    cout<<"currentPicPath = "<<currentPicPath.toStdString()<<endl;
    cout<<"currentFile = "<<currentFile.toStdString()<<endl;


    file=fopen(currentFile.toStdString().c_str(), "r+");

    fscanf(file, "NumOfSlices %d \n", &NumOfSlices);
    fscanf(file, "NumOfSlicesEyesCut %d \n", &Eyes);

    //not to show when LNwnd
    if(ui->checkBox->isChecked())
    {
        if(painter->isActive()) painter->end();
        currentPic.load(currentPicPath);
        painter->begin(&currentPic);
        ui->picLabel->setText(currentPicPath);

        //initial zoom
        zoomPrev = 1.;
        zoomCurr = NumOfSlices/1050.; //generality

        //picLabel varies
        //Pixmap & scrollArea are constant:NumOfSlices and 1050.
        if(zoomCurr>1.)
        {
            ui->picLabel->setPixmap(currentPic.scaled(ui->scrollArea->size().width()*zoomCurr, ui->scrollArea->size().height()-20));  //-20 generality
            zoomPrev=zoomCurr;
            rightLimit = NumOfSlices;
        }
        else
        {
            zoomCurr = 1.;
            ui->picLabel->setPixmap(currentPic.scaled(ui->scrollArea->size().width(), ui->scrollArea->size().height()-20));  //-20 generality
            rightLimit = ui->scrollArea->size().width();
        }
        //endof initial zoom



//        ui->picLabel->setPixmap(currentPic.scaled(ui->scrollArea->size().width(), ui->scrollArea->size().height()-20)); //-20 generality
//        rightLimit=ui->picLabel->size().width();
    }
    else
    {
        rightLimit=ui->picLabel->size().width();
    }
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(0);
//    rightLimit=ui->picLabel->size().width();
//    rightLimit = NumOfSlices;

//    file=fopen(currentFile.toStdString().c_str(), "r+");

//    fscanf(file, "NumOfSlices %d \n", &NumOfSlices);
//    fscanf(file, "NumOfSlicesEyesCut 0 \n");


//    data3 = new double* [ns];         //array for all data[numOfChan][numOfTimePin]
//    for(int i=0; i<ns; ++i)
//    {
//        data3[i]=new double [NumOfSlices];         /////////generality
//    }

    for(int i=0; i<NumOfSlices; ++i)         //saved BY SLICES!!
    {
        for(int k=0; k<ns; ++k)
        {
            fscanf(file, "%lf\n", &data3[k][i]);
        }
    }
    fclose(file);

    this->ui->spinBox->setValue(NumOfSlices);
    this->ui->doubleSpinBox->setValue(NumOfSlices/250.);
}

void Cut::mousePressSlot(char btn, int coord)
{
    if(btn=='l' && coord<rightLimit) leftLimit=coord;
    if(btn=='r' && coord>leftLimit) rightLimit=coord;

    painter->end();
    currentPic.load(currentPicPath, 0, Qt::ColorOnly);
    painter->begin(&currentPic);


    painter->setPen(QPen(QBrush("blue"), 2));
    painter->drawLine(leftLimit*(currentPic.width()/double(ui->picLabel->width())), 0, leftLimit*(currentPic.width()/double(ui->picLabel->width())), currentPic.height());

    painter->setPen(QPen(QBrush("red"), 2));
    painter->drawLine(rightLimit*(currentPic.width()/double(ui->picLabel->width())), 0, rightLimit*(currentPic.width()/double(ui->picLabel->width())), currentPic.height());

    ui->picLabel->setPixmap(currentPic.scaled(ui->picLabel->size()));
}

void Cut::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

void Cut::next()
{
    if(currentNumber+1 < lst.length())  //generality
    {
        if(currentFile.contains("Realisations"))
        {
            helpString = dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator());
        }
        else if(currentFile.contains("windows") && !currentFile.contains("fromreal"))
        {
            helpString = dir->absolutePath().append(QDir::separator()).append("windows").append(QDir::separator());
        }
        else if(currentFile.contains("cut"))
        {
            helpString = dir->absolutePath().append(QDir::separator()).append("cut").append(QDir::separator());
        }

        if(lst[currentNumber+1].contains("_num")) ++currentNumber; //generality crutch bicycle

        helpString.append(lst[currentNumber+1]);
        if(painter->isActive() == true)
        {
            painter->end();
        }
        cout<<"file to open = "<<helpString.toStdString()<<endl;
        emit openFile(helpString);               //sets dir into ExpName directory
    }
    else
    {
        cout<<"bad number, too big"<<endl; //QMessageBox
    }
}

void Cut::prev()
{
    if(currentNumber-1 >= 0)
    {
        if(currentFile.contains("Realisations"))
        {
            helpString = dir->absolutePath().append(QDir::separator()).append("Realisations").append(QDir::separator());
        }
        else if(currentFile.contains("windows") && !currentFile.contains("fromreal"))
        {
            helpString = dir->absolutePath().append(QDir::separator()).append("windows").append(QDir::separator());
        }
        else if(currentFile.contains("cut"))
        {
            helpString = dir->absolutePath().append(QDir::separator()).append("cut").append(QDir::separator());
        }


        if(lst[currentNumber-1].contains("_num")) --currentNumber; //generality crutch bicycle

        helpString.append(lst.at(currentNumber-1));        
//        cout<<painter->isActive()<<endl;
        if(painter->isActive())
            painter->end();
        cout<<"file to open = "<<helpString.toStdString()<<endl;
        emit openFile(helpString);          //sets dir into ExpName directory
    }
    else
    {
        cout<<"bad number, too little"<<endl; //QMessageBox
    }
}

void Cut::zero()
{
    int h=0;
    for(int i=int(leftLimit*NumOfSlices/ui->picLabel->width()); i<int(rightLimit*NumOfSlices/ui->picLabel->width()); ++i)         //zoom
    {
        for(int k=0; k<ns; ++k)
        {
            if(data3[k][i]==0.) h+=1;
            data3[k][i]=0.;
        }
        if(h < ns) Eyes+=1;      ///generality if there are channel with non-zero values
    }

    this->ui->spinBox->setValue(NumOfSlices-Eyes);
    this->ui->doubleSpinBox->setValue((NumOfSlices-Eyes)/250.);

    paint();


}

void Cut::cut()
{
    dir->cd("windows");
    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(fileName).append(".").append(qstr(addNum)); //TSL
//    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(fileName).append("_").append(qstr(addNum)).append(".").append(fileNumber); //Ln
    ++addNum;
    file=fopen(helpString.toStdString().c_str(),"w");
    fprintf(file, "NumOfSlices %d\n", int(rightLimit*NumOfSlices/ui->picLabel->width())-int(leftLimit*NumOfSlices/ui->picLabel->width()));
    for(int i=int(leftLimit*NumOfSlices/ui->picLabel->width()); i<int(rightLimit*NumOfSlices/ui->picLabel->width()); ++i)         //zoom
    {
        for(int k=0; k<ns; ++k)
        {
            fprintf(file, "%lf\n", data3[k][i]);
        }
    }
    fclose(file);
    dir->cdUp();
    rightLimit=ui->picLabel->width();
    leftLimit=0;
    paint();
}

void Cut::save()
{
    dir->setPath(currentFile);  //.../expName/Realisations/fileName;
    fileName=dir->dirName();    // real fileName
    dir->cdUp();
    dir->cdUp();
    //ExpName dir

    helpString=QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("cut").append(QDir::separator()).append(fileName);

    cout<<helpString.toStdString()<<endl;

    file = fopen(helpString.toStdString().c_str(), "w");
    fprintf(file, "NumOfSlices %d \n", NumOfSlices);
//    fprintf(file, "NumOfSlicesEyesCut %d \n", Eyes);

    for(int i=0; i<NumOfSlices; ++i)         //saved BY SLICES!!
    {
        for(int k=0; k<ns; ++k)
        {
            fprintf(file, "%lf\n", data3[k][i]);
        }
    }
    fclose(file);

//    for(int i=0; i<ns; ++i)
//    {
//        delete []data3[i];
//    }
//    delete []data3;

    fileName.replace('.', '_');

    if(ns==19)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("after").append(QDir::separator()).append(fileName).append(".jpg"));
    }
    if(ns==21)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append("before").append(QDir::separator()).append(fileName).append(".jpg"));
    }
//         helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append(fileName).append(".jpg"));
    currentPic.save(helpString, 0 ,100);
}


void Cut::rewrite()
{
    dir->setPath(currentFile);  //.../expName/Realisations/fileName;
    fileName=dir->dirName();    // real fileName
    dir->cdUp();
    dir->cdUp();
    //ExpName dir

    helpString = currentFile;

    cout<<helpString.toStdString()<<endl;

    file = fopen(helpString.toStdString().c_str(), "w");
    fprintf(file, "NumOfSlices %d \n", NumOfSlices);
//    fprintf(file, "NumOfSlicesEyesCut %d \n", Eyes);

    for(int i=0; i<NumOfSlices; ++i)         //saved BY SLICES!!
    {
        for(int k=0; k<ns; ++k)
        {
            fprintf(file, "%lf\n", data3[k][i]);
        }
    }
    fclose(file);

//    for(int i=0; i<ns; ++i)
//    {
//        delete []data3[i];
//    }
//    delete []data3;

    fileName.replace('.', '_');

    if(ns==19)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Signals").append(QDir::separator()).append("after").append(QDir::separator()).append(fileName).append(".jpg"));
    }
    if(ns==21)
    {
        helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Signals").append(QDir::separator()).append("before").append(QDir::separator()).append(fileName).append(".jpg"));
    }
//         helpString=QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("SignalsCut").append(QDir::separator()).append(fileName).append(".jpg"));
    currentPic.save(helpString, 0 ,100);

}

void Cut::paint()
{
    if(!painter->isActive()) {return;}
    painter->end();
    currentPic = QPixmap(NumOfSlices, 600); //mainwindow.cpp MainWindow::drawEeg generality
    currentPic.fill();
    painter->begin(&currentPic);

    painter->setPen(QPen(QBrush("black"), 1));

    double norm=1.0;
    double stretch=1.0;
    for(int c1=0; c1<currentPic.width(); ++c1)
    {
        for(int c2=0; c2<ns; ++c2)
        {
            if(ns==21 && c2==19) painter->setPen("red");
            else if(ns==21 && c2==20) painter->setPen("blue");
            else painter->setPen("black");

            painter->drawLine(c1, (c2+1)*currentPic.height()/(ns+2) +data3[c2][int(c1*stretch)]/norm, c1+1, (c2+1)*currentPic.height()/(ns+2) +data3[c2][int((c1+1)*stretch)]/norm);
        }
    }
    painter->setPen("black");
    for(int c3=0; c3<150; ++c3)
    {
        if(c3%5==0) norm=15.;
        if(c3%10==0) norm=20.;

        painter->drawLine(c3*50/stretch, currentPic.height()-2, c3*50/stretch, currentPic.height()-2*norm);
        int helpInt=c3;
        helpString.setNum(helpInt);
        painter->drawText(c3*250/stretch, currentPic.height()-35, helpString);
        norm=10.;
    }
    norm=1.;
    helpString = dir->absolutePath().append(QDir::separator()).append("tmp.jpg");
    currentPicPath = helpString;
    currentPic.save(helpString, 0, 100);  //generality path

    ui->picLabel->setPixmap(currentPic.scaled(ui->picLabel->size()));


    rightLimit=ui->picLabel->width();
    leftLimit=0;
}

