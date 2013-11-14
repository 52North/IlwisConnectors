#include  <stdio.h>
#include  <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QRegExp>
#include <QDir>
#include "ilwis.h"
#include "catalog.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwiscontext.h"
#include "inifile.h"

using namespace Ilwis;

IniFile::IniFile()
{
}

IniFile::~IniFile()
{
}

bool IniFile::setIniFile(const QUrl& fn, const UPContainerConnector &container, bool loadfile) {
    if ( !container || !container->isValid())
        return false;

     _filename = fn;
    if ( !loadfile ) // not interested in loading an inifile; we are creating a new one
        return true;

    if(!load(container))
        return ERROR1(ERR_MISSING_DATA_FILE_1,fn.toLocalFile());

    return true;

}

void IniFile::setKeyValue(const QString& section, const QString& key, const QString& value)
{
    Sections::iterator iterSect = _sections.find(section);
    if (iterSect == _sections.end())
    {
        // Add a new section, with one new key/value entry
        SectionEntries entries;
        entries[key.toLower()] = value;
        _sections[section] = entries;
    }
    else
    {
        // Add one new key/value entry in an existing section
        SectionEntries& entries = iterSect.value();
        entries[key.toLower()] = value;
    }
}

QString IniFile::value(const QString& section, const QString& key) const
{

    Sections::const_iterator iterSect = _sections.find(section);
    if (iterSect != _sections.end())
    {
        const SectionEntries& entries = iterSect.value();
        SectionEntries::const_iterator iterEntry = entries.find(key.toLower());
        if (iterEntry != entries.end())
            return iterEntry.value();
    }

    return sUNDEF;
}

void IniFile::removeKeyValue(const QString& section, const QString& key)
{
    Sections::iterator iterSect = _sections.find(section);
    if (iterSect != _sections.end())
    {
        // The section exists, now erase entry "key"
        SectionEntries& entries = iterSect.value();
        entries.remove(key.toLower());
    }
}

void IniFile::removeSection(const QString& section)
{
    Sections::iterator iterSect = _sections.find(section);
    if (iterSect != _sections.end())
    {
        // The section exists, so remove it and all its entries.
        SectionEntries& entries = iterSect.value();
        entries.clear();
        _sections.erase(iterSect);
    }
}

QString IniFile::file() const
{
    return _filename.toString();
}

QStringList IniFile::childKeys(const QString &section) const
{
    QStringList keys;
    for (Sections::const_iterator iter = _sections.begin(); iter != _sections.end(); ++iter)
    {
        if ( iter.key() == section) {
            const SectionEntries& entries = iter.value();
            for(SectionEntries::const_iterator cur = entries.begin(); cur != entries.end(); ++cur)
                keys.push_back(cur.value()) ;
        }
    }
    return keys;
}

bool IniFile::load(const UPContainerConnector& container)
{
    enum ParseState { FindSection, FindKey, ReadFindKey, StoreKey, None } state;
    QFileInfo fileinfo = container->toLocalFile(_filename);
    if (!fileinfo.exists())
        return false;

    QFile txtfile(fileinfo.absoluteFilePath());
    if (!txtfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1, fileinfo.fileName());
    }
    QTextStream textfile(&txtfile);
    QString text = textfile.readAll();
    if (text == ""){
        return false;
    }
     QStringList lines = text.split("\n");

     QString section, key, value;
     state = FindSection;
     QString line;
     int linecount =0;
     while(linecount < lines.size())
     {
         switch (state)
         {
         case FindSection:
             line = lines[linecount++];
             if (line.size() == 0)
                 continue;

             if (line[0] == '[')
             {
                 int iLast = line.indexOf(']');
                 if (iLast != -1)
                 {
                     section = line.mid(1, iLast - 1);
                     state = ReadFindKey;
                 }
             }
             else
                 state = FindKey;
             break;
         case ReadFindKey:
             line = lines[linecount++];
         case FindKey:
         {
             int iEqu = line.indexOf('=');
             if (iEqu != -1)
             {
                 key = line.mid(0, iEqu);
                 value = line.mid(iEqu + 1);
                 state = StoreKey;
             }
             else
                 state = ReadFindKey;
         }
             break;
         case StoreKey:
             setKeyValue(section, key, value);
             state = FindSection;
             break;

         case None:
             // Do we need to do anything?  Perhaps this never occurs.
             break;
         }
     }
     return true;
}

void IniFile::store(const QString& ext, const UPContainerConnector &container )
{
    if (!container || !container->isValid())
        return;

    QFileInfo fileinf = container->toLocalFile(_filename);
    QString path = fileinf.absoluteFilePath();

    if ( ext != "" && fileinf.suffix() != ext ) {
        path = fileinf.absolutePath() + "/" + fileinf.baseName() + "." + ext;
    }
    QFile fileIni(path);

    if (!fileIni.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QTextStream text(&fileIni);
    Sections::iterator iterSect;
    for (iterSect = _sections.begin(); iterSect != _sections.end(); ++iterSect)
    {
        text << "[" << iterSect.key() << "]"  << "\n";
        SectionEntries& entries = iterSect.value();
        SectionEntries::iterator iterEntry;
        for (iterEntry = entries.begin(); iterEntry != entries.end(); ++iterEntry)
        {
            QString key = iterEntry.key();
            key[0] = key[0].toUpper();
            text <<  key.trimmed() << "=" << iterEntry.value() << "\n";
        }
    }
    text.flush();
    fileIni.close();

}
