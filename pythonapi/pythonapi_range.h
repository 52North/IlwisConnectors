#ifndef PYTHONAPI_RANGE_H
#define PYTHONAPI_RANGE_H

#include "pythonapi_object.h"
#include<string>
#include<map>
#include "pythonapi_rangeiterator.h"

namespace Ilwis {
class Range;
class NumericRange;
}

typedef struct _object PyObject;

namespace pythonapi {

class Range: public Object{
    friend class Domain;
    friend class DataDefinition;
    friend class ItemDomain;
    template<typename OutputType, typename RangeType, typename IlwOutput,  typename IlwRange>
    friend class RangeIterator;
    friend class Interval;

public:
    bool __bool__() const;
    std::string __str__();
    IlwisTypes ilwisType();

    IlwisTypes valueType() const;
    PyObject* ensure(const PyObject* v, bool inclusive = true) const;
    bool contains(const PyObject *value, bool inclusive = true) const;

    bool isContinuous() const;
    PyObject* impliedValue(const PyObject *value) const;

protected:
    std::shared_ptr<Ilwis::Range> _range;
    Range(Ilwis::Range *rng);

    Range();
    virtual ~Range();
private:
};

class NumericRange : public Range {

public:
    NumericRange(double mi, double ma, double resolution = 0);
    NumericRange(const NumericRange &vr);
    NumericRange(Ilwis::NumericRange* nr);
    ~NumericRange();

    bool contains(double v, bool inclusive = true) const;
    double max() const;
    void setMax(double v);
    double min() const;
    void setMin(double v);
    double distance() const;

    void setResolution(double resolution);
    double resolution() const ;

    void set(const NumericRange& vr);

    pythonapi::NumericRangeIterator __iter__();
    pythonapi::NumericRangeIterator begin();
    pythonapi::NumericRangeIterator end();

    void clear();
};

class ItemRange : public Range {
public:
    virtual void add(PyObject *dItem) = 0;
    void count();
    void remove(const std::string& name);
    void clear();
};

class NumericItemRange : public ItemRange{
public:
    NumericItemRange();
    void add(std::string name, double min, double max, double resolution=0);
    void add(PyObject *item);
    double index(double);
    qint32 gotoIndex(qint32 index, qint32 step) const;
    NumericItemRange* clone();
};

class IndexedItemRange : public ItemRange{
public:
    IndexedItemRange();
    void add(PyObject* item);
    qint32 gotoIndex(qint32 index, qint32 step) const;
    IndexedItemRange* clone();
};

class NamedItemRange : public ItemRange {
public:
    NamedItemRange();
    void add(PyObject *item);
    qint32 gotoIndex(qint32 index, qint32 step) const;
    NamedItemRange* clone();
};

class ThematicRange : public ItemRange {
public:
    ThematicRange();
    void add(std::string name, std::string id="", std::string descr="");
    void add(PyObject *item);
    ThematicRange* clone();
};

#ifdef SWIG
%rename(ColorModel) ColorModelNS;
#endif

struct ColorModelNS{
    enum Value{cmNONE, cmRGBA, cmHSLA, cmCYMKA, cmGREYSCALE};
};

typedef ColorModelNS::Value ColorModel;

class Color{
public:
    Color();
    Color(ColorModel type, PyObject* obj);
    double getItem(std::string key) const;
    ColorModel getColorModel() const;
    std::string toString() const;
private:
    void readColor(ColorModel type, PyObject* obj);
    ColorModel _type = ColorModel::cmRGBA;
    PyObject* _colorVal;
};


class ColorRange : public Range{
public:

    ColorRange();
    ColorRange(IlwisTypes tp, ColorModel clrmodel);
    ColorModel defaultColorModel() const;
    void defaultColorModel(ColorModel m);

    static Color toColor(quint64 clrint, ColorModel clrModel) ;
    static Color toColor(PyObject*, ColorModel colortype);
    std::string toString(const Color &clr, ColorModel clrType);
    ColorModel stringToColorModel(std::string clrmd);

private:
    IlwisTypes _valuetype;
    ColorModel _defaultModel = ColorModel::cmRGBA;
};

class ContinousColorRange : public ColorRange{
public:
    ContinousColorRange();
    ContinousColorRange(const Color& clr1, const Color& clr2);
    std::string __str__() const;
    bool __bool__() const;
    ContinousColorRange *clone() const;
    PyObject* ensure(const PyObject *v, bool inclusive = true) const;
    bool containsVar(const PyObject *v, bool inclusive = true) const;
    bool containsColor(const Color &clr, bool inclusive = true) const;
    bool containsRange(ColorRange *v, bool inclusive = true) const;
    Color impliedValue(const PyObject* v) const;
};

class TimeInterval : public NumericRange{
public:
    TimeInterval(IlwisTypes tp = itUNKNOWN);
    TimeInterval(const PyObject *beg, const PyObject *end, std::string step="", IlwisTypes tp = itUNKNOWN);

    //TimeInterval& operator=(const TimeInterval& tiv);
    PyObject* begin() const;
    PyObject* end() const ;
    void begin(const PyObject* t) ;
    void end(const PyObject* t);
    //Duration getStep() const { return _step;}
    std::string toString(bool local, IlwisTypes) const;
    bool contains(const std::string& value, bool inclusive = true) const;
    bool contains(const PyObject* value, bool inclusive = true) const;

    Ilwis::Range *clone() const ;
    bool isValid() const;
};

}

#endif // PYTHONAPI_RANGE_H
