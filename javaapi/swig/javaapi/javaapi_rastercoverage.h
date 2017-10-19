#ifndef JAVAAPI_RASTERCOVERAGE_H
#define JAVAAPI_RASTERCOVERAGE_H

#include "javaapi_coverage.h"
#include "javaapi_geometry.h"
#include "javaapi_pixeliterator.h"
#include "javaapi_georeference.h"

namespace Ilwis {
    class RasterCoverage;
    typedef IlwisData<RasterCoverage> IRasterCoverage;
}

namespace javaapi {

    class DataDefinition;
    class Domain;

    class RasterCoverage : public Coverage{
         friend class PixelIterator;
         friend class Engine;
         friend class Catalog;
    private:
         RasterCoverage(Ilwis::IRasterCoverage* coverage);
         //QVariant resolveIndex(PyObject* pyTrackIndex);
         std::string dateToString(int datepart);
    public:
        RasterCoverage();
        RasterCoverage(const std::string& resource);
        ~RasterCoverage();
        RasterCoverage* operator+ (RasterCoverage &rc);
        RasterCoverage* operator+ (double value);
        RasterCoverage* __radd__(double value);
        RasterCoverage* operator-(RasterCoverage &rc);
        RasterCoverage* operator- (double value);
        RasterCoverage* __rsub__(double value);
        RasterCoverage* operator* (RasterCoverage &rc);
        RasterCoverage* operator* (double value);
        RasterCoverage* __rmul__(double value);
        RasterCoverage* __truediv__ (RasterCoverage &rc);
        RasterCoverage* __truediv__ (double value);
        RasterCoverage* __rtruediv__(double value);
        //RasterCoverage* sin();
        PixelIterator __iter__();
        double coord2value(const Coordinate& c);
        double pix2value(const Pixel& pix);
        double pix2value(const PixelD& pix);

        Size size();
        void setSize(const Size& sz);
        void unloadBinary();

        CoordinateSystem coordinateSystem();

        GeoReference geoReference();
        void setGeoReference(const GeoReference& gr);

        DataDefinition& datadef() const;
        void setDataDef(DataDefinition* datdef);
        void setDataDef(Domain& dm);

        NumericStatistics* statistics(int mode=0, int bins=0);

        static RasterCoverage* toRasterCoverage(IObject *obj);

        PixelIterator begin();
        PixelIterator end();

        PixelIterator band(double index);
        PixelIterator band(const std::string& index);
        /*PixelIterator band(PyObject* pyTrackIndex);
        void addBand(PyObject* pyTrackIndex, PixelIterator* pyIter);
        void setBandDefinition(PyObject* pyTrackIndex, const DataDefinition& datdef);
        */
        //void setSubDefinition(const Domain& dom, PyObject* items);
        quint32 indexOf(const std::string& variantId) const;
        quint32 indexOf(double domainItem) const;
        //quint32 indexOf(PyObject* obj) const;
        std::string atIndex(quint32 idx) const;
        //PyObject* indexes() const;
        quint32 countSubs() const;
        Domain subDomain() const;
        void clear();

        RasterCoverage select(std::string selectionQ);
        RasterCoverage select(Geometry& geom);
        RasterCoverage* reprojectRaster(std::string newName, quint32 epsg, std::string interpol);

        RasterCoverage* clone();

        javaapi::Envelope envelope();
    };

}
#endif // JAVAAPI_RASTERCOVERAGE_H
