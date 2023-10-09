#-------------------------------------------------
#
# Project created by QtCreator 2018-12-02T12:52:57
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = My_Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    process_data.cpp \
    mythread.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    process_data.h \
    mythread.h \
    data.h

FORMS    += mainwindow.ui \
    process_data.ui
