/* The ILWIS SWIG interface file*/

%module ilwisobjects

%include "exception.i"


%{
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

