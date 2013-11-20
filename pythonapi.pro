include(global.pri)

QT += core

TARGET = _ilwisobjects

QMAKE_EXTENSION_SHLIB = pyd

TEMPLATE = lib

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/pythonapi
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

HEADERS += \
    pythonapi/pythonapi_rastercoverage.h \
    pythonapi/pythonapi_coordinatesystem.h \
    pythonapi/pythonapi_engine.h \
    pythonapi/pythonapi_coverage.h \
    pythonapi/pythonapi_feature.h \
    pythonapi/pythonapi_featurecoverage.h \
    pythonapi/pythonapi_featureiterator.h \
    pythonapi/pythonapi_ilwis.h \
    pythonapi/pythonapi_ilwisobject.h \
    pythonapi/pythonapi_object.h \
    pythonapi/pythonapi_qissuelogger.h \
    pythonapi/pythonapi_pyvariant.h \
    pythonapi/pythonapi_geometry.h

SOURCES += \
    pythonapi/ilwisobjects_wrap.cxx \
    pythonapi/pythonapi_coordinatesystem.cpp \
    pythonapi/pythonapi_coverage.cpp \
    pythonapi/pythonapi_engine.cpp \
    pythonapi/pythonapi_feature.cpp \
    pythonapi/pythonapi_featurecoverage.cpp \
    pythonapi/pythonapi_featureiterator.cpp \
    pythonapi/pythonapi_ilwis.cpp \
    pythonapi/pythonapi_ilwisobject.cpp \
    pythonapi/pythonapi_qissuelogger.cpp \
    pythonapi/pythonapi_rastercoverage.cpp \
    pythonapi/pythonapi_pyvariant.cpp \
    pythonapi/pythonapi_pyerror.cpp \
    pythonapi/pythonapi_geometry.cpp

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
    pythonapi/ilwisobjects.conf

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
    pythonapi/test.bat \
    pythonapi/ilwisobjects.conf \
    pythonapi/qt.conf
mytarget.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

python_target.files = pythonapi/ilwisobjects.py \
    pythonapi/ilwisobjects.conf
python_target.path = C:/Python33/Lib/site-packages

python_base_tg.files = pythonapi/qt.conf
python_base_tg.files = C:/Python33

target.path = C:/Python33/Lib/site-packages

INSTALLS += mytarget python_target target


