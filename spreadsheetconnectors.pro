CONFIG += plugin
TARGET = spreadsheetconnectors

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui

TEMPLATE = lib

DEFINES += POSTGRESSQLCONNECTOR_LIBRARY

OTHER_FILES += \
    spreadsheetconnectors/spreadsheetconnectors.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/spreadsheetconnectors -lfreexl
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/spreadsheet \

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \

HEADERS += \
    spreadsheetconnectors/spreadsheetconnectorsmodule.h \
    spreadsheetconnectors/spreadsheetconnectorsobjectfactory.h \
    spreadsheetconnectors/exceltableconnector.h \
    spreadsheetconnectors/excelcatalogexplorer.h \
    spreadsheetconnectors/spreadsheetcatalogfileexplorer.h

SOURCES += \
    spreadsheetconnectors/spreadsheetconnectorsmodule.cpp \
    spreadsheetconnectors/spreadsheetconnectorsobjectfactory.cpp \
    spreadsheetconnectors/exceltableconnector.cpp \
    spreadsheetconnectors/excelcatalogexplorer.cpp \
    spreadsheetconnectors/spreadsheetcatalogfileexplorer.cpp



