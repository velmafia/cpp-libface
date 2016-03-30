#-------------------------------------------------
#
# Project created by QtCreator 2016-03-30T13:12:48
#
#-------------------------------------------------

QT       -= core gui

TARGET = mman
TEMPLATE = lib
CONFIG += staticlib

DEFINES += MMAN_LIBRARY

SOURCES += \
    ../mman-win32/mman.c

HEADERS += \
    ../mman-win32/mman.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
