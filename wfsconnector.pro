#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T19:05:01
#
#-------------------------------------------------

QT = sql network

CONFIG += plugin
TARGET = wfsconnector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

TEMPLATE = lib

DEFINES += WFSCONNECTOR_LIBRARY

SOURCES += \
    wfsconnector/wfsobjectfactory.cpp \
    wfsconnector/wfsconnector.cpp \
    wfsconnector/wfscatalogconnector.cpp \
    wfsconnector/wfsmodule.cpp \
    wfsconnector/wfsfeatureconnector.cpp \
    wfsconnector/wfs.cpp

HEADERS += \
    wfsconnector/wfsobjectfactory.h \
    wfsconnector/wfsconnector.h \
    wfsconnector/wfscatalogconnector.h \
    wfsconnector/wfsmodule.h \
    wfsconnector/wfsfeatureconnector.h \
    wfsconnector/wfs.h \
    wfsconnector/wfsconnector_global.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
                                              -L$$PWD/../libraries/$$PLATFORM$$CONF/pugixml/ -lpugixml
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
                                              -L$$PWD/../libraries/$$PLATFORM$$CONF/pugixml/ -lpugixml

INCLUDEPATH += $$PWD/core \
            $$PWD/../external/
DEPENDPATH += $$PWD/core

OTHER_FILES += \
    wfsconnector/wfsconnector.json


