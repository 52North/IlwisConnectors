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
#include "ilwiscontext.h"
#include "inifile.h"

using namespace Ilwis;

IniFile::IniFile()
{
}

IniFile::~IniFile()
{
}

bool IniFile::setIniFile(const QString& fn, bool loadfile) {
    QRegExp exp("/");
    QString file = fn;

    if (exp.indexIn(fn) == -1) {
        file = context()->workingCatalog()->resolve(file);
    }
    _filename = file;

    if ( !loadfile ) // not interested in loading an inifile; we are creating a new one
        return true;

    if ( _filename.exists() )
        load();
    else {
        kernel()->issues()->log(TR(ERR_MISSING_DATA_FILE_1).arg(fn));
        return false;
    }
    return true;

}

bool IniFile::setIniFile(const QUrl &fn, bool loadfile)
{
    QString file = fn.toLocalFile();
    return setIniFile(file, loadfile);

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

QFileInfo IniFile::fileinfo() const
{
    return _filename;
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

void IniFile::load()
{
    enum ParseState { FindSection, FindKey, ReadFindKey, StoreKey, None } state;
    QFile inifile(_filename.canonicalFilePath());
    if (!inifile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream textfile(&inifile);
     QString text = textfile.readAll();
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
}

void IniFile::store(const QString& ext)
{
    QString path = _filename.absoluteFilePath();
    if ( ext != "") {
        path = _filename.absolutePath() + "/" + _filename.baseName() + "." + ext;
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
