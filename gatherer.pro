##-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = gatherer

include(global.pri)

QT = network xmlpatterns sql
QT       -= gui

TEMPLATE = lib

DEFINES += GATHERER_LIBRARY

OTHER_FILES += \
    gatherer/gatherer.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lhttpserver
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/extensions/httpserver \
                $$PWD/extensions/httpserver/httpserver

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos 

HEADERS += \
    gatherer/gatherermodule.h \
    gatherer/gathererrequesthandler.h

SOURCES += \
    gatherer/gatherermodule.cpp \
    gatherer/gathererrequesthandler.cpp

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET




