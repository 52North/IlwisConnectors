
#include <QMap>
#include <QString>
#include <QXmlStreamReader>

#include "xmlparser.h"

XmlParser::XmlParser()
{
}

XmlParser::XmlParser(QXmlStreamReader *reader): _reader(reader)
{
}

QXmlStreamReader *XmlParser::reader() const
{
    return _reader;
}

void XmlParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
}

void XmlParser::setXmlReader(QXmlStreamReader *reader)
{
    _reader = reader;
}

bool XmlParser::startParsing(QString qName) const
{
    return !atEnd() && _reader->readNextStartElement() && isAtBeginningOf(qName);
}

bool XmlParser::hasError() const
{
    return _reader->hasError();
}

QXmlStreamAttributes XmlParser::attributes() const
{
    return _reader->attributes();
}

bool XmlParser::atEnd() const
{
    return _reader->atEnd();
}

QString XmlParser::readElementText() const
{
    return _reader->readElementText();
}


bool XmlParser::moveTo(QString qName, bool stepIn) const
{
    bool found = false;
    while ( !(_reader->atEnd() || isAtBeginningOf(qName, stepIn))) {
        _reader->skipCurrentElement();
        found  = _reader->readNextStartElement();
    }

    return found;
}


bool XmlParser::isAtBeginningOf(QString qName, bool stepIn) const
{

    if (_reader->atEnd() || !_reader->isStartElement()) {
        return false;
    }

    if (!isAtElement(qName)) {
        return false;
    }

    if (stepIn) {
        _reader->readNextStartElement();
        return true;
    } else {
        return true;
    }
}

bool XmlParser::isAtEndOf(QString qName) const
{
    if (_reader->atEnd() || !_reader->isEndElement()) {
        return false;
    }
    return isAtElement(qName);
}


bool XmlParser::isAtElement(QString qName) const
{
    QString prefix;
    int splitIndex = qName.indexOf(":");
    if (splitIndex > 0) {
        prefix = qName.left(splitIndex);
    }
    QString name = qName.mid(splitIndex + 1);
    if (_reader->namespaceProcessing()) {
        QString currentNamespace = _reader->namespaceUri().toString();
        QString declaredNamespace = _namespaces[prefix];
        if (currentNamespace == declaredNamespace) {
            return name == _reader->name().toString();
        } else {
            return false;
        }
    } else {
        if (_reader->qualifiedName().isEmpty()) {
            return _reader->name().toString() == qName;
        } else {
            return _reader->qualifiedName().toString() == qName;
        }
    }
}


