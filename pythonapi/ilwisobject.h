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
    protected:
        quint64 _ilwisObjectID;
        Ilwis::IIlwisObject ptr() const;
    public:
        IlwisObject();
        virtual ~IlwisObject(){};
        bool isValid();
        const char *__str__();
        quint64 id() const;
    };

}

#endif // PYTHONAPI_ILWISOBJECT_H
