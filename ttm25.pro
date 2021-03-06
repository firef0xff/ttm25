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

INCLUDEPATH += Widgets\
               communication\

SOURCES += main.cpp\
        main_window.cpp \
    communication/fins/endpoint.cpp \
    communication/fins/paskage.cpp \
    communication/fins/command.cpp \
    unit_tests/test_fins.cpp\
    test/test.cpp\
    test/test_params.cpp \
    test/worker.cpp\
    test/viewer.cpp\
    settings/settings.cpp\
    settings/settings_wnd.cpp\
    users/users.cpp\
    users/login.cpp \
    Widgets/GraphBuilder/graph_builder.cpp \
    Widgets/RoundDial/round_dial.cpp \
    COMPort/ComPort.cpp \
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
    cpu/net_connection.cpp \
    cpu/sensors.cpp \
    test/impl/attestation_params.cpp \
    cpu/params.cpp \
    cpu/indication.cpp \
    cpu/launch_control.cpp \
    main_testing.cpp \
    main_attestation.cpp \
    test/impl/attestation_tests.cpp \
    test/impl/work_params.cpp \
    test/impl/main_tests.cpp \
    cpu/attestation_launch_control.cpp \
    test/impl/defect_question.cpp \
    cpu/err_codes.cpp \
    communication/metran/metran_command.cpp \
    communication/metran/metran_paskage.cpp \
    communication/metran/funcs/metran_read.cpp \
    communication/metran/funcs/metran_write.cpp \
    unit_tests/test_metran.cpp \
    cpu/metran/metran.cpp

HEADERS  += main_window.h \
    communication/fins/endpoint.h \
    communication/fins/paskage.h \
    communication/fins/command.h \
    cpu/data_block_base.h \
    test/test.h\
    test/test_params.h \
    test/worker.h\
    test/viewer.h\
    settings/settings.h\
    settings/settings_wnd.h\
    users/users.h\
    users/login.h \
    Widgets/GraphBuilder/graph_builder.h\
    Widgets/RoundDial/round_dial.h \
    COMPort/ComPort.h \
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
    cpu/net_connection.h \
    cpu/sensors.h \
    test/impl/work_params.h \
    test/impl/attestation_params.h \
    cpu/params.h \
    cpu/indication.h \
    cpu/launch_control.h \
    test/impl/main_tests.h \
    test/impl/attestation_tests.h \
    cpu/attestation_launch_control.h \
    test/impl/defect_question.h \
    cpu/err_codes.h \
    communication/metran/metran_command.h \
    communication/buffer_tools.h \
    communication/metran/metran_paskage.h \
    communication/metran/funcs/metran_read.h \
    communication/metran/funcs/metran_write.h \
    cpu/metran/metran.h

FORMS    += main_window.ui\
            test/viewer.ui\
            settings/settings_wnd.ui\
            users/users.ui\
            users/login.ui \
    settings/textitem.ui \
    test/impl/defect_question.ui

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
#DEFINES += LOG
#DEFINES += DEBUG
#DEFINES += TESTS

DISTFILES += \    
    data/* \
    img/*
