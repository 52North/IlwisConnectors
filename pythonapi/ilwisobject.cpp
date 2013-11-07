#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "ilwisobject.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
    this->_ilwisObject = NULL;
}

IlwisObject::~IlwisObject(){
//    delete this->_ilwisObject;
}

void IlwisObject::connectTo(const char *url, const char *format, const char *fnamespace, ConnectorMode cmode){
    if(this->isValid()){
        if (cmode == cmINPUT || cmode == cmEXTENDED){
            if (!this->ptr().prepare(QString(url)))
                throw Ilwis::ErrorObject(QString("Cannot reconnect %1 for input").arg(url));
        }else{
            this->ptr()->connectTo(QUrl(url), QString(format), QString(fnamespace), (Ilwis::IlwisObject::ConnectorMode)cmode);
        }
    }else{
        throw Ilwis::ErrorObject(QString("Cannot connect to invalid object"));
    }
}

void IlwisObject::store(IlwisObject::ConnectorMode storeMode){
    this->ptr()->store((Ilwis::IlwisObject::ConnectorMode)storeMode);
}

bool IlwisObject::isValid() const{
    return this->_ilwisObject != NULL && this->ptr().isValid() && this->ptr()->isValid();
}

const char* IlwisObject::__str__(){
    return QString("%1(%2)").arg(Ilwis::IlwisObject::type2Name(this->ptr()->ilwisType())).arg(this->ptr()->name()).toLocal8Bit();
}

Ilwis::IIlwisObject IlwisObject::ptr() const{
    return (*this->_ilwisObject);
}

quint64 IlwisObject::ilwisID() const{
    if (this->isValid())
        throw Ilwis::ErrorObject(QString("invalid IlwisObject"));
    return this->ptr()->id();
}
