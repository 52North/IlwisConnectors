#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "ilwisobject.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
}

bool IlwisObject::isValid(){
    return this->_ilwisObjectID != Ilwis::i64UNDEF && this->ptr().isValid() && this->ptr()->isValid();
}

const char* IlwisObject::__str__(){
    return QString("%1(%2)").arg(Ilwis::IlwisObject::type2Name(this->ptr()->ilwisType())).arg(this->ptr()->name()).toLocal8Bit();
}

Ilwis::IIlwisObject IlwisObject::ptr() const{
    Ilwis::IIlwisObject fc;
    fc.prepare(this->_ilwisObjectID);
    return fc;
}

quint64 IlwisObject::ilwisID() const{
    return this->_ilwisObjectID;
}
