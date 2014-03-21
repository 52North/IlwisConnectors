#ifndef PYTHONAPI_EXTENSION_H
#define PYTHONAPI_EXTENSION_H

#include "pythonapi_qtGNUTypedefs.h"

namespace pythonapi{
    //init modul
    bool initIlwisObjects();
    void disconnectIssueLogger();
    void connectIssueLogger();
}
#endif // PYTHONAPI_EXTENSION_H
