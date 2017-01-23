#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT       += core gui svg

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console #for better display of processes with std::cout.flush()
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-result
LIBS += -fopenmp


#DSP
INCLUDEPATH += ./DSP
LIBS += -L../SecondGUI/DSP -ldspFilters

#cwt
INCLUDEPATH += ./matlab/for_testing
DEPENDPATH += ./matlab/for_testing
INCLUDEPATH += ./matlab/mcr_extern_include
LIBS += -L../SecondGUI/matlab/for_testing -lcwt_r

LIBS += -L/media/Files/Matlab/MCR/v83/bin/glnxa64 -leng -lmwcpp11compat
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
    myLib/myLib_signalProcessing.cpp \
    myLib/myLib_draws.cpp \
    myLib/myLib_dataHandler.cpp \
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
    net/net_data.cpp \
    myLib/myLib_drw.cpp \
    myLib/myLib_wavelet.cpp \
    myLib/myLib_statistics.cpp \
    myLib/myLib_output.cpp \
    myLib/myLib_mati.cpp \
    myLib/myLib_smallFuncs.cpp \
    myLib/myLib_iitp.cpp

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
    classifierdata.h \
    myLib/drw.h \
    myLib/signalProcessing.h \
    myLib/wavelet.h \
    myLib/dataHandlers.h \
    myLib/draws.h \
    myLib/statistics.h \
    myLib/clustering.h \
    myLib/output.h \
DSP/DspFilters/Dsp.h \
    myLib/mati.h \
    myLib/iitp.h \
    myLib/mySpectre.h

FORMS    += mainwindow.ui \
    cut.ui \
    net.ui \
    spectre.ui

OTHER_FILES +=

RESOURCES +=
