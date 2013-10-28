include(global.pri)

QT += core

TARGET = _ilwisobjects

QMAKE_EXTENSION_SHLIB = pyd

TEMPLATE = lib

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/pythonapi
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

HEADERS += \
    pythonapi/ilwisobject.h \
    pythonapi/coverage.h \
    pythonapi/featurecoverage.h \
    pythonapi/ilwis.h

SOURCES += \
    pythonapi/ilwisobject.cpp \
    pythonapi/coverage.cpp \
    pythonapi/featurecoverage.cpp \
    pythonapi/ilwisobjects_wrap.cxx \
    pythonapi/ilwis.cpp

OTHER_FILES += \
    pythonapi/test.py \
    pythonapi/setup.py \
    pythonapi/README \
    pythonapi/ilwisobjects.i \
    pythonapi/ilwisobjects.sip \
    pythonapi/configure.py \
    pythonapi/test.sh \
    pythonapi/test.bat \
    pythonapi/qt.conf \
    pythonapi/ilwisobjects.config

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

LIBS += -LC:\Python33\libs -lpython33

INCLUDEPATH += $$PWD/../ilwiscore/core \
                C:/Python33/include/
DEPENDPATH += $$PWD/../ilwiscore/core

mytarget.files = pythonapi/ilwisobjects.py \
    pythonapi/test.py \
    pythonapi/README \
    pythonapi/test.sh \
    pythonapi/test.bat
mytarget.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

python_target.files = pythonapi/ilwisobjects.py \
    pythonapi/ilwisobjects.config
python_target.path = C:/Python33/Lib/site-packages

python_base_tg.files = pythonapi/qt.conf
python_base_tg.files = C:/Python33

target.path = C:/Python33/Lib/site-packages

INSTALLS += mytarget python_target target


