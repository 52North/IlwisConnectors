#ifndef JSONCONFIG_H
#define JSONCONFIG_H

class JsonConfig
{
    bool _isValidSystem;
    bool _isValidUser;

    // System config:
    // members for WMS GetMap query
    QString _geo_host;
    QString _geo_port;
    QString _geo_srs;
    QString _geo_Xmin;
    QString _geo_Xmax;
    QString _geo_Ymin;
    QString _geo_Ymax;
    QString _wms_version;
    QString _wms_width;
    QString _wms_height;

    // User config:
    QString _workspace;

public:
    JsonConfig();

    bool isValid() { return _isValidSystem && _isValidUser; }

    bool loadSystemConfig(const QString name);
    bool loadUserConfig(const QString name);

    QString getWMSGetMapURL(const QString layer);
    QString getBBox();
//    void store();
};

#endif // JSONCONFIG_H
