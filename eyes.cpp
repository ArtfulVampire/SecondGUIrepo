#include "eyes.h"
#include "ui_eyes.h"

Eyes::Eyes() :
    ui(new Ui::Eyes)
{
    ui->setupUi(this);

    dirBC = new QDir();



    ui->progressBar->setValue(0);

    dirBC->cd(def::dir->absolutePath());
    ui->lineEdit_3->setText(QDir::toNativeSeparators(def::dir->absolutePath() + slash()));


    ui->spinBox->setValue(19);  //for encephalan
    ui->lineEdit_2->setText("20 21");

    if(def::ns == 41) //generality mati + amod
    {
        ui->spinBox->setValue(21);
        ui->lineEdit_2->setText("22 23");
    }

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
//    cout << but->text().toStdString() << endl;
    ui->lineEdit_3->setText(QDir::toNativeSeparators(def::dir->absolutePath()
                                                      + slash() + but->text()));
}

void Eyes::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

void Eyes::eyesClean()
{

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "eyes.txt");
    FILE * coef = fopen(helpString, "r");
    if(coef == NULL)
    {
        QMessageBox::critical((QWidget*)this, tr("Warning"), tr("no coeffs found"), QMessageBox::Ok);
        return;
    }

    def::dir->cd(ui->lineEdit_3->text()); //or you can write in manually
    QStringList list = def::dir->entryList(QDir::Files); // files in the directory
    int NumEog, NumEeg;
    int NumOfSlices = 0;

    QStringList lst = ui->lineEdit_2->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);

    fscanf(coef, "NumOfEyesChannels %d\n", &NumEog);
    cout << "NumEog = " << NumEog << endl;
    fscanf(coef, "NumOfEegChannels %d\n", &NumEeg);
    cout << "NumEeg = " << NumEeg << endl;

    double **coefficients = new double * [NumEeg];
    for(int i = 0; i < NumEeg; ++i)
    {
        coefficients[i] = new double [NumEog];
    }

    matrix dataF;
    /*
    double **dataF = new double * [ns];
    for(int j = 0; j < ns; ++j)
    {
        dataF[j] = new double [250 * 60]; // generality for 1 minute realisations
    }
    */

    for(int k = 0; k < NumEeg; ++k)
    {
        for(int i = 0; i < NumEog; ++i)
        {
            fscanf(coef, "%lf", &coefficients[k][i]);
        }
        fscanf(coef, "\n");
    }
    fclose(coef);

    for(int i = 0; i < list.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + list[i]);
        readPlainData(helpString, dataF, def::ns, NumOfSlices);

        for(int k = 0; k < NumEeg; ++k)
        {
            for(int j = 0; j < NumOfSlices; ++j)
            {
                for(int z = 0; z < NumEog; ++z)
                {
                    dataF[k][j] -= coefficients[k][z]
                            * dataF[ lst[z].toInt() - 1 ][j]; // generality
                }
            }
        }

        helpString = QDir::toNativeSeparators(def::dir->absolutePath() + slash() + list[i]);

        ofstream outStr;
        outStr.open(helpString.toStdString().c_str());
        outStr << "NumOfSlices " << NumOfSlices << endl;
        for (int i = 0; i < NumOfSlices; ++i)
        {
            for(int j = 0; j < def::ns; ++j)
            {
                if(lst.contains(QString::number(j+1))) continue;
                outStr << fitNumber(doubleRound(dataF[j][i], 3), 7) << '\t';
            }
            outStr << '\n';
        }
        outStr.flush();
        outStr.close();


        ui->progressBar->setValue(i*100. / list.length());
        qApp->processEvents();


    }

    ui->progressBar->setValue(0);
    def::dir->cd(dirBC->absolutePath());

    cout << "eyes cleaned" << endl;
    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Info"), tr("Eyes cleaned"), QMessageBox::Ok);

    def::ns -= lst.length();  // generality

    //error here
//    delete []dataF;


    for(int i = 0; i < NumEeg; ++i)
    {
        if(coefficients[i] != NULL) delete []coefficients[i];
    }
    delete []coefficients;
    this->close();
}

void Eyes::eyesProcessing()
{
    QStringList list; // list of filepaths with eye-movements
    if(!autoFlag)
    {
        list = QFileDialog::getOpenFileNames(this,
                                             tr("Files to build regression"),
                                             def::dir->absolutePath());
        if(list.isEmpty())
        {
            QMessageBox::critical((QWidget*)this, tr("Warning"), tr("No files were chosen"), QMessageBox::Ok);
            return;
        }
    }
    else //automatization
    {
        QString helpString = def::dir->absolutePath()
                + slash() + "eyesSlices";
        list = QStringList(helpString);
    }

    QStringList lst = ui->lineEdit_2->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);  //numbers of eog channels, counting from 1

    int NumOfSlices = 0, help = 0;
    FILE * file;
    int Size = lst.length() + 1;
    // data for all eye-movements concatenated together
    dataE = new double * [def::ns];
    for(int i = 0; i < def::ns; ++i)
    {
        dataE[i] = new double [250*60*5];           //for 5 minutes generality
    }

    //arrays for usability
    double **signal = new double * [Size]; //for all EOG channels and one EEG channel
    for(int i = 0; i < Size; ++i)
    {
        signal[i] = new double [250*60*5];           //for 5 minutes generality
    }

    double **matrix = new double * [Size];
    for(int i = 0; i < Size; ++i)
    {
        matrix[i] = new double [Size];
    }

    double **matrixInv = new double * [Size];
    for(int i = 0; i < Size; ++i)
    {
        matrixInv[i] = new double [Size];
    }

    double **matrixTemp = new double * [Size];
    for(int i = 0; i < Size; ++i)
    {
        matrixTemp[i] = new double [Size];
    }

    double **coefficients = new double * [def::ns]; //final output coefficients
    for(int i = 0; i < def::ns; ++i)
    {
        coefficients[i] = new double [lst.length()];
    }


    //make dataE array to count covariation matrix
    for(int i = 0; i < list.length(); ++i)
    {
        file = fopen(list[i].toStdString().c_str(), "r");
        fscanf(file, "NumOfSlices %d\n", &help);
        for(int j = 0; j < help; ++j)
        {
            for(int k = 0; k < def::ns; ++k)
            {
                fscanf(file, "%lf", &dataE[k][NumOfSlices + j]);
            }
        }
        NumOfSlices += help;
        fclose(file);
    }
//    cout << "eyesProcessing: NumOfSlices = " << NumOfSlices << endl;


    for(int i = 0; i < lst.length(); ++i)
    {
        signal[i] = dataE[lst[i].toInt() - 1];     //-1 because of counting from 0
    }


    //for every channel count eog coefficients
    //assuming EEG channels are from 1st to ui->spinBox->value()'th
    //Size is the dimensionality of all matrices

    for(int k = 0; k < ui->spinBox->value(); ++k)
    {
        signal[lst.length()] = dataE[k];
        for(int j = 0; j < Size; ++j)
        {
            for(int z = 0; z < Size; ++z)
            {
                matrix[j][z] = 0.;
            }
        }

        //count covariations into matrix
        for(int j = 0; j < Size; ++j)
        {
            for(int z = 0; z < Size; ++z)
            {
                for(int i = 0; i < NumOfSlices; ++i)
                {
                    matrix[j][z] += signal[j][i] * signal[z][i] / double(NumOfSlices); // maybe (NumOfSlices-1), but it's not important
                }
            }
        }
        matrixInvertGauss(matrix, Size, matrixInv);
        //set coeffs
        for(int i = 0; i<lst.length(); ++i)
        {
            coefficients[k][i] = - matrixInv[i][lst.length()] / matrixInv[lst.length()][lst.length()];
        }

    } // end of cycle through the channels

    //print inv-matrix into file
    FILE * coef = fopen(QDir::toNativeSeparators(def::dir->absolutePath()
                                                 + slash() + "eyes.txt"), "w");
    fprintf(coef, "NumOfEyesChannels %d\n", lst.length());
    fprintf(coef, "NumOfEegChannels %d\n", ui->spinBox->value());
    for(int k = 0; k < ui->spinBox->value(); ++k)
    {
        for(int i = 0; i < lst.length(); ++i)
        {
            fprintf(coef, "%lf ", coefficients[k][i]);
        }
        fprintf(coef, "\r\n");
    }
    fclose(coef);
    cout << "coeffs ready" << endl;

    //automatization
    if(!autoFlag) QMessageBox::information((QWidget*)this, tr("Info"), tr("Coeffs ready"), QMessageBox::Ok);


    def::dir->cd(dirBC->absolutePath());

    //clear memory
    for(int i = 0; i < def::ns; ++i)
    {
        delete []coefficients[i];
        delete []dataE[i];
    }
    delete []coefficients;
    delete []dataE;

    for(int i = 0; i < Size; ++i)
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
