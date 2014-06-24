#ifndef WCSSERVICE_H
#define WCSSERVICE_H

namespace Ilwis {
namespace Wcs {

class WcsService
{
public:
    WcsService();
    WcsService(WcsResponse *response, QUrl wcsUrl);
    virtual ~WcsService();

protected:
    QUrl _url;
    std::unique_ptr<XPathParser> _parser;
};
}
}

#endif // WCSSERVICE_H
