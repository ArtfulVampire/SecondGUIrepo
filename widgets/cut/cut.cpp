#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/qtlib.h>
#include <myLib/output.h>

using namespace myOut;

Cut::Cut() :
    ui(new Ui::Cut)
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle("Cut-e");

	/// files
	ui->subdirComboBox->addItem("");
	ui->subdirComboBox->addItem("Reals");
	ui->subdirComboBox->addItem("winds");
	ui->subdirComboBox->addItem("winds/fromreal"); // generality
	ui->subdirComboBox->setCurrentText(""); /// me

	ui->suffixComboBox->addItem("");
	ui->suffixComboBox->addItem("eeg");
	ui->suffixComboBox->addItem("emg");
	ui->suffixComboBox->addItem("sum");
	ui->suffixComboBox->addItem("new");
	ui->suffixComboBox->setCurrentText("");
//	ui->suffixComboBox->setCurrentText("sum"); /// iitp

	/// draws
	ui->yNormDoubleSpinBox->setDecimals(2);
	ui->yNormDoubleSpinBox->setMaximum(5.);
	ui->yNormDoubleSpinBox->setMinimum(0.01);
	ui->yNormDoubleSpinBox->setValue(1.);
	ui->yNormDoubleSpinBox->setSingleStep(0.05);

    ui->paintStartDoubleSpinBox->setDecimals(1);
    ui->paintStartDoubleSpinBox->setSingleStep(0.1);
	ui->paintStartDoubleSpinBox->setValue(0);

    ui->paintLengthDoubleSpinBox->setDecimals(1);
	ui->paintLengthDoubleSpinBox->setSingleStep(0.2);

//	drawSamples();

    ui->scrollArea->setWidget(ui->picLabel);
    ui->scrollArea->installEventFilter(this);
	ui->picLabel->installEventFilter(this);

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
	ui->derivVal1SpinBox->setMaximum(1000);		ui->derivVal1SpinBox->setMinimum(-1000);
	ui->derivVal2SpinBox->setMaximum(1000);		ui->derivVal2SpinBox->setMinimum(-1000);
	ui->derivFirst1SpinBox->setMaximum(500);	ui->derivFirst1SpinBox->setMinimum(-500);
	ui->derivFirst2SpinBox->setMaximum(500);	ui->derivFirst2SpinBox->setMinimum(-500);
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
	ui->nextButton->setShortcut(tr("d"));
	ui->prevButton->setShortcut(tr("a"));
	ui->saveSubsecPushButton->setShortcut(tr("c"));
	ui->zeroButton->setShortcut(tr("z"));
	ui->saveButton->setShortcut(tr("s"));
	ui->splitButton->setShortcut(tr("x"));
	ui->rewriteButton->setShortcut(tr("r"));
	ui->forwardFrameButton->setShortcut(QKeySequence::Forward);
	ui->backwardFrameButton->setShortcut(QKeySequence::Back);
	ui->forwardFrameButton->setShortcut(tr("e"));
	ui->backwardFrameButton->setShortcut(tr("q"));
	QShortcut * undoShortcut = new QShortcut(QKeySequence(tr("Ctrl+z")), this);
	QShortcut * copyShortcut = new QShortcut(QKeySequence(tr("Ctrl+c")), this);
	QShortcut * pasteShortcut = new QShortcut(QKeySequence(tr("Ctrl+v")), this);
	QShortcut * cutShortcut = new QShortcut(QKeySequence(tr("Ctrl+x")), this);

	/// files
	QObject::connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseSlot()));
    QObject::connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(next()));
	QObject::connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prev()));
	QObject::connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));
	QObject::connect(ui->saveSubsecPushButton, SIGNAL(clicked()), this, SLOT(saveSubsecSlot()));
	QObject::connect(ui->rewriteButton, SIGNAL(clicked()), this, SLOT(rewrite()));

	/// modify
	QObject::connect(undoShortcut, SIGNAL(activated()), this, SLOT(undoSlot()));
	QObject::connect(copyShortcut, SIGNAL(activated()), this, SLOT(copySlot()));
	QObject::connect(pasteShortcut, SIGNAL(activated()), this, SLOT(pasteSlot()));
	QObject::connect(cutShortcut, SIGNAL(activated()), this, SLOT(cutSlot()));
	QObject::connect(ui->linearApproxPushButton, SIGNAL(clicked()), this, SLOT(linearApproxSlot()));
	QObject::connect(ui->zeroButton, SIGNAL(clicked()), this, SLOT(zeroSlot()));
	QObject::connect(ui->splitButton, SIGNAL(clicked()), this, SLOT(splitSlot()));
	QObject::connect(ui->zeroFromZeroPushButton, SIGNAL(clicked()), this, SLOT(zeroFromZeroSlot()));
	QObject::connect(ui->splitFromZeroPushButton, SIGNAL(clicked()), this, SLOT(splitFromZeroSlot()));
	QObject::connect(ui->zeroTillEndPushButton, SIGNAL(clicked()), this, SLOT(zeroTillEndSlot()));
	QObject::connect(ui->splitTillEndPushButton, SIGNAL(clicked()), this, SLOT(splitTillEndSlot()));
	QObject::connect(ui->cutPausesPushButton, SIGNAL(clicked()), this, SLOT(cutPausesSlot()));
	QObject::connect(ui->subtractMeansPushButton, &QPushButton::clicked,
					 [this]()
	{ for(auto & row : dataCutLocal) { row -= smLib::mean(row); } paint(); }); /// MARKERS!
	QObject::connect(ui->setMarkLeftPushButton, &QPushButton::clicked,
					 [this]() { this->setMarkerSlot(true); });
	QObject::connect(ui->setMarkRightPushButton, &QPushButton::clicked,
					 [this]() { this->setMarkerSlot(false); });

	/// navi
    QObject::connect(ui->forwardStepButton, SIGNAL(clicked()), this, SLOT(forwardStepSlot()));
    QObject::connect(ui->backwardStepButton, SIGNAL(clicked()), this, SLOT(backwardStepSlot()));
    QObject::connect(ui->forwardFrameButton, SIGNAL(clicked()), this, SLOT(forwardFrameSlot()));
	QObject::connect(ui->backwardFrameButton, SIGNAL(clicked()), this, SLOT(backwardFrameSlot()));
	QObject::connect(ui->findNextMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(ui->findNextMarkSpinBox->value()); });
	QObject::connect(ui->findNextNonzeroMarkPushButton, &QPushButton::clicked,
					 [this](){ findNextMark(-1); });
	QObject::connect(ui->findPrevMarkPushButton, &QPushButton::clicked,
					 [this](){ findPrevMark(ui->findPrevMarkSpinBox->value()); });
	QObject::connect(ui->findPrevNonzeroMarkPushButton, &QPushButton::clicked,
					 [this](){ findPrevMark(-1); });


	/// draws
	QObject::connect(ui->iitpDisableEcgCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormInvertCheckBox, &QCheckBox::clicked, this, &Cut::paint);
	QObject::connect(ui->yNormDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintStartDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(paint()));
	QObject::connect(ui->paintLengthDoubleSpinBox, SIGNAL(valueChanged(double)),
					 this, SLOT(resizeWidget(double)));
	QObject::connect(this, SIGNAL(buttonPressed(char,int)), this, SLOT(mousePressSlot(char,int)));
	QObject::connect(ui->leftLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
	QObject::connect(ui->rightLimitSpinBox, SIGNAL(valueChanged(int)),
					 this, SLOT(timesAndDiffSlot()));
	for(auto p : colouredWidgets)
	{
		QObject::connect(std::get<0>(p), static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
						 [this, p](){ this->colorSpinSlot(std::get<0>(p), std::get<1>(p)); } );
		QObject::connect(std::get<2>(p), SIGNAL(returnPressed()), this, SLOT(paint()));
	}

	/// smartFind
	QObject::connect(ui->smartFindShowPushButton, &QPushButton::clicked,
					 [this](){ this->smartFindShowValues(); });
	QObject::connect(ui->smartFindLearnPushButton, SIGNAL(clicked()), this, SLOT(smartFindLearnSlot()));
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
		std::cout << "smartFindAbsThrSet: " << this->paramAbsThreshold << std::endl;
	});

	/// IITP
	QObject::connect(ui->iitpAutoCorrPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoCorrSlot()));
	QObject::connect(ui->iitpAutoJumpPushButton, SIGNAL(clicked()), this, SLOT(iitpAutoJumpSlot()));
	QObject::connect(ui->iitpManualPushButton, SIGNAL(clicked()), this, SLOT(iitpManualSlot()));
	QObject::connect(ui->iitpSaveNewNumPushButton, SIGNAL(clicked()), this, SLOT(saveNewNumSlot()));

	smartFindSetFuncs();
	ui->smartFindSetAbsThrPushButton->click();
}

Cut::~Cut()
{
    delete ui;
}


void Cut::timesAndDiffSlot()
{
	ui->rightTimeSpinBox->setValue(ui->rightLimitSpinBox->value() / currFreq);
	ui->leftTimeSpinBox->setValue(ui->leftLimitSpinBox->value() / currFreq);
	ui->diffLimitSpinBox->setValue(ui->rightLimitSpinBox->value() -
								   ui->leftLimitSpinBox->value());
	ui->diffTimeSpinBox->setValue(ui->diffLimitSpinBox->value() / currFreq);
	showDerivatives();
	paintLimits();
}

//void Cut::drawSamples()
//{
//	std::vector<QLabel *> picLabels{
//		ui->hz5Label,
//				ui->hz10Label,
//				ui->hz15Label,
//				ui->hz20Label,
//				ui->hz25Label,
//				ui->hz30Label
//	};

//	std::valarray<double> seen(150);
//	for(int num = 0; num < 6; ++num)
//	{
//		int freq = 5 * (num + 1);
//		seen = myLib::makeSine(150, freq);
//		QPixmap pic = myLib::drawOneSignal(seen, 50);

//		picLabels[num]->setPixmap(pic);
////		picLabels[num]->setPixmap(pic.scaledToHeight(picLabels[num]->height()));
//	}
////	ui->hzLayout->setGeometry(QRect(ui->hzLayout->geometry().x(),
////									ui->hzLayout->geometry().y(),
////									ui->hzLayout->sizeHint().width(),
////									ui->hzLayout->sizeHint().height()));
//}

void Cut::resizeEvent(QResizeEvent * event)
{
    // adjust scrollArea size
	double newLen = smLib::doubleRound((event->size().width() - 10 * 2) / currFreq,
										  ui->paintLengthDoubleSpinBox->decimals());
    ui->scrollArea->setGeometry(ui->scrollArea->geometry().x(),
                                ui->scrollArea->geometry().y(),
								newLen * currFreq,
                                ui->scrollArea->geometry().height());
	ui->paintLengthDoubleSpinBox->setValue(newLen);
	if(!dataCutLocal.isEmpty())
	{
		paint();
	}
}

bool Cut::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->picLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) emit buttonPressed('l', mouseEvent->x());
            if(mouseEvent->button() == Qt::RightButton) emit buttonPressed('r', mouseEvent->x());
            return true;
        }
        else
        {
            return false;
        }
    }

    if(obj == ui->scrollArea)
    {
        if(event->type() == QEvent::Wheel)
        {
            QWheelEvent * scrollEvent = static_cast<QWheelEvent*>(event);
            int offset = -0.8 * scrollEvent->delta();

            if(myFileType == fileType::real)
            {
//                std::cout << "real" << std::endl;
                ui->scrollArea->horizontalScrollBar()->setSliderPosition(
                            ui->scrollArea->horizontalScrollBar()->sliderPosition() +
                            offset);
                return true;
            }
            else if(myFileType == fileType::edf)
            {
//                std::cout << "edf" << std::endl;
				if((leftDrawLimit + ui->scrollArea->width() > dataCutLocal.cols() && offset > 0) ||
                   (leftDrawLimit == 0 && offset < 0))
                {
                    return false;
                }
				leftDrawLimit = std::min(leftDrawLimit + offset, int(dataCutLocal.cols()));
				ui->paintStartDoubleSpinBox->setValue(leftDrawLimit / currFreq);
                return true;
            }
            else
            {
//                std::cout << "none" << std::endl;
                return false;
            }
        }
		else if(event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent * clickEvent = static_cast<QMouseEvent*>(event);
			if(clickEvent->button() == Qt::BackButton)
			{
				this->backwardFrameSlot();
			}
			else if(clickEvent->button() == Qt::ForwardButton)
			{
				this->forwardFrameSlot();
			}

			return true;
		}
		else if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);

			if((keyEvent->key() == Qt::Key_Left) || (keyEvent->key() == Qt::Key_Right))
			{
				auto * targetSpin = ui->leftLimitSpinBox;
				if(keyEvent->modifiers().testFlag(Qt::ControlModifier))
				{
					targetSpin = ui->rightLimitSpinBox;
				}

				if(keyEvent->key() == Qt::Key_Left)
				{
					targetSpin->stepDown();
				}
				else if(keyEvent->key() == Qt::Key_Right)
				{
					targetSpin->stepUp();
				}
				showDerivatives();
				paintLimits();
			}
		}

        else
		{
            return false;
        }
    }

    return QWidget::eventFilter(obj, event);
}


void Cut::resetLimits()
{
	if( !fileOpened ) { return; }

	ui->leftLimitSpinBox->setValue(0);
	ui->rightLimitSpinBox->setValue(dataCutLocal.cols());
}


void Cut::showDerivatives()
{
	if( !fileOpened ) { return; }

	const int st = 5;

	const std::valarray<double> & sig1 = dataCutLocal[ui->derivChan1SpinBox->value()];
	const int ind1 = ui->leftLimitSpinBox->value();
	ui->derivVal1SpinBox->setValue(sig1[ind1]);
	if(ind1 + st < sig1.size() && ind1 - st >=0)
	{
		ui->derivFirst1SpinBox->setValue(sig1[ind1 + st] -  sig1[ind1 - st]);
	}
	if(ind1 + 2 * st < sig1.size() && ind1 - 2 * st >=0)
	{
		ui->derivSecond1SpinBox->setValue(sig1[ind1 + 2 * st] + sig1[ind1 - 2 * st] - 2 * sig1[ind1]);
	}

	const std::valarray<double> & sig2 = dataCutLocal[ui->derivChan2SpinBox->value()];
	const int ind2 = ui->rightLimitSpinBox->value();
	ui->derivVal2SpinBox->setValue(sig2[ind2]);
	if(ind2 + st < sig2.size() && ind2 - st >=0)
	{
		ui->derivFirst2SpinBox->setValue(sig2[ind2 + st] -  sig2[ind2 - st]);
	}
	if(ind2 + 2 * st < sig2.size() && ind2 - 2 * st >=0)
	{
		ui->derivSecond2SpinBox->setValue(sig2[ind2 + 2 * st] + sig2[ind2 - 2 * st] - 2 * sig2[ind2]);
	}


}



void Cut::mousePressSlot(char btn, int coord)
{
	if(btn == 'l' &&
	   coord + leftDrawLimit < ui->rightLimitSpinBox->value())
	{
		ui->leftLimitSpinBox->setValue(coord + leftDrawLimit);
	}
	else if(btn == 'r' &&
			coord + leftDrawLimit > ui->leftLimitSpinBox->value() &&
			coord < dataCutLocal.cols())
	{
		ui->rightLimitSpinBox->setValue(coord + leftDrawLimit);
	}
	showDerivatives();
	paintLimits();
}





void Cut::resizeWidget(double a)
{
	if(this->size().width() == a * currFreq + 20) return;
	this->resize(a * currFreq + 20, this->height());
}

void Cut::colorSpinSlot(QSpinBox * spin, QLineEdit * lin)
{
	const int n = spin->value();
	if(n >=0)
	{
		if(myFileType == fileType::edf && !edfFil.isEmpty())
		{
			QString ch = QString(edfFil.getLabels()[n]);
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
		if(
		   std::get<0>(p)->value() >= 0 &&
		   std::get<0>(p)->value() < edfFil.getNs()
		   )
		{
			res.push_back(std::make_pair(std::get<0>(p)->value(),
										 QColor(std::get<2>(p)->text())));
		}
	}
	return res;
}

void Cut::paint() // save to tmp.jpg and display
{
	if(dataCutLocal.isEmpty()) return;

	int rightDrawLimit = 0.;
    if(myFileType == fileType::edf)
    {
		leftDrawLimit = ui->paintStartDoubleSpinBox->value() * currFreq;
		rightDrawLimit = std::min(leftDrawLimit + ui->scrollArea->width(), int(dataCutLocal.cols()));
    }
    else if(myFileType == fileType::real)
    {
        leftDrawLimit = 0;
		rightDrawLimit = dataCutLocal.cols();
    }
	matrix subData = dataCutLocal.subCols(leftDrawLimit, rightDrawLimit);

	int ecg = edfFil.findChannel("ECG");
	if(ui->iitpDisableEcgCheckBox->isChecked() && ecg >= 0)
	{
		subData[ecg] = 0; /// for iitp ecg
	}

	double coeff = ui->yNormDoubleSpinBox->value()
				   * ((ui->yNormInvertCheckBox->isChecked())? -1 : 1);
	auto colouredChans = this->makeColouredChans();

	currentPic = myLib::drw::drawEeg(subData * coeff,
									 currFreq,
									 colouredChans);


	/// draw markers numbers
	if(1)
	{
		const int mrk = edfFil.getMarkChan();
//		std::cout << mrk << std::endl;
		if(mrk > 0)
		{
			QPainter pnt;
			pnt.begin(&currentPic);
			pnt.setFont(QFont("", 18)); /// magic const

			for(int i = 0; i < subData.cols(); ++i)
			{
				if(subData[mrk][i] != 0.)
				{
					/// magic consts
					pnt.drawText(i,
								 pnt.device()->height() * (mrk + 1) / (subData.rows() + 2) - 3,
								 nm(int(subData[mrk][i])));
				}
			}
			pnt.end();
		}
	}

	paintLimits();
}

void Cut::paintLimits()
{
	if( !fileOpened ) { return; }
	if(currentPic.isNull()) { return; }

	QPixmap tempPic = currentPic;
	QPainter paint;
	paint.begin(&tempPic);

	int leftX = ui->leftLimitSpinBox->value() - leftDrawLimit;
	if(leftX >= 0 && leftX < ui->paintLengthDoubleSpinBox->value() * this->currFreq)
	{
		paint.setPen(QPen(QBrush("blue"), 2));
		paint.drawLine(leftX, 0, leftX, tempPic.height());
	}

	int rightX = ui->rightLimitSpinBox->value() - leftDrawLimit;
	if(rightX >= 0 && rightX < ui->paintLengthDoubleSpinBox->value() * this->currFreq)
	{
		paint.setPen(QPen(QBrush("red"), 2));
		paint.drawLine(rightX, 0, rightX, tempPic.height());
	}

	ui->picLabel->setPixmap(tempPic.scaled(tempPic.width() - 2,
											  ui->scrollArea->height() - 20));
	paint.end();
}

