#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "ilwisobject.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
}

IlwisObject::IlwisObject(Ilwis::IIlwisObject *object): _ilwisObject(std::shared_ptr<Ilwis::IIlwisObject>(object)){
}

IlwisObject::~IlwisObject(){
}

void IlwisObject::connectTo(const char *url, const char *format, const char *fnamespace, ConnectorMode cmode){
    if(this->__bool__()){
        if (cmode == cmINPUT || cmode == cmEXTENDED){
            if (!this->ptr()->prepare(QString(url), (*this->ptr())->ilwisType()))
                throw Ilwis::ErrorObject(QString("Cannot reconnect %1 for input").arg(url));
        }else{
            (*this->ptr())->connectTo(QUrl(url), QString(format), QString(fnamespace), (Ilwis::IlwisObject::ConnectorMode)cmode);
        }
    }else{
        throw Ilwis::ErrorObject(QString("Cannot connect to invalid object"));
    }
}

bool IlwisObject::store(IlwisObject::ConnectorMode storeMode){
    return (*this->ptr())->store((Ilwis::IlwisObject::ConnectorMode)storeMode);
}

bool IlwisObject::__bool__() const{
    return this->_ilwisObject != NULL && this->ptr()->isValid() && (*this->ptr())->isValid();
}

const char* IlwisObject::__str__(){
    if (this->__bool__())
        return QString("%1(%2)").arg(Ilwis::IlwisObject::type2Name((*this->ptr())->ilwisType())).arg((*this->ptr())->name()).toLocal8Bit();
    else
        return QString("invalid IlwisObject").toLocal8Bit();
}

std::shared_ptr<Ilwis::IIlwisObject> IlwisObject::ptr() const{
    return this->_ilwisObject;
}

quint64 IlwisObject::ilwisID() const{
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid IlwisObject"));
    return (*this->ptr())->id();
}

IlwisTypes IlwisObject::ilwisType(){
    if (!this->__bool__())
        throw Ilwis::ErrorObject(QString("invalid IlwisObject"));
    return (*this->ptr())->ilwisType();
}
