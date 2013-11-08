#ifndef PYTHONAPI_ENGINE_H
#define PYTHONAPI_ENGINE_H

#include "object.h"
#include "ilwisobject.h"

namespace pythonapi {

    class Engine{
    public:
        Engine();
        static Object* _do(const char* command);
    };

}
#endif // PYTHONAPI_ENGINE_H
