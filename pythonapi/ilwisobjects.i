/* The ILWIS SWIG interface file*/

%module ilwisobjects

%{
#include "ilwis.h"
#include "ilwisobject.h"
#include "coverage.h"
#include "featurecoverage.h"
%}

%init %{
    pythonapi::initIlwisObjects();
%}

%include "ilwisobject.h"

%include "coverage.h"

%include "featurecoverage.h"
