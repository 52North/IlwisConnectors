#ifndef WFSCAPABILITIESPARSER_H
#define WFSCAPABILITIESPARSER_H

#include "wfsconnector_global.h"

#include <QList>

class QUrl;
class QXmlItem;

namespace Ilwis {

class XPathParser;

namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCapabilitiesParser
{
public:
    WfsCapabilitiesParser(WfsResponse *response, const Resource wfsResource);
    ~WfsCapabilitiesParser();

    void parseFeatures(std::vector<Ilwis::Resource> &wfsFeaturesTypes);

private:
    Resource _wfsResource;
    XPathParser *_parser;

    void createGetFeatureUrl(const QString &featureType, QUrl &rawUrl, QUrl &normalizedUrl) const;
    void parseFeature(QXmlItem &item, WfsFeature &feature) const;

    QString valueOf(QXmlItem &item, const QString &xpathQuqery) const;
};
}
}

#endif // WFSCAPABILITIESPARSER_H
