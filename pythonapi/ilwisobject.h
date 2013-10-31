#ifndef PYTHONAPI_ILWISOBJECT_H
#define PYTHONAPI_ILWISOBJECT_H

#include <memory>

namespace Ilwis {
    class IlwisObject;
    template<class T> class IlwisData;
    typedef std::shared_ptr<IlwisObject> ESPIlwisObject;
}


namespace pythonapi {

    class IlwisObject{
    protected:
        Ilwis::ESPIlwisObject _ilwisIIlwisObject;
    public:
        IlwisObject();
        virtual ~IlwisObject(){};
        virtual bool isValid() = 0;
    };

}

#endif // PYTHONAPI_ILWISOBJECT_H
