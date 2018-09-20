#include "choosechans.h"
#include "ui_choosechans.h"

#include <QLabel>

ChooseChans::ChooseChans() :
	ui(new Ui::ChooseChans)
{
	ui->setupUi(this);

	/// consider 128 channels + status
	QLabel * checkLabel = new QLabel("check", this);
	QLabel * uncheckLabel = new QLabel("uncheck", this);

	checkLabel->setGeometry(drawGap,
							drawGap,
							checkUncheckWidth,
							pushButtonHeight);
	uncheckLabel->setGeometry(drawGap,
							  drawGap + pushButtonHeight + drawGap,
							  checkUncheckWidth,
							  pushButtonHeight);
}

ChooseChans::~ChooseChans()
{
	delete ui;
}
