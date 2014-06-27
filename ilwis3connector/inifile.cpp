#include  <stdio.h>
#include  <stdlib.h>
#include <QTextStream>
#include <QRegExp>
#include "catalog.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "ilwiscontext.h"
#include "inifile.h"

using namespace Ilwis;

IniFile::IniFile()
{
}

IniFile::IniFile(const QFileInfo &file, bool loadfile)
{
    setIniFile(file, loadfile);
}

IniFile::~IniFile()
{
}

bool IniFile::setIniFile(const QFileInfo& file, bool loadfile) {

     _filename = file;
    if ( !loadfile ) // not interested in loading an inifile; we are creating a new one
        return true;

    if(!load())
        return ERROR1(ERR_MISSING_DATA_FILE_1,file.fileName());

    return true;

}

void IniFile::setValue(const QString& section, const QString& key, const QString& value){
    Sections::iterator iterSect = _sections.find(section);
    if (iterSect == _sections.end())
    {
        // Add a new section, with one new key/value entry
        SectionEntries entries;
        //entries[key.toLower()] = value;
        entries[key] = value;
        _sections[section] = entries;
    }
    else
    {
        // Add one new key/value entry in an existing section
        SectionEntries& entries = iterSect.value();
        //entries[key.toLower()] = value;
        entries[key] = value;
    }
}

void IniFile::setKeyValue(const QString& section, const QString& key, const QString& value)
{
    setValue(section, key, value);
}

void IniFile::setKeyValue(const QString& section, const QString& key, double value)
{
    setValue(section, key, QString::number(value)) ;
}

void IniFile::setKeyValue(const QString& section, const QString& key, int value)
{
    setValue(section, key, QString::number(value)) ;
}

QString IniFile::value(const QString& section, const QString& key) const
{

    Sections::const_iterator iterSect = _sections.find(section);
    if (iterSect != _sections.end())
    {
        const SectionEntries& entries = iterSect.value();
        SectionEntries::const_iterator iterEntry = entries.find(key);
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
        entries.remove(key);
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
    return QUrl::fromLocalFile(_filename.absoluteFilePath()).toString();
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

bool IniFile::load()
{
    enum ParseState { FindSection, FindKey, ReadFindKey, StoreKey, None } state;
    if (!_filename.exists())
        return false;

    QFile txtfile(_filename.absoluteFilePath());
    if (!txtfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return ERROR1(ERR_COULD_NOT_OPEN_READING_1, _filename.fileName());
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

void IniFile::store(const QString& ext, const QFileInfo& file )
{
    QString path = file.absoluteFilePath();

    if ( ext != "" && file.suffix() != ext ) {
        path = file.absolutePath() + "/" + file.baseName() + "." + ext;
    }
    QFile fileIni(path);

    if (!fileIni.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
        return;

    QTextStream text(&fileIni);
    Sections::iterator iterSect;
    bool first = true;
    for (iterSect = _sections.begin(); iterSect != _sections.end(); ++iterSect)
    {
        if ( !first)
            text << "\n";
        text << "[" << iterSect.key() << "]"  << "\n";
        SectionEntries& entries = iterSect.value();
        SectionEntries::iterator iterEntry;
        for (iterEntry = entries.begin(); iterEntry != entries.end(); ++iterEntry)
        {
            QString key = iterEntry.key();
            key[0] = key[0].toUpper();
            text <<  key.trimmed() << "=" << iterEntry.value() << "\n";
        }
        first = false;
    }
    text.flush();
    fileIni.close();

}
