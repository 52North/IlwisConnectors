#ifndef WFSFEATUREPARSER_H
#define WFSFEATUREPARSER_H

#include "wfsconnector_global.h"

class QString;
class XmlStreamParser;

namespace Ilwis {
namespace Wfs {

class WfsResponse;
class FillerColumnDef;

class WFSCONNECTORSHARED_EXPORT WfsFeatureParser
{
public:
    WfsFeatureParser(WfsResponse *response, FeatureCoverage *fcoverage);
    ~WfsFeatureParser();

    /**
     * Parses feature instances and add them to the feature coverage.<br/>
     * <br/>
     * Note that the feature's attribute table has to be constructed first to give
     * valuable results. This can be done by means of a WfsFeatureDescriptionParser
     * which uses the feature's schema description served by the WFS to create appropriate
     * Domain columns for each feature attribute.<br/>
     * <br/>
     * The parsed xml schema description may use different namespace mappings than the
     * feature collection. To keep track of the elements the WfsFeatureDescriptionParser
     * gives access to those used in the schema document.
     *
     * TODO: for now only the target namespace is being used ... propably this is enough
     *
     * @param wfsSchemaInfo meta info about the parsed WFS schema.
     */
    void parseFeatureMembers(WfsSchemaInfo &wfsSchemaInfo);

private:
    XmlStreamParser *_parser;
    FeatureCoverage *_fcoverage;

    void parseFeature(std::vector<QVariant> &record, QString geomAttributeName);

    QVariant fillStringColumn();
    QVariant fillDoubleColumn();
    QVariant fillDateTimeColumn();
    QVariant fillIntegerColumn();
    QVariant parseFeatureGeometry();

    void updateSrsInfo(QString &srsName, QString &srsDimension);
    bool updateSrsInfoUntil(QString qname, QString &srsName, QString srsDimension);
    geos::geom::Geometry *parsePolygon();
    geos::geom::LinearRing *parseExteriorRing();
    std::vector<geos::geom::Geometry *> *parseInteriorRings();

};

}
}
#endif // WFSFEATUREPARSER_H
