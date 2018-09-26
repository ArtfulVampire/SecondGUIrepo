#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/qtlib.h>
#include <myLib/output.h>

using namespace myOut;


void Cut::next()
{
	QString helpString;
	if(fileListIter + 1 == std::end(filesList))
	{
		std::cout << "prev: bad number, too big" << std::endl;
		return;
	}
	++fileListIter;
	helpString = edfFil.getFilePath() + "/" + (*fileListIter);
	openFile(helpString);
}

void Cut::prev()
{
	if(fileListIter == std::begin(filesList))
	{
		std::cout << "prev: bad number, too little" << std::endl;
		return;
	}

	--fileListIter;
	QString helpString = edfFil.getFilePath() + "/" + (*fileListIter);
	openFile(helpString);
}


void Cut::rewrite()
{
	if( !fileOpened ) { return; }
	/// new, only edf is possible
	std::cout << "Cut:: Rewrite deliberately forbidden for edfs, use Save instead" << std::endl;

#if 0
	/// old
	if(myFileType == fileType::real) /// to deprecate
	{
		myLib::writePlainData(currentFile, dataCutLocal);
		currentPic.save(myLib::getPicPath(currentFile, this->edfFil.getDirPath()), nullptr, 100);
	}
	else if(myFileType == fileType::edf)
	{
		std::cout << "Cut:: Rewrite deliberately forbidden for edfs, use Save instead" << std::endl;
	}
#endif
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
	logAction(static_cast<const char*>("saveAs"), static_cast<const char*>("_new"));
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
	QString newPath = edfFil.getDirPath() + "/winds/" + edfFil.getExpName();
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

void Cut::browseSlot()
{
	/// filter bu suffix
	const QString suffix = ui->suffixComboBox->currentText();

	QString filter{};
	for(const QString & in : def::edfFilters)
	{
		filter += (suffix.isEmpty() ? "" :  ("*" + suffix)) + in + " ";
	}
	/// deprecated plainDataExtension
//	filter += (suffix.isEmpty() ? "" :  ("*" + suffix)) + "*." + def::plainDataExtension;

	const QString helpString = QFileDialog::getOpenFileName((QWidget*)this,
															tr("Open file"),
															DEFS.dirPath(),
															filter);
	if(helpString.isEmpty()) { return; }


	ui->filePathLineEdit->setText(helpString);
//	this->setFileType(helpString); /// deprecated fileType

	DEFS.setDir(myLib::getDirPathLib(helpString));
#if 0
	/// fileType::real deprecated
	if(this->myFileType == fileType::real)
	{
		DEFS.dirCdUp();
	}
#endif

	filesList = QDir(myLib::getDirPathLib(helpString)).entryList(
	{"*" + ui->suffixComboBox->currentText() +  "*." + myLib::getExtension(helpString)});

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

	dataCutLocal = edfFil.getData(); /// expensive

	const bool iitpFlag = edfFil.getDirPath().contains("iitp", Qt::CaseInsensitive);

	const int localLimit = (edfFil.getNs() >= coords::egi::manyChannels) ?
						  (coords::egi::chans128to20.size() - 1) : (edfFil.getNs() - 1);
	for(auto * a : {ui->derivChan1SpinBox, ui->derivChan2SpinBox})
	{
		a->setMaximum(localLimit);
		a->setValue(localLimit); /// markers
	}


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

	addNum = 1;
//	currentFile = dataFileName;
	ui->filePathLineEdit->setText(dataFileName);
	ui->iitpDisableEcgCheckBox->setChecked(false);

	manualDrawFlag = false;
	manualDrawStart = QPoint{};
	manualDrawDataBackup.clear();

	marksToDrawSet();

	leftDrawLimit = 0;

	/// new, only edf is possible
	edfFil.readEdfFile(dataFileName);
	currentPic = QPixmap{};
	fileOpened = true;
	logAction(edfFil.getExpName());
	drawFlag = false;
	setValuesByEdf(); /// needs fileOpened

#if 0
	/// fileType::real deprecated
	if(this->myFileType == fileType::real) /// to deprecate
	{
		dataCutLocal = edfFile(dataFileName).getData();
		fileOpened = true;
	}
	else if(this->myFileType == fileType::edf)
	{
		edfFil.readEdfFile(dataFileName);
		currentPic = QPixmap{};
		fileOpened = true;
		logAction(edfFil.getExpName());
		drawFlag = false;
		setValuesByEdf(); /// needs fileOpened
	}
#endif
	drawFlag = true;
	paint();
}


