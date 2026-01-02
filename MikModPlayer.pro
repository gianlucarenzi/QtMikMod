QT       += core gui widgets

TARGET = MikModPlayer
TEMPLATE = app
CONFIG += c++11

# Get Git commit ID for versioning
GIT_COMMIT_ID = $$system(git rev-parse --short HEAD)
DEFINES += GIT_COMMIT_ID=\\\"$$GIT_COMMIT_ID\\\"

# --- CONFIGURAZIONE WINDOWS (MSVC / MinGW) ---
win32 {
    # Su Windows usiamo QLibrary, quindi serve solo l'header per compilare
    INCLUDEPATH += $$PWD/3rdparty/mikmod/include
    
    # Copia automatica della DLL nella cartella di esecuzione (Post-Build)
    DLL_PATH = $$replace($$PWD/3rdparty/mikmod/bin/libmikmod.dll, /, \\)
    QMAKE_POST_LINK += xcopy /d /y "$$DLL_PATH" "$$replace(OUT_PWD, /, \\)"
}

# --- CONFIGURAZIONE UNIX (Linux e macOS) ---
unix {
    LIBS += -lmikmod
    # Percorsi per Homebrew su macOS (Intel e Apple Silicon)
    macx {
        INCLUDEPATH += /usr/local/include /opt/homebrew/include
        LIBS += -L/usr/local/lib -L/opt/homebrew/lib
    }
}

macx {
    ICON = bitmaps/icon.png
    QMAKE_APPLE_ARCHS = x86_64 arm64
    QMAKE_BUNDLE_DATA.fonts.files = fonts/TopazPlus_a500_v1.0.ttf
    QMAKE_BUNDLE_DATA.fonts.path = Contents/Resources/fonts
    BUNDLE_DATA += QMAKE_BUNDLE_DATA.fonts
}

HEADERS += mainwindow.h mikmodplayer.h vumeterwidget.h
SOURCES += main.cpp mainwindow.cpp mikmodplayer.cpp vumeterwidget.cpp
RESOURCES += resources.qrc
