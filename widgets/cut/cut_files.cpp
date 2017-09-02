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
		if((*fileListIter).contains(QRegExp(R"({_num|_000|_sht})")))
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
		if((*fileListIter).contains(QRegExp(R"({_num|_000|_sht})")))
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
		currentPic.save(myLib::getPicPath(currentFile), 0, 100);
	}
	else if(myFileType == fileType::edf)
	{
		std::cout << "Cut::rewrite: deliberately forbidden for edfs, use Save instead" << std::endl;
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
		QString helpString = def::dir->absolutePath()
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

	if(myFileType == fileType::real || 1) /// write plain windows (eyes)
	{

		QString helpString;
		helpString = def::dir->absolutePath() +
					 "/winds" +
					 "/" + myLib::getFileName(currentFile) +
					 "." + rn(addNum++, 3);
		myLib::writePlainData(helpString,
							  dataCutLocal,
							  ui->leftLimitSpinBox->value(),
							  ui->rightLimitSpinBox->value());
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
	resetLimits();
	paint();
}

void Cut::browseSlot()
{
	QString path;
	if(def::dir->isRoot())
	{
		path = def::dataFolder;
	}
	else
	{
		if(this->myFileType == fileType::edf)
		{
			path = edfFil.getDirPath();
		}
		else if(this->myFileType == fileType::real)
		{
			path = def::dir->absolutePath() +
				   "/" + ui->subdirComboBox->currentText();
		}

	}

	const QString suffix = ui->suffixComboBox->currentText();
	QString filter{};
	for(const QString & in : def::edfFilters)
	{
		filter += (suffix.isEmpty() ? "" :  ("*" + suffix)) + in + " ";
	}
	filter += (suffix.isEmpty() ? "" :  ("*" + suffix)) + "*." + def::plainDataExtension;

	const QString helpString = QFileDialog::getOpenFileName((QWidget*)this,
															tr("Open file"),
															path,
															filter);
	if(helpString.isEmpty())
	{
//        QMessageBox::information((QWidget*)this, tr("Warning"), tr("No file was chosen"), QMessageBox::Ok);
		return;
	}
	ui->lineEdit->setText(helpString);

	setFileType(helpString);

	/// set def::dir
	if(def::dir->isRoot())
	{
		def::dir->cd(myLib::getDirPathLib(helpString));
		if(this->myFileType == fileType::edf)
		{
			// do nothing
		}
		else if(this->myFileType == fileType::real)
		{
			def::dir->cdUp();
		}
	}
	filesList = QDir(myLib::getDirPathLib(helpString)).entryList(
	{"*" + ui->suffixComboBox->currentText() +  "*." + myLib::getExt(helpString)});

	fileListIter = std::find(std::begin(filesList),
							 std::end(filesList),
							 myLib::getFileName(helpString));


	undoData.clear();
	undos.clear();
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

	currFreq = edfFil.getFreq();
	dataCutLocal = edfFil.getData(); /// expensive


	ui->leftLimitSpinBox->setMaximum(edfFil.getDataLen());
	ui->rightLimitSpinBox->setMaximum(edfFil.getDataLen());
	ui->diffLimitSpinBox->setMaximum(edfFil.getDataLen());
	ui->leftTimeSpinBox->setMaximum(edfFil.getDataLen() / edfFil.getFreq());
	ui->leftTimeSpinBox->setDecimals(1);
	ui->rightTimeSpinBox->setMaximum(edfFil.getDataLen() / edfFil.getFreq());
	ui->rightTimeSpinBox->setDecimals(1);
	ui->diffTimeSpinBox->setMaximum(edfFil.getDataLen() / edfFil.getFreq());
	ui->diffTimeSpinBox->setDecimals(1);

	ui->paintStartDoubleSpinBox->setMaximum(floor(dataCutLocal.cols() / currFreq));
	ui->paintStartDoubleSpinBox->setValue(0); /// or not needed?
	ui->paintStartLabel->setText("start (max " + nm(floor(dataCutLocal.cols() / currFreq)) + ")");
	ui->paintLengthDoubleSpinBox->setMinimum((this->minimumWidth() - 20) / currFreq);
	ui->paintLengthDoubleSpinBox->setValue((this->width() - 20) / currFreq);

	resetLimits();

	/// set coloured channels
	QString redStr = "EOG1";	// ~horizontal
	QString blueStr = "EOG2";	// ~vertical

	/// iitp
	if(edfFil.getNs() > 35
	   || (edfFil.getNs() == 24 && edfFil.getDirPath().contains("iitp", Qt::CaseInsensitive))
	   )
	{
		redStr = "ECG";
		blueStr = "Artefac";
	}

	int eog1 = edfFil.findChannel(blueStr);
	int eog2 = edfFil.findChannel(redStr);
	if(eog2 == eog1) { eog2 = edfFil.findChannel("EOG3"); }

	ui->color1SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color2SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color3SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->color1SpinBox->setValue(eog1);
	ui->color2SpinBox->setValue(eog2);
	ui->color3SpinBox->setValue(-1);

	ui->derivChan1SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->derivChan2SpinBox->setMaximum(edfFil.getNs() - 1);
	ui->derivChan2SpinBox->setValue(edfFil.getNs() - 1); // markers
	ui->derivChan1SpinBox->setValue(edfFil.getNs() - 1); // markers
}

void Cut::openFile(const QString & dataFileName)
{
	addNum = 1;
	currentFile = dataFileName;
	ui->lineEdit->setText(dataFileName);

	leftDrawLimit = 0;
	if(this->myFileType == fileType::real)
	{
		myLib::readPlainData(dataFileName, dataCutLocal);
		fileOpened = true;
	}
	else if(this->myFileType == fileType::edf)
	{
		edfFil.readEdfFile(dataFileName);
		fileOpened = true;
		setValuesByEdf(); /// needs fileOpened
	}
	ui->iitpDisableEcgCheckBox->setChecked(false);
	paint();
	ui->scrollArea->horizontalScrollBar()->setSliderPosition(0);
}


