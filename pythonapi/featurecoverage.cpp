#include "../../IlwisCore/core/kernel.h"
#include "../../IlwisCore/core/ilwisobjects/ilwisobject.h"

#include "../../IlwisCore/core/util/angle.h"
#include "../../IlwisCore/core/util/point.h"
#include "../../IlwisCore/core/util/line.h"
#include "../../IlwisCore/core/util/box.h"
#include "../../IlwisCore/core/util/polygon.h"

#include "../../IlwisCore/core/ilwisobjects/ilwisdata.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/geometry.h"

#include "../../IlwisCore/core/ilwisobjects/domain/domain.h"
#include "../../IlwisCore/core/ilwisobjects/domain/datadefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/columndefinition.h"
#include "../../IlwisCore/core/ilwisobjects/table/table.h"
#include "../../IlwisCore/core/ilwisobjects/table/attributerecord.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/feature.h"
#include "../../IlwisCore/core/ilwisobjects/coverage/coverage.h"

#include "../../IlwisCore/core/ilwisobjects/coverage/featurecoverage.h"

#include "featurecoverage.h"

#include <QtCore/QCoreApplication>
#include <QString>

using namespace pythonapi;

FeatureCoverage::FeatureCoverage(){
}

FeatureCoverage::FeatureCoverage(const char* resource){
    int argc = 0;
    char* argv[0];
    QCoreApplication app(argc, argv);
    Ilwis::initIlwis();
    this->_ilwisIFeatureCoverage = new Ilwis::IFeatureCoverage();
    this->_ilwisIFeatureCoverage->prepare(QString(resource));
}

const char* FeatureCoverage::toStr(){
    if (this->_ilwisIFeatureCoverage->isValid())
        return "FeatureCoverage";
    else
        return "invalid_FeatureCoverage";
}
