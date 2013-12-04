#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"

#include <vector>

typedef struct _object PyObject;

namespace pythonapi{

    class Coverage : public IlwisObject{
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
