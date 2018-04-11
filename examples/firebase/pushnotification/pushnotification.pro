QT += quick cloudmessagingfirebase

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

SOURCES += \
    main.cpp \
    pushnotification.cpp

OTHER_FILES += \
    qml/main.qml \
    android-sources/src/org/qtproject/example/pushnotification/NotificationActivity.java \
    android-sources/AndroidManifest.xml \
    android-sources/build.gradle \
    android-sources/google-services.json \
    android-sources/res/drawable/icon.png

RESOURCES += \
    main.qrc

HEADERS += \
    pushnotification.h

GOOGLE_FIREBASE_SDK = $$(GOOGLE_FIREBASE_SDK)
!isEmpty(GOOGLE_FIREBASE_SDK): INCLUDEPATH += $${GOOGLE_FIREBASE_SDK}/include

target.path = $$[QT_INSTALL_EXAMPLES]/firebase/pushnotification
INSTALLS += target
