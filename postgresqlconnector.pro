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
    postgresqlconnector/postgresqlconnector.json 

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
    postgresqlconnector/postgresqlmodule.h \
    postgresqlconnector/postgresqlobjectfactory.h \
    postgresqlconnector/postgresqlconnector.h \
    postgresqlconnector/postgresqltableconnector.h \
    postgresqlconnector/postgresqlcatalogconnector.h

SOURCES += \
    postgresqlconnector/postgresqlmodule.cpp \
    postgresqlconnector/postgresqlobjectfactory.cpp \
    postgresqlconnector/postgresqlconnector.cpp \
    postgresqlconnector/postgresqltableconnector.cpp \
    postgresqlconnector/postgresqlcatalogconnector.cpp

