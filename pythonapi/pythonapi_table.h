#ifndef PYTHONAPI_TABLE_H
#define PYTHONAPI_TABLE_H

#include "pythonapi_ilwisobject.h"

namespace Ilwis {
    class Table;
    typedef IlwisData<Table> ITable;
}

namespace pythonapi {

    class PyVariant;

    class Table : public IlwisObject{
        public:
            Table();
            Table(Ilwis::ITable* table);
            Table(std::string resource);

            quint32 recordCount() const;
            quint32 columnCount() const;
            PyObject* columns() const;
            bool addColumn(const std::string& name, const std::string &domain);
            PyObject* select(const std::string& conditions) const;

            quint32 columnIndex(const std::string& name) const;
            PyVariant* cell(const std::string& name, quint32 rec);
            PyVariant* cell(quint32 colIndex, quint32 rec);
            void setCell(const std::string &name, quint32 rec, PyVariant &value);
            void setCell(quint32 colIndex, quint32 rec, PyVariant &value);
            void setCell(const std::string &name, quint32 rec, qint64 value);
            void setCell(quint32 colIndex, quint32 rec, qint64 value);
            void setCell(const std::string &name, quint32 rec, std::string value);
            void setCell(quint32 colIndex, quint32 rec, std::string value);
            void setCell(const std::string &name, quint32 rec, double value);
            void setCell(quint32 colIndex, quint32 rec, double value);
    };

} // namespace pythonapi

#endif //PYTHONAPI_TABLE_H
