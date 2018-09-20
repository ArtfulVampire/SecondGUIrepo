#ifndef CHOOSECHANS_H
#define CHOOSECHANS_H

#include <QWidget>

namespace Ui {
class ChooseChans;
}

class ChooseChans : public QWidget
{
	Q_OBJECT

public:
	explicit ChooseChans();
	~ChooseChans();

private:
	Ui::ChooseChans *ui;
	static const int drawGap = 5;
	static const int checkBoxHeight = 20;
	static const int checkBoxWidth = 50;
	static const int pushButtonHeight = 30;
	static const int pushButtonWidth = 25;
	static const int checkUncheckWidth = 50;

};

#endif // CHOOSECHANS_H
