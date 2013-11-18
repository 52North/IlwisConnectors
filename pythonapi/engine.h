#ifndef PYTHONAPI_ENGINE_H
#define PYTHONAPI_ENGINE_H

#include "ilwisobject.h"

namespace pythonapi {

    class Engine{
    public:
        Engine();
        static Object* _do(const char* output_name,const char* operation,const char* c3 = "",const char* c4 = "",const char* c5 = "",const char* c6 = "",const char* c7="", const char* c8="", const char* c9="");
        static bool setWorkingCatalog(const char* location);
    };

}
#endif // PYTHONAPI_ENGINE_H
