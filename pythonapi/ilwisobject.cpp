#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
#include "ilwisobject.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
    this->_ilwisObjectID = Ilwis::i64UNDEF;
}

void IlwisObject::connectTo(const char *url, const char *format, const char *fnamespace, ConnectorMode cmode){
    Ilwis::IIlwisObject io;
    if(this->isValid())
        io = this->ptr();
    else{
        if (cmode == cmINPUT){
            io.prepare(QString(url), Ilwis::IlwisObject::findType(url));
            this->_ilwisObjectID = io->id();
        }else
            throw Ilwis::ErrorObject(QString("Cannot connect to invalid object."));
    }
    this->ptr()->connectTo(QUrl(url), QString(format), QString(fnamespace), (Ilwis::IlwisObject::ConnectorMode)cmode);
}

bool IlwisObject::isValid(){
    return this->_ilwisObjectID != Ilwis::i64UNDEF && this->ptr().isValid() && this->ptr()->isValid();
}

const char* IlwisObject::__str__(){
    return QString("%1(%2)").arg(Ilwis::IlwisObject::type2Name(this->ptr()->ilwisType())).arg(this->ptr()->name()).toLocal8Bit();
}

Ilwis::IIlwisObject IlwisObject::ptr() const{
    Ilwis::IIlwisObject io;
    io.prepare(this->_ilwisObjectID);
    return io;
}

quint64 IlwisObject::ilwisID() const{
    if (this->_ilwisObjectID == Ilwis::i64UNDEF)
        throw Ilwis::ErrorObject(QString("invalid IlwisObjectID"));
    return this->_ilwisObjectID;
}
