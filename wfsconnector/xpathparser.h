#ifndef XPATHPARSER_H
#define XPATHPARSER_H

#include <QXmlQuery>
#include <QXmlItem>
#include <QIODevice>
#include <QMapIterator>

#include "wfsconnector_global.h"

class QXmlItem;

class WFSCONNECTORSHARED_EXPORT XPathParser
{
public:
    XPathParser();
    XPathParser(QIODevice *device);
    ~XPathParser();

    QString createXPathNamespaceDeclarations(QMap<QString, QString> &mappings)
    {
        QString declarationString;
        QMapIterator<QString,QString> iterator(mappings);
        while(iterator.hasNext()) {
            iterator.next();
            QString declaration = QString("declare namespace %1 = \"%2\"; \n ").arg(iterator.key(), iterator.value());
            declarationString.push_back(declaration);
        }
        return declarationString;
    }


    QXmlQuery *queryFromRoot(QString query);
    QXmlQuery *queryRelativeFrom(QXmlItem &item, QString query);
    void addNamespaceMapping(QString prefix, QString ns);

private:
    QXmlQuery *_query;
    QIODevice *_iodevice;
    QMap<QString,QString> _namespaces;

};

#endif // XPATHPARSER_H
