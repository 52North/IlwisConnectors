#ifndef WcsUTILS_H
#define WcsUTILS_H

#include "kernel.h"

namespace Ilwis {
namespace Wcs {

class WcsUtils
{

public:
    static bool isValidWcsUrl(QUrl url)
    {
        QUrlQuery query(url);
        lowerCaseKeys(query);

        bool isHttpRequest = url.scheme().startsWith("http");
        bool isWcsRequest = isExpectedValue(query.queryItemValue("service"), "wcs");
        if(isHttpRequest && isWcsRequest) checkVersionCompatibility(query.queryItemValue("acceptversions"));
        return isHttpRequest && isWcsRequest;
    }

    static void lowerCaseKeys(QUrlQuery &query)
    {
        for (QPair<QString,QString> kvm : query.queryItems()) {
            query.removeQueryItem(kvm.first);
            query.addQueryItem(kvm.first.toLower(), kvm.second);
        }
    }

    static QString normalizeEpsgCode(QString epsgCode) {
        QString normalizedCode("epsg:");
        int splitIndex = epsgCode.lastIndexOf(":");
        if (splitIndex == -1) {
            splitIndex = epsgCode.lastIndexOf("/");
        }
        QString code = epsgCode.mid(splitIndex + 1);
        return normalizedCode.append(code.trimmed());
    }

    static QString getInternalNameFrom(QString name, quint64 id) {
        return QString("ilwis://internalcatalog/%1_%2").arg(name).arg(id);
    }

private:
    static void checkVersionCompatibility(QString kvpValueVersions) {
        QStringList versions = kvpValueVersions.split(",",QString::SkipEmptyParts);
        if (versions.isEmpty() || !versions.contains("1.1.1") || !versions.contains("1.0")) {
            WARN("Module supports Wcs 1.1.0 only and will set version parameter explicitly on each request!");
        }
    }

    /**
     * @brief WcsCatalogConnector::hasParameterValue checks if a certain parameter value is present.
     * @param actual the actual value to check.
     * @param expected the expected value.
     * @return true if the actual parameter is equal (incasesensitive) to the expected value.
     */
    static bool isExpectedValue(QString actual, QString expected)
    {
        if (expected != "" && actual == "") {
            return false;
        }
        return actual.compare(expected, Qt::CaseInsensitive) == 0;
    }

};

}
}

#endif // WcsUTILS_H
