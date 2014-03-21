#ifndef PYTHONAPI_RANGE_H
#define PYTHONAPI_RANGE_H

#include "pythonapi_ilwis.h"

namespace Ilwis {
    class Range;
}
namespace pythonapi {
class Range
{
public:
    friend class Domain;

    bool isValid() const;
    std::string toString() const;
    IlwisTypes valueType() const;

    PyObject *ensure(const PyObject* v, bool inclusive = true) const;
    bool contains(const PyObject *value, bool inclusive = true) const;

    bool isContinuous() const;
    PyObject *impliedValue(const PyObject *value) const;

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
}

#endif // PYTHONAPI_RANGE_H
