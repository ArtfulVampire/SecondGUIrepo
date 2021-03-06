#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT   += core gui widgets

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console #for better display of processes with std::cout.flush()

QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-result
QMAKE_CXXFLAGS += -Wno-unused-function
LIBS += -fopenmp
#QMAKE_CXXFLAGS_RELEASE += -static -static-libgcc

CONFIG += c++14
#for c++1z
#CONFIG += c++1z
QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=1



#cwt
#INCLUDEPATH += ./matlab/for_testing
#DEPENDPATH += ./matlab/for_testing
#INCLUDEPATH += ./matlab/mcr_extern_include
#LIBS += -L../SecondGUI/matlab/for_testing -lcwt_r

#linux-only
#LIBS += -L/media/Files/Matlab/MCR/v83/bin/glnxa64 -lm -lmwcpp11compat
#LIBS += -L/media/Files/Matlab/MCR/v83/runtime/glnxa64 -lmwmclmcrrt
#INCLUDEPATH += /media/Files/Matlab/MCR/v83/extern/include
#LIBS += -L/media/Files/Matlab/Projects/libcwt_r/for_testing -lcwt_r
#INCLUDEPATH += /media/Files/Matlab/Projects/libcwt_r/for_testing
#DEPENDPATH += /media/Files/Matlab/Projects/libcwt_r/for_testing

SOURCES += \
main.cpp\
widgets/mainwindow/mainwindow.cpp \
widgets/mainwindow/mainwindow_slices.cpp \
widgets/mainwindow/mainwindow_ica.cpp \
widgets/mainwindow/mainwindow_editedf.cpp \
widgets/mainwindow/mainwindow_autos.cpp \
widgets/mainwindow/mainwindow_customF.cpp \
widgets/mainwindow/mainwindow_mati.cpp \
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
myLib/myLib_temp.cpp \
myLib/myLib_qtlib.cpp \
myLib/myLib_vectr.cpp \
myLib/myLib_highlevel.cpp \
myLib/myLib_ica.cpp \
myLib/myLib_adhoc.cpp \
classifier/classifier-data.cpp \
classifier/classifier_lib.cpp \
classifier/classifier.cpp \
classifier/classifier_ann.cpp \
classifier/classifier_svm.cpp \
classifier/classifier_dist.cpp \
classifier/classifier_nbc.cpp \
classifier/classifier_knn.cpp \
classifier/classifier_rda.cpp \
classifier/classifier_ward.cpp \
other/edfFile_repair.cpp \
other/matrix.cpp \
other/autos_iitp.cpp \
other/autos_galya.cpp \
other/autos.cpp \
other/autos_elena.cpp \
other/autos_settings.cpp \
other/coords.cpp \
other/edfFile.cpp \
other/defs.cpp \
other/feedback_proc.cpp \
other/feedback_fbClass.cpp \
other/feedback_fbedf.cpp \
widgets/net/net.cpp \
widgets/net/net_classification.cpp \
widgets/net/net_setsgets.cpp \
widgets/net/net_successive.cpp \
widgets/net/net_data.cpp \
widgets/spectre.cpp \
widgets/cut/cut.cpp \
widgets/cut/cut_smartFind.cpp \
widgets/cut/cut_modify.cpp \
widgets/cut/cut_navi.cpp \
widgets/cut/cut_iitp.cpp \
widgets/cut/cut_files.cpp \
widgets/choosechans.cpp

HEADERS += \
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
myLib/highlevel.h \
myLib/iitp_consts.h \
myLib/iitp_variables.h \
myLib/qtlib.h \
myLib/temp.h \
myLib/vectr.h \
widgets/cut.h \
widgets/mainwindow.h \
widgets/net.h \
widgets/spectre.h \
classifier/classifier.h \
classifier/classifierdata.h \
other/autos.h \
other/matrix.h \
other/edffile.h \
other/subjects.h \
other/feedback.h \
other/defs.h \
other/consts.h \
myLib/ica.h \
other/feedback_autos.h \
myLib/adhoc.h \
widgets/choosechans.h \
other/coords.h

FORMS +=\
mainwindow.ui \
cut.ui \
net.ui \
spectre.ui \
choosechans.ui

OTHER_FILES +=

RESOURCES +=
