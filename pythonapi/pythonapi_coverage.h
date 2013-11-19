#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "pythonapi_ilwisobject.h"

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
    };

}

#endif // PYTHONAPI_COVERAGE_H
