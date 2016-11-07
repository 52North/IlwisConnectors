QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

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
    BOOST=../external
    SHAREDEXT=dll
    PREFIXSHARED=
}
linux{
    BOOST=/usr/include
    OPENCVLIB=/usr/local/lib
    SHAREDEXT=so
    PREFIXSHARED=lib
}

linux-g++{
   !contains(QT_ARCH, x86_64){
       LINUXLIB=/usr/lib/i386-linux-gnu/
       LINUXSYSTEMLIB=/lib/i386-linux-gnu/
    } else {
       LINUXLIB=/usr/lib/x86_64-linux-gnu/
       LINUXSYSTEMLIB=/lib/x86_64-linux-gnu/
   }
}

QT += sql network

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
                ../IlwisCore/core/ilwisobjects/operation/modeller \
                ../IlwisCore/core/ilwisobjects/representation \
                ../IlwisCore/core/catalog \
                ../IlwisCore/core/ilwisobjects/domain \
                ../IlwisCore/core/geos/include \
                $$BOOST


