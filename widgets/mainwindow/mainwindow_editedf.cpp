#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"


#include <other/defs.h>
#include <myLib/output.h>
#include <myLib/mati.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

std::vector<int> MainWindow::makeChanList() const
{
	std::vector<int> chanList{};
    QStringList lst = ui->reduceChannelsLineEdit->text().split(QRegExp("[,.; ]"), QString::SkipEmptyParts);
	for(const auto & str : lst)
    {
        chanList.push_back(str.toInt() - 1);
    }
	return chanList;
}


void MainWindow::cleanEdfFromEyesSlot()
{
    readData();
	QString helpString = globalEdf.getFilePath();
	helpString.replace(".edf", "_eyesClean.edf", Qt::CaseInsensitive);

	globalEdf.cleanFromEyes().writeEdfFile(helpString);
}

void MainWindow::rereferenceDataSlot()
{
	QTime myTime;
	myTime.start();

	QString helpString = globalEdf.getFilePath();
	helpString.replace(".edf", "_rr.edf", Qt::CaseInsensitive);
	globalEdf.rereferenceData(strToRef.at(ui->rereferenceDataComboBox->currentText()),
							  ui->eogAsIsCheckBox->isChecked(),
							  ui->eogBipolarCheckBox->isChecked());

	if(1)
	{
		globalEdf.writeEdfFile(helpString);
	}
	else
	{
		/// do nothing
	}
	outStream << "rereferenceDataSlot: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

#if 0
//// check remake to do
void MainWindow::rereferenceCARSlot()
{
	QTime myTime;
	myTime.start();

	QString helpString = globalEdf.getFilePath();

#if 0
	/// Encephalan
	globalEdf.rereferenceData(reference::N, false, false);
	const auto & usedChannels = coords::lbl19;	/// to build reref array
	const auto & rerefChannels = coords::lbl21;	/// list to reref (with EOG)
#else
	/// Geodesics
	auto usedChannels = smLib::range<std::vector<int>>(0, globalEdf.getNs() - 2);	/// to build reref array
	auto rerefChannels = smLib::range<std::vector<int>>(0, globalEdf.getNs() - 2);	/// list to reref (with EOG)
#endif

	/// refArr = (Fp1 + Fp2 + ... + O1 + O2)/19 - N

	std::valarray<double> refArr(globalEdf.getDataLen());
	for(const auto & chan : usedChannels) /// works for both int and QString
	{
		refArr += globalEdf.getData(chan);
	}
	refArr /= usedChannels.size();

#if 0
	/// old 20-Apr-18
	for(int i = 0; i < globalEdf.getNs(); ++i)
	{
		auto it = std::find_if(std::begin(rerefChannels),
							   std::end(rerefChannels),
							   [this, i](const QString & in)
		{ return globalEdf.getLabels(i).contains(in); });

		if(it != std::end(rerefChannels))
		{
			if(!(*it).contains("EOG"))
			{
				globalEdf.setData(i, globalEdf.getData(i) - refArr);
			}
			else
			{
				/// N-EOG1, N-EOG2
				/// crutch because of inversed EOG in Encephalan
				globalEdf.setData(i, globalEdf.getData(i) + refArr);
			}

			/// set new label *-CAR
			QString newLabel = globalEdf.getLabels(i);
			newLabel = myLib::fitString(newLabel.left(newLabel.indexOf('-') + 1) + "CAR", 16);
			globalEdf.setLabel(i, newLabel);
		}
	}
#else
	/// new 20-Apr-18
	for(int rerefChannel : rerefChannels)
	{
		int chan = globalEdf.findChannel(rerefChannel);

		if(!globalEdf.getLabels(chan).contains("EOG"))
		{
			/// EEG
			globalEdf.setData(chan, globalEdf.getData(chan) - refArr);
		}
		else
		{
			/// N-EOG1, N-EOG2
			/// crutch because of inversed EOG in Encephalan
			globalEdf.setData(chan, globalEdf.getData(chan) + refArr);
		}

		/// set new label *-CAR
		QString newLabel = globalEdf.getLabels(chan);
		newLabel = myLib::fitString(newLabel.left(newLabel.indexOf('-') + 1) + "CAR", 16);
		globalEdf.setLabel(chan, newLabel);
	}
#endif
	helpString.replace(".edf", "_car.edf", Qt::CaseInsensitive);
	if(1)
	{
		globalEdf.writeEdfFile(helpString);
	}
	else
	{
		/// do nothing
	}
	outStream << "rereferenceDataCAR: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}
#endif

#if 0
void MainWindow::rereferenceData(reference newRef)
{
	/// globalEdf is const

	/// A1, A2, Ar, N
	/// A1-A2, A1-N
    /// Ar means 0.5*(A1+A2)

	int groundChan = -1;	/// A1-N
	int earsChan1 = -1;		/// A1-A2
	int earsChan2 = -1;		/// A2-A1
	int eog1 = -1;			/// EOG1
	int eog2 = -1;			/// EOG2

	auto label = globalEdf.getLabels();
	for(int i = 0; i < globalEdf.getNs(); ++i)
    {
		if(label[i].contains("A1-N"))		{ groundChan = i; }
		else if(label[i].contains("A1-A2"))	{ earsChan1 = i; }
		else if(label[i].contains("A2-A1"))	{ earsChan2 = i; }
		else if(label[i].contains("EOG1"))	{ eog1 = i; }
		else if(label[i].contains("EOG2"))	{ eog2 = i; }
    }
    if(groundChan == -1 || (earsChan1 == -1 && earsChan2 == -1))
    {
		outStream << "rereferenceData: some of ref channels are absent" << std::endl;
        return;
    }

	int earsChan{};
	std::vector<QString> sign{};
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

	QString helpString{};
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
				/// EOG inversion is made in edfFile::reduceChannels
				/// here deal with them like EOG*-A*

				if(globalEdf.getEogType() == eogType::cross)
				{
					/// (EOG1-A2) - (EOG2-A1) - (A1-A2)
					helpString += nm(eog1 + 1) + "-" + nm(eog2 + 1) + sign[0] + nm(earsChan + 1) + " ";
				}
				else if(globalEdf.getEogType() == eogType::correspond)
				{
					/// (EOG1-A1) - (EOG2-A2) + (A1-A2)
					helpString += nm(eog1 + 1) + "-" + nm(eog2 + 1) + sign[1] + nm(earsChan + 1) + " ";
				}
			}
			else { helpString += currNumStr + " "; }
		}
		else /// EEG and usual EOG
        {
            /// define current ref
            QRegExp forRef(R"([\-].{1,4}[ ])");
            forRef.indexIn(label[i]);
            QString refName = forRef.cap();
            refName.remove(QRegExp(R"([\-\s])"));

			/// if no reference found - leave as is
			if(strToRef.count(refName) == 0) { helpString += currNumStr + " "; continue; }

			QString chanName = myLib::getLabelName(label[i]);

			reference targetRef = newRef;
			if(newRef == reference::Base)
            {
                if(std::find(std::begin(coords::lbl_A1),
                             std::end(coords::lbl_A1),
                             chanName) != std::end(coords::lbl_A1))
                {
					targetRef = reference::A1;
                }
                else
                {
					targetRef = reference::A2;
                }
            }
			helpString += edfFile::rerefChannel(strToRef.at(refName),
												targetRef,
												currNumStr,
												earsChanStr,
												groundChanStr,
												sign) + " ";
			label[i].replace(refName, refToStr.at(targetRef));
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

	/// the very processing
	edfFile fil = globalEdf.reduceChannels(ui->reduceChannelsLineEdit->text());
	fil.setLabels(label); /// necessary after the processing

	/// inverse EOG2-EOG1 back (look edfFile::reduceChannels near the end)
	auto a = fil.findChannel("EOG1-EOG2");
	if(a != -1)
	{
		fil.multiplyChannel(a, -1.);
	}
	else
	{
		outStream << "rereferenceData: no bipolar EOG" << std::endl;
	}

	/// set back channels string
	ui->reduceChannelsLineEdit->setText(ui->reduceChannelsComboBox->currentData().toString());
}
#endif

void MainWindow::refilterDataSlot()
{
	/// globalEdf is const
    QTime myTime;
    myTime.start();

    const double lowFreq = ui->lowFreqFilterDoubleSpinBox->value();
    const double highFreq = ui->highFreqFilterDoubleSpinBox->value();
    const bool notch = ui->notchCheckBox->isChecked();

	QString helpString = globalEdf.getFilePath();
    if(!notch)
    {
        helpString.replace(".edf",
                           "_f"
						   + nm(lowFreq) + '-' + nm(highFreq)
						   + ".edf"
						   , Qt::CaseInsensitive
						   );
    }
    else
    {
        helpString.replace(".edf",
						   "_n"
						   + nm(lowFreq) + '-' + nm(highFreq)
						   + ".edf"
						   , Qt::CaseInsensitive
						   );
    }

	edfFile fil(globalEdf);
	fil.refilter(lowFreq, highFreq, notch);

	if(1)
	{
		fil.writeEdfFile(helpString);
	}
	else
	{
		/// do nothing
	}
	outStream << "refilterDataSlot: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void MainWindow::reduceChannelsEDFSlot() const
{
	/// globalEdf is const
	QTime myTime;
	myTime.start();

	edfFile fil = globalEdf.reduceChannels(this->makeChanList());

	QString helpString = globalEdf.getFilePath();
	if(ui->reduceChannelsComboBox->currentText() == "other,mark")
	{
		helpString.replace(".edf", "_veget.edf", Qt::CaseInsensitive);
	}
	else
	{
		helpString.replace(".edf", "_rdc.edf", Qt::CaseInsensitive);
	}


	if(1)
	{
		fil.writeEdfFile(helpString);
	}
	else
	{
		/// do nothing
	}

	/// why is there error here ???????????????????????
	/// "string literal doesn't decay into pointer"
//	outStream << "reduceChannelsEDF: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
	/// but this kinda works
//	outStream << "refilterDataSlot: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

/// Ossadtchi only ?
void MainWindow::reduceChannelsSlot()
{
	/// reduce channels in Reals
#if 0
    QStringList lst;
    matrix dataR;

    std::set<int, std::greater<int>> excludeList;
	for(int i = 0; i < DEFS.getNs(); ++i)
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
	outStream << "reduceChannelsSlot: excludeList = ";
	for(int in : excludeList)
    {
		outStream << in << "  ";
    }
	outStream << std::endl;

	QDir localDir(DEFS.dirPath());
	localDir.cd("Reals");
    lst = localDir.entryList(QDir::Files, QDir::NoSort);


    int localNs;
    for(const QString & fileName : lst)
    {
		localNs = DEFS.getNs();
        helpString = (localDir.absolutePath()
											  + "/" + fileName);
		dataR = edfFile(helpString).getData();
		localNs = dataR.rows(); /// needed?
        for(int exclChan : excludeList)
        {
            dataR.eraseRow(exclChan);
            --localNs;
        }
		/// remake with edfs
//		myLib::writePlainData(helpString, dataR);
    }

	DEFS.setNs(DEFS.getNs() - excludeList.size());
	outStream << "channels reduced" << std::endl;
#endif
}
