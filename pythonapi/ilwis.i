/* The ILWIS SWIG interface file*/

%module ilwis

%{
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"
%}

class KERNELSHARED_EXPORT IlwisObject : public QObject, public Identity{

};
