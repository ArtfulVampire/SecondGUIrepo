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
	// A1, A2, Ar, N
	// A1-A2, A1-N
    // Ar means 0.5*(A1+A2)

    QTime myTime;
    myTime.start();

	QString helpString;

    readData();
	auto label = globalEdf.getLabels();

	int groundChan = -1;	// A1-N
	int earsChan1 = -1;		// A1-A2
	int earsChan2 = -1;		// A2-A1
	int eog1 = -1;			// EOG1
	int eog2 = -1;			// EOG2
	/// EOG references unclear, look edfFile::handleEdfFile()

    for(int i = 0; i < def::ns; ++i)
    {
		if(label[i].contains("A1-N"))		{ groundChan = i; }
		else if(label[i].contains("A1-A2"))	{ earsChan1 = i; }
		else if(label[i].contains("A2-A1"))	{ earsChan2 = i; }
		else if(label[i].contains("EOG1"))	{ eog1 = i; }
		else if(label[i].contains("EOG2"))	{ eog2 = i; }
    }
    if(groundChan == -1 || (earsChan1 == -1 && earsChan2 == -1))
    {
		std::cout << "rereferenceData: some of ref channels are absent" << std::endl;
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

	const QString earsChanStr = nm(earsChan + 1);
	const QString groundChanStr = nm(groundChan + 1);


    for(int i = 0; i < def::ns; ++i) //ns -> 21
    {
		const QString currNumStr = nm(i + 1);

		if(i == groundChan || i == earsChan1 || i == earsChan2) /// reref chans
		{
			helpString += currNumStr + " ";
		}
		else if(!label[i].contains(QRegExp("E[EO]G"))) /// not EOG, not EEG
        {
            helpString += currNumStr + " ";
		}
		else if(label[i].contains("EOG") && ui->eogAsIsCheckBox->isChecked())
		{
			helpString += currNumStr + " ";
		}
		else if(label[i].contains("EOG") && ui->eogBipolarCheckBox->isChecked())
		{
			if(label[i].contains("EOG1")) { /* do nothing */ }
			else if(label[i].contains("EOG2")) /// make bipolar EOG1-EOG2
			{
				/// sign[0] for EOG1-A2; sign[1] for EOG1-A1
				/// sign[1] for sure 20.06.2017, look also edfFile::handleEdfFile()
				helpString += nm(eog1 + 1) + "-" + nm(eog2 + 1) + sign[1] + nm(earsChan + 1) + " ";
			}
			else { helpString += currNumStr + " "; }
		}
		else /// EEG and usual EOG
        {
            // define current ref
            QRegExp forRef(R"([\-].{1,4}[ ])");
            forRef.indexIn(label[i]);
            QString refName = forRef.cap();
            refName.remove(QRegExp(R"([\-\s])"));

			/// if no reference found - leave as is
			if(refName.isEmpty()) { helpString += currNumStr + " "; }

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

			label[i].replace(refName, targetRef);
		}

    }

	// fix EOG1-EOG2 label when bipolar
	/// generality
	if(ui->eogBipolarCheckBox->isChecked())
	{
		/// erase EOG1-A1
		label.erase(std::find_if(std::begin(label),
								 std::end(label),
								 [](const QString & in)
		{ return in.contains("EOG1-"); }));

		/// insert EOG1-EOG2
		label.insert(std::find_if(std::begin(label),
								  std::end(label),
								  [](const QString & in)
		 { return in.contains("EOG2-"); }),
					 myLib::fitString("EOG EOG1-EOG2", 16));

		/// erase EOG2-A2
		label.erase(std::find_if(std::begin(label),
								 std::end(label),
								 [](const QString & in)
		{ return in.contains("EOG2-"); }));
	}
//	std::cout << "rereferenceData: " << newRef << "\n"
//			  << helpString.toStdString() << std::endl;

    ui->reduceChannelsLineEdit->setText(helpString);

	// change labels
//	std::cout << label << std::endl;
	globalEdf = globalEdf.reduceChannels(ui->reduceChannelsLineEdit->text());
    globalEdf.setLabels(label);
	def::ns = globalEdf.getNs();

	// set back channels string
	ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->currentData().toString());

	globalEdf.writeEdfFile(newPath);
	std::cout << "rereferenceData: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
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
						   + nm(lowFreq) + '-' + nm(highFreq)
                           + ".edf");
    }
    else
    {
        helpString.replace(".edf",
						   "_n"
						   + nm(lowFreq) + '-' + nm(highFreq)
                           + ".edf");
    }
	globalEdf.refilter(lowFreq, highFreq, helpString, notch);
	std::cout << "refilterDataSlot: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
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
#if 0
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
	helpString += nm(def::ns);
    ui->textEdit->append(helpString);

	std::cout << "reduceChannelsSlot: finished";
#endif
}

/// to deprecate
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

/// to deprecate
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