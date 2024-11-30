#ifndef JAVAAPI_TABLE_H
#define JAVAAPI_TABLE_H

#include "javaapi_ilwisobject.h"
#include "javaapi_columndefinition.h"

namespace Ilwis {
    class Table;
    typedef IlwisData<Table> ITable;
}

namespace javaapi {
    class ColumnDefinition;
    class Table : public IlwisObject{
        friend class Coverage;
        friend class Catalog;
        friend class FeatureCoverage;
        public:
            Table();
            Table(std::string resource, const IOOptions& opt = IOOptions());

            quint32 columnCount() const;
            std::vector<std::string> columns() const;
            bool addColumn(const std::string& name, const std::string &domain);
            bool addColumn(ColumnDefinition& coldef);
            qint32 columnIndex(const std::string& name) const;
            std::vector<std::string> column(const std::string& name) const;
            std::vector<std::string> column(quint32 columnIndex) const;

            quint32 recordCount() const;
            std::vector<quint32> select(const std::string& conditions) const;
            std::vector<std::string> record(quint32 rec) const;

            std::string cell(const std::string& name, quint32 rec);
            std::string cell(quint32 colIndex, quint32 rec);
            void setCell(const std::string &name, quint32 rec, const QVariant* value);
            void setCell(quint32 colIndex, quint32 rec, const QVariant* value);
            void setCell(const std::string &name, quint32 rec, qint64 value);
            void setCell(quint32 colIndex, quint32 rec, qint64 value);
            void setCell(const std::string &name, quint32 rec, std::string value);
            void setCell(quint32 colIndex, quint32 rec, std::string value);
            void setCell(const std::string &name, quint32 rec, double value);
            void setCell(quint32 colIndex, quint32 rec, double value);

            static Table* toTable(IObject* obj);

            ColumnDefinition columnDefinition(const std::string& name) const;
            ColumnDefinition columnDefinition(quint32 index) const;
            void setColumnDefinition(ColumnDefinition& coldef);
            void setColumnDefinition(const std::string& name, ColumnDefinition* coldef);
            void setColumnDefinition(quint32 index, ColumnDefinition* coldef);

        private:
            Table(Ilwis::ITable* table);
    };

} // namespace javaapi

#endif //JAVAAPI_TABLE_H
