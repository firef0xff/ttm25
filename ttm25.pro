#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T21:50:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ttm25
TEMPLATE = app


SOURCES += main.cpp\
        main_window.cpp \
    communication/fins/endpoint.cpp \
    communication/fins/paskage.cpp \
    communication/fins/command.cpp \
    tests/test_fins.cpp

HEADERS  += main_window.h \
    communication/fins/endpoint.h \
    communication/fins/paskage.h \
    communication/fins/command.h

FORMS    += main_window.ui
