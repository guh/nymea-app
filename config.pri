CONFIG += c++11
#DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -Wall -Werror

top_srcdir=$$PWD
top_builddir=$$shadowed($$PWD)

VERSION_INFO=$$cat(version.txt)
APP_VERSION=$$member(VERSION_INFO, 0)
APP_REVISION=$$member(VERSION_INFO, 1)

DEFINES+=APP_VERSION=\\\"$${APP_VERSION}\\\"

android: {
    QMAKE_POST_LINK += cp $$top_srcdir/version.txt $$top_builddir/

    !equals(OVERLAY_PATH, ""):!equals(BRANDING, "") {
        ANDROID_PACKAGE_SOURCE_DIR = $${OVERLAY_PATH}/packaging/android_$$BRANDING
    } else {
        ANDROID_PACKAGE_SOURCE_DIR = $${top_srcdir}/packaging/android
    }

    !no-firebase:DEFINES+=WITH_FIREBASE
}

