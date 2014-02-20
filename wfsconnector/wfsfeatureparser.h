#ifndef WFSFEATUREPARSER_H
#define WFSFEATUREPARSER_H

#include "wfsconnector_global.h"

class QString;
class XmlStreamParser;

namespace Ilwis {
namespace Wfs {

class WfsResponse;
class FillerColumnDef;

class WFSCONNECTORSHARED_EXPORT WfsFeatureParser
{
public:
    WfsFeatureParser(WfsResponse *response);
    ~WfsFeatureParser();

    /**
     * Parses feature instances to the given attribute table.<br/>
     * <br/>
     * Note that a WFS feature attribute table has to be constructed first to give
     * valuable results. This can be done by means of a WfsFeatureDescriptionParser
     * which uses the feature's schema description served by the WFS to create appropriate
     * Domain columns for each feature attribute.<br/>
     * <br/>
     * The parsed xml schema description may use different namespace mappings than the
     * feature collection. To keep track of the elements the WfsFeatureDescriptionParser
     * gives access to those used in the schema document.
     *
     * TODO: for now only the target namespace is being used ... propably this is enough
     *
     * @param table the feature's attribute table.
     * @param mappings the xml schema namespace mappings.
     */
    void parseFeature(ITable &table, QMap<QString,QString> &mappings);

    /**
     * Sets individual column handlers according to the domain associated with the given
     * table.<br/>
     * <br/>
     * Note that a WFS feature attribute table has to be constructed first to give
     * valuable results. This can be done by means of a WfsFeatureDescriptionParser
     * which uses the feature's schema description served by the WFS to create appropriate
     * Domain columns for each feature attribute.<br/>
     *
     * @param table the table having domain descriptions for each column.
     */
    void setColumnCallbacks(ITable &table);

private:
    XmlStreamParser *_parser;
    std::vector<FillerColumnDef*> _columnFillers;

    void loadRecord(ITable &table, std::vector<QVariant> &record);

    QVariant fillStringColumn();
    QVariant fillDoubleColumn();
    QVariant fillDateTimeColumn();
    QVariant fillIntegerColumn();

};

struct FillerColumnDef {
    FillerColumnDef(QVariant(WfsFeatureParser::* func)()): fillFunc(func){}
    QVariant (WfsFeatureParser::* fillFunc)();
};

}
}
#endif // WFSFEATUREPARSER_H
