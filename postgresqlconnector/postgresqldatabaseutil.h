#ifndef POSTGRESQLDATABASEUTIL_H
#define POSTGRESQLDATABASEUTIL_H

#include <QSqlDatabase>

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

        db.setHostName(url.host());
        db.setDatabaseName(url.path().remove('/'));
        db.setPort(url.port());

        db.setUserName(resource["pg.user"].toString());
        db.setPassword(resource["pg.password"].toString());
        return db;
    }
};

}
}
#endif // POSTGRESQLDATABASEUTIL_H
