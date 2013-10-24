include(global.pri)

QT += core sql

TARGET = _ilwisobjects

TEMPLATE = lib

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/pythonapi
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

HEADERS += \
    pythonapi/ilwisobject.h \
    pythonapi/coverage.h \
    pythonapi/featurecoverage.h

SOURCES += \
    pythonapi/ilwisobject.cpp \
    pythonapi/coverage.cpp \
    pythonapi/featurecoverage.cpp \
    pythonapi/ilwisobjects_wrap.cxx

OTHER_FILES += \
    pythonapi/test.py \
    pythonapi/setup.py \
    pythonapi/README \
    pythonapi/ilwisobjects.i \
    pythonapi/ilwisobjects.sip \
    pythonapi/configure.py

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

LIBS += -LC:\Python33\libs -lpython33

INCLUDEPATH += $$PWD/../ilwiscore/core \
                C:/Python33/include/
DEPENDPATH += $$PWD/../ilwiscore/core
