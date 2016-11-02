#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T21:50:54
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/lib\''

TARGET = ttm25
TEMPLATE = app

INCLUDEPATH += ../mylib/Widgets\

SOURCES += main.cpp\
        main_window.cpp \
    communication/fins/endpoint.cpp \
    communication/fins/paskage.cpp \
    communication/fins/command.cpp \
    unit_tests/test_fins.cpp\
    cpu/db33.cpp\
    test/test.cpp\
    test/test_params.cpp \
    test/worker.cpp\
    test/viewer.cpp\
    settings/settings.cpp\
    settings/settings_wnd.cpp\
    users/users.cpp\
    users/login.cpp \
    test/impl/inside_pressure.cpp\
    ../mylib/Widgets/GraphBuilder/graph_builder.cpp \
    ../mylib/Widgets/RoundDial/round_dial.cpp \
    ../mylib/COMPort/ComPort.cpp \
    test/impl/common_data.cpp

HEADERS  += main_window.h \
    communication/fins/endpoint.h \
    communication/fins/paskage.h \
    communication/fins/command.h \
    cpu/data_block_base.h \
    cpu/db33.h\
    test/test.h\
    test/test_params.h \
    test/worker.h\
    test/viewer.h\
    settings/settings.h\
    settings/settings_wnd.h\
    users/users.h\
    users/login.h \
    test/impl/inside_pressure.h\
    ../mylib/Widgets/GraphBuilder/graph_builder.h\
    ../mylib/Widgets/RoundDial/round_dial.h \
    ../mylib/COMPort/ComPort.h \
    test/impl/common_data.h

FORMS    += main_window.ui\
            test/viewer.ui\
            settings/settings_wnd.ui\
            users/users.ui\
            users/login.ui

win32 {
    RC_ICONS += img/Main.ico
    DEFINES += WINDOWS
    LIBS += -lole32 -loleaut32
}

unix {
    DEFINES -= WINDOWS
    DEFINES += DEMO
}

debug {
    DEFINES += DEBUG
}

release {
    DEFINES -= DEBUG
}
#DEFINES += WINDOWS
DEFINES += DEMO
DEFINES += DEBUG
