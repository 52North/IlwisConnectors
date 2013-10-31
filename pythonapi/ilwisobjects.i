/* The ILWIS SWIG interface file*/

%module ilwisobjects

%include "exception.i"


%{
#include <assert.h>
static int myErr = 0;

#include "ilwis.h"
#include "ilwisobject.h"
#include "coverage.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
%}

%init %{
    pythonapi::initIlwisObjects();
%}

%include "ilwisobject.h"

%include "coverage.h"

%include "feature.h"

%include "featurecoverage.h"

%exception pythonapi::FeatureIterator::next {
  assert(!myErr);
  $action
  if (myErr) {
    myErr = 0; // clear flag for next time
    PyErr_SetString(PyExc_StopIteration, "End of iterator");
    return NULL;
  }
}

%include "featureiterator.h"

%extend pythonapi::FeatureIterator {
%insert("python") %{
    def __iter__():
        return self
%}
}

%extend pythonapi::FeatureCoverage {
%insert("python") %{
    def __iter__():
        return FeatureIterator(self)
%}
}

