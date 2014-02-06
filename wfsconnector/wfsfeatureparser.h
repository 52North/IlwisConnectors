#ifndef WFSFEATUREPARSER_H
#define WFSFEATUREPARSER_H

#include "wfsconnector_global.h"

namespace Ilwis {
namespace Wfs {

class WfsResponse;
class XmlStreamParser;

class WFSCONNECTORSHARED_EXPORT WfsFeatureParser
{
public:
    WfsFeatureParser(WfsResponse *response, ITable &table);
    ~WfsFeatureParser();

private:
    XmlStreamParser *_parser;
};

}
}
#endif // WFSFEATUREPARSER_H
