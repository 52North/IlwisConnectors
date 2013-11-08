#ifndef PYTHONAPI_ILWISOBJECT_H
#define PYTHONAPI_ILWISOBJECT_H

#include <memory>

namespace Ilwis {
    class IlwisObject;

    template<class T> class IlwisData;
    typedef IlwisData<IlwisObject> IIlwisObject;
    typedef std::shared_ptr<IlwisObject> ESPIlwisObject;
}

//Qt typedefs only for GNU compiler
typedef unsigned long long quint64; /* 64 bit unsigned */

namespace pythonapi {

    class IlwisObject{
    public:
        //should be the same as enum Ilwis::IlwisObject::ConnectorMode (ilwisobject.h)
        enum ConnectorMode{cmINPUT=1, cmOUTPUT=2, cmEXTENDED=4};
        enum StoreMode{smMETADATA=1, smBINARYDATA=2};

    protected:
        std::shared_ptr<Ilwis::IIlwisObject> _ilwisObject;
        std::shared_ptr<Ilwis::IIlwisObject> ptr() const;
    public:
        IlwisObject();
        virtual ~IlwisObject();

        void connectTo(const char* url, const char* format  = "", const char* fnamespace = "", ConnectorMode cmode = cmINPUT);
        bool store(ConnectorMode storeMode);
        bool isValid() const;
        const char *__str__();
        quint64 ilwisID() const;
    };

}

#endif // PYTHONAPI_ILWISOBJECT_H
