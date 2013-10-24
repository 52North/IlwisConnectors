#ifndef PYTHONAPI_ILWISOBJECT_H
#define PYTHONAPI_ILWISOBJECT_H

namespace Ilwis {
    class IlwisObject;
}

namespace pythonapi {

    class IlwisObject{
        protected:
            Ilwis::IlwisObject* data;

        public:
            IlwisObject();
    };

}

#endif // PYTHONAPI_ILWISOBJECT_H
