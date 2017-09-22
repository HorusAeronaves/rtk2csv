TARGET = RTK2CSV

CONFIG += \
    c++14 \
    static

QT += core gui widgets charts

RC_ICONS = icon/icon.ico

FORMS += \
    src/window.ui

HEADERS += \
    src/window.h \
    src/qstring3d.h

SOURCES += \
    src/window.cpp \
    src/qstring3d.cpp

test{
    message(Configuring test build...)

    QT += testlib

    HEADERS += \
        src/test.h

    SOURCES += \
        src/test.cpp
}
else{
    SOURCES += \
        src/main.cpp
}
