/********************************************************************************
** Form generated from reading UI file 'net.ui'
**
** Created: Tue Aug 28 20:34:01 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NET_H
#define UI_NET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Net
{
public:
    QPushButton *testButton;
    QCheckBox *drawWtsBox;
    QLineEdit *lineEdit;
    QPushButton *nextButton;
    QPushButton *prevButton;
    QLabel *label_7;
    QLabel *label_8;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QDoubleSpinBox *errorSpinBox;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QSpinBox *epochSpinBox;
    QLabel *label_4;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QSpinBox *tempBox;
    QDoubleSpinBox *errorBox;
    QDoubleSpinBox *learnRateBox;
    QPushButton *compressButton;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_2;
    QPushButton *loadWtsButton;
    QPushButton *saveWtsButton;
    QPushButton *loadNetButton;
    QPushButton *loadPaButton;
    QPushButton *resetButton;
    QPushButton *testAllButton;
    QPushButton *learnButton;
    QPushButton *StopButton;
    QPushButton *drawWtsButton;
    QLineEdit *zeroChannelsLineEdit;
    QLabel *label_6;

    void setupUi(QWidget *Net)
    {
        if (Net->objectName().isEmpty())
            Net->setObjectName(QString::fromUtf8("Net"));
        Net->resize(906, 512);
        testButton = new QPushButton(Net);
        testButton->setObjectName(QString::fromUtf8("testButton"));
        testButton->setGeometry(QRect(290, 130, 85, 27));
        drawWtsBox = new QCheckBox(Net);
        drawWtsBox->setObjectName(QString::fromUtf8("drawWtsBox"));
        drawWtsBox->setGeometry(QRect(20, 130, 94, 22));
        lineEdit = new QLineEdit(Net);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(290, 170, 371, 27));
        nextButton = new QPushButton(Net);
        nextButton->setObjectName(QString::fromUtf8("nextButton"));
        nextButton->setGeometry(QRect(400, 210, 93, 27));
        prevButton = new QPushButton(Net);
        prevButton->setObjectName(QString::fromUtf8("prevButton"));
        prevButton->setGeometry(QRect(290, 210, 93, 27));
        label_7 = new QLabel(Net);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(290, 270, 221, 151));
        label_8 = new QLabel(Net);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(530, 270, 221, 151));
        layoutWidget = new QWidget(Net);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 200, 241, 165));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        errorSpinBox = new QDoubleSpinBox(layoutWidget);
        errorSpinBox->setObjectName(QString::fromUtf8("errorSpinBox"));

        horizontalLayout->addWidget(errorSpinBox);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 0, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        epochSpinBox = new QSpinBox(layoutWidget);
        epochSpinBox->setObjectName(QString::fromUtf8("epochSpinBox"));

        horizontalLayout_2->addWidget(epochSpinBox);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 1, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 2, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 1, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 4, 1, 1, 1);

        tempBox = new QSpinBox(layoutWidget);
        tempBox->setObjectName(QString::fromUtf8("tempBox"));

        gridLayout->addWidget(tempBox, 4, 0, 1, 1);

        errorBox = new QDoubleSpinBox(layoutWidget);
        errorBox->setObjectName(QString::fromUtf8("errorBox"));

        gridLayout->addWidget(errorBox, 3, 0, 1, 1);

        learnRateBox = new QDoubleSpinBox(layoutWidget);
        learnRateBox->setObjectName(QString::fromUtf8("learnRateBox"));

        gridLayout->addWidget(learnRateBox, 2, 0, 1, 1);

        compressButton = new QPushButton(Net);
        compressButton->setObjectName(QString::fromUtf8("compressButton"));
        compressButton->setGeometry(QRect(290, 10, 93, 30));
        layoutWidget1 = new QWidget(Net);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(12, 12, 269, 95));
        gridLayout_2 = new QGridLayout(layoutWidget1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        loadWtsButton = new QPushButton(layoutWidget1);
        loadWtsButton->setObjectName(QString::fromUtf8("loadWtsButton"));

        gridLayout_2->addWidget(loadWtsButton, 0, 0, 1, 1);

        saveWtsButton = new QPushButton(layoutWidget1);
        saveWtsButton->setObjectName(QString::fromUtf8("saveWtsButton"));

        gridLayout_2->addWidget(saveWtsButton, 0, 1, 1, 1);

        loadNetButton = new QPushButton(layoutWidget1);
        loadNetButton->setObjectName(QString::fromUtf8("loadNetButton"));

        gridLayout_2->addWidget(loadNetButton, 0, 2, 1, 1);

        loadPaButton = new QPushButton(layoutWidget1);
        loadPaButton->setObjectName(QString::fromUtf8("loadPaButton"));

        gridLayout_2->addWidget(loadPaButton, 1, 0, 1, 1);

        resetButton = new QPushButton(layoutWidget1);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));

        gridLayout_2->addWidget(resetButton, 1, 1, 1, 1);

        testAllButton = new QPushButton(layoutWidget1);
        testAllButton->setObjectName(QString::fromUtf8("testAllButton"));

        gridLayout_2->addWidget(testAllButton, 1, 2, 1, 1);

        learnButton = new QPushButton(layoutWidget1);
        learnButton->setObjectName(QString::fromUtf8("learnButton"));

        gridLayout_2->addWidget(learnButton, 2, 0, 1, 1);

        StopButton = new QPushButton(layoutWidget1);
        StopButton->setObjectName(QString::fromUtf8("StopButton"));

        gridLayout_2->addWidget(StopButton, 2, 1, 1, 1);

        drawWtsButton = new QPushButton(layoutWidget1);
        drawWtsButton->setObjectName(QString::fromUtf8("drawWtsButton"));

        gridLayout_2->addWidget(drawWtsButton, 2, 2, 1, 1);

        zeroChannelsLineEdit = new QLineEdit(Net);
        zeroChannelsLineEdit->setObjectName(QString::fromUtf8("zeroChannelsLineEdit"));
        zeroChannelsLineEdit->setGeometry(QRect(400, 80, 401, 27));
        label_6 = new QLabel(Net);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(400, 50, 350, 30));

        retranslateUi(Net);

        QMetaObject::connectSlotsByName(Net);
    } // setupUi

    void retranslateUi(QWidget *Net)
    {
        Net->setWindowTitle(QApplication::translate("Net", "Form", 0, QApplication::UnicodeUTF8));
        testButton->setText(QApplication::translate("Net", "Test", 0, QApplication::UnicodeUTF8));
        drawWtsBox->setText(QApplication::translate("Net", "Draw Wts", 0, QApplication::UnicodeUTF8));
        nextButton->setText(QApplication::translate("Net", "Next", 0, QApplication::UnicodeUTF8));
        prevButton->setText(QApplication::translate("Net", "Prev", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Net", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Net", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Net", "Error", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Net", "Epoch", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Net", "Learn Rate", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Net", "Error limit", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Net", "Temperature", 0, QApplication::UnicodeUTF8));
        compressButton->setText(QApplication::translate("Net", "Compress", 0, QApplication::UnicodeUTF8));
        loadWtsButton->setText(QApplication::translate("Net", "Load wts", 0, QApplication::UnicodeUTF8));
        saveWtsButton->setText(QApplication::translate("Net", "Save Wts", 0, QApplication::UnicodeUTF8));
        loadNetButton->setText(QApplication::translate("Net", "Load CFG", 0, QApplication::UnicodeUTF8));
        loadPaButton->setText(QApplication::translate("Net", "Load pa", 0, QApplication::UnicodeUTF8));
        resetButton->setText(QApplication::translate("Net", "Reset", 0, QApplication::UnicodeUTF8));
        testAllButton->setText(QApplication::translate("Net", "Test All", 0, QApplication::UnicodeUTF8));
        learnButton->setText(QApplication::translate("Net", "Learn", 0, QApplication::UnicodeUTF8));
        StopButton->setText(QApplication::translate("Net", "Stop", 0, QApplication::UnicodeUTF8));
        drawWtsButton->setText(QApplication::translate("Net", "drawWts", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Net", "channels not to consider (effects right after CFG)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Net: public Ui_Net {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NET_H
