#ifndef WFSFEATUREDESCRIPTIONPARSER_H
#define WFSFEATUREDESCRIPTIONPARSER_H

#include "wfsconnector_global.h"

class QUrl;

namespace Ilwis {
namespace Wfs {

class WfsResponse;
class XmlStreamParser;

class WFSCONNECTORSHARED_EXPORT WfsFeatureDescriptionParser
{
public:
    WfsFeatureDescriptionParser();
    WfsFeatureDescriptionParser(WfsResponse *response, QUrl &url);
    ~WfsFeatureDescriptionParser();

    void parseSchemaDescription(ITable &table);

private:
    QUrl _url;
    XmlStreamParser *_parser;
};

}
}
#endif // WFSFEATUREDESCRIPTIONPARSER_H
