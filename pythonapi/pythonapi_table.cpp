#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"

#include "pythonapi_table.h"

#include "pythonapi_pycontainer.h"
#include "pythonapi_feature.h"

namespace pythonapi {

    Table::Table(){
    }

    Table::Table(Ilwis::ITable *table):IlwisObject(new Ilwis::IIlwisObject(*table)){
    }

    Table::Table(std::string resource){
        Ilwis::ITable t(QString::fromStdString(resource), itTABLE);
        if (t.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(t));
    }

    quint32 Table::recordCount() const{
        return this->ptr()->get<Ilwis::Table>()->recordCount();
    }

    quint32 Table::columnCount() const{
        return this->ptr()->get<Ilwis::Table>()->columnCount();
    }

    bool Table::addColumn(const std::string& name, const std::string& domain){
        return this->ptr()->get<Ilwis::Table>()->addColumn(QString::fromStdString(name),QString::fromStdString(domain));
    }

    PyObject* Table::columns() const{
        Ilwis::ITable tbl = this->ptr()->get<Ilwis::Table>();
        PyObject* list = newPyTuple(tbl->columnCount());
        for(int i = 0; i < tbl->columnCount(); i++){
            if (!setTupleItem(list, i,tbl->columndefinition(i).name().toStdString().data()))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of columns").arg(tbl->columndefinition(i).name()));
        }
        return list;
    }

    PyObject* Table::select(const std::string& conditions) const{
        std::vector<quint32> lst = this->ptr()->get<Ilwis::Table>()->select(QString::fromStdString(conditions));
        PyObject* list = newPyTuple(lst.size());
        int i = 0;
        for(auto it = lst.begin(); it != lst.end(); it++){
            if (!setTupleItem(list, i++, *it))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of columns").arg(*it));
        }
        return list;
    }

    quint32 Table::columnIndex(const std::string& name) const{
        return this->ptr()->get<Ilwis::Table>()->columnIndex(QString::fromStdString(name));
    }

    PyObject* Table::cell(const std::string& name, quint32 rec){
        QVariant ret = this->ptr()->get<Ilwis::Table>()->cell(QString::fromStdString(name), rec,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute '%1' found or record %2 out of bound").arg(name.c_str()).arg(rec).toStdString());
        return PyVariant::toPyObject(ret);
    }

    PyObject* Table::cell(quint32 colIndex, quint32 rec){
        QVariant ret = this->ptr()->get<Ilwis::Table>()->cell(colIndex, rec,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute in '%1.' column found or record %2 out of bound").arg(colIndex).arg(rec).toStdString());
        return PyVariant::toPyObject(ret);
    }

    void Table::setCell(const std::string& name, quint32 rec, PyVariant& value){
        this->ptr()->get<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, value.data());
    }

    void Table::setCell(quint32 colIndex, quint32 rec, PyVariant& value){
        this->ptr()->get<Ilwis::Table>()->setCell(colIndex, rec, value.data());
    }

    void Table::setCell(const std::string& name, quint32 rec, qint64 value){
        this->ptr()->get<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, qint64 value){
        this->ptr()->get<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    void Table::setCell(const std::string& name, quint32 rec, std::string value){
        this->ptr()->get<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value.c_str()));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, std::string value){
        this->ptr()->get<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value.c_str()));
    }

    void Table::setCell(const std::string& name, quint32 rec, double value){
        this->ptr()->get<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, double value){
        this->ptr()->get<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    PyObject* Table::column(const std::string& name) const{
        std::vector<QVariant> col = this->ptr()->get<Ilwis::Table>()->column(QString::fromStdString(name));
        PyObject* list = newPyTuple(col.size());
        int i = 0;
        for(auto it = col.begin(); it != col.end(); it++){
            bool ok = false;
            double value = (*it).toDouble(&ok);
            if (ok){
                setTupleItem(list, i++, value);
            }else{
                setTupleItem(list, i++, (*it).toString().toStdString().data());
            }
        }
        return list;
    }

    PyObject* Table::column(quint32 columnIndex) const{
        std::vector<QVariant> col = this->ptr()->get<Ilwis::Table>()->column(columnIndex);
        PyObject* list = newPyTuple(col.size());
        int i = 0;
        for(auto it = col.begin(); it != col.end(); it++){
            bool ok = false;
            double value = (*it).toDouble(&ok);
            if (ok){
                setTupleItem(list, i++, value);
            }else{
                setTupleItem(list, i++, (*it).toString().toStdString().data());
            }
        }
        return list;
    }

    PyObject* Table::record(quint32 rec) const{
        std::vector<QVariant> col = this->ptr()->get<Ilwis::Table>()->record(rec);
        PyObject* list = newPyTuple(col.size());
        int i = 0;
        for(auto it = col.begin(); it != col.end(); it++){
            bool ok = false;
            double value = (*it).toDouble(&ok);
            if (ok){
                setTupleItem(list, i++, value);
            }else{
                setTupleItem(list, i++, (*it).toString().toStdString().data());
            }
        }
        return list;
    }


} // namespace pythonapi
