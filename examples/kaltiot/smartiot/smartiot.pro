QT += quick cloudmessagingembeddedkaltiot

SOURCES += \
    main.cpp \
    smartiot.cpp

OTHER_FILES += \
    qml/main.qml

RESOURCES += \
    main.qrc

HEADERS += \
    smartiot.h

KALTIOT_SDK = $$(KALTIOT_SDK)
!isEmpty(KALTIOT_SDK): INCLUDEPATH += $${KALTIOT_SDK}/src

target.path = $$[QT_INSTALL_EXAMPLES]/kaltiot/smartiot
INSTALLS += target
