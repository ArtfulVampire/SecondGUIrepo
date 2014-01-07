/********************************************************************************
** Form generated from reading UI file 'cut.ui'
**
** Created: Mon 3. Sep 14:39:18 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUT_H
#define UI_CUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Cut
{
public:
    QLabel *label_7;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QPushButton *cutButton;
    QPushButton *zeroButton;
    QSpacerItem *horizontalSpacer_3;
    QSpinBox *nsBox;
    QPushButton *saveButton;
    QSpinBox *eogSpinBox;
    QSpinBox *extYbox;
    QLabel *label_5;
    QSpinBox *tempSpinBox;
    QLabel *label_6;
    QDoubleSpinBox *eogDoubleSpinBox;
    QLabel *label_4;
    QLabel *label_10;
    QPushButton *cutEyesButton;
    QDoubleSpinBox *timeShiftSpinBox;
    QLabel *label_2;
    QLabel *label_3;
    QDoubleSpinBox *wndLengthSpinBox;
    QPushButton *cycleTimeShiftButton;
    QPushButton *cycleWndLengthButton;
    QPushButton *windowsButton;
    QCheckBox *checkBox_2;
    QPushButton *paintDistrButton;
    QLabel *label_8;
    QSpinBox *eyesSlicesSpinBox;
    QComboBox *dirBox;
    QLabel *picLabel;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QPushButton *browseButton;
    QCheckBox *checkBox;

    void setupUi(QWidget *Cut)
    {
        if (Cut->objectName().isEmpty())
            Cut->setObjectName(QString::fromUtf8("Cut"));
        Cut->resize(1079, 770);
        Cut->setMinimumSize(QSize(0, 770));
        label_7 = new QLabel(Cut);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(690, 20, 341, 241));
        layoutWidget = new QWidget(Cut);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 40, 651, 191));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        spinBox = new QSpinBox(layoutWidget);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));

        gridLayout->addWidget(spinBox, 0, 0, 1, 2);

        doubleSpinBox = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));

        gridLayout->addWidget(doubleSpinBox, 0, 2, 1, 2);

        prevButton = new QPushButton(layoutWidget);
        prevButton->setObjectName(QString::fromUtf8("prevButton"));

        gridLayout->addWidget(prevButton, 1, 0, 1, 2);

        nextButton = new QPushButton(layoutWidget);
        nextButton->setObjectName(QString::fromUtf8("nextButton"));

        gridLayout->addWidget(nextButton, 1, 2, 1, 2);

        cutButton = new QPushButton(layoutWidget);
        cutButton->setObjectName(QString::fromUtf8("cutButton"));

        gridLayout->addWidget(cutButton, 2, 0, 1, 2);

        zeroButton = new QPushButton(layoutWidget);
        zeroButton->setObjectName(QString::fromUtf8("zeroButton"));

        gridLayout->addWidget(zeroButton, 3, 0, 1, 2);

        horizontalSpacer_3 = new QSpacerItem(77, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 3, 2, 1, 2);

        nsBox = new QSpinBox(layoutWidget);
        nsBox->setObjectName(QString::fromUtf8("nsBox"));

        gridLayout->addWidget(nsBox, 4, 2, 1, 1);

        saveButton = new QPushButton(layoutWidget);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        gridLayout->addWidget(saveButton, 2, 2, 1, 2);

        eogSpinBox = new QSpinBox(layoutWidget);
        eogSpinBox->setObjectName(QString::fromUtf8("eogSpinBox"));

        gridLayout->addWidget(eogSpinBox, 5, 2, 1, 1);

        extYbox = new QSpinBox(layoutWidget);
        extYbox->setObjectName(QString::fromUtf8("extYbox"));

        gridLayout->addWidget(extYbox, 3, 5, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 3, 4, 1, 1);

        tempSpinBox = new QSpinBox(layoutWidget);
        tempSpinBox->setObjectName(QString::fromUtf8("tempSpinBox"));

        gridLayout->addWidget(tempSpinBox, 4, 5, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 4, 4, 1, 1);

        eogDoubleSpinBox = new QDoubleSpinBox(layoutWidget);
        eogDoubleSpinBox->setObjectName(QString::fromUtf8("eogDoubleSpinBox"));

        gridLayout->addWidget(eogDoubleSpinBox, 5, 4, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 5, 3, 1, 1);

        label_10 = new QLabel(layoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 4, 1, 1, 1);

        cutEyesButton = new QPushButton(layoutWidget);
        cutEyesButton->setObjectName(QString::fromUtf8("cutEyesButton"));

        gridLayout->addWidget(cutEyesButton, 5, 1, 1, 1);

        timeShiftSpinBox = new QDoubleSpinBox(layoutWidget);
        timeShiftSpinBox->setObjectName(QString::fromUtf8("timeShiftSpinBox"));

        gridLayout->addWidget(timeShiftSpinBox, 0, 5, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 4, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 1, 4, 1, 1);

        wndLengthSpinBox = new QDoubleSpinBox(layoutWidget);
        wndLengthSpinBox->setObjectName(QString::fromUtf8("wndLengthSpinBox"));

        gridLayout->addWidget(wndLengthSpinBox, 1, 5, 1, 1);

        cycleTimeShiftButton = new QPushButton(layoutWidget);
        cycleTimeShiftButton->setObjectName(QString::fromUtf8("cycleTimeShiftButton"));

        gridLayout->addWidget(cycleTimeShiftButton, 2, 4, 1, 1);

        cycleWndLengthButton = new QPushButton(layoutWidget);
        cycleWndLengthButton->setObjectName(QString::fromUtf8("cycleWndLengthButton"));

        gridLayout->addWidget(cycleWndLengthButton, 2, 5, 1, 1);

        windowsButton = new QPushButton(layoutWidget);
        windowsButton->setObjectName(QString::fromUtf8("windowsButton"));

        gridLayout->addWidget(windowsButton, 2, 6, 1, 1);

        checkBox_2 = new QCheckBox(layoutWidget);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        gridLayout->addWidget(checkBox_2, 1, 6, 1, 1);

        paintDistrButton = new QPushButton(layoutWidget);
        paintDistrButton->setObjectName(QString::fromUtf8("paintDistrButton"));

        gridLayout->addWidget(paintDistrButton, 3, 6, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 5, 5, 1, 1);

        eyesSlicesSpinBox = new QSpinBox(layoutWidget);
        eyesSlicesSpinBox->setObjectName(QString::fromUtf8("eyesSlicesSpinBox"));

        gridLayout->addWidget(eyesSlicesSpinBox, 5, 6, 1, 1);

        dirBox = new QComboBox(layoutWidget);
        dirBox->setObjectName(QString::fromUtf8("dirBox"));

        gridLayout->addWidget(dirBox, 5, 0, 1, 1);

        picLabel = new QLabel(Cut);
        picLabel->setObjectName(QString::fromUtf8("picLabel"));
        picLabel->setGeometry(QRect(10, 300, 261, 131));
        scrollArea = new QScrollArea(Cut);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(10, 240, 1050, 460));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1048, 458));
        scrollArea->setWidget(scrollAreaWidgetContents);
        layoutWidget1 = new QWidget(Cut);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(0, 0, 501, 31));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        lineEdit = new QLineEdit(layoutWidget1);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);

        browseButton = new QPushButton(layoutWidget1);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));

        horizontalLayout->addWidget(browseButton);

        checkBox = new QCheckBox(layoutWidget1);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        horizontalLayout->addWidget(checkBox);


        retranslateUi(Cut);

        QMetaObject::connectSlotsByName(Cut);
    } // setupUi

    void retranslateUi(QWidget *Cut)
    {
        Cut->setWindowTitle(QApplication::translate("Cut", "Form", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Cut", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Logistic</p></body></html>", 0, QApplication::UnicodeUTF8));
        prevButton->setText(QApplication::translate("Cut", "Prev", 0, QApplication::UnicodeUTF8));
        nextButton->setText(QApplication::translate("Cut", "Next", 0, QApplication::UnicodeUTF8));
        cutButton->setText(QApplication::translate("Cut", "Cut", 0, QApplication::UnicodeUTF8));
        zeroButton->setText(QApplication::translate("Cut", "Zero", 0, QApplication::UnicodeUTF8));
        saveButton->setText(QApplication::translate("Cut", "Save", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Cut", "extY", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Cut", "Temperature", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Cut", "NumOfDispersion", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Cut", "ns", 0, QApplication::UnicodeUTF8));
        cutEyesButton->setText(QApplication::translate("Cut", "Cut Eyes", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Cut", "timeShift", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Cut", "wndLength", 0, QApplication::UnicodeUTF8));
        cycleTimeShiftButton->setText(QApplication::translate("Cut", "Cycle timeSh", 0, QApplication::UnicodeUTF8));
        cycleWndLengthButton->setText(QApplication::translate("Cut", "Cycle wndL", 0, QApplication::UnicodeUTF8));
        windowsButton->setText(QApplication::translate("Cut", "Windows", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("Cut", "write windows?", 0, QApplication::UnicodeUTF8));
        paintDistrButton->setText(QApplication::translate("Cut", "Paint Distribution", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Cut", "NumOfEyesSlices", 0, QApplication::UnicodeUTF8));
        picLabel->setText(QApplication::translate("Cut", "TextLabel", 0, QApplication::UnicodeUTF8));
        browseButton->setText(QApplication::translate("Cut", "Bro&wse", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("Cut", "paint?", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Cut: public Ui_Cut {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUT_H
