TARGET = qt-qr-code
TEMPLATE = app
CONFIG += debug_and_release

QT += svg
QMAKE_CFLAGS += -std=c99

SOURCES = main.cpp ext/qrcodegen.c
DEFINES += USE_C_API

