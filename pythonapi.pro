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
    pythonapi/pythonapi_geometry.h \
    pythonapi/pythonapi_qtGNUTypedefs.h \
    pythonapi/pythonapi_pixeliterator.h \
    pythonapi/pythonapi_util.h \
    pythonapi/pythonapi_pycontainer.h \
    pythonapi/pythonapi_table.h \
    pythonapi/pythonapi_georeference.h \
    pythonapi/pythonapi_pydatetime.h

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
    pythonapi/pythonapi_geometry.cpp \
    pythonapi/pythonapi_pycontainer.cpp \
    pythonapi/pythonapi_pixeliterator.cpp \
    pythonapi/pythonapi_util.cpp \
    pythonapi/pythonapi_table.cpp \
    pythonapi/pythonapi_georeference.cpp \
    pythonapi/pythonapi_pydatetime.cpp

OTHER_FILES += \
    pythonapi/test.py \
    pythonapi/setup.py \
    pythonapi/ilwisobjects.i \
    pythonapi/test.sh \
    pythonapi/test.bat \
    pythonapi/qt.conf \
    pythonapi/ilwisobjects.conf \
    pythonapi/prepare_PATH.bat \
    pythonapi/prepare_PATH.sh \
    pythonapi/README \
    pythonapi/UPDATE \
    pythonapi/CHANGELOG

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
        -LC:\Python33\libs -lpython33

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH += $$PWD/../ilwiscore/core \
               $$PWD/../external/geos \
               C:/Python33/include/
DEPENDPATH += $$PWD/../ilwiscore/core \
              $$PWD/../external/geos

mytarget.files = pythonapi/ilwisobjects.py \
                 pythonapi/README \
                 pythonapi/UPDATE \
                 pythonapi/CHANGELOG
mytarget.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

python_target.files = pythonapi/ilwisobjects.py \
                      pythonapi/CHANGELOG
python_target.path = C:/Python33/Lib/site-packages

target.path = C:/Python33/Lib/site-packages

INSTALLS += mytarget python_target target


