CONFIG += plugin
TARGET = spreadsheetconnectors

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

TEMPLATE = lib

DEFINES += SPREADSHEETCONNECTOR_LIBRARY

resources.files += spreadsheetconnectors/resources/libraries.config
resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

INSTALLS += resources

OTHER_FILES += \
    spreadsheetconnectors/spreadsheetconnectors.json

win32{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/spreadsheetconnectors -lfreexl \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/spreadsheetconnectors -lQOds \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/spreadsheetconnectors -lQt0Xlsx
}


linux{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$GEOSLIB/ -lgeos-3.4.2 \
        -L$$LINUXLIB -lfreexl \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/QOds -lQOds \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/Qt0Xlsx -lQt0Xlsx
}


		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

win32{
INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/spreadsheet \
                $$PWD/../external/xlsx

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/ods \
                $$PWD/../external/xlsx
}

linux{
INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/QOds \
                $$PWD/../external/xlsx

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/QOds \
                $$PWD/../external/xlsx
}

HEADERS += \
    spreadsheetconnectors/spreadsheetconnectorsmodule.h \
    spreadsheetconnectors/spreadsheetconnectorsobjectfactory.h \
    spreadsheetconnectors/spreadsheetcatalogfileexplorer.h \
    spreadsheetconnectors/SpreadSheet.h \
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

resources.files = spreadsheetconnectors/resources/spreadsheetformats.config
resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

DISTFILES += \
    spreadsheetconnectors/resources/libraries.config \
    spreadsheetconnectors/resources/spreadsheetformats.config

linux{
    dependencies.files = $$GEOSLIB/libgeos-3.4.2.so \
        $$LINUXLIB\libquazip.so \
        $$LINUXLIB\libfreexl.so.1 \
        $$PWD/../libraries/$$PLATFORM$$CONF/QOds/libQOds.so \
        $$PWD/../libraries/$$PLATFORM$$CONF/Qt0Xlsx/libQt0Xlsx.so


    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}
