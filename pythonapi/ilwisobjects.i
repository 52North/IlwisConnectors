/* The ILWIS SWIG interface file*/

%module ilwisobjects

%include "exception.i"

%{
#include "pythonapi_ilwis.h"
#include "pythonapi_object.h"
#include "pythonapi_engine.h"
#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"
#include "pythonapi_coverage.h"
#include "pythonapi_pyvariant.h"
#include "pythonapi_object.h"
#include "pythonapi_geometry.h"
#include "pythonapi_feature.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_featureiterator.h"
#include "pythonapi_rastercoverage.h"
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


namespace pythonapi {
    void muteIssueLogger(); //(pythonapi_ilwis.h)
}

%include "pythonapi_object.h"

%include "pythonapi_engine.h"
%extend pythonapi::Engine {
%insert("python") %{
    @staticmethod
    def do(operation,arg1="",arg2="",arg3="",arg4="",arg5="",arg6="",arg7="",out=""):
        if str(operation) != "":
            obj = Engine__do(str(operation),str(arg1),str(arg2),str(arg3),str(arg4),str(arg5),str(arg6),str(arg7),str(out))
        else:
            raise IlwisException("no operation given!")
        if obj.ilwisType() == 8:
            return RasterCoverage.toRasterCoverage(obj)
        elif (obj.ilwisType() <= 7) and (obj.ilwisType() >= 1):
            return FeatureCoverage.toFeatureCoverage(obj)
        elif obj.ilwisType() <= 0xFFFFFFFFFFFFFFFF:
            return PyVariant.toPyVariant(obj)
        elif obj.ilwisType() == 0:
            raise TypeError("unknown IlwisType")
        else:
            return obj
%}
}

%include "pythonapi_ilwisobject.h"

%include "pythonapi_coordinatesystem.h"

%include "pythonapi_coverage.h"

%include "pythonapi_pyvariant.h"

%include "pythonapi_object.h"

%include "pythonapi_geometry.h"

%newobject pythonapi::Feature::attribute(const char*, int);//possibly no effect
%include "pythonapi_feature.h"

%include "pythonapi_featurecoverage.h"

%include "pythonapi_featureiterator.h"

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

//%include "std_vector.i"
//%include "std_string.i"
//// Instantiate templates used by example
//namespace std {
// %template(StdVector_String) vector<string>;
//}

%extend pythonapi::FeatureCoverage {
%insert("python") %{
    def __iter__(self):
        return FeatureIterator(self)
%}
}

%include "pythonapi_rastercoverage.h"
