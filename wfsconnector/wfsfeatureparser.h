#ifndef WFSFEATUREPARSER_H
#define WFSFEATUREPARSER_H

#include "wfsconnector_global.h"

#include "geos/geom/Polygon.h"

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
     * @param context context info about the parsed WFS.
     */
    void parseFeatureMembers(WfsParsingContext &context);

private:
    XmlStreamParser *_parser;
    FeatureCoverage *_fcoverage;

    void parseFeature(std::vector<QVariant> &record, WfsParsingContext &context);

    QVariant fillStringColumn();
    QVariant fillDoubleColumn();
    QVariant fillDateTimeColumn();
    QVariant fillIntegerColumn();
    /**
     * Parses the feature's geometry from GML. <br/>
     * <br/>
     * Note that parsing GML has its limitations, so don't expect a full GML parsing engine
     * here, e.g. attributes are only read until a certain depth.
     *
     * @param context parsing context.
     */
    void parseFeatureGeometry(WfsParsingContext &context);

    void updateSrsInfo(WfsParsingContext &context);
    bool updateSrsInfoUntil(QString qname, WfsParsingContext &context);

    geos::geom::LineString *parseLineString(WfsParsingContext &context, bool &ok);
    geos::geom::Polygon *parsePolygon(WfsParsingContext &context, bool &ok);
    geos::geom::LinearRing *parseExteriorRing(WfsParsingContext &context);
    std::vector<geos::geom::Geometry *> *parseInteriorRings(WfsParsingContext &context);


    QString gmlPosListToWktCoords(QString gmlPosList, WfsParsingContext &context);
    QString gmlPosListToWktPolygon(QString gmlPosList, WfsParsingContext &context);
    QString gmlPosListToWktLineString(QString gmlPosList, WfsParsingContext &context);
    QString gmlPosListToWktPoint(QString gmlPosList, WfsParsingContext &context);

};

}
}
#endif // WFSFEATUREPARSER_H
