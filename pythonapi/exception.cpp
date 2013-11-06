#include "../../IlwisCore/core/errorobject.h"

#include "exception.h"

#include <QString>

const char* pythonapi::get_err_message(std::exception &e){
    Ilwis::ErrorObject* err = dynamic_cast<Ilwis::ErrorObject*>(&e);
    if (err)
        return QString("%1: %2").arg(err->what(),err->message()).toLocal8Bit();
    else
        return QString("non-ILWIS Exception: ").append(err->what()).toLocal8Bit();
}
