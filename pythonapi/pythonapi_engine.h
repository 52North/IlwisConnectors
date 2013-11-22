#ifndef PYTHONAPI_ENGINE_H
#define PYTHONAPI_ENGINE_H

#include "pythonapi_ilwisobject.h"

namespace pythonapi {

    class Engine{
    public:
        Engine();
        static Object* _do(const char* operation,const char* c3 = "",const char* c4 = "",const char* c5 = "",const char* c6 = "",const char* c7="", const char* c8="", const char* c9="", const char* output_name="");
        static bool setWorkingCatalog(const char* location);
    };

}
#endif // PYTHONAPI_ENGINE_H
