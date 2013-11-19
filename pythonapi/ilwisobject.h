#ifndef PYTHONAPI_ILWISOBJECT_H
#define PYTHONAPI_ILWISOBJECT_H

#include <memory>
#include "object.h"

namespace Ilwis {
    class IlwisObject;

    template<class T> class IlwisData;
    typedef IlwisData<IlwisObject> IIlwisObject;
    typedef std::shared_ptr<IlwisObject> ESPIlwisObject;
}

namespace pythonapi {

    class IlwisObject: public Object{
    public:
        //should be the same as enum Ilwis::IlwisObject::ConnectorMode (ilwisobject.h)
        enum ConnectorMode{cmINPUT=1, cmOUTPUT=2, cmEXTENDED=4};
        enum StoreMode{smMETADATA=1, smBINARYDATA=2};

    protected:
        std::shared_ptr<Ilwis::IIlwisObject> _ilwisObject;
        std::shared_ptr<Ilwis::IIlwisObject> ptr() const;
    public:
        IlwisObject();
        IlwisObject(Ilwis::IIlwisObject* object);
        virtual ~IlwisObject();

        void connectTo(const char* url, const char* format  = "", const char* fnamespace = "", ConnectorMode cmode = cmINPUT);
        bool store(ConnectorMode storeMode);
        bool __bool__() const;
        const char *__str__();
        const char *__add__(const char* value);
        const char *__radd__(const char* value);
        const char *name();
        const char *type();
        quint64 ilwisID() const;
        IlwisTypes ilwisType();
    };

}

#endif // PYTHONAPI_ILWISOBJECT_H
