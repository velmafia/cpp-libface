#-------------------------------------------------
#
# Project created by QtCreator 2016-03-30T11:21:10
#
#-------------------------------------------------

QT       -= core gui

TARGET = libface
TEMPLATE = lib
CONFIG += staticlib

DEFINES += LIBFACE_STATIC

INCLUDEPATH += ../

SOURCES += \
    ../src/libfaceapi.cpp \
    ../include/benderrmq.cpp \
    ../include/phrase_map.cpp \
    ../include/segtree.cpp \
    ../include/sparsetable.cpp

HEADERS += \
    ../src/defines.hpp \
    ../src/libfaceapi.hpp \
    ../src/parser.hpp \
    ../include/benderrmq.hpp \
    ../include/editdistance.hpp \
    ../include/phrase_map.hpp \
    ../include/segtree.hpp \
    ../include/soundex.hpp \
    ../include/sparsetable.hpp \
    ../include/suggest.hpp \
    ../include/types.hpp \
    ../include/utils.hpp
unix {
    target.path = /usr/lib
    INSTALLS += target
}
