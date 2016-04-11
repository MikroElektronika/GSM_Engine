TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    ../src/at_parser.c \
    ../src/at_timer.c \
    ../src/gsm_adapter.c \
    ../src/gsm_engine.c \
    ../src/gsm_hal.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    ../include/at_config.h \
    ../include/at_parser.h \
    ../include/at_timer.h \
    ../include/gsm_adapter.h \
    ../include/gsm_engine.h \
    ../include/gsm_hal.h

INCLUDEPATH += \
    ../include
