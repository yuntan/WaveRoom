#-------------------------------------------------
#
# Project created by QtCreator 2014-09-21T15:12:25
#
#-------------------------------------------------

QT += core gui opengl widgets concurrent script
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11
LIBS += -lGLU

TARGET = WaveRoom
TEMPLATE = app


SOURCES += \
    main.cpp\
    mainwindow.cpp \
    glwidget.cpp \
    facedata.cpp \
    wavecalculator.cpp

HEADERS += \
    mainwindow.h \
    glwidget.h \
    facedata.h \
    wavecalculator.h \
    consts.h

FORMS += mainwindow.ui

OTHER_FILES +=
