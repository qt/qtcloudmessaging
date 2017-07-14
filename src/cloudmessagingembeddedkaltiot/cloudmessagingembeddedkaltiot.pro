TARGET = QtCloudMessagingEmbeddedKaltiot
QT = core cloudmessaging

# Check for KALTIOT_SDK environment
ENV_KALTIOT_SDK = $$(KALTIOT_SDK)

# Or define KALTIOT_SDK path here
KALTIOT_SDK =

isEmpty(ENV_KALTIOT_SDK) {
    isEmpty(KALTIOT_SDK) {
        message("KALTIOT_SDK" environment variable or define in radardemo.pro file not detected!)
    }
}

INCLUDEPATH += $$(KALTIOT_SDK)
INCLUDEPATH += $$(KALTIOT_SDK)/src

HEADERS += \
    qcloudmessagingembeddedkaltiotclient.h \
    qcloudmessagingembeddedkaltiotprovider.h \
    qcloudmessagingembeddedkaltiotclient_p.h \
    qcloudmessagingembeddedkaltiotprovider_p.h \
    qcloudmessagingembeddedkaltiotrest.h

SOURCES += \
    qcloudmessagingembeddedkaltiotclient.cpp \
    qcloudmessagingembeddedkaltiotprovider.cpp \
    qcloudmessagingembeddedkaltiotrest.cpp

android {
    DEFINES += ANDROID_OS
    QT += androidextras
    HEADERS += \
         $$PWD/android/ks_gw_client_android.h
} else {

    DEFINES += EMBEDDED_AND_DESKTOP_OS
    HEADERS += \
         $$(KALTIOT_SDK)/src/ks_gw_client.h

    LIBS += $$(KALTIOT_SDK)/libks_gw_client.a
}

load(qt_module)

DISTFILES += \
    README.md
