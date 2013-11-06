/* The ILWIS SWIG interface file*/

%module ilwisobjects

%include "exception.i"

%{
#include "ilwis.h"
#include "ilwisobject.h"
#include "coverage.h"
#include "pyvariant.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
%}

%init %{
    //init QtCoreApllication, Ilwis library and IssueLogger connection
    pythonapi::initIlwisObjects();

    //init IlwisException for Python
    pythonapi::ilwisException = PyErr_NewException("_ilwisobjects.IlwisException",NULL,NULL);
    Py_INCREF(pythonapi::ilwisException);
    PyModule_AddObject(m, "IlwisException", pythonapi::ilwisException);//m is SWIG declaration for Python C API modul creation
%}
//adds the export flag to pyd library for the IlwisException
%pythoncode %{
    IlwisException = _ilwisobjects.IlwisException
%}
//catch std::exception's on all C API function calls
%exception{
    try {
        $action
    }catch (std::exception& e) {
        PyErr_SetString(pythonapi::translate_Exception_type(e),pythonapi::get_err_message(e));
        SWIG_fail;
    }
}

%include "ilwisobject.h"

%include "coverage.h"

%include "pyvariant.h"

%newobject pythonapi::Feature::cell(const char*, int);//possibly no effect
%include "feature.h"

%include "featurecoverage.h"

%include "featureiterator.h"

%extend pythonapi::FeatureIterator {
%insert("python") %{
    def __iter__(self):
        return self
%}

%insert("python") %{
    def __next__(self):
        if self.hasNext():
            return self.next()
        else:
            raise StopIteration
%}
}

%extend pythonapi::FeatureCoverage {
%insert("python") %{
    def __iter__(self):
        return FeatureIterator(self)
%}
}

