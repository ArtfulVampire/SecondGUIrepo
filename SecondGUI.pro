#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT       += core gui svg

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console c++14 #for better display of processes with cout.flush()
#QMAKE_CXXFLAGS += -std=c++14 #for C++14 standard
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

#INCLUDEPATH += /usr/include/libxml2
#LIBS += -L/usr/lib/x86_64-linux-gnu -lOpenCL
#LIBS += -L/usr/lib/x86_64-linux-gnu/mesa
#LIBS += -lxml2 -lmlpack

SOURCES += main.cpp\
        mainwindow.cpp \
    cut.cpp \
    spectre.cpp \
    edffile.cpp \
    matrix.cpp \
    coord.cpp \
    mainwindow_autos.cpp \
    myLib_clustering.cpp \
    myLib_general.cpp \
    net_classification.cpp \
    net_setsgets.cpp \
    myLib_signalProcessing.cpp \
    mainwindow_slices.cpp \
    mainwindow_visualisation.cpp \
    mainwindow_ica.cpp \
    mainwindow_editedf.cpp \
    myLib_draws.cpp \
    myLib_deprecate.cpp \
    myLib_dataHandler.cpp \
    mainwindow_customF.cpp \
    myLib_smallFuncs.cpp \
    net.cpp \
    classifier.cpp \
    classifier_ann.cpp \
    classifier_qda.cpp \
    classifier_svm.cpp \
    classifier_lda.cpp \
    classifier_ann_suc.cpp \
    classifier_dist.cpp \
    classifier_nbc.cpp \
    classifier_knn.cpp

HEADERS  += mainwindow.h \
    cut.h \
    net.h \
    spectre.h \
    qtempevent.h \
    tempthread.h \
    library.h \
    edffile.h \
    matrix.h \
    coord.h \
    library.h \
    smallLib.h \
    classifier.h

FORMS    += mainwindow.ui \
    cut.ui \
    net.ui \
    spectre.ui

OTHER_FILES +=

RESOURCES +=
