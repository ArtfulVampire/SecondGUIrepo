/********************************************************************************
** Form generated from reading UI file 'eyes.ui'
**
** Created: Tue Aug 28 20:34:01 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EYES_H
#define UI_EYES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Eyes
{
public:
    QLineEdit *lineEdit;
    QPushButton *browseButton;
    QLabel *label_2;
    QPushButton *startButton;
    QLabel *label;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QSpinBox *spinBox;
    QLineEdit *lineEdit_3;

    void setupUi(QWidget *Eyes)
    {
        if (Eyes->objectName().isEmpty())
            Eyes->setObjectName(QString::fromUtf8("Eyes"));
        Eyes->resize(653, 221);
        lineEdit = new QLineEdit(Eyes);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(10, 30, 351, 27));
        browseButton = new QPushButton(Eyes);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setGeometry(QRect(370, 30, 93, 27));
        label_2 = new QLabel(Eyes);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 10, 240, 20));
        startButton = new QPushButton(Eyes);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setGeometry(QRect(370, 140, 93, 27));
        label = new QLabel(Eyes);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 70, 170, 17));
        lineEdit_2 = new QLineEdit(Eyes);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(10, 90, 61, 27));
        label_3 = new QLabel(Eyes);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(280, 70, 120, 17));
        spinBox = new QSpinBox(Eyes);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(290, 90, 55, 27));
        lineEdit_3 = new QLineEdit(Eyes);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(10, 140, 351, 27));

        retranslateUi(Eyes);

        QMetaObject::connectSlotsByName(Eyes);
    } // setupUi

    void retranslateUi(QWidget *Eyes)
    {
        Eyes->setWindowTitle(QApplication::translate("Eyes", "Form", 0, QApplication::UnicodeUTF8));
        browseButton->setText(QApplication::translate("Eyes", "Browse", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Eyes", "choose files to build regression", 0, QApplication::UnicodeUTF8));
        startButton->setText(QApplication::translate("Eyes", "Start", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Eyes", "numbers of eog channels", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Eyes", "last eeg channel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Eyes: public Ui_Eyes {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EYES_H
