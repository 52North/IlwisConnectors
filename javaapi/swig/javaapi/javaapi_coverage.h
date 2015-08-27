#ifndef JAVAAPI_COVERAGE_H
#define JAVAAPI_COVERAGE_H

#include "javaapi_ilwisobject.h"
#include "javaapi_coordinatesystem.h"
#include "javaapi_table.h"
#include "javaapi_util.h"

#include <vector>

namespace Ilwis {
    class Coverage;
    typedef IlwisData<Coverage> ICoverage;
}

namespace javaapi{
    class Domain;
    class ColumnDefinition;
    class Coverage : public IlwisObject{
        public:
            enum AttributeType{atCOVERAGE, atINDEX};
        protected:
            Coverage();
            Coverage(Ilwis::ICoverage* coverage);
        public:
            virtual ~Coverage(){}
            CoordinateSystem coordinateSystem();
            javaapi::Envelope envelope();
            void setEnvelope(const javaapi::Envelope& env);
    };

}

#endif // JAVAAPI_COVERAGE_H
