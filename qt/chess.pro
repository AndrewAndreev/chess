
DEPENDPATH += .
include(chess.pri)

TEMPLATE = app
TARGET   = chess
QT      += core network widgets gui

win32:DEFINES += _WINDOWS WIN64
unix:DEFINES  += UNIX
DEFINES  += QT_WIDGETS_LIB

INCLUDEPATH += ../chess/code \
               ./GeneratedFiles \
               $(QTDIR)/mkspecs/win32-msvc2015

CONFIG(debug, debug|release) {
    message("debug")
    Configuration = debug
} else {
    message("release")
    Configuration = release
}

contains(QT_ARCH, i386) {
    message("32-bit")
    Platform = 32bit
} else {
    message("64-bit")
    Platform = 64bit
}

DESTDIR     = ./$${Platform}/$${Configuration}
OBJECTS_DIR = objs/$${Platform}/$${Configuration}
MOC_DIR    += ./GeneratedFiles/$${Configuration}

UI_DIR  += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

TRANSLATIONS += ../chess/translations/chess_en.ts \
                ../chess/translations/chess_ru.ts

win32:RC_FILE = ../msvc/chess.rc
