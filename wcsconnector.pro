#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

QT = network xmlpatterns

CONFIG += plugin
TARGET = wcsconnector

include(global.pri)

TEMPLATE = lib

DEFINES += WCSCONNECTOR_LIBRARY

SOURCES += \
    wcsconnector/wcsmodule.cpp \
    wcsconnector/wcsobjectfactory.cpp \
    wcsconnector/wcscatalogexplorer.cpp \
    wcsconnector/wcs.cpp \
    wcsconnector/wcsresponse.cpp \
    wcsconnector/wcsitem.cpp \
    wcsconnector/wcscapabilitiesparser.cpp

HEADERS += \ 
    wcsconnector/wcsmodule.h \
    wcsconnector/wcsobjectfactory.h \
    wcsconnector/wcsutils.h \
    wcsconnector/wcscatalogexplorer.h \
    wcsconnector/wcs.h \
    wcsconnector/wcsresponse.h \
    wcsconnector/wcsitem.h \
    wcsconnector/wcscapabilitiesparser.h

OTHER_FILES += \
    wcsconnector/wcsconnector.json

INCLUDEPATH +=  $$PWD/../external/gdalheaders

resources.files += gdalconnector/resources/libraries.config
resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/gdalconnector/resources

INSTALLS += resources

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/gdalconnector
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/gdalconnector
