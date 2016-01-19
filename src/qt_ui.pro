#-------------------------------------------------
#
# Project created by QtCreator 2016-01-19T16:02:54
#
#-------------------------------------------------

QT       += core gui
QT       += webkit
QT       += network
QT       += sql

INCLUDEPATH += "../lib3party/json/include"
LIBS        += "../lib3party/json/lib/json_vc71_libmt.lib"
INCLUDEPATH += "../lib3party/mysql/include"
LIBS        += "../lib3party/mysql/lib/opt/libmysql.lib"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_ui
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    qt_ui.cpp

HEADERS  += widget.h \
    qt_ui.h
