#include "eyes.h"
#include "ui_eyes.h"

Eyes::Eyes(QDir *dir_, int ns_) :
    ui(new Ui::Eyes)
{
    ui->setupUi(this);
    ns=ns_;
    dir = new QDir();
    dirBC = new QDir();

    dir->cd(QDir::toNativeSeparators(dir_->absolutePath()));
    dirBC->cd(QDir::toNativeSeparators(dir_->absolutePath()));
    ui->lineEdit_3->setText(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()));


    ui->spinBox->setValue(19);  //for encephalan
    ui->lineEdit_2->setText("20 21");
    QButtonGroup *group1 = new QButtonGroup();
    group1->addButton(ui->realisationsRadioButton);
    group1->addButton(ui->cutRadioButton);
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(eyesProcessing()));
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(eyesClean()));
    connect(group1, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(setDir(QAbstractButton*)));
    this->setAttribute(Qt::WA_DeleteOnClose);
//    group1->button(1)->setChecked(true); //cut by default
}

void Eyes::setDir(QAbstractButton* but)
{
    cout<<but->text().toStdString()<<endl;
    ui->lineEdit_3->setText(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(but->text()));
}

void Eyes::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

void Eyes::eyesClean()
{


    FILE * coef=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("eyes.txt").toStdString().c_str(), "r");
    if(coef==NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("no coeffs found"), QMessageBox::Ok);
        return;
    }


//    QString str = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(NULL, tr("choose dir"), dir->absolutePath())); // ExpName/Realisations or ExpName/cut
//    if(str=="")
//    {
//        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("no dir was chosen"), QMessageBox::Ok);
//        return;
//    }
//    ui->lineEdit_3->setText(str);

    dir->cd(ui->lineEdit_3->text()); //or you can write in manually

    QStringList list = dir->entryList(QDir::Files|QDir::NoDotAndDotDot); // files in the directory concerned

    int NumEog, NumEeg;
    int NumOfSlices=0;

    lst = ui->lineEdit_2->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts); //Numbers of eog channels

    fscanf(coef, "NumOfEyesChannels %d\n", &NumEog);
    cout<<"NumEog="<<NumEog<<endl;
    fscanf(coef, "NumOfEegChannels %d\n", &NumEeg);
    cout<<"NumEeg="<<NumEeg<<endl;

    double **coefficients = new double * [NumEeg];
    for(int i=0; i<NumEeg; ++i)
    {
        coefficients[i] = new double [NumEog];
    }


    double **dataF = new double * [ns];


    for(int k=0; k<NumEeg; ++k)
    {
        for(int i=0; i<NumEog; ++i)
        {
            fscanf(coef, "%lf ", &coefficients[k][i]);
            cout<<"coefficients["<<k<<"]["<<i<<"] = " <<coefficients[k][i]<<endl;
        }
        fscanf(coef, "\n");
    }
    fclose(coef);

    int a, NumOfEyes = 0;
    FILE * file;
    cout<<"ns = "<<ns<<endl;
    cout<<"dir of files to clean from eyes = "<<dir->absolutePath().toStdString()<<endl;
    for(int i=0; i<list.length(); ++i)
    {
//        cout<<list.at(i).toStdString()<<endl;
        file=fopen((QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(list.at(i))).toStdString().c_str(), "r");
        if(file==NULL)
        {
            cout<<"file to read==NULL"<<endl;
            continue;
        }
        fscanf(file, "NumOfSlices %d\n", &NumOfSlices);        
        fscanf(file, "NumOfSlicesEyesCut %d\n", &NumOfEyes);

        cout<<"NumOfSlices = "<<NumOfSlices<<endl;
        for(int j=0; j<ns; ++j)
        {
            dataF[j] = new double [NumOfSlices]; //set memory for data
        }
        for(int j=0; j<NumOfSlices; ++j)
        {
            for(int k=0; k<ns; ++k)
            {
                fscanf(file, "%lf\n", &dataF[k][j]); //read data
            }
        }
        fclose(file);

        for(int k=0; k<NumEeg; ++k)
        {
            for(int j=0; j<NumOfSlices; ++j)
            {
                for(int z=0; z<NumEog; ++z)
                {
                    a=lst.at(z).toInt()-1;
                    dataF[k][j]-=coefficients[k][z]*dataF[a][j];
                }
            }
        }

        file=fopen((QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append(list.at(i))).toStdString().c_str(), "w");
        if(file==NULL)
        {
            cout<<"file to write==NULL"<<endl;
            continue;
        }
        fprintf(file, "NumOfSlices %d\r\n", NumOfSlices);
//        fprintf(file, "NumOfSlicesEyesCut %d\n", &NumOfEyes);

        for(int j=0; j<NumOfSlices; ++j)
        {
            for(int k=0; k< ns-lst.length(); ++k) //don't write eog channels - generality (if k!= lst[i] foreach i)
            {
                fprintf(file, "%lf\n", dataF[k][j]);
            }
        }
        fclose(file);
        cout<<list.at(i).toStdString()<<endl;

        for(int i=0; i<ns; ++i)
        {
            delete []dataF[i];
        }


        NumOfEyes = 0;


    }
    dir->cd(dirBC->absolutePath());

    cout<<"eyes cleaned"<<endl;
    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Info"), tr("Eyes cleaned"), QMessageBox::Ok);

    ns-=lst.length();  // generality
    emit setNsMain(ns);

    //error here
    delete []dataF;


    for(int i=0; i<NumEeg; ++i)
    {
        if(coefficients[i]!=NULL) delete []coefficients[i];
    }
    delete []coefficients;
    this->close();
}

void Eyes::eyesProcessing()
{
    QStringList list;
    if(!autoFlag)
    {
        list = QFileDialog::getOpenFileNames(NULL, tr("Files to build regression"), dir->absolutePath());
        if(list.isEmpty())
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No files were chosen"), QMessageBox::Ok);
            return;
        }
    }
    //automatization
    else
    {

        QString helpString = dir->absolutePath();
        helpString.append(QDir::separator()).append("eyesSlices");
        //    cout<<helpString.toStdString()<<endl;
        list = QStringList(helpString);
    }

    lst = ui->lineEdit_2->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);  //numbers of eog channels

    int NumOfSlices=0, help=0;
    FILE * file;

    dataE = new double* [ns];
    for(int i=0; i<ns; ++i)
    {
        dataE[i] = new double [250*60*5];           //for 5 minutes generality
    }

    //arrays for usability
    double **signal = new double * [lst.length()+1];
    for(int i=0; i<lst.length()+1; ++i)
    {
        signal[i] = new double [250*60*5];           //for 5 minutes generality
    }

    double **matrix = new double * [lst.length()+1];
    for(int i=0; i<lst.length()+1; ++i)
    {
        matrix[i] = new double [lst.length()+1];
    }

    double **matrixInv = new double * [lst.length()+1];
    for(int i=0; i<lst.length()+1; ++i)
    {
        matrixInv[i] = new double [lst.length()+1];
    }

    double **matrixTemp = new double * [lst.length()+1];
    for(int i=0; i<lst.length()+1; ++i)
    {
        matrixTemp[i] = new double [lst.length()+1];
    }

    double **coefficients = new double * [ns];
    for(int i=0; i<ns; ++i)
    {
        coefficients[i] = new double [lst.length()];
    }

    double *average = new double [lst.length()+1];
//    cout<<"2"<<endl;

    //make dataE array to count covariation matrix
    for(int i=0; i<list.length(); ++i)
    {
        file=fopen((list.at(i)).toStdString().c_str(), "r");
//        cout<<(list.at(i)).toStdString().c_str()<<endl;
        fscanf(file, "NumOfSlices %d\n", &help);
//        cout<<help<<endl;
        for(int j=0; j<help; ++j)
        {
            for(int k=0; k<ns; ++k)
            {
                fscanf(file, "%lf", &dataE[k][NumOfSlices+j]);
            }
        }
        NumOfSlices+=help;

        fclose(file);
    }
//    cout<<NumOfSlices<<endl;
    for(int j=0; j<lst.length()+1; ++j)
    {
        average[j]=0.;
    }
//    cout<<"3"<<endl;





    //count eog averages
//    for(int i=0; i<NumOfSlices; ++i)
//    {
//        for(int j=0; j<lst.length(); ++j)
//        {
//            average[j]+=(dataE[lst.at(j).toInt()-1][i])/double(NumOfSlices); //cool    //-1 coz of 0 as first
//        }
//    }
//    cout<<"4"<<endl;






    for(int i=0; i<lst.length(); ++i)
    {
        signal[i]=dataE[lst.at(i).toInt()-1];     //-1 coz of 0 as first
    }


    //for every channel count eog coefficients
    for(int k=0; k<ui->spinBox->value(); ++k)
    {

        signal[lst.length()]=dataE[k];
        average[lst.length()]=0.;

//        //print the signals
//        for(int j=0; j<lst.length()+1; ++j)
//        {
//            for(int i=0; i<NumOfSlices; ++i)
//            {
//                cout<<signal[j][i]<<"  ";
//            }
//            cout<<endl<<endl;
//        }
        for(int j=0; j<lst.length()+1; ++j)
        {
            for(int z=0; z<lst.length()+1; ++z)
            {
                matrix[j][z]=0.;
            }
        }

        //count covariation-matrix
        for(int i=0; i<NumOfSlices; ++i)
        {
            average[lst.length()]+=(dataE[k][i])/double(NumOfSlices);
            for(int j=0; j<lst.length()+1; ++j)
            {
                for(int z=0; z<lst.length()+1; ++z)
                {
                    matrix[j][z]+=(signal[j][i]-average[j])*(signal[z][i]-average[z])/double(NumOfSlices);
                }
            }
        }
        //count Inverse Matrix


        //1) set E-matrix
        for(int j=0; j<lst.length()+1; ++j)
        {
            for(int z=0; z<lst.length()+1; ++z)
            {
                if(j==z) matrixInv[j][z]=1.;
                else matrixInv[j][z]=0.;
            }
        }

//        //print cov-matrix
//        for(int j=0; j<lst.length()+1; ++j)
//        {
//            for(int z=0; z<lst.length()+1; ++z)
//            {
//                cout<<matrix[j][z]<<"  ";
//            }
//            cout<<endl;
//        }



        //2) make diagonal matrix
        //col - current number of column
        //matrixTemp - current new auxiliary matrix
        //matrixInv - current inverse matrix
        for(int col=0; col<lst.length()+1; ++col)
        {
            for(int j=0; j<lst.length()+1; ++j)
            {
                for(int z=0; z<lst.length()+1; ++z)
                {
                    if(z==col)
                    {
                        if(z!=j) matrixTemp[j][z]=-matrix[j][z]/matrix[col][col];
                        else matrixTemp[j][z]=1/matrix[col][col];
                    }
                    else
                    {
                        if(z==j) matrixTemp[j][z]=1.;
                        else matrixTemp[j][z]=0.;
                    }
                }
            }
            matrixProduct(matrixTemp, matrixInv, matrixInv, lst.length()+1, lst.length()+1);
            matrixProduct(matrixTemp, matrix, matrix, lst.length()+1, lst.length()+1);
        }

        //set coeffs
        for(int i=0; i<lst.length(); ++i)
        {
            coefficients[k][i]=-matrixInv[i][lst.length()]/matrixInv[lst.length()][lst.length()];
        }
    }

    //print inv-matrix into file
    FILE * coef=fopen(QDir::toNativeSeparators(dir->absolutePath()).append(QDir::separator()).append("eyes.txt").toStdString().c_str(), "w");
    fprintf(coef, "NumOfEyesChannels %d\n", lst.length());
    fprintf(coef, "NumOfEegChannels %d\n", ui->spinBox->value());
    for(int k=0; k<ui->spinBox->value(); ++k)
    {
        for(int i=0; i<lst.length(); ++i)
        {
            fprintf(coef, "%lf ", coefficients[k][i]);
        }
        fprintf(coef, "\r\n");
    }
    fclose(coef);
    cout<<"coeffs ready"<<endl;

    //automatization
    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Info"), tr("Coeffs ready"), QMessageBox::Ok);


    dir->cd(dirBC->absolutePath());



    //clear memory
    for(int i=0; i<ns; ++i)
    {
        delete []coefficients[i];
        delete []dataE[i];
    }
    delete []coefficients;
    delete []dataE;

    for(int i=0; i<lst.length()+1; ++i)
    {
        delete []matrixTemp[i];
        delete []matrix[i];
        delete []matrixInv[i];
    }
    delete []matrixTemp;
    delete []matrix;
    delete []matrixInv;
}

Eyes::~Eyes()
{
    delete ui;
}
