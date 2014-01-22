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
#include "pythonapi_table.h"
#include "pythonapi_coverage.h"
#include "pythonapi_pyvariant.h"
#include "pythonapi_object.h"
#include "pythonapi_util.h"
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

%include "pythonapi_table.h"

%include "pythonapi_coverage.h"

%include "pythonapi_pyvariant.h"

%include "pythonapi_object.h"

%include "pythonapi_util.h"

namespace pythonapi {
%template(Pixel) Point2DTemplate<qint32>;
%template(Coordinate2D) Point2DTemplate<double>;
%template(Voxel) Point3DTemplate<qint32>;
%template(Coordinate) Point3DTemplate<double>;
%template(Box) BoxTemplate<qint32>;
%template(Envelope) BoxTemplate<double>;
}
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
%extend pythonapi::Point2DTemplate<qint32> {//Pixel
%insert("python") %{
    __swig_getmethods__["x"] = x
    __swig_getmethods__["y"] = y
    __swig_setmethods__["x"] = setX
    __swig_setmethods__["y"] = setY
    if _newclass:
        x = property(x,setX)
        y = property(y,setY)
%}
}
%extend pythonapi::Point2DTemplate<double> {//Coordinate2D
%insert("python") %{
    __swig_getmethods__["x"] = x
    __swig_getmethods__["y"] = y
    __swig_setmethods__["x"] = setX
    __swig_setmethods__["y"] = setY
    if _newclass:
        x = property(x,setX)
        y = property(y,setY)
%}
}
%extend pythonapi::Point3DTemplate<qint32> {//Voxel
%insert("python") %{
    __swig_getmethods__["z"] = z
    __swig_setmethods__["z"] = setZ
    if _newclass:
        z = property(z,setZ)
%}
}
%extend pythonapi::Point3DTemplate<double> {//Coordinate
%insert("python") %{
    __swig_getmethods__["z"] = z
    __swig_setmethods__["z"] = setZ
    if _newclass:
        z = property(z,setZ)
%}
}

%include "pythonapi_geometry.h"

%include "pythonapi_feature.h"

%include "pythonapi_featureiterator.h"

%include "pythonapi_featurecoverage.h"

%typemap(out) Py_buffer* {
    $result = PyMemoryView_FromBuffer($1);
}

%include "pythonapi_pixeliterator.h"

%include "pythonapi_rastercoverage.h"

