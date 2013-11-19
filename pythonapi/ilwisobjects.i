/* The ILWIS SWIG interface file*/

%module ilwisobjects

%include "exception.i"

%{
#include "ilwis.h"
#include "object.h"
#include "engine.h"
#include "ilwisobject.h"
#include "coverage.h"
#include "pyvariant.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "rastercoverage.h"
#include "coordinatesystem.h"
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
    void muteIssueLogger(); //(ilwis.h)
}

%include "object.h"

%include "engine.h"
%extend pythonapi::Engine {
%insert("python") %{
    def do(*args):
        if len(args) > 1:
            if len (args) > 2:
                if len (args) > 3:
                    if len (args) > 4:
                        if len (args) > 5:
                            if len (args) > 6:
                                if len (args) > 7:
                                    if len (args) > 8:
                                        obj = Engine__do(str(args[0]),str(args[1]),str(args[2]),str(args[3]),str(args[4]),str(args[5]),str(args[6]),str(args[7]),str(args[8]))
                                    else:
                                        obj = Engine__do(str(args[0]),str(args[1]),str(args[2]),str(args[3]),str(args[4]),str(args[5]),str(args[6]),str(args[7]))
                                else:
                                    obj = Engine__do(str(args[0]),str(args[1]),str(args[2]),str(args[3]),str(args[4]),str(args[5]),str(args[6]))
                            else:
                                obj = Engine__do(str(args[0]),str(args[1]),str(args[2]),str(args[3]),str(args[4]),str(args[5]))
                        else:
                            obj = Engine__do(str(args[0]),str(args[1]),str(args[2]),str(args[3]),str(args[4]))
                    else:
                        obj = Engine__do(str(args[0]),str(args[1]),str(args[2]),str(args[3]))
                else:
                    obj = Engine__do(str(args[0]),str(args[1]),str(args[2]))
            else:
                obj = Engine__do(str(args[0]),str(args[1]))
        else:
            raise IlwisError("no operation given!")
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

%include "ilwisobject.h"

%include "coverage.h"

%include "pyvariant.h"

%newobject pythonapi::Feature::attribute(const char*, int);//possibly no effect
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

%include "rastercoverage.h"

%include "coordinatesystem.h"
