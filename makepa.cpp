#include "makepa.h"
#include "ui_makepa.h"

MakePa::MakePa(QString spectraPath, QVector<int> vect_) :
    ui(new Ui::MakePa)
{

    ui->setupUi(this);

    //left & right for drawing only

    vect = vect_;

    this->setWindowTitle("Make PA");

    ui->spLBox->setMaximum(1000);
    ui->spLBox->setMinimum(1);
    ui->spLBox->setValue(def::spLength());

    ui->nsBox->setMaximum(300);
    ui->nsBox->setMinimum(1);
    ui->nsBox->setValue(def::nsWOM());

    ui->foldSpinBox->setMaximum(10);
    ui->foldSpinBox->setMinimum(2);
    ui->foldSpinBox->setSingleStep(1);
    ui->foldSpinBox->setValue(2);

    ui->rdcCoeffBox->setValue(4);
    ui->rdcCoeffBox->setDecimals(3);
    ui->rdcCoeffBox->setMinimum(1e-3);
    ui->rdcCoeffBox->setMaximum(1e3);
//    QObject::connect(ui->rdcCoeffBox, SIGNAL(valueChanged(double)), ui->rdcCoeffBox, SLOT(mySlot(double)));
//    ui->rdcCoeffBox->setAdaptiveSingleStep();

    group1 = new QButtonGroup();
    group1->addButton(ui->realsRadioButton);
    group1->addButton(ui->windowsRadioButton);
    group1->addButton(ui->bayesRadioButton);
    group1->addButton(ui->pcaRadioButton);


    ui->numOfClassesSpinBox->setMaximum(5);
    ui->numOfClassesSpinBox->setMinimum(1);
    ui->numOfClassesSpinBox->setValue(def::numOfClasses());



    QString helpString = QDir::toNativeSeparators(spectraPath);
    ui->spectraDirLineEdit->setText(helpString);


    ui->alphaSpinBox->setSingleStep(0.01);
    ui->alphaSpinBox->setMaximum(1.0);
    ui->alphaSpinBox->setValue(0.95); // p=0.05
    ui->mwNormSpinBox->setValue(1.);
    ui->mwNormSpinBox->setSingleStep(0.1);

    helpCharArr = new char [64];
    QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(dirSlot()));
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(makePaSlot()));
    QObject::connect(ui->mwTestButton, SIGNAL(clicked()), this, SLOT(mwTest()));
    QObject::connect(ui->kwTestButton, SIGNAL(clicked()), this, SLOT(kwTest()));
    QObject::connect(ui->dispersionAnalysisButton, SIGNAL(clicked()), this, SLOT(dispersionAnalysis()));
    QObject::connect(ui->vdvTestButton, SIGNAL(clicked()), this, SLOT(correlationDifference()));
    QObject::connect(group1, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(changeSpectraDir(QAbstractButton*)));

    ui->numOfClassesSpinBox->setValue(def::numOfClasses());
    this->setAttribute(Qt::WA_DeleteOnClose);
}

MakePa::~MakePa()
{
    delete ui;
//    delete browser;
}

void MakePa::dirSlot()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose input dir"),
                                                           def::dir->absolutePath());
    if(!helpString.isEmpty())
    {
        ui->spectraDirLineEdit->setText(helpString);
    }
}

void MakePa::changeSpectraDir(QAbstractButton * button)
{
    QString str = button->text();
    QString hlp = ui->spectraDirLineEdit->text();

    // was
//    hlp.resize(hlp.lastIndexOf("SpectraSmooth") + QString("SpectraSmooth").length());
    // new
    hlp = def::dir->absolutePath()
            + slash() + "SpectraSmooth";
    if(!str.isEmpty())
    {
        hlp += slash() + str;
    }
    ui->spectraDirLineEdit->setText(hlp);
}

void MakePa::mwTest()
{
    ui->mwTestLine->clear();
    matrix inSpectraAv;
    matrix dists;
    trivector<int> MW;

    countMannWhitney(MW,
                     ui->spectraDirLineEdit->text(),
                     &inSpectraAv,
                     &dists);



    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "Help"
                                                  + slash() + def::ExpName
                                                  + "_Mann-Whitney"
                                                  + ui->addNameLineEdit->text() + ".jpg");
    drawTemplate(helpString);
    drawArrays(helpString,
               inSpectraAv);
    drawMannWitney(helpString,
                   MW);

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "results.txt");
    ofstream outStr;
    outStr.open(helpString.toStdString(), ios_base::app);
    if(!outStr.good())
    {
        cout << "can't open log file: " << helpString << endl;
    }
    else
    {
        for(int i = 0; i < def::numOfClasses(); ++i)
        {
            for(int j = i + 1; j < def::numOfClasses(); ++j)
            {
                outStr << "dist " << i << " - " << j << '\t' << dists[i][j - i] << '\n';
            }
        }
    }
    outStr.flush();
    outStr.close();

    ui->mwTestLine->setText("MW test made");

    QTimer::singleShot(600, ui->mwTestLine, SLOT(clear()));
}
/*
void MakePa::vdvTest()
{
    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->spectraDirLineEdit->text());          /////////which dir?
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
        spectre[0][i] = new double [def::nsWOM() * def::spLength()];
    }
    double ** spectre[1] = new double *[n[1]];
    for(int i=0; i<n[1]; ++i)
    {
        spectre[1][i] = new double [def::nsWOM() * def::spLength()];
    }

    //read the spectra into matrixes
    for(int i=0; i<n[0]; ++i)
    {
        helpString=dir_->absolutePath().append(slash()).append(lst[0].at(i));
        currFile = fopen(helpString.toStdString().c_str(), "r");
        for(int j=0; j<def::nsWOM() * def::spLength(); ++j)
        {
            fscanf(currFile, "%lf", &spectre[0][i][j]);
        }
        fclose(currFile);
    }

    for(int i=0; i<n[1]; ++i)
    {
        helpString=dir_->absolutePath().append(slash()).append(lst[1].at(i));
        currFile = fopen(helpString.toStdString().c_str(), "r");
        for(int j=0; j<def::nsWOM() * def::spLength(); ++j)
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

    double *X = new double[def::nsWOM() * def::spLength()];


    //set & sort the arrays

    for(int j=0; j<def::nsWOM() * def::spLength(); ++j)
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
    int spL1=def::spLength(), spL2=def::spLength();


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
        for(int j=0; j<def::spLength(); ++j)
        {
            sp1[i][j]=0.;
            for(int k=0; k<n[0]; ++k)
            {
                sp1[i][j]+=spectre[0][k][i*def::spLength()+j];
            }
            sp1[i][j]/=n[0];

            sp2[i][j]=0.;
            for(int k=0; k<n[1]; ++k)
            {
                sp2[i][j]+=spectre[1][k][i*def::spLength()+j];
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
        for(int j=c2*def::spLength(); j<(c2+1)*def::spLength(); ++j)
        {
            if((abs(X[j]/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) && (sp1[c2][j%def::spLength()] < sp2[c2][j%def::spLength()]))
            {

                paint->setPen("red");
                paint->setBrush(QBrush("red"));
//                paint->drawLine(paint->device()->width() * coords::x[c2]+ (j-c2*def::spLength())-barWidth*(250/def::spLength()), paint->device()->height() * coords::y[c2] -15 , paint->device()->width() * coords::x[c2] + (j-c2*def::spLength()) + barWidth*(250/def::spLength()), paint->device()->height() * coords::y[c2]+10); //-15 graphical 1/3 graphical
                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*def::spLength()))*(250./def::spLength()), paint->device()->height() * coords::y[c2] + 15 , 2*barWidth*(250./def::spLength()), 5); //-15 graphical 1/3 graphical
            }
            if((abs(X[j]/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) && (sp1[c2][j%def::spLength()] > sp2[c2][j%def::spLength()]))
            {

                paint->setPen("blue");
                paint->setBrush(QBrush("blue"));
//                paint->drawLine(paint->device()->width() * coords::x[c2]+ (j-c2*def::spLength())-barWidth*(250/def::spLength()), paint->device()->height() * coords::y[c2] -15 , paint->device()->width() * coords::x[c2] + (j-c2*def::spLength()) + barWidth*(250/def::spLength()), paint->device()->height() * coords::y[c2]+10); //-15 graphical 1/3 graphical
                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*def::spLength()))*(250./def::spLength()), paint->device()->height() * coords::y[c2] + 15 + 7 , 2*barWidth*(250./def::spLength()), 5); //-15 graphical 1/3 graphical
            }

        }

        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);
        paint->setPen("black");

        paint->setFont(QFont("Helvitica", 8));
        for(int k=0; k<250-1; ++k) //for every Hz generality
        {
//            paint->drawLine(paint->device()->width() * coords::x[c2]+250*k/15, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250*k/15, paint->device()->height() * coords::y[c2]+10);
            if( (left + k*(def::spLength())/250.)*spStep - floor((left + k*(def::spLength())/250.)*spStep) < def::spLength()/250.*spStep/2. || ceil((left + k*(def::spLength())/250.)*spStep) - (left + k*(def::spLength())/250.)*spStep < def::spLength()/250.*spStep/2.)
            {
                paint->drawLine(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]+5);

                helpInt = int((left + k*(def::spLength())/250.)*spStep + 0.5);
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
    helpString = def::dir->absolutePath().append(slash()).append("Help").append(slash()).append("Van der Varden").append(ui->addNameLineEdit->text()).append(".jpg");
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
#if 0

    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->spectraDirLineEdit->text());          /////////which dir?
    helpString=dir_->absolutePath();

    FILE * currFile;

    QStringList nameFilters, leest;
    QStringList lst[3];//0 - Spatial, 1 - Verbal, 2 - Gaps

    int k = 0;
    for(const QString & fileMark : def::fileMarkers)
    {
        nameFilters.clear();
        leest.clear();
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(int i = 0; i < leest.length(); ++i)
        {
            helpString = "*" + leest[i] + "*";
            nameFilters << helpString;
        }
        lst[k++] = dir_->entryList(nameFilters, QDir::Files, QDir::Name);
    }


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
            spectre[i][j] = new double [def::nsWOM() * def::spLength()]; // "NetLength"
        }
    }


    //read the spectra into matrixes
    for(int j=0; j<3; ++j)
    {
        for(int i=0; i<n[j]; ++i)
        {
            helpString=dir_->absolutePath().append(slash()).append(lst[j][i]);
            currFile = fopen(helpString.toStdString().c_str(), "r");
            for(int k=0; k<def::nsWOM() * def::spLength(); ++k)
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

    double *H = new double [def::nsWOM() * def::spLength()];

    cout<<"N="<<N<<endl;


    //for every spectra-bin
    for(int j=0; j<def::nsWOM() * def::spLength(); ++j)
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
    FILE * res = fopen(QDir::toNativeSeparators(def::dir->absolutePath().append(slash()).append("results.txt")).toStdString().c_str(), "a+");
    fprintf(res, "U-test\t%d\n", numOfDiff);
    fclose(res);



    //paint
    int * spL = new int [3];
    for(int i=0; i<3; ++i)
    {
        spL[i] = def::spLength();
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
        for(int j=0; j<def::spLength(); ++j)
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
        for(int j=c2*def::spLength(); j<(c2+1)*def::spLength(); ++j)
        {
            if(H[j] >= 5.9915)
            {
                paint->setPen("black");
                paint->setBrush(QBrush("black"));
                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*def::spLength()))*(250./def::spLength()), paint->device()->height() * coords::y[c2] + 15 , 2*barWidth*(250./def::spLength()), 5); //-15 graphical 1/3 graphical
            }
//            if((abs((U[j]-average)/dispersion)>quantile((1.00+ui->alphaSpinBox->value())/2.)) && ((sp1[c2][j%def::spLength()] > sp2[c2][j%def::spLength()])))
//            {

//                paint->setPen("blue");
//                paint->setBrush(QBrush("blue"));
//                paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*def::spLength()))*(250./def::spLength()), paint->device()->height() * coords::y[c2] + 15 + 7 , 2*barWidth*(250./def::spLength()), 5); //-15 graphical 1/3 graphical
//            }

        }
        paint->setPen("black");

        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]-250);
        paint->drawLine(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2]+250, paint->device()->height() * coords::y[c2]);



        paint->setFont(QFont("Helvitica", 8));
        //Hz markers
        for(int k=0; k<250-1; ++k)
        {
            if( (left + k*(def::spLength())/250.)*spStep - floor((left + k*(def::spLength())/250.)*spStep) < def::spLength()/250.*spStep/2. || ceil((left + k*(def::spLength())/250.)*spStep) - (left + k*(def::spLength())/250.)*spStep < def::spLength()/250.*spStep/2.)
            {
                paint->drawLine(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]+5);

                helpInt = int((left + k*(def::spLength())/250.)*spStep + 0.5);
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
    helpString = def::dir->absolutePath().append(slash()).append("Help").append(slash()).append("Kruscal-Wallis").append(ui->addNameLineEdit->text()).append(".jpg");
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
#endif
}

void MakePa::setFold(int a)
{
    ui->foldSpinBox->setValue(a);
}

void MakePa::makePaSlot()
{

    ui->lineEdit->clear();

    QString helpString;
    double coeff = ui->rdcCoeffBox->value();
    int fold = ui->foldSpinBox->value();

    const QString filePath = ui->spectraDirLineEdit->text();

    //generality
    vector<QStringList> lst; // usually 0 - Spatial, 1 - Verbal, 2 - Gaps
    makeFileLists(filePath, lst);

    /*
    int k = 0;
    for(const QString & fileMark : def::fileMarkers)
    {
        nameFilters.clear();
        leest.clear();
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(int i = 0; i < leest.length(); ++i)
        {
            helpString = "*" + leest[i] + "*";
            nameFilters << helpString;
        }
        lst[k++] = dir_->entryList(nameFilters, QDir::Files, QDir::Name);
    }
    */


    int len[def::numOfClasses()];
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        len[i] = lst[i].length();
        if(len[i] == 0)
        {
            cout << "no spectra files found of type " << i << " found" << endl;
            return;
        }
    }

    int Length;
    Length = len[0];
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        Length = min(Length, len[i]);
    }

    vector<vector<int>> arr;
    arr.resize(def::numOfClasses());
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        arr[i].resize(len[i]);

        for(int j = 0; j < len[i]; ++j)
        {
            arr[i][j] = j;
        }
    }

    //generality
    if(def::nsWOM() == -1) return;
    if(def::spLength() == -1) return;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        std::shuffle(arr[i].begin(),
                     arr[i].end(),
                     std::default_random_engine(seed));
    }



    QStringList listToWrite;
    listToWrite.clear();
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        for(int i = 0; i < (len[j] / fold) * (fold - 1); ++i)
        {
            listToWrite << lst[j][arr[j][i]];
        }
    }
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "PA"
                                          + slash() + "1.pa");
    makePaFile(filePath, listToWrite, coeff, helpString);



    listToWrite.clear();
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        for(int i = (len[j] / fold) * (fold - 1); i < (len[j] / fold) * fold; ++i)
        {
            listToWrite << lst[j][arr[j][i]];
        }
    }
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "PA"
                                          + slash() + "2.pa");
    makePaFile(filePath, listToWrite, coeff, helpString);



    listToWrite.clear();
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        for(int i = 0; i < (len[j] / fold) * fold; ++i)
        {
            listToWrite << lst[j][arr[j][i]];
        }
    }

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "PA"
                                          + slash() + "all.pa");
    makePaFile(filePath, listToWrite, coeff, helpString);

    helpString="PA-files have been made \n";
    this->ui->lineEdit->setText(helpString);

    QTimer::singleShot(600, ui->lineEdit, SLOT(clear()));
}

void MakePa::setRdcCoeff(double newCoeff)
{
    ui->rdcCoeffBox->setValue(newCoeff);
}

double MakePa::getRdcCoeff()
{
    return ui->rdcCoeffBox->value();
}

void MakePa::dispersionAnalysis()
{

    QString helpString;

    QDir *dir_ = new QDir();
    dir_->cd(ui->spectraDirLineEdit->text());
    QStringList leest, lst[3];
    QStringList nameFilters;

    int k = 0;
    for(const QString & fileMark : def::fileMarkers)
    {
        nameFilters.clear();
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(int i = 0; i < leest.length(); ++i)
        {
            helpString = "*" + leest[i] + "*";
            nameFilters << helpString;
        }
        lst[k++] = dir_->entryList(nameFilters, QDir::Files, QDir::Name);
    }

    //all.pa
    leest.clear();
    nameFilters.clear();


    helpString.clear();
    for(const QString & fileMark : def::fileMarkers)
    {
        helpString += fileMark + " ";
    }
    leest = helpString.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
    for(int i = 0; i < leest.length(); ++i)
    {
        helpString = "*" + leest[i] + "*";
        nameFilters << helpString;
    }

    cout << lst[0].length() << "\t" << lst[1].length() << "\t" << lst[2].length() << endl;

    leest = dir_->entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
    leest.clear();
    leest += lst[0];
    leest += lst[1];
    leest += lst[2];

    if(leest.empty())
    {
        cout << "no spectra-files found" << endl;
        return;
    }

    double ** average = new double * [def::numOfClasses()];
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        average[i] = new double [def::nsWOM() * def::spLength() + 1];
    }
    double ** dispersion = new double * [def::numOfClasses()];
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        dispersion[i] = new double [def::nsWOM() * def::spLength() + 1];
    }
    double ** meanVar = new double * [def::numOfClasses()];
    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        meanVar[i] = new double [def::nsWOM() * def::spLength() + 1];
    }

    double ** matrix = new double * [leest.length()];
    for(int i = 0; i < leest.length(); ++i)
    {
        matrix[i] = new double [def::nsWOM() * def::spLength()];
    }
//    cout<<"memory end"<<endl;

//    for(int k = 0; k < list.length(); ++k)
//    {
//        if(list[k].contains(ui->lineEdit_1->text())) average[k][def::nsWOM() * def::spLength()] = 0;
//        else if(list[k].contains(ui->lineEdit_2->text())) average[k][def::nsWOM() * def::spLength()] = 1;
//        else average[k][def::nsWOM() * def::spLength()] = 2;
//    }
//    for(int k = 0; k < list.length(); ++k)
//    {
//        cout<<average[k][def::nsWOM() * def::spLength()]<<endl;
//    }

//    cout<<"ready"<<endl;

    QFile curr;
//    int num;
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        for(int k = 0; k < lst[j].length(); ++k)
        {
            helpString = QDir::toNativeSeparators(dir_->absolutePath()
                                                  + slash() + lst[j][k]);
            curr.setFileName(helpString);
            curr.open(QIODevice::ReadOnly);
            for(int i = 0; i < def::nsWOM(); ++i)
            {
                for(int l = 0; l < def::spLength(); ++l)
                {
                    helpString = curr.readLine();
//                    num = 0;
//                    for(int z = 0; z < j; ++z)
//                    {
//                        num += lst[z].length();
//                    }
//                    num+=k;

                    matrix[k][i * def::spLength() + l] = helpString.toDouble();
                }
                helpString = curr.readLine(); //fscanf("\n");
            }
            curr.close();
        }

        for(int i = 0; i < def::nsWOM() * def::spLength(); ++i)
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
        }
    }

    cout << "meanVar counted" << endl;
    for(int i = 0; i < def::nsWOM() * def::spLength(); ++i)
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
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        for(int i = 0; i < def::nsWOM() * def::spLength(); ++i)
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
    for(int j = 0; j < def::numOfClasses(); ++j)
    {
//        maxVar = 0.;
//        minVar = 100500.;
//        for(int i = 0; i < def::nsWOM() * def::spLength(); ++i)
//        {
//            maxVar = max(maxVar, dispersion[j][i]);
//            minVar = min(minVar, dispersion[j][i]);
//        }

        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Help"
                                              + slash() + def::ExpName
                                              + "_Variance_" + QString::number(j));
        if(!ui->addNameLineEdit->text().isEmpty())
        {
            helpString += "-" + ui->addNameLineEdit->text();
        }
        helpString += ".jpg";
        kernelest(dispersion[j], def::nsWOM() * def::spLength(), helpString, minVar, maxVar, 800);
    }

    for(int j = 0; j < def::numOfClasses(); ++j)
    {
//        maxMean = 0.;
//        minMean = 100500.;
//        for(int i = 0; i < def::nsWOM() * def::spLength(); ++i)
//        {
//            maxMean = max(maxMean, average[j][i]);
//            minMean = min(minMean, average[j][i]);
//        }

        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Help"
                                              + slash() + def::ExpName
                                              + "_Mean_"+QString::number(j));
        if(!ui->addNameLineEdit->text().isEmpty())
        {
            helpString += "-" + ui->addNameLineEdit->text();
        }
        helpString += ".jpg";
        kernelest(average[j], def::nsWOM() * def::spLength(), helpString, minMean, maxMean, 800);
    }

    for(int j = 0; j < def::numOfClasses(); ++j)
    {
//        maxMeanVar = 0.;
//        minMeanVar = 100500.;
//        for(int i = 0; i < def::nsWOM() * def::spLength(); ++i)
//        {
//            maxMeanVar = max(maxMeanVar, average[j][i]);
//            minMeanVar = min(minMeanVar, average[j][i]);
//        }

        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Help"
                                              + slash() + def::ExpName
                                              + "_meanVar_" + QString::number(j));
        if(!ui->addNameLineEdit->text().isEmpty())
        {
            helpString += "-" + ui->addNameLineEdit->text();
        }
        helpString += ".jpg";
        kernelest(meanVar[j], def::nsWOM() * def::spLength(), helpString, minMeanVar, maxMeanVar, 800);
    }

    //cross stability
    maxMeanVar = 0.;
    for(int i = 0; i < def::nsWOM() * def::spLength(); ++i)
    {
        meanVar[0][i] = fabs(average[0][i] - average[1][i])
                / (dispersion[0][i] + dispersion[1][i]);
        maxMeanVar = fmax(maxMeanVar, meanVar[0][i]);
    }
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash() + def::ExpName
                                          + "_meanVar_diff");
    if(!ui->addNameLineEdit->text().isEmpty())
    {
        helpString += "-" + ui->addNameLineEdit->text();
    }
    helpString += ".jpg";
    kernelest(meanVar[0], def::nsWOM() * def::spLength(), helpString, 0, maxMeanVar, 800);

    drawSamples(meanVar[0], 0.65, 1.5);

    for(int i = 0; i < def::numOfClasses(); ++i)
    {
        delete [] average[i];
        delete [] dispersion[i];
        delete [] meanVar[i];
    }
    delete [] average;
    delete [] dispersion;
    delete [] meanVar;

    delete dir_;
    cout << "dispersion analysis made" << endl;
}

/// deprecate!
double MakePa::drawSamples(double * drawArray, double leftLim, double rightLim)
{

    ui->mwTestLine->clear();

    QDir *dir_ = new QDir();
    dir_->cd(ui->spectraDirLineEdit->text());          /////////which dir?
    QString helpString = dir_->absolutePath();

    QStringList nameFilters, leest, lst[3]; //0 - Spatial, 1 - Verbal
    FILE * currFile;

    int k = 0;
    for(const QString & fileMark : def::fileMarkers)
    {
        nameFilters.clear();
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(int i = 0; i < leest.length(); ++i)
        {
            helpString = "*" + leest[i] + "*";
            nameFilters << helpString;
        }
        lst[k++] = dir_->entryList(nameFilters, QDir::Files, QDir::Name);
    }


    int n[2];
    n[0] = lst[0].length();
    n[1] = lst[1].length();

    double *** spectre = new double ** [2];
    for(int i = 0; i < 2; ++i)
    {
        spectre[i] = new double * [n[i]];
        for(int j = 0; j < n[i]; ++j)
        {
            spectre[i][j] = new double [def::nsWOM() * def::spLength()];
        }
    }

    //read the spectra into matrixes
    for(int j = 0; j < 2; ++j)
    {
        for(int i=0; i<n[j]; ++i)
        {
            helpString = dir_->absolutePath() + slash() + lst[j][i];
            currFile = fopen(helpString, "r");
            for(int k = 0; k < def::nsWOM() * def::spLength(); ++k)
            {
                fscanf(currFile, "%lf", &spectre[j][i][k]);
            }
            fclose(currFile);
        }
    }

    //paint
    int * spL = new int [2];
    for(int i = 0; i < 2; ++i)
    {
        spL[i] = def::spLength();
    }

    double *** sp = new double **[2];
    for(int i=0; i<2; ++i)
    {
        sp[i] = new double * [def::nsWOM()];
        for(int j=0; j<def::nsWOM(); ++j)
        {
            sp[i][j] = new double [spL[i]];
        }
    }

    //create sp1 & sp2 - average spectra
    for(int i=0; i<def::nsWOM(); ++i)
    {
        for(int j=0; j<def::spLength(); ++j)
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


    const double graphHeight = paint->device()->height() * coords::scale;
    const double graphWidth = paint->device()->width() * coords::scale;
    const double graphScale = def::spLength() / graphWidth;

    int helpInt;

    for(int c2=0; c2<def::nsWOM(); ++c2)  //exept markers channel
    {

        const double Y = paint->device()->height() * coords::y[c2];
        const double X = paint->device()->width() * coords::x[c2];

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
        for(int j=c2*def::spLength(); j<(c2+1)*def::spLength(); ++j)
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
                    paint->drawRect(paint->device()->width() * coords::x[c2] + ((j-c2*def::spLength()))*(250./def::spLength()), paint->device()->height() * coords::y[c2] + 15 + 7*h , 2*barWidth*(250./def::spLength()), 5); //-15 graphical 1/3 graphical
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
            if( abs((def::left() + k * graphScale) * def::spStep()
                    - floor((def::left() + k * graphScale) * def::spStep() + 0.5))
                    < graphScale * def::spStep() / 2. )
            {
                paint->drawLine(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2], paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]+5);

                helpInt = int((def::left() + k*(def::spLength())/250.)*def::spStep() + 0.5);
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
    for(int c2=0; c2<def::nsWOM(); ++c2)  //exept markers channel
    {
        paint->drawText((paint->device()->width() * coords::x[c2]-20),
                        (paint->device()->height() * coords::y[c2]-252),
                        QString(coords::lbl[c2]));
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
    helpString = def::dir->absolutePath()
            + slash() + "Help"
            + slash() + "meanVar_samples"
            + ui->addNameLineEdit->text() + ".jpg";
    cout<<helpString.toStdString()<<endl;
    pic.save(helpString, 0, 100);
    paint->end();
    return 0.;

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
    dir_->cd(ui->spectraDirLineEdit->text());          /////////which dir?
    QString helpString = dir_->absolutePath();

    FILE * currFile;

    QStringList nameFilters, leest;
    QStringList lst[3];//0 - Spatial, 1 - Verbal, 2 - Gaps

    int k = 0;
    for(const QString & fileMark : def::fileMarkers)
    {
        nameFilters.clear();
        leest.clear();
        leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
        for(int i = 0; i < leest.length(); ++i)
        {
            helpString = "*" + leest[i] + "*";
            nameFilters << helpString;
        }
        lst[k++] = dir_->entryList(nameFilters, QDir::Files, QDir::Name);
    }


    int * n = new int[2];
    n[0]=lst[0].length();
    n[1]=lst[1].length();

    double *** spectre = new double ** [2];
    for(int i=0; i<2; ++i)
    {
        spectre[i] = new double * [n[i]];
        for(int j=0; j<n[i]; ++j)
        {
            spectre[i][j] = new double [def::nsWOM() * def::spLength()];
        }
    }

    //read the spectra into matrixes
    for(int j=0; j<2; ++j)
    {
        for(int i=0; i<n[j]; ++i)
        {
            helpString=dir_->absolutePath().append(slash()).append(lst[j][i]);
            currFile = fopen(helpString.toStdString().c_str(), "r");
            for(int k = 0; k < def::nsWOM() * def::spLength(); ++k)
            {
                fscanf(currFile, "%lf", &spectre[j][i][k]);
            }
            fclose(currFile);
        }
    }

    double mean[2];
    double ** pointMean = new double *[2];
    pointMean[0] = new double [def::nsWOM() * def::spLength()];
    pointMean[1] = new double [def::nsWOM() * def::spLength()];

    double variance[2];
    double covariance = 0.;

    double distance = 0.;


    for(int h = 0; h < def::nsWOM() * def::spLength(); ++h)
    {
        pointMean[0][h] = 0.;
        pointMean[1][h] = 0.;
    }

    for(int j = 0; j < 2; ++j) //j - number of set
    {
        mean[j] = 0.;
        variance[j] = 0.;
        for(int h = 0; h < def::nsWOM() * def::spLength(); ++h)
        {
            for(int i = 0; i < n[j]; ++i)
            {
                mean[j] += spectre[j][i][h];
                pointMean[j][h] += spectre[j][i][h];
            }
            pointMean[j][h] /= n[j];
        }
        mean[j] /= n[j];
        mean[j] /= def::nsWOM() * def::spLength();


        for(int i = 0; i < n[j]; ++i)
        {
            for(int h = 0; h < def::nsWOM() * def::spLength(); ++h)
            {
                variance[j] += (pointMean[j][h]- mean[j]) * (pointMean[j][h] - mean[j]);
            }
        }
        variance[j] /=n[j];
        variance[j] = sqrt(variance[j]);
    }
    for(int h = 0; h < def::nsWOM() * def::spLength(); ++h)
    {
        covariance += (pointMean[0][h]- mean[0]) * (pointMean[1][h] - mean[1]);
    }
    distance = covariance/(variance[0]*variance[1]);

    //automatization
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "results.txt");
    FILE * res = fopen(helpString, "a+");
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


void MakePa::on_rdcCoeffBox_valueChanged(double arg1)
{
    ui->rdcCoeffBox->setSingleStep(2.5 * pow(10., floor(log10(ui->rdcCoeffBox->value())) - 1.));
}
