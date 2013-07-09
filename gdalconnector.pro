#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------
TARGET = gdalconnector
CONFIG += plugin

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui

DEFINES += GDALCONNECTOR_LIBRARY

SOURCES += \
    gdalconnector/gdalconnector.cpp \
    gdalconnector/gdalmodule.cpp \
    gdalconnector/gdalproxy.cpp \
    gdalconnector/gdalcatalogconnector.cpp \
    gdalconnector/gdalitem.cpp \
    gdalconnector/coverageconnector.cpp \
    gdalconnector/coordinatesystemconnector.cpp \
    gdalconnector/domainconnector.cpp \
    gdalconnector/gridcoverageconnector.cpp \
    gdalconnector/gdalobjectfactory.cpp \
    gdalconnector/georefconnector.cpp

HEADERS += gdalconnector/gdalconnector.h\
        gdalconnector/gdalconnector_global.h \
    gdalconnector/gdalmodule.h \
    gdalconnector/gdalproxy.h \
    gdalconnector/gdalcatalogconnector.h \
    gdalconnector/gdalitem.h \
    gdalconnector/coverageconnector.h \
    gdalconnector/coordinatesystemconnector.h \
    gdalconnector/domainconnector.h \
    gdalconnector/gridcoverageconnector.h \
    gdalconnector/gdalobjectfactory.h \
    gdalconnector/georefconnector.h
		
TEMPLATE = lib

INCLUDEPATH += $$PWD/../external/gdalheaders

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/core \
            $$PWD/../external/gdalheaders

DEPENDPATH += $$PWD/core

OTHER_FILES += \
    gdalconnector/gdalconnector.json


