#ifndef INIFILE_H
#define INIFILE_H

#include <QHash>
#include <QString>
#include <QFileInfo>
#include <map>

using namespace std;

namespace Ilwis {

typedef QHash<QString, QString> SectionEntries;
typedef QHash<QString, SectionEntries> Sections;

class IniFile
{
public:
    IniFile();
    bool setIniFile(const QUrl& fn, const UPContainerConnector& container, bool loadfile=true);
    virtual ~IniFile();

    void setKeyValue(const QString& section, const QString& key, const QString& value);
    QString value(const QString& section, const QString& key) const;

    void removeKeyValue(const QString& section, const QString& key);
    void removeSection(const QString& section);
    QString file()const;
    QStringList childKeys(const QString& section) const;

    void store(const QString &ext, const UPContainerConnector &container);

private:
    QUrl _filename;
    Sections _sections;

    bool load(const UPContainerConnector &container);

};

typedef QScopedPointer<IniFile> ODF;

}

#endif
