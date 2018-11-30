#include <widgets/cut.h>
#include "ui_cut.h"

#include <other/coords.h>
#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/output.h>

#include <QFileDialog>

using namespace myOut;

void Cut::next()
{
	++fileListIter;
	if(fileListIter == std::end(filesList))
	{
		--fileListIter;
		std::cout << "prev: bad number, too big" << std::endl;
		return;
	}
	openFile(edfFil.getDirPath() + "/" + (*fileListIter));
}

void Cut::prev()
{
	if(fileListIter == std::begin(filesList))
	{
		std::cout << "prev: bad number, too little" << std::endl;
		return;
	}
	--fileListIter;
	openFile(edfFil.getDirPath() + "/" + (*fileListIter));
}

void Cut::saveAs(const QString & addToName)
{
	QString newPath = edfFil.getFilePath();
	newPath.insert(newPath.lastIndexOf('.'), addToName);
	edfFil.writeOtherData(dataCutLocal, newPath);
}

void Cut::saveSlot()
{
	if( !fileOpened ) { return; }

	this->saveAs("_new");
	logAction(static_cast<std::string>("saveAs"), static_cast<std::string>("_new"));
	std::cout << "Cut::save: edfFile saved" << std::endl;
#if 0
	/// old
	if(myFileType == fileType::real)
	{
		QString helpString = edfFil.getDirPath()
							 + "/cut"
							 + "/" + myLib::getFileName(currentFile);

		/// new
		myLib::writePlainData(helpString, dataCutLocal);
	}
	else if(myFileType == fileType::edf)
	{
		this->saveAs("_new");
		/// problem with template function definition
		/// "string literal doesn't decay into pointer"
//		logAction("saveAs", "_new");
		std::cout << "Cut::save: edfFile saved" << std::endl;
	}
#endif
}

void Cut::saveSubsecSlot()
{
	if( !fileOpened ) { return; }
#if 01
	/// check subsec if too long
	/// turn off for SummerPractice2018 to make 30 min edfs
	if(checkBadRange(ui->leftLimitSpinBox->value(),
					  ui->rightLimitSpinBox->value(),
					 "saveSubsec")) { return; }
#endif
	/// new, only edf is possible
	QString newPath = edfFil.getDirPath() + "/";
	if(DEFS.getUser() != username::GalyaP)
	{
		QDir(edfFil.getDirPath()).mkdir("winds");
		newPath += "winds/";
	}
	newPath += edfFil.getFileName(false);

	const QString ad = ui->saveSubsecAddNameLineEdit->text();
	if(!ad.isEmpty()) { newPath += "_" + ad; }
	newPath += ".edf";

	const QString newestPath{newPath};
	int counter = 0;
	while(QFile::exists(newPath))
	{
		newPath = newestPath;
		newPath.insert(newPath.lastIndexOf('.'), "_" + nm(counter));
		++counter;
	}

	/// for the first one
	if(myLib::getFileName(newPath).toLower() == edfFil.getExpName().toLower())
	{
		newPath.replace(".edf", "_0.edf", Qt::CaseInsensitive);
	}
	edfFil.writeOtherData(dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
											   ui->rightLimitSpinBox->value()), newPath);
	iitpLog("saveSub", 2, newPath);
	resetLimits();
	paint();
#if 0
	/// old


	if(myFileType == fileType::real)
	{

		QString helpString;
		helpString = edfFil.getDirPath() +
					 "/winds" +
					 "/" + myLib::getFileName(currentFile) +
					 "." + rn(addNum++, 3);
		myLib::writePlainData(helpString,
							  dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
												   ui->rightLimitSpinBox->value())
							  );
	}
	else if(myFileType == fileType::edf)
	{
		QString newPath = edfFil.getDirPath() + "/winds/" + edfFil.getExpName();
		const QString ad = ui->saveSubsecAddNameLineEdit->text();
		if(!ad.isEmpty())
		{
			newPath += "_" + ad;
		}
		newPath += ".edf";

		const QString newestPath{newPath};
		int counter = 0;
		while(QFile::exists(newPath))
		{
			newPath = newestPath;
			newPath.insert(newPath.lastIndexOf('.'), "_" + nm(counter));
			++counter;
		}
		/// for the first one
		if(myLib::getFileName(newPath).toLower() == edfFil.getExpName().toLower())
		{
			newPath.replace(".edf", "_0.edf", Qt::CaseInsensitive);
		}

		edfFil.writeOtherData(dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
												   ui->rightLimitSpinBox->value()), newPath);

		iitpLog("saveSub", 2, newPath);

		std::cout << "Cut::saveSubsecSlot: edfFile saved - " << newPath << std::endl;
	}

	resetLimits();
	paint();
#endif
}

void Cut::concatSlot()
{
	const QString fil1 = QFileDialog::getOpenFileName(this,
													  tr("Open first file"),
													  DEFS.getDirPath(),
													  "*.edf");
	if(fil1.isEmpty())
	{
		std::cout << "concatSlot: first file path is empty" << std::endl;
		return;
	}
	DEFS.setDir(myLib::getDirPathLib(fil1));
	const QString fil2 = QFileDialog::getOpenFileName(this,
													  tr("Open second file"),
													  DEFS.getDirPath(),
													  "*.edf");

	if(fil2.isEmpty())
	{
		std::cout << "concatSlot: second file path is empty" << std::endl;
		return;
	}

	QString outPath{fil1};
	outPath.replace(".edf", "_concat.edf");

	edfFile fil(fil1);
	fil.concatFile(fil2).writeEdfFile(outPath);
}

void Cut::browseSlot()
{
	const auto suffix = ui->suffixComboBox->currentText();
	const QString helpString = QFileDialog::getOpenFileName(
								   this,
								   tr("Open file"),
								   DEFS.dirPath(),
								   myLib::getDialogFilter(suffix));
	if(helpString.isEmpty()) { return; }

	ui->filePathLineEdit->setText(helpString);
	DEFS.setDir(myLib::getDirPathLib(helpString));

	filesList = QDir(myLib::getDirPathLib(helpString)).entryList(myLib::getFilters(suffix));

	for(const QString & in : filesList)
	{
		/// magic const
		if(in.contains(QRegExp(R"({_num|_000|_sht})")))
		{
			filesList.removeAll(in);
		}
	}

	fileListIter = std::find(std::begin(filesList),
							 std::end(filesList),
							 myLib::getFileName(helpString));
	undoData.clear();
	undoActions.clear();
	copyData.clear();

	openFile(helpString);
}

#if 0
void Cut::setFileType(const QString & dataFileName)
{
	this->myFileType = fileType::edf;
#if 0
	/// fileType::real deprecated
	if(dataFileName.endsWith(".edf", Qt::CaseInsensitive))
	{
		this->myFileType = fileType::edf;
	}
	else if(dataFileName.endsWith(def::plainDataExtension, Qt::CaseInsensitive))
	{
		this->myFileType = fileType::real;
	}
#endif
}
#endif

void Cut::setValuesByEdf()
{
	if( !fileOpened ) { return; }

    /// to prevent dataCutLocal overflow
    ui->derivChan1SpinBox->setValue(0);
    ui->derivChan2SpinBox->setValue(0);

	dataCutLocal = edfFil.getData(); /// expensive

	const bool iitpFlag = edfFil.getDirPath().contains("iitp", Qt::CaseInsensitive);
	const int localLimit = (edfFil.getNs() >= coords::egi::manyChannels) ?
						  (coords::egi::chans128to20.size() - 1) : (edfFil.getNs() - 1);

	ui->leftLimitSpinBox->setMinimum(0);
	ui->leftLimitSpinBox->setMaximum(dataCutLocal.cols());
	ui->leftTimeSpinBox->setMinimum(0);
	ui->leftTimeSpinBox->setMaximum(dataCutLocal.cols() / edfFil.getFreq());
	ui->leftTimeSpinBox->setDecimals(1);

	ui->rightLimitSpinBox->setMinimum(0);
	ui->rightLimitSpinBox->setMaximum(dataCutLocal.cols());
	ui->rightTimeSpinBox->setMinimum(0);
	ui->rightTimeSpinBox->setMaximum(dataCutLocal.cols() / edfFil.getFreq());
	ui->rightTimeSpinBox->setDecimals(1);

	resetLimits();

    for(QSpinBox * a : {ui->derivChan1SpinBox, ui->derivChan2SpinBox})
    {
        a->setMaximum(localLimit);
        a->setValue(localLimit); /// markers
    }



	ui->diffLimitSpinBox->setMinimum(0);
	ui->diffLimitSpinBox->setMaximum(dataCutLocal.cols());
	ui->diffTimeSpinBox->setMinimum(0);
	ui->diffTimeSpinBox->setMaximum(dataCutLocal.cols() / edfFil.getFreq());
	ui->diffTimeSpinBox->setDecimals(1);

	ui->paintStartDoubleSpinBox->setMaximum(std::floor(dataCutLocal.cols() / edfFil.getFreq()));
	ui->paintStartDoubleSpinBox->setValue(0); /// or not needed? -> paint
	ui->paintStartLabel->setText("start (max " + nm(std::floor(dataCutLocal.cols() / edfFil.getFreq())) + ")");
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - scrollAreaGapX) / edfFil.getFreq());
	ui->paintLengthDoubleSpinBox->setValue((this->width() - scrollAreaGapX) / edfFil.getFreq()); /// -> paint

	/// set coloured channels
	QString redStr = "EOG1";	/// ~horizontal
	QString blueStr = "EOG2";	/// ~vertical

	/// iitp
	if(iitpFlag)
	{
		redStr = "ECG";
		blueStr = "Artefac";
	}

	int eog1 = edfFil.findChannel(blueStr);
	int eog2 = edfFil.findChannel(redStr);
	if(eog2 == eog1) { eog2 = edfFil.findChannel("EOG3"); }

	if(iitpFlag)
	{
		ui->derivChan1SpinBox->setValue(eog1); /// ECG
		ui->derivChan2SpinBox->setValue(eog2); /// Artefac
	}

	for(auto p : colouredWidgets)
	{
		std::get<0>(p)->setMaximum(localLimit);
	}
	ui->color1SpinBox->setValue(eog1);	/// -> paint
	ui->color2SpinBox->setValue(eog2);	/// -> paint
	ui->color3SpinBox->setValue(-1);	/// maybe -> paint
}

void Cut::marksToDrawSet()
{
	QStringList a = ui->marksToDrawLineEdit->text().split(QRegExp("[,; ]"),
														  QString::SkipEmptyParts);
	marksToDraw.clear();
	for(const auto & in : a)
	{
		bool ok = false;
		int b = in.toInt(&ok);
		if(ok)
		{
			marksToDraw.push_back(b);
		}
	}
}

void Cut::openFile(const QString & dataFileName)
{
	fileOpened = false; /// to prevent many paints
	drawFlag = false;

	if(dataFileName.contains("_veget")) { ui->vegetCheckBox->setChecked(true); }

	addNum = 1;
	ui->filePathLineEdit->setText(dataFileName);
	ui->iitpDisableEcgCheckBox->setChecked(false);

	manualDrawFlag = false;
	manualDrawStart = QPoint{};
	manualDrawDataBackup.clear();
	leftDrawLimit = 0;
	currentPic = QPixmap{};
	marksToDrawSet();

	edfFil.readEdfFile(dataFileName);
	fileOpened = true;
	logAction(edfFil.getExpName());

	disconnectStuff();
	drawFlag = false;
	setValuesByEdf(); /// needs fileOpened
	drawFlag = true;
	connectStuff();

	paint();
}