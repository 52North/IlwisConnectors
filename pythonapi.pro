include(global.pri)

QT += core

TARGET = _ilwisobjects

PYTHONAPI_FOLDER = pythonapi

QMAKE_EXTENSION_SHLIB = pyd

TEMPLATE = lib

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$PYTHONAPI_FOLDER

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
    pythonapi/ilwisobjects.py \
    pythonapi/paths.py \
    pythonapi/installerPy.nsi \
    pythonapi/LICENSE-2.0.txt \
    pythonapi/README \
    pythonapi/UPDATE \
    pythonapi/CHANGELOG

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
INCLUDEPATH += $$PWD/../ilwiscore/core \
               $$PWD/../external/geos \
               $$PWD/../external/python35

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
        LIBS += -LC:/Python3$$PYMINORVERSION/libs -lpython3$$PYMINORVERSION
        INCLUDEPATH += C:/Python3$$PYMINORVERSION/include/
    }
} else {
    message("Configuring for Python 3.$$PYMINORVERSION found in $$clean_path($$PWD/../external/Python3$$PYMINORVERSION/include)/ and in $$clean_path($$DESTDIR)/")
    exists($$DESTDIR/python3$$PYMINORVERSION*) {
    } else {
        exists($$DESTDIR/libpython3$$PYMINORVERSION*) {
        } else {
            message("Check!! Missing $$clean_path($$DESTDIR/python3$$PYMINORVERSION)*.lib or $$clean_path($$DESTDIR/libpython3$$PYMINORVERSION).a")
        }
    }
    LIBS += -L$$DESTDIR -lpython3$$PYMINORVERSION
    INCLUDEPATH += $$PWD/../external/Python3$$PYMINORVERSION/include/
}

DEPENDPATH += $$PWD/../ilwiscore/core \
              $$PWD/../external/geos

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

installer_target.files = pythonapi/installerPy.nsi
installer_target.path = $$PWD/../output/$$PLATFORM$$CONF
INSTALLS += installer_target

COPY_FILES = ilwisobjects.py test.py README UPDATE CHANGELOG
SOURCE_DIR = $$clean_path($$PWD/$$PYTHONAPI_FOLDER)

linux {
    DEST_DIR = $$clean_path($$DESTDIR)
    QMAKE_PRE_LINK += $$quote(cat $$SOURCE_DIR/paths.py $$SOURCE_DIR/ilwisobjects.py > $$SOURCE_DIR/temp.py$$escape_expand(\n\t))
    QMAKE_PRE_LINK += $$quote(mv -f $$SOURCE_DIR/temp.py $$SOURCE_DIR/ilwisobjects.py$$escape_expand(\n\t))
    QMAKE_POST_LINK +=$$quote(test -d $$DEST_DIR || mkdir -p $$DEST_DIR$$escape_expand(\n\t))
    for(FILE,COPY_FILES){
        QMAKE_POST_LINK += $$quote(cp $$SOURCE_DIR/$$FILE $$DEST_DIR$$escape_expand(\n\t))
    }
}

win32 {
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$PYTHONAPI_FOLDER
    SOURCE_DIR = $$replace(SOURCE_DIR,/,\\)
    DEST_DIR = $$clean_path($$DLLDESTDIR)
    DEST_DIR = $$replace(DEST_DIR,/,\\)
    QMAKE_PRE_LINK += $$quote(copy /y /b $$SOURCE_DIR\\paths.py + $$SOURCE_DIR\\ilwisobjects.py $$SOURCE_DIR\\temp.py$$escape_expand(\n\t))
    QMAKE_PRE_LINK += $$quote(move /y $$SOURCE_DIR\\temp.py $$SOURCE_DIR\\ilwisobjects.py$$escape_expand(\n\t))
    QMAKE_POST_LINK +=$$quote(if not exist $$DEST_DIR mkdir $$DEST_DIR$$escape_expand(\n\t))
    for(FILE,COPY_FILES){
        QMAKE_POST_LINK +=$$quote(copy /y $$SOURCE_DIR\\$$FILE $$DEST_DIR$$escape_expand(\n\t))
    }
}
