#ifndef PYTHONAPI_EXTENSION_H
#define PYTHONAPI_EXTENSION_H

#include "pythonapi_qtGNUTypedefs.h"

#include "../../core/ilwistypes.h"

namespace pythonapi{
    //init modul
    bool _initIlwisObjects();
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
            DOMAIN = itDOMAIN,
            NUMERICDOMAIN = itNUMERICDOMAIN,
            OPERATIONMETADATA = itOPERATIONMETADATA,
            PROJECTION = itPROJECTION,
            ELLIPSOID = itELLIPSOID,
            CATALOG = itCATALOG,
            CONTINUOUSCOLOR = itCONTINUOUSCOLOR,
            PALETTECOLOR = itPALETTECOLOR
        };
    };
}
#endif // PYTHONAPI_EXTENSION_H
