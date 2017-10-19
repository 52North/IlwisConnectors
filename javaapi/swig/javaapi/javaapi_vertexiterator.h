#ifndef JAVAAPI_VERTEXITERATOR_H
#define JAVAAPI_VERTEXITERATOR_H

#include <memory>
#include "javaapi_util.h"

namespace Ilwis{
class VertexIterator;
}


namespace javaapi {

    class Geometry;

class VertexIterator
{
    friend class Feature;
public:
    VertexIterator();
    VertexIterator(const Geometry& geom);
    VertexIterator(const std::string& wkt);
    ~VertexIterator();

    bool __bool__() const;
    std::string __str__() const;
    VertexIterator* __iter__();
    Coordinate __next__();
    bool hasNext();

    VertexIterator operator+(int n);
    VertexIterator __radd__(int n);
    VertexIterator* operator+=(int n);

    VertexIterator operator-(int n);
    VertexIterator __rsub__(int n);
    VertexIterator* operator-=(int n);

    Coordinate __getitem__(quint32 linearpos);
    Coordinate current() const;

    int compareTo(const VertexIterator& other) const;

    bool nextSubGeometry() const;
    bool isInteriorVertex() const;

    VertexIterator begin();
    VertexIterator end();

protected:
    VertexIterator(Ilwis::VertexIterator* ilwIt);
    Ilwis::VertexIterator& ptr() const;
    std::shared_ptr<Ilwis::VertexIterator> _ilwisVertexIterator;
};
}
#endif // JAVAAPI_VERTEXITERATOR_H
