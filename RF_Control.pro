#-------------------------------------------------
#
# Project created by QtCreator 2015-06-14T12:24:29
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RF_Control
TEMPLATE = app

SOURCES += main.cpp\
        rf_dialog.cpp

HEADERS  += rf_dialog.h

FORMS    += rf_dialog.ui
