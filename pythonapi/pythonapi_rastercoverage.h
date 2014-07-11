#ifndef PYTHONAPI_RASTERCOVERAGE_H
#define PYTHONAPI_RASTERCOVERAGE_H

#include "pythonapi_coverage.h"
#include "pythonapi_geometry.h"
#include "pythonapi_pixeliterator.h"
#include "pythonapi_georeference.h"

namespace Ilwis {
    class RasterCoverage;
    typedef IlwisData<RasterCoverage> IRasterCoverage;
}

namespace pythonapi {

    const quint32 WHOLE_RASTER = 200000;

    class DataDefinition;
    class Domain;

    class RasterCoverage : public Coverage{
         friend class PixelIterator;
         friend class Engine;
         friend class Catalog;
    private:
         RasterCoverage(Ilwis::IRasterCoverage* coverage);
    public:
        RasterCoverage();
        RasterCoverage(std::string resource);
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

        GeoReference geoReference();
        void setGeoReference(const GeoReference& gr);

        DataDefinition& datadef(quint32 layer = WHOLE_RASTER) const;
        void setDataDef(DataDefinition* datdef);
        void setDataDef(Domain& dm);

        NumericStatistics* statistics(int mode=0, int bins=0);

        static RasterCoverage* toRasterCoverage(Object *obj);

        PixelIterator begin();
        PixelIterator end();

        PixelIterator band(PyObject* pyTrackIndex);
        void addBand(PyObject* pyTrackIndex, PixelIterator* pyIter);

        RasterCoverage select(std::string selectionQ);
        RasterCoverage select(Geometry& geom);
    };

}
#endif // PYTHONAPI_RASTERCOVERAGE_H
