#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T19:05:01
#
#-------------------------------------------------

CONFIG += plugin
TARGET = ilwis3connector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui
QT       += sql

TEMPLATE = lib

DEFINES += ILWIS3CONNECTOR_LIBRARY

SOURCES += \
    ilwis3connector/ilwis3connector.cpp \
    ilwis3connector/ilwis3module.cpp \
    ilwis3connector/ilwis3objectfactory.cpp \
    ilwis3connector/ellipsoidconnector.cpp \
    ilwis3connector/coordinatesystemconnector.cpp \
    ilwis3connector/domainconnector.cpp \
    ilwis3connector/georefconnector.cpp \
    ilwis3connector/coverageconnector.cpp \
    ilwis3connector/inifile.cpp \
    ilwis3connector/tableconnector.cpp \
    ilwis3connector/binaryilwis3table.cpp \
    ilwis3connector/ilwis3range.cpp \
    ilwis3connector/odfitem.cpp \
    ilwis3connector/ilwis3catalogconnector.cpp \
    ilwis3connector/ilwis3projectionconnector.cpp \
    ilwis3connector/RawConverter.cpp \
    ilwis3connector/ilwisrastercoverageconnector.cpp \
    ilwis3connector/ilwis3featureconnector.cpp

HEADERS += \
    ilwis3connector/ilwis3connector_global.h \
    ilwis3connector/ilwis3connector.h \
    ilwis3connector/ilwis3module.h \
    ilwis3connector/ilwis3objectfactory.h \
    ilwis3connector/ellipsoidconnector.h \
    ilwis3connector/coordinatesystemconnector.h \
    ilwis3connector/domainconnector.h \
    ilwis3connector/georefconnector.h \
    ilwis3connector/coverageconnector.h \
    ilwis3connector/rawconverter.h \
    ilwis3connector/inifile.h \
    ilwis3connector/tableconnector.h \
    ilwis3connector/binaryilwis3table.h \
    ilwis3connector/ilwis3range.h \
    ilwis3connector/odfitem.h \
    ilwis3connector/ilwis3catalogconnector.h \
    ilwis3connector/ilwis3projectionconnector.h \
    ilwis3connector/ilwisrastercoverageconnector.h \
    ilwis3connector/ilwis3featureconnector.h

OTHER_FILES += \
    ilwis3connector/ilwis3connector.json \
    ilwis3connector/resources/projection_allias.csv \
    ilwis3connector/resources/ellipsoid_allias.csv \
    ilwis3connector/resources/datum_allias.csv \
    ilwis3connector/resources/domain_allias.csv

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos
DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos

resources.files = ilwis3connector/ilwis3connector.json \
    ilwis3connector/resources/projection_allias.csv \
    ilwis3connector/resources/ellipsoid_allias.csv \
    ilwis3connector/resources/datum_allias.csv \
    ilwis3connector/resources/domain_allias.csv
    ilwis3connector/resources/ilwis3formats.config

resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

INSTALLS += resources
