#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"
#include "pythonapi_table.h"
#include "pythonapi_util.h"

#include <vector>

typedef struct _object PyObject;

namespace Ilwis {
    class Coverage;
    typedef IlwisData<Coverage> ICoverage;
}

namespace pythonapi{
    class Domain;
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
            void setTable(Table& tbl, AttributeType attType=atCOVERAGE);
            bool hasAttributes(AttributeType attType=atCOVERAGE) const;
            CoordinateSystem coordinateSystem();
            void setCoordinateSystem(const CoordinateSystem &cs);
            pythonapi::Envelope envelope();
            void setEnvelope(const pythonapi::Envelope& env);
            void indexDomain(const Domain& dom);
            PyObject* indexValues();
            PyObject* value(const std::string& colName, quint32 itemid, qint32 layerIndex = -1);
    };

}

#endif // PYTHONAPI_COVERAGE_H
