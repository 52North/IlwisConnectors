#ifndef ILWISOBJECT_H
#define ILWISOBJECT_H

namespace pythonapi {

    class IlwisObject{
        private:
            Ilwis::IlwisObject* data;

        public:
            IlwisObject();
    };

}

#endif // ILWISOBJECT_H
