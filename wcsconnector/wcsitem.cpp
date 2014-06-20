#include <QString>
#include <QUrlQuery>

#include "identity.h"
#include "resource.h"
#include "wcsitem.h"

using namespace Ilwis;
using namespace Wcs;

WcsItem::WcsItem()
{
}

WcsItem::WcsItem(const QUrl &rawfeatureUrl, const QUrl &normalizedUrl): Resource(normalizedUrl, rawfeatureUrl, itFEATURE)
{
    if ( !normalizedUrl.isValid()){
        if ( rawfeatureUrl.hasQuery()) {
            QString normUrl = rawfeatureUrl.toString(QUrl::RemoveQuery);
            QUrlQuery query(rawfeatureUrl);
            if ( query.hasQueryItem("typeName")){
                QString tpName = query.queryItemValue("typeName");
                setUrl(normUrl + "/" + tpName);

            }
        }
    }
}

WcsItem::~WcsItem()
{
}

QString WcsItem::title() const
{
    return _title;
}

void WcsItem::setTitle(QString title)
{
    _title = title;
}

QString WcsItem::abstract() const
{
    return _abstract;
}

void WcsItem::setAbstract(QString abstract)
{
    _abstract = abstract;
}

Envelope WcsItem::bbox() const
{
    return _bbox;
}

void WcsItem::setBBox(Envelope &bbox)
{
    _bbox = bbox;
}
