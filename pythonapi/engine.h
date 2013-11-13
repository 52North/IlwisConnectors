#ifndef PYTHONAPI_ENGINE_H
#define PYTHONAPI_ENGINE_H

#include "ilwisobject.h"

namespace pythonapi {

    class Engine{
    public:
        Engine();
        static Object* _do(const char* command);
        static bool setWorkingCatalog(const char* location);
    };

}
#endif // PYTHONAPI_ENGINE_H
