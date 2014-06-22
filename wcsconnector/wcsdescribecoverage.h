#ifndef WCSDESCRIBECOVERAGE_H
#define WCSDESCRIBECOVERAGE_H

namespace Ilwis {
namespace Wcs {
class WcsDescribeCoverage : public WcsService
{
public:
    WcsDescribeCoverage();

    WcsDescribeCoverage(WcsResponse *response, QUrl wcsUrl);
    ~WcsDescribeCoverage();

    void parse(std::map<QString, Ilwis::Resource> &rastercoverages) const;


private:
    void parseCoverage(QXmlItem &item, std::map<QString, Ilwis::Resource> &rastercoverages) const;
    QString valueOf(QXmlItem &item, const QString &xpathQuqery) const;
};
}
}

#endif // WCSDESCRIBECOVERAGE_H
