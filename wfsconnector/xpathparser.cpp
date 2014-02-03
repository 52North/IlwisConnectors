
#include <QXmlQuery>

#include "xpathparser.h"

XPathParser::XPathParser()
{

}

XPathParser::XPathParser(QIODevice *device): _iodevice(device)
{
}


XPathParser::~XPathParser()
{
    delete _query;
}

QXmlQuery *XPathParser::parseAbsolute(QString query)
{
    _query = new QXmlQuery;
    QString xPath(createXPathNamespaceDeclarations(_namespaces));
    xPath.append("doc($xml)").append(query);
    _query->bindVariable("xml", _iodevice);
    _query->setQuery(xPath);
    return _query;
}

QXmlQuery *XPathParser::parseRelativeFrom(QXmlItem &item, QString query)
{
    _query->setFocus(item);
    QString xPath(createXPathNamespaceDeclarations(_namespaces));
    xPath.append(query);
    _query->setQuery(xPath);
    return _query;
}

void XPathParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
}
