#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "pythonapi_ilwisobject.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
}

IlwisObject::IlwisObject(Ilwis::IIlwisObject *object): _ilwisObject(std::shared_ptr<Ilwis::IIlwisObject>(object)){
}

IlwisObject::~IlwisObject(){
}

bool IlwisObject::connectTo(const char *url, const char *format, const char *fnamespace, ConnectorMode cmode){
    Ilwis::IIlwisObject io = (*this->ptr());
    if (cmode == cmINPUT || cmode == cmEXTENDED){
        if (!io.prepare(QString(url), io->ilwisType()))
            throw Ilwis::ErrorObject(QString("Cannot reconnect %1 for input").arg(url));
        else
            return true;
    }else{
        return io->connectTo(QUrl(url), QString(format), QString(fnamespace), (Ilwis::IlwisObject::ConnectorMode)cmode);
    }
}

bool IlwisObject::store(int storeMode){
    return (*this->ptr())->store(storeMode);
}

bool IlwisObject::__bool__() const{
    return this->_ilwisObject != NULL && this->_ilwisObject->isValid() && (*this->_ilwisObject)->isValid();
}

const char* IlwisObject::__str__(){
    if (this->__bool__())
        return QString("%1%2").arg(NAME_ALIAS).arg((*this->ptr())->id()).toLocal8Bit();
    else
        return QString("invalid IlwisObject!").toLocal8Bit();
}

const char* IlwisObject::name(){
    if (this->__bool__())
        return (*this->ptr())->name().toLocal8Bit();
    else
        return QString("invalid IlwisObject!").toLocal8Bit();
}

void IlwisObject::name(const char* name){
    (*this->ptr())->setName(QString(name));
}

const char *IlwisObject::type(){
    if (this->__bool__())
        return Ilwis::IlwisObject::type2Name((*this->ptr())->ilwisType()).toLocal8Bit();
    else
        return QString("invalid IlwisObject!").toLocal8Bit();
}

const char *IlwisObject::__add__(const char *value){
    if (this->__bool__())
        return QString("%2%1").arg(value).arg(this->__str__()).toLocal8Bit();
    else
        return QString("invalid IlwisObject!").toLocal8Bit();
}

const char *IlwisObject::__radd__(const char *value){
    if (this->__bool__())
        return QString("%1%2").arg(value).arg(this->__str__()).toLocal8Bit();
    else
        return QString("invalid IlwisObject!").toLocal8Bit();
}

std::shared_ptr<Ilwis::IIlwisObject> IlwisObject::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("invalid IlwisObject");
    return this->_ilwisObject;
}

quint64 IlwisObject::ilwisID() const{
    return (*this->ptr())->id();
}

IlwisTypes IlwisObject::ilwisType(){
    return (*this->ptr())->ilwisType();
}
