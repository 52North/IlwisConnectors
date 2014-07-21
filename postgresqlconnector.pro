##-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = postgresqlconnector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui

TEMPLATE = lib

DEFINES += POSTGRESSQLCONNECTOR_LIBRARY

OTHER_FILES += \
    postgresqlconnector/postgresqlconnector.json \ 
    postgresqlconnector/scratch_pad.txt

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/postgresql \

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \

HEADERS += \
    postgresqlconnector/postgresqlconnector.h \
    postgresqlconnector/postgresqlmodule.h \
    postgresqlconnector/postgresqlobjectfactory.h \
    postgresqlconnector/postgresqlfeatureconnector.h \
    postgresqlconnector/postgresqltableconnector.h \
    postgresqlconnector/postgresqlcatalogexplorer.h \
    postgresqlconnector/postgresqldatabaseutil.h

SOURCES += \
    postgresqlconnector/postgresqlconnector.cpp \
    postgresqlconnector/postgresqlmodule.cpp \
    postgresqlconnector/postgresqlobjectfactory.cpp \
    postgresqlconnector/postgresqlfeatureconnector.cpp \
    postgresqlconnector/postgresqltableconnector.cpp \
    postgresqlconnector/postgresqlcatalogexplorer.cpp

