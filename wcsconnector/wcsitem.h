#ifndef WcsItem_H
#define WcsItem_H

#include "kernel.h"
#include "geometries.h"

#include "identity.h"
#include "resource.h"

class QUrl;
class QString;

namespace Ilwis {
namespace Wcs {

class WcsItem : public Resource
{
public:
    WcsItem();
    WcsItem(const QUrl& rawfeatureUrl, const QUrl& normalizedUrl=QUrl());
    ~WcsItem();

    QString title() const;
    void setTitle(QString title="");

    QString abstract() const;
    void setAbstract(QString abstract="");

    Envelope bbox() const;
    void setBBox(Envelope &bbox);

private:
    QString _title;
    QString _abstract;
    Envelope _bbox;
};


}
}
#endif // WcsItem_H
