#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T19:05:01
#
#-------------------------------------------------

CONFIG += plugin
TARGET = httpserver

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui
QT       += sql
QT       += network

TEMPLATE = lib

DEFINES += HTTPSERVER_LIBRARY

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/core
DEPENDPATH += $$PWD/core

OTHER_FILES += \
    httpserver/httpserver.json

SOURCES += \
    httpserver/httpservermodule.cpp \
    httpserver/httpserver.cpp \
    ../external/mongoose/mongoose.c

HEADERS += \
    httpserver/httpservermodule.h \
    httpserver/httpserver.h \
    ../external/mongoose/mongoose.h

win32: LIBS += -L$$PWD/../libraries/win32debug/ -lws32_32

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
