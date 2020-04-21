#include "poiservice.h"

PoiService::PoiService(POI *poi, QObject *parent):
    QObject(parent),
    webClient(parent),
    poiapi(poi)
{
    QObject::connect(poiapi, &POI::connected, this, &PoiService::onConnected);
    QObject::connect(poiapi, &POI::requestReceivedEvent, this, &PoiService::onRequestReceived);
}

PoiService::~PoiService() {
}

void PoiService::onConnected() {
	qDebug() << "Connected";
}

void PoiService::onRequestReceived(uint32_t requestId, const QString &requestType, const QJsonValue &requestParameter)
{
    if (requestType == POI_ParameterValueRequestTypeExternalOnlineRequest)
    {
        QJsonValue result = webClient.externalOnlineRequestHttpGetJson(requestParameter);

        // Reply to binding
        poiapi->requestProcessed(requestId, requestType, result);
    }
    else
    {
        /* UNDEFINED */
    }
}
