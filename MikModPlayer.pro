QT       += core gui widgets
TEMPLATE = app
TARGET   = MikModPlayer
CONFIG  += c++11

# -------------------------------------------------
# Versioning: Git commit ID
# -------------------------------------------------
GIT_COMMIT_ID = $$system(git rev-parse --short HEAD)
DEFINES += GIT_COMMIT_ID=\\\"$$GIT_COMMIT_ID\\\"

# -------------------------------------------------
# WINDOWS (MSVC) — libmikmod via vcpkg
# -------------------------------------------------
win32 {
    VCPKG_ROOT = $$(VCPKG_ROOT)

    isEmpty(VCPKG_ROOT) {
        error("VCPKG_ROOT environment variable not set")
    }

    message("Using vcpkg from: $$VCPKG_ROOT")

    INCLUDEPATH += $$VCPKG_ROOT/installed/x64-windows/include
    LIBS += -L$$VCPKG_ROOT/installed/x64-windows/lib -lmikmod
}

# -------------------------------------------------
# UNIX (Linux + macOS)
# -------------------------------------------------
unix {
    LIBS += -lmikmod

    macx {
        # Homebrew (Intel + Apple Silicon)
        INCLUDEPATH += /usr/local/include /opt/homebrew/include
        LIBS += -L/usr/local/lib -L/opt/homebrew/lib
    }
}

# -------------------------------------------------
# macOS bundle configuration
# -------------------------------------------------
macx {
    ICON = bitmaps/icon.png
    QMAKE_APPLE_ARCHS = x86_64 arm64

    QMAKE_BUNDLE_DATA.fonts.files = fonts/TopazPlus_a500_v1.0.ttf
    QMAKE_BUNDLE_DATA.fonts.path  = Contents/Resources/fonts
    BUNDLE_DATA += QMAKE_BUNDLE_DATA.fonts
}

# -------------------------------------------------
# Sources & headers
# -------------------------------------------------
HEADERS += \
    mainwindow.h \
    mikmodplayer.h \
    vumeterwidget.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mikmodplayer.cpp \
    vumeterwidget.cpp

RESOURCES += resources.qrc
