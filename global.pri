QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

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
BOOST=../external

QT += sql

plugin{
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}dll{
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
}else {
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
}

INCLUDEPATH += ../IlwisCore/core \
                ../IlwisCore/core/ilwisobjects \
                ../IlwisCore/core/ilwisobjects/geometry \
                ../IlwisCore/core/util \
                ../IlwisCore/core/ilwisobjects/geometry/geodeticdatum \
                ../IlwisCore/core/ilwisobjects/geometry/projection \
                ../IlwisCore/core/ilwisobjects/geometry/coordinatesystem \
                ../IlwisCore/core/ilwisobjects/geometry/georeference \
                ../IlwisCore/core/ilwisobjects/coverage \
                ../IlwisCore/core/ilwisobjects/table \
                ../IlwisCore/core/ilwisobjects/operation \
                ../IlwisCore/core/catalog \
                ../IlwisCore/core/ilwisobjects/domain \
                $$BOOST



