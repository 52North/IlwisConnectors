#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = gdalconnector

include(global.pri)

TEMPLATE = lib

DEFINES += GDALCONNECTOR_LIBRARY

SOURCES += \
    gdalconnector/gdalconnector.cpp \
    gdalconnector/gdalmodule.cpp \
    gdalconnector/gdalproxy.cpp \
    gdalconnector/gdalitem.cpp \
    gdalconnector/coverageconnector.cpp \
    gdalconnector/coordinatesystemconnector.cpp \
    gdalconnector/domainconnector.cpp \
    gdalconnector/gridcoverageconnector.cpp \
    gdalconnector/gdalobjectfactory.cpp \
    gdalconnector/georefconnector.cpp \
    gdalconnector/gdalfeatureconnector.cpp \
    gdalconnector/gdalfeaturetableconnector.cpp \
    gdalconnector/gdaltableloader.cpp \
    gdalconnector/gdalcatalogexplorer.cpp \
    gdalconnector/gdalcatalogfileexplorer.cpp

HEADERS += \
    gdalconnector/gdalconnector.h\
    gdalconnector/gdalconnector_global.h \
    gdalconnector/gdalmodule.h \
    gdalconnector/gdalproxy.h \
    gdalconnector/gdalitem.h \
    gdalconnector/coverageconnector.h \
    gdalconnector/coordinatesystemconnector.h \
    gdalconnector/domainconnector.h \
    gdalconnector/gridcoverageconnector.h \
    gdalconnector/gdalobjectfactory.h \
    gdalconnector/georefconnector.h \
    gdalconnector/gdalfeatureconnector.h \
    gdalconnector/gdalfeaturetableconnector.h \
    gdalconnector/gdaltableloader.h \
    gdalconnector/gdalcatalogexplorer.h \
    gdalconnector/gdalcatalogfileexplorer.h

OTHER_FILES += \
    gdalconnector/gdalconnector.json \
    gdalconnector/resources/ogr_formats.config

INCLUDEPATH +=  $$PWD/../external/gdal

resources.files = gdalconnector/resources/ogr_formats.config
resources.files += gdalconnector/resources/libraries.config
resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

INSTALLS += resources

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET


linux{
    dependencies.files = $$LINUXSYSTEMLIB/libz.so.1 \
                        $$LINUXLIB/libsqlite3.so \
                        $$LINUXLIB/libfreexl.so.1 \
                        /usr/lib/libgdal.so.1 \
                        /usr/lib/libpq.so \
                        $$LINUXLIB/libcurl.so.4 \
                        $$LINUXSYSTEMLIB/libexpat.so.1 \
                        /usr/lib/libproj.so.0 \
                        /usr/lib/libnetcdf.so.7 \
                        $$LINUXLIB/libhdf5.so \
                        $$LINUXLIB/libhdf5_hl.so \
                        /usr/local/libiconv/lib/libcharset.so.1 \
                        /usr/local/libiconv/lib/libiconv.so.2 \
                        $$LINUXLIB/libcrypto.so \
                        $$LINUXLIB/libssl.so



    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}
