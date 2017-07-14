QT       += testlib cloudmessaging
QT       -= gui

TARGET = tst_qcloudmessaging
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        tst_qcloudmessaging.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
