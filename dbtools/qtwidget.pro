#-------------------------------------------------
#
# Project created by QtCreator 2015-08-27T16:55:02
#
#-------------------------------------------------

QT       += core gui
QT       += webkit
QT       += network
QT       += sql

INCLUDEPATH += "../json/include"
LIBS        += "../json/lib/json_vc71_libmt.lib"
INCLUDEPATH += "../include"
LIBS += "../lib/opt/libmysql.lib"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtwidget
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    qtwebkit.cpp \
    mainfunc.cpp

HEADERS  += widget.h \
    qtwebkit.h \
    mainfunc.h

FORMS    += widget.ui
