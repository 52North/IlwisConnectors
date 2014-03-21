#ifndef PYTHONAPI_TABLE_H
#define PYTHONAPI_TABLE_H

#include "pythonapi_ilwisobject.h"

namespace Ilwis {
    class Table;
    typedef IlwisData<Table> ITable;
}

typedef struct _object PyObject;

namespace pythonapi {
    class Table : public IlwisObject{
        friend class Coverage;
        public:
            Table();
            Table(std::string resource);

            quint32 columnCount() const;
            PyObject* columns() const;
            bool addColumn(const std::string& name, const std::string &domain);
            qint32 columnIndex(const std::string& name) const;
            PyObject* column(const std::string& name) const;
            PyObject* column(quint32 columnIndex) const;

            quint32 recordCount() const;
            PyObject* select(const std::string& conditions) const;
            PyObject* record(quint32 rec) const;

            PyObject* cell(const std::string& name, quint32 rec);
            PyObject* cell(quint32 colIndex, quint32 rec);
            void setCell(const std::string &name, quint32 rec, const PyObject* value);
            void setCell(quint32 colIndex, quint32 rec, const PyObject* value);
            void setCell(const std::string &name, quint32 rec, qint64 value);
            void setCell(quint32 colIndex, quint32 rec, qint64 value);
            void setCell(const std::string &name, quint32 rec, std::string value);
            void setCell(quint32 colIndex, quint32 rec, std::string value);
            void setCell(const std::string &name, quint32 rec, double value);
            void setCell(quint32 colIndex, quint32 rec, double value);
        private:
            Table(Ilwis::ITable* table);
    };

} // namespace pythonapi

#endif //PYTHONAPI_TABLE_H
