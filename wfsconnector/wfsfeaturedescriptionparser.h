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

    void parseSchemaDescription(ITable &table, QMap<QString,QString> &namespaceMappings) const;

private:
    XmlStreamParser *_parser;

    void parseNamespaces(QMap<QString,QString> &namespaceMappings) const;
    void parseFeatureProperties(ITable &table) const;
    void setDomainViaType(QString &type, IDomain &domain) const;
};

}
}
#endif // WFSFEATUREDESCRIPTIONPARSER_H
