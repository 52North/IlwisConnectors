#ifndef PYTHONAPI_FEATURE_H
#define PYTHONAPI_FEATURE_H

#include "pyvariant.h"

namespace Ilwis{
    class SPFeatureI;
}

namespace pythonapi{

    class Feature{
    public:
        Feature(Ilwis::SPFeatureI* ilwisFeature);
        const char *__str__();
        PyVariant* cell(const char* name, int index = -1);

    private:
        //TODO: replace with std::shared_ptr of that feature if done in IlwisCore
        Ilwis::SPFeatureI* _ilwisSPFeatureI;
    };

}

#endif // PYTHONAPI_FEATURE_H
