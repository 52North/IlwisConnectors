#ifndef WFSFEATUREDESCRIPTIONPARSER_H
#define WFSFEATUREDESCRIPTIONPARSER_H

#include "wfsconnector_global.h"

class QUrl;
class QString;
class XmlStreamParser;

namespace Ilwis {
namespace Wfs {

class WfsResponse;

class WFSCONNECTORSHARED_EXPORT WfsFeatureDescriptionParser
{
public:
    WfsFeatureDescriptionParser();
    WfsFeatureDescriptionParser(WfsResponse *response);
    ~WfsFeatureDescriptionParser();

    bool parseSchemaDescription(FeatureCoverage *fcoverage, QMap<QString,QString> &namespaceMappings);

private:
    XmlStreamParser *_parser;
    IlwisTypes _coverageType;

    void parseNamespaces(QMap<QString,QString> &namespaceMappings);
    void parseFeatureProperties(ITable &table);

    /**
     * Initiates the domain according to the given xml schema type.
     *
     * @param type the schema type.
     * @param domain the (empty) domain to initiate.
     * @return true if domain could be initiated, false otherwise.
     */
    bool initDomainViaType(QString &type, IDomain &domain);

    bool isPolygonType() const;
    bool isLineType() const;
    bool isPointType() const;
};

}
}
#endif // WFSFEATUREDESCRIPTIONPARSER_H