#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"

#include <string>
#include <vector>

namespace pythonapi{

    class Coverage : public IlwisObject{
        public:
            Coverage();
            virtual ~Coverage(){};
            bool addAttribute(const char* name, const char* domain);
            quint32 attributeCount();
            std::vector<std::string> attributes();
            int __len__();
            const char* __getitem__(int i);
            CoordinateSystem coordinateSystem();
    };

}

#endif // PYTHONAPI_COVERAGE_H
