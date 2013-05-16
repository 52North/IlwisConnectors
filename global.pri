QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -unused-parameter

CONFIG(debug, debug|release) {
CONF=debug
}

CONFIG(release, debug|release) {
CONF=release
}

OUTDIR = $$PWD/output/$$CONF
PLATFORM = generic
win32{
    PLATFORM = win32
}

QT += sql

plugin{
DESTDIR = $$PWD/libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}dll{
DESTDIR = $$PWD/libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/output/$$PLATFORM$$CONF/bin
}else {
DESTDIR = $$PWD/libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/output/$$PLATFORM$$CONF/bin
}

INCLUDEPATH += sources/core \
                sources/core/ilwisobjects \
                sources/core/ilwisobjects/geometry \
                sources/core/util \
                sources/core/ilwisobjects/geometry/geodeticdatum \
                sources/core/ilwisobjects/geometry/projection \
                sources/core/ilwisobjects/geometry/coordinatesystem \
                sources/core/ilwisobjects/geometry/georeference \
                sources/core/ilwisobjects/coverage \
                sources/core/ilwisobjects/table \
                sources/core/ilwisobjects/operation \
                sources/core/catalog \
                sources/core/ilwisobjects/domain

HEADERS +=

SOURCES +=

INCLUDEPATH += sources

