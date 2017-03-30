#-------------------------------------------------
#
# Project created by QtCreator 2017-03-30T23:43:20
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app

INCLUDEPATH += E:\ttm25\communication\

SOURCES += test_app\main.cpp\
        test_app\mainwindow.cpp\
        ..\mylib\COMPort\COMPort.cpp\
        communication\metran\metran_command.cpp\
        communication\metran\metran_paskage.cpp\
        communication\metran\funcs\metran_read.cpp\
        communication\metran\funcs\metran_write.cpp\
        cpu\metran\metran.cpp\
        settings\settings.cpp

HEADERS  += test_app\mainwindow.h\
        ..\mylib\COMPort\*.h\
        communication\*.h\
        communication\metran\*.h\
        communication\metran\funcs\*.h\
        cpu\metran\*.h\
        settings\settings.h

FORMS    += test_app\mainwindow.ui

win32 {
    RC_ICONS += img/Main.ico
    DEFINES += WINDOWS
#    LIBS += -lole32 -loleaut32
}
