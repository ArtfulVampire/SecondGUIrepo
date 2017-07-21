#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/mati.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

std::vector<int> MainWindow::makeChanList()
{
	std::vector<int> chanList{};
    QStringList lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
    for(auto str : lst)
    {
        chanList.push_back(str.toInt() - 1);
    }
	return chanList;
}


void MainWindow::cleanEdfFromEyesSlot()
{
    readData();
    globalEdf.cleanFromEyes();
	QString helpString = globalEdf.getFilePath();
	helpString.replace(".edf", "_eyesClean.edf", Qt::CaseInsensitive);
    globalEdf.writeEdfFile(helpString);
}

void MainWindow::rereferenceDataSlot()
{
	QTime myTime;
	myTime.start();

	QString helpString = globalEdf.getFilePath();
	helpString.replace(".edf", "_rr.edf", Qt::CaseInsensitive);
	rereferenceData(ui->rereferenceDataComboBox->currentText());

	if(1)
	{
		globalEdf.writeEdfFile(helpString);
	}
	else
	{
		/// do nothing
	}
	std::cout << "rereferenceData: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}



void MainWindow::rereferenceData(const QString & newRef)
{
	/// could be in edfFile
	// A1, A2, Ar, N
	// A1-A2, A1-N
    // Ar means 0.5*(A1+A2)



	int groundChan = -1;	// A1-N
	int earsChan1 = -1;		// A1-A2
	int earsChan2 = -1;		// A2-A1
	int eog1 = -1;			// EOG1
	int eog2 = -1;			// EOG2
	/// EOG references unclear, look edfFile::handleEdfFile()

	auto label = globalEdf.getLabels();
//	std::cout << label << std::endl;
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

	QString helpString;
	for(int i = 0; i < globalEdf.getNs(); ++i)
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
				if(globalEdf.getEogType() == eogType::cross)
				{
					/// sign[0] for EOG1-A2; sign[1] for EOG1-A1
					helpString += nm(eog1 + 1) + "-" + nm(eog2 + 1) + sign[0] + nm(earsChan + 1) + " ";
				}
				else if(globalEdf.getEogType() == eogType::correspond)
				{
					/// sign[1] for sure 20.06.2017, look also edfFile::handleEdfFile()
					helpString += nm(eog1 + 1) + "-" + nm(eog2 + 1) + sign[1] + nm(earsChan + 1) + " ";
				}
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

	/// fix EOG1-EOG2 label when bipolar
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
    ui->reduceChannelsLineEdit->setText(helpString);
	std::cout << helpString << std::endl;

	// change labels
	globalEdf = globalEdf.reduceChannels(ui->reduceChannelsLineEdit->text());
    globalEdf.setLabels(label);
	def::ns = globalEdf.getNs();

	// set back channels string
	ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->currentData().toString());
}

void MainWindow::refilterDataSlot()
{

    QTime myTime;
    myTime.start();

    const double lowFreq = ui->lowFreqFilterDoubleSpinBox->value();
    const double highFreq = ui->highFreqFilterDoubleSpinBox->value();
    const bool notch = ui->notchCheckBox->isChecked();

	readData();

	QString helpString = globalEdf.getFilePath();
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

	globalEdf.refilter(lowFreq, highFreq, notch);

	if(1)
	{
		globalEdf.writeEdfFile(helpString);
	}
	else
	{
		/// do nothing
	}
	std::cout << "refilterDataSlot: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void MainWindow::reduceChannelsEDFSlot()
{
	QTime myTime;
	myTime.start();

	QString helpString = globalEdf.getFilePath();
	helpString.replace(".edf", "_rdc.edf", Qt::CaseInsensitive);

	globalEdf = globalEdf.reduceChannels(this->makeChanList());

	if(1)
	{
		globalEdf.writeEdfFile(helpString);
	}
	else
	{
		/// do nothing
	}

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