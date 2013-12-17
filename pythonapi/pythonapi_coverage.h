#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"

#include <vector>

typedef struct _object PyObject;

namespace Ilwis {
    class Coverage;
    typedef IlwisData<Coverage> ICoverage;
}

namespace pythonapi{

    class Coverage : public IlwisObject{
        protected:
            Coverage(Ilwis::ICoverage* coverage);
        public:
            Coverage();
            virtual ~Coverage(){};
            bool addAttribute(const char* name, const char* domain);
            quint32 attributeCount();
            PyObject *attributes();
            CoordinateSystem coordinateSystem();
            void setCoordinateSystem(const CoordinateSystem &cs);
    };

}

#endif // PYTHONAPI_COVERAGE_H
