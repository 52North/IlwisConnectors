#ifndef JAVAAPI_RANGE_H
#define JAVAAPI_RANGE_H

#include "javaapi_object.h"
#include<string>
#include<map>
#include "javaapi_rangeiterator.h"
#include <QSharedPointer>
#include "javaapi_util.h"

namespace Ilwis {
class Range;
class NumericRange;
class DomainItem;
typedef QSharedPointer<DomainItem> SPDomainItem;
}

namespace javaapi {

class Range: public IObject{
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
    QVariant* ensure(const QVariant* v, bool inclusive = true) const;
    bool contains(const QVariant* value, bool inclusive = true) const;

    bool isContinuous() const;
    QVariant* impliedValue(const QVariant* value) const;

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
    NumericRange();
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

    javaapi::NumericRangeIterator __iter__();
    javaapi::NumericRangeIterator begin();
    javaapi::NumericRangeIterator end();

    void clear();
};

class ItemRange : public virtual Range {
public:
    //virtual void add(const std::string &name, double min, double max, double resolution=0) = 0;
    quint32 count();
    void remove(const std::string& name);
    void clear();
};

class NumericItemRange : public ItemRange{
public:
    NumericItemRange();
    void add(const std::string &name, double min, double max, double resolution=0);
    std::vector<std::vector<std::string> > listAll();
    double index(double);
    qint32 gotoIndex(qint32 index, qint32 step) const;
    NumericItemRange* clone();
};

/*class IndexedItemRange : public ItemRange{
public:
    IndexedItemRange();
    //void add(QVariant* item);
    void add(const std::string &label);
    void add(const std::string &label, int index);
    void add(const std::string &label, int index, int count);
    qint32 gotoIndex(qint32 index, qint32 step) const;
    IndexedItemRange* clone();
};*/

class NamedItemRange : public ItemRange {
public:
    NamedItemRange();
    void add(const std::string& item);
    std::vector<std::string> listAll();
    qint32 gotoIndex(qint32 index, qint32 step) const;
    NamedItemRange* clone();
};

class ThematicRange : public ItemRange {
public:
    ThematicRange();
    void add(const std::string &name, const std::string &id="", const std::string &descr="");
    std::vector<std::vector<std::string> > listAll();
    ThematicRange* clone();
};


class ColorRange : public virtual Range{
    friend class ColorDomain;
    friend class ItemDomain;

public:
    ColorRange();
    ColorRange(IlwisTypes tp, ColorModel clrmodel);
    ColorModel defaultColorModel() const;
    void defaultColorModel(ColorModel m);

    static Color toColor(quint64 clrint, ColorModel clrModel, const std::string& name = "") ;
    static Color toColor(const std::vector<double> &v, ColorModel colortype, const std::string& name = "");
    static Color toColor(const std::string &v, ColorModel colortype, const std::string& name = "");

protected:
    Color qColorToColor(QColor qCol, const std::string& name = "") const;
    QColor colorToQColor(const Color& pyCol) const;

private:
    std::string toString(const Color &clr, ColorModel clrType);
    ColorModel stringToColorModel(std::string clrmd);
    IlwisTypes _valuetype;
    ColorModel _defaultModel = ColorModel::cmRGBA;
};

class ContinuousColorRange : public ColorRange{
public:
    ContinuousColorRange();
    ContinuousColorRange(const Color& clr1, const Color& clr2);
    ContinuousColorRange *clone() const;
    QVariant* ensure(const QVariant* v, bool inclusive = true) const;
    bool containsVar(const QVariant* v, bool inclusive = true) const;
    bool containsColor(const Color &clr, bool inclusive = true) const;
    bool containsRange(ColorRange *v, bool inclusive = true) const;
    //Color impliedValue(const QVariant* v) const;
};

class ColorPalette : public ItemRange, public ColorRange{
public:

    ColorPalette();

    Color item(quint32 raw) const;
    Color item(const std::string& name) const;
    Color itemByOrder(quint32 index) const;

    Color color(int index);

    void add(const Color& pyColor);
    void remove(const std::string& name);
    void clear();

    bool containsColor(const Color &clr, bool inclusive = true) const;
    bool containsRange(ColorRange *v, bool inclusive = true) const;

    quint32 count();

    Color valueAt(quint32 index, ItemRange* rng);
    qint32 gotoIndex(qint32 index, qint32 step) const;

private:
    void add(QVariant* item);
    Color itemToColor(Ilwis::SPDomainItem item) const;
    std::shared_ptr<Ilwis::Range> _rangeColor = javaapi::ColorRange::_range;
    std::shared_ptr<Ilwis::Range> _rangeItem = javaapi::ItemRange::_range;
};
/*
class TimeInterval : public NumericRange{
public:
    TimeInterval(IlwisTypes tp = itUNKNOWN);
    TimeInterval(const QVariant* beg, const QVariant* end, std::string step="", IlwisTypes tp = itUNKNOWN);

    //TimeInterval& operator=(const TimeInterval& tiv);
    QVariant* begin() const;
    QVariant* end() const ;
    void begin(const QVariant* t) ;
    void end(const QVariant* t);
    //Duration getStep() const { return _step;}
    bool contains(const std::string& value, bool inclusive = true) const;
    bool contains(const QVariant* value, bool inclusive = true) const;

    TimeInterval* clone() const ;

protected:
    TimeInterval(Ilwis::Range* ti);
};
*/
}

#endif // JAVAAPI_RANGE_H
