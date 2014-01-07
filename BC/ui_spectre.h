/********************************************************************************
** Form generated from reading UI file 'spectre.ui'
**
** Created: Tue Aug 28 20:34:01 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPECTRE_H
#define UI_SPECTRE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Spectre
{
public:
    QLabel *label_7;
    QPushButton *psaButton;
    QLabel *label_8;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_5;
    QLineEdit *lineEdit_m1;
    QLabel *label_5;
    QLineEdit *lineEdit_m2;
    QLabel *label_6;
    QPushButton *avButton;
    QSpacerItem *horizontalSpacer;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_3;
    QComboBox *comboBox;
    QSpinBox *leftSpinBox;
    QLabel *label_2;
    QGridLayout *gridLayout_2;
    QLineEdit *leftHzEdit;
    QLabel *label_9;
    QLineEdit *rightHzEdit;
    QLabel *label_10;
    QSpinBox *rightSpinBox;
    QLabel *label_3;
    QPushButton *countButton;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QCheckBox *fullSpBox;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout_4;
    QLineEdit *lineEdit_1;
    QPushButton *inputBroswe;
    QLineEdit *lineEdit_2;
    QPushButton *outputBroswe;
    QProgressBar *progressBar;
    QWidget *layoutWidget3;
    QGridLayout *gridLayout;
    QSpinBox *smoothBox;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QSpinBox *normBox;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_11;
    QPushButton *smoothButton;
    QCheckBox *checkBox_2;
    QPushButton *integrateButton;
    QLineEdit *integrateLineEdit;
    QLabel *label_12;

    void setupUi(QWidget *Spectre)
    {
        if (Spectre->objectName().isEmpty())
            Spectre->setObjectName(QString::fromUtf8("Spectre"));
        Spectre->resize(745, 419);
        label_7 = new QLabel(Spectre);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 280, 311, 30));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_7->setFont(font);
        psaButton = new QPushButton(Spectre);
        psaButton->setObjectName(QString::fromUtf8("psaButton"));
        psaButton->setGeometry(QRect(530, 40, 110, 27));
        label_8 = new QLabel(Spectre);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(470, 10, 260, 30));
        label_8->setFont(font);
        layoutWidget = new QWidget(Spectre);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 310, 221, 95));
        gridLayout_5 = new QGridLayout(layoutWidget);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        lineEdit_m1 = new QLineEdit(layoutWidget);
        lineEdit_m1->setObjectName(QString::fromUtf8("lineEdit_m1"));

        gridLayout_5->addWidget(lineEdit_m1, 0, 0, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_5->addWidget(label_5, 0, 1, 1, 1);

        lineEdit_m2 = new QLineEdit(layoutWidget);
        lineEdit_m2->setObjectName(QString::fromUtf8("lineEdit_m2"));

        gridLayout_5->addWidget(lineEdit_m2, 1, 0, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_5->addWidget(label_6, 1, 1, 1, 1);

        avButton = new QPushButton(layoutWidget);
        avButton->setObjectName(QString::fromUtf8("avButton"));

        gridLayout_5->addWidget(avButton, 2, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer, 2, 1, 1, 1);

        layoutWidget1 = new QWidget(Spectre);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 120, 231, 158));
        gridLayout_3 = new QGridLayout(layoutWidget1);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        comboBox = new QComboBox(layoutWidget1);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout_3->addWidget(comboBox, 0, 0, 1, 2);

        leftSpinBox = new QSpinBox(layoutWidget1);
        leftSpinBox->setObjectName(QString::fromUtf8("leftSpinBox"));

        gridLayout_3->addWidget(leftSpinBox, 1, 0, 1, 1);

        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 1, 1, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        leftHzEdit = new QLineEdit(layoutWidget1);
        leftHzEdit->setObjectName(QString::fromUtf8("leftHzEdit"));

        gridLayout_2->addWidget(leftHzEdit, 0, 0, 1, 1);

        label_9 = new QLabel(layoutWidget1);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_2->addWidget(label_9, 0, 1, 1, 1);

        rightHzEdit = new QLineEdit(layoutWidget1);
        rightHzEdit->setObjectName(QString::fromUtf8("rightHzEdit"));

        gridLayout_2->addWidget(rightHzEdit, 1, 0, 1, 1);

        label_10 = new QLabel(layoutWidget1);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_2->addWidget(label_10, 1, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 1, 2, 2, 1);

        rightSpinBox = new QSpinBox(layoutWidget1);
        rightSpinBox->setObjectName(QString::fromUtf8("rightSpinBox"));

        gridLayout_3->addWidget(rightSpinBox, 2, 0, 1, 1);

        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_3->addWidget(label_3, 2, 1, 1, 1);

        countButton = new QPushButton(layoutWidget1);
        countButton->setObjectName(QString::fromUtf8("countButton"));

        gridLayout_3->addWidget(countButton, 3, 0, 1, 2);

        checkBox = new QCheckBox(layoutWidget1);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout_3->addWidget(checkBox, 3, 2, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(layoutWidget1);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        fullSpBox = new QCheckBox(layoutWidget1);
        fullSpBox->setObjectName(QString::fromUtf8("fullSpBox"));

        horizontalLayout->addWidget(fullSpBox);


        gridLayout_3->addLayout(horizontalLayout, 0, 2, 1, 1);

        layoutWidget2 = new QWidget(Spectre);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(10, 11, 431, 93));
        gridLayout_4 = new QGridLayout(layoutWidget2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        lineEdit_1 = new QLineEdit(layoutWidget2);
        lineEdit_1->setObjectName(QString::fromUtf8("lineEdit_1"));

        gridLayout_4->addWidget(lineEdit_1, 0, 0, 1, 1);

        inputBroswe = new QPushButton(layoutWidget2);
        inputBroswe->setObjectName(QString::fromUtf8("inputBroswe"));

        gridLayout_4->addWidget(inputBroswe, 0, 1, 1, 1);

        lineEdit_2 = new QLineEdit(layoutWidget2);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        gridLayout_4->addWidget(lineEdit_2, 1, 0, 1, 1);

        outputBroswe = new QPushButton(layoutWidget2);
        outputBroswe->setObjectName(QString::fromUtf8("outputBroswe"));

        gridLayout_4->addWidget(outputBroswe, 1, 1, 1, 1);

        progressBar = new QProgressBar(layoutWidget2);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        gridLayout_4->addWidget(progressBar, 2, 0, 1, 1);

        layoutWidget3 = new QWidget(Spectre);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(270, 120, 241, 95));
        gridLayout = new QGridLayout(layoutWidget3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        smoothBox = new QSpinBox(layoutWidget3);
        smoothBox->setObjectName(QString::fromUtf8("smoothBox"));

        gridLayout->addWidget(smoothBox, 0, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(37, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 1, 1, 1);

        label_4 = new QLabel(layoutWidget3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 0, 2, 1, 1);

        normBox = new QSpinBox(layoutWidget3);
        normBox->setObjectName(QString::fromUtf8("normBox"));

        gridLayout->addWidget(normBox, 1, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(37, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 1, 1, 1);

        label_11 = new QLabel(layoutWidget3);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 1, 2, 1, 1);

        smoothButton = new QPushButton(layoutWidget3);
        smoothButton->setObjectName(QString::fromUtf8("smoothButton"));

        gridLayout->addWidget(smoothButton, 2, 0, 1, 2);

        checkBox_2 = new QCheckBox(layoutWidget3);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        gridLayout->addWidget(checkBox_2, 2, 2, 1, 1);

        integrateButton = new QPushButton(Spectre);
        integrateButton->setObjectName(QString::fromUtf8("integrateButton"));
        integrateButton->setGeometry(QRect(270, 240, 93, 27));
        integrateLineEdit = new QLineEdit(Spectre);
        integrateLineEdit->setObjectName(QString::fromUtf8("integrateLineEdit"));
        integrateLineEdit->setGeometry(QRect(370, 270, 311, 27));
        label_12 = new QLabel(Spectre);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(370, 240, 200, 30));

        retranslateUi(Spectre);

        QMetaObject::connectSlotsByName(Spectre);
    } // setupUi

    void retranslateUi(QWidget *Spectre)
    {
        Spectre->setWindowTitle(QApplication::translate("Spectre", "Form", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Spectre", "Please dont forget about I/O Dirs", 0, QApplication::UnicodeUTF8));
        psaButton->setText(QApplication::translate("Spectre", "Compare PSA", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Spectre", "Dont forget output filename", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Spectre", "file markers", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Spectre", "output filename", 0, QApplication::UnicodeUTF8));
        avButton->setText(QApplication::translate("Spectre", "AV compare", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Spectre", "left", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Spectre", "left Hz", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Spectre", "right Hz", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Spectre", "right", 0, QApplication::UnicodeUTF8));
        countButton->setText(QApplication::translate("Spectre", "set spValues", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("Spectre", "count spectra", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Spectre", "fftLength", 0, QApplication::UnicodeUTF8));
        fullSpBox->setText(QApplication::translate("Spectre", "full", 0, QApplication::UnicodeUTF8));
        inputBroswe->setText(QApplication::translate("Spectre", "Input Dir", 0, QApplication::UnicodeUTF8));
        outputBroswe->setText(QApplication::translate("Spectre", "Output Dir", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Spectre", "# smooth steps", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Spectre", "* norm coefficient", 0, QApplication::UnicodeUTF8));
        smoothButton->setText(QApplication::translate("Spectre", "Smooth", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("Spectre", "paint?", 0, QApplication::UnicodeUTF8));
        integrateButton->setText(QApplication::translate("Spectre", "Integrate", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Spectre", "specify the intervals", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Spectre: public Ui_Spectre {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPECTRE_H
