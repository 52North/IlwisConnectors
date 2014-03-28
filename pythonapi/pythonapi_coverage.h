#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"
#include "pythonapi_table.h"

#include <vector>

typedef struct _object PyObject;

namespace Ilwis {
    class Coverage;
    typedef IlwisData<Coverage> ICoverage;
}

namespace pythonapi{

    class Coverage : public IlwisObject{
        public:
            enum AttributeType{atCOVERAGE, atINDEX};
        protected:
            Coverage();
            Coverage(Ilwis::ICoverage* coverage);
        public:
            virtual ~Coverage(){}
            bool addAttribute(const std::string& name, const std::string& domain);
            quint32 attributeCount();
            PyObject *attributes();
            Table attributeTable(AttributeType attType=atCOVERAGE);
            CoordinateSystem coordinateSystem();
            void setCoordinateSystem(const CoordinateSystem &cs);
    };

}

#endif // PYTHONAPI_COVERAGE_H
