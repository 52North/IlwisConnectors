#include <QtDebug>
#include <QString>
#include <QStringBuilder>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "jsonconfig.h"

JsonConfig::JsonConfig() :
    _isValidSystem(false), _isValidUser(false)
{
}

bool JsonConfig::loadSystemConfig(const QString name)
{
    QString val;
    QFile file;
    file.setFileName(name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject geo = doc.object().value(QString("geoserver")).toObject();
    QJsonValue v;
    v = geo["host"]; _geo_host = v.toString();                       // f.e:  130.89.221.193, or geoserver.utwente.nl
    v = geo["port"]; _geo_port = QString("%1").arg(v.toDouble());    // f.e:  85
    v = geo["SRS"]; _geo_srs = v.toString();                         // f.e:  EPSG:21036
    v = geo["Xmin"]; _geo_Xmin = QString("%1").arg(v.toDouble());    // f.e:  696275.4
    v = geo["Xmax"]; _geo_Xmax = QString("%1").arg(v.toDouble());    // f.e:  813775.4
    v = geo["Ymin"]; _geo_Ymin = QString("%1").arg(v.toDouble());    // f.e:  9797373.37
    v = geo["Ymax"]; _geo_Ymax = QString("%1").arg(v.toDouble());    // f.e:  9885123.37
    v = geo["WMS_version"]; _wms_version = v.toString();             // f.e: 1.1.1, or 1.3.0 (needed for correct order of bbox)
    v = geo["width"]; _wms_width = QString("%1").arg(v.toDouble());  // f.e:  800
    v = geo["height"];_wms_height = QString("%1").arg(v.toDouble()); // f.e:  600

    _isValidSystem = true;
    return _isValidSystem;
}

bool JsonConfig::loadUserConfig(const QString name)
{
    QString val;
    QFile file;
    file.setFileName(name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject geo = doc.object().value(QString("geoserver")).toObject();
    _workspace = geo.value(QString("workspace")).toString();

    _isValidUser = true;
    return _isValidUser;
}

QString JsonConfig::getWMSGetMapURL(const QString layer)
{
    QString base = QString("http://") % _geo_host % QString(":") % _geo_port % QString("/geoserver/") % _workspace % QString("/wms");
    QString request = QString("service=WMS&version=%1&request=GetMap&").arg(_wms_version) %
                      QString("layers=%1:%2").arg(_workspace).arg(layer) %
                      QString("styles=&bbox=%1").arg(getBBox()) %
                      QString("width=%1&height=%2").arg(_wms_width).arg(_wms_height) %
                      QString("srs=%1").arg(_geo_srs) %
                      QString("format=application/openlayers");

    return base % QString("?") % request;

}

QString JsonConfig::getBBox()
{
    return QString();
}
