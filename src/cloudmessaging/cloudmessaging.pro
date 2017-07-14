TARGET = QtCloudMessaging

QT = core network

QMAKE_DOCS = $$PWD/doc/qtcloudmessaging.qdocconf

HEADERS += \
    $$PWD/qcloudmessaging.h \
    $$PWD/qcloudmessagingclient.h \
    $$PWD/qcloudmessagingprovider.h \
    $$PWD/qtcloudmessagingglobal.h \
    $$PWD/qcloudmessaging_p.h \
    $$PWD/qcloudmessagingclient_p.h \
    $$PWD/qcloudmessagingprovider_p.h \
    $$PWD/qcloudmessagingrestapi_p.h \
    $$PWD/qcloudmessagingrestapi.h

SOURCES += \
    $$PWD/qcloudmessaging.cpp \
    $$PWD/qcloudmessagingclient.cpp \
    $$PWD/qcloudmessagingprovider.cpp \
    $$PWD/qcloudmessagingrestapi.cpp

load(qt_module)
