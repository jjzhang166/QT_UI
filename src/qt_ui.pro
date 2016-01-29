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
INCLUDEPATH += "../lib3party/pbc"
LIBS        += "../lib3party/json/lib/json_vc71_libmt.lib"
INCLUDEPATH += "../lib3party/mysql/include"
LIBS        += "../lib3party/mysql/lib/opt/libmysql.lib"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_ui
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    qt_ui.cpp \
    ../lib3party/pbc/alloc.c \
    ../lib3party/pbc/bootstrap.c \
    ../lib3party/pbc/decode.c \
    ../lib3party/pbc/map.c \
    ../lib3party/pbc/pattern.c \
    ../lib3party/pbc/proto.c \
    ../lib3party/pbc/context.c \
    ../lib3party/pbc/register.c \
    ../lib3party/pbc/rmessage.c \
    ../lib3party/pbc/stringpool.c \
    ../lib3party/pbc/wmessage.c \
    ../lib3party/pbc/dump.c \
    ../lib3party/pbc/varint.c \
    ../lib3party/pbc/array.c

HEADERS  += widget.h \
    qt_ui.h \
    ByteBuffer.h \
    ../lib3party/pbc/pbc.h \
    ../lib3party/pbc/proto.h
