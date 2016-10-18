#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT       += core gui svg

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console #for better display of processes with cout.flush()
QMAKE_CXXFLAGS += -std=c++1y #for C++14 standard
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-result
LIBS += -fopenmp

#matlab cwt function
LIBS += -L/media/Files/Matlab/Projects/libcwt_r/for_testing -lcwt_r
INCLUDEPATH += /media/Files/Matlab/Projects/libcwt_r/for_testing
DEPENDPATH += /media/Files/Matlab/Projects/libcwt_r/for_testing

INCLUDEPATH += /media/Files/Matlab/MCR/v83/extern/include
LIBS += -L/media/Files/Matlab/MCR/v83/bin/glnxa64 -leng -lm -lmwcpp11compat
LIBS += -L/media/Files/Matlab/MCR/v83/runtime/glnxa64 -lmwmclmcrrt


SOURCES += main.cpp\
    other/cut.cpp \
    other/spectre.cpp \
    other/edffile.cpp \
    other/edfFile_repair.cpp \
    other/matrix.cpp \
    other/coord.cpp \
    mainwindow/mainwindow.cpp \
    mainwindow/mainwindow_slices.cpp \
    mainwindow/mainwindow_ica.cpp \
    mainwindow/mainwindow_editedf.cpp \
    mainwindow/mainwindow_autos.cpp \
    mainwindow/mainwindow_customF.cpp \
    myLib/myLib_clustering.cpp \
    myLib/myLib_general.cpp \
    myLib/myLib_matlab.cpp \
    myLib/myLib_signalProcessing.cpp \
    myLib/myLib_draws.cpp \
    myLib/myLib_deprecate.cpp \
    myLib/myLib_dataHandler.cpp \
    myLib/myLib_smallFuncs.cpp \
    net/net.cpp \
    net/net_classification.cpp \
    net/net_setsgets.cpp \
    net/net_successive.cpp \
    classifier/classifier.cpp \
    classifier/classifier_ann.cpp \
    classifier/classifier_svm.cpp \
    classifier/classifier_dist.cpp \
    classifier/classifier_nbc.cpp \
    classifier/classifier_knn.cpp \
    classifier/classifier_word.cpp \
    classifier/classifier_rda.cpp \
    other/autos.cpp \
    classifier/classifier-data.cpp \
    net/net_data.cpp

HEADERS  += mainwindow.h \
    cut.h \
    net.h \
    spectre.h \
    tempthread.h \
    library.h \
    edffile.h \
    matrix.h \
    coord.h \
    library.h \
    smallLib.h \
    classifier.h \
    autos.h \
    classifierdata.h

FORMS    += mainwindow.ui \
    cut.ui \
    net.ui \
    spectre.ui

OTHER_FILES +=

RESOURCES +=
