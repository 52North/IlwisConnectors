CONFIG += plugin
TARGET = spreadsheetconnectors

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

TEMPLATE = lib

DEFINES += POSTGRESSQLCONNECTOR_LIBRARY

OTHER_FILES += \
    spreadsheetconnectors/spreadsheetconnectors.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/spreadsheetconnectors -lfreexl \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/spreadsheetconnectors -lQOds \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/spreadsheetconnectors -lQt0Xlsx
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/spreadsheet \
                $$PWD/../external/xlsx

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/ods \
                $$PWD/../external/xlsx


HEADERS += \
    spreadsheetconnectors/spreadsheetconnectorsmodule.h \
    spreadsheetconnectors/spreadsheetconnectorsobjectfactory.h \
    spreadsheetconnectors/spreadsheetcatalogfileexplorer.h \
    spreadsheetconnectors/spreadsheet.h \
    spreadsheetconnectors/odsformat.h \
    spreadsheetconnectors/spreadsheettableconnector.h \
    spreadsheetconnectors/xlsformat.h \
    spreadsheetconnectors/spreadsheetcatalogexplorer.h \
    spreadsheetconnectors/xlsxformat.h

SOURCES += \
    spreadsheetconnectors/spreadsheetconnectorsmodule.cpp \
    spreadsheetconnectors/spreadsheetconnectorsobjectfactory.cpp \
    spreadsheetconnectors/spreadsheetcatalogfileexplorer.cpp \
    spreadsheetconnectors/odsformat.cpp \
    spreadsheetconnectors/spreadsheettableconnector.cpp \
    spreadsheetconnectors/xlsformat.cpp \
    spreadsheetconnectors/spreadsheetcatalogexplorer.cpp \
    spreadsheetconnectors/xlsxformat.cpp



