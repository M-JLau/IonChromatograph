#-------------------------------------------------
#
# Project created by QtCreator 2018-12-02T16:54:34
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = My_Server
TEMPLATE = app


SOURCES += main.cpp\
        controlpanel.cpp \
    qcustomplot.cpp \
    mythread.cpp

HEADERS  += controlpanel.h \
    qcustomplot.h \
    mythread.h

FORMS    += \
    controlpanel.ui
