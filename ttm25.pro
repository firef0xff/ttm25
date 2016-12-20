#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T21:50:54
#
#-------------------------------------------------

QT       += core gui printsupport network
QMAKE_CXXFLAGS += -std=c++11

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
    test/impl/common_data.cpp \
    settings/strings_source.cpp \
    settings/textitem.cpp \
    communication/acr/current_values.cpp \
    communication/acr/acr_command.cpp \
    unit_tests/test_acr.cpp \
    communication/fins/funcs/memory_area_read.cpp \
    communication/fins/memoryaddr.cpp \
    communication/fins/funcs/memory_area_write.cpp \
    communication/fins/communicator.cpp \
    communication/fins/funcs/memory_area_fill.cpp \
    communication/fins/funcs/multiple_memory_area_read.cpp \
    communication/fins/funcs/memory_area_transfer.cpp \
    cpu/controls.cpp\
    cpu/cpu_memory.cpp \
    main_window_controls.cpp \
    cpu/net_connection.cpp

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
    test/impl/common_data.h \
    settings/strings_source.h \
    settings/textitem.h \
    communication/acr/command.h \
    communication/acr/current_values.h \
    communication/swap_endian.h \
    communication/fins/funcs/memory_area_read.h \
    communication/fins/memoryaddr.h \
    communication/fins/mem_type.h \
    communication/fins/funcs/memory_area_write.h \
    communication/fins/data_element.h \
    communication/fins/communicator.h \
    communication/fins/funcs/memory_area_fill.h \
    communication/fins/funcs/multiple_memory_area_read.h \
    communication/fins/funcs/memory_area_transfer.h \
    cpu/controls.h\
    cpu/cpu_memory.h \
    cpu/net_connection.h

FORMS    += main_window.ui\
            test/viewer.ui\
            settings/settings_wnd.ui\
            users/users.ui\
            users/login.ui \
    settings/textitem.ui

win32 {
    RC_ICONS += img/Main.ico
    DEFINES += WINDOWS
#    LIBS += -lole32 -loleaut32
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
#DEFINES += DEMO
DEFINES += LOG
DEFINES += DEBUG
#DEFINES += TESTS

DISTFILES += \
    data/kkt.json \
    data/method.json \
    data/tires_mark.json \
    data/marks.json \
    data/models.json \
    data/TODO
