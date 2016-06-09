include(global.pri)

QT += core

TARGET = _ilwisobjects

TARGETDIR = pythonapi

QMAKE_EXTENSION_SHLIB = pyd

TEMPLATE = lib

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGETDIR
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGETDIR

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
    pythonapi/pythonapi_collection.h \
    pythonapi/pythonapi_catalog.h \
    pythonapi/pythonapi_datadefinition.h \
    pythonapi/pythonapi_rangeiterator.h \
    pythonapi/pythonapi_columndefinition.h \
    pythonapi/pythonapi_domainitem.h \
    pythonapi/pythonapi_vertexiterator.h \

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
    pythonapi/pythonapi_collection.cpp \
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
    pythonapi/paths.py \
    pythonapi/installerPy.nsi \
    pythonapi/LICENSE-2.0.txt \
    pythonapi/README \
    pythonapi/UPDATE \
    pythonapi/CHANGELOG

PYMINORVERSION = 0
PYVERLIST = 1 2 3 4 5 6 7 8 9
for (VER, PYVERLIST): exists($$PWD/../external/Python3$$VER/include/Python.h) {
    PYMINORVERSION = $$VER
}
equals(PYMINORVERSION, 0) {
    for (VER, PYVERLIST): exists(C:/Python3$$VER/include/Python.h) {
        PYMINORVERSION = $$VER
    }
    equals(PYMINORVERSION, 0) {
        message("Error: Python not found in $$clean_path($$PWD/../external/) or in C:/")
    } else {
        message("Configuring for Python 3.$$PYMINORVERSION found in C:/Python3$$PYMINORVERSION")
        LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
                -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
                -LC:/Python3$$PYMINORVERSION/libs -lpython3$$PYMINORVERSION

        INCLUDEPATH += $$PWD/../ilwiscore/core \
                       $$PWD/../external/geos \
                       C:/Python3$$PYMINORVERSION/include/
    }
} else {
    message("Configuring for Python 3.$$PYMINORVERSION found in $$clean_path($$PWD/../external/Python3$$PYMINORVERSION/include)/ and in $$clean_path($$DESTDIR)/")
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
            -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
            -L$$DESTDIR -lpython3$$PYMINORVERSION

    INCLUDEPATH += $$PWD/../ilwiscore/core \
                   $$PWD/../external/geos \
                   $$PWD/../external/Python3$$PYMINORVERSION/include/
}

DEPENDPATH += $$PWD/../ilwiscore/core \
              $$PWD/../external/geos

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

mytarget.files = pythonapi/ilwisobjects.py \
                 pythonapi/test.py \
                 pythonapi/README \
                 pythonapi/UPDATE \
                 pythonapi/CHANGELOG

mytarget.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonapi

install_target.files = pythonapi/installerPy.nsi

install_target.path = $$PWD/../output/$$PLATFORM$$CONF

INSTALLS += mytarget install_target
