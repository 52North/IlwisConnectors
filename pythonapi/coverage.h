#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "ilwisobject.h"

namespace pythonapi{

    class Coverage : public IlwisObject{
        public:
            Coverage();
            virtual ~Coverage(){};
            bool addAttribute(const char* name, const char* domain);
    };

}

#endif // PYTHONAPI_COVERAGE_H
