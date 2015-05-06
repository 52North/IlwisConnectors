#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = gslconnector

include(global.pri)

TEMPLATE = lib

DEFINES += gslCONNECTOR_LIBRARY

resources.files += gslconnector/resources/libraries.config
resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

INSTALLS += resources

OTHER_FILES += \ 
    gslconnector/gslconnector.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET -lgsl.dll

INCLUDEPATH += $$PWD/../external/gsl

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

HEADERS += \
    gslconnector/relativeaggregaterasterstatistics.h \
    gslconnector/rastercorrelation.h \
    gslconnector/gslmodule.h \
    gslconnector/aggregaterasterstatistics.h \
    gslconnector/rastercovariance.h \
    gslconnector/rasterquantile.h

SOURCES += \
    gslconnector/relativeaggregaterasterstatistics.cpp \
    gslconnector/rastercorrelation.cpp \
    gslconnector/gslmodule.cpp \
    gslconnector/aggregaterasterstatistics.cpp \
    gslconnector/rastercovariance.cpp \
    gslconnector/rasterquantile.cpp

