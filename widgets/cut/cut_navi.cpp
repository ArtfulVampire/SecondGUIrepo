#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/qtlib.h>
#include <myLib/output.h>

using namespace myOut;

void Cut::xNormSlot()
{
	/// horzNorm
	const double newXnorm = ui->xNormSpinBox->value();
	const double prevStart = ui->paintStartDoubleSpinBox->value();
	double newStart{prevStart};
	auto pos = ui->scrollArea->mapFromGlobal(QCursor::pos());
	if(pos.y() >= 0) /// xNorm widget is higher than scrollArea
	{
		newStart += pos.x() / edfFil.getFreq() * (prevXnorm - newXnorm);
	}
	ui->paintLengthDoubleSpinBox->setValue(ui->paintLengthDoubleSpinBox->value() * newXnorm / prevXnorm);
	ui->paintStartDoubleSpinBox->setValue(newStart);
	prevXnorm = newXnorm;
	paint();
}

void Cut::forwardStepSlot()
{
	if( !fileOpened ) { return; }

	if(leftDrawLimit + ui->scrollArea->width() > dataCutLocal.cols())
	{
		std::cout << "end of file" << std::endl;
		return;
	}

	leftDrawLimit = std::min(leftDrawLimit + edfFil.getFreq(),
							 static_cast<double>(dataCutLocal.cols()));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / edfFil.getFreq());
}
void Cut::backwardStepSlot()
{
	if( !fileOpened ) { return; }

	if(leftDrawLimit == 0)
	{
		std::cout << "begin of file" << std::endl;
		return;
	}
	leftDrawLimit = std::max(leftDrawLimit - edfFil.getFreq(), 0.);
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / edfFil.getFreq());
}
void Cut::forwardFrameSlot()
{
	if( !fileOpened ) { return; }

	if(leftDrawLimit + ui->scrollArea->width() > dataCutLocal.cols())
	{
		std::cout << "end of file" << std::endl;
		return;
	}
	leftDrawLimit = std::min(leftDrawLimit +
							 ui->paintLengthDoubleSpinBox->value() * edfFil.getFreq(),
							 static_cast<double>(dataCutLocal.cols()));
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / edfFil.getFreq());
}
void Cut::backwardFrameSlot()
{
	if( !fileOpened ) { return; }

	if(leftDrawLimit == 0)
	{
		std::cout << "begin of file" << std::endl;
		return;
	}
	leftDrawLimit = std::max(leftDrawLimit -
							 ui->paintLengthDoubleSpinBox->value() * edfFil.getFreq(), 0.);
	ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / edfFil.getFreq());
}


void Cut::findPrevMark(double mark)
{
	if( !fileOpened ) { return; }

	/// make on dataCutLocal
	if(myFileType == fileType::edf)
	{
		const std::valarray<double> & markArr = dataCutLocal[edfFil.getMarkChan()];
		const auto beg = std::begin(markArr);
		auto it = std::begin(markArr) + leftDrawLimit;
		if(mark > 0)
		{
			while (it != beg)
			{
				if (*it == mark) { break; }
				/// else
				--it;
			}
		}
		else
		{
			while (it != beg)
			{
				if (*it != 0.) { break; }
				/// else
				--it;
			}
		}

		if(it == beg)
		{
			std::cout << "findPrevMark: marker not found" << std::endl;
			return;
		}

		int index = std::distance(beg, it);
		ui->paintStartDoubleSpinBox->setValue(
					/// horzNorm
					std::max(0.,
							 index / edfFil.getFreq()
							 - findNextGap * 0.8 * ui->xNormSpinBox->value()));
		showDerivatives();
		paint();
	}
}


void Cut::findNextMark(int mark)
{
	if( !fileOpened ) { return; }

	/// make on dataCutLocal
	if(myFileType == fileType::edf)
	{
		const std::valarray<double> & markArr = dataCutLocal[edfFil.getMarkChan()];

		decltype(std::begin(markArr)) it;
		if(mark > 0)
		{
			it = std::find(std::begin(markArr) + leftDrawLimit
						   /// horzNorm
						   + static_cast<int>(findNextGap * edfFil.getFreq()
											  / ui->xNormSpinBox->value()),
						   std::end(markArr),
						   mark);
		}
		else
		{
			it = std::find_if(std::begin(markArr) + leftDrawLimit
							  /// horzNorm
							  + static_cast<int>(findNextGap * edfFil.getFreq()
												 / ui->xNormSpinBox->value()),
							  std::end(markArr),
							  [](double in){ return in != 0.; });
		}

		if(it == std::end(markArr))
		{
			std::cout << "findNextMark: marker not found" << std::endl;
			return;
		}

		int index = std::distance(std::begin(markArr), it);
		ui->paintStartDoubleSpinBox->setValue(
					/// horzNorm
					std::max(0.,
							 index / edfFil.getFreq()
							 - findNextGap * 0.8 * ui->xNormSpinBox->value()));
		showDerivatives();
		paint();
	}
}

