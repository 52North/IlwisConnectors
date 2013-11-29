#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T19:05:01
#
#-------------------------------------------------

CONFIG += plugin
TARGET = wfsconnector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui

TEMPLATE = lib

DEFINES += WFSCONNECTOR_LIBRARY

SOURCES += \
    wfsconnector/wfsobjectfactory.cpp \
    wfsconnector/wfsconnector.cpp \
    wfsconnector/wfscatalogconnector.cpp

HEADERS += \
    wfsconnector/wfsobjectfactory.h \
    wfsconnector/wfsConnector_global.h \
    wfsconnector/wfsconnector.h \
    wfsconnector/wfscatalogconnector.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/core
DEPENDPATH += $$PWD/core

OTHER_FILES += \
    wfsconnector/wfsconnector.json


