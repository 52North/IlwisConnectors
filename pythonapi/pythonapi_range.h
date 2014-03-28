#ifndef PYTHONAPI_RANGE_H
#define PYTHONAPI_RANGE_H

#include "pythonapi_object.h"

#include<string>

namespace Ilwis {
class Range;
}

typedef struct _object PyObject;

namespace pythonapi {

class Range: public Object{
public:
    friend class Domain;

    bool __bool__() const;
    std::string __str__();
    IlwisTypes valueType();

    PyObject* ensure(const PyObject* v, bool inclusive = true) const;
    bool contains(const PyObject *value, bool inclusive = true) const;

    bool isContinuous() const;
    PyObject* impliedValue(const PyObject *value) const;

protected:
    std::shared_ptr<Ilwis::Range> _range;
    Range(Ilwis::Range *rng);

    Range();
private:
};

class NumericRange : public Range {
public:
    NumericRange(double mi, double ma, double resolution=0);
    NumericRange(const NumericRange &vr);


    bool contains(double v, bool inclusive = true) const;
    double max() const;
    void setMax(double v);
    double min() const;
    void setMin(double v);
    double distance() const;

    void setResolution(double resolution);
    double resolution() const ;

    void set(const NumericRange& vr);
    void clear();
};

class ItemRange : public Range {
public:
    virtual void add(PyObject *dItem) = 0;
    void remove(const std::string& name);
    void clear();
};

class NumericItemRange : public ItemRange{

    void add(PyObject *item);
};

class NamedItemRange : public ItemRange {
    void add(PyObject *item);
};

class ThematicRange : public ItemRange {
    void add(PyObject *item);
};
}

#endif // PYTHONAPI_RANGE_H
