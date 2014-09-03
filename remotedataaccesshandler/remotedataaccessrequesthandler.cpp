#include "kernel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QDirIterator>
#include <QXmlStreamWriter>
#include <QBuffer>
#include "geometries.h"
#include "ilwiscontext.h"
#include "ilwisconfiguration.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "coordinatesystem.h"
#include "geometryhelper.h"
#include "remotedataaccessrequesthandler.h"

using namespace Ilwis;
using namespace RemoteDataAccess;

RemoteDataAccessRequestHandler::RemoteDataAccessRequestHandler()
{
    QString dataRoot = ilwisconfig("remotedataserver/root-data-folder",QString("?"));
    _datafolder.prepare(dataRoot);
}

void RemoteDataAccessRequestHandler::service(HttpRequest &request, HttpResponse &response)
{

    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();
    QMultiMap<QByteArray,QByteArray>::Iterator iter;
    if ( (iter = parameters.find("datasource")) != parameters.end() ){
        QString name = iter.value();
        IIlwisObject obj = getObject(name, iter.value());
        response.setHeader("Content-Type", qPrintable("application/octet-stream"));
        response.setHeader("Content-Disposition", qPrintable("attachment;filename=" + name + ".bin"));

        writeObject(obj, request, response);
    }

}

void RemoteDataAccessRequestHandler::writeObject(const IIlwisObject& obj, const HttpRequest &request, HttpResponse &response){
    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();
    QMultiMap<QByteArray,QByteArray>::Iterator iter;
    QString typeName =  parameters.find("ilwistype").value();
    Resource bufferResource(obj->name(),IlwisObject::name2Type(typeName));
    bufferResource.addProperty("remote", true);
    _response = &response;
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    Ilwis::ConnectorInterface *conn = factory->createFromFormat(bufferResource, typeName,"stream");
    IlwisObjectConnector *ioObjectConnector = static_cast<IlwisObjectConnector *>(conn);
    ioObjectConnector->connect(ioObjectConnector,&IlwisObjectConnector::dataAvailable,this,&RemoteDataAccessRequestHandler::sendData);
    obj->setConnector(ioObjectConnector,IlwisObject::cmOUTPUT);

    iter = parameters.find("datatype");
    int storemode = IlwisObject::smMETADATA | IlwisObject::smBINARYDATA;
    if ( iter != parameters.end()){
        storemode = iter.value() == "data" ? IlwisObject::smBINARYDATA : IlwisObject::smMETADATA;
    }
    obj->store(storemode);

}

IIlwisObject RemoteDataAccessRequestHandler::getObject(const QString& name, const QString& ilwTypeName){
    IlwisTypes tp = IlwisObject::name2Type(ilwTypeName);
    QString url = _datafolder->resolve(name, tp);
    if ( url == sUNDEF)
        return IIlwisObject();
    IIlwisObject obj;
    obj.prepare(url, tp);
    return obj;

}

HttpRequestHandler *RemoteDataAccessRequestHandler::create()
{
    return new RemoteDataAccessRequestHandler();
}

void RemoteDataAccessRequestHandler::sendData(QBuffer *buf, bool lastBlock)
{
    QByteArray& bytes = buf->buffer();
    quint32 pos = buf->pos();
    bytes.resize(pos);
    _response->write(bytes, lastBlock);
    // prepare buffer for next use
    bytes.resize(STREAMBLOCKSIZE);
    buf->seek(0);
}
