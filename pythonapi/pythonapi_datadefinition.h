#ifndef PYTHONAPI_DataDefinition_H
#define PYTHONAPI_DataDefinition_H

namespace Ilwis{
class DataDefinition;
}

namespace pythonapi {

class Domain;
class Range;

class DataDefinition{
public:
    DataDefinition();
    DataDefinition(Ilwis::DataDefinition def);
    ~DataDefinition();
    DataDefinition(Domain* dm, const Range &rng);
    DataDefinition(const DataDefinition &def);
    bool __bool__() const;
    void range(const Range &rng);
    void domain(Domain* dm);
    bool isCompatibleWith(const DataDefinition& def);
    DataDefinition* merge(const DataDefinition& def1, const DataDefinition& def2);
    DataDefinition* operator=(const DataDefinition& def);

protected:
    Ilwis::DataDefinition& ptr() const;
    std::shared_ptr<Ilwis::DataDefinition> _ilwisDatadef;
};

}

#endif // PYTHONAPI_DataDefinition_H
