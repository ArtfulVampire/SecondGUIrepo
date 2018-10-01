#ifndef CHOOSECHANS_H
#define CHOOSECHANS_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>

namespace Ui {
class ChooseChans;
}

class ChooseChans : public QWidget
{
	Q_OBJECT

public:
	explicit ChooseChans();
	~ChooseChans();

signals:
	void strSig(const std::vector<int> & str);

private:
	Ui::ChooseChans *ui;
	static const int drawGap = 5;
	static const int checkBoxHeight = 20;
	static const int checkBoxWidth = 50;
	static const int pushButtonHeight = 30;
	static const int pushButtonWidth = 25;
	static const int checkUncheckWidth = 50;
	static const int checkColSize = 10;

	QVBoxLayout * both{new QVBoxLayout(this)};
	QGridLayout * checks{new QGridLayout()};

};

#endif // CHOOSECHANS_H
