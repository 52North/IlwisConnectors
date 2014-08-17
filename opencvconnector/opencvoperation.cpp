#include "kernel.h"
#include <functional>
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"

using namespace Ilwis;
using namespace OpenCV;

OpenCVOperation::OpenCVOperation()
{
}

OpenCVOperation::OpenCVOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr  ){

}

bool OpenCVOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    try{
        for(int index = 0; index < _inputRaster->indexDefinition().indexSize(); ++index ){
            QVariant trackIndex = _inputRaster->indexDefinition().byOrder(index);
            PixelIterator inputIter = _inputRaster->band(trackIndex);

            cv::Mat cvRaster;

            if(!OpenCVHelper::raster2Mat(inputIter,cvRaster))
                return false;

            cv::Mat cvProcessed;
            doOperation(cvRaster, cvProcessed);

            _outputRaster->addBand(index,_outputRaster->datadef(),trackIndex);
            PixelIterator outputIter = _outputRaster->band(trackIndex);

            OpenCVHelper::mat2Raster(cvProcessed,outputIter);
        }
        if ( ctx != 0) {
            QVariant value;
            value.setValue<IRasterCoverage>(_outputRaster);
            ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->source() );
        }
        return true;

    }catch(cv::Exception& ex){
        ERROR0(QString::fromStdString(ex.msg));
    }
    return false;
}

void OpenCVOperation::createInputOutputRasters(quint64 copyProperties)
{
    QString outputName = _expression.parm(0,false).value();

    OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    _outputRaster = OperationHelperRaster::initialize(_inputRaster.as<IlwisObject>(), itRASTER, copyProperties);
    OperationHelper::check([&] ()->bool { return _outputRaster.isValid(); },
    {ERR_NO_INITIALIZED_1,"output rastercoverage" } );

    if ( outputName != sUNDEF)
        _outputRaster->name(outputName);
}
