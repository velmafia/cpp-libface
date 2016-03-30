TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += LIBFACE_STATIC

LIBS += -L"$$PWD/../libs/"
LIBS += -lmman
LIBS += -lface

INCLUDEPATH += ../

SOURCES += \
    ../src/main.cpp
