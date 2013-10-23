include(global.pri)

QT += core

TARGET = _ilwis

TEMPLATE = lib

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/pythonapi
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

HEADERS +=

SOURCES += \
    pythonapi/ilwis_wrap.cxx

OTHER_FILES += \
    pythonapi/ilwis.i \
    pythonapi/test.py \
    pythonapi/setup.py \
    pythonapi/README

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/../ilwiscore/core \
                C:/Python33/include/
DEPENDPATH += $$PWD/../ilwiscore/core
