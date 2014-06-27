#include <QUrl>
#include "kernel.h"
#include "wcsresponse.h"
#include "xpathparser.h"
#include "wcsservice.h"

using namespace Ilwis;
using namespace Wcs;

WcsService::WcsService()
{
}

WcsService::WcsService(WcsResponse *response, QUrl wcsUrl) : _url(wcsUrl)
{
    _parser.reset(new XPathParser(response->device()));
    _parser->addNamespaceMapping("wcs", "http://www.opengis.net/wcs/1.1.1");
    _parser->addNamespaceMapping("ows", "http://www.opengis.net/ows/1.1");
    _parser->addNamespaceMapping("xlink", "http://www.w3.org/1999/xlink");
}

Ilwis::Wcs::WcsService::~WcsService()
{

}
