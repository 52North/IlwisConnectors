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
    WfsFeatureDescriptionParser(WfsResponse *response, FeatureCoverage *fcoverage);
    ~WfsFeatureDescriptionParser();

    void parseSchemaDescription(ITable &table, QMap<QString,QString> &namespaceMappings) const;

private:
    FeatureCoverage *_fcoverage;
    XmlStreamParser *_parser;

    void parseNamespaces(QMap<QString,QString> &namespaceMappings) const;
    void parseFeatureProperties(ITable &table) const;
};

}
}
#endif // WFSFEATUREDESCRIPTIONPARSER_H
