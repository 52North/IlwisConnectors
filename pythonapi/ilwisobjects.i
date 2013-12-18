/* The ILWIS SWIG interface file*/

%module(docstring="The Python API for ILWIS NG") ilwisobjects

%feature("autodoc","1");

%include "exception.i"
%include "std_string.i"

%{
#include "pythonapi_qtGNUTypedefs.h"
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
#include "pythonapi_featureiterator.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_pixeliterator.h"
#include "pythonapi_rastercoverage.h"
%}

%include "pythonapi_qtGNUTypedefs.h"

%init %{
    //init IlwisException for Python
    pythonapi::ilwisException = PyErr_NewException("_ilwisobjects.IlwisException",NULL,NULL);
    Py_INCREF(pythonapi::ilwisException);
    PyModule_AddObject(m, "IlwisException", pythonapi::ilwisException);//m is SWIG declaration for Python C API modul creation
    //init InvalidObjectException for Python
    pythonapi::invalidObjectException = PyErr_NewException("_ilwisobjects.InvalidObjectException",NULL,NULL);
    Py_INCREF(pythonapi::invalidObjectException);
    PyModule_AddObject(m, "InvalidObjectException", pythonapi::invalidObjectException);//m is SWIG declaration for Python C API modul creation

    //init QtCoreApllication, Ilwis library and IssueLogger connection
    try {
        if (!pythonapi::initIlwisObjects()){
            PyErr_SetString(PyExc_ImportError,"ILWIS couldn't be initiallized!");
            return NULL;
        }
    }catch (std::exception& e) {
        PyErr_SetString(pythonapi::translate_Exception_type(e),pythonapi::get_err_message(e));
        return NULL;
    }
%}
//adds the export flag to pyd library for the IlwisException
%pythoncode %{
    IlwisException = _ilwisobjects.IlwisException
    InvalidObjectException = _ilwisobjects.InvalidObjectException

    def readOnly():
        raise TypeError("this property is read only!")


    class ReadOnly(type):
        @property
        def sUNDEF(cls):
            return "?"
        @property
        def shUNDEF(cls):
            return -32767
        @property
        def iUNDEF(cls):
            return -2147483647
        @property
        def rUNDEF(cls):
            return -1e308
        @property
        def flUNDEF(cls):
            return -1e38
        @property
        def i64UNDEF(cls):
            return -9223372036854775808

    class Const(metaclass=ReadOnly):pass
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
    //instead of including whole (pythonapi_ilwis.h)
    void disconnectIssueLogger();
    void connectIssueLogger();
//    class UNDEF{
//    public:
//        static std::string __str__();
//        static double __float__();
//        static qint64 __int__();
//    };
}

%include "pythonapi_object.h"

%include "pythonapi_engine.h"
%extend pythonapi::Engine {
%insert("python") %{
    @staticmethod
    def do(operation,arg1="",arg2="",arg3="",arg4="",arg5="",arg6="",arg7="",out=""):
        if str(operation) != "":
            obj = Engine__do(str(out),str(operation),str(arg1),str(arg2),str(arg3),str(arg4),str(arg5),str(arg6),str(arg7))
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
%extend pythonapi::Size {
%insert("python") %{
    __swig_getmethods__["xsize"] = xsize
    __swig_getmethods__["ysize"] = ysize
    __swig_getmethods__["zsize"] = zsize
    __swig_setmethods__["xsize"] = setXsize
    __swig_setmethods__["ysize"] = setYsize
    __swig_setmethods__["zsize"] = setZsize
    if _newclass:
        xsize = property(xsize,setXsize)
        ysize = property(ysize,setYsize)
        zsize = property(zsize,setZsize)
%}
}
%include "pythonapi_feature.h"

%include "pythonapi_featureiterator.h"

%include "pythonapi_featurecoverage.h"

%include "pythonapi_pixeliterator.h"

%include "pythonapi_rastercoverage.h"

