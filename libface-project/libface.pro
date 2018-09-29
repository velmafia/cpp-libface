#-------------------------------------------------
#
# Project created by QtCreator 2016-03-30T11:21:10
#
#-------------------------------------------------

QT       -= core gui

TARGET = face
TEMPLATE = lib
# CONFIG += staticlib
CONFIG += c++11

QMAKE_MAC_SDK = macosx10.11

DEFINES += BUILDING_LIBFACE

LIBS += -L"$$PWD/../libs"

win32 {
    # xpiks build
    CONFIG(debug, debug|release) {
        LIBS += -L"$$PWD/../../../libs/debug"
    } else {
        LIBS += -L"$$PWD/../../../libs/release"
    }

    LIBS += -lmman
}

INCLUDEPATH += ../
INCLUDEPATH += ../include/

SOURCES += \
    ../src/libfaceapi.cpp \
    ../src/benderrmq.cpp \
    ../src/phrase_map.cpp \
    ../src/segtree.cpp \
    ../src/sparsetable.cpp

HEADERS += \
    ../include/defines.hpp \
    ../include/libfaceapi.hpp \
    ../include/parser.hpp \
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
