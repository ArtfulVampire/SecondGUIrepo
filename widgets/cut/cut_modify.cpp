#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/output.h>

using namespace myOut;

void Cut::refilterFrameSlot()
{
	/// only to have a look
	drawData = myLib::refilterMat(drawData,
								  ui->refilterLowFreqSpinBox->value(),
								  ui->refilterHighFreqSpinBox->value(),
								  false,
								  edfFil.getFreq(),
								  true);
	paintData(drawData);
}

void Cut::refilterAllSlot()
{
	if(this->dataCutLocalBackup.isEmpty()) { this->dataCutLocalBackup = this->dataCutLocal; }

	this->dataCutLocal = myLib::refilterMat(this->dataCutLocal,
											ui->refilterLowFreqSpinBox->value(),
											ui->refilterHighFreqSpinBox->value(),
											false,
											edfFil.getFreq(),
											true);
	paint();
}

void Cut::refilterResetSlot()
{
	this->dataCutLocal = std::move(this->dataCutLocalBackup);
	paint();
}

void Cut::subtractMeansSlot()
{
	for(int i = 0; i < dataCutLocal.rows(); ++i)
	{
		if(myFileType == fileType::edf && edfFil.getMarkChan() == i) { continue; }
		dataCutLocal[i] -= smLib::mean(dataCutLocal[i]);
	}
	paint();
}

void Cut::subtractMeanFrameSlot()
{
	for(int i = 0; i < drawData.rows() - 1; ++i)
	{
		drawData[i] -= smLib::mean(drawData[i]);
	}
	paintData(drawData);
}

/// FULL REMAKE with fileType::edf
void Cut::matiAdjustLimits() /////// should TEST !!!!!
{
#if 0
	QStringList lst = currentFile.split(QRegExp("[_.]"),
										QString::SkipEmptyParts); /// to detect session, type, piece
	int tempNum = 0;
	for(int i = 0; i < lst.length(); ++i)
	{
		if(nm(lst[i].toInt()) == lst[i])
		{
			tempNum = i;
			break;
		}
	}
	int typeNum = lst[tempNum].toInt();
	int sesNum = lst[tempNum + 1].toInt();
	int pieceNum = lst[tempNum + 2].toInt();

	if(typeNum != 0 && typeNum != 2) return; /// adjust only for counting or composed activity

	int newLeftLimit = leftLimit;
	int newRightLimit = rightLimit;

	while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][newLeftLimit], 14)
		   && newLeftLimit > 0)
	{
		--newLeftLimit;
	}
	while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][newRightLimit], 14)
		   && newRightLimit < data3.cols())
	{
		++newRightLimit; /// maximum of Num Of Slices
	}


	std::cout << "newLeftLimit = " << newLeftLimit << std::endl;
	std::cout << "newRightLimit = " << newRightLimit << std::endl;

	/// adjust limits if slice by whole count problems
	++newLeftLimit;
	if(newRightLimit != data3.cols()) ++newRightLimit;

	leftLimit = newLeftLimit;
	rightLimit = newRightLimit;
	return;



	/// adjust limits if slice by N seconds
	if(newLeftLimit > 0 || myLib::matiCountBit(data3[edfFil.getNs() - 1][0], 14))
	{
		++newLeftLimit; /// after the previous marker
	}
	else /// if(newLeftLimit == 0)
	{
		/// cut end in previous file, suspect that there ARE count answers
		if(pieceNum != 0) /// if this piece is not the first in the session
		{
			prev();
			leftLimit = rightLimit;
			while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][leftLimit], 14)) /// there ARE count answers
			{
				--leftLimit;
			}
			++leftLimit;
			std::cout << "prev file leftLimit = " << leftLimit << std::endl;
			/// zero() from tempLimit to rightLimit
			zeroData(data3, leftLimit, rightLimit);
			rewrite();
			next();
		}
	}


	if(newRightLimit < data3.cols())
	{
		++newRightLimit; /// after the previous marker
	}
	else if (myLib::matiCountBit(data3[edfFil.getNs() - 1][data3.cols() - 1], 14))
	{
		/// do nothing
	}
	else /// if(newRightLimit == data3.cols() && bit == 0)
	{
		/// cut start in next file, suspect that there ARE count answers
		next();
		lst = currentFile.split(QRegExp("[_.]"),
								QString::SkipEmptyParts);
		if (lst[tempNum + 1].toInt() == sesNum
			&& lst[tempNum].toInt() == typeNum
			&& lst[tempNum + 2].toInt() == pieceNum + 1) /// if next piece is ok
		{
			rightLimit = leftLimit;
			while (!myLib::matiCountBit(data3[edfFil.getNs() - 1][rightLimit], 14)) /// there ARE count answers
			{
				++rightLimit;
			}
			++rightLimit;
			std::cout << "next file rightLimit = " << rightLimit << std::endl;

			zeroData(data3, leftLimit, rightLimit);
			rewrite();
		}
		prev();
	}
	leftLimit = newLeftLimit;
	rightLimit = newRightLimit;
#endif

}

void Cut::applyLog(const QString & logPath)
{
	std::ifstream inStr;
	inStr.open(logPath.toStdString());

	QFile inFile(logPath);
	inFile.open(QIODevice::ReadOnly);


	std::string func;
	int left;
	int right;
	bool tmpBool;
	std::string add;
	while(1)
	{
		inStr >> func;
		if(func == "zeroData")
		{
			inStr >> left;
			inStr >> right;
			this->zero(left, right);
		}
		else if(func == "saveAs")
		{
			inStr >> add;
			this->saveAs(QString(add.c_str()));
		}
		else if(func == "split")
		{
			inStr >> left;
			inStr >> right;
			this->splitSemiSlot(left, right, false);
		}
		else if(func == "linearApprox")
		{
			inStr >> left;
			inStr >> right;

			char * buf = new char[1000];
			inStr.getline(buf, 1000);

			auto lst = QString(buf).split(QRegExp(R"(\s)"), QString::SkipEmptyParts);
			std::vector<int> chanList;
			for(auto in : lst)
			{
				chanList.push_back(in.toInt());
			}

			this->linearApprox(left, right, chanList);

			delete[] buf;
		}
		else if(func == "copy")
		{
			inStr >> left;
			inStr >> right;
			this->copy(left, right);
		}
		else if(func == "paste")
		{
			inStr >> left;
			inStr >> tmpBool;
			this->paste(left, tmpBool);
		}
		else if(func == "undo")
		{
			this->undo();
		}
		else if(func == "setMarker")
		{
			inStr >> right;		/// offset
			inStr >> left;		/// value
			this->setMarker(left, right);
		}

	}
}


bool Cut::checkBadRange(int start, int end, QString func)
{
	if(end - start == this->dataCutLocal.cols()
	   || (end - start > this->edfFil.getFreq() * 60 * 10)	/// magic const 10 min
	   )
	{
		std::cout << "Cut::" << func << ":"
				  << " too long range, return" << std::endl;
		return true;
	}
	return false;
}

void Cut::zero(int start, int end)
{
	if( !fileOpened ) { return; }

	if(start > end) /// not really possible
	{
		std::cout << "Cut::zero: leftEdge > rightEdge" << std::endl;
		return;
	}

	if(this->checkBadRange(start, end, "zero")) { return; }

	/// if MATI with counting - adjust limits to small task edges
	/// move leftLimit on the nearest prev marker
	/// move rightLimit on the nearest next marker
	/// delete [leftLimit, rightLimit)
	/// MATI counting problem only
	/// SHOULD REMAKE FOR EDF
	{
		/// ExpName.left(3)_fileSuffix_TYPE_SESSION_PIECE.MARKER
		QString helpString = "_0_[0-9]_[0-9]{2,2}";
		if(edfFil.getFilePath().contains(QRegExp(helpString)))
		{
			std::cout << "zero: adjust limits   " << edfFil.getFilePath() << std::endl;
			matiAdjustLimits();
		}
	}

	undoData.push_back(dataCutLocal.subCols(start, end));
	auto undoAction = [start, this]()
	{
		for(int k = 0; k < dataCutLocal.rows() - 1; ++k) /// don't affect markers
		{
			std::copy(std::begin(undoData.back()[k]),
					  std::end(undoData.back()[k]),
					  std::begin(dataCutLocal[k]) + start);
		}
		undoData.pop_back();
	};
	undoActions.push_back(undoAction);


	logAction("zeroData", start, end);
	for(int k = 0; k < dataCutLocal.rows() - 1; ++k)
	{
		std::fill(std::begin(dataCutLocal[k]) + start,
				  std::begin(dataCutLocal[k]) + end,
				  0.);
	}
	resetLimits();
	paint();

}

void Cut::zeroChannel(int chanNum, int start, int end)
{
	if(chanNum < 0)
	{
		std::cout << "Cut::zeroChannel: you haven't chosen orange channel" << std::endl;
		return;
	}

	undoData.push_back(dataCutLocal.subCols(start, end));
	auto undoAction = [start, this]()
	{
		for(int k = 0; k < dataCutLocal.rows() - 1; ++k) /// don't affect markers
		{
			std::copy(std::begin(undoData.back()[k]),
					  std::end(undoData.back()[k]),
					  std::begin(dataCutLocal[k]) + start);
		}
		undoData.pop_back();
	};
	undoActions.push_back(undoAction);

	logAction("zeroDataChannel", chanNum, start, end);
	std::fill(std::begin(dataCutLocal[chanNum]) + start,
			  std::begin(dataCutLocal[chanNum]) + end,
			  0.);
	resetLimits();
	paint();
}


void Cut::cutPausesSlot()
{
	if( !fileOpened ) { return; }

	while(1)
	{
		const auto beg = std::begin(dataCutLocal[edfFil.getMarkChan()]);
		const auto en = std::end(dataCutLocal[edfFil.getMarkChan()]);

		auto sta = std::find(beg,
							 en,
							 201); /// magic const pause Presentation
		auto fin = std::find(sta,
							 en,
							 202); /// magic const resume Presentation


		if( (sta != en) && (fin != en) )
		{
			std::cout << "Cut::cutPausesSlot: found pause "
					  << (sta - beg) / edfFil.getFreq() << " "
					  << (fin - beg) / edfFil.getFreq()
					  << std::endl;
			this->splitSemiSlot(std::distance(beg, sta) - 1, std::distance(beg, fin) + 1, false);
		}
		else
		{
			break;
		}
	}
	std::cout << "CutPausesSlot: pauses cut" << std::endl;
}


void Cut::copy(int lef, int rig)
{
	this->copyData = dataCutLocal.subCols(lef, rig);
}

void Cut::copySlot()
{
	if( !fileOpened ) { return; }

	this->copy(ui->leftLimitSpinBox->value(), ui->rightLimitSpinBox->value());
	logAction("copy", ui->leftLimitSpinBox->value(), ui->rightLimitSpinBox->value());
	paint();
}

void Cut::cutSlot()
{
	this->copySlot();
	this->splitSlot();
}

void Cut::paste(int start, const matrix & inData, bool addUndo)
{
	if( !fileOpened ) { return; }

	int cls = inData.cols();

	if(addUndo)
	{
		auto undoAction = [start, cls, this]()
		{
			this->splitSemiSlot(start, start + cls, false);
		};
		undoActions.push_back(undoAction);
	}

	matrix data2 = this->dataCutLocal.subCols(ui->leftLimitSpinBox->value(), dataCutLocal.cols());
	dataCutLocal.resizeCols(ui->leftLimitSpinBox->value());
	dataCutLocal.horzCat(inData).horzCat(data2);
	logAction("paste", start, addUndo);
	paint();
}

void Cut::pasteSlot()
{
	if(copyData.isEmpty()) return;
	this->paste(ui->leftLimitSpinBox->value(), copyData);
}


void Cut::undo()
{
	if(undoActions.empty())
	{
		std::cout << "Cut::undoSlot: undos empty" << std::endl;
		return;
	}
	undoActions.back()();
	undoActions.pop_back();

	paint();
}

void Cut::undoSlot()
{
	this->undo();
	logAction("undo");
}


void Cut::setMarkerSlot(bool isLeft)
{
	int offset = 0;
	int newVal = 0;
	if(isLeft)
	{
		offset = ui->leftLimitSpinBox->value();
		newVal = ui->setMarkLeftSpinBox->value();
	}
	else
	{
		offset = ui->rightLimitSpinBox->value();
		newVal = ui->setMarkRightSpinBox->value();
	}
	this->setMarker(offset, newVal);
	logAction("setMarker",
			  offset,
			  newVal);
	paint();
}

void Cut::setMarker(int offset, int newVal)
{
	if( !fileOpened ) { return; }

	int num = edfFil.getMarkChan();
	if(num <= 0)
	{
		std::cout << "Cut::setMarker: haven't found markers channel" << std::endl;
		return;
	}
	int val = dataCutLocal[num][offset];
	auto undoAction = [num, offset, val, this](){ this->dataCutLocal[num][offset] = val; };
	undoActions.push_back(undoAction);
	dataCutLocal[num][offset] = newVal;

#if 0
	/// old, deprecate fileType::real
	if(myFileType == fileType::edf)
	{
		int num = edfFil.getMarkChan();
		if(num <= 0)
		{
			std::cout << "Cut::setMarker: haven't found markers channel" << std::endl;
			return;
		}


		int val = dataCutLocal[num][offset];
		auto undoAction = [num, offset, val, this](){ this->dataCutLocal[num][offset] = val; };
		undoActions.push_back(undoAction);

		dataCutLocal[num][offset] = newVal;
	}
	else if(myFileType == fileType::real)
	{
		int num = dataCutLocal.rows() - 1; /// last channel

		int val = dataCutLocal[num][offset];
		auto undoAction = [num, offset, val, this](){ dataCutLocal[num][offset] = val; };
		undoActions.push_back(undoAction);

		dataCutLocal[num][offset] = newVal;
	}
#endif
}


void Cut::zeroSlot()
{
	if(edfFil.getExpName().contains("_veget"))
	{
		this->zeroChannel(ui->color3SpinBox->value(),
						  ui->leftLimitSpinBox->value(),
						  ui->rightLimitSpinBox->value());
	}
	else
	{
		this->zero(ui->leftLimitSpinBox->value(), ui->rightLimitSpinBox->value());
	}
}

void Cut::zeroFromZeroSlot()
{
	this->zero(0, ui->rightLimitSpinBox->value());
}

void Cut::zeroTillEndSlot()
{
	this->zero(ui->leftLimitSpinBox->value(), dataCutLocal.cols());
}


void Cut::split(int start, int end)
{
	if(end == dataCutLocal.cols()) /// split till end
	{
		dataCutLocal.resizeCols(start);
	}
	else
	{
		matrix data2 = dataCutLocal.subCols(end, dataCutLocal.cols());
		dataCutLocal.resizeCols(start).horzCat(data2); /// +1 to save first marker in reals
	}

	/// passage from setValuesByEdf
	ui->rightLimitSpinBox->setMaximum(dataCutLocal.cols());
	ui->rightTimeSpinBox->setMaximum(dataCutLocal.cols() / edfFil.getFreq());
}

/// DANGER markers
void Cut::splitSemiSlot(int start, int end, bool addUndo)
{
	if( !fileOpened ) { return; }

	if(start > end)
	{
		std::cout << "Cut::split: leftEdge > rightEdge" << std::endl;
		return;
	}

	if(this->checkBadRange(start, end, "splitSemiSlot")) { return; }

	/// cout all nonzero markers
	int bin = start;
	for(auto it = std::begin(dataCutLocal.back()) + start;
		it != std::begin(dataCutLocal.back()) + end;
		++it, ++bin)
	{
		auto a = *it;
		if((a != 0)
		   && (a != 201)	/// magic const pause
		   && (a != 202)	/// magic const resume
		   )
		{
			std::cout << "Cut::splitSemiSlot: nonzero marker "
					  << bin << "\t"
					  << bin / edfFil.getFreq() << "\t"
					  << a << std::endl;
		}
	}

	if(addUndo)
	{
		undoData.push_back(dataCutLocal.subCols(start, end));
		auto undoAction = [start, this]()
		{
			this->paste(start, undoData.back(), false);
			undoData.pop_back();
		};
		undoActions.push_back(undoAction);
	}

	this->split(start, end);
	logAction("split", start, end);

	ui->paintStartLabel->setText("start (max " + nm(std::floor(dataCutLocal.cols() / edfFil.getFreq())) + ")");

	/// crutch with drawFlag
	this->drawFlag = false;
	resetLimits();
	ui->paintStartDoubleSpinBox->setValue(start / edfFil.getFreq() - 1.5);
	ui->leftLimitSpinBox->setValue(start); /// really needed?
	this->drawFlag = true;

	paint();

}

void Cut::splitSlot()
{
	if(edfFil.getExpName().contains("_veget"))
	{
		std::cout << "split is forbidden for vegetative data" << std::endl;
		return;
	}
	this->splitSemiSlot(ui->leftLimitSpinBox->value(),
						ui->rightLimitSpinBox->value());
}

void Cut::splitFromZeroSlot()
{
	this->splitSemiSlot(0, ui->rightLimitSpinBox->value());
	ui->paintStartDoubleSpinBox->setValue(0.);
}

void Cut::splitTillEndSlot()
{
	double pos = ui->paintStartDoubleSpinBox->value();
	bool fl = pos < ui->leftLimitSpinBox->value() / edfFil.getFreq();
	this->splitSemiSlot(ui->leftLimitSpinBox->value(), dataCutLocal.cols(), true);
	if(fl) { ui->paintStartDoubleSpinBox->setValue(pos); }
}

void Cut::linearApprox(int lef, int rig, std::vector<int> chanList)
{
	/// just for fun
	if(chanList.empty())
	{
		chanList = smLib::range<decltype(chanList)>(0, dataCutLocal.rows());
	}

	/// addUndo
	if(1)
	{
		undoData.push_back(dataCutLocal.subCols(lef, rig));
		auto undoAction = [lef, chanList, this]()
		{
			for(int k : chanList) /// don't affect markers
			{
				std::copy(std::begin(undoData.back()[k]),
						  std::end(undoData.back()[k]),
						  std::begin(dataCutLocal[k]) + lef);
			}
			undoData.pop_back();
		};
		undoActions.push_back(undoAction);
	}


	for(int ch : chanList)
	{
		const double coeff = (dataCutLocal[ch][rig] - dataCutLocal[ch][lef]) / (rig - lef);
		for(int i = lef + 1; i < rig; ++i)
		{
			dataCutLocal[ch][i] = dataCutLocal[ch][lef] + coeff * (i - lef);
		}
	}
}

void Cut::linearApproxSlot()
{
	if( !fileOpened ) { return; }

	const int lef = ui->leftLimitSpinBox->value();
	const int rig = ui->rightLimitSpinBox->value();


	std::vector<int> chanList{};
	for(int i = 0; i < dataCutLocal.rows(); ++i)
	{
		if(ui->linearApproxAllEegCheckBox->isChecked()
		   && edfFil.getLabels(i).startsWith("EEG "))
		{
			chanList.push_back(i); continue;
		}
		if(ui->linearApproxAllGoniosCheckBox->isChecked()
		   && edfFil.getLabels(i).contains("IT")
		   && !edfFil.getLabels(i).contains("EMG"))
		{
			chanList.push_back(i); continue;
		}
		if(ui->linearApproxAllEmgCheckBox->isChecked()
		   && edfFil.getLabels(i).contains("IT")
		   && edfFil.getLabels(i).contains("EMG"))
		{
			chanList.push_back(i); continue;
		}
	}
	if(chanList.empty() && ui->color3SpinBox->value() > 0)
	{
		chanList.push_back(ui->color3SpinBox->value());
	}

	/// if chanList.empty() - approx all channels


	logAction("linearApprox", lef, rig, chanList);
	this->linearApprox(lef, rig, chanList);
	resetLimits();

	drawData = makeDrawData();
	repaintData(drawData, lef, rig);
	ui->scrollArea->setFocus();
}

template void Cut::logAction(const QString & in);
