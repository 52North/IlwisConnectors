#ifndef WFSFEATUREPARSER_H
#define WFSFEATUREPARSER_H

#include "wfsconnector_global.h"

class QString;
class XmlStreamParser;

namespace Ilwis {
namespace Wfs {

class WfsResponse;

class WFSCONNECTORSHARED_EXPORT WfsFeatureParser
{
public:
    WfsFeatureParser(WfsResponse *response);
    ~WfsFeatureParser();

    void parseFeature(ITable &table, QMap<QString,QString> &mappings) const;

private:
    XmlStreamParser *_parser;

    void parseFeatureAttribute(quint64 id, QString attribute) const;
};

}
}
#endif // WFSFEATUREPARSER_H
