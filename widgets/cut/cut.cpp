#include <widgets/cut.h>
#include "ui_cut.h"

#include <other/coords.h>
#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/qtlib.h>
#include <myLib/output.h>
#include <myLib/iitp_consts.h>
#include <myLib/iitp.h>

#include <QMouseEvent>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

using namespace myOut;

Cut::Cut() :
	ui(new Ui::Cut)
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowTitle("Cut");

	ui->suffixComboBox->addItem("");
	ui->suffixComboBox->addItem("eeg");
	ui->suffixComboBox->addItem("emg");
	ui->suffixComboBox->addItem("sum");
	ui->suffixComboBox->addItem("new");
	ui->suffixComboBox->addItem("sum_new");
	ui->suffixComboBox->addItem("car");
	ui->suffixComboBox->addItem("stag");
	ui->suffixComboBox->setCurrentText("");

	if(DEFS.getUser() == username::IITP)
	{
		ui->suffixComboBox->setCurrentText("sum");
	}


	/// draws
	ui->yNormDoubleSpinBox->setDecimals(2);
	ui->yNormDoubleSpinBox->setMaximum(50.);
	ui->yNormDoubleSpinBox->setMinimum(0.01);
	ui->yNormDoubleSpinBox->setValue(1.);
	ui->yNormDoubleSpinBox->setSingleStep(0.05);

	ui->paintStartDoubleSpinBox->setDecimals(1);
	ui->paintStartDoubleSpinBox->setSingleStep(0.1);
	ui->paintStartDoubleSpinBox->setValue(0);

	ui->paintLengthDoubleSpinBox->setDecimals(1);
	ui->paintLengthDoubleSpinBox->setSingleStep(0.2);

	ui->scrollArea->setWidget(ui->picLabel);
	ui->scrollArea->installEventFilter(this);

	ui->color1LineEdit->setText("blue");
	ui->color2LineEdit->setText("red");
	ui->color3LineEdit->setText("orange");
	colouredWidgets =
	{
		std::make_tuple(ui->color1SpinBox, ui->colorChan1LineEdit, ui->color1LineEdit),
		std::make_tuple(ui->color2SpinBox, ui->colorChan2LineEdit, ui->color2LineEdit),
		std::make_tuple(ui->color3SpinBox, ui->colorChan3LineEdit, ui->color3LineEdit)
	};
	for(auto in : colouredWidgets)
	{
		std::get<0>(in)->setMinimum(-1);
	}

	/// derivativesGridLayout
	ui->derivVal1SpinBox->setMaximum(33000);		ui->derivVal1SpinBox->setMinimum(-33000);
	ui->derivVal2SpinBox->setMaximum(33000);		ui->derivVal2SpinBox->setMinimum(-33000);
	ui->derivFirst1SpinBox->setMaximum(1000);	ui->derivFirst1SpinBox->setMinimum(-1000);
	ui->derivFirst2SpinBox->setMaximum(1000);	ui->derivFirst2SpinBox->setMinimum(-1000);
	ui->derivSecond1SpinBox->setMaximum(500);	ui->derivSecond1SpinBox->setMinimum(-500);
	ui->derivSecond2SpinBox->setMaximum(500);	ui->derivSecond2SpinBox->setMinimum(-500);

	/// markers
	ui->setMarkLeftSpinBox->setMaximum(255);
	ui->setMarkRightSpinBox->setMaximum(255);
	ui->setMarkLeftSpinBox->setValue(241);
	ui->setMarkRightSpinBox->setValue(0);
	ui->findNextMarkSpinBox->setMaximum(255);
	ui->findNextMarkSpinBox->setValue(241);
	ui->findPrevMarkSpinBox->setMaximum(255);
	ui->findPrevMarkSpinBox->setValue(241);

	/// smartFind
	for(QDoubleSpinBox * a : qtLib::widgetsOfLayout<QDoubleSpinBox>(ui->smartFindGrid))
	{
		a->setMaximum(500.);
		a->setMinimum(10.);
		a->setSingleStep(5.);
	}
	ui->smartFindThetaSpinBox->setValue(80);
	ui->smartFindAlphaSpinBox->setValue(100);
	ui->smartFindBetaSpinBox->setValue(50);
	ui->smartFindGammaSpinBox->setValue(40);
	ui->smartFindAmplSpinBox->setValue(35);
	ui->smartFindIntgrlSpinBox->setValue(150);

	/// iitp
	ui->iitpSaveNewNumSpinBox->setMaximum(35);
	ui->iitpSaveNewNumSpinBox->setValue(24);

	/// shortcuts
	ui->saveSubsecPushButton->setShortcut(tr("c"));
	ui->saveButton->setShortcut(tr("s"));
	ui->zeroButton->setShortcut(tr("z"));
	ui->splitButton->setShortcut(tr("x"));
	ui->linearApproxPushButton->setShortcut(tr("l"));
	ui->refilterFramePushButton->setShortcut(tr("f"));
//	ui->iitpInverseCheckBox->setShortcut(tr("i"));
//	ui->iitpDisableEcgCheckBox->setShortcut(tr("p"));

	/// stuff
	if(DEFS.getUser() == username::GalyaP)
	{
		ui->refilterLowFreqSpinBox->setValue(1.6);
		ui->refilterHighFreqSpinBox->setValue(30);
		QObject::connect(ui->concatPushButton, &QPushButton::clicked,
						 this, &Cut::concatSlot);
	}
	else
	{
		ui->refilterLowFreqSpinBox->setValue(DEFS.getLeftFreq());
		ui->refilterHighFreqSpinBox->setValue(DEFS.getRightFreq());
		ui->concatPushButton->hide();
	}


	/// files
	QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseSlot()));
//	QObject::connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));
//	QObject::connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));
	QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));
	QObject::connect(ui->saveSubsecPushButton, SIGNAL(clicked()), this, SLOT(saveSubsecSlot()));

	/// modify
	QObject::connect(ui->linearApproxPushButton, SIGNAL(clicked()), this, SLOT(linearApproxSlot()));
	QObject::connect(ui->zeroButton, SIGNAL(clicked()), this, SLOT(zeroSlot()));
	QObject::connect(ui->splitButton, SIGNAL(clicked()), this, SLOT(splitSlot()));
	QObject::connect(ui->zeroFromZeroPushButton, SIGNAL(clicked()), this, SLOT(zeroFromZeroSlot()));
	QObject::connect(ui->splitFromZeroPushButton, SIGNAL(clicked()), this, SLOT(splitFromZeroSlot()));
	QObject::connect(ui->zeroTillEndPushButton, SIGNAL(clicked()), this, SLOT(zeroTillEndSlot()));
	QObject::connect(ui->splitTillEndPushButton, SIGNAL(clicked()), this, SLOT(splitTillEndSlot()));
	QObject::connect(ui->cutPausesPushButton, SIGNAL(clicked()), this, SLOT(cutPausesSlot()));
	QObject::connect(ui->subtractMeansPushButton, SIGNAL(clicked()),
					 this, SLOT(subtractMeansSlot()));
	QObject::connect(ui->vegetCheckBox, &QCheckBox::clicked,
					 [this](bool ch){ vegetFlag = ch; });


	QObject::connect(ui->setMarkLeftPushButton, &QPushButton::clicked,
					 [this]() { this->setMarkerSlot(true); });
	QObject::connect(ui->setMarkRightPushButton, &QPushButton::clicked,
					 [this]() { this->setMarkerSlot(false); });

	/// navi
//	QObject::connect(ui->forwardStepButton, SIGNAL(clicked()), this, SLOT(forwardStepSlot()));
//	QObject::connect(ui->backwardStepButton, SIGNAL(clicked()), this, SLOT(backwardStepSlot()));
//	QObject::connect(ui->forwardFrameButton, SIGNAL(clicked()), this, SLOT(forwardFrameSlot()));
//	QObject::connect(ui->backwardFrameButton, SIGNAL(clicked()), this, SLOT(backwardFrameSlot()));
	QObject::connect(ui->findNextMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(ui->findNextMarkSpinBox->value()); });
	QObject::connect(ui->findNextNonzeroMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(-1); });
	QObject::connect(ui->findPrevMarkPushButton, &QPushButton::clicked,
					 [this](){ findPrevMark(ui->findPrevMarkSpinBox->value()); });
	QObject::connect(ui->findPrevNonzeroMarkPushButton, &QPushButton::clicked,
					 [this](){ findPrevMark(-1); });

	/// deriv
	QObject::connect(ui->derivChan1SpinBox,  SIGNAL( valueChanged(int) ),
					 this, SLOT( showDerivatives() ));
	QObject::connect(ui->derivChan2SpinBox, SIGNAL( valueChanged(int) ),
					 this, SLOT( showDerivatives() ));


	/// draws
	QObject::connect(ui->iitpDisableEcgCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormInvertCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->xNormSpinBox,
					 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 this, &Cut::xNormSlot);
	QObject::connect(ui->paintStartDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT( paintResizedSlot(double)) );
	QObject::connect(ui->leftLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
	QObject::connect(ui->rightLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
	QObject::connect(ui->zeroChannelsLineEdit, &QLineEdit::editingFinished,
					 [this](){ zeroedChannels = myLib::splitStringIntoVec(
													ui->zeroChannelsLineEdit->text());
		paint(); this->setFocus(); });
	for(auto p : colouredWidgets)
	{
		QObject::connect(std::get<0>(p),
						 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
						 [this, p](){ this->colorSpinSlot(std::get<0>(p), std::get<1>(p)); } );
		QObject::connect(std::get<2>(p), SIGNAL(returnPressed()), this, SLOT(paint()));
	}
	QObject::connect(ui->marksToDrawLineEdit, &QLineEdit::editingFinished,
					 [this](){ marksToDrawSet(); paint(); });
	QObject::connect(ui->refilterFramePushButton, SIGNAL(clicked(bool)),
					 this, SLOT(refilterFrameSlot()));
	QObject::connect(ui->refilterAllPushButton, SIGNAL(clicked(bool)),
					 this, SLOT(refilterAllSlot()));
	QObject::connect(ui->refilterResetPushButton, SIGNAL(clicked(bool)),
					 this, SLOT(refilterResetSlot()));
	QObject::connect(ui->subtractMeanFramePushButton, SIGNAL(clicked(bool)),
					 this, SLOT(subtractMeanFrameSlot()));
	QObject::connect(ui->drawSpectrumPushButton, SIGNAL(clicked(bool)),
					 this, SLOT(drawSpectre()));
	QObject::connect(ui->savePicPushButton, &QPushButton::clicked,
			[this](){ currentPic.save(edfFil.getDirPath() + "/"
									  + ui->saveSubsecAddNameLineEdit->text() + ".jpg"); });


	/// smartFind
	QObject::connect(ui->smartFindShowPushButton, &QPushButton::clicked,
					 [this](){ this->smartFindShowValues(); });
	QObject::connect(ui->smartFindLearnPushButton, SIGNAL(clicked()),
					 this, SLOT(smartFindLearnSlot()));
	QObject::connect(ui->smartFindNextPushButton, &QPushButton::clicked,
					 [this](){ this->smartFindFind(true);});
	QObject::connect(ui->smartFindPrevPushButton, &QPushButton::clicked,
					 [this](){ this->smartFindFind(false);});
	QObject::connect(ui->smartFindClearPushButton, &QPushButton::clicked,
					 [this]()
	{
		smartFindLearnData.clear();
		smartFindWindParams.clear();
		smartFindThresholds.clear();
	});
	QObject::connect(ui->smartFindSetAbsThrPushButton, &QPushButton::clicked,
					 [this]()
	{
		this->paramAbsThreshold.clear();
		for(auto p : qtLib::widgetsOfLayout<QDoubleSpinBox>(ui->smartFindGrid))
		{
			this->paramAbsThreshold.push_back(p->value()); /// CARE ABOUT ORDER
		}
	});

	/// IITP
//	QObject::connect(ui->iitpAutoCorrPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoCorrSlot()));
//	QObject::connect(ui->iitpAutoJumpPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoJumpSlot()));
	QObject::connect(ui->iitpManualPushButton, SIGNAL(clicked()), this, SLOT(iitpManualSlot()));
	QObject::connect(ui->iitpSaveNewNumPushButton, SIGNAL(clicked()), this, SLOT(saveNewNumSlot()));
	QObject::connect(ui->iitpRectifyEmgPushButton, SIGNAL(clicked()), this, SLOT(rectifyEmgSlot()));
#if !SHOW_IITP_WIDGETS
	qtLib::hideLayout(ui->iitpGrid);
//	qtLib::hideLayout(ui->linearApproxVertLayout);
#endif

	smartFindSetFuncs();
	ui->smartFindSetAbsThrPushButton->click();

	qtLib::hideLayout(ui->smartFindGrid);		/// for better times
}

Cut::~Cut()
{
	delete ui;
}

void Cut::paintResizedSlot(double a)
{
	if(this->size().width() == a * edfFil.getFreq() + scrollAreaGapX) { return; }
	/// horzNorm
	this->resize(a * edfFil.getFreq() / ui->xNormSpinBox->value()
				 + scrollAreaGapX, this->height());
}

void Cut::connectStuff()
{
	/// deriv
	QObject::connect(ui->derivChan1SpinBox,  SIGNAL(valueChanged(int)),
					 this, SLOT( showDerivatives() ));
	QObject::connect(ui->derivChan2SpinBox, SIGNAL( valueChanged(int) ),
					 this, SLOT( showDerivatives() ));


	/// draws
	QObject::connect(ui->iitpDisableEcgCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormInvertCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->xNormSpinBox,
					 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 this, &Cut::xNormSlot);
	QObject::connect(ui->paintStartDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT( paintResizedSlot(double)) );

	QObject::connect(ui->leftLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
	QObject::connect(ui->rightLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
}

void Cut::disconnectStuff()
{
	/// deriv
	QObject::disconnect(ui->derivChan1SpinBox,  SIGNAL( valueChanged(int) ),
					 this, SLOT( showDerivatives() ));
	QObject::disconnect(ui->derivChan2SpinBox, SIGNAL( valueChanged(int) ),
					 this, SLOT( showDerivatives() ));


	/// draws
	QObject::disconnect(ui->iitpDisableEcgCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::disconnect(ui->yNormInvertCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::disconnect(ui->yNormDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::disconnect(ui->xNormSpinBox,
					 static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 this, &Cut::xNormSlot);
	QObject::disconnect(ui->paintStartDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::disconnect(ui->paintLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
						this, SLOT( paintResizedSlot(double)) );
	QObject::disconnect(ui->leftLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
	QObject::disconnect(ui->rightLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
}

void Cut::timesAndDiffSlot()
{
	ui->rightTimeSpinBox->setValue(ui->rightLimitSpinBox->value() / edfFil.getFreq());
	ui->leftTimeSpinBox->setValue(ui->leftLimitSpinBox->value() / edfFil.getFreq());
	ui->diffLimitSpinBox->setValue(ui->rightLimitSpinBox->value() -
								   ui->leftLimitSpinBox->value());
	ui->diffTimeSpinBox->setValue(ui->diffLimitSpinBox->value() / edfFil.getFreq());
	showDerivatives();
	paintLimits();
}

#if 0
void Cut::drawSamples()
{
	std::vector<QLabel*> picLabels
	{
		ui->hz5Label,
				ui->hz10Label,
				ui->hz15Label,
				ui->hz20Label,
				ui->hz25Label,
				ui->hz30Label,
	};

	std::valarray<double> seen(150);
	for(int num = 0; num < 6; ++num)
	{
		int freq = 5 * (num + 1);
		seen = myLib::makeSine(150, freq);
		QPixmap pic = myLib::drawOneSignal(seen, 50);

		picLabels[num]->setPixmap(pic);
	}
}
#endif

void Cut::resizeEvent(QResizeEvent * event)
{
	if(event->size() == event->oldSize()) { return; }

	/// adjust scrollArea size
	double newPaintLength = smLib::doubleRound((event->size().width() - scrollAreaGapX)
											   / edfFil.getFreq() * ui->xNormSpinBox->value(),
									   ui->paintLengthDoubleSpinBox->decimals());
	double newHei = std::max(static_cast<int>(smLib::doubleRound(event->size().height(), -1)),
							 this->minimumHeight())
					- ui->scrollArea->geometry().y() - scrollAreaGapY;

	ui->scrollArea->setGeometry(ui->scrollArea->geometry().x(),
								ui->scrollArea->geometry().y(),
								/// horzNorm
								newPaintLength * edfFil.getFreq() / ui->xNormSpinBox->value(),
								newHei);
	ui->paintLengthDoubleSpinBox->setValue(newPaintLength);

	if(!dataCutLocal.isEmpty())
	{
		paint();
	}
}



bool Cut::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == ui->scrollArea)
	{
		switch(event->type())
		{
		case QEvent::Wheel:
		{
			/// for one "wheel step" there are TWO identical(?) wheel events
			/// workaround to process only one of two QWheelEvents
//			static bool procFlag = true; /// first
			static bool procFlag = false; /// second
			if(!procFlag)
			{
				procFlag = true;
				return false;
			}
			procFlag = false;

			QWheelEvent * scrollEvent = static_cast<QWheelEvent*>(event);
			const int step = (scrollEvent->delta() > 0) ? 1 : -1;

			/// control and no shift - x/y scaling
			if(scrollEvent->modifiers().testFlag(Qt::ControlModifier) &&
			   !scrollEvent->modifiers().testFlag(Qt::ShiftModifier))
			{
				if(scrollEvent->modifiers().testFlag(Qt::AltModifier))
				{
					/// horzNorm
					ui->xNormSpinBox->stepBy(step);
					return true;
				}
				else
				{
					ui->yNormDoubleSpinBox->stepBy(step);
					return true;
				}
			}
			/// else - horizontal scrolling
			/// step sign was changed 18-Oct-18 for x/y scales, so -1 in offset appeared
			double offset = ui->paintLengthDoubleSpinBox->value() * step * -1;
			if(scrollEvent->modifiers().testFlag(Qt::ShiftModifier))
			{
				if(scrollEvent->modifiers().testFlag(Qt::ControlModifier)) { offset *= 5; }
			}
			else { offset *= 0.125; }

			/// scroll overflow/underflow check
			if((leftDrawLimit
				+ ui->scrollArea->width()
				/// horzNorm
				* ui->xNormSpinBox->value() > dataCutLocal.cols()
				&& offset > 0)
			   || (leftDrawLimit == 0 && offset < 0))
			{
				break;
			}
			ui->paintStartDoubleSpinBox->setValue(
						ui->paintStartDoubleSpinBox->value() + offset);
			return true;
		} /// end of Wheel
		case QEvent::MouseButtonPress:
		{
			QMouseEvent * ev = static_cast<QMouseEvent*>(event);
			if(!ev->modifiers().testFlag(Qt::ShiftModifier)) { break; }

			/// start manualDraw
			int numChan = ui->color3SpinBox->value();
			if(numChan == -1) { break; }

			manualDrawFlag = true;
			manualDrawStart = ev->pos();
			manualDrawDataBackup = drawData;
			return true;
		} /// end of MouseButtonPress
		case QEvent::MouseMove:
		{
			/// draw some orange signal
			QMouseEvent * mouseMoveEvent = static_cast<QMouseEvent*>(event);
			if((mouseMoveEvent->buttons() & Qt::LeftButton) && manualDrawFlag)
			{
				manualDraw(mouseMoveEvent->pos());
				return true;
			}
			break;
		} /// end of MouseMove
		case QEvent::MouseButtonRelease:
		{
			if(manualDrawFlag)
			{
				manualDrawFlag = false;
				manualDrawAddUndo();
				return true;
			}
			QMouseEvent * clickEvent = static_cast<QMouseEvent*>(event);

			switch(clickEvent->button())
			{
			case Qt::BackButton:
			{
				if(clickEvent->modifiers().testFlag(Qt::ShiftModifier)) /// move to the start
				{ ui->paintStartDoubleSpinBox->setValue(0.); }
				else
				{ this->backwardFrameSlot(); }
				break;
			}
			case Qt::ForwardButton:
			{
				if(clickEvent->modifiers().testFlag(Qt::ShiftModifier)) /// move to the end
				{ ui->paintStartDoubleSpinBox->setValue(
								this->dataCutLocal.cols() / edfFil.getFreq() -
								ui->paintLengthDoubleSpinBox->value()); }
				else
				{ this->forwardFrameSlot(); }
				break;
			}
			case Qt::LeftButton:
			{
				if(clickEvent->modifiers().testFlag(Qt::ControlModifier)) /// select channel
				{
					if(clickEvent->modifiers().testFlag(Qt::AltModifier)) /// track amplitudes
					{
						ui->derivChan1SpinBox->setValue(this->getDrawnChannel(clickEvent->pos()));
					}
					else /// draw orange and manual draw/linearApprox
					{
						ui->color3SpinBox->setValue(this->getDrawnChannel(clickEvent->pos()));
					}
				}
				else
				{
					this->mousePressSlot(clickEvent->button(),  clickEvent->x());
				}
				return true;
			}
			case Qt::RightButton:
			{
				if(clickEvent->modifiers().testFlag(Qt::ControlModifier)
				   && clickEvent->modifiers().testFlag(Qt::AltModifier))
				{
					/// choose a channel to track amplitudes
					ui->derivChan2SpinBox->setValue(this->getDrawnChannel(clickEvent->pos()));
				}
				else
				{
					this->mousePressSlot(clickEvent->button(),
										 clickEvent->x());
				}
				return true;
			}
			case Qt::MiddleButton:
			{
				if(clickEvent->modifiers().testFlag(Qt::ControlModifier))
				{
					if(clickEvent->modifiers().testFlag(Qt::AltModifier))
					{
						ui->xNormSpinBox->setValue(1.); /// default xNorm
					}
					else
					{
						ui->yNormDoubleSpinBox->setValue(1.); /// default xNorm
					}
				}
				else
				{
					ui->yNormInvertCheckBox->click(); /// invert y
				}
				return true;
			}
			default: { break; }
			}
			break;
		} /// end of MouseButtonRelease
		case QEvent::KeyPress:
		{
			QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
			switch(keyEvent->key())
			{
			case Qt::Key_Home:
			{
				ui->paintStartDoubleSpinBox->setValue(0.);
				return true;
			}
			case Qt::Key_End:
			{
				ui->paintStartDoubleSpinBox->setValue(
							this->dataCutLocal.cols() / edfFil.getFreq() -
							ui->paintLengthDoubleSpinBox->value());
				return true;
			}
			case Qt::Key_Left:
			case Qt::Key_Right:
			{
				auto * targetSpin = ui->leftLimitSpinBox;
				if(keyEvent->modifiers().testFlag(Qt::ControlModifier))
				{
					targetSpin = ui->rightLimitSpinBox;
				}
				int steps = 1;
				if(keyEvent->modifiers().testFlag(Qt::ShiftModifier))
				{
					steps = 10; /// magic constant
				}

				switch(keyEvent->key())
				{
				case Qt::Key_Left:	{ targetSpin->stepBy(-steps);	break; }
				case Qt::Key_Right:	{ targetSpin->stepBy(+steps);	break; }
				default:			{ /* cant get here */ }
				}
				showDerivatives();
				paintLimits();
				return true;
			}
			case Qt::Key_Z:
			case Qt::Key_X:
			case Qt::Key_C:
			case Qt::Key_V:
			{
				if(keyEvent->modifiers().testFlag(Qt::ControlModifier))
				{
					/// Ctrl is ON
					switch(keyEvent->key())
					{
					case Qt::Key_Z:	{ undoSlot();	return true; }
					case Qt::Key_X:	{ cutSlot();	return true; }
					case Qt::Key_C:	{ copySlot();	return true; }
					case Qt::Key_V:	{ pasteSlot();	return true; }
					default:		{ /* can't really get here */ }
					}
				}
				break;
			}
			case Qt::Key_Up:
			case Qt::Key_Down:
			{
				const int step = (keyEvent->key() == Qt::Key_Up) ? +1 : -1;
				if(keyEvent->modifiers().testFlag(Qt::ControlModifier))
				{
					if(keyEvent->modifiers().testFlag(Qt::AltModifier))
					{
						/// horzNorm
						ui->xNormSpinBox->stepBy(step);
					}
					else
					{
						ui->yNormDoubleSpinBox->stepBy(step);
					}
					return true;
				}
				break; /// simple up/down has no effect
			}
			case Qt::Key_Q: { this->backwardFrameSlot(); return true; }
			case Qt::Key_E: { this->forwardFrameSlot(); return true; }
			}
			break;
		} /// end of KeyPress
		default: { /* do nothing */ }
		}
	}
	/// global Shortcuts
	{
		switch(event->type())
		{
		case QEvent::KeyPress:
		{
			QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
			switch(keyEvent->key())
			{
			case Qt::Key_A: { this->prev(); return true; }
			case Qt::Key_D: { this->next(); return true; }
			case Qt::Key_O: { this->browseSlot(); return true; }
			default: { break; }
			}
			break;
		}
		default: { break; }
		}
	}
	return QWidget::eventFilter(obj, event);
}

void Cut::resetLimits()
{
	if( !fileOpened ) { return; }
	ui->rightLimitSpinBox->setValue(dataCutLocal.cols());
	ui->leftLimitSpinBox->setValue(0); /// sometimes fails under Windows
}

void Cut::drawSpectre()
{
	const int num = ui->color3SpinBox->value();
	if(num == -1) { return; }

	if(ui->rightLimitSpinBox->value() - ui->leftLimitSpinBox->value() < 512) { return; }

	std::valarray<double> sig = smLib::contSubsec(dataCutLocal[num],
												  ui->leftLimitSpinBox->value(),
												  ui->rightLimitSpinBox->value());

	auto smoothSpec = myLib::smoothSpectre(
						  myLib::spectreWelchRtoR(sig,
												  0.5,
												  edfFil.getFreq(),
												  myLib::windowName::Hann,
												  512),
						  5);

	std::valarray<double> drawArr = myLib::subSpectrumR(
										smoothSpec,
										ui->refilterLowFreqSpinBox->value(),
										ui->refilterHighFreqSpinBox->value(),
										edfFil.getFreq());

	QString lab = edfFil.getLabels(num); lab.remove("EEG "); lab = lab.left(lab.indexOf('-'));
	QPixmap toDraw = myLib::drw::drawOneArray(
						 myLib::drw::drawOneTemplate(lab,
													 ui->refilterLowFreqSpinBox->value(),
													 ui->refilterHighFreqSpinBox->value()),
						 drawArr,
						 drawArr.max(),
						 "black");

	ui->spectreLabel->setPixmap(toDraw.scaled(ui->spectreLabel->size()));
}

void Cut::showDerivatives()
{
	if( !fileOpened ) { return; }
	const int st = 5;

	/// set value first chan
	int numSig1 = ui->derivChan1SpinBox->value();
	if(edfFil.getNs() >= coords::egi::manyChannels)
	{
		numSig1 = coords::egi::chans128to20[ui->derivChan1SpinBox->value()];
	}
//	if(numSig1 < dataCutLocal.rows())
	{
		const std::valarray<double> & sig1 = dataCutLocal[numSig1];
		const int ind1 = ui->leftLimitSpinBox->value();
		ui->derivVal1SpinBox->setValue(sig1[ind1]);
#if 01
		/// set derivatives first chan
		if(ind1 + st < sig1.size() && ind1 - st >=0)
		{
			ui->derivFirst1SpinBox->setValue(sig1[ind1 + st] -  sig1[ind1 - st]);
		}
		if(ind1 + 2 * st < sig1.size() && ind1 - 2 * st >= 0)
		{
			ui->derivSecond1SpinBox->setValue(sig1[ind1 + 2 * st] + sig1[ind1 - 2 * st] - 2 * sig1[ind1]);
		}
#endif
	}

	/// set value second chan
	int numSig2 = ui->derivChan2SpinBox->value();
	if(edfFil.getNs() >= coords::egi::manyChannels)
	{
		numSig2 = coords::egi::chans128to20[ui->derivChan2SpinBox->value()];
	}
//	if(numSig2 < dataCutLocal.rows())
	{
		const std::valarray<double> & sig2 = dataCutLocal[numSig2];
		const int ind2 = ui->rightLimitSpinBox->value();
		ui->derivVal2SpinBox->setValue(sig2[ind2]);
#if 01
		/// set derivatives second chan
		if(ind2 + st < sig2.size() && ind2 - st >=0)
		{
			ui->derivFirst2SpinBox->setValue(sig2[ind2 + st] -  sig2[ind2 - st]);
		}
		if(ind2 + 2 * st < sig2.size() && ind2 - 2 * st >= 0)
		{
			ui->derivSecond2SpinBox->setValue(sig2[ind2 + 2 * st] + sig2[ind2 - 2 * st] - 2 * sig2[ind2]);
		}
#endif
	}
}

void Cut::mousePressSlot(Qt::MouseButton btn, int coord)
{
	if(btn == Qt::LeftButton &&
	   /// horzNorm
	   leftDrawLimit + coord * ui->xNormSpinBox->value() < ui->rightLimitSpinBox->value())
	{
		ui->leftLimitSpinBox->setValue(leftDrawLimit + coord * ui->xNormSpinBox->value());
	}
	else if(btn == Qt::RightButton &&
			/// horzNorm
			leftDrawLimit + coord * ui->xNormSpinBox->value() > ui->leftLimitSpinBox->value() &&
			coord < dataCutLocal.cols())
	{
		ui->rightLimitSpinBox->setValue(leftDrawLimit + coord * ui->xNormSpinBox->value());
	}
	showDerivatives();
	paintLimits();
}

void Cut::colorSpinSlot(QSpinBox * spin, QLineEdit * lin)
{
	const int n = spin->value();
	if(n >=0)
	{
		if(myFileType == fileType::edf && !edfFil.isEmpty())
		{
			QString ch{};
			if(edfFil.getNs() >= coords::egi::manyChannels)
			{
				ch = edfFil.getLabels(coords::egi::chans128to20[n]);
			}
			else
			{
				ch = edfFil.getLabels(n);
			}

			/// iitp interesting markers
			if(DEFS.isUser(username::IITP) && ch.contains("IT") && ch.contains("_"))
			{
				int gonioNum{};
				for(const QString & in : iitp::gonioNames)
				{
					if(ch.contains(in))
					{
						gonioNum = myLib::indexOfVal(iitp::gonioNames, in);
						break;
					}
				}
				ui->iitpInterMarksLineEdit->setText(nm(iitp::gonioMinMarker(gonioNum))
													+ " "
													+ nm(iitp::gonioMinMarker(gonioNum) + 1));
			}

			lin->setText(ch
						 .remove("EEG ")
						 .remove("IT ")
						 .remove("EOG ")
						 .remove("EMG ")
						 );
		}

	}
	else
	{
		lin->clear();
	}
	paint();
}

std::vector<std::pair<int, QColor>> Cut::makeColouredChans()
{
	std::vector<std::pair<int, QColor>> res;
	for(auto p : this->colouredWidgets)
	{
		if(std::get<0>(p)->value() >= 0)
		{
			res.push_back(std::make_pair(std::get<0>(p)->value(),
										 QColor(std::get<2>(p)->text())));
		}
	}
	return res;
}

double Cut::normCoeff()
{
	return ui->yNormDoubleSpinBox->value()
			* ((ui->yNormInvertCheckBox->isChecked()) ? -1 : 1);
}

void Cut::paint()
{
	drawData = this->makeDrawData();
	if(drawData.rows() >= coords::egi::manyChannels)
	{
		drawData = drawData.subRows(coords::egi::chans128to20);
	}
	if(!drawData.isEmpty()) { this->paintData(drawData); }
}


matrix Cut::makeDrawData()
{
	if(dataCutLocal.isEmpty() || !drawFlag || !fileOpened) { return {}; }

	leftDrawLimit = ui->paintStartDoubleSpinBox->value() * edfFil.getFreq();
	int rightDrawLimit = std::min(leftDrawLimit +
								  /// horzNorm inside paintLength
								  static_cast<int>(ui->paintLengthDoubleSpinBox->value()
									  * edfFil.getFreq()),
								  static_cast<int>(dataCutLocal.cols()));
#if 0
	/// deprecate fileType::real
	if(myFileType == fileType::edf)
	{
		leftDrawLimit = ui->paintStartDoubleSpinBox->value() * edfFil.getFreq();
		rightDrawLimit = std::min(leftDrawLimit +
								  /// horzNorm
								  static_cast<int>(ui->paintLengthDoubleSpinBox->value()
								  * edfFil.getFreq()),
								  static_cast<int>(dataCutLocal.cols()));
	}
	else if(myFileType == fileType::real) /// to deprecate
	{
		leftDrawLimit = 0;
		rightDrawLimit = dataCutLocal.cols();
	}
#endif
	return dataCutLocal.subCols(leftDrawLimit, rightDrawLimit);
}

void Cut::paintData(matrix & drawDataLoc)
{
	int ecg = edfFil.findChannel("ECG");
	if(ui->iitpDisableEcgCheckBox->isChecked() && ecg != -1)
	{
		drawDataLoc[ecg] = 0; /// for iitp ecg
	}
	for(int ch : this->zeroedChannels)
	{
		if(ch < drawDataLoc.rows()) { drawDataLoc[ch] = 0; }
	}

	/// new horzNorm
	currentPic = myLib::drw::drawEeg(
					 /// horzNorm
					 drawDataLoc.subColsStride(0, ui->xNormSpinBox->value())  * normCoeff(),
					 /// horzNorm for time sticks
					 edfFil.getFreq() / ui->xNormSpinBox->value(),
					 ui->scrollArea->height(),
					 this->makeColouredChans());

	/// old horzNorm
//	currentPic = currentPic.scaledToWidth(drawDataLoc.cols() / ui->xNormSpinBox->value());

	paintMarkers(drawDataLoc);
	paintLimits();
}

void Cut::paintMarkers(const matrix & drawDataLoc)
{
	int mrk{-1};
	if(edfFil.getNs() >= coords::egi::manyChannels)
	{
		mrk = drawDataLoc.rows() - 1;
	}
	else
	{
		mrk = edfFil.getMarkChan();
	}
	if(mrk < 0) { return; }

	QPainter pnt;
	pnt.begin(&currentPic);

	pnt.setFont(QFont("", 14)); /// magic const
	for(int i = 0; i < drawDataLoc.cols(); ++i)
	{
		int toDraw = drawDataLoc[mrk][i];
		bool allowed = marksToDraw.empty() || myLib::contains(marksToDraw, toDraw);
		if(toDraw != 0. && allowed)
		{
			/// magic consts
			/// horzNorm
			pnt.drawText(i / ui->xNormSpinBox->value(),
						 pnt.device()->height() * (mrk + 1) / (drawDataLoc.rows() + 2) - 3,
						 nm(toDraw));
		}
	}
	pnt.end();
}

void Cut::paintLimits()
{
	if( !fileOpened ) { return; }
	if(currentPic.isNull() || !drawFlag) { return; }

	QPixmap tempPic = currentPic;
	QPainter paint;
	paint.begin(&tempPic);

	/// horzNorm
	int leftX = (ui->leftLimitSpinBox->value() - leftDrawLimit) / ui->xNormSpinBox->value();
	if(leftX >= 0 && leftX < ui->paintLengthDoubleSpinBox->value() * edfFil.getFreq())
	{
		paint.setPen(QPen(QBrush("blue"), 2));
		paint.drawLine(leftX, 0, leftX, tempPic.height());
	}

	/// horzNorm
	int rightX = (ui->rightLimitSpinBox->value() - leftDrawLimit) / ui->xNormSpinBox->value();
	if(rightX >= 0 && rightX < ui->paintLengthDoubleSpinBox->value() * edfFil.getFreq())
	{
		paint.setPen(QPen(QBrush("red"), 2));
		paint.drawLine(rightX, 0, rightX, tempPic.height());
	}

	ui->picLabel->setPixmap(tempPic.scaled(tempPic.width() - 2,				/// magic const
										   ui->scrollArea->height() - 2));	/// magic const
	paint.end();
}

/// check - works not especially accurate
int Cut::getDrawnChannel(const QPoint & clickPos)
{
	auto vals = this->drawData.getCol(clickPos.x());
	const double norm = normCoeff();
	double dist = 1000;
	int num = -1;

//	std::cout << "Y = " << clickPos.y() << "\t"
//			  << "scrH = " << ui->scrollArea->height() << "\t"
//			  << "picL = " << ui->picLabel->height() << "\t"
//			  << "pic = " << ui->picLabel->pixmap()->height() << "\t"
//			  << std::endl;
	for(int i = 0; i < drawData.rows(); ++i)
	{
		/// look myLib_drw.cpp, drawEeg(...), offsetY
		const double offsetY = (i + 1)
							   * ui->scrollArea->height()
							   / (drawData.rows() + 2.);

		const double D = std::abs(offsetY
								  + vals[i] * norm

								  - clickPos.y());
//		std::cout << i + 1 << "\t"
//				  << "val = " << vals[i] << "\t"
//				  << "norm = " << norm << "\t"
//				  << "offset = " << offsetY << "\t"
//				  << "sig = " << offsetY + vals[i] * norm << "\t"
//				  << "D = " << D << std::endl;
		if(D < dist)
		{
			dist = D;
			num = i;
		}
	}
	return num;
}

void Cut::manualDrawAddUndo()
{
	int start = leftDrawLimit;
	int k = ui->color3SpinBox->value();
	undoData.push_back(manualDrawDataBackup);
	auto undoAction = [start, k, this]()
	{
		std::copy(std::begin(undoData.back()[k]),
				  std::end(undoData.back()[k]),
				  std::begin(dataCutLocal[k]) + start);
		undoData.pop_back();
	};
	undoActions.push_back(undoAction);
}

void Cut::manualDraw(QPoint finP)
{
	const int numChan = ui->color3SpinBox->value();
	if(numChan == -1) { return; }
	if(manualDrawStart.x() == finP.x()) { return; }

	QPoint sta = manualDrawStart;
	QPoint fin = finP;
	manualDrawStart = fin;
	if(sta.x() > fin.x())
	{
		std::swap(sta, fin);
	}

	/// like in myLib_drw.cpp, function drawEeg, variable offsetY
	const double offsetY = (numChan + 1)
						   * ui->scrollArea->height()
						   / (drawData.rows() + 2.);
	/// horzNorm
	const int Xsta = sta.x() * ui->xNormSpinBox->value();
	const int Xfin = fin.x() * ui->xNormSpinBox->value();
	const double norm = normCoeff();

	for(int x = Xsta; x <= Xfin; ++x)
	{
		dataCutLocal[numChan][x + leftDrawLimit] =
				((sta.y() - offsetY)										/// init value
				 + (fin.y() - sta.y()) / static_cast<double>(Xfin - Xsta)	/// inclination
				 * (x - Xsta))												/// range
				/ norm;														/// norm
	}
	drawData = this->makeDrawData();
	repaintData(drawData, Xsta, Xfin);
}

void Cut::repaintData(matrix & drawDataLoc, int sta, int fin)
{
	int ecg = edfFil.findChannel("ECG");
	if(ui->iitpDisableEcgCheckBox->isChecked() && ecg != -1)
	{
		drawDataLoc[ecg] = 0; /// for iitp ecg
	}
	for(int ch : this->zeroedChannels)
	{
		if(ch < drawDataLoc.rows()) { drawDataLoc[ch] = 0; }
	}

	myLib::drw::redrawEeg(currentPic,
						  sta / ui->xNormSpinBox->value(),
						  fin / ui->xNormSpinBox->value(),
						  /// horzNorm
						  drawDataLoc.subColsStride(0, ui->xNormSpinBox->value()) * normCoeff(),
						  /// horzNorm
						  edfFil.getFreq() / ui->xNormSpinBox->value(),
						  /// horzNorm
						  ui->xNormSpinBox->value(),
						  this->makeColouredChans());

//	paintMarkers(drawDataLoc);
	paintLimits();
}
