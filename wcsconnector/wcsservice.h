#ifndef WCSSERVICE_H
#define WCSSERVICE_H

namespace Ilwis {
class XPathParser;

namespace Wcs {

typedef std::unique_ptr<XPathParser> UPXPathParser;

class WcsService
{
public:
    WcsService();
    WcsService(WcsResponse *response, QUrl wcsUrl);
    virtual ~WcsService();

protected:
    QString query(const QString &id) const;
    QString valueOf(QXmlItem &item, const QString &xpathQuqery) const;
    const UPXPathParser &parser() const;
    QString currentVersion() const;

private:
    std::map<QString, std::map<QString, QString>> _queries;
    QString _currentVersion = "1.1.0";
    UPXPathParser _parser;
    QUrl _url;
};

}
}

#endif // WCSSERVICE_H
