/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue Aug 28 20:34:01 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

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
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *browseButton;
    QLineEdit *filePath;
    QCheckBox *checkBox;
    QLineEdit *setNsLine;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_6;
    QSpacerItem *verticalSpacer_2;
    QPushButton *cutEDF;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout;
    QCheckBox *sliceBox;
    QCheckBox *eyesBox;
    QCheckBox *chRdcBox;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *realButton;
    QRadioButton *windButton;
    QGridLayout *gridLayout;
    QComboBox *wndLengthBox;
    QLabel *label_2;
    QSpinBox *timeShiftBox;
    QLabel *label_3;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *ntRadio;
    QRadioButton *enRadio;
    QProgressBar *progressBar;
    QPushButton *rdcNsButton;
    QComboBox *reduceNsBox;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *nsLine;
    QComboBox *rdcChannelBox;
    QPushButton *countSpectra;
    QPushButton *cleanDirsButton;
    QPushButton *cut_e;
    QPushButton *eyesButton;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *makeCFG;
    QPushButton *makePA;
    QPushButton *netButton;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QLineEdit *Name;
    QLabel *label_8;
    QLabel *label;
    QTextEdit *textEdit;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *drawButton;
    QDoubleSpinBox *doubleSpinBox;
    QComboBox *dirBox;
    QWidget *layoutWidget3;
    QHBoxLayout *horizontalLayout_7;
    QGridLayout *gridLayout_4;
    QPushButton *waveletButton;
    QSpinBox *tempBox;
    QLabel *label_4;
    QVBoxLayout *verticalLayout_7;
    QCheckBox *classBox;
    QHBoxLayout *horizontalLayout_6;
    QCheckBox *weightsBox;
    QDoubleSpinBox *normBox;
    QLabel *label_7;
    QCheckBox *waveletsBox;
    QCheckBox *fullBox;
    QGridLayout *gridLayout_3;
    QDoubleSpinBox *startTimeBox;
    QLabel *label_5;
    QPushButton *datFileButton;
    QDoubleSpinBox *finishTimeBox;
    QLabel *label_6;
    QCheckBox *edfBox;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1174, 703);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        browseButton = new QPushButton(centralWidget);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setGeometry(QRect(550, 0, 93, 27));
        browseButton->setFlat(false);
        filePath = new QLineEdit(centralWidget);
        filePath->setObjectName(QString::fromUtf8("filePath"));
        filePath->setGeometry(QRect(10, 0, 511, 27));
        checkBox = new QCheckBox(centralWidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(670, 0, 94, 22));
        setNsLine = new QLineEdit(centralWidget);
        setNsLine->setObjectName(QString::fromUtf8("setNsLine"));
        setNsLine->setGeometry(QRect(300, 60, 110, 27));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(297, 100, 665, 403));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_2);

        cutEDF = new QPushButton(layoutWidget);
        cutEDF->setObjectName(QString::fromUtf8("cutEDF"));

        verticalLayout_6->addWidget(cutEDF);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);


        horizontalLayout_2->addLayout(verticalLayout_6);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        sliceBox = new QCheckBox(layoutWidget);
        sliceBox->setObjectName(QString::fromUtf8("sliceBox"));

        verticalLayout->addWidget(sliceBox);

        eyesBox = new QCheckBox(layoutWidget);
        eyesBox->setObjectName(QString::fromUtf8("eyesBox"));

        verticalLayout->addWidget(eyesBox);

        chRdcBox = new QCheckBox(layoutWidget);
        chRdcBox->setObjectName(QString::fromUtf8("chRdcBox"));

        verticalLayout->addWidget(chRdcBox);


        horizontalLayout_2->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        realButton = new QRadioButton(layoutWidget);
        realButton->setObjectName(QString::fromUtf8("realButton"));

        verticalLayout_2->addWidget(realButton);

        windButton = new QRadioButton(layoutWidget);
        windButton->setObjectName(QString::fromUtf8("windButton"));

        verticalLayout_2->addWidget(windButton);


        horizontalLayout_2->addLayout(verticalLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        wndLengthBox = new QComboBox(layoutWidget);
        wndLengthBox->setObjectName(QString::fromUtf8("wndLengthBox"));

        gridLayout->addWidget(wndLengthBox, 0, 0, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 1, 1, 1);

        timeShiftBox = new QSpinBox(layoutWidget);
        timeShiftBox->setObjectName(QString::fromUtf8("timeShiftBox"));

        gridLayout->addWidget(timeShiftBox, 1, 0, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 1, 1, 1, 1);


        horizontalLayout_2->addLayout(gridLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        ntRadio = new QRadioButton(layoutWidget);
        ntRadio->setObjectName(QString::fromUtf8("ntRadio"));

        verticalLayout_3->addWidget(ntRadio);

        enRadio = new QRadioButton(layoutWidget);
        enRadio->setObjectName(QString::fromUtf8("enRadio"));

        verticalLayout_3->addWidget(enRadio);


        horizontalLayout_2->addLayout(verticalLayout_3);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 3);

        progressBar = new QProgressBar(layoutWidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        gridLayout_2->addWidget(progressBar, 1, 1, 1, 2);

        rdcNsButton = new QPushButton(layoutWidget);
        rdcNsButton->setObjectName(QString::fromUtf8("rdcNsButton"));

        gridLayout_2->addWidget(rdcNsButton, 2, 0, 1, 1);

        reduceNsBox = new QComboBox(layoutWidget);
        reduceNsBox->setObjectName(QString::fromUtf8("reduceNsBox"));

        gridLayout_2->addWidget(reduceNsBox, 2, 1, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        nsLine = new QLineEdit(layoutWidget);
        nsLine->setObjectName(QString::fromUtf8("nsLine"));

        horizontalLayout_5->addWidget(nsLine);

        rdcChannelBox = new QComboBox(layoutWidget);
        rdcChannelBox->setObjectName(QString::fromUtf8("rdcChannelBox"));

        horizontalLayout_5->addWidget(rdcChannelBox);


        gridLayout_2->addLayout(horizontalLayout_5, 2, 2, 1, 1);

        countSpectra = new QPushButton(layoutWidget);
        countSpectra->setObjectName(QString::fromUtf8("countSpectra"));

        gridLayout_2->addWidget(countSpectra, 3, 0, 1, 1);

        cleanDirsButton = new QPushButton(layoutWidget);
        cleanDirsButton->setObjectName(QString::fromUtf8("cleanDirsButton"));

        gridLayout_2->addWidget(cleanDirsButton, 4, 0, 1, 1);

        cut_e = new QPushButton(layoutWidget);
        cut_e->setObjectName(QString::fromUtf8("cut_e"));

        gridLayout_2->addWidget(cut_e, 5, 0, 1, 1);

        eyesButton = new QPushButton(layoutWidget);
        eyesButton->setObjectName(QString::fromUtf8("eyesButton"));

        gridLayout_2->addWidget(eyesButton, 5, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        gridLayout_2->addLayout(horizontalLayout_4, 5, 2, 1, 1);

        makeCFG = new QPushButton(layoutWidget);
        makeCFG->setObjectName(QString::fromUtf8("makeCFG"));

        gridLayout_2->addWidget(makeCFG, 6, 0, 1, 1);

        makePA = new QPushButton(layoutWidget);
        makePA->setObjectName(QString::fromUtf8("makePA"));

        gridLayout_2->addWidget(makePA, 7, 0, 1, 1);

        netButton = new QPushButton(layoutWidget);
        netButton->setObjectName(QString::fromUtf8("netButton"));

        gridLayout_2->addWidget(netButton, 8, 0, 1, 1);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(11, 52, 258, 401));
        verticalLayout_4 = new QVBoxLayout(layoutWidget1);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Name = new QLineEdit(layoutWidget1);
        Name->setObjectName(QString::fromUtf8("Name"));

        horizontalLayout->addWidget(Name);

        label_8 = new QLabel(layoutWidget1);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout->addWidget(label_8);


        verticalLayout_4->addLayout(horizontalLayout);

        label = new QLabel(layoutWidget1);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("Andale Mono"));
        font.setPointSize(14);
        label->setFont(font);

        verticalLayout_4->addWidget(label);

        textEdit = new QTextEdit(layoutWidget1);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout_4->addWidget(textEdit);

        layoutWidget2 = new QWidget(centralWidget);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(970, 130, 158, 71));
        verticalLayout_5 = new QVBoxLayout(layoutWidget2);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        drawButton = new QPushButton(layoutWidget2);
        drawButton->setObjectName(QString::fromUtf8("drawButton"));

        horizontalLayout_3->addWidget(drawButton);

        doubleSpinBox = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));

        horizontalLayout_3->addWidget(doubleSpinBox);


        verticalLayout_5->addLayout(horizontalLayout_3);

        dirBox = new QComboBox(layoutWidget2);
        dirBox->setObjectName(QString::fromUtf8("dirBox"));

        verticalLayout_5->addWidget(dirBox);

        layoutWidget3 = new QWidget(centralWidget);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(302, 512, 651, 117));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget3);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setSpacing(6);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        waveletButton = new QPushButton(layoutWidget3);
        waveletButton->setObjectName(QString::fromUtf8("waveletButton"));

        gridLayout_4->addWidget(waveletButton, 0, 0, 1, 2);

        tempBox = new QSpinBox(layoutWidget3);
        tempBox->setObjectName(QString::fromUtf8("tempBox"));

        gridLayout_4->addWidget(tempBox, 1, 0, 1, 1);

        label_4 = new QLabel(layoutWidget3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_4->addWidget(label_4, 1, 1, 1, 1);


        horizontalLayout_7->addLayout(gridLayout_4);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        classBox = new QCheckBox(layoutWidget3);
        classBox->setObjectName(QString::fromUtf8("classBox"));

        verticalLayout_7->addWidget(classBox);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        weightsBox = new QCheckBox(layoutWidget3);
        weightsBox->setObjectName(QString::fromUtf8("weightsBox"));

        horizontalLayout_6->addWidget(weightsBox);

        normBox = new QDoubleSpinBox(layoutWidget3);
        normBox->setObjectName(QString::fromUtf8("normBox"));

        horizontalLayout_6->addWidget(normBox);

        label_7 = new QLabel(layoutWidget3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_6->addWidget(label_7);


        verticalLayout_7->addLayout(horizontalLayout_6);

        waveletsBox = new QCheckBox(layoutWidget3);
        waveletsBox->setObjectName(QString::fromUtf8("waveletsBox"));

        verticalLayout_7->addWidget(waveletsBox);

        fullBox = new QCheckBox(layoutWidget3);
        fullBox->setObjectName(QString::fromUtf8("fullBox"));

        verticalLayout_7->addWidget(fullBox);


        horizontalLayout_7->addLayout(verticalLayout_7);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        startTimeBox = new QDoubleSpinBox(layoutWidget3);
        startTimeBox->setObjectName(QString::fromUtf8("startTimeBox"));

        gridLayout_3->addWidget(startTimeBox, 0, 0, 1, 1);

        label_5 = new QLabel(layoutWidget3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_3->addWidget(label_5, 0, 1, 1, 1);

        datFileButton = new QPushButton(layoutWidget3);
        datFileButton->setObjectName(QString::fromUtf8("datFileButton"));

        gridLayout_3->addWidget(datFileButton, 0, 2, 1, 1);

        finishTimeBox = new QDoubleSpinBox(layoutWidget3);
        finishTimeBox->setObjectName(QString::fromUtf8("finishTimeBox"));

        gridLayout_3->addWidget(finishTimeBox, 1, 0, 1, 1);

        label_6 = new QLabel(layoutWidget3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_3->addWidget(label_6, 1, 1, 1, 1);

        edfBox = new QCheckBox(layoutWidget3);
        edfBox->setObjectName(QString::fromUtf8("edfBox"));

        gridLayout_3->addWidget(edfBox, 1, 2, 1, 1);


        horizontalLayout_7->addLayout(gridLayout_3);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1174, 25));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        browseButton->setText(QApplication::translate("MainWindow", "Bro&wse", 0, QApplication::UnicodeUTF8));
        filePath->setText(QString());
        checkBox->setText(QApplication::translate("MainWindow", "Allow", 0, QApplication::UnicodeUTF8));
        cutEDF->setText(QApplication::translate("MainWindow", "Cut EDF", 0, QApplication::UnicodeUTF8));
        sliceBox->setText(QApplication::translate("MainWindow", "Slice?", 0, QApplication::UnicodeUTF8));
        eyesBox->setText(QApplication::translate("MainWindow", "clean from eyes?", 0, QApplication::UnicodeUTF8));
        chRdcBox->setText(QApplication::translate("MainWindow", "reduce channels?", 0, QApplication::UnicodeUTF8));
        realButton->setText(QApplication::translate("MainWindow", "Realisations", 0, QApplication::UnicodeUTF8));
        windButton->setText(QApplication::translate("MainWindow", "windows", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "wndLength", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "timeShift", 0, QApplication::UnicodeUTF8));
        ntRadio->setText(QApplication::translate("MainWindow", "NeuroTravel", 0, QApplication::UnicodeUTF8));
        enRadio->setText(QApplication::translate("MainWindow", "Encephalan", 0, QApplication::UnicodeUTF8));
        rdcNsButton->setText(QApplication::translate("MainWindow", "Reduce ns", 0, QApplication::UnicodeUTF8));
        countSpectra->setText(QApplication::translate("MainWindow", "Count &Spectra", 0, QApplication::UnicodeUTF8));
        cleanDirsButton->setText(QApplication::translate("MainWindow", "&Clean wndDirs", 0, QApplication::UnicodeUTF8));
        cut_e->setText(QApplication::translate("MainWindow", "Look &through", 0, QApplication::UnicodeUTF8));
        eyesButton->setText(QApplication::translate("MainWindow", "&Eyes", 0, QApplication::UnicodeUTF8));
        makeCFG->setText(QApplication::translate("MainWindow", "Make C&FG", 0, QApplication::UnicodeUTF8));
        makePA->setText(QApplication::translate("MainWindow", "Make P&A", 0, QApplication::UnicodeUTF8));
        netButton->setText(QApplication::translate("MainWindow", "&Net", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Testee name", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Noctification Area", 0, QApplication::UnicodeUTF8));
        drawButton->setText(QApplication::translate("MainWindow", "&Draw", 0, QApplication::UnicodeUTF8));
        waveletButton->setText(QApplication::translate("MainWindow", "Wavelet", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "temp", 0, QApplication::UnicodeUTF8));
        classBox->setText(QApplication::translate("MainWindow", "class", 0, QApplication::UnicodeUTF8));
        weightsBox->setText(QApplication::translate("MainWindow", "weights", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "norm", 0, QApplication::UnicodeUTF8));
        waveletsBox->setText(QApplication::translate("MainWindow", "wavelets", 0, QApplication::UnicodeUTF8));
        fullBox->setText(QApplication::translate("MainWindow", "full", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "begin", 0, QApplication::UnicodeUTF8));
        datFileButton->setText(QApplication::translate("MainWindow", "Make .dat", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "end", 0, QApplication::UnicodeUTF8));
        edfBox->setText(QApplication::translate("MainWindow", "save as EDF", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
