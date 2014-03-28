#include "makepa.h"
#include "ui_makepa.h"

MakePa::MakePa(QString spectraPath, QString ExpName_, int ns_, int left_, int right_, double spStep_) :
    ui(new Ui::MakePa)
{

    ui->setupUi(this);
    browser = new QFileDialog();

    //left & right for drawing only
    left = left_;
    right = right_;
    spLength = right_ - left_ + 1;
    spStep = spStep_;
    ExpName = ExpName_;

    ns=ns_;



    this->setWindowTitle("Make PA");

//    helpString.setNum(spLength);
//    ui->lineEdit_5->setText(helpString);
    ui->spLBox->setMaximum(1000);
    ui->spLBox->setMinimum(1);
    ui->spLBox->setValue(spLength);

    ui->nsBox->setMaximum(300);
    ui->nsBox->setMinimum(1);
    ui->nsBox->setValue(ns);

    ui->lineEdit_1->setText("_241");
    ui->lineEdit_2->setText("_247");
    ui->lineEdit_3->setText("_254");
    //generality
//    ui->lineEdit_4->setText("8"); //sqrt
//    ui->lineEdit_4->setText("20"); //no sqrt
    ui->rdcCoeffBox->setValue(20);


    helpString = QDir::toNativeSeparators(spectraPath);
    ui->paLineEdit->setText(helpString);


    ui->alphaSpinBox->setSingleStep(0.01);
    ui->alphaSpinBox->setMaximum(1.0);
    ui->alphaSpinBox->setValue(0.95); // p=0.05
    ui->mwNormSpinBox->setValue(20.);
    ui->mwNormSpinBox->setSingleStep(0.5);


    dir = new QDir();
    dir->cd(spectraPath);
    dir->cdUp();
    if(spStep == 250./1024.) dir->cdUp(); //generality

    browser->setDirectory(QDir::toNativeSeparators(dir->absolutePath()));

    helpCharArr = new char [64];

//    browser->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    browser->setWindowTitle("Choose spectra directory");
    ui->cl3_Button->setChecked(true);
    QObject::connect(ui->browseButton, SIGNAL(clicked()), browser, SLOT(show()));
    QObject::connect(browser, SIGNAL(directoryEntered(QString)), ui->paLineEdit, SLOT(setText(QString)));
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(makePaSlot()));
    QObject::connect(ui->spLBox, SIGNAL(valueChanged(int)), this, SLOT(setSpLength()));
    QObject::connect(ui->nsBox, SIGNAL(valueChanged(int)), this, SLOT(setNs()));
    QObject::connect(ui->mwTestButton, SIGNAL(clicked()), this, SLOT(mwTest()));
    QObject::connect(ui->kwTestButton, SIGNAL(clicked()), this, SLOT(kwTest()));
    QObject::connect(ui->dispersionAnalysisButton, SIGNAL(clicked()), this, SLOT(dispersionAnalysis()));
    QObject::connect(ui->vdvTestButton, SIGNAL(clicked()), this, SLOT(correlationDifference()));
    this->setAttribute(Qt::WA_DeleteOnClose);
}

MakePa::~MakePa()
{
    delete ui;
    delete dir;
    delete browser;
}

void MakePa::setNumOfClasses(int a)
{
    if(a==2)
    {
        ui->cl2_Button->setChecked(true);
        return;
    }
    if(a==3)
    {
        ui->cl3_Button->setChecked(true);
        return;
    }
    return;
}

double quantile(double arg)
{
    double a, b;
    a=exp(0.14*log(arg));
    b=exp(0.14*log(1-arg));
    return (4.91*(a-b));
}

void MakePa::setSpLength()
{
    spLength = ui->spLBox->value(); //bad bicycle
}

void MakePa::setNs()
{
    ns = ui->nsBox->value(); //bad bicycle
}

void MakePa::mwTest()
{
    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->paLineEdit->text());          /////////which dir?
    helpString=dir_->absolutePath();

    QStringList nameFilters, list, lst[2]; //0 - Spatial, 1 - Verbal
    FILE * currFile;

    //make lists of files
    //Spatial
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_1->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[0]=dir_->entryList(nameFilters, QDir::Files);

    //Verbal
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_2->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[1]=dir_->entryList(nameFilters, QDir::Files);


    int * n = new int[2];
    n[0]=lst[0].length();
    n[1]=lst[1].length();

    double *** spectre = new double ** [2];
    for(int i=0; i<2; ++i)
    {
        spectre[i] = new double * [n[i]];
        for(int j=0; j<n[i]; ++j)
        {
            spectre[i][j] = new double [ns*spLength];
        }
    }

    //read the spectra into matrixes
    for(int j=0; j<2; ++j)
    {
        for(int i=0; i<n[j]; ++i)
        {
            helpString=dir_->absolutePath().append(QDir::separator()).append(lst[j][i]);
            currFile = fopen(helpString.toStdString().c_str(), "r");
            for(int k=0; k<ns*spLength; ++k)
            {
                fscanf(currFile, "%lf", &spectre[j][i][k]);
            }
            fclose(currFile);
        }
    }

    double ** array = new double *[2];
    for(int i=0; i<2; ++i)
    {
        array[i] = new double [n[0]+n[1]];
    }

    double temp;
    double sum0;
    int sumAll;
    double average = n[0]*n[1]/2.;
    double dispersion = sqrt(n[0]*n[1]*(n[0]+n[1])/12.);
    int numOfDiff = 0;

    double *U = new double [ns*spLength];

    int numOfZero = 0;

//    cout<<n[0]<<"\t"<<n[1]<<endl;
//    for(int i = 0; i < n[0]; ++i)
//    {
//        cout<<lst[0][i].toStdString()<<endl;
//    }
//    for(int i = 0; i < n[1]; ++i)
//    {
//        cout<<lst[1][i].toStdString()<<endl;
//    }

    //for every spectra-bin
    for(int j=0; j<ns*spLength; ++j)
    {
//        cout<<"Start"<<endl;

        //fill the arrays for sort
        for(int i=0; i<n[0]; ++i)
        {
            array[0][i] = spectre[0][i][j];
            array[1][i] = 0.;
        }
        for(int i=n[0]; i<n[0] + n[1]; ++i)
        {
            array[0][i] = 0.;
            array[1][i] = spectre[1][i - n[0]][j];
        }

        //for zeroed bins
        sum0 = 0;
        for(int i = 0; i < n[0] +n [1]; ++i)
        {
            sum0 += max(array[0][i], array[1][i]);
        }
        if(sum0 == 0.)
        {
            U[j] = average;
            numOfZero +=1;
            cout<<j<<"'th zero"<<endl;
            continue;
        }


        //sort all
        for(int k=0; k<n[0] + n[1]; ++k)
        {
            for(int i=0; i<n[0] + n[1] - 1; ++i)
            {
                if( max( array[0][i], array[1][i]) > max( array[0][i+1], array[1][i+1]))
                {
                    for(int j=0; j<2; ++j)
                    {
                        temp=array[j][i];
                        array[j][i]=array[j][i+1];
                        array[j][i+1]=temp;
                    }
                }
            }
        }

//        cout<<"second array sorted"<<endl;

        //count sums

        sum0 = 0;
        for(int i=0; i<n[0] + n[1]; ++i)
        {
            if(array[0][i]!=0) sum0 += (i+1);
        }

        //if sum0 is bigger

        sumAll = (n[0]+n[1])*(n[0]+n[1]+1)/2;
        if(sum0 > sumAll/2 )
        {
            U[j] = double(n[0]*n[1] + n[0]*(n[0]+1)/2. - sum0);
        }
        else
        {
            U[j] = double(n[1]*n[0] + n[1]*(n[1]+1)/2. - double(sumAll - sum0));
        }

//        if(j%200==0) cout<<"j="<<j<<"  U="<<U[j]<<endl;

        if(fabs((U[j]-average)/double(dispersion))>quantile((1.00+ui->alphaSpinBox->value())/2.)) numOfDiff+=1;
    }
    cout<<"num of different freq-bins "<<numOfDiff<<endl;

    //automatization
    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "U-test\t%d\n", numOfDiff);
    fprintf(res, "U-test part\t%lf\n", double(numOfDiff/double(ns*spLength)));
    fprintf(res, "U-test part zeroed\t%lf\n", double(numOfDiff/double(ns*spLength - numOfZero)));
    fclose(res);

    //paint
    int * spL = new int [2];
    for(int i=0; i<2; ++i)
    {
        spL[i] = spLength;
    }

    double *** sp = new double **[2];
    for(int i=0; i<2; ++i)
    {
        sp[i] = new double * [ns];
        for(int j=0; j<ns; ++j)
        {
            sp[i][j] = new double [spL[i]];
        }
    }

    //create sp1 & sp2 - average spectra
    for(int i=0; i<ns; ++i)
    {
        for(int j=0; j<spLength; ++j)
        {
            for(int h=0; h<2; ++h)
            {

                sp[h][i][j]=0.;
                for(int k=0; k<n[h]; ++k)
                {
                    sp[h][i][j]+=spectre[h][k][i*spL[h]+j];
                }
                sp[h][i][j]/=n[h];
            }
        }
    }


//    cout<<"data read"<<endl;


    QPixmap pic(1600,1600);
//    cout<<"before QPainter"<<endl;
    QPainter *paint = new QPainter;
//    cout<<"after QPainter"<<endl;
    pic.fill();
    paint->begin(&pic);


    double norm = ui->mwNormSpinBox->value(); //10 pixels=1mV^2 / Hz
    double barWidth=1/2.;
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {

        //average spectra drawing
        for(int k=0; k<250-1; ++k)
        {
            for(int h=0; h<2; ++h)
            {
                if(h==0) paint->setPen(QPen(QBrush("blue"), 2));
                if(h==1) paint->setPen(QPen(QBrush("red"), 2));
                if(h==2) paint->setPen(QPen(QBrush("green"), 2));
                paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - sp[h][c2][int(k*spL[h]/250.)]*norm, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - sp[h][c2][int((k+1)*spL[h]/250.)]*norm);
            }
        }

        paint->setPen("black");
        paint->setBrush(QBrush("black"));

        //statistic difference bars
        for(int j=c2*spLength; j<(c2+1)*spLength; ++j)
        {
            for(int h=0; h<2; ++h)
            {
                if(h==0)
                {
                    paint->setPen("blue");
                    paint->setBrush(QBrush("blue"));
                }
                if(h==1)
                {
                    paint->setPen("red");
                    paint->setBrush(QBrush("red"));
                }
                if((fabs((U[j]-average)/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) && ((sp[h][c2][j%spLength] == max(sp[0][c2][j%spLength], sp[1][c2][j%spLength]))))
                {
                    paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*spLength))*(250./spLength), paint->device()->height() * coords::y[c2] + 15 + 7*h , 2*barWidth*(250./spLength), 5); //-15 graphical 1/3 graphical
                }
            }

        }
        paint->setPen("black");

        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);

        paint->setFont(QFont("Helvitica", 8));

        //Hz markers
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

    //channel labels
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText((paint->device()->width() * coords::x[c2]-20), (paint->device()->height() * coords::y[c2]-252), QString(coords::lbl[c2]));
    }

    //draw scale
    //draw scale
    paint->drawLine(QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1]), QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())));  //250 - graph height generality

    //returning norm = max magnitude

    norm = (coords::scale * paint->device()->height()) / norm;
    norm = int(norm*10)/10.;
    helpString.setNum(norm);
    helpString.append(tr(" mcV^2/Hz"));
    paint->drawText(QPointF(paint->device()->width() * coords::x[6]+5., paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())/2.), helpString);



    //save
    helpString = dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("Mann-Whitney").append(ui->addNameLineEdit->text()).append(".jpg");
    cout<<helpString.toStdString()<<endl;
    pic.save(helpString, 0, 100);
    paint->end();


    for(int h=0; h<2; ++h)
    {
        for(int i=0; i<ns; ++i)
        {
            delete []sp[h][i];
        }
        delete []sp[h];
    }
    delete []sp;


    for(int i=0; i<2; ++i)
    {
        delete []array[i];
    }
    delete []array;


    for(int h=0; h<2; ++h)
    {
        for(int i=0; i<n[h]; ++i)
        {
            delete []spectre[h][i];
        }
        delete []spectre[h];
    }

    delete []U;
    delete dir_;
    delete paint;

    helpString = "MW test made";
    ui->mwTestLine->setText(helpString);
}
/*
void MakePa::vdvTest()
{
    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->paLineEdit->text());          /////////which dir?
    helpString=dir_->absolutePath();

    QStringList nameFilters, list, lst[2]; //0 - Spatial, 1 - Verbal
    FILE * currFile;

    //make lists of files
    //Spatial
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_1->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[0]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);

    //Verbal
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_2->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[1]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);


    int n[0], n[1];
    n[0]=lst[0].length();
    n[1]=lst[1].length();

    double ** spectre[0] = new double *[n[0]];
    for(int i=0; i<n[0]; ++i)
    {
        spectre[0][i] = new double [ns*spLength];
    }
    double ** spectre[1] = new double *[n[1]];
    for(int i=0; i<n[1]; ++i)
    {
        spectre[1][i] = new double [ns*spLength];
    }

    //read the spectra into matrixes
    for(int i=0; i<n[0]; ++i)
    {
        helpString=dir_->absolutePath().append(QDir::separator()).append(lst[0].at(i));
        currFile = fopen(helpString.toStdString().c_str(), "r");
        for(int j=0; j<ns*spLength; ++j)
        {
            fscanf(currFile, "%lf", &spectre[0][i][j]);
        }
        fclose(currFile);
    }

    for(int i=0; i<n[1]; ++i)
    {
        helpString=dir_->absolutePath().append(QDir::separator()).append(lst[1].at(i));
        currFile = fopen(helpString.toStdString().c_str(), "r");
        for(int j=0; j<ns*spLength; ++j)
        {
            fscanf(currFile, "%lf", &spectre[1][i][j]);
        }
        fclose(currFile);
    }

    double ** array = new double *[2];
    for(int i=0; i<2; ++i)
    {
        array[i] = new double[n[0] + n[1]];
    }

    double temp;
    double dispersion = 0.;
    int numOfDiff=0;

    double *X = new double[ns*spLength];


    //set & sort the arrays

    for(int j=0; j<ns*spLength; ++j)
    {
        for(int i=0; i<n[0]; ++i)
        {
            array[0][i] = spectre[0][i][j];
            array[1][i] = 0.;
        }
        for(int i=n[0]; i<n[0] + n[1]; ++i)
        {
            array[1][i] = spectre[1][i - n[0]][j];  ///ololo
            array[0][i] = 0.;
        }

        //sort first array bubbles
        for(int i=0; i<n[0] - 1; ++i)
        {
            for(int k=0; k<n[0] - 1; ++k)
            {
                if(array[0][k]>array[0][k+1])
                {
                    temp=array[0][k];
                    array[0][k]=array[0][k+1];
                    array[0][k+1]=temp;
                }
            }
        }

        //sort second
        for(int i=n[0]; i<n[0] + n[1]; ++i)
        {
            for(int j=0; j<n[0] - 1; ++j)
            {
//                if(array[1][i]==array[0][j] && (array[1][i]!=0)) {cout<<"equality!!! [0]["<<j<<"] and [1]["<<i<<"]"<<endl; }  //exception

                if(array[1][i]>array[0][j] && array[1][i]<array[0][j+1])
                {
                    array[1][j+1] = array[1][i];
                    array[1][i] = 0.;
                    for(int k=i; k>j+1; --k)     ///?????
                    {
                        array[0][k] = array[0][k-1];
                    }
                    array[0][j+1] = 0.;
                }
            }
        }

        X[j]=0.;
        //count the statistic
        for(int i=0; i<n[0] + n[1]; ++i)
        {
            if(array[0][i]!=0) X[j]+=quantile((i+1)/double(n[0]+n[1]+1));
        }

        cout<<"j="<<j<<"  X="<<X[j]<<endl;
        dispersion=0.;
        for(int i=1; i<=n[0] + n[1]; ++i)
        {
            dispersion+=quantile(i/double(n[0]+n[1]+1))*quantile(i/double(n[0]+n[1]+1));
        }
        dispersion*=n[0]*n[1]/double((n[0]+n[1])*(n[0]+n[1]-1));
        dispersion = sqrt(dispersion);

        if(abs(X[j]/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) numOfDiff+=1;

    }
    cout<<"num of different freq-bins "<<numOfDiff<<endl;




    //paint
    int spL1=spLength, spL2=spLength;


    double **sp1 = new double* [ns];
    for(int i=0; i<ns; ++i)
    {
        sp1[i] = new double [spL1];
    }
    double **sp2 = new double* [ns];
    for(int i=0; i<ns; ++i)
    {
        sp2[i] = new double [spL2];
    }

    //create sp1 & sp2
    for(int i=0; i<ns; ++i)
    {
        for(int j=0; j<spLength; ++j)
        {
            sp1[i][j]=0.;
            for(int k=0; k<n[0]; ++k)
            {
                sp1[i][j]+=spectre[0][k][i*spLength+j];
            }
            sp1[i][j]/=n[0];

            sp2[i][j]=0.;
            for(int k=0; k<n[1]; ++k)
            {
                sp2[i][j]+=spectre[1][k][i*spLength+j];
            }
            sp2[i][j]/=n[1];
        }
    }

    for(int i=0; i<n[0]; ++i)
    {
        delete []spectre[0][i];
    }
    for(int i=0; i<n[1]; ++i)
    {
        delete []spectre[1][i];
    }
    delete []spectre[0];
    delete []spectre[1];


    cout<<"data read"<<endl;

    QPixmap pic(1600,1600);
    QPainter *paint = new QPainter;
    pic.fill();
    paint->begin(&pic);

    double norm=50; //10 pixels=1mV^2 / Hz
    double barWidth=1/2.;
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        for(int k=0; k<250-1; ++k)
        {
            paint->setPen("blue");
            paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - sp1[c2][int(k*spL1/250.)]*norm, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - sp1[c2][int((k+1)*spL1/250.)]*norm);
            paint->setPen("red");
            paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - sp2[c2][int(k*spL2/250.)]*norm, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - sp2[c2][int((k+1)*spL2/250.)]*norm);
        }

        paint->setPen("black");
        paint->setBrush(QBrush("black"));
        for(int j=c2*spLength; j<(c2+1)*spLength; ++j)
        {
            if((abs(X[j]/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) && (sp1[c2][j%spLength] < sp2[c2][j%spLength]))
            {

                paint->setPen("red");
                paint->setBrush(QBrush("red"));
//                paint->drawLine(paint->device()->width() * coords::x[c2]+ (j-c2*spLength)-barWidth*(250/spLength), paint->device()->height() * coords::y[c2] -15 , paint->device()->width() * coords::x[c2] + (j-c2*spLength) + barWidth*(250/spLength), paint->device()->height() * coords::y[c2]+10); //-15 graphical 1/3 graphical
                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*spLength))*(250./spLength), paint->device()->height() * coords::y[c2] + 15 , 2*barWidth*(250./spLength), 5); //-15 graphical 1/3 graphical
            }
            if((abs(X[j]/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) && (sp1[c2][j%spLength] > sp2[c2][j%spLength]))
            {

                paint->setPen("blue");
                paint->setBrush(QBrush("blue"));
//                paint->drawLine(paint->device()->width() * coords::x[c2]+ (j-c2*spLength)-barWidth*(250/spLength), paint->device()->height() * coords::y[c2] -15 , paint->device()->width() * coords::x[c2] + (j-c2*spLength) + barWidth*(250/spLength), paint->device()->height() * coords::y[c2]+10); //-15 graphical 1/3 graphical
                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*spLength))*(250./spLength), paint->device()->height() * coords::y[c2] + 15 + 7 , 2*barWidth*(250./spLength), 5); //-15 graphical 1/3 graphical
            }

        }

        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);
        paint->setPen("black");

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
    helpString = dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("Van der Varden").append(ui->addNameLineEdit->text()).append(".jpg");
    cout<<helpString.toStdString()<<endl;

    pic.save(helpString, 0, 100);
    paint->end();


    for(int i=0; i<ns; ++i)
    {
        delete []sp1[i];
        delete []sp2[i];
    }
    delete []sp1;
    delete []sp2;

    for(int i=0; i<2; ++i)
    {
        delete []array[i];
    }
    delete []array;

    delete []X;
    delete dir_;
    delete paint;

    helpString = "VdV test made";
    ui->mwTestLine->setText(helpString);
}
*/

void MakePa::kwTest()
{

    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->paLineEdit->text());          /////////which dir?
    helpString=dir_->absolutePath();

    QStringList nameFilters, list, lst[3]; //0 - Spatial, 1 - Verbal, 2- Idle
    FILE * currFile;

    //make lists of files
    //Spatial
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_1->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[0]=dir_->entryList(nameFilters, QDir::Files);

    //Verbal
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_2->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[1]=dir_->entryList(nameFilters, QDir::Files);

    //Idle
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_3->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[2]=dir_->entryList(nameFilters, QDir::Files);


    //n - number of files every type
    int * n = new int [3];
    for(int i=0; i<3; ++i)
    {
        n[i] = lst[i].length();
    }
    int N = n[0]+n[1]+n[2];

    //array for all spectra
    double *** spectre = new double ** [3]; //3 classes
    for(int i=0; i<3; ++i)
    {
        spectre[i] = new double * [n[i]]; //n[i] files
        for(int j=0; j<n[i]; ++j)
        {
            spectre[i][j] = new double [ns*spLength]; // "NetLength"
        }
    }


    //read the spectra into matrixes
    for(int j=0; j<3; ++j)
    {
        for(int i=0; i<n[j]; ++i)
        {
            helpString=dir_->absolutePath().append(QDir::separator()).append(lst[j][i]);
            currFile = fopen(helpString.toStdString().c_str(), "r");
            for(int k=0; k<ns*spLength; ++k)
            {
                fscanf(currFile, "%lf", &spectre[j][i][k]);
            }
            fclose(currFile);
        }
    }

    double ** array = new double *[3];
    for(int i=0; i<3; ++i)
    {
        array[i] = new double [N];
    }

    double temp;
    int numOfDiff=0;


    double * R = new double [3]; //sum of all ranks
//    double Rall;

    double *H = new double [ns*spLength];

    cout<<"N="<<N<<endl;


    //for every spectra-bin
    for(int j=0; j<ns*spLength; ++j)
    {
//        Rall=0.;
        H[j]=0.;
//        cout<<"Start"<<endl;

        //fill the arrays for sort
        for(int i=0; i<n[0]; ++i)
        {
            array[0][i] = spectre[0][i][j];
            array[1][i] = 0.;
            array[2][i] = 0.;
        }
        for(int i=n[0]; i<n[0] + n[1]; ++i)
        {
            array[0][i] = 0.;
            array[1][i] = spectre[1][i - n[0]][j];
            array[2][i] = 0.;
        }

        for(int i= n[0] + n[1]; i< n[0] + n[1] + n[2]; ++i)
        {
            array[0][i] = 0.;
            array[1][i] = 0.;
            array[2][i] = spectre[2][i - (n[0] + n[1])][j];;
        }

        //sort all
        for(int k=0; k<N; ++k)
        {
            for(int i=0; i<N - 1; ++i)
            {
                if( max( max(array[0][i], array[1][i]), array[2][i]) > max( max(array[0][i+1], array[1][i+1]), array[2][i+1]) )
                {
                    for(int j=0; j<3; ++j)
                    {
                        temp=array[j][i];
                        array[j][i]=array[j][i+1];
                        array[j][i+1]=temp;
                    }
                }
            }
        }

        for(int k=0; k<3; ++k)
        {
            R[k]=0.;

        }
        //count R[i] - sum of all ranks i'th pack
        for(int i=0; i<N; ++i)
        {
            for(int k=0; k<3; ++k)
            {
                if(array[k][i]!=0) R[k]+=(i+1);
            }
        }
//        cout<<j<<endl;S
//        cout<<"R[0]="<<R[0]<<"  R[1]="<<R[1]<<"  R[2]="<<R[2]<<endl;   //" sumR="<<R[0]+R[1]+R[2]<<endl;
//        for(int i=0; i<N; ++i)
//        {
//            for(int k=0; k<3; ++k)
//            {
//                if(array[k][i]!=0) Rall+=(i+1);
//            }
//        }
//        cout<<"Rall="<<Rall<<endl;
//        Rall=0.;
//        for(int i=0; i<N; ++i)
//        {
//            Rall+=(i+1);
//        }
//        cout<<"Rall="<<Rall<<endl;


        //count statistic
        for(int i=0; i<3; ++i) //
        {
            H[j]+=R[i]*R[i]/double(n[i]);
        }
        H[j]*=(12./(N*(N+1)));
        H[j]-=3*(N+1);
//        cout<<"H["<<j<<"]="<<H[j]<<endl<<endl;
        if(H[j]>=5.9915) numOfDiff+=1; //from table for 3 classes, 0,95





//        if(abs((U[j]-average)/double(dispersion))>quantile((1.00+ui->alphaSpinBox->value())/2.)) numOfDiff+=1;
    }
    cout<<"num of different freq-bins "<<numOfDiff<<endl;

    //automatization
    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "U-test\t%d\n", numOfDiff);
    fclose(res);



    //paint
    int * spL = new int [3];
    for(int i=0; i<3; ++i)
    {
        spL[i] = spLength;
    }

    double *** sp = new double **[3];
    for(int i=0; i<3; ++i)
    {
        sp[i] = new double * [ns];
        for(int j=0; j<ns; ++j)
        {
            sp[i][j] = new double [spL[i]];
        }
    }

    //create sp1 & sp2 - average spectra
    for(int i=0; i<ns; ++i)
    {
        for(int j=0; j<spLength; ++j)
        {
            for(int h=0; h<3; ++h)
            {

                sp[h][i][j]=0.;
                for(int k=0; k<n[h]; ++k)
                {
                    sp[h][i][j]+=spectre[h][k][i*spL[h]+j];
                }
                sp[h][i][j]/=n[h];
            }


        }
    }


//    cout<<"data read"<<endl;


    QPixmap pic(1600,1600);
//    cout<<"before QPainter"<<endl;
    QPainter *paint = new QPainter;
//    cout<<"after QPainter"<<endl;
    pic.fill();
    paint->begin(&pic);


    double norm=20; //10 pixels=1mV^2 / Hz
    double barWidth=1/2.;

    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        for(int k=0; k<250-1; ++k)
        {
            for(int h=0; h<3; ++h)
            {
                if(h==0) paint->setPen(QPen(QBrush("blue"), 2));
                if(h==1) paint->setPen(QPen(QBrush("red"), 2));
                if(h==2) paint->setPen(QPen(QBrush("green"), 2));
                paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - sp[h][c2][int(k*spL[h]/250.)]*norm, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - sp[h][c2][int((k+1)*spL[h]/250.)]*norm);
            }
        }

        paint->setPen("black");
        paint->setBrush(QBrush("black"));

        //statistic difference
        for(int j=c2*spLength; j<(c2+1)*spLength; ++j)
        {
            if(H[j] >= 5.9915)
            {
                paint->setPen("black");
                paint->setBrush(QBrush("black"));
                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*spLength))*(250./spLength), paint->device()->height() * coords::y[c2] + 15 , 2*barWidth*(250./spLength), 5); //-15 graphical 1/3 graphical
            }
//            if((abs((U[j]-average)/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) && ((sp1[c2][j%spLength] > sp2[c2][j%spLength])))
//            {

//                paint->setPen("blue");
//                paint->setBrush(QBrush("blue"));
//                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*spLength))*(250./spLength), paint->device()->height() * coords::y[c2] + 15 + 7 , 2*barWidth*(250./spLength), 5); //-15 graphical 1/3 graphical
//            }

        }
        paint->setPen("black");

        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);



        paint->setFont(QFont("Helvitica", 8));
        //Hz markers
        for(int k=0; k<250-1; ++k)
        {
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

    //print channel labels
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText((paint->device()->width() * coords::x[c2]-20), (paint->device()->height() * coords::y[c2]-252), QString(coords::lbl[c2]));
    }



    //save the picture
    helpString = dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("Kruscal-Wallis").append(ui->addNameLineEdit->text()).append(".jpg");
    cout<<helpString.toStdString()<<endl;
    pic.save(helpString, 0, 100);
    paint->end();

    for(int h=0; h<3; ++h)
    {
        for(int i=0; i<ns; ++i)
        {
            delete []sp[h][i];
        }
        delete []sp[h];
    }
    delete []sp;


    for(int i=0; i<3; ++i)
    {
        delete []array[i];
    }
    delete []array;


    for(int h=0; h<3; ++h)
    {
        for(int i=0; i<n[h]; ++i)
        {
            delete []spectre[h][i];
        }
        delete []spectre[h];
    }
    delete []spectre;

    delete []R;
    delete []H;
    delete dir_;
    delete paint;

    helpString = "KW test made";
    ui->mwTestLine->setText(helpString);
}



void MakePa::makePaSlot()
{
    QString typeString;

    if(ui->paLineEdit->text().contains("windows", Qt::CaseInsensitive))
    {
        typeString = "_wnd.pa";
    }
    else if(ui->paLineEdit->text().contains("PCA", Qt::CaseInsensitive))
    {
        typeString = "_pca.pa";
    }
    else
    {
        typeString = ".pa";
    }
    cout << "makePa typeString = " << typeString.toStdString() << endl;

    ui->lineEdit->clear();
    double coeff = ui->rdcCoeffBox->value();



    if(ui->cl3_Button->isChecked()) NumOfClasses=3;    ///////////////generality
    if(ui->cl2_Button->isChecked()) NumOfClasses=2;

    QDir *dir_ = new QDir();
    dir_->cd(ui->paLineEdit->text());          /////////which dir?
    helpString=dir_->absolutePath();
//    cout<<"spectra path = "<<helpString.toStdString()<<endl;


    FILE * spectre;

    FILE * pa;
    FILE * svm;
    dir_->setSorting(QDir::Name);


    //generality
    QStringList nameFilters, list, lst[3]; //0 - Spatial, 1 - Verbal, 2 - Gaps

    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_1->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }

//    lst[0]=dir_->entryList(nameFilters, QDir::Files, QDir::Size);
    lst[0]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);

    nameFilters.clear();

    list.clear();
    list = ui->lineEdit_2->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }

//    lst[1]=dir_->entryList(nameFilters, QDir::Files, QDir::Size);
    lst[1]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);

    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_3->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
//    lst[2]=dir_->entryList(nameFilters, QDir::Files, QDir::Size);
    lst[2]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);



    //all.pa
    list.clear();
    nameFilters.clear();
    helpString.clear();
    helpString.append(ui->lineEdit_1->text()).append(" ").append(ui->lineEdit_2->text()).append(" ").append(ui->lineEdit_3->text()).append(" ");

    list = helpString.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }

    list=dir_->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);

    if(list.empty())
    {
        cout<<"no spectra-files found"<<endl;
        return;
    }

//    for(int i = 0; i < 3; ++i)
//    {
//        cout<<"lst["<<i<<"].length() = "<<lst[i].length()<<endl;
//    }


    int Length = lst[0].length();

    for(int i=0; i<NumOfClasses; ++i)
    {
//        cout<<"Length["<<i<<"]="<<lst[i].length()<<endl;
        Length=min(Length, lst[i].length());
    }

    cout<<"Length="<<Length<<endl;  //get the maximum quantity of files belong to one type
    if(Length==0)
    {
        cout<<"Length==0"<<endl; return;
    }

    for(int i=0; i<NumOfClasses; ++i)
    {
        while(lst[i].length()>Length) {lst[i].removeAt(rand()%(lst[i].length()));} //fit all lists to appropriate number of files
    }

    int **arr = new int* [NumOfClasses];
    for(int i=0; i<NumOfClasses; ++i)
    {
        arr[i] = new int [Length];
    }
    for(int i=0; i<NumOfClasses; ++i)
    {
        for(int j=0; j<Length; ++j)
        {
            arr[i][j]=j;
        }
    }

//    cout<<"ns="<<ns<<endl;
    //generality
    if(ns==-1) return;
    if(spLength==-1) return;

    double ** data4 = new double * [ns];
    for(int i=0; i<ns; ++i)
    {
        data4[i] = new double [spLength];
    }

    /////////////////mix lst///////////////////////
    srand(QTime::currentTime().msec());

    int a1,a2, tmp;

    //mix list
    for(int j=0; j<NumOfClasses; ++j)
    {
        for(int i=0; i<15*Length; ++i)
        {
            a1=rand()%Length;
            a2=rand()%Length;
            tmp=arr[j][a1];
            arr[j][a1]=arr[j][a2];
            arr[j][a2]=tmp;
        }
    }

    //all.pa

    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("all").append(typeString));


    FILE * paAll=fopen(helpString.toStdString().c_str(), "w");   ////////separator
    if(paAll==NULL)
    {
        cout<<helpString.toStdString().c_str() << endl << " all.pa==NULL" << endl;
        return;
    }
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("svmAll"));
    FILE * svmAll = fopen(helpString.toStdString().c_str(), "w");
    if(svmAll==NULL)
    {
        cout<<helpString.toStdString().c_str()<<endl<<" svmAll==NULL"<<endl;
        return;
    }

    //create first PA
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("1").append(typeString));
    pa=fopen(helpString.toStdString().c_str(), "w");   ////////separator
    if(pa == NULL)
    {
        cout<<helpString.toStdString().c_str()<<endl<<"1.pa==NULL"<<endl;
        return;
    }
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("svm1"));
    svm = fopen(helpString.toStdString().c_str(), "w");
    if(svm==NULL)
    {
        cout<<helpString.toStdString().c_str()<<endl<<" svm1==NULL"<<endl;
        return;
    }


    for(int i=0; i<Length/2; ++i)  //Length - number of files every type
    {
        for(int j=0; j<NumOfClasses; ++j)
        {

            helpString=dir_->absolutePath();
            helpString.append(QDir::separator()).append(lst[j].at(arr[j][i]));
//            cout<<helpString.toStdString()<<endl;

            spectre=fopen(helpString.toStdString().c_str(), "r");
            fprintf(pa, "%s\n", lst[j].at(arr[j][i]).toStdString().c_str());
            fprintf(paAll, "%s\n", lst[j].at(arr[j][i]).toStdString().c_str());
            if(spectre == NULL) return;

            for(int l=0; l<ns; ++l)
            {
                for(int k=0; k<spLength; ++k)
                {
                    fscanf(spectre, "%lf\n", &data4[l][k]);
                }
                fscanf(spectre, "\n");
            }
            fclose(spectre);
            fprintf(svm, "%d ", j);

            for(int l=0; l<ns; ++l)
            {
                for(int k=0; k<spLength; ++k)
                {
                    fprintf(svm, "%d:%lf ", int(l*spLength+k+1), data4[l][k]/coeff);


                    fprintf(pa, "  %lf ", data4[l][k]/coeff);
                    fprintf(paAll, "  %lf ", data4[l][k]/coeff);
                    if(k%10==9)
                    {
                        fprintf(pa, "\n");
                        fprintf(paAll, "\n");
                    }
                }
                fprintf(pa, "\n");
                fprintf(paAll, "\n");
            }
            fprintf(svm, "\n");
            if(NumOfClasses==3)
                {
                if(j==0)
                {
                    fprintf(pa, "1 0 0 \n\n");
                    fprintf(paAll, "1 0 0 \n\n");
                }
                if(j==1)
                {
                    fprintf(pa, "0 1 0 \n\n");
                    fprintf(paAll, "0 1 0 \n\n");
                }
                if(j==2)
                {
                    fprintf(pa, "0 0 1 \n\n");
                    fprintf(paAll, "0 0 1 \n\n");
                }
            }
            if(NumOfClasses==2)
                {
                if(j==0)
                {
                    fprintf(pa, "1 0 \n\n");
                    fprintf(paAll, "1 0 \n\n");
                }
                if(j==1)
                {
                    fprintf(pa, "0 1 \n\n");
                    fprintf(paAll, "0 1 \n\n");
                }
            }
        }
//        cout<<i<<" ";
    }
    fclose(svm);
    fclose(pa);

//    cout<<endl;

    //create second PA
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("2").append(typeString));
    pa=fopen(helpString.toStdString().c_str(), "w");   ////////separator
    if(pa == NULL)
    {
        cout<<helpString.toStdString().c_str()<<endl<<"2.pa==NULL"<<endl;
        return;
    }
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("svm2"));
    svm = fopen(helpString.toStdString().c_str(), "w");
    if(svm==NULL)
    {
        cout<<helpString.toStdString().c_str()<<endl<<" svm2==NULL"<<endl;
        return;
    }

    for(int i=Length/2; i<(Length/2)*2; ++i)
    {
        for(int j=0; j<NumOfClasses; ++j)
        {

            helpString=dir_->absolutePath();
            helpString.append(QDir::separator()).append(lst[j].at(arr[j][i]));
//            cout<<helpString.toStdString()<<endl;

            spectre=fopen(helpString.toStdString().c_str(), "r");
            fprintf(pa, "%s\n", lst[j].at(arr[j][i]).toStdString().c_str());
            fprintf(paAll, "%s\n", lst[j].at(arr[j][i]).toStdString().c_str());
            if(spectre==NULL) return;

            for(int l=0; l<ns; ++l)
            {
                for(int k=0; k<spLength; ++k)
                {
                    fscanf(spectre, "%lf\n", &data4[l][k]);
                }
                fscanf(spectre, "\n");
            }
            fclose(spectre);
            fprintf(svm, "%d ", j);

            for(int l=0; l<ns; ++l)
            {
                for(int k=0; k<spLength; ++k)
                {
                    fprintf(svm, "%d:%lf ", int(l*spLength+k+1), data4[l][k]/coeff);


                    fprintf(pa, "  %lf ", data4[l][k]/coeff);
                    fprintf(paAll, "  %lf ", data4[l][k]/coeff);
                    if(k%10==9)
                    {
                        fprintf(pa, "\n");
                        fprintf(paAll, "\n");
                    }
                }
                fprintf(pa, "\n");
                fprintf(paAll, "\n");
            }
            fprintf(svm, "\n");
            if(NumOfClasses==3)
                {
                if(j==0)
                {
                    fprintf(pa, "1 0 0 \n\n");
                    fprintf(paAll, "1 0 0 \n\n");
                }
                if(j==1)
                {
                    fprintf(pa, "0 1 0 \n\n");
                    fprintf(paAll, "0 1 0 \n\n");
                }
                if(j==2)
                {
                    fprintf(pa, "0 0 1 \n\n");
                    fprintf(paAll, "0 0 1 \n\n");
                }
            }
            if(NumOfClasses==2)
                {
                if(j==0)
                {
                    fprintf(pa, "1 0 \n\n");
                    fprintf(paAll, "1 0 \n\n");
                }
                if(j==1)
                {
                    fprintf(pa, "0 1 \n\n");
                    fprintf(paAll, "0 1 \n\n");
                }
            }
        }
//        cout<<i<<" ";
    }
    fclose(svm);
    fclose(pa);

    fclose(paAll);




    FILE * paWhole;
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("PA").append(QDir::separator()).append("whole").append(typeString));
    list = dir_->entryList(QDir::Files);
    paWhole = fopen(helpString.toStdString().c_str(), "w");

    for(int i=0; i<list.length(); ++i)
    {
        if(list[i].contains("300")) continue;

            helpString=dir_->absolutePath();
            helpString.append(QDir::separator()).append(list[i]);

            spectre=fopen(helpString.toStdString().c_str(), "r");
            fprintf(paWhole, "%s\n", list[i].toStdString().c_str());
            if(spectre==NULL) return;

            for(int l=0; l<ns; ++l)
            {
                for(int k=0; k<spLength; ++k)
                {
                    fscanf(spectre, "%lf\n", &data4[l][k]);
                }
                fscanf(spectre, "\n");
            }
            fclose(spectre);

            for(int l=0; l<ns; ++l)
            {
                for(int k=0; k<spLength; ++k)
                {
                    fprintf(paWhole, "  %lf ", data4[l][k]/coeff);
                    if(k%10==9)
                    {
                        fprintf(paWhole, "\n");
                    }
                }
                fprintf(paWhole, "\n");
            }

            //generality
            if(NumOfClasses==3)
            {
                if(list[i].contains("_241")) //generality
                {
                    fprintf(paWhole, "1 0 0 \n\n");
                }
                if(list[i].contains("_247")) //generality
                {
                    fprintf(paWhole, "0 1 0 \n\n");
                }
                if(list[i].contains("_254")) //generality
                {
                    fprintf(paWhole, "0 0 1 \n\n");
                }
            }
            if(NumOfClasses==2)
                {
                if(list[i].contains("_241")) //generality
                {
                    fprintf(paWhole, "1 0 \n\n");
                }
                if(list[i].contains("_247")) //generality
                {
                    fprintf(paWhole, "0 1 \n\n");
                }
            }

//        cout<<i<<" ";
    }

    fclose(paWhole);




    for(int i=0; i<ns; ++i)
    {
        delete []data4[i];
    }
    delete []data4;

    for(int i=0; i<NumOfClasses; ++i)
    {
        delete []arr[i];
    }
    delete []arr;

    delete dir_;

    helpString="PA-files have been made \n";
    this->ui->lineEdit->setText(helpString);

    QTimer::singleShot(600, ui->lineEdit, SLOT(clear()));
    cout<<"PA-files have been made"<<endl;
}

void MakePa::setRdcCoeff(int newCoeff)
{
    ui->rdcCoeffBox->setValue(newCoeff);
}

int MakePa::getRdcCoeff()
{
    return ui->rdcCoeffBox->value();
}

void MakePa::dispersionAnalysis()
{
    if(ui->cl3_Button->isChecked()) NumOfClasses=3;    ///////////////generality
    if(ui->cl2_Button->isChecked()) NumOfClasses=2;

    QDir *dir_ = new QDir();
    dir_->cd(ui->paLineEdit->text());
    QStringList nameFilters, list, lst[3];
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_1->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[0]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);

    nameFilters.clear();

    list.clear();
    list = ui->lineEdit_2->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[1]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);

    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_3->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[2]=dir_->entryList(nameFilters, QDir::Files, QDir::Name);

    //all.pa
    list.clear();
    nameFilters.clear();
    helpString.clear();
    helpString.append(ui->lineEdit_1->text()).append(" ").append(ui->lineEdit_2->text()).append(" ").append(ui->lineEdit_3->text()).append(" ");

    list = helpString.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }

    cout<<lst[0].length()<<"\t"<<lst[1].length()<<"\t"<<lst[2].length()<<"\t"<<endl;

    list=dir_->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
    list.clear();
    list += lst[0];
    list += lst[1];
    list += lst[2];
    cout<<list.length()<<endl;
    if(list.empty())
    {
        cout<<"no spectra-files found"<<endl;
        return;
    }

//    cout<<"memory"<<endl;

    double ** average = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        average[i] = new double [ns*spLength+1];
    }
    double ** dispersion = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        dispersion[i] = new double [ns*spLength+1];
    }
    double ** meanVar = new double * [NumOfClasses];
    for(int i = 0; i < NumOfClasses; ++i)
    {
        meanVar[i] = new double [ns*spLength+1];
    }

    double ** matrix = new double * [list.length()];
    for(int i = 0; i < list.length(); ++i)
    {
        matrix[i] = new double [ns*spLength];
    }
//    cout<<"memory end"<<endl;

//    for(int k = 0; k < list.length(); ++k)
//    {
//        if(list[k].contains(ui->lineEdit_1->text())) average[k][ns*spLength] = 0;
//        else if(list[k].contains(ui->lineEdit_2->text())) average[k][ns*spLength] = 1;
//        else average[k][ns*spLength] = 2;
//    }
//    for(int k = 0; k < list.length(); ++k)
//    {
//        cout<<average[k][ns*spLength]<<endl;
//    }

//    cout<<"ready"<<endl;

    QFile curr;
    int num;
    for(int j = 0; j < NumOfClasses; ++j)
    {
        for(int k = 0; k < lst[j].length(); ++k)
        {
            helpString = QDir::toNativeSeparators(dir_->absolutePath().append(QDir::separator()).append(lst[j][k]));
            curr.setFileName(helpString);
            curr.open(QIODevice::ReadOnly);
            for(int i = 0; i < ns; ++i)
            {
                for(int l = 0; l < spLength; ++l)
                {
                    helpString = curr.readLine();
//                    num = 0;
//                    for(int z = 0; z < j; ++z)
//                    {
//                        num += lst[z].length();
//                    }
//                    num+=k;

                    matrix[k][i*spLength + l] = helpString.toDouble();
                }
                helpString = curr.readLine(); //fscanf("\n");
            }
            curr.close();
        }

        for(int i = 0; i < ns*spLength; ++i)
        {
            average[j][i] = 0.;
            dispersion[j][i] = 0.;
            for(int k = 0; k < lst[j].length(); ++k)
            {
                average[j][i] += matrix[k][i];
            }
            average[j][i] /= lst[j].length();

            for(int k = 0; k < lst[j].length(); ++k)
            {
                dispersion[j][i] += (matrix[k][i] - average[j][i]) * (matrix[k][i] - average[j][i]);
            }

            dispersion[j][i] /= lst[j].length();
            dispersion[j][i] = sqrt(dispersion[j][i]);

            meanVar[j][i] = average[j][i] / dispersion[j][i];
//            cout<<meanVar[j][i]<<endl;
        }
    }

    cout<<"meanVar counted"<<endl;
    for(int i = 0; i < ns*spLength; ++i)
    {
//        cout<<meanVar[0][i]<<endl;
    }
    double maxMean = 0.;
    double minMean = 100500.;
    double maxVar = 0.;
    double minVar = 100500.;
    double maxMeanVar = 0.;
    double minMeanVar = 100500.;



    minMean = 0.;
    minVar = 0.;
    minMeanVar = 0.;
    for(int j = 0; j < NumOfClasses; ++j)
    {
        for(int i = 0; i < ns*spLength; ++i)
        {
            maxMean = max(maxMean, average[j][i]);
//            minMean = min(minMean, average[j][i]);
            maxVar = max(maxVar, dispersion[j][i]);
//            minVar = min(minVar, dispersion[j][i]);
            maxMeanVar = max(maxMeanVar, meanVar[j][i]);
//            minMeanVar = min(minMeanVar, meanVar[j][i]);
        }
    }


    //3 histogramms of stability of each class
    for(int j = 0; j < NumOfClasses; ++j)
    {
//        maxVar = 0.;
//        minVar = 100500.;
//        for(int i = 0; i < ns*spLength; ++i)
//        {
//            maxVar = max(maxVar, dispersion[j][i]);
//            minVar = min(minVar, dispersion[j][i]);
//        }

        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append('_').append("Variance_").append(QString::number(j)));
        if(!ui->addNameLineEdit->text().isEmpty())
        {
            helpString.append("-").append(ui->addNameLineEdit->text());
        }
        helpString.append(".jpg");
        kernelest(dispersion[j], ns*spLength, helpString, minVar, maxVar, 800);
    }

    for(int j = 0; j < NumOfClasses; ++j)
    {
//        maxMean = 0.;
//        minMean = 100500.;
//        for(int i = 0; i < ns*spLength; ++i)
//        {
//            maxMean = max(maxMean, average[j][i]);
//            minMean = min(minMean, average[j][i]);
//        }

        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append('_').append("Mean_").append(QString::number(j)));
        if(!ui->addNameLineEdit->text().isEmpty())
        {
            helpString.append("-").append(ui->addNameLineEdit->text());
        }
        helpString.append(".jpg");
        kernelest(average[j], ns*spLength, helpString, minMean, maxMean, 800);
    }

    for(int j = 0; j < NumOfClasses; ++j)
    {
//        maxMeanVar = 0.;
//        minMeanVar = 100500.;
//        for(int i = 0; i < ns*spLength; ++i)
//        {
//            maxMeanVar = max(maxMeanVar, average[j][i]);
//            minMeanVar = min(minMeanVar, average[j][i]);
//        }
        helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append('_').append("meanVar_").append(QString::number(j)));
        if(!ui->addNameLineEdit->text().isEmpty())
        {
            helpString.append("-").append(ui->addNameLineEdit->text());
        }
        helpString.append(".jpg");
        kernelest(meanVar[j], ns*spLength, helpString, minMeanVar, maxMeanVar, 800);
    }

    //cross stability
    maxMeanVar = 0.;
    for(int i = 0; i < ns*spLength; ++i)
    {
        meanVar[0][i] = fabs(average[0][i] - average[1][i]) / (dispersion[0][i] + dispersion[1][i]);
        maxMeanVar = fmax(maxMeanVar, meanVar[0][i]);
    }
    helpString = QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append(ExpName).append('_').append("meanVar_diff"));
    if(!ui->addNameLineEdit->text().isEmpty())
    {
        helpString.append("-").append(ui->addNameLineEdit->text());
    }
    helpString.append(".jpg");
    kernelest(meanVar[0], ns*spLength, helpString, 0, maxMeanVar, 800);

    drawSamples(meanVar[0], 0.65, 1.5);

    for(int i = 0; i < NumOfClasses; ++i)
    {
        delete [] average[i];
        delete [] dispersion[i];
        delete [] meanVar[i];
    }
    delete [] average;
    delete [] dispersion;
    delete [] meanVar;

    delete dir_;
    cout<<"dispersion analysis made"<<endl;
}

double MakePa::drawSamples(double * drawArray, double leftLim, double rightLim)
{

    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->paLineEdit->text());          /////////which dir?
    helpString=dir_->absolutePath();

    QStringList nameFilters, list, lst[2]; //0 - Spatial, 1 - Verbal
    FILE * currFile;

    //make lists of files
    //Spatial
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_1->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[0]=dir_->entryList(nameFilters, QDir::Files);

    //Verbal
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_2->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[1]=dir_->entryList(nameFilters, QDir::Files);


    int * n = new int[2];
    n[0]=lst[0].length();
    n[1]=lst[1].length();

    double *** spectre = new double ** [2];
    for(int i=0; i<2; ++i)
    {
        spectre[i] = new double * [n[i]];
        for(int j=0; j<n[i]; ++j)
        {
            spectre[i][j] = new double [ns*spLength];
        }
    }

    //read the spectra into matrixes
    for(int j=0; j<2; ++j)
    {
        for(int i=0; i<n[j]; ++i)
        {
            helpString=dir_->absolutePath().append(QDir::separator()).append(lst[j][i]);
            currFile = fopen(helpString.toStdString().c_str(), "r");
            for(int k=0; k<ns*spLength; ++k)
            {
                fscanf(currFile, "%lf", &spectre[j][i][k]);
            }
            fclose(currFile);
        }
    }

    //paint
    int * spL = new int [2];
    for(int i=0; i<2; ++i)
    {
        spL[i] = spLength;
    }

    double *** sp = new double **[2];
    for(int i=0; i<2; ++i)
    {
        sp[i] = new double * [ns];
        for(int j=0; j<ns; ++j)
        {
            sp[i][j] = new double [spL[i]];
        }
    }

    //create sp1 & sp2 - average spectra
    for(int i=0; i<ns; ++i)
    {
        for(int j=0; j<spLength; ++j)
        {
            for(int h=0; h<2; ++h)
            {

                sp[h][i][j]=0.;
                for(int k=0; k<n[h]; ++k)
                {
                    sp[h][i][j]+=spectre[h][k][i*spL[h]+j];
                }
                sp[h][i][j]/=n[h];
            }
        }
    }



    QPixmap pic(1600,1600);
//    cout<<"before QPainter"<<endl;
    QPainter *paint = new QPainter;
//    cout<<"after QPainter"<<endl;
    pic.fill();
    paint->begin(&pic);


    double norm = ui->mwNormSpinBox->value(); //10 pixels=1mV^2 / Hz
    double barWidth=1/2.;
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {

        //average spectra drawing
        for(int k=0; k<250-1; ++k)
        {
            for(int h=0; h<2; ++h)
            {
                if(h==0) paint->setPen(QPen(QBrush("blue"), 2));
                if(h==1) paint->setPen(QPen(QBrush("red"), 2));
                if(h==2) paint->setPen(QPen(QBrush("green"), 2));
                paint->drawLine(paint->device()->width() * coords::x[c2]+k, paint->device()->height() * coords::y[c2] - sp[h][c2][int(k*spL[h]/250.)]*norm, paint->device()->width() * coords::x[c2]+k+1, paint->device()->height() * coords::y[c2] - sp[h][c2][int((k+1)*spL[h]/250.)]*norm);
            }
        }

        paint->setPen("black");
        paint->setBrush(QBrush("black"));

        //statistic difference bars
        for(int j=c2*spLength; j<(c2+1)*spLength; ++j)
        {
            for(int h=0; h<2; ++h)
            {
                if(h==0)
                {
                    paint->setPen("blue");
                    paint->setBrush(QBrush("blue"));
                }
                if(h==1)
                {
                    paint->setPen("red");
                    paint->setBrush(QBrush("red"));
                }
                if(leftLim < drawArray[j] && drawArray[j] < rightLim)
                {
                    paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*spLength))*(250./spLength), paint->device()->height() * coords::y[c2] + 15 + 7*h , 2*barWidth*(250./spLength), 5); //-15 graphical 1/3 graphical
                }
            }

        }
        paint->setPen("black");

        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);

        paint->setFont(QFont("Helvitica", 8));

        //Hz markers
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

    //channel labels
    for(int c2=0; c2<ns; ++c2)  //exept markers channel
    {
        paint->drawText((paint->device()->width() * coords::x[c2]-20), (paint->device()->height() * coords::y[c2]-252), QString(coords::lbl[c2]));
    }

    //draw scale
    //draw scale
    paint->drawLine(QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1]), QPointF(paint->device()->width() * coords::x[6], paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())));  //250 - graph height generality

    //returning norm = max magnitude

    norm = (coords::scale * paint->device()->height()) / norm;
    norm = int(norm*10)/10.;
    helpString.setNum(norm);
    helpString.append(tr(" mcV^2/Hz"));
    paint->drawText(QPointF(paint->device()->width() * coords::x[6]+5., paint->device()->height() * coords::y[1] - (coords::scale * paint->device()->height())/2.), helpString);



    //save
    helpString = dir->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("meanVar_samples").append(ui->addNameLineEdit->text()).append(".jpg");
    cout<<helpString.toStdString()<<endl;
    pic.save(helpString, 0, 100);
    paint->end();

}

double MakePa::gaussian(double x)
{
    return 1/(sqrt(2*M_PI))*exp(-x*x/2.);
}
void MakePa::kernelest(double * arr, int num, QString path, double minVal, double maxVal, int numOfRanges)
{
    double average = 0.;
    double sigma = 0.;
    for(int i = 0; i < num; ++i)
    {
        average += arr[i];
    }
    average /= num;
    for(int i = 0; i < num; ++i)
    {
        sigma += (average-arr[i]) * (average-arr[i]);
    }
    sigma = sqrt(sigma/num);   //// (sigma)/num ??
    average = -0.2*log(num);

    double h = 1.06*sigma*exp(average); //KDE parameter

    QPixmap pic(800, 600);
    QPainter * pnt = new QPainter();
    pic.fill();
    pnt->begin(&pic);
    pnt->setPen("black");

//    for(int i = 0; i < pic.width(); ++i)
//    {
//        average = 0.;
//        for(int j = 0; j < num; ++j)
//        {
//            average += 1/(num*h) * gaussian((i - arr[j])/h)*1e4; //1e4 - graphics param
//        }

//        if(i>0) pnt->drawLine(i-1, pic.height()*0.9 - sigma, i, pic.height()*0.9 - average);
//        sigma = average;
//    }

    pnt->setPen("black");
    pnt->setBrush(QBrush("black"));
    double * drawVec = new double [numOfRanges];
    double norm = 0.;
    for(int i = 0; i < numOfRanges; ++i)
    {
        drawVec[i] = 0.;
        for(int j = 0; j < num; ++j)
        {
            drawVec[i] += 1/(num*h) * gaussian((i*(maxVal-minVal)/numOfRanges - arr[j])/h);
        }
        norm = fmax(norm, drawVec[i]);
    }

    double heightNorm = 0.95;
    for(int i = 0; i < numOfRanges; ++i)
    {
//         pnt->drawLine(i-1, pic.height()*0.9 - sigma, i, pic.height()*0.9 - average);
        pnt->drawRect(pic.width()/double(numOfRanges) * i, pic.height() * heightNorm, pic.width()/double(numOfRanges), -pic.height() * drawVec[i]/norm * heightNorm);
    }
    double step = int((maxVal - minVal)) + 1;
    step /= 0.1;
    step = int(step/20.);
    if(step == 0) step = 0.5;
    step *= 0.1;


    cout<<step<<endl;
    for(double i = minVal; i < maxVal; i += step)
    {
        pnt->drawLine(pic.width() * i / (maxVal - minVal), pic.height() * heightNorm, pic.width() * i / (maxVal - minVal), pic.height());
        pnt->drawText(pic.width() * i / (maxVal - minVal), pic.height(), QString::number(i));
    }
    pic.save(path, 0, 100);

    delete pnt;
    delete [] drawVec;
}

void MakePa::correlationDifference()
{
    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->paLineEdit->text());          /////////which dir?
    helpString=dir_->absolutePath();

    QStringList nameFilters, list, lst[2]; //0 - Spatial, 1 - Verbal
    FILE * currFile;

    //make lists of files
    //Spatial
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_1->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[0]=dir_->entryList(nameFilters, QDir::Files);

    //Verbal
    nameFilters.clear();
    list.clear();
    list = ui->lineEdit_2->text().split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i=0; i<list.length(); ++i)
    {
        helpString.clear();
        helpString.append("*");
        helpString.append(list.at(i));
        helpString.append("*");
        nameFilters.append(helpString);
    }
    lst[1]=dir_->entryList(nameFilters, QDir::Files);


    int * n = new int[2];
    n[0]=lst[0].length();
    n[1]=lst[1].length();

    double *** spectre = new double ** [2];
    for(int i=0; i<2; ++i)
    {
        spectre[i] = new double * [n[i]];
        for(int j=0; j<n[i]; ++j)
        {
            spectre[i][j] = new double [ns*spLength];
        }
    }

    //read the spectra into matrixes
    for(int j=0; j<2; ++j)
    {
        for(int i=0; i<n[j]; ++i)
        {
            helpString=dir_->absolutePath().append(QDir::separator()).append(lst[j][i]);
            currFile = fopen(helpString.toStdString().c_str(), "r");
            for(int k=0; k<ns*spLength; ++k)
            {
                fscanf(currFile, "%lf", &spectre[j][i][k]);
            }
            fclose(currFile);
        }
    }

    double mean[2];
    double ** pointMean = new double *[2];
    pointMean[0] = new double [ns*spLength];
    pointMean[1] = new double [ns*spLength];

    double variance[2];
    double covariance = 0.;

    double distance = 0.;


    for(int h = 0; h < ns*spLength; ++h)
    {
        pointMean[0][h] = 0.;
        pointMean[1][h] = 0.;
    }

    for(int j = 0; j < 2; ++j) //j - number of set
    {
        mean[j] = 0.;
        variance[j] = 0.;
        for(int h = 0; h < ns*spLength; ++h)
        {
            for(int i = 0; i < n[j]; ++i)
            {
                mean[j] += spectre[j][i][h];
                pointMean[j][h] += spectre[j][i][h];
            }
            pointMean[j][h] /= n[j];
        }
        mean[j] /= n[j];
        mean[j] /= ns*spLength;


        for(int i = 0; i < n[j]; ++i)
        {
            for(int h = 0; h < ns*spLength; ++h)
            {
                variance[j] += (pointMean[j][h]- mean[j]) * (pointMean[j][h] - mean[j]);
            }
        }
        variance[j] /=n[j];
        variance[j] = sqrt(variance[j]);
    }
    for(int h = 0; h < ns*spLength; ++h)
    {
        covariance += (pointMean[0][h]- mean[0]) * (pointMean[1][h] - mean[1]);
    }
    distance = covariance/(variance[0]*variance[1]);

    //automatization
    FILE * res = fopen(QDir::toNativeSeparators(dir->absolutePath().append(QDir::separator()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "Correlation difference\t%lf\n", distance);
    fclose(res);


    for(int h=0; h<2; ++h)
    {
        for(int i=0; i<n[h]; ++i)
        {
            delete []spectre[h][i];
        }
        delete []spectre[h];
    }


//    delete []U;
    delete dir_;

    helpString = "Pearson difference made";
    ui->mwTestLine->setText(helpString);
}

