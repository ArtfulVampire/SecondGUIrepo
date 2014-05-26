#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT       += core gui svg

TARGET = SecondGUI
TEMPLATE = app

LIBS += -L/usr/lib/x86_64-linux-gnu -lOpenCL
LIBS += -L/usr/lib/x86_64-linux-gnu/mesa

SOURCES += main.cpp\
        mainwindow.cpp \
    cut.cpp \
    cfg.cpp \
    net.cpp \
    spectre.cpp \
    makepa.cpp \
    eyes.cpp \
    coord.cpp \
    qtempevent.cpp \
    tempthread.cpp \
    library.cpp

HEADERS  += mainwindow.h \
    cut.h \
    cfg.h \
    net.h \
    spectre.h \
    makepa.h \
    eyes.h \
    qtempevent.h \
    tempthread.h \
    library.h

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
