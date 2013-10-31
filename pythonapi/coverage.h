#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "ilwisobject.h"

namespace pythonapi{

    class Coverage : public IlwisObject{
        public:
            Coverage();
            virtual ~Coverage(){};
            virtual bool isValid() = 0;
    };

}

#endif // PYTHONAPI_COVERAGE_H
