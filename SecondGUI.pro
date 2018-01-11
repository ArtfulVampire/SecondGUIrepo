#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT   += core gui svg

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console #for better display of processes with std::cout.flush()

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-result
LIBS += -fopenmp

CONFIG += c++14
#CONFIG += c++1z
#for c++1z
INCLUDEPATH += /usr/local/include/c++/7.2.0
INCLUDEPATH += /usr/local/lib/gcc/x86_64-pc-linux-gnu/7.2.0/include
QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0


#QMAKE_CXXFLAGS_RELEASE += -static -static-libgcc

#cwt
INCLUDEPATH += ./matlab/for_testing
DEPENDPATH += ./matlab/for_testing
INCLUDEPATH += ./matlab/mcr_extern_include
#LIBS += -L../SecondGUI/matlab/for_testing -lcwt_r
#linux-only
#LIBS += -L/media/Files/Matlab/MCR/v83/bin/glnxa64 -leng -lmwcpp11compat
#LIBS += -L/media/Files/Matlab/MCR/v83/runtime/glnxa64 -lmwmclmcrrt

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
#DSP/DspFilters/Bessel.cpp \
#DSP/DspFilters/Biquad.cpp \
#DSP/DspFilters/Butterworth.cpp \
#DSP/DspFilters/Cascade.cpp \
#DSP/DspFilters/ChebyshevI.cpp \
#DSP/DspFilters/ChebyshevII.cpp \
#DSP/DspFilters/Custom.cpp \
#DSP/DspFilters/Design.cpp \
#DSP/DspFilters/Documentation.cpp \
#DSP/DspFilters/Elliptic.cpp \
#DSP/DspFilters/Filter.cpp \
#DSP/DspFilters/Legendre.cpp \
#DSP/DspFilters/Param.cpp \
#DSP/DspFilters/PoleFilter.cpp \
#DSP/DspFilters/RBJ.cpp \
#DSP/DspFilters/RootFinder.cpp \
#DSP/DspFilters/State.cpp \
myLib/myLib_qtlib.cpp \
    classifier/classifier_ward.cpp \
    myLib/myLib_temp.cpp \
    other/autos_iitp.cpp \
    widgets/cut/cut.cpp \
    widgets/cut/cut_smartFind.cpp \
    widgets/cut/cut_modify.cpp \
    widgets/cut/cut_navi.cpp \
    widgets/cut/cut_iitp.cpp \
    widgets/cut/cut_files.cpp \
    widgets/mainwindow/mainwindow_mati.cpp \
    other/autos_fb.cpp \
    other/autos_galya.cpp \
    other/autos.cpp \
    myLib/myLib_vectr.cpp

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
#DSP/DspFilters/Dsp.h \
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
    myLib/temp.h \
    myLib/vectr.h \
    other/subjects.h

FORMS+= mainwindow.ui \
cut.ui \
net.ui \
spectre.ui

OTHER_FILES +=

RESOURCES +=
