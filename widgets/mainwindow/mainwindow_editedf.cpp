#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/mati.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

void MainWindow::makeChanList(std::vector<int> & chanList)
{
    chanList.clear();
    QStringList lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(auto str : lst)
    {
        chanList.push_back(str.toInt() - 1);
    }
}


void MainWindow::cleanEdfFromEyesSlot()
{
    readData();
    globalEdf.cleanFromEyes();
    QString helpString = def::dir->absolutePath()
						 + "/" + def::ExpName + "_eyesClean.edf";
    globalEdf.writeEdfFile(helpString);
}

void MainWindow::rereferenceDataSlot()
{
	QString helpString = def::dir->absolutePath() + "/" + def::ExpName + ".edf"; //ui->filePathLineEdit->text()
    helpString.replace(".edf", "_rr.edf");
    rereferenceData(ui->rereferenceDataComboBox->currentText(), helpString);
}



void MainWindow::rereferenceData(const QString & newRef,
								 const QString & newPath)
{
    //A1, A2, Ar, N
    //A1-A2, A1-N
    // Ar means 0.5*(A1+A2)

    QTime myTime;
    myTime.start();

    QString helpString;
    QString helpString2;

    readData();
	auto label = globalEdf.getLabels();

    std::vector<int> chanList(def::ns);
    std::iota(chanList.begin(),
              chanList.end(),
              0);

    int groundChan = -1; //A1-N
    int earsChan1 = -1; //A1-A2
    int earsChan2 = -1; //A2-A1
	std::cout << def::ns << std::endl;
    for(int i = 0; i < def::ns; ++i)
    {
        if(label[i].contains("A1-N"))
        {
            groundChan = i;
        }
        else if(label[i].contains("A1-A2"))
        {
            earsChan1 = i;
        }
        else if(label[i].contains("A2-A1"))
        {
            earsChan2 = i;
        }
    }
    if(groundChan == -1 || (earsChan1 == -1 && earsChan2 == -1))
    {
		std::cout << "some of reref channels are absent" << std::endl;
        return;
    }

    int earsChan;
    std::vector<QString> sign;
    if(earsChan1 != -1)
    {
        earsChan = earsChan1;
        sign = {"-", "+"};
    }
    else
    {
        earsChan = earsChan2;
        sign = {"+", "-"};
    }

    const QString earsChanStr = QString::number(earsChan + 1);
    const QString groundChanStr = QString::number(groundChan + 1);

    for(int i = 0; i < def::ns; ++i) //ns -> 21
    {
        const QString currNumStr = QString::number(i + 1);

        if((label[i].contains("EOG") && ui->eogAsIsCheckBox->isChecked()) ||
           (i == groundChan || i == earsChan))
        {
            helpString += currNumStr + " "; continue;
        }

        if(!label[i].contains(QRegExp("E[EO]G"))) /// generality
        {
            helpString += currNumStr + " ";
        }
        else
        {
            // define current ref
            QRegExp forRef(R"([\-].{1,4}[ ])");
            forRef.indexIn(label[i]);
            QString refName = forRef.cap();
            refName.remove(QRegExp(R"([\-\s])"));

			QString chanName = myLib::getLabelName(label[i]);

            QString targetRef = newRef;
            /// if newRef == "Base"
            if(!(newRef == "A1" ||
                 newRef == "A2" ||
                 newRef == "Ar" ||
                 newRef == "N"))
            {
                if(std::find(std::begin(coords::lbl_A1),
                             std::end(coords::lbl_A1),
                             chanName) != std::end(coords::lbl_A1))
                {
                    targetRef = "A1";
                }
                else
                {
                    targetRef = "A2";
                }
            }
			helpString += myLib::rerefChannel(refName,
											  targetRef,
											  currNumStr,
											  earsChanStr,
											  groundChanStr,
											  sign) + " ";

            helpString2 = label[i];
            helpString2.replace(refName, targetRef);
            label[i] = helpString2;
        }
    }
	std::cout << "rereferenceData: " << newRef << "\n" << helpString.toStdString() << std::endl;
    ui->reduceChannelsLineEdit->setText(helpString);

    //change labels
    globalEdf.setLabels(label);
	globalEdf = globalEdf.reduceChannels(ui->reduceChannelsLineEdit->text());
	def::ns = globalEdf.getNs();

    //set all of channels to the lineedit
    helpString.clear();
    for(int i = 0; i < def::ns; ++i)
    {
        helpString += QString::number(i + 1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);

    globalEdf.writeEdfFile(newPath);
	std::cout << "rereferenceData: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
}

void MainWindow::refilterDataSlot()
{

    QTime myTime;
    myTime.start();

    const double lowFreq = ui->lowFreqFilterDoubleSpinBox->value();
    const double highFreq = ui->highFreqFilterDoubleSpinBox->value();
    const bool notch = ui->notchCheckBox->isChecked();

    QString helpString = def::dir->absolutePath()
			+ "/" + def::ExpName + ".edf"; //ui->filePathLineEdit->text()
    readData();
	/// EDF case sensitivity !!!
    if(!notch)
    {
        helpString.replace(".edf",
                           "_f"
                           + QString::number(lowFreq) + '-' + QString::number(highFreq)
                           + ".edf");
    }
    else
    {
        helpString.replace(".edf",
						   "_n"
                           + QString::number(lowFreq) + '-' + QString::number(highFreq)
                           + ".edf");
    }

    refilterData(lowFreq, highFreq, helpString, notch);
    int tmp = ui->reduceChannelsComboBox->currentIndex();
    ui->reduceChannelsComboBox->setCurrentIndex(0);
    ui->reduceChannelsComboBox->setCurrentIndex(tmp);

	std::cout << "refilterDataSlot: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void MainWindow::refilterData(double lowFreq,
                              double highFreq,
                              const QString & newPath,
                              bool notch)
{
    globalEdf.refilter(lowFreq, highFreq, newPath, notch);
}

void MainWindow::reduceChannelsEDFSlot()
{
    QString helpString;
    helpString = def::dir->absolutePath()
				 + "/" + def::ExpName + "_rdc.edf";
    reduceChannelsEDF(helpString);
}

void MainWindow::reduceChannelsEDF(const QString & newFilePath)
{
    QTime myTime;
    myTime.start();
    edfFile temp;
    temp.readEdfFile(ui->filePathLineEdit->text());

	std::vector<int> chanList;
    makeChanList(chanList);

	temp = temp.reduceChannels(chanList);
    temp.writeEdfFile(newFilePath);

	std::cout << "reduceChannelsEDF: time = " << myTime.elapsed()/1000. << " sec" << std::endl;
}

/// Ossadtchi only ?
void MainWindow::reduceChannelsSlot()
{
#if 1
	// reduce channels in Reals
    QStringList lst;
    matrix dataR;

    std::set<int, std::greater<int>> excludeList;
    for(int i = 0; i < def::ns; ++i)
    {
        excludeList.emplace(i);
    }


    QString helpString = ui->reduceChannelsLineEdit->text();
    QStringList leest = helpString.split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(const QString & chanStr : leest)
    {
        excludeList.erase(std::find(excludeList.begin(),
                                    excludeList.end(),
                                    chanStr.toInt() - 1));
    }
	std::cout << "reduceChannelsSlot: excludeList = ";
    for(int in : excludeList)
    {
		std::cout << in << "  ";
    }
	std::cout << std::endl;

    QDir localDir(def::dir->absolutePath());
	localDir.cd("Reals");
    lst = localDir.entryList(QDir::Files, QDir::NoSort);


    int localNs;
    for(const QString & fileName : lst)
    {
        localNs = def::ns;
        helpString = (localDir.absolutePath()
											  + "/" + fileName);
		myLib::readPlainData(helpString, dataR);
		localNs = dataR.rows(); /// needed?
        for(int exclChan : excludeList)
        {
            dataR.eraseRow(exclChan);
            --localNs;
        }
		myLib::writePlainData(helpString, dataR);
    }

    def::ns -= excludeList.size();

    helpString = "channels reduced ";
    ui->textEdit->append(helpString);

    helpString = "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);

	std::cout << "reduceChannelsSlot: finished";
#endif
}


void MainWindow::concatenateEDFs(QStringList inPath, QString outPath)
{
    if(inPath.isEmpty())
    {
		std::cout << "concatenateEDFs: input list is empty" << std::endl;
        return;
    }
    QTime myTime;
    myTime.start();

    //assume the files are concatenable
    edfFile * resultEdf = new edfFile;
    resultEdf->readEdfFile(inPath[0]);
    for(int k = 1; k < inPath.length(); ++k)
    {
        resultEdf->concatFile(inPath[k]);
    }
    resultEdf->writeEdfFile(outPath);
    delete resultEdf;
	std::cout << "concatenateEDF: " << myLib::getFileName(outPath)
			  << "\ttime = " << myTime.elapsed()/1000. << " sec" << std::endl;
}

void MainWindow::concatenateEDFs(QString inPath1, QString inPath2, QString outPath)
{
    QTime myTime;
    myTime.start();
    QStringList lst;
    lst << inPath1 << inPath2;
    concatenateEDFs(lst, outPath);
	std::cout << "concatenateEDFs: " << myLib::getFileName(outPath)
			  << "\ttime = " << myTime.elapsed()/1000. << " sec" << std::endl;
}

/// to deprecate
void MainWindow::constructEDFSlot()
{
    QTime myTime;
    myTime.start();
    QString helpString;
    QStringList filters;
    if(!ui->matiCheckBox->isChecked())
    {
		helpString = def::dir->absolutePath() + "/" + def::ExpName + "_new.edf";
        constructEDF(helpString);
    }
    else //if(ui->matiCheckBox->isChecked())
    {
        const QString initEDF = ui->filePathLineEdit->text();
        helpString = def::dir->absolutePath()
				+ "/" + def::ExpName.left(3)
                + "_splitZerosLog.txt";

		std::ofstream outStream;
        outStream.open(helpString.toStdString());
        outStream << def::ExpName.left(3).toStdString() << "\t";
        outStream << "type" << "\t";
        outStream << "sessn" << "\t";
        outStream << "offset" << "\t";
		outStream << "offsetS" << std::endl;
        outStream.close();

        for(int i = 0; i < 3; ++i) //every type 0-count, 1-track, 2-composed, 3-rest
        {
            setEdfFile(initEDF);
            for(int j = 0; j < 6; ++j) //every session
            {

                filters.clear();
				// filter for Reals
                helpString = def::ExpName
                        + "_" + QString::number(i)
                        + "_" + QString::number(j)
                        + "*";
                filters << helpString;

                // outPath for session edfs
                helpString = def::dir->absolutePath()

						+ "/auxEdfs"

						+ "/" + def::ExpName
                        + "_c"
                        + "_" + QString::number(i)
                        + "_" + QString::number(j)
                        + ".edf";

				constructEDF(helpString, filters); // construct 1 session from Reals
                ui->progressBar->setValue(100. *
                                          (1. / 3. * (i + 1. / 6. * j)));
                qApp->processEvents();
            }

            def::dir->cd("auxEdfs");
            // template for session edfs
            helpString = def::ExpName
                    + "_c"
                    + "_" + QString::number(i)
                    + "_*.edf";
            QStringList lst = def::dir->entryList(QStringList(helpString));  //filter for edfs

            for(int k = 0; k < lst.length(); ++k)
            {
				lst[k].prepend( def::dir->absolutePath() + "/" );
            }

//            def::dir->cdUp(); // if save concatenated into def::ExpName dir

            //outPath for concatenated
            helpString = def::dir->absolutePath()
					+ "/" + def::ExpName.left(3)
                    + "_" + QString::number(i)
                    + ".edf";

//            def::dir->cdUp(); // if save concatenated into auxEdfs

            concatenateEDFs(lst, helpString);

            setEdfFile(initEDF);
        }

        // concatenate all session files
        helpString = def::dir->absolutePath()
					 + "/" + def::ExpName.left(3)
                     + "_cl";
        if(ui->splitZerosCheckBox->isChecked()) helpString += "_nz";
        helpString += ".edf";

        def::dir->cd("auxEdfs");
        QStringList lst;
        for(auto i : {"_0.edf", "_1.edf", "_2.edf"})
        {
			lst << def::dir->absolutePath() + "/" + def::ExpName.left(3) + i;
        }
        concatenateEDFs(lst, helpString);
        setEdfFile(initEDF);
    }
    ui->progressBar->setValue(0);
	std::cout << "constructEdf: FULL time = " << myTime.elapsed()/1000. << " sec" << std::endl;

    helpString = "constructEdf finished\n";
    helpString += "ns equals to ";
    helpString += QString::number(def::ns);
    ui->textEdit->append(helpString);
}

/// check this sheet
void MainWindow::constructEDF(const QString & newPath,
                              const QStringList & nameFilters)
{
	// all the Reals, to newPath based on ui->filePathLineEdit
    QString helpString;

    QTime myTime;
    myTime.start();

    readData(); // read globalEdf based on ui->filePathEdit to renew the initial channels list

    std::vector<int> chanList;
    makeChanList(chanList);

    def::ns = chanList.size();
    const int ns = chanList.size();

    QStringList lst;
    if(!nameFilters.isEmpty())
    {
		lst = QDir(def::dir->absolutePath() + "/Reals").entryList(
                  nameFilters, QDir::Files, QDir::Name); /// Name ~ order
    }
    else
    {
		lst = QDir(def::dir->absolutePath() + "/Reals").entryList(
                  QDir::Files, QDir::Name); /// Name ~ order
    }
    if(lst.isEmpty())
    {
		std::cout << "constructEDF: list of Reals is empty. filter[0] = " << nameFilters[0].toStdString() << std::endl;
        return;
    }


	matrix newData = matrix();

    for(const QString & fileName : lst)
    {
        helpString = (def::dir->absolutePath()
											  + "/Reals"
											  + "/" + fileName);
		/// changed 9.3.17
		myLib::readPlainData(helpString, newData, newData.cols());
	}
	int helpInt = newData.cols();



	int offset = 0;
	int currSlice = newData.cols();
    if(ui->matiCheckBox->isChecked()) // bicycle generality
    {
		QString fileName = myLib::getFileName(newPath);
        helpString = def::dir->absolutePath()
					 + "/" + def::ExpName.left(3)
                     + "_splitZerosLog.txt";

		myLib::splitZeros(newData, helpInt, currSlice, helpString, fileName); // helpString unchanged

		std::ofstream outStream;
		outStream.open(helpString.toStdString(), std::ios_base::app);

        if(ui->roundOffsetCheckBox->isChecked())
        {
            if(currSlice < 16 * def::freq)
            {
				std::cout << "too little data to construct edf" << std::endl;
                return;
            }

            const double saveMarker = newData[ns - 1][0];
            // Mati offset ~20 seconds in the beginning
            offset = currSlice % int(16 * def::freq) + 16 * def::freq;

            if(currSlice <= offset)
            {
				std::cout << "constructEDF: too little data " << currSlice << std::endl;
                return;
            }


            for (int i = 0; i < ns; ++i) //shift start pointers
            {
                // vector
//                newData[i].erase(newData[i].begin(),
//                                 newData[i].begin() + offset);
                // valarray
                std::valarray<double> newArr = newData[i][std::slice(offset,
                                                        newData[i].size() - offset,
                                                        1)];
                newData[i] = newArr;
//                std::remove_if(begin(newData[i]),
//                               begin(newData[i]) + offset,
//                               [](double){return true;});
            }

            newData[ns - 1][0] = saveMarker;
            currSlice -= offset;

            //write the exclusion of the first part [start, finish)
            outStream << fileName.toStdString() << "\t";
            outStream << 0 << "\t"; // first bin to exclude
            outStream << offset << "\t"; // first bin NOT TO exclude
            outStream << offset << "\t"; // length

            outStream << "0.000" << "\t"; // start time to exclude
            outStream << offset / def::freq << "\t"; // first time NOT TO exclude
			outStream << offset / def::freq << std::endl << std::endl; // length
            outStream.close();
        }



        //set the last marker if it's not
        //fix the first resting file
        if(newData[ns - 1][0] == 0)
        {
			newData[ns - 1][0] = myLib::matiCountDecimal("0000 0101 1000 0000"); //as a count session end
        }
        else
        {
//            matiPrintMarker(newData[ns - 1][0], "after if == 0");
        }

        double & firstMarker = newData[ns - 1][0];
        double & lastMarker = newData[ns - 1][currSlice -  1];
		myLib::matiFixMarker(firstMarker); //fix the start marker for this small edf file
		myLib::matiFixMarker(lastMarker);  //fix the last  marker for this small edf file

        //if not one of them is the end of some session
		if( (myLib::matiCountBit(firstMarker, 10) == myLib::matiCountBit(lastMarker, 10))
		   || lastMarker == 0)
        {
            lastMarker = firstMarker
					+ pow(2, 10) * (myLib::matiCountBit(firstMarker, 10) ? -1 : 1); //adjust the last marker
        }
    }
    else if(ui->splitZerosCheckBox->isChecked())
    {
		myLib::splitZeros(newData, helpInt, currSlice);
    }

    /// remake with dataType

//    std::cout << "constructEDF: before writeData\n"
//         << "rows = " << newData.rows() << "   "
//         << "cols = " << newData.cols() << "   "
//         << std::endl;
    globalEdf.writeOtherData(newData, newPath, chanList); // new to check

//    def::ns = globalEdf.getNs(); /// should test

	std::cout << "constructEDF: " << myLib::getFileName(newPath)
			  << "\ttime = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}