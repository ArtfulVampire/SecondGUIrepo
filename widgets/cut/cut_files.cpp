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

	auto iterBackup = fileListIter;
	++fileListIter;
	for(; fileListIter != std::end(filesList); ++fileListIter)
	{
		if((*fileListIter).contains(QRegExp(R"({_num|_000|_sht})"))) /// magig const regexp
		{
			std::cout << "next: bad name" << std::endl;
			continue;
		}
		helpString = myLib::getDirPathLib(currentFile) + "/" + (*fileListIter);
		openFile(helpString);
		return;
	}
	fileListIter = iterBackup;
	std::cout << "next: bad number, too big" << std::endl;
}

void Cut::prev()
{
	QString helpString;

	auto iterBackup = fileListIter;
	--fileListIter;
	for(; fileListIter != std::begin(filesList); --fileListIter)
	{
		if((*fileListIter).contains(QRegExp(R"({_num|_000|_sht})"))) /// magig const regexp
		{
			std::cout << "prev: bad name" << std::endl;
			continue;
		}
		helpString = myLib::getDirPathLib(currentFile) + "/" + (*fileListIter);
		openFile(helpString);
		return;
	}
	fileListIter = iterBackup;
	std::cout << "prev: bad number, too little" << std::endl;
}


void Cut::rewrite()
{
	if( !fileOpened ) { return; }

	if(myFileType == fileType::real)
	{
		myLib::writePlainData(currentFile, dataCutLocal);
		currentPic.save(myLib::getPicPath(currentFile, this->edfFil.getDirPath()), 0, 100);
	}
	else if(myFileType == fileType::edf)
	{
		std::cout << "Cut:: Rewrite deliberately forbidden for edfs, use Save instead" << std::endl;
	}
}


void Cut::saveAs(const QString & addToName)
{
	QString newPath = currentFile;
	newPath.insert(newPath.lastIndexOf('.'), addToName);
	edfFil.writeOtherData(dataCutLocal, newPath);
}

void Cut::saveSlot()
{
	if( !fileOpened ) { return; }

	if(myFileType == fileType::real)
	{
		QString helpString = edfFil.getDirPath()
							 + "/cut"
							 + "/" + myLib::getFileName(currentFile);

		// new
		myLib::writePlainData(helpString, dataCutLocal);
	}
	else if(myFileType == fileType::edf)
	{
		this->saveAs("_new");
//		logAction("saveAs", "_new"); // problem with template function definition
		std::cout << "Cut::save: edfFile saved" << std::endl;
	}

}

void Cut::saveSubsecSlot()
{
	if( !fileOpened ) { return; }

	if(!checkBadRange(ui->leftLimitSpinBox->value(),
					  ui->rightLimitSpinBox->value(),
					  "saveSubsec"))
	{

		if(myFileType == fileType::real || 1) /// write plain windows (eyes)
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


			QString newPath = currentFile;
			QString ad = ui->saveSubsecAddNameLineEdit->text();
			if( !ad.isEmpty())
			{
				newPath.insert(newPath.lastIndexOf('.'), "_" + ad);
			}

			int counter = 0;
			while(QFile::exists(newPath))
			{
				newPath = currentFile;
				newPath.insert(newPath.lastIndexOf('.'), "_" + nm(counter));
				++counter;
			}

			edfFil.writeOtherData(dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
													   ui->rightLimitSpinBox->value()), newPath);

			iitpLog("saveSub", 2, newPath);

			std::cout << "Cut::saveSubsecSlot: edfFile saved - " << newPath << std::endl;
		}
	}
	resetLimits();
	paint();
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
	filter += (suffix.isEmpty() ? "" :  ("*" + suffix)) + "*." + def::plainDataExtension;

	const QString helpString = QFileDialog::getOpenFileName((QWidget*)this,
															tr("Open file"),
															DEFS.dirPath(),
															filter);
	if(helpString.isEmpty()) { return; }


	ui->filePathLineEdit->setText(helpString);
	this->setFileType(helpString);

	DEFS.setDir(myLib::getDirPathLib(helpString));
	if(this->myFileType == fileType::real)
	{
		DEFS.dirCdUp();
	}

	filesList = QDir(myLib::getDirPathLib(helpString)).entryList(
	{"*" + ui->suffixComboBox->currentText() +  "*." + myLib::getExtension(helpString)});

	fileListIter = std::find(std::begin(filesList),
							 std::end(filesList),
							 myLib::getFileName(helpString));


	undoData.clear();
	undoActions.clear();
	copyData.clear();

	openFile(helpString);
}

void Cut::setFileType(const QString & dataFileName)
{
	if(dataFileName.endsWith(".edf", Qt::CaseInsensitive))
	{
		this->myFileType = fileType::edf;
	}
	else if(dataFileName.endsWith(def::plainDataExtension, Qt::CaseInsensitive))
	{
		this->myFileType = fileType::real;
	}
}


void Cut::setValuesByEdf()
{
	if( !fileOpened ) { return; }

	dataCutLocal = edfFil.getData(); /// expensive

	const bool iitpFlag = edfFil.getDirPath().contains("iitp", Qt::CaseInsensitive);

	const int localLimit = (edfFil.getNs() >= coords::manyChannels) ?
						  (coords::chans128to20.size() - 1) : (edfFil.getNs() - 1);
	for(auto * a : {ui->derivChan1SpinBox, ui->derivChan2SpinBox})
	{
		a->setMaximum(localLimit);
		a->setValue(localLimit); // markers
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


	ui->paintStartDoubleSpinBox->setMaximum(floor(dataCutLocal.cols() / edfFil.getFreq()));
	ui->paintStartDoubleSpinBox->setValue(0); /// or not needed? -> paint
	ui->paintStartLabel->setText("start (max " + nm(floor(dataCutLocal.cols() / edfFil.getFreq())) + ")");
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - scrollAreaGapX) / edfFil.getFreq());
	ui->paintLengthDoubleSpinBox->setValue((this->width() - scrollAreaGapX) / edfFil.getFreq()); /// -> paint



	/// set coloured channels
	QString redStr = "EOG1";	// ~horizontal
	QString blueStr = "EOG2";	// ~vertical

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
	currentFile = dataFileName;
	ui->filePathLineEdit->setText(dataFileName);
	ui->iitpDisableEcgCheckBox->setChecked(false);

	manualDrawFlag = false;
	manualDrawStart = QPoint{};
	manualDrawDataBackup.clear();

	marksToDrawSet();

	leftDrawLimit = 0;
	if(this->myFileType == fileType::real)
	{
		dataCutLocal = myLib::readPlainData(dataFileName);
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
	drawFlag = true;
	paint();
}


