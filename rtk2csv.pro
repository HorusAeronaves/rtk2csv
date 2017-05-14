TARGET = RTK2CSV

CONFIG += \
    c++14 \
    static

QT += core gui widgets charts

RC_ICONS = icon/icon.ico

HEADERS += \
    src/window.h

SOURCES += \
    src/main.cpp \
    src/window.cpp

FORMS += \
    src/window.ui
