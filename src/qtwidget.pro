#-------------------------------------------------
#
# Project created by QtCreator 2015-08-27T16:55:02
#
#-------------------------------------------------

QT       += core gui
QT       += webkit
QT       += network
QT       += sql

INCLUDEPATH += "../lib3party/json/include"
LIBS        += "../lib3party/json/lib/json_vc71_libmt.lib"
INCLUDEPATH += "../lib3party/mysql/include"
LIBS += "../lib3party/mysql/lib/opt/libmysql.lib"

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
