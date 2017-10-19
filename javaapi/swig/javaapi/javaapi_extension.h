#ifndef JAVAAPI_EXTENSION_H
#define JAVAAPI_EXTENSION_H

#include "javaapi_qtGNUTypedefs.h"

#include "../../core/ilwistypes.h"

namespace javaapi{
    //init modul
    bool _initIlwisObjects(const char * ilwisDir);
    void _exitIlwisObjects();
    void disconnectIssueLogger();
    void connectIssueLogger();

    //ilwisTypes
    struct it{
        enum ilwisType{
            FEATURE = itFEATURE,
            RASTER = itRASTER,
            COVERAGE = itCOVERAGE,
            TABLE = itTABLE,
            GEOREF = itGEOREF,
            COORDSYSTEM = itCOORDSYSTEM,
            DOMAIN_ = itDOMAIN,
            NUMERICDOMAIN = itNUMERICDOMAIN,
            ITEMDOMAIN = itITEMDOMAIN,
            OPERATIONMETADATA = itOPERATIONMETADATA,
            PROJECTION = itPROJECTION,
            ELLIPSOID = itELLIPSOID,
            CATALOG = itCATALOG,
            CONTINUOUSCOLOR = itCONTINUOUSCOLOR,
            PALETTECOLOR = itPALETTECOLOR,
            VALUERANGE = itVALUERANGE
        };
    };
}
#endif // JAVAAPI_EXTENSION_H
