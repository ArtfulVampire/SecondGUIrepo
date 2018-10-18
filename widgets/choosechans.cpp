#include "choosechans.h"
#include "ui_choosechans.h"

#include <other/coords.h>
#include <other/defs.h>
#include <myLib/output.h>

#include <QLabel>
#include <QCheckBox>

using namespace myOut;

std::pair<int, int> getPosition(int i, int colSize)
{
	return {(i - 1) % colSize, (i - 1) / colSize};
}

ChooseChans::ChooseChans() :
	ui(new Ui::ChooseChans)
{
	ui->setupUi(this);

	/// checkboxes
	/// 8 colomns by 16 items
	for(int i = 1; i <= 128; ++i)
	{
		QCheckBox * bx = new QCheckBox(nm(i), this);
		bx->setGeometry(0, 0, checkBoxWidth, checkBoxHeight);
		bx->setChecked(true);
		auto pos = getPosition(i, checkColSize);
		checks->addWidget(bx, pos.first, pos.second);
	}
	checks->setGeometry(QRect(drawGap, drawGap + 2 * pushButtonHeight + drawGap,
							  std::ceil(128. / checkColSize) * checkBoxWidth,
							  checkColSize * checkBoxHeight));

	/// check/uncheck cluster
	auto setCheckBoxes = [this](const QString & area, bool ch)
	{
		if(!coords::egi::chans128.count(area)) { return; }
		std::vector<QString> chans = coords::egi::chans128.at(area);
		for(auto chan : chans)
		{
			int num = chan.remove(" ").toInt();
			auto pos = getPosition(num, checkColSize);
			static_cast<QCheckBox*>
					( checks->itemAtPosition(pos.first, pos.second)->widget() )
					->setChecked(ch);
		}
	};

	/// cluster checkboxes
	QHBoxLayout * buttons{new QHBoxLayout()};
	int colCounter = 1;
	for(const auto & area : coords::egi::chans128groups)
	{
		QCheckBox * chk = new QCheckBox(area, this);
		chk->setGeometry(0, 0, pushButtonWidth, pushButtonHeight);
		chk->setChecked(true);
		QObject::connect(chk, &QCheckBox::clicked,
						 [area, this, setCheckBoxes](bool ch){ setCheckBoxes(area, ch); sendSig(); });
		buttons->addWidget(chk, colCounter);
		++colCounter;
	}

	/// "all" cluster
	QCheckBox * chk = new QCheckBox("all", this);
	chk->setGeometry(0, 0, pushButtonWidth, pushButtonHeight);
	chk->setChecked(true);
	QObject::connect(chk, &QCheckBox::clicked,
					 [this](bool i)
	{
		for(int num = 1; num <= 128; ++num)
		{
			auto pos = getPosition(num, checkColSize);
			static_cast<QCheckBox*>
					(checks->itemAtPosition(pos.first, pos.second)->widget())->setChecked(i);
		}
	});
	buttons->addWidget(chk, colCounter);
	buttons->setGeometry(QRect(drawGap, drawGap,
							   checkUncheckWidth + (colCounter - 1) * pushButtonWidth,
							   2 * pushButtonHeight + drawGap));

	/// unite
	both->addItem(buttons);
	both->addItem(checks);
	both->setGeometry(QRect(drawGap, drawGap,
							std::max(buttons->geometry().width(), checks->geometry().width()),
							buttons->geometry().height() + checks->geometry().height()));
	both->setSizeConstraint(QLayout::SetFixedSize);

	this->setAttribute(Qt::WA_DeleteOnClose);
}

ChooseChans::~ChooseChans()
{
	delete ui;
}

void ChooseChans::sendSig()
{
	std::vector<int> chansToSkip{};
	for(int i = 1; i <= 128; ++i)
	{
		auto pos = getPosition(i, checkColSize);
		if(!(static_cast<QCheckBox*>
			 (checks->itemAtPosition(pos.first, pos.second)->widget())
			 ->isChecked())
		   )
		{
			chansToSkip.push_back(i - 1); /// count form 0 when sending
		}
	}
	emit strSig(chansToSkip);
}
