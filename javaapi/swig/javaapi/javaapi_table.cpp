#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwiscontext.h"
#include "../../IlwisCore/core/catalog/catalog.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/basetable.h"

#include "javaapi_table.h"
#include "javaapi_feature.h"
#include "javaapi_error.h"
#include "javaapi_columndefinition.h"

namespace javaapi {

    Table::Table(){        
    }

    Table::Table(Ilwis::ITable *table):IlwisObject(new Ilwis::IIlwisObject(*table)){
        delete table;
    }

    Table::Table(std::string resource, const IOOptions& opt){
        QString input (QString::fromStdString(resource));
        input.replace('\\','/');
        // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
        if (input.indexOf("://") < 0) {
            int pos = input.indexOf('/');
            if (pos > 0) {
                if ((input.count('/') > 1) || QFileInfo(input).exists()) // full path starting with drive-letter (MS-DOS-style)
                    input = "file:///" + input;
                else // container object without path, e.g. myfile.hdf/subdataset: look for it in workingCatalog()
                    input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
            }  else if (pos == 0) // full path starting with path-separator (UNIX-style)
                input = "file://" + input;
            else { // pos < 0: file without path, or new object
                QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
                if (QFileInfo (file).exists()) // file without path
                    input = "file:///" + file;
            }
        }
        Ilwis::ITable t(input, itTABLE, opt.ptr());
        if (t.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(t));
    }

    quint32 Table::recordCount() const{
        return this->ptr()->as<Ilwis::Table>()->recordCount();
    }

    quint32 Table::columnCount() const{
        return this->ptr()->as<Ilwis::Table>()->columnCount();
    }

    bool Table::addColumn(const std::string& name, const std::string& domain){
        return this->ptr()->as<Ilwis::Table>()->addColumn(QString::fromStdString(name),QString::fromStdString(domain));
    }

    bool Table::addColumn(ColumnDefinition& coldef){
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        return this->ptr()->as<Ilwis::Table>()->addColumn(*ilwDef);
    }

    std::vector<std::string> Table::columns() const{
        Ilwis::ITable tbl = this->ptr()->as<Ilwis::Table>();
        std::vector<std::string> vector;
        for(int i = 0; i < tbl->columnCount(); i++){
            Ilwis::ColumnDefinition coldef = tbl->columndefinition(i);
            vector.push_back( coldef.name().toStdString() );
            //if (!setTupleItem(list, i, PyUnicodeFromString(coldef.name().toStdString().data())))
                //throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of columns").arg(tbl->columndefinition(i).name()));
        }
        return vector;
    }

    std::vector<quint32> Table::select(const std::string& conditions) const{
        std::vector<quint32> lst = this->ptr()->as<Ilwis::Table>()->select(QString::fromStdString(conditions));
        return lst;
    }

    qint32 Table::columnIndex(const std::string& name) const{
        quint32 idx = this->ptr()->as<Ilwis::Table>()->columnIndex(QString::fromStdString(name));
        if (idx == Ilwis::iUNDEF)
            return Ilwis::iUNDEF;
        else
            return idx;
    }

    std::string Table::cell(const std::string& name, quint32 rec){
        QVariant ret = this->ptr()->as<Ilwis::Table>()->cell(QString::fromStdString(name), rec,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute '%1' found or record %2 out of bound").arg(name.c_str()).arg(rec).toStdString());
        return ret.toString().toStdString();
    }

    std::string Table::cell(quint32 colIndex, quint32 rec){
        if(this->ptr()->as<Ilwis::Table>()->isDataLoaded()){
            QVariant ret = this->ptr()->as<Ilwis::Table>()->cell(colIndex, rec,false);
            if (!ret.isValid())
                throw std::out_of_range(QString("No attribute in '%1.' column found or record %2 out of bound").arg(colIndex).arg(rec).toStdString());
            return ret.toString().toStdString();
        } else {
            throw InvalidObject("Data of the table is not loaded. Access a feature first.");
        }
    }

    void Table::setCell(const std::string& name, quint32 rec, const QVariant* value){
        const QVariant* v = value;
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, *v);
        delete v;
    }

    void Table::setCell(quint32 colIndex, quint32 rec, const QVariant* value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, *value);
    }

    void Table::setCell(const std::string& name, quint32 rec, qint64 value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, qint64 value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    void Table::setCell(const std::string& name, quint32 rec, std::string value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value.c_str()));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, std::string value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value.c_str()));
    }

    void Table::setCell(const std::string& name, quint32 rec, double value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, double value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    std::vector<std::string> Table::column(const std::string& name) const{
        std::vector<QVariant> source = this->ptr()->as<Ilwis::Table>()->column(QString::fromStdString(name));
        std::vector<std::string> result(source.size());
        for(int i=0; i<source.size(); ++i)
            result[i] = source[i].toString().toStdString();
        return result;
    }

    std::vector<std::string> Table::column(quint32 columnIndex) const{
        std::vector<QVariant> source = this->ptr()->as<Ilwis::Table>()->column(columnIndex);
        std::vector<std::string> result(source.size());
        for(int i=0; i<source.size(); ++i)
            result[i] = source[i].toString().toStdString();
        return result;
   }

    std::vector<std::string> Table::record(quint32 rec) const{
        std::vector<QVariant> source = this->ptr()->as<Ilwis::Table>()->record(rec);
        std::vector<std::string> result(source.size());
        for(int i=0; i<source.size(); ++i)
            result[i] = source[i].toString().toStdString();
        return result;
    }

    Table* Table::toTable(IObject *obj){
        Table* ptr = dynamic_cast<Table*>(obj);
        if(!ptr)
            throw InvalidObject("cast to Table not possible");
        return ptr;
    }

    ColumnDefinition Table::columnDefinition(const std::string& name) const{
        QString qName;
        qName = qName.fromStdString(name);
        Ilwis::ColumnDefinition* coldef = new Ilwis::ColumnDefinition(this->ptr()->as<Ilwis::Table>()->columndefinition(qName));
        return ColumnDefinition(coldef);
    }

    ColumnDefinition Table::columnDefinition(quint32 index) const{
        Ilwis::ColumnDefinition* coldef = new Ilwis::ColumnDefinition(this->ptr()->as<Ilwis::Table>()->columndefinition(index));
        return ColumnDefinition(coldef);
    }

    void Table::setColumnDefinition(ColumnDefinition& coldef){
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        this->ptr()->as<Ilwis::Table>()->columndefinition(*ilwDef);
    }

    void Table::setColumnDefinition(const std::string &name, ColumnDefinition* coldef){
        QString qName;
        qName = qName.fromStdString(name);
        Ilwis::ColumnDefinition& ilwdef = this->ptr()->as<Ilwis::Table>()->columndefinitionRef(qName);
        Ilwis::ColumnDefinition* newDef = coldef->ptr().get();
        ilwdef = Ilwis::ColumnDefinition(*newDef, ilwdef.columnindex());
    }

    void Table::setColumnDefinition(quint32 index, ColumnDefinition *coldef){
        Ilwis::ColumnDefinition& ilwdef = this->ptr()->as<Ilwis::Table>()->columndefinitionRef(index);
        Ilwis::ColumnDefinition* newDef = coldef->ptr().get();
        ilwdef = Ilwis::ColumnDefinition(*newDef, ilwdef.columnindex());
    }


} // namespace javaapi
