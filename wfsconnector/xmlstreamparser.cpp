
#include <QMap>
#include <QString>
#include <QXmlStreamReader>

#include "xmlstreamparser.h"

XmlStreamParser::XmlStreamParser()
{
}

XmlStreamParser::XmlStreamParser(QXmlStreamReader *reader): _reader(reader)
{
}

XmlStreamParser::XmlStreamParser(QIODevice *device)
{
    _reader = new QXmlStreamReader(device);
}

void XmlStreamParser::setXmlReader(QXmlStreamReader *reader)
{
    _reader = reader;
}

QXmlStreamReader *XmlStreamParser::reader() const
{
    return _reader;
}

void XmlStreamParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
}

bool XmlStreamParser::startParsing(QString qName) const
{
    return !atEnd() && _reader->readNextStartElement() && isAtBeginningOf(qName);
}

bool XmlStreamParser::hasError() const
{
    return _reader->hasError();
}

QXmlStreamAttributes XmlStreamParser::attributes() const
{
    return _reader->attributes();
}

//QString XmlStreamParser::attribue(QString qName) const
//{
//    if (hasAttribute(qName)) {

//    } else {
//        return "";
//    }
//}

//bool XmlStreamParser::hasAttribute(QString qName) const
//{
//    foreach (QXmlStreamAttribute attribute, attributes()) {

//    }
//}

bool XmlStreamParser::atEnd() const
{
    return _reader->atEnd();
}

QString XmlStreamParser::readElementText() const
{
    return _reader->readElementText();
}


bool XmlStreamParser::moveTo(QString qName, bool stepIn) const
{
    bool found = false;
    while ( !(_reader->atEnd() || isAtBeginningOf(qName, stepIn))) {
        _reader->skipCurrentElement();
        found  = _reader->readNextStartElement();
    }
    return found;
}


bool XmlStreamParser::isAtBeginningOf(QString qName, bool stepIn) const
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

bool XmlStreamParser::isAtEndOf(QString qName) const
{
    if (_reader->atEnd() || !_reader->isEndElement()) {
        return false;
    }
    return isAtElement(qName);
}


bool XmlStreamParser::isAtElement(QString qName) const
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


