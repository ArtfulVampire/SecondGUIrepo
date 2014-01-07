/********************************************************************************
** Form generated from reading UI file 'cfg.ui'
**
** Created: Tue Aug 28 20:34:01 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CFG_H
#define UI_CFG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cfg
{
public:
    QWidget *widget;
    QGridLayout *gridLayout;
    QLineEdit *nsLine;
    QLabel *label;
    QLineEdit *freqLine;
    QLabel *label_2;
    QLineEdit *classesLine;
    QLabel *label_3;
    QLineEdit *eLine;
    QLabel *label_4;
    QLineEdit *ErrLine;
    QLabel *label_5;
    QLineEdit *tempLine;
    QLabel *label_6;
    QLineEdit *nameEdit;
    QLabel *label_7;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *cfg)
    {
        if (cfg->objectName().isEmpty())
            cfg->setObjectName(QString::fromUtf8("cfg"));
        cfg->resize(326, 301);
        widget = new QWidget(cfg);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(20, 20, 286, 260));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        nsLine = new QLineEdit(widget);
        nsLine->setObjectName(QString::fromUtf8("nsLine"));

        gridLayout->addWidget(nsLine, 0, 0, 1, 1);

        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 1, 1, 1);

        freqLine = new QLineEdit(widget);
        freqLine->setObjectName(QString::fromUtf8("freqLine"));

        gridLayout->addWidget(freqLine, 1, 0, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 1, 1, 1);

        classesLine = new QLineEdit(widget);
        classesLine->setObjectName(QString::fromUtf8("classesLine"));

        gridLayout->addWidget(classesLine, 2, 0, 1, 1);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 1, 1, 1);

        eLine = new QLineEdit(widget);
        eLine->setObjectName(QString::fromUtf8("eLine"));

        gridLayout->addWidget(eLine, 3, 0, 1, 1);

        label_4 = new QLabel(widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 3, 1, 1, 1);

        ErrLine = new QLineEdit(widget);
        ErrLine->setObjectName(QString::fromUtf8("ErrLine"));

        gridLayout->addWidget(ErrLine, 4, 0, 1, 1);

        label_5 = new QLabel(widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 4, 1, 1, 1);

        tempLine = new QLineEdit(widget);
        tempLine->setObjectName(QString::fromUtf8("tempLine"));

        gridLayout->addWidget(tempLine, 5, 0, 1, 1);

        label_6 = new QLabel(widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 5, 1, 1, 1);

        nameEdit = new QLineEdit(widget);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        gridLayout->addWidget(nameEdit, 6, 0, 1, 1);

        label_7 = new QLabel(widget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 6, 1, 1, 1);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 7, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 7, 1, 1, 1);


        retranslateUi(cfg);

        QMetaObject::connectSlotsByName(cfg);
    } // setupUi

    void retranslateUi(QWidget *cfg)
    {
        cfg->setWindowTitle(QApplication::translate("cfg", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("cfg", "Number of channels", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("cfg", "spLength", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("cfg", "Number of outputs", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("cfg", "Learn Rate - Epsilon", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("cfg", "Error Limit", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("cfg", "Temperature", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("cfg", "File Name", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("cfg", "make", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class cfg: public Ui_cfg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CFG_H
