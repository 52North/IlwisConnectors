#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "wfsconnector_global.h"

class QXmlStreamReader;
class QXmlStreamAttributes;

class WFSCONNECTORSHARED_EXPORT XmlParser
{
public:
    XmlParser();
    XmlParser(QXmlStreamReader *reader);

    QXmlStreamReader *reader() const;
    void setXmlReader(QXmlStreamReader *reader);
    void addNamespaceMapping(QString prefix, QString ns);

    bool startParsing(QString qName) const;
    bool hasError() const;
    bool atEnd() const;

    QXmlStreamAttributes attributes() const;
    QString readElementText() const;

    /**
     * Moves to the element living on current level, named by qName, and steps in directly by default.
     *
     * @param qName the name of the element to move to.
     * @param stepIn if to step in the element if found.
     * @return true if element was found, false if element was not found.
     */
    bool moveTo(QString qName, bool stepIn=true) const;
    bool isAtBeginningOf(QString qName, bool stepIn=true) const;
    bool isAtEndOf(QString qName) const;


private:
    QXmlStreamReader *_reader;
    QMap<QString,QString> _namespaces;

    bool isAtElement(QString qName) const;
};

#endif // XMLPARSER_H
