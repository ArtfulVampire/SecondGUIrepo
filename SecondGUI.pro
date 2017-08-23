#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT   += core gui svg

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console #for better display of processes with std::cout.flush()
QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-result
LIBS += -fopenmp

#QMAKE_CXXFLAGS_RELEASE += -static -static-libgcc

#DSP
INCLUDEPATH += ../SecondGUI/DSP
#LIBS += -L../SecondGUI/DSP -ldspFilters

#cwt
INCLUDEPATH += ./matlab/for_testing
DEPENDPATH += ./matlab/for_testing
INCLUDEPATH += ./matlab/mcr_extern_include
LIBS += -L../SecondGUI/matlab/for_testing -lcwt_r
#linux-only
LIBS += -L/media/Files/Matlab/MCR/v83/bin/glnxa64 -leng -lmwcpp11compat
LIBS += -L/media/Files/Matlab/MCR/v83/runtime/glnxa64 -lmwmclmcrrt

SOURCES += main.cpp\
widgets/mainwindow/mainwindow.cpp \
widgets/mainwindow/mainwindow_slices.cpp \
widgets/mainwindow/mainwindow_ica.cpp \
widgets/mainwindow/mainwindow_editedf.cpp \
widgets/mainwindow/mainwindow_autos.cpp \
widgets/mainwindow/mainwindow_customF.cpp \
myLib/myLib_clustering.cpp \
myLib/myLib_signalProcessing.cpp \
myLib/myLib_draws.cpp \
myLib/myLib_dataHandler.cpp \
myLib/myLib_drw.cpp \
myLib/myLib_wavelet.cpp \
myLib/myLib_statistics.cpp \
myLib/myLib_output.cpp \
myLib/myLib_mati.cpp \
myLib/myLib_iitp.cpp \
myLib/myLib_valar.cpp \
myLib/myLib_general.cpp \
classifier/classifier-data.cpp \
classifier/classifier_lib.cpp \
classifier/classifier.cpp \
classifier/classifier_ann.cpp \
classifier/classifier_svm.cpp \
classifier/classifier_dist.cpp \
classifier/classifier_nbc.cpp \
classifier/classifier_knn.cpp \
classifier/classifier_rda.cpp \
other/autos.cpp \
other/edffile.cpp \
other/edfFile_repair.cpp \
other/coord.cpp \
other/matrix.cpp \
widgets/net/net.cpp \
widgets/net/net_classification.cpp \
widgets/net/net_setsgets.cpp \
widgets/net/net_successive.cpp \
widgets/net/net_data.cpp \
widgets/spectre.cpp \
widgets/cut.cpp \
DSP/DspFilters/Bessel.cpp \
DSP/DspFilters/Biquad.cpp \
DSP/DspFilters/Butterworth.cpp \
DSP/DspFilters/Cascade.cpp \
DSP/DspFilters/ChebyshevI.cpp \
DSP/DspFilters/ChebyshevII.cpp \
DSP/DspFilters/Custom.cpp \
DSP/DspFilters/Design.cpp \
DSP/DspFilters/Documentation.cpp \
DSP/DspFilters/Elliptic.cpp \
DSP/DspFilters/Filter.cpp \
DSP/DspFilters/Legendre.cpp \
DSP/DspFilters/Param.cpp \
DSP/DspFilters/PoleFilter.cpp \
DSP/DspFilters/RBJ.cpp \
DSP/DspFilters/RootFinder.cpp \
DSP/DspFilters/State.cpp \
myLib/myLib_qtlib.cpp \
    classifier/classifier_ward.cpp \
    myLib/myLib_temp.cpp \
    other/autos_iitp.cpp

HEADERS  += \
widgets/cut.h \
myLib/drw.h \
myLib/signalProcessing.h \
myLib/wavelet.h \
myLib/dataHandlers.h \
myLib/draws.h \
myLib/statistics.h \
myLib/clustering.h \
myLib/output.h \
myLib/mati.h \
myLib/iitp.h \
myLib/valar.h \
myLib/small.h \
myLib/general.h \
DSP/DspFilters/Dsp.h \
widgets/mainwindow.h \
widgets/net.h \
widgets/spectre.h \
classifier/classifier.h \
classifier/classifierdata.h \
other/autos.h \
other/coord.h \
other/matrix.h \
other/edffile.h \
    other/defaults.h \
    myLib/qtlib.h \
    myLib/temp.h

FORMS+= mainwindow.ui \
cut.ui \
net.ui \
spectre.ui

OTHER_FILES +=

RESOURCES +=
