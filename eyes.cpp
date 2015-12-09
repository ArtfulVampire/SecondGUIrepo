#include "eyes.h"
#include "ui_eyes.h"

Eyes::Eyes() :
    ui(new Ui::Eyes)
{
    ui->setupUi(this);


    ui->progressBar->setValue(0);

    ui->spinBox->setValue(19);  //for encephalan
    ui->lineEdit_2->setText("20 21");
    ui->lineEdit_2->setText("22 23");

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
}

void Eyes::setDir(QAbstractButton* but)
{
    ui->lineEdit_3->setText(QDir::toNativeSeparators(def::dir->absolutePath()
                                                      + slash() + but->text()));
}

void Eyes::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

void Eyes::eyesClean()
{
#if 0

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

        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + list[i]);

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
#endif
}

void Eyes::eyesProcessing()
{
    QTime myTime;
    myTime.start();

    QStringList leest; // list of filepaths with eye-movements
    if(!autoFlag)
    {
        leest = QFileDialog::getOpenFileNames(this,
                                             tr("Files to build regression"),
                                             def::dir->absolutePath());
        if(leest.isEmpty())
        {
            QMessageBox::critical((QWidget*)this,
                                  tr("Warning"),
                                  tr("No files were chosen"),
                                  QMessageBox::Ok);
            return;
        }
    }
    else //automatization
    {
#if 0
        // eyesSlices
        QString helpString = def::dir->absolutePath()
                             + slash() + "eyesSlices";
        leest = QStringList(helpString);
#else

        // windows
        QString helpString = def::dir->absolutePath()
                             + slash() + "windows";
        makeFullFileList(helpString, leest);
        for(QString & item : leest)
        {
            item.prepend(helpString + slash());
        }
#endif
    }

    QStringList lst;
    lst = ui->lineEdit_2->text().split(QRegExp("[,.; ]"),
                                       QString::SkipEmptyParts);  // 20 21

    int Size = lst.length() + 1; // usually 3

    matrix dataE;
    matrix matrixInit(Size, Size);
    matrix coefficients(def::ns, lst.length());

    //make dataE array to count covariation matrix
    int NumOfSlices = 0;
    int help;
    for(auto filePath : leest)
    {
        readPlainData(filePath,
                      dataE,
                      def::ns,
                      help,
                      NumOfSlices);
        NumOfSlices += help;
    }
    vector<int> signalNums;
    for(QString eogNum : lst)
    {
        signalNums.push_back(eogNum.toInt() - 1);
    }
    for(int k = 0; k < ui->spinBox->value(); ++k)
    {
        signalNums.push_back(k);
        for(int meow : signalNums)
        {
            cout << meow << " ";
        }
        cout << endl;
        for(int j = 0; j < Size; ++j)
        {
            for(int z = j; z < Size; ++z)
            {
                matrixInit[j][z] = covariance(  dataE[ signalNums[j] ],
                                                dataE[ signalNums[z] ],
                                                NumOfSlices,
                                                0,
                                                1) / NumOfSlices;
                if(j != z)
                {
                    matrixInit[z][j] = matrixInit[j][z];
                }
                 // maybe (NumOfSlices-1), but it's not important here
            }
        }
        if(k == 0) matrixInit.print();
        matrixInit.invert();
        if(k == 0) matrixInit.print();
        //set coeffs
        for(int i = 0; i < lst.length(); ++i)
        {
            coefficients[k][i] = - matrixInit[i][lst.length()]
                                 / matrixInit[lst.length()][lst.length()];
        }
        signalNums.pop_back();
    }

    ofstream outStr;
    outStr.open((def::dir->absolutePath()
                + slash() + "eyes.txt").toStdString());
    outStr << "NumOfEyesChannels " << lst.length() << endl;
    outStr << "NumOfEegChannels " << ui->spinBox->value() << endl;
    for(int k = 0; k < ui->spinBox->value(); ++k)
    {
        for(int i = 0; i < lst.length(); ++i)
        {
            outStr << doubleRound(coefficients[k][i], 3) << "\t";
        }
        outStr << endl;
    }
    outStr.close();

    //automatization
    if(!autoFlag) QMessageBox::information((QWidget*)this,
                                           tr("Info"),
                                           tr("Coeffs ready"),
                                           QMessageBox::Ok);
    cout << "eyesProcessing: time elapsed = " << myTime.elapsed() / 1000. << " sec" << endl;
}

Eyes::~Eyes()
{
    delete ui;
}
