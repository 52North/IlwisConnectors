#ifndef POSTGRESQLDATABASEUTIL_H
#define POSTGRESQLDATABASEUTIL_H

#include <QSqlDatabase>

#include "kernel.h"
#include "resource.h"

namespace Ilwis {
namespace Postgresql {


class PostgresqlDatabaseUtil {

public:

    static QSqlDatabase connectionFromResource(const Resource resource, QString connectionname="") {

        QUrl url = resource.url();
        if (connectionname.isEmpty()) {
            connectionname = resource.url().toString();
        }
        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionname);

        qint64 port = url.port();
        QString host = url.host();
        QString path = url.path().remove('/');
        validateNotNullOrEmpty("Host", host);
        validateNotNullOrEmpty("Path", path);
        validateNotNullOrEmpty("Port", port);

        db.setHostName(host);
        db.setDatabaseName(path);
        db.setPort(port);

        QString username = resource["pg.user"].toString();
        QString password = resource["pg.password"].toString();
        validateNotNullOrEmpty("Username", username);
        validateNotNullOrEmpty("Password", password);

        db.setUserName(username);
        db.setPassword(password);
        return db;
    }

private:
    static void validateNotNullOrEmpty(QString parameter, QVariant value) {
        if (value.isNull() || !value.isValid()) {
            WARN1("Property '%1' is null or empty.", parameter);
        }
    }
};

}
}
#endif // POSTGRESQLDATABASEUTIL_H
