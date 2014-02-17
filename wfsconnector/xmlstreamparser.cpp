
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

QString XmlStreamParser::getPrefixForNamespaceUri(QString namespaceUri) const
{
    _namespaces.key(namespaceUri);
}

bool XmlStreamParser::startParsing(QString qName) const
{
    if ( !_reader->isStartDocument()) {
      _reader->readNext();
    }
    _reader->readNextStartElement();
    return isAtBeginningOf(qName);
}

bool XmlStreamParser::hasError() const
{
    return _reader->hasError();
}

QXmlStreamAttributes XmlStreamParser::attributes() const
{
    return _reader->attributes();
}

bool XmlStreamParser::atEnd() const
{
    return _reader->atEnd();
}

QString XmlStreamParser::readElementText() const
{
    return _reader->readElementText();
}

QString XmlStreamParser::name() const
{
    return _reader->name().toString();
}

QString XmlStreamParser::namespaceUri() const
{
    return _reader->namespaceUri().toString();
}

QString XmlStreamParser::qname() const
{
    return _reader->qualifiedName().toString();
}

bool XmlStreamParser::readNextStartElement() const
{
    if (_reader->isEndElement()) {
        _reader->readNext();
    }
    return _reader->readNextStartElement();
}

bool XmlStreamParser::moveToNext(QString qName) const
{
    if (_reader->atEnd()) {
        return false;
    }

    bool found = false;
    _reader->readNextStartElement();
    while ( !(_reader->atEnd() || found)) {
        if (_reader->isStartElement()) {
            found = isAtBeginningOf(qName);
            if ( !found) {
                _reader->skipCurrentElement();
                _reader->readNext();
            }
        } else {
            if ( !_reader->readNextStartElement()) {
                break;
            }
        }
    }
    return found;
}

bool XmlStreamParser::isAtBeginningOf(QString qName) const
{
    if (_reader->atEnd() || !_reader->isStartElement()) {
        return false;
    }
    return isAtElement(qName);
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

