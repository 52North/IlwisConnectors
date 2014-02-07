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

    void setNamespaceMappings(QMap<QString,QString> &mappings);
    void parseFeature(ITable &table);

private:
    XmlStreamParser *_parser;
};

}
}
#endif // WFSFEATUREPARSER_H
