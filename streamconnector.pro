##-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = streamconnector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui

TEMPLATE = lib

DEFINES += STREAMCONNECTOR_LIBRARY

OTHER_FILES += \
    streamconnector/streamconnector.json 

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos 

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \

HEADERS += \
    streamconnector/streamobjectfactory.h \
    streamconnector/streammodule.h \
    streamconnector/streamconnector.h \
    streamconnector/streamconnectorv1.h \
    streamconnector/versioneddatastreamfactory.h \
    streamconnector/streamcoveragedatainterfacev1.h \
    streamconnector/streamcoordinatesystemdatainterfacev1.h \
    streamconnector/streamprojectiondatainterfacev1.h \
    streamconnector/streamellipsoiddatainterfacev1.h \
    streamconnector/streamfeaturedatainterfacev1.h \
    streamconnector/streamtabledatainterfacev1.h \
    streamconnector/streamdomaindatainterfacev1.h \
    streamconnector/streamrasterdatainterfacev1.h \
    streamconnector/rawconverter.h \
    streamconnector/streamgeorefdatainterfacev1.h

SOURCES += \
    streamconnector/streammodule.cpp \
    streamconnector/streamconnector.cpp \
    streamconnector/streamobjectfactory.cpp \
    streamconnector/streamconnectorv1.cpp \
    streamconnector/versioneddatastreamfactory.cpp \
    streamconnector/streamcoveragedatainterfacev1.cpp \
    streamconnector/streamcoordinatesystemdatainterfacev1.cpp \
    streamconnector/streamprojectiondatainterfacev1.cpp \
    streamconnector/streamellipsoiddatainterfacev1.cpp \
    streamconnector/streamfeaturedatainterfacev1.cpp \
    streamconnector/streamtabledatainterfacev1.cpp \
    streamconnector/streamdomaindatainterfacev1.cpp \
    streamconnector/streamrasterdatainterfacev1.cpp \
    streamconnector/rawconverter.cpp \
    streamconnector/streamgeorefdatainterfacev1.cpp



