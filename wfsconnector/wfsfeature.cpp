#include <QString>
#include <QUrlQuery>

#include "identity.h"
#include "resource.h"
#include "wfsfeature.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeature::WfsFeature()
{
}

WfsFeature::WfsFeature(QUrl featureUrl): Resource(featureUrl, itFEATURE)
{
}

WfsFeature::~WfsFeature()
{
}

QUrl WfsFeature::getFeatureUrl()
{
    return url();
}

QString WfsFeature::title() const
{
    return _title;
}

void WfsFeature::setTitle(QString title)
{
    _title = title;
}

QString WfsFeature::abstract() const
{
    return _abstract;
}

void WfsFeature::setAbstract(QString abstract)
{
    _abstract = abstract;
}

Envelope WfsFeature::bbox() const
{
    return _bbox;
}

void WfsFeature::setBBox(Envelope &bbox)
{
    _bbox = bbox;
}
