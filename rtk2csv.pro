TARGET = RTK2CSV

CONFIG += \
    c++14 \
    static

QT += core gui widgets charts

HEADERS += \
    src/window.h

SOURCES += \
    src/main.cpp \
    src/window.cpp

FORMS += \
    src/window.ui
