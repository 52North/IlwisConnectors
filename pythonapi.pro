include(global.pri)

QT += core

TARGET = _ilwisobjects

QMAKE_EXTENSION_SHLIB = pyd

TEMPLATE = lib

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

PYTHONDIR = C:/Python33

HEADERS += \
    pythonapi/pythonapi_util.h \
    pythonapi/pythonapi_table.h \
    pythonapi/pythonapi_rastercoverage.h \
    pythonapi/pythonapi_range.h \
    pythonapi/pythonapi_qvariant.h \
    pythonapi/pythonapi_qtGNUTypedefs.h \
    pythonapi/pythonapi_qissuelogger.h \
    pythonapi/pythonapi_pyobject.h \
    pythonapi/pythonapi_pixeliterator.h \
    pythonapi/pythonapi_object.h \
    pythonapi/pythonapi_ilwisobject.h \
    pythonapi/pythonapi_georeference.h \
    pythonapi/pythonapi_geometry.h \
    pythonapi/pythonapi_featureiterator.h \
    pythonapi/pythonapi_featurecoverage.h \
    pythonapi/pythonapi_feature.h \
    pythonapi/pythonapi_extension.h \
    pythonapi/pythonapi_error.h \
    pythonapi/pythonapi_engine.h \
    pythonapi/pythonapi_domain.h \
    pythonapi/pythonapi_coverage.h \
    pythonapi/pythonapi_coordinatesystem.h \
    pythonapi/pythonapi_catalog.h \
    pythonapi/pythonapi_datadefinition.h \
    pythonapi/pythonapi_rangeiterator.h \
    pythonapi/pythonapi_columndefinition.h \
    pythonapi/pythonapi_domainitem.h \
    pythonapi/pythonapi_vertexiterator.h \
    pythonapi/pythonapi_attributedefinition.h


SOURCES += \
    pythonapi/ilwisobjects_wrap.cxx \
    pythonapi/pythonapi_util.cpp \
    pythonapi/pythonapi_table.cpp \
    pythonapi/pythonapi_rastercoverage.cpp \
    pythonapi/pythonapi_range.cpp \
    pythonapi/pythonapi_qvariant.cpp \
    pythonapi/pythonapi_qissuelogger.cpp \
    pythonapi/pythonapi_pyobject.cpp \
    pythonapi/pythonapi_pixeliterator.cpp \
    pythonapi/pythonapi_ilwisobject.cpp \
    pythonapi/pythonapi_georeference.cpp \
    pythonapi/pythonapi_geometry.cpp \
    pythonapi/pythonapi_featureiterator.cpp \
    pythonapi/pythonapi_featurecoverage.cpp \
    pythonapi/pythonapi_feature.cpp \
    pythonapi/pythonapi_extension.cpp \
    pythonapi/pythonapi_error.cpp \
    pythonapi/pythonapi_engine.cpp \
    pythonapi/pythonapi_domain.cpp \
    pythonapi/pythonapi_coverage.cpp \
    pythonapi/pythonapi_coordinatesystem.cpp \
    pythonapi/pythonapi_catalog.cpp \
    pythonapi/pythonapi_datadefinition.cpp \
    pythonapi/pythonapi_rangeiterator.cpp \
    pythonapi/pythonapi_columndefinition.cpp \
    pythonapi/pythonapi_domainitem.cpp \
    pythonapi/pythonapi_vertexiterator.cpp


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
    pythonapi/paths.py \
    pythonapi/installerPy.nsi \
    pythonapi/LICENSE-2.0.txt \
    pythonapi/README \
    pythonapi/UPDATE \
    pythonapi/CHANGELOG

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
        -L$$PYTHONDIR/libs -lpython33

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH += $$PWD/../ilwiscore/core \
               $$PWD/../external/geos \
               $$PYTHONDIR/include/
DEPENDPATH += $$PWD/../ilwiscore/core \
              $$PWD/../external/geos

mytarget.files = pythonapi/ilwisobjects.py \
                 $$PWD/../libraries/$$PLATFORM$$CONF/extensions/_ilwisobjects/_ilwisobjects.pyd \
                 pythonapi/test.py \
                 pythonapi/README \
                 pythonapi/UPDATE \
                 pythonapi/CHANGELOG

mytarget.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

python_target.files = pythonapi/ilwisobjects.py \
                      pythonapi/CHANGELOG

python_target.path = $$PYTHONDIR/Lib/site-packages

install_target.files = pythonapi/installerPy.nsi

install_target.path = $$PWD/../output/$$PLATFORM$$CONF

target.path = $$PYTHONDIR/Lib/site-packages

INSTALLS += mytarget install_target python_target target
