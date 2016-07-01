#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT       += core gui svg

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console #for better display of processes with cout.flush()
QMAKE_CXXFLAGS += -std=c++11 #for C++11 standard
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
    net_successive.cpp \
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
    net.cpp

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
    smallLib.h

FORMS    += mainwindow.ui \
    cut.ui \
    cfg.ui \
    net.ui \
    spectre.ui \
    makepa.ui \
    eyes.ui

OTHER_FILES += \
    kernels.cl

RESOURCES +=
