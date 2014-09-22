#ifndef RANGEWIDGET_H
#define RANGEWIDGET_H

#include <QWidget>
#include "spectre.h"

namespace Ui {
    class RangeWidget;
}

class RangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RangeWidget(QWidget *parent = 0, int ns_ = 19, int left_=82, int right_=328, double spStep = 250./4096., int spLength_ = 247, QDir * dir_ = new QDir("/"));
    ~RangeWidget();
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::RangeWidget *ui;
    QPixmap pic;
    QPainter * paint;
    QString helpString;
    QDir * dirBC;
    QLabel * specLabel;
    int ** rangeLimits;
    int spLength;
    int ns;
    int left, right;
    double spStep;
    int helpInt;
    int chanNum;

};

#endif // RANGEWIDGET_H
